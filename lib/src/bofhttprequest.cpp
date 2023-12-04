/*
 * Copyright (c) 2013-2033, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bofuri module of the bofstd library
 *
 * Name:        bofhttprequest.cpp
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
#include "bofstd/bofstring.h"
#include <bofstd/bofhttprequest.h>
#include <bofstd/bofuri.h>
#include <bofstd/bofenum.h>

#include <iterator>

BEGIN_BOF_NAMESPACE()

static BOF::BofEnum<BOF_HTTP_REQUEST_TYPE> S_BofHttpRequestTypeEnumConverter(
{
  {BOF_HTTP_REQUEST_UNKNOWN,"UNKNOWN"},
  {BOF_HTTP_REQUEST_GET,    "GET"},
  {BOF_HTTP_REQUEST_POST,   "POST"},
  {BOF_HTTP_REQUEST_PUT,    "PUT"},
  {BOF_HTTP_REQUEST_PATH,   "PATH"},
  {BOF_HTTP_REQUEST_DELETE, "DELETE"},
  {BOF_HTTP_REQUEST_HEAD,   "HEAD"},
  {BOF_HTTP_REQUEST_OPTIONS,"OPTIONS"},
  {BOF_HTTP_REQUEST_CONNECT,"CONNECT"},
  {BOF_HTTP_REQUEST_TRACE,  "TRACE"},
  {BOF_HTTP_REQUEST_DBG_ECHO,  "DBG_ECHO"},   
  },
  BOF_HTTP_REQUEST_UNKNOWN);

BofHttpRequest::BofHttpRequest()
{
  InitHttpRequestField("");
}

BofHttpRequest::BofHttpRequest(const char *_pHttpRequest_c)
{
  std::string HttpRequest_S;

  if (_pHttpRequest_c)
  {
    HttpRequest_S = _pHttpRequest_c;
    InitHttpRequestField(HttpRequest_S);
  }
  else
  {
    mValid_B = false;
    // InitHttpRequestField("");
  }
}

BofHttpRequest::BofHttpRequest(const std::string &_rHttpRequest_S)
{
  InitHttpRequestField(_rHttpRequest_S);
}

BofHttpRequest::BofHttpRequest(const std::string &_rMethod_S, const BofPath &_rPath, std::map<std::string, std::string> &_rQueryParamCollection, const std::string &_rFragment_S)
{
  mMethod_S = _rMethod_S;
  mMethod_E = S_BofHttpRequestTypeEnumConverter.ToEnum(mMethod_S);
  mPath = _rPath;
  mQueryParamCollection = _rQueryParamCollection;
  mFragment_S = _rFragment_S;
  InitHttpRequestField(ToString());
}
BofHttpRequest::BofHttpRequest(BOF_HTTP_REQUEST_TYPE _Method_E, const BofPath &_rPath, std::map<std::string, std::string> &_rQueryParamCollection, const std::string &_rFragment_S)
{
  BofHttpRequest(S_BofHttpRequestTypeEnumConverter.ToString(_Method_E), _rPath, _rQueryParamCollection, _rFragment_S);
}
BofHttpRequest::BofHttpRequest(const BofHttpRequest &_rOther_O)
{
  InitHttpRequestField(_rOther_O.ToString());
}

BofHttpRequest::BofHttpRequest(BofHttpRequest &&_rrOther_O)
{
  // Pilfer others resource
  InitHttpRequestField(_rrOther_O.ToString());
  // Reset other
  _rrOther_O.InitHttpRequestField("");
}

BofHttpRequest &BofHttpRequest::operator=(const BofHttpRequest &_rOther_O)
{
  InitHttpRequestField(_rOther_O.ToString());
  return *this;
}

BofHttpRequest &BofHttpRequest::operator=(BofHttpRequest &&_rrOther_O)
{
  if (this != &_rrOther_O)
  {
    // Release the current object�s resources
    // Pilfer others resource
    InitHttpRequestField(_rrOther_O.ToString());
    // Reset other
    _rrOther_O.InitHttpRequestField("");
  }
  return *this;
}

BofHttpRequest &BofHttpRequest::operator=(const char *_pNewHttpRequest_c)
{
  std::string HttpRequest_S;

  if (_pNewHttpRequest_c)
  {
    HttpRequest_S = _pNewHttpRequest_c;
    InitHttpRequestField(HttpRequest_S);
  }
  else
  {
    // InitHttpRequestField("");
    mValid_B = false;
  }
  return *this;
}

BofHttpRequest &BofHttpRequest::operator=(const std::string &_rNewHttpRequest_S)
{
  InitHttpRequestField(_rNewHttpRequest_S);
  return *this;
}

BofHttpRequest::~BofHttpRequest()
{
}

bool BofHttpRequest::operator==(const BofHttpRequest &_rOther_O) const
{
  return ((mMethod_S == _rOther_O.mMethod_S) && (mMethod_E == _rOther_O.mMethod_E) && (mPath == _rOther_O.mPath) && (mQueryParamCollection == _rOther_O.mQueryParamCollection) && (mFragment_S == _rOther_O.mFragment_S) &&
          (mQueryParamDelimiter_c == _rOther_O.mQueryParamDelimiter_c) && (mValid_B == _rOther_O.mValid_B));
}

bool BofHttpRequest::operator!=(const BofHttpRequest &_rOther_O) const
{
  return !(*this == _rOther_O);
}
BOF_HTTP_REQUEST_TYPE BofHttpRequest::S_RequestType(const char *_pRequest_c)
{
  BOF_HTTP_REQUEST_TYPE Rts_E = BOF_HTTP_REQUEST_TYPE::BOF_HTTP_REQUEST_UNKNOWN;
  uint32_t i_U32;
  const char *pRequestEnum_c;

  if (_pRequest_c)
  {
    for (i_U32 = BOF_HTTP_REQUEST_TYPE::BOF_HTTP_REQUEST_GET; i_U32 < BOF_HTTP_REQUEST_TYPE::BOF_HTTP_REQUEST_MAX; i_U32++)
    {
      pRequestEnum_c = S_BofHttpRequestTypeEnumConverter.ToString((BOF_HTTP_REQUEST_TYPE)i_U32).c_str();
      if (!strncmp(pRequestEnum_c, _pRequest_c, strlen(pRequestEnum_c)))
      {
        Rts_E= (BOF_HTTP_REQUEST_TYPE)i_U32;
          break;
      }
    }
  }
  return Rts_E;
}

std::string BofHttpRequest::S_RequestString(BOF_HTTP_REQUEST_TYPE _Method_E) 
{
  return S_BofHttpRequestTypeEnumConverter.ToString(_Method_E);
}

bool BofHttpRequest::IsValid() const
{
  return (mValid_B);
}

BOFERR BofHttpRequest::SetQueryParamDelimiter(char _QueryParamDelimiter_c)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if ((_QueryParamDelimiter_c == '&') || (_QueryParamDelimiter_c == ';'))
  {
    mQueryParamDelimiter_c = _QueryParamDelimiter_c;
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

BOFERR BofHttpRequest::SetMethod(const std::string &_rMethod_S)
{
  BOFERR Rts_E;

  mMethod_S= _rMethod_S; 
  mMethod_E = S_BofHttpRequestTypeEnumConverter.ToEnum(mMethod_S);
  InitHttpRequestField(ToString());                   // Can fail if path is not set for example -> Rts is ok but is valid is false
  Rts_E = BOF_ERR_NO_ERROR;

  return Rts_E;
}
BOFERR BofHttpRequest::SetMethod(BOF_HTTP_REQUEST_TYPE _Method_E)
{
  return SetMethod(S_BofHttpRequestTypeEnumConverter.ToString(_Method_E));
}
BOFERR BofHttpRequest::SetPath(const BofPath &_rPath)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  BofPath Path(_rPath);

  if (Path.IsValid())
  {
    mPath = Path;             // Needed by ToString below
    InitHttpRequestField(ToString()); // Can fail if scheme is not set for example -> Rts is ok but is valid is false
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}
BOFERR BofHttpRequest::SetQueryParamCollection(const std::map<std::string, std::string> &_rQueryParamCollection)
{
  BOFERR Rts_E;

  mQueryParamCollection = _rQueryParamCollection; // Needed by ToString below
  InitHttpRequestField(ToString());                       // Can fail if path is not set for example -> Rts is ok but is valid is false
  Rts_E = BOF_ERR_NO_ERROR;
  return Rts_E;
}
BOFERR BofHttpRequest::SetQueryParamCollection(const std::string &_rQueryParam_S)
{
  BOFERR Rts_E;
  std::map<std::string, std::string> QueryParamCollection;

  Rts_E = ExtractQueryParamIntoCollection(_rQueryParam_S, QueryParamCollection);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Rts_E = SetQueryParamCollection(QueryParamCollection);
  }
  return Rts_E;
}

BOFERR BofHttpRequest::AddToQueryParamCollection(const std::string &_rKey_S, const std::string &_rValue_S)
{
  BOFERR Rts_E = BOF_ERR_DUPLICATE;

  const auto &_rIt = mQueryParamCollection.find(_rKey_S);
  if (_rIt == mQueryParamCollection.cend())
  {
    mQueryParamCollection[_rKey_S] = _rValue_S; // Needed by ToString below
    InitHttpRequestField(ToString());                   // Can fail if path is not set for example -> Rts is ok but is valid is false
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

BOFERR BofHttpRequest::RemoveFromQueryParamCollection(const std::string &_rQueryParamKeyToRemove_S)
{
  BOFERR Rts_E = BOF_ERR_DONT_EXIST;

  const auto &_rIt = mQueryParamCollection.find(_rQueryParamKeyToRemove_S);
  if (_rIt != mQueryParamCollection.cend())
  {
    mQueryParamCollection.erase(_rQueryParamKeyToRemove_S); // Needed by ToString below
    InitHttpRequestField(ToString());                               // Can fail if path is not set for example -> Rts is ok but is valid is false
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}
BOFERR BofHttpRequest::SetFragment(const std::string &_rFragment_S)
{
  BOFERR Rts_E;
  mFragment_S = _rFragment_S; // Needed by ToString below
  InitHttpRequestField(ToString());   // Can fail if path is not set for example -> Rts is ok but is valid is false
  Rts_E = BOF_ERR_NO_ERROR;
  return Rts_E;
}
char BofHttpRequest::QueryParamDelimiter() const
{
  return mQueryParamDelimiter_c;
}

const std::string &BofHttpRequest::Method(BOF_HTTP_REQUEST_TYPE &_rMethod_E) const
{
  _rMethod_E = mMethod_E;
  return mMethod_S;
}

const BofPath &BofHttpRequest::Path(std::string &_rPath_S) const
{
  _rPath_S = mPath.FullPathName(false);
  return mPath;
}
const BofPath &BofHttpRequest::Path() const
{
  return mPath;
}
const std::map<std::string, std::string> &BofHttpRequest::QueryParamCollection(std::string &_rQueryParamCollection_S) const
{
  _rQueryParamCollection_S = "";
  if (!mQueryParamCollection.empty())
  {
    for (auto It = mQueryParamCollection.begin(); It != mQueryParamCollection.end();)
    {
      if (It->second != "")
      {
        _rQueryParamCollection_S += It->first + '=' + It->second;
      }
      else
      {
        _rQueryParamCollection_S += It->first;
      }
      if (++It != mQueryParamCollection.end())
      {
        _rQueryParamCollection_S += mQueryParamDelimiter_c;
      }
    }
  }

  return mQueryParamCollection;
}
const std::map<std::string, std::string> &BofHttpRequest::QueryParamCollection() const
{
  return mQueryParamCollection;
}
const std::string &BofHttpRequest::Fragment() const
{
  return mFragment_S;
}
std::string BofHttpRequest::ToString() const
{
  std::ostringstream Rts;

  //  if ((mQueryDelimiter_c == '&') || (mQueryDelimiter_c == ';'))
  {
    Rts << mMethod_S << " ";
    Rts << mPath.FullPathName(false);
    if (!mQueryParamCollection.empty())
    {
      Rts << '?';
      for (auto It = mQueryParamCollection.begin(); It != mQueryParamCollection.end();)
      {
        if (It->second != "")
        {
          Rts << It->first << '=' << It->second;
        }
        else
        {
          Rts << It->first;
        }
        if (++It != mQueryParamCollection.end())
        {
          Rts << mQueryParamDelimiter_c;
        }
      }
    }
    if (mFragment_S != "")
    {
      Rts << '#' << mFragment_S;
    }
  }
  return Rts.str();
}

BOFERR BofHttpRequest::InitHttpRequestField(const std::string &_rHttpRequest_S)
{
  BOFERR Rts_E = BOF_ERR_FORMAT;
  std::string Method_S, Path_S, Query_S, Fragment_S;

  mValid_B = false;
  Rts_E = Bof_SplitHttpRequest(_rHttpRequest_S, Method_S, Path_S, Query_S, Fragment_S);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    mMethod_S = Method_S;
    mMethod_E = S_BofHttpRequestTypeEnumConverter.ToEnum(mMethod_S);
    mPath = BofPath(Path_S);
    mFragment_S = Fragment_S;
    if (mPath.IsValid())
    {
      if (Query_S != "")
      {
        Rts_E = ExtractQueryParamIntoCollection(Query_S, mQueryParamCollection);
      }
      else
      {
        mQueryParamCollection.clear();
      }
    }
    else
    {
      Rts_E = BOF_ERR_ENOENT;
    }
  }
#if 0
  std::string::size_type PosEop, PosSlash, PosQuestion, PosDash, PosEqual;
  std::string Scheme_S, Path_S, Query_S, Key_S, Val_S;
  std::vector<std::string> KeyValCollection;

  Rts_E = BOF_ERR_FORMAT;
  //printf("_rHttpRequest_S %s\n", _rHttpRequest_S.c_str());
  PosEop = _rHttpRequest_S.find("://");
  if (PosEop != std::string::npos)
  {
    //printf("PosEop %I64d: %s\n", PosEop, _rHttpRequest_S.substr(0, PosEop + 3).c_str());
    PosSlash = _rHttpRequest_S.find('/', PosEop + 3);
    if (PosSlash != std::string::npos)
    {
      //printf("PosSlash %I64d: %s\n", PosSlash, _rHttpRequest_S.substr(PosSlash).c_str());
      PosQuestion = _rHttpRequest_S.find('?', PosSlash + 1);
      PosDash = _rHttpRequest_S.rfind('#');
      if ((PosQuestion == std::string::npos) && (PosDash == std::string::npos))
      {
        Path_S = _rHttpRequest_S.substr(PosSlash);
        Query_S = "";
        mFragment_S = "";
        Rts_E = BOF_ERR_NO_ERROR;
      }
      if ((PosQuestion != std::string::npos) && (PosDash != std::string::npos))
      {
        //printf("PosQuestion1 %I64d: %s\n", PosQuestion, _rHttpRequest_S.substr(PosQuestion + 1).c_str());
        //printf("PosDash1 %I64d: %s\n", PosDash, _rHttpRequest_S.substr(PosDash).c_str());
        if (PosQuestion < PosDash)
        {
          Path_S = _rHttpRequest_S.substr(PosSlash, PosQuestion - PosSlash);
          Query_S = _rHttpRequest_S.substr(PosQuestion + 1, PosDash - PosQuestion - 1);
          mFragment_S = _rHttpRequest_S.substr(PosDash + 1);
          Rts_E = BOF_ERR_NO_ERROR;
        }
      }
      else if ((PosQuestion != std::string::npos) && (PosDash == std::string::npos))
      {
        //printf("PosQuestion2 %I64d: %s\n", PosQuestion, _rHttpRequest_S.substr(PosQuestion + 1).c_str());
        Path_S = _rHttpRequest_S.substr(PosSlash, PosQuestion - PosSlash);
        Query_S = _rHttpRequest_S.substr(PosQuestion + 1);
        mFragment_S = "";
        Rts_E = BOF_ERR_NO_ERROR;
      }
      else if ((PosQuestion == std::string::npos) && (PosDash != std::string::npos))
      {
        //printf("PosDash %I64d: %s\n", PosDash, _rHttpRequest_S.substr(PosDash).c_str());
        Path_S = _rHttpRequest_S.substr(PosSlash, PosDash - PosSlash);
        Query_S = "";
        mFragment_S = _rHttpRequest_S.substr(PosDash + 1);
        Rts_E = BOF_ERR_NO_ERROR;
      }
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Scheme_S = _rHttpRequest_S.substr(0, PosSlash);
        //printf("Scheme_S %s\n", Scheme_S.c_str());

        Rts_E = Bof_SplitIpAddress(Scheme_S, mSchemeAuthority_X);
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          mPath = BofPath(Path_S);
          if (mPath.IsValid())
          {
            if (Query_S != "")
            {
              Rts_E = ExtractQueryParamIntoCollection(Query_S, mQueryParamCollection);
            }
            else
            {
              mQueryParamCollection.clear();
            }
          }
          else
          {
            Rts_E = BOF_ERR_ENOENT;
          }
        }
      }
    }
  }
#endif
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    mValid_B = true;
  }

  return (Rts_E);
}

BOFERR BofHttpRequest::ExtractQueryParamIntoCollection(const std::string &_rQueryParam_S, std::map<std::string, std::string> &_rQueryParamCollection)
{
  BOFERR Rts_E;
  std::string Query_S, Key_S, Val_S;
  std::vector<std::string> KeyValCollection;
  std::string::size_type PosEqual;

  Query_S = BofUri::S_UrlDecode(_rQueryParam_S);
  _rQueryParamCollection.clear();
  Rts_E = BOF_ERR_NO_ERROR;
  KeyValCollection = Bof_StringSplit(Query_S, "&;");
  for (const auto &rIt : KeyValCollection)
  {
    PosEqual = rIt.find('=');
    if (PosEqual != std::string::npos)
    {
      Key_S = rIt.substr(0, PosEqual);
      Val_S = rIt.substr(PosEqual + 1);
    }
    else
    {
      Key_S = rIt;
      Val_S = "";
    }
    if (_rQueryParamCollection.find(Key_S) == _rQueryParamCollection.end())
    {
      _rQueryParamCollection[Key_S] = Val_S;
    }
    else
    {
      Rts_E = BOF_ERR_EKEYREVOKED;
      break;
    }
  }
  return Rts_E;
}

END_BOF_NAMESPACE()