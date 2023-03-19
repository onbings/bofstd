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

#include <bofstd/bofrational.h>
#include <bofstd/bofstring.h>

BEGIN_BOF_NAMESPACE()

#define BOF_VIDEO_STANDARD_ID(Width, Height, Fps, Type) \
    ((uint32_t)((Width - 1) << 20) | (uint32_t)((Height - 1) << 8) | (uint32_t)((Fps - 1) << 2) | \
     ((Type == 'i') ? 1 : (Type == 'p') ? 2 : (Type == 's') ? 3 : 0))

typedef uint32_t VideoStandardId;
//static const VideoStandardId  DefaultVideoStandard = BOF_VIDEO_STANDARD_ID(1920, 1080, 50, 'p');
struct BOF_VIDEO_STANDARD_ENTRY
{
  //    const char *pIdText_c;
  char     pDescription_c[32];
  uint32_t NbCol_U32;
  uint32_t NbRow_U32;
  uint32_t Fps_U32;
  char     Type_c;
  uint32_t NbActiveCol_U32;
  uint32_t NbActiveRow_U32;
  uint32_t NbTotalCol_U32;
  uint32_t NbTotalRow_U32;
  uint32_t FrameRateNum_U32;
  uint32_t FrameRateDen_U32;
  uint32_t SwitchLine1_U32; // line counting starts with 0
  uint32_t SwitchLine2_U32; // line counting starts with 0
  uint32_t ImageAspectRatioNum_U32;
  uint32_t ImageAspectRatioDen_U32;
  uint64_t AudioClockRateNum_S64;
  uint64_t AudioClockRateDen_U64;
  uint32_t Smpte352PayloadId_U32;
  uint32_t PidAncillaryData_U32;

  BOF_VIDEO_STANDARD_ENTRY()
  {
    Reset();
  }
  BOF_VIDEO_STANDARD_ENTRY(const char *_pDescription_c, uint32_t _NbCol_U32, uint32_t _NbRow_U32, uint32_t _Fps_U32,
    char _Type_c, uint32_t _NbActiveCol_U32, uint32_t _NbActiveRow_U32, uint32_t _NbTotalCol_U32, uint32_t _NbTotalRow_U32,
    uint32_t _FrameRateNum_U32, uint32_t _FrameRateDen_U32, uint32_t _SwitchLine1_U32, uint32_t _SwitchLine2_U32, 
    uint32_t _ImageAspectRatioNum_U32, uint32_t _ImageAspectRatioDen_U32, uint64_t _AudioClockRateNum_S64, 
    uint64_t _AudioClockRateDen_U64, uint32_t _Smpte352PayloadId_U32, uint32_t _PidAncillaryData_U32)
  {
    if (_pDescription_c)
    {
      Bof_StrNCpy(pDescription_c, _pDescription_c, sizeof(pDescription_c));
    }
    else
    {
      pDescription_c[0] = 0;
    }
    NbCol_U32 = _NbCol_U32;
    NbRow_U32 = _NbRow_U32;
    Fps_U32 = _Fps_U32;
    Type_c = _Type_c;
    NbActiveCol_U32 = _NbActiveCol_U32;
    NbActiveRow_U32 = _NbActiveRow_U32;
    NbTotalCol_U32 = _NbTotalCol_U32;
    NbTotalRow_U32 = _NbTotalRow_U32;
    FrameRateNum_U32 = _FrameRateNum_U32;
    FrameRateDen_U32 = _FrameRateDen_U32;
    SwitchLine1_U32 = _SwitchLine1_U32;
    SwitchLine2_U32 = _SwitchLine2_U32;
    ImageAspectRatioNum_U32 = _ImageAspectRatioNum_U32;
    ImageAspectRatioDen_U32 = _ImageAspectRatioDen_U32;
    AudioClockRateNum_S64 = _AudioClockRateNum_S64;
    AudioClockRateDen_U64 = _AudioClockRateDen_U64;
    Smpte352PayloadId_U32 = _Smpte352PayloadId_U32;
    PidAncillaryData_U32 = _PidAncillaryData_U32;
  }
  void Reset()
  {
    pDescription_c[0] = 0;
    NbCol_U32 = 0;
    NbRow_U32 = 0;
    Fps_U32 = 0;
    Type_c = 0;
    NbActiveCol_U32 = 0;
    NbActiveRow_U32 = 0;
    NbTotalCol_U32 = 0;
    NbTotalRow_U32 = 0;
    FrameRateNum_U32 = 0;
    FrameRateDen_U32 = 0;
    SwitchLine1_U32 = 0;
    SwitchLine2_U32 = 0;
    ImageAspectRatioNum_U32 = 0;
    ImageAspectRatioDen_U32 = 0;
    AudioClockRateNum_S64 = 0;
    AudioClockRateDen_U64 = 0;
    Smpte352PayloadId_U32 = 0;
    PidAncillaryData_U32 = 0;
  }
};
class BOFSTD_EXPORT BofVideoStandard
{
public:
  BofVideoStandard();
  BofVideoStandard(const BofVideoStandard &_rStandard);
  BofVideoStandard(const std::string &_rStandard_S);
  BofVideoStandard(uint32_t _NbCol_U32, uint32_t _NbRow_U32, uint32_t _Fps_U32, char _Type_c);
  BofVideoStandard &operator=(const BofVideoStandard &_rOther);
  bool operator==(const BofVideoStandard &_rOther) const;
  uint32_t NbActiveCol() const;
  uint32_t NbActiveRow() const;
  uint32_t NbCol() const;
  uint32_t NbRow() const;
  uint32_t NbTotalCol() const;
  uint32_t NbTotalRow() const;
  BofRational AudioClockRate() const;
  std::string ToString() const;
  VideoStandardId Id() const;
  BofRational EffectiveFrameRate() const;
  uint32_t FieldsPerFrame() const;
  BofRational FrameRate() const;
  BofRational ImageAspectRatio() const;
  BofRational PixelAspectRatio() const;
  BofRational PixelAspectRatio(const BofRational &_rAr) const;
  BofRational PixelAspectRatio(uint32_t _Width_U32, uint32_t _Height_U32, const BofRational &_rAr) const;
  uint32_t SwitchLine1() const;
  uint32_t SwitchLine2() const;
  uint32_t Fps() const;
  uint32_t Duration() const;
  uint32_t FieldDuration() const;
  uint32_t Smpte352PayloadID() const;
  uint32_t PidAncillaryData() const;
  bool IsValid() const;
  BofVideoStandard FromIndex(int _Index_i);
  bool S_Parse(const std::string &_rStandard_S, uint32_t &_rWidth_U32, uint32_t &_rHeight_U32, BofRational &_rFps, char &_rType_c);
  bool IsHdtv() const;
  bool IsInterlaced() const;
  bool IsProgressive() const;
  bool IsSdtv() const;
  bool SegmentedFrame() const;

private:

  static const BOF_VIDEO_STANDARD_ENTRY S_mpVideoStandardTable_X[];
  BOF_VIDEO_STANDARD_ENTRY mCustomFormat_X;
  const BOF_VIDEO_STANDARD_ENTRY *mpVideoStandardEntry_X = nullptr;
  int                       mIndex_i;
  static int S_FindIndex(const std::string &_rStandard_S);
  static int S_FindIndex(uint32_t _NbCol_U32, uint32_t _NbRow_U32, uint32_t _Fps_U32, char _Type_c);
};

END_BOF_NAMESPACE()