/*
Copyright (c) 2000-2006, Sci. All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
PURPOSE.

This module contains routines for initialising the 2d part of the Bof library
(Bernard's Object Framework) and various utility functions.

Name:        Bof2d.h
Author:      Bernard HARMEL: onbings@dscloud.me
Revision:    1.0

Rem:         Nothing

History:

V 1.00  Sep 30 2000  BHA : Initial release
*/

#pragma once

/*** Include ****************************************************************/

#include <cstdint>
#include <bofstd/bofstd.h>
#include <bofstd/bofpath.h>
#include <bofstd/bofsystem.h>
#include <MediaInfo/MediaInfo.h>

BEGIN_BOF_NAMESPACE()

/*** Defines ****************************************************************/

#define BOF_FOURCC(a, b, c, d)                                     \
  (((uint32_t)(a)) | ((uint32_t)(b) << 8) |       /* NOLINT */ \
   ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24)) /* NOLINT */

enum BOF_ROTATION
{
  BOF_ROTATION_0 = 0,      // No rotation.
  BOF_ROTATION_90 = 90,    // Rotate 90 degrees clockwise.
  BOF_ROTATION_180 = 180,  // Rotate 180 degrees.
  BOF_ROTATION_270 = 270,  // Rotate 270 degrees clockwise.
} ;

#define BOF_BI_RGB          0L
#define BOF_BI_RLE8         1L
#define BOF_BI_RLE4         2L
#define BOF_BI_BITFIELDS    3L

#define BOF_TGA_NULL        0
#define BOF_TGA_MAP         1          // uncompressed color mapped
#define BOF_TGA_RGB         2          // uncompressed true color
#define BOF_TGA_MONO        3          // uncompressed grey scale
#define BOF_TGA_RLEMAP      9          // RLE compressed color mapped
#define BOF_TGA_RLERGB      10         // RLE compressed true color
#define BOF_TGA_RLEMONO     11         // RLE compressed grey scale
#define BOF_TGA_COMPMAP     32         // obsolete types
#define BOF_TGA_COMPMAP4    33

// Definitions for interleave flag.
#define BOF_TGA_IL_NONE     0
#define BOF_TGA_IL_TWO      1
#define BOF_TGA_IL_FOUR     2


/*** Structures *************************************************************/
/*!
* Summary
* Definition of a size entity
*/
struct BOFSTD_EXPORT BOF_SIZE
{
		uint32_t Width_U32;               /*! Width value*/
		uint32_t Height_U32;              /*! Height value*/
		BOF_SIZE()
		{
			Reset();
		}

		BOF_SIZE(uint32_t _Width_U32, uint32_t _Height_U32)
		{
			Width_U32 = _Width_U32;
			Height_U32 = _Height_U32;
		}

		void Reset()
		{
			Width_U32 = 0;
			Height_U32 = 0;
		}
};

struct BOF_RECT;

/*!
* Summary
* Definition of a 2D point entity
*/
struct BOFSTD_EXPORT BOF_POINT_2D
{
		int32_t x_S32;                    /*! Point x coordinate*/
		int32_t y_S32;                    /*! Point y coordinate*/
		BOF_POINT_2D()
		{
			Reset();
		}

		BOF_POINT_2D(int32_t _x_S32, int32_t _y_S32)
		{
			x_S32 = _x_S32;
			y_S32 = _y_S32;
		}

		void Reset()
		{
			x_S32 = 0;
			y_S32 = 0;
		}

		bool IsInside(const BOF_RECT &_rBox) const;
//  {
//    return ((_rBox.Left_S32 <= x_S32) && (_rBox.Top_S32 <= y_S32) && (_rBox.Right_S32 >= x_S32) && (_rBox.Bottom_S32 >= y_S32));
//  }
};

/*!
* Summary
* Definition of a 3D point entity
*/
struct BOFSTD_EXPORT BOF_POINT_3D
{
		int32_t x_S32;                    /*! Point x coordinate*/
		int32_t y_S32;                    /*! Point y coordinate*/
		int32_t z_S32;                    /*! Point z coordinate*/
		BOF_POINT_3D()
		{
			Reset();
		}

		BOF_POINT_3D(int32_t _x_S32, int32_t _y_S32, int32_t _z_S32)
		{
			x_S32 = _x_S32;
			y_S32 = _y_S32;
			z_S32 = _z_S32;
		}

		void Reset()
		{
			x_S32 = 0;
			y_S32 = 0;
			z_S32 = 0;
		}
};

#if 0
struct BOFSTD_EXPORT BOF_SIZE_FLOAT
{
	float    Width_f;                    /*! Width value*/
	float    Height_f;                   /*! Height value*/
	BOF_SIZE_FLOAT()
	{
		Reset();
	}
	BOF_SIZE_FLOAT(float _Width_f, float _Height_f)
	{
		Width_f = _Width_f;
		Height_f = _Height_f;
	}
	void     Reset()
	{
		Width_f = 0;
		Height_f = 0;
	}
};
#endif

/*!
* Summary
* Definition of a rectangle zone
*/
struct BOFSTD_EXPORT BOF_RECT
{
		int32_t Left_S32;                 /*! Rectangle zone left x position*/
		int32_t Right_S32;                /*! Rectangle zone right x position outside of rect: Width_U32  = (_rRect_X.Right_S32 - _rRect_X.Left_S32)*/
		int32_t Top_S32;                  /*! Rectangle zone top y position*/
		int32_t Bottom_S32;               /*! Rectangle zone bottom y position outside of rect: Height_U32 = (_rRect_X.Bottom_S32 - _rRect_X.Top_S32);*/
		BOF_RECT()
		{
			Reset();
		}

		BOF_RECT(BOF_POINT_2D _TopLeft_X, BOF_POINT_2D _BottomRight_X)
		{
			Top_S32 = _TopLeft_X.y_S32;
			Left_S32 = _TopLeft_X.x_S32;
			Bottom_S32 = _BottomRight_X.y_S32;
			Right_S32 = _BottomRight_X.x_S32;
		}

		BOF_RECT(int32_t _x_S32, int32_t _y_S32, uint32_t _Width_U32, uint32_t _Height_U32)
		{
			Left_S32 = _x_S32;
			Right_S32 = _x_S32 + _Width_U32;
			Top_S32 = _y_S32;
			Bottom_S32 = _y_S32 + _Height_U32;
		}

		void Reset()
		{
			Left_S32 = 0;
			Right_S32 = 0;
			Top_S32 = 0;
			Bottom_S32 = 0;
		}

		bool IsInside(const BOF_RECT &_rBox) const
		{
			return ((_rBox.Left_S32 <= Left_S32) && (_rBox.Top_S32 <= Top_S32) && (_rBox.Right_S32 >= Right_S32) && (_rBox.Bottom_S32 >= Bottom_S32));
		}

		uint32_t Width() const
		{
			return Right_S32 - Left_S32;
		}

		uint32_t Height() const
		{
			return Bottom_S32 - Top_S32;
		}

		BOF_SIZE Size() const
		{
			return BOF_SIZE(Width(), Height());
		}
};


/*!
* Summary
* Definition of a color in the RGBA color space
*/
struct BOFSTD_EXPORT BOF_RGBA
{
		uint8_t r_U8;                     /*! Color R component(Red)*/
		uint8_t g_U8;                     /*! Color G component(Greeen)*/
		uint8_t b_U8;                     /*! Color B component(Blue)*/
		uint8_t a_U8;                     /*! Color alpha component(Opacity)*/
		BOF_RGBA()
		{
			Reset();
		}

		BOF_RGBA(uint8_t _r_U8, uint8_t _g_U8, uint8_t _b_U8, uint8_t _a_U8)
		{
			r_U8 = _r_U8;
			g_U8 = _g_U8;
			b_U8 = _b_U8;
			a_U8 = _a_U8;
		}

		void Reset()
		{
			r_U8 = 0;
			g_U8 = 0;
			b_U8 = 0;
			a_U8 = 0;
		}
};

/*!
* Summary
* Definition of a color in the YUVA color space
*/
struct BOFSTD_EXPORT BOF_YUVA
{
		uint8_t y_U8;                     /*! Color Y component(Luminance)*/
		uint8_t u_U8;                     /*! Color U component(Chrominance)*/
		uint8_t v_U8;                     /*! Color V component(Chrominance)*/
		uint8_t a_U8;                     /*! Color alpha component(Opacity)*/
		BOF_YUVA()
		{
			Reset();
		}

		BOF_YUVA(uint8_t _y_U8, uint8_t _u_U8, uint8_t _v_U8, uint8_t _a_U8)
		{
			y_U8 = _y_U8;
			u_U8 = _u_U8;
			v_U8 = _v_U8;
			a_U8 = _a_U8;
		}

		void Reset()
		{
			y_U8 = 0;
			u_U8 = 0;
			v_U8 = 0;
			a_U8 = 0;
		}
};

		extern const BOF_YUVA GL_pYuvRainbow75_X[];
		extern const BOF_YUVA GL_pYuvRainbow100_X[];
		extern const BOF_YUVA GL_pYuvRainbowHd_X[];
		extern const BOF_YUVA GL_pYuvWobNair_X[];
		extern const BOF_YUVA GL_YuvWhite_X;
		extern const BOF_YUVA GL_YuvNeg4Ire_X;
		extern const BOF_YUVA GL_YuvPos4Ire_X;
		extern const BOF_YUVA GL_YuvIPixel_X;
		extern const BOF_YUVA GL_YuvQPixel_X;
		extern const BOF_YUVA GL_YuvGray40_X;
		extern const BOF_YUVA GL_YuvGray15_X;
		extern const BOF_YUVA GL_YuvCyan_X;
		extern const BOF_YUVA GL_YuvYellow_X;
		extern const BOF_YUVA GL_YuvRed_X;
		extern const BOF_YUVA GL_YuvGreen_X;
		extern const BOF_YUVA GL_YuvBlue_X;
		extern const BOF_YUVA GL_YuvBlack0_X;
		extern const BOF_YUVA GL_YuvBlack2_X;
		extern const BOF_YUVA GL_YuvBlack4_X;
		extern const BOF_YUVA GL_YuvNeg2_X;
#pragma pack(2)

struct BOFSTD_EXPORT BOF_BITMAP_FILE_HEADER
{
		uint16_t Type_U16;
		uint32_t Size_U32;
		uint16_t Reserved1_U16;
		uint16_t Reserved2_U16;
		uint32_t OffBits_U32;

		BOF_BITMAP_FILE_HEADER()
		{
			Reset();
		}

		void Reset()
		{
			Type_U16 = 0;
			Size_U32 = 0;
			Reserved1_U16 = 0;
			Reserved2_U16 = 0;
			OffBits_U32 = 0;
		}
};

#pragma pack()

#pragma pack (1)

struct BOFSTD_EXPORT BOF_PALETTE_ENTRY
{
		uint8_t r_U8;
		uint8_t g_U8;
		uint8_t b_U8;
		uint8_t Flg_U8;

		BOF_PALETTE_ENTRY()
		{
			Reset();
		}

		void Reset()
		{
			r_U8 = 0;
			g_U8 = 0;
			b_U8 = 0;
			Flg_U8 = 0;
		}
};

struct BOFSTD_EXPORT BOF_BITMAP_INFO_HEADER
{
		uint32_t Size_U32;
		int32_t Width_S32;
		int32_t Height_S32;
		uint16_t Planes_U16;
		uint16_t BitCount_U16;
		uint32_t Compression_U32;
		uint32_t SizeImage_U32;
		int32_t XPelsPerMeter_S32;
		int32_t YPelsPerMeter_S32;
		uint32_t ClrUsed_U32;
		uint32_t ClrImportant_U32;

		BOF_BITMAP_INFO_HEADER()
		{
			Reset();
		}

		void Reset()
		{
			Size_U32 = 0;
			Width_S32 = 0;
			Height_S32 = 0;
			Planes_U16 = 0;
			BitCount_U16 = 0;
			Compression_U32 = 0;
			SizeImage_U32 = 0;
			XPelsPerMeter_S32 = 0;
			YPelsPerMeter_S32 = 0;
			ClrUsed_U32 = 0;
			ClrImportant_U32 = 0;
		}
};

struct BOFSTD_EXPORT BOF_TGA_FLAG
{
		uint8_t AttBitsPerPixel_U8 : 4;   // Bits 0-3: 4 bits, number of attribute bits per pixel
		uint8_t Reserved_U8 : 1;   // Bit 4: 1 bit, origin: 0=left, 1=right
		uint8_t Origin_UB : 1;   // Bit 5: 1 bit, origin: 0=lower left, 1=upper left
		uint8_t InterLeaveMode_U8 : 2;   // bit 7-6: 2 bits, interleaving flag 00 = non-interleaved. 01 = two-way (even/odd) interleaving. 10 = four way interleaving.11 = reserved.
		BOF_TGA_FLAG()
		{
			Reset();
		}

		void Reset()
		{
			AttBitsPerPixel_U8 = 0;
			Reserved_U8 = 0;
			Origin_UB = 0;
			InterLeaveMode_U8 = 0;
		}
};

/*
0  -  No image data included.
1  -  Uncompressed, color-mapped images.
2  -  Uncompressed, RGB images.
3  -  Uncompressed, black and white images.
9  -  Runlength encoded color-mapped images.
10 -  Runlength encoded RGB images.
11 -  Compressed, black and white images.
32 -  Compressed color-mapped data, using Huffman, Delta, and
runlength encoding.
33 -  Compressed color-mapped data, using Huffman, Delta, and
runlength encoding.  4-pass quadtree-type process.
*/
struct BOFSTD_EXPORT BOF_TGA_HEADER
{
		uint8_t IdentSize_U8;        // size of ID field that follows 18 uint8_t header (0 usually)
		uint8_t ColourMapType_U8;    // type of colour map 0=none, 1=has palette
		uint8_t ImageType_U8;        // type of image 0=none,1=indexed,2=rgb,3=grey,>8=rle packed

		uint16_t ColourMapStart_U16;  // first colour map entry in palette
		uint16_t ColourMapLength_U16; // number of colours in palette
		uint8_t ColourMapBits_U8;    // number of bits per palette entry 15,16,24,32

		uint16_t XStart_U16;          // image x origin
		uint16_t YStart_U16;          // image y origin
		uint16_t Width_U16;           // image width in pixels
		uint16_t Height_U16;          // image height in pixels
		uint8_t Bits_U8;             // image bits per pixel 8,16,24,32

		BOF_TGA_FLAG Descriptor_X;        // image descriptor bits (vh flip bits)
		BOF_TGA_HEADER()
		{
			Reset();
		}

		void Reset()
		{
			IdentSize_U8 = 0;
			ColourMapType_U8 = 0;
			ImageType_U8 = 0;

			ColourMapStart_U16 = 0;
			ColourMapLength_U16 = 0;
			ColourMapBits_U8 = 0;

			XStart_U16 = 0;
			YStart_U16 = 0;
			Width_U16 = 0;
			Height_U16 = 0;
			Bits_U8 = 0;

			Descriptor_X.Reset();
		}
};

#pragma pack ()

/*** Enum ******************************************************************/

/*** functions *******************************************************/

BOFSTD_EXPORT BOFERR Bof_Rgb2Yuv(uint8_t _r_UB, uint8_t _g_UB, uint8_t _b_UB, uint8_t &_rY_UB, uint8_t &_rU_UB, uint8_t &_rV_UB);
BOFSTD_EXPORT BOFERR Bof_Yuv2Rgb(uint8_t _y_UB, uint8_t _u_UB, uint8_t _v_UB, uint8_t &_rR_UB, uint8_t &_rG_UB, uint8_t &_rB_UB);
BOFSTD_EXPORT BOFERR Bof_LookForZoneCoordinate(BOF_RECT &_rRect_X, uint32_t _BitCount_U32, void *_pData, uint32_t _Pitch_U32, uint32_t _Color_U32);
BOFSTD_EXPORT BOFERR Bof_EraseZoneBorder(BOF_RECT &_rRect_X, uint32_t _BitCount_U32, void *_pData, uint32_t _Pitch_U32);
BOFSTD_EXPORT BOFERR Bof_DecimateGraphicData(uint8_t _BytePerPixel_UB, uint8_t *_pData_UB, uint32_t _Width_U32, uint32_t _Height_U32, float _DecimationStepX_f, float _DecimationStepY_f);
BOFSTD_EXPORT BOFERR Bof_LookForColor(uint32_t _NbColorEntry_U32, BOF_PALETTE_ENTRY *_pColorTable_X, BOF_PALETTE_ENTRY _Color_X, uint32_t *_pColorEntry_U32);
BOFSTD_EXPORT BOFERR Bof_SwapColorEntries(uint8_t _BytePerPixel_UB, uint8_t *pData_UB, uint32_t _Width_U32, uint32_t _Height_U32, uint32_t _ColorEntry1_U32, uint32_t _ColorEntry2_U32);
BOFSTD_EXPORT BOFERR Bof_LoadGraphicFile(BofPath &_rPath, BOF_BITMAP_INFO_HEADER &_rBmInfo_X, BOF_PALETTE_ENTRY *_pPaletteEntry_X, uint8_t *pData_UB, uint32_t _LoadIndex_U32, uint32_t &_rSize_U32);
BOFSTD_EXPORT BOFERR Bof_LoadTgaFile(BofPath &_rPath, BOF_TGA_HEADER &_rTgaHeader_X, BOF_PALETTE_ENTRY *_pPaletteEntry_X, uint8_t *_pData_UB, uint32_t _LoadIndex_U32, uint32_t &_rSize_U32);
BOFSTD_EXPORT BOFERR Bof_GenerateTgaFile(BofPath &_rPath, uint32_t _Width_U32, uint32_t _Height_U32);
BOFSTD_EXPORT BOFERR Bof_LoadBmpFile(BofPath &_rPath, BOF_BITMAP_INFO_HEADER &_rBmInfo_X, BOF_PALETTE_ENTRY *_pPaletteEntry_X, uint8_t *_pData_UB, uint32_t _LoadIndex_U32, uint32_t &_rSize_U32);
BOFSTD_EXPORT BOFERR Bof_ReadGraphicFile(intptr_t Io, uint32_t _RecordIndex_U32, bool _TopDown_B, uint8_t _BytePerPixel_UB, uint8_t *_pData_UB, uint32_t _Width_U32, uint32_t _Height_U32, uint32_t *_pRecordSize_U32);
BOFSTD_EXPORT void Bof_YuvToRgbReference(int y, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b);
// JPEG YUV to RGB reference
BOFSTD_EXPORT void Bof_YuvjToRgbReference(int y, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b);
// BT.709 YUV to RGB reference
// See also http://www.equasys.de/colorconversion.html
BOFSTD_EXPORT void Bof_YuvhToRgbReference(int y, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b);
// BT.2020 YUV to RGB reference
BOFSTD_EXPORT void Bof_YuvRec2020ToRgbReference(int y, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b);

BOFSTD_EXPORT BOFERR Bof_UyvyToBgra(uint32_t _Width_U32, int _Height_i, uint32_t _UyvyStride_U32, const uint8_t *_pUyvy_U8, uint32_t _BrgaStride_U32, BOF_RECT *_pCrop_X,BOF_ROTATION _Rotation_E, uint8_t *_pBgra_U8);
BOFSTD_EXPORT BOFERR Bof_BgraToUyvy(uint32_t _Width_U32, int _Height_i, uint32_t _BrgaStride_U32, const uint8_t *_pBgra_U8, uint32_t _UyvyStride_U32,BOF_RECT *_pCrop_X, BOF_ROTATION _Rotation_E, uint8_t *_pUyvy_U8);

class BOFSTD_EXPORT BofMediaDetector
{
public:
	enum class ResultFormat
	{
		Text, Html, Json
	};
	enum class MediaStreamType
	{
		General, Video, Audio, Text, Other, Image, Menu
	};
	enum class InfoType
	{
		Name, Text, Measure, Options, Name_Text, Measure_Text, Info, HowTo, Domain
	};
	BofMediaDetector();
	virtual ~BofMediaDetector();

	BOFERR ParseFile(const BofPath &_rPathName, ResultFormat _ResultFormat_E, std::string &_rResult_S);
	BOFERR ParseBuffer(const BOF_BUFFER &_rBuffer_X, ResultFormat _ResultFormat_E, std::string &_rResult_S, uint64_t &_rOffsetInBuffer_U64);
	BOFERR Query(MediaStreamType _MediaStreamType_E, const std::string &_rOption_S, InfoType _InfoType_E, std::string &_rResult_S);

private:
	MediaInfoLib::MediaInfo mMediaInfo;
};
END_BOF_NAMESPACE()