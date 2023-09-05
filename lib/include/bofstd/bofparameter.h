/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the BofParameter class
 *
 * Name:        bofparameter.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#pragma once

#include <bofstd/bofflag.h>

#include <cstdint>
#include <functional>
#include <vector>

BEGIN_BOF_NAMESPACE()

#define BOF_PARAM_DEF_ENUM(varname, minval, maxval, BofEnumConverter, enumtype)                                                                                                                                                                                \
  BOF::BOFPARAMETER_ARG_TYPE::ENUM, static_cast<double>(minval), static_cast<double>(maxval), &varname, sizeof(varname), 0, 0, 0, 0, 0, 0, BOF_BIND_1_ARG_TO_METHOD(&BofEnumConverter, BOF::BofEnum<enumtype>::ToStringFromInt),                               \
      BOF_BIND_1_ARG_TO_METHOD(&BofEnumConverter, BOF::BofEnum<enumtype>::FromStringToInt)
#define BOF_PARAM_DEF_SHORT_OPT_VARIABLE(varname, val) BOF::BOFPARAMETER_ARG_TYPE::BOOL, static_cast<double>(val), static_cast<double>(0), &varname, sizeof(varname), 0, 0, 0, 0, 0, 0
#define BOF_PARAM_DEF_VARIABLE(varname, typevar, minval, maxval) BOF::BOFPARAMETER_ARG_TYPE::typevar, static_cast<double>(minval), static_cast<double>(maxval), &varname, sizeof(varname), 0, 0, 0, 0, 0, 0
#define BOF_PARAM_DEF_ARRAY(varname, typevar, minval, maxval)                                                                                                                                                                                                  \
  BOF::BOFPARAMETER_ARG_TYPE::typevar, static_cast<double>(minval), static_cast<double>(maxval), &varname, sizeof(varname[0]), BOF_NB_ELEM_IN_ARRAY(varname), BOF_NB_ELEM_IN_ARRAY(varname), 0, 0, 0, 0
#define BOF_PARAM_DEF_ARRAY_OF_STRUCT(structname, varname, varfield, typevar, minval, maxval)                                                                                                                                                                  \
  BOF::BOFPARAMETER_ARG_TYPE::typevar, static_cast<double>(minval), static_cast<double>(maxval), &varname[0].varfield, sizeof(structname), BOF_NB_ELEM_IN_ARRAY(varname), BOF_NB_ELEM_IN_ARRAY(varname), 0, 0, 0, 0
#define BOF_PARAM_DEF_VECTOR(varname, typevar, minval, maxval)  BOF::BOFPARAMETER_ARG_TYPE::typevar, static_cast<double>(minval), static_cast<double>(maxval), &varname, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0
// p=mRoot_O["MmgwSetting"]["Board"][1]["InHr"][0]["AudioIpAddress"][0].asCString();
#define BOF_PARAM_DEF_MULTI_ARRAY(varname, typevar, minval, maxval, nbmaxmultiarrayentry, typeelemearray1, typeelemearray2, unused)                                                                                                                            \
  BOF::BOFPARAMETER_ARG_TYPE::typevar, static_cast<double>(minval), static_cast<double>(maxval), &varname, sizeof(varname[0]), BOF_NB_ELEM_IN_ARRAY(varname), BOF_NB_ELEM_IN_ARRAY(varname), nbmaxmultiarrayentry, sizeof(typeelemearray1),                    \
      sizeof(typeelemearray2), unused
#define BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(structname, varname, varfield, typevar, minval, maxval, nbmaxmultiarrayentry, typeelemearray1, typeelemearray2, unused)                                                                                            \
  BOF::BOFPARAMETER_ARG_TYPE::typevar, static_cast<double>(minval), static_cast<double>(maxval), &varname[0].varfield, sizeof(structname), BOF_NB_ELEM_IN_ARRAY(varname), BOF_NB_ELEM_IN_ARRAY(varname), nbmaxmultiarrayentry, sizeof(typeelemearray1),        \
      sizeof(typeelemearray2), unused

enum class BOFPARAMETER_ARG_FLAG : uint32_t // Bitflag
{
  NONE = 0x00000000,                     // No special option
  CMDLINE_LONGOPT_NEED_ARG = 0x00000001, // for cmdlineparser: if the option requires an argument,
  XML_ATTRIBUTE = 0x00000002,            // for xml parser if the argument is an attribute
                                         // Replaced by IP_FORMAT_SCHEME below IP_FORMAT_PROTOCOL = 0x00000004,				// Add protocol specification to IPV4,/IPV46 output of S_ParameterToString
  // Replaced by IP_FORMAT_PORT below   IP_FORMAT_PORT = 0x00000008,						// Add port specification to IPV4,/IPV46 output of S_ParameterToString
  PATH_IS_DIR = 0x00000010,     // Path must be a directory
  PATH_IS_FILE = 0x00000020,    // Path must be a file
  PATH_MUST_EXIST = 0x00000040, // Path (file or dir) must exist
  COMA_IS_NOT_A_SEPARATOR = 0x00000080,
  URI_NEED_SCHEME = 0x00000100,
  URI_NEED_AUTHORITY = 0x00000200,
  URI_NEED_PATH = 0x00000400,
  URI_NEED_QUERY = 0x00000800,
  URI_NEED_FRAG = 0x00001000,

  IP_FORMAT_SCHEME = 0x00002000,
  IP_FORMAT_USER = 0x00004000,
  IP_FORMAT_PASSWORD = 0x00008000,
  IP_FORMAT_PORT = 0x000010000,

  STR_FORMAT_ISREGEXP = 0x000020000,

  READ_ONLY = 0x80000000, // This is a read only parameter
  //	WRITE_ONCE=0x00000020,								//Can only be written to once, after the first write op in turns into READ_ONLY
};
template <> struct IsItAnEnumBitFLag<BOFPARAMETER_ARG_FLAG> : std::true_type
{
};

enum class BOFPARAMETER_ARG_TYPE : uint32_t
{
  BOOL = 0,
  CHARSTRING,
  STDSTRING,
  DATE,
  TIME,
  DATETIME,
  UINT8,
  UINT16,
  UINT32,
  UINT64,
  INT8,
  INT16,
  INT32,
  INT64,
  FLOAT,
  DOUBLE,
  ENUM,
  IPV4,
  IPV6,
  URI,
  GUID,
  PATH,
  TC,
  VIDEOSTANDARD,
  AUDIOSTANDARD,
  SIZE2D,
};
using BOF_PARAMETER_ENUM_TO_STRING = std::function<const std::string &(int _EnumValue_i)>;
// typedef const std::string &(* BOF_PARAMETER_ENUM_TO_STRING)(int _EnumValue_i);
using BOF_PARAMETER_STRING_TO_ENUM = std::function<int(const std::string &_rEnumValue_S)>;

struct BOFPARAMETER
{
  void *pUser;
  std::string Name_S;
  std::string Description_S;
  std::string Format_S; // For date/time format or specify STR_FORMAT_ISREGEXP for str type
  std::string Path_S;   // For xml/json parser
  BOFPARAMETER_ARG_FLAG ArgFlag_E;
  BOFPARAMETER_ARG_TYPE ArgType_E;
  double Min_lf; // for binary min/max val for char min/max len if min=max=0->not used
  double Max_lf;
  void *pValue;
  uint32_t ArrayElementSize_U32;
  uint32_t ArrayCapacity_U32;
  uint32_t ActiveArrayEntry_U32;
  uint32_t pExtraParam_U32[4]; // Used for example with Json ToByte method and new enhanced json parser with multi array def"MmgwSetting.Board.%.InHr.%" "VideoStandard" or "MmgwSetting.Board%.InHr.%.AudioIpAddress.%" "
  BOF_PARAMETER_ENUM_TO_STRING EnumToString;
  BOF_PARAMETER_STRING_TO_ENUM StringToEnum;

  BOFPARAMETER()
  {
    Reset();
  }

  BOFPARAMETER(void *_pUser, const std::string &_rName_S, const std::string &_rDescription_S, const std::string &_rFormat_S, const std::string &_rPath_S, BOFPARAMETER_ARG_FLAG _ArgFlag_E, BOFPARAMETER_ARG_TYPE _ArgType_E, double _Min_lf, double _Max_lf,
               void *_pValue, uint32_t _ArrayElementSize_U32, uint32_t _ArrayCapacity_U32, uint32_t _ActiveArrayEntry_U32, uint32_t _ExtraParam1_U32, uint32_t _ExtraParam2_U32, uint32_t _ExtraParam3_U32, uint32_t _ExtraParam4_U32,
               BOF_PARAMETER_ENUM_TO_STRING _EnumToString = nullptr, BOF_PARAMETER_STRING_TO_ENUM _StringToEnum = nullptr)
  {
    pUser = _pUser;
    Name_S = _rName_S;
    Description_S = _rDescription_S;
    Format_S = _rFormat_S;
    Path_S = _rPath_S;
    ArgFlag_E = _ArgFlag_E;
    ArgType_E = _ArgType_E;
    Min_lf = _Min_lf;
    Max_lf = _Max_lf;
    pValue = _pValue;
    ArrayElementSize_U32 = _ArrayElementSize_U32;
    ArrayCapacity_U32 = _ArrayCapacity_U32;
    ActiveArrayEntry_U32 = _ActiveArrayEntry_U32;
    pExtraParam_U32[0] = _ExtraParam1_U32;
    pExtraParam_U32[1] = _ExtraParam2_U32;
    pExtraParam_U32[2] = _ExtraParam3_U32;
    pExtraParam_U32[3] = _ExtraParam4_U32;
    EnumToString = _EnumToString;
    StringToEnum = _StringToEnum;
  }

  void Reset()
  {
    uint32_t i_U32;
    pUser = nullptr;
    Name_S = "";
    Description_S = "";
    Format_S = "";
    Path_S = "";
    ArgFlag_E = BOFPARAMETER_ARG_FLAG::NONE;
    ArgType_E = BOFPARAMETER_ARG_TYPE::CHARSTRING;
    Min_lf = 0;
    Max_lf = 0;
    pValue = nullptr;
    ArrayElementSize_U32 = 0;
    ArrayCapacity_U32 = 0;
    ActiveArrayEntry_U32 = 0;
    for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(pExtraParam_U32); i_U32++)
    {
      pExtraParam_U32[i_U32] = 0;
    }
    EnumToString = nullptr;
    StringToEnum = nullptr;
  }
};

typedef std::function<BOFERR(uint32_t _Index_U32, const BOFPARAMETER &_rBofCommandlineOption_X, BOFERR _Err_E, const char *_pOptNewVal_c)> BOFPARAMETER_PARSE_CALLBACK;

class BOFSTD_EXPORT BofParameter
{
public:
  static BOFERR S_Parse(uint32_t _Index_U32, const BOFPARAMETER _rBofParameter_X, const char *_pOptVal_c, const BOFPARAMETER_PARSE_CALLBACK _pParseCallback_O);

  static void S_BuildHelpString(const std::vector<BOFPARAMETER> &_rListOfParam_X, uint32_t _IndexInArray_U32, const std::string &_rTitle_S, std::string &_rHelpString_S);

  static const char *S_ParameterToString(uint32_t _Index_U32, const BOFPARAMETER _rBofParameter_X, bool _AsHexa_B, uint32_t _MaxSize_U32, char *_pToString_c, uint32_t &_rVectorCapacity_U32);

  static const char *S_ArgTypeToString(BOFPARAMETER_ARG_TYPE _ArgType_E);

  static const char *S_FlagArgToString(BOFPARAMETER_ARG_FLAG _ReqArg_E);

  template <typename T> static void S_StringToNative(const std::string &_rVal_S, T &_rVal)
  {
    if ((_rVal_S[0] == '0') && ((_rVal_S[1] == 'x') || (_rVal_S[1] == 'X')))
    {
      size_t Sz = 0;
      _rVal = static_cast<T>(std::stoll(_rVal_S.substr(2), &Sz, 16));
    }
    else
    {
      _rVal = static_cast<T>(std::stoll(_rVal_S));
    }
  }

  static void S_StringToNative(const std::string &_rVal_S, double &_rVal)
  {
    _rVal = std::stod(_rVal_S);
  }

  static void S_StringToNative(const std::string &_rVal_S, float &_rVal)
  {
    _rVal = std::stof(_rVal_S);
  }
};

END_BOF_NAMESPACE()