/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines routines for creating and managing an actor. It is
 * based on czmq zactor
 *
 * Name:        BofActor.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Mar 11 2002  BHA : Initial release
 */

#pragma once

/*** Include ****************************************************************/
#include <bofstd/bofstd.h>
#include <functional>
#include <memory>

BEGIN_BOF_NAMESPACE()

/*** Define *****************************************************************/

/*** Enum *****************************************************************/

/*** Structure **************************************************************/

using BofActorFunction = std::function<std::string(const std::string &_rCommand_S, void *_pContext)>;

/*** BofActor **********************************************************************/

class BofActor
{
// Opaque pointer design pattern: all public and protected stuff goes here ...
public:
		BofActor(const std::string &_rName_S, BofActorFunction _ActorFunction, void *_pUserContext);

		BofActor &operator=(const BofActor &) = delete;     // Disallow copying
		BofActor(const BofActor &) = delete;

		virtual ~BofActor();

		BOFERR SendCommandToActor(const std::string &_rCmd_S, uint32_t &_rTicket_U32);

		BOFERR WaitForReplyFromActor(std::string &_rReply_S, uint32_t &_rTicket_U32);

		bool IsCommandRunning();

		bool IsReplyPending();
		// Opaque pointer design pattern: opaque type here
public:
		class ActorImplementation;

		std::unique_ptr<ActorImplementation> mpuActorImplementation;
};

END_BOF_NAMESPACE()