/*
 * Copyright (c) 2013-2033, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bofpath module of the bofstd library
 *
 * Name:        bofpath.cpp
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

/*** Include files ***********************************************************/
#include <cctype>
#include <bofstd/bofpath.h>
#include <bofstd/bofsystem.h>
#include "bofstd/bofstring.h"
#include "bofstd/boffs.h"

#if defined (_WIN32)
#include <windows.h>
#endif
BEGIN_BOF_NAMESPACE()

/*** Global variables ********************************************************/

/*** Definitions *************************************************************/

/*** Class *******************************************************************/

BofPath::BofPath()
{
	InitPathField("");
}

BofPath::BofPath(const char *_pPath_c)
{
	std::string Path_S;

	if (_pPath_c)
	{
		Path_S = _pPath_c;
		InitPathField(Path_S);
	}
	else
	{
	  mValid_B=false;
		//InitPathField("");
	}
}

BofPath::BofPath(const std::string &_rPath_S)
{
	InitPathField(_rPath_S);
}

BofPath::BofPath(const std::string &_rDirectory_S, const std::string &_rFile_S)
{
	InitPathField(_rDirectory_S + _rFile_S);
}

BofPath::BofPath(const BofPath &_rOther_O)
{
	InitPathField(_rOther_O.FullPathName(false));
}

BofPath::BofPath(BofPath &&_rrOther_O)
{
	// Pilfer other�s resource
	InitPathField(_rrOther_O.FullPathName(false));
	// Reset other
	_rrOther_O.InitPathField("");
}

BofPath &BofPath::operator=(const BofPath &_rOther_O)
{
	InitPathField(_rOther_O.FullPathName(false));
	return *this;
}

BofPath &BofPath::operator=(BofPath &&_rrOther_O)
{
	if (this != &_rrOther_O)
	{
		// Release the current object�s resources
		// Pilfer other�s resource
		InitPathField(_rrOther_O.FullPathName(false));
		// Reset other
		_rrOther_O.InitPathField("");
	}
	return *this;
}

BofPath &BofPath::operator=(const char *_pNewPath_c)
{
	std::string Path_S;

	if (_pNewPath_c)
	{
		Path_S = _pNewPath_c;
		InitPathField(Path_S);
	}
	else
	{
		//InitPathField("");
    mValid_B=false;
	}
	return *this;
}

BofPath &BofPath::operator=(const std::string &_rNewPath_S)
{
	InitPathField(_rNewPath_S);
	return *this;
}

BofPath::~BofPath()
{}


bool BofPath::operator==(const BofPath &_rOther_O) const
{
	return (mDiskName_S == _rOther_O.mDiskName_S) && (mDirectoryName_S == _rOther_O.mDirectoryName_S) && (mFileNameWithExtension_S == _rOther_O.mFileNameWithExtension_S);
}

bool BofPath::operator!=(const BofPath &_rOther_O) const
{
	return !(*this == _rOther_O);
}

std::string BofPath::DirectoryName(bool _Windows_B) const
{
	std::string Rts_S;

	Rts_S = mDiskName_S + mDirectoryName_S;
	if (_Windows_B)
	{
		Rts_S = Bof_StringReplace(Rts_S, "/", '\\');
	}
	return Rts_S;
}

const std::string &BofPath::FileNameWithoutExtension() const
{
	return mFileNameWithoutExtension_S;
}

BOFERR BofPath::FileNameWithoutExtension(const std::string &_rFileNameWithoutExtension_S)
{
	BOFERR Rts_E = BOF_ERR_EINVAL;
	std::string::size_type ExtensionPos, DelimiterPos;

	ExtensionPos = _rFileNameWithoutExtension_S.find('.');
	if (ExtensionPos == std::string::npos)
	{
		DelimiterPos = _rFileNameWithoutExtension_S.find("/\\");
		if (DelimiterPos == std::string::npos)
		{
			Rts_E = BOF_ERR_NO_ERROR;
			mFileNameWithoutExtension_S = _rFileNameWithoutExtension_S;
		}
	}
	return Rts_E;
}

const std::string &BofPath::Extension() const
{
	return mExtension_S;
}

BOFERR BofPath::Extension(const std::string &_rExtension_S)
{
	BOFERR Rts_E = BOF_ERR_EINVAL;
	std::string::size_type ExtensionPos, DelimiterPos;

	ExtensionPos = _rExtension_S.find('.');
	if (ExtensionPos == std::string::npos)
	{
		DelimiterPos = _rExtension_S.find("/\\");
		if (DelimiterPos == std::string::npos)
		{
			Rts_E = BOF_ERR_NO_ERROR;
			mFileNameWithoutExtension_S = _rExtension_S;
		}
	}
	return Rts_E;
}

std::string BofPath::FullPathName(bool _Windows_B) const
{
	std::string Rts_S;

	Rts_S = DirectoryName(_Windows_B);
	if (IsFile())
	{
		Rts_S += mFileNameWithoutExtension_S;
		if (!mExtension_S.empty())
		{
			Rts_S += ('.' + mExtension_S);
		}
	}
	return Rts_S;
}

std::wstring BofPath::FullWidePathName(bool _Windows_B) const
{
	std::wstring Rts_WS;
	std::string Fn_S = FullPathName(_Windows_B);
	Rts_WS = Bof_Utf8ToUtf16(Fn_S);

	return Rts_WS;
}
BofPath BofPath::operator+(const std::string &_rRelativePath_S) const
{
	BofPath Rts_O = *this;     // Make a copy of myself.  Same as BofPath Rts_O(*this);
	Rts_O.Combine(_rRelativePath_S);
	return Rts_O;
}

BOFERR BofPath::Combine(const std::string &_rRelativePath_S)
{
  BOFERR Rts_E; // = BOF_ERR_BAD_TYPE;

	//no as we want to be able to Combine file with '\' to get a directory if (IsDirectory())
	{
		Rts_E = InitPathField(FullPathName(false) + _rRelativePath_S);
	}
	return Rts_E;
}

const std::string &BofPath::FileNameWithExtension() const
{
	return mFileNameWithExtension_S;
}


bool BofPath::IsDirectory() const
{
	return ((!mDirectoryName_S.empty()) && (mFileNameWithExtension_S.empty())); // "" is a file !
}

bool BofPath::IsFile() const
{
	return (!IsDirectory());
}

bool BofPath::IsExist() const
{
	bool Rts_B = false;
	BOF_FILE_TYPE FileType_E;

	FileType_E = Bof_GetFileType(*this);
	Rts_B = (FileType_E != BOF_FILE_TYPE::BOF_FILE_DONT_EXIST);

	return (Rts_B);
}

bool BofPath::IsValid() const
{
	return (mValid_B);
}

bool BofPath::IsEmpty() const
{
	return (FullPathName(false) == "");
}

std::string BofPath::CurrentDirectoryName() const
{
	return mCurrentDirectoryName_S;
}

BOFERR BofPath::CurrentDirectoryName(const std::string &_rNewCurrentDirectoryName_S)
{
	BOFERR Rts_E;
	std::string ThePath_S, DiskName_S;
	char LastChar_c;

	Rts_E = Normalize(_rNewCurrentDirectoryName_S, ThePath_S, DiskName_S);
	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		Rts_E = BOF_ERR_FORMAT;
		if (!ThePath_S.empty())
		{
			LastChar_c = *ThePath_S.rbegin();
			// Path is not empty, check if it is a pure directory
			if (LastChar_c == '/')
			{
				mCurrentDirectoryName_S = ThePath_S;
				Rts_E = BOF_ERR_NO_ERROR;
			}
		}
	}
	return Rts_E;
}

BOFERR BofPath::InitPathField(const std::string &_rPath_S)
{
	BOFERR Rts_E;
	char LastChar_c;
	std::string ThePath_S;
	std::string::size_type ExtensionPos, FilenamePos;

  mFileNameWithExtension_S = "";
  mFileNameWithoutExtension_S = "";
  mExtension_S = "";
  mCurrentDirectoryName_S = "";
  mDirectoryName_S="";
  mDiskName_S="";
	Rts_E = Normalize(_rPath_S, ThePath_S, mDiskName_S);
  if (Rts_E == BOF_ERR_EMPTY) //for "" filename
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		mValid_B = true;

		if (!ThePath_S.empty())
		{
			LastChar_c = *ThePath_S.rbegin();
			// Path is not empty, check if it is a pure directory
			if (LastChar_c == '/')
			{
				mDirectoryName_S = ThePath_S;
			}
			else
			{
				// Path is not a pure directory, extract file and extension value
				FilenamePos = ThePath_S.rfind('/');
				if (FilenamePos == std::string::npos)
				{
					Rts_E = BOF_ERR_INTERNAL;
					mFileNameWithExtension_S = ThePath_S;
					// pure filename: not possible the normalize step should have returned an error
					BOF_ASSERT(0);
				}
				else
				{
					// Directory+filename
					mDirectoryName_S = ThePath_S.substr(0, FilenamePos + 1);
					mFileNameWithExtension_S = ThePath_S.substr(FilenamePos + 1);
					if (*mFileNameWithExtension_S.rbegin() == '.')                            // File.
					{
						mFileNameWithExtension_S.pop_back();
					}
				}
				// Process extension
				ExtensionPos = ThePath_S.rfind('.');
				if ((ExtensionPos == std::string::npos) || (ExtensionPos <= FilenamePos)) // . in subdir
				{
					mFileNameWithoutExtension_S = mFileNameWithExtension_S;
				}
				else
				{
					mExtension_S = ThePath_S.substr(ExtensionPos + 1);
					mFileNameWithoutExtension_S = ThePath_S.substr(FilenamePos + 1, ExtensionPos - FilenamePos - 1);
				}
			}
		}
	}
	else
	{
		mValid_B = false;
	}
	return (Rts_E);
}

bool BofPath::IsWindowsDiskPath(const std::string &_rPath_S)
{
	return ((std::isalpha(_rPath_S[0])) && (_rPath_S[1] == ':') && ((_rPath_S[2] == '/') || (_rPath_S[2] == '\\')));
}

bool BofPath::IsForbiddenChar(const std::string &_rPath_S)
{
	return (Bof_StringIsPresent(_rPath_S, "<>:\"\\|?*\a\f\n\r\t\v"));
}

BOFERR BofPath::Normalize(const std::string &_rRawPath_S, std::string &_rNormalizedPath_S, std::string &_rDiskName_S)
{
	BOFERR Rts_E;
	std::vector<std::string> ListOfDir_S;
	std::string Pwd_S;
  std::string::size_type SlashDelimiterPos, SlashPrevDelimiterPos;

	_rDiskName_S = "";
	// Remove bad char on the left and rigth side
//Should be error
  if (Bof_StringIsPresent(_rRawPath_S,"\a\f\n\r\t\v"))
  {
    Rts_E=BOF_ERR_EINVAL;
  }
  else
  {
    _rNormalizedPath_S = Bof_StringTrim(_rRawPath_S);
    if (_rNormalizedPath_S.empty())
    {
      _rNormalizedPath_S = "";
      Rts_E              = BOF_ERR_EMPTY;
    }
    else
    {
      Rts_E = BOF_ERR_NO_ERROR;

      // Replace \ by /
      _rNormalizedPath_S = Bof_StringReplace(_rNormalizedPath_S, "\\", '/');
			SlashDelimiterPos = _rNormalizedPath_S.find('/');
			Rts_E = Bof_GetCurrentDirectory(Pwd_S);
			Pwd_S = Bof_StringReplace(Pwd_S, "\\", '/');
			if (Rts_E == BOF_ERR_NO_ERROR)
			{
				if (SlashDelimiterPos == std::string::npos)
				{
					_rNormalizedPath_S = Pwd_S + _rNormalizedPath_S;
					//_rNormalizedPath_S = Bof_StringReplace(_rNormalizedPath_S, "\\", '/');
				}
				else
				{
#if 1
					if ((!_rNormalizedPath_S.empty()) && (_rNormalizedPath_S[0] == '.'))
					{
						while ((Rts_E == BOF_ERR_NO_ERROR) && (!_rNormalizedPath_S.empty()) && (_rNormalizedPath_S[0] == '.'))
						{
							if (_rNormalizedPath_S[1] == '/')
							{
								_rNormalizedPath_S = _rNormalizedPath_S.substr(2);
							}
							else if (_rNormalizedPath_S[1] == '.')
							{
								Rts_E = BOF_ERR_TOO_SMALL;
								if (Pwd_S.size() > 2)
								{
									SlashPrevDelimiterPos = Pwd_S.rfind('/', Pwd_S.size() - 2);
									if (SlashPrevDelimiterPos == std::string::npos)
									{
										Rts_E = BOF_ERR_TOO_BIG;
									}
									else
									{
										Pwd_S = Pwd_S.substr(0, SlashPrevDelimiterPos + 1);
										_rNormalizedPath_S = _rNormalizedPath_S.substr(3);
										Rts_E = BOF_ERR_NO_ERROR;
									}
								}
							}
						}
						if (Rts_E == BOF_ERR_NO_ERROR)
						{
							_rNormalizedPath_S = Pwd_S + _rNormalizedPath_S;
						}
					}
#else
					if ((_rNormalizedPath_S[0] == '.') && (_rNormalizedPath_S[1] == '/'))
					{
						_rNormalizedPath_S = Pwd_S + _rNormalizedPath_S.substr(2);
						//_rNormalizedPath_S = Bof_StringReplace(_rNormalizedPath_S, "\\", '/');
					}
					else if ((_rNormalizedPath_S[0] == '.') && (_rNormalizedPath_S[1] == '.'))
					{
						Rts_E = BOF_ERR_TOO_SMALL;
						if (Pwd_S.size() > 2)
						{
							SlashPrevDelimiterPos = Pwd_S.rfind('/', Pwd_S.size() - 2);
							if (SlashPrevDelimiterPos == std::string::npos)
							{
								Rts_E = BOF_ERR_TOO_BIG;
							}
							else
							{
								Pwd_S = Pwd_S.substr(0, SlashPrevDelimiterPos + 1);
								_rNormalizedPath_S = Pwd_S + _rNormalizedPath_S.substr(3);
								Rts_E = BOF_ERR_NO_ERROR;
								//_rNormalizedPath_S = Bof_StringReplace(_rNormalizedPath_S, "\\", '/');
							}
						}
					}
#endif
				}
      }
    }
  }
	if (Rts_E == BOF_ERR_NO_ERROR)
	{
		// Extract disk name for windows path
		if (IsWindowsDiskPath(_rNormalizedPath_S))
		{
			_rDiskName_S = _rNormalizedPath_S.substr(0, 2);
			_rNormalizedPath_S = _rNormalizedPath_S.substr(2);
		}
		if (_rNormalizedPath_S[0] != '/')	// data/dir/file without ./ or ../
		{
			_rNormalizedPath_S = Pwd_S + _rNormalizedPath_S;
			if (IsWindowsDiskPath(_rNormalizedPath_S))
			{
				_rDiskName_S = _rNormalizedPath_S.substr(0, 2);
				_rNormalizedPath_S = _rNormalizedPath_S.substr(2);
			}
		}
// Reject relative path
		if (_rNormalizedPath_S[0] != '/')
		{
			if (mCurrentDirectoryName_S != "")
			{
				_rNormalizedPath_S = mCurrentDirectoryName_S + _rNormalizedPath_S;
				if (IsWindowsDiskPath(_rNormalizedPath_S))
				{
					_rDiskName_S = _rNormalizedPath_S.substr(0, 2);
					_rNormalizedPath_S = _rNormalizedPath_S.substr(2);
				}
			}
			else
			{
				Rts_E = BOF_ERR_FORMAT;
			}
		}
//		else
		if (Rts_E == BOF_ERR_NO_ERROR)
		{
			// Check for forbidden char in path
			if (IsForbiddenChar(_rNormalizedPath_S))
			{
				Rts_E = BOF_ERR_EINVAL;
			}
			else
			{
				// Reduce path (. and .. processing)
				if (_rNormalizedPath_S == "/")
				{}
				else
				{
// Track ./././
#if 0
					fails with ./babar
					std::string::iterator it = _rNormalizedPath_S.begin();
					++it;                        // Skip first char
					do
					{
						it = std::adjacent_find(it, _rNormalizedPath_S.end(), [_rNormalizedPath_S](char _Char1_c, char _Char2_c) { printf("%s %c %c\r\n", _rNormalizedPath_S.c_str(),_Char1_c, _Char2_c);  return ((_Char1_c == '.') && (_Char2_c == '/')); });
						if (it != _rNormalizedPath_S.end() )
						{
							if (*(it - 1) == '/')
							{
								Rts_E = BOF_ERR_FORMAT;
								break;
							}
							++it;
							// need a test against _rNormalizedPath_S.end() to be sure   -> ++it;  ->skip it perf are the same (-;
						}
					}
					while (it != _rNormalizedPath_S.end() );
#endif
					// Track //
					if (Rts_E == BOF_ERR_NO_ERROR)
					{
						std::string::iterator itslash = std::adjacent_find(_rNormalizedPath_S.begin(), _rNormalizedPath_S.end(),
						                                                   [](char _Char1_c, char _Char2_c) { return ((_Char1_c == '/') && (_Char2_c == '/')); });
						if (itslash != _rNormalizedPath_S.end())
						{
							Rts_E = BOF_ERR_FORMAT;
						}
					}

					if (Rts_E == BOF_ERR_NO_ERROR)
					{
						ListOfDir_S = Bof_StringSplit(_rNormalizedPath_S, "/");
						if (ListOfDir_S.size())
						{
							// Remove .
							ListOfDir_S.erase(std::remove_if(ListOfDir_S.begin(), ListOfDir_S.end(), [](const std::string &_rStr_S) { return _rStr_S == "."; }), ListOfDir_S.end());

							if (ListOfDir_S.size() > 1)
							{
								// Process ..
								std::vector<std::string>::iterator itdot = ListOfDir_S.begin();
								do
								{
									itdot = std::adjacent_find(itdot, ListOfDir_S.end(), [](const std::string &_rStr1_S, const std::string &_rStr2_S) { return ((_rStr1_S != "..") && (_rStr2_S == "..")); });
									if (itdot != ListOfDir_S.end())
									{
										itdot = ListOfDir_S.erase(itdot, itdot + 2);
										if (itdot == ListOfDir_S.begin())
										{
											///tmp/../..
											Rts_E = BOF_ERR_UNDERRUN;
											break;
										}
										else
										{
											--itdot;
										}
									}
								} while (itdot != ListOfDir_S.end());
							}
						}
					}
					if (Rts_E == BOF_ERR_NO_ERROR)
					{
						// Special case such as C:/.. or V:/tmp/......
						for (std::string &rStr_S : ListOfDir_S)
						{
							if (Bof_StringIsAllTheSameChar(rStr_S, '.'))
							{
								Rts_E = BOF_ERR_INVALID_STATE;
								break;
							}
						}
					}
					if (Rts_E == BOF_ERR_NO_ERROR)
					{
						_rNormalizedPath_S = "/" + Bof_StringJoin(ListOfDir_S, "/");
						BOF_ASSERT(_rNormalizedPath_S[0] == '/');
						if (_rNormalizedPath_S[1] == '/')
						{
							_rNormalizedPath_S = _rNormalizedPath_S.erase(0, 1);
						}
						std::string::const_iterator itslash = std::adjacent_find(_rNormalizedPath_S.begin(), _rNormalizedPath_S.end(),
						                                                         [](const char _Char1_c, const char _Char2_c) { return ((_Char1_c == '/') && (_Char2_c == '/')); });
						if (itslash != _rNormalizedPath_S.end())
						{
							Rts_E = BOF_ERR_INVALID_STATE;
						}
					}
				}
			}
		}
	}
	return Rts_E;
}
END_BOF_NAMESPACE()