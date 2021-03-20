/*!
Copyright (c) 2008, EVS. All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
PURPOSE.

Remarks

  Name:              CritSect.h
  Author:            Julien Melchior (JME)

Summary:

  The C-Wrapper of the critical section object definition

History:
  V 1.00  ???? ?? ????  JME : First Release
*/

#pragma once

/*** Include ***********************************************************************************************************************/
#include <bofstd/bofstd.h>
BEGIN_BOF_NAMESPACE();

/*** Defines ***********************************************************************************************************************/

typedef void * OS_CRITSECT;

#define CRITSECT_DEFINE()   CritSectCreate()
#define CRITSECT_ENTER(x)   CritSectEnter(x)
#define CRITSECT_LEAVE(x)   CritSectLeave(x)
#define CRITSECT_DESTROY(x) CritSectDestroy(x)

/*** Enums *************************************************************************************************************************/

/*** Structures ********************************************************************************************************************/

/*** Prototypes ********************************************************************************************************************/

BOF_EXTERN_C OS_CRITSECT CritSectCreate   ();
BOF_EXTERN_C void        CritSectEnter    (OS_CRITSECT _CritSect_h);
BOF_EXTERN_C void        CritSectLeave    (OS_CRITSECT _CritSect_h);
BOF_EXTERN_C void        CritSectDestroy  (OS_CRITSECT _CritSect_h);

END_BOF_NAMESPACE();
