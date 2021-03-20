/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the ut_bofmp class
 *
 * Name:        ut_bofmp_mmap.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */

/*** Include files ***********************************************************/

#if defined( __linux__ ) || defined(__APPLE__)

#include <gtest/gtest.h>

#include <bofstd/bofthreadapi.h>
#include <bofstd/bofsystem.h>
#include "../include/ut_mmap.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include <setjmp.h>
USE_BOF_NAMESPACE()

/*** Defines ***********************************************************************************************************************/

#define TEST_FILENAME    "./testmap"
#define TEST_STRING1     "Virtual memory mapping test"
#define TEST_STRING2     "Successful"
#define MAP_MAGIC        ( (uint32_t) 0xA55A1234 )


/*** Enums *************************************************************************************************************************/

/*** Structures ********************************************************************************************************************/

typedef struct
{
	uint32_t  Magic_U32;
	char pString_U8[32];
} MEMORY_MAP_TESTSTRUCTURE;

/*** Global variables ********************************************************/

MEMORY_MAP_TESTSTRUCTURE Test_X;

/* Global used for illegal memory access tests */
/* Declaring global jmp_buf variable to be used by both main and signal handler */
jmp_buf buf;

int TestFileDescriptor;
#if defined(__ANDROID__)
#else
void sighandler(int signo)  //, siginfo_t *si, ucontext_t *context)
{
	printf("Handler executed for signal : %s\n", strsignal(signo) );
	longjmp(buf, 1);
}
#endif

void Bof_VirtualMemoryMAP_Test::SetUp( )
{
	/* catch SIGSEGV */
#if defined(__ANDROID__)
#else
  signal(SIGSEGV, sighandler);
#endif
	/* Init file mapping test */
	TestFileDescriptor = open(TEST_FILENAME, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
	ASSERT_TRUE(TestFileDescriptor != -1);
	ASSERT_TRUE(ftruncate(TestFileDescriptor, sizeof( Test_X ) ) != -1);
	memset(&Test_X, 0, sizeof( Test_X ) );
	Test_X.Magic_U32 = MAP_MAGIC;
	snprintf(Test_X.pString_U8, sizeof( Test_X.pString_U8 ), TEST_STRING1);

	ASSERT_TRUE(write(TestFileDescriptor, &Test_X, sizeof( Test_X ) ) == sizeof( Test_X ) );
}


void Bof_VirtualMemoryMAP_Test::TearDown( )
{
	ASSERT_TRUE(close(TestFileDescriptor) == 0);
	ASSERT_TRUE(remove(TEST_FILENAME) == 0);
}


TEST_F(Bof_VirtualMemoryMAP_Test, CreateAndClose)
{
	/* Create virtual memory file mapping */
	MEMORY_MAP_TESTSTRUCTURE *pVirtualStruct_X;

	pVirtualStruct_X = (MEMORY_MAP_TESTSTRUCTURE *) Bof_CreateVirtualMemoryMap(BOF_THREAD_ACCESS_READWRITE, TEST_FILENAME, 0, sizeof( MEMORY_MAP_TESTSTRUCTURE ) );

  if (pVirtualStruct_X != nullptr)   //Under linux must be root
  {

    /* Check if successful */
    ASSERT_NE(pVirtualStruct_X, (MEMORY_MAP_TESTSTRUCTURE *)nullptr);

    ASSERT_EQ(pVirtualStruct_X->Magic_U32, MAP_MAGIC);
    ASSERT_EQ(strcmp(pVirtualStruct_X->pString_U8, TEST_STRING1), 0);

    /* Close virtual memory file mapping */
    ASSERT_EQ(Bof_CloseVirtualMemoryMap(pVirtualStruct_X, sizeof(MEMORY_MAP_TESTSTRUCTURE)), BOFERR_NO_ERROR);
  }
}

TEST_F(Bof_VirtualMemoryMAP_Test, WriteFile)
{
	/* Create virtual memory file mapping */
	MEMORY_MAP_TESTSTRUCTURE *pVirtualStruct_X;

	pVirtualStruct_X = (MEMORY_MAP_TESTSTRUCTURE *) Bof_CreateVirtualMemoryMap(BOF_THREAD_ACCESS_READWRITE, TEST_FILENAME, 0, sizeof( MEMORY_MAP_TESTSTRUCTURE ) );

  if (pVirtualStruct_X != nullptr)   //Under linux must be root
  {
    /* Check if successful */
    ASSERT_NE(pVirtualStruct_X, (MEMORY_MAP_TESTSTRUCTURE *)nullptr);

    /* Write file */
    lseek(TestFileDescriptor, 0, SEEK_SET);
    uint32_t SwapedMagic_U32 = MAP_MAGIC;
    BOF_SWAP32(SwapedMagic_U32);
    Test_X.Magic_U32 = SwapedMagic_U32;
    snprintf(Test_X.pString_U8, sizeof(Test_X.pString_U8), TEST_STRING2);
    ASSERT_EQ(write(TestFileDescriptor, &Test_X, sizeof(Test_X)), sizeof(Test_X));

    /* Check on the virtual memory side */
    ASSERT_EQ(pVirtualStruct_X->Magic_U32, SwapedMagic_U32);
    ASSERT_EQ(strcmp(pVirtualStruct_X->pString_U8, TEST_STRING2), 0);

    /* Close virtual memory file mapping */
    ASSERT_EQ(Bof_CloseVirtualMemoryMap(pVirtualStruct_X, sizeof(MEMORY_MAP_TESTSTRUCTURE)), BOFERR_NO_ERROR);
  }
}

TEST_F(Bof_VirtualMemoryMAP_Test, WriteMem)
{
	/* Create virtual memory file mapping */
	MEMORY_MAP_TESTSTRUCTURE *pVirtualStruct_X;

	pVirtualStruct_X = (MEMORY_MAP_TESTSTRUCTURE *) Bof_CreateVirtualMemoryMap(BOF_THREAD_ACCESS_READWRITE, TEST_FILENAME, 0, sizeof( MEMORY_MAP_TESTSTRUCTURE ) );
  if (pVirtualStruct_X != nullptr)   //Under linux must be root
  {

    /* Check if successful */
    ASSERT_NE(pVirtualStruct_X, (MEMORY_MAP_TESTSTRUCTURE *)nullptr);

    /* Write virtual memory */
    uint32_t SwapedMagic_U32 = MAP_MAGIC;
    BOF_SWAP32(SwapedMagic_U32);

    pVirtualStruct_X->Magic_U32 = SwapedMagic_U32;
    snprintf(pVirtualStruct_X->pString_U8, sizeof(pVirtualStruct_X->pString_U8), TEST_STRING2);

    /* Read file */
    lseek(TestFileDescriptor, 0, SEEK_SET);
    ASSERT_EQ(read(TestFileDescriptor, &Test_X, sizeof(Test_X)), sizeof(Test_X));

    /* Check on the virtual memory side */
    ASSERT_EQ(Test_X.Magic_U32, SwapedMagic_U32);
    ASSERT_EQ(strcmp(Test_X.pString_U8, TEST_STRING2), 0);

    /* Close virtual memory file mapping */
    ASSERT_EQ(Bof_CloseVirtualMemoryMap(pVirtualStruct_X, sizeof(MEMORY_MAP_TESTSTRUCTURE)), BOFERR_NO_ERROR);
  }
}



#if 0
TEST_F(Bof_VirtualMemoryMAP_Test, AccessRightsRO)
{
	/* Create virtual memory file mapping */
	MEMORY_MAP_TESTSTRUCTURE *pVirtualStruct_X;

	pVirtualStruct_X = (MEMORY_MAP_TESTSTRUCTURE *) Bof_CreateVirtualMemoryMap(BOF_THREAD_ACCESS_READ, TEST_FILENAME, 0, sizeof( MEMORY_MAP_TESTSTRUCTURE ) );

	/* Check if successful */
	ASSERT_NE(pVirtualStruct_X, (MEMORY_MAP_TESTSTRUCTURE *) nullptr);

	/* Write virtual memory */
	uint32_t SwapedMagic_U32 = MAP_MAGIC;
	BOF_SWAP32(SwapedMagic_U32);

	if ( !setjmp(buf) )
	{
		/* Should not work and trigger a segmentation fault */
		pVirtualStruct_X->Magic_U32 = SwapedMagic_U32;
		ASSERT_FALSE(true);
	}
	else
	{
		/* Ok recovering from segmentation fault */
		ASSERT_FALSE(false);
	}

	/* Write file */
	lseek(TestFileDescriptor, 0, SEEK_SET);
	Test_X.Magic_U32 = SwapedMagic_U32;
	snprintf(Test_X.pString_U8, sizeof( Test_X.pString_U8 ), TEST_STRING2);
	ASSERT_EQ(write(TestFileDescriptor, &Test_X, sizeof( Test_X ) ), sizeof( Test_X ) );

	/* Check on the virtual memory side */
	ASSERT_EQ(pVirtualStruct_X->Magic_U32, SwapedMagic_U32);
	ASSERT_EQ(strcmp(pVirtualStruct_X->pString_U8, TEST_STRING2), 0);

	/* Close virtual memory file mapping */
	ASSERT_EQ(Bof_CloseVirtualMemoryMap(pVirtualStruct_X, sizeof( MEMORY_MAP_TESTSTRUCTURE ) ), BOFERR_NO_ERROR);
}
#endif
TEST_F(Bof_VirtualMemoryMAP_Test, ValidateFm)
{
	/* depend de l'infrastructure /mnt/....
	void * MemoryMap_h;
	bool AlreaderExist_B;
	uint8_t  *pData_U8;
	uint32_t	Size_U32, Sts_U32;

	pData_U8 = nullptr;
	Size_U32 = 0x10000;
	MemoryMap_h = Bof_OpenFileMapping("/tmp/memory.shr/1", Size_U32, false, &AlreaderExist_B, (void **)&pData_U8);
	ASSERT_TRUE(MemoryMap_h != BOF_INVALID_HANDLE_VALUE);
	ASSERT_TRUE(pData_U8 != nullptr);
	memset(pData_U8, 0, Size_U32);
	Sts_U32 = Bof_CloseFileMapping(MemoryMap_h, pData_U8,Size_U32, true);
	ASSERT_EQ(Sts_U32, 0);
	*/
}
#endif 

