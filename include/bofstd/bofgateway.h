/*!
 * Copyright (c) 2008, EVS. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * Remarks
 *
 * Name:			        BofGateway.h
 * Author:		        Nicolas Marique (NMA)
 *
 * Summary:
 *
 *  The C code to programmatically read the gateway address
 *
 * History:
 *  V 1.00  February 04 2016  NMA : Original version
 */

#pragma once

/*** Include ***********************************************************************************************************************/
#include <bofstd/bofstd.h>
#include <cstdint>

BEGIN_BOF_NAMESPACE()


/*** Defines ***********************************************************************************************************************/

/*** Globals ***********************************************************************************************************************/

/*** Enums *************************************************************************************************************************/

/*** Structures ********************************************************************************************************************/

/*** Constants *********************************************************************************************************************/

/*** Prototypes ********************************************************************************************************************/

int Bof_GetGatewayIpAddress(const char *_pInterface_c, char *gatewayip, uint32_t size, int _Family_i);

/*** Classes ***********************************************************************************************************************/

END_BOF_NAMESPACE()
