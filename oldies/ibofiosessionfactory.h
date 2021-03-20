#pragma once

#include <bofstd/bofstd.h>

BEGIN_BOF_NAMESPACE()
class BofIoServer;

class BofIoSession;

class IBofIoSessionFactory
{
public:
		virtual ~IBofIoSessionFactory()
		{}

//The BofIoServer server invokes this method when a new connection has been accepted.
		virtual BofIoSession *V_CreateSession(BofIoServer *_pIoServer) = 0;

		virtual BOFERR V_SessionDisconnected(BofIoSession *_pSession) = 0;

};

END_BOF_NAMESPACE()
