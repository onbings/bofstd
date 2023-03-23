/*
 * Copyright (c) 2013-2033, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bofpath module of the bofstd library
 *
 * Name:        bofpath.h
 * Author:      Bernard HARMEL: onbings@gmail.com
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         None
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#pragma once

#include <bofstd/bofstd.h>

#include <string>
#include <vector>

BEGIN_BOF_NAMESPACE()
/// @brief This class implements a Path parser. A path is a full pathname, a directory or a file.
/// Relative path or directory or base filename must be combined with a base path to produce a full path or directory
/// This step can be made with a constructor  or the combine method
/// PathName (C:/tmp/subdir/bha.txt) is the concatenation of a DirectoryName (C:/tmp/subdir/) and an optional FileName (bha.txt).
/// The Pathname separator is alwways /. A DirectoryName always ends with the / separator
/// The FileName or Pathname extension is the dot separated string at the very end of the path or file name excluding the dot (txt)
/// Directory name can contains .
/// Extension cannot contains . or the Pathname separator
/// All \ present in a path are converted into / (unix style). A converter to Windows path delimiter (\) is included (FullPathName)
/// No wildcard character (* or ?) at any place in the path
/// Forbidden character: <,>,:,",\ (: can only appear on index 2 of a windows pathname such as C:/... In this case the disk id (C:) is
/// extracted from the path and stored as mDiskName_S. For linusx mDiskName_S is "";
/// Directory . and .. are processed and reduced in the full pathname
class BOFSTD_EXPORT BofPath
{
private:
  std::string mDiskName_S = "";                 /*! Contains the disk name (C:,D:,.. for windows or "" for linux */
  std::string mDirectoryName_S = "";            /*! Contains the directory name */
  std::string mFileNameWithExtension_S = "";    /*! Contains the filename with the extension */
  std::string mFileNameWithoutExtension_S = ""; /*! Contains the filename without the extension */
  std::string mExtension_S = "";                /*! Contains the filename extension */
  bool mValid_B = false;                        /*! true if the pathname has a good synthax */
  std::vector<std::string> mSubdirCollection;
  std::string mCurrentDirectoryName_S = ""; /*! Contains the current directory name used to compute relative path */

public:
  /// @brief Initializes a new instance of the Path class.
  /// @remarks Default constructor.
  BofPath(bool _PureFilename_B = false);

  /// @brief Initializes a new instance of the Path class.
  /// @param _pPath_c A pointer to a char array containing the path
  /// @param _PureFilename_B True if we have only the file part of a path name. If false and only the file part is provided the current working dir is automatically pre-appended to the filename.
  /// @remarks None.
  BofPath(const char *_pPath_c, bool _PureFilename_B = false);

  /// @brief Initializes a new instance of the Path class.
  /// @param _rPath_S A string containing the path
  /// @param _PureFilename_B True if we have only the file part of a path name. If false and only the file part is provided the current working dir is automatically pre-appended to the filename.
  /// @remarks None.
  BofPath(const std::string &_rPath_S, bool _PureFilename_B = false);

  /// @brief Initializes a new instance of the Path class.
  /// @param _rDirectory_S A string containing the directory
  /// @param _rFile_S A string containing the file
  /// @remarks None.
  BofPath(const std::string &_rDirectory_S, const std::string &_rFile_S);

  /// @brief Initializes a new instance of the Path class.
  /// @param _rOther_O A reference to another Path class instance.
  /// @remarks Copy constructor.
  BofPath(const BofPath &_rOther_O);

  /// @brief Initializes a new instance of the Path class.
  /// @param _rrOther_O A rvalue reference to another Path class instance.
  /// @remarks Move constructor.
  BofPath(BofPath &&_rrOther_O);

  /// @brief Initializes a new instance of the Path class.
  /// @param _rOther_O A reference to another Path class instance.
  /// @remarks  Copy assignment operator.
  BofPath &operator=(const BofPath &_rOther_O); // Copy assignment operator
  /// @brief Create a new path by assigning a new string value.
  /// @param _rNewPath_S A reference to a string containing the new path.
  /// @return a modified class instance
  /// @remarks Assignment operator.
  BofPath &operator=(const std::string &_rNewPath_S);

  /// @brief Create a new path by assigning a new char array value.
  /// @param _pNewPath_c A pointer to a char array containing the new path.
  /// @return a modified class instance
  /// @remarks Assignment operator.
  BofPath &operator=(const char *_pNewPath_c);

  /// @brief Initializes a new instance of the Path class.
  /// @param _rrOther_O A rvalue reference to another Path class instance.
  /// @remarks Move constructor.
  BofPath &operator=(BofPath &&_rrOther_O);

  /// @brief Release all class instance resources.
  /// @remarks Virtual destructor.
  virtual ~BofPath();

  /// @brief Create a new path by concataining this path (must be a directory) with a relative path or file
  /// @param _rRelativePath_S : Specifies the directory name (must be terminated by a / or \)  or the filename to add
  /// @return a new Bofpath instance
  /// @remarks Equal operator.
  BofPath operator+(const std::string &_rRelativePath_S) const;

  /// @brief Compares two object to check for equality
  /// @param _rOther_O A reference to another Path class instance.
  /// @return true if both object are equal
  /// @remarks Equal operator.
  bool operator==(const BofPath &_rOther_O) const;

  /// @brief Compares two object to check for difference
  /// @param _rOther_O A reference to another Path class instance.
  /// @return true if the objects are different (not equal)
  /// @remarks Different operator.
  bool operator!=(const BofPath &_rOther_O) const;

  /// @brief Returns the directory name.
  /// @param _WithDiskName_B : true if the directory must be preceeded by the disk name (Only for wiondows)
  /// @param _Windows_B : true if the directory must be returned with windows path separator (\) or unix one (/)
  /// @return A std::string containing the directory name.
  /// @remarks None
  std::string DirectoryName(bool _WithDiskName_B, bool _Windows_B) const;

  /// @brief Returns the number of sub directory in path.
  /// @return The number of sub directory in path.
  /// @remarks None
  uint32_t NumberOfSubDirectory() const;

  /// @brief Returns a subdirectory from the path
  /// @param _Level_U32 : specifies the sub directory level to return
  /// @param _Windows_B : true if the directory must be returned with windows path separator (\) or unix one (/)
  /// @return A std::string containing the sub directory name.
  /// @remarks None
  std::string SubDirectory(uint32_t _Level_U32, bool _Windows_B) const;

  /// @brief Returns the filename without its extension.
  /// @return A std::string containing the filename without its extension.
  /// @remarks None
  const std::string &FileNameWithoutExtension() const;

  /// @brief Sets the filename without its extension.
  /// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
  /// @remarks It cannot contains . nor slash or backslash
  BOFERR FileNameWithoutExtension(const std::string &_rFileNameWithoutExtension_S);

  /// @brief Returns the filename extension.
  /// @return A std::string containing the filename extension.
  /// @remarks None
  const std::string &Extension() const;

  /// @brief sets the filename extension.
  /// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
  /// @remarks It cannot contains . nor slash or backslash
  BOFERR Extension(const std::string &_rExtension_S);

  /// @brief Returns the full pathname as a string.
  /// @param _Windows_B : true if the full pathname must be returned with windows path separator (\) or unix one (/)
  /// @return A std::string containing the full pathname.
  /// @remarks None
  std::string FullPathName(bool _Windows_B) const;
  std::string FullPathNameWithoutExtension(bool _Windows_B) const;

  /// @brief Returns the full pathname as a string (std::wstring).
  /// @param _Windows_B : true if the full pathname must be returned with windows path separator (\) or unix one (/)
  /// @return A std::string containing the full pathname.
  /// @remarks Shortcut to FullPathName.
  std::string ToString(bool _Windows_B = false) const;

  /// @brief Returns the full pathname as a wide string (std::wstring).
  /// @param _Windows_B : true if the full pathname must be returned with windows path separator (\) or unix one (/)
  /// @return A std::string containing the full pathname.
  /// @remarks None
  std::wstring FullWidePathName(bool _Windows_B) const;

  /// @brief Build an absolute file path or directory by concataining this path (must be a directory) with a relative path or file
  /// @param _rRelativePath_S : Specifies the directory name (must be terminated by a / or \)  or the filename to add
  /// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
  /// @remarks None
  BOFERR Combine(const std::string &_rRelativePath_S);

  /// @brief Returns the filename with its extension.
  /// @return A std::string containing the filename with its extension.
  /// @remarks None
  const std::string &FileNameWithExtension() const;
  /// @brief Sets the filename with its extension.
  /// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
  /// @remarks It cannot contains slash or backslash
  BOFERR FileNameWithExtension(const std::string &_rFileNameWithExtension_S);

  /// @brief Return true if the path is a directory.
  /// @return true if the path is a directory.
  /// @remarks None
  bool IsDirectory() const;

  /// @brief Return true if the path is a file.
  /// @return true if the path is a file.
  /// @remarks None
  bool IsFile() const;

  /// @brief Return true if the file or directory exist on the disk.
  /// @return true if the path exist.
  /// @remarks None
  bool IsExist() const;

  /// @brief Return true if the file or directory has a good synthax (ex: bad /tmp/../..)
  /// @return true if the path exist.
  /// @remarks None
  bool IsValid() const;

  /// @brief Return true if the path is equal to ""
  /// @return true if the path is empty.
  /// @remarks None
  bool IsEmpty() const;

  /// @brief Returns the current directory name.
  /// @return A std::string containing the current directory name.
  /// @remarks It is used to compute path from relative string
  std::string CurrentDirectoryName() const;

  /// @brief sets the current directory name.
  /// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
  /// @remarks None
  BOFERR CurrentDirectoryName(const std::string &_rNewCurrentDirectoryName_S);

private:
  /// @cond

  /// @brief Initializes the class instance members.
  /// @param _PureFilename_B True if we have only the file part of a path name. If false and only the file part is provided the current working dir is automatically pre-appended to the filename.
  /// @param _rPath_S A string containing the path
  /// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
  /// @remarks None
  BOFERR InitPathField(bool _PureFilename_B, const std::string &_rPath_S);

  /// @brief Transform the raw path input string into a normalized one. Normalization includes:
  /// - Normalization of directory separator
  /// - Remove .. and . optional character in input
  /// - Track input string format error such as double . in extension,...
  /// @param _PureFilename_B True if we have only the file part of a path name. If false and only the file part is provided the current working dir is automatically pre-appended to the filename.
  /// @param _rRawPath_S A string containing the 'raw' path
  /// @param _rNormalizedPath_S A string containing the 'normalized' path computed from the 'raw' one.
  /// @param _rDiskName_S A string containing the disk name in the path .
  /// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
  /// @remarks None
  BOFERR Normalize(bool _PureFilename_B, const std::string &_rRawPath_S, std::string &_rNormalizedPath_S, std::string &_rDiskName_S);

  /// @brief Check if a path is a windows one. A full windows path begins with a letter (disk) followed by a : and then a /
  /// @param _rPath_S : Specifies the path to check
  /// @return true if the path is a windows one
  /// @remarks None
  bool IsWindowsDiskPath(const std::string &_rPath_S);

  /// @brief Check if a path contains forbidden char such as "<>:\"\\|?*\a\f\n\r\t\v"
  /// @param _rPath_S : Specifies the path to check
  /// @return true if the path contains forbidden char
  /// @remarks None
  bool IsForbiddenChar(const std::string &_rPath_S);
  /// @endcond

  bool IsDirectoryDelimiterPresent(const std::string &_rPath_S) const;
};
END_BOF_NAMESPACE()