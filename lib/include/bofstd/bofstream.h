/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines various stream class def.
 *
 * Name:        bofstream.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Aug 28 2022  BHA : Initial release
 */
#pragma once

#include <bofstd/bofstd.h>

#include <sstream>
#include <streambuf>

BEGIN_BOF_NAMESPACE()

class BofMemoryStream : public std::streambuf
{
public:
  BofMemoryStream(char const *base, size_t size)
  {
    char *p(const_cast<char *>(base));
    this->setg(p, p, p + size);
  }
};
class BofIMemoryStream : virtual BofMemoryStream, public std::istream
{
public:
  BofIMemoryStream(size_t size, char const *base) : BofMemoryStream(base, size), std::istream(static_cast<std::streambuf *>(this))
  {
  }
};

END_BOF_NAMESPACE()