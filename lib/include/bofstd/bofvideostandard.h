/*
 * Copyright (c) 2015-2025, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines video standard class manipulation
 *
 * Name:        bofvideostandard.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  May 26 2020  BHA : Initial release
 */

#pragma once

#include <bofstd/bofstd.h>
#include <bofstd/bofrational.h>

BEGIN_BOF_NAMESPACE()

#define BOF_VIDEO_STANDARD_ID(__w, __h, __fps, __type) \
    ((uint32_t)((__w - 1) << 20) | (uint32_t)((__h - 1) << 8) | (uint32_t)((__fps - 1) << 2) | \
     ((__type == 'i') ? 1 : (__type == 'p') ? 2 : (__type == 's') ? 3 : 0))

typedef uint32_t VideoStandardId;

static const VideoStandardId  DefaultVideoStandard = BOF_VIDEO_STANDARD_ID(1920,  1080, 50, 'p');

class BOFSTD_EXPORT BofVideoStandard
{
public:
  BofVideoStandard() : mIndex_i(-1)
  {
  }
  BofVideoStandard(const BofVideoStandard &standard) : mIndex_i(standard.mIndex_i)
  {
  }
  BofVideoStandard(const VideoStandardId standard)
  {
    mIndex_i= S_FindIndex(standard);
  }
  BofVideoStandard(const char *standard)
  {
    mIndex_i= S_FindIndex(standard);
  }
  BofVideoStandard(const std::string& standard)
  {
    mIndex_i= S_FindIndex(standard.c_str());
  }
  BofVideoStandard(const int numCols, const int numRows, const int fps, const char type)
  {
    mIndex_i= S_FindIndex(numCols, numRows, fps, type);
  }
  BofVideoStandard& operator=(const BofVideoStandard& standard)
  {
    mIndex_i=standard.mIndex_i;
    return *this;
  }
  bool operator==(const BofVideoStandard& standard) const
  {
    return mIndex_i == standard.mIndex_i;
  }
  inline int ActiveCols() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].ActiveCols_i : 0;
  }
  inline int ActiveRows() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].ActiveRows_i : 0;
  }
  inline int DescriptionCols() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].DescriptionCols_i : 0;
  }
  inline int DescriptionRows() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].DescriptionRows_i : 0;
  }
  inline BofRational AudioClockRate() const
  {
    return (mIndex_i >= 0)
           ? BofRational(S_mpTable_X[mIndex_i].AudioClockRateNum_S64, S_mpTable_X[mIndex_i].AudioClockRateDen_i,true)
           : BofRational();
  }
  inline const char* Description() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].pDescription_c : nullptr;
  }
  inline std::string ToString() const
  {
    return (mIndex_i >= 0) ? std::string(S_mpTable_X[mIndex_i].pDescription_c) : std::string("");
  }
  inline BofRational EffectiveFrameRate() const
  {
    return (mIndex_i >= 0)  ? (BofRational(S_mpTable_X[mIndex_i].FrameRateNum_i, S_mpTable_X[mIndex_i].FrameRateDen_i, true) * ((S_mpTable_X[mIndex_i].Type_c == 'i') ? 2 : 1)) : BofRational();
  }
  inline int FieldsPerFrame() const
  {
    return (mIndex_i >= 0) ? (((S_mpTable_X[mIndex_i].Type_c == 'i') || (S_mpTable_X[mIndex_i].Type_c == 's')) ? 2 : 1) : 0;
  }
  inline BofRational FrameRate() const
  {
    return (mIndex_i >= 0) ? BofRational(S_mpTable_X[mIndex_i].FrameRateNum_i, S_mpTable_X[mIndex_i].FrameRateDen_i,true) : BofRational();
  }
  static BofVideoStandard FromIndex(const int index);
  inline bool Hdtv() const
  {
    return (mIndex_i >= 0) ? (S_mpTable_X[mIndex_i].DescriptionRows_i >= 720) : false;
  }
  inline VideoStandardId Id() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].Id_U32 : 0;
  }
  inline const char* IdTxt() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].pIdText_c : 0;
  }
  inline BofRational ImageAspectRatio() const
  {
    return (mIndex_i >= 0)
           ? BofRational(S_mpTable_X[mIndex_i].ImageAspectRatioNum_i, S_mpTable_X[mIndex_i].ImageAspectRatioDen_i,true) : BofRational(1);
  }
  inline bool Interlaced() const
  {
    return (mIndex_i >= 0) ? (S_mpTable_X[mIndex_i].Type_c == 'i') : false;
  }
  inline BofRational PixelAspectRatio() const
  {
    return (mIndex_i >= 0) ? PixelAspectRatio(S_mpTable_X[mIndex_i].ActiveCols_i, S_mpTable_X[mIndex_i].ActiveRows_i, BofRational(S_mpTable_X[mIndex_i].ImageAspectRatioNum_i, S_mpTable_X[mIndex_i].ImageAspectRatioDen_i,true)) : BofRational(1);
  }
  // imageA is 4:3 or 16:9
  inline BofRational PixelAspectRatio(const BofRational imageA) const
  {
    return (mIndex_i >= 0) ? PixelAspectRatio(S_mpTable_X[mIndex_i].ActiveCols_i, S_mpTable_X[mIndex_i].ActiveRows_i, imageA) : BofRational(1);
  }
  static BofRational PixelAspectRatio(const int imageW, const int imageH, const BofRational imageA)
  {
    return BofRational(imageH) / (BofRational(imageW) / imageA);
  }
  inline bool Progressive() const
  {
    return (mIndex_i >= 0) ? (S_mpTable_X[mIndex_i].Type_c == 'p') : false;
  }
  inline bool Sdtv() const
  {
    return (mIndex_i >= 0) ? (S_mpTable_X[mIndex_i].DescriptionRows_i < 720) : false;
  }
  inline bool SegmentedFrame() const
  {
    return (mIndex_i >= 0) ? (S_mpTable_X[mIndex_i].Type_c == 's') : false;
  }
  inline int SwitchLine1() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].SwitchLine1_i : 0;
  }
  inline int SwitchLine2() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].SwitchLine2_i : 0;
  }
  inline int TotalCols() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].TotalCols_i : 0;
  }
  inline int TotalRows() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].TotalRows_i : 0;
  }
  inline int Fps() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].DescriptionFps_i : 0;
  }
  inline int Duration() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].FrameRateDen_i * 1000000 / S_mpTable_X[mIndex_i].FrameRateNum_i : 0;   // [usecs]
  }
  inline int FieldDuration() const
  {
    return Duration() / (Interlaced() ? 2 : 1);
  }
  inline int Smpte352PayloadID() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].Smpte352PayloadId_U32 : 0;
  }
  inline int PidAncillaryData() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].PidAncillaryData_U32 : 0;
  }
  inline int VideoDelayOffset() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].VDelayOffset_i : 0;
  }
  inline int UserTableIndex() const
  {
    return (mIndex_i >= 0) ? S_mpTable_X[mIndex_i].UserTableIndex_i : -1;
  }

  inline bool Valid() const
  {
    return (mIndex_i >= 0);
  }
  static bool S_Parse(const char* standard, int &w, int &h, int &fps, char &type);

private:
  struct Table {
    VideoStandardId Id_U32;
    const char      *pIdText_c;
    const char *pDescription_c;
    int      DescriptionCols_i;
    int      DescriptionRows_i;
    int      DescriptionFps_i;
    char     Type_c;
    int      ActiveCols_i;
    int      ActiveRows_i;
    int      TotalCols_i;
    int      TotalRows_i;
    int      FrameRateNum_i;
    int      FrameRateDen_i;
    int      SwitchLine1_i; // line counting starts with 0
    int      SwitchLine2_i; // line counting starts with 0
    int     ImageAspectRatioNum_i;
    int     ImageAspectRatioDen_i;
    int64_t AudioClockRateNum_S64;
    int      AudioClockRateDen_i;
    uint32_t Smpte352PayloadId_U32;
    uint32_t PidAncillaryData_U32;
    int VDelayOffset_i;
    int UserTableIndex_i;
  };
  static const Table S_mpTable_X[];
  int                mIndex_i;

  static int S_FindIndex(const VideoStandardId standard);
  static int S_FindIndex(const char *standard);
  static int S_FindIndex(const int numCols, const int numRows, const int fps, const char type);
};

END_BOF_NAMESPACE()