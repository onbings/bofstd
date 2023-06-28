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
  BOF_RGBA ForeColor_X, BackColor_X;

  BOF_CONIO_PARAM BofConioParam_X;
  std::unique_ptr<BofConio> puBofConio;
  puBofConio.reset(new BofConio(BofConioParam_X));

  puBofConio->SetTextWindowTitle("puBofConio->SetTextWindowTitle");
  puBofConio->GetTextCursorPosition(X_U32, Y_U32);
  puBofConio->SetTextCursorState(CONIO_TEXT_CURSOR_STATE::CONIO_TEXT_CURSOR_STATE_ON);
  puBofConio->SetTextCursorState(CONIO_TEXT_CURSOR_STATE::CONIO_TEXT_CURSOR_STATE_BLINK_OFF);
  puBofConio->SetTextCursorState(CONIO_TEXT_CURSOR_STATE::CONIO_TEXT_CURSOR_STATE_BLINK_ON);
  puBofConio->SetTextCursorState(CONIO_TEXT_CURSOR_STATE::CONIO_TEXT_CURSOR_STATE_OFF);

  puBofConio->SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE);
  puBofConio->Printf("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE\n");

  puBofConio->Clear(CONIO_CLEAR::CONIO_CLEAR_ALL);
  puBofConio->Printf("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE\n");
  puBofConio->GetTextCursorPosition(X_U32, Y_U32);
  puBofConio->SetTextCursorPosition(X_U32 + 30, Y_U32 - 1);
  puBofConio->Clear(CONIO_CLEAR::CONIO_CLEAR_ALL_FROM_CURSOR_TO_BEGIN);
  puBofConio->SetTextCursorPosition(X_U32 + 32, Y_U32 - 1);
  puBofConio->Clear(CONIO_CLEAR::CONIO_CLEAR_ALL_FROM_CURSOR_TO_END);

  puBofConio->SetTextCursorPosition(1, 1);
  puBofConio->Printf("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE\n");
  puBofConio->Clear(CONIO_CLEAR::CONIO_CLEAR_LINE);
  puBofConio->Printf("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE\n");
  puBofConio->Clear(CONIO_CLEAR::CONIO_CLEAR_LINE_FROM_CURSOR_TO_BEGIN);
  puBofConio->Printf("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE\n");
  puBofConio->Clear(CONIO_CLEAR::CONIO_CLEAR_LINE_FROM_CURSOR_TO_END);

  puBofConio->SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE);
  puBofConio->Printf("CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE\n");
  puBofConio->SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BOLD);
  puBofConio->Printf("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD\n");
  puBofConio->SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE);
  puBofConio->Printf("CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE\n");
  puBofConio->SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_NORMAL);
  puBofConio->Printf("CONIO_TEXT_ATTRIBUTE_FLAG_NORMAL\n");

  puBofConio->SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE);
  puBofConio->Printf("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE\n");

  puBofConio->SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE);
  puBofConio->Printf("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE\n");

  puBofConio->SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE | CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE);
  puBofConio->Printf("CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE | CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE\n");

  puBofConio->SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE | CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE);
  puBofConio->Printf("CONIO_TEXT_ATTRIBUTE_FLAG_BOLD | CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE | CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE\n");

  puBofConio->SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_NORMAL);
  for (i = 0; i < 16; i++)
  {
    for (j = 0; j < 16; j++)
    {
      puBofConio->SetForegroundTextColor(static_cast<CONIO_TEXT_COLOR>(i));
      puBofConio->SetBackgroundTextColor(static_cast<CONIO_TEXT_COLOR>(j));
      puBofConio->Printf("Text in color %d,%d\n", i, j);
    }
  }

  puBofConio->SetTextAttribute(CONIO_TEXT_ATTRIBUTE_FLAG_NORMAL);
  for (ForeColor_X.r_U8 = 0; ForeColor_X.r_U8 < 250; ForeColor_X.r_U8 = static_cast<uint8_t>(ForeColor_X.r_U8 + 50))
  {
    BackColor_X.r_U8 = static_cast<uint8_t>(255 - ForeColor_X.r_U8);
    for (ForeColor_X.g_U8 = 0; ForeColor_X.g_U8 < 250; ForeColor_X.g_U8 = static_cast<uint8_t>(ForeColor_X.g_U8 + 50))
    {
      BackColor_X.g_U8 = static_cast<uint8_t>(255 - ForeColor_X.g_U8);
      for (ForeColor_X.b_U8 = 0; ForeColor_X.b_U8 < 250; ForeColor_X.b_U8 = static_cast<uint8_t>(ForeColor_X.b_U8 + 50))
      {
        BackColor_X.b_U8 = static_cast<uint8_t>(255 - ForeColor_X.b_U8);
        Color_U32 = (static_cast<uint32_t>(ForeColor_X.b_U8) + (static_cast<uint32_t>(ForeColor_X.g_U8) << 8) + (static_cast<uint32_t>(ForeColor_X.r_U8) << 16));
        puBofConio->SetForegroundTextColor(ForeColor_X);
        puBofConio->SetBackgroundTextColor(BackColor_X);
        puBofConio->Printf("%08X ", Color_U32);
      }
      puBofConio->Printf("\n");
    }
    puBofConio->Printf("\n");
  }
}

// Cannot be run under teamcity as it need keystrokes in the terminal to succeed
TEST(ConIo_Test, DISABLED_Input)
{
  BOF_CONIO_PARAM BofConioParam_X;
  std::unique_ptr<BofConio> puBofConio = nullptr;
  uint32_t Key_U32, KeyEnd_U32;
  std::string InputLine_S;

  puBofConio.reset(new BofConio(BofConioParam_X));
  puBofConio->Readline("Prompt>", InputLine_S);

  do
  {
    printf("Wait for a key press (ESC to leave)\n");
    do
    {
      Key_U32 = puBofConio->KbHit(100);
    } while (Key_U32 == 0);
    printf("You press %s/%08X\n", puBofConio->KeyToString(Key_U32).c_str(), Key_U32);
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
    Key_U32 = puBofConio->GetCh(false);
    printf("You press '%s'/0x%08X (0x%08X will exit)\n", puBofConio->KeyToString(Key_U32).c_str(), Key_U32, KeyEnd_U32);
  } while (Key_U32 != KeyEnd_U32);

  printf("Edit line and history, Type 'quit' to leave\n");
  do
  {
    puBofConio->Readline("Prompt>", InputLine_S);
  } while (InputLine_S != "quit");
}
