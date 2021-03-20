/*!
Copyright (c) 2008, EVS. All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
PURPOSE.

Remarks

  Name:              CritSect.cpp
  Author:            Julien Melchior (JME)

Summary:

  The C-Wrapper of the critical section object definition

History:
  V 1.00  ???? ?? ????  JME : First Release
*/

/*** Include ***********************************************************************************************************************/

#include <bofstd/critsect.h>
#include <bofstd/ccritsect.h>
#include <stddef.h>
BEGIN_BOF_NAMESPACE();


/*** Defines ***********************************************************************************************************************/

/*** Enums *************************************************************************************************************************/

/*** Structures ********************************************************************************************************************/

/*** Prototypes ********************************************************************************************************************/

/*!
Description
  This function creates a critical section
  
Parameters
  None
  
Returns
  The critical section handle

Remarks
  None
*/
OS_CRITSECT CritSectCreate()
{
  OS_CRITSECT         Ret_h              = nullptr;
  CCriticalSection  * pCriticalSection_O = nullptr;
  
  pCriticalSection_O = new CCriticalSection();

  if (pCriticalSection_O->Init() == 0)
    Ret_h = (OS_CRITSECT)pCriticalSection_O;
  
  return Ret_h;
}

/*!
Description
  This function indicates that the
  current thread is entering the 
  specified critical section
  
Parameters
  _CritSect_h - The critical section handle
  
Returns
  Nothing

Remarks
  None
*/
void CritSectEnter(OS_CRITSECT _CritSect_h)
{
  CCriticalSection * pCriticalSection_O = (CCriticalSection *)_CritSect_h;
  
  if(pCriticalSection_O != nullptr)
  {
    pCriticalSection_O->Enter();
  }
}

/*!
Description
  This function indicates that the
  current thread is leaving the 
  specified critical section
  
Parameters
  _CritSect_h - The critical section handle
  
Returns
  Nothing

Remarks
  None
*/
void CritSectLeave(OS_CRITSECT _CritSect_h)
{
  CCriticalSection * pCriticalSection_O = (CCriticalSection *)_CritSect_h;
  
  if(pCriticalSection_O != nullptr)
  {
    pCriticalSection_O->Leave();
  }
}

/*!
Description
  This function destroy the critical section
  
Parameters
  _CritSect_h - The critical section handle
  
Returns
  Nothing

Remarks
  None
*/
void CritSectDestroy(OS_CRITSECT _CritSect_h)
{
  CCriticalSection * pCriticalSection_O = (CCriticalSection *)_CritSect_h;
  
  if(pCriticalSection_O != nullptr)
  {
    BOF_SAFE_DELETE(pCriticalSection_O);
  }
}
END_BOF_NAMESPACE();

