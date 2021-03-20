#pragma once

#include <bofstd/bofstd.h>
//#include <bofstd/bofsocketsession.h>

BEGIN_BOF_NAMESPACE()
class BofSocketServer;

class BofSocketSession;

class BofSocket;

class IBofSocketSessionFactory
{
public:
		virtual ~IBofSocketSessionFactory()
		{}

//The bofsocket server invokes this method when a new connection has been accepted.
		virtual BofSocketSession *V_CreateSession(BofSocketServer *_pSocketServer, BofSocket *_pSocket) = 0;

		virtual BOFERR V_SessionDisconnected(BofSocketSession *_pSession) = 0;

};

END_BOF_NAMESPACE()
