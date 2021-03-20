#pragma once

#include <memory>
#include <uv.h>
#include <bofstd/bofIo.h>
//#include <bofstd/bofstd.h>

BEGIN_BOF_NAMESPACE()

class BofIoClientManager;

struct BOF_UV_CALLBACK;

struct BOF_IO_CONNECT_PARAM
{
		std::string BindIpAddress_S;
		std::string ConnectToIpAddress_S;
		uint32_t Timeout_U32;

		BOF_IO_CONNECT_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			BindIpAddress_S = "";
			ConnectToIpAddress_S = "";
			Timeout_U32 = 500;
		}
};


class BofIoClient
{
private:
		BofIoClientManager *mpIoClientManager;
		BOF_IO_STATE mIoState_E;

public:
		BofIoClient(BofIoClientManager *_pIoClientManager);

		virtual ~BofIoClient();

		bool IsConnected();

		BOF_IO_STATE IoState();

		void IoState(BOF_IO_STATE _IoState_E);
//	BOFERR Connect(const BOF_IO_CONNECT_PARAM &_rBofIoConnectParam_X);
//	BOFERR Disconnect();
};


END_BOF_NAMESPACE()
