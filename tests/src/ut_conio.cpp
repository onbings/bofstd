/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bofconio class
 *
 * Name:        ut_bofconio.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Apr 13 2019  BHA : Initial release
 */

/*** Include files ***********************************************************/

#include "gtestrunner.h"
#include <bofstd/bofconio.h>
#include <bofstd/bofsystem.h>

/*** Class *************************************************************************************************************************/

USE_BOF_NAMESPACE()

TEST(ConIo_Test, Output)
{
  int i, j;
  uint32_t Color_U32, X_U32, Y_U32;
  BOF_RGBA<uint8_t> ForeColor_X, BackColor_X;

  BOF_CONIO_PARAM BofConioParam_X;
  BofConio::S_Initialize(BofConioParam_X);

  BofConio::S_SetForegroundTextColor(CONIO_TEXT_COLOR_BRIGHT_WHITE);
  BofConio::S_SetBackgroundTextColor(CONIO_TEXT_COLOR_BLACK);

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BOLD);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD (1)\n");
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD (2)\n");

  BofConio::S_SetTextWindowTitle("BofConio::S_SetTextWindowTitle");
  BofConio::S_GetTextCursorPosition(X_U32, Y_U32);
  BofConio::S_SetTextCursorState(CONIO_TEXT_CURSOR_STATE::CONIO_TEXT_CURSOR_STATE_ON);
  BofConio::S_SetTextCursorState(CONIO_TEXT_CURSOR_STATE::CONIO_TEXT_CURSOR_STATE_BLINK_OFF);
  BofConio::S_SetTextCursorState(CONIO_TEXT_CURSOR_STATE::CONIO_TEXT_CURSOR_STATE_BLINK_ON);
  BofConio::S_SetTextCursorState(CONIO_TEXT_CURSOR_STATE::CONIO_TEXT_CURSOR_STATE_OFF);

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BOLD);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD (1)\n");
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD (2)\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_FAINT);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_FAINT\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_ITALIC);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_ITALIC\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE (1)\n");
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE (2)\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BLINK_SLOW);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_BLINK_SLOW\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BLINK_FAST);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_BLINK_FAST\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_CONCEAL);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_CONCEAL\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_STRIKE);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_STRIKE\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_FRAME);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_FRAME\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_ENCIRCLE);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_ENCIRCLE\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_OVERLINE);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_OVERLINE\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_FAINT | CONIO_TEXT_ATTRIBUTE_FLAG_ITALIC | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_FAINT | CONIO_TEXT_ATTRIBUTE_FLAG_ITALIC | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE\n");

  BofConio::S_Clear(CONIO_CLEAR::CONIO_CLEAR_ALL);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE\n");
  BofConio::S_GetTextCursorPosition(X_U32, Y_U32);
  BofConio::S_SetTextCursorPosition(X_U32 + 30, Y_U32 - 1);
  BofConio::S_Clear(CONIO_CLEAR::CONIO_CLEAR_ALL_FROM_CURSOR_TO_BEGIN);
  BofConio::S_SetTextCursorPosition(X_U32 + 32, Y_U32 - 1);
  BofConio::S_Clear(CONIO_CLEAR::CONIO_CLEAR_ALL_FROM_CURSOR_TO_END);

  BofConio::S_SetTextCursorPosition(1, 1);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE\n");
  BofConio::S_Clear(CONIO_CLEAR::CONIO_CLEAR_LINE);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE\n");
  BofConio::S_Clear(CONIO_CLEAR::CONIO_CLEAR_LINE_FROM_CURSOR_TO_BEGIN);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE\n");
  BofConio::S_Clear(CONIO_CLEAR::CONIO_CLEAR_LINE_FROM_CURSOR_TO_END);

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE\n");
  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BOLD);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD\n");
  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE\n");
  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_NORMAL);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_NORMAL\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE | CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE | CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE\n");

  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE | CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE);
  BofConio::S_PrintLine("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE | CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE\n");

  // BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_NORMAL);
  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE | CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE);
  for (i = 0; i < 16; i++)
  {
    BofConio::S_SetForegroundTextColor(static_cast<CONIO_TEXT_COLOR>(i));
    for (j = 0; j < 16; j++)
    {
      BofConio::S_SetBackgroundTextColor(static_cast<CONIO_TEXT_COLOR>(j));
      BofConio::S_PrintLine("Text in color %d,%d (1)\nText in color %d,%d (2)\n", i, j);
      //      BofConio::S_PrintLine("Text in color %d,%d (2)\n", i, j);
    }
  }

  // BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_NORMAL);
  BofConio::S_SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE | CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE);
  for (ForeColor_X.r = 0; ForeColor_X.r < 250; ForeColor_X.r = static_cast<uint8_t>(ForeColor_X.r + 50))
  {
    BackColor_X.r = static_cast<uint8_t>(255 - ForeColor_X.r);
    for (ForeColor_X.g = 0; ForeColor_X.g < 250; ForeColor_X.g = static_cast<uint8_t>(ForeColor_X.g + 50))
    {
      BackColor_X.g = static_cast<uint8_t>(255 - ForeColor_X.g);
      for (ForeColor_X.b = 0; ForeColor_X.b < 250; ForeColor_X.b = static_cast<uint8_t>(ForeColor_X.b + 50))
      {
        BackColor_X.b = static_cast<uint8_t>(255 - ForeColor_X.b);
        Color_U32 = (static_cast<uint32_t>(ForeColor_X.b) + (static_cast<uint32_t>(ForeColor_X.g) << 8) + (static_cast<uint32_t>(ForeColor_X.r) << 16));
        BofConio::S_SetForegroundTextColor(ForeColor_X);
        BofConio::S_SetBackgroundTextColor(BackColor_X);
        BofConio::S_PrintLine("%08X (1)\n", Color_U32);
        BofConio::S_PrintLine("%08X (2)\n", Color_U32);
      }
      BofConio::S_PrintLine("\n");
    }
    BofConio::S_PrintLine("\n");
  }
  BofConio::S_Shutdown();
}

// Cannot be run under teamcity as it need keystrokes in the terminal to succeed
TEST(ConIo_Test, DISABLED_Input)
// TEST(ConIo_Test, Input)
{
  BOF_CONIO_PARAM BofConioParam_X;
  uint32_t Key_U32, KeyEnd_U32;
  std::string InputLine_S;

  BofConio::S_Initialize(BofConioParam_X);
  BofConio::S_Readline("Prompt>", InputLine_S);

  do
  {
    printf("Wait for a key press (ESC to leave)\n");
    do
    {
      Key_U32 = BofConio::S_KbHit(100);
    } while (Key_U32 == 0);
    printf("You press %s/%08X\n", BofConio::S_KeyToString(Key_U32).c_str(), Key_U32);
  } while (Key_U32 != CONIO_SPECIAL_KEY_ESC);

#if defined(_WIN32)
  printf("Display keystroke, Press Left CTRL-ALT-SHIFT to exit\n");
  KeyEnd_U32 = (CONIO_MODIFIER_KEY_FLAG_LEFT_CTRL | CONIO_MODIFIER_KEY_FLAG_LEFT_MENU | CONIO_MODIFIER_KEY_FLAG_LEFT_SHIFT);
#else
  printf("Display keystroke, Press Esc to exit\n");
  KeyEnd_U32 = CONIO_SPECIAL_KEY_ESC;
#endif
  do
  {
    Key_U32 = BofConio::S_GetCh(false);
    printf("You press '%s'/0x%08X (0x%08X will exit)\n", BofConio::S_KeyToString(Key_U32).c_str(), Key_U32, KeyEnd_U32);
  } while (Key_U32 != KeyEnd_U32);

  printf("Edit line and history, Type 'quit' to leave\n");
  do
  {
    BofConio::S_Readline("Prompt>", InputLine_S);
  } while (InputLine_S != "quit");
  BofConio::S_Shutdown();
}
