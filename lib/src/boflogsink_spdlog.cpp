/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the logger channel interface based on spdlog
 *
 * Name:        boflogsink_spdlog.cpp
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
#include <bofstd/boflogger.h>
#include "boflogsink_spdlog.h"

#include <spdlog/details/fmt_helper.h>

BEGIN_BOF_NAMESPACE()

bool CheckIfLimitedSizeIsReached(spdlog::details::file_helper &_rFileHelper, uint32_t _LogLineSiwze_U32, uint32_t &_rCrtFileSizeInByte_U32, uint32_t _MaxLogSizeInByte_U32)
{
  bool Rts_B = false;
  uint32_t LastSize_U32;

  _rCrtFileSizeInByte_U32 = _rCrtFileSizeInByte_U32 + _LogLineSiwze_U32;
  if (_rCrtFileSizeInByte_U32 >= _MaxLogSizeInByte_U32) //>= to be sure that when we reach the limit a line of '-' (1 up to ...) is present in the file
  {
    LastSize_U32 = _MaxLogSizeInByte_U32 - (_rCrtFileSizeInByte_U32 - _LogLineSiwze_U32);
    BOF_ASSERT(LastSize_U32 < _MaxLogSizeInByte_U32);
    if (LastSize_U32 < _MaxLogSizeInByte_U32)
    {
#if 0 // spdlog 1.3.1
      fmt::memory_buffer last;
      fmt::writer writer(last);
      for (uint32_t i_U32 = 0; i_U32 < LastSize_U32; i_U32++)
      {
        writer.write('-');
      }
#else
      std::string Pad_S = "-";
      spdlog::memory_buf_t last;
      for (uint32_t i_U32 = 0; i_U32 < LastSize_U32; i_U32++)
      {
        last.append(Pad_S.data(), Pad_S.data() + Pad_S.size());
      }
#endif
      _rFileHelper.write(last);
    }
    _rCrtFileSizeInByte_U32 = _MaxLogSizeInByte_U32;
    Rts_B = true;
  }
  return Rts_B;
}

END_BOF_NAMESPACE()