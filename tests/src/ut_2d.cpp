/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bof2d class
 *
 * Name:        ut_enum.cpp
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

/*** Include files ***********************************************************/

#include "gtestrunner.h"
#include <bofstd/bof2d.h>
#include <bofstd/boffs.h>


/*** Class *************************************************************************************************************************/

USE_BOF_NAMESPACE()

constexpr uint32_t SAMPLE_WIDTH  = 1920;
constexpr uint32_t SAMPLE_HEIGHT = 540;
uint8_t            GL_pFileData_U8[SAMPLE_WIDTH * SAMPLE_HEIGHT * 4];
uint8_t            GL_ConvertedData_U8[SAMPLE_WIDTH * SAMPLE_HEIGHT * 4];

TEST(Bof2d_Test, UyvyToBgra)
{
  uint32_t Start_U32, DeltaInMs_U32, i_U32,FileSize_U32,ConvertedSize_U32;
  BOFERR   Sts_E;
  intptr_t Io;
  BOF::BOF_RECT Crop_X(SAMPLE_WIDTH/2,SAMPLE_HEIGHT/2,SAMPLE_WIDTH/2,SAMPLE_HEIGHT/2);

  FileSize_U32=SAMPLE_WIDTH * SAMPLE_HEIGHT * 2;
  ConvertedSize_U32=SAMPLE_WIDTH * SAMPLE_HEIGHT * 4;
//  ConvertedSize_U32=(SAMPLE_WIDTH * SAMPLE_HEIGHT * 4)/4;//With crop and rotate
//  Sts_E = Bof_OpenFile("./colorbar_1920x1080p59_8bit_captured_by_sdk.422", true, Io);
  Sts_E = Bof_OpenFile("./File_00000000.422", true, Io);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    Sts_E    = BOF::Bof_ReadFile(Io, FileSize_U32, GL_pFileData_U8);
    Sts_E    = BOF::Bof_CloseFile(Io);

    memset(GL_ConvertedData_U8, 0, ConvertedSize_U32);
    Start_U32 = BOF::Bof_GetMsTickCount();
    for (i_U32 = 0; i_U32 < 100; i_U32++)
    {
      Sts_E=BOF::Bof_UyvyToBgra(SAMPLE_WIDTH, SAMPLE_HEIGHT, SAMPLE_WIDTH * 2, GL_pFileData_U8, SAMPLE_WIDTH * 4,nullptr,BOF_ROTATION_0, GL_ConvertedData_U8);
//      Sts_E=BOF_NAMESPACE::Bof_UyvyToBgra(SAMPLE_WIDTH, SAMPLE_HEIGHT, SAMPLE_WIDTH * 2, GL_pFileData_U8, SAMPLE_WIDTH * 4,&Crop_X,BOF_ROTATION_90, GL_ConvertedData_U8);
      if (Sts_E!=BOF_ERR_NO_ERROR)
      {
        break;
      }
    }
    DeltaInMs_U32 = BOF::Bof_ElapsedMsTime(Start_U32);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    if (!i_U32)
    {
      i_U32=1;
    }
    printf("%d conv in %d ms->%f / loop\n", i_U32, DeltaInMs_U32, (float) DeltaInMs_U32 / (float) i_U32);

    Sts_E = Bof_CreateFile(BOF::BOF_FILE_PERMISSION_READ_FOR_ALL | BOF::BOF_FILE_PERMISSION_WRITE_FOR_ALL, "./bgra.raw", false, Io);
    if (Sts_E == BOF_ERR_NO_ERROR)
    {
      Sts_E    = BOF::Bof_WriteFile(Io, ConvertedSize_U32, GL_ConvertedData_U8);
      Sts_E    = BOF::Bof_CloseFile(Io);
    }
  }
}

TEST(Bof2d_Test, BgraToUyvy)
{
  uint32_t Start_U32, DeltaInMs_U32, i_U32,FileSize_U32,ConvertedSize_U32;
  BOFERR   Sts_E;
  intptr_t Io;
  BOF::BOF_RECT Crop_X(SAMPLE_WIDTH/2,SAMPLE_HEIGHT/2,SAMPLE_WIDTH/2,SAMPLE_HEIGHT/2);

  FileSize_U32=SAMPLE_WIDTH * SAMPLE_HEIGHT * 4;
  ConvertedSize_U32=SAMPLE_WIDTH * SAMPLE_HEIGHT * 2;
 // ConvertedSize_U32=(SAMPLE_WIDTH * SAMPLE_HEIGHT * 2)/4; //With crop and rotate
  Sts_E = Bof_OpenFile("./bgra.raw", true, Io);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    Sts_E    = BOF::Bof_ReadFile(Io, FileSize_U32, GL_pFileData_U8);
    Sts_E    = BOF::Bof_CloseFile(Io);

    memset(GL_ConvertedData_U8, 0, ConvertedSize_U32);
    Start_U32 = BOF::Bof_GetMsTickCount();
    for (i_U32 = 0; i_U32 < 100; i_U32++)
    {
      Sts_E=BOF::Bof_BgraToUyvy(SAMPLE_WIDTH, SAMPLE_HEIGHT, SAMPLE_WIDTH * 4, GL_pFileData_U8, SAMPLE_WIDTH * 2, nullptr,BOF_ROTATION_0, GL_ConvertedData_U8);
//      Sts_E=BOF_NAMESPACE::Bof_BgraToUyvy(SAMPLE_WIDTH, SAMPLE_HEIGHT, SAMPLE_WIDTH * 4, GL_pFileData_U8, SAMPLE_WIDTH * 2, &Crop_X,BOF_ROTATION_90, GL_ConvertedData_U8);
      if (Sts_E!=BOF_ERR_NO_ERROR)
      {
        break;
      }
    }
    DeltaInMs_U32 = BOF::Bof_ElapsedMsTime(Start_U32);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    if (!i_U32)
    {
      i_U32=1;
    }
    printf("%d conv in %d ms->%f / loop\n", i_U32, DeltaInMs_U32, (float) DeltaInMs_U32 / (float) i_U32);

    Sts_E = Bof_CreateFile(BOF::BOF_FILE_PERMISSION_READ_FOR_ALL | BOF::BOF_FILE_PERMISSION_WRITE_FOR_ALL, "./uyvy.422", false, Io);
    if (Sts_E == BOF_ERR_NO_ERROR)
    {
      Sts_E    = BOF::Bof_WriteFile(Io, ConvertedSize_U32, GL_ConvertedData_U8);
      Sts_E    = BOF::Bof_CloseFile(Io);
    }
  }
}