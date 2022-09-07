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

BofUri::BofUri(const BOF_SOCKET_ADDRESS_COMPONENT &_rScheme_X, const BofPath &_rPath, std::map<std::string, std::string> &_rQueryCollection, const std::string &_rFragment_S)
{
  mScheme_X = _rScheme_X;
  mPath = _rPath;
  mQueryCollection = _rQueryCollection;
  mFragment_S = _rFragment_S;
  InitUriField(ToString());
}

BofUri::BofUri(const BofUri &_rOther_O)
{
  InitUriField(_rOther_O.ToString());
}

BofUri::BofUri(BofUri &&_rrOther_O)
{
  // Pilfer other�s resource
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
    // Pilfer other�s resource
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
  return (mScheme_X == _rOther_O.mScheme_X) && (mPath == _rOther_O.mPath) && (mQueryCollection == _rOther_O.mQueryCollection) && (mFragment_S == _rOther_O.mFragment_S) && (mValid_B == _rOther_O.mValid_B);
}

bool BofUri::operator!=(const BofUri &_rOther_O) const
{
  return !(*this == _rOther_O);
}

bool BofUri::IsValid() const
{
  return (mValid_B);
}

BOFERR BofUri::SetScheme(const std::string &_rScheme_S)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  return Rts_E;
}

BOFERR BofUri::SetScheme(const BOF_SOCKET_ADDRESS_COMPONENT &_rScheme_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  return Rts_E;
}
BOFERR BofUri::SetPath(const std::string &_rPath_S)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  return Rts_E;
}
BOFERR BofUri::SetPath(const BofPath &_rBofPath)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  return Rts_E;
}
BOFERR BofUri::SetQueryCollection(const std::map<std::string, std::string> &_rQueryCollection)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  return Rts_E;
}
BOFERR BofUri::AddToQueryCollection(const std::pair<std::string, std::string> &_rQuery)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  return Rts_E;
}
BOFERR BofUri::RemoveFromQueryCollection(const std::string &_rQueryKeyToRemove_S)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  return Rts_E;
}
BOFERR BofUri::SetFragment(const std::string &_rFragment_S)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  return Rts_E;
}
const BOF_SOCKET_ADDRESS_COMPONENT &BofUri::Scheme(std::string &_rScheme_S) const
{
  _rScheme_S = mScheme_X.ToString();
  return mScheme_X;
}
const BOF_SOCKET_ADDRESS &BofUri::IpAddress(std::string &_rIpAddress_S) const
{
  _rIpAddress_S = mScheme_X.Protocol_S + "://" + Bof_SocketAddressToString(mIpAddress_X, false, true);
  return mIpAddress_X;
}
const BofPath &BofUri::Path(std::string &_rPath_S) const
{
  _rPath_S = mPath.FullPathName(false);
  return mPath;
}
const std::map<std::string, std::string> &BofUri::QueryCollection(std::string &_rQueryCollection_S) const
{
  _rQueryCollection_S = "";
  if (!mQueryCollection.empty())
  {
    for (auto &rIt = mQueryCollection.cbegin(); rIt != mQueryCollection.cend();)
    {
      if (rIt->second != "")
      {
        _rQueryCollection_S += rIt->first + '=' + rIt->second;
      }
      else
      {
        _rQueryCollection_S += rIt->first;
      }
      if (++rIt != mQueryCollection.cend())
      {
        _rQueryCollection_S += mQueryDelimiter_c;
      }
    }
  }

  return mQueryCollection;
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
    Rts << mScheme_X.ToString();
    Rts << mPath.FullPathName(false);
    if (!mQueryCollection.empty())
    {
      Rts << '?';
      for (auto &rIt = mQueryCollection.cbegin(); rIt != mQueryCollection.cend();)
      {
        if (rIt->second != "")
        {
          Rts << rIt->first << '=' << rIt->second;
        }
        else
        {
          Rts << rIt->first;
        }
        if (++rIt != mQueryCollection.cend())
        {
          Rts << mQueryDelimiter_c;
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
  std::string::size_type PosEop, PosSlash, PosQuestion, PosDash, PosEqual;
  std::string Scheme_S, Path_S, Query_S, Key_S, Val_S;
  std::vector<std::string> KeyValCollection;

  mScheme_X.Reset();
  mIpAddress_X.Reset();
  mPath = BofPath();
  mQueryCollection.clear();
  mFragment_S = "";
  mValid_B = false;

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
        Rts_E = BOF_ERR_NO_ERROR;
      }
      else if ((PosQuestion == std::string::npos) && (PosDash != std::string::npos))
      {
        //printf("PosDash %I64d: %s\n", PosDash, _rUri_S.substr(PosDash).c_str());
        Path_S = _rUri_S.substr(PosSlash, PosDash - PosSlash);
        mFragment_S = _rUri_S.substr(PosDash + 1);
        Rts_E = BOF_ERR_NO_ERROR;
      }
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Scheme_S = _rUri_S.substr(0, PosSlash);
        //printf("Scheme_S %s\n", Scheme_S.c_str());

        Rts_E = Bof_SplitIpAddress(Scheme_S, mScheme_X, mIpAddress_X);
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          mPath = BofPath(Path_S);
          if (mPath.IsValid())
          {
            if (Query_S != "")
            {
              Query_S = S_UrlDecode(Query_S);
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
                if (mQueryCollection.find(Key_S) == mQueryCollection.end())
                {
                  mQueryCollection[Key_S] = Val_S;
                }
                else
                {
                  Rts_E = BOF_ERR_EKEYREVOKED;
                  break;
                }
              }
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
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    mValid_B = true;
  }

  return (Rts_E);
}


bool BofUri::IsForbiddenChar(const std::string &_rUri_S)
{
  return (Bof_StringIsPresent(_rUri_S, "<>:\"\\|?*\a\f\n\r\t\v"));
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
END_BOF_NAMESPACE()