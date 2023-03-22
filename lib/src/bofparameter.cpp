/*
 * Copyright (Sts_i) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the BofParameter class
 *
 * Name:        BofParameter.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#include <bofstd/bofparameter.h>
#include <bofstd/bofvideostandard.h>
#include <bofstd/bofaudiostandard.h>
#include <bofstd/boftimecode.h>
#include <bofstd/bofguid.h>
#include <bofstd/bofuri.h>
#include <bofstd/bofsystem.h>

#include <regex>
#include <stdlib.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#if defined (_WIN32)
#else
#include <arpa/inet.h>
#endif

BEGIN_BOF_NAMESPACE()

template<typename T>
BOFERR CheckAndGetValueFromString(const char *_pOptVal_c, const BOFPARAMETER _rBofParameter_X, T &_rVal)
{
  BOFERR     Rts_E = BOF_ERR_EINVAL;
  bool    Hexa_B;
  char     pFormat_c[32];
  uint32_t Len_U32;
  int      Nb_i;

  if (_pOptVal_c)
  {
    Len_U32 = static_cast<uint32_t>(strlen(_pOptVal_c));
    Hexa_B = false;
    if ((Len_U32 > 2) && (_pOptVal_c[0] == '0'))
    {
      Hexa_B = ((_pOptVal_c[1] == 'x') || (_pOptVal_c[1] == 'X'));
    }
    Rts_E = BOF_ERR_NO_ERROR;
    switch (_rBofParameter_X.ArgType_E)
    {
      case BOFPARAMETER_ARG_TYPE::UINT8:
        if (Hexa_B)
        {
          strcpy(pFormat_c, "%" SCNx8);
        }
        else
        {
          strcpy(pFormat_c, "%" SCNu8);
        }
        break;

      case BOFPARAMETER_ARG_TYPE::UINT16:
        if (Hexa_B)
        {
          strcpy(pFormat_c, "%" SCNx16);
        }
        else
        {
          strcpy(pFormat_c, "%" SCNu16);
        }
        break;

      case BOFPARAMETER_ARG_TYPE::UINT32:
        if (Hexa_B)
        {
          strcpy(pFormat_c, "%X");
        }
        else
        {
          strcpy(pFormat_c, "%u");
        }
        break;

      case BOFPARAMETER_ARG_TYPE::UINT64:
        if (Hexa_B)
        {
          strcpy(pFormat_c, "%" PRIX64);
        }
        else
        {
          strcpy(pFormat_c, "%" PRIu64);
        }
        break;

      case BOFPARAMETER_ARG_TYPE::INT8:
        if (Hexa_B)
        {
          strcpy(pFormat_c, "%" SCNx8);
        }
        else
        {
          strcpy(pFormat_c, "%" SCNd8);
        }
        break;

      case BOFPARAMETER_ARG_TYPE::INT16:
        if (Hexa_B)
        {
          strcpy(pFormat_c, "%" SCNx16);
        }
        else
        {
          strcpy(pFormat_c, "%" SCNd16);
        }
        break;

      case BOFPARAMETER_ARG_TYPE::ENUM:
      case BOFPARAMETER_ARG_TYPE::INT32:
        if (Hexa_B)
        {
          strcpy(pFormat_c, "%X");
        }
        else
        {
          strcpy(pFormat_c, "%d");
        }
        break;

      case BOFPARAMETER_ARG_TYPE::INT64:
        if (Hexa_B)
        {
          strcpy(pFormat_c, "%" PRIX64);
        }
        else
        {
          strcpy(pFormat_c, "%" PRId64);
        }
        break;

      case BOFPARAMETER_ARG_TYPE::FLOAT:
        if (Hexa_B)
        {
          Rts_E = BOF_ERR_PARSER;
        }
        else
        {
          strcpy(pFormat_c, "%f");
        }
        break;

      case BOFPARAMETER_ARG_TYPE::DOUBLE:
        if (Hexa_B)
        {
          Rts_E = BOF_ERR_PARSER;
        }
        else
        {
          strcpy(pFormat_c, "%lf");
        }
        break;

      default:
        Rts_E = BOF_ERR_INTERNAL;
        break;
    }

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Rts_E = BOF_ERR_PARSER;

      if (_pOptVal_c[0] == 0) //Empty so we want the default value which is in ther min value
      {
        Nb_i = 1;
        _rVal = static_cast<T>(_rBofParameter_X.Min_lf);
      }
      else
      {
        if (Hexa_B)
        {
          Nb_i = static_cast<int>(sscanf(_pOptVal_c + 2, pFormat_c, &_rVal));
        }
        else
        {
          Nb_i = static_cast<int>(sscanf(_pOptVal_c, pFormat_c, &_rVal));
        }
      }
      if (Nb_i == 1)
      {
        Rts_E = BOF_ERR_NO_ERROR;
        if (_rBofParameter_X.Min_lf < _rBofParameter_X.Max_lf)   // O,O no test
        {
          if ((_rVal < static_cast<T>(_rBofParameter_X.Min_lf)) || (_rVal > static_cast<T>(_rBofParameter_X.Max_lf)))
          {
            Rts_E = BOF_ERR_OUT_OF_RANGE;
          }
        }
      }
    }
  }
  return Rts_E;
}



// -1 ok parse end
// -2 error
BOFERR BofParameter::S_Parse(uint32_t _Index_U32, const BOFPARAMETER _rBofParameter_X, const char *_pOptVal_c, const BOFPARAMETER_PARSE_CALLBACK _pParseCallback_O)
{
  BOFERR                  Rts_E = BOF_ERR_EINVAL;                                                  // -1
  //Static as it can takes time (on gcc 4.9 for example)
  //static const std::regex S_RegExInteger_O("[\\+\\-]?([0-9]+)|(0[xX]([0-9a-fA-F])+)");                    // (*, meaning "zero or more") or a plus sign (+, meaning "one or more")
  //static const std::regex S_RegExDecimal_O("[\\+\\-]?([0-9]+)[.]?([0-9]*)");                              // (*, meaning "zero or more") or a plus sign (+, meaning "one or more")
  static const std::regex S_RegExBoolTrue_O("true|True|TRUE|Y|y|1|on|On|ON");
  static const std::regex S_RegExBoolFalse_O("false|False|FALSE|N|n|0|off|O |OFF");
  //  static const std::regex S_RegExIpV4_O("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?).(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?).(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?).(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
  //  static const std::regex S_RegExIpV6_O("^((([0-9A-Fa-f]{1,4}:){7}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){6}:[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){5}:([0-9A-Fa-f]{1,4}:)?[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){4}:([0-9A-Fa-f]{1,4}:){0,2}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){3}:([0-9A-Fa-f]{1,4}:){0,3}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){2}:([0-9A-Fa-f]{1,4}:){0,4}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){6}((b((25[0-5])|(1d{2})|(2[0-4]d)|(d{1,2}))b).){3}(b((25[0-5])|(1d{2})|(2[0-4]d)|(d{1,2}))b))|(([0-9A-Fa-f]{1,4}:){0,5}:((b((25[0-5])|(1d{2})|(2[0-4]d)|(d{1,2}))b).){3}(b((25[0-5])|(1d{2})|(2[0-4]d)|(d{1,2}))b))|(::([0-9A-Fa-f]{1,4}:){0,5}((b((25[0-5])|(1d{2})|(2[0-4]d)|(d{1,2}))b).){3}(b((25[0-5])|(1d{2})|(2[0-4]d)|(d{1,2}))b))|([0-9A-Fa-f]{1,4}::([0-9A-Fa-f]{1,4}:){0,5}[0-9A-Fa-f]{1,4})|(::([0-9A-Fa-f]{1,4}:){0,6}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){1,7}:))$");

  const char *pTheOptVal_c, *pComaSep_c, *pNextComaSep_c;
  char              pAllTheOptVal_c[0x1000], pDateTimeFormat_c[0x100], pEnumVal_c[64];
  std::cmatch       MatchCharString_O;

  bool               Val_B = false;
  uint8_t            Val_U8 = 0;
  uint16_t           Val_U16 = 0;
  uint32_t           Val_U32 = 0;
  uint64_t           Val_U64 = 0;
  int8_t             Val_S8 = 0;
  int16_t            Val_S16 = 0;
  int32_t            Val_S32 = 0;
  int64_t            Val_S64 = 0;
  float              Val_f = 0;
  double             Val_lf = 0;
  char *p_c;
  int32_t            ScopeId_U32 = 0, FlowInfo_U32 = 0;
  BofVideoStandard	 VideoStandard;
  BofAudioStandard	 AudioStandard;
  BofTimecode			   TimeCode;
  int                Len_i, LenMin_i, LenMax_i;
  bool               InsertInStdVector_B = false, ShortOpt_B, LastArrayArg_B, ContinueIfError_B;
  void *pValue;
  //BOF_PROTOCOL_TYPE ProtocolType_E = BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN;
  BOF_SIZE					 Size_X;
  int Width_i, Height_i;
  BofGuid            Guid;
  BofPath            Path;
  BofUri             Uri;
  std::string        Path_S, Query_S;
  //struct in_addr     IpV4_X;
  //struct in6_addr    IpV6_X;
  //BOF_SOCKET_ADDRESS *pIpV6_X;
  //BOF_SOCKET_ADDRESS *pIpV4_X;
  //BOF_SOCKET_ADDRESS Ip_X;
  //BOF_SOCK_TYPE      SocketType_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
  //char pAllTheIpAddress_c[0x100], *pIpAddress_c;
  //int16_t            IpPort_U16 = 0;
  // std::get_time not present in gcc 4.9
  std::tm Tm_X;
  BOF_SOCKET_ADDRESS_COMPONENT IpAddress_X;

  ShortOpt_B = false;
  if (_pOptVal_c == nullptr)  //Short non required option
  {
    _pOptVal_c = "0"; //0 is good for bool and integral type bus is it good for string (side effect) ? //(_rBofParameter_X.ArgType_E==BOFPARAMETER_ARG_TYPE::BOOL)
    ShortOpt_B = true;
  }
  //pIpAddress_c = pAllTheIpAddress_c;
  Len_i = 0;
  LenMax_i = 0;
  if ((_pOptVal_c) && ((Len_i = static_cast<int>(strlen(_pOptVal_c))) < static_cast<int>(sizeof(pAllTheOptVal_c))))
  {
    pTheOptVal_c = _pOptVal_c;
    pComaSep_c = nullptr;
    LastArrayArg_B = false;

    if ((!ShortOpt_B) && (_rBofParameter_X.ArrayCapacity_U32))
    {
      //"--a32=0xABCDEF,1,2,0xDEADBEEF,44,55,,"->But if you have a text with , it will break
      if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::COMA_IS_NOT_A_SEPARATOR))
      {
        pComaSep_c = nullptr;
      }
      else
      {
        pComaSep_c = strchr(pTheOptVal_c, ',');
        if (pComaSep_c)
        {
          strcpy(pAllTheOptVal_c, _pOptVal_c);  //Copy the whole stuff

          Len_i = static_cast<int>(pComaSep_c - pTheOptVal_c);
          pComaSep_c = &pAllTheOptVal_c[Len_i];
          pTheOptVal_c = pAllTheOptVal_c;
          pAllTheOptVal_c[Len_i] = 0;
        }
      }
    }

    do
    {
      ContinueIfError_B = false;
      Rts_E = BOF_ERR_INIT;
      InsertInStdVector_B = false;

      pValue = _rBofParameter_X.pValue;
      if (pValue)
      {
        Rts_E = BOF_ERR_NO_ERROR;
        if (_rBofParameter_X.ArrayCapacity_U32)
        {
          // Array
          if (_Index_U32 < _rBofParameter_X.ArrayCapacity_U32) //&& (_rBofParameter_X.ArrayElementSize_U32))
          {
            if (_rBofParameter_X.ArrayCapacity_U32 == 0xFFFFFFFF)  //std::vector
            {
              InsertInStdVector_B = true;
            }
            else
            {
              if (_rBofParameter_X.ArrayElementSize_U32)
              {
                pValue = static_cast<uint8_t *> (pValue) + (_Index_U32 * _rBofParameter_X.ArrayElementSize_U32);
              }
              else
              {
                Rts_E = BOF_ERR_TOO_SMALL;
              }
            }
          }
          else
          {
            Rts_E = BOF_ERR_OUT_OF_RANGE;
          }
        }
      }
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Rts_E = BOF_ERR_FORMAT;
        switch (_rBofParameter_X.ArgType_E)
        {
          case BOFPARAMETER_ARG_TYPE::BOOL:
            if (regex_match(pTheOptVal_c, MatchCharString_O, S_RegExBoolTrue_O))
            {
              Val_B = true;
              Rts_E = BOF_ERR_NO_ERROR;
            }
            else
            {
              if (regex_match(pTheOptVal_c, MatchCharString_O, S_RegExBoolFalse_O))
              {
                Val_B = false;
                Rts_E = BOF_ERR_NO_ERROR;
              }
            }
            break;

          case BOFPARAMETER_ARG_TYPE::CHARSTRING:
          case BOFPARAMETER_ARG_TYPE::STDSTRING:
            Rts_E = BOF_ERR_NO_ERROR;
            LenMin_i = static_cast<int> (_rBofParameter_X.Min_lf);
            LenMax_i = static_cast<int> (_rBofParameter_X.Max_lf);
            if (((LenMin_i) || (LenMax_i)) && (LenMin_i <= LenMax_i))
            {
              if ((Len_i < LenMin_i) || (Len_i > LenMax_i))
              {
                Rts_E = BOF_ERR_WRONG_SIZE;
              }
            }
            break;

          case BOFPARAMETER_ARG_TYPE::DATE:
          case BOFPARAMETER_ARG_TYPE::TIME:
          case BOFPARAMETER_ARG_TYPE::DATETIME:
            Rts_E = BOF_ERR_NO_ERROR;
            break;

          case BOFPARAMETER_ARG_TYPE::UINT8:
            Rts_E = ShortOpt_B ? BOF_ERR_NO_ERROR : CheckAndGetValueFromString<uint8_t>(pTheOptVal_c, _rBofParameter_X, Val_U8);
            break;

          case BOFPARAMETER_ARG_TYPE::UINT16:
            Rts_E = ShortOpt_B ? BOF_ERR_NO_ERROR : CheckAndGetValueFromString<uint16_t>(pTheOptVal_c, _rBofParameter_X, Val_U16);
            break;

          case BOFPARAMETER_ARG_TYPE::UINT32:
            Rts_E = ShortOpt_B ? BOF_ERR_NO_ERROR : CheckAndGetValueFromString<uint32_t>(pTheOptVal_c, _rBofParameter_X, Val_U32);
            break;

          case BOFPARAMETER_ARG_TYPE::UINT64:
            Rts_E = ShortOpt_B ? BOF_ERR_NO_ERROR : CheckAndGetValueFromString<uint64_t>(pTheOptVal_c, _rBofParameter_X, Val_U64);
            break;

          case BOFPARAMETER_ARG_TYPE::INT8:
            Rts_E = ShortOpt_B ? BOF_ERR_NO_ERROR : CheckAndGetValueFromString<int8_t>(pTheOptVal_c, _rBofParameter_X, Val_S8);
            break;

          case BOFPARAMETER_ARG_TYPE::INT16:
            Rts_E = ShortOpt_B ? BOF_ERR_NO_ERROR : CheckAndGetValueFromString<int16_t>(pTheOptVal_c, _rBofParameter_X, Val_S16);
            break;

          case BOFPARAMETER_ARG_TYPE::ENUM:
            if (_rBofParameter_X.StringToEnum)
            {
              int Enum_i = _rBofParameter_X.StringToEnum(pTheOptVal_c);
              //Not under gcc              itoa(Enum_i, pEnumVal_c, 10);
              sprintf(pEnumVal_c, "%d", Enum_i);
              pTheOptVal_c = pEnumVal_c;
            }
          case BOFPARAMETER_ARG_TYPE::INT32:
            Rts_E = ShortOpt_B ? BOF_ERR_NO_ERROR : CheckAndGetValueFromString<int32_t>(pTheOptVal_c, _rBofParameter_X, Val_S32);
            break;

          case BOFPARAMETER_ARG_TYPE::INT64:
            Rts_E = ShortOpt_B ? BOF_ERR_NO_ERROR : CheckAndGetValueFromString<int64_t>(pTheOptVal_c, _rBofParameter_X, Val_S64);
            break;

          case BOFPARAMETER_ARG_TYPE::FLOAT:
            Rts_E = ShortOpt_B ? BOF_ERR_NO_ERROR : CheckAndGetValueFromString<float>(pTheOptVal_c, _rBofParameter_X, Val_f);
            break;

          case BOFPARAMETER_ARG_TYPE::DOUBLE:
            Rts_E = ShortOpt_B ? BOF_ERR_NO_ERROR : CheckAndGetValueFromString<double>(pTheOptVal_c, _rBofParameter_X, Val_lf);
            break;

          case BOFPARAMETER_ARG_TYPE::IPV4:
          case BOFPARAMETER_ARG_TYPE::IPV6:
            //IpPort_U16 = 0;
            Rts_E = Bof_SplitIpAddress(pTheOptVal_c, IpAddress_X);
            if (Rts_E == BOF_ERR_NO_ERROR)
            {
              if (IpAddress_X.Ip_X.IpV6_B == (_rBofParameter_X.ArgType_E == BOFPARAMETER_ARG_TYPE::IPV6) ? true : false)
              {
                if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::IP_FORMAT_SCHEME))
                {
                  if (IpAddress_X.Protocol_S.empty())
                  {
                    Rts_E = BOF_ERR_PROTOCOL;
                  }
                }
                if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::IP_FORMAT_USER))
                {
                  if (IpAddress_X.User_S.empty())
                  {
                    Rts_E = BOF_ERR_EUSERS;
                  }
                }
                if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::IP_FORMAT_PASSWORD))
                {
                  if (IpAddress_X.Password_S.empty())
                  {
                    Rts_E = BOF_ERR_EACCES;
                  }
                }
                if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::IP_FORMAT_PORT))
                {
                  if (IpAddress_X.Port_U16 == 0)
                  {
                    Rts_E = BOF_ERR_EINVAL;
                  }
                }
              }
              else
              {
                Rts_E = BOF_ERR_ADDRESS;
              }
            }
            break;

          case BOFPARAMETER_ARG_TYPE::GUID:
            Guid = BofGuid(pTheOptVal_c);
            if (Guid.IsValid())
            {
              Rts_E = BOF_ERR_NO_ERROR;
            }
            break;

          case BOFPARAMETER_ARG_TYPE::PATH:
            Path = BofPath(pTheOptVal_c);
            if (Path.IsValid())
            {
              Rts_E = BOF_ERR_NO_ERROR;
              if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::PATH_IS_DIR))
              {
                if (!Path.IsDirectory())
                {
                  Rts_E = BOF_ERR_ENOTDIR;
                }
              }
              if (Rts_E == BOF_ERR_NO_ERROR)
              {
                if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::PATH_IS_FILE))
                {
                  if (!Path.IsFile())
                  {
                    Rts_E = BOF_ERR_ENOTNAM;
                  }
                }
              }
              if (Rts_E == BOF_ERR_NO_ERROR)
              {
                if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::PATH_MUST_EXIST))
                {
                  if (!Path.IsExist())
                  {
                    Rts_E = BOF_ERR_DONT_EXIST;
                  }
                }
              }
            }
            break;

          case BOFPARAMETER_ARG_TYPE::URI:
            Uri = BofUri(pTheOptVal_c);
            if (Uri.IsValid())
            {
              Rts_E = BOF_ERR_NO_ERROR;

              if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::URI_NEED_SCHEME))
              {
                if (Uri.Scheme().empty())
                {
                  Rts_E = BOF_ERR_PROTOCOL;
                }
              }
              if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::URI_NEED_AUTHORITY))
              {
                if (Uri.Authority().empty())
                {
                  Rts_E = BOF_ERR_EUSERS;
                }
              }
              if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::URI_NEED_PATH))
              {
                Uri.Path(Path_S);
                if (Path_S.empty())
                {
                  Rts_E = BOF_ERR_EBADFD;
                }
              }
              if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::URI_NEED_QUERY))
              {
                Uri.QueryParamCollection(Query_S);
                if (Query_S.empty())
                {
                  Rts_E = BOF_ERR_EILSEQ;
                }
              }
              if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::URI_NEED_FRAG))
              {
                if (Uri.Fragment().empty())
                {
                  Rts_E = BOF_ERR_EDESTADDRREQ;
                }
              }

            }
            break;

          case BOFPARAMETER_ARG_TYPE::TC:
            TimeCode = BofTimecode(pTheOptVal_c);
            if (TimeCode.IsTimecodeValid())
            {
              Rts_E = BOF_ERR_NO_ERROR;
            }
            break;

          case BOFPARAMETER_ARG_TYPE::VIDEOSTANDARD:
            VideoStandard = BofVideoStandard(pTheOptVal_c);
            if (VideoStandard.IsValid())
            {
              Rts_E = BOF_ERR_NO_ERROR;
            }
            break;

          case BOFPARAMETER_ARG_TYPE::AUDIOSTANDARD:
            AudioStandard = BofAudioStandard(pTheOptVal_c);
            if (AudioStandard.IsValid())
            {
              Rts_E = BOF_ERR_NO_ERROR;
            }
            break;

          case BOFPARAMETER_ARG_TYPE::SIZE2D:
            if (sscanf(pTheOptVal_c, "%dx%d", &Width_i, &Height_i) == 2)
            {
              Size_X.Width_U32 = static_cast<uint32_t>(Width_i);
              Size_X.Height_U32 = static_cast<uint32_t>(Height_i);
              Rts_E = BOF_ERR_NO_ERROR;
            }
            break;

          default:
            Rts_E = BOF_ERR_INTERNAL;
            break;
        } //switch (_rBofParameter_X.ArgType_E)

        if (Rts_E != BOF_ERR_NO_ERROR)
        {
          if (_pParseCallback_O != nullptr)
          {
            // Call parse callback->can invalidate
            Rts_E = _pParseCallback_O(_Index_U32, _rBofParameter_X, Rts_E, _pOptVal_c);
          }
        }
        if (Rts_E == BOF_ERR_NO_ERROR) //&& (CheckOk_B))//Callback has confirmed and we have no error -> store it
        {
          if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::READ_ONLY))
          {
            Rts_E = BOF_ERR_WRITE;
          }
          else  //if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::READ_ONLY))
          {
            if (ShortOpt_B)  //Short non required option
            {
              Val_B = _rBofParameter_X.Min_lf ? true : false;
              Val_U8 = static_cast<uint8_t> (_rBofParameter_X.Min_lf);
              Val_U16 = static_cast<uint16_t> (_rBofParameter_X.Min_lf);
              Val_U32 = static_cast<uint32_t> (_rBofParameter_X.Min_lf);
              Val_U64 = static_cast<uint64_t> (_rBofParameter_X.Min_lf);
              Val_S8 = static_cast<int8_t> (_rBofParameter_X.Min_lf);
              Val_S16 = static_cast<int16_t> (_rBofParameter_X.Min_lf);
              Val_S32 = static_cast<int32_t> (_rBofParameter_X.Min_lf);
              Val_S64 = static_cast<int64_t> (_rBofParameter_X.Min_lf);
              Val_f = static_cast<float> (_rBofParameter_X.Min_lf);
              Val_lf = _rBofParameter_X.Min_lf;
            }
            switch (_rBofParameter_X.ArgType_E)
            {
              case BOFPARAMETER_ARG_TYPE::BOOL:
                if (InsertInStdVector_B)
                {
                  std::vector<bool> *pVectorBool;
                  pVectorBool = reinterpret_cast<std::vector<bool> *>(pValue);
                  pVectorBool->push_back(Val_B);
                }
                else
                {
                  *static_cast<bool *> (pValue) = Val_B;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::CHARSTRING:
              case BOFPARAMETER_ARG_TYPE::STDSTRING:
                if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::STR_FORMAT_ISREGEXP))
                {
                  const std::regex Pattern(_rBofParameter_X.Format_S.c_str());
                  if (!regex_match(pTheOptVal_c, Pattern))
                  {
                    ContinueIfError_B = true;
                    Rts_E = BOF_ERR_FORMAT;
                  }
                }
                if (Rts_E == BOF_ERR_NO_ERROR)
                {
                  if (((LenMin_i) || (LenMax_i)) && (LenMin_i <= LenMax_i))
                  {
                    if ((Len_i < LenMin_i) || (Len_i > LenMax_i))
                    {
                      Len_i = LenMax_i;                    // No  -1 as max is the maximum number of char excluding the terminating 0
                    }
                  }

                  if (_rBofParameter_X.ArgType_E == BOFPARAMETER_ARG_TYPE::CHARSTRING)
                  {
                    if (InsertInStdVector_B)
                    {
                      Rts_E = BOF_ERR_NOT_AVAILABLE;
                    }
                    else
                    {
                      strncpy(static_cast<char*> (pValue), pTheOptVal_c, Len_i);
                      static_cast<char*> (pValue)[Len_i] = 0;
                    }
                  }
                  else
                  {
                    if (InsertInStdVector_B)
                    {
                      std::vector<std::string>* pVectorString;
                      pVectorString = reinterpret_cast<std::vector<std::string> *>(pValue);
                      pVectorString->push_back(std::string(pTheOptVal_c, Len_i));
                    }
                    else
                    {
                      *(static_cast<std::string*> (pValue)) = std::string(pTheOptVal_c, Len_i);
                    }
                  }
                }
                break;

              case BOFPARAMETER_ARG_TYPE::DATE:
                if (_rBofParameter_X.Format_S == "")
                {
                  strcpy(pDateTimeFormat_c, "%Y-%m-%d");
                }
                else
                {
                  strcpy(pDateTimeFormat_c, _rBofParameter_X.Format_S.c_str());
                }
                goto DateTimeParse;

              case BOFPARAMETER_ARG_TYPE::TIME:
                if (_rBofParameter_X.Format_S == "")
                {
                  strcpy(pDateTimeFormat_c, "%H:%M:%S");
                }
                else
                {
                  strcpy(pDateTimeFormat_c, _rBofParameter_X.Format_S.c_str());
                }
                goto DateTimeParse;

              case BOFPARAMETER_ARG_TYPE::DATETIME:
                if (_rBofParameter_X.Format_S == "")
                {
                  strcpy(pDateTimeFormat_c, "%Y-%m-%d %H:%M:%S"); //%q is used by BofDateTime to display MicroSecond_U32
                }
                else
                {
                  strcpy(pDateTimeFormat_c, _rBofParameter_X.Format_S.c_str());
                }
DateTimeParse:
                memset(&Tm_X, 0, sizeof(Tm_X));
                //To be shure to insert a 'correct' value in vector if for example we only set the time value (all other value can be 0
                Tm_X.tm_mday = 1;

//See BofDateTime::FromString
                char *pPercentQ_c, *pFromString_c;
                uint32_t uS_U32;
                bool AddMicroSec_B;
                int OffsetOption_i, LenAfterFormat_i;

                AddMicroSec_B = false;
                pPercentQ_c = strstr(pDateTimeFormat_c, "%q");
                if (pPercentQ_c)    
                {
                  OffsetOption_i = pPercentQ_c - pDateTimeFormat_c;
                  LenAfterFormat_i = strlen(pDateTimeFormat_c) - OffsetOption_i - 2;
                  //Manage only one instance of %q and this one whould be the last one of the format string
                  AddMicroSec_B = true;
                  //Need to remove %q because il lead to an assertion during strftime call as it is not supported
                  memcpy(&pDateTimeFormat_c[OffsetOption_i], &pDateTimeFormat_c[OffsetOption_i+2], LenAfterFormat_i + 1); //+1 for null terminator
                }

                pFromString_c = strptime(_pOptVal_c, pDateTimeFormat_c, &Tm_X);
                if (pFromString_c == nullptr)
                {
                  Rts_E = BOF_ERR_PARSER; // -1;
                }
                else
                {
                  uS_U32 = 0;
                  if (AddMicroSec_B)
                  {
                    if (*pFromString_c)
                    {
                      uS_U32 = std::atoi(pFromString_c);
                    }
                  }
                  BofDateTime DateTime(Tm_X, uS_U32);
                  if (InsertInStdVector_B)
                  {
                    std::vector<BofDateTime> *pVectorDt;
                    pVectorDt = reinterpret_cast<std::vector<BofDateTime> *>(pValue);
                    pVectorDt->push_back(DateTime);
                  }
                  else
                  {
                    BofDateTime *pDateTime = static_cast<BofDateTime *> (pValue);
                    *pDateTime = DateTime;
                  }
                }
                break;

              case BOFPARAMETER_ARG_TYPE::UINT8:
                if (InsertInStdVector_B)
                {
                  std::vector<uint8_t> *pVectorUint8;
                  pVectorUint8 = reinterpret_cast<std::vector<uint8_t> *>(pValue);
                  pVectorUint8->push_back(Val_U8);
                }
                else
                {
                  *static_cast<uint8_t *> (pValue) = Val_U8;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::UINT16:
                if (InsertInStdVector_B)
                {
                  std::vector<uint16_t> *pVectorUint16;
                  pVectorUint16 = reinterpret_cast<std::vector<uint16_t> *>(pValue);
                  pVectorUint16->push_back(Val_U16);
                }
                else
                {
                  *static_cast<uint16_t *> (pValue) = Val_U16;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::UINT32:
                if (InsertInStdVector_B)
                {
                  std::vector<uint32_t> *pVectorUint32;
                  pVectorUint32 = reinterpret_cast<std::vector<uint32_t> *>(pValue);
                  pVectorUint32->push_back(Val_U32);
                }
                else
                {
                  *static_cast<uint32_t *> (pValue) = Val_U32;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::UINT64:
                if (InsertInStdVector_B)
                {
                  std::vector<uint64_t> *pVectorUint64;
                  pVectorUint64 = reinterpret_cast<std::vector<uint64_t> *>(pValue);
                  pVectorUint64->push_back(Val_U64);
                }
                else
                {
                  *static_cast<uint64_t *> (pValue) = Val_U64;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::INT8:
                if (InsertInStdVector_B)
                {
                  std::vector<int8_t> *pVectorInt8;
                  pVectorInt8 = reinterpret_cast<std::vector<int8_t> *>(pValue);
                  pVectorInt8->push_back(Val_S8);
                }
                else
                {
                  *static_cast<int8_t *> (pValue) = Val_S8;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::INT16:
                if (InsertInStdVector_B)
                {
                  std::vector<int16_t> *pVectorInt16;
                  pVectorInt16 = reinterpret_cast<std::vector<int16_t> *>(pValue);
                  pVectorInt16->push_back(Val_S16);
                }
                else
                {
                  *static_cast<int16_t *> (pValue) = Val_S16;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::INT32:
              case BOFPARAMETER_ARG_TYPE::ENUM:
                if (InsertInStdVector_B)
                {
                  std::vector<int32_t> *pVectorInt32;
                  pVectorInt32 = reinterpret_cast<std::vector<int32_t> *>(pValue);
                  pVectorInt32->push_back(Val_S32);
                }
                else
                {
                  *static_cast<int32_t *> (pValue) = Val_S32;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::INT64:
                if (InsertInStdVector_B)
                {
                  std::vector<int64_t> *pVectorInt64;
                  pVectorInt64 = reinterpret_cast<std::vector<int64_t> *>(pValue);
                  pVectorInt64->push_back(Val_S64);
                }
                else
                {
                  *static_cast<int64_t *> (pValue) = Val_S64;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::FLOAT:
                if (InsertInStdVector_B)
                {
                  std::vector<float> *pVectorFloat;
                  pVectorFloat = reinterpret_cast<std::vector<float> *>(pValue);
                  pVectorFloat->push_back(Val_f);
                }
                else
                {
                  *static_cast<float *> (pValue) = Val_f;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::DOUBLE:
                if (InsertInStdVector_B)
                {
                  std::vector<double> *pVectorDouble;
                  pVectorDouble = reinterpret_cast<std::vector<double> *>(pValue);
                  pVectorDouble->push_back(Val_lf);
                }
                else
                {
                  *static_cast<double *> (pValue) = Val_lf;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::IPV4:
              case BOFPARAMETER_ARG_TYPE::IPV6:
                if (InsertInStdVector_B)
                {
                  std::vector<BOF_SOCKET_ADDRESS_COMPONENT> *pVectorIp;
                  pVectorIp = reinterpret_cast<std::vector<BOF_SOCKET_ADDRESS_COMPONENT> *>(pValue);
                  pVectorIp->push_back(IpAddress_X);
                }
                else
                {
//                  BOF_SOCKET_ADDRESS_COMPONENT *p = static_cast<BOF_SOCKET_ADDRESS_COMPONENT *>(pValue);
//                  *p = IpAddress_X;
//                  *p = IpAddress_X;
                  *static_cast<BOF_SOCKET_ADDRESS_COMPONENT *>(pValue) = IpAddress_X;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::GUID:
                if (InsertInStdVector_B)
                {
                  std::vector<BofGuid> *pVectorGuid;
                  pVectorGuid = reinterpret_cast<std::vector<BofGuid> *>(pValue);
                  pVectorGuid->push_back(Guid);
                }
                else
                {
                  *static_cast<BofGuid *> (pValue) = Guid;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::PATH:
                if (InsertInStdVector_B)
                {
                  std::vector<BofPath> *pVectorPath;
                  pVectorPath = reinterpret_cast<std::vector<BofPath> *>(pValue);
                  pVectorPath->push_back(Path);
                }
                else
                {
                  *static_cast<BofPath *> (pValue) = Path;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::URI:
                if (InsertInStdVector_B)
                {
                  std::vector<BofUri> *pVectorUri;
                  pVectorUri = reinterpret_cast<std::vector<BofUri> *>(pValue);
                  pVectorUri->push_back(Uri);
                }
                else
                {
                  *static_cast<BofUri *> (pValue) = Uri;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::TC:
                if (InsertInStdVector_B)
                {
                  std::vector<BofTimecode> *pVectorTimeCode;
                  pVectorTimeCode = reinterpret_cast<std::vector<BofTimecode> *>(pValue);
                  pVectorTimeCode->push_back(TimeCode);
                }
                else
                {
                  *static_cast<BofTimecode *> (pValue) = TimeCode;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::VIDEOSTANDARD:
                if (InsertInStdVector_B)
                {
                  std::vector<BofVideoStandard> *pVectorVideoStandard;
                  pVectorVideoStandard = reinterpret_cast<std::vector<BofVideoStandard> *>(pValue);
                  pVectorVideoStandard->push_back(VideoStandard);
                }
                else
                {
                  *static_cast<BofVideoStandard *> (pValue) = VideoStandard;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::AUDIOSTANDARD:
                if (InsertInStdVector_B)
                {
                  std::vector<BofAudioStandard> *pVectorAudioStandard;
                  pVectorAudioStandard = reinterpret_cast<std::vector<BofAudioStandard> *>(pValue);
                  pVectorAudioStandard->push_back(AudioStandard);
                }
                else
                {
                  *static_cast<BofAudioStandard *> (pValue) = AudioStandard;
                }
                break;

              case BOFPARAMETER_ARG_TYPE::SIZE2D:
                if (InsertInStdVector_B)
                {
                  std::vector<BOF_SIZE> *pVectorSize;
                  pVectorSize = reinterpret_cast<std::vector<BOF_SIZE> *>(pValue);
                  pVectorSize->push_back(Size_X);
                }
                else
                {
                  *static_cast<BOF_SIZE *> (pValue) = Size_X;
                }
                break;
              default:
                Rts_E = BOF_ERR_INTERNAL;
                break;
            } // switch (_rBofParameter_X.ArgType_E)
          } //else if (Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::READ_ONLY))
        } //if ((Rts_E == BOF_ERR_NO_ERROR) && (CheckOk_B))/

        if (pComaSep_c)
        {
          if (LastArrayArg_B)
          {
            pComaSep_c = nullptr;
          }
          else
          {
            _Index_U32++;
            pNextComaSep_c = strchr(pComaSep_c + 1, ',');
            if (pNextComaSep_c)
            {
              Len_i = static_cast<int>(pNextComaSep_c - pComaSep_c - 1);
              pTheOptVal_c = pComaSep_c + 1;
              pComaSep_c = pNextComaSep_c;
              pAllTheOptVal_c[pNextComaSep_c - pAllTheOptVal_c] = 0;
            }
            else
            {
              pComaSep_c = (pComaSep_c[1] == 0) ? nullptr : pComaSep_c + 1;
              if (pComaSep_c)
              {
                pTheOptVal_c = pComaSep_c;
                LastArrayArg_B = true;
                Len_i = static_cast<int>(strlen(pTheOptVal_c));
              }
            }
            if (ContinueIfError_B)
            {
              Rts_E = BOF_ERR_NO_ERROR; //for STR_FORMAT_ISREGEXP is list rocessing
            }
          }
        }
      } //if (Rts_E == BOF_ERR_NO_ERROR)
    } while ((pComaSep_c) && (Rts_E == BOF_ERR_NO_ERROR));
  } //if ((_pOptVal_c) && ((Len_i = static_cast<int>(strlen(_pOptVal_c))) < static_cast<int>(sizeof(pTheOptVal_c))))

  return Rts_E;
}


const char *BofParameter::S_FlagArgToString(BOFPARAMETER_ARG_FLAG _ReqArg_E)
{
  switch (_ReqArg_E)
  {
    case BOFPARAMETER_ARG_FLAG::NONE:
    {
      return "NONE";
    }

    case BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG:
    {
      return "CMDLINE_LONGOPT_NEED_ARG";
    }

    case BOFPARAMETER_ARG_FLAG::XML_ATTRIBUTE:
    {
      return "XML_ATTRIBUTE";
    }
    case BOFPARAMETER_ARG_FLAG::IP_FORMAT_PORT:
    {
      return "IP_FORMAT_PORT";
    }
    case BOFPARAMETER_ARG_FLAG::READ_ONLY:
    {
      return "RO";
    }
    default:
      return "???";
  }

}


const char *BofParameter::S_ArgTypeToString(BOFPARAMETER_ARG_TYPE _ArgType_E)
{
  switch (_ArgType_E)
  {
    case BOFPARAMETER_ARG_TYPE::BOOL:
    {
      return "BOOL";
    }

    case BOFPARAMETER_ARG_TYPE::CHARSTRING:
    {
      return "CHARSTRING";
    }

    case BOFPARAMETER_ARG_TYPE::STDSTRING:
    {
      return "STDSTRING";
    }

    case BOFPARAMETER_ARG_TYPE::DATE:
    {
      return "DATE";
    }

    case BOFPARAMETER_ARG_TYPE::TIME:
    {
      return "TIME";
    }

    case BOFPARAMETER_ARG_TYPE::DATETIME:
    {
      return "DATETIME";
    }

    case BOFPARAMETER_ARG_TYPE::UINT8:
    {
      return "UINT8";
    }

    case BOFPARAMETER_ARG_TYPE::INT8:
    {
      return "INT8";
    }

    case BOFPARAMETER_ARG_TYPE::UINT16:
    {
      return "UINT16";
    }

    case BOFPARAMETER_ARG_TYPE::INT16:
    {
      return "INT16";
    }

    case BOFPARAMETER_ARG_TYPE::UINT32:
    {
      return "UINT32";
    }

    case BOFPARAMETER_ARG_TYPE::INT32:
    {
      return "INT32";
    }
    case BOFPARAMETER_ARG_TYPE::ENUM:
    {
      return "ENUM";
    }
    case BOFPARAMETER_ARG_TYPE::UINT64:
    {
      return "UINT64";
    }

    case BOFPARAMETER_ARG_TYPE::INT64:
    {
      return "INT64";
    }

    case BOFPARAMETER_ARG_TYPE::FLOAT:
    {
      return "FLOAT";
    }

    case BOFPARAMETER_ARG_TYPE::DOUBLE:
    {
      return "DOUBLE";
    }

    case BOFPARAMETER_ARG_TYPE::IPV4:
    {
      return "IPV4";
    }

    case BOFPARAMETER_ARG_TYPE::IPV6:
    {
      return "IPV6";
    }

    case BOFPARAMETER_ARG_TYPE::GUID:
    {
      return "GUID";
    }

    case BOFPARAMETER_ARG_TYPE::PATH:
    {
      return "PATH";
    }

    case BOFPARAMETER_ARG_TYPE::URI:
    {
      return "URI";
    }

    case BOFPARAMETER_ARG_TYPE::TC:
    {
      return "TIMECODE";
    }
    break;

    case BOFPARAMETER_ARG_TYPE::VIDEOSTANDARD:
    {
      return "VIDEOSTANDARD";
    }

    case BOFPARAMETER_ARG_TYPE::AUDIOSTANDARD:
    {
      return "AUDIOSTANDARD";
    }

    case BOFPARAMETER_ARG_TYPE::SIZE2D:
    {
      return "SIZE2D";
    }

    break;
    default:
      return "???";

  }

}


const char *BofParameter::S_ParameterToString(uint32_t _Index_U32, const BOFPARAMETER _rBofParameter_X, bool _AsHexa_B, uint32_t _MaxSize_U32, char *_pToString_c, uint32_t &_rVectorCapacity_U32)
{
  void *pValue = nullptr;
  const char *pRts_c = nullptr;
  char       pFormat_c[4096];
  bool       GetFromStdVector_B = false;

  _rVectorCapacity_U32 = 0;
  if ((_rBofParameter_X.pValue) && (_pToString_c))
  {
    if (_rBofParameter_X.ArrayCapacity_U32)
    {
      // Array
      if (_Index_U32 < _rBofParameter_X.ArrayCapacity_U32)
      {
        if (_rBofParameter_X.ArrayCapacity_U32 == 0xFFFFFFFF)  //std::vector
        {
          GetFromStdVector_B = true;
          pValue = static_cast<uint8_t *> (_rBofParameter_X.pValue);
        }
        else
        {
          pValue = static_cast<uint8_t *> (_rBofParameter_X.pValue) + (_Index_U32 * _rBofParameter_X.ArrayElementSize_U32);
        }
      }
      _rVectorCapacity_U32 = _rBofParameter_X.ArrayCapacity_U32;
    }
    else
    {
      pValue = static_cast<uint8_t *> (_rBofParameter_X.pValue);
      _rVectorCapacity_U32 = 1;
    }
    if (pValue)
    {
      pRts_c = _pToString_c;
      strcpy(pFormat_c, _rBofParameter_X.Format_S.c_str());

      switch (_rBofParameter_X.ArgType_E)
      {
        case BOFPARAMETER_ARG_TYPE::BOOL:
        {
          if (GetFromStdVector_B)
          {
            std::vector<bool> *pVectorBool;
            pVectorBool = reinterpret_cast<std::vector<bool> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorBool->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              snprintf(_pToString_c, _MaxSize_U32, "%s", (pVectorBool->operator[](_Index_U32) != 0) ? "true" : "false");
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            snprintf(_pToString_c, _MaxSize_U32, "%s", (*static_cast<bool *> (pValue) != 0) ? "true" : "false");
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::CHARSTRING:
        {
          if (GetFromStdVector_B)
          {
            pRts_c = nullptr;  //not supported
          }
          else
          {
            if (pFormat_c[0])
            {
              snprintf(_pToString_c, _MaxSize_U32, pFormat_c, static_cast<char *> (pValue));
            }
            else
            {
              snprintf(_pToString_c, _MaxSize_U32, "%s", static_cast<char *> (pValue));
            }
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::STDSTRING:
        {
          const char *pValue_c = nullptr;
          if (GetFromStdVector_B)
          {
            std::vector<std::string> *pVectorString;
            pVectorString = reinterpret_cast<std::vector<std::string> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorString->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              //pValue_c=pVectorString->at(_Index_U32).c_str();
              pValue_c = pVectorString->operator[](_Index_U32).c_str();
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            pValue_c = reinterpret_cast<std::string *> (pValue)->c_str();
          }
          if (pRts_c)
          {
            if (pFormat_c[0])
            {
              snprintf(_pToString_c, _MaxSize_U32, pFormat_c, pValue_c);
            }
            else
            {
              strncpy(_pToString_c, pValue_c, _MaxSize_U32);
            }
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::DATE:
        {
          if (!pFormat_c[0])
          {
            strcpy(pFormat_c, "%Y-%m-%d");
          }
          goto DateTimeToString;
        }

        case BOFPARAMETER_ARG_TYPE::TIME:
        {
          if (!pFormat_c[0])
          {
            strcpy(pFormat_c, "%H:%M:%S");
          }
          goto DateTimeToString;
        }

        case BOFPARAMETER_ARG_TYPE::DATETIME:
        {
          if (!pFormat_c[0])
          {
            strcpy(pFormat_c, "%Y-%m-%d %H:%M:%S");
          }
DateTimeToString:
          {
            BofDateTime DateTime;
            if (GetFromStdVector_B)
            {
              std::vector<BofDateTime> *pVectorDt;
              pVectorDt = reinterpret_cast<std::vector<BofDateTime> *>(pValue);
              _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorDt->size());
              if (_Index_U32 < _rVectorCapacity_U32)
              {
                DateTime = pVectorDt->operator[](_Index_U32);
              }
              else
              {
                pRts_c = nullptr;
              }
            }
            else
            {
              DateTime = *(static_cast<BofDateTime *> (pValue));
            }
            if (pRts_c)
            {
              if (DateTime.IsValid())
              {
                snprintf(_pToString_c, _MaxSize_U32, "%s", DateTime.ToString(pFormat_c).c_str());
              }
              else
              {
                _pToString_c[0] = 0;
                //							pRts_c = nullptr;
              }
            }
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::UINT8:
        {
          uint8_t Value_U8 = 0;
          if (GetFromStdVector_B)
          {
            std::vector<uint8_t> *pVectorUint8;
            pVectorUint8 = reinterpret_cast<std::vector<uint8_t> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorUint8->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              Value_U8 = pVectorUint8->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            Value_U8 = *static_cast<uint8_t *> (pValue);
          }
          if (pRts_c)
          {
            if (pFormat_c[0])
            {
              snprintf(_pToString_c, _MaxSize_U32, pFormat_c, Value_U8);
            }
            else
            {
              if (_AsHexa_B)
              {
                snprintf(_pToString_c, _MaxSize_U32, "%X", Value_U8);
              }
              else
              {
                snprintf(_pToString_c, _MaxSize_U32, "%u", Value_U8);
              }
            }
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::INT8:
        {
          int8_t Value_S8 = 0;
          if (GetFromStdVector_B)
          {
            std::vector<int8_t> *pVectorInt8;
            pVectorInt8 = reinterpret_cast<std::vector<int8_t> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorInt8->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              Value_S8 = pVectorInt8->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            Value_S8 = *static_cast<int8_t *> (pValue);
          }
          if (pRts_c)
          {
            if (pFormat_c[0])
            {
              snprintf(_pToString_c, _MaxSize_U32, pFormat_c, Value_S8);
            }
            else
            {
              if (_AsHexa_B)
              {
                snprintf(_pToString_c, _MaxSize_U32, "%X", Value_S8);
              }
              else
              {
                snprintf(_pToString_c, _MaxSize_U32, "%d", Value_S8);
              }
            }
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::UINT16:
        {
          uint16_t Value_U16 = 0;
          if (GetFromStdVector_B)
          {
            std::vector<uint16_t> *pVectorUint16;
            pVectorUint16 = reinterpret_cast<std::vector<uint16_t> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorUint16->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              Value_U16 = pVectorUint16->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            Value_U16 = *static_cast<uint16_t *> (pValue);
          }
          if (pRts_c)
          {
            if (pFormat_c[0])
            {
              snprintf(_pToString_c, _MaxSize_U32, pFormat_c, Value_U16);
            }
            else
            {
              if (_AsHexa_B)
              {
                snprintf(_pToString_c, _MaxSize_U32, "%X", Value_U16);
              }
              else
              {
                snprintf(_pToString_c, _MaxSize_U32, "%u", Value_U16);
              }
            }
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::INT16:
        {
          int16_t Value_S16 = 0;
          if (GetFromStdVector_B)
          {
            std::vector<int16_t> *pVectorInt16;
            pVectorInt16 = reinterpret_cast<std::vector<int16_t> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorInt16->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              Value_S16 = pVectorInt16->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            Value_S16 = *static_cast<int16_t *> (pValue);
          }
          if (pRts_c)
          {
            if (pFormat_c[0])
            {
              snprintf(_pToString_c, _MaxSize_U32, pFormat_c, Value_S16);
            }
            else
            {
              if (_AsHexa_B)
              {
                snprintf(_pToString_c, _MaxSize_U32, "%X", Value_S16);
              }
              else
              {
                snprintf(_pToString_c, _MaxSize_U32, "%d", Value_S16);
              }
            }
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::UINT32:
        {
          uint32_t Value_U32 = 0;
          if (GetFromStdVector_B)
          {
            std::vector<uint32_t> *pVectorUint32;
            pVectorUint32 = reinterpret_cast<std::vector<uint32_t> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorUint32->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              Value_U32 = pVectorUint32->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            Value_U32 = *static_cast<uint32_t *> (pValue);
          }
          if (pRts_c)
          {
            if (pFormat_c[0])
            {
              snprintf(_pToString_c, _MaxSize_U32, pFormat_c, Value_U32);
            }
            else
            {
              if (_AsHexa_B)
              {
                snprintf(_pToString_c, _MaxSize_U32, "%X", Value_U32);
              }
              else
              {
                snprintf(_pToString_c, _MaxSize_U32, "%u", Value_U32);
              }
            }
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::INT32:
        case BOFPARAMETER_ARG_TYPE::ENUM:
        {
          int32_t Value_S32 = 0;
          if (GetFromStdVector_B)
          {
            std::vector<int32_t> *pVectorInt32;
            pVectorInt32 = reinterpret_cast<std::vector<int32_t> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorInt32->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              Value_S32 = pVectorInt32->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            Value_S32 = *static_cast<int32_t *> (pValue);
          }
          if (pRts_c)
          {
            if (_rBofParameter_X.EnumToString)
            {
              std::string EnumVal_S = _rBofParameter_X.EnumToString(Value_S32);
              snprintf(_pToString_c, _MaxSize_U32, "%s", EnumVal_S.c_str());
            }
            else
            {
              if (pFormat_c[0])
              {
                snprintf(_pToString_c, _MaxSize_U32, pFormat_c, Value_S32);
              }
              else
              {
                if (_AsHexa_B)
                {
                  snprintf(_pToString_c, _MaxSize_U32, "%X", Value_S32);
                }
                else
                {
                  snprintf(_pToString_c, _MaxSize_U32, "%d", Value_S32);
                }
              }
            }
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::UINT64:
        {
          uint64_t Value_U64 = 0;
          if (GetFromStdVector_B)
          {
            std::vector<int64_t> *pVectorUint64;
            pVectorUint64 = reinterpret_cast<std::vector<int64_t> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorUint64->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              Value_U64 = pVectorUint64->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            Value_U64 = *static_cast<int64_t *> (pValue);
          }
          if (pRts_c)
          {
            if (pFormat_c[0])
            {
              snprintf(_pToString_c, _MaxSize_U32, pFormat_c, Value_U64);
            }
            else
            {
              if (_AsHexa_B)
              {
                snprintf(_pToString_c, _MaxSize_U32, "%" PRIX64, Value_U64);
              }
              else
              {
                snprintf(_pToString_c, _MaxSize_U32, "%" PRIu64, Value_U64);
              }
            }
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::INT64:
        {
          int64_t Value_S64 = 0;
          if (GetFromStdVector_B)
          {
            std::vector<int64_t> *pVectorInt64;
            pVectorInt64 = reinterpret_cast<std::vector<int64_t> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorInt64->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              Value_S64 = pVectorInt64->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            Value_S64 = *static_cast<int64_t *> (pValue);
          }
          if (pRts_c)
          {
            if (pFormat_c[0])
            {
              snprintf(_pToString_c, _MaxSize_U32, pFormat_c, Value_S64);
            }
            else
            {
              if (_AsHexa_B)
              {
                snprintf(_pToString_c, _MaxSize_U32, "%" PRIX64, Value_S64);
              }
              else
              {
                snprintf(_pToString_c, _MaxSize_U32, "%" PRId64, Value_S64);
              }
            }
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::FLOAT:
        {
          float Value_f = 0;
          if (GetFromStdVector_B)
          {
            std::vector<float> *pVectorFloat;
            pVectorFloat = reinterpret_cast<std::vector<float> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorFloat->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              Value_f = pVectorFloat->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            Value_f = *static_cast<float *> (pValue);
          }
          if (pRts_c)
          {
            if (pFormat_c[0])
            {
              snprintf(_pToString_c, _MaxSize_U32, pFormat_c, Value_f);
            }
            else
            {
              snprintf(_pToString_c, _MaxSize_U32, "%f", Value_f);
            }
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::DOUBLE:
        {
          double Value_lf = 0;
          if (GetFromStdVector_B)
          {
            std::vector<double> *pVectorDouble;
            pVectorDouble = reinterpret_cast<std::vector<double> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorDouble->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              Value_lf = pVectorDouble->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            Value_lf = *static_cast<double *> (pValue);
          }
          if (pRts_c)
          {
            if (pFormat_c[0])
            {
              snprintf(_pToString_c, _MaxSize_U32, pFormat_c, Value_lf);
            }
            else
            {
              snprintf(_pToString_c, _MaxSize_U32, "%lf", Value_lf);
            }
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::IPV4:
        case BOFPARAMETER_ARG_TYPE::IPV6:
        {
          BOF_SOCKET_ADDRESS_COMPONENT IpAddressComponent_X;
          if (GetFromStdVector_B)
          {
            std::vector<BOF_SOCKET_ADDRESS_COMPONENT> *pVectorIp;
            pVectorIp = reinterpret_cast<std::vector<BOF_SOCKET_ADDRESS_COMPONENT> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorIp->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              IpAddressComponent_X = pVectorIp->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            IpAddressComponent_X = *(static_cast<BOF_SOCKET_ADDRESS_COMPONENT *> (pValue));
          }
          if (pRts_c)
          {
            //  snprintf(_pToString_c, _MaxSize_U32, "%s", Bof_SocketAddressToString(IpV4_X, false, Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::IP_FORMAT_PORT)).c_str());
            bool Scheme_B = Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::IP_FORMAT_SCHEME);
            bool User_B = Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::IP_FORMAT_USER);
            bool Password_B = Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::IP_FORMAT_PASSWORD);
            bool Port_B = Bof_IsAnyBitFlagSet(_rBofParameter_X.ArgFlag_E, BOFPARAMETER_ARG_FLAG::IP_FORMAT_PORT);
            snprintf(_pToString_c, _MaxSize_U32, "%s", IpAddressComponent_X.ToString(Scheme_B, User_B, Password_B, Port_B).c_str());
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::GUID:
        {
          BofGuid Guid;
          if (GetFromStdVector_B)
          {
            std::vector<BofGuid> *pVectorGuid;
            pVectorGuid = reinterpret_cast<std::vector<BofGuid> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorGuid->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              Guid = pVectorGuid->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            Guid = *static_cast<BofGuid *> (pValue);
          }
          if (pRts_c)
          {
            snprintf(_pToString_c, _MaxSize_U32, "%s", Guid.ToString((pFormat_c[0] != 0) ? false : true).c_str());
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::PATH:
        {
          BofPath Path;
          if (GetFromStdVector_B)
          {
            std::vector<BofPath> *pVectorPath;
            pVectorPath = reinterpret_cast<std::vector<BofPath> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorPath->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              Path = pVectorPath->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            Path = *static_cast<BofPath *> (pValue);
          }
          if (pRts_c)
          {
            snprintf(_pToString_c, _MaxSize_U32, "%s", Path.FullPathName((pFormat_c[0] != 0) ? true : false).c_str());
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::URI:
        {
          BofUri Uri;
          if (GetFromStdVector_B)
          {
            std::vector<BofUri> *pVectorUri;
            pVectorUri = reinterpret_cast<std::vector<BofUri> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorUri->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              Uri = pVectorUri->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            Uri = *static_cast<BofUri *> (pValue);
          }
          if (pRts_c)
          {
            snprintf(_pToString_c, _MaxSize_U32, "%s", Uri.ToString().c_str());
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::TC:
        {
          BofTimecode TimeCode;
          if (GetFromStdVector_B)
          {
            std::vector<BofTimecode> *pVectorTimeCode;
            pVectorTimeCode = reinterpret_cast<std::vector<BofTimecode> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorTimeCode->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              TimeCode = pVectorTimeCode->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            TimeCode = *static_cast<BofTimecode *> (pValue);
          }
          if (pRts_c)
          {
            snprintf(_pToString_c, _MaxSize_U32, "%s", TimeCode.ToString((pFormat_c[0] != 0) ? false : true).c_str());
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::VIDEOSTANDARD:
        {
          BofVideoStandard VideoStandard;
          if (GetFromStdVector_B)
          {
            std::vector<BofVideoStandard> *pVectorVideoStandard;
            pVectorVideoStandard = reinterpret_cast<std::vector<BofVideoStandard> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorVideoStandard->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              VideoStandard = pVectorVideoStandard->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            VideoStandard = *static_cast<BofVideoStandard *> (pValue);
          }
          if (pRts_c)
          {
            snprintf(_pToString_c, _MaxSize_U32, "%s", VideoStandard.ToString().c_str());
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::AUDIOSTANDARD:
        {
          BofAudioStandard AudioStandard;
          if (GetFromStdVector_B)
          {
            std::vector<BofAudioStandard> *pVectorAudioStandard;
            pVectorAudioStandard = reinterpret_cast<std::vector<BofAudioStandard> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorAudioStandard->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              AudioStandard = pVectorAudioStandard->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            AudioStandard = *static_cast<BofAudioStandard *> (pValue);
          }
          if (pRts_c)
          {
            snprintf(_pToString_c, _MaxSize_U32, "%s", AudioStandard.ToString().c_str());
          }
        }
        break;

        case BOFPARAMETER_ARG_TYPE::SIZE2D:
        {
          BOF_SIZE Size_X;
          if (GetFromStdVector_B)
          {
            std::vector<BOF_SIZE> *pVectorSize;
            pVectorSize = reinterpret_cast<std::vector<BOF_SIZE> *>(pValue);
            _rVectorCapacity_U32 = static_cast<uint32_t>(pVectorSize->size());
            if (_Index_U32 < _rVectorCapacity_U32)
            {
              Size_X = pVectorSize->operator[](_Index_U32);
            }
            else
            {
              pRts_c = nullptr;
            }
          }
          else
          {
            Size_X = *static_cast<BOF_SIZE *> (pValue);
          }
          if (pRts_c)
          {
            snprintf(_pToString_c, _MaxSize_U32, "%dx%d", Size_X.Width_U32, Size_X.Height_U32);
          }
          break;
        }

        default:
        {
          strncpy(_pToString_c, "???", _MaxSize_U32);
        }
        break;
      }
    }
  }
  return pRts_c;
}

//If _IndexInArray_U32==0xFFFFFFFF->Insert all array entries in help string
void BofParameter::S_BuildHelpString(const std::vector<BOFPARAMETER> &_rListOfParam_X, uint32_t _IndexInArray_U32, const std::string &_rTitle_S, std::string &_rHelpString_S)
{
  uint32_t i_U32, MaxLen_U32, VectorCapacity_U32, Index_U32;
  char     pStr_c[0x1000], pPadding_c[128], *p_c, pValue_c[256], pEntry_c[128];

  _rHelpString_S = _rTitle_S;
  MaxLen_U32 = 0;

  for (i_U32 = 0; i_U32 < _rListOfParam_X.size(); i_U32++)
  {
    if (_rListOfParam_X[i_U32].Name_S.size() > MaxLen_U32)
    {
      MaxLen_U32 = static_cast<uint32_t>(_rListOfParam_X[i_U32].Name_S.size());
    }
  }

  if (MaxLen_U32 > sizeof(pPadding_c))
  {
    MaxLen_U32 = sizeof(pPadding_c);
  }

  pStr_c[0] = 0;
  memset(pPadding_c, ' ', sizeof(pPadding_c));
  p_c = pStr_c;

  for (i_U32 = 0; i_U32 < _rListOfParam_X.size(); i_U32++)
  {
    if (_rListOfParam_X[i_U32].Name_S.size() == 1)
    {
      p_c += std::sprintf(p_c, " -%s%.*s: %s%s", _rListOfParam_X[i_U32].Name_S.c_str(), MaxLen_U32 - 1, pPadding_c, _rListOfParam_X[i_U32].Description_S.c_str(), Bof_Eol());
    }
    else
    {
      p_c += std::sprintf(p_c, "--%s%.*s: %s%s", _rListOfParam_X[i_U32].Name_S.c_str(), MaxLen_U32 - static_cast<uint32_t>(_rListOfParam_X[i_U32].Name_S.size()), pPadding_c,
                          _rListOfParam_X[i_U32].Description_S.c_str(),
                          Bof_Eol());
    }
    _rHelpString_S = _rHelpString_S + std::string(pStr_c);
    p_c = pStr_c;

    Index_U32 = (_IndexInArray_U32 == 0xFFFFFFFF) ? 0 : _IndexInArray_U32;
    VectorCapacity_U32 = 1;
    do
    {
      if (_rListOfParam_X[i_U32].pValue)
      {
        p_c += std::sprintf(p_c, "%.*s", MaxLen_U32 + 4, pPadding_c);

        //		if (_rListOfParam_X[i_U32].ArgFlag_E != BOFPARAMETER_ARG_FLAG::NONE)
        //		{
        //			p_c += std::sprintf(p_c, "%s ", S_ArgTypeToString(_rListOfParam_X[i_U32].ArgType_E));
        //		}

        p_c += std::sprintf(p_c, "(%s) %s ", Bof_IsAnyBitFlagSet(_rListOfParam_X[i_U32].ArgFlag_E, BOFPARAMETER_ARG_FLAG::READ_ONLY) ? "RO" : "RW", S_ArgTypeToString(_rListOfParam_X[i_U32].ArgType_E));
        if (_rListOfParam_X[i_U32].ArgType_E == BOFPARAMETER_ARG_TYPE::CHARSTRING)
        {
          p_c += std::sprintf(p_c, "Value '%s' ", S_ParameterToString(Index_U32, _rListOfParam_X[i_U32], false, sizeof(pValue_c), pValue_c, VectorCapacity_U32));
        }
        else
        {
          p_c += std::sprintf(p_c, "Value %s ", S_ParameterToString(Index_U32, _rListOfParam_X[i_U32], false, sizeof(pValue_c), pValue_c, VectorCapacity_U32));
        }

        if ((_rListOfParam_X[i_U32].ArgType_E >= BOFPARAMETER_ARG_TYPE::UINT8) && (_rListOfParam_X[i_U32].ArgType_E <= BOFPARAMETER_ARG_TYPE::INT64))
        {
          p_c += std::sprintf(p_c, "(0x%s) ", S_ParameterToString(Index_U32, _rListOfParam_X[i_U32], true, sizeof(pValue_c), pValue_c, VectorCapacity_U32));
        }
      }

      if (_rListOfParam_X[i_U32].ArgFlag_E != BOFPARAMETER_ARG_FLAG::NONE)
      {
        if (((_rListOfParam_X[i_U32].Min_lf) || (_rListOfParam_X[i_U32].Max_lf)) && (_rListOfParam_X[i_U32].Min_lf <= _rListOfParam_X[i_U32].Max_lf))
        {
          if (_rListOfParam_X[i_U32].ArgType_E == BOFPARAMETER_ARG_TYPE::CHARSTRING)
          {
            p_c += std::sprintf(p_c, "Min %d Max %d char (%d now) ", static_cast<uint32_t> (_rListOfParam_X[i_U32].Min_lf), static_cast<uint32_t> (_rListOfParam_X[i_U32].Max_lf),
                                _rListOfParam_X[i_U32].pValue ? static_cast<uint32_t>(strlen(static_cast<char *> (_rListOfParam_X[i_U32].pValue))) : 0);
          }
          else if (_rListOfParam_X[i_U32].ArgType_E == BOFPARAMETER_ARG_TYPE::STDSTRING)
          {
            p_c += std::sprintf(p_c, "Min %d Max %d char (%d now) ", static_cast<uint32_t> (_rListOfParam_X[i_U32].Min_lf), static_cast<uint32_t> (_rListOfParam_X[i_U32].Max_lf),
                                _rListOfParam_X[i_U32].pValue ? static_cast<uint32_t>(static_cast<std::string *> (_rListOfParam_X[i_U32].pValue)->size()) : 0);
          }
          else if (_rListOfParam_X[i_U32].ArgType_E == BOFPARAMETER_ARG_TYPE::PATH)
          {
            p_c += std::sprintf(p_c, "Min %d Max %d char (%d now) ", static_cast<uint32_t> (_rListOfParam_X[i_U32].Min_lf), static_cast<uint32_t> (_rListOfParam_X[i_U32].Max_lf),
                                _rListOfParam_X[i_U32].pValue ? static_cast<uint32_t>(static_cast<BofPath *> (_rListOfParam_X[i_U32].pValue)->FullPathName(false).size()) : 0);
          }
          else if (_rListOfParam_X[i_U32].ArgType_E == BOFPARAMETER_ARG_TYPE::URI)
          {
            p_c += std::sprintf(p_c, "Min %d Max %d char (%d now) ", static_cast<uint32_t> (_rListOfParam_X[i_U32].Min_lf), static_cast<uint32_t> (_rListOfParam_X[i_U32].Max_lf),
                                _rListOfParam_X[i_U32].pValue ? static_cast<uint32_t>(static_cast<BofUri *> (_rListOfParam_X[i_U32].pValue)->ToString().size()) : 0);
          }
          else
          {
            if ((_rListOfParam_X[i_U32].ArgType_E == BOFPARAMETER_ARG_TYPE::FLOAT) || (_rListOfParam_X[i_U32].ArgType_E == BOFPARAMETER_ARG_TYPE::DOUBLE))
            {
              p_c += std::sprintf(p_c, "Min %lf Max %lf ", _rListOfParam_X[i_U32].Min_lf, _rListOfParam_X[i_U32].Max_lf);
            }
            else
            {
              if ((_rListOfParam_X[i_U32].ArgType_E >= BOFPARAMETER_ARG_TYPE::UINT8) && (_rListOfParam_X[i_U32].ArgType_E <= BOFPARAMETER_ARG_TYPE::UINT64))
              {
                p_c += std::sprintf(p_c, "Min %" PRIu64 " (0x%" PRIX64 ") Max %" PRIu64 "(0x%" PRIX64 ") ", static_cast<uint64_t> (_rListOfParam_X[i_U32].Min_lf),
                                    static_cast<uint64_t> (_rListOfParam_X[i_U32].Min_lf), static_cast<uint64_t> (_rListOfParam_X[i_U32].Max_lf),
                                    static_cast<uint64_t> (_rListOfParam_X[i_U32].Max_lf));
              }
              else
              {
                p_c += std::sprintf(p_c, "Min %" PRId64 " (0x%" PRIX64 ") Max %" PRId64  "(0x%" PRIX64 ") ", static_cast<int64_t> (_rListOfParam_X[i_U32].Min_lf),
                                    static_cast<int64_t> (_rListOfParam_X[i_U32].Min_lf), static_cast<int64_t> (_rListOfParam_X[i_U32].Max_lf),
                                    static_cast<int64_t> (_rListOfParam_X[i_U32].Max_lf));
              }
            }
          }
        }
      }
      if (VectorCapacity_U32 > 1)
      {
        std::sprintf(pEntry_c, "[%04d/%04d]", Index_U32, VectorCapacity_U32);

        _rHelpString_S = _rHelpString_S + std::string(pEntry_c) + std::string(pStr_c + 12) + Bof_Eol();   //+12k to skip padding space used by pEntry_c
      }
      else
      {
        _rHelpString_S = _rHelpString_S + std::string(pStr_c) + Bof_Eol();
      }
      p_c = pStr_c;
      Index_U32++;
      if (_IndexInArray_U32 != 0xFFFFFFFF)
      {
        break;
      }
    } while (Index_U32 < VectorCapacity_U32);
  }
}

END_BOF_NAMESPACE()