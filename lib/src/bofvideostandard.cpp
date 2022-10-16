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

BEGIN_BOF_NAMESPACE()

// As for pixel aspect ratios, see http://en.wikipedia.org/wiki/Pixel_aspect_ratio

// 720x525_59iLB is not supported
const BofVideoStandard::Table BofVideoStandard::S_mpTable_X[] = {
  // id                                     idText           description                    dcol  drow fps type  acol  arow  tcol  trow  framerate   s1  s2   imgA/R  audioClockRate      smpte352    pid, 		vdlyOff, utabIdx
  { BOF_VIDEO_STANDARD_ID(720,  525, 59, 'i'), "720x525_59i",    "720x525@59.94i (NTSC)",   720,  525, 59, 'i',  720,  488,  858,  525, 30000, 1001, 9, 272,  4, 3, 27000000LL,    1001, 0x81060001, 0x000D01CE,    0,     1 },
  { BOF_VIDEO_STANDARD_ID(720,  625, 50, 'i'), "720x625_50i",    "720x625@50i (PAL)",       720,  625, 50, 'i',  720,  576,  864,  625, 25,    1,    5, 318,  4, 3, 27000000LL,    1,    0x81050001, 0x000901CD, 1480,     0 },
  { BOF_VIDEO_STANDARD_ID(1280,  720, 50, 'p'), "1280x720_50p",   "1280x720@50p",           1280,  720, 50, 'p', 1280,  720, 1980,  750, 50,    1,    6,  -1, 16, 9, 74250000LL,    1,    0x84490001, 0x000A0114,  500,     2 },
  { BOF_VIDEO_STANDARD_ID(1280,  720, 59, 'p'), "1280x720_59p",   "1280x720@59.94p",        1280,  720, 59, 'p', 1280,  720, 1650,  750, 60000, 1001, 6,  -1, 16, 9, 74250000000LL, 1001, 0x844A0001, 0x000A0115,  300,     3 },
  { BOF_VIDEO_STANDARD_ID(1280,  720, 24, 's'), "1280x720_24sF",  "1280x720@24sF",          1280,  720, 50, 's', 1280,  720,    0,    0, 50,    1,    6,  -1, 16, 9, 74250000LL,    1,    0,          0,             0,    -1 }, // FIXME
  { BOF_VIDEO_STANDARD_ID(1280,  720, 50, 'p'), "1280x720_50p",   "1280x720@50p",           1280,  720, 50, 'p', 1280,  720, 1980,  750, 50,    1,    6,  -1, 16, 9, 74250000LL,    1,    0x84498001, 0x000A0294,  500,     2 },
  { BOF_VIDEO_STANDARD_ID(1280,  720, 59, 'p'), "1280x720_59p",   "1280x720@59.94p",        1280,  720, 59, 'p', 1280,  720, 1650,  750, 60000, 1001, 6,  -1, 16, 9, 74250000000LL, 1001, 0x844A8001, 0x000A0295,  300,     3 },
  { BOF_VIDEO_STANDARD_ID(1280,  720, 60, 'p'), "1280x720_60p",   "1280x720@60p",           1280,  720, 60, 'p', 1280,  720, 1650,  750, 60,    1,    6,  -1, 16, 9, 74250000LL,    1,    0x844B8001, 0x000A0216,  320,     4 },
  { BOF_VIDEO_STANDARD_ID(1920, 1080, 23, 'p'), "1920x1080_23p",  "1920x1080@23.97p",       1920, 1080, 23, 'p', 1920, 1080, 2750, 1125, 24000, 1001, 6, 568, 16, 9, 74250000000LL, 1001, 0x85C28001, 0x000A028E,    0,    11 },
  { BOF_VIDEO_STANDARD_ID(1920, 1080, 23, 's'), "1920x1080_23sF", "1920x1080@23.97sF",      1920, 1080, 23, 's', 1920, 1080, 2750, 1125, 24000, 1001, 6, 568, 16, 9, 74250000000LL, 1001, 0,          0,             0,    -1 },
  { BOF_VIDEO_STANDARD_ID(1920, 1080, 24, 'p'), "1920x1080_24p",  "1920x1080@24p",          1920, 1080, 24, 'p', 1920, 1080, 2750, 1125, 24,    1,    6, 568, 16, 9, 74250000LL,    1,    0x85C38001, 0x000A018F,    0,    12 },
  { BOF_VIDEO_STANDARD_ID(1920, 1080, 24, 's'), "1920x1080_24sF", "1920x1080@24sF",         1920, 1080, 24, 's', 1920, 1080, 2750, 1125, 24,    1,    6, 568, 16, 9, 74250000LL,    1,    0,          0,             0,    -1 },
  { BOF_VIDEO_STANDARD_ID(1920, 1080, 25, 'p'), "1920x1080_25p",  "1920x1080@25p",          1920, 1080, 25, 'p', 1920, 1080, 2640, 1125, 25,    1,    6, 568, 16, 9, 74250000LL,    1,    0x85C58001, 0x000A0191,    0,    13 },
  { BOF_VIDEO_STANDARD_ID(1920, 1080, 25, 's'), "1920x1080_25sF", "1920x1080@25sF",         1920, 1080, 25, 's', 1920, 1080, 2640, 1125, 25,    1,    6, 568, 16, 9, 74250000LL,    1,    0,          0,             0,    -1 },
  { BOF_VIDEO_STANDARD_ID(1920, 1080, 29, 'p'), "1920x1080_29p",  "1920x1080@29.97p",       1920, 1080, 29, 'p', 1920, 1080, 2200, 1125, 30000, 1001, 6, 568, 16, 9, 74250000000LL, 1001, 0x85C68001, 0x000A0191,    0,    14 },
  { BOF_VIDEO_STANDARD_ID(1920, 1080, 29, 's'), "1920x1080_29sF", "1920x1080@29.97sF",      1920, 1080, 29, 's', 1920, 1080, 2200, 1125, 30000, 1001, 6, 568, 16, 9, 74250000000LL, 1001, 0,          0,             0,    -1 },
  { BOF_VIDEO_STANDARD_ID(1920, 1080, 30, 'p'), "1920x1080_30p",  "1920x1080@30p",          1920, 1080, 30, 'p', 1920, 1080, 2200, 1125, 30,    1,    6, 568, 16, 9, 74250000LL,    1,    0x85C78001, 0x000A0293,    0,    15 },
  { BOF_VIDEO_STANDARD_ID(1920, 1080, 30, 's'), "1920x1080_30sF", "1920x1080@30sF",         1920, 1080, 30, 's', 1920, 1080, 2200, 1125, 30,    1,    6, 568, 16, 9, 74250000LL,    1,    0,          0,             0,    -1 },
  { BOF_VIDEO_STANDARD_ID(1920, 1080, 50, 'i'), "1920x1080_50i",  "1920x1080@50i",          1920, 1080, 50, 'i', 1920, 1080, 2640, 1125, 25,    1,    6, 568, 16, 9, 74250000LL,    1,    0x85052001, 0x0FFF0251,  600,     5 },
  { BOF_VIDEO_STANDARD_ID(1920, 1080, 50, 'p'), "1920x1080_50p",  "1920x1080@50p",          1920, 1080, 50, 'p', 1920, 1080, 2640, 1125, 50,    1,    6, 568, 16, 9, 74250000LL,    1,    0x89C98001, 0x000A0119,    0,     8 },
  { BOF_VIDEO_STANDARD_ID(1920, 1080, 59, 'i'), "1920x1080_59i",  "1920x1080@59.94i",       1920, 1080, 59, 'i', 1920, 1080, 2200, 1125, 30000, 1001, 6, 568, 16, 9, 74250000000LL, 1001, 0x85062001, 0x0FFF01F2,  440,     6 },
  { BOF_VIDEO_STANDARD_ID(1920, 1080, 59, 'p'), "1920x1080_59p",  "1920x1080@59.94p",       1920, 1080, 59, 'p', 1920, 1080, 2200, 1125, 60000, 1001, 6, 568, 16, 9, 74250000000LL, 1001, 0x89CA8001, 0x000A011A,    0,     9 },
  { BOF_VIDEO_STANDARD_ID(1920, 1080, 60, 'i'), "1920x1080_60i",  "1920x1080@60i",          1920, 1080, 60, 'i', 1920, 1080, 2200, 1125, 30,    1,    6, 568, 16, 9, 74250000LL,    1,    0x85078001, 0x0FFF01D3,  460,     7 },
  { BOF_VIDEO_STANDARD_ID(1920, 1080, 60, 'p'), "1920x1080_60p",  "1920x1080@60p",          1920, 1080, 60, 'p', 1920, 1080, 2200, 1125, 60,    1,    6, 568, 16, 9, 74250000LL,    1,    0x89CB8001, 0x000A029B,    0,    10 },
  { BOF_VIDEO_STANDARD_ID(3840, 2160, 50, 'p'), "3840x2160_50p",  "3840x2160@50p",          3840, 2160, 50, 'p', 3840, 2160, 4000, 2200, 50,    1,    6, 568, 16, 9, 74250000LL,    1,    0x89CB8001, 0x000AFFFF,    0,    -1 }, // FIXME
  { BOF_VIDEO_STANDARD_ID(3840, 2160, 59, 'p'), "3840x2160_59p",  "3840x2160@59p",          3840, 2160, 59, 'p', 3840, 2160, 4000, 2200, 60000, 1001, 6, 568, 16, 9, 74250000000LL, 1001, 0x89CB8001, 0x000AFFFF,    0,    -1 }, // FIXME
  { BOF_VIDEO_STANDARD_ID(3840, 2160, 60, 'p'), "3840x2160_60p",  "3840x2160@60p",          3840, 2160, 60, 'p', 3840, 2160, 4000, 2200, 60,    1,    6, 568, 16, 9, 74250000LL,    1,    0x89CB8001, 0x000AFFFF,    0,    -1 }  // FIXME
};

int BofVideoStandard::S_FindIndexFromVideoStandardId(const VideoStandardId _Standard)
{
  const Table *ptr = S_mpTable_X;
  for (unsigned int idx = 0; idx < (sizeof(S_mpTable_X) / sizeof(S_mpTable_X[0])); ++idx, ++ptr)
  {
    if (ptr->Id_U32 == _Standard)
    {
      return idx;
    }
  }
  return (-1);
}

int BofVideoStandard::S_FindIndexFromIdText(const char *_pStandard_c)
{
  const Table *ptr = S_mpTable_X;
  for (unsigned int idx = 0; idx < (sizeof(S_mpTable_X) / sizeof(S_mpTable_X[0])); ++idx, ++ptr)
  {
#if defined(_WIN32)
    if (!_stricmp(ptr->pIdText_c, _pStandard_c))
    {
#else
    if (!strcasecmp(ptr->pIdText_c, _pStandard_c))
    {
#endif
      return idx;
    }
  }
  return (-1);
}

int BofVideoStandard::S_FindIndexFromDescription(const char *_pStandard_c)
{
  const Table *ptr = S_mpTable_X;
  for (unsigned int idx = 0; idx < (sizeof(S_mpTable_X) / sizeof(S_mpTable_X[0])); ++idx, ++ptr)
  {
#if defined(_WIN32)
    if (!_stricmp(ptr->pDescription_c, _pStandard_c))
    {
#else
    if (!strcasecmp(ptr->pDescription_c, _pStandard_c))
    {
#endif
      return idx;
    }
  }
  return (-1);
}
int BofVideoStandard::S_FindIndex(const int numCols, const int numRows, const int fps, const char type)
{
  const Table *ptr = S_mpTable_X;
  for (unsigned int idx = 0; idx < (sizeof(S_mpTable_X) / sizeof(S_mpTable_X[0])); ++idx, ++ptr) {
    if (ptr->DescriptionCols_i == numCols) {
      if (ptr->DescriptionRows_i == numRows) {
        if (ptr->DescriptionFps_i == fps) {
          if (ptr->Type_c == type) {
            return idx;
          }
        }
      }
    }
  }
  return (-1);
}

BofVideoStandard BofVideoStandard::FromIndex(const int index)
{
  BofVideoStandard Rts;

  if ((index >= 0) && (index < (int)(sizeof(S_mpTable_X) / sizeof(S_mpTable_X[0]))))
  {
    Rts.mIndex_i = index;
  }
  return Rts;
}

bool BofVideoStandard::S_Parse(const char *standard, int &w, int &h, int &fps, char &type)
{
  w = h = fps = type = 0;
  while (isspace(*standard)) {
    ++standard;
  }
  while (isdigit(*standard)) {
    w = w * 10 + *standard - '0';
    ++standard;
  }
  if ((*standard != 'x') && (*standard != 'X')) {
    return false;
  }
  ++standard;
  while (isdigit(*standard)) {
    h = h * 10 + *standard - '0';
    ++standard;
  }
  if ((*standard != '_') && (*standard != '@')) {
    return false;
  }
  ++standard;
  while (isdigit(*standard)) {
    fps = fps * 10 + *standard - '0';
    ++standard;
  }
  char c = (char)tolower(*standard);
  if ((c != 'i') && (c != 'p') && (c != 's')) {
    return false;
  }
  type = c;
  return true;
}

END_BOF_NAMESPACE()