/*
   Copyright (c) 2000-2006, Sci. All rights reserved.

   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
   PURPOSE.

   This module contains routines for initialising the 2d part of the Bof library
   (Bernard's Object Framework) and various utility functions.

   Name:        Bof2d.cpp
   Author:      Bernard HARMEL: onbings@dscloud.me
   Revision:    1.0

   Rem:         Nothing

   History:

   V 1.00  Sep 30 2000  BHA : Initial release
 */

/*** Include files ***********************************************************/

#include <bofstd/bof2d.h>
#include <bofstd/boffs.h>
#include <string.h>
#include <libyuv.h>

BEGIN_BOF_NAMESPACE()
//YUV Color
const BOF_YUVA GL_pYuvRainbow75_X[] =
                 {
                   {180, 128, 128, 255},     /* 75% white */
                   {162, 44,  142, 255},     /* 75% yellow */
                   {131, 156, 44,  255},     /* 75% cyan */
                   {112, 72,  58,  255},     /* 75% green */
                   {84,  184, 198, 255},     /* 75% magenta */
                   {65,  100, 212, 255},     /* 75% red */
                   {35,  212, 114, 255},     /* 75% blue */
                 };

const BOF_YUVA GL_pYuvRainbow100_X[] =
                 {
                   {235, 128, 128, 255},     /* 100% white */
                   {210, 16,  146, 255},     /* 100% yellow */
                   {170, 166, 16,  255},     /* 100% cyan */
                   {145, 54,  34,  255},     /* 100% green */
                   {106, 202, 222, 255},     /* 100% magenta */
                   {81,  90,  240, 255},     /* 100% red */
                   {41,  240, 110, 255},     /* 100% blue */
                 };

const BOF_YUVA GL_pYuvRainbowHd_X[] =
                 {
                   {180, 128, 128, 255},     /* 75% white */
                   {168, 44,  136, 255},     /* 75% yellow */
                   {145, 147, 44,  255},     /* 75% cyan */
                   {133, 63,  52,  255},     /* 75% green */
                   {63,  193, 204, 255},      /* 75% magenta */
                   {51,  109, 212, 255},      /* 75% red */
                   {28,  212, 120, 255},      /* 75% blue */
                 };

const BOF_YUVA GL_pYuvWobNair_X[] =
                 {
                   {35,  212, 114, 255},     /* 75% blue */
                   {19,  128, 128, 255},     /* 7.5% intensity black */
                   {84,  184, 198, 255},     /* 75% magenta */
                   {19,  128, 128, 255},     /* 7.5% intensity black */
                   {131, 156, 44,  255},     /* 75% cyan */
                   {19,  128, 128, 255},     /* 7.5% intensity black */
                   {180, 128, 128, 255},     /* 75% white */
                 };

const BOF_YUVA GL_YuvWhite_X = {235, 128, 128, 255};

/* pluge pulses */
const BOF_YUVA GL_YuvNeg4Ire_X = {7, 128, 128, 255};
const BOF_YUVA GL_YuvPos4Ire_X = {24, 128, 128, 255};

/* fudged Q/-I */
const BOF_YUVA GL_YuvIPixel_X = {57, 156, 97, 255};
const BOF_YUVA GL_YuvQPixel_X = {44, 171, 147, 255};

const BOF_YUVA GL_YuvGray40_X = {104, 128, 128, 255};
const BOF_YUVA GL_YuvGray15_X = {49, 128, 128, 255};
const BOF_YUVA GL_YuvCyan_X   = {188, 154, 16, 255};
const BOF_YUVA GL_YuvYellow_X = {219, 16, 138, 255};
const BOF_YUVA GL_YuvRed_X    = {63, 102, 240, 255};
const BOF_YUVA GL_YuvGreen_X  = {145, 54, 34, 255};
const BOF_YUVA GL_YuvBlue_X   = {32, 240, 118, 255};
const BOF_YUVA GL_YuvBlack0_X = {16, 128, 128, 255};
const BOF_YUVA GL_YuvBlack2_X = {20, 128, 128, 255};
const BOF_YUVA GL_YuvBlack4_X = {25, 128, 128, 255};
const BOF_YUVA GL_YuvNeg2_X   = {12, 128, 128, 255};

/*** LookForZoneCoordinate ***************************************************************/

/*!
   Description
   The LookForZoneCoordinate function scans a pixel buffer looking for a pixel color. It is
   used to define rectangular zone inside a picture

   Parameters
   _rRect_X: Specifies the rectangular zone to cover inside the pixel buffer and returns the rectangle found.
   This one is defined by the upper Left_S32 and lower Right_S32 corner.
   _BitCount_U32: Specifies the number of bits per pixel
   _pData: Specifies the pixel buffer address
   _Pitch_U32: Specifies the pixel buffer pitch
   _Color_U32: Specifies the pixel color to look for

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */
 

BOFERR Bof_LookForZoneCoordinate(BOF_RECT &_rRect_X, uint32_t _BitCount_U32, void *_pData, uint32_t _Pitch_U32, uint32_t _Color_U32)
{
  bool     Rts_B = false;
  uint32_t x_U32 = 0, y_U32, *p_U32, Offset_U32, Width_U32, Height_U32, Left_U32, Top_U32;
//	uint16_t *p_U16;	// , Color_U16;
//	uint8_t  *p_UB;	// , Color_UB;

  if (_pData)
  {
    //	p_UB       = (uint8_t *)_pData;
//		Color_UB   = (uint8_t)_Color_U32;
    //	p_U16      = (uint16_t *)_pData;
    //Color_U16  = (uint16_t)_Color_U32;
    p_U32      = (uint32_t *) _pData;
    Width_U32  = (_rRect_X.Right_S32 - _rRect_X.Left_S32); // Compatible with rect definition Right_S32,Bottom_S32 is outside of the rect
    Height_U32 = (_rRect_X.Bottom_S32 - _rRect_X.Top_S32);
    Left_U32   = _rRect_X.Left_S32;
    Top_U32    = _rRect_X.Top_S32;
    switch (_BitCount_U32)
    {
      case 8:
//				p_UB  += ( (_rRect_X.Top_S32 * _Pitch_U32) + _rRect_X.Left_S32);
        break;

      case 16:
//				p_U16 += ( ( (_rRect_X.Top_S32 * _Pitch_U32) + (_rRect_X.Left_S32 << 1) ) >> 1);
        break;

      case 24:
        p_U32 = (uint32_t *) ((uint8_t *) p_U32 + ((_rRect_X.Top_S32 * _Pitch_U32) + (_rRect_X.Left_S32 * 3)));
        break;

      default:
      case 32:
        p_U32 += (((_rRect_X.Top_S32 * _Pitch_U32) + (_rRect_X.Left_S32 << 2)) >> 2);
        break;
    }

    switch (_BitCount_U32)
    {
      case 8:
        Offset_U32 = _Pitch_U32 - Width_U32;
        break;

      case 16:
        Offset_U32 = _Pitch_U32 - (Width_U32 << 1);
        break;

      case 24:
        Offset_U32 = _Pitch_U32 - (Width_U32 * 3);
        break;

      default:
      case 32:
        Offset_U32 = _Pitch_U32 - (Width_U32 << 2);
        break;
    }
    // Get Top_S32,Left_S32
    for (y_U32 = 0; y_U32 < Height_U32; y_U32++)
    {
      switch (_BitCount_U32)
      {
        case 32:
          for (x_U32 = 0; x_U32 < Width_U32; x_U32++, p_U32++)
          {
            if (*p_U32 == _Color_U32)
            {
              Rts_B = true;
              _rRect_X.Top_S32  = Top_U32 + y_U32;
              _rRect_X.Left_S32 = Left_U32 + x_U32;
              p_U32 = (uint32_t *) ((uint8_t *) p_U32 + _Pitch_U32);
              break;
            }
          }
          if (!Rts_B)
          { p_U32 = (uint32_t *) ((uint8_t *) p_U32 + Offset_U32); }
          break;

        case 24:
          for (x_U32 = 0; x_U32 < Width_U32; x_U32++)
          {
            if ((x_U32 < (Width_U32 - 1))
                || (y_U32 < (Height_U32 - 1))
              )                     // Avoid bus error on last pixel (.net)
            {
              if ((*p_U32 & 0x00FFFFFFFF) == _Color_U32)
              {
                Rts_B = true;
                _rRect_X.Top_S32  = Top_U32 + y_U32;
                _rRect_X.Left_S32 = Left_U32 + x_U32;
                p_U32 = (uint32_t *) ((uint8_t *) p_U32 + _Pitch_U32);
                break;
              }
              p_U32 = (uint32_t *) ((uint8_t *) p_U32 + 3);
            }
          }
          if (!Rts_B)
          { p_U32 = (uint32_t *) ((uint8_t *) p_U32 + Offset_U32); }
          break;

        default:
          break;
      }
      if (Rts_B)
      { break; }
    }

    if (Rts_B)
    {
      Rts_B = false;
      // Get Bottom_S32
      for (y_U32++; y_U32 < Height_U32; y_U32++)
      {
        switch (_BitCount_U32)
        {
          default:
          case 32:
            if ((*p_U32 & 0x00FFFFFFFF) == _Color_U32)
            {
              Rts_B = true;
              _rRect_X.Bottom_S32 = Top_U32 + y_U32;
              p_U32++;
            }
            else
            { p_U32 = (uint32_t *) ((uint8_t *) p_U32 + _Pitch_U32); }
            break;

          case 24:
            if ((*p_U32 & 0x00FFFFFFFF) == _Color_U32)
            {
              Rts_B = true;
              _rRect_X.Bottom_S32 = Top_U32 + y_U32;
              p_U32 = (uint32_t *) ((uint8_t *) p_U32 + 3);
            }
            else
            { p_U32 = (uint32_t *) ((uint8_t *) p_U32 + _Pitch_U32); }
            break;
        }
        if (Rts_B)
        { break; }
      }
    }
    if (Rts_B)
    {
      Rts_B = false;
      // Get Right_S32
      for (x_U32++; x_U32 < Width_U32; x_U32++)
      {
        switch (_BitCount_U32)
        {
          default:
          case 32:
            if (*p_U32 == _Color_U32)
            {
              Rts_B = true;
              _rRect_X.Right_S32 = Left_U32 + x_U32;
            }
            else
            { p_U32++; }
            break;

          case 24:
            if ((*p_U32 & 0x00FFFFFFFF) == _Color_U32)
            {
              Rts_B = true;
              _rRect_X.Right_S32 = Left_U32 + x_U32;
            }
            else
            { p_U32 = (uint32_t *) ((uint8_t *) p_U32 + _Pitch_U32); }
            break;
        }
        if (Rts_B)
        { break; }
      }
    }
  }
  return (Rts_B ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL);
}

/*** EraseZoneBorder ***************************************************************/

/*!
   Description
   The EraseZoneBorder function erase the rectangular delimiter present in a pixel buffer

   Parameters
   _rRect_X: Specifies the rectangular zone to erase inside the pixel buffer.
   _BitCount_U32: Specifies the number of bits per pixel
   _pData: Specifies the pixel buffer address
   _Pitch_U32: Specifies the pixel buffer pitch

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */

BOFERR Bof_EraseZoneBorder(BOF_RECT &_rRect_X, uint32_t _BitCount_U32, void *_pData, uint32_t _Pitch_U32)
{
  bool     Rts_B = false;
  uint32_t *p_U32;
//	uint16_t *p_U16;
//	uint8_t  *p_UB;

  if (_pData)
  {
    //	p_UB  = (uint8_t *)_pData;
    //	p_U16 = (uint16_t *)_pData;
    p_U32 = (uint32_t *) _pData;

    switch (_BitCount_U32)
    {
      case 8:
        //		p_UB  += ( (_rRect_X.Top_S32 * _Pitch_U32) + _rRect_X.Left_S32);
        break;

      case 16:
        //		p_U16 += ( ( (_rRect_X.Top_S32 * _Pitch_U32) + (_rRect_X.Left_S32 << 1) ) >> 1);
        break;

      case 24:
        p_U32 = (uint32_t *) ((uint8_t *) p_U32 + ((_rRect_X.Top_S32 * _Pitch_U32) + (_rRect_X.Left_S32 * 3)));
        break;
      default:
      case 32:
        p_U32 += (((_rRect_X.Top_S32 * _Pitch_U32) + (_rRect_X.Left_S32 << 2)) >> 2);
        break;
    }
    switch (_BitCount_U32)
    {
      case 32:
        *p_U32 = *(p_U32 + 1);
        p_U32 = (uint32_t *) ((uint8_t *) p_U32 + ((_rRect_X.Bottom_S32 - _rRect_X.Top_S32) * _Pitch_U32));
        *p_U32 = *(p_U32 + 1);
        p_U32 += (_rRect_X.Right_S32 - _rRect_X.Left_S32);
        *p_U32 = *(p_U32 - 1);
        break;

      default:
        break;
    }
    Rts_B = true;
  }
  return (Rts_B ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL);

}



/*** DecimateGraphicData ***************************************************************/

/*!
   Description
   The DecimateGraphicData function compute a downscaled picture from a full scaled one.

   Parameters
   _BytePerPixel_UB: Specifies the number of byte per pixel
   _pData_UB: Specifies a pointer to the buffer which will contain the data read.
   _Width_U32: Specifies the graphic width
   _Height_U32: Specifies the graphic height
   _DecimationStepX_f: Specifies the X scaling factor (must be >= 1.0f)
   _DecimationStepY_f: Specifies the Y scaling factor (must be >= 1.0f)

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */

BOFERR Bof_DecimateGraphicData(uint8_t _BytePerPixel_UB, uint8_t *_pData_UB, uint32_t _Width_U32, uint32_t _Height_U32, float _DecimationStepX_f, float _DecimationStepY_f)
{
  bool     Rts_B = false;
  uint8_t  *p_UB;
  uint16_t *p_U16;
  uint32_t *p_U32;
  float    r_F, c_F, Index_F;

  if ((_pData_UB)
      && (_DecimationStepX_f >= 1.0f)
      && (_DecimationStepY_f >= 1.0f)
    )
  {
    Rts_B = true;
    if ((_DecimationStepX_f != 1.0f)
        || (_DecimationStepY_f != 1.0f)
      )
    {
      p_UB  = _pData_UB;
      p_U16 = (uint16_t *) _pData_UB;
      p_U32 = (uint32_t *) _pData_UB;
      switch (_BytePerPixel_UB)
      {
        case 1:
          for (r_F = 0.0f; (uint32_t) (r_F + 0.5f) < _Height_U32; r_F += _DecimationStepY_f)
          {
            Index_F  = (float) ((uint32_t) r_F * _Width_U32);
            for (c_F = 0.0f; (uint32_t) (c_F + 0.5f) < _Width_U32; c_F += _DecimationStepX_f)
            {
              *p_UB++ = _pData_UB[(uint32_t) (Index_F + 0.5f)];
              Index_F += _DecimationStepX_f;
            }
          }
          break;

        case 2:
          for (r_F = 0.0f; (uint32_t) (r_F + 0.5f) < _Height_U32; r_F += _DecimationStepY_f)
          {
            Index_F  = (float) ((uint32_t) r_F * _Width_U32);
            for (c_F = 0.0f; (uint32_t) (c_F + 0.5f) < _Width_U32; c_F += _DecimationStepX_f)
            {
              *p_U16++ = ((uint16_t *) _pData_UB)[(uint32_t) (Index_F + 0.5f)];
              Index_F += _DecimationStepX_f;
            }
          }
          break;

        case 4:
          for (r_F = 0.0f; (uint32_t) (r_F + 0.5f) < _Height_U32; r_F += _DecimationStepY_f)
          {
            Index_F  = (float) ((uint32_t) r_F * _Width_U32);
            for (c_F = 0.0f; (uint32_t) (c_F + 0.5f) < _Width_U32; c_F += _DecimationStepX_f)
            {
              *p_U32++ = ((uint32_t *) _pData_UB)[(uint32_t) (Index_F + 0.5f)];
              Index_F += _DecimationStepX_f;
            }
          }
          break;

        default:
          Rts_B = false;
          break;
      }
    }
  }
  return (Rts_B ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL);
}

/*** LookForColor ***************************************************************/

/*!
   Description
   The LookForColor function scans a color table looking for a particula value

   Parameters
   _NbColorEntry_U32: Specifies the number of colors in the pColorTable_X array
   _pColorTable_X: Specifies the color table array
   _Color_X: Specifies the color to look for
   _pColorEntry_U32: \Returns the color table entry found (-1 if no match)

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */

BOFERR Bof_LookForColor(uint32_t _NbColorEntry_U32, BOF_PALETTE_ENTRY *_pColorTable_X, BOF_PALETTE_ENTRY _Color_X, uint32_t *_pColorEntry_U32)
{
  bool     Rts_B = false;
  uint32_t i_U32;

  if ((_pColorTable_X) && (_pColorEntry_U32))
  {
    *_pColorEntry_U32 = (uint32_t) -1;
    for (i_U32 = 0; i_U32 < _NbColorEntry_U32; i_U32++, _pColorTable_X++)
    {
      if ((_pColorTable_X->r_U8 == _Color_X.r_U8)
          && (_pColorTable_X->g_U8 == _Color_X.g_U8)
          && (_pColorTable_X->b_U8 == _Color_X.b_U8)
        )
      {
        Rts_B = true;
        *_pColorEntry_U32 = i_U32;
        break;
      }
    }
  }
  return (Rts_B ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL);
}

/*** SwapColorEntries ***************************************************************/

/*!
   Description
   The SwapColorEntries function parse a paletized picture data buffer and swap two
   pixel color (For example each red pixel is turned into blue and each blue pixel is turned into red)

   Parameters
   _BytePerPixel_UB: Specifies the number of byte per pixel
   _pData_UB: Specifies a pointer to the buffer which will contain the data read.
   _Width_U32: Specifies the graphic width
   _Height_U32: Specifies the graphic height
   _ColorEntry1_U32. Specifies the first color to swap
   _ColorEntry2_U32. Specifies the first color to swap

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */

BOFERR Bof_SwapColorEntries(uint8_t _BytePerPixel_UB, uint8_t *_pData_UB, uint32_t _Width_U32, uint32_t _Height_U32, uint32_t _ColorEntry1_U32, uint32_t _ColorEntry2_U32)
{
  bool     Rts_B = false;
  uint32_t i_U32, j_U32;
  uint8_t  *p_UB;
  uint16_t *p_U16;
  uint32_t *p_U32;

  if (_pData_UB)
  {
    Rts_B = true;
    if (_ColorEntry1_U32 != _ColorEntry2_U32)
    {
      p_UB  = _pData_UB;
      p_U16 = (uint16_t *) _pData_UB;
      p_U32 = (uint32_t *) _pData_UB;
      switch (_BytePerPixel_UB)
      {
        case 1:
          for (i_U32 = 0; i_U32 < _Height_U32; i_U32++)
          {
            for (j_U32 = 0; j_U32 < _Width_U32; j_U32++, p_UB++)
            {
              if (*p_UB == (uint8_t) _ColorEntry1_U32)
              { *p_UB = (uint8_t) _ColorEntry2_U32; }
              else if (*p_UB == (uint8_t) _ColorEntry2_U32)
              { *p_UB = (uint8_t) _ColorEntry1_U32; }
            }
          }
          break;

        case 2:
          for (i_U32 = 0; i_U32 < _Height_U32; i_U32++)
          {
            for (j_U32 = 0; j_U32 < _Width_U32; j_U32++, p_U16++)
            {
              if (*p_U16 == (uint16_t) _ColorEntry1_U32)
              { *p_U16 = (uint16_t) _ColorEntry2_U32; }
              else if (*p_U16 == (uint16_t) _ColorEntry2_U32)
              { *p_U16 = (uint16_t) _ColorEntry1_U32; }
            }
          }
          break;

        case 4:
          for (i_U32 = 0; i_U32 < _Height_U32; i_U32++)
          {
            for (j_U32 = 0; j_U32 < _Width_U32; j_U32++, p_U32++)
            {
              if (*p_U32 == _ColorEntry1_U32)
              { *p_U32 = _ColorEntry2_U32; }
              else if (*p_U32 == _ColorEntry2_U32)
              { *p_U32 = _ColorEntry1_U32; }
            }
          }
          break;

        default:
          Rts_B = false;
          break;
      }
    }
  }
  return (Rts_B ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL);
}


/*** LoadGraphicFile ***************************************************************/

/*!
   Description
   The LoadGraphicFile function loads a .bmp or .tga graphic file

   Parameters
   _rPath: Specifies the graphic filename to open
   _rBmInfo_X: \Returns the graphic file characteristics
   _pPaletteEntry_X: if not nullptr \returns palette table
   _pData_UB: if nullptr just returns the graphic file characteristics in pBmInfo_X
   and _pPaletteEntry_X otherwise store the graphic pixel data
   _LoadIndex_U32: Return the graphic data stored at offset ((FileHEaderSize)+RecordIndex_U32*(*pSize_U32)).
   This feature is usefull to load big graphic file in a small buffer in several passes
   _rSize_U32:   Specifies and returns the number of data read inside the pData_UB buffer. If nullptr
   the buffer is supposed to be large enought to read the data.

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */

BOFERR Bof_LoadGraphicFile(BofPath &_rPath, BOF_BITMAP_INFO_HEADER &_rBmInfo_X, BOF_PALETTE_ENTRY *_pPaletteEntry_X, uint8_t *pData_UB, uint32_t _LoadIndex_U32, uint32_t &_rSize_U32)
{
  BOFERR         Rts_E;
  BOF_TGA_HEADER TgaHeader_X;

  Rts_E = Bof_LoadBmpFile(_rPath, _rBmInfo_X, _pPaletteEntry_X, pData_UB, _LoadIndex_U32, _rSize_U32);
  if (Rts_E != BOF_ERR_NO_ERROR)
  {
    Rts_E = Bof_LoadTgaFile(_rPath, TgaHeader_X, _pPaletteEntry_X, pData_UB, _LoadIndex_U32, _rSize_U32);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      _rBmInfo_X.Reset();
      _rBmInfo_X.Size_U32      = sizeof(BOF_BITMAP_INFO_HEADER);
      _rBmInfo_X.BitCount_U16  = TgaHeader_X.Bits_U8;
      _rBmInfo_X.Height_S32    = TgaHeader_X.Height_U16;
      _rBmInfo_X.Planes_U16    = 1;
      _rBmInfo_X.Width_S32     = TgaHeader_X.Width_U16;
      _rBmInfo_X.ClrUsed_U32   = TgaHeader_X.ColourMapLength_U16;
      _rBmInfo_X.SizeImage_U32 = (TgaHeader_X.Width_U16 * TgaHeader_X.Height_U16 * (TgaHeader_X.Bits_U8 >> 3));
    }
  }
  return (Rts_E);
}

/*** LoadBmpFile ***************************************************************/

/*!
   Description
   The LoadBmpFile function loads a .bmp graphic file

   Parameters
   _rPath: Specifies the graphic filename to open
   _rBmInfo_X: \Returns the graphic file characteristics
   _pPaletteEntry_X: if not nullptr \returns palette table if needed
   _pData_UB: if nullptr just returns the graphic file characteristics in pBmInfo_X
   and _pPaletteEntry_X otherwise store the graphic pixel data
   _LoadIndex_U32: Return the graphic data stored at offset ((FileHEaderSize)+RecordIndex_U32*(*pSize_U32)).
   This feature is usefull to load big graphic file in a small buffer in several passes
   _rSize_U32:   Specifies and returns the number of data read inside the pData_UB buffer. If nullptr
   the buffer is supposed to be large enought to read the data.

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */

BOFERR Bof_LoadBmpFile(BofPath &_rPath, BOF_BITMAP_INFO_HEADER &_rBmInfo_X, BOF_PALETTE_ENTRY *_pPaletteEntry_X, uint8_t *_pData_UB, uint32_t _LoadIndex_U32, uint32_t &_rSize_U32)
{
  BOFERR                 Rts_E;
  intptr_t               Io;
  uint32_t               i_U32, Width_U32;
  uint32_t               Nb_U32, Size_U32;
  uint8_t                Val_UB;
  BOF_BITMAP_FILE_HEADER BmFile_X;

  Rts_E = Bof_OpenFile(_rPath, true, Io);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Nb_U32 = sizeof(BOF_BITMAP_FILE_HEADER);
    Rts_E  = Bof_ReadFile(Io, Nb_U32, reinterpret_cast<uint8_t *>(&BmFile_X));
    if ((Rts_E == BOF_ERR_NO_ERROR)
        && (Nb_U32 == sizeof(BOF_BITMAP_FILE_HEADER))
        && (BmFile_X.Type_U16 == 0x4D42)              // 'BM
      )
    {
      Nb_U32 = sizeof(BOF_BITMAP_INFO_HEADER);
      Rts_E  = Bof_ReadFile(Io, Nb_U32, reinterpret_cast<uint8_t *>(&_rBmInfo_X));
      if ((Rts_E == BOF_ERR_NO_ERROR)
          && (Nb_U32 == sizeof(BOF_BITMAP_INFO_HEADER))
          && (_rBmInfo_X.Size_U32 == sizeof(BOF_BITMAP_INFO_HEADER))
          && (_rBmInfo_X.Compression_U32 == BOF_BI_RGB)
          && (_rBmInfo_X.Planes_U16 == 1)
        )
      {
        if (_rBmInfo_X.BitCount_U16 == 8)
        {
          if (!_rBmInfo_X.ClrUsed_U32)
          { _rBmInfo_X.ClrUsed_U32 = 256; }
          Size_U32 = _rBmInfo_X.ClrUsed_U32 * static_cast<uint32_t>(sizeof(BOF_PALETTE_ENTRY));
          if (_pPaletteEntry_X)
          {
            Nb_U32 = Size_U32;
            Rts_E  = Bof_ReadFile(Io, Nb_U32, reinterpret_cast<uint8_t *>(_pPaletteEntry_X));
            if ((Rts_E == BOF_ERR_NO_ERROR)
                && (Nb_U32 == Size_U32)
              )
            {
              // the palette is stored as RGBQUAD array instead of BOF_PALETTE_ENTRY->swap r & b
              for (i_U32 = 0; i_U32 < _rBmInfo_X.ClrUsed_U32; i_U32++)
              {
                Val_UB = _pPaletteEntry_X[i_U32].b_U8;
                _pPaletteEntry_X[i_U32].b_U8   = _pPaletteEntry_X[i_U32].r_U8;
                _pPaletteEntry_X[i_U32].r_U8   = Val_UB;
                _pPaletteEntry_X[i_U32].Flg_U8 = 0xFF; // Opaque
              }
            }
            else
            {
              Rts_E = BOF_ERR_FORMAT;
            }
          }
          else
          {
            Rts_E = (Bof_SetFileIoPosition(Io, Size_U32, BOF_SEEK_METHOD::BOF_SEEK_CURRENT) != (int64_t) -1) ? BOF_ERR_NO_ERROR : BOF_ERR_SEEK;
          }
        }
        else
        {
          Rts_E = BOF_ERR_NO_ERROR;
        }
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          // Bitmap line length are aligned on 4 bytes
          Width_U32 = (_rBmInfo_X.BitCount_U16 >= 8) ? _rBmInfo_X.Width_S32 * (_rBmInfo_X.BitCount_U16 >> 3) : _rBmInfo_X.Width_S32 * (8 / _rBmInfo_X.BitCount_U16);
          if (_rBmInfo_X.Width_S32 & 0x03)
          {
            Width_U32 += (4 - (_rBmInfo_X.Width_S32 & 0x03));
          }
          _rBmInfo_X.Width_S32 = Width_U32 / (_rBmInfo_X.BitCount_U16 >> 3);

          Rts_E = Bof_ReadGraphicFile(Io, _LoadIndex_U32, (_rBmInfo_X.Height_S32 < 0) ? true : false, (uint8_t) _rBmInfo_X.BitCount_U16, _pData_UB, _rBmInfo_X.Width_S32, _rBmInfo_X.Height_S32,
                                      &_rSize_U32);
        }
      }
      else
      {
        Rts_E = BOF_ERR_FORMAT;
      }
    }
    else
    {
      Rts_E = BOF_ERR_FORMAT;
    }
    Bof_CloseFile(Io);
  }
  return (Rts_E);
}
/*** LoadTgaFile ***************************************************************/

/*!
   Description
   The LoadTgaFile function loads a .tga graphic file

   Parameters
   _rPath: Specifies the graphic filename to open
   _rTgaHeader_X: \Returns the graphic file characteristics
   _pPaletteEntry_X: if not nullptr \returns palette table if needed
   _pData_UB: if nullptr just returns the graphic file characteristics in pTgaHeader_X
   and _pPaletteEntry_X otherwise store the graphic pixel data
   _LoadIndex_U32: Return the graphic data stored at offset ((FileHEaderSize)+RecordIndex_U32*(*pSize_U32)).
   This feature is usefull to load big graphic file in a small buffer in several passes
   _rSize_U32:   Specifies and returns the number of data read inside the pData_UB buffer. If nullptr
   the buffer is supposed to be large enough to read the data.

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */

BOFERR Bof_LoadTgaFile(BofPath &_rPath, BOF_TGA_HEADER &_rTgaHeader_X, BOF_PALETTE_ENTRY *_pPaletteEntry_X, uint8_t *_pData_UB, uint32_t _LoadIndex_U32, uint32_t &_rSize_U32)
{
  BOFERR   Rts_E;
  intptr_t Io;
  uint32_t i_U32;
  uint32_t Nb_U32, Size_U32;
  uint8_t  pPalette_UB[8];

  Rts_E = Bof_OpenFile(_rPath, true, Io);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Nb_U32 = sizeof(BOF_TGA_HEADER);

    Rts_E = Bof_ReadFile(Io, Nb_U32, reinterpret_cast<uint8_t *>(&_rTgaHeader_X));
    if ((Rts_E == BOF_ERR_NO_ERROR)
        && (Nb_U32 == sizeof(BOF_TGA_HEADER))
        && (_rTgaHeader_X.ColourMapType_U8 <= 8)
      )
    {
      Rts_E = (Bof_SetFileIoPosition(Io, _rTgaHeader_X.IdentSize_U8, BOF_SEEK_METHOD::BOF_SEEK_CURRENT) != (int64_t) -1) ? BOF_ERR_NO_ERROR : BOF_ERR_SEEK;
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        if (_rTgaHeader_X.ColourMapType_U8)
        {
          Size_U32 = (_rTgaHeader_X.ColourMapBits_U8 >> 3);
          if (_pPaletteEntry_X)
          {
            for (i_U32 = 0; i_U32 < _rTgaHeader_X.ColourMapLength_U16; i_U32++, _pPaletteEntry_X++)
            {
              Nb_U32 = Size_U32;
              Rts_E  = Bof_ReadFile(Io, Nb_U32, pPalette_UB);
              if ((Rts_E == BOF_ERR_NO_ERROR)
                  && (Nb_U32 == Size_U32)
                )
              {
                switch (Size_U32)
                {
                  case 3:
                    _pPaletteEntry_X->r_U8   = pPalette_UB[2];
                    _pPaletteEntry_X->g_U8   = pPalette_UB[1];
                    _pPaletteEntry_X->b_U8   = pPalette_UB[0];
                    _pPaletteEntry_X->Flg_U8 = 0xFF;
                    break;

                  case 4:
                    _pPaletteEntry_X->r_U8   = pPalette_UB[2];
                    _pPaletteEntry_X->g_U8   = pPalette_UB[1];
                    _pPaletteEntry_X->b_U8   = pPalette_UB[0];
                    _pPaletteEntry_X->Flg_U8 = pPalette_UB[3];
                    break;

                  default:
                    Rts_E = BOF_ERR_EINVAL;
                    break;
                }
              }
              else
              {
                Rts_E = BOF_ERR_READ;
              }
              if (Rts_E != BOF_ERR_NO_ERROR)
              {
                break;
              }
            }
          }
          else
          {
            Rts_E = (Bof_SetFileIoPosition(Io, (Size_U32 * _rTgaHeader_X.ColourMapLength_U16), BOF_SEEK_METHOD::BOF_SEEK_CURRENT) != (int64_t) -1) ? BOF_ERR_NO_ERROR : BOF_ERR_SEEK;
          }
        }
        else
        {
          Rts_E = BOF_ERR_NO_ERROR;
        }
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          if (_rTgaHeader_X.ImageType_U8 > 3)
          {
            Rts_E = BOF_ERR_BAD_TYPE;   // Accept only uncompressed image
          }
          else
          {
            Rts_E = Bof_ReadGraphicFile(Io, _LoadIndex_U32, (_rTgaHeader_X.Descriptor_X.Origin_UB != 0) ? true : false, static_cast<uint8_t>(_rTgaHeader_X.Bits_U8 >> 3), _pData_UB,
                                        _rTgaHeader_X.Width_U16, _rTgaHeader_X.Height_U16, &_rSize_U32);
          }
        }
      }
    }
    else
    {
      Rts_E = BOF_ERR_FORMAT;
    }
    Bof_CloseFile(Io);
  }

  return (Rts_E);
}

/*** GenerateTgaFile ***************************************************************/

/*!
   Description
   The GenerateTgaFile function creates a dummy .tga graphic file

   Parameters
   pFn_c: Specifies the graphic filename to create
   Width_U32: Specifies the graphic width
   Height_U32: Specifies the graphic height

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */


BOFERR Bof_GenerateTgaFile(BofPath &_rPath, uint32_t _Width_U32, uint32_t _Height_U32)
{
  BOF_TGA_HEADER TgaHeader_X;
  BOFERR         Rts_E = BOF_ERR_ENOMEM;
  intptr_t       Io;
  uint32_t       x_U32, y_U32, v_U32, *p_U32, w_U32;
  uint32_t       Nb_U32, Size_U32;
  uint8_t        *pData_UB;

  Size_U32      = _Height_U32 * (_Width_U32 << 2);
  if ((pData_UB = new uint8_t[(uint32_t) Size_U32]) != nullptr)
  {
    Rts_E = Bof_CreateFile(BOF_FILE_PERMISSION_ALL_FOR_ALL, _rPath, false, Io);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      TgaHeader_X.Reset();
      TgaHeader_X.Bits_U8                = 32;
      TgaHeader_X.ImageType_U8           = 2;
      TgaHeader_X.Descriptor_X.Origin_UB = 1;
      TgaHeader_X.Height_U16             = (uint16_t) _Height_U32;
      TgaHeader_X.Width_U16              = (uint16_t) _Width_U32;
      Nb_U32 = sizeof(BOF_TGA_HEADER);
      Rts_E  = Bof_WriteFile(Io, Nb_U32, reinterpret_cast<const uint8_t *>(&TgaHeader_X));
      if ((Rts_E == BOF_ERR_NO_ERROR)
          && (Nb_U32 == sizeof(BOF_TGA_HEADER))
        )
      {
        Rts_E = (Bof_SetFileIoPosition(Io, TgaHeader_X.IdentSize_U8, BOF_SEEK_METHOD::BOF_SEEK_CURRENT) != (int64_t) -1) ? BOF_ERR_NO_ERROR : BOF_ERR_SEEK;
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          memset(pData_UB, 0, (size_t) Size_U32);
          p_U32      = (uint32_t *) pData_UB;
          for (y_U32 = 0; y_U32 < TgaHeader_X.Height_U16; y_U32++, p_U32 += _Width_U32)
          {
            w_U32      = TgaHeader_X.Width_U16 - 1;
            v_U32      = 0xFFFF0000;
            for (x_U32 = 0; x_U32 < w_U32; x_U32++)
            {
              if (x_U32 == (w_U32 - 1))
              {
                v_U32 &= 0x00FFFFFF;
                v_U32 |= 0x80000000;
              }
              p_U32[x_U32] = v_U32;
            }
          }
          Nb_U32 = Size_U32;
          Rts_E  = Bof_WriteFile(Io, Nb_U32, pData_UB);
          if ((Rts_E == BOF_ERR_NO_ERROR)
              && (Nb_U32 == Size_U32)
            )
          { }
          else
          {
            Rts_E = BOF_ERR_WRITE;
          }
        }
      }
      else
      {
        Rts_E = BOF_ERR_READ;
      }
      Bof_CloseFile(Io);
    }
    BOF_SAFE_DELETE(pData_UB);
//		delete pData_UB;
  }
  return (Rts_E);
}

/*** ReadGraphicFile ***************************************************************/

/*!
   Description
   The ReadGraphicFile function reads the data part of a tga or bmp file. These one can be
   Top_S32 down or down Top_S32 oriented

   Parameters
   Io: Specifies the file handle. The file read pointer must be located on the first byte of
   the graphic file data zone
   _RecordIndex_U32: Specifies the record index inside the file data zone to access. The file data zone is view as a list of
   successive binary record of length
   TopDown_B: true if the data byte in the file are Top_S32 down oriented
   _BytePerPixel_UB: Specifies the number of byte per pixel
   _pData_UB: Specifies a pointer to the buffer which will contain the data read.
   _Width_U32: Specifies the graphic width
   _Height_U32: Specifies the graphic height
   _rRecordSize_U32. Specifies the size of a data record. This information is used with RecordIndex_U32 and
   the TopDown_B argument to compute the file offset to read
   The data really read are also returned to the called via this pointer

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */

BOFERR Bof_ReadGraphicFile(intptr_t Io, uint32_t _RecordIndex_U32, bool _TopDown_B, uint8_t _BytePerPixel_UB, uint8_t *_pData_UB, uint32_t _Width_U32, uint32_t _Height_U32, uint32_t *_pRecordSize_U32)
{
  BOFERR   Rts_E = BOF_ERR_EINVAL;
  uint32_t NbLineToRead_U32, i_U32, LineInFile_U32, WidthInByte_U32;
  uint32_t Size_U32, Nb_U32;
  uint8_t  *p_UB;

  if (_pData_UB)
  {
    WidthInByte_U32 = _Width_U32 * _BytePerPixel_UB;
    if (_pRecordSize_U32)
    {
      NbLineToRead_U32 = (*_pRecordSize_U32 / WidthInByte_U32);
      if (NbLineToRead_U32 > _Height_U32)
      { NbLineToRead_U32 = _Height_U32; }

      if (_TopDown_B)
      {
        LineInFile_U32 = _RecordIndex_U32 * NbLineToRead_U32;
        if (LineInFile_U32 < _Height_U32)
        {
          if ((LineInFile_U32 + NbLineToRead_U32) >= _Height_U32)
          {
            NbLineToRead_U32 = _Height_U32 - LineInFile_U32;
          }
        }
      }
      else
      {
        LineInFile_U32 = _Height_U32 - ((_RecordIndex_U32 + 1) * NbLineToRead_U32);
        if (LineInFile_U32 & 0x80000000)
        {
          NbLineToRead_U32 = _Height_U32 + LineInFile_U32; // LineInFile_U32 is <0
          LineInFile_U32   = 0;
        }
      }

      if ((!(LineInFile_U32 & 0x80000000))
          && (!(NbLineToRead_U32 & 0x80000000))
          && (LineInFile_U32 < _Height_U32)
        )
      {
        Rts_E = (Bof_SetFileIoPosition(Io, LineInFile_U32 * WidthInByte_U32, BOF_SEEK_METHOD::BOF_SEEK_CURRENT) != (int64_t) -1) ? BOF_ERR_NO_ERROR : BOF_ERR_SEEK;
      }
      *_pRecordSize_U32 = 0;
    }
    else
    {
      NbLineToRead_U32 = _Height_U32;
      Rts_E            = BOF_ERR_NO_ERROR;
    }

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Size_U32 = 0;
      if (_TopDown_B)
      {
        Size_U32 = WidthInByte_U32 * NbLineToRead_U32;
        Nb_U32   = Size_U32;
        Rts_E    = Bof_ReadFile(Io, Nb_U32, _pData_UB);
        if ((Rts_E == BOF_ERR_NO_ERROR)
            && (Nb_U32 == Size_U32)
          )
        { }
        else
        {
          Rts_E = BOF_ERR_READ;
        }
      }
      else
      {
        Nb_U32     = 0;
        p_UB       = (_pData_UB) + ((NbLineToRead_U32 - 1) * WidthInByte_U32);
        for (i_U32 = 0; i_U32 < NbLineToRead_U32; i_U32++)
        {
          Nb_U32 = WidthInByte_U32;
          if ((Bof_ReadFile(Io, Nb_U32, p_UB) != 0)
              || (Nb_U32 != WidthInByte_U32)
            )
          {
            Size_U32 += Nb_U32;
            break;
          }
          else
          {
            p_UB -= WidthInByte_U32;
          }
          Size_U32 += Nb_U32;
        }
        Rts_E      = (i_U32 == NbLineToRead_U32) ? BOF_ERR_NO_ERROR : BOF_ERR_READ;
      }
      if (_pRecordSize_U32)
      {
        *_pRecordSize_U32 = (uint32_t) Size_U32;
      }
    }
  }
  else
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return (Rts_E);
}

// Pick a method for rounding.
#define ROUND(f) static_cast<int>(f + 0.5f)
static uint8_t RoundToByte(float f)
{
  int i = ROUND(f);
  return (i < 0) ? 0 : ((i > 255) ? 255 : static_cast<uint8_t>(i));
}
//From /home/bha/pingu/bofstd/third/libyuv/unit_test/color_test.cc
// BT.601 YUV to RGB reference

void Bof_YuvToRgbReference(int y, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b)
{
  *r = RoundToByte(static_cast<float>(y - 16) * 1.164f - static_cast<float>(v - 128) * -1.596f);
  *g = RoundToByte(static_cast<float>(y - 16) * 1.164f - static_cast<float>(u - 128) * 0.391f - static_cast<float>(v - 128) * 0.813f);
  *b = RoundToByte(static_cast<float>(y - 16) * 1.164f - static_cast<float>(u - 128) * -2.018f);
}

// JPEG YUV to RGB reference
void Bof_YuvjToRgbReference(int y, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b)
{
  *r = RoundToByte(static_cast<float>(y) - static_cast<float>(v - 128) * -1.40200f);
  *g = RoundToByte(static_cast<float>(y) - static_cast<float>(u - 128) * 0.34414f - static_cast<float>(v - 128) * 0.71414f);
  *b = RoundToByte(static_cast<float>(y) - static_cast<float>(u - 128) * -1.77200f);
}

// BT.709 YUV to RGB reference
// See also http://www.equasys.de/colorconversion.html
void Bof_YuvhToRgbReference(int y, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b)
{
  *r = RoundToByte(static_cast<float>(y - 16) * 1.164f - static_cast<float>(v - 128) * -1.793f);
  *g = RoundToByte(static_cast<float>(y - 16) * 1.164f - static_cast<float>(u - 128) * 0.213f - static_cast<float>(v - 128) * 0.533f);
  *b = RoundToByte(static_cast<float>(y - 16) * 1.164f - static_cast<float>(u - 128) * -2.112f);
}

// BT.2020 YUV to RGB reference
void Bof_YuvRec2020ToRgbReference(int y, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b)
{
  *r = RoundToByte(static_cast<float>(y - 16) * 1.164384f - static_cast<float>(v - 128) * -1.67867f);
  *g = RoundToByte(static_cast<float>(y - 16) * 1.164384f - static_cast<float>(u - 128) * 0.187326f - static_cast<float>(v - 128) * 0.65042f);
  *b = RoundToByte(static_cast<float>(y - 16) * 1.164384f - static_cast<float>(u - 128) * -2.14177f);
}

BOFERR Bof_UyvyToBgra(uint32_t _Width_U32, int _Height_i, uint32_t /*_UyvyStride_U32*/, const uint8_t *_pUyvy_U8, uint32_t _BrgaStride_U32, BOF_RECT *_pCrop_X, BOF_ROTATION _Rotation_E, uint8_t *_pBgra_U8)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

#if 1
//125 times faster than #else part
  BOF_RECT r_X(0, 0, _Width_U32, _Height_i);
  int      DstStrideBgra_i, Sts_i;

  if ((_pCrop_X) && (_pCrop_X->IsInside(r_X)))
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
  else
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    DstStrideBgra_i = _BrgaStride_U32;
    if (_Rotation_E != BOF_ROTATION::BOF_ROTATION_0)
    {
      if ((_Rotation_E != BOF_ROTATION::BOF_ROTATION_90) || (_Rotation_E != BOF_ROTATION::BOF_ROTATION_180))
      {
        DstStrideBgra_i = _pCrop_X ? _pCrop_X->Height() * 4 : _Height_i * 4;
      }
    }
//  int Sts_i = libyuv::UYVYToARGB(_pYuYv_U8, _YuYvStride_U32, _pARgb_U8, _ARgbStride_U32, _Width_U32, _Height_i);

    if (Rts_E == BOF_ERR_NO_ERROR)
    {

//  int Sts_i = libyuv::ConvertToARGB(_pYuYv_U8,0,_pARgb_U8,_ARgbStride_U32,0,0,_Width_U32, _Height_i,_Width_U32,_Height_i,libyuv::RotationMode::kRotate0,libyuv::FOURCC_UYVY);
      Rts_E = BOF_ERR_INIT;
      if (_pCrop_X)
      {
        Sts_i = libyuv::ConvertToARGB(_pUyvy_U8, 0, _pBgra_U8, DstStrideBgra_i, _pCrop_X->Left_S32, _pCrop_X->Top_S32, _Width_U32, _Height_i, _pCrop_X->Width(), _pCrop_X->Height(), static_cast<libyuv::RotationMode>(_Rotation_E), libyuv::FOURCC_UYVY);
      }
      else
      {
        Sts_i = libyuv::ConvertToARGB(_pUyvy_U8, 0, _pBgra_U8, DstStrideBgra_i, 0, 0, _Width_U32, _Height_i, _Width_U32, _Height_i, static_cast<libyuv::RotationMode>(_Rotation_E), libyuv::FOURCC_UYVY);
      }
      if (Sts_i == 0)
      {
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
  }
#else
  uint32_t      x_U32, y_U32, Height_U32, IncARgb_U32, IncYuYv_U32;
  uint8_t       y_U8, u_U8, v_U8;
  const uint8_t *pStartOfYuYv_U8;

  Height_U32 = (_Height_i < 0) ? -_Height_i : _Height_i;
  if ((_Width_U32) && ((_Width_U32 & 3) == 0) && (Height_U32) && (_YuYvStride_U32 >= (_Width_U32 * 2)) && (_pYuYv_U8) && (_ARgbStride_U32 >= (_Width_U32 * 4)) && (_pARgb_U8))
  {
    Rts_E       = BOF_ERR_NO_ERROR;
    IncYuYv_U32 = (_YuYvStride_U32 - (_Width_U32 * 2));
    IncARgb_U32 = (_ARgbStride_U32 - (_Width_U32 * 4));
    _pYuYv_U8   = (_Height_i < 0) ? _pYuYv_U8 + ((Height_U32 - 1) * _YuYvStride_U32) : _pYuYv_U8;
    for (y_U32  = 0; y_U32 < Height_U32; y_U32++)
    {
      pStartOfYuYv_U8 = _pYuYv_U8;
      for (x_U32 = 0; x_U32 < (_Width_U32 / 2); x_U32++)
      {
        u_U8 = _pYuYv_U8[0];
        y_U8 = _pYuYv_U8[1];
        v_U8 = _pYuYv_U8[2];
        Bof_YuvToRgbReference(y_U8, u_U8, v_U8, &_pARgb_U8[0], &_pARgb_U8[1], &_pARgb_U8[2]);
        _pARgb_U8[3] = 0xFF;
        _pARgb_U8 += 4;

        y_U8 = _pYuYv_U8[3];
        Bof_YuvToRgbReference(y_U8, u_U8, v_U8, &_pARgb_U8[0], &_pARgb_U8[1], &_pARgb_U8[2]);
        _pARgb_U8[3] = 0xFF;
        _pARgb_U8 += 4;
        _pYuYv_U8 += 4;
      }
      if (IncARgb_U32)
      {
        _pARgb_U8 += IncARgb_U32;
      }
      if (IncYuYv_U32)
      {
        _pYuYv_U8 += IncYuYv_U32;
      }
      if (_Height_i < 0)
      {
        _pYuYv_U8 = pStartOfYuYv_U8 - _YuYvStride_U32;
      }
    }
  }
#endif
  return Rts_E;
}

BOFERR Bof_BgraToUyvy(uint32_t _Width_U32, int _Height_i, uint32_t _BrgaStride_U32, const uint8_t *_pBgra_U8, uint32_t _UyvyStride_U32,BOF_RECT *_pCrop_X, BOF_ROTATION _Rotation_E, uint8_t *_pUyvy_U8)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  BOF_RECT r_X(0, 0, _Width_U32, _Height_i);
  int      DstStrideBgra_i, DstStrideUyvy_i, Sts_i, DstWidth_i,DstHeight_i;
  uint8_t *pBgra_U8;

  if ((_pCrop_X) && (_pCrop_X->IsInside(r_X)))
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
  else
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    DstStrideBgra_i = _BrgaStride_U32;
    DstStrideUyvy_i=_UyvyStride_U32;
    DstWidth_i=_Width_U32;
    DstHeight_i=_Height_i;
    if (_Rotation_E != BOF_ROTATION::BOF_ROTATION_0)
    {
      if ((_Rotation_E != BOF_ROTATION::BOF_ROTATION_90) || (_Rotation_E != BOF_ROTATION::BOF_ROTATION_180))
      {
        DstStrideBgra_i = _pCrop_X ? _pCrop_X->Height() * 4 : _Height_i * 4;
        DstStrideUyvy_i = _pCrop_X ? _pCrop_X->Height() * 2 : _Height_i * 2;
        DstWidth_i=_pCrop_X ? _pCrop_X->Height():_Height_i;
        DstHeight_i=_pCrop_X ? _pCrop_X->Width():_Width_U32;
      }
    }

    pBgra_U8=nullptr;
    if (_pCrop_X)
    {
      pBgra_U8=new uint8_t[_pCrop_X->Width()*_pCrop_X->Height()*4];
      Rts_E=BOF_ERR_ENOMEM;
      if (pBgra_U8)
      {
        Rts_E = BOF_ERR_INIT;
        Sts_i = libyuv::ConvertToARGB(_pBgra_U8, 0, pBgra_U8, DstStrideBgra_i, _pCrop_X->Left_S32, _pCrop_X->Top_S32, _Width_U32, _Height_i, _pCrop_X->Width(), _pCrop_X->Height(), static_cast<libyuv::RotationMode>(_Rotation_E), libyuv::FOURCC_ABGR); //FOURCC_BGRA);
        if (Sts_i == 0)
        {
/*
          intptr_t Io;
          BOFERR Sts_E = Bof_CreateFile(BOF_NAMESPACE::BOF_FILE_PERMISSION_READ_FOR_ALL | BOF_NAMESPACE::BOF_FILE_PERMISSION_WRITE_FOR_ALL, "./crop.raw", false, Io);
          if (Sts_E == BOF_ERR_NO_ERROR)
          {
            uint32_t Size_U32=_pCrop_X->Width()*_pCrop_X->Height()*4;
            Sts_E    = BOF_NAMESPACE::Bof_WriteFile(Io, Size_U32, pBgra_U8);
            Sts_E    = BOF_NAMESPACE::Bof_CloseFile(Io);
          }
          */
          Sts_i = libyuv::ARGBToUYVY(pBgra_U8, DstStrideBgra_i, _pUyvy_U8, DstStrideUyvy_i, DstWidth_i,DstHeight_i);
          if (Sts_i == 0)
          {
            Rts_E = BOF_ERR_NO_ERROR;
          }
        }
        BOF_SAFE_DELETE(pBgra_U8);
      }
    }
    else
    {
      Rts_E = BOF_ERR_INIT;
      Sts_i = libyuv::ARGBToUYVY(_pBgra_U8, _BrgaStride_U32, _pUyvy_U8, _UyvyStride_U32, _Width_U32, _Height_i);
      if (Sts_i == 0)
      {
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
  }
  return Rts_E;
}

//http://adec.altervista.org/blog/yuv-422-v210-10-bit-packed-decoder-in-glsl/
//https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/loading.php
#if 0
unsigned long getFileLength(ifstream& file)
{
  if(!file.good()) return 0;

  unsigned long pos=file.tellg();
  file.seekg(0,ios::end);
  unsigned long len = file.tellg();
  file.seekg(ios::beg);

  return len;
}

int loadshader(char* filename, GLchar** ShaderSource, unsigned long* len)
{
  ifstream file;
  file.open(filename, ios::in); // opens as ASCII!
  if(!file) return -1;

  len = getFileLength(file);

  if (len==0) return -2;   // Error: Empty File

  *ShaderSource = (GLubyte*) new char[len+1];
  if (*ShaderSource == 0) return -3;   // can't reserve memory

  // len isn't always strlen cause some characters are stripped in ascii read...
  // it is important to 0-terminate the real length later, len is just max possible value...
  *ShaderSource[len] = 0;

  unsigned int i=0;
  while (file.good())
  {
    *ShaderSource[i] = file.get();       // get character from file.
    if (!file.eof())
      i++;
  }

  *ShaderSource[i] = 0;  // 0-terminate it at the correct position

  file.close();

  return 0; // No Error
}


int unloadshader(GLubyte** ShaderSource)
{
  if (*ShaderSource != 0)
    delete[] *ShaderSource;
  *ShaderSource = 0;
}
#endif

// =====================================
// YUV4:2:2 v210 10-bit decoder
// (C) by Andrea de Carolis 12/4/2015
// =====================================
// protected under MIT license
/*
Copyright (c) 2015 Andrea De Carolis

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/
#if 0
// decode a packet of 6 neighbours pixels in a scanline. The input // image must have width = multiple of 6 pixel.
#version 430 core
//#extension GL_ARB_gpu_shader5 : require // not required by GTX9x
#define FACTOR 3
// size of workgroup: a single 6 pixel packet
// (4 encoded 32bit WORD)
layout (local_size_x = 6, local_size_y = 1) in;
// single channel, 32bit WORD
layout (binding=1,r32ui) uniform uimage1D data_in;
// output image RGBA buffer
layout (binding=2,rgba8ui) uniform uimage2D data_out;
// local shared buffer for YUV components (6 pixels*3 YUV values)
shared uint shared_buf[18];
uniform int imageW;
uniform int imageH;
// number of DWORD in an encoded YUV scanline
uniform int stride_row;
const uint local_buf_ofs[24]=
              {
                1,4,0,0,2,5, // Cb0,Y0,Cr0 offset into the shared buffer
                3,3,7,10,6,6, // Y1,Cb1,Y2
                8,11,9,9,13,16, // Cr1,Y3,Cb2
                12,12,14,17,15,15 // Y4,Cr2,Y5
              };
void main (void)
{
  uint pxidx = gl_GlobalInvocationID.x%6;

  if (pxidx<4)
  {
    // parallel load of all 4 DWORD
    uint cp1,cp2,cp3;
    // stride_row = selected by caller, generally W*4
    // because all DWORD are consecutive for simplification.
    // that
    uint value = imageLoad(data_in,int((gl_GlobalInvocationID.x/6)*4+
                                       pxidx+stride_row*gl_GlobalInvocationID.y)).r;
    cp1 = value & 1023;
    cp2 = (value >> 10) & 1023;
    cp3 = (value >> 20) & 1023;
    uint baseofs = pxidx*6;
// copy each component in at least one buffer location,
    // and at max into two locations. For example
    // Yi will be written twice in the same location,
    // Cb and Cr in two different location because they are
    // shared across two consecutive pixels

    shared_buf[baseofs+0]=shared_buf[baseofs+1]=cp1;
    shared_buf[baseofs+2]=shared_buf[baseofs+3]=cp2;
    shared_buf[baseofs+4]=shared_buf[baseofs+5]=cp3;
  }

  barrier (); // wait all threads in the workgroup
// execute the YUV to RGB conversion for each of 6 pixel
// of the workgroup
  //
  // there are many different version of this conversion.
// This is one of them, but must be checked against
// the specific values contained in Y'CbCr buffer
  vec3 YUV = vec3(
    shared_buf[pxidx*3],
    shared_buf[pxidx*3+1],
    shared_buf[pxidx*3+2]);
  const mat3 YUV2RGB_MAPPING = mat3 (
    1.1678f, 0.0f ,1.6007f,
    1.1678f, -0.3929f ,-0.81532f,
    1.1678f, +2.0232f ,0.0f
  );
  YUV=YUV+vec3(-64,-512,-512);
  vec3 rgb = YUV2RGB_MAPPING*YUV;
  ivec4 rgba = ivec4 (
    clamp(int(rgb.r) >> FACTOR, 0, 255),
    clamp(int(rgb.g) >> FACTOR, 0, 255),
    clamp(int(rgb.b) >> FACTOR, 0, 255),
    255
  );
// store final rgba value
  imageStore (data_out,ivec2(gl_GlobalInvocationID.xy),rgba);
}
#endif
END_BOF_NAMESPACE()