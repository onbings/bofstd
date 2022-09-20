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


//https://fossies.org/linux/MediaInfo_CLI/MediaInfoLib/Source/Example/HowToUse_Dll.cpp
/*
[==========] Running 3 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 2 tests from muse_service_test
[ RUN      ] muse_service_test.get_version
[       OK ] muse_service_test.get_version (20 ms)
[ RUN      ] muse_service_test.MountStorage
[info][muse_storage_service_api.cpp:54] _rStorageParam_X: StorageMountParam(IpEndpoint_S=, TimeoutInMs_S32=0)
[       OK ] muse_service_test.MountStorage (5 ms)
[----------] 2 tests from muse_service_test (25 ms total)

[----------] 1 test from Muse_Storage_FileSystem_Test
[ RUN      ] Muse_Storage_FileSystem_Test.MediaInfo
MediaInfoLib - v21.03

Info_Parameters
General
Count                     : Count of objects available in this stream
Status                    : Status of bit field (0=IsAccepted, 1=IsFilled, 2=IsUpdated, 3=IsFinished)
StreamCount               : Count of streams of this kind available (base=1)
StreamKind                : Stream type name
StreamKind/String         : Stream type name
StreamKindID              : Stream number (base=0)
StreamKindPos             : Number of the stream, when multiple (base=1)
StreamOrder               : Stream order in the file for type of stream (base=0)
FirstPacketOrder          : Order of the first fully decodable packet met in the file for stream type (base=0)
Inform                    : Last **Inform** call
ID                        : The ID for this stream in this file
ID/String                 : The ID for this stream in this file
OriginalSourceMedium_ID   : The ID for this stream in the original medium of the material
OriginalSourceMedium_ID/S : The ID for this stream in the original medium of the material
UniqueID                  : The unique ID for this stream, should be copied with stream copy
UniqueID/String           : The unique ID for this stream, should be copied with stream copy
MenuID                    : The menu ID for this stream in this file
MenuID/String             : The menu ID for this stream in this file
GeneralCount              : Number of general streams
VideoCount                : Number of video streams
AudioCount                : Number of audio streams
TextCount                 : Number of text streams
OtherCount                : Number of other streams
ImageCount                : Number of image streams
MenuCount                 : Number of menu streams
Video_Format_List         : Video Codecs in this file, separated by /
Video_Format_WithHint_Lis : Video Codecs in this file with popular name (hint), separated by /
Video_Codec_List          : Deprecated, do not use in new projects
Video_Language_List       : Video languages in this file, full names, separated by /
Audio_Format_List         : Audio Codecs in this file,separated by /
Audio_Format_WithHint_Lis : Audio Codecs in this file with popular name (hint), separated by /
Audio_Codec_List          : Deprecated, do not use in new projects
Audio_Language_List       : Audio languages in this file separated by /
Text_Format_List          : Text Codecs in this file, separated by /
Text_Format_WithHint_List : Text Codecs in this file with popular name (hint),separated by /
Text_Codec_List           : Deprecated, do not use in new projects
Text_Language_List        : Text languages in this file, separated by /
Other_Format_List         : Other formats in this file, separated by /
Other_Format_WithHint_Lis : Other formats in this file with popular name (hint), separated by /
Other_Codec_List          : Deprecated, do not use in new projects
Other_Language_List       : Chapters languages in this file, separated by /
Image_Format_List         : Image Codecs in this file, separated by /
Image_Format_WithHint_Lis : Image Codecs in this file with popular name (hint), separated by /
Image_Codec_List          : Deprecated, do not use in new projects
Image_Language_List       : Image languages in this file, separated by /
Menu_Format_List          : Menu Codecs in this file, separated by /
Menu_Format_WithHint_List : Menu Codecs in this file with popular name (hint),separated by /
Menu_Codec_List           : Deprecated, do not use in new projects
Menu_Language_List        : Menu languages in this file, separated by /
CompleteName              : Complete name (Folder+Name+Extension)
FolderName                : Folder name only
FileNameExtension         : File name and extension
FileName                  : File name only
FileExtension             : File extension only
CompleteName_Last         : Complete name (Folder+Name+Extension) of the last file (in the case of a sequence of files)
FolderName_Last           : Folder name only of the last file (in the case of a sequence of files)
FileNameExtension_Last    : File name and extension of the last file (in the case of a sequence of files)
FileName_Last             : File name only of the last file (in the case of a sequence of files)
FileExtension_Last        : File extension only of the last file (in the case of a sequence of files)
Format                    : Format used
Format/String             : Format used + additional features
Format/Info               : Info about this Format
Format/Url                : Link to a description of this format
Format/Extensions         : Known extensions of this format
Format_Commercial         : Commercial name used by vendor for these settings or Format field if there is no difference
Format_Commercial_IfAny   : Commercial name used by vendor for these settings if there is one
Format_Version            : Version of this format
Format_Profile            : Profile of the Format (old XML: 'Profile@Level' format
Format_Level              : Level of the Format (only MIXML)
Format_Compression        : Compression method used
Format_Settings           : Settings needed for decoder used
Format_AdditionalFeatures : Format features needed for fully supporting the content
InternetMediaType         : Internet Media Type (aka MIME Type, Content-Type)
CodecID                   : Codec ID (found in some containers)
CodecID/String            : Codec ID (found in some containers)
CodecID/Info              : Info about this Codec
CodecID/Hint              : A hint/popular name for this Codec
CodecID/Url               : A link to more details about this Codec ID
CodecID_Description       : Manual description given by the container
CodecID_Version           : Version of the CodecID
CodecID_Compatible        : Compatible CodecIDs
Interleaved               : If Audio and video are muxed
Codec                     : Deprecated, do not use in new projects
Codec/String              : Deprecated, do not use in new projects
Codec/Info                : Deprecated, do not use in new projects
Codec/Url                 : Deprecated, do not use in new projects
Codec/Extensions          : Deprecated, do not use in new projects
Codec_Settings            : Deprecated, do not use in new projects
Codec_Settings_Automatic  : Deprecated, do not use in new projects
FileSize                  : File size in bytes
FileSize/String           : File size (with measure)
FileSize/String1          : File size (with measure, 1 digit mini)
FileSize/String2          : File size (with measure, 2 digit mini)
FileSize/String3          : File size (with measure, 3 digit mini)
FileSize/String4          : File size (with measure, 4 digit mini)
Duration                  : Play time of the stream (in ms)
Duration/String           : Play time in format : XXx YYy only, YYy omitted if zero
Duration/String1          : Play time in format : HHh MMmn SSs MMMms, XX omitted if zero
Duration/String2          : Play time in format : XXx YYy only, YYy omitted if zero
Duration/String3          : Play time in format : HH:MM:SS.MMM
Duration/String4          : Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Duration/String5          : Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Duration_Start
Duration_End
OverallBitRate_Mode       : Bit rate mode of all streams (VBR, CBR)
OverallBitRate_Mode/Strin : Bit rate mode of all streams (Variable, Constant)
OverallBitRate            : Bit rate of all streams (in bps)
OverallBitRate/String     : Bit rate of all streams (with measure)
OverallBitRate_Minimum    : Minimum bit rate (in bps)
OverallBitRate_Minimum/St : Minimum bit rate (with measurement)
OverallBitRate_Nominal    : Nominal bit rate (in bps)
OverallBitRate_Nominal/St : Nominal bit rate (with measurement)
OverallBitRate_Maximum    : Maximum bit rate (in bps)
OverallBitRate_Maximum/St : Maximum bit rate (with measurement)
FrameRate                 : Frames per second
FrameRate/String          : Frames per second (with measurement)
FrameRate_Num             : Frames per second, numerator
FrameRate_Den             : Frames per second, denominator
FrameCount                : Frame count (if a frame contains a count of samples)
Delay                     : Delay fixed in the stream (relative) (in ms)
Delay/String              : Delay with measurement
Delay/String1             : Delay with measurement
Delay/String2             : Delay with measurement
Delay/String3             : format : HH:MM:SS.MMM
Delay/String4             : Delay in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Delay/String5             : Delay in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Delay_Settings            : Delay settings (in case of timecode, for example)
Delay_DropFrame           : Delay drop frame
Delay_Source              : Delay source (Container, Stream, empty)
Delay_Source/String       : Delay source (Container, Stream, empty)
StreamSize                : Stream size (in bytes)
StreamSize/String
StreamSize/String1
StreamSize/String2
StreamSize/String3
StreamSize/String4
StreamSize/String5        : Stream size with proportion
StreamSize_Proportion     : Stream size divided by file size
StreamSize_Demuxed        : StreamSize after demux (in bytes)
StreamSize_Demuxed/String : StreamSize_Demuxed with percentage value
StreamSize_Demuxed/String1
StreamSize_Demuxed/String2
StreamSize_Demuxed/String3
StreamSize_Demuxed/String4
StreamSize_Demuxed/String : StreamSize_Demuxed with percentage value (note: theoretical value, not for real use)
HeaderSize
DataSize
FooterSize
IsStreamable
Album_ReplayGain_Gain     : The gain to apply to reach 89dB SPL on playback
Album_ReplayGain_Gain/String
Album_ReplayGain_Peak     : The maximum absolute peak value of the item
Encryption
Encryption_Format
Encryption_Length
Encryption_Method
Encryption_Mode
Encryption_Padding
Encryption_InitializationVector
UniversalAdID/String      : Universal Ad-ID, see https://ad-id.org
UniversalAdID_Registry    : Universal Ad-ID registry
UniversalAdID_Value       : Universal Ad-ID value
Title                     : (Generic)Title of file
Title_More                : (Generic)More info about the title of file
Title/Url                 : (Generic)Url
Domain                    : Universe movies belong to, e.g. Star Wars, Stargate, Buffy, Dragonball
Collection                : Name of the series, e.g. Star Wars movies, Stargate SG-1, Stargate Atlantis, Buffy, Angel
Season                    : Name of the season, e.g. Star Wars first Trilogy, Season 1
Season_Position           : Number of the Season
Season_Position_Total     : Place of the season, e.g. 2 of 7
Movie                     : Name of the movie. Eg : Star Wars, A New Hope
Movie_More                : More info about the movie
Movie/Country             : Country, where the movie was produced
Movie/Url                 : Homepage for the movie
Album                     : Name of an audio-album. Eg : The Joshua Tree
Album_More                : More info about the album
Album/Sort
Album/Performer           : Album performer/artist of this file
Album/Performer/Sort
Album/Performer/Url       : Homepage of the album performer/artist
Comic                     : Name of the comic.
Comic_More                : More info about the comic
Comic/Position_Total      : Place of the comic, e.g. 1 of 10
Part                      : Name of the part. e.g. CD1, CD2
Part/Position             : Number of the part
Part/Position_Total       : Place of the part e.g. 2 of 3
Track                     : Name of the track. e.g. track 1, track 2
Track_More                : More info about the track
Track/Url                 : Link to a site about this track
Track/Sort
Track/Position            : Number of this track
Track/Position_Total      : Place of this track, e.g. 3 of 15
PackageName               : Package name i.e. technical flavor of the content
Grouping                  : iTunes grouping
Chapter                   : Name of the chapter
SubTrack                  : Name of the subtrack
Original/Album            : Original name of the album
Original/Movie            : Original name of the movie
Original/Part             : Original name of the part
Original/Track            : Original name of the track
Compilation               : iTunes compilation
Compilation/String        : iTunes compilation
Performer                 : Main performer(s)/artist(s)
Performer/Sort
Performer/Url             : Homepage of the performer/artist
Original/Performer        : Original artist(s)/performer(s)
Accompaniment             : Band/orchestra/accompaniment/musician
Composer                  : Name of the original composer
Composer/Nationality      : Nationality of the primary composer of the piece (mostly for classical music)
Composer/Sort
Arranger                  : The person who arranged the piece (e.g. Ravel)
Lyricist                  : The person who wrote the lyrics for the piece
Original/Lyricist         : Original lyricist(s)/text writer(s).
Conductor                 : The artist(s) who performed the work. In classical music this would be the conductor, orchestra, soloists, etc.
Director                  : Name of the director
CoDirector                : Name of the codirector
AssistantDirector         : Name of the assistant director
DirectorOfPhotography     : Name of the director of photography, also known as cinematographer
SoundEngineer             : Name of the sound engineer or sound recordist
ArtDirector               : Name of the person who oversees the artists and craftspeople who build the sets
ProductionDesigner        : Name of the person responsible for designing the overall visual appearance of a movie
Choreographer             : Name of the choreographer
CostumeDesigner           : Name of the costume designer
Actor                     : Real name of an actor/actress playing a role in the movie
Actor_Character           : Name of the character an actor or actress plays in this movie
WrittenBy                 : Author of the story or script
ScreenplayBy              : Author of the screenplay or scenario (used for movies and TV shows)
EditedBy                  : Editors name
CommissionedBy            : Name of the person or organization that commissioned the subject of the file
Producer                  : Name of the producer of the movie
CoProducer                : Name of a co-producer
ExecutiveProducer         : Name of an executive producer
MusicBy                   : Main musical artist for a movie
DistributedBy             : Company responsible for distribution of the content
OriginalSourceForm/Distri : Name of the person or organization who supplied the original subject
MasteredBy                : The engineer who mastered the content for a physical medium or for digital distribution
EncodedBy                 : Name of the person/organisation that encoded/ripped the audio file.
RemixedBy                 : Name of the artist(s) that interpreted, remixed, or otherwise modified the content
ProductionStudio          : Main production studio
ThanksTo                  : A very general tag for everyone else that wants to be listed
Publisher                 : Name of the organization publishing the album (i.e. the 'record label') or movie
Publisher/URL             : Publisher's official webpage
Label                     : Brand or trademark associated with the marketing of music recordings and music videos
Genre                     : Main genre of the audio or video. e.g. classical, ambient-house, synthpop, sci-fi, drama, etc.
PodcastCategory           : Podcast category
Mood                      : Intended to reflect the mood of the item with a few keywords, e.g. Romantic, Sad, Uplifting, etc.
ContentType               : The type or genre of the content. e.g. Documentary, Feature Film, Cartoon, Music Video, Music, Sound FX, etc.
Subject                   : Describes the topic of the file, such as 'Aerial view of Seattle.'
Description               : A short description of the contents, such as 'Two birds flying.'
Keywords                  : Keywords for the content separated by a comma, used for searching
Summary                   : Plot outline or a summary of the story
Synopsis                  : Description of the story line of the item
Period                    : Describes the period that the piece is from or about. e.g. Renaissance.
LawRating                 : Legal rating of a movie. Format depends on country of origin (e.g. PG or R in the USA, an age in other countries or a URI defining a logo)
LawRating_Reason          : Reason for the law rating
ICRA                      : The ICRA rating (previously RSACi)
Released_Date             : Date/year that the content was released
Original/Released_Date    : Date/year that the content was originally released
Recorded_Date             : Time/date/year that the recording began
Encoded_Date              : Time/date/year that the encoding of this content was completed
Tagged_Date               : Time/date/year that the tags were added to this content
Written_Date              : Time/date/year that the composition of the music/script began
Mastered_Date             : Time/date/year that the content was transferred to a digital medium.
File_Created_Date         : Time that the file was created on the file system
File_Created_Date_Local   : Time that the file was created on the file system (Warning: this field depends of local configuration, do not use it in an international database)
File_Modified_Date        : Time that the file was last modified on the file system
File_Modified_Date_Local  : Time that the file was last modified on the file system (Warning: this field depends of local configuration, do not use it in an international database)
Recorded_Location         : Location where track was recorded (See COMPOSITION_LOCATION for format)
Written_Location          : Location that the item was originally designed/written. Information should be stored in the following format: country code, state/province, city where the country code is the same 2 octets as in Internet domains, or possibly ISO-3166. e.g. US, Texas, Austin or US, , Austin.
Archival_Location         : Location where an item is archived (e.g. Louvre, Paris, France)
Encoded_Application       : Name of the software package used to create the file, such as Microsoft WaveEdiTY
Encoded_Application/Strin : Name of the software package used to create the file, such as Microsoft WaveEdit, trying to have the format 'CompanyName ProductName (OperatingSystem) Version (Date)'
Encoded_Application_Compa : Name of the company of the encoding application
Encoded_Application_Name  : Name of the encoding product
Encoded_Application_Versi : Version of the encoding product
Encoded_Application_Url   : URL associated with the encoding software
Encoded_Library           : Software used to create the file
Encoded_Library/String    : Software used to create the file, trying to have the format 'CompanyName ProductName (OperatingSystem) Version (Date)'
Encoded_Library_CompanyNa : Name of the company
Encoded_Library_Name      : Name of the the encoding library
Encoded_Library_Version   : Version of encoding library
Encoded_Library_Date      : Release date of encoding library
Encoded_Library_Settings  : Parameters used by the encoding library
Encoded_OperatingSystem   : Operating System used by encoding application
Cropped                   : Describes whether an image has been cropped and, if so, how it was cropped
Dimensions                : Specifies the size of the original subject of the file (e.g. 8.5 in h, 11 in w)
DotsPerInch               : Stores dots per inch setting of the digitization mechanism used to produce the file
Lightness                 : Describes the changes in lightness settings on the digitization mechanism made during the production of the file
OriginalSourceMedium      : Original medium of the material (e.g. vinyl, Audio-CD, Super8 or BetaMax)
OriginalSourceForm        : Original form of the material (e.g. slide, paper, map)
OriginalSourceForm/NumCol : Number of colors requested when digitizing (e.g. 256 for images or 32 bit RGB for video)
OriginalSourceForm/Name   : Name of the product the file was originally intended for
OriginalSourceForm/Croppe : Describes whether the original image has been cropped and, if so, how it was cropped (e.g. 16:9 to 4:3, top and bottom)
OriginalSourceForm/Sharpn : Identifies changes in sharpness the digitization mechanism made during the production of the file
Tagged_Application        : Software used to tag the file
BPM                       : Average number of beats per minute
ISRC                      : International Standard Recording Code, excluding the ISRC prefix and including hyphens
ISBN                      : International Standard Book Number.
BarCode                   : EAN-13 (13-digit European Article Numbering) or UPC-A (12-digit Universal Product Code) bar code identifier
LCCN                      : Library of Congress Control Number
CatalogNumber             : A label-specific catalogue number used to identify the release. e.g. TIC 01
LabelCode                 : A 4-digit or 5-digit number to identify the record label, typically printed as (LC) xxxx or (LC) 0xxxx on CDs medias or covers, with only the number being stored
Owner                     : Owner of the file
Copyright                 : Copyright attribution
Copyright/Url             : Link to a site with copyright/legal information
Producer_Copyright        : Copyright information as per the production copyright holder
TermsOfUse                : License information (e.g. All Rights Reserved, Any Use Permitted)
ServiceName
ServiceChannel
Service/Url
ServiceProvider
ServiceProvider/Url
ServiceType
NetworkName
OriginalNetworkName
Country
TimeZone
Cover                     : Is there a cover
Cover_Description         : Short description (e.g. Earth in space)
Cover_Type
Cover_Mime                : Mime type of cover file
Cover_Data                : Cover, in binary format, encoded as BASE64
Lyrics                    : Text of a song
Comment                   : Any comment related to the content
Rating                    : A numeric value defining how much a person likes the song/movie. The number is between 0 and 5 with decimal values possible (e.g. 2.7), 5(.0) being the highest possible rating.
Added_Date                : Date/year the item was added to the owners collection
Played_First_Date         : Date the owner first played an item
Played_Last_Date          : Date the owner last played an item
Played_Count              : Number of times an item was played
EPG_Positions_Begin
EPG_Positions_End

Video
Count                     : Count of objects available in this stream
Status                    : bit field (0=IsAccepted, 1=IsFilled, 2=IsUpdated, 3=IsFinished)
StreamCount               : Count of streams of that kind available
StreamKind                : Stream type name
StreamKind/String         : Stream type name
StreamKindID              : Number of the stream (base=0)
StreamKindPos             : When multiple streams, number of the stream (base=1)
StreamOrder               : Stream order in the file, whatever is the kind of stream (base=0)
FirstPacketOrder          : Order of the first fully decodable packet met in the file, whatever is the kind of stream (base=0)
Inform                    : Last **Inform** call
ID                        : The ID for this stream in this file
ID/String                 : The ID for this stream in this file
OriginalSourceMedium_ID   : The ID for this stream in the original medium of the material
OriginalSourceMedium_ID/S : The ID for this stream in the original medium of the material
UniqueID                  : The unique ID for this stream, should be copied with stream copy
UniqueID/String           : The unique ID for this stream, should be copied with stream copy
MenuID                    : The menu ID for this stream in this file
MenuID/String             : The menu ID for this stream in this file
Format                    : Format used
Format/String             : Format used + additional features
Format/Info               : Info about Format
Format/Url                : Link
Format_Commercial         : Commercial name used by vendor for theses setings or Format field if there is no difference
Format_Commercial_IfAny   : Commercial name used by vendor for theses setings if there is one
Format_Version            : Version of this format
Format_Profile            : Profile of the Format (old XML: 'Profile@Level@Tier' format
Format_Level              : Level of the Format (only MIXML)
Format_Tier               : Tier of the Format (only MIXML)
Format_Compression        : Compression method used
Format_AdditionalFeatures : Format features needed for fully supporting the content
MultiView_BaseProfile     : Multiview, profile of the base stream
MultiView_Count           : Multiview, count of views
MultiView_Layout          : Multiview, how views are muxed in the container in case of it is not muxing in the stream
HDR_Format                : Format used
HDR_Format/String         : Format used + version + profile + level + layers + settings + compatibility
HDR_Format_Commercial     : Commercial name used by vendor for theses HDR settings or HDR Format field if there is no difference
HDR_Format_Version        : Version of this format
HDR_Format_Profile        : Profile of the Format
HDR_Format_Level          : Level of the Format
HDR_Format_Settings       : Settings of the Format
HDR_Format_Compatibility  : Compatibility with some commercial namings
Format_Settings           : Settings needed for decoder used, summary
Format_Settings_BVOP      : Settings needed for decoder used, detailled
Format_Settings_BVOP/Stri : Settings needed for decoder used, detailled
Format_Settings_QPel      : Settings needed for decoder used, detailled
Format_Settings_QPel/Stri : Settings needed for decoder used, detailled
Format_Settings_GMC       : Settings needed for decoder used, detailled
Format_Settings_GMC/String
Format_Settings_Matrix    : Settings needed for decoder used, detailled
Format_Settings_Matrix/St : Settings needed for decoder used, detailled
Format_Settings_Matrix_Da : Matrix, in binary format encoded BASE64. Order = intra, non-intra, gray intra, gray non-intra
Format_Settings_CABAC     : Settings needed for decoder used, detailled
Format_Settings_CABAC/Str : Settings needed for decoder used, detailled
Format_Settings_RefFrames : Settings needed for decoder used, detailled
Format_Settings_RefFrames : Settings needed for decoder used, detailled
Format_Settings_Pulldown  : Settings needed for decoder used, detailled
Format_Settings_Endianness
Format_Settings_Packing
Format_Settings_FrameMode : Settings needed for decoder used, detailled
Format_Settings_GOP       : Settings needed for decoder used, detailled (M=x N=y)
Format_Settings_PictureSt : Settings needed for decoder used, detailled (Type of frame, and field/frame info)
Format_Settings_Wrapping  : Wrapping mode (Frame wrapped or Clip wrapped)
InternetMediaType         : Internet Media Type (aka MIME Type, Content-Type)
MuxingMode                : How this file is muxed in the container
CodecID                   : Codec ID (found in some containers)
CodecID/String            : Codec ID (found in some containers)
CodecID/Info              : Info on the codec
CodecID/Hint              : Hint/popular name for this codec
CodecID/Url               : Homepage for more details about this codec
CodecID_Description       : Manual description given by the container
Codec                     : Deprecated, do not use in new projects
Codec/String              : Deprecated, do not use in new projects
Codec/Family              : Deprecated, do not use in new projects
Codec/Info                : Deprecated, do not use in new projects
Codec/Url                 : Deprecated, do not use in new projects
Codec/CC                  : Deprecated, do not use in new projects
Codec_Profile             : Deprecated, do not use in new projects
Codec_Description         : Deprecated, do not use in new projects
Codec_Settings            : Deprecated, do not use in new projects
Codec_Settings_PacketBitS : Deprecated, do not use in new projects
Codec_Settings_BVOP       : Deprecated, do not use in new projects
Codec_Settings_QPel       : Deprecated, do not use in new projects
Codec_Settings_GMC        : Deprecated, do not use in new projects
Codec_Settings_GMC/String : Deprecated, do not use in new projects
Codec_Settings_Matrix     : Deprecated, do not use in new projects
Codec_Settings_Matrix_Dat : Deprecated, do not use in new projects
Codec_Settings_CABAC      : Deprecated, do not use in new projects
Codec_Settings_RefFrames  : Deprecated, do not use in new projects
Duration                  : Play time of the stream in ms
Duration/String           : Play time in format : XXx YYy only, YYy omited if zero
Duration/String1          : Play time in format : HHh MMmn SSs MMMms, XX omited if zero
Duration/String2          : Play time in format : XXx YYy only, YYy omited if zero
Duration/String3          : Play time in format : HH:MM:SS.MMM
Duration/String4          : Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Duration/String5          : Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Duration_FirstFrame       : Duration of the first frame if it is longer than others, in ms
Duration_FirstFrame/Strin : Duration of the first frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Duration_FirstFrame/Strin : Duration of the first frame if it is longer than others, in format : HHh MMmn SSs MMMms, XX omited if zero
Duration_FirstFrame/Strin : Duration of the first frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Duration_FirstFrame/Strin : Duration of the first frame if it is longer than others, in format : HH:MM:SS.MMM
Duration_FirstFrame/Strin : Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Duration_FirstFrame/Strin : Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Duration_LastFrame        : Duration of the last frame if it is longer than others, in ms
Duration_LastFrame/String : Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Duration_LastFrame/String : Duration of the last frame if it is longer than others, in format : HHh MMmn SSs MMMms, XX omited if zero
Duration_LastFrame/String : Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Duration_LastFrame/String : Duration of the last frame if it is longer than others, in format : HH:MM:SS.MMM
Duration_LastFrame/String : Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Duration_LastFrame/String : Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Source_Duration           : Source Play time of the stream, in ms
Source_Duration/String    : Source Play time in format : XXx YYy only, YYy omited if zero
Source_Duration/String1   : Source Play time in format : HHh MMmn SSs MMMms, XX omited if zero
Source_Duration/String2   : Source Play time in format : XXx YYy only, YYy omited if zero
Source_Duration/String3   : Source Play time in format : HH:MM:SS.MMM
Source_Duration/String4   : Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Source_Duration/String5   : Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in ms
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : HHh MMmn SSs MMMms, XX omited if zero
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : HH:MM:SS.MMM
Source_Duration_FirstFram : Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Source_Duration_FirstFram : Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in ms
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : HHh MMmn SSs MMMms, XX omited if zero
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : HH:MM:SS.MMM
Source_Duration_LastFrame : Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Source_Duration_LastFrame : Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
BitRate_Mode              : Bit rate mode (VBR, CBR)
BitRate_Mode/String       : Bit rate mode (Variable, Cconstant)
BitRate                   : Bit rate in bps
BitRate/String            : Bit rate (with measurement)
BitRate_Minimum           : Minimum Bit rate in bps
BitRate_Minimum/String    : Minimum Bit rate (with measurement)
BitRate_Nominal           : Nominal Bit rate in bps
BitRate_Nominal/String    : Nominal Bit rate (with measurement)
BitRate_Maximum           : Maximum Bit rate in bps
BitRate_Maximum/String    : Maximum Bit rate (with measurement)
BitRate_Encoded           : Encoded (with forced padding) bit rate in bps, if some container padding is present
BitRate_Encoded/String    : Encoded (with forced padding) bit rate (with measurement), if some container padding is present
Width                     : Width (aperture size if present) in pixel
Width/String              : Width (aperture size if present) with measurement (pixel)
Width_Offset              : Offset between original width and displayed width in pixel
Width_Offset/String       : Offset between original width and displayed width in pixel
Width_Original            : Original (in the raw stream) width in pixel
Width_Original/String     : Original (in the raw stream) width with measurement (pixel)
Width_CleanAperture       : Clean Aperture width in pixel
Width_CleanAperture/Strin : Clean Aperture width with measurement (pixel)
Height                    : Height in pixel
Height/String             : Height with measurement (pixel)
Height_Offset             : Offset between original height and displayed height in pixel
Height_Offset/String      : Offset between original height and displayed height  in pixel
Height_Original           : Original (in the raw stream) height in pixel
Height_Original/String    : Original (in the raw stream) height with measurement (pixel)
Height_CleanAperture      : Clean Aperture height in pixel
Height_CleanAperture/Stri : Clean Aperture height with measurement (pixel)
Stored_Width              : Stored width
Stored_Height             : Stored height
Sampled_Width             : Sampled width
Sampled_Height            : Sampled height
PixelAspectRatio          : Pixel Aspect ratio
PixelAspectRatio/String   : Pixel Aspect ratio
PixelAspectRatio_Original : Original (in the raw stream) Pixel Aspect ratio
PixelAspectRatio_Original : Original (in the raw stream) Pixel Aspect ratio
PixelAspectRatio_CleanApe : Clean Aperture Pixel Aspect ratio
PixelAspectRatio_CleanApe : Clean Aperture Pixel Aspect ratio
DisplayAspectRatio        : Display Aspect ratio
DisplayAspectRatio/String : Display Aspect ratio
DisplayAspectRatio_Origin : Original (in the raw stream) Display Aspect ratio
DisplayAspectRatio_Origin : Original (in the raw stream) Display Aspect ratio
DisplayAspectRatio_CleanA : Clean Aperture Display Aspect ratio
DisplayAspectRatio_CleanA : Clean Aperture Display Aspect ratio
ActiveFormatDescription   : Active Format Description (AFD value)
ActiveFormatDescription/S : Active Format Description (text)
ActiveFormatDescription_M : Active Format Description (AFD value) muxing mode (Ancillary or Raw stream)
Rotation                  : Rotation
Rotation/String           : Rotation (if not horizontal)
FrameRate_Mode            : Frame rate mode (CFR, VFR)
FrameRate_Mode/String     : Frame rate mode (Constant, Variable)
FrameRate_Mode_Original   : Original frame rate mode (CFR, VFR)
FrameRate_Mode_Original/S : Original frame rate mode (Constant, Variable)
FrameRate                 : Frames per second
FrameRate/String          : Frames per second (with measurement)
FrameRate_Num             : Frames per second, numerator
FrameRate_Den             : Frames per second, denominator
FrameRate_Minimum         : Minimum Frames per second
FrameRate_Minimum/String  : Minimum Frames per second (with measurement)
FrameRate_Nominal         : Nominal Frames per second
FrameRate_Nominal/String  : Nominal Frames per second (with measurement)
FrameRate_Maximum         : Maximum Frames per second
FrameRate_Maximum/String  : Maximum Frames per second (with measurement)
FrameRate_Original        : Original (in the raw stream) frames per second
FrameRate_Original/String : Original (in the raw stream) frames per second (with measurement)
FrameRate_Original_Num    : Frames per second, numerator
FrameRate_Original_Den    : Frames per second, denominator
FrameCount                : Number of frames
Source_FrameCount         : Source Number of frames
Standard                  : NTSC or PAL
Resolution                : Deprecated, do not use in new projects
Resolution/String         : Deprecated, do not use in new projects
Colorimetry               : Deprecated, do not use in new projects
ColorSpace
ChromaSubsampling
ChromaSubsampling/String
ChromaSubsampling_Position
BitDepth                  : 16/24/32
BitDepth/String           : 16/24/32 bits
ScanType
ScanType/String
ScanType_Original
ScanType_Original/String
ScanType_StoreMethod
ScanType_StoreMethod_FieldsPerBlock
ScanType_StoreMethod/String
ScanOrder
ScanOrder/String
ScanOrder_Stored
ScanOrder_Stored/String
ScanOrder_StoredDisplayedInverted
ScanOrder_Original
ScanOrder_Original/String
Interlacement             : Deprecated, do not use in new projects
Interlacement/String      : Deprecated, do not use in new projects
Compression_Mode          : Compression mode (Lossy or Lossless)
Compression_Mode/String   : Compression mode (Lossy or Lossless)
Compression_Ratio         : Current stream size divided by uncompressed stream size
Bits-(Pixel*Frame)        : bits/(Pixel*Frame) (like Gordian Knot)
Delay                     : Delay fixed in the stream (relative) IN MS
Delay/String              : Delay with measurement
Delay/String1             : Delay with measurement
Delay/String2             : Delay with measurement
Delay/String3             : Delay in format : HH:MM:SS.MMM
Delay/String4             : Delay in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Delay/String5             : Delay in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Delay_Settings            : Delay settings (in case of timecode for example)
Delay_DropFrame           : Delay drop frame
Delay_Source              : Delay source (Container or Stream or empty)
Delay_Source/String       : Delay source (Container or Stream or empty)
Delay_Original            : Delay fixed in the raw stream (relative) IN MS
Delay_Original/String     : Delay with measurement
Delay_Original/String1    : Delay with measurement
Delay_Original/String2    : Delay with measurement
Delay_Original/String3    : Delay in format: HH:MM:SS.MMM
Delay_Original/String4    : Delay in format: HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Delay_Original/String5    : Delay in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Delay_Original_Settings   : Delay settings (in case of timecode for example)
Delay_Original_DropFrame  : Delay drop frame info
Delay_Original_Source     : Delay source (Stream or empty)
TimeStamp_FirstFrame      : TimeStamp fixed in the stream (relative) IN MS
TimeStamp_FirstFrame/Stri : TimeStamp with measurement
TimeStamp_FirstFrame/Stri : TimeStamp with measurement
TimeStamp_FirstFrame/Stri : TimeStamp with measurement
TimeStamp_FirstFrame/Stri : TimeStamp in format : HH:MM:SS.MMM
TimeStamp_FirstFrame/Stri : TimeStamp in format: HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
TimeStamp_FirstFrame/Stri : TimeStamp in format : HH:MM:SS.mmm (HH:MM:SS:FF)
TimeCode_FirstFrame       : Time code in HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available format
TimeCode_Settings         : Time code settings
TimeCode_Source           : Time code source (Container, Stream, SystemScheme1, SDTI, ANC...)
Gop_OpenClosed            : Time code information about Open/Closed
Gop_OpenClosed/String     : Time code information about Open/Closed
Gop_OpenClosed_FirstFrame : Time code information about Open/Closed of first frame if GOP is Open for the other GOPs
Gop_OpenClosed_FirstFrame : Time code information about Open/Closed of first frame if GOP is Open for the other GOPs
StreamSize                : Streamsize in bytes
StreamSize/String         : Streamsize in with percentage value
StreamSize/String1
StreamSize/String2
StreamSize/String3
StreamSize/String4
StreamSize/String5        : Streamsize in with percentage value
StreamSize_Proportion     : Stream size divided by file size
StreamSize_Demuxed        : StreamSize in bytes of hte stream after demux
StreamSize_Demuxed/String : StreamSize_Demuxed in with percentage value
StreamSize_Demuxed/String1
StreamSize_Demuxed/String2
StreamSize_Demuxed/String3
StreamSize_Demuxed/String4
StreamSize_Demuxed/String : StreamSize_Demuxed in with percentage value (note: theoritical value, not for real use)
Source_StreamSize         : Source Streamsize in bytes
Source_StreamSize/String  : Source Streamsize in with percentage value
Source_StreamSize/String1
Source_StreamSize/String2
Source_StreamSize/String3
Source_StreamSize/String4
Source_StreamSize/String5 : Source Streamsize in with percentage value
Source_StreamSize_Proport : Source Stream size divided by file size
StreamSize_Encoded        : Encoded Streamsize in bytes
StreamSize_Encoded/String : Encoded Streamsize in with percentage value
StreamSize_Encoded/String1
StreamSize_Encoded/String2
StreamSize_Encoded/String3
StreamSize_Encoded/String4
StreamSize_Encoded/String : Encoded Streamsize in with percentage value
StreamSize_Encoded_Propor : Encoded Stream size divided by file size
Source_StreamSize_Encoded : Source Encoded Streamsize in bytes
Source_StreamSize_Encoded : Source Encoded Streamsize in with percentage value
Source_StreamSize_Encoded/String1
Source_StreamSize_Encoded/String2
Source_StreamSize_Encoded/String3
Source_StreamSize_Encoded/String4
Source_StreamSize_Encoded : Source Encoded Streamsize in with percentage value
Source_StreamSize_Encoded : Source Encoded Stream size divided by file size
Alignment                 : How this stream file is aligned in the container
Alignment/String
Title                     : Name of the track
Encoded_Application       : Name of the software package used to create the file, such as Microsoft WaveEdit
Encoded_Application/Strin : Name of the software package used to create the file, such as Microsoft WaveEdit, trying to have the format 'CompanyName ProductName (OperatingSystem) Version (Date)'
Encoded_Application_Compa : Name of the company
Encoded_Application_Name  : Name of the product
Encoded_Application_Versi : Version of the product
Encoded_Application_Url   : Name of the software package used to create the file, such as Microsoft WaveEdit.
Encoded_Library           : Software used to create the file
Encoded_Library/String    : Software used to create the file, trying to have the format 'CompanyName ProductName (OperatingSystem) Version (Date)'
Encoded_Library_CompanyNa : Name of the company
Encoded_Library_Name      : Name of the the encoding-software
Encoded_Library_Version   : Version of encoding-software
Encoded_Library_Date      : Release date of software
Encoded_Library_Settings  : Parameters used by the software
Encoded_OperatingSystem   : Operating System of encoding-software
Language                  : Language (2-letter ISO 639-1 if exists, else 3-letter ISO 639-2, and with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn)
Language/String           : Language (full)
Language/String1          : Language (full)
Language/String2          : Language (2-letter ISO 639-1 if exists, else empty)
Language/String3          : Language (3-letter ISO 639-2 if exists, else empty)
Language/String4          : Language (2-letter ISO 639-1 if exists with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn, else empty)
Language_More             : More info about Language (e.g. Director's Comment)
ServiceKind               : Service kind, e.g. visually impaired, commentary, voice over
ServiceKind/String        : Service kind (full)
Disabled                  : Set if that track should not be used
Disabled/String           : Set if that track should not be used
Default                   : Set if that track should be used if no language found matches the user preference.
Default/String            : Set if that track should be used if no language found matches the user preference.
Forced                    : Set if that track should be used if no language found matches the user preference.
Forced/String             : Set if that track should be used if no language found matches the user preference.
AlternateGroup            : Number of a group in order to provide versions of the same track
AlternateGroup/String     : Number of a group in order to provide versions of the same track
Encoded_Date              : UTC time that the encoding of this item was completed began.
Tagged_Date               : UTC time that the tags were done for this item.
Encryption
BufferSize                : Defines the size of the buffer needed to decode the sequence.
colour_description_presen : Presence of colour description
colour_description_presen : Presence of colour description (source)
colour_description_presen : Presence of colour description (if incoherencies)
colour_description_presen : Presence of colour description (source if incoherencies)
colour_range              : Colour range for YUV colour space
colour_range_Source       : Colour range for YUV colour space (source)
colour_range_Original     : Colour range for YUV colour space (if incoherencies)
colour_range_Original_Sou : Colour range for YUV colour space (source if incoherencies)
colour_primaries          : Chromaticity coordinates of the source primaries
colour_primaries_Source   : Chromaticity coordinates of the source primaries (source)
colour_primaries_Original : Chromaticity coordinates of the source primaries (if incoherencies)
colour_primaries_Original : Chromaticity coordinates of the source primaries (source if incoherencies)
transfer_characteristics  : Opto-electronic transfer characteristic of the source picture
transfer_characteristics_ : Opto-electronic transfer characteristic of the source picture (source)
transfer_characteristics_ : Opto-electronic transfer characteristic of the source picture (if incoherencies)
transfer_characteristics_ : Opto-electronic transfer characteristic of the source picture (source if incoherencies)
matrix_coefficients       : Matrix coefficients used in deriving luma and chroma signals from the green, blue, and red primaries
matrix_coefficients_Sourc : Matrix coefficients used in deriving luma and chroma signals from the green, blue, and red primaries (source)
matrix_coefficients_Origi : Matrix coefficients used in deriving luma and chroma signals from the green, blue, and red primaries (if incoherencies)
matrix_coefficients_Origi : Matrix coefficients used in deriving luma and chroma signals from the green, blue, and red primaries (source if incoherencies)
MasteringDisplay_ColorPri : Chromaticity coordinates of the source primaries of the mastering display
MasteringDisplay_ColorPri : Chromaticity coordinates of the source primaries of the mastering display (source)
MasteringDisplay_ColorPri : Chromaticity coordinates of the source primaries of the mastering display (if incoherencies)
MasteringDisplay_ColorPri : Chromaticity coordinates of the source primaries of the mastering display (source if incoherencies)
MasteringDisplay_Luminanc : Luminance of the mastering display
MasteringDisplay_Luminanc : Luminance of the mastering display (source)
MasteringDisplay_Luminanc : Luminance of the mastering display (if incoherencies)
MasteringDisplay_Luminanc : Luminance of the mastering display (source if incoherencies)
MaxCLL                    : Maximum content light level
MaxCLL_Source             : Maximum content light level (source)
MaxCLL_Original           : Maximum content light level (if incoherencies)
MaxCLL_Original_Source    : Maximum content light level (source if incoherencies)
MaxFALL                   : Maximum frame average light level
MaxFALL_Source            : Maximum frame average light level (source)
MaxFALL_Original          : Maximum frame average light level (if incoherencies)
MaxFALL_Original_Source   : Maximum frame average light level (source if incoherencies)

Audio
Count                     : Count of objects available in this stream
Status                    : bit field (0=IsAccepted, 1=IsFilled, 2=IsUpdated, 3=IsFinished)
StreamCount               : Count of streams of that kind available
StreamKind                : Stream type name
StreamKind/String         : Stream type name
StreamKindID              : Number of the stream (base=0)
StreamKindPos             : When multiple streams, number of the stream (base=1)
StreamOrder               : Stream order in the file, whatever is the kind of stream (base=0)
FirstPacketOrder          : Order of the first fully decodable packet met in the file, whatever is the kind of stream (base=0)
Inform                    : Last **Inform** call
ID                        : The ID for this stream in this file
ID/String                 : The ID for this stream in this file
OriginalSourceMedium_ID   : The ID for this stream in the original medium of the material
OriginalSourceMedium_ID/S : The ID for this stream in the original medium of the material
UniqueID                  : The unique ID for this stream, should be copied with stream copy
UniqueID/String           : The unique ID for this stream, should be copied with stream copy
MenuID                    : The menu ID for this stream in this file
MenuID/String             : The menu ID for this stream in this file
Format                    : Format used
Format/String             : Format used + additional features
Format/Info               : Info about the format
Format/Url                : Homepage of this format
Format_Commercial         : Commercial name used by vendor for theses setings or Format field if there is no difference
Format_Commercial_IfAny   : Commercial name used by vendor for theses setings if there is one
Format_Version            : Version of this format
Format_Profile            : Profile of the Format (old XML: 'Profile@Level' format
Format_Level              : Level of the Format (only MIXML)
Format_Compression        : Compression method used
Format_Settings           : Settings needed for decoder used, summary
Format_Settings_SBR
Format_Settings_SBR/String
Format_Settings_PS
Format_Settings_PS/String
Format_Settings_Mode
Format_Settings_ModeExtension
Format_Settings_Emphasis
Format_Settings_Floor
Format_Settings_Firm
Format_Settings_Endianness
Format_Settings_Sign
Format_Settings_Law
Format_Settings_ITU
Format_Settings_Wrapping  : Wrapping mode (Frame wrapped or Clip wrapped)
Format_AdditionalFeatures : Format features needed for fully supporting the content
Matrix_Format             : Matrix format (e.g. DTS Neural)
InternetMediaType         : Internet Media Type (aka MIME Type, Content-Type)
MuxingMode                : How this stream is muxed in the container
MuxingMode_MoreInfo       : More info (text) about the muxing mode
CodecID                   : Codec ID (found in some containers)
CodecID/String            : Codec ID (found in some containers)
CodecID/Info              : Info about codec ID
CodecID/Hint              : Hint/popular name for this codec ID
CodecID/Url               : Homepage for more details about this codec ID
CodecID_Description       : Manual description given by the container
Codec                     : Deprecated, do not use in new projects
Codec/String              : Deprecated, do not use in new projects
Codec/Family              : Deprecated, do not use in new projects
Codec/Info                : Deprecated, do not use in new projects
Codec/Url                 : Deprecated, do not use in new projects
Codec/CC                  : Deprecated, do not use in new projects
Codec_Description         : Deprecated, do not use in new projects
Codec_Profile             : Deprecated, do not use in new projects
Codec_Settings            : Deprecated, do not use in new projects
Codec_Settings_Automatic  : Deprecated, do not use in new projects
Codec_Settings_Floor      : Deprecated, do not use in new projects
Codec_Settings_Firm       : Deprecated, do not use in new projects
Codec_Settings_Endianness : Deprecated, do not use in new projects
Codec_Settings_Sign       : Deprecated, do not use in new projects
Codec_Settings_Law        : Deprecated, do not use in new projects
Codec_Settings_ITU        : Deprecated, do not use in new projects
Duration                  : Play time of the stream, in ms
Duration/String           : Play time in format : XXx YYy only, YYy omited if zero
Duration/String1          : Play time in format : HHh MMmn SSs MMMms, XX omited if zero
Duration/String2          : Play time in format : XXx YYy only, YYy omited if zero
Duration/String3          : Play time in format : HH:MM:SS.MMM
Duration/String4          : Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Duration/String5          : Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Duration_FirstFrame       : Duration of the first frame if it is longer than others, in ms
Duration_FirstFrame/Strin : Duration of the first frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Duration_FirstFrame/Strin : Duration of the first frame if it is longer than others, in format : HHh MMmn SSs MMMms, XX omited if zero
Duration_FirstFrame/Strin : Duration of the first frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Duration_FirstFrame/Strin : Duration of the first frame if it is longer than others, in format : HH:MM:SS.MMM
Duration_FirstFrame/Strin : Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Duration_FirstFrame/Strin : Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Duration_LastFrame        : Duration of the last frame if it is longer than others, in ms
Duration_LastFrame/String : Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Duration_LastFrame/String : Duration of the last frame if it is longer than others, in format : HHh MMmn SSs MMMms, XX omited if zero
Duration_LastFrame/String : Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Duration_LastFrame/String : Duration of the last frame if it is longer than others, in format : HH:MM:SS.MMM
Duration_LastFrame/String : Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Duration_LastFrame/String : Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Source_Duration           : Source Play time of the stream, in ms
Source_Duration/String    : Source Play time in format : XXx YYy only, YYy omited if zero
Source_Duration/String1   : Source Play time in format : HHh MMmn SSs MMMms, XX omited if zero
Source_Duration/String2   : Source Play time in format : XXx YYy only, YYy omited if zero
Source_Duration/String3   : Source Play time in format : HH:MM:SS.MMM
Source_Duration/String4   : Source Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Source_Duration/String5   : Source Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in ms
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : HHh MMmn SSs MMMms, XX omited if zero
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : HH:MM:SS.MMM
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in ms
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : HHh MMmn SSs MMMms, XX omited if zero
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : HH:MM:SS.MMM
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : HH:MM:SS.mmm (HH:MM:SS:FF)
BitRate_Mode              : Bit rate mode (VBR, CBR)
BitRate_Mode/String       : Bit rate mode (Constant, Variable)
BitRate                   : Bit rate in bps
BitRate/String            : Bit rate (with measurement)
BitRate_Minimum           : Minimum Bit rate in bps
BitRate_Minimum/String    : Minimum Bit rate (with measurement)
BitRate_Nominal           : Nominal Bit rate in bps
BitRate_Nominal/String    : Nominal Bit rate (with measurement)
BitRate_Maximum           : Maximum Bit rate in bps
BitRate_Maximum/String    : Maximum Bit rate (with measurement)
BitRate_Encoded           : Encoded (with forced padding) bit rate in bps, if some container padding is present
BitRate_Encoded/String    : Encoded (with forced padding) bit rate (with measurement), if some container padding is present
Channel(s)                : Number of channels
Channel(s)/String         : Number of channels (with measurement)
Channel(s)_Original       : Number of channels
Channel(s)_Original/Strin : Number of channels (with measurement)
Matrix_Channel(s)         : Number of channels after matrix decoding
Matrix_Channel(s)/String  : Number of channels after matrix decoding (with measurement)
ChannelPositions          : Position of channels
ChannelPositions_Original : Position of channels
ChannelPositions/String2  : Position of channels (x/y.z format)
ChannelPositions_Original : Position of channels (x/y.z format)
Matrix_ChannelPositions   : Position of channels after matrix decoding
Matrix_ChannelPositions/S : Position of channels after matrix decoding (x/y.z format)
ChannelLayout             : Layout of channels (in the stream)
ChannelLayout_Original    : Layout of channels (in the stream)
ChannelLayoutID           : ID of layout of channels (e.g. MXF descriptor channel assignment). Warning, sometimes this is not enough for uniquely identifying a layout (e.g. MXF descriptor channel assignment is SMPTE 377-4). For AC-3, the form is x,y with x=acmod and y=lfeon.
SamplesPerFrame           : Sampling rate
SamplingRate              : Sampling rate
SamplingRate/String       : in KHz
SamplingCount             : Sample count (based on sampling rate)
Source_SamplingCount      : Source Sample count (based on sampling rate)
FrameRate                 : Frames per second
FrameRate/String          : Frames per second (with measurement)
FrameRate_Num             : Frames per second, numerator
FrameRate_Den             : Frames per second, denominator
FrameCount                : Frame count (a frame contains a count of samples depends of the format)
Source_FrameCount         : Source Frame count (a frame contains a count of samples depends of the format)
Resolution                : Deprecated, do not use in new projects
Resolution/String         : Deprecated, do not use in new projects
BitDepth                  : Resolution in bits (8, 16, 20, 24). Note: significant bits in case the stored bit depth is different
BitDepth/String           : Resolution in bits (8, 16, 20, 24). Note: significant bits in case the stored bit depth is different
BitDepth_Detected         : Detected (during scan of the input by the muxer) resolution in bits
BitDepth_Detected/String  : Detected (during scan of the input by the muxer) resolution in bits
BitDepth_Stored           : Stored Resolution in bits (8, 16, 20, 24)
BitDepth_Stored/String    : Stored Resolution in bits (8, 16, 20, 24)
Compression_Mode          : Compression mode (Lossy or Lossless)
Compression_Mode/String   : Compression mode (Lossy or Lossless)
Compression_Ratio         : Current stream size divided by uncompressed stream size
Delay                     : Delay fixed in the stream (relative) IN MS
Delay/String              : Delay with measurement
Delay/String1             : Delay with measurement
Delay/String2             : Delay with measurement
Delay/String3             : Delay in format : HH:MM:SS.MMM
Delay/String4             : Delay in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Delay/String5             : Delay in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Delay_Settings            : Delay settings (in case of timecode for example)
Delay_DropFrame           : Delay drop frame
Delay_Source              : Delay source (Container or Stream or empty)
Delay_Source/String       : Delay source (Container or Stream or empty)
Delay_Original            : Delay fixed in the raw stream (relative) IN MS
Delay_Original/String     : Delay with measurement
Delay_Original/String1    : Delay with measurement
Delay_Original/String2    : Delay with measurement
Delay_Original/String3    : Delay in format: HH:MM:SS.MMM
Delay_Original/String4    : Delay in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Delay_Original/String5    : Delay in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Delay_Original_Settings   : Delay settings (in case of timecode for example)
Delay_Original_DropFrame  : Delay drop frame info
Delay_Original_Source     : Delay source (Stream or empty)
Video_Delay               : Delay fixed in the stream (absolute / video)
Video_Delay/String
Video_Delay/String1
Video_Delay/String2
Video_Delay/String3
Video_Delay/String4
Video_Delay/String5
Video0_Delay              : Deprecated, do not use in new projects
Video0_Delay/String       : Deprecated, do not use in new projects
Video0_Delay/String1      : Deprecated, do not use in new projects
Video0_Delay/String2      : Deprecated, do not use in new projects
Video0_Delay/String3      : Deprecated, do not use in new projects
Video0_Delay/String4      : Deprecated, do not use in new projects
Video0_Delay/String5      : Deprecated, do not use in new projects
ReplayGain_Gain           : The gain to apply to reach 89dB SPL on playback
ReplayGain_Gain/String
ReplayGain_Peak           : The maximum absolute peak value of the item
StreamSize                : Streamsize in bytes
StreamSize/String         : Streamsize in with percentage value
StreamSize/String1
StreamSize/String2
StreamSize/String3
StreamSize/String4
StreamSize/String5        : Streamsize in with percentage value
StreamSize_Proportion     : Stream size divided by file size
StreamSize_Demuxed        : StreamSize in bytes of hte stream after demux
StreamSize_Demuxed/String : StreamSize_Demuxed in with percentage value
StreamSize_Demuxed/String1
StreamSize_Demuxed/String2
StreamSize_Demuxed/String3
StreamSize_Demuxed/String4
StreamSize_Demuxed/String : StreamSize_Demuxed in with percentage value (note: theoritical value, not for real use)
Source_StreamSize         : Source Streamsize in bytes
Source_StreamSize/String  : Source Streamsize in with percentage value
Source_StreamSize/String1
Source_StreamSize/String2
Source_StreamSize/String3
Source_StreamSize/String4
Source_StreamSize/String5 : Source Streamsize in with percentage value
Source_StreamSize_Proport : Source Stream size divided by file size
StreamSize_Encoded        : Encoded Streamsize in bytes
StreamSize_Encoded/String : Encoded Streamsize in with percentage value
StreamSize_Encoded/String1
StreamSize_Encoded/String2
StreamSize_Encoded/String3
StreamSize_Encoded/String4
StreamSize_Encoded/String : Encoded Streamsize in with percentage value
StreamSize_Encoded_Propor : Encoded Stream size divided by file size
Source_StreamSize_Encoded : Source Encoded Streamsize in bytes
Source_StreamSize_Encoded : Source Encoded Streamsize in with percentage value
Source_StreamSize_Encoded/String1
Source_StreamSize_Encoded/String2
Source_StreamSize_Encoded/String3
Source_StreamSize_Encoded/String4
Source_StreamSize_Encoded : Source Encoded Streamsize in with percentage value
Source_StreamSize_Encoded : Source Encoded Stream size divided by file size
Alignment                 : How this stream file is aligned in the container
Alignment/String          : Where this stream file is aligned in the container
Interleave_VideoFrames    : Between how many video frames the stream is inserted
Interleave_Duration       : Between how much time (ms) the stream is inserted
Interleave_Duration/Strin : Between how much time and video frames the stream is inserted (with measurement)
Interleave_Preload        : How much time is buffered before the first video frame
Interleave_Preload/String : How much time is buffered before the first video frame (with measurement)
Title                     : Name of the track
Encoded_Application       : Name of the software package used to create the file, such as Microsoft WaveEdit
Encoded_Application/Strin : Name of the software package used to create the file, such as Microsoft WaveEdit, trying to have the format 'CompanyName ProductName (OperatingSystem) Version (Date)'
Encoded_Application_Compa : Name of the company
Encoded_Application_Name  : Name of the product
Encoded_Application_Versi : Version of the product
Encoded_Application_Url   : Name of the software package used to create the file, such as Microsoft WaveEdit.
Encoded_Library           : Software used to create the file
Encoded_Library/String    : Software used to create the file, trying to have the format 'CompanyName ProductName (OperatingSystem) Version (Date)'
Encoded_Library_CompanyNa : Name of the company
Encoded_Library_Name      : Name of the the encoding-software
Encoded_Library_Version   : Version of encoding-software
Encoded_Library_Date      : Release date of software
Encoded_Library_Settings  : Parameters used by the software
Encoded_OperatingSystem   : Operating System of encoding-software
Language                  : Language (2-letter ISO 639-1 if exists, else 3-letter ISO 639-2, and with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn)
Language/String           : Language (full)
Language/String1          : Language (full)
Language/String2          : Language (2-letter ISO 639-1 if exists, else empty)
Language/String3          : Language (3-letter ISO 639-2 if exists, else empty)
Language/String4          : Language (2-letter ISO 639-1 if exists with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn, else empty)
Language_More             : More info about Language (e.g. Director's Comment)
ServiceKind               : Service kind, e.g. visually impaired, commentary, voice over
ServiceKind/String        : Service kind (full)
Disabled                  : Set if that track should not be used
Disabled/String           : Set if that track should not be used
Default                   : Set if that track should be used if no language found matches the user preference.
Default/String            : Set if that track should be used if no language found matches the user preference.
Forced                    : Set if that track should be used if no language found matches the user preference.
Forced/String             : Set if that track should be used if no language found matches the user preference.
AlternateGroup            : Number of a group in order to provide versions of the same track
AlternateGroup/String     : Number of a group in order to provide versions of the same track
Encoded_Date              : UTC time that the encoding of this item was completed began.
Tagged_Date               : UTC time that the tags were done for this item.
Encryption

Text
Count                     : Count of objects available in this stream
Status                    : bit field (0=IsAccepted, 1=IsFilled, 2=IsUpdated, 3=IsFinished)
StreamCount               : Count of streams of that kind available
StreamKind                : Stream type name
StreamKind/String         : Stream type name
StreamKindID              : Number of the stream (base=0)
StreamKindPos             : When multiple streams, number of the stream (base=1)
StreamOrder               : Stream order in the file, whatever is the kind of stream (base=0)
FirstPacketOrder          : Order of the first fully decodable packet met in the file, whatever is the kind of stream (base=0)
Inform                    : Last **Inform** call
ID                        : The ID for this stream in this file
ID/String                 : The ID for this stream in this file
OriginalSourceMedium_ID   : The ID for this stream in the original medium of the material
OriginalSourceMedium_ID/S : The ID for this stream in the original medium of the material
UniqueID                  : The unique ID for this stream, should be copied with stream copy
UniqueID/String           : The unique ID for this stream, should be copied with stream copy
MenuID                    : The menu ID for this stream in this file
MenuID/String             : The menu ID for this stream in this file
Format                    : Format used
Format/String             : Format used + additional features
Format/Info               : Info about Format
Format/Url                : Link
Format_Commercial         : Commercial name used by vendor for theses setings or Format field if there is no difference
Format_Commercial_IfAny   : Commercial name used by vendor for theses setings if there is one
Format_Version            : Version of this format
Format_Profile            : Profile of the Format
Format_Compression        : Compression method used
Format_Settings           : Settings needed for decoder used
Format_Settings_Wrapping  : Wrapping mode (Frame wrapped or Clip wrapped)
Format_AdditionalFeatures : Format features needed for fully supporting the content
InternetMediaType         : Internet Media Type (aka MIME Type, Content-Type)
MuxingMode                : How this stream is muxed in the container
MuxingMode_MoreInfo       : More info (text) about the muxing mode
CodecID                   : Codec ID (found in some containers)
CodecID/String            : Codec ID (found in some containers)
CodecID/Info              : Info about codec ID
CodecID/Hint              : A hint for this codec ID
CodecID/Url               : A link for more details about this codec ID
CodecID_Description       : Manual description given by the container
Codec                     : Deprecated
Codec/String              : Deprecated
Codec/Info                : Deprecated
Codec/Url                 : Deprecated
Codec/CC                  : Deprecated
Duration                  : Play time of the stream, in ms
Duration/String           : Play time (formated)
Duration/String1          : Play time in format : HHh MMmn SSs MMMms, XX omited if zero
Duration/String2          : Play time in format : XXx YYy only, YYy omited if zero
Duration/String3          : Play time in format : HH:MM:SS.MMM
Duration/String4          : Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Duration/String5          : Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Duration_FirstFrame       : Duration of the first frame if it is longer than others, in ms
Duration_FirstFrame/Strin : Duration of the first frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Duration_FirstFrame/Strin : Duration of the first frame if it is longer than others, in format : HHh MMmn SSs MMMms, XX omited if zero
Duration_FirstFrame/Strin : Duration of the first frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Duration_FirstFrame/Strin : Duration of the first frame if it is longer than others, in format : HH:MM:SS.MMM
Duration_FirstFrame/Strin : Duration of the first frame if it is longer than others, in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Duration_FirstFrame/Strin : Duration of the first frame if it is longer than others, in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Duration_LastFrame        : Duration of the last frame if it is longer than others, in ms
Duration_LastFrame/String : Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Duration_LastFrame/String : Duration of the last frame if it is longer than others, in format : HHh MMmn SSs MMMms, XX omited if zero
Duration_LastFrame/String : Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Duration_LastFrame/String : Duration of the last frame if it is longer than others, in format : HH:MM:SS.MMM
Duration_LastFrame/String : Duration of the last frame if it is longer than others, in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Duration_LastFrame/String : Duration of the last frame if it is longer than others, in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Source_Duration           : Source Play time of the stream, in ms
Source_Duration/String    : Source Play time in format : XXx YYy only, YYy omited if zero
Source_Duration/String1   : Source Play time in format : HHh MMmn SSs MMMms, XX omited if zero
Source_Duration/String2   : Source Play time in format : XXx YYy only, YYy omited if zero
Source_Duration/String3   : Source Play time in format : HH:MM:SS.MMM
Source_Duration/String4   : Source Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Source_Duration/String5   : Source Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in ms
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : HHh MMmn SSs MMMms, XX omited if zero
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : HH:MM:SS.MMM
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in ms
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : HHh MMmn SSs MMMms, XX omited if zero
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : HH:MM:SS.MMM
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : HH:MM:SS.mmm (HH:MM:SS:FF)
BitRate_Mode              : Bit rate mode (VBR, CBR)
BitRate_Mode/String       : Bit rate mode (Constant, Variable)
BitRate                   : Bit rate in bps
BitRate/String            : Bit rate (with measurement)
BitRate_Minimum           : Minimum Bit rate in bps
BitRate_Minimum/String    : Minimum Bit rate (with measurement)
BitRate_Nominal           : Nominal Bit rate in bps
BitRate_Nominal/String    : Nominal Bit rate (with measurement)
BitRate_Maximum           : Maximum Bit rate in bps
BitRate_Maximum/String    : Maximum Bit rate (with measurement)
BitRate_Encoded           : Encoded (with forced padding) bit rate in bps, if some container padding is present
BitRate_Encoded/String    : Encoded (with forced padding) bit rate (with measurement), if some container padding is present
Width                     : Width
Width/String
Height                    : Height
Height/String
FrameRate_Mode            : Frame rate mode (CFR, VFR)
FrameRate_Mode/String     : Frame rate mode (Constant, Variable)
FrameRate                 : Frames per second
FrameRate/String          : Frames per second (with measurement)
FrameRate_Num             : Frames per second, numerator
FrameRate_Den             : Frames per second, denominator
FrameRate_Minimum         : Minimum Frames per second
FrameRate_Minimum/String  : Minimum Frames per second (with measurement)
FrameRate_Nominal         : Nominal Frames per second
FrameRate_Nominal/String  : Nominal Frames per second (with measurement)
FrameRate_Maximum         : Maximum Frames per second
FrameRate_Maximum/String  : Maximum Frames per second (with measurement)
FrameRate_Original        : Original (in the raw stream) Frames per second
FrameRate_Original/String : Original (in the raw stream) Frames per second (with measurement)
FrameCount                : Number of frames
ElementCount              : Number of displayed elements
Source_FrameCount         : Source Number of frames
ColorSpace
ChromaSubsampling
Resolution                : Deprecated, do not use in new projects
Resolution/String         : Deprecated, do not use in new projects
BitDepth
BitDepth/String
Compression_Mode          : Compression mode (Lossy or Lossless)
Compression_Mode/String   : Compression mode (Lossy or Lossless)
Compression_Ratio         : Current stream size divided by uncompressed stream size
Delay                     : Delay fixed in the stream (relative) IN MS
Delay/String              : Delay with measurement
Delay/String1             : Delay with measurement
Delay/String2             : Delay with measurement
Delay/String3             : Delay in format : HH:MM:SS.MMM
Delay/String4             : Delay in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Delay/String5             : Delay in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Delay_Settings            : Delay settings (in case of timecode for example)
Delay_DropFrame           : Delay drop frame
Delay_Source              : Delay source (Container or Stream or empty)
Delay_Source/String       : Delay source (Container or Stream or empty)
Delay_Original            : Delay fixed in the raw stream (relative) IN MS
Delay_Original/String     : Delay with measurement
Delay_Original/String1    : Delay with measurement
Delay_Original/String2    : Delay with measurement
Delay_Original/String3    : Delay in format: HH:MM:SS.MMM
Delay_Original/String4    : Delay in format: HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Delay_Original/String5    : Delay in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Delay_Original_Settings   : Delay settings (in case of timecode for example)
Delay_Original_DropFrame  : Delay drop frame info
Delay_Original_Source     : Delay source (Stream or empty)
Video_Delay               : Delay fixed in the stream (absolute / video)
Video_Delay/String
Video_Delay/String1
Video_Delay/String2
Video_Delay/String3
Video_Delay/String4
Video_Delay/String5
Video0_Delay              : Deprecated, do not use in new projects
Video0_Delay/String       : Deprecated, do not use in new projects
Video0_Delay/String1      : Deprecated, do not use in new projects
Video0_Delay/String2      : Deprecated, do not use in new projects
Video0_Delay/String3      : Deprecated, do not use in new projects
Video0_Delay/String4      : Deprecated, do not use in new projects
Video0_Delay/String5      : Deprecated, do not use in new projects
StreamSize                : Streamsize in bytes
StreamSize/String         : Streamsize in with percentage value
StreamSize/String1
StreamSize/String2
StreamSize/String3
StreamSize/String4
StreamSize/String5        : Streamsize in with percentage value
StreamSize_Proportion     : Stream size divided by file size
StreamSize_Demuxed        : StreamSize in bytes of hte stream after demux
StreamSize_Demuxed/String : StreamSize_Demuxed in with percentage value
StreamSize_Demuxed/String1
StreamSize_Demuxed/String2
StreamSize_Demuxed/String3
StreamSize_Demuxed/String4
StreamSize_Demuxed/String : StreamSize_Demuxed in with percentage value (note: theoritical value, not for real use)
Source_StreamSize         : Source Streamsize in bytes
Source_StreamSize/String  : Source Streamsize in with percentage value
Source_StreamSize/String1
Source_StreamSize/String2
Source_StreamSize/String3
Source_StreamSize/String4
Source_StreamSize/String5 : Source Streamsize in with percentage value
Source_StreamSize_Proport : Source Stream size divided by file size
StreamSize_Encoded        : Encoded Streamsize in bytes
StreamSize_Encoded/String : Encoded Streamsize in with percentage value
StreamSize_Encoded/String1
StreamSize_Encoded/String2
StreamSize_Encoded/String3
StreamSize_Encoded/String4
StreamSize_Encoded/String : Encoded Streamsize in with percentage value
StreamSize_Encoded_Propor : Encoded Stream size divided by file size
Source_StreamSize_Encoded : Source Encoded Streamsize in bytes
Source_StreamSize_Encoded : Source Encoded Streamsize in with percentage value
Source_StreamSize_Encoded/String1
Source_StreamSize_Encoded/String2
Source_StreamSize_Encoded/String3
Source_StreamSize_Encoded/String4
Source_StreamSize_Encoded : Source Encoded Streamsize in with percentage value
Source_StreamSize_Encoded : Source Encoded Stream size divided by file size
Title                     : Name of the track
Encoded_Application       : Name of the software package used to create the file, such as Microsoft WaveEdit
Encoded_Application/Strin : Name of the software package used to create the file, such as Microsoft WaveEdit, trying to have the format 'CompanyName ProductName (OperatingSystem) Version (Date)'
Encoded_Application_Compa : Name of the company
Encoded_Application_Name  : Name of the product
Encoded_Application_Versi : Version of the product
Encoded_Application_Url   : Name of the software package used to create the file, such as Microsoft WaveEdit.
Encoded_Library           : Software used to create the file
Encoded_Library/String    : Software used to create the file, trying to have the format 'CompanyName ProductName (OperatingSystem) Version (Date)'
Encoded_Library_CompanyNa : Name of the company
Encoded_Library_Name      : Name of the the encoding-software
Encoded_Library_Version   : Version of encoding-software
Encoded_Library_Date      : Release date of software
Encoded_Library_Settings  : Parameters used by the software
Encoded_OperatingSystem   : Operating System of encoding-software
Language                  : Language (2-letter ISO 639-1 if exists, else 3-letter ISO 639-2, and with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn)
Language/String           : Language (full)
Language/String1          : Language (full)
Language/String2          : Language (2-letter ISO 639-1 if exists, else empty)
Language/String3          : Language (3-letter ISO 639-2 if exists, else empty)
Language/String4          : Language (2-letter ISO 639-1 if exists with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn, else empty)
Language_More             : More info about Language (e.g. Director's Comment)
ServiceKind               : Service kind, e.g. visually impaired, commentary, voice over
ServiceKind/String        : Service kind (full)
Disabled                  : Set if that track should not be used
Disabled/String           : Set if that track should not be used
Default                   : Set if that track should be used if no language found matches the user preference.
Default/String            : Set if that track should be used if no language found matches the user preference.
Forced                    : Set if that track should be used if no language found matches the user preference.
Forced/String             : Set if that track should be used if no language found matches the user preference.
AlternateGroup            : Number of a group in order to provide versions of the same track
AlternateGroup/String     : Number of a group in order to provide versions of the same track
Summary
Encoded_Date              : The time that the encoding of this item was completed began.
Tagged_Date               : The time that the tags were done for this item.
Encryption

Other
Count                     : Count of objects available in this stream
Status                    : bit field (0=IsAccepted, 1=IsFilled, 2=IsUpdated, 3=IsFinished)
StreamCount               : Count of streams of that kind available
StreamKind                : Stream type name
StreamKind/String         : Stream type name
StreamKindID              : Number of the stream (base=0)
StreamKindPos             : When multiple streams, number of the stream (base=1)
StreamOrder               : Stream order in the file, whatever is the kind of stream (base=0)
FirstPacketOrder          : Order of the first fully decodable packet met in the file, whatever is the kind of stream (base=0)
Inform                    : Last **Inform** call
ID                        : The ID for this stream in this file
ID/String                 : The ID for this stream in this file
OriginalSourceMedium_ID   : The ID for this stream in the original medium of the material
OriginalSourceMedium_ID/S : The ID for this stream in the original medium of the material
UniqueID                  : The unique ID for this stream, should be copied with stream copy
UniqueID/String           : The unique ID for this stream, should be copied with stream copy
MenuID                    : The menu ID for this stream in this file
MenuID/String             : The menu ID for this stream in this file
Type                      : Type
Format                    : Format used
Format/String             : Format used + additional features
Format/Info               : Info about Format
Format/Url                : Link
Format_Commercial         : Commercial name used by vendor for theses setings or Format field if there is no difference
Format_Commercial_IfAny   : Commercial name used by vendor for theses setings if there is one
Format_Version            : Version of this format
Format_Profile            : Profile of the Format
Format_Compression        : Compression method used
Format_Settings           : Settings needed for decoder used
Format_AdditionalFeatures : Format features needed for fully supporting the content
MuxingMode                : How this file is muxed in the container
CodecID                   : Codec ID (found in some containers)
CodecID/String            : Codec ID (found in some containers)
CodecID/Info              : Info about this codec
CodecID/Hint              : A hint/popular name for this codec
CodecID/Url               : A link to more details about this codec ID
CodecID_Description       : Manual description given by the container
Duration                  : Play time of the stream in ms
Duration/String           : Play time in format : XXx YYy only, YYy omited if zero
Duration/String1          : Play time in format : HHh MMmn SSs MMMms, XX omited if zero
Duration/String2          : Play time in format : XXx YYy only, YYy omited if zero
Duration/String3          : Play time in format : HH:MM:SS.MMM
Duration/String4          : Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Duration/String5          : Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Duration_Start
Duration_End
Source_Duration           : Source Play time of the stream, in ms
Source_Duration/String    : Source Play time in format : XXx YYy only, YYy omited if zero
Source_Duration/String1   : Source Play time in format : HHh MMmn SSs MMMms, XX omited if zero
Source_Duration/String2   : Source Play time in format : XXx YYy only, YYy omited if zero
Source_Duration/String3   : Source Play time in format : HH:MM:SS.MMM
Source_Duration/String4   : Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Source_Duration/String5   : Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in ms
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : HHh MMmn SSs MMMms, XX omited if zero
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Source_Duration_FirstFram : Source Duration of the first frame if it is longer than others, in format : HH:MM:SS.MMM
Source_Duration_FirstFram : Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Source_Duration_FirstFram : Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in ms
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : HHh MMmn SSs MMMms, XX omited if zero
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : XXx YYy only, YYy omited if zero
Source_Duration_LastFrame : Source Duration of the last frame if it is longer than others, in format : HH:MM:SS.MMM
Source_Duration_LastFrame : Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Source_Duration_LastFrame : Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
BitRate_Mode              : Bit rate mode (VBR, CBR)
BitRate_Mode/String       : Bit rate mode (Variable, Cconstant)
BitRate                   : Bit rate in bps
BitRate/String            : Bit rate (with measurement)
BitRate_Minimum           : Minimum Bit rate in bps
BitRate_Minimum/String    : Minimum Bit rate (with measurement)
BitRate_Nominal           : Nominal Bit rate in bps
BitRate_Nominal/String    : Nominal Bit rate (with measurement)
BitRate_Maximum           : Maximum Bit rate in bps
BitRate_Maximum/String    : Maximum Bit rate (with measurement)
BitRate_Encoded           : Encoded (with forced padding) bit rate in bps, if some container padding is present
BitRate_Encoded/String    : Encoded (with forced padding) bit rate (with measurement), if some container padding is present
FrameRate                 : Frames per second
FrameRate/String          : Frames per second (with measurement)
FrameRate_Num             : Frames per second, numerator
FrameRate_Den             : Frames per second, denominator
FrameCount                : Number of frames
Source_FrameCount         : Source Number of frames
Delay                     : Delay fixed in the stream (relative) IN MS
Delay/String              : Delay with measurement
Delay/String1             : Delay with measurement
Delay/String2             : Delay with measurement
Delay/String3             : Delay in format : HH:MM:SS.MMM
Delay/String4             : Delay in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Delay/String5             : Delay in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Delay_Settings            : Delay settings (in case of timecode for example)
Delay_DropFrame           : Delay drop frame
Delay_Source              : Delay source (Container or Stream or empty)
Delay_Source/String       : Delay source (Container or Stream or empty)
Delay_Original            : Delay fixed in the raw stream (relative) IN MS
Delay_Original/String     : Delay with measurement
Delay_Original/String1    : Delay with measurement
Delay_Original/String2    : Delay with measurement
Delay_Original/String3    : Delay in format: HH:MM:SS.MMM
Delay_Original/String4    : Delay in format: HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Delay_Original/String5    : Delay in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Delay_Original_Settings   : Delay settings (in case of timecode for example)
Delay_Original_DropFrame  : Delay drop frame info
Delay_Original_Source     : Delay source (Stream or empty)
Video_Delay               : Delay fixed in the stream (absolute / video)
Video_Delay/String
Video_Delay/String1
Video_Delay/String2
Video_Delay/String3
Video_Delay/String4
Video_Delay/String5
Video0_Delay              : Deprecated, do not use in new projects
Video0_Delay/String       : Deprecated, do not use in new projects
Video0_Delay/String1      : Deprecated, do not use in new projects
Video0_Delay/String2      : Deprecated, do not use in new projects
Video0_Delay/String3      : Deprecated, do not use in new projects
Video0_Delay/String4      : Deprecated, do not use in new projects
Video0_Delay/String5      : Deprecated, do not use in new projects
TimeStamp_FirstFrame      : TimeStamp fixed in the stream (relative) IN MS
TimeStamp_FirstFrame/Stri : TimeStamp with measurement
TimeStamp_FirstFrame/Stri : TimeStamp with measurement
TimeStamp_FirstFrame/Stri : TimeStamp with measurement
TimeStamp_FirstFrame/Stri : TimeStamp in format : HH:MM:SS.MMM
TimeStamp_FirstFrame/Stri : TimeStamp in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
TimeStamp_FirstFrame/Stri : TimeStamp in format : HH:MM:SS.mmm (HH:MM:SS:FF)
TimeCode_FirstFrame       : Time code in HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available format
TimeCode_Settings         : Time code settings
TimeCode_Striped          : Time code is striped (only 1st time code, no discontinuity)
TimeCode_Striped/String   : Time code is striped (only 1st time code, no discontinuity)
StreamSize                : Streamsize in bytes
StreamSize/String         : Streamsize in with percentage value
StreamSize/String1
StreamSize/String2
StreamSize/String3
StreamSize/String4
StreamSize/String5        : Streamsize in with percentage value
StreamSize_Proportion     : Stream size divided by file size
StreamSize_Demuxed        : StreamSize in bytes of hte stream after demux
StreamSize_Demuxed/String : StreamSize_Demuxed in with percentage value
StreamSize_Demuxed/String1
StreamSize_Demuxed/String2
StreamSize_Demuxed/String3
StreamSize_Demuxed/String4
StreamSize_Demuxed/String : StreamSize_Demuxed in with percentage value (note: theoritical value, not for real use)
Source_StreamSize         : Source Streamsize in bytes
Source_StreamSize/String  : Source Streamsize in with percentage value
Source_StreamSize/String1
Source_StreamSize/String2
Source_StreamSize/String3
Source_StreamSize/String4
Source_StreamSize/String5 : Source Streamsize in with percentage value
Source_StreamSize_Proport : Source Stream size divided by file size
StreamSize_Encoded        : Encoded Streamsize in bytes
StreamSize_Encoded/String : Encoded Streamsize in with percentage value
StreamSize_Encoded/String1
StreamSize_Encoded/String2
StreamSize_Encoded/String3
StreamSize_Encoded/String4
StreamSize_Encoded/String : Encoded Streamsize in with percentage value
StreamSize_Encoded_Propor : Encoded Stream size divided by file size
Source_StreamSize_Encoded : Source Encoded Streamsize in bytes
Source_StreamSize_Encoded : Source Encoded Streamsize in with percentage value
Source_StreamSize_Encoded/String1
Source_StreamSize_Encoded/String2
Source_StreamSize_Encoded/String3
Source_StreamSize_Encoded/String4
Source_StreamSize_Encoded : Source Encoded Streamsize in with percentage value
Source_StreamSize_Encoded : Source Encoded Stream size divided by file size
Title                     : Name of this menu
Language                  : Language (2-letter ISO 639-1 if exists, else 3-letter ISO 639-2, and with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn)
Language/String           : Language (full)
Language/String1          : Language (full)
Language/String2          : Language (2-letter ISO 639-1 if exists, else empty)
Language/String3          : Language (3-letter ISO 639-2 if exists, else empty)
Language/String4          : Language (2-letter ISO 639-1 if exists with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn, else empty)
Language_More             : More info about Language (e.g. Director's Comment)
ServiceKind               : Service kind, e.g. visually impaired, commentary, voice over
ServiceKind/String        : Service kind (full)
Disabled                  : Set if that track should not be used
Disabled/String           : Set if that track should not be used
Default                   : Set if that track should be used if no language found matches the user preference.
Default/String            : Set if that track should be used if no language found matches the user preference.
Forced                    : Set if that track should be used if no language found matches the user preference.
Forced/String             : Set if that track should be used if no language found matches the user preference.
AlternateGroup            : Number of a group in order to provide versions of the same track
AlternateGroup/String     : Number of a group in order to provide versions of the same track

Image
Count                     : Count of objects available in this stream
Status                    : bit field (0=IsAccepted, 1=IsFilled, 2=IsUpdated, 3=IsFinished)
StreamCount               : Count of streams of that kind available
StreamKind                : Stream type name
StreamKind/String         : Stream type name
StreamKindID              : Number of the stream (base=0)
StreamKindPos             : When multiple streams, number of the stream (base=1)
StreamOrder               : Stream order in the file, whatever is the kind of stream (base=0)
FirstPacketOrder          : Order of the first fully decodable packet met in the file, whatever is the kind of stream (base=0)
Inform                    : Last **Inform** call
ID                        : The ID for this stream in this file
ID/String                 : The ID for this stream in this file
OriginalSourceMedium_ID   : The ID for this stream in the original medium of the material
OriginalSourceMedium_ID/S : The ID for this stream in the original medium of the material
UniqueID                  : The unique ID for this stream, should be copied with stream copy
UniqueID/String           : The unique ID for this stream, should be copied with stream copy
MenuID                    : The menu ID for this stream in this file
MenuID/String             : The menu ID for this stream in this file
Title                     : Name of the track
Format                    : Format used
Format/String             : Format used + additional features
Format/Info               : Info about Format
Format/Url                : Link
Format_Commercial         : Commercial name used by vendor for theses setings or Format field if there is no difference
Format_Commercial_IfAny   : Commercial name used by vendor for theses setings if there is one
Format_Version            : Version of this format
Format_Profile            : Profile of the Format
Format_Settings_Endianness
Format_Settings_Packing
Format_Compression        : Compression method used
Format_Settings           : Settings needed for decoder used
Format_Settings_Wrapping  : Wrapping mode (Frame wrapped or Clip wrapped)
Format_AdditionalFeatures : Format features needed for fully supporting the content
InternetMediaType         : Internet Media Type (aka MIME Type, Content-Type)
CodecID                   : Codec ID (found in some containers)
CodecID/String            : Codec ID (found in some containers)
CodecID/Info              : Info about codec ID
CodecID/Hint              : A hint for this codec ID
CodecID/Url               : A link for more details about this codec ID
CodecID_Description       : Manual description given by the container
Codec                     : Deprecated, do not use in new projects
Codec/String              : Deprecated, do not use in new projects
Codec/Family              : Deprecated, do not use in new projects
Codec/Info                : Deprecated, do not use in new projects
Codec/Url                 : Deprecated, do not use in new projects
Width                     : Width (aperture size if present) in pixel
Width/String              : Width (aperture size if present) with measurement (pixel)
Width_Offset              : Offset between original width and displayed width (aperture size) in pixel
Width_Offset/String       : Offset between original width and displayed width (aperture size)  in pixel
Width_Original            : Original (in the raw stream) width in pixel
Width_Original/String     : Original (in the raw stream) width with measurement (pixel)
Height                    : Height (aperture size if present) in pixel
Height/String             : Height (aperture size if present) with measurement (pixel)
Height_Offset             : Offset between original height and displayed height (aperture size) in pixel
Height_Offset/String      : Offset between original height and displayed height (aperture size)  in pixel
Height_Original           : Original (in the raw stream) height in pixel
Height_Original/String    : Original (in the raw stream) height with measurement (pixel)
PixelAspectRatio          : Pixel Aspect ratio
PixelAspectRatio/String   : Pixel Aspect ratio
PixelAspectRatio_Original : Original (in the raw stream) Pixel Aspect ratio
PixelAspectRatio_Original : Original (in the raw stream) Pixel Aspect ratio
DisplayAspectRatio        : Display Aspect ratio
DisplayAspectRatio/String : Display Aspect ratio
DisplayAspectRatio_Origin : Original (in the raw stream) Display Aspect ratio
DisplayAspectRatio_Origin : Original (in the raw stream) Display Aspect ratio
ColorSpace
ChromaSubsampling
Resolution                : Deprecated, do not use in new projects
Resolution/String         : Deprecated, do not use in new projects
BitDepth
BitDepth/String
Compression_Mode          : Compression mode (Lossy or Lossless)
Compression_Mode/String   : Compression mode (Lossy or Lossless)
Compression_Ratio         : Current stream size divided by uncompressed stream size
StreamSize                : Stream size in bytes
StreamSize/String
StreamSize/String1
StreamSize/String2
StreamSize/String3
StreamSize/String4
StreamSize/String5        : With proportion
StreamSize_Proportion     : Stream size divided by file size
StreamSize_Demuxed        : StreamSize in bytes of hte stream after demux
StreamSize_Demuxed/String : StreamSize_Demuxed in with percentage value
StreamSize_Demuxed/String1
StreamSize_Demuxed/String2
StreamSize_Demuxed/String3
StreamSize_Demuxed/String4
StreamSize_Demuxed/String : StreamSize_Demuxed in with percentage value (note: theoritical value, not for real use)
Encoded_Library           : Software used to create the file
Encoded_Library/String    : Software used to create the file
Encoded_Library_Name      : Info from the software
Encoded_Library_Version   : Version of software
Encoded_Library_Date      : Release date of software
Encoded_Library_Settings  : Parameters used by the software
Language                  : Language (2-letter ISO 639-1 if exists, else 3-letter ISO 639-2, and with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn)
Language/String           : Language (full)
Language/String1          : Language (full)
Language/String2          : Language (2-letter ISO 639-1 if exists, else empty)
Language/String3          : Language (3-letter ISO 639-2 if exists, else empty)
Language/String4          : Language (2-letter ISO 639-1 if exists with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn, else empty)
Language_More             : More info about Language (e.g. Director's Comment)
ServiceKind               : Service kind, e.g. visually impaired, commentary, voice over
ServiceKind/String        : Service kind (full)
Disabled                  : Set if that track should not be used
Disabled/String           : Set if that track should not be used
Default                   : Set if that track should be used if no language found matches the user preference.
Default/String            : Set if that track should be used if no language found matches the user preference.
Forced                    : Set if that track should be used if no language found matches the user preference.
Forced/String             : Set if that track should be used if no language found matches the user preference.
AlternateGroup            : Number of a group in order to provide versions of the same track
AlternateGroup/String     : Number of a group in order to provide versions of the same track
Summary
Encoded_Date              : The time that the encoding of this item was completed began.
Tagged_Date               : The time that the tags were done for this item.
Encryption
colour_description_presen : Presence of colour description
colour_primaries          : Chromaticity coordinates of the source primaries
transfer_characteristics  : Opto-electronic transfer characteristic of the source picture
matrix_coefficients       : Matrix coefficients used in deriving luma and chroma signals from the green, blue, and red primaries
colour_description_presen : Presence of colour description
colour_primaries_Original : Chromaticity coordinates of the source primaries
transfer_characteristics_ : Opto-electronic transfer characteristic of the source picture
matrix_coefficients_Origi : Matrix coefficients used in deriving luma and chroma signals from the green, blue, and red primaries

Menu
Count                     : Count of objects available in this stream
Status                    : bit field (0=IsAccepted, 1=IsFilled, 2=IsUpdated, 3=IsFinished)
StreamCount               : Count of streams of that kind available
StreamKind                : Stream type name
StreamKind/String         : Stream type name
StreamKindID              : Number of the stream (base=0)
StreamKindPos             : When multiple streams, number of the stream (base=1)
StreamOrder               : Stream order in the file, whatever is the kind of stream (base=0)
FirstPacketOrder          : Order of the first fully decodable packet met in the file, whatever is the kind of stream (base=0)
Inform                    : Last **Inform** call
ID                        : The ID for this stream in this file
ID/String                 : The ID for this stream in this file
OriginalSourceMedium_ID   : The ID for this stream in the original medium of the material
OriginalSourceMedium_ID/S : The ID for this stream in the original medium of the material
UniqueID                  : The unique ID for this stream, should be copied with stream copy
UniqueID/String           : The unique ID for this stream, should be copied with stream copy
MenuID                    : The menu ID for this stream in this file
MenuID/String             : The menu ID for this stream in this file
Format                    : Format used
Format/String             : Format used + additional features
Format/Info               : Info about Format
Format/Url                : Link
Format_Commercial         : Commercial name used by vendor for theses setings or Format field if there is no difference
Format_Commercial_IfAny   : Commercial name used by vendor for theses setings if there is one
Format_Version            : Version of this format
Format_Profile            : Profile of the Format
Format_Compression        : Compression method used
Format_Settings           : Settings needed for decoder used
Format_AdditionalFeatures : Format features needed for fully supporting the content
CodecID                   : Codec ID (found in some containers)
CodecID/String            : Codec ID (found in some containers)
CodecID/Info              : Info about this codec
CodecID/Hint              : A hint/popular name for this codec
CodecID/Url               : A link to more details about this codec ID
CodecID_Description       : Manual description given by the container
Codec                     : Deprecated
Codec/String              : Deprecated
Codec/Info                : Deprecated
Codec/Url                 : Deprecated
Duration                  : Play time of the stream in ms
Duration/String           : Play time in format : XXx YYy only, YYy omited if zero
Duration/String1          : Play time in format : HHh MMmn SSs MMMms, XX omited if zero
Duration/String2          : Play time in format : XXx YYy only, YYy omited if zero
Duration/String3          : Play time in format : HH:MM:SS.MMM
Duration/String4          : Play time in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Duration/String5          : Play time in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Duration_Start
Duration_End
Delay                     : Delay fixed in the stream (relative) IN MS
Delay/String              : Delay with measurement
Delay/String1             : Delay with measurement
Delay/String2             : Delay with measurement
Delay/String3             : Delay in format : HH:MM:SS.MMM
Delay/String4             : Delay in format : HH:MM:SS:FF, last colon replaced by semicolon for drop frame if available
Delay/String5             : Delay in format : HH:MM:SS.mmm (HH:MM:SS:FF)
Delay_Settings            : Delay settings (in case of timecode for example)
Delay_DropFrame           : Delay drop frame
Delay_Source              : Delay source (Container or Stream or empty)
List_StreamKind           : List of programs available
List_StreamPos            : List of programs available
List                      : List of programs available
List/String               : List of programs available
Title                     : Name of this menu
Language                  : Language (2-letter ISO 639-1 if exists, else 3-letter ISO 639-2, and with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn)
Language/String           : Language (full)
Language/String1          : Language (full)
Language/String2          : Language (2-letter ISO 639-1 if exists, else empty)
Language/String3          : Language (3-letter ISO 639-2 if exists, else empty)
Language/String4          : Language (2-letter ISO 639-1 if exists with optional ISO 3166-1 country separated by a dash if available, e.g. en, en-us, zh-cn, else empty)
Language_More             : More info about Language (e.g. Director's Comment)
ServiceKind               : Service kind, e.g. visually impaired, commentary, voice over
ServiceKind/String        : Service kind (full)
ServiceName
ServiceChannel
Service/Url
ServiceProvider
ServiceProvider/Url
ServiceType
NetworkName
Original/NetworkName
Countries
TimeZones
LawRating                 : Depending on the country it's the format of the rating of a movie (P, R, X in the USA, an age in other countries or a URI defining a logo).
LawRating_Reason          : Reason for the law rating
Disabled                  : Set if that track should not be used
Disabled/String           : Set if that track should not be used
Default                   : Set if that track should be used if no language found matches the user preference.
Default/String            : Set if that track should be used if no language found matches the user preference.
Forced                    : Set if that track should be used if no language found matches the user preference.
Forced/String             : Set if that track should be used if no language found matches the user preference.
AlternateGroup            : Number of a group in order to provide versions of the same track
AlternateGroup/String     : Number of a group in order to provide versions of the same track
Chapters_Pos_Begin        : Used by third-party developers to know about the beginning of the chapters list, to be used by Get(Stream_Menu, x, Pos), where Pos is an Integer between Chapters_Pos_Begin and Chapters_Pos_End
Chapters_Pos_End          : Used by third-party developers to know about the end of the chapters list (this position excluded)
*/

BofMediaDetector::BofMediaDetector()
{

}
BofMediaDetector::~BofMediaDetector()
{

}

BOFERR BofMediaDetector::ParseFile(const BofPath &_rPathName, ResultFormat _ResultFormat_E, std::string &_rResult_S)
{
  String Oss;
  Oss = mMediaInfo.Option(__T("info_version"));
  std::wcout << Oss << std::endl;
  Oss = mMediaInfo.Option(__T("info_outputformats"));
  std::wcout << Oss << std::endl;


  return BOF_ERR_NO_ERROR;
}
#if 0

//https://github.com/MediaArea/MediaInfoLib/issues/485
//#include <MediaInfoDLL/MediaInfoDLL.h>
BOFERR BofMediaDetector::ParseFile(const BofPath &_rPathName, ResultFormat _ResultFormat_E, std::string &_rResult_S)
{
  BOFERR Rts_E;
  MediaInfoLib::String Oss;

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
  MediaInfoLib::String Oss;
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

BOFERR BofMediaDetector::Query(MediaStreamType _MediaStreamType_E, const std::string &_rOption_S, InfoType _InfoType_E, std::string &_rResult_S)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  MediaInfoLib::String Oss;
  MediaInfoLib::stream_t MediaStreamType_E;
  MediaInfoLib::info_t InfoType_E;
  std::wstring Option_WS;

  switch (_MediaStreamType_E)
  {
    case MediaStreamType::General:
      MediaStreamType_E = MediaInfoLib::Stream_General;
      break;

    case MediaStreamType::Video:
      MediaStreamType_E = MediaInfoLib::Stream_Video;
      break;

    case MediaStreamType::Audio:
      MediaStreamType_E = MediaInfoLib::Stream_Audio;
      break;

    case MediaStreamType::Text:
      MediaStreamType_E = MediaInfoLib::Stream_Text;
      break;

    case MediaStreamType::Other:
      MediaStreamType_E = MediaInfoLib::Stream_Other;
      break;

    case MediaStreamType::Image:
      MediaStreamType_E = MediaInfoLib::Stream_Image;
      break;

    case MediaStreamType::Menu:
      MediaStreamType_E = MediaInfoLib::Stream_Menu;
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
        InfoType_E = MediaInfoLib::Info_Name;
        break;

      case InfoType::Text:
        InfoType_E = MediaInfoLib::Info_Text;
        break;

      case InfoType::Measure:
        InfoType_E = MediaInfoLib::Info_Measure;
        break;

      case InfoType::Options:
        InfoType_E = MediaInfoLib::Info_Options;
        break;

      case InfoType::Name_Text:
        InfoType_E = MediaInfoLib::Info_Name_Text;
        break;

      case InfoType::Measure_Text:
        InfoType_E = MediaInfoLib::Info_Measure_Text;
        break;

      case InfoType::Info:
        InfoType_E = MediaInfoLib::Info_Info;
        break;

      case InfoType::HowTo:
        InfoType_E = MediaInfoLib::Info_HowTo;
        break;

      case InfoType::Domain:
        InfoType_E = MediaInfoLib::Info_Domain;
        break;

      default:
        Rts_E = BOF_ERR_EPROTOTYPE;
        break;
    }
  }
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Option_WS = Bof_Utf8ToUtf16(_rOption_S);
    Oss = mMediaInfo.Get(MediaStreamType_E, 0, Option_WS, InfoType_E, MediaInfoLib::Info_Name);
    //Oss = mMediaInfo.Get(MediaStreamType_E, 0, _rOption_S, InfoType_E, MediaInfoLib::Info_Name);
    //std::wcout << Oss << std::endl;
    _rResult_S = Bof_Utf16ToUtf8(Oss);

    //Oss += mMediaInfo.Get(MediaInfoLib::Stream_General, 0, __T("FileSize"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
    //Ztring MediaInfo_Internal::Get(stream_t StreamKind, size_t StreamPos, const String & Parameter, info_t KindOfInfo, info_t KindOfSearch)
  }

  return Rts_E;
}
#endif

BOFERR BofMediaDetector::ParseBuffer(const BOF_BUFFER &_rBuffer_X, ResultFormat _ResultFormat_E, std::string &_rResult_S, uint64_t &_rOffsetInBuffer_U64)
{
  return BOF_ERR_NO_ERROR;
}
BOFERR BofMediaDetector::Query(MediaStreamType _MediaStreamType_E, const std::string &_rOption_S, InfoType _InfoType_E, std::string &_rResult_S)
{
  return BOF_ERR_NO_ERROR;
}


#if 0
BOFERR BofMediaDetector::Info(const BofPath &_rPathName)
{
  BOFERR Rts_E;
  Oss += __T("\r\n\r\nInform with Complete=false\r\n");
  mMediaInfo.Option(__T("Complete"));
  Oss += mMediaInfo.Inform();
  std::wcout << Oss;

  Oss = __T("\r\n\r\nInform with Complete=true\r\n");
  mMediaInfo.Option(__T("Complete"), __T("1"));
  Oss += mMediaInfo.Inform();
  std::wcout << Oss;
  FILE *F = fopen("./data/pexels-pixabay-247431.jpg", "rb");
  if (F)
  {

    //From: preparing a memory buffer for reading
    const uint32_t BUFFER_SIZE = 4 * 1024;    //48 PNG 3408 Jpeg
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
#endif

#if 0
    TEST(Muse_Storage_FileSystem_Test, MediaInfo)
    {
      MediaInfoLib::MediaInfo mMediaInfo;
      MediaInfoLib::toStringStream SS;
      MediaInfoLib::String Oss;

      goto l;

      Oss = mMediaInfo.Option(__T("Info_Version"), __T("0.7.13;MediaInfoDLL_Example_MSVC;0.7.13"));

      Oss += __T("\r\n\r\nInfo_Parameters\r\n");
      Oss += mMediaInfo.Option(__T("Info_Parameters"));
      std::wcout << Oss;

      Oss = __T("\r\n\r\nInfo_Codecs\r\n");
      Oss += mMediaInfo.Option(__T("Info_Codecs"));
      std::wcout << Oss;

      //An example of how to use the library
      Oss = __T("\r\n\r\nOpen\r\n");
      size_t Sz = mMediaInfo.Open(__T("./data/pexels-alexander-grey-1149347.png"));
      //Sz = mMediaInfo.Open("./data/pexels-christian-heitz-842711.jpg");

      Oss += __T("\r\n\r\nInform with Complete=false\r\n");
      mMediaInfo.Option(__T("Complete"));
      Oss += mMediaInfo.Inform();
      std::wcout << Oss;

      Oss = __T("\r\n\r\nInform with Complete=true\r\n");
      mMediaInfo.Option(__T("Complete"), __T("1"));
      Oss += mMediaInfo.Inform();
      std::wcout << Oss;

      Oss = __T("\r\n\r\nCustom Inform\r\n");
      mMediaInfo.Option(__T("Inform"), __T("General;Example : FileSize=%FileSize%"));
      Oss += mMediaInfo.Inform();
      std::wcout << Oss;

      Oss = __T("\r\n\r\nGet with Stream=General and Parameter=\"FileSize\"\r\n");
      Oss += mMediaInfo.Get(MediaInfoLib::Stream_General, 0, __T("FileSize"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
      std::wcout << Oss;

      Oss = __T("\r\n\r\nGetI with Stream=General and Parameter=46\r\n");
      Oss += mMediaInfo.Get(MediaInfoLib::Stream_General, 0, 46, MediaInfoLib::Info_Text);
      std::wcout << Oss;

      Oss = __T("\r\n\r\nCount_Get with StreamKind=Stream_Audio\r\n");
#ifdef __MINGW32__
      Char *C1 = new Char[33];
      _itot(mMediaInfo.Count_Get(Stream_Audio), C1, 10);
      Oss += C1;
      delete[] C1;
#else
      SS << std::setbase(10) << mMediaInfo.Count_Get(MediaInfoLib::Stream_Audio);
      Oss += SS.str();
#endif
      std::wcout << Oss;

      Oss = __T("\r\n\r\nGet with Stream=General and Parameter=\"AudioCount\"\r\n");
      Oss += mMediaInfo.Get(MediaInfoLib::Stream_General, 0, __T("AudioCount"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
      std::wcout << Oss;

      Oss = __T("\r\n\r\nGet with Stream=Audio and Parameter=\"StreamCount\"\r\n");
      Oss += mMediaInfo.Get(MediaInfoLib::Stream_Audio, 0, __T("StreamCount"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
      std::wcout << Oss;

      Oss = __T("\r\n\r\nClose\r\n");
      mMediaInfo.Close();

#ifdef _UNICODE
      std::wcout << Oss;
#else
      std::cout << Oss;
#endif

l:
      //From: preparing an example file for reading
      //  FILE *F = fopen("./data/pexels-alexander-grey-1149347.png", "rb"); //You can use something else than a file
      //FILE *F = fopen("./data/pexels-christian-heitz-842711.jpg", "rb"); //You can use something else than a file
      FILE *F = fopen("./data/pexels-pixabay-247431.jpg", "rb");
      if (F)
      {

        //From: preparing a memory buffer for reading
        const uint32_t BUFFER_SIZE = 4 * 1024;    //48 PNG 3408 Jpeg
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
        /*
        Oss = __T("\r\n\r\nCustom Inform\r\n");
        mMediaInfo.Option(__T("Inform"), __T("General;Example : FileSize=%FileSize%"));
        Oss += mMediaInfo.Inform();
        std::wcout << Oss;

        Oss = __T("\r\n\r\nGet with Stream=General and Parameter=\"FileSize\"\r\n");
        Oss += mMediaInfo.Get(MediaInfoLib::Stream_General, 0, __T("Format"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        std::wcout << Oss;
        */
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_General, 0, __T("Format"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_General, 0, __T("Format/Info"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_General, 0, __T("FileSize"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_General, 0, __T("FileSize/String2"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;

        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("Format"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("Format/Info"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("Format_Compression"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("Width"), MediaInfoLib::Info_Name, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("Width"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("Width"), MediaInfoLib::Info_Measure, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("Width"), MediaInfoLib::Info_Options, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("Width"), MediaInfoLib::Info_Name_Text, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("Width"), MediaInfoLib::Info_Measure_Text, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("Width"), MediaInfoLib::Info_Info, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("Width"), MediaInfoLib::Info_HowTo, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("Width"), MediaInfoLib::Info_Domain, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;

        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("Height"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("ColorSpace"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("BitDepth"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("Compression_Mode"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_Image, 0, __T("StreamSize"), MediaInfoLib::Info_Text, MediaInfoLib::Info_Name);
        std::wcout << Oss << std::endl;


        mMediaInfo.Option(__T("Complete"));
        Oss += mMediaInfo.Inform();
        std::wcout << Oss;




        //Get() example
        Oss = mMediaInfo.Get(MediaInfoLib::Stream_General, 0, __T("Inform"));
        Oss += mMediaInfo.Get(MediaInfoLib::Stream_General, 0, __T("Format"));
        Oss += mMediaInfo.Get(MediaInfoLib::Stream_General, 0, __T("Width"));
        Oss += mMediaInfo.Get(MediaInfoLib::Stream_General, 0, __T("Height"));
        Oss += mMediaInfo.Get(MediaInfoLib::Stream_General, 0, __T("PixelAspectRatio"));

#ifdef _UNICODE
        std::wcout << Oss;
#else
        std::cout << Oss;
#endif
      }
    }
#endif


    END_BOF_NAMESPACE()