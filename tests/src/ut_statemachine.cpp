/*
* Copyright (c) 2013-2023, OnBings All rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
* PURPOSE.
*
* This module implements the unit testing of the asyncmulticastdelegate class
*
* Name:        ut_asyncmulticastdelegate.cpp
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
#include "statemachine/SelfTestEngine.h"

using namespace DelegateLib;
AsyncCallback<SelfTestStatus> SelfTestEngine::StatusCallback;

//------------------------------------------------------------------------------
// GetInstance
//------------------------------------------------------------------------------
SelfTestEngine& SelfTestEngine::GetInstance()
{
	static SelfTestEngine instance;
	return instance;
}

//------------------------------------------------------------------------------
// SelfTestEngine
//------------------------------------------------------------------------------
SelfTestEngine::SelfTestEngine() :
	SelfTest(ST_MAX_STATES),
	m_thread("SelfTestEngine")
{
	StartCallback.Register(&SelfTestEngine::StartPrivateCallback, &m_thread, this);

	// Register for callbacks when sub self-test state machines complete or fail
	m_centrifugeTest.CompletedCallback.Register(&SelfTestEngine::Complete, &m_thread, this);
	m_centrifugeTest.FailedCallback.Register(&SelfTestEngine::Cancel, &m_thread, this);
	m_pressureTest.CompletedCallback.Register(&SelfTestEngine::Complete, &m_thread, this);
	m_pressureTest.FailedCallback.Register(&SelfTestEngine::Cancel, &m_thread, this);
}

//------------------------------------------------------------------------------
// InvokeStatusCallback
//------------------------------------------------------------------------------
void SelfTestEngine::InvokeStatusCallback(std::string msg)
{
	// Client(s) registered?
	if (StatusCallback)
	{
		SelfTestStatus status;
		status.message = msg;

		// Callback registered client(s)
		StatusCallback(status);
	}
}

//------------------------------------------------------------------------------
// Start
//------------------------------------------------------------------------------
void SelfTestEngine::Start()
{
	// Asynchronously call StartPrivateCallback
	StartCallback(NoData());
}

//------------------------------------------------------------------------------
// StartPrivateCallback
//------------------------------------------------------------------------------
void SelfTestEngine::StartPrivateCallback()
{
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (ST_START_CENTRIFUGE_TEST)		// ST_IDLE
		TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)				// ST_COMPLETED
		TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)				// ST_FAILED
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_START_CENTRIFUGE_TEST
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_START_PRESSURE_TEST
	END_TRANSITION_MAP(NULL)
}

//------------------------------------------------------------------------------
// Complete
//------------------------------------------------------------------------------
void SelfTestEngine::Complete()
{
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_IDLE
		TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)				// ST_COMPLETED
		TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)				// ST_FAILED
		TRANSITION_MAP_ENTRY (ST_START_PRESSURE_TEST)		// ST_START_CENTRIFUGE_TEST
		TRANSITION_MAP_ENTRY (ST_COMPLETED)					// ST_START_PRESSURE_TEST
	END_TRANSITION_MAP(NULL)
}

//------------------------------------------------------------------------------
// StartCentrifugeTest
//------------------------------------------------------------------------------
STATE_DEFINE(SelfTestEngine, StartCentrifugeTest, NoEventData)
{
	InvokeStatusCallback("SelfTestEngine::ST_CentrifugeTest");
	m_centrifugeTest.Start();
}

//------------------------------------------------------------------------------
// StartPressureTest
//------------------------------------------------------------------------------
STATE_DEFINE(SelfTestEngine, StartPressureTest, NoEventData)
{
	InvokeStatusCallback("SelfTestEngine::ST_PressureTest");
	m_pressureTest.Start();
}

