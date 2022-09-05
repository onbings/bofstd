/*
 * Copyright (c) 2013-2033, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bofpath class
 *
 * Name:        ut_Fs.cpp
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
#include <bofstd/bofuri.h>

USE_BOF_NAMESPACE()
/*
Timelesschunk:  storage://10.129.4.172:11000/5/file/cg2/data1

storage ip: 10.129.4.172

api port: 11000

Partition: 5

Timelesschunk: file

Timelesschunk name: cg2

Timelesschunk sub: data1
*/
TEST(Uri_Test, UriConstructorDestructor)
{
  EXPECT_EQ(0, BOF_ERR_NO_ERROR);
}