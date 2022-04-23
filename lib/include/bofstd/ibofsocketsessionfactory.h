/*
* Copyright (c) 2015-2025, Onbings. All rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
* PURPOSE.
*
* This module defines the IBofSocketSessionFactory interface.
*
* Author:      Bernard HARMEL: onbings@dscloud.me
* Web:			   onbings.dscloud.me
* Revision:    1.0
*
* Rem:         Nothing
*
* History:
*
* V 1.00  Jan 05 2019  BHA : Initial release
*/
#pragma once
#include <bofstd/bofstd.h>
#include <memory>

BEGIN_BOF_NAMESPACE()
class BofSocketServer;
class BofSocketIo;
class BofSocket;

class IBofSocketSessionFactory
{
public:
    virtual ~IBofSocketSessionFactory()   {}
//The bofsocket server invokes this method when a new connection has been accepted.
		virtual std::shared_ptr<BofSocketIo> V_OpenSession(BOF_SOCKET_SESSION_TYPE _SessionType_E, uint32_t _SessionIndex_U32, std::unique_ptr<BofSocket> _puSocket) = 0;
    virtual BOFERR V_CloseSession(std::shared_ptr<BofSocketIo> _psSocketSession) = 0;
};

END_BOF_NAMESPACE()
