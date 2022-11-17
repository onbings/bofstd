/*
 * Copyright (c) 2000-2020, Onbings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bofstring interface.
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
#pragma once

#include <bofstd/bofstd.h>

#include <cstdint>
#include <vector>

BEGIN_BOF_NAMESPACE()

BOFSTD_EXPORT std::string Bof_BuildFixedLengthLine(const char *_pLine_c, uint32_t _MinLength_U32, char _LeftChar_c, char _FillChar_c, char _RightChar_c);

static const char *S_pBofDefaultTrimCharList_c = " \a\f\n\r\t\v"; /*! Default character list used by Bof_RightTrim/Bof_LeftTrim/Bof_Trim*/

BOFSTD_EXPORT std::string Bof_StringToLower(const std::string &_rStrToConvert_S);
BOFSTD_EXPORT std::string Bof_StringToUpper(const std::string &_rStrToConvert_S);

// @brief trim from end of string (right)
// @param _rStrToTrim_S : Specifies the string to trim
// @param _pTrimCharList_c : Specifies the character list which must be removed from the right part of the string
// @return The right trimmed string
// @remarks None
BOFSTD_EXPORT std::string Bof_StringRightTrim(const std::string &_rStrToTrim_S, const char *_pTrimCharList_c = S_pBofDefaultTrimCharList_c);

// @brief Trim from beginning of string (left)
// @param _rStrToTrim_S : Specifies the string to trim
// @param _pTrimCharList_c : Specifies the character list which must be removed from the left part of string
// @return The left trimmed string
// @remarks None
BOFSTD_EXPORT std::string Bof_StringLeftTrim(const std::string &_rStrToTrim_S, const char *_pTrimCharList_c = S_pBofDefaultTrimCharList_c);

// @brief Trim from both ends of string (left & right)
// @param _rStrToTrim_S : Specifies the string to trim
// @param _pTrimCharList_c : Specifies the character list which must be removed from the left and right part of the string
// @return The trimmed string
// @remarks None
BOFSTD_EXPORT std::string Bof_StringTrim(const std::string &_rStrToTrim_S, const char *_pTrimCharList_c = S_pBofDefaultTrimCharList_c);

// @brief Remove a list of character from a string
// @param _rStr_S : Specifies the string to process
// @param _rCharToRemove_S : Specifies the character list which must be removed from the string
// @return The new string without the 'removed' character
// @remarks None
BOFSTD_EXPORT std::string Bof_StringRemove(const std::string &_rStr_S, const std::string &_rCharToRemove_S);

// @brief Replace a list of character in a string by another one
// @param _rStr_S : Specifies the string to process
// @param _rCharToReplace_S : Specifies the character list which must be replaced in the string
// @param _ReplaceChar_c : Specifies the character to use as replacement
// @return The new string with the 'replaced' characters
// @remarks None
BOFSTD_EXPORT std::string Bof_StringReplace(const std::string &_rStr_S, const std::string &_rCharToReplace_S, char _ReplaceChar_c);

// @brief Replace all occurence of a substring with another one
// @param _rStr_S : Specifies the string to process
// @param _rStringToReplace_S : Specifies the substring which must be replaced in the string
// @param _rReplaceString_S : Specifies the string to use as replacement
// @return The new string with the 'replaced' substring
// @remarks None
BOFSTD_EXPORT std::string Bof_StringReplace(const std::string &_rStr_S, const std::string &_rStringToReplace_S, const std::string &_rReplaceString_S);

// @brief Check if a string contains some specific characters
// @param _rStr_S : Specifies the string to process
// @param _rCharToLookFor_S : Specifies the character list which must be checked in the string
// @return true if one of the character present in _rCharToLookFor_S is detected in _rStr_S
// @remarks None
BOFSTD_EXPORT bool Bof_StringIsPresent(const std::string &_rStr_S, const std::string &_rCharToLookFor_S);

// @brief Check if a string if fully filled with a specific char
// @param _rStr_S : Specifies the string to process
// @param _CharToLookFor_c : Specifies the character to look for inside the string
// @return true if the string only contains the char _CharToLookFor_c
// @remarks None
BOFSTD_EXPORT bool Bof_StringIsAllTheSameChar(const std::string &_rStr_S, char _CharToLookFor_c);

// @brief Split a string given a list of potential delimiter
// @param _rStr_S : Specifies the string to process
// @param _rSplitDelimiter_S : Specifies the character list which must be used to split the string
// @return A vector containing the different string extracted from the input string
// @remarks None
BOFSTD_EXPORT std::vector<std::string> Bof_StringSplit(const std::string &_rStr_S, const std::string &_rSplitDelimiter_S);

// @brief Join a vector of string using a given delimiter (which can be several character long)
// @param _rStr_S : Specifies the string to process
// @param _rJoinDelimiter_S : Specifies the character string which must be used as delimiter in the join operation
// @return The new concatenated string
// @remarks None
BOFSTD_EXPORT std::string Bof_StringJoin(const std::vector<std::string> &_rStr_S, const std::string &_rJoinDelimiter_S);

// @brief Converts a multi byte character string to its wide character representation
// @param _pMultiByteStr_c : Specifies a pointer to the first element of a null - terminated multi byte string to convert
// @param _NbWideChar_U32 : Specifies the number of wide characters available in the array pointed to by _pWideCharStr_wc
// @param _pWideCharStr_wc : Specifies a pointer to wide character array where the wide string will be stored. If it is a null pointer, this function returns the number of wide characters that would be written to _pWideCharStr_wc
// @param _pLocale_c : Specifies system-specific locale identifier. Can be "" for the user-preferred locale or "C" for the minimal locale (cf std::setlocale)
// @return The number of wide characters, excluding the terminating L'\0', written to the destination array or -1 in case of error
// @remarks None
BOFSTD_EXPORT size_t Bof_MultiByteToWideChar(const char *_pMultiByteStr_c, uint32_t _NbWideChar_U32, wchar_t *_pWideCharStr_wc, const char *_pLocale_c = nullptr);

// @brief Converts a std::string into a std::wstring.
// @param _rUtf8Str_S : Specifies a reference to a multi byte string to convert
// @return The std::wstring representation of _rUtf8Str_S 
// @remarks None
BOFSTD_EXPORT std::wstring Bof_Utf8ToUtf16(const std::string &_rUtf8Str_S);

// @brief Converts a sequence of wide characters  to its narrow multi byte representation
// @param _pWideCharStr_wc : Specifies a pointer to the first element of a null-terminated wide string to convert
// @param _NbMultibyteChar_U32 : Specifies number of byte available in the array pointed to by _pMultiByteStr_c
// @param _pMultiByteStr_c : Specifies a pointer to narrow character array where the multi byte character will be stored
// @param _pLocale_c : Specifies system-specific locale identifier. Can be "" for the user-preferred locale or "C" for the minimal locale (cf std::setlocale)
// @return The number of multi byte characters, excluding the terminating '\0', written to the destination array or -1 in case of error
// @remarks None
BOFSTD_EXPORT size_t Bof_WideCharToMultiByte(const wchar_t *_pWideCharStr_wc, uint32_t _NbMultibyteChar_U32, char *_pMultiByteStr_c, const char *_pLocale_c = nullptr);

// @brief Converts a std::wstring into a std::string.
// @param _rUtf16Str_WS : Specifies a reference to a wide sharacter.
// @return The std::string representation of _rUtf16Str_WS 
// @remarks None
BOFSTD_EXPORT std::string Bof_Utf16ToUtf8(const std::wstring &_rUtf16Str_S);
// @brief Check if a string begin with a given string
// @param _CaseInsensitive_B : True if the comparaison is case insensitive
// @param _rString_S : Specifies the string which starts perhaps with _rBeginWithThis_S
// @param _rBeginWithThis_S : Specifies the string to compare
// @return True if _rString_S begins with _rStartWiththis_S
// @remarks None
BOFSTD_EXPORT bool Bof_StringBeginWith(bool _CaseInsensitive_B, const std::string &_rString_S, const std::string &_rBeginWithThis_S);

// @brief Check if a string begin with a given string
// @param _CaseInsensitive_B : True if the comparaison is case insensitive
// @param _rString_S : Specifies the string which conatins perhaps with _rContaingThis_S
// @param _rContaingThis_S : Specifies the string which must be contained in _rString_S
// @return True if _rString_S contains _rContaingThis_S
// @remarks None
BOFSTD_EXPORT bool Bof_StringContain(bool _CaseInsensitive_B, const std::string &_rString_S, const std::string &_rContaingThis_S);

// @brief Returns a vector of string which includes a given string
// @param _CaseSensitiveSearch_B : true if the search must be case sensitive
// @param _MustBeginWith_B : True if the string must appear in first postion, False if it must be conatained inside
// @param _rStringToLookFor_S : Specifies the which string we look fro in _rStringCollection
// @param _rStringCollection: Specifies a collection of string in which we look for
// @return a vector of string which contains a given string
// @remarks None
BOFSTD_EXPORT std::vector<std::string> Bof_FindAllStringIncluding(bool _CaseSensitiveSearch_B, bool _MustBeginWith_B, const std::string &_rStringToLookFor_S, const std::vector<std::string> &_rStringCollection);


// @brief Modify the input string by removing specified or unspecified successive character
// @param _rInputString_S : Specifies the string to parse
// @param _Char_c : Specifies the char to check for duplicate successive removal. If 0 is speced all dusplicate successive character are removed
// @return None
// @remarks Ex AAABBC->ABC
BOFSTD_EXPORT void Bof_RemoveDuplicateSuccessiveCharacter(std::string &_rInputString_S, char _Char_c);

// @brief Returns the unsigned integer key value of a key name. Each Key Value pair is composed of a key name, a separator and the key value.
// @param _rMultiKeyValueString_S : Specifies the multi key value string to parse
// @param _rMultiKeyValueDelimiter_S : Specifies a list of character which can be used to separate the different key value string (could be ;\r\n for example)
// @param _rKeyName_S : Specifies the key name to find
// @param _KeyValueSeparator_c : Specifies the separator character which separate the key from the value
// @param _rValue_U32 : Returns the unsigned integer option value
// @return 0 if the operation is successful
// @remarks Ex "TO=123;CONNECT=Hello word;Val=-1"
BOFSTD_EXPORT BOFERR Bof_GetUnsignedIntegerFromMultipleKeyValueString(const std::string &_rMultiKeyValueString_S, const std::string _rMultiKeyValueDelimiter_S, const std::string &_rKeyName_S,
                                                                      const char _KeyValueSeparator_c, uint32_t &_rValue_U32);

// @brief Returns the signed integer key value of a key name. Each Key Value pair is composed of a key name, a separator and the key value.
// @param _rMultiKeyValueString_S : Specifies the multi key value string to parse
// @param _rMultiKeyValueDelimiter_S : Specifies a list of character which can be used to separate the different key value string (could be ;\r\n for example)
// @param _rKeyName_S : Specifies the key name to find
// @param _KeyValueSeparator_c : Specifies the separator character which separate the key from the value
// @param _rValue_S32 : Returns the signed integer value
// @return 0 if the operation is successful
// @remarks Ex "TO=123;CONNECT=Hello word;Val=-1"
BOFSTD_EXPORT BOFERR
Bof_GetIntegerFromMultipleKeyValueString(const std::string &_rMultiKeyValueString_S, const std::string _rMultiKeyValueDelimiter_S, const std::string &_rKeyName_S, const char _KeyValueSeparator_c,
                                         int32_t &_rValue_S32);

// @brief Returns the string key value of a key name. Each Key Value pair is composed of a key name, a separator and the key value.
// @param _rMultiKeyValueString_S : Specifies the multi key value string to parse
// @param _rMultiKeyValueDelimiter_S : Specifies a list of character which can be used to separate the different key value string (could be ;\r\n for example)
// @param _rKeyName_S : Specifies the key name to find
// @param _KeyValueSeparator_c : Specifies the separator character which separate the key from the value
// @param _rValue_S : Returns the string key value
// @return 0 if the operation is successful
// @remarks Ex "TO=123;CONNECT=Hello word;Val=-1"
BOFSTD_EXPORT BOFERR
Bof_GetStringFromMultipleKeyValueString(const std::string &_rMultiKeyValueString_S, const std::string _rMultiKeyValueDelimiter_S, const std::string &_rKeyName_S, const char _KeyValueSeparator_c,
                                        std::string &_rValue_S);

// @brief Copies the first _NbMaxToCopy characters of _pSrc_c to _pDest_c. It is faster than the C strncpy standard function as destination is not padded with zeros until a total of _NbMaxToCopy characters have been written to it.
// @param _pDest_c : Specifies a pointer to the destination array where the content is to be copied.
// @param _pSrc_c : Specifies the C string to be copied.
// @param _NbMaxToCopy : Specifies the Maximum number of characters to be copied from source.
// @return 0 if the operation is successful
// @remarks In every case a null terminating char is inserted in the  string which can erased the last character if the stribng is not long enougth
BOFSTD_EXPORT char *Bof_FastZeroStrncpy(char *_pDest_c, const char *_pSrc_c, size_t _NbMaxToCopy);

BOFSTD_EXPORT bool Bof_IsDecimal(const std::string &_rInput_S, int32_t &_rVal_S32);
BOFSTD_EXPORT bool Bof_IsDecimal(const std::string &_rInput_S, uint32_t &_rVal_U32);
BOFSTD_EXPORT bool Bof_IsDecimal(const std::string &_rInput_S, int64_t &_rVal_S64);
BOFSTD_EXPORT bool Bof_IsDecimal(const std::string &_rInput_S, uint64_t &_rVal_U64);
BOFSTD_EXPORT bool Bof_IsHexadecimal(const std::string &_rInput_S, uint32_t &_rVal_U32);
BOFSTD_EXPORT bool Bof_IsHexadecimal(const std::string &_rInput_S, uint64_t &_rVal_U64);
BOFSTD_EXPORT bool Bof_IsDouble(const std::string &_rInput_S, double &_rVal_lf);

END_BOF_NAMESPACE()