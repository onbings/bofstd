/*
 * Copyright (c) 2015-2025, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements videostandard class
 *
 * Name:        bofvideostandard.cpp
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
#include <bofstd/bofvideostandard.h>

#include <regex>

BEGIN_BOF_NAMESPACE()

// As//{ BOF_VIDEO_STANDARD_ID(720,  525, 59, 'i'),
const std::string BofVideoStandard::S_mEmpty_S("");

const BOF_VIDEO_STANDARD_ENTRY BofVideoStandard::S_mpVideoStandardTable_X[] = {
 // description              dcol  drow fps type acol  arow  tcol   trow FrmRtN   Den s1   s2    A/R    audClkRateN   Den    smpte352   pidtabIdx
  { "0x0@0?",                   0,    0,  0, '?',   0,    0,    0,     0,     0,    1, 0,   0,           0, 0,           0LL,    1,          0,          0 },
  { "720x525@59.94i",         720,  525, 59, 'i', 720,  488,  858,   525, 30000, 1001, 9, 272,           4, 3,    27000000LL, 1001, 0x81060001, 0x000D01CE },
  { "720x625@50i",            720,  625, 50, 'i', 720,  576,  864,   625,    25,    1, 5, 318,           4, 3,    27000000LL,    1, 0x81050001, 0x000901CD },
  { "1280x720@50p",          1280,  720, 50, 'p', 1280,  720, 1980,  750,     50,    1, 6,(uint32_t)-1, 16, 9,    74250000LL,    1, 0x84490001, 0x000A0114},
  { "1280x720@59.94p",       1280,  720, 59, 'p', 1280,  720, 1650,  750,  60000, 1001, 6,(uint32_t)-1, 16, 9, 74250000000LL, 1001, 0x844A0001, 0x000A0115 },
  { "1280x720@24sF",         1280,  720, 50, 's', 1280,  720,    0,    0,     50,    1, 6,(uint32_t)-1, 16, 9,    74250000LL,    1, 0,          0,         },
  { "1280x720@50p",          1280,  720, 50, 'p', 1280,  720, 1980,  750,     50,    1, 6,(uint32_t)-1, 16, 9,    74250000LL,    1, 0x84498001, 0x000A0294 },
  { "1280x720@59.94p",       1280,  720, 59, 'p', 1280,  720, 1650,  750,  60000, 1001, 6,(uint32_t)-1, 16, 9, 74250000000LL, 1001, 0x844A8001, 0x000A0295 },
  { "1280x720@60p",          1280,  720, 60, 'p', 1280,  720, 1650,  750,     60,    1, 6,(uint32_t)-1, 16, 9,    74250000LL,    1, 0x844B8001, 0x000A0216 },
  { "1920x1080@23.97p",      1920, 1080, 23, 'p', 1920, 1080, 2750, 1125,  24000, 1001, 6, 568,         16, 9, 74250000000LL, 1001, 0x85C28001, 0x000A028E },
  { "1920x1080@23.97s",      1920, 1080, 23, 's', 1920, 1080, 2750, 1125,  24000, 1001, 6, 568,         16, 9, 74250000000LL, 1001, 0,          0,         },
  { "1920x1080@24p",         1920, 1080, 24, 'p', 1920, 1080, 2750, 1125,     24,    1, 6, 568,         16, 9,    74250000LL,    1, 0x85C38001, 0x000A018F },
  { "1920x1080@24sF",        1920, 1080, 24, 's', 1920, 1080, 2750, 1125,     24,    1, 6, 568,         16, 9,    74250000LL,    1, 0,          0,         },
  { "1920x1080@25p",         1920, 1080, 25, 'p', 1920, 1080, 2640, 1125,     25,    1, 6, 568,         16, 9,    74250000LL,    1, 0x85C58001, 0x000A0191 },
  { "1920x1080@25sF",        1920, 1080, 25, 's', 1920, 1080, 2640, 1125,     25,    1, 6, 568,         16, 9,    74250000LL,    1, 0,          0,         },
  { "1920x1080@29.97p",      1920, 1080, 29, 'p', 1920, 1080, 2200, 1125,  30000, 1001, 6, 568,         16, 9, 74250000000LL, 1001, 0x85C68001, 0x000A0191 },
  { "1920x1080@29.97s",      1920, 1080, 29, 's', 1920, 1080, 2200, 1125,  30000, 1001, 6, 568,         16, 9, 74250000000LL, 1001, 0,          0,         },
  { "1920x1080@30p",         1920, 1080, 30, 'p', 1920, 1080, 2200, 1125,     30,    1, 6, 568,         16, 9,    74250000LL,    1, 0x85C78001, 0x000A0293 },
  { "1920x1080@30sF",        1920, 1080, 30, 's', 1920, 1080, 2200, 1125,     30,    1, 6, 568,         16, 9,    74250000LL,    1, 0,          0,         },
  { "1920x1080@50i",         1920, 1080, 50, 'i', 1920, 1080, 2640, 1125,     25,    1, 6, 568,         16, 9,    74250000LL,    1, 0x85052001, 0x0FFF0251 },
  { "1920x1080@50p",         1920, 1080, 50, 'p', 1920, 1080, 2640, 1125,     50,    1, 6, 568,         16, 9,    74250000LL,    1, 0x89C98001, 0x000A0119 },
  { "1920x1080@59.94i",      1920, 1080, 59, 'i', 1920, 1080, 2200, 1125,  30000, 1001, 6, 568,         16, 9, 74250000000LL, 1001, 0x85062001, 0x0FFF01F2 },
  { "1920x1080@59.94p",      1920, 1080, 59, 'p', 1920, 1080, 2200, 1125,  60000, 1001, 6, 568,         16, 9, 74250000000LL, 1001, 0x89CA8001, 0x000A011A },
  { "1920x1080@60i",         1920, 1080, 60, 'i', 1920, 1080, 2200, 1125,     30,    1, 6, 568,         16, 9,    74250000LL,    1, 0x85078001, 0x0FFF01D3 },
  { "1920x1080@60p",         1920, 1080, 60, 'p', 1920, 1080, 2200, 1125,     60,    1, 6, 568,         16, 9,    74250000LL,    1, 0x89CB8001, 0x000A029B },
  { "3840x2160@50p",         3840, 2160, 50, 'p', 3840, 2160, 4000, 2200,     50,    1, 6, 568,         16, 9,    74250000LL,    1, 0x89CB8001, 0x000AFFFF },
  { "3840x2160@59p",         3840, 2160, 59, 'p', 3840, 2160, 4000, 2200,  60000, 1001, 6, 568,         16, 9, 74250000000LL, 1001, 0x89CB8001, 0x000AFFFF }, 
  { "3840x2160@60p",         3840, 2160, 60, 'p', 3840, 2160, 4000, 2200,     60,    1, 6, 568,         16, 9,    74250000LL,    1, 0x89CB8001, 0x000AFFFF }  
};

BofVideoStandard::BofVideoStandard() : mIndex_i(-1)
{
  printf("BofVideoStandard1\n");
  mCustomFormat_X = S_mpVideoStandardTable_X[0];
  printf("BofVideoStandard2\n");
  mpVideoStandardEntry_X = (mIndex_i <= 0) ? &mCustomFormat_X : &S_mpVideoStandardTable_X[mIndex_i];
  printf("BofVideoStandard3\n");
}
BofVideoStandard::BofVideoStandard(const BofVideoStandard &_rStandard) : mIndex_i(_rStandard.mIndex_i)
{
  printf("BofVideoStandardA\n");
  mCustomFormat_X = S_mpVideoStandardTable_X[0];
  printf("BofVideoStandardB\n");
  mpVideoStandardEntry_X = (mIndex_i <= 0) ? &mCustomFormat_X : &S_mpVideoStandardTable_X[mIndex_i];
  printf("BofVideoStandardC\n");
}
BofVideoStandard::BofVideoStandard(const std::string &_rStandard_S)
{
  uint32_t Width_U32, Height_U32;
  BofRational Fps, AspectRatio;
  char Type_c;
  double Ar_lf, Fr_lf;

  printf("BofVideoStandarda\n");

  mCustomFormat_X = S_mpVideoStandardTable_X[0];

  mIndex_i = S_FindIndex(_rStandard_S);
  printf("BofVideoStandardb\n");

  if (mIndex_i == -1)
  {
    if (S_Parse(_rStandard_S, Width_U32, Height_U32, Fps, Type_c))
    {
      Ar_lf = (double)Width_U32 / (double)Height_U32;
      Fr_lf = (double)Fps.Num() / (double)Fps.Den();
      AspectRatio = BofRational(Ar_lf, 10);
      mCustomFormat_X.Description_S = _rStandard_S;
      mCustomFormat_X.Fps_U32 = (uint32_t)Fr_lf;
      mCustomFormat_X.FrameRateNum_U32 = Fps.Num();
      mCustomFormat_X.FrameRateDen_U32 = Fps.Den();
      mCustomFormat_X.ImageAspectRatioNum_U32 = AspectRatio.Num();
      mCustomFormat_X.ImageAspectRatioDen_U32 = AspectRatio.Den();
      mCustomFormat_X.NbRow_U32 = Height_U32;
      mCustomFormat_X.NbActiveRow_U32 = Height_U32;
      mCustomFormat_X.NbTotalRow_U32 = Height_U32;
      mCustomFormat_X.NbCol_U32 = Width_U32;
      mCustomFormat_X.NbActiveCol_U32 = Width_U32;
      mCustomFormat_X.NbTotalCol_U32 = Width_U32;
      mCustomFormat_X.Type_c = Type_c;
      mIndex_i = 0;
    }
  }
  printf("BofVideoStandardc\n");

  mpVideoStandardEntry_X = (mIndex_i <= 0) ? &mCustomFormat_X : &S_mpVideoStandardTable_X[mIndex_i];
}
BofVideoStandard::BofVideoStandard(uint32_t _NbCol_U32, uint32_t _NbRow_U32, uint32_t _Fps_U32, char _Type_c)
{
  mCustomFormat_X = S_mpVideoStandardTable_X[0];
  mIndex_i = S_FindIndex(_NbCol_U32, _NbRow_U32, _Fps_U32, _Type_c);
  mpVideoStandardEntry_X = (mIndex_i <= 0) ? &mCustomFormat_X : &S_mpVideoStandardTable_X[mIndex_i];
}
BofVideoStandard &BofVideoStandard::operator=(const BofVideoStandard &_rOther)
{
  printf("BofVideoStandard=1\n");
  mCustomFormat_X = _rOther.mCustomFormat_X;
  printf("BofVideoStandard=2\n");
  mIndex_i = _rOther.mIndex_i;
  printf("BofVideoStandard=3\n");
  mpVideoStandardEntry_X = (mIndex_i <= 0) ? &mCustomFormat_X : &S_mpVideoStandardTable_X[mIndex_i];
  printf("BofVideoStandard=4\n");
  return *this;
}
bool BofVideoStandard::operator==(const BofVideoStandard &_rOther) const
{
  return mIndex_i == _rOther.mIndex_i;
}

uint32_t BofVideoStandard::NbActiveCol() const
{
  return (mIndex_i >= 0) ? mpVideoStandardEntry_X->NbActiveCol_U32 : 0;
}
uint32_t BofVideoStandard::NbActiveRow() const
{
  return (mIndex_i >= 0) ? mpVideoStandardEntry_X->NbActiveRow_U32 : 0;
}
uint32_t BofVideoStandard::NbCol() const
{
  return (mIndex_i >= 0) ? mpVideoStandardEntry_X->NbCol_U32 : 0;
}
uint32_t BofVideoStandard::NbRow() const
{
  return (mIndex_i >= 0) ? mpVideoStandardEntry_X->NbRow_U32 : 0;
}
BofRational BofVideoStandard::AudioClockRate() const
{
  return (mIndex_i >= 0) ? BofRational(mpVideoStandardEntry_X->AudioClockRateNum_S64, mpVideoStandardEntry_X->AudioClockRateDen_U64, true) : BofRational();
}
VideoStandardId BofVideoStandard::Id() const
{
  VideoStandardId Rts = 0;

  if (mIndex_i >= 0)
  {
    Rts = BOF_VIDEO_STANDARD_ID(mpVideoStandardEntry_X->NbCol_U32, mpVideoStandardEntry_X->NbRow_U32, mpVideoStandardEntry_X->Fps_U32, mpVideoStandardEntry_X->Type_c);
  } 
  return Rts;
}
const std::string &BofVideoStandard::ToString() const
{
  return (mIndex_i >= 0) ? mpVideoStandardEntry_X->Description_S : S_mEmpty_S;
}
BofRational BofVideoStandard::EffectiveFrameRate() const
{
  return (mIndex_i >= 0) ? (BofRational((int64_t)mpVideoStandardEntry_X->FrameRateNum_U32, (uint64_t)mpVideoStandardEntry_X->FrameRateDen_U32, true) * ((mpVideoStandardEntry_X->Type_c == 'i') ? 2 : 1)) : BofRational();
}
uint32_t BofVideoStandard::FieldsPerFrame() const
{
  return (mIndex_i >= 0) ? (((mpVideoStandardEntry_X->Type_c == 'i') || (mpVideoStandardEntry_X->Type_c == 's')) ? 2 : 1) : 0;
}
BofRational BofVideoStandard::FrameRate() const
{
  return (mIndex_i >= 0) ? BofRational((int64_t)mpVideoStandardEntry_X->FrameRateNum_U32, (uint64_t)mpVideoStandardEntry_X->FrameRateDen_U32, true) : BofRational();
}
bool BofVideoStandard::IsHdtv() const
{
  return (mIndex_i >= 0) ? (mpVideoStandardEntry_X->NbRow_U32 >= 720) : false;
}
BofRational BofVideoStandard::ImageAspectRatio() const
{
  return (mIndex_i >= 0) ? BofRational((int64_t)mpVideoStandardEntry_X->ImageAspectRatioNum_U32, (uint64_t)mpVideoStandardEntry_X->ImageAspectRatioDen_U32, true) : BofRational(1);
}
bool BofVideoStandard::IsInterlaced() const
{
  return (mIndex_i >= 0) ? (mpVideoStandardEntry_X->Type_c == 'i') : false;
}
BofRational BofVideoStandard::PixelAspectRatio() const
{
  return (mIndex_i >= 0) ? PixelAspectRatio(mpVideoStandardEntry_X->NbActiveCol_U32, mpVideoStandardEntry_X->NbActiveRow_U32, BofRational((int64_t)mpVideoStandardEntry_X->ImageAspectRatioNum_U32, (uint64_t)mpVideoStandardEntry_X->ImageAspectRatioDen_U32, true)) : BofRational(1);
}
// imageA is 4:3 or 16:9
BofRational BofVideoStandard::PixelAspectRatio(const BofRational &_rAr) const
{
  return (mIndex_i >= 0) ? PixelAspectRatio(mpVideoStandardEntry_X->NbActiveCol_U32, mpVideoStandardEntry_X->NbActiveCol_U32, _rAr) : BofRational(1);
}
BofRational BofVideoStandard::PixelAspectRatio(uint32_t _Width_U32, uint32_t _Height_U32, const BofRational &_rAr) const
{
  return BofRational(_Height_U32) / (BofRational(_Width_U32) / _rAr);
}
bool BofVideoStandard::IsProgressive() const
{
  return (mIndex_i >= 0) ? (mpVideoStandardEntry_X->Type_c == 'p') : false;
}
bool BofVideoStandard::IsSdtv() const
{
  return (mIndex_i >= 0) ? (mpVideoStandardEntry_X->NbRow_U32 < 720) : false;
}
bool BofVideoStandard::SegmentedFrame() const
{
  return (mIndex_i >= 0) ? (mpVideoStandardEntry_X->Type_c == 's') : false;
}
uint32_t BofVideoStandard::SwitchLine1() const
{
  return (mIndex_i >= 0) ? mpVideoStandardEntry_X->SwitchLine1_U32 : 0;
}
uint32_t BofVideoStandard::SwitchLine2() const
{
  return (mIndex_i >= 0) ? mpVideoStandardEntry_X->SwitchLine2_U32 : 0;
}
uint32_t BofVideoStandard::NbTotalCol() const
{
  return (mIndex_i >= 0) ? mpVideoStandardEntry_X->NbTotalCol_U32 : 0;
}
uint32_t BofVideoStandard::NbTotalRow() const
{
  return (mIndex_i >= 0) ? mpVideoStandardEntry_X->NbTotalRow_U32 : 0;
}
uint32_t BofVideoStandard::Fps() const
{
  return (mIndex_i >= 0) ? mpVideoStandardEntry_X->Fps_U32 : 0;
}
uint32_t BofVideoStandard::Duration() const
{
  return (mIndex_i >= 0) ? mpVideoStandardEntry_X->FrameRateDen_U32 * 1000000 / mpVideoStandardEntry_X->FrameRateNum_U32 : 0;   // [usecs]
}
uint32_t BofVideoStandard::FieldDuration() const
{
  return Duration() / (IsInterlaced() ? 2 : 1);
}
uint32_t BofVideoStandard::Smpte352PayloadID() const
{
  return (mIndex_i >= 0) ? mpVideoStandardEntry_X->Smpte352PayloadId_U32 : 0;
}
uint32_t BofVideoStandard::PidAncillaryData() const
{
  return (mIndex_i >= 0) ? mpVideoStandardEntry_X->PidAncillaryData_U32 : 0;
}
bool BofVideoStandard::IsValid() const
{
  return (mIndex_i >= 0);
}

int BofVideoStandard::S_FindIndex(const std::string &_rStandard_S)
{
  int Rts_i = -1;
  uint32_t i_U32;

  for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(S_mpVideoStandardTable_X); i_U32++)
  {
    if (S_mpVideoStandardTable_X[i_U32].Description_S == _rStandard_S)
    {
      Rts_i = i_U32;
      break;
    }
  }
  return Rts_i;
}

int BofVideoStandard::S_FindIndex(uint32_t _NbCol_U32, uint32_t _NbRow_U32, uint32_t _Fps_U32, char _Type_c)
{
  int Rts_i = -1;
  uint32_t i_U32;

  for (i_U32 = 0; i_U32 < BOF_NB_ELEM_IN_ARRAY(S_mpVideoStandardTable_X); i_U32++) 
  {
    if (S_mpVideoStandardTable_X[i_U32].NbCol_U32 == _NbCol_U32) 
    {
      if (S_mpVideoStandardTable_X[i_U32].NbRow_U32 == _NbRow_U32)
      {
        if (S_mpVideoStandardTable_X[i_U32].Fps_U32 == _Fps_U32)
        {
          if (S_mpVideoStandardTable_X[i_U32].Type_c == _Type_c)
          {
            Rts_i = i_U32;
            break;
          }
        }
      }
    }
  }
  return Rts_i;
}

BofVideoStandard BofVideoStandard::FromIndex(int _Index_i)
{
  BofVideoStandard Rts;

  if ((_Index_i >= 0) && (_Index_i < BOF_NB_ELEM_IN_ARRAY(S_mpVideoStandardTable_X)))
  {
    Rts.mIndex_i = _Index_i;
  }
  return Rts;
}

bool BofVideoStandard::S_Parse(const std::string &_rStandard_S, uint32_t &_rWidth_U32, uint32_t &_rHeight_U32, BofRational &_rFps, char &_rType_c)
{
  bool Rts_B = false;
  static const std::regex S_RegExVideoStandard(R"(^(\d*)x(\d*)@(.*)(.))");
  std::smatch MatchString;

  if (std::regex_search(_rStandard_S, MatchString, S_RegExVideoStandard))
  {
    if (MatchString.size() == 5)
    {
      _rWidth_U32 = std::atol(MatchString[1].str().c_str());
      _rHeight_U32 = std::atol(MatchString[2].str().c_str());
      _rFps = BofRational(std::atof(MatchString[3].str().c_str()));
      _rType_c = MatchString[4].str().c_str()[0];
      Rts_B = true;
    }
  }
  return Rts_B;
}

END_BOF_NAMESPACE()