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
 * Name:        bofuri.cpp
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

#include <bofstd/bofuri.h>
#include "bofstd/bofstring.h"
#include <iterator>
 
BEGIN_BOF_NAMESPACE()

BofUri::BofUri()
{
  InitUriField("");
}

BofUri::BofUri(const char *_pUri_c)
{
  std::string Uri_S;

  if (_pUri_c)
  {
    Uri_S = _pUri_c;
    InitUriField(Uri_S);
  }
  else
  {
    mValid_B = false;
    //InitUriField("");
  }
}

BofUri::BofUri(const std::string &_rUri_S)
{
  InitUriField(_rUri_S);
}

BofUri::BofUri(const BOF_SOCKET_ADDRESS_COMPONENT &_rSchemeAuthority_X, const BofPath &_rPath, std::map<std::string, std::string> &_rQueryParamCollection, const std::string &_rFragment_S)
{
  mSchemeAuthority_X = _rSchemeAuthority_X;
  mPath = _rPath;
  mQueryParamCollection = _rQueryParamCollection;
  mFragment_S = _rFragment_S;
  InitUriField(ToString());
}

BofUri::BofUri(const std::string &_rScheme_S, const std::string &_rAuthority_S, const std::string &_rPath_S, const std::string &_rQueryParam_S, const std::string &_rFragment_S)
{
  if (SetScheme(_rScheme_S) == BOF_ERR_NO_ERROR)
  {
    if (SetAuthority(_rAuthority_S) == BOF_ERR_NO_ERROR)
    {
      if (SetPath(_rPath_S) == BOF_ERR_NO_ERROR)
      {
        if (SetQueryParamCollection(_rQueryParam_S) == BOF_ERR_NO_ERROR)
        {
          if (SetFragment(_rFragment_S) == BOF_ERR_NO_ERROR)
          {
          }
        }
      }
    }
  }
}

BofUri::BofUri(const BofUri &_rOther_O)
{
  InitUriField(_rOther_O.ToString());
}

BofUri::BofUri(BofUri &&_rrOther_O)
{
  // Pilfer others resource
  InitUriField(_rrOther_O.ToString());
  // Reset other
  _rrOther_O.InitUriField("");
}

BofUri &BofUri::operator=(const BofUri &_rOther_O)
{
  InitUriField(_rOther_O.ToString());
  return *this;
}

BofUri &BofUri::operator=(BofUri &&_rrOther_O)
{
  if (this != &_rrOther_O)
  {
    // Release the current object�s resources
    // Pilfer others resource
    InitUriField(_rrOther_O.ToString());
    // Reset other
    _rrOther_O.InitUriField("");
  }
  return *this;
}

BofUri &BofUri::operator=(const char *_pNewUri_c)
{
  std::string Uri_S;

  if (_pNewUri_c)
  {
    Uri_S = _pNewUri_c;
    InitUriField(Uri_S);
  }
  else
  {
    //InitUriField("");
    mValid_B = false;
  }
  return *this;
}

BofUri &BofUri::operator=(const std::string &_rNewUri_S)
{
  InitUriField(_rNewUri_S);
  return *this;
}

BofUri::~BofUri()
{}


bool BofUri::operator==(const BofUri &_rOther_O) const
{
  return ( (mSchemeAuthority_X == _rOther_O.mSchemeAuthority_X) && (mPath == _rOther_O.mPath) &&
           (mQueryParamCollection == _rOther_O.mQueryParamCollection) && (mFragment_S == _rOther_O.mFragment_S) && 
           (mQueryParamDelimiter_c == _rOther_O.mQueryParamDelimiter_c) && (mValid_B == _rOther_O.mValid_B));
}

bool BofUri::operator!=(const BofUri &_rOther_O) const
{
  return !(*this == _rOther_O);
}

bool BofUri::IsValid() const
{
  return (mValid_B);
}

BOFERR BofUri::SetQueryParamDelimiter(char _QueryParamDelimiter_c)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if ((_QueryParamDelimiter_c == '&') || (_QueryParamDelimiter_c == ';'))
  {
    mQueryParamDelimiter_c = _QueryParamDelimiter_c;
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

BOFERR BofUri::SetScheme(const std::string &_rScheme_S)
{
  BOFERR Rts_E;

  mSchemeAuthority_X.Protocol_S = _rScheme_S; //Needed by ToString below 
  InitUriField(ToString()); //Can fail if path is not set for example -> Rts is ok but is valid is false
  Rts_E = BOF_ERR_NO_ERROR;
  
  return Rts_E;
}
BOFERR BofUri::SetAuthority(const std::string &_rAuthority_S)
{
  BOFERR Rts_E;
  BOF_SOCKET_ADDRESS_COMPONENT				SchemeAuthority_X;

  if (_rAuthority_S == "")
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
  else
  {
    Rts_E = Bof_SplitIpAddress(_rAuthority_S, SchemeAuthority_X);
  }
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    mSchemeAuthority_X.User_S = SchemeAuthority_X.User_S; //Needed by ToString below 
    mSchemeAuthority_X.Password_S = SchemeAuthority_X.Password_S; //Needed by ToString below 
    mSchemeAuthority_X.IpAddress_S = SchemeAuthority_X.IpAddress_S; //Needed by ToString below 
    mSchemeAuthority_X.Port_U16 = SchemeAuthority_X.Port_U16; //Needed by ToString below 
    InitUriField(ToString()); //Can fail if path is not set for example -> Rts is ok but is valid is false
  }
  return Rts_E;
}

BOFERR BofUri::SetSchemeAuthority(const std::string &_rSchemeAuthority_S)
{
  BOFERR Rts_E;
  BOF_SOCKET_ADDRESS_COMPONENT				SchemeAuthority_X;

  Rts_E = Bof_SplitIpAddress(_rSchemeAuthority_S, SchemeAuthority_X);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    mSchemeAuthority_X = SchemeAuthority_X; //Needed by ToString below
    InitUriField(ToString()); //Can fail if path is not set for example -> Rts is ok but is valid is false
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

BOFERR BofUri::SetSchemeAuthority(const BOF_SOCKET_ADDRESS_COMPONENT &_rSchemeAuthority_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if ((_rSchemeAuthority_X.Protocol_S != "") && (_rSchemeAuthority_X.IpAddress_S != ""))
  {
    mSchemeAuthority_X = _rSchemeAuthority_X; //Needed by ToString below
    InitUriField(ToString()); //Can fail if path is not set for example -> Rts is ok but is valid is false
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}
BOFERR BofUri::SetPath(const std::string &_rPath_S)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  BofPath Path(_rPath_S);

  if (Path.IsValid())
  {
    mPath = Path; //Needed by ToString below
    InitUriField(ToString()); //Can fail if scheme is not set for example -> Rts is ok but is valid is false
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}
BOFERR BofUri::SetPath(const BofPath &_rPath)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  BofPath Path(_rPath);

  if (Path.IsValid())
  {
    mPath = Path; //Needed by ToString below
    InitUriField(ToString()); //Can fail if scheme is not set for example -> Rts is ok but is valid is false
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}
BOFERR BofUri::SetQueryParamCollection(const std::map<std::string, std::string> &_rQueryParamCollection)
{
  BOFERR Rts_E;

  mQueryParamCollection = _rQueryParamCollection; //Needed by ToString below
  InitUriField(ToString()); //Can fail if path is not set for example -> Rts is ok but is valid is false
  Rts_E = BOF_ERR_NO_ERROR;
  return Rts_E;
}
BOFERR BofUri::SetQueryParamCollection(const std::string &_rQueryParam_S)
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

BOFERR BofUri::AddToQueryParamCollection(const std::string &_rKey_S, const std::string &_rValue_S)
{
  BOFERR Rts_E = BOF_ERR_DUPLICATE;

  const auto &_rIt = mQueryParamCollection.find(_rKey_S);
  if (_rIt == mQueryParamCollection.cend())
  {
    mQueryParamCollection[_rKey_S] = _rValue_S; //Needed by ToString below
    InitUriField(ToString()); //Can fail if path is not set for example -> Rts is ok but is valid is false
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

BOFERR BofUri::RemoveFromQueryParamCollection(const std::string &_rQueryParamKeyToRemove_S)
{
  BOFERR Rts_E = BOF_ERR_DONT_EXIST;

  const auto &_rIt = mQueryParamCollection.find(_rQueryParamKeyToRemove_S);
  if (_rIt != mQueryParamCollection.cend())
  {
    mQueryParamCollection.erase(_rQueryParamKeyToRemove_S); //Needed by ToString below
    InitUriField(ToString()); //Can fail if path is not set for example -> Rts is ok but is valid is false
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}
BOFERR BofUri::SetFragment(const std::string &_rFragment_S)
{
  BOFERR Rts_E;
  mFragment_S = _rFragment_S; //Needed by ToString below
  InitUriField(ToString()); //Can fail if path is not set for example -> Rts is ok but is valid is false
  Rts_E = BOF_ERR_NO_ERROR;
  return Rts_E;
}
char BofUri::QueryParamDelimiter() const
{
  return mQueryParamDelimiter_c;
}
const BOF_SOCKET_ADDRESS_COMPONENT &BofUri::SchemeAuthority(std::string &_rSchemeAuthority_S) const
{
  _rSchemeAuthority_S = mSchemeAuthority_X.ToString(true,true,true,true);
  return mSchemeAuthority_X;
}
const std::string &BofUri::Scheme() const
{
  return mSchemeAuthority_X.Protocol_S;
}
std::string BofUri::Authority() const
{
  BOF_SOCKET_ADDRESS_COMPONENT Rts_X;

  Rts_X = mSchemeAuthority_X;
  Rts_X.Protocol_S = ""; 
 
  return  Rts_X.ToString(true, true, true, true);
}
const BOF_SOCKET_ADDRESS &BofUri::IpAddress(std::string &_rIpAddress_S) const
{
  _rIpAddress_S = mSchemeAuthority_X.Protocol_S + "://" + Bof_SocketAddressToString(mSchemeAuthority_X.Ip_X, false, true);
  return mSchemeAuthority_X.Ip_X;
}
const BofPath &BofUri::Path(std::string &_rPath_S) const
{
  _rPath_S = mPath.FullPathName(false);
  return mPath;
}
const std::map<std::string, std::string> &BofUri::QueryParamCollection(std::string &_rQueryParamCollection_S) const
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
const std::string &BofUri::Fragment() const
{
  return mFragment_S;
}
std::string	BofUri::ToString() const
{
  std::ostringstream Rts;

//  if ((mQueryDelimiter_c == '&') || (mQueryDelimiter_c == ';'))
  {
    Rts << mSchemeAuthority_X.ToString(true, true, true, true);
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

BOFERR BofUri::InitUriField(const std::string &_rUri_S)
{
  BOFERR Rts_E = BOF_ERR_FORMAT;
  BOF_SOCKET_ADDRESS_COMPONENT Uri_X;
  std::string Path_S, Query_S, Fragment_S;

  mValid_B = false;
  Rts_E = Bof_SplitUri(_rUri_S, Uri_X, Path_S, Query_S, Fragment_S);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    mSchemeAuthority_X = Uri_X;
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
  //printf("_rUri_S %s\n", _rUri_S.c_str());
  PosEop = _rUri_S.find("://");
  if (PosEop != std::string::npos)
  {
    //printf("PosEop %I64d: %s\n", PosEop, _rUri_S.substr(0, PosEop + 3).c_str());
    PosSlash = _rUri_S.find('/', PosEop + 3);
    if (PosSlash != std::string::npos)
    {
      //printf("PosSlash %I64d: %s\n", PosSlash, _rUri_S.substr(PosSlash).c_str());
      PosQuestion = _rUri_S.find('?', PosSlash + 1);
      PosDash = _rUri_S.rfind('#');
      if ((PosQuestion == std::string::npos) && (PosDash == std::string::npos))
      {
        Path_S = _rUri_S.substr(PosSlash);
        Query_S = "";
        mFragment_S = "";
        Rts_E = BOF_ERR_NO_ERROR;
      }
      if ((PosQuestion != std::string::npos) && (PosDash != std::string::npos))
      {
        //printf("PosQuestion1 %I64d: %s\n", PosQuestion, _rUri_S.substr(PosQuestion + 1).c_str());
        //printf("PosDash1 %I64d: %s\n", PosDash, _rUri_S.substr(PosDash).c_str());
        if (PosQuestion < PosDash)
        {
          Path_S = _rUri_S.substr(PosSlash, PosQuestion - PosSlash);
          Query_S = _rUri_S.substr(PosQuestion + 1, PosDash - PosQuestion - 1);
          mFragment_S = _rUri_S.substr(PosDash + 1);
          Rts_E = BOF_ERR_NO_ERROR;
        }
      }
      else if ((PosQuestion != std::string::npos) && (PosDash == std::string::npos))
      {
        //printf("PosQuestion2 %I64d: %s\n", PosQuestion, _rUri_S.substr(PosQuestion + 1).c_str());
        Path_S = _rUri_S.substr(PosSlash, PosQuestion - PosSlash);
        Query_S = _rUri_S.substr(PosQuestion + 1);
        mFragment_S = "";
        Rts_E = BOF_ERR_NO_ERROR;
      }
      else if ((PosQuestion == std::string::npos) && (PosDash != std::string::npos))
      {
        //printf("PosDash %I64d: %s\n", PosDash, _rUri_S.substr(PosDash).c_str());
        Path_S = _rUri_S.substr(PosSlash, PosDash - PosSlash);
        Query_S = "";
        mFragment_S = _rUri_S.substr(PosDash + 1);
        Rts_E = BOF_ERR_NO_ERROR;
      }
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Scheme_S = _rUri_S.substr(0, PosSlash);
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

std::string BofUri::S_UrlEncode(const std::string &_rIn_S)
{
  std::string Rts_S = "";
  char c_c, pHexBuffer_c[8];
  const char *pText_c = _rIn_S.c_str();
  int i, Char_i, Len_i = strlen(pText_c);

  for (i = 0; i < Len_i; i++) 
  {
    c_c = pText_c[i];
    Char_i = c_c;
    // uncomment this if you want to encode spaces with +
    /*if (c==' ') new_str += '+';
    else */if (isalnum(c_c) || c_c == '-' || c_c == '_' || c_c == '.' || c_c == '~')
    {
      Rts_S += c_c;
    }
    else 
    {
      sprintf(pHexBuffer_c, "%X", c_c);
      if (Char_i < 16)
      {
        Rts_S += "%0";
      }
      else
      {
        Rts_S += "%";
      }
      Rts_S += pHexBuffer_c;
    }
  }
  return Rts_S;
}

std::string BofUri::S_UrlDecode(const std::string &_rIn_S)
{
  std::string Rts_S;
  char c_c;
  int i, Val_i, Len_i = _rIn_S.length();

  for (i = 0; i < Len_i; i++) 
  {
    if (_rIn_S[i] != '%') 
    {
      if (_rIn_S[i] == '+')
      {
        Rts_S += ' ';
      }
      else
      {
        Rts_S += _rIn_S[i];
      }
    }
    else 
    {
      sscanf(_rIn_S.substr(i + 1, 2).c_str(), "%x", &Val_i);
      c_c = static_cast<char>(Val_i);
      Rts_S += c_c;
      i = i + 2;
    }
  }
  return Rts_S;
}

BOFERR BofUri::ExtractQueryParamIntoCollection(const std::string &_rQueryParam_S, std::map<std::string, std::string> &_rQueryParamCollection)
{
  BOFERR Rts_E;
  std::string Query_S, Key_S, Val_S;
  std::vector<std::string> KeyValCollection;
  std::string::size_type PosEqual;

  Query_S = S_UrlDecode(_rQueryParam_S);
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