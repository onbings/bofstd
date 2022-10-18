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
#include <bofstd/bof2d.h>
#include <bofstd/boffs.h>
#include <bofstd/bofstring.h>

#include <libyuv.h>
#include <iostream>
#include <string>
 //#include <MediaInfoDLL/MediaInfoDLL.h>

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

const BOF_YUVA GL_YuvWhite_X = { 235, 128, 128, 255 };

/* pluge pulses */
const BOF_YUVA GL_YuvNeg4Ire_X = { 7, 128, 128, 255 };
const BOF_YUVA GL_YuvPos4Ire_X = { 24, 128, 128, 255 };

/* fudged Q/-I */
const BOF_YUVA GL_YuvIPixel_X = { 57, 156, 97, 255 };
const BOF_YUVA GL_YuvQPixel_X = { 44, 171, 147, 255 };

const BOF_YUVA GL_YuvGray40_X = { 104, 128, 128, 255 };
const BOF_YUVA GL_YuvGray15_X = { 49, 128, 128, 255 };
const BOF_YUVA GL_YuvCyan_X = { 188, 154, 16, 255 };
const BOF_YUVA GL_YuvYellow_X = { 219, 16, 138, 255 };
const BOF_YUVA GL_YuvRed_X = { 63, 102, 240, 255 };
const BOF_YUVA GL_YuvGreen_X = { 145, 54, 34, 255 };
const BOF_YUVA GL_YuvBlue_X = { 32, 240, 118, 255 };
const BOF_YUVA GL_YuvBlack0_X = { 16, 128, 128, 255 };
const BOF_YUVA GL_YuvBlack2_X = { 20, 128, 128, 255 };
const BOF_YUVA GL_YuvBlack4_X = { 25, 128, 128, 255 };
const BOF_YUVA GL_YuvNeg2_X = { 12, 128, 128, 255 };

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
    p_U32 = (uint32_t *)_pData;
    Width_U32 = (_rRect_X.Right_S32 - _rRect_X.Left_S32); // Compatible with rect definition Right_S32,Bottom_S32 is outside of the rect
    Height_U32 = (_rRect_X.Bottom_S32 - _rRect_X.Top_S32);
    Left_U32 = _rRect_X.Left_S32;
    Top_U32 = _rRect_X.Top_S32;
    switch (_BitCount_U32)
    {
      case 8:
        //				p_UB  += ( (_rRect_X.Top_S32 * _Pitch_U32) + _rRect_X.Left_S32);
        break;

      case 16:
        //				p_U16 += ( ( (_rRect_X.Top_S32 * _Pitch_U32) + (_rRect_X.Left_S32 << 1) ) >> 1);
        break;

      case 24:
        p_U32 = (uint32_t *)((uint8_t *)p_U32 + ((_rRect_X.Top_S32 * _Pitch_U32) + (_rRect_X.Left_S32 * 3)));
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
              _rRect_X.Top_S32 = Top_U32 + y_U32;
              _rRect_X.Left_S32 = Left_U32 + x_U32;
              p_U32 = (uint32_t *)((uint8_t *)p_U32 + _Pitch_U32);
              break;
            }
          }
          if (!Rts_B)
          {
            p_U32 = (uint32_t *)((uint8_t *)p_U32 + Offset_U32);
          }
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
                _rRect_X.Top_S32 = Top_U32 + y_U32;
                _rRect_X.Left_S32 = Left_U32 + x_U32;
                p_U32 = (uint32_t *)((uint8_t *)p_U32 + _Pitch_U32);
                break;
              }
              p_U32 = (uint32_t *)((uint8_t *)p_U32 + 3);
            }
          }
          if (!Rts_B)
          {
            p_U32 = (uint32_t *)((uint8_t *)p_U32 + Offset_U32);
          }
          break;

        default:
          break;
      }
      if (Rts_B)
      {
        break;
      }
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
            {
              p_U32 = (uint32_t *)((uint8_t *)p_U32 + _Pitch_U32);
            }
            break;

          case 24:
            if ((*p_U32 & 0x00FFFFFFFF) == _Color_U32)
            {
              Rts_B = true;
              _rRect_X.Bottom_S32 = Top_U32 + y_U32;
              p_U32 = (uint32_t *)((uint8_t *)p_U32 + 3);
            }
            else
            {
              p_U32 = (uint32_t *)((uint8_t *)p_U32 + _Pitch_U32);
            }
            break;
        }
        if (Rts_B)
        {
          break;
        }
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
            {
              p_U32++;
            }
            break;

          case 24:
            if ((*p_U32 & 0x00FFFFFFFF) == _Color_U32)
            {
              Rts_B = true;
              _rRect_X.Right_S32 = Left_U32 + x_U32;
            }
            else
            {
              p_U32 = (uint32_t *)((uint8_t *)p_U32 + _Pitch_U32);
            }
            break;
        }
        if (Rts_B)
        {
          break;
        }
      }
    }
  }
  return (Rts_B ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL);
}

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
    p_U32 = (uint32_t *)_pData;

    switch (_BitCount_U32)
    {
      case 8:
        //		p_UB  += ( (_rRect_X.Top_S32 * _Pitch_U32) + _rRect_X.Left_S32);
        break;

      case 16:
        //		p_U16 += ( ( (_rRect_X.Top_S32 * _Pitch_U32) + (_rRect_X.Left_S32 << 1) ) >> 1);
        break;

      case 24:
        p_U32 = (uint32_t *)((uint8_t *)p_U32 + ((_rRect_X.Top_S32 * _Pitch_U32) + (_rRect_X.Left_S32 * 3)));
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
        p_U32 = (uint32_t *)((uint8_t *)p_U32 + ((_rRect_X.Bottom_S32 - _rRect_X.Top_S32) * _Pitch_U32));
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
  uint8_t *p_UB;
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
      p_UB = _pData_UB;
      p_U16 = (uint16_t *)_pData_UB;
      p_U32 = (uint32_t *)_pData_UB;
      switch (_BytePerPixel_UB)
      {
        case 1:
          for (r_F = 0.0f; (uint32_t)(r_F + 0.5f) < _Height_U32; r_F += _DecimationStepY_f)
          {
            Index_F = (float)((uint32_t)r_F * _Width_U32);
            for (c_F = 0.0f; (uint32_t)(c_F + 0.5f) < _Width_U32; c_F += _DecimationStepX_f)
            {
              *p_UB++ = _pData_UB[(uint32_t)(Index_F + 0.5f)];
              Index_F += _DecimationStepX_f;
            }
          }
          break;

        case 2:
          for (r_F = 0.0f; (uint32_t)(r_F + 0.5f) < _Height_U32; r_F += _DecimationStepY_f)
          {
            Index_F = (float)((uint32_t)r_F * _Width_U32);
            for (c_F = 0.0f; (uint32_t)(c_F + 0.5f) < _Width_U32; c_F += _DecimationStepX_f)
            {
              *p_U16++ = ((uint16_t *)_pData_UB)[(uint32_t)(Index_F + 0.5f)];
              Index_F += _DecimationStepX_f;
            }
          }
          break;

        case 4:
          for (r_F = 0.0f; (uint32_t)(r_F + 0.5f) < _Height_U32; r_F += _DecimationStepY_f)
          {
            Index_F = (float)((uint32_t)r_F * _Width_U32);
            for (c_F = 0.0f; (uint32_t)(c_F + 0.5f) < _Width_U32; c_F += _DecimationStepX_f)
            {
              *p_U32++ = ((uint32_t *)_pData_UB)[(uint32_t)(Index_F + 0.5f)];
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
    *_pColorEntry_U32 = (uint32_t)-1;
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
  uint8_t *p_UB;
  uint16_t *p_U16;
  uint32_t *p_U32;

  if (_pData_UB)
  {
    Rts_B = true;
    if (_ColorEntry1_U32 != _ColorEntry2_U32)
    {
      p_UB = _pData_UB;
      p_U16 = (uint16_t *)_pData_UB;
      p_U32 = (uint32_t *)_pData_UB;
      switch (_BytePerPixel_UB)
      {
        case 1:
          for (i_U32 = 0; i_U32 < _Height_U32; i_U32++)
          {
            for (j_U32 = 0; j_U32 < _Width_U32; j_U32++, p_UB++)
            {
              if (*p_UB == (uint8_t)_ColorEntry1_U32)
              {
                *p_UB = (uint8_t)_ColorEntry2_U32;
              }
              else if (*p_UB == (uint8_t)_ColorEntry2_U32)
              {
                *p_UB = (uint8_t)_ColorEntry1_U32;
              }
            }
          }
          break;

        case 2:
          for (i_U32 = 0; i_U32 < _Height_U32; i_U32++)
          {
            for (j_U32 = 0; j_U32 < _Width_U32; j_U32++, p_U16++)
            {
              if (*p_U16 == (uint16_t)_ColorEntry1_U32)
              {
                *p_U16 = (uint16_t)_ColorEntry2_U32;
              }
              else if (*p_U16 == (uint16_t)_ColorEntry2_U32)
              {
                *p_U16 = (uint16_t)_ColorEntry1_U32;
              }
            }
          }
          break;

        case 4:
          for (i_U32 = 0; i_U32 < _Height_U32; i_U32++)
          {
            for (j_U32 = 0; j_U32 < _Width_U32; j_U32++, p_U32++)
            {
              if (*p_U32 == _ColorEntry1_U32)
              {
                *p_U32 = _ColorEntry2_U32;
              }
              else if (*p_U32 == _ColorEntry2_U32)
              {
                *p_U32 = _ColorEntry1_U32;
              }
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
      _rBmInfo_X.Size_U32 = sizeof(BOF_BITMAP_INFO_HEADER);
      _rBmInfo_X.BitCount_U16 = TgaHeader_X.Bits_U8;
      _rBmInfo_X.Height_S32 = TgaHeader_X.Height_U16;
      _rBmInfo_X.Planes_U16 = 1;
      _rBmInfo_X.Width_S32 = TgaHeader_X.Width_U16;
      _rBmInfo_X.ClrUsed_U32 = TgaHeader_X.ColourMapLength_U16;
      _rBmInfo_X.SizeImage_U32 = (TgaHeader_X.Width_U16 * TgaHeader_X.Height_U16 * (TgaHeader_X.Bits_U8 >> 3));
    }
  }
  return (Rts_E);
}

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
    Rts_E = Bof_ReadFile(Io, Nb_U32, reinterpret_cast<uint8_t *>(&BmFile_X));
    if ((Rts_E == BOF_ERR_NO_ERROR)
        && (Nb_U32 == sizeof(BOF_BITMAP_FILE_HEADER))
        && (BmFile_X.Type_U16 == 0x4D42)              // 'BM
        )
    {
      Nb_U32 = sizeof(BOF_BITMAP_INFO_HEADER);
      Rts_E = Bof_ReadFile(Io, Nb_U32, reinterpret_cast<uint8_t *>(&_rBmInfo_X));
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
          {
            _rBmInfo_X.ClrUsed_U32 = 256;
          }
          Size_U32 = _rBmInfo_X.ClrUsed_U32 * static_cast<uint32_t>(sizeof(BOF_PALETTE_ENTRY));
          if (_pPaletteEntry_X)
          {
            Nb_U32 = Size_U32;
            Rts_E = Bof_ReadFile(Io, Nb_U32, reinterpret_cast<uint8_t *>(_pPaletteEntry_X));
            if ((Rts_E == BOF_ERR_NO_ERROR)
                && (Nb_U32 == Size_U32)
                )
            {
              // the palette is stored as RGBQUAD array instead of BOF_PALETTE_ENTRY->swap r & b
              for (i_U32 = 0; i_U32 < _rBmInfo_X.ClrUsed_U32; i_U32++)
              {
                Val_UB = _pPaletteEntry_X[i_U32].b_U8;
                _pPaletteEntry_X[i_U32].b_U8 = _pPaletteEntry_X[i_U32].r_U8;
                _pPaletteEntry_X[i_U32].r_U8 = Val_UB;
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
            Rts_E = (Bof_SetFileIoPosition(Io, Size_U32, BOF_SEEK_METHOD::BOF_SEEK_CURRENT) != (int64_t)-1) ? BOF_ERR_NO_ERROR : BOF_ERR_SEEK;
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

          Rts_E = Bof_ReadGraphicFile(Io, _LoadIndex_U32, (_rBmInfo_X.Height_S32 < 0) ? true : false, (uint8_t)_rBmInfo_X.BitCount_U16, _pData_UB, _rBmInfo_X.Width_S32, _rBmInfo_X.Height_S32,
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
      Rts_E = (Bof_SetFileIoPosition(Io, _rTgaHeader_X.IdentSize_U8, BOF_SEEK_METHOD::BOF_SEEK_CURRENT) != (int64_t)-1) ? BOF_ERR_NO_ERROR : BOF_ERR_SEEK;
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
              Rts_E = Bof_ReadFile(Io, Nb_U32, pPalette_UB);
              if ((Rts_E == BOF_ERR_NO_ERROR)
                  && (Nb_U32 == Size_U32)
                  )
              {
                switch (Size_U32)
                {
                  case 3:
                    _pPaletteEntry_X->r_U8 = pPalette_UB[2];
                    _pPaletteEntry_X->g_U8 = pPalette_UB[1];
                    _pPaletteEntry_X->b_U8 = pPalette_UB[0];
                    _pPaletteEntry_X->Flg_U8 = 0xFF;
                    break;

                  case 4:
                    _pPaletteEntry_X->r_U8 = pPalette_UB[2];
                    _pPaletteEntry_X->g_U8 = pPalette_UB[1];
                    _pPaletteEntry_X->b_U8 = pPalette_UB[0];
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
            Rts_E = (Bof_SetFileIoPosition(Io, (Size_U32 * _rTgaHeader_X.ColourMapLength_U16), BOF_SEEK_METHOD::BOF_SEEK_CURRENT) != (int64_t)-1) ? BOF_ERR_NO_ERROR : BOF_ERR_SEEK;
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
  uint8_t *pData_UB;

  Size_U32 = _Height_U32 * (_Width_U32 << 2);
  if ((pData_UB = new uint8_t[(uint32_t)Size_U32]) != nullptr)
  {
    Rts_E = Bof_CreateFile(BOF_FILE_PERMISSION_ALL_FOR_ALL, _rPath, false, Io);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      TgaHeader_X.Reset();
      TgaHeader_X.Bits_U8 = 32;
      TgaHeader_X.ImageType_U8 = 2;
      TgaHeader_X.Descriptor_X.Origin_UB = 1;
      TgaHeader_X.Height_U16 = (uint16_t)_Height_U32;
      TgaHeader_X.Width_U16 = (uint16_t)_Width_U32;
      Nb_U32 = sizeof(BOF_TGA_HEADER);
      Rts_E = Bof_WriteFile(Io, Nb_U32, reinterpret_cast<const uint8_t *>(&TgaHeader_X));
      if ((Rts_E == BOF_ERR_NO_ERROR)
          && (Nb_U32 == sizeof(BOF_TGA_HEADER))
          )
      {
        Rts_E = (Bof_SetFileIoPosition(Io, TgaHeader_X.IdentSize_U8, BOF_SEEK_METHOD::BOF_SEEK_CURRENT) != (int64_t)-1) ? BOF_ERR_NO_ERROR : BOF_ERR_SEEK;
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          memset(pData_UB, 0, (size_t)Size_U32);
          p_U32 = (uint32_t *)pData_UB;
          for (y_U32 = 0; y_U32 < TgaHeader_X.Height_U16; y_U32++, p_U32 += _Width_U32)
          {
            w_U32 = TgaHeader_X.Width_U16 - 1;
            v_U32 = 0xFFFF0000;
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
          Rts_E = Bof_WriteFile(Io, Nb_U32, pData_UB);
          if ((Rts_E == BOF_ERR_NO_ERROR)
              && (Nb_U32 == Size_U32)
              )
          {
          }
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
    BOF_SAFE_DELETE_ARRAY(pData_UB);
    //		delete pData_UB;
  }
  return (Rts_E);
}

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
  uint8_t *p_UB;

  if (_pData_UB)
  {
    WidthInByte_U32 = _Width_U32 * _BytePerPixel_UB;
    if (_pRecordSize_U32)
    {
      NbLineToRead_U32 = (*_pRecordSize_U32 / WidthInByte_U32);
      if (NbLineToRead_U32 > _Height_U32)
      {
        NbLineToRead_U32 = _Height_U32;
      }

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
          LineInFile_U32 = 0;
        }
      }

      if ((!(LineInFile_U32 & 0x80000000))
          && (!(NbLineToRead_U32 & 0x80000000))
          && (LineInFile_U32 < _Height_U32)
          )
      {
        Rts_E = (Bof_SetFileIoPosition(Io, LineInFile_U32 * WidthInByte_U32, BOF_SEEK_METHOD::BOF_SEEK_CURRENT) != (int64_t)-1) ? BOF_ERR_NO_ERROR : BOF_ERR_SEEK;
      }
      *_pRecordSize_U32 = 0;
    }
    else
    {
      NbLineToRead_U32 = _Height_U32;
      Rts_E = BOF_ERR_NO_ERROR;
    }

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Size_U32 = 0;
      if (_TopDown_B)
      {
        Size_U32 = WidthInByte_U32 * NbLineToRead_U32;
        Nb_U32 = Size_U32;
        Rts_E = Bof_ReadFile(Io, Nb_U32, _pData_UB);
        if ((Rts_E == BOF_ERR_NO_ERROR)
            && (Nb_U32 == Size_U32)
            )
        {
        }
        else
        {
          Rts_E = BOF_ERR_READ;
        }
      }
      else
      {
        Nb_U32 = 0;
        p_UB = (_pData_UB)+((NbLineToRead_U32 - 1) * WidthInByte_U32);
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
        Rts_E = (i_U32 == NbLineToRead_U32) ? BOF_ERR_NO_ERROR : BOF_ERR_READ;
      }
      if (_pRecordSize_U32)
      {
        *_pRecordSize_U32 = (uint32_t)Size_U32;
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
    Rts_E = BOF_ERR_NO_ERROR;
    IncYuYv_U32 = (_YuYvStride_U32 - (_Width_U32 * 2));
    IncARgb_U32 = (_ARgbStride_U32 - (_Width_U32 * 4));
    _pYuYv_U8 = (_Height_i < 0) ? _pYuYv_U8 + ((Height_U32 - 1) * _YuYvStride_U32) : _pYuYv_U8;
    for (y_U32 = 0; y_U32 < Height_U32; y_U32++)
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

BOFERR Bof_BgraToUyvy(uint32_t _Width_U32, int _Height_i, uint32_t _BrgaStride_U32, const uint8_t *_pBgra_U8, uint32_t _UyvyStride_U32, BOF_RECT *_pCrop_X, BOF_ROTATION _Rotation_E, uint8_t *_pUyvy_U8)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  BOF_RECT r_X(0, 0, _Width_U32, _Height_i);
  int      DstStrideBgra_i, DstStrideUyvy_i, Sts_i, DstWidth_i, DstHeight_i;
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
    DstStrideUyvy_i = _UyvyStride_U32;
    DstWidth_i = _Width_U32;
    DstHeight_i = _Height_i;
    if (_Rotation_E != BOF_ROTATION::BOF_ROTATION_0)
    {
      if ((_Rotation_E != BOF_ROTATION::BOF_ROTATION_90) || (_Rotation_E != BOF_ROTATION::BOF_ROTATION_180))
      {
        DstStrideBgra_i = _pCrop_X ? _pCrop_X->Height() * 4 : _Height_i * 4;
        DstStrideUyvy_i = _pCrop_X ? _pCrop_X->Height() * 2 : _Height_i * 2;
        DstWidth_i = _pCrop_X ? _pCrop_X->Height() : _Height_i;
        DstHeight_i = _pCrop_X ? _pCrop_X->Width() : _Width_U32;
      }
    }

    pBgra_U8 = nullptr;
    if (_pCrop_X)
    {
      pBgra_U8 = new uint8_t[_pCrop_X->Width() * _pCrop_X->Height() * 4];
      Rts_E = BOF_ERR_ENOMEM;
      if (pBgra_U8)
      {
        Rts_E = BOF_ERR_INIT;
        Sts_i = libyuv::ConvertToARGB(_pBgra_U8, 0, pBgra_U8, DstStrideBgra_i, _pCrop_X->Left_S32, _pCrop_X->Top_S32, _Width_U32, _Height_i, _pCrop_X->Width(), _pCrop_X->Height(), static_cast<libyuv::RotationMode>(_Rotation_E), libyuv::FOURCC_ABGR); //FOURCC_BGRA);
        if (Sts_i == 0)
        {
          /*
                    intptr_t Io;
                    BOFERR Sts_E = Bof_CreateFile(BOF::BOF_FILE_PERMISSION_READ_FOR_ALL | BOF::BOF_FILE_PERMISSION_WRITE_FOR_ALL, "./crop.raw", false, Io);
                    if (Sts_E == BOF_ERR_NO_ERROR)
                    {
                      uint32_t Size_U32=_pCrop_X->Width()*_pCrop_X->Height()*4;
                      Sts_E    = BOF::Bof_WriteFile(Io, Size_U32, pBgra_U8);
                      Sts_E    = BOF::Bof_CloseFile(Io);
                    }
                    */
          Sts_i = libyuv::ARGBToUYVY(pBgra_U8, DstStrideBgra_i, _pUyvy_U8, DstStrideUyvy_i, DstWidth_i, DstHeight_i);
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
unsigned long getFileLength(ifstream &file)
{
  if (!file.good()) return 0;

  unsigned long pos = file.tellg();
  file.seekg(0, ios::end);
  unsigned long len = file.tellg();
  file.seekg(ios::beg);

  return len;
}

int loadshader(char *filename, GLchar **ShaderSource, unsigned long *len)
{
  ifstream file;
  file.open(filename, ios::in); // opens as ASCII!
  if (!file) return -1;

  len = getFileLength(file);

  if (len == 0) return -2;   // Error: Empty File

  *ShaderSource = (GLubyte *) new char[len + 1];
  if (*ShaderSource == 0) return -3;   // can't reserve memory

  // len isn't always strlen cause some characters are stripped in ascii read...
  // it is important to 0-terminate the real length later, len is just max possible value...
  *ShaderSource[len] = 0;

  unsigned int i = 0;
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


int unloadshader(GLubyte **ShaderSource)
{
  if (*ShaderSource != 0)
    delete[] * ShaderSource;
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
layout(local_size_x = 6, local_size_y = 1) in;
// single channel, 32bit WORD
layout(binding = 1, r32ui) uniform uimage1D data_in;
// output image RGBA buffer
layout(binding = 2, rgba8ui) uniform uimage2D data_out;
// local shared buffer for YUV components (6 pixels*3 YUV values)
shared uint shared_buf[18];
uniform int imageW;
uniform int imageH;
// number of DWORD in an encoded YUV scanline
uniform int stride_row;
const uint local_buf_ofs[24] =
{
  1,4,0,0,2,5, // Cb0,Y0,Cr0 offset into the shared buffer
  3,3,7,10,6,6, // Y1,Cb1,Y2
  8,11,9,9,13,16, // Cr1,Y3,Cb2
  12,12,14,17,15,15 // Y4,Cr2,Y5
};
void main(void)
{
  uint pxidx = gl_GlobalInvocationID.x % 6;

  if (pxidx < 4)
  {
    // parallel load of all 4 DWORD
    uint cp1, cp2, cp3;
    // stride_row = selected by caller, generally W*4
    // because all DWORD are consecutive for simplification.
    // that
    uint value = imageLoad(data_in, int((gl_GlobalInvocationID.x / 6) * 4 +
                           pxidx + stride_row * gl_GlobalInvocationID.y)).r;
    cp1 = value & 1023;
    cp2 = (value >> 10) & 1023;
    cp3 = (value >> 20) & 1023;
    uint baseofs = pxidx * 6;
    // copy each component in at least one buffer location,
        // and at max into two locations. For example
        // Yi will be written twice in the same location,
        // Cb and Cr in two different location because they are
        // shared across two consecutive pixels

    shared_buf[baseofs + 0] = shared_buf[baseofs + 1] = cp1;
    shared_buf[baseofs + 2] = shared_buf[baseofs + 3] = cp2;
    shared_buf[baseofs + 4] = shared_buf[baseofs + 5] = cp3;
  }

  barrier(); // wait all threads in the workgroup
// execute the YUV to RGB conversion for each of 6 pixel
// of the workgroup
  //
  // there are many different version of this conversion.
// This is one of them, but must be checked against
// the specific values contained in Y'CbCr buffer
  vec3 YUV = vec3(
    shared_buf[pxidx * 3],
    shared_buf[pxidx * 3 + 1],
    shared_buf[pxidx * 3 + 2]);
  const mat3 YUV2RGB_MAPPING = mat3(
    1.1678f, 0.0f, 1.6007f,
    1.1678f, -0.3929f, -0.81532f,
    1.1678f, +2.0232f, 0.0f
  );
  YUV = YUV + vec3(-64, -512, -512);
  vec3 rgb = YUV2RGB_MAPPING * YUV;
  ivec4 rgba = ivec4(
    clamp(int(rgb.r) >> FACTOR, 0, 255),
    clamp(int(rgb.g) >> FACTOR, 0, 255),
    clamp(int(rgb.b) >> FACTOR, 0, 255),
    255
  );
  // store final rgba value
  imageStore(data_out, ivec2(gl_GlobalInvocationID.xy), rgba);
}
#endif



BofMediaDetector::BofMediaDetector()
{

}
BofMediaDetector::~BofMediaDetector()
{

}

#if 0
BOFERR BofMediaDetector::ParseFile(const BofPath &_rPathName, ResultFormat _ResultFormat_E, std::string &_rResult_S)
{
  String Oss;
  Oss = mMediaInfo.Option(__T("info_version"));
  std::wcout << Oss << std::endl;
  Oss = mMediaInfo.Option(__T("info_outputformats"));
  std::wcout << Oss << std::endl;

  return BOF_ERR_NO_ERROR;
}
BOFERR BofMediaDetector::ParseBuffer(const BOF_BUFFER &_rBuffer_X, ResultFormat _ResultFormat_E, std::string &_rResult_S, uint64_t &_rOffsetInBuffer_U64)
{
  return BOF_ERR_NO_ERROR;
}
BOFERR BofMediaDetector::Query(MediaStreamType _MediaStreamType_E, const std::string &_rOption_S, InfoType _InfoType_E, std::string &_rResult_S)
{
  return BOF_ERR_NO_ERROR;
}
#endif

//https://github.com/MediaArea/MediaInfoLib/issues/485
//#include <MediaInfoDLL/MediaInfoDLL.h>

std::string BofMediaDetector::Option(const std::string &_rOption_S)
{
  std::wstring Option_WS;
  Option_WS = Bof_Utf8ToUtf16(_rOption_S);
  return Bof_Utf16ToUtf8(mMediaInfo.Option(Option_WS));
}

BOFERR BofMediaDetector::ParseFile(const BofPath &_rPathName, ResultFormat _ResultFormat_E, std::string &_rResult_S)
{
  BOFERR Rts_E;
  String Oss;

#if defined(_WIN32)
  size_t Sz = mMediaInfo.Open(_rPathName.FullWidePathName(true));
#else
  size_t Sz = mMediaInfo.Open(_rPathName.FullWidePathName(false));
#endif
  Rts_E = Sz ? BOF_ERR_NO_ERROR : BOF_ERR_DONT_EXIST;
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    /*
    Ztring MediaInfo_Config::Option (const String &Option, const String &Value_Raw)
    at C:\pro\vcpkg\buildtrees\libmediainfo\src\v21.03-c3817db36f.clean\Source\MediaInfo\MediaInfo_Config.cpp
    */
    //Oss = mMediaInfo.Option(__T("info_version"));
    //std::wcout << Oss<< std::endl;
    //Oss = mMediaInfo.Option(__T("info_outputformats"));
    //std::wcout << Oss<< std::endl;
    //Oss = mMediaInfo.Option(__T("info_outputformats_json"));
    //std::wcout << Oss<< std::endl;
    //Oss = mMediaInfo.Option(__T("info_parameters"));
    //std::wcout << Oss<< std::endl;
    //Oss = mMediaInfo.Option(__T("Info_Codecs"));
    //std::wcout << Oss<< std::endl;

    switch (_ResultFormat_E)
    {
      case ResultFormat::Text:
        mMediaInfo.Option(__T("output"), __T("TEXT"));
        break;

      case ResultFormat::Html:
        mMediaInfo.Option(__T("output"), __T("HTML"));
        break;

      case ResultFormat::Json:
        mMediaInfo.Option(__T("output"), __T("JSON"));
        break;

      default:
        Rts_E = BOF_ERR_FORMAT;
        break;
    }
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      mMediaInfo.Option(__T("Complete")); //Complete=false
      Oss = mMediaInfo.Inform();
      //      std::wcout << Oss;
      _rResult_S = Bof_Utf16ToUtf8(Oss);
    }
    /*
    mMediaInfo.Option(__T("Complete"), __T("1")); //Complete=true
    Oss += mMediaInfo.Inform();
    std::wcout << Oss << std::endl;;
    */
  }
  return Rts_E;
}
//Accumulatted buffer size must be at least 48 PNG 3408 Jpeg
BOFERR BofMediaDetector::ParseBuffer(const BOF_BUFFER &_rBuffer_X, ResultFormat _ResultFormat_E, std::string &_rResult_S, uint64_t &_rOffsetInBuffer_U64)
{
#if 0	
  goto l;
  FILE *F = fopen("./data/colorbar.jpg", "rb");
  if (F)
  {

    //From: preparing a memory buffer for reading
    const uint32_t BUFFER_SIZE = 64;  //Not less than 644 * 1024;    //48 PNG 3408 Jpeg
    unsigned char *From_Buffer = new unsigned char[BUFFER_SIZE]; //Note: you can do your own buffer
    size_t From_Buffer_Size, Total; //The size of the read file buffer

    //From: retrieving file size
    fseek(F, 0, SEEK_END);
    long F_Size = ftell(F);
    fseek(F, 0, SEEK_SET);


    //Preparing to fill MediaInfo with a buffer
    mMediaInfo.Open_Buffer_Init(F_Size, 0);
    Total = 0;
    //The parsing loop
    do
    {
      //Reading data somewhere, do what you want for this.
      From_Buffer_Size = fread(From_Buffer, 1, BUFFER_SIZE, F);
      Total += From_Buffer_Size;

      //Sending the buffer to MediaInfo
      size_t Status = mMediaInfo.Open_Buffer_Continue(From_Buffer, From_Buffer_Size);
      printf("read %lld out of %ld, total read is %lld status %zd\n", From_Buffer_Size, F_Size, Total, Status);
      if (Status & 0x08) //Bit3=Finished
        break;

      //Testing if there is a MediaInfo request to go elsewhere
      if (mMediaInfo.Open_Buffer_Continue_GoTo_Get() != (uint64_t)-1)
      {
        fseek(F, (long)mMediaInfo.Open_Buffer_Continue_GoTo_Get(), SEEK_SET);   //Position the file
        mMediaInfo.Open_Buffer_Init(F_Size, ftell(F));                          //Informing MediaInfo we have seek
      }
    } while (From_Buffer_Size > 0);

    //Finalizing
    mMediaInfo.Open_Buffer_Finalize(); //This is the end of the stream, MediaInfo must finnish some work
  }
l:
#endif  
  BOFERR Rts_E = BOF_ERR_EINVAL;
  String Oss;
  size_t Status;

  if ((_rBuffer_X.Capacity_U64 >= 64) && (_rBuffer_X.Size_U64 >= 64) && (_rBuffer_X.Size_U64 <= _rBuffer_X.Capacity_U64))
  {
    Rts_E = BOF_ERR_NO_ERROR;
    //Preparing to fill MediaInfo with a buffer
    mMediaInfo.Open_Buffer_Init(_rBuffer_X.Capacity_U64, _rOffsetInBuffer_U64);
    //Sending the buffer to MediaInfo
    Status = mMediaInfo.Open_Buffer_Continue(_rBuffer_X.pData_U8, _rBuffer_X.Size_U64);
    if (Status & 0x08) //Bit3=Finished
    {
      //71    D 0001BB53 MediaInfoA_Open_Buffer_Finalize = @ILT + 109390(MediaInfoA_Open_Buffer_Finalize)
      mMediaInfo.Open_Buffer_Finalize(); //This is the end of the stream, MediaInfo must finish some work
      switch (_ResultFormat_E)
      {
        case ResultFormat::Text:
          mMediaInfo.Option(__T("output"), __T("TEXT"));
          break;

        case ResultFormat::Html:
          mMediaInfo.Option(__T("output"), __T("HTML"));
          break;

        case ResultFormat::Json:
          mMediaInfo.Option(__T("output"), __T("JSON"));
          break;

        default:
          Rts_E = BOF_ERR_FORMAT;
          break;
      }
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        mMediaInfo.Option(__T("Complete")); //Complete=false
        Oss = mMediaInfo.Inform();
        //     std::wcout << Oss << std::endl;
        _rResult_S = Bof_Utf16ToUtf8(Oss);
      }
    }
    else
    {
      //Testing if there is a MediaInfo request to go elsewhere
      _rOffsetInBuffer_U64 = mMediaInfo.Open_Buffer_Continue_GoTo_Get();
      if (_rOffsetInBuffer_U64 == (uint64_t)-1)
      {
        Rts_E = BOF_ERR_PENDING;
      }
      else
      {
        Rts_E = BOF_ERR_SEEK; //Need to seek in buffer
      }
    }
  }
  return Rts_E;
}

BOFERR BofMediaDetector::Query(MediaStreamType _MediaStreamType_E, const std::string &_rParam_S, InfoType _InfoType_E, std::string &_rResult_S)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  String Oss;
  stream_t MediaStreamType_E;
  info_t InfoType_E;
  std::wstring Param_WS;

  switch (_MediaStreamType_E)
  {
    case MediaStreamType::General:
      MediaStreamType_E = Stream_General;
      break;

    case MediaStreamType::Video:
      MediaStreamType_E = Stream_Video;
      break;

    case MediaStreamType::Audio:
      MediaStreamType_E = Stream_Audio;
      break;

    case MediaStreamType::Text:
      MediaStreamType_E = Stream_Text;
      break;

    case MediaStreamType::Other:
      MediaStreamType_E = Stream_Other;
      break;

    case MediaStreamType::Image:
      MediaStreamType_E = Stream_Image;
      break;

    case MediaStreamType::Menu:
      MediaStreamType_E = Stream_Menu;
      break;

    default:
      Rts_E = BOF_ERR_EINVAL;
      break;
  }
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    switch (_InfoType_E)
    {
      case InfoType::Name:
        InfoType_E = Info_Name;
        break;

      case InfoType::Text:
        InfoType_E = Info_Text;
        break;

      case InfoType::Measure:
        InfoType_E = Info_Measure;
        break;

      case InfoType::Options:
        InfoType_E = Info_Options;
        break;

      case InfoType::Name_Text:
        InfoType_E = Info_Name_Text;
        break;

      case InfoType::Measure_Text:
        InfoType_E = Info_Measure_Text;
        break;

      case InfoType::Info:
        InfoType_E = Info_Info;
        break;

      case InfoType::HowTo:
        InfoType_E = Info_HowTo;
        break;

      case InfoType::Domain:
        InfoType_E = Info_Domain;
        break;

      default:
        Rts_E = BOF_ERR_EPROTOTYPE;
        break;
    }
  }
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Param_WS = Bof_Utf8ToUtf16(_rParam_S);
    Oss = mMediaInfo.Get(MediaStreamType_E, 0, Param_WS, InfoType_E, Info_Name);
    //Oss = mMediaInfo.Get(MediaStreamType_E, 0, _rOption_S, InfoType_E, Info_Name);
    //std::wcout << Oss << std::endl;
    _rResult_S = Bof_Utf16ToUtf8(Oss);

    //Oss += mMediaInfo.Get(Stream_General, 0, __T("FileSize"), Info_Text, Info_Name);
    //Ztring MediaInfo_Internal::Get(stream_t StreamKind, size_t StreamPos, const String & Parameter, info_t KindOfInfo, info_t KindOfSearch)
  }

  return Rts_E;
}

END_BOF_NAMESPACE()

//https://fossies.org/linux/MediaInfo_CLI/MediaInfoLib/Source/Example/HowToUse_Dll.cpp
/*

[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from Bof2d_Test
[ RUN      ] Bof2d_Test.MediaDetectorParam
[--- colorbar.jpg ---------------------------------------------------------------]
----->General
General Param 'Count' =  I:'Count of objects available in this stream' T:'331' O:'N NI'
General Param 'Status' =  I:'Status of bit field (0=IsAccepted, 1=IsFilled, 2=IsUpdated, 3=IsFinished)' O:'N NI'
General Param 'StreamCount' =  I:'Count of streams of this kind available (base=1)' T:'1' O:'N NI'
General Param 'StreamKind' =  I:'Stream type name' T:'General' O:'N NT'
General Param 'StreamKind/String' =  I:'Stream type name' T:'General' O:'N NT'
General Param 'StreamKindID' =  I:'Stream number (base=0)' T:'0' O:'N NI'
General Param 'StreamKindPos' =  I:'Number of the stream, when multiple (base=1)' O:'N NI'
General Param 'StreamOrder' =  I:'Stream order in the file for type of stream (base=0)' O:'N YIY'
General Param 'FirstPacketOrder' =  I:'Order of the first fully decodable packet met in the file for stream type (base=0)' O:'N YI'
General Param 'Inform' =  I:'Inform'
General Param 'ID' =  I:'The ID for this stream in this file' O:'N YTY'
General Param 'ID/String' =  I:'The ID for this stream in this file' O:'Y NT'
General Param 'OriginalSourceMedium_ID' =  I:'The ID for this stream in the original medium of the material' O:'N YTY'
General Param 'OriginalSourceMedium_ID/S' =  I:'???'
General Param 'UniqueID' =  I:'The unique ID for this stream, should be copied with stream copy' O:'N YTY'
General Param 'UniqueID/String' =  I:'The unique ID for this stream, should be copied with stream copy' O:'Y NT'
General Param 'MenuID' =  I:'The menu ID for this stream in this file' O:'N YTY'
General Param 'MenuID/String' =  I:'The menu ID for this stream in this file' O:'Y NT'
General Param 'GeneralCount' =  I:'Number of general streams' O:'N NIY'
General Param 'VideoCount' =  I:'Number of video streams' O:'N NIY'
General Param 'AudioCount' =  I:'Number of audio streams' O:'N NIY'
General Param 'TextCount' =  I:'Number of text streams' O:'N NIY'
General Param 'OtherCount' =  I:'Number of other streams' O:'N NIY'
General Param 'ImageCount' =  I:'Number of image streams' T:'1' O:'N NIY'
General Param 'MenuCount' =  I:'Number of menu streams' O:'N NIY'
General Param 'Video_Format_List' =  I:'Video Codecs in this file, separated by /' O:'N NT'
General Param 'Video_Format_WithHint_Lis' =  I:'???'
General Param 'Video_Codec_List' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Video_Language_List' =  I:'Video languages in this file, full names, separated by /' O:'N NT'
General Param 'Audio_Format_List' =  I:'Audio Codecs in this file,separated by /' O:'N NT'
General Param 'Audio_Format_WithHint_Lis' =  I:'???'
General Param 'Audio_Codec_List' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Audio_Language_List' =  I:'Audio languages in this file separated by /' O:'N NT'
General Param 'Text_Format_List' =  I:'Text Codecs in this file, separated by /' O:'N NT'
General Param 'Text_Format_WithHint_List' =  I:'Text Codecs in this file with popular name (hint),separated by /' O:'N NT'
General Param 'Text_Codec_List' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Text_Language_List' =  I:'Text languages in this file, separated by /' O:'N NT'
General Param 'Other_Format_List' =  I:'Other formats in this file, separated by /' O:'N NT'
General Param 'Other_Format_WithHint_Lis' =  I:'???'
General Param 'Other_Codec_List' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Other_Language_List' =  I:'Chapters languages in this file, separated by /' O:'N NT'
General Param 'Image_Format_List' =  I:'Image Codecs in this file, separated by /' T:'JPEG' O:'N NT'
General Param 'Image_Format_WithHint_Lis' =  I:'???'
General Param 'Image_Codec_List' =  I:'Deprecated, do not use in new projects' T:'JPEG' O:'N NT'
General Param 'Image_Language_List' =  I:'Image languages in this file, separated by /' O:'N NT'
General Param 'Menu_Format_List' =  I:'Menu Codecs in this file, separated by /' O:'N NT'
General Param 'Menu_Format_WithHint_List' =  I:'Menu Codecs in this file with popular name (hint),separated by /' O:'N NT'
General Param 'Menu_Codec_List' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Menu_Language_List' =  I:'Menu languages in this file, separated by /' O:'N NT'
General Param 'CompleteName' =  I:'Complete name (Folder+Name+Extension)' T:'C:\bld\bofstd\tests\data\colorbar.jpg' O:'Y YT'
General Param 'FolderName' =  I:'Folder name only' T:'C:\bld\bofstd\tests\data' O:'N NT'
General Param 'FileNameExtension' =  I:'File name and extension' T:'colorbar.jpg' O:'N NT'
General Param 'FileName' =  I:'File name only' T:'colorbar' O:'N NT'
General Param 'FileExtension' =  I:'File extension only' T:'jpg' O:'N NTY'
General Param 'CompleteName_Last' =  I:'Complete name (Folder+Name+Extension) of the last file (in the case of a sequence of files)' O:'Y YTY'
General Param 'FolderName_Last' =  I:'Folder name only of the last file (in the case of a sequence of files)' O:'N NT'
General Param 'FileNameExtension_Last' =  I:'File name and extension of the last file (in the case of a sequence of files)' O:'N NT'
General Param 'FileName_Last' =  I:'File name only of the last file (in the case of a sequence of files)' O:'N NT'
General Param 'FileExtension_Last' =  I:'File extension only of the last file (in the case of a sequence of files)' O:'N NT'
General Param 'Format' =  I:'Format used' T:'JPEG' O:'N YTY'
General Param 'Format/String' =  I:'Format used + additional features' T:'JPEG' O:'Y NT'
General Param 'Format/Info' =  I:'Info about this Format' O:'Y NT'
General Param 'Format/Url' =  I:'Link to a description of this format' O:'N NT'
General Param 'Format/Extensions' =  I:'Known extensions of this format' T:'h3d jpeg jpg jpe jps mpo' O:'N NT'
General Param 'Format_Commercial' =  I:'Commercial name used by vendor for these settings or Format field if there is no difference' T:'JPEG' O:'N NT'
General Param 'Format_Commercial_IfAny' =  I:'Commercial name used by vendor for these settings if there is one' O:'Y YTY'
General Param 'Format_Version' =  I:'Version of this format' O:'Y YTY'
General Param 'Format_Profile' =  I:'Profile of the Format (old XML: 'Profile@Level' format' O:'Y YTY' H:' MIXML: 'Profile' only)'
General Param 'Format_Level' =  I:'Level of the Format (only MIXML)' O:'Y YTY'
General Param 'Format_Compression' =  I:'Compression method used' O:'Y YTY'
General Param 'Format_Settings' =  I:'Settings needed for decoder used' O:'Y YTY'
General Param 'Format_AdditionalFeatures' =  I:'Format features needed for fully supporting the content' O:'N YTY'
General Param 'InternetMediaType' =  I:'Internet Media Type (aka MIME Type, Content-Type)' T:'image/jpeg' O:'N YT'
General Param 'CodecID' =  I:'Codec ID (found in some containers)' O:'N YTY'
General Param 'CodecID/String' =  I:'Codec ID (found in some containers)' O:'Y NT'
General Param 'CodecID/Info' =  I:'Info about this Codec' O:'Y NT'
General Param 'CodecID/Hint' =  I:'A hint/popular name for this Codec' O:'Y NT'
General Param 'CodecID/Url' =  I:'A link to more details about this Codec ID' O:'N NT'
General Param 'CodecID_Description' =  I:'Manual description given by the container' O:'Y YTY'
General Param 'CodecID_Version' =  I:'Version of the CodecID' O:'N YTY'
General Param 'CodecID_Compatible' =  I:'Compatible CodecIDs' O:'N YTY'
General Param 'Interleaved' =  I:'If Audio and video are muxed' O:'N YTY'
General Param 'Codec' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Codec/String' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Codec/Info' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Codec/Url' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Codec/Extensions' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Codec_Settings' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Codec_Settings_Automatic' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'FileSize' =  I:'File size in bytes' T:'9830' M:' byte' O:'N YTY' m:' byte'
General Param 'FileSize/String' =  I:'File size (with measure)' T:'9.60 KiB' O:'Y NT'
General Param 'FileSize/String1' =  I:'File size (with measure, 1 digit mini)' T:'10 KiB' O:'N NT'
General Param 'FileSize/String2' =  I:'File size (with measure, 2 digit mini)' T:'9.6 KiB' O:'N NT'
General Param 'FileSize/String3' =  I:'File size (with measure, 3 digit mini)' T:'9.60 KiB' O:'N NT'
General Param 'FileSize/String4' =  I:'File size (with measure, 4 digit mini)' T:'9.600 KiB' O:'N NT'
General Param 'Duration' =  I:'Play time of the stream (in ms)' M:' ms' O:'N YFY' m:' ms'
General Param 'Duration/String' =  I:'Play time in format : XXx YYy only, YYy omitted if zero' O:'Y NT'
General Param 'Duration/String1' =  I:'Play time in format : HHh MMmn SSs MMMms, XX omitted if zero' O:'N NT'
General Param 'Duration/String2' =  I:'Play time in format : XXx YYy only, YYy omitted if zero' O:'N NT'
General Param 'Duration/String3' =  I:'Play time in format : HH:MM:SS.MMM' O:'N NT'
General Param 'Duration/String4' =  I:'Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available' O:'N NT'
General Param 'Duration/String5' =  I:'Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)' O:'N NT'
General Param (Duration_Start)
General Param (Duration_End)
General Param 'OverallBitRate_Mode' =  I:'Bit rate mode of all streams (VBR, CBR)' O:'N YTY'
General Param 'OverallBitRate_Mode/Strin' =  I:'???'
General Param 'OverallBitRate' =  I:'Bit rate of all streams (in bps)' M:' bps' O:'N YFY' m:' b/s'
General Param 'OverallBitRate/String' =  I:'Bit rate of all streams (with measure)' O:'Y NT'
General Param 'OverallBitRate_Minimum' =  I:'Minimum bit rate (in bps)' M:' bps' O:'N YFY' m:' b/s'
General Param 'OverallBitRate_Minimum/St' =  I:'???'
General Param 'OverallBitRate_Nominal' =  I:'Nominal bit rate (in bps)' M:' bps' O:'N YFY' m:' b/s'
General Param 'OverallBitRate_Nominal/St' =  I:'???'
General Param 'OverallBitRate_Maximum' =  I:'Maximum bit rate (in bps)' M:' bps' O:'N YFY' m:' b/s'
General Param 'OverallBitRate_Maximum/St' =  I:'???'
General Param 'FrameRate' =  I:'Frames per second' M:' fps' O:'N YFY' m:' fps'
General Param 'FrameRate/String' =  I:'Frames per second (with measurement)' O:'N NT'
General Param 'FrameRate_Num' =  I:'Frames per second, numerator' O:'N NFN'
General Param 'FrameRate_Den' =  I:'Frames per second, denominator' O:'N NFN'
General Param 'FrameCount' =  I:'Frame count (if a frame contains a count of samples)' O:'N NIY'
General Param 'Delay' =  I:'Delay fixed in the stream (relative) (in ms)' M:' ms' O:'N YI' m:' ms'
General Param 'Delay/String' =  I:'Delay with measurement' O:'N NT'
General Param 'Delay/String1' =  I:'Delay with measurement' O:'N NT'
General Param 'Delay/String2' =  I:'Delay with measurement' O:'N NT'
General Param 'Delay/String3' =  I:'format : HH:MM:SS.MMM' O:'N NT'
General Param 'Delay/String4' =  I:'Delay in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available' O:'N NT'
General Param 'Delay/String5' =  I:'Delay in format : HH:MM:SS.mmm (HH:MM:SS:FF)' O:'N NT'
General Param 'Delay_Settings' =  I:'Delay settings (in case of timecode, for example)' O:'N NTY'
General Param 'Delay_DropFrame' =  I:'Delay drop frame' O:'N NTY'
General Param 'Delay_Source' =  I:'Delay source (Container, Stream, empty)' O:'N NTY'
General Param 'Delay_Source/String' =  I:'Delay source (Container, Stream, empty)' O:'N NT'
General Param 'StreamSize' =  I:'Stream size (in bytes)' T:'0' M:' byte' O:'N YIY' m:' byte'
General Param (StreamSize/String)
General Param (StreamSize/String1)
General Param (StreamSize/String2)
General Param (StreamSize/String3)
General Param (StreamSize/String4)
General Param 'StreamSize/String5' =  I:'Stream size with proportion' T:'0.00 Byte (0%)' O:'N NT'
General Param 'StreamSize_Proportion' =  I:'Stream size divided by file size' T:'0.00000' O:'N NT'
General Param 'StreamSize_Demuxed' =  I:'StreamSize after demux (in bytes)' M:' byte' O:'N YIN' m:' byte'
General Param 'StreamSize_Demuxed/String' =  I:'StreamSize_Demuxed with percentage value' O:'N NT'
General Param (StreamSize_Demuxed/String1)
General Param (StreamSize_Demuxed/String2)
General Param (StreamSize_Demuxed/String3)
General Param (StreamSize_Demuxed/String4)
General Param 'StreamSize_Demuxed/String' =  I:'StreamSize_Demuxed with percentage value' O:'N NT'
General Param (HeaderSize)
General Param (DataSize)
General Param (FooterSize)
General Param (IsStreamable)
General Param 'Album_ReplayGain_Gain' =  I:'The gain to apply to reach 89dB SPL on playback' M:' dB' O:'N YTY' m:' dB'
General Param (Album_ReplayGain_Gain/String)
General Param 'Album_ReplayGain_Peak' =  I:'The maximum absolute peak value of the item' O:'Y YTY'
General Param (Encryption)
General Param (Encryption_Format)
General Param (Encryption_Length)
General Param (Encryption_Method)
General Param (Encryption_Mode)
General Param (Encryption_Padding)
General Param (Encryption_InitializationVector)
General Param 'UniversalAdID/String' =  I:'Universal Ad-ID, see https://ad-id.org' O:'Y NT'
General Param 'UniversalAdID_Registry' =  I:'Universal Ad-ID registry' O:'N YTY'
General Param 'UniversalAdID_Value' =  I:'Universal Ad-ID value' O:'N YTY'
General Param 'Title' =  I:'(Generic)Title of file' O:'N NTY' D:'Title'
General Param 'Title_More' =  I:'(Generic)More info about the title of file' O:'N NTY' D:'Title'
General Param 'Title/Url' =  I:'(Generic)Url' O:'N NT' D:'Title'
General Param 'Domain' =  I:'Universe movies belong to, e.g. Star Wars, Stargate, Buffy, Dragonball' O:'Y YTY' D:'Title'
General Param 'Collection' =  I:'Name of the series, e.g. Star Wars movies, Stargate SG-1, Stargate Atlantis, Buffy, Angel' O:'Y YTY' D:'Title'
General Param 'Season' =  I:'Name of the season, e.g. Star Wars first Trilogy, Season 1' O:'Y YTY' D:'Title'
General Param 'Season_Position' =  I:'Number of the Season' O:'Y YIY' D:'Title'
General Param 'Season_Position_Total' =  I:'Place of the season, e.g. 2 of 7' O:'Y YIY' D:'Title'
General Param 'Movie' =  I:'Name of the movie. Eg : Star Wars, A New Hope' O:'Y YTY' D:'Title'
General Param 'Movie_More' =  I:'More info about the movie' O:'Y YTY' D:'Title'
General Param 'Movie/Country' =  I:'Country, where the movie was produced' O:'Y YTY' D:'Title'
General Param 'Movie/Url' =  I:'Homepage for the movie' O:'Y YT' D:'Title'
General Param 'Album' =  I:'Name of an audio-album. Eg : The Joshua Tree' O:'Y YTY' D:'Title'
General Param 'Album_More' =  I:'More info about the album' O:'Y YTY' D:'Title'
General Param (Album/Sort)
General Param 'Album/Performer' =  I:'Album performer/artist of this file' O:'Y YTY' D:'Entity'
General Param (Album/Performer/Sort)
General Param 'Album/Performer/Url' =  I:'Homepage of the album performer/artist' O:'Y YT' D:'Entity'
General Param 'Comic' =  I:'Name of the comic.' O:'Y YTY' D:'Title'
General Param 'Comic_More' =  I:'More info about the comic' O:'Y YTY' D:'Title'
General Param 'Comic/Position_Total' =  I:'Place of the comic, e.g. 1 of 10' O:'Y YIY' D:'Title'
General Param 'Part' =  I:'Name of the part. e.g. CD1, CD2' O:'Y YTY' D:'Title'
General Param 'Part/Position' =  I:'Number of the part' O:'Y YIY' D:'Title'
General Param 'Part/Position_Total' =  I:'Place of the part e.g. 2 of 3' O:'Y YIY' D:'Title'
General Param 'Track' =  I:'Name of the track. e.g. track 1, track 2' O:'Y YTY' D:'Title'
General Param 'Track_More' =  I:'More info about the track' O:'Y YTY' D:'Title'
General Param 'Track/Url' =  I:'Link to a site about this track' O:'Y YT' D:'Title'
General Param (Track/Sort)
General Param 'Track/Position' =  I:'Number of this track' O:'Y YIY' D:'Title'
General Param 'Track/Position_Total' =  I:'Place of this track, e.g. 3 of 15' O:'Y YIY' D:'Title'
General Param 'PackageName' =  I:'Package name i.e. technical flavor of the content' O:'Y YTY'
General Param 'Grouping' =  I:'iTunes grouping' O:'Y YTY' D:'Title'
General Param 'Chapter' =  I:'Name of the chapter' O:'Y YTY' D:'Title'
General Param 'SubTrack' =  I:'Name of the subtrack' O:'Y YTY' D:'Title'
General Param 'Original/Album' =  I:'Original name of the album' O:'Y YTY' D:'Title'
General Param 'Original/Movie' =  I:'Original name of the movie' O:'Y YTY' D:'Title'
General Param 'Original/Part' =  I:'Original name of the part' O:'Y YTY' D:'Title'
General Param 'Original/Track' =  I:'Original name of the track' O:'Y YTY' D:'Title'
General Param 'Compilation' =  I:'iTunes compilation' M:'Yes' O:'N YTY' m:'Yes' D:'Title'
General Param 'Compilation/String' =  I:'iTunes compilation' O:'Y NT' D:'Title'
General Param 'Performer' =  I:'Main performer(s)/artist(s)' O:'Y YTY' D:'Entity'
General Param (Performer/Sort)
General Param 'Performer/Url' =  I:'Homepage of the performer/artist' O:'Y YT' D:'Entity'
General Param 'Original/Performer' =  I:'Original artist(s)/performer(s)' O:'Y YTY' D:'Entity'
General Param 'Accompaniment' =  I:'Band/orchestra/accompaniment/musician' O:'Y YTY' D:'Entity'
General Param 'Composer' =  I:'Name of the original composer' O:'Y YTY' D:'Entity'
General Param 'Composer/Nationality' =  I:'Nationality of the primary composer of the piece (mostly for classical music)' O:'Y YTY' D:'Entity'
General Param (Composer/Sort)
General Param 'Arranger' =  I:'The person who arranged the piece (e.g. Ravel)' O:'Y YTY' D:'Entity'
General Param 'Lyricist' =  I:'The person who wrote the lyrics for the piece' O:'Y YTY' D:'Entity'
General Param 'Original/Lyricist' =  I:'Original lyricist(s)/text writer(s).' O:'Y YTY' D:'Entity'
General Param 'Conductor' =  I:'The artist(s) who performed the work. In classical music this would be the conductor, orchestra, soloists, etc.' O:'Y YTY' D:'Entity'
General Param 'Director' =  I:'Name of the director' O:'Y YTY' D:'Entity'
General Param 'CoDirector' =  I:'Name of the codirector' O:'Y YTY' D:'Entity'
General Param 'AssistantDirector' =  I:'Name of the assistant director' O:'Y YTY' D:'Entity'
General Param 'DirectorOfPhotography' =  I:'Name of the director of photography, also known as cinematographer' O:'Y YTY' D:'Entity'
General Param 'SoundEngineer' =  I:'Name of the sound engineer or sound recordist' O:'Y YTY' D:'Entity'
General Param 'ArtDirector' =  I:'Name of the person who oversees the artists and craftspeople who build the sets' O:'Y YTY' D:'Entity'
General Param 'ProductionDesigner' =  I:'Name of the person responsible for designing the overall visual appearance of a movie' O:'Y YTY' D:'Entity'
General Param 'Choreographer' =  I:'Name of the choreographer' O:'Y YTY' D:'Entity'
General Param 'CostumeDesigner' =  I:'Name of the costume designer' O:'Y YTY' D:'Entity'
General Param 'Actor' =  I:'Real name of an actor/actress playing a role in the movie' O:'Y YTY' D:'Entity'
General Param 'Actor_Character' =  I:'Name of the character an actor or actress plays in this movie' O:'Y YTY' D:'Entity'
General Param 'WrittenBy' =  I:'Author of the story or script' O:'Y YTY' D:'Entity'
General Param 'ScreenplayBy' =  I:'Author of the screenplay or scenario (used for movies and TV shows)' O:'Y YTY' D:'Entity'
General Param 'EditedBy' =  I:'Editors name' O:'Y YTY' D:'Entity'
General Param 'CommissionedBy' =  I:'Name of the person or organization that commissioned the subject of the file' O:'Y YTY' D:'Entity'
General Param 'Producer' =  I:'Name of the producer of the movie' O:'Y YTY' D:'Entity'
General Param 'CoProducer' =  I:'Name of a co-producer' O:'Y YTY' D:'Entity'
General Param 'ExecutiveProducer' =  I:'Name of an executive producer' O:'Y YTY' D:'Entity'
General Param 'MusicBy' =  I:'Main musical artist for a movie' O:'Y YTY' D:'Entity'
General Param 'DistributedBy' =  I:'Company responsible for distribution of the content' O:'Y YTY' D:'Entity'
General Param 'OriginalSourceForm/Distri' =  I:'???'
General Param 'MasteredBy' =  I:'The engineer who mastered the content for a physical medium or for digital distribution' O:'Y YTY' D:'Entity'
General Param 'EncodedBy' =  I:'Name of the person/organisation that encoded/ripped the audio file.' O:'Y YTY' D:'Entity'
General Param 'RemixedBy' =  I:'Name of the artist(s) that interpreted, remixed, or otherwise modified the content' O:'Y YTY' D:'Entity'
General Param 'ProductionStudio' =  I:'Main production studio' O:'Y YTY' D:'Entity'
General Param 'ThanksTo' =  I:'A very general tag for everyone else that wants to be listed' O:'Y YTY' D:'Entity'
General Param 'Publisher' =  I:'Name of the organization publishing the album (i.e. the 'record label') or movie' O:'Y YTY' D:'Entity'
General Param 'Publisher/URL' =  I:'Publisher's official webpage' O:'Y YTY' D:'Entity'
General Param 'Label' =  I:'Brand or trademark associated with the marketing of music recordings and music videos' O:'Y YTY' D:'Entity'
General Param 'Genre' =  I:'Main genre of the audio or video. e.g. classical, ambient-house, synthpop, sci-fi, drama, etc.' O:'Y YTY' D:'Classification'
General Param 'PodcastCategory' =  I:'Podcast category' O:'Y YTY' D:'Classification'
General Param 'Mood' =  I:'Intended to reflect the mood of the item with a few keywords, e.g. Romantic, Sad, Uplifting, etc.' O:'Y YTY' D:'Classification'
General Param 'ContentType' =  I:'The type or genre of the content. e.g. Documentary, Feature Film, Cartoon, Music Video, Music, Sound FX, etc.' O:'Y YTY' D:'Classification'
General Param 'Subject' =  I:'Describes the topic of the file, such as 'Aerial view of Seattle.'' O:'Y YTY' D:'Classification'
General Param 'Description' =  I:'A short description of the contents, such as 'Two birds flying.'' O:'Y YTY' D:'Classification'
General Param 'Keywords' =  I:'Keywords for the content separated by a comma, used for searching' O:'Y YTY' D:'Classification'
General Param 'Summary' =  I:'Plot outline or a summary of the story' O:'Y YTY' D:'Classification'
General Param 'Synopsis' =  I:'Description of the story line of the item' O:'Y YTY' D:'Classification'
General Param 'Period' =  I:'Describes the period that the piece is from or about. e.g. Renaissance.' O:'Y YTY' D:'Classification'
General Param 'LawRating' =  I:'Legal rating of a movie. Format depends on country of origin (e.g. PG or R in the USA, an age in other countries or a URI defining a logo)' O:'Y YTY' D:'Classification'
General Param 'LawRating_Reason' =  I:'Reason for the law rating' O:'Y YTY' D:'Classification'
General Param 'ICRA' =  I:'The ICRA rating (previously RSACi)' O:'Y YTY' D:'Classification'
General Param 'Released_Date' =  I:'Date/year that the content was released' O:'Y YTY' D:'Temporal'
General Param 'Original/Released_Date' =  I:'Date/year that the content was originally released' O:'Y YTY' D:'Temporal'
General Param 'Recorded_Date' =  I:'Time/date/year that the recording began' O:'Y YTY' D:'Temporal'
General Param 'Encoded_Date' =  I:'Time/date/year that the encoding of this content was completed' O:'Y YTY' D:'Temporal'
General Param 'Tagged_Date' =  I:'Time/date/year that the tags were added to this content' O:'Y YTY' D:'Temporal'
General Param 'Written_Date' =  I:'Time/date/year that the composition of the music/script began' O:'Y YTY' D:'Temporal'
General Param 'Mastered_Date' =  I:'Time/date/year that the content was transferred to a digital medium.' O:'Y YTY' D:'Temporal'
General Param 'File_Created_Date' =  I:'Time that the file was created on the file system' T:'UTC 2022-09-17 08:12:46.389' O:'N NTY' D:'Temporal'
General Param 'File_Created_Date_Local' =  I:'Time that the file was created on the file system (Warning: this field depends of local configuration, do not use it in an international database)' T:'2022-09-17 10:12:46.389' O:'N NTY' D:'Temporal'
General Param 'File_Modified_Date' =  I:'Time that the file was last modified on the file system' T:'UTC 2022-09-17 11:29:29.185' O:'N NTY' D:'Temporal'
General Param 'File_Modified_Date_Local' =  I:'Time that the file was last modified on the file system (Warning: this field depends of local configuration, do not use it in an international database)' T:'2022-09-17 13:29:29.185' O:'N NTY' D:'Temporal'
General Param 'Recorded_Location' =  I:'Location where track was recorded (See COMPOSITION_LOCATION for format)' O:'Y YTY' D:'Spatial'
General Param 'Written_Location' =  I:'Location that the item was originally designed/written. Information should be stored in the following format: country code, state/province, city where the country code is the same 2 octets as in Internet domains, or possibly ISO-3166. e.g. US, Texas, Austin or US, , Austin.' O:'Y YTY' D:'Spatial'
General Param 'Archival_Location' =  I:'Location where an item is archived (e.g. Louvre, Paris, France)' O:'Y YTY' D:'Spatial'
General Param 'Encoded_Application' =  I:'Name of the software package used to create the file, such as Microsoft WaveEdiTY' O:'N YTY' D:'Technical'
General Param 'Encoded_Application/Strin' =  I:'???'
General Param 'Encoded_Application_Compa' =  I:'???'
General Param 'Encoded_Application_Name' =  I:'Name of the encoding product' O:'N YTY' D:'Technical'
General Param 'Encoded_Application_Versi' =  I:'???'
General Param 'Encoded_Application_Url' =  I:'URL associated with the encoding software' O:'N YTY' D:'Technical'
General Param 'Encoded_Library' =  I:'Software used to create the file' O:'N YTY' D:'Technical'
General Param 'Encoded_Library/String' =  I:'Software used to create the file, trying to have the format 'CompanyName ProductName (OperatingSystem) Version (Date)'' O:'Y NT' D:'Technical'
General Param 'Encoded_Library_CompanyNa' =  I:'???'
General Param 'Encoded_Library_Name' =  I:'Name of the the encoding library' O:'N NTY' D:'Technical'
General Param 'Encoded_Library_Version' =  I:'Version of encoding library' O:'N NTY' D:'Technical'
General Param 'Encoded_Library_Date' =  I:'Release date of encoding library' O:'N NTY' D:'Technical'
General Param 'Encoded_Library_Settings' =  I:'Parameters used by the encoding library' O:'Y YTY' D:'Technical'
General Param 'Encoded_OperatingSystem' =  I:'Operating System used by encoding application' O:'N YTY' D:'Technical'
General Param 'Cropped' =  I:'Describes whether an image has been cropped and, if so, how it was cropped' O:'Y YTY' D:'Technical'
General Param 'Dimensions' =  I:'Specifies the size of the original subject of the file (e.g. 8.5 in h, 11 in w)' O:'Y YTY' D:'Technical'
General Param 'DotsPerInch' =  I:'Stores dots per inch setting of the digitization mechanism used to produce the file' O:'Y YTY' D:'Technical'
General Param 'Lightness' =  I:'Describes the changes in lightness settings on the digitization mechanism made during the production of the file' O:'Y YTY' D:'Technical'
General Param 'OriginalSourceMedium' =  I:'Original medium of the material (e.g. vinyl, Audio-CD, Super8 or BetaMax)' O:'Y YTY' D:'Technical'
General Param 'OriginalSourceForm' =  I:'Original form of the material (e.g. slide, paper, map)' O:'Y YTY' D:'Technical'
General Param 'OriginalSourceForm/NumCol' =  I:'???'
General Param 'OriginalSourceForm/Name' =  I:'Name of the product the file was originally intended for' O:'Y YTY' D:'Technical'
General Param 'OriginalSourceForm/Croppe' =  I:'???'
General Param 'OriginalSourceForm/Sharpn' =  I:'???'
General Param 'Tagged_Application' =  I:'Software used to tag the file' O:'Y YTY' D:'Technical'
General Param 'BPM' =  I:'Average number of beats per minute' O:'Y YTY' D:'Technical'
General Param 'ISRC' =  I:'International Standard Recording Code, excluding the ISRC prefix and including hyphens' O:'Y YTY' D:'Identifier'
General Param 'ISBN' =  I:'International Standard Book Number.' O:'Y YTY' D:'Identifier'
General Param 'BarCode' =  I:'EAN-13 (13-digit European Article Numbering) or UPC-A (12-digit Universal Product Code) bar code identifier' O:'Y YTY' D:'Identifier'
General Param 'LCCN' =  I:'Library of Congress Control Number' O:'Y YTY' D:'Identifier'
General Param 'CatalogNumber' =  I:'A label-specific catalogue number used to identify the release. e.g. TIC 01' O:'Y YTY' D:'Identifier'
General Param 'LabelCode' =  I:'A 4-digit or 5-digit number to identify the record label, typically printed as (LC) xxxx or (LC) 0xxxx on CDs medias or covers, with only the number being stored' O:'Y YTY' D:'Identifier'
General Param 'Owner' =  I:'Owner of the file' O:'Y YTY' D:'Legal'
General Param 'Copyright' =  I:'Copyright attribution' O:'Y YTY' D:'Legal'
General Param 'Copyright/Url' =  I:'Link to a site with copyright/legal information' O:'Y YTY' D:'Legal'
General Param 'Producer_Copyright' =  I:'Copyright information as per the production copyright holder' O:'Y YTY' D:'Legal'
General Param 'TermsOfUse' =  I:'License information (e.g. All Rights Reserved, Any Use Permitted)' O:'Y YTY' D:'Legal'
General Param (ServiceName)
General Param (ServiceChannel)
General Param (Service/Url)
General Param (ServiceProvider)
General Param (ServiceProvider/Url)
General Param (ServiceType)
General Param (NetworkName)
General Param (OriginalNetworkName)
General Param (Country)
General Param (TimeZone)
General Param 'Cover' =  I:'Is there a cover' O:'Y YTY' D:'Info'
General Param 'Cover_Description' =  I:'Short description (e.g. Earth in space)' O:'Y YTY' D:'Info'
General Param (Cover_Type)
General Param 'Cover_Mime' =  I:'Mime type of cover file' O:'Y YTY' D:'Info'
General Param 'Cover_Data' =  I:'Cover, in binary format, encoded as BASE64' O:'N YTY' D:'Info'
General Param 'Lyrics' =  I:'Text of a song' O:'Y YTY' D:'Info'
General Param 'Comment' =  I:'Any comment related to the content' O:'Y YTY' D:'Personal'
General Param 'Rating' =  I:'A numeric value defining how much a person likes the song/movie. The number is between 0 and 5 with decimal values possible (e.g. 2.7), 5(.0) being the highest possible rating.' O:'Y YTY' D:'Personal'
General Param 'Added_Date' =  I:'Date/year the item was added to the owners collection' O:'Y YTY' D:'Personal'
General Param 'Played_First_Date' =  I:'Date the owner first played an item' O:'Y YTY' D:'Personal'
General Param 'Played_Last_Date' =  I:'Date the owner last played an item' O:'Y YTY' D:'Personal'
General Param 'Played_Count' =  I:'Number of times an item was played' O:'Y YIY' D:'Personal'
General Param (EPG_Positions_Begin)
General Param (EPG_Positions_End)
----->Video
Video Param 'Count' =  I:'???'
Video Param 'Status' =  I:'???'
Video Param 'StreamCount' =  I:'???'
Video Param 'StreamKind' =  I:'???'
Video Param 'StreamKind/String' =  I:'???'
Video Param 'StreamKindID' =  I:'???'
Video Param 'StreamKindPos' =  I:'???'
Video Param 'StreamOrder' =  I:'???'
Video Param 'FirstPacketOrder' =  I:'???'
Video Param 'Inform' =  I:'Inform'
Video Param 'ID' =  I:'???'
Video Param 'ID/String' =  I:'???'
Video Param 'OriginalSourceMedium_ID' =  I:'???'
Video Param 'OriginalSourceMedium_ID/S' =  I:'???'
Video Param 'UniqueID' =  I:'???'
Video Param 'UniqueID/String' =  I:'???'
Video Param 'MenuID' =  I:'???'
Video Param 'MenuID/String' =  I:'???'
Video Param 'Format' =  I:'???'
Video Param 'Format/String' =  I:'???'
Video Param 'Format/Info' =  I:'???'
Video Param 'Format/Url' =  I:'???'
Video Param 'Format_Commercial' =  I:'???'
Video Param 'Format_Commercial_IfAny' =  I:'???'
Video Param 'Format_Version' =  I:'???'
Video Param 'Format_Profile' =  I:'???'
Video Param 'Format_Level' =  I:'???'
Video Param 'Format_Tier' =  I:'???'
Video Param 'Format_Compression' =  I:'???'
Video Param 'Format_AdditionalFeatures' =  I:'???'
Video Param 'MultiView_BaseProfile' =  I:'???'
Video Param 'MultiView_Count' =  I:'???'
Video Param 'MultiView_Layout' =  I:'???'
Video Param 'HDR_Format' =  I:'???'
Video Param 'HDR_Format/String' =  I:'???'
Video Param 'HDR_Format_Commercial' =  I:'???'
Video Param 'HDR_Format_Version' =  I:'???'
Video Param 'HDR_Format_Profile' =  I:'???'
Video Param 'HDR_Format_Level' =  I:'???'
Video Param 'HDR_Format_Settings' =  I:'???'
Video Param 'HDR_Format_Compatibility' =  I:'???'
Video Param 'Format_Settings' =  I:'???'
Video Param 'Format_Settings_BVOP' =  I:'???'
Video Param 'Format_Settings_BVOP/Stri' =  I:'???'
Video Param 'Format_Settings_QPel' =  I:'???'
Video Param 'Format_Settings_QPel/Stri' =  I:'???'
Video Param 'Format_Settings_GMC' =  I:'???'
Video Param (Format_Settings_GMC/String)
Video Param 'Format_Settings_Matrix' =  I:'???'
Video Param 'Format_Settings_Matrix/St' =  I:'???'
Video Param 'Format_Settings_Matrix_Da' =  I:'???'
Video Param 'Format_Settings_CABAC' =  I:'???'
Video Param 'Format_Settings_CABAC/Str' =  I:'???'
Video Param 'Format_Settings_RefFrames' =  I:'???'
Video Param 'Format_Settings_RefFrames' =  I:'???'
Video Param 'Format_Settings_Pulldown' =  I:'???'
Video Param (Format_Settings_Endianness)
Video Param (Format_Settings_Packing)
Video Param 'Format_Settings_FrameMode' =  I:'???'
Video Param 'Format_Settings_GOP' =  I:'???'
Video Param 'Format_Settings_PictureSt' =  I:'???'
Video Param 'Format_Settings_Wrapping' =  I:'???'
Video Param 'InternetMediaType' =  I:'???'
Video Param 'MuxingMode' =  I:'???'
Video Param 'CodecID' =  I:'???'
Video Param 'CodecID/String' =  I:'???'
Video Param 'CodecID/Info' =  I:'???'
Video Param 'CodecID/Hint' =  I:'???'
Video Param 'CodecID/Url' =  I:'???'
Video Param 'CodecID_Description' =  I:'???'
Video Param 'Codec' =  I:'???'
Video Param 'Codec/String' =  I:'???'
Video Param 'Codec/Family' =  I:'???'
Video Param 'Codec/Info' =  I:'???'
Video Param 'Codec/Url' =  I:'???'
Video Param 'Codec/CC' =  I:'???'
Video Param 'Codec_Profile' =  I:'???'
Video Param 'Codec_Description' =  I:'???'
Video Param 'Codec_Settings' =  I:'???'
Video Param 'Codec_Settings_PacketBitS' =  I:'???'
Video Param 'Codec_Settings_BVOP' =  I:'???'
Video Param 'Codec_Settings_QPel' =  I:'???'
Video Param 'Codec_Settings_GMC' =  I:'???'
Video Param 'Codec_Settings_GMC/String' =  I:'???'
Video Param 'Codec_Settings_Matrix' =  I:'???'
Video Param 'Codec_Settings_Matrix_Dat' =  I:'???'
Video Param 'Codec_Settings_CABAC' =  I:'???'
Video Param 'Codec_Settings_RefFrames' =  I:'???'
Video Param 'Duration' =  I:'???'
Video Param 'Duration/String' =  I:'???'
Video Param 'Duration/String1' =  I:'???'
Video Param 'Duration/String2' =  I:'???'
Video Param 'Duration/String3' =  I:'???'
Video Param 'Duration/String4' =  I:'???'
Video Param 'Duration/String5' =  I:'???'
Video Param 'Duration_FirstFrame' =  I:'???'
Video Param 'Duration_FirstFrame/Strin' =  I:'???'
Video Param 'Duration_FirstFrame/Strin' =  I:'???'
Video Param 'Duration_FirstFrame/Strin' =  I:'???'
Video Param 'Duration_FirstFrame/Strin' =  I:'???'
Video Param 'Duration_FirstFrame/Strin' =  I:'???'
Video Param 'Duration_FirstFrame/Strin' =  I:'???'
Video Param 'Duration_LastFrame' =  I:'???'
Video Param 'Duration_LastFrame/String' =  I:'???'
Video Param 'Duration_LastFrame/String' =  I:'???'
Video Param 'Duration_LastFrame/String' =  I:'???'
Video Param 'Duration_LastFrame/String' =  I:'???'
Video Param 'Duration_LastFrame/String' =  I:'???'
Video Param 'Duration_LastFrame/String' =  I:'???'
Video Param 'Source_Duration' =  I:'???'
Video Param 'Source_Duration/String' =  I:'???'
Video Param 'Source_Duration/String1' =  I:'???'
Video Param 'Source_Duration/String2' =  I:'???'
Video Param 'Source_Duration/String3' =  I:'???'
Video Param 'Source_Duration/String4' =  I:'???'
Video Param 'Source_Duration/String5' =  I:'???'
Video Param 'Source_Duration_FirstFram' =  I:'???'
Video Param 'Source_Duration_FirstFram' =  I:'???'
Video Param 'Source_Duration_FirstFram' =  I:'???'
Video Param 'Source_Duration_FirstFram' =  I:'???'
Video Param 'Source_Duration_FirstFram' =  I:'???'
Video Param 'Source_Duration_FirstFram' =  I:'???'
Video Param 'Source_Duration_FirstFram' =  I:'???'
Video Param 'Source_Duration_LastFrame' =  I:'???'
Video Param 'Source_Duration_LastFrame' =  I:'???'
Video Param 'Source_Duration_LastFrame' =  I:'???'
Video Param 'Source_Duration_LastFrame' =  I:'???'
Video Param 'Source_Duration_LastFrame' =  I:'???'
Video Param 'Source_Duration_LastFrame' =  I:'???'
Video Param 'Source_Duration_LastFrame' =  I:'???'
Video Param 'BitRate_Mode' =  I:'???'
Video Param 'BitRate_Mode/String' =  I:'???'
Video Param 'BitRate' =  I:'???'
Video Param 'BitRate/String' =  I:'???'
Video Param 'BitRate_Minimum' =  I:'???'
Video Param 'BitRate_Minimum/String' =  I:'???'
Video Param 'BitRate_Nominal' =  I:'???'
Video Param 'BitRate_Nominal/String' =  I:'???'
Video Param 'BitRate_Maximum' =  I:'???'
Video Param 'BitRate_Maximum/String' =  I:'???'
Video Param 'BitRate_Encoded' =  I:'???'
Video Param 'BitRate_Encoded/String' =  I:'???'
Video Param 'Width' =  I:'???'
Video Param 'Width/String' =  I:'???'
Video Param 'Width_Offset' =  I:'???'
Video Param 'Width_Offset/String' =  I:'???'
Video Param 'Width_Original' =  I:'???'
Video Param 'Width_Original/String' =  I:'???'
Video Param 'Width_CleanAperture' =  I:'???'
Video Param 'Width_CleanAperture/Strin' =  I:'???'
Video Param 'Height' =  I:'???'
Video Param 'Height/String' =  I:'???'
Video Param 'Height_Offset' =  I:'???'
Video Param 'Height_Offset/String' =  I:'???'
Video Param 'Height_Original' =  I:'???'
Video Param 'Height_Original/String' =  I:'???'
Video Param 'Height_CleanAperture' =  I:'???'
Video Param 'Height_CleanAperture/Stri' =  I:'???'
Video Param 'Stored_Width' =  I:'???'
Video Param 'Stored_Height' =  I:'???'
Video Param 'Sampled_Width' =  I:'???'
Video Param 'Sampled_Height' =  I:'???'
Video Param 'PixelAspectRatio' =  I:'???'
Video Param 'PixelAspectRatio/String' =  I:'???'
Video Param 'PixelAspectRatio_Original' =  I:'???'
Video Param 'PixelAspectRatio_Original' =  I:'???'
Video Param 'PixelAspectRatio_CleanApe' =  I:'???'
Video Param 'PixelAspectRatio_CleanApe' =  I:'???'
Video Param 'DisplayAspectRatio' =  I:'???'
Video Param 'DisplayAspectRatio/String' =  I:'???'
Video Param 'DisplayAspectRatio_Origin' =  I:'???'
Video Param 'DisplayAspectRatio_Origin' =  I:'???'
Video Param 'DisplayAspectRatio_CleanA' =  I:'???'
Video Param 'DisplayAspectRatio_CleanA' =  I:'???'
Video Param 'ActiveFormatDescription' =  I:'???'
Video Param 'ActiveFormatDescription/S' =  I:'???'
Video Param 'ActiveFormatDescription_M' =  I:'???'
Video Param 'Rotation' =  I:'???'
Video Param 'Rotation/String' =  I:'???'
Video Param 'FrameRate_Mode' =  I:'???'
Video Param 'FrameRate_Mode/String' =  I:'???'
Video Param 'FrameRate_Mode_Original' =  I:'???'
Video Param 'FrameRate_Mode_Original/S' =  I:'???'
Video Param 'FrameRate' =  I:'???'
Video Param 'FrameRate/String' =  I:'???'
Video Param 'FrameRate_Num' =  I:'???'
Video Param 'FrameRate_Den' =  I:'???'
Video Param 'FrameRate_Minimum' =  I:'???'
Video Param 'FrameRate_Minimum/String' =  I:'???'
Video Param 'FrameRate_Nominal' =  I:'???'
Video Param 'FrameRate_Nominal/String' =  I:'???'
Video Param 'FrameRate_Maximum' =  I:'???'
Video Param 'FrameRate_Maximum/String' =  I:'???'
Video Param 'FrameRate_Original' =  I:'???'
Video Param 'FrameRate_Original/String' =  I:'???'
Video Param 'FrameRate_Original_Num' =  I:'???'
Video Param 'FrameRate_Original_Den' =  I:'???'
Video Param 'FrameCount' =  I:'???'
Video Param 'Source_FrameCount' =  I:'???'
Video Param 'Standard' =  I:'???'
Video Param 'Resolution' =  I:'???'
Video Param 'Resolution/String' =  I:'???'
Video Param 'Colorimetry' =  I:'???'
Video Param (ColorSpace)
Video Param (ChromaSubsampling)
Video Param (ChromaSubsampling/String)
Video Param (ChromaSubsampling_Position)
Video Param 'BitDepth' =  I:'???'
Video Param 'BitDepth/String' =  I:'???'
Video Param (ScanType)
Video Param (ScanType/String)
Video Param (ScanType_Original)
Video Param (ScanType_Original/String)
Video Param (ScanType_StoreMethod)
Video Param (ScanType_StoreMethod_FieldsPerBlock)
Video Param (ScanType_StoreMethod/String)
Video Param (ScanOrder)
Video Param (ScanOrder/String)
Video Param (ScanOrder_Stored)
Video Param (ScanOrder_Stored/String)
Video Param (ScanOrder_StoredDisplayedInverted)
Video Param (ScanOrder_Original)
Video Param (ScanOrder_Original/String)
Video Param 'Interlacement' =  I:'???'
Video Param 'Interlacement/String' =  I:'???'
Video Param 'Compression_Mode' =  I:'???'
Video Param 'Compression_Mode/String' =  I:'???'
Video Param 'Compression_Ratio' =  I:'???'
Video Param 'Bits-(Pixel*Frame)' =  I:'???'
Video Param 'Delay' =  I:'???'
Video Param 'Delay/String' =  I:'???'
Video Param 'Delay/String1' =  I:'???'
Video Param 'Delay/String2' =  I:'???'
Video Param 'Delay/String3' =  I:'???'
Video Param 'Delay/String4' =  I:'???'
Video Param 'Delay/String5' =  I:'???'
Video Param 'Delay_Settings' =  I:'???'
Video Param 'Delay_DropFrame' =  I:'???'
Video Param 'Delay_Source' =  I:'???'
Video Param 'Delay_Source/String' =  I:'???'
Video Param 'Delay_Original' =  I:'???'
Video Param 'Delay_Original/String' =  I:'???'
Video Param 'Delay_Original/String1' =  I:'???'
Video Param 'Delay_Original/String2' =  I:'???'
Video Param 'Delay_Original/String3' =  I:'???'
Video Param 'Delay_Original/String4' =  I:'???'
Video Param 'Delay_Original/String5' =  I:'???'
Video Param 'Delay_Original_Settings' =  I:'???'
Video Param 'Delay_Original_DropFrame' =  I:'???'
Video Param 'Delay_Original_Source' =  I:'???'
Video Param 'TimeStamp_FirstFrame' =  I:'???'
Video Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Video Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Video Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Video Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Video Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Video Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Video Param 'TimeCode_FirstFrame' =  I:'???'
Video Param 'TimeCode_Settings' =  I:'???'
Video Param 'TimeCode_Source' =  I:'???'
Video Param 'Gop_OpenClosed' =  I:'???'
Video Param 'Gop_OpenClosed/String' =  I:'???'
Video Param 'Gop_OpenClosed_FirstFrame' =  I:'???'
Video Param 'Gop_OpenClosed_FirstFrame' =  I:'???'
Video Param 'StreamSize' =  I:'???'
Video Param 'StreamSize/String' =  I:'???'
Video Param (StreamSize/String1)
Video Param (StreamSize/String2)
Video Param (StreamSize/String3)
Video Param (StreamSize/String4)
Video Param 'StreamSize/String5' =  I:'???'
Video Param 'StreamSize_Proportion' =  I:'???'
Video Param 'StreamSize_Demuxed' =  I:'???'
Video Param 'StreamSize_Demuxed/String' =  I:'???'
Video Param (StreamSize_Demuxed/String1)
Video Param (StreamSize_Demuxed/String2)
Video Param (StreamSize_Demuxed/String3)
Video Param (StreamSize_Demuxed/String4)
Video Param 'StreamSize_Demuxed/String' =  I:'???'
Video Param 'Source_StreamSize' =  I:'???'
Video Param 'Source_StreamSize/String' =  I:'???'
Video Param (Source_StreamSize/String1)
Video Param (Source_StreamSize/String2)
Video Param (Source_StreamSize/String3)
Video Param (Source_StreamSize/String4)
Video Param 'Source_StreamSize/String5' =  I:'???'
Video Param 'Source_StreamSize_Proport' =  I:'???'
Video Param 'StreamSize_Encoded' =  I:'???'
Video Param 'StreamSize_Encoded/String' =  I:'???'
Video Param (StreamSize_Encoded/String1)
Video Param (StreamSize_Encoded/String2)
Video Param (StreamSize_Encoded/String3)
Video Param (StreamSize_Encoded/String4)
Video Param 'StreamSize_Encoded/String' =  I:'???'
Video Param 'StreamSize_Encoded_Propor' =  I:'???'
Video Param 'Source_StreamSize_Encoded' =  I:'???'
Video Param 'Source_StreamSize_Encoded' =  I:'???'
Video Param (Source_StreamSize_Encoded/String1)
Video Param (Source_StreamSize_Encoded/String2)
Video Param (Source_StreamSize_Encoded/String3)
Video Param (Source_StreamSize_Encoded/String4)
Video Param 'Source_StreamSize_Encoded' =  I:'???'
Video Param 'Source_StreamSize_Encoded' =  I:'???'
Video Param 'Alignment' =  I:'???'
Video Param (Alignment/String)
Video Param 'Title' =  I:'???'
Video Param 'Encoded_Application' =  I:'???'
Video Param 'Encoded_Application/Strin' =  I:'???'
Video Param 'Encoded_Application_Compa' =  I:'???'
Video Param 'Encoded_Application_Name' =  I:'???'
Video Param 'Encoded_Application_Versi' =  I:'???'
Video Param 'Encoded_Application_Url' =  I:'???'
Video Param 'Encoded_Library' =  I:'???'
Video Param 'Encoded_Library/String' =  I:'???'
Video Param 'Encoded_Library_CompanyNa' =  I:'???'
Video Param 'Encoded_Library_Name' =  I:'???'
Video Param 'Encoded_Library_Version' =  I:'???'
Video Param 'Encoded_Library_Date' =  I:'???'
Video Param 'Encoded_Library_Settings' =  I:'???'
Video Param 'Encoded_OperatingSystem' =  I:'???'
Video Param 'Language' =  I:'???'
Video Param 'Language/String' =  I:'???'
Video Param 'Language/String1' =  I:'???'
Video Param 'Language/String2' =  I:'???'
Video Param 'Language/String3' =  I:'???'
Video Param 'Language/String4' =  I:'???'
Video Param 'Language_More' =  I:'???'
Video Param 'ServiceKind' =  I:'???'
Video Param 'ServiceKind/String' =  I:'???'
Video Param 'Disabled' =  I:'???'
Video Param 'Disabled/String' =  I:'???'
Video Param 'Default' =  I:'???'
Video Param 'Default/String' =  I:'???'
Video Param 'Forced' =  I:'???'
Video Param 'Forced/String' =  I:'???'
Video Param 'AlternateGroup' =  I:'???'
Video Param 'AlternateGroup/String' =  I:'???'
Video Param 'Encoded_Date' =  I:'???'
Video Param 'Tagged_Date' =  I:'???'
Video Param (Encryption)
Video Param 'BufferSize' =  I:'???'
Video Param 'colour_description_presen' =  I:'???'
Video Param 'colour_description_presen' =  I:'???'
Video Param 'colour_description_presen' =  I:'???'
Video Param 'colour_description_presen' =  I:'???'
Video Param 'colour_range' =  I:'???'
Video Param 'colour_range_Source' =  I:'???'
Video Param 'colour_range_Original' =  I:'???'
Video Param 'colour_range_Original_Sou' =  I:'???'
Video Param 'colour_primaries' =  I:'???'
Video Param 'colour_primaries_Source' =  I:'???'
Video Param 'colour_primaries_Original' =  I:'???'
Video Param 'colour_primaries_Original' =  I:'???'
Video Param 'transfer_characteristics' =  I:'???'
Video Param 'transfer_characteristics_' =  I:'???'
Video Param 'transfer_characteristics_' =  I:'???'
Video Param 'transfer_characteristics_' =  I:'???'
Video Param 'matrix_coefficients' =  I:'???'
Video Param 'matrix_coefficients_Sourc' =  I:'???'
Video Param 'matrix_coefficients_Origi' =  I:'???'
Video Param 'matrix_coefficients_Origi' =  I:'???'
Video Param 'MasteringDisplay_ColorPri' =  I:'???'
Video Param 'MasteringDisplay_ColorPri' =  I:'???'
Video Param 'MasteringDisplay_ColorPri' =  I:'???'
Video Param 'MasteringDisplay_ColorPri' =  I:'???'
Video Param 'MasteringDisplay_Luminanc' =  I:'???'
Video Param 'MasteringDisplay_Luminanc' =  I:'???'
Video Param 'MasteringDisplay_Luminanc' =  I:'???'
Video Param 'MasteringDisplay_Luminanc' =  I:'???'
Video Param 'MaxCLL' =  I:'???'
Video Param 'MaxCLL_Source' =  I:'???'
Video Param 'MaxCLL_Original' =  I:'???'
Video Param 'MaxCLL_Original_Source' =  I:'???'
Video Param 'MaxFALL' =  I:'???'
Video Param 'MaxFALL_Source' =  I:'???'
Video Param 'MaxFALL_Original' =  I:'???'
Video Param 'MaxFALL_Original_Source' =  I:'???'
----->Audio
Audio Param 'Count' =  I:'???'
Audio Param 'Status' =  I:'???'
Audio Param 'StreamCount' =  I:'???'
Audio Param 'StreamKind' =  I:'???'
Audio Param 'StreamKind/String' =  I:'???'
Audio Param 'StreamKindID' =  I:'???'
Audio Param 'StreamKindPos' =  I:'???'
Audio Param 'StreamOrder' =  I:'???'
Audio Param 'FirstPacketOrder' =  I:'???'
Audio Param 'Inform' =  I:'Inform'
Audio Param 'ID' =  I:'???'
Audio Param 'ID/String' =  I:'???'
Audio Param 'OriginalSourceMedium_ID' =  I:'???'
Audio Param 'OriginalSourceMedium_ID/S' =  I:'???'
Audio Param 'UniqueID' =  I:'???'
Audio Param 'UniqueID/String' =  I:'???'
Audio Param 'MenuID' =  I:'???'
Audio Param 'MenuID/String' =  I:'???'
Audio Param 'Format' =  I:'???'
Audio Param 'Format/String' =  I:'???'
Audio Param 'Format/Info' =  I:'???'
Audio Param 'Format/Url' =  I:'???'
Audio Param 'Format_Commercial' =  I:'???'
Audio Param 'Format_Commercial_IfAny' =  I:'???'
Audio Param 'Format_Version' =  I:'???'
Audio Param 'Format_Profile' =  I:'???'
Audio Param 'Format_Level' =  I:'???'
Audio Param 'Format_Compression' =  I:'???'
Audio Param 'Format_Settings' =  I:'???'
Audio Param (Format_Settings_SBR)
Audio Param (Format_Settings_SBR/String)
Audio Param (Format_Settings_PS)
Audio Param (Format_Settings_PS/String)
Audio Param (Format_Settings_Mode)
Audio Param (Format_Settings_ModeExtension)
Audio Param (Format_Settings_Emphasis)
Audio Param (Format_Settings_Floor)
Audio Param (Format_Settings_Firm)
Audio Param (Format_Settings_Endianness)
Audio Param (Format_Settings_Sign)
Audio Param (Format_Settings_Law)
Audio Param (Format_Settings_ITU)
Audio Param 'Format_Settings_Wrapping' =  I:'???'
Audio Param 'Format_AdditionalFeatures' =  I:'???'
Audio Param 'Matrix_Format' =  I:'???'
Audio Param 'InternetMediaType' =  I:'???'
Audio Param 'MuxingMode' =  I:'???'
Audio Param 'MuxingMode_MoreInfo' =  I:'???'
Audio Param 'CodecID' =  I:'???'
Audio Param 'CodecID/String' =  I:'???'
Audio Param 'CodecID/Info' =  I:'???'
Audio Param 'CodecID/Hint' =  I:'???'
Audio Param 'CodecID/Url' =  I:'???'
Audio Param 'CodecID_Description' =  I:'???'
Audio Param 'Codec' =  I:'???'
Audio Param 'Codec/String' =  I:'???'
Audio Param 'Codec/Family' =  I:'???'
Audio Param 'Codec/Info' =  I:'???'
Audio Param 'Codec/Url' =  I:'???'
Audio Param 'Codec/CC' =  I:'???'
Audio Param 'Codec_Description' =  I:'???'
Audio Param 'Codec_Profile' =  I:'???'
Audio Param 'Codec_Settings' =  I:'???'
Audio Param 'Codec_Settings_Automatic' =  I:'???'
Audio Param 'Codec_Settings_Floor' =  I:'???'
Audio Param 'Codec_Settings_Firm' =  I:'???'
Audio Param 'Codec_Settings_Endianness' =  I:'???'
Audio Param 'Codec_Settings_Sign' =  I:'???'
Audio Param 'Codec_Settings_Law' =  I:'???'
Audio Param 'Codec_Settings_ITU' =  I:'???'
Audio Param 'Duration' =  I:'???'
Audio Param 'Duration/String' =  I:'???'
Audio Param 'Duration/String1' =  I:'???'
Audio Param 'Duration/String2' =  I:'???'
Audio Param 'Duration/String3' =  I:'???'
Audio Param 'Duration/String4' =  I:'???'
Audio Param 'Duration/String5' =  I:'???'
Audio Param 'Duration_FirstFrame' =  I:'???'
Audio Param 'Duration_FirstFrame/Strin' =  I:'???'
Audio Param 'Duration_FirstFrame/Strin' =  I:'???'
Audio Param 'Duration_FirstFrame/Strin' =  I:'???'
Audio Param 'Duration_FirstFrame/Strin' =  I:'???'
Audio Param 'Duration_FirstFrame/Strin' =  I:'???'
Audio Param 'Duration_FirstFrame/Strin' =  I:'???'
Audio Param 'Duration_LastFrame' =  I:'???'
Audio Param 'Duration_LastFrame/String' =  I:'???'
Audio Param 'Duration_LastFrame/String' =  I:'???'
Audio Param 'Duration_LastFrame/String' =  I:'???'
Audio Param 'Duration_LastFrame/String' =  I:'???'
Audio Param 'Duration_LastFrame/String' =  I:'???'
Audio Param 'Duration_LastFrame/String' =  I:'???'
Audio Param 'Source_Duration' =  I:'???'
Audio Param 'Source_Duration/String' =  I:'???'
Audio Param 'Source_Duration/String1' =  I:'???'
Audio Param 'Source_Duration/String2' =  I:'???'
Audio Param 'Source_Duration/String3' =  I:'???'
Audio Param 'Source_Duration/String4' =  I:'???'
Audio Param 'Source_Duration/String5' =  I:'???'
Audio Param 'Source_Duration_FirstFram' =  I:'???'
Audio Param 'Source_Duration_FirstFram' =  I:'???'
Audio Param 'Source_Duration_FirstFram' =  I:'???'
Audio Param 'Source_Duration_FirstFram' =  I:'???'
Audio Param 'Source_Duration_FirstFram' =  I:'???'
Audio Param 'Source_Duration_FirstFram' =  I:'???'
Audio Param 'Source_Duration_FirstFram' =  I:'???'
Audio Param 'Source_Duration_LastFrame' =  I:'???'
Audio Param 'Source_Duration_LastFrame' =  I:'???'
Audio Param 'Source_Duration_LastFrame' =  I:'???'
Audio Param 'Source_Duration_LastFrame' =  I:'???'
Audio Param 'Source_Duration_LastFrame' =  I:'???'
Audio Param 'Source_Duration_LastFrame' =  I:'???'
Audio Param 'Source_Duration_LastFrame' =  I:'???'
Audio Param 'BitRate_Mode' =  I:'???'
Audio Param 'BitRate_Mode/String' =  I:'???'
Audio Param 'BitRate' =  I:'???'
Audio Param 'BitRate/String' =  I:'???'
Audio Param 'BitRate_Minimum' =  I:'???'
Audio Param 'BitRate_Minimum/String' =  I:'???'
Audio Param 'BitRate_Nominal' =  I:'???'
Audio Param 'BitRate_Nominal/String' =  I:'???'
Audio Param 'BitRate_Maximum' =  I:'???'
Audio Param 'BitRate_Maximum/String' =  I:'???'
Audio Param 'BitRate_Encoded' =  I:'???'
Audio Param 'BitRate_Encoded/String' =  I:'???'
Audio Param 'Channel(s)' =  I:'???'
Audio Param 'Channel(s)/String' =  I:'???'
Audio Param 'Channel(s)_Original' =  I:'???'
Audio Param 'Channel(s)_Original/Strin' =  I:'???'
Audio Param 'Matrix_Channel(s)' =  I:'???'
Audio Param 'Matrix_Channel(s)/String' =  I:'???'
Audio Param 'ChannelPositions' =  I:'???'
Audio Param 'ChannelPositions_Original' =  I:'???'
Audio Param 'ChannelPositions/String2' =  I:'???'
Audio Param 'ChannelPositions_Original' =  I:'???'
Audio Param 'Matrix_ChannelPositions' =  I:'???'
Audio Param 'Matrix_ChannelPositions/S' =  I:'???'
Audio Param 'ChannelLayout' =  I:'???'
Audio Param 'ChannelLayout_Original' =  I:'???'
Audio Param 'ChannelLayoutID' =  I:'???'
Audio Param 'SamplesPerFrame' =  I:'???'
Audio Param 'SamplingRate' =  I:'???'
Audio Param 'SamplingRate/String' =  I:'???'
Audio Param 'SamplingCount' =  I:'???'
Audio Param 'Source_SamplingCount' =  I:'???'
Audio Param 'FrameRate' =  I:'???'
Audio Param 'FrameRate/String' =  I:'???'
Audio Param 'FrameRate_Num' =  I:'???'
Audio Param 'FrameRate_Den' =  I:'???'
Audio Param 'FrameCount' =  I:'???'
Audio Param 'Source_FrameCount' =  I:'???'
Audio Param 'Resolution' =  I:'???'
Audio Param 'Resolution/String' =  I:'???'
Audio Param 'BitDepth' =  I:'???'
Audio Param 'BitDepth/String' =  I:'???'
Audio Param 'BitDepth_Detected' =  I:'???'
Audio Param 'BitDepth_Detected/String' =  I:'???'
Audio Param 'BitDepth_Stored' =  I:'???'
Audio Param 'BitDepth_Stored/String' =  I:'???'
Audio Param 'Compression_Mode' =  I:'???'
Audio Param 'Compression_Mode/String' =  I:'???'
Audio Param 'Compression_Ratio' =  I:'???'
Audio Param 'Delay' =  I:'???'
Audio Param 'Delay/String' =  I:'???'
Audio Param 'Delay/String1' =  I:'???'
Audio Param 'Delay/String2' =  I:'???'
Audio Param 'Delay/String3' =  I:'???'
Audio Param 'Delay/String4' =  I:'???'
Audio Param 'Delay/String5' =  I:'???'
Audio Param 'Delay_Settings' =  I:'???'
Audio Param 'Delay_DropFrame' =  I:'???'
Audio Param 'Delay_Source' =  I:'???'
Audio Param 'Delay_Source/String' =  I:'???'
Audio Param 'Delay_Original' =  I:'???'
Audio Param 'Delay_Original/String' =  I:'???'
Audio Param 'Delay_Original/String1' =  I:'???'
Audio Param 'Delay_Original/String2' =  I:'???'
Audio Param 'Delay_Original/String3' =  I:'???'
Audio Param 'Delay_Original/String4' =  I:'???'
Audio Param 'Delay_Original/String5' =  I:'???'
Audio Param 'Delay_Original_Settings' =  I:'???'
Audio Param 'Delay_Original_DropFrame' =  I:'???'
Audio Param 'Delay_Original_Source' =  I:'???'
Audio Param 'Video_Delay' =  I:'???'
Audio Param (Video_Delay/String)
Audio Param (Video_Delay/String1)
Audio Param (Video_Delay/String2)
Audio Param (Video_Delay/String3)
Audio Param (Video_Delay/String4)
Audio Param (Video_Delay/String5)
Audio Param 'Video0_Delay' =  I:'???'
Audio Param 'Video0_Delay/String' =  I:'???'
Audio Param 'Video0_Delay/String1' =  I:'???'
Audio Param 'Video0_Delay/String2' =  I:'???'
Audio Param 'Video0_Delay/String3' =  I:'???'
Audio Param 'Video0_Delay/String4' =  I:'???'
Audio Param 'Video0_Delay/String5' =  I:'???'
Audio Param 'ReplayGain_Gain' =  I:'???'
Audio Param (ReplayGain_Gain/String)
Audio Param 'ReplayGain_Peak' =  I:'???'
Audio Param 'StreamSize' =  I:'???'
Audio Param 'StreamSize/String' =  I:'???'
Audio Param (StreamSize/String1)
Audio Param (StreamSize/String2)
Audio Param (StreamSize/String3)
Audio Param (StreamSize/String4)
Audio Param 'StreamSize/String5' =  I:'???'
Audio Param 'StreamSize_Proportion' =  I:'???'
Audio Param 'StreamSize_Demuxed' =  I:'???'
Audio Param 'StreamSize_Demuxed/String' =  I:'???'
Audio Param (StreamSize_Demuxed/String1)
Audio Param (StreamSize_Demuxed/String2)
Audio Param (StreamSize_Demuxed/String3)
Audio Param (StreamSize_Demuxed/String4)
Audio Param 'StreamSize_Demuxed/String' =  I:'???'
Audio Param 'Source_StreamSize' =  I:'???'
Audio Param 'Source_StreamSize/String' =  I:'???'
Audio Param (Source_StreamSize/String1)
Audio Param (Source_StreamSize/String2)
Audio Param (Source_StreamSize/String3)
Audio Param (Source_StreamSize/String4)
Audio Param 'Source_StreamSize/String5' =  I:'???'
Audio Param 'Source_StreamSize_Proport' =  I:'???'
Audio Param 'StreamSize_Encoded' =  I:'???'
Audio Param 'StreamSize_Encoded/String' =  I:'???'
Audio Param (StreamSize_Encoded/String1)
Audio Param (StreamSize_Encoded/String2)
Audio Param (StreamSize_Encoded/String3)
Audio Param (StreamSize_Encoded/String4)
Audio Param 'StreamSize_Encoded/String' =  I:'???'
Audio Param 'StreamSize_Encoded_Propor' =  I:'???'
Audio Param 'Source_StreamSize_Encoded' =  I:'???'
Audio Param 'Source_StreamSize_Encoded' =  I:'???'
Audio Param (Source_StreamSize_Encoded/String1)
Audio Param (Source_StreamSize_Encoded/String2)
Audio Param (Source_StreamSize_Encoded/String3)
Audio Param (Source_StreamSize_Encoded/String4)
Audio Param 'Source_StreamSize_Encoded' =  I:'???'
Audio Param 'Source_StreamSize_Encoded' =  I:'???'
Audio Param 'Alignment' =  I:'???'
Audio Param 'Alignment/String' =  I:'???'
Audio Param 'Interleave_VideoFrames' =  I:'???'
Audio Param 'Interleave_Duration' =  I:'???'
Audio Param 'Interleave_Duration/Strin' =  I:'???'
Audio Param 'Interleave_Preload' =  I:'???'
Audio Param 'Interleave_Preload/String' =  I:'???'
Audio Param 'Title' =  I:'???'
Audio Param 'Encoded_Application' =  I:'???'
Audio Param 'Encoded_Application/Strin' =  I:'???'
Audio Param 'Encoded_Application_Compa' =  I:'???'
Audio Param 'Encoded_Application_Name' =  I:'???'
Audio Param 'Encoded_Application_Versi' =  I:'???'
Audio Param 'Encoded_Application_Url' =  I:'???'
Audio Param 'Encoded_Library' =  I:'???'
Audio Param 'Encoded_Library/String' =  I:'???'
Audio Param 'Encoded_Library_CompanyNa' =  I:'???'
Audio Param 'Encoded_Library_Name' =  I:'???'
Audio Param 'Encoded_Library_Version' =  I:'???'
Audio Param 'Encoded_Library_Date' =  I:'???'
Audio Param 'Encoded_Library_Settings' =  I:'???'
Audio Param 'Encoded_OperatingSystem' =  I:'???'
Audio Param 'Language' =  I:'???'
Audio Param 'Language/String' =  I:'???'
Audio Param 'Language/String1' =  I:'???'
Audio Param 'Language/String2' =  I:'???'
Audio Param 'Language/String3' =  I:'???'
Audio Param 'Language/String4' =  I:'???'
Audio Param 'Language_More' =  I:'???'
Audio Param 'ServiceKind' =  I:'???'
Audio Param 'ServiceKind/String' =  I:'???'
Audio Param 'Disabled' =  I:'???'
Audio Param 'Disabled/String' =  I:'???'
Audio Param 'Default' =  I:'???'
Audio Param 'Default/String' =  I:'???'
Audio Param 'Forced' =  I:'???'
Audio Param 'Forced/String' =  I:'???'
Audio Param 'AlternateGroup' =  I:'???'
Audio Param 'AlternateGroup/String' =  I:'???'
Audio Param 'Encoded_Date' =  I:'???'
Audio Param 'Tagged_Date' =  I:'???'
Audio Param (Encryption)
----->Text
Text Param 'Count' =  I:'???'
Text Param 'Status' =  I:'???'
Text Param 'StreamCount' =  I:'???'
Text Param 'StreamKind' =  I:'???'
Text Param 'StreamKind/String' =  I:'???'
Text Param 'StreamKindID' =  I:'???'
Text Param 'StreamKindPos' =  I:'???'
Text Param 'StreamOrder' =  I:'???'
Text Param 'FirstPacketOrder' =  I:'???'
Text Param 'Inform' =  I:'Inform'
Text Param 'ID' =  I:'???'
Text Param 'ID/String' =  I:'???'
Text Param 'OriginalSourceMedium_ID' =  I:'???'
Text Param 'OriginalSourceMedium_ID/S' =  I:'???'
Text Param 'UniqueID' =  I:'???'
Text Param 'UniqueID/String' =  I:'???'
Text Param 'MenuID' =  I:'???'
Text Param 'MenuID/String' =  I:'???'
Text Param 'Format' =  I:'???'
Text Param 'Format/String' =  I:'???'
Text Param 'Format/Info' =  I:'???'
Text Param 'Format/Url' =  I:'???'
Text Param 'Format_Commercial' =  I:'???'
Text Param 'Format_Commercial_IfAny' =  I:'???'
Text Param 'Format_Version' =  I:'???'
Text Param 'Format_Profile' =  I:'???'
Text Param 'Format_Compression' =  I:'???'
Text Param 'Format_Settings' =  I:'???'
Text Param 'Format_Settings_Wrapping' =  I:'???'
Text Param 'Format_AdditionalFeatures' =  I:'???'
Text Param 'InternetMediaType' =  I:'???'
Text Param 'MuxingMode' =  I:'???'
Text Param 'MuxingMode_MoreInfo' =  I:'???'
Text Param 'CodecID' =  I:'???'
Text Param 'CodecID/String' =  I:'???'
Text Param 'CodecID/Info' =  I:'???'
Text Param 'CodecID/Hint' =  I:'???'
Text Param 'CodecID/Url' =  I:'???'
Text Param 'CodecID_Description' =  I:'???'
Text Param 'Codec' =  I:'???'
Text Param 'Codec/String' =  I:'???'
Text Param 'Codec/Info' =  I:'???'
Text Param 'Codec/Url' =  I:'???'
Text Param 'Codec/CC' =  I:'???'
Text Param 'Duration' =  I:'???'
Text Param 'Duration/String' =  I:'???'
Text Param 'Duration/String1' =  I:'???'
Text Param 'Duration/String2' =  I:'???'
Text Param 'Duration/String3' =  I:'???'
Text Param 'Duration/String4' =  I:'???'
Text Param 'Duration/String5' =  I:'???'
Text Param 'Duration_FirstFrame' =  I:'???'
Text Param 'Duration_FirstFrame/Strin' =  I:'???'
Text Param 'Duration_FirstFrame/Strin' =  I:'???'
Text Param 'Duration_FirstFrame/Strin' =  I:'???'
Text Param 'Duration_FirstFrame/Strin' =  I:'???'
Text Param 'Duration_FirstFrame/Strin' =  I:'???'
Text Param 'Duration_FirstFrame/Strin' =  I:'???'
Text Param 'Duration_LastFrame' =  I:'???'
Text Param 'Duration_LastFrame/String' =  I:'???'
Text Param 'Duration_LastFrame/String' =  I:'???'
Text Param 'Duration_LastFrame/String' =  I:'???'
Text Param 'Duration_LastFrame/String' =  I:'???'
Text Param 'Duration_LastFrame/String' =  I:'???'
Text Param 'Duration_LastFrame/String' =  I:'???'
Text Param 'Source_Duration' =  I:'???'
Text Param 'Source_Duration/String' =  I:'???'
Text Param 'Source_Duration/String1' =  I:'???'
Text Param 'Source_Duration/String2' =  I:'???'
Text Param 'Source_Duration/String3' =  I:'???'
Text Param 'Source_Duration/String4' =  I:'???'
Text Param 'Source_Duration/String5' =  I:'???'
Text Param 'Source_Duration_FirstFram' =  I:'???'
Text Param 'Source_Duration_FirstFram' =  I:'???'
Text Param 'Source_Duration_FirstFram' =  I:'???'
Text Param 'Source_Duration_FirstFram' =  I:'???'
Text Param 'Source_Duration_FirstFram' =  I:'???'
Text Param 'Source_Duration_FirstFram' =  I:'???'
Text Param 'Source_Duration_FirstFram' =  I:'???'
Text Param 'Source_Duration_LastFrame' =  I:'???'
Text Param 'Source_Duration_LastFrame' =  I:'???'
Text Param 'Source_Duration_LastFrame' =  I:'???'
Text Param 'Source_Duration_LastFrame' =  I:'???'
Text Param 'Source_Duration_LastFrame' =  I:'???'
Text Param 'Source_Duration_LastFrame' =  I:'???'
Text Param 'Source_Duration_LastFrame' =  I:'???'
Text Param 'BitRate_Mode' =  I:'???'
Text Param 'BitRate_Mode/String' =  I:'???'
Text Param 'BitRate' =  I:'???'
Text Param 'BitRate/String' =  I:'???'
Text Param 'BitRate_Minimum' =  I:'???'
Text Param 'BitRate_Minimum/String' =  I:'???'
Text Param 'BitRate_Nominal' =  I:'???'
Text Param 'BitRate_Nominal/String' =  I:'???'
Text Param 'BitRate_Maximum' =  I:'???'
Text Param 'BitRate_Maximum/String' =  I:'???'
Text Param 'BitRate_Encoded' =  I:'???'
Text Param 'BitRate_Encoded/String' =  I:'???'
Text Param 'Width' =  I:'???'
Text Param (Width/String)
Text Param 'Height' =  I:'???'
Text Param (Height/String)
Text Param 'FrameRate_Mode' =  I:'???'
Text Param 'FrameRate_Mode/String' =  I:'???'
Text Param 'FrameRate' =  I:'???'
Text Param 'FrameRate/String' =  I:'???'
Text Param 'FrameRate_Num' =  I:'???'
Text Param 'FrameRate_Den' =  I:'???'
Text Param 'FrameRate_Minimum' =  I:'???'
Text Param 'FrameRate_Minimum/String' =  I:'???'
Text Param 'FrameRate_Nominal' =  I:'???'
Text Param 'FrameRate_Nominal/String' =  I:'???'
Text Param 'FrameRate_Maximum' =  I:'???'
Text Param 'FrameRate_Maximum/String' =  I:'???'
Text Param 'FrameRate_Original' =  I:'???'
Text Param 'FrameRate_Original/String' =  I:'???'
Text Param 'FrameCount' =  I:'???'
Text Param 'ElementCount' =  I:'???'
Text Param 'Source_FrameCount' =  I:'???'
Text Param (ColorSpace)
Text Param (ChromaSubsampling)
Text Param 'Resolution' =  I:'???'
Text Param 'Resolution/String' =  I:'???'
Text Param (BitDepth)
Text Param (BitDepth/String)
Text Param 'Compression_Mode' =  I:'???'
Text Param 'Compression_Mode/String' =  I:'???'
Text Param 'Compression_Ratio' =  I:'???'
Text Param 'Delay' =  I:'???'
Text Param 'Delay/String' =  I:'???'
Text Param 'Delay/String1' =  I:'???'
Text Param 'Delay/String2' =  I:'???'
Text Param 'Delay/String3' =  I:'???'
Text Param 'Delay/String4' =  I:'???'
Text Param 'Delay/String5' =  I:'???'
Text Param 'Delay_Settings' =  I:'???'
Text Param 'Delay_DropFrame' =  I:'???'
Text Param 'Delay_Source' =  I:'???'
Text Param 'Delay_Source/String' =  I:'???'
Text Param 'Delay_Original' =  I:'???'
Text Param 'Delay_Original/String' =  I:'???'
Text Param 'Delay_Original/String1' =  I:'???'
Text Param 'Delay_Original/String2' =  I:'???'
Text Param 'Delay_Original/String3' =  I:'???'
Text Param 'Delay_Original/String4' =  I:'???'
Text Param 'Delay_Original/String5' =  I:'???'
Text Param 'Delay_Original_Settings' =  I:'???'
Text Param 'Delay_Original_DropFrame' =  I:'???'
Text Param 'Delay_Original_Source' =  I:'???'
Text Param 'Video_Delay' =  I:'???'
Text Param (Video_Delay/String)
Text Param (Video_Delay/String1)
Text Param (Video_Delay/String2)
Text Param (Video_Delay/String3)
Text Param (Video_Delay/String4)
Text Param (Video_Delay/String5)
Text Param 'Video0_Delay' =  I:'???'
Text Param 'Video0_Delay/String' =  I:'???'
Text Param 'Video0_Delay/String1' =  I:'???'
Text Param 'Video0_Delay/String2' =  I:'???'
Text Param 'Video0_Delay/String3' =  I:'???'
Text Param 'Video0_Delay/String4' =  I:'???'
Text Param 'Video0_Delay/String5' =  I:'???'
Text Param 'StreamSize' =  I:'???'
Text Param 'StreamSize/String' =  I:'???'
Text Param (StreamSize/String1)
Text Param (StreamSize/String2)
Text Param (StreamSize/String3)
Text Param (StreamSize/String4)
Text Param 'StreamSize/String5' =  I:'???'
Text Param 'StreamSize_Proportion' =  I:'???'
Text Param 'StreamSize_Demuxed' =  I:'???'
Text Param 'StreamSize_Demuxed/String' =  I:'???'
Text Param (StreamSize_Demuxed/String1)
Text Param (StreamSize_Demuxed/String2)
Text Param (StreamSize_Demuxed/String3)
Text Param (StreamSize_Demuxed/String4)
Text Param 'StreamSize_Demuxed/String' =  I:'???'
Text Param 'Source_StreamSize' =  I:'???'
Text Param 'Source_StreamSize/String' =  I:'???'
Text Param (Source_StreamSize/String1)
Text Param (Source_StreamSize/String2)
Text Param (Source_StreamSize/String3)
Text Param (Source_StreamSize/String4)
Text Param 'Source_StreamSize/String5' =  I:'???'
Text Param 'Source_StreamSize_Proport' =  I:'???'
Text Param 'StreamSize_Encoded' =  I:'???'
Text Param 'StreamSize_Encoded/String' =  I:'???'
Text Param (StreamSize_Encoded/String1)
Text Param (StreamSize_Encoded/String2)
Text Param (StreamSize_Encoded/String3)
Text Param (StreamSize_Encoded/String4)
Text Param 'StreamSize_Encoded/String' =  I:'???'
Text Param 'StreamSize_Encoded_Propor' =  I:'???'
Text Param 'Source_StreamSize_Encoded' =  I:'???'
Text Param 'Source_StreamSize_Encoded' =  I:'???'
Text Param (Source_StreamSize_Encoded/String1)
Text Param (Source_StreamSize_Encoded/String2)
Text Param (Source_StreamSize_Encoded/String3)
Text Param (Source_StreamSize_Encoded/String4)
Text Param 'Source_StreamSize_Encoded' =  I:'???'
Text Param 'Source_StreamSize_Encoded' =  I:'???'
Text Param 'Title' =  I:'???'
Text Param 'Encoded_Application' =  I:'???'
Text Param 'Encoded_Application/Strin' =  I:'???'
Text Param 'Encoded_Application_Compa' =  I:'???'
Text Param 'Encoded_Application_Name' =  I:'???'
Text Param 'Encoded_Application_Versi' =  I:'???'
Text Param 'Encoded_Application_Url' =  I:'???'
Text Param 'Encoded_Library' =  I:'???'
Text Param 'Encoded_Library/String' =  I:'???'
Text Param 'Encoded_Library_CompanyNa' =  I:'???'
Text Param 'Encoded_Library_Name' =  I:'???'
Text Param 'Encoded_Library_Version' =  I:'???'
Text Param 'Encoded_Library_Date' =  I:'???'
Text Param 'Encoded_Library_Settings' =  I:'???'
Text Param 'Encoded_OperatingSystem' =  I:'???'
Text Param 'Language' =  I:'???'
Text Param 'Language/String' =  I:'???'
Text Param 'Language/String1' =  I:'???'
Text Param 'Language/String2' =  I:'???'
Text Param 'Language/String3' =  I:'???'
Text Param 'Language/String4' =  I:'???'
Text Param 'Language_More' =  I:'???'
Text Param 'ServiceKind' =  I:'???'
Text Param 'ServiceKind/String' =  I:'???'
Text Param 'Disabled' =  I:'???'
Text Param 'Disabled/String' =  I:'???'
Text Param 'Default' =  I:'???'
Text Param 'Default/String' =  I:'???'
Text Param 'Forced' =  I:'???'
Text Param 'Forced/String' =  I:'???'
Text Param 'AlternateGroup' =  I:'???'
Text Param 'AlternateGroup/String' =  I:'???'
Text Param (Summary)
Text Param 'Encoded_Date' =  I:'???'
Text Param 'Tagged_Date' =  I:'???'
Text Param (Encryption)
----->Other
Other Param 'Count' =  I:'???'
Other Param 'Status' =  I:'???'
Other Param 'StreamCount' =  I:'???'
Other Param 'StreamKind' =  I:'???'
Other Param 'StreamKind/String' =  I:'???'
Other Param 'StreamKindID' =  I:'???'
Other Param 'StreamKindPos' =  I:'???'
Other Param 'StreamOrder' =  I:'???'
Other Param 'FirstPacketOrder' =  I:'???'
Other Param 'Inform' =  I:'Inform'
Other Param 'ID' =  I:'???'
Other Param 'ID/String' =  I:'???'
Other Param 'OriginalSourceMedium_ID' =  I:'???'
Other Param 'OriginalSourceMedium_ID/S' =  I:'???'
Other Param 'UniqueID' =  I:'???'
Other Param 'UniqueID/String' =  I:'???'
Other Param 'MenuID' =  I:'???'
Other Param 'MenuID/String' =  I:'???'
Other Param 'Type' =  I:'???'
Other Param 'Format' =  I:'???'
Other Param 'Format/String' =  I:'???'
Other Param 'Format/Info' =  I:'???'
Other Param 'Format/Url' =  I:'???'
Other Param 'Format_Commercial' =  I:'???'
Other Param 'Format_Commercial_IfAny' =  I:'???'
Other Param 'Format_Version' =  I:'???'
Other Param 'Format_Profile' =  I:'???'
Other Param 'Format_Compression' =  I:'???'
Other Param 'Format_Settings' =  I:'???'
Other Param 'Format_AdditionalFeatures' =  I:'???'
Other Param 'MuxingMode' =  I:'???'
Other Param 'CodecID' =  I:'???'
Other Param 'CodecID/String' =  I:'???'
Other Param 'CodecID/Info' =  I:'???'
Other Param 'CodecID/Hint' =  I:'???'
Other Param 'CodecID/Url' =  I:'???'
Other Param 'CodecID_Description' =  I:'???'
Other Param 'Duration' =  I:'???'
Other Param 'Duration/String' =  I:'???'
Other Param 'Duration/String1' =  I:'???'
Other Param 'Duration/String2' =  I:'???'
Other Param 'Duration/String3' =  I:'???'
Other Param 'Duration/String4' =  I:'???'
Other Param 'Duration/String5' =  I:'???'
Other Param (Duration_Start)
Other Param (Duration_End)
Other Param 'Source_Duration' =  I:'???'
Other Param 'Source_Duration/String' =  I:'???'
Other Param 'Source_Duration/String1' =  I:'???'
Other Param 'Source_Duration/String2' =  I:'???'
Other Param 'Source_Duration/String3' =  I:'???'
Other Param 'Source_Duration/String4' =  I:'???'
Other Param 'Source_Duration/String5' =  I:'???'
Other Param 'Source_Duration_FirstFram' =  I:'???'
Other Param 'Source_Duration_FirstFram' =  I:'???'
Other Param 'Source_Duration_FirstFram' =  I:'???'
Other Param 'Source_Duration_FirstFram' =  I:'???'
Other Param 'Source_Duration_FirstFram' =  I:'???'
Other Param 'Source_Duration_FirstFram' =  I:'???'
Other Param 'Source_Duration_FirstFram' =  I:'???'
Other Param 'Source_Duration_LastFrame' =  I:'???'
Other Param 'Source_Duration_LastFrame' =  I:'???'
Other Param 'Source_Duration_LastFrame' =  I:'???'
Other Param 'Source_Duration_LastFrame' =  I:'???'
Other Param 'Source_Duration_LastFrame' =  I:'???'
Other Param 'Source_Duration_LastFrame' =  I:'???'
Other Param 'Source_Duration_LastFrame' =  I:'???'
Other Param 'BitRate_Mode' =  I:'???'
Other Param 'BitRate_Mode/String' =  I:'???'
Other Param 'BitRate' =  I:'???'
Other Param 'BitRate/String' =  I:'???'
Other Param 'BitRate_Minimum' =  I:'???'
Other Param 'BitRate_Minimum/String' =  I:'???'
Other Param 'BitRate_Nominal' =  I:'???'
Other Param 'BitRate_Nominal/String' =  I:'???'
Other Param 'BitRate_Maximum' =  I:'???'
Other Param 'BitRate_Maximum/String' =  I:'???'
Other Param 'BitRate_Encoded' =  I:'???'
Other Param 'BitRate_Encoded/String' =  I:'???'
Other Param 'FrameRate' =  I:'???'
Other Param 'FrameRate/String' =  I:'???'
Other Param 'FrameRate_Num' =  I:'???'
Other Param 'FrameRate_Den' =  I:'???'
Other Param 'FrameCount' =  I:'???'
Other Param 'Source_FrameCount' =  I:'???'
Other Param 'Delay' =  I:'???'
Other Param 'Delay/String' =  I:'???'
Other Param 'Delay/String1' =  I:'???'
Other Param 'Delay/String2' =  I:'???'
Other Param 'Delay/String3' =  I:'???'
Other Param 'Delay/String4' =  I:'???'
Other Param 'Delay/String5' =  I:'???'
Other Param 'Delay_Settings' =  I:'???'
Other Param 'Delay_DropFrame' =  I:'???'
Other Param 'Delay_Source' =  I:'???'
Other Param 'Delay_Source/String' =  I:'???'
Other Param 'Delay_Original' =  I:'???'
Other Param 'Delay_Original/String' =  I:'???'
Other Param 'Delay_Original/String1' =  I:'???'
Other Param 'Delay_Original/String2' =  I:'???'
Other Param 'Delay_Original/String3' =  I:'???'
Other Param 'Delay_Original/String4' =  I:'???'
Other Param 'Delay_Original/String5' =  I:'???'
Other Param 'Delay_Original_Settings' =  I:'???'
Other Param 'Delay_Original_DropFrame' =  I:'???'
Other Param 'Delay_Original_Source' =  I:'???'
Other Param 'Video_Delay' =  I:'???'
Other Param (Video_Delay/String)
Other Param (Video_Delay/String1)
Other Param (Video_Delay/String2)
Other Param (Video_Delay/String3)
Other Param (Video_Delay/String4)
Other Param (Video_Delay/String5)
Other Param 'Video0_Delay' =  I:'???'
Other Param 'Video0_Delay/String' =  I:'???'
Other Param 'Video0_Delay/String1' =  I:'???'
Other Param 'Video0_Delay/String2' =  I:'???'
Other Param 'Video0_Delay/String3' =  I:'???'
Other Param 'Video0_Delay/String4' =  I:'???'
Other Param 'Video0_Delay/String5' =  I:'???'
Other Param 'TimeStamp_FirstFrame' =  I:'???'
Other Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Other Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Other Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Other Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Other Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Other Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Other Param 'TimeCode_FirstFrame' =  I:'???'
Other Param 'TimeCode_Settings' =  I:'???'
Other Param 'TimeCode_Striped' =  I:'???'
Other Param 'TimeCode_Striped/String' =  I:'???'
Other Param 'StreamSize' =  I:'???'
Other Param 'StreamSize/String' =  I:'???'
Other Param (StreamSize/String1)
Other Param (StreamSize/String2)
Other Param (StreamSize/String3)
Other Param (StreamSize/String4)
Other Param 'StreamSize/String5' =  I:'???'
Other Param 'StreamSize_Proportion' =  I:'???'
Other Param 'StreamSize_Demuxed' =  I:'???'
Other Param 'StreamSize_Demuxed/String' =  I:'???'
Other Param (StreamSize_Demuxed/String1)
Other Param (StreamSize_Demuxed/String2)
Other Param (StreamSize_Demuxed/String3)
Other Param (StreamSize_Demuxed/String4)
Other Param 'StreamSize_Demuxed/String' =  I:'???'
Other Param 'Source_StreamSize' =  I:'???'
Other Param 'Source_StreamSize/String' =  I:'???'
Other Param (Source_StreamSize/String1)
Other Param (Source_StreamSize/String2)
Other Param (Source_StreamSize/String3)
Other Param (Source_StreamSize/String4)
Other Param 'Source_StreamSize/String5' =  I:'???'
Other Param 'Source_StreamSize_Proport' =  I:'???'
Other Param 'StreamSize_Encoded' =  I:'???'
Other Param 'StreamSize_Encoded/String' =  I:'???'
Other Param (StreamSize_Encoded/String1)
Other Param (StreamSize_Encoded/String2)
Other Param (StreamSize_Encoded/String3)
Other Param (StreamSize_Encoded/String4)
Other Param 'StreamSize_Encoded/String' =  I:'???'
Other Param 'StreamSize_Encoded_Propor' =  I:'???'
Other Param 'Source_StreamSize_Encoded' =  I:'???'
Other Param 'Source_StreamSize_Encoded' =  I:'???'
Other Param (Source_StreamSize_Encoded/String1)
Other Param (Source_StreamSize_Encoded/String2)
Other Param (Source_StreamSize_Encoded/String3)
Other Param (Source_StreamSize_Encoded/String4)
Other Param 'Source_StreamSize_Encoded' =  I:'???'
Other Param 'Source_StreamSize_Encoded' =  I:'???'
Other Param 'Title' =  I:'???'
Other Param 'Language' =  I:'???'
Other Param 'Language/String' =  I:'???'
Other Param 'Language/String1' =  I:'???'
Other Param 'Language/String2' =  I:'???'
Other Param 'Language/String3' =  I:'???'
Other Param 'Language/String4' =  I:'???'
Other Param 'Language_More' =  I:'???'
Other Param 'ServiceKind' =  I:'???'
Other Param 'ServiceKind/String' =  I:'???'
Other Param 'Disabled' =  I:'???'
Other Param 'Disabled/String' =  I:'???'
Other Param 'Default' =  I:'???'
Other Param 'Default/String' =  I:'???'
Other Param 'Forced' =  I:'???'
Other Param 'Forced/String' =  I:'???'
Other Param 'AlternateGroup' =  I:'???'
Other Param 'AlternateGroup/String' =  I:'???'
----->Image
Image Param 'Count' =  I:'Count of objects available in this stream' T:'124' O:'N NI'
Image Param 'Status' =  I:'bit field (0=IsAccepted, 1=IsFilled, 2=IsUpdated, 3=IsFinished)' O:'N NI'
Image Param 'StreamCount' =  I:'Count of streams of that kind available' T:'1' O:'N NI'
Image Param 'StreamKind' =  I:'Stream type name' T:'Image' O:'N NT'
Image Param 'StreamKind/String' =  I:'Stream type name' T:'Image' O:'N NT'
Image Param 'StreamKindID' =  I:'Number of the stream (base=0)' T:'0' O:'N NI'
Image Param 'StreamKindPos' =  I:'When multiple streams, number of the stream (base=1)' O:'N NI'
Image Param 'StreamOrder' =  I:'Stream order in the file, whatever is the kind of stream (base=0)' O:'N YIY'
Image Param 'FirstPacketOrder' =  I:'Order of the first fully decodable packet met in the file, whatever is the kind of stream (base=0)' O:'N YIY'
Image Param 'Inform' =  I:'Inform'
Image Param 'ID' =  I:'The ID for this stream in this file' O:'N YTY'
Image Param 'ID/String' =  I:'The ID for this stream in this file' O:'Y NT'
Image Param 'OriginalSourceMedium_ID' =  I:'The ID for this stream in the original medium of the material' O:'N YTY'
Image Param 'OriginalSourceMedium_ID/S' =  I:'???'
Image Param 'UniqueID' =  I:'The unique ID for this stream, should be copied with stream copy' O:'N YTY'
Image Param 'UniqueID/String' =  I:'The unique ID for this stream, should be copied with stream copy' O:'Y NT'
Image Param 'MenuID' =  I:'The menu ID for this stream in this file' O:'N YTY'
Image Param 'MenuID/String' =  I:'The menu ID for this stream in this file' O:'Y NT'
Image Param 'Title' =  I:'Name of the track' O:'Y YTY'
Image Param 'Format' =  I:'Format used' T:'JPEG' O:'N YTY'
Image Param 'Format/String' =  I:'Format used + additional features' T:'JPEG' O:'Y NT'
Image Param 'Format/Info' =  I:'Info about Format' O:'Y NT'
Image Param 'Format/Url' =  I:'Link' O:'N NT'
Image Param 'Format_Commercial' =  I:'Commercial name used by vendor for theses setings or Format field if there is no difference' T:'JPEG' O:'N NT'
Image Param 'Format_Commercial_IfAny' =  I:'Commercial name used by vendor for theses setings if there is one' O:'Y YTY'
Image Param 'Format_Version' =  I:'Version of this format' O:'Y NTY'
Image Param 'Format_Profile' =  I:'Profile of the Format' O:'Y NTY'
Image Param (Format_Settings_Endianness)
Image Param (Format_Settings_Packing)
Image Param 'Format_Compression' =  I:'Compression method used' O:'Y YTY'
Image Param 'Format_Settings' =  I:'Settings needed for decoder used' O:'Y NT'
Image Param 'Format_Settings_Wrapping' =  I:'Wrapping mode (Frame wrapped or Clip wrapped)' O:'Y YTY'
Image Param 'Format_AdditionalFeatures' =  I:'Format features needed for fully supporting the content' O:'N YTY'
Image Param 'InternetMediaType' =  I:'Internet Media Type (aka MIME Type, Content-Type)' T:'image/jpeg' O:'N YT'
Image Param 'CodecID' =  I:'Codec ID (found in some containers)' O:'Y YTY'
Image Param 'CodecID/String' =  I:'Codec ID (found in some containers)' O:'Y NT'
Image Param 'CodecID/Info' =  I:'Info about codec ID' O:'Y NT'
Image Param 'CodecID/Hint' =  I:'A hint for this codec ID' O:'Y NT'
Image Param 'CodecID/Url' =  I:'A link for more details about this codec ID' O:'N NT'
Image Param 'CodecID_Description' =  I:'Manual description given by the container' O:'Y YT'
Image Param 'Codec' =  I:'Deprecated, do not use in new projects' O:'N NT'
Image Param 'Codec/String' =  I:'Deprecated, do not use in new projects' O:'N NT'
Image Param 'Codec/Family' =  I:'Deprecated, do not use in new projects' O:'N NT'
Image Param 'Codec/Info' =  I:'Deprecated, do not use in new projects' O:'N NT'
Image Param 'Codec/Url' =  I:'Deprecated, do not use in new projects' O:'N NT'
Image Param 'Width' =  I:'Width (aperture size if present) in pixel' T:'259' M:' pixel' O:'N YIY' m:' pixel'
Image Param 'Width/String' =  I:'Width (aperture size if present) with measurement (pixel)' T:'259 pixels' O:'Y NT'
Image Param 'Width_Offset' =  I:'Offset between original width and displayed width (aperture size) in pixel' M:' pixel' O:'N YIY' m:' pixel'
Image Param 'Width_Offset/String' =  I:'Offset between original width and displayed width (aperture size)  in pixel' O:'N NT'
Image Param 'Width_Original' =  I:'Original (in the raw stream) width in pixel' M:' pixel' O:'N YIY' m:' pixel'
Image Param 'Width_Original/String' =  I:'Original (in the raw stream) width with measurement (pixel)' O:'Y NT'
Image Param 'Height' =  I:'Height (aperture size if present) in pixel' T:'194' M:' pixel' O:'N YIY' m:' pixel'
Image Param 'Height/String' =  I:'Height (aperture size if present) with measurement (pixel)' T:'194 pixels' O:'Y NT'
Image Param 'Height_Offset' =  I:'Offset between original height and displayed height (aperture size) in pixel' M:' pixel' O:'N YIY' m:' pixel'
Image Param 'Height_Offset/String' =  I:'Offset between original height and displayed height (aperture size)  in pixel' O:'N NT'
Image Param 'Height_Original' =  I:'Original (in the raw stream) height in pixel' M:' pixel' O:'N YIY' m:' pixel'
Image Param 'Height_Original/String' =  I:'Original (in the raw stream) height with measurement (pixel)' O:'Y NT'
Image Param 'PixelAspectRatio' =  I:'Pixel Aspect ratio' O:'N YFY'
Image Param 'PixelAspectRatio/String' =  I:'Pixel Aspect ratio' O:'N NT'
Image Param 'PixelAspectRatio_Original' =  I:'Original (in the raw stream) Pixel Aspect ratio' O:'N YFY'
Image Param 'PixelAspectRatio_Original' =  I:'Original (in the raw stream) Pixel Aspect ratio' O:'N YFY'
Image Param 'DisplayAspectRatio' =  I:'Display Aspect ratio' O:'N YFY'
Image Param 'DisplayAspectRatio/String' =  I:'Display Aspect ratio' O:'Y NT'
Image Param 'DisplayAspectRatio_Origin' =  I:'???'
Image Param 'DisplayAspectRatio_Origin' =  I:'???'
Image Param (ColorSpace)
Image Param (ChromaSubsampling)
Image Param 'Resolution' =  I:'Deprecated, do not use in new projects' M:' bit' O:'N NI' m:' bit'
Image Param 'Resolution/String' =  I:'Deprecated, do not use in new projects' O:'N NT'
Image Param (BitDepth)
Image Param (BitDepth/String)
Image Param 'Compression_Mode' =  I:'Compression mode (Lossy or Lossless)' T:'Lossy' O:'N YTY'
Image Param 'Compression_Mode/String' =  I:'Compression mode (Lossy or Lossless)' T:'Lossy' O:'Y NT'
Image Param 'Compression_Ratio' =  I:'Current stream size divided by uncompressed stream size' O:'Y YF'
Image Param 'StreamSize' =  I:'Stream size in bytes' T:'9830' M:' byte' O:'N YIY' m:' byte'
Image Param (StreamSize/String)
Image Param (StreamSize/String1)
Image Param (StreamSize/String2)
Image Param (StreamSize/String3)
Image Param (StreamSize/String4)
Image Param 'StreamSize/String5' =  I:'With proportion' T:'9.60 KiB (100%)' O:'N NT'
Image Param 'StreamSize_Proportion' =  I:'Stream size divided by file size' T:'1.00000' O:'N NT'
Image Param 'StreamSize_Demuxed' =  I:'StreamSize in bytes of hte stream after demux' M:' byte' O:'N YIN' m:' byte'
Image Param 'StreamSize_Demuxed/String' =  I:'StreamSize_Demuxed in with percentage value' O:'N NT'
Image Param (StreamSize_Demuxed/String1)
Image Param (StreamSize_Demuxed/String2)
Image Param (StreamSize_Demuxed/String3)
Image Param (StreamSize_Demuxed/String4)
Image Param 'StreamSize_Demuxed/String' =  I:'StreamSize_Demuxed in with percentage value' O:'N NT'
Image Param 'Encoded_Library' =  I:'Software used to create the file' O:'N YTY'
Image Param 'Encoded_Library/String' =  I:'Software used to create the file' O:'Y NT'
Image Param 'Encoded_Library_Name' =  I:'Info from the software' O:'N NTY'
Image Param 'Encoded_Library_Version' =  I:'Version of software' O:'N NTY'
Image Param 'Encoded_Library_Date' =  I:'Release date of software' O:'N NTY'
Image Param 'Encoded_Library_Settings' =  I:'Parameters used by the software' O:'Y YTY'
Image Param 'Language' =  I:'Language (2-letter ISO 639-1 if exists, else 3-letter ISO 639-2, and with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn)' O:'N YTY'
Image Param 'Language/String' =  I:'Language (full)' O:'Y NT'
Image Param 'Language/String1' =  I:'Language (full)' O:'N NT'
Image Param 'Language/String2' =  I:'Language (2-letter ISO 639-1 if exists, else empty)' O:'N NT'
Image Param 'Language/String3' =  I:'Language (3-letter ISO 639-2 if exists, else empty)' O:'N NT'
Image Param 'Language/String4' =  I:'Language (2-letter ISO 639-1 if exists with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn, else empty)' O:'N NT'
Image Param 'Language_More' =  I:'More info about Language (e.g. Director's Comment)' O:'Y YTY'
Image Param 'ServiceKind' =  I:'Service kind, e.g. visually impaired, commentary, voice over' O:'N YTY'
Image Param 'ServiceKind/String' =  I:'Service kind (full)' O:'Y NT'
Image Param 'Disabled' =  I:'Set if that track should not be used' M:'Yes' O:'N YTY' m:'Yes'
Image Param 'Disabled/String' =  I:'Set if that track should not be used' O:'Y NT'
Image Param 'Default' =  I:'Set if that track should be used if no language found matches the user preference.' M:'Yes' O:'N YTY' m:'Yes'
Image Param 'Default/String' =  I:'Set if that track should be used if no language found matches the user preference.' O:'Y NT'
Image Param 'Forced' =  I:'Set if that track should be used if no language found matches the user preference.' M:'Yes' O:'N YTY' m:'Yes'
Image Param 'Forced/String' =  I:'Set if that track should be used if no language found matches the user preference.' O:'Y NT'
Image Param 'AlternateGroup' =  I:'Number of a group in order to provide versions of the same track' M:'Yes' O:'N YTY' m:'Yes'
Image Param 'AlternateGroup/String' =  I:'Number of a group in order to provide versions of the same track' O:'Y NT'
Image Param (Summary)
Image Param 'Encoded_Date' =  I:'The time that the encoding of this item was completed began.' O:'Y YTY' D:'Temporal'
Image Param 'Tagged_Date' =  I:'The time that the tags were done for this item.' O:'Y YTY' D:'Temporal'
Image Param (Encryption)
Image Param 'colour_description_presen' =  I:'???'
Image Param 'colour_primaries' =  I:'Chromaticity coordinates of the source primaries' O:'Y YTY'
Image Param 'transfer_characteristics' =  I:'Opto-electronic transfer characteristic of the source picture' O:'Y YTY'
Image Param 'matrix_coefficients' =  I:'Matrix coefficients used in deriving luma and chroma signals from the green, blue, and red primaries' O:'Y YTY'
Image Param 'colour_description_presen' =  I:'???'
Image Param 'colour_primaries_Original' =  I:'Chromaticity coordinates of the source primaries' O:'Y YTY'
Image Param 'transfer_characteristics_' =  I:'???'
Image Param 'matrix_coefficients_Origi' =  I:'???'
----->Menu
Menu Param 'Count' =  I:'???'
Menu Param 'Status' =  I:'???'
Menu Param 'StreamCount' =  I:'???'
Menu Param 'StreamKind' =  I:'???'
Menu Param 'StreamKind/String' =  I:'???'
Menu Param 'StreamKindID' =  I:'???'
Menu Param 'StreamKindPos' =  I:'???'
Menu Param 'StreamOrder' =  I:'???'
Menu Param 'FirstPacketOrder' =  I:'???'
Menu Param 'Inform' =  I:'Inform'
Menu Param 'ID' =  I:'???'
Menu Param 'ID/String' =  I:'???'
Menu Param 'OriginalSourceMedium_ID' =  I:'???'
Menu Param 'OriginalSourceMedium_ID/S' =  I:'???'
Menu Param 'UniqueID' =  I:'???'
Menu Param 'UniqueID/String' =  I:'???'
Menu Param 'MenuID' =  I:'???'
Menu Param 'MenuID/String' =  I:'???'
Menu Param 'Format' =  I:'???'
Menu Param 'Format/String' =  I:'???'
Menu Param 'Format/Info' =  I:'???'
Menu Param 'Format/Url' =  I:'???'
Menu Param 'Format_Commercial' =  I:'???'
Menu Param 'Format_Commercial_IfAny' =  I:'???'
Menu Param 'Format_Version' =  I:'???'
Menu Param 'Format_Profile' =  I:'???'
Menu Param 'Format_Compression' =  I:'???'
Menu Param 'Format_Settings' =  I:'???'
Menu Param 'Format_AdditionalFeatures' =  I:'???'
Menu Param 'CodecID' =  I:'???'
Menu Param 'CodecID/String' =  I:'???'
Menu Param 'CodecID/Info' =  I:'???'
Menu Param 'CodecID/Hint' =  I:'???'
Menu Param 'CodecID/Url' =  I:'???'
Menu Param 'CodecID_Description' =  I:'???'
Menu Param 'Codec' =  I:'???'
Menu Param 'Codec/String' =  I:'???'
Menu Param 'Codec/Info' =  I:'???'
Menu Param 'Codec/Url' =  I:'???'
Menu Param 'Duration' =  I:'???'
Menu Param 'Duration/String' =  I:'???'
Menu Param 'Duration/String1' =  I:'???'
Menu Param 'Duration/String2' =  I:'???'
Menu Param 'Duration/String3' =  I:'???'
Menu Param 'Duration/String4' =  I:'???'
Menu Param 'Duration/String5' =  I:'???'
Menu Param (Duration_Start)
Menu Param (Duration_End)
Menu Param 'Delay' =  I:'???'
Menu Param 'Delay/String' =  I:'???'
Menu Param 'Delay/String1' =  I:'???'
Menu Param 'Delay/String2' =  I:'???'
Menu Param 'Delay/String3' =  I:'???'
Menu Param 'Delay/String4' =  I:'???'
Menu Param 'Delay/String5' =  I:'???'
Menu Param 'Delay_Settings' =  I:'???'
Menu Param 'Delay_DropFrame' =  I:'???'
Menu Param 'Delay_Source' =  I:'???'
Menu Param 'List_StreamKind' =  I:'???'
Menu Param 'List_StreamPos' =  I:'???'
Menu Param 'List' =  I:'???'
Menu Param 'List/String' =  I:'???'
Menu Param 'Title' =  I:'???'
Menu Param 'Language' =  I:'???'
Menu Param 'Language/String' =  I:'???'
Menu Param 'Language/String1' =  I:'???'
Menu Param 'Language/String2' =  I:'???'
Menu Param 'Language/String3' =  I:'???'
Menu Param 'Language/String4' =  I:'???'
Menu Param 'Language_More' =  I:'???'
Menu Param 'ServiceKind' =  I:'???'
Menu Param 'ServiceKind/String' =  I:'???'
Menu Param (ServiceName)
Menu Param (ServiceChannel)
Menu Param (Service/Url)
Menu Param (ServiceProvider)
Menu Param (ServiceProvider/Url)
Menu Param (ServiceType)
Menu Param (NetworkName)
Menu Param (Original/NetworkName)
Menu Param (Countries)
Menu Param (TimeZones)
Menu Param 'LawRating' =  I:'???'
Menu Param 'LawRating_Reason' =  I:'???'
Menu Param 'Disabled' =  I:'???'
Menu Param 'Disabled/String' =  I:'???'
Menu Param 'Default' =  I:'???'
Menu Param 'Default/String' =  I:'???'
Menu Param 'Forced' =  I:'???'
Menu Param 'Forced/String' =  I:'???'
Menu Param 'AlternateGroup' =  I:'???'
Menu Param 'AlternateGroup/String' =  I:'???'
Menu Param 'Chapters_Pos_Begin' =  I:'???'
[--- sample-mp4-file.mp4 ---------------------------------------------------------------]
----->General
General Param 'Count' =  I:'Count of objects available in this stream' T:'331' O:'N NI'
General Param 'Status' =  I:'Status of bit field (0=IsAccepted, 1=IsFilled, 2=IsUpdated, 3=IsFinished)' O:'N NI'
General Param 'StreamCount' =  I:'Count of streams of this kind available (base=1)' T:'1' O:'N NI'
General Param 'StreamKind' =  I:'Stream type name' T:'General' O:'N NT'
General Param 'StreamKind/String' =  I:'Stream type name' T:'General' O:'N NT'
General Param 'StreamKindID' =  I:'Stream number (base=0)' T:'0' O:'N NI'
General Param 'StreamKindPos' =  I:'Number of the stream, when multiple (base=1)' O:'N NI'
General Param 'StreamOrder' =  I:'Stream order in the file for type of stream (base=0)' O:'N YIY'
General Param 'FirstPacketOrder' =  I:'Order of the first fully decodable packet met in the file for stream type (base=0)' O:'N YI'
General Param 'Inform' =  I:'Inform'
General Param 'ID' =  I:'The ID for this stream in this file' O:'N YTY'
General Param 'ID/String' =  I:'The ID for this stream in this file' O:'Y NT'
General Param 'OriginalSourceMedium_ID' =  I:'The ID for this stream in the original medium of the material' O:'N YTY'
General Param 'OriginalSourceMedium_ID/S' =  I:'???'
General Param 'UniqueID' =  I:'The unique ID for this stream, should be copied with stream copy' O:'N YTY'
General Param 'UniqueID/String' =  I:'The unique ID for this stream, should be copied with stream copy' O:'Y NT'
General Param 'MenuID' =  I:'The menu ID for this stream in this file' O:'N YTY'
General Param 'MenuID/String' =  I:'The menu ID for this stream in this file' O:'Y NT'
General Param 'GeneralCount' =  I:'Number of general streams' O:'N NIY'
General Param 'VideoCount' =  I:'Number of video streams' T:'1' O:'N NIY'
General Param 'AudioCount' =  I:'Number of audio streams' T:'1' O:'N NIY'
General Param 'TextCount' =  I:'Number of text streams' O:'N NIY'
General Param 'OtherCount' =  I:'Number of other streams' O:'N NIY'
General Param 'ImageCount' =  I:'Number of image streams' O:'N NIY'
General Param 'MenuCount' =  I:'Number of menu streams' O:'N NIY'
General Param 'Video_Format_List' =  I:'Video Codecs in this file, separated by /' T:'AVC' O:'N NT'
General Param 'Video_Format_WithHint_Lis' =  I:'???'
General Param 'Video_Codec_List' =  I:'Deprecated, do not use in new projects' T:'AVC' O:'N NT'
General Param 'Video_Language_List' =  I:'Video languages in this file, full names, separated by /' O:'N NT'
General Param 'Audio_Format_List' =  I:'Audio Codecs in this file,separated by /' T:'AAC LC' O:'N NT'
General Param 'Audio_Format_WithHint_Lis' =  I:'???'
General Param 'Audio_Codec_List' =  I:'Deprecated, do not use in new projects' T:'AAC LC' O:'N NT'
General Param 'Audio_Language_List' =  I:'Audio languages in this file separated by /' O:'N NT'
General Param 'Text_Format_List' =  I:'Text Codecs in this file, separated by /' O:'N NT'
General Param 'Text_Format_WithHint_List' =  I:'Text Codecs in this file with popular name (hint),separated by /' O:'N NT'
General Param 'Text_Codec_List' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Text_Language_List' =  I:'Text languages in this file, separated by /' O:'N NT'
General Param 'Other_Format_List' =  I:'Other formats in this file, separated by /' O:'N NT'
General Param 'Other_Format_WithHint_Lis' =  I:'???'
General Param 'Other_Codec_List' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Other_Language_List' =  I:'Chapters languages in this file, separated by /' O:'N NT'
General Param 'Image_Format_List' =  I:'Image Codecs in this file, separated by /' O:'N NT'
General Param 'Image_Format_WithHint_Lis' =  I:'???'
General Param 'Image_Codec_List' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Image_Language_List' =  I:'Image languages in this file, separated by /' O:'N NT'
General Param 'Menu_Format_List' =  I:'Menu Codecs in this file, separated by /' O:'N NT'
General Param 'Menu_Format_WithHint_List' =  I:'Menu Codecs in this file with popular name (hint),separated by /' O:'N NT'
General Param 'Menu_Codec_List' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Menu_Language_List' =  I:'Menu languages in this file, separated by /' O:'N NT'
General Param 'CompleteName' =  I:'Complete name (Folder+Name+Extension)' T:'C:\bld\bofstd\tests\data\sample-mp4-file.mp4' O:'Y YT'
General Param 'FolderName' =  I:'Folder name only' T:'C:\bld\bofstd\tests\data' O:'N NT'
General Param 'FileNameExtension' =  I:'File name and extension' T:'sample-mp4-file.mp4' O:'N NT'
General Param 'FileName' =  I:'File name only' T:'sample-mp4-file' O:'N NT'
General Param 'FileExtension' =  I:'File extension only' T:'mp4' O:'N NTY'
General Param 'CompleteName_Last' =  I:'Complete name (Folder+Name+Extension) of the last file (in the case of a sequence of files)' O:'Y YTY'
General Param 'FolderName_Last' =  I:'Folder name only of the last file (in the case of a sequence of files)' O:'N NT'
General Param 'FileNameExtension_Last' =  I:'File name and extension of the last file (in the case of a sequence of files)' O:'N NT'
General Param 'FileName_Last' =  I:'File name only of the last file (in the case of a sequence of files)' O:'N NT'
General Param 'FileExtension_Last' =  I:'File extension only of the last file (in the case of a sequence of files)' O:'N NT'
General Param 'Format' =  I:'Format used' T:'MPEG-4' O:'N YTY'
General Param 'Format/String' =  I:'Format used + additional features' T:'MPEG-4' O:'Y NT'
General Param 'Format/Info' =  I:'Info about this Format' O:'Y NT'
General Param 'Format/Url' =  I:'Link to a description of this format' O:'N NT'
General Param 'Format/Extensions' =  I:'Known extensions of this format' T:'braw mov mp4 m4v m4a m4b m4p m4r 3ga 3gpa 3gpp 3gp 3gpp2 3g2 k3g jpm jpx mqv ismv isma ismt f4a f4b f4v' O:'N NT'
General Param 'Format_Commercial' =  I:'Commercial name used by vendor for these settings or Format field if there is no difference' T:'MPEG-4' O:'N NT'
General Param 'Format_Commercial_IfAny' =  I:'Commercial name used by vendor for these settings if there is one' O:'Y YTY'
General Param 'Format_Version' =  I:'Version of this format' O:'Y YTY'
General Param 'Format_Profile' =  I:'Profile of the Format (old XML: 'Profile@Level' format' T:'Base Media' O:'Y YTY' H:' MIXML: 'Profile' only)'
General Param 'Format_Level' =  I:'Level of the Format (only MIXML)' O:'Y YTY'
General Param 'Format_Compression' =  I:'Compression method used' O:'Y YTY'
General Param 'Format_Settings' =  I:'Settings needed for decoder used' O:'Y YTY'
General Param 'Format_AdditionalFeatures' =  I:'Format features needed for fully supporting the content' O:'N YTY'
General Param 'InternetMediaType' =  I:'Internet Media Type (aka MIME Type, Content-Type)' T:'video/mp4' O:'N YT'
General Param 'CodecID' =  I:'Codec ID (found in some containers)' T:'isom' O:'N YTY'
General Param 'CodecID/String' =  I:'Codec ID (found in some containers)' T:'isom (isom/iso2/avc1/mp41)' O:'Y NT'
General Param 'CodecID/Info' =  I:'Info about this Codec' O:'Y NT'
General Param 'CodecID/Hint' =  I:'A hint/popular name for this Codec' O:'Y NT'
General Param 'CodecID/Url' =  I:'A link to more details about this Codec ID' T:'http://www.apple.com/quicktime/download/standalone.html' O:'N NT'
General Param 'CodecID_Description' =  I:'Manual description given by the container' O:'Y YTY'
General Param 'CodecID_Version' =  I:'Version of the CodecID' O:'N YTY'
General Param 'CodecID_Compatible' =  I:'Compatible CodecIDs' T:'isom/iso2/avc1/mp41' O:'N YTY'
General Param 'Interleaved' =  I:'If Audio and video are muxed' O:'N YTY'
General Param 'Codec' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Codec/String' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Codec/Info' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Codec/Url' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Codec/Extensions' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Codec_Settings' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'Codec_Settings_Automatic' =  I:'Deprecated, do not use in new projects' O:'N NT'
General Param 'FileSize' =  I:'File size in bytes' T:'10546620' M:' byte' O:'N YTY' m:' byte'
General Param 'FileSize/String' =  I:'File size (with measure)' T:'10.1 MiB' O:'Y NT'
General Param 'FileSize/String1' =  I:'File size (with measure, 1 digit mini)' T:'10 MiB' O:'N NT'
General Param 'FileSize/String2' =  I:'File size (with measure, 2 digit mini)' T:'10 MiB' O:'N NT'
General Param 'FileSize/String3' =  I:'File size (with measure, 3 digit mini)' T:'10.1 MiB' O:'N NT'
General Param 'FileSize/String4' =  I:'File size (with measure, 4 digit mini)' T:'10.06 MiB' O:'N NT'
General Param 'Duration' =  I:'Play time of the stream (in ms)' T:'125952' M:' ms' O:'N YFY' m:' ms'
General Param 'Duration/String' =  I:'Play time in format : XXx YYy only, YYy omitted if zero' T:'2 min 5 s' O:'Y NT'
General Param 'Duration/String1' =  I:'Play time in format : HHh MMmn SSs MMMms, XX omitted if zero' T:'2 min 5 s 952 ms' O:'N NT'
General Param 'Duration/String2' =  I:'Play time in format : XXx YYy only, YYy omitted if zero' T:'2 min 5 s' O:'N NT'
General Param 'Duration/String3' =  I:'Play time in format : HH:MM:SS.MMM' T:'00:02:05.952' O:'N NT'
General Param 'Duration/String4' =  I:'Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available' T:'00:02:05:14' O:'N NT'
General Param 'Duration/String5' =  I:'Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)' T:'00:02:05.952 (00:02:05:14)' O:'N NT'
General Param (Duration_Start)
General Param (Duration_End)
General Param 'OverallBitRate_Mode' =  I:'Bit rate mode of all streams (VBR, CBR)' T:'VBR' O:'N YTY'
General Param 'OverallBitRate_Mode/Strin' =  I:'???'
General Param 'OverallBitRate' =  I:'Bit rate of all streams (in bps)' T:'669882' M:' bps' O:'N YFY' m:' b/s'
General Param 'OverallBitRate/String' =  I:'Bit rate of all streams (with measure)' T:'670 kb/s' O:'Y NT'
General Param 'OverallBitRate_Minimum' =  I:'Minimum bit rate (in bps)' M:' bps' O:'N YFY' m:' b/s'
General Param 'OverallBitRate_Minimum/St' =  I:'???'
General Param 'OverallBitRate_Nominal' =  I:'Nominal bit rate (in bps)' M:' bps' O:'N YFY' m:' b/s'
General Param 'OverallBitRate_Nominal/St' =  I:'???'
General Param 'OverallBitRate_Maximum' =  I:'Maximum bit rate (in bps)' M:' bps' O:'N YFY' m:' b/s'
General Param 'OverallBitRate_Maximum/St' =  I:'???'
General Param 'FrameRate' =  I:'Frames per second' T:'15.000' M:' fps' O:'N YFY' m:' fps'
General Param 'FrameRate/String' =  I:'Frames per second (with measurement)' T:'15.000 FPS' O:'N NT'
General Param 'FrameRate_Num' =  I:'Frames per second, numerator' O:'N NFN'
General Param 'FrameRate_Den' =  I:'Frames per second, denominator' O:'N NFN'
General Param 'FrameCount' =  I:'Frame count (if a frame contains a count of samples)' T:'1889' O:'N NIY'
General Param 'Delay' =  I:'Delay fixed in the stream (relative) (in ms)' M:' ms' O:'N YI' m:' ms'
General Param 'Delay/String' =  I:'Delay with measurement' O:'N NT'
General Param 'Delay/String1' =  I:'Delay with measurement' O:'N NT'
General Param 'Delay/String2' =  I:'Delay with measurement' O:'N NT'
General Param 'Delay/String3' =  I:'format : HH:MM:SS.MMM' O:'N NT'
General Param 'Delay/String4' =  I:'Delay in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available' O:'N NT'
General Param 'Delay/String5' =  I:'Delay in format : HH:MM:SS.mmm (HH:MM:SS:FF)' O:'N NT'
General Param 'Delay_Settings' =  I:'Delay settings (in case of timecode, for example)' O:'N NTY'
General Param 'Delay_DropFrame' =  I:'Delay drop frame' O:'N NTY'
General Param 'Delay_Source' =  I:'Delay source (Container, Stream, empty)' O:'N NTY'
General Param 'Delay_Source/String' =  I:'Delay source (Container, Stream, empty)' O:'N NT'
General Param 'StreamSize' =  I:'Stream size (in bytes)' T:'53360' M:' byte' O:'N YIY' m:' byte'
General Param (StreamSize/String)
General Param (StreamSize/String1)
General Param (StreamSize/String2)
General Param (StreamSize/String3)
General Param (StreamSize/String4)
General Param 'StreamSize/String5' =  I:'Stream size with proportion' T:'52.1 KiB (0%)' O:'N NT'
General Param 'StreamSize_Proportion' =  I:'Stream size divided by file size' T:'0.00506' O:'N NT'
General Param 'StreamSize_Demuxed' =  I:'StreamSize after demux (in bytes)' M:' byte' O:'N YIN' m:' byte'
General Param 'StreamSize_Demuxed/String' =  I:'StreamSize_Demuxed with percentage value' O:'N NT'
General Param (StreamSize_Demuxed/String1)
General Param (StreamSize_Demuxed/String2)
General Param (StreamSize_Demuxed/String3)
General Param (StreamSize_Demuxed/String4)
General Param 'StreamSize_Demuxed/String' =  I:'StreamSize_Demuxed with percentage value' O:'N NT'
General Param (HeaderSize)
General Param (DataSize)
General Param (FooterSize)
General Param (IsStreamable)
General Param 'Album_ReplayGain_Gain' =  I:'The gain to apply to reach 89dB SPL on playback' M:' dB' O:'N YTY' m:' dB'
General Param (Album_ReplayGain_Gain/String)
General Param 'Album_ReplayGain_Peak' =  I:'The maximum absolute peak value of the item' O:'Y YTY'
General Param (Encryption)
General Param (Encryption_Format)
General Param (Encryption_Length)
General Param (Encryption_Method)
General Param (Encryption_Mode)
General Param (Encryption_Padding)
General Param (Encryption_InitializationVector)
General Param 'UniversalAdID/String' =  I:'Universal Ad-ID, see https://ad-id.org' O:'Y NT'
General Param 'UniversalAdID_Registry' =  I:'Universal Ad-ID registry' O:'N YTY'
General Param 'UniversalAdID_Value' =  I:'Universal Ad-ID value' O:'N YTY'
General Param 'Title' =  I:'(Generic)Title of file' O:'N NTY' D:'Title'
General Param 'Title_More' =  I:'(Generic)More info about the title of file' O:'N NTY' D:'Title'
General Param 'Title/Url' =  I:'(Generic)Url' O:'N NT' D:'Title'
General Param 'Domain' =  I:'Universe movies belong to, e.g. Star Wars, Stargate, Buffy, Dragonball' O:'Y YTY' D:'Title'
General Param 'Collection' =  I:'Name of the series, e.g. Star Wars movies, Stargate SG-1, Stargate Atlantis, Buffy, Angel' O:'Y YTY' D:'Title'
General Param 'Season' =  I:'Name of the season, e.g. Star Wars first Trilogy, Season 1' O:'Y YTY' D:'Title'
General Param 'Season_Position' =  I:'Number of the Season' O:'Y YIY' D:'Title'
General Param 'Season_Position_Total' =  I:'Place of the season, e.g. 2 of 7' O:'Y YIY' D:'Title'
General Param 'Movie' =  I:'Name of the movie. Eg : Star Wars, A New Hope' O:'Y YTY' D:'Title'
General Param 'Movie_More' =  I:'More info about the movie' O:'Y YTY' D:'Title'
General Param 'Movie/Country' =  I:'Country, where the movie was produced' O:'Y YTY' D:'Title'
General Param 'Movie/Url' =  I:'Homepage for the movie' O:'Y YT' D:'Title'
General Param 'Album' =  I:'Name of an audio-album. Eg : The Joshua Tree' O:'Y YTY' D:'Title'
General Param 'Album_More' =  I:'More info about the album' O:'Y YTY' D:'Title'
General Param (Album/Sort)
General Param 'Album/Performer' =  I:'Album performer/artist of this file' O:'Y YTY' D:'Entity'
General Param (Album/Performer/Sort)
General Param 'Album/Performer/Url' =  I:'Homepage of the album performer/artist' O:'Y YT' D:'Entity'
General Param 'Comic' =  I:'Name of the comic.' O:'Y YTY' D:'Title'
General Param 'Comic_More' =  I:'More info about the comic' O:'Y YTY' D:'Title'
General Param 'Comic/Position_Total' =  I:'Place of the comic, e.g. 1 of 10' O:'Y YIY' D:'Title'
General Param 'Part' =  I:'Name of the part. e.g. CD1, CD2' O:'Y YTY' D:'Title'
General Param 'Part/Position' =  I:'Number of the part' O:'Y YIY' D:'Title'
General Param 'Part/Position_Total' =  I:'Place of the part e.g. 2 of 3' O:'Y YIY' D:'Title'
General Param 'Track' =  I:'Name of the track. e.g. track 1, track 2' O:'Y YTY' D:'Title'
General Param 'Track_More' =  I:'More info about the track' O:'Y YTY' D:'Title'
General Param 'Track/Url' =  I:'Link to a site about this track' O:'Y YT' D:'Title'
General Param (Track/Sort)
General Param 'Track/Position' =  I:'Number of this track' O:'Y YIY' D:'Title'
General Param 'Track/Position_Total' =  I:'Place of this track, e.g. 3 of 15' O:'Y YIY' D:'Title'
General Param 'PackageName' =  I:'Package name i.e. technical flavor of the content' O:'Y YTY'
General Param 'Grouping' =  I:'iTunes grouping' O:'Y YTY' D:'Title'
General Param 'Chapter' =  I:'Name of the chapter' O:'Y YTY' D:'Title'
General Param 'SubTrack' =  I:'Name of the subtrack' O:'Y YTY' D:'Title'
General Param 'Original/Album' =  I:'Original name of the album' O:'Y YTY' D:'Title'
General Param 'Original/Movie' =  I:'Original name of the movie' O:'Y YTY' D:'Title'
General Param 'Original/Part' =  I:'Original name of the part' O:'Y YTY' D:'Title'
General Param 'Original/Track' =  I:'Original name of the track' O:'Y YTY' D:'Title'
General Param 'Compilation' =  I:'iTunes compilation' M:'Yes' O:'N YTY' m:'Yes' D:'Title'
General Param 'Compilation/String' =  I:'iTunes compilation' O:'Y NT' D:'Title'
General Param 'Performer' =  I:'Main performer(s)/artist(s)' O:'Y YTY' D:'Entity'
General Param (Performer/Sort)
General Param 'Performer/Url' =  I:'Homepage of the performer/artist' O:'Y YT' D:'Entity'
General Param 'Original/Performer' =  I:'Original artist(s)/performer(s)' O:'Y YTY' D:'Entity'
General Param 'Accompaniment' =  I:'Band/orchestra/accompaniment/musician' O:'Y YTY' D:'Entity'
General Param 'Composer' =  I:'Name of the original composer' O:'Y YTY' D:'Entity'
General Param 'Composer/Nationality' =  I:'Nationality of the primary composer of the piece (mostly for classical music)' O:'Y YTY' D:'Entity'
General Param (Composer/Sort)
General Param 'Arranger' =  I:'The person who arranged the piece (e.g. Ravel)' O:'Y YTY' D:'Entity'
General Param 'Lyricist' =  I:'The person who wrote the lyrics for the piece' O:'Y YTY' D:'Entity'
General Param 'Original/Lyricist' =  I:'Original lyricist(s)/text writer(s).' O:'Y YTY' D:'Entity'
General Param 'Conductor' =  I:'The artist(s) who performed the work. In classical music this would be the conductor, orchestra, soloists, etc.' O:'Y YTY' D:'Entity'
General Param 'Director' =  I:'Name of the director' O:'Y YTY' D:'Entity'
General Param 'CoDirector' =  I:'Name of the codirector' O:'Y YTY' D:'Entity'
General Param 'AssistantDirector' =  I:'Name of the assistant director' O:'Y YTY' D:'Entity'
General Param 'DirectorOfPhotography' =  I:'Name of the director of photography, also known as cinematographer' O:'Y YTY' D:'Entity'
General Param 'SoundEngineer' =  I:'Name of the sound engineer or sound recordist' O:'Y YTY' D:'Entity'
General Param 'ArtDirector' =  I:'Name of the person who oversees the artists and craftspeople who build the sets' O:'Y YTY' D:'Entity'
General Param 'ProductionDesigner' =  I:'Name of the person responsible for designing the overall visual appearance of a movie' O:'Y YTY' D:'Entity'
General Param 'Choreographer' =  I:'Name of the choreographer' O:'Y YTY' D:'Entity'
General Param 'CostumeDesigner' =  I:'Name of the costume designer' O:'Y YTY' D:'Entity'
General Param 'Actor' =  I:'Real name of an actor/actress playing a role in the movie' O:'Y YTY' D:'Entity'
General Param 'Actor_Character' =  I:'Name of the character an actor or actress plays in this movie' O:'Y YTY' D:'Entity'
General Param 'WrittenBy' =  I:'Author of the story or script' O:'Y YTY' D:'Entity'
General Param 'ScreenplayBy' =  I:'Author of the screenplay or scenario (used for movies and TV shows)' O:'Y YTY' D:'Entity'
General Param 'EditedBy' =  I:'Editors name' O:'Y YTY' D:'Entity'
General Param 'CommissionedBy' =  I:'Name of the person or organization that commissioned the subject of the file' O:'Y YTY' D:'Entity'
General Param 'Producer' =  I:'Name of the producer of the movie' O:'Y YTY' D:'Entity'
General Param 'CoProducer' =  I:'Name of a co-producer' O:'Y YTY' D:'Entity'
General Param 'ExecutiveProducer' =  I:'Name of an executive producer' O:'Y YTY' D:'Entity'
General Param 'MusicBy' =  I:'Main musical artist for a movie' O:'Y YTY' D:'Entity'
General Param 'DistributedBy' =  I:'Company responsible for distribution of the content' O:'Y YTY' D:'Entity'
General Param 'OriginalSourceForm/Distri' =  I:'???'
General Param 'MasteredBy' =  I:'The engineer who mastered the content for a physical medium or for digital distribution' O:'Y YTY' D:'Entity'
General Param 'EncodedBy' =  I:'Name of the person/organisation that encoded/ripped the audio file.' O:'Y YTY' D:'Entity'
General Param 'RemixedBy' =  I:'Name of the artist(s) that interpreted, remixed, or otherwise modified the content' O:'Y YTY' D:'Entity'
General Param 'ProductionStudio' =  I:'Main production studio' O:'Y YTY' D:'Entity'
General Param 'ThanksTo' =  I:'A very general tag for everyone else that wants to be listed' O:'Y YTY' D:'Entity'
General Param 'Publisher' =  I:'Name of the organization publishing the album (i.e. the 'record label') or movie' O:'Y YTY' D:'Entity'
General Param 'Publisher/URL' =  I:'Publisher's official webpage' O:'Y YTY' D:'Entity'
General Param 'Label' =  I:'Brand or trademark associated with the marketing of music recordings and music videos' O:'Y YTY' D:'Entity'
General Param 'Genre' =  I:'Main genre of the audio or video. e.g. classical, ambient-house, synthpop, sci-fi, drama, etc.' O:'Y YTY' D:'Classification'
General Param 'PodcastCategory' =  I:'Podcast category' O:'Y YTY' D:'Classification'
General Param 'Mood' =  I:'Intended to reflect the mood of the item with a few keywords, e.g. Romantic, Sad, Uplifting, etc.' O:'Y YTY' D:'Classification'
General Param 'ContentType' =  I:'The type or genre of the content. e.g. Documentary, Feature Film, Cartoon, Music Video, Music, Sound FX, etc.' O:'Y YTY' D:'Classification'
General Param 'Subject' =  I:'Describes the topic of the file, such as 'Aerial view of Seattle.'' O:'Y YTY' D:'Classification'
General Param 'Description' =  I:'A short description of the contents, such as 'Two birds flying.'' O:'Y YTY' D:'Classification'
General Param 'Keywords' =  I:'Keywords for the content separated by a comma, used for searching' O:'Y YTY' D:'Classification'
General Param 'Summary' =  I:'Plot outline or a summary of the story' O:'Y YTY' D:'Classification'
General Param 'Synopsis' =  I:'Description of the story line of the item' O:'Y YTY' D:'Classification'
General Param 'Period' =  I:'Describes the period that the piece is from or about. e.g. Renaissance.' O:'Y YTY' D:'Classification'
General Param 'LawRating' =  I:'Legal rating of a movie. Format depends on country of origin (e.g. PG or R in the USA, an age in other countries or a URI defining a logo)' O:'Y YTY' D:'Classification'
General Param 'LawRating_Reason' =  I:'Reason for the law rating' O:'Y YTY' D:'Classification'
General Param 'ICRA' =  I:'The ICRA rating (previously RSACi)' O:'Y YTY' D:'Classification'
General Param 'Released_Date' =  I:'Date/year that the content was released' O:'Y YTY' D:'Temporal'
General Param 'Original/Released_Date' =  I:'Date/year that the content was originally released' O:'Y YTY' D:'Temporal'
General Param 'Recorded_Date' =  I:'Time/date/year that the recording began' O:'Y YTY' D:'Temporal'
General Param 'Encoded_Date' =  I:'Time/date/year that the encoding of this content was completed' T:'UTC 1970-01-01 00:00:00' O:'Y YTY' D:'Temporal'
General Param 'Tagged_Date' =  I:'Time/date/year that the tags were added to this content' T:'UTC 2014-07-19 17:43:01' O:'Y YTY' D:'Temporal'
General Param 'Written_Date' =  I:'Time/date/year that the composition of the music/script began' O:'Y YTY' D:'Temporal'
General Param 'Mastered_Date' =  I:'Time/date/year that the content was transferred to a digital medium.' O:'Y YTY' D:'Temporal'
General Param 'File_Created_Date' =  I:'Time that the file was created on the file system' T:'UTC 2022-10-10 07:24:23.775' O:'N NTY' D:'Temporal'
General Param 'File_Created_Date_Local' =  I:'Time that the file was created on the file system (Warning: this field depends of local configuration, do not use it in an international database)' T:'2022-10-10 09:24:23.775' O:'N NTY' D:'Temporal'
General Param 'File_Modified_Date' =  I:'Time that the file was last modified on the file system' T:'UTC 2022-10-10 07:24:23.775' O:'N NTY' D:'Temporal'
General Param 'File_Modified_Date_Local' =  I:'Time that the file was last modified on the file system (Warning: this field depends of local configuration, do not use it in an international database)' T:'2022-10-10 09:24:23.775' O:'N NTY' D:'Temporal'
General Param 'Recorded_Location' =  I:'Location where track was recorded (See COMPOSITION_LOCATION for format)' O:'Y YTY' D:'Spatial'
General Param 'Written_Location' =  I:'Location that the item was originally designed/written. Information should be stored in the following format: country code, state/province, city where the country code is the same 2 octets as in Internet domains, or possibly ISO-3166. e.g. US, Texas, Austin or US, , Austin.' O:'Y YTY' D:'Spatial'
General Param 'Archival_Location' =  I:'Location where an item is archived (e.g. Louvre, Paris, France)' O:'Y YTY' D:'Spatial'
General Param 'Encoded_Application' =  I:'Name of the software package used to create the file, such as Microsoft WaveEdiTY' T:'Lavf53.24.2' O:'N YTY' D:'Technical'
General Param 'Encoded_Application/Strin' =  I:'???'
General Param 'Encoded_Application_Compa' =  I:'???'
General Param 'Encoded_Application_Name' =  I:'Name of the encoding product' O:'N YTY' D:'Technical'
General Param 'Encoded_Application_Versi' =  I:'???'
General Param 'Encoded_Application_Url' =  I:'URL associated with the encoding software' O:'N YTY' D:'Technical'
General Param 'Encoded_Library' =  I:'Software used to create the file' O:'N YTY' D:'Technical'
General Param 'Encoded_Library/String' =  I:'Software used to create the file, trying to have the format 'CompanyName ProductName (OperatingSystem) Version (Date)'' O:'Y NT' D:'Technical'
General Param 'Encoded_Library_CompanyNa' =  I:'???'
General Param 'Encoded_Library_Name' =  I:'Name of the the encoding library' O:'N NTY' D:'Technical'
General Param 'Encoded_Library_Version' =  I:'Version of encoding library' O:'N NTY' D:'Technical'
General Param 'Encoded_Library_Date' =  I:'Release date of encoding library' O:'N NTY' D:'Technical'
General Param 'Encoded_Library_Settings' =  I:'Parameters used by the encoding library' O:'Y YTY' D:'Technical'
General Param 'Encoded_OperatingSystem' =  I:'Operating System used by encoding application' O:'N YTY' D:'Technical'
General Param 'Cropped' =  I:'Describes whether an image has been cropped and, if so, how it was cropped' O:'Y YTY' D:'Technical'
General Param 'Dimensions' =  I:'Specifies the size of the original subject of the file (e.g. 8.5 in h, 11 in w)' O:'Y YTY' D:'Technical'
General Param 'DotsPerInch' =  I:'Stores dots per inch setting of the digitization mechanism used to produce the file' O:'Y YTY' D:'Technical'
General Param 'Lightness' =  I:'Describes the changes in lightness settings on the digitization mechanism made during the production of the file' O:'Y YTY' D:'Technical'
General Param 'OriginalSourceMedium' =  I:'Original medium of the material (e.g. vinyl, Audio-CD, Super8 or BetaMax)' O:'Y YTY' D:'Technical'
General Param 'OriginalSourceForm' =  I:'Original form of the material (e.g. slide, paper, map)' O:'Y YTY' D:'Technical'
General Param 'OriginalSourceForm/NumCol' =  I:'???'
General Param 'OriginalSourceForm/Name' =  I:'Name of the product the file was originally intended for' O:'Y YTY' D:'Technical'
General Param 'OriginalSourceForm/Croppe' =  I:'???'
General Param 'OriginalSourceForm/Sharpn' =  I:'???'
General Param 'Tagged_Application' =  I:'Software used to tag the file' O:'Y YTY' D:'Technical'
General Param 'BPM' =  I:'Average number of beats per minute' O:'Y YTY' D:'Technical'
General Param 'ISRC' =  I:'International Standard Recording Code, excluding the ISRC prefix and including hyphens' O:'Y YTY' D:'Identifier'
General Param 'ISBN' =  I:'International Standard Book Number.' O:'Y YTY' D:'Identifier'
General Param 'BarCode' =  I:'EAN-13 (13-digit European Article Numbering) or UPC-A (12-digit Universal Product Code) bar code identifier' O:'Y YTY' D:'Identifier'
General Param 'LCCN' =  I:'Library of Congress Control Number' O:'Y YTY' D:'Identifier'
General Param 'CatalogNumber' =  I:'A label-specific catalogue number used to identify the release. e.g. TIC 01' O:'Y YTY' D:'Identifier'
General Param 'LabelCode' =  I:'A 4-digit or 5-digit number to identify the record label, typically printed as (LC) xxxx or (LC) 0xxxx on CDs medias or covers, with only the number being stored' O:'Y YTY' D:'Identifier'
General Param 'Owner' =  I:'Owner of the file' O:'Y YTY' D:'Legal'
General Param 'Copyright' =  I:'Copyright attribution' O:'Y YTY' D:'Legal'
General Param 'Copyright/Url' =  I:'Link to a site with copyright/legal information' O:'Y YTY' D:'Legal'
General Param 'Producer_Copyright' =  I:'Copyright information as per the production copyright holder' O:'Y YTY' D:'Legal'
General Param 'TermsOfUse' =  I:'License information (e.g. All Rights Reserved, Any Use Permitted)' O:'Y YTY' D:'Legal'
General Param (ServiceName)
General Param (ServiceChannel)
General Param (Service/Url)
General Param (ServiceProvider)
General Param (ServiceProvider/Url)
General Param (ServiceType)
General Param (NetworkName)
General Param (OriginalNetworkName)
General Param (Country)
General Param (TimeZone)
General Param 'Cover' =  I:'Is there a cover' O:'Y YTY' D:'Info'
General Param 'Cover_Description' =  I:'Short description (e.g. Earth in space)' O:'Y YTY' D:'Info'
General Param (Cover_Type)
General Param 'Cover_Mime' =  I:'Mime type of cover file' O:'Y YTY' D:'Info'
General Param 'Cover_Data' =  I:'Cover, in binary format, encoded as BASE64' O:'N YTY' D:'Info'
General Param 'Lyrics' =  I:'Text of a song' O:'Y YTY' D:'Info'
General Param 'Comment' =  I:'Any comment related to the content' O:'Y YTY' D:'Personal'
General Param 'Rating' =  I:'A numeric value defining how much a person likes the song/movie. The number is between 0 and 5 with decimal values possible (e.g. 2.7), 5(.0) being the highest possible rating.' O:'Y YTY' D:'Personal'
General Param 'Added_Date' =  I:'Date/year the item was added to the owners collection' O:'Y YTY' D:'Personal'
General Param 'Played_First_Date' =  I:'Date the owner first played an item' O:'Y YTY' D:'Personal'
General Param 'Played_Last_Date' =  I:'Date the owner last played an item' O:'Y YTY' D:'Personal'
General Param 'Played_Count' =  I:'Number of times an item was played' O:'Y YIY' D:'Personal'
General Param (EPG_Positions_Begin)
General Param (EPG_Positions_End)
----->Video
Video Param 'Count' =  I:'Count of objects available in this stream' T:'378' O:'N NI'
Video Param 'Status' =  I:'bit field (0=IsAccepted, 1=IsFilled, 2=IsUpdated, 3=IsFinished)' O:'N NI'
Video Param 'StreamCount' =  I:'Count of streams of that kind available' T:'1' O:'N NI'
Video Param 'StreamKind' =  I:'Stream type name' T:'Video' O:'N NT'
Video Param 'StreamKind/String' =  I:'Stream type name' T:'Video' O:'N NT'
Video Param 'StreamKindID' =  I:'Number of the stream (base=0)' T:'0' O:'N NI'
Video Param 'StreamKindPos' =  I:'When multiple streams, number of the stream (base=1)' O:'N NI'
Video Param 'StreamOrder' =  I:'Stream order in the file, whatever is the kind of stream (base=0)' T:'0' O:'N YIY'
Video Param 'FirstPacketOrder' =  I:'Order of the first fully decodable packet met in the file, whatever is the kind of stream (base=0)' O:'N YIY'
Video Param 'Inform' =  I:'Inform'
Video Param 'ID' =  I:'The ID for this stream in this file' T:'1' O:'N YTY'
Video Param 'ID/String' =  I:'The ID for this stream in this file' T:'1' O:'Y NT'
Video Param 'OriginalSourceMedium_ID' =  I:'The ID for this stream in the original medium of the material' O:'N YTY'
Video Param 'OriginalSourceMedium_ID/S' =  I:'???'
Video Param 'UniqueID' =  I:'The unique ID for this stream, should be copied with stream copy' O:'N YTY'
Video Param 'UniqueID/String' =  I:'The unique ID for this stream, should be copied with stream copy' O:'Y NT'
Video Param 'MenuID' =  I:'The menu ID for this stream in this file' O:'N YTY'
Video Param 'MenuID/String' =  I:'The menu ID for this stream in this file' O:'Y NT'
Video Param 'Format' =  I:'Format used' T:'AVC' O:'N YTY'
Video Param 'Format/String' =  I:'Format used + additional features' T:'AVC' O:'Y NT'
Video Param 'Format/Info' =  I:'Info about Format' T:'Advanced Video Codec' O:'Y NT'
Video Param 'Format/Url' =  I:'Link' T:'http://developers.videolan.org/x264.html' O:'N NT'
Video Param 'Format_Commercial' =  I:'Commercial name used by vendor for theses setings or Format field if there is no difference' T:'AVC' O:'N NT'
Video Param 'Format_Commercial_IfAny' =  I:'Commercial name used by vendor for theses setings if there is one' O:'Y YTY'
Video Param 'Format_Version' =  I:'Version of this format' O:'Y YTY'
Video Param 'Format_Profile' =  I:'Profile of the Format (old XML: 'Profile@Level@Tier' format' T:'Main@L1.3' O:'Y YTY' H:' MIXML: 'Profile' only)'
Video Param 'Format_Level' =  I:'Level of the Format (only MIXML)' O:'Y YTY'
Video Param 'Format_Tier' =  I:'Tier of the Format (only MIXML)' O:'Y YTY'
Video Param 'Format_Compression' =  I:'Compression method used' O:'Y YTY'
Video Param 'Format_AdditionalFeatures' =  I:'Format features needed for fully supporting the content' O:'N YTY'
Video Param 'MultiView_BaseProfile' =  I:'Multiview, profile of the base stream' O:'Y YTY'
Video Param 'MultiView_Count' =  I:'Multiview, count of views' O:'Y YTY'
Video Param 'MultiView_Layout' =  I:'Multiview, how views are muxed in the container in case of it is not muxing in the stream' O:'Y YTY'
Video Param 'HDR_Format' =  I:'Format used' O:'N YTY'
Video Param 'HDR_Format/String' =  I:'Format used + version + profile + level + layers + settings + compatibility' O:'Y NT'
Video Param 'HDR_Format_Commercial' =  I:'Commercial name used by vendor for theses HDR settings or HDR Format field if there is no difference' O:'N NT'
Video Param 'HDR_Format_Version' =  I:'Version of this format' O:'N YTY'
Video Param 'HDR_Format_Profile' =  I:'Profile of the Format' O:'N YTY'
Video Param 'HDR_Format_Level' =  I:'Level of the Format' O:'N YTY'
Video Param 'HDR_Format_Settings' =  I:'Settings of the Format' O:'N YTY'
Video Param 'HDR_Format_Compatibility' =  I:'Compatibility with some commercial namings' O:'N YTY'
Video Param 'Format_Settings' =  I:'Settings needed for decoder used, summary' T:'CABAC / 1 Ref Frames' O:'Y NT'
Video Param 'Format_Settings_BVOP' =  I:'Settings needed for decoder used, detailled' M:'Yes' O:'N YTY' m:'Yes'
Video Param 'Format_Settings_BVOP/Stri' =  I:'???'
Video Param 'Format_Settings_QPel' =  I:'Settings needed for decoder used, detailled' M:'Yes' O:'N YTY' m:'Yes'
Video Param 'Format_Settings_QPel/Stri' =  I:'???'
Video Param 'Format_Settings_GMC' =  I:'Settings needed for decoder used, detailled' M:' warppoint' O:'N YIY' m:' warppoint'
Video Param (Format_Settings_GMC/String)
Video Param 'Format_Settings_Matrix' =  I:'Settings needed for decoder used, detailled' O:'N YTY'
Video Param 'Format_Settings_Matrix/St' =  I:'???'
Video Param 'Format_Settings_Matrix_Da' =  I:'???'
Video Param 'Format_Settings_CABAC' =  I:'Settings needed for decoder used, detailled' T:'Yes' M:'Yes' O:'N YTY' m:'Yes'
Video Param 'Format_Settings_CABAC/Str' =  I:'???'
Video Param 'Format_Settings_RefFrames' =  I:'Settings needed for decoder used, detailled' T:'1' M:' frame' O:'N YIY' m:' frame'
Video Param 'Format_Settings_RefFrames' =  I:'Settings needed for decoder used, detailled' T:'1' M:' frame' O:'N YIY' m:' frame'
Video Param 'Format_Settings_Pulldown' =  I:'Settings needed for decoder used, detailled' O:'Y YTY'
Video Param (Format_Settings_Endianness)
Video Param (Format_Settings_Packing)
Video Param 'Format_Settings_FrameMode' =  I:'Settings needed for decoder used, detailled' O:'Y YTY'
Video Param 'Format_Settings_GOP' =  I:'Settings needed for decoder used, detailled (M=x N=y)' O:'Y YTY'
Video Param 'Format_Settings_PictureSt' =  I:'???'
Video Param 'Format_Settings_Wrapping' =  I:'Wrapping mode (Frame wrapped or Clip wrapped)' O:'Y YTY'
Video Param 'InternetMediaType' =  I:'Internet Media Type (aka MIME Type, Content-Type)' T:'video/H264' O:'N YT'
Video Param 'MuxingMode' =  I:'How this file is muxed in the container' O:'Y YTY'
Video Param 'CodecID' =  I:'Codec ID (found in some containers)' T:'avc1' O:'Y YTY'
Video Param 'CodecID/String' =  I:'Codec ID (found in some containers)' O:'Y NT'
Video Param 'CodecID/Info' =  I:'Info on the codec' T:'Advanced Video Coding' O:'Y NT'
Video Param 'CodecID/Hint' =  I:'Hint/popular name for this codec' O:'Y NT'
Video Param 'CodecID/Url' =  I:'Homepage for more details about this codec' O:'N NT'
Video Param 'CodecID_Description' =  I:'Manual description given by the container' O:'Y YT'
Video Param 'Codec' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Codec/String' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Codec/Family' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Codec/Info' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Codec/Url' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Codec/CC' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Codec_Profile' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Codec_Description' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Codec_Settings' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Codec_Settings_PacketBitS' =  I:'???'
Video Param 'Codec_Settings_BVOP' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Codec_Settings_QPel' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Codec_Settings_GMC' =  I:'Deprecated, do not use in new projects' M:' warppoint' O:'N NT' m:' warppoint'
Video Param 'Codec_Settings_GMC/String' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Codec_Settings_Matrix' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Codec_Settings_Matrix_Dat' =  I:'???'
Video Param 'Codec_Settings_CABAC' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Codec_Settings_RefFrames' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Duration' =  I:'Play time of the stream in ms' T:'125934' M:' ms' O:'N YFY' m:' ms'
Video Param 'Duration/String' =  I:'Play time in format : XXx YYy only, YYy omited if zero' T:'2 min 5 s' O:'Y NT'
Video Param 'Duration/String1' =  I:'Play time in format : HHh MMmn SSs MMMms, XX omited if zero' T:'2 min 5 s 934 ms' O:'N NT'
Video Param 'Duration/String2' =  I:'Play time in format : XXx YYy only, YYy omited if zero' T:'2 min 5 s' O:'N NT'
Video Param 'Duration/String3' =  I:'Play time in format : HH:MM:SS.MMM' T:'00:02:05.934' O:'N NT'
Video Param 'Duration/String4' =  I:'Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available' T:'00:02:05:14' O:'N NT'
Video Param 'Duration/String5' =  I:'Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)' T:'00:02:05.934 (00:02:05:14)' O:'N NT'
Video Param 'Duration_FirstFrame' =  I:'Duration of the first frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Video Param 'Duration_FirstFrame/Strin' =  I:'???'
Video Param 'Duration_FirstFrame/Strin' =  I:'???'
Video Param 'Duration_FirstFrame/Strin' =  I:'???'
Video Param 'Duration_FirstFrame/Strin' =  I:'???'
Video Param 'Duration_FirstFrame/Strin' =  I:'???'
Video Param 'Duration_FirstFrame/Strin' =  I:'???'
Video Param 'Duration_LastFrame' =  I:'Duration of the last frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Video Param 'Duration_LastFrame/String' =  I:'Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero' O:'Y NT'
Video Param 'Duration_LastFrame/String' =  I:'Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero' O:'Y NT'
Video Param 'Duration_LastFrame/String' =  I:'Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero' O:'Y NT'
Video Param 'Duration_LastFrame/String' =  I:'Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero' O:'Y NT'
Video Param 'Duration_LastFrame/String' =  I:'Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero' O:'Y NT'
Video Param 'Duration_LastFrame/String' =  I:'Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero' O:'Y NT'
Video Param 'Source_Duration' =  I:'Source Play time of the stream, in ms' M:' ms' O:'N YFY' m:' ms'
Video Param 'Source_Duration/String' =  I:'Source Play time in format : XXx YYy only, YYy omited if zero' O:'Y NT'
Video Param 'Source_Duration/String1' =  I:'Source Play time in format : HHh MMmn SSs MMMms, XX omited if zero' O:'N NT'
Video Param 'Source_Duration/String2' =  I:'Source Play time in format : XXx YYy only, YYy omited if zero' O:'N NT'
Video Param 'Source_Duration/String3' =  I:'Source Play time in format : HH:MM:SS.MMM' O:'N NT'
Video Param 'Source_Duration/String4' =  I:'Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available' O:'N NT'
Video Param 'Source_Duration/String5' =  I:'Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)' O:'N NT'
Video Param 'Source_Duration_FirstFram' =  I:'???'
Video Param 'Source_Duration_FirstFram' =  I:'???'
Video Param 'Source_Duration_FirstFram' =  I:'???'
Video Param 'Source_Duration_FirstFram' =  I:'???'
Video Param 'Source_Duration_FirstFram' =  I:'???'
Video Param 'Source_Duration_FirstFram' =  I:'???'
Video Param 'Source_Duration_FirstFram' =  I:'???'
Video Param 'Source_Duration_LastFrame' =  I:'Source Duration of the last frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Video Param 'Source_Duration_LastFrame' =  I:'Source Duration of the last frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Video Param 'Source_Duration_LastFrame' =  I:'Source Duration of the last frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Video Param 'Source_Duration_LastFrame' =  I:'Source Duration of the last frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Video Param 'Source_Duration_LastFrame' =  I:'Source Duration of the last frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Video Param 'Source_Duration_LastFrame' =  I:'Source Duration of the last frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Video Param 'Source_Duration_LastFrame' =  I:'Source Duration of the last frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Video Param 'BitRate_Mode' =  I:'Bit rate mode (VBR, CBR)' O:'N YTY'
Video Param 'BitRate_Mode/String' =  I:'Bit rate mode (Variable, Cconstant)' O:'Y NT'
Video Param 'BitRate' =  I:'Bit rate in bps' T:'282823' M:' bps' O:'N YFY' m:' b/s'
Video Param 'BitRate/String' =  I:'Bit rate (with measurement)' T:'283 kb/s' O:'Y NT'
Video Param 'BitRate_Minimum' =  I:'Minimum Bit rate in bps' M:' bps' O:'N YFY' m:' b/s'
Video Param 'BitRate_Minimum/String' =  I:'Minimum Bit rate (with measurement)' O:'Y NT'
Video Param 'BitRate_Nominal' =  I:'Nominal Bit rate in bps' M:' bps' O:'N YFY' m:' b/s'
Video Param 'BitRate_Nominal/String' =  I:'Nominal Bit rate (with measurement)' O:'Y NT'
Video Param 'BitRate_Maximum' =  I:'Maximum Bit rate in bps' M:' bps' O:'N YFY' m:' b/s'
Video Param 'BitRate_Maximum/String' =  I:'Maximum Bit rate (with measurement)' O:'Y NT'
Video Param 'BitRate_Encoded' =  I:'Encoded (with forced padding) bit rate in bps, if some container padding is present' M:' bps' O:'N YFY' m:' b/s'
Video Param 'BitRate_Encoded/String' =  I:'Encoded (with forced padding) bit rate (with measurement), if some container padding is present' O:'N NT'
Video Param 'Width' =  I:'Width (aperture size if present) in pixel' T:'320' M:' pixel' O:'N YIY' m:' pixel'
Video Param 'Width/String' =  I:'Width (aperture size if present) with measurement (pixel)' T:'320 pixels' O:'Y NT'
Video Param 'Width_Offset' =  I:'Offset between original width and displayed width in pixel' M:' pixel' O:'N YIY' m:' pixel'
Video Param 'Width_Offset/String' =  I:'Offset between original width and displayed width in pixel' O:'N NT'
Video Param 'Width_Original' =  I:'Original (in the raw stream) width in pixel' M:' pixel' O:'N YIY' m:' pixel'
Video Param 'Width_Original/String' =  I:'Original (in the raw stream) width with measurement (pixel)' O:'Y NT'
Video Param 'Width_CleanAperture' =  I:'Clean Aperture width in pixel' M:' pixel' O:'N YIY' m:' pixel'
Video Param 'Width_CleanAperture/Strin' =  I:'???'
Video Param 'Height' =  I:'Height in pixel' T:'240' M:' pixel' O:'N YIY' m:' pixel'
Video Param 'Height/String' =  I:'Height with measurement (pixel)' T:'240 pixels' O:'Y NT'
Video Param 'Height_Offset' =  I:'Offset between original height and displayed height in pixel' M:' pixel' O:'N YIY' m:' pixel'
Video Param 'Height_Offset/String' =  I:'Offset between original height and displayed height  in pixel' O:'N NT'
Video Param 'Height_Original' =  I:'Original (in the raw stream) height in pixel' M:' pixel' O:'N YIY' m:' pixel'
Video Param 'Height_Original/String' =  I:'Original (in the raw stream) height with measurement (pixel)' O:'Y NT'
Video Param 'Height_CleanAperture' =  I:'Clean Aperture height in pixel' M:' pixel' O:'N YI' m:' pixel'
Video Param 'Height_CleanAperture/Stri' =  I:'???'
Video Param 'Stored_Width' =  I:'Stored width' O:'N YIY'
Video Param 'Stored_Height' =  I:'Stored height' O:'N YIY'
Video Param 'Sampled_Width' =  I:'Sampled width' T:'320' O:'N YIY'
Video Param 'Sampled_Height' =  I:'Sampled height' T:'240' O:'N YIY'
Video Param 'PixelAspectRatio' =  I:'Pixel Aspect ratio' T:'1.000' O:'N YFY'
Video Param 'PixelAspectRatio/String' =  I:'Pixel Aspect ratio' O:'N NT'
Video Param 'PixelAspectRatio_Original' =  I:'Original (in the raw stream) Pixel Aspect ratio' O:'N YFY'
Video Param 'PixelAspectRatio_Original' =  I:'Original (in the raw stream) Pixel Aspect ratio' O:'N YFY'
Video Param 'PixelAspectRatio_CleanApe' =  I:'???'
Video Param 'PixelAspectRatio_CleanApe' =  I:'???'
Video Param 'DisplayAspectRatio' =  I:'Display Aspect ratio' T:'1.333' O:'N YFY'
Video Param 'DisplayAspectRatio/String' =  I:'Display Aspect ratio' T:'4:3' O:'Y NT'
Video Param 'DisplayAspectRatio_Origin' =  I:'???'
Video Param 'DisplayAspectRatio_Origin' =  I:'???'
Video Param 'DisplayAspectRatio_CleanA' =  I:'???'
Video Param 'DisplayAspectRatio_CleanA' =  I:'???'
Video Param 'ActiveFormatDescription' =  I:'Active Format Description (AFD value)' O:'N YNY'
Video Param 'ActiveFormatDescription/S' =  I:'???'
Video Param 'ActiveFormatDescription_M' =  I:'???'
Video Param 'Rotation' =  I:'Rotation' T:'0.000' O:'N YTY'
Video Param 'Rotation/String' =  I:'Rotation (if not horizontal)' O:'Y NT'
Video Param 'FrameRate_Mode' =  I:'Frame rate mode (CFR, VFR)' T:'CFR' O:'N YTY'
Video Param 'FrameRate_Mode/String' =  I:'Frame rate mode (Constant, Variable)' T:'Constant' O:'Y NT'
Video Param 'FrameRate_Mode_Original' =  I:'Original frame rate mode (CFR, VFR)' T:'VFR' O:'N YTY'
Video Param 'FrameRate_Mode_Original/S' =  I:'???'
Video Param 'FrameRate' =  I:'Frames per second' T:'15.000' M:' fps' O:'N YFY' m:' fps'
Video Param 'FrameRate/String' =  I:'Frames per second (with measurement)' T:'15.000 FPS' O:'Y NT'
Video Param 'FrameRate_Num' =  I:'Frames per second, numerator' O:'N NFN'
Video Param 'FrameRate_Den' =  I:'Frames per second, denominator' O:'N NFN'
Video Param 'FrameRate_Minimum' =  I:'Minimum Frames per second' M:' fps' O:'N YFY' m:' fps'
Video Param 'FrameRate_Minimum/String' =  I:'Minimum Frames per second (with measurement)' O:'Y NT'
Video Param 'FrameRate_Nominal' =  I:'Nominal Frames per second' M:' fps' O:'N YFY' m:' fps'
Video Param 'FrameRate_Nominal/String' =  I:'Nominal Frames per second (with measurement)' O:'Y NT'
Video Param 'FrameRate_Maximum' =  I:'Maximum Frames per second' M:' fps' O:'N YFY' m:' fps'
Video Param 'FrameRate_Maximum/String' =  I:'Maximum Frames per second (with measurement)' O:'Y NT'
Video Param 'FrameRate_Original' =  I:'Original (in the raw stream) frames per second' M:' fps' O:'N YFY' m:' fps'
Video Param 'FrameRate_Original/String' =  I:'Original (in the raw stream) frames per second (with measurement)' O:'Y NT'
Video Param 'FrameRate_Original_Num' =  I:'Frames per second, numerator' O:'N NFN'
Video Param 'FrameRate_Original_Den' =  I:'Frames per second, denominator' O:'N NFN'
Video Param 'FrameCount' =  I:'Number of frames' T:'1889' O:'N NIY'
Video Param 'Source_FrameCount' =  I:'Source Number of frames' O:'N NI'
Video Param 'Standard' =  I:'NTSC or PAL' O:'Y NTY'
Video Param 'Resolution' =  I:'Deprecated, do not use in new projects' M:' bit' O:'N NI' m:' bit'
Video Param 'Resolution/String' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Colorimetry' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param (ColorSpace)
Video Param (ChromaSubsampling)
Video Param (ChromaSubsampling/String)
Video Param (ChromaSubsampling_Position)
Video Param 'BitDepth' =  I:'16/24/32' T:'8' M:' bit' O:'N YIY' m:' bit'
Video Param 'BitDepth/String' =  I:'16/24/32 bits' T:'8 bits' O:'Y NT'
Video Param (ScanType)
Video Param (ScanType/String)
Video Param (ScanType_Original)
Video Param (ScanType_Original/String)
Video Param (ScanType_StoreMethod)
Video Param (ScanType_StoreMethod_FieldsPerBlock)
Video Param (ScanType_StoreMethod/String)
Video Param (ScanOrder)
Video Param (ScanOrder/String)
Video Param (ScanOrder_Stored)
Video Param (ScanOrder_Stored/String)
Video Param (ScanOrder_StoredDisplayedInverted)
Video Param (ScanOrder_Original)
Video Param (ScanOrder_Original/String)
Video Param 'Interlacement' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Interlacement/String' =  I:'Deprecated, do not use in new projects' O:'N NT'
Video Param 'Compression_Mode' =  I:'Compression mode (Lossy or Lossless)' O:'N YTY'
Video Param 'Compression_Mode/String' =  I:'Compression mode (Lossy or Lossless)' O:'Y NT'
Video Param 'Compression_Ratio' =  I:'Current stream size divided by uncompressed stream size' O:'Y YFY'
Video Param 'Bits-(Pixel*Frame)' =  I:'bits/(Pixel*Frame) (like Gordian Knot)' T:'0.246' O:'Y NFN'
Video Param 'Delay' =  I:'Delay fixed in the stream (relative) IN MS' M:' ms' O:'N NFY' m:' ms'
Video Param 'Delay/String' =  I:'Delay with measurement' O:'N NT'
Video Param 'Delay/String1' =  I:'Delay with measurement' O:'N NT'
Video Param 'Delay/String2' =  I:'Delay with measurement' O:'N NT'
Video Param 'Delay/String3' =  I:'Delay in format : HH:MM:SS.MMM' O:'N NT'
Video Param 'Delay/String4' =  I:'Delay in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available' O:'N NT'
Video Param 'Delay/String5' =  I:'Delay in format : HH:MM:SS.mmm (HH:MM:SS:FF)' O:'N NT'
Video Param 'Delay_Settings' =  I:'Delay settings (in case of timecode for example)' O:'N NT'
Video Param 'Delay_DropFrame' =  I:'Delay drop frame' O:'N NT'
Video Param 'Delay_Source' =  I:'Delay source (Container or Stream or empty)' O:'N NT'
Video Param 'Delay_Source/String' =  I:'Delay source (Container or Stream or empty)' O:'N NT'
Video Param 'Delay_Original' =  I:'Delay fixed in the raw stream (relative) IN MS' M:' ms' O:'N NIY' m:' ms'
Video Param 'Delay_Original/String' =  I:'Delay with measurement' O:'N NT'
Video Param 'Delay_Original/String1' =  I:'Delay with measurement' O:'N NT'
Video Param 'Delay_Original/String2' =  I:'Delay with measurement' O:'N NT'
Video Param 'Delay_Original/String3' =  I:'Delay in format: HH:MM:SS.MMM' O:'N NT'
Video Param 'Delay_Original/String4' =  I:'Delay in format: HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available' O:'N NT'
Video Param 'Delay_Original/String5' =  I:'Delay in format : HH:MM:SS.mmm (HH:MM:SS:FF)' O:'N NT'
Video Param 'Delay_Original_Settings' =  I:'Delay settings (in case of timecode for example)' O:'N NT'
Video Param 'Delay_Original_DropFrame' =  I:'Delay drop frame info' O:'N NT'
Video Param 'Delay_Original_Source' =  I:'Delay source (Stream or empty)' O:'N NT'
Video Param 'TimeStamp_FirstFrame' =  I:'TimeStamp fixed in the stream (relative) IN MS' M:' ms' O:'N YFY' m:' ms'
Video Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Video Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Video Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Video Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Video Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Video Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Video Param 'TimeCode_FirstFrame' =  I:'Time code in HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available format' O:'Y YCY'
Video Param 'TimeCode_Settings' =  I:'Time code settings' O:'Y YTY'
Video Param 'TimeCode_Source' =  I:'Time code source (Container, Stream, SystemScheme1, SDTI, ANC...)' O:'Y YTY'
Video Param 'Gop_OpenClosed' =  I:'Time code information about Open/Closed' M:' ' O:'N YTY' m:' '
Video Param 'Gop_OpenClosed/String' =  I:'Time code information about Open/Closed' O:'Y NT'
Video Param 'Gop_OpenClosed_FirstFrame' =  I:'Time code information about Open/Closed of first frame if GOP is Open for the other GOPs' M:' ' O:'N YTY' m:' '
Video Param 'Gop_OpenClosed_FirstFrame' =  I:'Time code information about Open/Closed of first frame if GOP is Open for the other GOPs' M:' ' O:'N YTY' m:' '
Video Param 'StreamSize' =  I:'Streamsize in bytes' T:'4452108' M:' byte' O:'N YIY' m:' byte'
Video Param 'StreamSize/String' =  I:'Streamsize in with percentage value' T:'4.25 MiB (42%)' O:'Y NT'
Video Param (StreamSize/String1)
Video Param (StreamSize/String2)
Video Param (StreamSize/String3)
Video Param (StreamSize/String4)
Video Param 'StreamSize/String5' =  I:'Streamsize in with percentage value' T:'4.25 MiB (42%)' O:'N NT'
Video Param 'StreamSize_Proportion' =  I:'Stream size divided by file size' T:'0.42214' O:'N NT'
Video Param 'StreamSize_Demuxed' =  I:'StreamSize in bytes of hte stream after demux' M:' byte' O:'N YIN' m:' byte'
Video Param 'StreamSize_Demuxed/String' =  I:'StreamSize_Demuxed in with percentage value' O:'N NT'
Video Param (StreamSize_Demuxed/String1)
Video Param (StreamSize_Demuxed/String2)
Video Param (StreamSize_Demuxed/String3)
Video Param (StreamSize_Demuxed/String4)
Video Param 'StreamSize_Demuxed/String' =  I:'StreamSize_Demuxed in with percentage value' O:'N NT'
Video Param 'Source_StreamSize' =  I:'Source Streamsize in bytes' M:' byte' O:'N YIY' m:' byte'
Video Param 'Source_StreamSize/String' =  I:'Source Streamsize in with percentage value' O:'Y NT'
Video Param (Source_StreamSize/String1)
Video Param (Source_StreamSize/String2)
Video Param (Source_StreamSize/String3)
Video Param (Source_StreamSize/String4)
Video Param 'Source_StreamSize/String5' =  I:'Source Streamsize in with percentage value' O:'N NT'
Video Param 'Source_StreamSize_Proport' =  I:'???'
Video Param 'StreamSize_Encoded' =  I:'Encoded Streamsize in bytes' M:' byte' O:'N YIY' m:' byte'
Video Param 'StreamSize_Encoded/String' =  I:'Encoded Streamsize in with percentage value' O:'N NT'
Video Param (StreamSize_Encoded/String1)
Video Param (StreamSize_Encoded/String2)
Video Param (StreamSize_Encoded/String3)
Video Param (StreamSize_Encoded/String4)
Video Param 'StreamSize_Encoded/String' =  I:'Encoded Streamsize in with percentage value' O:'N NT'
Video Param 'StreamSize_Encoded_Propor' =  I:'???'
Video Param 'Source_StreamSize_Encoded' =  I:'Source Encoded Streamsize in bytes' M:' byte' O:'N YIY' m:' byte'
Video Param 'Source_StreamSize_Encoded' =  I:'Source Encoded Streamsize in bytes' M:' byte' O:'N YIY' m:' byte'
Video Param (Source_StreamSize_Encoded/String1)
Video Param (Source_StreamSize_Encoded/String2)
Video Param (Source_StreamSize_Encoded/String3)
Video Param (Source_StreamSize_Encoded/String4)
Video Param 'Source_StreamSize_Encoded' =  I:'Source Encoded Streamsize in bytes' M:' byte' O:'N YIY' m:' byte'
Video Param 'Source_StreamSize_Encoded' =  I:'Source Encoded Streamsize in bytes' M:' byte' O:'N YIY' m:' byte'
Video Param 'Alignment' =  I:'How this stream file is aligned in the container' O:'Y NTY'
Video Param (Alignment/String)
Video Param 'Title' =  I:'Name of the track' O:'Y YTY'
Video Param 'Encoded_Application' =  I:'Name of the software package used to create the file, such as Microsoft WaveEdit' O:'N YTY' D:'Technical'
Video Param 'Encoded_Application/Strin' =  I:'???'
Video Param 'Encoded_Application_Compa' =  I:'???'
Video Param 'Encoded_Application_Name' =  I:'Name of the product' O:'N YTY' D:'Technical'
Video Param 'Encoded_Application_Versi' =  I:'???'
Video Param 'Encoded_Application_Url' =  I:'Name of the software package used to create the file, such as Microsoft WaveEdit.' O:'N YTY' D:'Technical'
Video Param 'Encoded_Library' =  I:'Software used to create the file' O:'N YTY' D:'Technical'
Video Param 'Encoded_Library/String' =  I:'Software used to create the file, trying to have the format 'CompanyName ProductName (OperatingSystem) Version (Date)'' O:'Y NT' D:'Technical'
Video Param 'Encoded_Library_CompanyNa' =  I:'???'
Video Param 'Encoded_Library_Name' =  I:'Name of the the encoding-software' O:'N NTY' D:'Technical'
Video Param 'Encoded_Library_Version' =  I:'Version of encoding-software' O:'N NTY' D:'Technical'
Video Param 'Encoded_Library_Date' =  I:'Release date of software' O:'N NTY' D:'Technical'
Video Param 'Encoded_Library_Settings' =  I:'Parameters used by the software' O:'Y YTY' D:'Technical'
Video Param 'Encoded_OperatingSystem' =  I:'Operating System of encoding-software' O:'N YTY' D:'Technical'
Video Param 'Language' =  I:'Language (2-letter ISO 639-1 if exists, else 3-letter ISO 639-2, and with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn)' O:'N YTY'
Video Param 'Language/String' =  I:'Language (full)' O:'Y NT'
Video Param 'Language/String1' =  I:'Language (full)' O:'N NT'
Video Param 'Language/String2' =  I:'Language (2-letter ISO 639-1 if exists, else empty)' O:'N NT'
Video Param 'Language/String3' =  I:'Language (3-letter ISO 639-2 if exists, else empty)' O:'N NT'
Video Param 'Language/String4' =  I:'Language (2-letter ISO 639-1 if exists with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn, else empty)' O:'N NT'
Video Param 'Language_More' =  I:'More info about Language (e.g. Director's Comment)' O:'Y YTY'
Video Param 'ServiceKind' =  I:'Service kind, e.g. visually impaired, commentary, voice over' O:'N YTY'
Video Param 'ServiceKind/String' =  I:'Service kind (full)' O:'Y NT'
Video Param 'Disabled' =  I:'Set if that track should not be used' M:'Yes' O:'N YTY' m:'Yes'
Video Param 'Disabled/String' =  I:'Set if that track should not be used' O:'Y NT'
Video Param 'Default' =  I:'Set if that track should be used if no language found matches the user preference.' M:'Yes' O:'N YTY' m:'Yes'
Video Param 'Default/String' =  I:'Set if that track should be used if no language found matches the user preference.' O:'Y NT'
Video Param 'Forced' =  I:'Set if that track should be used if no language found matches the user preference.' M:'Yes' O:'N YTY' m:'Yes'
Video Param 'Forced/String' =  I:'Set if that track should be used if no language found matches the user preference.' O:'Y NT'
Video Param 'AlternateGroup' =  I:'Number of a group in order to provide versions of the same track' M:'Yes' O:'N YTY' m:'Yes'
Video Param 'AlternateGroup/String' =  I:'Number of a group in order to provide versions of the same track' O:'Y NT'
Video Param 'Encoded_Date' =  I:'UTC time that the encoding of this item was completed began.' T:'UTC 1970-01-01 00:00:00' O:'Y YTY' D:'Temporal'
Video Param 'Tagged_Date' =  I:'UTC time that the tags were done for this item.' T:'UTC 1970-01-01 00:00:00' O:'Y YTY' D:'Temporal'
Video Param (Encryption)
Video Param 'BufferSize' =  I:'Defines the size of the buffer needed to decode the sequence.' O:'N YTY'
Video Param 'colour_description_presen' =  I:'???'
Video Param 'colour_description_presen' =  I:'???'
Video Param 'colour_description_presen' =  I:'???'
Video Param 'colour_description_presen' =  I:'???'
Video Param 'colour_range' =  I:'Colour range for YUV colour space' O:'Y YTY'
Video Param 'colour_range_Source' =  I:'Colour range for YUV colour space (source)' O:'N YTY'
Video Param 'colour_range_Original' =  I:'Colour range for YUV colour space (if incoherencies)' O:'Y YTY'
Video Param 'colour_range_Original_Sou' =  I:'???'
Video Param 'colour_primaries' =  I:'Chromaticity coordinates of the source primaries' O:'Y YTY'
Video Param 'colour_primaries_Source' =  I:'Chromaticity coordinates of the source primaries (source)' O:'N YTY'
Video Param 'colour_primaries_Original' =  I:'Chromaticity coordinates of the source primaries (if incoherencies)' O:'Y YTY'
Video Param 'colour_primaries_Original' =  I:'Chromaticity coordinates of the source primaries (if incoherencies)' O:'Y YTY'
Video Param 'transfer_characteristics' =  I:'Opto-electronic transfer characteristic of the source picture' O:'Y YTY'
Video Param 'transfer_characteristics_' =  I:'???'
Video Param 'transfer_characteristics_' =  I:'???'
Video Param 'transfer_characteristics_' =  I:'???'
Video Param 'matrix_coefficients' =  I:'Matrix coefficients used in deriving luma and chroma signals from the green, blue, and red primaries' O:'Y YTY'
Video Param 'matrix_coefficients_Sourc' =  I:'???'
Video Param 'matrix_coefficients_Origi' =  I:'???'
Video Param 'matrix_coefficients_Origi' =  I:'???'
Video Param 'MasteringDisplay_ColorPri' =  I:'???'
Video Param 'MasteringDisplay_ColorPri' =  I:'???'
Video Param 'MasteringDisplay_ColorPri' =  I:'???'
Video Param 'MasteringDisplay_ColorPri' =  I:'???'
Video Param 'MasteringDisplay_Luminanc' =  I:'???'
Video Param 'MasteringDisplay_Luminanc' =  I:'???'
Video Param 'MasteringDisplay_Luminanc' =  I:'???'
Video Param 'MasteringDisplay_Luminanc' =  I:'???'
Video Param 'MaxCLL' =  I:'Maximum content light level' O:'Y YTY'
Video Param 'MaxCLL_Source' =  I:'Maximum content light level (source)' O:'N YTY'
Video Param 'MaxCLL_Original' =  I:'Maximum content light level (if incoherencies)' O:'Y YTY'
Video Param 'MaxCLL_Original_Source' =  I:'Maximum content light level (source if incoherencies)' O:'N YTY'
Video Param 'MaxFALL' =  I:'Maximum frame average light level' O:'Y YTY'
Video Param 'MaxFALL_Source' =  I:'Maximum frame average light level (source)' O:'N YTY'
Video Param 'MaxFALL_Original' =  I:'Maximum frame average light level (if incoherencies)' O:'Y YTY'
Video Param 'MaxFALL_Original_Source' =  I:'Maximum frame average light level (source if incoherencies)' O:'N YTY'
----->Audio
Audio Param 'Count' =  I:'Count of objects available in this stream' T:'280' O:'N NI'
Audio Param 'Status' =  I:'bit field (0=IsAccepted, 1=IsFilled, 2=IsUpdated, 3=IsFinished)' O:'N NI'
Audio Param 'StreamCount' =  I:'Count of streams of that kind available' T:'1' O:'N NI'
Audio Param 'StreamKind' =  I:'Stream type name' T:'Audio' O:'N NT'
Audio Param 'StreamKind/String' =  I:'Stream type name' T:'Audio' O:'N NT'
Audio Param 'StreamKindID' =  I:'Number of the stream (base=0)' T:'0' O:'N NI'
Audio Param 'StreamKindPos' =  I:'When multiple streams, number of the stream (base=1)' O:'N NI'
Audio Param 'StreamOrder' =  I:'Stream order in the file, whatever is the kind of stream (base=0)' T:'1' O:'N YIY'
Audio Param 'FirstPacketOrder' =  I:'Order of the first fully decodable packet met in the file, whatever is the kind of stream (base=0)' O:'N YIY'
Audio Param 'Inform' =  I:'Inform'
Audio Param 'ID' =  I:'The ID for this stream in this file' T:'2' O:'N YTY'
Audio Param 'ID/String' =  I:'The ID for this stream in this file' T:'2' O:'Y NT'
Audio Param 'OriginalSourceMedium_ID' =  I:'The ID for this stream in the original medium of the material' O:'N YTY'
Audio Param 'OriginalSourceMedium_ID/S' =  I:'???'
Audio Param 'UniqueID' =  I:'The unique ID for this stream, should be copied with stream copy' O:'N YTY'
Audio Param 'UniqueID/String' =  I:'The unique ID for this stream, should be copied with stream copy' O:'Y NT'
Audio Param 'MenuID' =  I:'The menu ID for this stream in this file' O:'N YTY'
Audio Param 'MenuID/String' =  I:'The menu ID for this stream in this file' O:'Y NT'
Audio Param 'Format' =  I:'Format used' T:'AAC' O:'N YTY'
Audio Param 'Format/String' =  I:'Format used + additional features' T:'AAC LC' O:'Y NT'
Audio Param 'Format/Info' =  I:'Info about the format' T:'Advanced Audio Codec Low Complexity' O:'Y NT'
Audio Param 'Format/Url' =  I:'Homepage of this format' O:'N NT'
Audio Param 'Format_Commercial' =  I:'Commercial name used by vendor for theses setings or Format field if there is no difference' T:'AAC' O:'N YT'
Audio Param 'Format_Commercial_IfAny' =  I:'Commercial name used by vendor for theses setings if there is one' O:'Y YTY'
Audio Param 'Format_Version' =  I:'Version of this format' O:'Y YTY'
Audio Param 'Format_Profile' =  I:'Profile of the Format (old XML: 'Profile@Level' format' O:'Y YTY' H:' MIXML: 'Profile' only)'
Audio Param 'Format_Level' =  I:'Level of the Format (only MIXML)' O:'Y YTY'
Audio Param 'Format_Compression' =  I:'Compression method used' O:'Y YTY'
Audio Param 'Format_Settings' =  I:'Settings needed for decoder used, summary' O:'Y NT'
Audio Param (Format_Settings_SBR)
Audio Param (Format_Settings_SBR/String)
Audio Param (Format_Settings_PS)
Audio Param (Format_Settings_PS/String)
Audio Param (Format_Settings_Mode)
Audio Param (Format_Settings_ModeExtension)
Audio Param (Format_Settings_Emphasis)
Audio Param (Format_Settings_Floor)
Audio Param (Format_Settings_Firm)
Audio Param (Format_Settings_Endianness)
Audio Param (Format_Settings_Sign)
Audio Param (Format_Settings_Law)
Audio Param (Format_Settings_ITU)
Audio Param 'Format_Settings_Wrapping' =  I:'Wrapping mode (Frame wrapped or Clip wrapped)' O:'Y YTY'
Audio Param 'Format_AdditionalFeatures' =  I:'Format features needed for fully supporting the content' T:'LC' O:'N YTY'
Audio Param 'Matrix_Format' =  I:'Matrix format (e.g. DTS Neural)' O:'Y YTY'
Audio Param 'InternetMediaType' =  I:'Internet Media Type (aka MIME Type, Content-Type)' O:'N YT'
Audio Param 'MuxingMode' =  I:'How this stream is muxed in the container' O:'Y YTY'
Audio Param 'MuxingMode_MoreInfo' =  I:'More info (text) about the muxing mode' O:'Y NT'
Audio Param 'CodecID' =  I:'Codec ID (found in some containers)' T:'mp4a-40-2' O:'Y YTY'
Audio Param 'CodecID/String' =  I:'Codec ID (found in some containers)' O:'Y NT'
Audio Param 'CodecID/Info' =  I:'Info about codec ID' O:'Y NT'
Audio Param 'CodecID/Hint' =  I:'Hint/popular name for this codec ID' O:'Y NT'
Audio Param 'CodecID/Url' =  I:'Homepage for more details about this codec ID' O:'N NT'
Audio Param 'CodecID_Description' =  I:'Manual description given by the container' O:'Y YT'
Audio Param 'Codec' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Codec/String' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Codec/Family' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Codec/Info' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Codec/Url' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Codec/CC' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Codec_Description' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Codec_Profile' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Codec_Settings' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Codec_Settings_Automatic' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Codec_Settings_Floor' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Codec_Settings_Firm' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Codec_Settings_Endianness' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Codec_Settings_Sign' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Codec_Settings_Law' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Codec_Settings_ITU' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Duration' =  I:'Play time of the stream, in ms' T:'125952' M:' ms' O:'N YFY' m:' ms'
Audio Param 'Duration/String' =  I:'Play time in format : XXx YYy only, YYy omited if zero' T:'2 min 5 s' O:'Y NT'
Audio Param 'Duration/String1' =  I:'Play time in format : HHh MMmn SSs MMMms, XX omited if zero' T:'2 min 5 s 952 ms' O:'N NT'
Audio Param 'Duration/String2' =  I:'Play time in format : XXx YYy only, YYy omited if zero' T:'2 min 5 s' O:'N NT'
Audio Param 'Duration/String3' =  I:'Play time in format : HH:MM:SS.MMM' T:'00:02:05.952' O:'N NT'
Audio Param 'Duration/String4' =  I:'Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available' T:'00:02:05:29' O:'N NT'
Audio Param 'Duration/String5' =  I:'Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)' T:'00:02:05.952 (00:02:05:29)' O:'N NT'
Audio Param 'Duration_FirstFrame' =  I:'Duration of the first frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Audio Param 'Duration_FirstFrame/Strin' =  I:'???'
Audio Param 'Duration_FirstFrame/Strin' =  I:'???'
Audio Param 'Duration_FirstFrame/Strin' =  I:'???'
Audio Param 'Duration_FirstFrame/Strin' =  I:'???'
Audio Param 'Duration_FirstFrame/Strin' =  I:'???'
Audio Param 'Duration_FirstFrame/Strin' =  I:'???'
Audio Param 'Duration_LastFrame' =  I:'Duration of the last frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Audio Param 'Duration_LastFrame/String' =  I:'Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero' O:'Y NT'
Audio Param 'Duration_LastFrame/String' =  I:'Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero' O:'Y NT'
Audio Param 'Duration_LastFrame/String' =  I:'Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero' O:'Y NT'
Audio Param 'Duration_LastFrame/String' =  I:'Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero' O:'Y NT'
Audio Param 'Duration_LastFrame/String' =  I:'Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero' O:'Y NT'
Audio Param 'Duration_LastFrame/String' =  I:'Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero' O:'Y NT'
Audio Param 'Source_Duration' =  I:'Source Play time of the stream, in ms' M:' ms' O:'N YFY' m:' ms'
Audio Param 'Source_Duration/String' =  I:'Source Play time in format : XXx YYy only, YYy omited if zero' O:'Y NT'
Audio Param 'Source_Duration/String1' =  I:'Source Play time in format : HHh MMmn SSs MMMms, XX omited if zero' O:'N NT'
Audio Param 'Source_Duration/String2' =  I:'Source Play time in format : XXx YYy only, YYy omited if zero' O:'N NT'
Audio Param 'Source_Duration/String3' =  I:'Source Play time in format : HH:MM:SS.MMM' O:'N NT'
Audio Param 'Source_Duration/String4' =  I:'Source Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available' O:'N NT'
Audio Param 'Source_Duration/String5' =  I:'Source Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)' O:'N NT'
Audio Param 'Source_Duration_FirstFram' =  I:'???'
Audio Param 'Source_Duration_FirstFram' =  I:'???'
Audio Param 'Source_Duration_FirstFram' =  I:'???'
Audio Param 'Source_Duration_FirstFram' =  I:'???'
Audio Param 'Source_Duration_FirstFram' =  I:'???'
Audio Param 'Source_Duration_FirstFram' =  I:'???'
Audio Param 'Source_Duration_FirstFram' =  I:'???'
Audio Param 'Source_Duration_LastFrame' =  I:'Source Duration of the last frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Audio Param 'Source_Duration_LastFrame' =  I:'Source Duration of the last frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Audio Param 'Source_Duration_LastFrame' =  I:'Source Duration of the last frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Audio Param 'Source_Duration_LastFrame' =  I:'Source Duration of the last frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Audio Param 'Source_Duration_LastFrame' =  I:'Source Duration of the last frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Audio Param 'Source_Duration_LastFrame' =  I:'Source Duration of the last frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Audio Param 'Source_Duration_LastFrame' =  I:'Source Duration of the last frame if it is longer than others, in ms' M:' ms' O:'N YFY' m:' ms'
Audio Param 'BitRate_Mode' =  I:'Bit rate mode (VBR, CBR)' T:'VBR' O:'N YTY'
Audio Param 'BitRate_Mode/String' =  I:'Bit rate mode (Constant, Variable)' T:'Variable' O:'Y NT'
Audio Param 'BitRate' =  I:'Bit rate in bps' T:'384000' M:' bps' O:'N YFY' m:' b/s'
Audio Param 'BitRate/String' =  I:'Bit rate (with measurement)' T:'384 kb/s' O:'Y NT'
Audio Param 'BitRate_Minimum' =  I:'Minimum Bit rate in bps' M:' bps' O:'N YFY' m:' b/s'
Audio Param 'BitRate_Minimum/String' =  I:'Minimum Bit rate (with measurement)' O:'Y NT'
Audio Param 'BitRate_Nominal' =  I:'Nominal Bit rate in bps' M:' bps' O:'N YFY' m:' b/s'
Audio Param 'BitRate_Nominal/String' =  I:'Nominal Bit rate (with measurement)' O:'Y NT'
Audio Param 'BitRate_Maximum' =  I:'Maximum Bit rate in bps' T:'467336' M:' bps' O:'N YFY' m:' b/s'
Audio Param 'BitRate_Maximum/String' =  I:'Maximum Bit rate (with measurement)' T:'467 kb/s' O:'Y NT'
Audio Param 'BitRate_Encoded' =  I:'Encoded (with forced padding) bit rate in bps, if some container padding is present' M:' bps' O:'N YFY' m:' b/s'
Audio Param 'BitRate_Encoded/String' =  I:'Encoded (with forced padding) bit rate (with measurement), if some container padding is present' O:'N NT'
Audio Param 'Channel(s)' =  I:'Number of channels' T:'6' M:' channel' O:'N YIY' m:' channel'
Audio Param 'Channel(s)/String' =  I:'Number of channels (with measurement)' T:'6 channels' O:'Y NT'
Audio Param 'Channel(s)_Original' =  I:'Number of channels' M:' channel' O:'N YIY' m:' channel'
Audio Param 'Channel(s)_Original/Strin' =  I:'???'
Audio Param 'Matrix_Channel(s)' =  I:'Number of channels after matrix decoding' M:' channel' O:'N YIY' m:' channel'
Audio Param 'Matrix_Channel(s)/String' =  I:'Number of channels after matrix decoding (with measurement)' O:'Y NT'
Audio Param 'ChannelPositions' =  I:'Position of channels' T:'Front: L C R, Side: L R, LFE' O:'N YTY'
Audio Param 'ChannelPositions_Original' =  I:'Position of channels' O:'N YTY'
Audio Param 'ChannelPositions/String2' =  I:'Position of channels (x/y.z format)' T:'3/2/0.1' O:'N NT'
Audio Param 'ChannelPositions_Original' =  I:'Position of channels' O:'N YTY'
Audio Param 'Matrix_ChannelPositions' =  I:'Position of channels after matrix decoding' O:'Y YTY'
Audio Param 'Matrix_ChannelPositions/S' =  I:'???'
Audio Param 'ChannelLayout' =  I:'Layout of channels (in the stream)' T:'C L R Ls Rs LFE' O:'Y YTY'
Audio Param 'ChannelLayout_Original' =  I:'Layout of channels (in the stream)' O:'Y YTY'
Audio Param 'ChannelLayoutID' =  I:'ID of layout of channels (e.g. MXF descriptor channel assignment). Warning, sometimes this is not enough for uniquely identifying a layout (e.g. MXF descriptor channel assignment is SMPTE 377-4). For AC-3, the form is x,y with x=acmod and y=lfeon.' O:'N YT'
Audio Param 'SamplesPerFrame' =  I:'Sampling rate' T:'1024' O:'N YFY'
Audio Param 'SamplingRate' =  I:'Sampling rate' T:'48000' M:' Hz' O:'N YFY' m:' Hz'
Audio Param 'SamplingRate/String' =  I:'in KHz' T:'48.0 kHz' O:'Y NT'
Audio Param 'SamplingCount' =  I:'Sample count (based on sampling rate)' T:'6045696' O:'N NIY'
Audio Param 'Source_SamplingCount' =  I:'Source Sample count (based on sampling rate)' O:'N NIY'
Audio Param 'FrameRate' =  I:'Frames per second' T:'46.875' M:' fps' O:'N YFY' m:' fps'
Audio Param 'FrameRate/String' =  I:'Frames per second (with measurement)' T:'46.875 FPS (1024 SPF)' O:'Y NT'
Audio Param 'FrameRate_Num' =  I:'Frames per second, numerator' O:'N NFN'
Audio Param 'FrameRate_Den' =  I:'Frames per second, denominator' O:'N NFN'
Audio Param 'FrameCount' =  I:'Frame count (a frame contains a count of samples depends of the format)' T:'5904' O:'N NIY'
Audio Param 'Source_FrameCount' =  I:'Source Frame count (a frame contains a count of samples depends of the format)' O:'N NIY'
Audio Param 'Resolution' =  I:'Deprecated, do not use in new projects' M:' bit' O:'N NI' m:' bit'
Audio Param 'Resolution/String' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'BitDepth' =  I:'Resolution in bits (8, 16, 20, 24). Note: significant bits in case the stored bit depth is different' M:' bit' O:'N YIY' m:' bit'
Audio Param 'BitDepth/String' =  I:'Resolution in bits (8, 16, 20, 24). Note: significant bits in case the stored bit depth is different' O:'Y NT'
Audio Param 'BitDepth_Detected' =  I:'Detected (during scan of the input by the muxer) resolution in bits' M:' bit' O:'N YIY' m:' bit'
Audio Param 'BitDepth_Detected/String' =  I:'Detected (during scan of the input by the muxer) resolution in bits' O:'Y NT'
Audio Param 'BitDepth_Stored' =  I:'Stored Resolution in bits (8, 16, 20, 24)' M:' bit' O:'N YIY' m:' bit'
Audio Param 'BitDepth_Stored/String' =  I:'Stored Resolution in bits (8, 16, 20, 24)' O:'Y NT'
Audio Param 'Compression_Mode' =  I:'Compression mode (Lossy or Lossless)' T:'Lossy' O:'N YTY'
Audio Param 'Compression_Mode/String' =  I:'Compression mode (Lossy or Lossless)' T:'Lossy' O:'Y NT'
Audio Param 'Compression_Ratio' =  I:'Current stream size divided by uncompressed stream size' O:'Y YF'
Audio Param 'Delay' =  I:'Delay fixed in the stream (relative) IN MS' M:' ms' O:'N NFY' m:' ms'
Audio Param 'Delay/String' =  I:'Delay with measurement' O:'N NT'
Audio Param 'Delay/String1' =  I:'Delay with measurement' O:'N NT'
Audio Param 'Delay/String2' =  I:'Delay with measurement' O:'N NT'
Audio Param 'Delay/String3' =  I:'Delay in format : HH:MM:SS.MMM' O:'N NT'
Audio Param 'Delay/String4' =  I:'Delay in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available' O:'N NT'
Audio Param 'Delay/String5' =  I:'Delay in format : HH:MM:SS.mmm (HH:MM:SS:FF)' O:'N NT'
Audio Param 'Delay_Settings' =  I:'Delay settings (in case of timecode for example)' O:'N NTY'
Audio Param 'Delay_DropFrame' =  I:'Delay drop frame' O:'N NTY'
Audio Param 'Delay_Source' =  I:'Delay source (Container or Stream or empty)' O:'N NTY'
Audio Param 'Delay_Source/String' =  I:'Delay source (Container or Stream or empty)' O:'N NT'
Audio Param 'Delay_Original' =  I:'Delay fixed in the raw stream (relative) IN MS' M:' ms' O:'N NIY' m:' ms'
Audio Param 'Delay_Original/String' =  I:'Delay with measurement' O:'N NT'
Audio Param 'Delay_Original/String1' =  I:'Delay with measurement' O:'N NT'
Audio Param 'Delay_Original/String2' =  I:'Delay with measurement' O:'N NT'
Audio Param 'Delay_Original/String3' =  I:'Delay in format: HH:MM:SS.MMM' O:'N NT'
Audio Param 'Delay_Original/String4' =  I:'Delay in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available' O:'N NT'
Audio Param 'Delay_Original/String5' =  I:'Delay in format : HH:MM:SS.mmm (HH:MM:SS:FF)' O:'N NT'
Audio Param 'Delay_Original_Settings' =  I:'Delay settings (in case of timecode for example)' O:'N NT'
Audio Param 'Delay_Original_DropFrame' =  I:'Delay drop frame info' O:'N NTY'
Audio Param 'Delay_Original_Source' =  I:'Delay source (Stream or empty)' O:'N NTY'
Audio Param 'Video_Delay' =  I:'Delay fixed in the stream (absolute / video)' M:' ms' O:'N NI' m:' ms'
Audio Param (Video_Delay/String)
Audio Param (Video_Delay/String1)
Audio Param (Video_Delay/String2)
Audio Param (Video_Delay/String3)
Audio Param (Video_Delay/String4)
Audio Param (Video_Delay/String5)
Audio Param 'Video0_Delay' =  I:'Deprecated, do not use in new projects' M:' ms' O:'N NI' m:' ms'
Audio Param 'Video0_Delay/String' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Video0_Delay/String1' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Video0_Delay/String2' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Video0_Delay/String3' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Video0_Delay/String4' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'Video0_Delay/String5' =  I:'Deprecated, do not use in new projects' O:'N NT'
Audio Param 'ReplayGain_Gain' =  I:'The gain to apply to reach 89dB SPL on playback' M:' dB' O:'N YTY' m:' dB'
Audio Param (ReplayGain_Gain/String)
Audio Param 'ReplayGain_Peak' =  I:'The maximum absolute peak value of the item' O:'Y YTY'
Audio Param 'StreamSize' =  I:'Streamsize in bytes' T:'6041152' M:' byte' O:'N YIY' m:' byte'
Audio Param 'StreamSize/String' =  I:'Streamsize in with percentage value' T:'5.76 MiB (57%)' O:'Y NT'
Audio Param (StreamSize/String1)
Audio Param (StreamSize/String2)
Audio Param (StreamSize/String3)
Audio Param (StreamSize/String4)
Audio Param 'StreamSize/String5' =  I:'Streamsize in with percentage value' T:'5.76 MiB (57%)' O:'N NT'
Audio Param 'StreamSize_Proportion' =  I:'Stream size divided by file size' T:'0.57280' O:'N NTY'
Audio Param 'StreamSize_Demuxed' =  I:'StreamSize in bytes of hte stream after demux' M:' byte' O:'N YIN' m:' byte'
Audio Param 'StreamSize_Demuxed/String' =  I:'StreamSize_Demuxed in with percentage value' O:'N NT'
Audio Param (StreamSize_Demuxed/String1)
Audio Param (StreamSize_Demuxed/String2)
Audio Param (StreamSize_Demuxed/String3)
Audio Param (StreamSize_Demuxed/String4)
Audio Param 'StreamSize_Demuxed/String' =  I:'StreamSize_Demuxed in with percentage value' O:'N NT'
Audio Param 'Source_StreamSize' =  I:'Source Streamsize in bytes' M:' byte' O:'N YIY' m:' byte'
Audio Param 'Source_StreamSize/String' =  I:'Source Streamsize in with percentage value' O:'Y NT'
Audio Param (Source_StreamSize/String1)
Audio Param (Source_StreamSize/String2)
Audio Param (Source_StreamSize/String3)
Audio Param (Source_StreamSize/String4)
Audio Param 'Source_StreamSize/String5' =  I:'Source Streamsize in with percentage value' O:'N NT'
Audio Param 'Source_StreamSize_Proport' =  I:'???'
Audio Param 'StreamSize_Encoded' =  I:'Encoded Streamsize in bytes' M:' byte' O:'N YIY' m:' byte'
Audio Param 'StreamSize_Encoded/String' =  I:'Encoded Streamsize in with percentage value' O:'N NT'
Audio Param (StreamSize_Encoded/String1)
Audio Param (StreamSize_Encoded/String2)
Audio Param (StreamSize_Encoded/String3)
Audio Param (StreamSize_Encoded/String4)
Audio Param 'StreamSize_Encoded/String' =  I:'Encoded Streamsize in with percentage value' O:'N NT'
Audio Param 'StreamSize_Encoded_Propor' =  I:'???'
Audio Param 'Source_StreamSize_Encoded' =  I:'Source Encoded Streamsize in bytes' M:' byte' O:'N YIY' m:' byte'
Audio Param 'Source_StreamSize_Encoded' =  I:'Source Encoded Streamsize in bytes' M:' byte' O:'N YIY' m:' byte'
Audio Param (Source_StreamSize_Encoded/String1)
Audio Param (Source_StreamSize_Encoded/String2)
Audio Param (Source_StreamSize_Encoded/String3)
Audio Param (Source_StreamSize_Encoded/String4)
Audio Param 'Source_StreamSize_Encoded' =  I:'Source Encoded Streamsize in bytes' M:' byte' O:'N YIY' m:' byte'
Audio Param 'Source_StreamSize_Encoded' =  I:'Source Encoded Streamsize in bytes' M:' byte' O:'N YIY' m:' byte'
Audio Param 'Alignment' =  I:'How this stream file is aligned in the container' O:'N YTY'
Audio Param 'Alignment/String' =  I:'Where this stream file is aligned in the container' O:'Y NT'
Audio Param 'Interleave_VideoFrames' =  I:'Between how many video frames the stream is inserted' O:'N YFY'
Audio Param 'Interleave_Duration' =  I:'Between how much time (ms) the stream is inserted' M:' ms' O:'N YFY' m:' ms'
Audio Param 'Interleave_Duration/Strin' =  I:'???'
Audio Param 'Interleave_Preload' =  I:'How much time is buffered before the first video frame' M:' ms' O:'N YFY' m:' ms'
Audio Param 'Interleave_Preload/String' =  I:'How much time is buffered before the first video frame (with measurement)' O:'Y NT'
Audio Param 'Title' =  I:'Name of the track' O:'Y YTY'
Audio Param 'Encoded_Application' =  I:'Name of the software package used to create the file, such as Microsoft WaveEdit' O:'N YTY' D:'Technical'
Audio Param 'Encoded_Application/Strin' =  I:'???'
Audio Param 'Encoded_Application_Compa' =  I:'???'
Audio Param 'Encoded_Application_Name' =  I:'Name of the product' O:'N YTY' D:'Technical'
Audio Param 'Encoded_Application_Versi' =  I:'???'
Audio Param 'Encoded_Application_Url' =  I:'Name of the software package used to create the file, such as Microsoft WaveEdit.' O:'N YT' D:'Technical'
Audio Param 'Encoded_Library' =  I:'Software used to create the file' O:'N YTY' D:'Technical'
Audio Param 'Encoded_Library/String' =  I:'Software used to create the file, trying to have the format 'CompanyName ProductName (OperatingSystem) Version (Date)'' O:'Y NT' D:'Technical'
Audio Param 'Encoded_Library_CompanyNa' =  I:'???'
Audio Param 'Encoded_Library_Name' =  I:'Name of the the encoding-software' O:'N NTY' D:'Technical'
Audio Param 'Encoded_Library_Version' =  I:'Version of encoding-software' O:'N NTY' D:'Technical'
Audio Param 'Encoded_Library_Date' =  I:'Release date of software' O:'N NTY' D:'Technical'
Audio Param 'Encoded_Library_Settings' =  I:'Parameters used by the software' O:'Y YTY' D:'Technical'
Audio Param 'Encoded_OperatingSystem' =  I:'Operating System of encoding-software' O:'N YTY' D:'Technical'
Audio Param 'Language' =  I:'Language (2-letter ISO 639-1 if exists, else 3-letter ISO 639-2, and with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn)' O:'N YTY'
Audio Param 'Language/String' =  I:'Language (full)' O:'Y NT'
Audio Param 'Language/String1' =  I:'Language (full)' O:'N NT'
Audio Param 'Language/String2' =  I:'Language (2-letter ISO 639-1 if exists, else empty)' O:'N NT'
Audio Param 'Language/String3' =  I:'Language (3-letter ISO 639-2 if exists, else empty)' O:'N NT'
Audio Param 'Language/String4' =  I:'Language (2-letter ISO 639-1 if exists with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn, else empty)' O:'N NT'
Audio Param 'Language_More' =  I:'More info about Language (e.g. Director's Comment)' O:'Y YTY'
Audio Param 'ServiceKind' =  I:'Service kind, e.g. visually impaired, commentary, voice over' O:'N YTY'
Audio Param 'ServiceKind/String' =  I:'Service kind (full)' O:'Y NT'
Audio Param 'Disabled' =  I:'Set if that track should not be used' M:'Yes' O:'N YTY' m:'Yes'
Audio Param 'Disabled/String' =  I:'Set if that track should not be used' O:'Y NT'
Audio Param 'Default' =  I:'Set if that track should be used if no language found matches the user preference.' T:'Yes' M:'Yes' O:'N YTY' m:'Yes'
Audio Param 'Default/String' =  I:'Set if that track should be used if no language found matches the user preference.' T:'Yes' O:'Y NT'
Audio Param 'Forced' =  I:'Set if that track should be used if no language found matches the user preference.' M:'Yes' O:'N YTY' m:'Yes'
Audio Param 'Forced/String' =  I:'Set if that track should be used if no language found matches the user preference.' O:'Y NT'
Audio Param 'AlternateGroup' =  I:'Number of a group in order to provide versions of the same track' T:'1' M:'Yes' O:'N YTY' m:'Yes'
Audio Param 'AlternateGroup/String' =  I:'Number of a group in order to provide versions of the same track' T:'1' O:'Y NT'
Audio Param 'Encoded_Date' =  I:'UTC time that the encoding of this item was completed began.' T:'UTC 1970-01-01 00:00:00' O:'Y YTY' D:'Temporal'
Audio Param 'Tagged_Date' =  I:'UTC time that the tags were done for this item.' T:'UTC 1970-01-01 00:00:00' O:'Y YTY' D:'Temporal'
Audio Param (Encryption)
----->Text
Text Param 'Count' =  I:'???'
Text Param 'Status' =  I:'???'
Text Param 'StreamCount' =  I:'???'
Text Param 'StreamKind' =  I:'???'
Text Param 'StreamKind/String' =  I:'???'
Text Param 'StreamKindID' =  I:'???'
Text Param 'StreamKindPos' =  I:'???'
Text Param 'StreamOrder' =  I:'???'
Text Param 'FirstPacketOrder' =  I:'???'
Text Param 'Inform' =  I:'Inform'
Text Param 'ID' =  I:'???'
Text Param 'ID/String' =  I:'???'
Text Param 'OriginalSourceMedium_ID' =  I:'???'
Text Param 'OriginalSourceMedium_ID/S' =  I:'???'
Text Param 'UniqueID' =  I:'???'
Text Param 'UniqueID/String' =  I:'???'
Text Param 'MenuID' =  I:'???'
Text Param 'MenuID/String' =  I:'???'
Text Param 'Format' =  I:'???'
Text Param 'Format/String' =  I:'???'
Text Param 'Format/Info' =  I:'???'
Text Param 'Format/Url' =  I:'???'
Text Param 'Format_Commercial' =  I:'???'
Text Param 'Format_Commercial_IfAny' =  I:'???'
Text Param 'Format_Version' =  I:'???'
Text Param 'Format_Profile' =  I:'???'
Text Param 'Format_Compression' =  I:'???'
Text Param 'Format_Settings' =  I:'???'
Text Param 'Format_Settings_Wrapping' =  I:'???'
Text Param 'Format_AdditionalFeatures' =  I:'???'
Text Param 'InternetMediaType' =  I:'???'
Text Param 'MuxingMode' =  I:'???'
Text Param 'MuxingMode_MoreInfo' =  I:'???'
Text Param 'CodecID' =  I:'???'
Text Param 'CodecID/String' =  I:'???'
Text Param 'CodecID/Info' =  I:'???'
Text Param 'CodecID/Hint' =  I:'???'
Text Param 'CodecID/Url' =  I:'???'
Text Param 'CodecID_Description' =  I:'???'
Text Param 'Codec' =  I:'???'
Text Param 'Codec/String' =  I:'???'
Text Param 'Codec/Info' =  I:'???'
Text Param 'Codec/Url' =  I:'???'
Text Param 'Codec/CC' =  I:'???'
Text Param 'Duration' =  I:'???'
Text Param 'Duration/String' =  I:'???'
Text Param 'Duration/String1' =  I:'???'
Text Param 'Duration/String2' =  I:'???'
Text Param 'Duration/String3' =  I:'???'
Text Param 'Duration/String4' =  I:'???'
Text Param 'Duration/String5' =  I:'???'
Text Param 'Duration_FirstFrame' =  I:'???'
Text Param 'Duration_FirstFrame/Strin' =  I:'???'
Text Param 'Duration_FirstFrame/Strin' =  I:'???'
Text Param 'Duration_FirstFrame/Strin' =  I:'???'
Text Param 'Duration_FirstFrame/Strin' =  I:'???'
Text Param 'Duration_FirstFrame/Strin' =  I:'???'
Text Param 'Duration_FirstFrame/Strin' =  I:'???'
Text Param 'Duration_LastFrame' =  I:'???'
Text Param 'Duration_LastFrame/String' =  I:'???'
Text Param 'Duration_LastFrame/String' =  I:'???'
Text Param 'Duration_LastFrame/String' =  I:'???'
Text Param 'Duration_LastFrame/String' =  I:'???'
Text Param 'Duration_LastFrame/String' =  I:'???'
Text Param 'Duration_LastFrame/String' =  I:'???'
Text Param 'Source_Duration' =  I:'???'
Text Param 'Source_Duration/String' =  I:'???'
Text Param 'Source_Duration/String1' =  I:'???'
Text Param 'Source_Duration/String2' =  I:'???'
Text Param 'Source_Duration/String3' =  I:'???'
Text Param 'Source_Duration/String4' =  I:'???'
Text Param 'Source_Duration/String5' =  I:'???'
Text Param 'Source_Duration_FirstFram' =  I:'???'
Text Param 'Source_Duration_FirstFram' =  I:'???'
Text Param 'Source_Duration_FirstFram' =  I:'???'
Text Param 'Source_Duration_FirstFram' =  I:'???'
Text Param 'Source_Duration_FirstFram' =  I:'???'
Text Param 'Source_Duration_FirstFram' =  I:'???'
Text Param 'Source_Duration_FirstFram' =  I:'???'
Text Param 'Source_Duration_LastFrame' =  I:'???'
Text Param 'Source_Duration_LastFrame' =  I:'???'
Text Param 'Source_Duration_LastFrame' =  I:'???'
Text Param 'Source_Duration_LastFrame' =  I:'???'
Text Param 'Source_Duration_LastFrame' =  I:'???'
Text Param 'Source_Duration_LastFrame' =  I:'???'
Text Param 'Source_Duration_LastFrame' =  I:'???'
Text Param 'BitRate_Mode' =  I:'???'
Text Param 'BitRate_Mode/String' =  I:'???'
Text Param 'BitRate' =  I:'???'
Text Param 'BitRate/String' =  I:'???'
Text Param 'BitRate_Minimum' =  I:'???'
Text Param 'BitRate_Minimum/String' =  I:'???'
Text Param 'BitRate_Nominal' =  I:'???'
Text Param 'BitRate_Nominal/String' =  I:'???'
Text Param 'BitRate_Maximum' =  I:'???'
Text Param 'BitRate_Maximum/String' =  I:'???'
Text Param 'BitRate_Encoded' =  I:'???'
Text Param 'BitRate_Encoded/String' =  I:'???'
Text Param 'Width' =  I:'???'
Text Param (Width/String)
Text Param 'Height' =  I:'???'
Text Param (Height/String)
Text Param 'FrameRate_Mode' =  I:'???'
Text Param 'FrameRate_Mode/String' =  I:'???'
Text Param 'FrameRate' =  I:'???'
Text Param 'FrameRate/String' =  I:'???'
Text Param 'FrameRate_Num' =  I:'???'
Text Param 'FrameRate_Den' =  I:'???'
Text Param 'FrameRate_Minimum' =  I:'???'
Text Param 'FrameRate_Minimum/String' =  I:'???'
Text Param 'FrameRate_Nominal' =  I:'???'
Text Param 'FrameRate_Nominal/String' =  I:'???'
Text Param 'FrameRate_Maximum' =  I:'???'
Text Param 'FrameRate_Maximum/String' =  I:'???'
Text Param 'FrameRate_Original' =  I:'???'
Text Param 'FrameRate_Original/String' =  I:'???'
Text Param 'FrameCount' =  I:'???'
Text Param 'ElementCount' =  I:'???'
Text Param 'Source_FrameCount' =  I:'???'
Text Param (ColorSpace)
Text Param (ChromaSubsampling)
Text Param 'Resolution' =  I:'???'
Text Param 'Resolution/String' =  I:'???'
Text Param (BitDepth)
Text Param (BitDepth/String)
Text Param 'Compression_Mode' =  I:'???'
Text Param 'Compression_Mode/String' =  I:'???'
Text Param 'Compression_Ratio' =  I:'???'
Text Param 'Delay' =  I:'???'
Text Param 'Delay/String' =  I:'???'
Text Param 'Delay/String1' =  I:'???'
Text Param 'Delay/String2' =  I:'???'
Text Param 'Delay/String3' =  I:'???'
Text Param 'Delay/String4' =  I:'???'
Text Param 'Delay/String5' =  I:'???'
Text Param 'Delay_Settings' =  I:'???'
Text Param 'Delay_DropFrame' =  I:'???'
Text Param 'Delay_Source' =  I:'???'
Text Param 'Delay_Source/String' =  I:'???'
Text Param 'Delay_Original' =  I:'???'
Text Param 'Delay_Original/String' =  I:'???'
Text Param 'Delay_Original/String1' =  I:'???'
Text Param 'Delay_Original/String2' =  I:'???'
Text Param 'Delay_Original/String3' =  I:'???'
Text Param 'Delay_Original/String4' =  I:'???'
Text Param 'Delay_Original/String5' =  I:'???'
Text Param 'Delay_Original_Settings' =  I:'???'
Text Param 'Delay_Original_DropFrame' =  I:'???'
Text Param 'Delay_Original_Source' =  I:'???'
Text Param 'Video_Delay' =  I:'???'
Text Param (Video_Delay/String)
Text Param (Video_Delay/String1)
Text Param (Video_Delay/String2)
Text Param (Video_Delay/String3)
Text Param (Video_Delay/String4)
Text Param (Video_Delay/String5)
Text Param 'Video0_Delay' =  I:'???'
Text Param 'Video0_Delay/String' =  I:'???'
Text Param 'Video0_Delay/String1' =  I:'???'
Text Param 'Video0_Delay/String2' =  I:'???'
Text Param 'Video0_Delay/String3' =  I:'???'
Text Param 'Video0_Delay/String4' =  I:'???'
Text Param 'Video0_Delay/String5' =  I:'???'
Text Param 'StreamSize' =  I:'???'
Text Param 'StreamSize/String' =  I:'???'
Text Param (StreamSize/String1)
Text Param (StreamSize/String2)
Text Param (StreamSize/String3)
Text Param (StreamSize/String4)
Text Param 'StreamSize/String5' =  I:'???'
Text Param 'StreamSize_Proportion' =  I:'???'
Text Param 'StreamSize_Demuxed' =  I:'???'
Text Param 'StreamSize_Demuxed/String' =  I:'???'
Text Param (StreamSize_Demuxed/String1)
Text Param (StreamSize_Demuxed/String2)
Text Param (StreamSize_Demuxed/String3)
Text Param (StreamSize_Demuxed/String4)
Text Param 'StreamSize_Demuxed/String' =  I:'???'
Text Param 'Source_StreamSize' =  I:'???'
Text Param 'Source_StreamSize/String' =  I:'???'
Text Param (Source_StreamSize/String1)
Text Param (Source_StreamSize/String2)
Text Param (Source_StreamSize/String3)
Text Param (Source_StreamSize/String4)
Text Param 'Source_StreamSize/String5' =  I:'???'
Text Param 'Source_StreamSize_Proport' =  I:'???'
Text Param 'StreamSize_Encoded' =  I:'???'
Text Param 'StreamSize_Encoded/String' =  I:'???'
Text Param (StreamSize_Encoded/String1)
Text Param (StreamSize_Encoded/String2)
Text Param (StreamSize_Encoded/String3)
Text Param (StreamSize_Encoded/String4)
Text Param 'StreamSize_Encoded/String' =  I:'???'
Text Param 'StreamSize_Encoded_Propor' =  I:'???'
Text Param 'Source_StreamSize_Encoded' =  I:'???'
Text Param 'Source_StreamSize_Encoded' =  I:'???'
Text Param (Source_StreamSize_Encoded/String1)
Text Param (Source_StreamSize_Encoded/String2)
Text Param (Source_StreamSize_Encoded/String3)
Text Param (Source_StreamSize_Encoded/String4)
Text Param 'Source_StreamSize_Encoded' =  I:'???'
Text Param 'Source_StreamSize_Encoded' =  I:'???'
Text Param 'Title' =  I:'???'
Text Param 'Encoded_Application' =  I:'???'
Text Param 'Encoded_Application/Strin' =  I:'???'
Text Param 'Encoded_Application_Compa' =  I:'???'
Text Param 'Encoded_Application_Name' =  I:'???'
Text Param 'Encoded_Application_Versi' =  I:'???'
Text Param 'Encoded_Application_Url' =  I:'???'
Text Param 'Encoded_Library' =  I:'???'
Text Param 'Encoded_Library/String' =  I:'???'
Text Param 'Encoded_Library_CompanyNa' =  I:'???'
Text Param 'Encoded_Library_Name' =  I:'???'
Text Param 'Encoded_Library_Version' =  I:'???'
Text Param 'Encoded_Library_Date' =  I:'???'
Text Param 'Encoded_Library_Settings' =  I:'???'
Text Param 'Encoded_OperatingSystem' =  I:'???'
Text Param 'Language' =  I:'???'
Text Param 'Language/String' =  I:'???'
Text Param 'Language/String1' =  I:'???'
Text Param 'Language/String2' =  I:'???'
Text Param 'Language/String3' =  I:'???'
Text Param 'Language/String4' =  I:'???'
Text Param 'Language_More' =  I:'???'
Text Param 'ServiceKind' =  I:'???'
Text Param 'ServiceKind/String' =  I:'???'
Text Param 'Disabled' =  I:'???'
Text Param 'Disabled/String' =  I:'???'
Text Param 'Default' =  I:'???'
Text Param 'Default/String' =  I:'???'
Text Param 'Forced' =  I:'???'
Text Param 'Forced/String' =  I:'???'
Text Param 'AlternateGroup' =  I:'???'
Text Param 'AlternateGroup/String' =  I:'???'
Text Param (Summary)
Text Param 'Encoded_Date' =  I:'???'
Text Param 'Tagged_Date' =  I:'???'
Text Param (Encryption)
----->Other
Other Param 'Count' =  I:'???'
Other Param 'Status' =  I:'???'
Other Param 'StreamCount' =  I:'???'
Other Param 'StreamKind' =  I:'???'
Other Param 'StreamKind/String' =  I:'???'
Other Param 'StreamKindID' =  I:'???'
Other Param 'StreamKindPos' =  I:'???'
Other Param 'StreamOrder' =  I:'???'
Other Param 'FirstPacketOrder' =  I:'???'
Other Param 'Inform' =  I:'Inform'
Other Param 'ID' =  I:'???'
Other Param 'ID/String' =  I:'???'
Other Param 'OriginalSourceMedium_ID' =  I:'???'
Other Param 'OriginalSourceMedium_ID/S' =  I:'???'
Other Param 'UniqueID' =  I:'???'
Other Param 'UniqueID/String' =  I:'???'
Other Param 'MenuID' =  I:'???'
Other Param 'MenuID/String' =  I:'???'
Other Param 'Type' =  I:'???'
Other Param 'Format' =  I:'???'
Other Param 'Format/String' =  I:'???'
Other Param 'Format/Info' =  I:'???'
Other Param 'Format/Url' =  I:'???'
Other Param 'Format_Commercial' =  I:'???'
Other Param 'Format_Commercial_IfAny' =  I:'???'
Other Param 'Format_Version' =  I:'???'
Other Param 'Format_Profile' =  I:'???'
Other Param 'Format_Compression' =  I:'???'
Other Param 'Format_Settings' =  I:'???'
Other Param 'Format_AdditionalFeatures' =  I:'???'
Other Param 'MuxingMode' =  I:'???'
Other Param 'CodecID' =  I:'???'
Other Param 'CodecID/String' =  I:'???'
Other Param 'CodecID/Info' =  I:'???'
Other Param 'CodecID/Hint' =  I:'???'
Other Param 'CodecID/Url' =  I:'???'
Other Param 'CodecID_Description' =  I:'???'
Other Param 'Duration' =  I:'???'
Other Param 'Duration/String' =  I:'???'
Other Param 'Duration/String1' =  I:'???'
Other Param 'Duration/String2' =  I:'???'
Other Param 'Duration/String3' =  I:'???'
Other Param 'Duration/String4' =  I:'???'
Other Param 'Duration/String5' =  I:'???'
Other Param (Duration_Start)
Other Param (Duration_End)
Other Param 'Source_Duration' =  I:'???'
Other Param 'Source_Duration/String' =  I:'???'
Other Param 'Source_Duration/String1' =  I:'???'
Other Param 'Source_Duration/String2' =  I:'???'
Other Param 'Source_Duration/String3' =  I:'???'
Other Param 'Source_Duration/String4' =  I:'???'
Other Param 'Source_Duration/String5' =  I:'???'
Other Param 'Source_Duration_FirstFram' =  I:'???'
Other Param 'Source_Duration_FirstFram' =  I:'???'
Other Param 'Source_Duration_FirstFram' =  I:'???'
Other Param 'Source_Duration_FirstFram' =  I:'???'
Other Param 'Source_Duration_FirstFram' =  I:'???'
Other Param 'Source_Duration_FirstFram' =  I:'???'
Other Param 'Source_Duration_FirstFram' =  I:'???'
Other Param 'Source_Duration_LastFrame' =  I:'???'
Other Param 'Source_Duration_LastFrame' =  I:'???'
Other Param 'Source_Duration_LastFrame' =  I:'???'
Other Param 'Source_Duration_LastFrame' =  I:'???'
Other Param 'Source_Duration_LastFrame' =  I:'???'
Other Param 'Source_Duration_LastFrame' =  I:'???'
Other Param 'Source_Duration_LastFrame' =  I:'???'
Other Param 'BitRate_Mode' =  I:'???'
Other Param 'BitRate_Mode/String' =  I:'???'
Other Param 'BitRate' =  I:'???'
Other Param 'BitRate/String' =  I:'???'
Other Param 'BitRate_Minimum' =  I:'???'
Other Param 'BitRate_Minimum/String' =  I:'???'
Other Param 'BitRate_Nominal' =  I:'???'
Other Param 'BitRate_Nominal/String' =  I:'???'
Other Param 'BitRate_Maximum' =  I:'???'
Other Param 'BitRate_Maximum/String' =  I:'???'
Other Param 'BitRate_Encoded' =  I:'???'
Other Param 'BitRate_Encoded/String' =  I:'???'
Other Param 'FrameRate' =  I:'???'
Other Param 'FrameRate/String' =  I:'???'
Other Param 'FrameRate_Num' =  I:'???'
Other Param 'FrameRate_Den' =  I:'???'
Other Param 'FrameCount' =  I:'???'
Other Param 'Source_FrameCount' =  I:'???'
Other Param 'Delay' =  I:'???'
Other Param 'Delay/String' =  I:'???'
Other Param 'Delay/String1' =  I:'???'
Other Param 'Delay/String2' =  I:'???'
Other Param 'Delay/String3' =  I:'???'
Other Param 'Delay/String4' =  I:'???'
Other Param 'Delay/String5' =  I:'???'
Other Param 'Delay_Settings' =  I:'???'
Other Param 'Delay_DropFrame' =  I:'???'
Other Param 'Delay_Source' =  I:'???'
Other Param 'Delay_Source/String' =  I:'???'
Other Param 'Delay_Original' =  I:'???'
Other Param 'Delay_Original/String' =  I:'???'
Other Param 'Delay_Original/String1' =  I:'???'
Other Param 'Delay_Original/String2' =  I:'???'
Other Param 'Delay_Original/String3' =  I:'???'
Other Param 'Delay_Original/String4' =  I:'???'
Other Param 'Delay_Original/String5' =  I:'???'
Other Param 'Delay_Original_Settings' =  I:'???'
Other Param 'Delay_Original_DropFrame' =  I:'???'
Other Param 'Delay_Original_Source' =  I:'???'
Other Param 'Video_Delay' =  I:'???'
Other Param (Video_Delay/String)
Other Param (Video_Delay/String1)
Other Param (Video_Delay/String2)
Other Param (Video_Delay/String3)
Other Param (Video_Delay/String4)
Other Param (Video_Delay/String5)
Other Param 'Video0_Delay' =  I:'???'
Other Param 'Video0_Delay/String' =  I:'???'
Other Param 'Video0_Delay/String1' =  I:'???'
Other Param 'Video0_Delay/String2' =  I:'???'
Other Param 'Video0_Delay/String3' =  I:'???'
Other Param 'Video0_Delay/String4' =  I:'???'
Other Param 'Video0_Delay/String5' =  I:'???'
Other Param 'TimeStamp_FirstFrame' =  I:'???'
Other Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Other Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Other Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Other Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Other Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Other Param 'TimeStamp_FirstFrame/Stri' =  I:'???'
Other Param 'TimeCode_FirstFrame' =  I:'???'
Other Param 'TimeCode_Settings' =  I:'???'
Other Param 'TimeCode_Striped' =  I:'???'
Other Param 'TimeCode_Striped/String' =  I:'???'
Other Param 'StreamSize' =  I:'???'
Other Param 'StreamSize/String' =  I:'???'
Other Param (StreamSize/String1)
Other Param (StreamSize/String2)
Other Param (StreamSize/String3)
Other Param (StreamSize/String4)
Other Param 'StreamSize/String5' =  I:'???'
Other Param 'StreamSize_Proportion' =  I:'???'
Other Param 'StreamSize_Demuxed' =  I:'???'
Other Param 'StreamSize_Demuxed/String' =  I:'???'
Other Param (StreamSize_Demuxed/String1)
Other Param (StreamSize_Demuxed/String2)
Other Param (StreamSize_Demuxed/String3)
Other Param (StreamSize_Demuxed/String4)
Other Param 'StreamSize_Demuxed/String' =  I:'???'
Other Param 'Source_StreamSize' =  I:'???'
Other Param 'Source_StreamSize/String' =  I:'???'
Other Param (Source_StreamSize/String1)
Other Param (Source_StreamSize/String2)
Other Param (Source_StreamSize/String3)
Other Param (Source_StreamSize/String4)
Other Param 'Source_StreamSize/String5' =  I:'???'
Other Param 'Source_StreamSize_Proport' =  I:'???'
Other Param 'StreamSize_Encoded' =  I:'???'
Other Param 'StreamSize_Encoded/String' =  I:'???'
Other Param (StreamSize_Encoded/String1)
Other Param (StreamSize_Encoded/String2)
Other Param (StreamSize_Encoded/String3)
Other Param (StreamSize_Encoded/String4)
Other Param 'StreamSize_Encoded/String' =  I:'???'
Other Param 'StreamSize_Encoded_Propor' =  I:'???'
Other Param 'Source_StreamSize_Encoded' =  I:'???'
Other Param 'Source_StreamSize_Encoded' =  I:'???'
Other Param (Source_StreamSize_Encoded/String1)
Other Param (Source_StreamSize_Encoded/String2)
Other Param (Source_StreamSize_Encoded/String3)
Other Param (Source_StreamSize_Encoded/String4)
Other Param 'Source_StreamSize_Encoded' =  I:'???'
Other Param 'Source_StreamSize_Encoded' =  I:'???'
Other Param 'Title' =  I:'???'
Other Param 'Language' =  I:'???'
Other Param 'Language/String' =  I:'???'
Other Param 'Language/String1' =  I:'???'
Other Param 'Language/String2' =  I:'???'
Other Param 'Language/String3' =  I:'???'
Other Param 'Language/String4' =  I:'???'
Other Param 'Language_More' =  I:'???'
Other Param 'ServiceKind' =  I:'???'
Other Param 'ServiceKind/String' =  I:'???'
Other Param 'Disabled' =  I:'???'
Other Param 'Disabled/String' =  I:'???'
Other Param 'Default' =  I:'???'
Other Param 'Default/String' =  I:'???'
Other Param 'Forced' =  I:'???'
Other Param 'Forced/String' =  I:'???'
Other Param 'AlternateGroup' =  I:'???'
Other Param 'AlternateGroup/String' =  I:'???'
----->Image
Image Param 'Count' =  I:'???'
Image Param 'Status' =  I:'???'
Image Param 'StreamCount' =  I:'???'
Image Param 'StreamKind' =  I:'???'
Image Param 'StreamKind/String' =  I:'???'
Image Param 'StreamKindID' =  I:'???'
Image Param 'StreamKindPos' =  I:'???'
Image Param 'StreamOrder' =  I:'???'
Image Param 'FirstPacketOrder' =  I:'???'
Image Param 'Inform' =  I:'Inform'
Image Param 'ID' =  I:'???'
Image Param 'ID/String' =  I:'???'
Image Param 'OriginalSourceMedium_ID' =  I:'???'
Image Param 'OriginalSourceMedium_ID/S' =  I:'???'
Image Param 'UniqueID' =  I:'???'
Image Param 'UniqueID/String' =  I:'???'
Image Param 'MenuID' =  I:'???'
Image Param 'MenuID/String' =  I:'???'
Image Param 'Title' =  I:'???'
Image Param 'Format' =  I:'???'
Image Param 'Format/String' =  I:'???'
Image Param 'Format/Info' =  I:'???'
Image Param 'Format/Url' =  I:'???'
Image Param 'Format_Commercial' =  I:'???'
Image Param 'Format_Commercial_IfAny' =  I:'???'
Image Param 'Format_Version' =  I:'???'
Image Param 'Format_Profile' =  I:'???'
Image Param (Format_Settings_Endianness)
Image Param (Format_Settings_Packing)
Image Param 'Format_Compression' =  I:'???'
Image Param 'Format_Settings' =  I:'???'
Image Param 'Format_Settings_Wrapping' =  I:'???'
Image Param 'Format_AdditionalFeatures' =  I:'???'
Image Param 'InternetMediaType' =  I:'???'
Image Param 'CodecID' =  I:'???'
Image Param 'CodecID/String' =  I:'???'
Image Param 'CodecID/Info' =  I:'???'
Image Param 'CodecID/Hint' =  I:'???'
Image Param 'CodecID/Url' =  I:'???'
Image Param 'CodecID_Description' =  I:'???'
Image Param 'Codec' =  I:'???'
Image Param 'Codec/String' =  I:'???'
Image Param 'Codec/Family' =  I:'???'
Image Param 'Codec/Info' =  I:'???'
Image Param 'Codec/Url' =  I:'???'
Image Param 'Width' =  I:'???'
Image Param 'Width/String' =  I:'???'
Image Param 'Width_Offset' =  I:'???'
Image Param 'Width_Offset/String' =  I:'???'
Image Param 'Width_Original' =  I:'???'
Image Param 'Width_Original/String' =  I:'???'
Image Param 'Height' =  I:'???'
Image Param 'Height/String' =  I:'???'
Image Param 'Height_Offset' =  I:'???'
Image Param 'Height_Offset/String' =  I:'???'
Image Param 'Height_Original' =  I:'???'
Image Param 'Height_Original/String' =  I:'???'
Image Param 'PixelAspectRatio' =  I:'???'
Image Param 'PixelAspectRatio/String' =  I:'???'
Image Param 'PixelAspectRatio_Original' =  I:'???'
Image Param 'PixelAspectRatio_Original' =  I:'???'
Image Param 'DisplayAspectRatio' =  I:'???'
Image Param 'DisplayAspectRatio/String' =  I:'???'
Image Param 'DisplayAspectRatio_Origin' =  I:'???'
Image Param 'DisplayAspectRatio_Origin' =  I:'???'
Image Param (ColorSpace)
Image Param (ChromaSubsampling)
Image Param 'Resolution' =  I:'???'
Image Param 'Resolution/String' =  I:'???'
Image Param (BitDepth)
Image Param (BitDepth/String)
Image Param 'Compression_Mode' =  I:'???'
Image Param 'Compression_Mode/String' =  I:'???'
Image Param 'Compression_Ratio' =  I:'???'
Image Param 'StreamSize' =  I:'???'
Image Param (StreamSize/String)
Image Param (StreamSize/String1)
Image Param (StreamSize/String2)
Image Param (StreamSize/String3)
Image Param (StreamSize/String4)
Image Param 'StreamSize/String5' =  I:'???'
Image Param 'StreamSize_Proportion' =  I:'???'
Image Param 'StreamSize_Demuxed' =  I:'???'
Image Param 'StreamSize_Demuxed/String' =  I:'???'
Image Param (StreamSize_Demuxed/String1)
Image Param (StreamSize_Demuxed/String2)
Image Param (StreamSize_Demuxed/String3)
Image Param (StreamSize_Demuxed/String4)
Image Param 'StreamSize_Demuxed/String' =  I:'???'
Image Param 'Encoded_Library' =  I:'???'
Image Param 'Encoded_Library/String' =  I:'???'
Image Param 'Encoded_Library_Name' =  I:'???'
Image Param 'Encoded_Library_Version' =  I:'???'
Image Param 'Encoded_Library_Date' =  I:'???'
Image Param 'Encoded_Library_Settings' =  I:'???'
Image Param 'Language' =  I:'???'
Image Param 'Language/String' =  I:'???'
Image Param 'Language/String1' =  I:'???'
Image Param 'Language/String2' =  I:'???'
Image Param 'Language/String3' =  I:'???'
Image Param 'Language/String4' =  I:'???'
Image Param 'Language_More' =  I:'???'
Image Param 'ServiceKind' =  I:'???'
Image Param 'ServiceKind/String' =  I:'???'
Image Param 'Disabled' =  I:'???'
Image Param 'Disabled/String' =  I:'???'
Image Param 'Default' =  I:'???'
Image Param 'Default/String' =  I:'???'
Image Param 'Forced' =  I:'???'
Image Param 'Forced/String' =  I:'???'
Image Param 'AlternateGroup' =  I:'???'
Image Param 'AlternateGroup/String' =  I:'???'
Image Param (Summary)
Image Param 'Encoded_Date' =  I:'???'
Image Param 'Tagged_Date' =  I:'???'
Image Param (Encryption)
Image Param 'colour_description_presen' =  I:'???'
Image Param 'colour_primaries' =  I:'???'
Image Param 'transfer_characteristics' =  I:'???'
Image Param 'matrix_coefficients' =  I:'???'
Image Param 'colour_description_presen' =  I:'???'
Image Param 'colour_primaries_Original' =  I:'???'
Image Param 'transfer_characteristics_' =  I:'???'
Image Param 'matrix_coefficients_Origi' =  I:'???'
----->Menu
Menu Param 'Count' =  I:'???'
Menu Param 'Status' =  I:'???'
Menu Param 'StreamCount' =  I:'???'
Menu Param 'StreamKind' =  I:'???'
Menu Param 'StreamKind/String' =  I:'???'
Menu Param 'StreamKindID' =  I:'???'
Menu Param 'StreamKindPos' =  I:'???'
Menu Param 'StreamOrder' =  I:'???'
Menu Param 'FirstPacketOrder' =  I:'???'
Menu Param 'Inform' =  I:'Inform'
Menu Param 'ID' =  I:'???'
Menu Param 'ID/String' =  I:'???'
Menu Param 'OriginalSourceMedium_ID' =  I:'???'
Menu Param 'OriginalSourceMedium_ID/S' =  I:'???'
Menu Param 'UniqueID' =  I:'???'
Menu Param 'UniqueID/String' =  I:'???'
Menu Param 'MenuID' =  I:'???'
Menu Param 'MenuID/String' =  I:'???'
Menu Param 'Format' =  I:'???'
Menu Param 'Format/String' =  I:'???'
Menu Param 'Format/Info' =  I:'???'
Menu Param 'Format/Url' =  I:'???'
Menu Param 'Format_Commercial' =  I:'???'
Menu Param 'Format_Commercial_IfAny' =  I:'???'
Menu Param 'Format_Version' =  I:'???'
Menu Param 'Format_Profile' =  I:'???'
Menu Param 'Format_Compression' =  I:'???'
Menu Param 'Format_Settings' =  I:'???'
Menu Param 'Format_AdditionalFeatures' =  I:'???'
Menu Param 'CodecID' =  I:'???'
Menu Param 'CodecID/String' =  I:'???'
Menu Param 'CodecID/Info' =  I:'???'
Menu Param 'CodecID/Hint' =  I:'???'
Menu Param 'CodecID/Url' =  I:'???'
Menu Param 'CodecID_Description' =  I:'???'
Menu Param 'Codec' =  I:'???'
Menu Param 'Codec/String' =  I:'???'
Menu Param 'Codec/Info' =  I:'???'
Menu Param 'Codec/Url' =  I:'???'
Menu Param 'Duration' =  I:'???'
Menu Param 'Duration/String' =  I:'???'
Menu Param 'Duration/String1' =  I:'???'
Menu Param 'Duration/String2' =  I:'???'
Menu Param 'Duration/String3' =  I:'???'
Menu Param 'Duration/String4' =  I:'???'
Menu Param 'Duration/String5' =  I:'???'
Menu Param (Duration_Start)
Menu Param (Duration_End)
Menu Param 'Delay' =  I:'???'
Menu Param 'Delay/String' =  I:'???'
Menu Param 'Delay/String1' =  I:'???'
Menu Param 'Delay/String2' =  I:'???'
Menu Param 'Delay/String3' =  I:'???'
Menu Param 'Delay/String4' =  I:'???'
Menu Param 'Delay/String5' =  I:'???'
Menu Param 'Delay_Settings' =  I:'???'
Menu Param 'Delay_DropFrame' =  I:'???'
Menu Param 'Delay_Source' =  I:'???'
Menu Param 'List_StreamKind' =  I:'???'
Menu Param 'List_StreamPos' =  I:'???'
Menu Param 'List' =  I:'???'
Menu Param 'List/String' =  I:'???'
Menu Param 'Title' =  I:'???'
Menu Param 'Language' =  I:'???'
Menu Param 'Language/String' =  I:'???'
Menu Param 'Language/String1' =  I:'???'
Menu Param 'Language/String2' =  I:'???'
Menu Param 'Language/String3' =  I:'???'
Menu Param 'Language/String4' =  I:'???'
Menu Param 'Language_More' =  I:'???'
Menu Param 'ServiceKind' =  I:'???'
Menu Param 'ServiceKind/String' =  I:'???'
Menu Param (ServiceName)
Menu Param (ServiceChannel)
Menu Param (Service/Url)
Menu Param (ServiceProvider)
Menu Param (ServiceProvider/Url)
Menu Param (ServiceType)
Menu Param (NetworkName)
Menu Param (Original/NetworkName)
Menu Param (Countries)
Menu Param (TimeZones)
Menu Param 'LawRating' =  I:'???'
Menu Param 'LawRating_Reason' =  I:'???'
Menu Param 'Disabled' =  I:'???'
Menu Param 'Disabled/String' =  I:'???'
Menu Param 'Default' =  I:'???'
Menu Param 'Default/String' =  I:'???'
Menu Param 'Forced' =  I:'???'
Menu Param 'Forced/String' =  I:'???'
Menu Param 'AlternateGroup' =  I:'???'
Menu Param 'AlternateGroup/String' =  I:'???'
Menu Param 'Chapters_Pos_Begin' =  I:'???'
[       OK ] Bof2d_Test.MediaDetectorParam (13017 ms)
[----------] 1 test from Bof2d_Test (13017 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (13020 ms total)
[  PASSED  ] 1 test.

Press any key followed by enter to to quit ...

*/
