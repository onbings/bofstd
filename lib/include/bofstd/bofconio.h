/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the console i/o functions
 *
 * Name:        bofconio.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Apr 13 2019  BHA : Initial release
 */
#pragma once

#include <bofstd/bofpath.h>

BEGIN_BOF_NAMESPACE()
// #define PrintfAt(x, y, pFormat, ...) PrintfAtColor(mForeColor_E, x, y, pFormat, ##__VA_ARGS__)
// #define PrintfColor(Color, pFormat, ...) PrintfAtColor(Color, 0, 0, pFormat, ##__VA_ARGS__)

enum CONIO_MODIFIER_KEY_FLAG : uint32_t
{
  CONIO_MODIFIER_KEY_FLAG_LEFT_CTRL = 0x80000000,
  CONIO_MODIFIER_KEY_FLAG_RIGHT_CTRL = 0x40000000,
  CONIO_MODIFIER_KEY_FLAG_LEFT_MENU = 0x20000000,
  CONIO_MODIFIER_KEY_FLAG_RIGHT_MENU = 0x10000000,
  CONIO_MODIFIER_KEY_FLAG_LEFT_SHIFT = 0x08000000,
  CONIO_MODIFIER_KEY_FLAG_RIGHT_SHIFT = 0x04000000,
  CONIO_MODIFIER_KEY_FLAG_PRINT_SCREEN = 0x02000000,
  CONIO_MODIFIER_KEY_FLAG_SCROLL_LOCK = 0x01000000,
  CONIO_MODIFIER_KEY_FLAG_PAUSE = 0x00800000,
  CONIO_MODIFIER_KEY_FLAG_NUM_LOCK = 0x00400000,
  CONIO_MODIFIER_KEY_FLAG_CAPS_LOCK = 0x00200000,
};
constexpr uint32_t CONIO_MODIFIER_MASK = 0xFF000000;
constexpr uint32_t CONIO_KEY_MASK = 0x0000FFFF;
constexpr uint32_t CONIO_IS_SHIFT_MASK = (CONIO_MODIFIER_KEY_FLAG_LEFT_SHIFT | CONIO_MODIFIER_KEY_FLAG_RIGHT_SHIFT | CONIO_MODIFIER_KEY_FLAG_CAPS_LOCK);
constexpr uint32_t CONIO_IS_CTRL_MASK = (CONIO_MODIFIER_KEY_FLAG_LEFT_CTRL | CONIO_MODIFIER_KEY_FLAG_RIGHT_CTRL);
constexpr uint32_t CONIO_IS_ALT_MASK = (CONIO_MODIFIER_KEY_FLAG_LEFT_MENU | CONIO_MODIFIER_KEY_FLAG_RIGHT_MENU);

enum CONIO_FUNCTION_KEY : uint32_t
{
  CONIO_FUNCTION_KEY_F1 = 0x00008001,
  CONIO_FUNCTION_KEY_F2,
  CONIO_FUNCTION_KEY_F3,
  CONIO_FUNCTION_KEY_F4,
  CONIO_FUNCTION_KEY_F5,
  CONIO_FUNCTION_KEY_F6,
  CONIO_FUNCTION_KEY_F7,
  CONIO_FUNCTION_KEY_F8,
  CONIO_FUNCTION_KEY_F9,
  CONIO_FUNCTION_KEY_F10,
  CONIO_FUNCTION_KEY_F11 = 0x0000E085,
  CONIO_FUNCTION_KEY_F12 = 0x0000E086,
};

enum CONIO_SPECIAL_KEY : uint32_t
{
  CONIO_SPECIAL_KEY_ESC = 0x0000001B,
  CONIO_SPECIAL_KEY_BACKSPACE = 0x00000008,
  CONIO_SPECIAL_KEY_TAB = 0x00000009,
  CONIO_SPECIAL_KEY_ENTER = 0x0000000D,
  CONIO_SPECIAL_KEY_INSERT = 0x0000E052,
  CONIO_SPECIAL_KEY_HOME = 0x0000E047,
  CONIO_SPECIAL_KEY_PAGE_UP = 0x0000E049,
  CONIO_SPECIAL_KEY_DELETE = 0x0000E053,
  CONIO_SPECIAL_KEY_END = 0x0000E04F,
  CONIO_SPECIAL_KEY_PAGE_DOWN = 0x000E051,
  CONIO_SPECIAL_KEY_ARROW_UP = 0x0000E048,
  CONIO_SPECIAL_KEY_ARROW_LEFT = 0x0000E04B,
  CONIO_SPECIAL_KEY_ARROW_DOWN = 0x0000E050,
  CONIO_SPECIAL_KEY_ARROW_RIGHT = 0x0000E04D,
  CONIO_SPECIAL_KEY_NONE = 0x0000FFFF,
};

enum CONIO_TEXT_ATTRIBUTE_FLAG : uint32_t
{
  CONIO_TEXT_ATTRIBUTE_FLAG_NORMAL = 0x00000000,
  CONIO_TEXT_ATTRIBUTE_FLAG_BOLD = 0x00000001,
  CONIO_TEXT_ATTRIBUTE_FLAG_UNDERLINE = 0x00000002,
  CONIO_TEXT_ATTRIBUTE_FLAG_REVERSE = 0x00000004,

  CONIO_TEXT_ATTRIBUTE_FLAG_FAINT = 0x00000100,
  CONIO_TEXT_ATTRIBUTE_FLAG_ITALIC = 0x00000200,
  CONIO_TEXT_ATTRIBUTE_FLAG_BLINK_SLOW = 0x00000400,
  CONIO_TEXT_ATTRIBUTE_FLAG_BLINK_FAST = 0x00000800,
  CONIO_TEXT_ATTRIBUTE_FLAG_CONCEAL = 0x00001000,
  CONIO_TEXT_ATTRIBUTE_FLAG_STRIKE = 0x00002000,
  CONIO_TEXT_ATTRIBUTE_FLAG_FRAME = 0x00004000,
  CONIO_TEXT_ATTRIBUTE_FLAG_ENCIRCLE = 0x00008000,
  CONIO_TEXT_ATTRIBUTE_FLAG_OVERLINE = 0x00010000,
};

enum CONIO_TEXT_COLOR : uint32_t
{
  CONIO_TEXT_COLOR_BLACK = 0,
  CONIO_TEXT_COLOR_RED,
  CONIO_TEXT_COLOR_GREEN,
  CONIO_TEXT_COLOR_YELLOW,
  CONIO_TEXT_COLOR_BLUE,
  CONIO_TEXT_COLOR_MAGENTA,
  CONIO_TEXT_COLOR_CYAN,
  CONIO_TEXT_COLOR_WHITE,
  CONIO_TEXT_COLOR_BRIGHT_BLACK,
  CONIO_TEXT_COLOR_BRIGHT_RED,
  CONIO_TEXT_COLOR_BRIGHT_GREEN,
  CONIO_TEXT_COLOR_BRIGHT_YELLOW,
  CONIO_TEXT_COLOR_BRIGHT_BLUE,
  CONIO_TEXT_COLOR_BRIGHT_MAGENTA,
  CONIO_TEXT_COLOR_BRIGHT_CYAN,
  CONIO_TEXT_COLOR_BRIGHT_WHITE,
  CONIO_TEXT_COLOR_NONE
};

enum CONIO_TEXT_CURSOR_STATE : uint32_t
{
  CONIO_TEXT_CURSOR_STATE_BLINK_ON = 0,
  CONIO_TEXT_CURSOR_STATE_BLINK_OFF,
  CONIO_TEXT_CURSOR_STATE_ON,
  CONIO_TEXT_CURSOR_STATE_OFF,
};

enum CONIO_CLEAR : uint32_t
{
  CONIO_CLEAR_LINE_FROM_CURSOR_TO_END = 0,
  CONIO_CLEAR_LINE_FROM_CURSOR_TO_BEGIN,
  CONIO_CLEAR_LINE,
  CONIO_CLEAR_ALL_FROM_CURSOR_TO_END,
  CONIO_CLEAR_ALL_FROM_CURSOR_TO_BEGIN,
  CONIO_CLEAR_ALL
};
struct BOF_CONIO_PARAM
{
  BofPath HistoryPathname;
  uint32_t HistoryMaxLength_U32;
  bool EditOnMultiLine_B;

  BOF_CONIO_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    HistoryPathname = "./history.txt";
    HistoryMaxLength_U32 = 4;
    EditOnMultiLine_B = true;
  }
};

class BOFSTD_EXPORT BofConio final
{

public:
  /*
    BofConio(const BOF_CONIO_PARAM &_rBofConioParam_X);
    virtual ~BofConio();

    // delete copy and move constructors and assign operators
    BofConio(BofConio const &) = delete;            // Copy construct
    BofConio(BofConio &&) = delete;                 // Move construct
    BofConio &operator=(BofConio const &) = delete; // Copy assign
    BofConio &operator=(BofConio &&) = delete;      // Move assign
  */
  static bool S_Initialize(const BOF_CONIO_PARAM &_rBofConioParam_X);
  static bool S_Shutdown();
  static uint32_t S_KbHit(uint32_t _TimeOutInMs_U32);
  static uint32_t S_GetCh(bool _OnlyModifier_B);
  static std::string S_KeyToString(uint32_t _Key_U32);

  static BOFERR S_Readline(const std::string &_rPrompt_S, std::string &_rInputLine_S);

  static BOFERR S_SetTextAttribute(uint32_t _TextAttributeFlag_U32); // Use CONIO_TEXT_ATTRIBUTE_FLAG enum ored value
  static BOFERR S_SetForegroundTextColor(CONIO_TEXT_COLOR _ForegroundColor_E);
  static BOFERR S_SetBackgroundTextColor(CONIO_TEXT_COLOR _BackgroundColor_E);
  static BOFERR S_SetForegroundTextColor(BOF_RGBA<uint8_t> _ForegroundColor_E);
  static BOFERR S_SetBackgroundTextColor(BOF_RGBA<uint8_t> _BackgroundColor_X);

  static BOFERR S_SetTextCursorState(CONIO_TEXT_CURSOR_STATE _CursorState_E);
  static BOFERR S_SetTextCursorPosition(uint32_t _x_U32, uint32_t _y_U32);
  static BOFERR S_GetTextCursorPosition(uint32_t &_rX_U32, uint32_t &_rY_U32);
  static BOFERR S_SetTextWindowTitle(const char *_pTitle_c);
  static BOFERR S_Clear(CONIO_CLEAR _ClearType_E);

  static BOFERR S_PrintLine(const char *_pFormat_c, ...);
  static void S_PrintLineColorAt(CONIO_TEXT_COLOR _ForegroundColor_E, uint32_t _x_U32, uint32_t _y_U32, const char *_pFormat_c, ...);

private:
  static uint32_t S_GetChAfterOneChar(bool _OnlyModifier_B, char _FirstChar_c);

  static BOF_CONIO_PARAM S_mBofConioParam_X;
  static uint32_t S_mConsoleWidth_U32;
  static uint32_t S_mConsoleHeight_U32;
  static char S_mpTextForeAttribute_c[0x40];
  static char S_mpTextBackAttribute_c[0x40];
  static char S_mpTextAttribute_c[0x40];
};

END_BOF_NAMESPACE()