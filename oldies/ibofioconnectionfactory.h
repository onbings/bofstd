#pragma once

#include <bofstd/bofstd.h>

BEGIN_BOF_NAMESPACE()
class IBofIoConnection;

class BofIoConnectionManager;

struct BOF_IO_CHANNEL_INFO
{
		std::string Name_S;
		BOF_NAMESPACE::BOF_PROTOCOL_TYPE ProtocolType_E;

		BOF_IO_CHANNEL_INFO()
		{
			Reset();
		}

		void Reset()
		{
			Name_S = "";
			ProtocolType_E = BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN;
		}
};

class IBofIoConnectionFactory
{
public:
		IBofIoConnectionFactory()
		{}

		virtual ~IBofIoConnectionFactory()
		{}

		//The BofIoServer server invokes this method when a new connection has been accepted.
		virtual BOF_NAMESPACE::IBofIoConnection *V_CreateSession(const BOF_NAMESPACE::BOF_IO_CHANNEL_INFO &_rBofIoChannelInfo_X, BOF_NAMESPACE::BofIoConnectionManager *_pIoConnectionManager,
		                                                         BOF_NAMESPACE::BOF_IO_CHANNEL_PARAM &_rBofIoChannelParam_X) = 0;

		virtual BOFERR V_SessionDisconnected(BOF_NAMESPACE::IBofIoConnection *_pIoConnection) = 0;

		//https://cpppatterns.com/patterns/rule-of-five.html
		IBofIoConnectionFactory(const IBofIoConnectionFactory &_rOther) = delete;

		IBofIoConnectionFactory(IBofIoConnectionFactory &&_rrOther) = delete;

		IBofIoConnectionFactory &operator=(const IBofIoConnectionFactory &_rOther) = delete;

		IBofIoConnectionFactory &operator=(IBofIoConnectionFactory &&_rrOther) = delete;

};

END_BOF_NAMESPACE()

