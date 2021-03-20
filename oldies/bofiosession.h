#pragma once

#include <bofstd/bofstd.h>

BEGIN_BOF_NAMESPACE()
class BofIoServer;

class BofIoSession
{
private:
		BofIoServer *mpBofIoServer;
public:
		BofIoSession(BofIoServer *_pBofIoServer);

		virtual ~BofIoSession();
};

END_BOF_NAMESPACE()
