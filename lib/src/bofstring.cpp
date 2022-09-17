/*
 * Copyright (c) 2000-2020, Onbings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bofstring interface.
 *
 * Name:        bofstring.h
 * Author:      Bernard HARMEL: b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Jan 19 2017  BHA : Initial release
 */
/*** Include files ***********************************************************/

#include <bofstd/bofstring.h>
#include <iterator>
#include <algorithm>
#include <string.h>
#include <codecvt>
#include <locale>
#include <inttypes.h>

#if defined (_WIN32)

#include <clocale>
#include  <cctype>

#else

#endif

BEGIN_BOF_NAMESPACE()
std::string Bof_BuildFixedLengthLine(const char *_pLine_c, uint32_t _MinLength_U32, char _LeftChar_c, char _FillChar_c, char _RightChar_c)
{
  std::string Rts_S(_pLine_c);

  if ((Rts_S.size() + 3) <= _MinLength_U32)
  {
    Rts_S += _LeftChar_c;
    Rts_S = Rts_S.insert(Rts_S.size(), (_MinLength_U32 - Rts_S.size() - 1), _FillChar_c);
    Rts_S += _RightChar_c;

  }
  return Rts_S;
}

std::string Bof_StringRightTrim(const std::string &_rStrToTrim_S, const char *_pTrimCharList_c)
{
  std::string Rts_S = _rStrToTrim_S;
  if (_pTrimCharList_c)
  {
    std::size_t Pos = Rts_S.find_last_not_of(_pTrimCharList_c);
    if (Pos != std::string::npos)
    {
      Rts_S.erase(Pos + 1);
    }
    else
    {
      Rts_S.clear();
    }
  }
  return Rts_S;
}

std::string Bof_StringLeftTrim(const std::string &_rStrToTrim_S, const char *_pTrimCharList_c)
{
  std::string Rts_S = _rStrToTrim_S;
  if (_pTrimCharList_c)
  {
    std::size_t Pos = Rts_S.find_first_not_of(_pTrimCharList_c);
    if (Pos != std::string::npos)
    {
      Rts_S.erase(0, Pos);
    }
    else
    {
      Rts_S.clear();
    }
  }
  return Rts_S;
}

std::string Bof_StringTrim(const std::string &_rStrToTrim_S, const char *_pTrimCharList_c)
{
  return Bof_StringLeftTrim(Bof_StringRightTrim(_rStrToTrim_S, _pTrimCharList_c), _pTrimCharList_c);
}

std::string Bof_StringRemove(const std::string &_rStr_S, const std::string &_rCharToRemove_S)
{
  std::string Rts_S;
  std::copy_if(_rStr_S.begin(), _rStr_S.end(), std::back_inserter(Rts_S), [_rCharToRemove_S](char c) { return (strchr(_rCharToRemove_S.c_str(), c) == nullptr); });
  return Rts_S;
}

std::string Bof_StringReplace(const std::string &_rStr_S, const std::string &_rCharToReplace_S, char _ReplaceChar_c)
{
  std::string Rts_S = _rStr_S;
  std::replace_if(Rts_S.begin(), Rts_S.end(), [_rCharToReplace_S](char c) { return (strchr(_rCharToReplace_S.c_str(), c) != nullptr); }, _ReplaceChar_c);
  return Rts_S;
}

std::string Bof_StringReplace(const std::string &_rStr_S, const std::string &_rStringToReplace_S, const std::string &_rReplaceString_S)
{
  std::string  Rts_S   = _rStr_S;
  const size_t OldSize = _rStringToReplace_S.length(), NewSize = _rReplaceString_S.length();
  size_t       Pos;

  // do nothing if line is shorter than the string to find
  if (OldSize <= Rts_S.length())
  {
    for (Pos = 0;; Pos += NewSize)
    {
      // Locate the substring to replace
      Pos = Rts_S.find(_rStringToReplace_S, Pos);
      if (Pos == std::string::npos)
      {
        break;
      }
      else
      {
        if (OldSize == NewSize)
        {
          // if they're same size, use std::string::replace
          Rts_S.replace(Pos, OldSize, _rReplaceString_S);
        }
        else
        {
          // if not same size, replace by erasing and inserting
          Rts_S.erase(Pos, OldSize);
          Rts_S.insert(Pos, _rReplaceString_S);
        }
      }
    }
  }
  return Rts_S;

}

bool Bof_StringIsPresent(const std::string &_rStr_S, const std::string &_rCharToLookFor_S)
{
  return (_rStr_S.find_first_of(_rCharToLookFor_S) != std::string::npos);
}

bool Bof_StringIsAllTheSameChar(const std::string &_rStr_S, char _CharToLookFor_c)
{
  bool Rts_B = false;

  if (!_rStr_S.empty())
  {
    Rts_B = (_rStr_S.find_first_not_of(_CharToLookFor_c) == std::string::npos) ? true : false;
  }
  return (Rts_B);
}

std::vector<std::string> Bof_StringSplit(const std::string &_rStr_S, const std::string &_rSplitDelimiter_S)
{
  std::vector<std::string> Rts;
  std::string::size_type   StartPos = 0;
  std::string::size_type   EndPos   = _rStr_S.find_first_of(_rSplitDelimiter_S);

  if (EndPos == std::string::npos)
  {
    Rts.push_back(_rStr_S);
  }
  else
  {
    do
    {
      Rts.push_back(_rStr_S.substr(StartPos, EndPos - StartPos));
      StartPos = ++EndPos;
      EndPos   = _rStr_S.find_first_of(_rSplitDelimiter_S, EndPos);

      if (EndPos == std::string::npos)
      {
        Rts.push_back(_rStr_S.substr(StartPos, _rStr_S.length()));
      }
    } while (EndPos != std::string::npos);
  }
  return Rts;
}

std::string Bof_StringJoin(const std::vector<std::string> &_rStr_S, const std::string &_JoinDelimiter_S)
{
  std::string Rts_S;

  for (std::vector<std::string>::const_iterator it = _rStr_S.begin(); it != _rStr_S.end(); ++it)
  {
    Rts_S += *it;
    if (it != _rStr_S.end() - 1)
    {
      Rts_S += _JoinDelimiter_S;
    }
  }
  return Rts_S;
}


size_t Bof_MultiByteToWideChar(const char *_pMultiByteStr_c, uint32_t _NbWideChar_U32, wchar_t *_pWideCharStr_wc, const char *_pLocale_c)
{
  size_t      Rts = static_cast< size_t > (-1);
  bool        Ok_B;
  std::locale GlobalLocale;

  if (_pMultiByteStr_c)
  {
    if (_pLocale_c)
    {
      if (_pLocale_c[0])
      {
        Ok_B = (std::setlocale(LC_ALL, _pLocale_c) != nullptr);
      }
      else
      {
        Ok_B = (std::setlocale(LC_ALL, "C") != nullptr);
      }
    }
    else
    {
      Ok_B = true;
    }
    if (Ok_B)
    {
      Rts = std::mbstowcs(_pWideCharStr_wc, _pMultiByteStr_c, _NbWideChar_U32);
    }
    if (_pLocale_c)
    {
      std::locale::global(GlobalLocale);
    }
  }

  return Rts;
}

size_t Bof_WideCharToMultiByte(const wchar_t *_pWideCharStr_wc, uint32_t _NbMultibyteChar_U32, char *_pMultiByteStr_c, const char *_pLocale_c)
{
  size_t      Rts = static_cast< size_t > (-1);
  bool        Ok_B;
  std::locale GlobalLocale;

  if (_pWideCharStr_wc)
  {
    if (_pLocale_c)
    {
      GlobalLocale = std::locale::global(std::locale(_pLocale_c));
      if (_pLocale_c[0])
      {
        Ok_B = (std::setlocale(LC_ALL, _pLocale_c) != nullptr);
      }
      else
      {
        Ok_B = (std::setlocale(LC_ALL, "C") != nullptr);
      }
    }
    else
    {
      Ok_B = true;
    }
    if (Ok_B)
    {
      Rts = std::wcstombs(_pMultiByteStr_c, _pWideCharStr_wc, _NbMultibyteChar_U32);
    }
    if (_pLocale_c)
    {
      std::locale::global(GlobalLocale);
    }

  }

  return Rts;
}
//https://codereview.stackexchange.com/questions/419/converting-between-stdwstring-and-stdstring
/*
It really depends what codecs are being used with std::wstring and std::string.
This answer assumes that the std::wstring is using a UTF - 16 encoding, andthat the conversion to std::string will use a UTF - 8 encoding.
*/

std::wstring Bof_Utf8ToUtf16(const std::string &_rUtf8Str_S)
{
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> Converter;
  return Converter.from_bytes(_rUtf8Str_S);
}

std::string Bof_Utf16ToUtf8(const std::wstring &_rUtf16Str_WS)
{
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> Converter;
  return Converter.to_bytes(_rUtf16Str_WS);
}

void Bof_RemoveDuplicateSuccessiveCharacter(std::string &_rInputString_S, char _Char_c)
{
  if (_Char_c == 0)
  {
    _rInputString_S.erase(std::unique(_rInputString_S.begin(), _rInputString_S.end()), _rInputString_S.end());
  }
  else
  {
    _rInputString_S.erase(std::unique(_rInputString_S.begin(), _rInputString_S.end(), [_Char_c](const char _Char1_c, const char _Char2_c) { return ((_Char1_c == _Char_c) && (_Char2_c == _Char_c)); }),
                          _rInputString_S.end());
  }
}


BOFERR Bof_GetUnsignedIntegerFromMultipleKeyValueString(const std::string &_rMultiKeyValueString_S, const std::string _rMultiKeyValueDelimiter_S, const std::string &_rKeyName_S,
                                                        const char _KeyValueSeparator_c, uint32_t &_rValue_U32)
{
  BOFERR      Rts_E;
  std::string Str_S;

  Rts_E = Bof_GetStringFromMultipleKeyValueString(_rMultiKeyValueString_S, _rMultiKeyValueDelimiter_S, _rKeyName_S, _KeyValueSeparator_c, Str_S);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    _rValue_U32 = static_cast<uint32_t>(std::atoi(Str_S.c_str()));
  }
  return Rts_E;
}

BOFERR
Bof_GetIntegerFromMultipleKeyValueString(const std::string &_rMultiKeyValueString_S, const std::string _rMultiKeyValueDelimiter_S, const std::string &_rKeyName_S, const char _KeyValueSeparator_c,
                                         int32_t &_rValue_S32)
{
  BOFERR   Rts_E;
  uint32_t Val_U32;

  Rts_E = Bof_GetUnsignedIntegerFromMultipleKeyValueString(_rMultiKeyValueString_S, _rMultiKeyValueDelimiter_S, _rKeyName_S, _KeyValueSeparator_c, Val_U32);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    _rValue_S32 = static_cast<int32_t>(Val_U32);
  }
  return Rts_E;
}

BOFERR
Bof_GetStringFromMultipleKeyValueString(const std::string &_rMultiKeyValueString_S, const std::string _rMultiKeyValueDelimiter_S, const std::string &_rKeyName_S, const char _KeyValueSeparator_c,
                                        std::string &_rValue_S)
{
  BOFERR                   Rts_E = BOF_ERR_EINVAL;
  std::string              Separator_S;
  std::vector<std::string> KeyValueList_S;
  uint32_t                 i_U32;
  std::string              MultiKeyValueString_S;

  if (_rMultiKeyValueDelimiter_S.size() >= 1)
  {
    if (_rMultiKeyValueDelimiter_S[0] != _KeyValueSeparator_c)
    {
      MultiKeyValueString_S = Bof_StringReplace(_rMultiKeyValueString_S, _rMultiKeyValueDelimiter_S, _rMultiKeyValueDelimiter_S[0]);
      Bof_RemoveDuplicateSuccessiveCharacter(MultiKeyValueString_S, _rMultiKeyValueDelimiter_S[0]);
      Separator_S    = std::string(1, _rMultiKeyValueDelimiter_S[0]) + std::string(1, _KeyValueSeparator_c);
      KeyValueList_S = Bof_StringSplit(MultiKeyValueString_S, Separator_S);

      Rts_E      = BOF_ERR_NOT_FOUND;
      for (i_U32 = 0; i_U32 < KeyValueList_S.size(); i_U32 += 2)
      {
        if (Bof_StringTrim(KeyValueList_S[i_U32]) == _rKeyName_S)
        {
          if ((i_U32 + 1) < KeyValueList_S.size())
          {
            _rValue_S = KeyValueList_S[i_U32 + 1];
            Rts_E     = BOF_ERR_NO_ERROR;
          }
          break;
        }
      }
    }
  }
  return Rts_E;
}

char *Bof_FastZeroStrncpy(char *_pDest_c, const char *_pSrc_c, size_t _NbMaxToCopy)
{
  char c_c, *pRts_c;

  pRts_c = _pDest_c;
  if ((_pDest_c) && (_pSrc_c) && (_NbMaxToCopy))
  {
    _NbMaxToCopy--;
    c_c = 0x01;
    while ((c_c) && (_NbMaxToCopy))
    {
      _NbMaxToCopy--;
      c_c = *_pSrc_c++;
      *_pDest_c++ = c_c;
    }

    if (c_c)
    {
      *_pDest_c = 0;
    }
  }

  return (pRts_c);
}

bool Bof_StringBeginWith(bool _CaseInsensitive_B, const std::string &_rString_S, const std::string &_rBeginWithThis_S)
{
  bool   Rts_B = true;
  size_t i;

  if (&_rBeginWithThis_S != &_rString_S)  // _rString_S and _rStartWiththis_S are not the same string
  {
    if (_rBeginWithThis_S.length() > _rString_S.length())
    {
      Rts_B = false;
    }
    else
    {
      for (i = 0; i < _rBeginWithThis_S.length(); i++)
      {
        if (_CaseInsensitive_B)
        {
          if (std::tolower(_rBeginWithThis_S[i]) != std::tolower(_rString_S[i]))
          {
            Rts_B = false;
            break;
          }
        }
        else
        {
          if (_rBeginWithThis_S[i] != _rString_S[i])
          {
            Rts_B = false;
            break;
          }
        }
      }
    }
  }
  return Rts_B;
}
bool Bof_StringContain(bool _CaseInsensitive_B, const std::string &_rString_S, const std::string &_rContaingThis_S)
{
  bool Rts_B = false;

  if (_rContaingThis_S.length() <= _rString_S.length())
  {
    auto It = std::search(_rString_S.begin(), _rString_S.end(), _rContaingThis_S.begin(), _rContaingThis_S.end(), [_CaseInsensitive_B](char _Ch1_c, char _Ch2_c) { return _CaseInsensitive_B ? (std::toupper(_Ch1_c) == std::toupper(_Ch2_c)) : (_Ch1_c == _Ch2_c); });
    Rts_B = (It != _rString_S.end());
  }
  return Rts_B;
}
std::vector<std::string> Bof_FindAllStringIncluding(bool _CaseInsensitive_B, bool _MustBeginWith_B, const std::string &_rStringToLookFor_S, const std::vector<std::string> &_rStringCollection)
{
  std::vector<std::string> Rts;

  if (_MustBeginWith_B)
  {
    for (auto Item : _rStringCollection)
    {
      if (Bof_StringBeginWith(_CaseInsensitive_B, Item, _rStringToLookFor_S))
      {
        Rts.push_back(Item);
      }
    }
  }
  else
  {
    for (auto Item : _rStringCollection)
    {
      if (Bof_StringContain(_CaseInsensitive_B, Item, _rStringToLookFor_S))
      {
        Rts.push_back(Item);
      }
    }
  }
  return Rts;
}

bool Bof_IsDecimal(const std::string &_rInput_S,int32_t &_rVal_S32)
{
//scanf returns 0 if the %d digit extraction fails, and 2 if there is anything after the digits captured by %c. And since * prevents the value from being stored, you can't even get an overflow.
  int Sts_i ;
  //Sts_i= sscanf(_rInput_S.c_str(), "%*u%*c");
  //int64_t Val_S64;
  char Ch_c;
  Sts_i = sscanf(_rInput_S.c_str(), "%d%c",&_rVal_S32,&Ch_c);
  return (Sts_i == 1);
}
bool Bof_IsDecimal(const std::string &_rInput_S,uint32_t &_rVal_U32)
{
  int Sts_i ;
  char Ch_c;
  Sts_i = sscanf(_rInput_S.c_str(), "%u%c",&_rVal_U32,&Ch_c);
  return (Sts_i == 1);
}
bool Bof_IsDecimal(const std::string &_rInput_S,int64_t &_rVal_S64)
{
  int Sts_i ;
  char Ch_c;
  Sts_i = sscanf(_rInput_S.c_str(), "%" PRId64 "%c",&_rVal_S64,&Ch_c);
  return (Sts_i == 1);
}
bool Bof_IsDecimal(const std::string &_rInput_S,uint64_t &_rVal_U64)
{
  int Sts_i ;
  char Ch_c;
  Sts_i = sscanf(_rInput_S.c_str(), "%" PRIu64 "%c",&_rVal_U64,&Ch_c);
  return (Sts_i == 1);
}
bool Bof_IsHexadecimal(const std::string &_rInput_S,uint32_t &_rVal_U32)
{
  int Sts_i ;
  char Ch_c;
  Sts_i = sscanf(_rInput_S.c_str(), "%x%c",&_rVal_U32,&Ch_c);
  return (Sts_i == 1);
}
bool Bof_IsHexadecimal(const std::string &_rInput_S,uint64_t &_rVal_U64)
{
  int Sts_i ;
  char Ch_c;
  Sts_i = sscanf(_rInput_S.c_str(), "%" PRIx64 "%c",&_rVal_U64,&Ch_c);
  return (Sts_i == 1);
}
bool Bof_IsDouble(const std::string &_rInput_S, double &_rVal_lf)
{
  int Sts_i ;
  char Ch_c;
  Sts_i = sscanf(_rInput_S.c_str(), "%lf%c",&_rVal_lf,&Ch_c);
  return (Sts_i == 1);
}
END_BOF_NAMESPACE()