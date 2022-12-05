/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the console i/o functions
 *
 * Name:        bofconio.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         None
 *
 * History:
 *
 * V 1.00  Apr 13 2019  BHA : Initial release
 */
/*
Unit test:
  std::string Input_S;
  BOF::BOF_CONIO_PARAM ConioParam_X;
  std::unique_ptr<BOF::BofConio> puConio = std::make_unique<BOF::BofConio>(ConioParam_X);
  puConio->SetForegroundTextColor(BOF::CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_BRIGHT_YELLOW);
  puConio->SetBackgroundTextColor(BOF::CONIO_TEXT_COLOR::CONIO_TEXT_COLOR_RED);
  puConio->Clear(BOF::CONIO_CLEAR::CONIO_CLEAR_ALL);
  puConio->SetTextCursorPosition(4, 10);
  puConio->Printf("Hello world");
  puConio->SetTextCursorPosition(5, 9);
  puConio->Printf("!!!");
  puConio->SetTextCursorState(BOF::CONIO_TEXT_CURSOR_STATE::CONIO_TEXT_CURSOR_STATE_BLINK_OFF);
  puConio->SetTextWindowTitle("M F S");
*/
 //https://solarianprogrammer.com/2019/04/08/c-programming-ansi-escape-codes-windows-macos-linux-terminals/
 //https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
#include <bofstd/bofconio.h>
#include <bofstd/bofstring.h>
#include <bofstd/bofsystem.h>
#include <bofstd/bofstringformatter.h>

#include <linenoise.hpp>

#if defined(_WIN32)
#include <conio.h>
#include <Windows.h>
#else
#include <sys/ioctl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <linenoise.hpp>
#include <poll.h>
#include <linux/kd.h>
#endif

#include <stdlib.h>
#include <map>

BEGIN_BOF_NAMESPACE()

static std::map<uint32_t, std::string> S_KeyLookupCollection{
  {CONIO_FUNCTION_KEY_F1,         "F01"},
  {CONIO_FUNCTION_KEY_F2,         "F02"},
  {CONIO_FUNCTION_KEY_F3,         "F03"},
  {CONIO_FUNCTION_KEY_F4,         "F04"},
  {CONIO_FUNCTION_KEY_F5,         "F05"},
  {CONIO_FUNCTION_KEY_F6,         "F06"},
  {CONIO_FUNCTION_KEY_F7,         "F07"},
  {CONIO_FUNCTION_KEY_F8,         "F08"},
  {CONIO_FUNCTION_KEY_F9,         "F09"},
  {CONIO_FUNCTION_KEY_F10,        "F10"},
  {CONIO_FUNCTION_KEY_F11,        "F11"},
  {CONIO_FUNCTION_KEY_F12,        "F12"},
  {CONIO_SPECIAL_KEY_ESC,         "Esc"},
  {CONIO_SPECIAL_KEY_BACKSPACE,   "Bs"},
  {CONIO_SPECIAL_KEY_TAB,         "Tab"},
  {CONIO_SPECIAL_KEY_ENTER,       "Enter"},
  {CONIO_SPECIAL_KEY_INSERT,      "Ins"},
  {CONIO_SPECIAL_KEY_HOME,        "Home"},
  {CONIO_SPECIAL_KEY_PAGE_UP,     "PgUp"},
  {CONIO_SPECIAL_KEY_DELETE,      "Del"},
  {CONIO_SPECIAL_KEY_END,         "End"},
  {CONIO_SPECIAL_KEY_PAGE_DOWN,   "PgDw"},
  {CONIO_SPECIAL_KEY_ARROW_UP,    "Up"},
  {CONIO_SPECIAL_KEY_ARROW_LEFT,  "Left"},
  {CONIO_SPECIAL_KEY_ARROW_DOWN,  "Down"},
  {CONIO_SPECIAL_KEY_ARROW_RIGHT, "Right"},
  {CONIO_SPECIAL_KEY_NONE,        "None"},
};


BofConio::BofConio(const BOF_CONIO_PARAM &_rBofConioParam_X)
{
  mBofConioParam_X = _rBofConioParam_X;

#if defined(_WIN32)
  CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo_X;
  HANDLE Out_h = GetStdHandle(STD_OUTPUT_HANDLE);
  if (Out_h != INVALID_HANDLE_VALUE)
  {
    GetConsoleScreenBufferInfo(Out_h, &ScreenBufferInfo_X);
    mConsoleWidth_U32 = ScreenBufferInfo_X.srWindow.Right - ScreenBufferInfo_X.srWindow.Left + 1;
    mConsoleHeight_U32 = ScreenBufferInfo_X.srWindow.Bottom - ScreenBufferInfo_X.srWindow.Top + 1;
  }
#else
  //struct winsize ScreenBufferInfo_X;
  //ioctl(STDOUT_FILENO, TIOCGWINSZ, &ScreenBufferInfo_X);
  //mConsoleWidth_U32 = ScreenBufferInfo_X.ws_row;
  //mConsoleHeight_U32 = ScreenBufferInfo_X.ws_col;
//	mConsoleHeight_U32 = atoi(getenv("LINES"));
//	mConsoleWidth_U32 = atoi(getenv("COLUMNS"));
  mConsoleHeight_U32 = 80;
  mConsoleWidth_U32 = 24;
#endif

  linenoise::SetMultiLine(mBofConioParam_X.EditOnMultiLine_B);
  linenoise::SetHistoryMaxLen(mBofConioParam_X.HistoryMaxLength_U32);
  linenoise::LoadHistory(mBofConioParam_X.HistoryPathname.FullPathName(false).c_str());

  // Setup completion words every time when a user types tab
  linenoise::SetCompletionCallback([](const char *_pEditBuffer_c, std::vector<std::string> &_rCompletionCollection) {
    const std::vector<std::string> HistoryCollection = linenoise::GetHistory();
    _rCompletionCollection = Bof_FindAllStringIncluding(false, true, _pEditBuffer_c, HistoryCollection);
    /*
    if (editBuffer[0] == 'h') {
      completions.push_back("hello");
      completions.push_back("hello there");
    }
    */
                                   });
}

BofConio::~BofConio()
{
  linenoise::SaveHistory(mBofConioParam_X.HistoryPathname.FullPathName(false).c_str());
}


std::string BofConio::KeyToString(uint32_t _Key_U32)
{
  std::string Rts_S, Modifier_S;

  Rts_S = S_KeyLookupCollection[_Key_U32 & CONIO_KEY_MASK];
  if (Rts_S == "")
  {
    if ((_Key_U32 & CONIO_KEY_MASK) <= 0xFF)
    {
      Rts_S = static_cast<char>(_Key_U32 & CONIO_KEY_MASK);
    }
    else
    {
      Rts_S = "None";
    }
  }
  if (_Key_U32 & CONIO_MODIFIER_KEY_FLAG_LEFT_CTRL)
  {
    Modifier_S += "LCTRL|";
  }
  if (_Key_U32 & CONIO_MODIFIER_KEY_FLAG_RIGHT_CTRL)
  {
    Modifier_S += "RCTRL|";
  }
  if (_Key_U32 & CONIO_MODIFIER_KEY_FLAG_LEFT_MENU)
  {
    Modifier_S += "LMENU|";
  }
  if (_Key_U32 & CONIO_MODIFIER_KEY_FLAG_RIGHT_MENU)
  {
    Modifier_S += "RMENU|";
  }
  if (_Key_U32 & CONIO_MODIFIER_KEY_FLAG_LEFT_SHIFT)
  {
    Modifier_S += "LSHFT|";
  }
  if (_Key_U32 & CONIO_MODIFIER_KEY_FLAG_RIGHT_SHIFT)
  {
    Modifier_S += "RSHFT|";
  }
  if (_Key_U32 & CONIO_MODIFIER_KEY_FLAG_PRINT_SCREEN)
  {
    Modifier_S += "PRT_SCR|";
  }
  if (_Key_U32 & CONIO_MODIFIER_KEY_FLAG_SCROLL_LOCK)
  {
    Modifier_S += "SCRL_LCK|";
  }
  if (_Key_U32 & CONIO_MODIFIER_KEY_FLAG_PAUSE)
  {
    Modifier_S += "PAUSE|";
  }
  if (_Key_U32 & CONIO_MODIFIER_KEY_FLAG_NUM_LOCK)
  {
    Modifier_S += "NUM_LCK|";
  }
  if (_Key_U32 & CONIO_MODIFIER_KEY_FLAG_CAPS_LOCK)
  {
    Modifier_S += "CAP_LCK|";
  }
  if (Modifier_S != "")
  {
    Modifier_S[Modifier_S.size() - 1] = ' ';
  }
  Rts_S = Modifier_S + Rts_S;

  return Rts_S;
}

uint32_t BofConio::KbHit(uint32_t _TimeOutInMs_U32)
{
  uint32_t Rts_U32 = 0;

#if defined(_WIN32)
  uint32_t Start_U32, Delta_U32 = 0;
  Start_U32 = Bof_GetMsTickCount();
  do
  {
    if (_kbhit())
    {
      Rts_U32 = _getch();
      break;
    }
    else
    {
      if (_TimeOutInMs_U32)
      {
        Bof_MsSleep(10);
      }
    }
    Delta_U32 = Bof_ElapsedMsTime(Start_U32);
  } while (Delta_U32 < _TimeOutInMs_U32);
#else
  struct pollfd Poll_X;
  char Ch_c;

  if (isatty(STDIN_FILENO))
  {
    if (linenoise::enableRawMode(STDIN_FILENO))
    {
      /* See if there is data available */
      Poll_X.fd = STDIN_FILENO;
      Poll_X.events = POLLIN;
      if (poll(&Poll_X, 1, _TimeOutInMs_U32) == 1)
      {
        if (read(STDIN_FILENO, &Ch_c, 1) == 1)
        {
          Rts_U32 = GetChAfterOneChar(false, Ch_c);
        }
        linenoise::disableRawMode(STDIN_FILENO);

      }
      else
      {
        linenoise::disableRawMode(STDIN_FILENO);
      }
    }
  }
#endif

  return Rts_U32;
}

uint32_t BofConio::GetChAfterOneChar(bool _OnlyModifier_B, char _FirstChar_c)
{
  uint32_t Rts_U32, Modifier_U32;

  Modifier_U32 = 0;
  Rts_U32 = 0;

#if defined(_WIN32)
  uint32_t ExtraKey_U32;
  uint16_t State_U16;

  if (!_OnlyModifier_B)
  {
    Rts_U32 = _FirstChar_c;
    if (!Rts_U32)
    {
      ExtraKey_U32 = _getch();
      if ((ExtraKey_U32 >= 0x0000003B) && (ExtraKey_U32 <= (0x0000003B + 10)))	//F1->F10
      {
        Rts_U32 = CONIO_FUNCTION_KEY_F1 + ExtraKey_U32 - 0x0000003B;
      }
      else	//Numeric keypad
      {
        Rts_U32 = 0x0000E000 | ExtraKey_U32;
      }
    }
    else if (Rts_U32 == 0xE0)
    {
      ExtraKey_U32 = _getch();
      Rts_U32 = 0x0000E000 | ExtraKey_U32;
    }
  }
  //http://www.kbdedit.com/manual/low_level_vk_list.html
  State_U16 = GetAsyncKeyState(VK_LCONTROL);
  if (State_U16 & 0x8000)
  {
    Modifier_U32 |= CONIO_MODIFIER_KEY_FLAG_LEFT_CTRL;
  }
  State_U16 = GetAsyncKeyState(VK_RCONTROL);
  if (State_U16 & 0x8000)
  {
    Modifier_U32 |= CONIO_MODIFIER_KEY_FLAG_RIGHT_CTRL;
  }
  State_U16 = GetAsyncKeyState(VK_LMENU);
  if (State_U16 & 0x8000)
  {
    Modifier_U32 |= CONIO_MODIFIER_KEY_FLAG_LEFT_MENU;
  }
  State_U16 = GetAsyncKeyState(VK_RMENU);
  if (State_U16 & 0x8000)
  {
    Modifier_U32 |= CONIO_MODIFIER_KEY_FLAG_RIGHT_MENU;
  }
  State_U16 = GetAsyncKeyState(VK_LSHIFT);
  if (State_U16 & 0x8000)
  {
    Modifier_U32 |= CONIO_MODIFIER_KEY_FLAG_LEFT_SHIFT;
  }
  State_U16 = GetAsyncKeyState(VK_RSHIFT);
  if (State_U16 & 0x8000)
  {
    Modifier_U32 |= CONIO_MODIFIER_KEY_FLAG_RIGHT_SHIFT;
  }
  State_U16 = GetKeyState(VK_SNAPSHOT);
  if (State_U16 & 0x0001)
  {
    Modifier_U32 |= CONIO_MODIFIER_KEY_FLAG_PRINT_SCREEN;
  }
  State_U16 = GetKeyState(VK_SCROLL);
  if (State_U16 & 0x0001)
  {
    Modifier_U32 |= CONIO_MODIFIER_KEY_FLAG_SCROLL_LOCK;
  }
  State_U16 = GetKeyState(VK_PAUSE);
  if (State_U16 & 0x0001)
  {
    Modifier_U32 |= CONIO_MODIFIER_KEY_FLAG_PAUSE;
  }
  State_U16 = GetKeyState(VK_NUMLOCK);
  if (State_U16 & 0x001)
  {
    Modifier_U32 |= CONIO_MODIFIER_KEY_FLAG_NUM_LOCK;
  }
  State_U16 = GetKeyState(VK_CAPITAL);
  if (State_U16 & 0x0001)
  {
    Modifier_U32 |= CONIO_MODIFIER_KEY_FLAG_CAPS_LOCK;
  }

#else
  char          pEscSeq_c[4];
  struct pollfd Poll_X;

  pEscSeq_c[0] = 0;
  pEscSeq_c[1] = 0;
  pEscSeq_c[2] = 0;
  Rts_U32 = _FirstChar_c;

  /* Does not work in terminal
   * char LedFlag_c,ShiftState_c;

        if (ioctl(STDIN_FILENO, KDGKBLED, &LedFlag_c) == 0)
        {
          if (LedFlag_c & 0x04)
          {
            Modifier_U32 |= CONIO_MODIFIER_KEY_FLAG_CAPS_LOCK;
          }
          if (LedFlag_c & 0x02)
          {
            Modifier_U32 |= CONIO_MODIFIER_KEY_FLAG_NUM_LOCK;
          }
          if (LedFlag_c & 0x01)
          {
            Modifier_U32 |= CONIO_MODIFIER_KEY_FLAG_SCROLL_LOCK;
          }
          printf("Led Flag %02X\n", LedFlag_c);
        }


        ShiftState_c = 6;
        if (ioctl(0, TIOCLINUX, &ShiftState_c) == 0)
        {
          printf("Shift State %02X\n", ShiftState_c);
        }
        */
  if ((Rts_U32) && (Rts_U32 <= 26))
  {
    Modifier_U32 |= CONIO_MODIFIER_KEY_FLAG_LEFT_CTRL | CONIO_MODIFIER_KEY_FLAG_RIGHT_CTRL;
    Rts_U32 = 'a' + Rts_U32 - 1;
  }
  else if ((Rts_U32 >= 'A') && (Rts_U32 <= 'Z'))
  {
    Modifier_U32 |= CONIO_MODIFIER_KEY_FLAG_LEFT_SHIFT | CONIO_MODIFIER_KEY_FLAG_RIGHT_SHIFT;
    Rts_U32 = 'a' + Rts_U32 - 1;
  }
  else if (Rts_U32 == CONIO_SPECIAL_KEY_ESC)
  {
    /* See if there is data available */
    Poll_X.fd = STDIN_FILENO;
    Poll_X.events = POLLIN;
    if (poll(&Poll_X, 1, 1) == 1)
    {
      /* Read the next two bytes representing the escape sequence. Use two calls to handle slow terminals returning the two chars at different times. */
      if ((read(STDIN_FILENO, &pEscSeq_c[0], 1) == 1) && (read(STDIN_FILENO, &pEscSeq_c[1], 1) == 1))
      {
        switch (pEscSeq_c[0])
        {
          case 'O':
            switch (pEscSeq_c[1])
            {
              case 'P':
                Rts_U32 = CONIO_FUNCTION_KEY_F1;
                break;

              case 'Q':
                Rts_U32 = CONIO_FUNCTION_KEY_F2;
                break;

              case 'R':
                Rts_U32 = CONIO_FUNCTION_KEY_F3;
                break;

              case 'S':
                Rts_U32 = CONIO_FUNCTION_KEY_F4;
                break;

              default:
                break;
            }
            break;

          case '[':
            if (pEscSeq_c[1] >= '0' && pEscSeq_c[1] <= '9')
            {
              /* Extended escape, read additional byte. */
              if (read(STDIN_FILENO, &pEscSeq_c[2], 1) != 1)
              {
                break;
              }
            }
            switch (pEscSeq_c[1])
            {
              case '1':
                switch (pEscSeq_c[2])
                {
                  case '5':
                    Rts_U32 = CONIO_FUNCTION_KEY_F5;
                    break;

                  case '7':  //Not a mistake (no 6)
                    Rts_U32 = CONIO_FUNCTION_KEY_F6;
                    break;

                  case '8':
                    Rts_U32 = CONIO_FUNCTION_KEY_F7;
                    break;

                  case '9':
                    Rts_U32 = CONIO_FUNCTION_KEY_F8;
                    break;

                  default:
                    break;
                }
                break;

              case '2':
                switch (pEscSeq_c[2])
                {
                  case '0':
                    Rts_U32 = CONIO_FUNCTION_KEY_F9;
                    break;

                  case '1':
                    Rts_U32 = CONIO_FUNCTION_KEY_F10;
                    break;

                  case '3': //Or 4 can't test F11 fullscreen
                    Rts_U32 = CONIO_FUNCTION_KEY_F11;
                    break;

                  case '4':
                    Rts_U32 = CONIO_FUNCTION_KEY_F12;
                    break;

                  case '~':
                    Rts_U32 = CONIO_SPECIAL_KEY_INSERT;
                    break;

                  default:
                    break;
                }
                break;

              case '3':
                switch (pEscSeq_c[2])
                {
                  case '~':
                    Rts_U32 = CONIO_SPECIAL_KEY_DELETE;
                    break;

                  default:
                    break;
                }
                break;

              case '5':
                switch (pEscSeq_c[2])
                {
                  case '~':
                    Rts_U32 = CONIO_SPECIAL_KEY_PAGE_UP;
                    break;

                  default:
                    break;
                }
                break;

              case '6':
                switch (pEscSeq_c[2])
                {
                  case '~':
                    Rts_U32 = CONIO_SPECIAL_KEY_PAGE_DOWN;
                    break;

                  default:
                    break;
                }
                break;

              case 'A':
                Rts_U32 = CONIO_SPECIAL_KEY_ARROW_UP;
                break;

              case 'B':
                Rts_U32 = CONIO_SPECIAL_KEY_ARROW_DOWN;
                break;

              case 'C':
                Rts_U32 = CONIO_SPECIAL_KEY_ARROW_RIGHT;
                break;

              case 'D':
                Rts_U32 = CONIO_SPECIAL_KEY_ARROW_LEFT;
                break;

              case 'F':
                Rts_U32 = CONIO_SPECIAL_KEY_END;
                break;

              case 'H':
                Rts_U32 = CONIO_SPECIAL_KEY_HOME;
                break;

              default:
                break;
            }
        } //switch
      } //if ((read(STDIN_FILENO, &pEscSeq_c[0], 1) == 1) && (read(STDIN_FILENO, &pEscSeq_c[1], 1) == 1))
    } //if (poll(&Poll_X, 1, 1) == 1)
  } //else if (Rts_U32==CONIO_SPECIAL_KEY_ESC)
  if (_OnlyModifier_B)
  {
    Rts_U32 = 0;
  }
#endif
  Rts_U32 = Modifier_U32 | Rts_U32;
  return Rts_U32;
}


uint32_t BofConio::GetCh(bool _OnlyModifier_B)
{
  uint32_t Rts_U32, Modifier_U32;

  Modifier_U32 = 0;
  Rts_U32 = 0;

#if defined(_WIN32)
  Rts_U32 = GetChAfterOneChar(_OnlyModifier_B, _OnlyModifier_B ? 0 : _getch());
#else
  char Ch_c;
  if (isatty(STDIN_FILENO))
  {
    if (linenoise::enableRawMode(STDIN_FILENO))
    {
      if (read(STDIN_FILENO, &Ch_c, 1) == 1)
      {
        Rts_U32 = GetChAfterOneChar(_OnlyModifier_B, Ch_c);
      }
      linenoise::disableRawMode(STDIN_FILENO);
      if (_OnlyModifier_B)
      {
        Rts_U32 = 0;
      }
    }
  }
#endif
  Rts_U32 = Modifier_U32 | Rts_U32;
  return Rts_U32;
}

//use ctrl-c to exit
BOFERR BofConio::Readline(const std::string &_rPrompt_S, std::string &_rInputLine_S)
{
  BOFERR Rts_E;

  _rInputLine_S = "";
  linenoise::Readline(_rPrompt_S.c_str(), _rInputLine_S);
  if (_rInputLine_S != "")
  {
    linenoise::AddHistory(_rInputLine_S.c_str());
  }
  Rts_E = BOF_ERR_NO_ERROR;

  return Rts_E;
}

BOFERR BofConio::SetTextAttribute(uint32_t _TextAttributeFlag_U32)  //Use CONIO_TEXT_ATTRIBUTE_FLAG enum ored value
{
  BOFERR Rts_E = BOF_ERR_NOT_SUPPORTED;

#if defined(_WIN32)
  char pAttribute_c[64], *p_c;
  p_c = pAttribute_c;

  if ((_TextAttributeFlag_U32 & ~(CONIO_TEXT_ATTRIBUTE_FLAG_NORMAL | CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE | CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE)) == 0)
  {
    *p_c++ = 0x1B;
    *p_c++ = '[';
    *p_c++ = (_TextAttributeFlag_U32 & CONIO_TEXT_ATTRIBUTE_FLAG_BOLD) ? '1' : '0';
    *p_c++ = ';';
    if (_TextAttributeFlag_U32 & CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE)
    {
      *p_c++ = '4';
    }
    else
    {
      *p_c++ = '2';
      *p_c++ = '4';
    }
    *p_c++ = ';';

    if (_TextAttributeFlag_U32 & CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE)
    {
      *p_c++ = '7';
    }
    else
    {
      *p_c++ = '2';
      *p_c++ = '7';
    }
    *p_c++ = 'm';
    *p_c = 0;
    printf("%s", pAttribute_c);

    Rts_E = BOF_ERR_NO_ERROR;
  }
#else
  _TextAttributeFlag_U32 = _TextAttributeFlag_U32;
#endif
  return Rts_E;
}

BOFERR BofConio::SetForegroundTextColor(CONIO_TEXT_COLOR _ForegroundColor_E)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  char   pAttribute_c[64], *p_c;
  p_c = pAttribute_c;
  *p_c++ = 0x1B;
  *p_c++ = '[';
  switch (_ForegroundColor_E)
  {
    case CONIO_TEXT_COLOR_BLACK:
      *p_c++ = '3';
      *p_c++ = '0';
      break;

    case CONIO_TEXT_COLOR_RED:
      *p_c++ = '3';
      *p_c++ = '1';
      break;

    case CONIO_TEXT_COLOR_GREEN:
      *p_c++ = '3';
      *p_c++ = '2';
      break;

    case CONIO_TEXT_COLOR_YELLOW:
      *p_c++ = '3';
      *p_c++ = '3';
      break;

    case CONIO_TEXT_COLOR_BLUE:
      *p_c++ = '3';
      *p_c++ = '4';
      break;

    case CONIO_TEXT_COLOR_MAGENTA:
      *p_c++ = '3';
      *p_c++ = '5';
      break;

    case CONIO_TEXT_COLOR_CYAN:
      *p_c++ = '3';
      *p_c++ = '6';
      break;

    case CONIO_TEXT_COLOR_WHITE:
      *p_c++ = '3';
      *p_c++ = '7';
      break;

    case CONIO_TEXT_COLOR_BRIGHT_BLACK:
      *p_c++ = '9';
      *p_c++ = '0';
      break;

    case CONIO_TEXT_COLOR_BRIGHT_RED:
      *p_c++ = '9';
      *p_c++ = '1';
      break;

    case CONIO_TEXT_COLOR_BRIGHT_GREEN:
      *p_c++ = '9';
      *p_c++ = '2';
      break;

    case CONIO_TEXT_COLOR_BRIGHT_YELLOW:
      *p_c++ = '9';
      *p_c++ = '3';
      break;

    case CONIO_TEXT_COLOR_BRIGHT_BLUE:
      *p_c++ = '9';
      *p_c++ = '4';
      break;

    case CONIO_TEXT_COLOR_BRIGHT_MAGENTA:
      *p_c++ = '9';
      *p_c++ = '5';
      break;

    case CONIO_TEXT_COLOR_BRIGHT_CYAN:
      *p_c++ = '9';
      *p_c++ = '6';
      break;

    case CONIO_TEXT_COLOR_BRIGHT_WHITE:
    default:
      *p_c++ = '9';
      *p_c++ = '7';
      break;
  }
  *p_c++ = 'm';
  *p_c = 0;
  printf("%s", pAttribute_c);

  return Rts_E;
}
BOFERR BofConio::SetBackgroundTextColor(CONIO_TEXT_COLOR _BackgroundColor_E)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  char   pAttribute_c[64], *p_c;
  p_c = pAttribute_c;
  *p_c++ = 0x1B;
  *p_c++ = '[';
  switch (_BackgroundColor_E)
  {
    case CONIO_TEXT_COLOR_BLACK:
      *p_c++ = '4';
      *p_c++ = '0';
      break;

    case CONIO_TEXT_COLOR_RED:
      *p_c++ = '4';
      *p_c++ = '1';
      break;

    case CONIO_TEXT_COLOR_GREEN:
      *p_c++ = '4';
      *p_c++ = '2';
      break;

    case CONIO_TEXT_COLOR_YELLOW:
      *p_c++ = '4';
      *p_c++ = '3';
      break;

    case CONIO_TEXT_COLOR_BLUE:
      *p_c++ = '4';
      *p_c++ = '4';
      break;

    case CONIO_TEXT_COLOR_MAGENTA:
      *p_c++ = '4';
      *p_c++ = '5';
      break;

    case CONIO_TEXT_COLOR_CYAN:
      *p_c++ = '4';
      *p_c++ = '6';
      break;

    case CONIO_TEXT_COLOR_WHITE:
      *p_c++ = '4';
      *p_c++ = '7';
      break;


    case CONIO_TEXT_COLOR_BRIGHT_BLACK:
      *p_c++ = '1';
      *p_c++ = '0';
      *p_c++ = '0';
      break;

    case CONIO_TEXT_COLOR_BRIGHT_RED:
      *p_c++ = '1';
      *p_c++ = '0';
      *p_c++ = '1';
      break;

    case CONIO_TEXT_COLOR_BRIGHT_GREEN:
      *p_c++ = '1';
      *p_c++ = '0';
      *p_c++ = '2';
      break;

    case CONIO_TEXT_COLOR_BRIGHT_YELLOW:
      *p_c++ = '1';
      *p_c++ = '0';
      *p_c++ = '3';
      break;

    case CONIO_TEXT_COLOR_BRIGHT_BLUE:
      *p_c++ = '1';
      *p_c++ = '0';
      *p_c++ = '4';
      break;

    case CONIO_TEXT_COLOR_BRIGHT_MAGENTA:
      *p_c++ = '1';
      *p_c++ = '0';
      *p_c++ = '5';
      break;

    case CONIO_TEXT_COLOR_BRIGHT_CYAN:
      *p_c++ = '1';
      *p_c++ = '0';
      *p_c++ = '6';
      break;

    case CONIO_TEXT_COLOR_BRIGHT_WHITE:
    default:
      *p_c++ = '1';
      *p_c++ = '0';
      *p_c++ = '7';
      break;
  }
  *p_c++ = 'm';
  *p_c = 0;
  printf("%s", pAttribute_c);

  return Rts_E;
}

BOFERR BofConio::SetForegroundTextColor(BOF_RGBA _ForegroundColor_X)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  char   pAttribute_c[64];

  sprintf(pAttribute_c, "\x1b[38;2;%d;%d;%dm", _ForegroundColor_X.r_U8, _ForegroundColor_X.g_U8, _ForegroundColor_X.b_U8);
  printf("%s", pAttribute_c);
  return Rts_E;
}
BOFERR BofConio::SetBackgroundTextColor(BOF_RGBA _BackgroundColor_X)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  char   pAttribute_c[64];

  sprintf(pAttribute_c, "\x1b[48;2;%d;%d;%dm", _BackgroundColor_X.r_U8, _BackgroundColor_X.g_U8, _BackgroundColor_X.b_U8);
  printf("%s", pAttribute_c);
  return Rts_E;
}

BOFERR BofConio::SetTextCursorState(CONIO_TEXT_CURSOR_STATE _CursorState_E)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  char   pAttribute_c[64], *p_c;
  p_c = pAttribute_c;
  *p_c++ = 0x1B;
  *p_c++ = '[';
  *p_c++ = '?';
  switch (_CursorState_E)
  {
    case CONIO_TEXT_CURSOR_STATE_BLINK_ON:
      *p_c++ = '1';
      *p_c++ = '2';
      *p_c++ = 'h';
      break;

    case CONIO_TEXT_CURSOR_STATE_BLINK_OFF:
      *p_c++ = '1';
      *p_c++ = '2';
      *p_c++ = 'l';
      break;

    case CONIO_TEXT_CURSOR_STATE_OFF:
      *p_c++ = '2';
      *p_c++ = '5';
      *p_c++ = 'l';
      break;

    default:
    case CONIO_TEXT_CURSOR_STATE_ON:
      *p_c++ = '2';
      *p_c++ = '5';
      *p_c++ = 'h';
      break;
  }
  *p_c = 0;
  printf("%s", pAttribute_c);

  return Rts_E;
}
BOFERR BofConio::SetTextCursorPosition(uint32_t _x_U32, uint32_t _y_U32)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if ((_x_U32 >= 1) && (_x_U32 <= mConsoleWidth_U32) && (_y_U32 >= 1) && (_y_U32 <= mConsoleHeight_U32))
  {
    Rts_E = BOF_ERR_NO_ERROR;
    printf("\x1b[%d;%df", _y_U32, _x_U32);
  }
  return Rts_E;
}

//https://github.com/sol-prog/ansi-escape-codes-windows-posix-terminals-c-programming-examples/blob/master/ansi_escapes.c

BOFERR BofConio::GetTextCursorPosition(uint32_t &_rX_U32, uint32_t &_rY_U32)
{
  BOFERR Rts_E = BOF_ERR_INPUT;

#if defined(_WIN32)
  char pAttribute_c[64];
  int Char_i, Index_i;

  printf("\x1b[6n");

  Index_i = 0;
  do
  {
    Char_i = getchar();
    pAttribute_c[Index_i++] = (char)Char_i;
    if (Char_i == 'R')
    {
      pAttribute_c[Index_i] = 0;
      sscanf(pAttribute_c, "\x1b[%d;%dR", &_rY_U32, &_rX_U32);
      Rts_E = BOF_ERR_NO_ERROR;
      break;
    }
  } while (Index_i < sizeof(pAttribute_c) - 4);
  fseek(stdin, 0, SEEK_END);
#else
  _rX_U32 = 1;
  _rY_U32 = 1;
#endif
  return Rts_E;
}
BOFERR BofConio::SetTextWindowTitle(const std::string &_rTitle_S)
{
  BOFERR      Rts_E = BOF_ERR_NO_ERROR;
  std::string Attribute_S;
  Attribute_S = Bof_Sprintf("\x1b]2;%s\x07", _rTitle_S.c_str());
  printf("%s", Attribute_S.c_str());

  //	char pAttribute_c[64], *p_c;
  //	sprintf(pAttribute_c, "\x1b]2;%s\x07", _rTitle_S.c_str());
  //	printf("%s", pAttribute_c);

  return Rts_E;
}

BOFERR BofConio::Clear(CONIO_CLEAR _ClearType_E)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  char   pAttribute_c[64], *p_c;

  p_c = pAttribute_c;
  *p_c++ = 0x1B;
  *p_c++ = '[';
  switch (_ClearType_E)
  {
    case CONIO_CLEAR_LINE_FROM_CURSOR_TO_END:
      *p_c++ = '0';
      *p_c++ = 'd';
      *p_c++ = 'K';
      break;

    case CONIO_CLEAR_LINE_FROM_CURSOR_TO_BEGIN:
      *p_c++ = '1';
      *p_c++ = 'd';
      *p_c++ = 'K';
      break;

    case CONIO_CLEAR_LINE:
      *p_c++ = 'K';
      break;

    case CONIO_CLEAR_ALL_FROM_CURSOR_TO_END:
      *p_c++ = '0';
      *p_c++ = 'J';
      break;

    case CONIO_CLEAR_ALL_FROM_CURSOR_TO_BEGIN:
      *p_c++ = '1';
      *p_c++ = 'J';
      break;

    default:
    case CONIO_CLEAR_ALL:
      SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_NORMAL);
      SetTextCursorPosition(1, 1);
      *p_c++ = '2';
      *p_c++ = 'J';
      break;

  }
  *p_c = 0;
  printf("%s", pAttribute_c);

  return Rts_E;
}
BOFERR BofConio::Reset()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  printf("\x1b[0m");
  return Rts_E;
}

BOFERR BofConio::Printf(const char *_pFormat_c, ...)
{
  BOFERR  Rts_E = BOF_ERR_NO_ERROR;
  char    pText_c[0x1000];
  va_list Arg;

  va_start(Arg, _pFormat_c);
  vsnprintf(pText_c, sizeof(pText_c), _pFormat_c, Arg);
  va_end(Arg);

  printf("%s", pText_c);




  /*
    std::cout << (bold, underline) << "Testing common SGR codes:\n";
    std::cout << (bold) << "Bold text\n";
    std::cout << (underline) << "Underlined text\n";
    std::cout << (reverse) << "Reversed text\n\n";

    std::cout << (bold, underline) << "Testing unusual codes:\n";
    std::cout << (faint) << "Faint";
    std::cout << " ";
    std::cout << (italic) << "Italic";
    std::cout << " ";
    std::cout << (blink_slow) << "Blinking slowly";
    std::cout << " ";
    std::cout << (blink_fast) << "Blinking quickly";
    std::cout << "\n";
    std::cout << (conceal) << "Concealed";
    std::cout << " (concealed)";
    std::cout << " ";
    std::cout << (strike) << "Crossed out";
    std::cout << " ";
    std::cout << (frame) << "Framed";
    std::cout << " ";
    std::cout << (encircle) << "Encircled";
    std::cout << "\n";
    std::cout << (overline) << "Overlined";
    std::cout << "\n";

    std::cout << (bold, underline) << "Testing colors:\n";
    std::cout << (red_fg) << "Red foreground";
    std::cout << "\n";
    std::cout << (cyan_bg) << "Cyan fackground";
    std::cout << "\n";
    std::cout << (white_fg, black_bg) << "White foreground, black background";
    std::cout << "\n";
    std::cout << (blue_fg) << "Blue foreground";
    std::cout << "\n";
    std::cout << (b_green_fg) << "Bright green foreground";
    std::cout << "\n\n";

      std::cout << (bold, underline) << "Testing 24-bit color:\n";

      for(int r = 0; r <= 255; r+=51)
      {
        for(int g = 0; g <= 255; g+=51)
        {
          for(int b = 0; b <= 255; b+=51)
          {
            int color = (b + (g << 8) + (r << 16));
            std::cout << color::fg(r,g,b) + bold << std::setfill('0') <<
            std::showbase << std::setw(8) << std::hex << std::internal << color;
            std::cout << " ";
          }
          std::cout << "\n";
        }
        std::cout << "\n";
      }
    */
  return Rts_E;
}
END_BOF_NAMESPACE()