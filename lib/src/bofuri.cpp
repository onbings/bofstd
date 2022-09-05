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


#include <cctype>
#include <bofstd/bofpath.h>
#include <bofstd/bofsystem.h>
#include "bofstd/bofstring.h"
#include "bofstd/boffs.h"

#if defined (_WIN32)
#include <windows.h>
#endif






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
	  mValid_B=false;
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
	InitUriField(ToString(';'));
}

BofUri::BofUri(const BofUri &_rOther_O)
{
	InitUriField(_rOther_O.ToString(';'));
}

BofUri::BofUri(BofUri &&_rrOther_O)
{
	// Pilfer other�s resource
	InitUriField(_rrOther_O.ToString(';'));
	// Reset other
	_rrOther_O.InitUriField("");
}

BofUri &BofUri::operator=(const BofUri &_rOther_O)
{
	InitUriField(_rOther_O.ToString(';'));
	return *this;
}

BofUri &BofUri::operator=(BofUri &&_rrOther_O)
{
	if (this != &_rrOther_O)
	{
		// Release the current object�s resources
		// Pilfer other�s resource
		InitUriField(_rrOther_O.ToString(';'));
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
    mValid_B=false;
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
	return mScheme_X;
}
const BofPath &BofUri::Path(std::string &_rPath_S) const
{
	return mPath;
}
const std::map<std::string, std::string> &BofUri::QueryCollection() const
{
	return mQueryCollection;
}
const std::string &BofUri::Fragment() const
{
	return mFragment_S;
}
std::string	BofUri::ToString(char _QueryDelimiter_c) const
{
	return "BofUri::ToString";
}

BOFERR BofUri::InitUriField(const std::string &_rUri_S)
{
	BOFERR Rts_E;
	// storage://10.129.170.18:11000/file/still/pic1.jpg?raster=1920x1080;order=;rem=this is a pic#10:11:12:14

	Rts_E = Bof_SplitIpAddress(_rUri_S, mScheme_X);
	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		mValid_B = false;
	}
	return (Rts_E);
}


bool BofUri::IsForbiddenChar(const std::string &_rUri_S)
{
	return (Bof_StringIsPresent(_rUri_S, "<>:\"\\|?*\a\f\n\r\t\v"));
}

END_BOF_NAMESPACE()