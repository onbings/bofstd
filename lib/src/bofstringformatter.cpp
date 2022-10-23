/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements some specific string formatting function
 *
 * Name:        bofstringformatter.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         None
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#include <bofstd/bofstringformatter.h>

BEGIN_BOF_NAMESPACE()

char *Bof_FastSnprintf(char *_pBuffer_c, uint32_t _MaxBufferSize_U32, const char *_pFormat_c, ...)
{
  char *pRts_c = nullptr;
  std::va_list Arg;
  int Size_i;

  if ((_pBuffer_c) && (_MaxBufferSize_U32) && (_pFormat_c))
  {
    va_start(Arg, _pFormat_c);
    Size_i = vsnprintf(_pBuffer_c, _MaxBufferSize_U32 - 1, _pFormat_c, Arg);
    if (Size_i >= 0)
    {
      _pBuffer_c[Size_i] = 0;
      pRts_c = _pBuffer_c;
    }
    va_end(Arg);
  }
  return pRts_c;
}


END_BOF_NAMESPACE()