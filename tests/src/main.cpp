#if defined(_WIN32)
//#define _CRTDBG_MAP_ALLOC  
//#include <stdlib.h>  
//#include <crtdbg.h> 
#else
#include <malloc.h>
#endif

#include "../include/gtestrunner.h"
#include <iostream>
#include <bofstd/bofstd.h>
#include <bofstd/bofsystem.h>

USE_BOF_NAMESPACE()

BOFERR AppBofAssertCallback(const std::string &_rFile_S, uint32_t _Line_U32, const std::string &_rMasg_S)
{
	printf("Assert in %s line %d Msg %s\r\n", _rFile_S.c_str(), _Line_U32, _rMasg_S.c_str());
	return BOF_ERR_NO_ERROR;
}



int main(int argc, char *argv[])
{
	BOFERR Sts_E;


#if defined (_WIN32)
#else
	mallopt(M_CHECK_ACTION, 3);
	signal(SIGPIPE, SIG_IGN);
#endif
	BOFSTDPARAM StdParam_X;
	StdParam_X.AssertInRelease_B = true;
	StdParam_X.AssertCallback = AppBofAssertCallback;
	Sts_E = Bof_Initialize(StdParam_X);
	BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);
	std::cout << "BofStd version " << Bof_GetVersion() << std::endl;

/*
	::testing::GTEST_FLAG(filter) = "Bof2d_Test.*";
	::testing::GTEST_FLAG(filter) = "CallbackCollection_Test.*";
	::testing::GTEST_FLAG(filter) = "Bit_Test.*";
	::testing::GTEST_FLAG(filter) = "Enum_Test.*";
	::testing::GTEST_FLAG(filter) = "Guid_Test.*";

	::testing::GTEST_FLAG(filter) = "Pipe_Test.*";
	::testing::GTEST_FLAG(filter) = "SocketOs_Test.*";
	::testing::GTEST_FLAG(filter) = "SocketTcp_Test.*";
	::testing::GTEST_FLAG(filter) = "SocketUdp_Test.*";
	::testing::GTEST_FLAG(filter) = "Uart_Test.*";
	::testing::GTEST_FLAG(filter) = "BofCircularBuffer_Test.*";
	::testing::GTEST_FLAG(filter) = "BofPot_Test.*";
	::testing::GTEST_FLAG(filter) = "Queue_Test.*";
	::testing::GTEST_FLAG(filter) = "BofRawCircularBufferWithSlotsize_Test.*";
	::testing::GTEST_FLAG(filter) = "BofStringCircularBuffer_Test.*";
	::testing::GTEST_FLAG(filter) = "BofCrypto_Test.*";	//LLLL
	::testing::GTEST_FLAG(filter) = "BofRamDb_Test.*";
	::testing::GTEST_FLAG(filter) = "Fs_Test.*";
	::testing::GTEST_FLAG(filter) = "Logger_Test.*";

//ut_stringformatter
	::testing::GTEST_FLAG(filter) = "Sprintf_Test.*";
	::testing::GTEST_FLAG(filter) = "PrintfTest.*";
	::testing::GTEST_FLAG(filter) = "StringViewTest.*";
	::testing::GTEST_FLAG(filter) = "WriterTest.*";
	::testing::GTEST_FLAG(filter) = "FormatToTest.*";
	::testing::GTEST_FLAG(filter) = "FormatterTest.*";
	::testing::GTEST_FLAG(filter) = "FormatIntTest.*";
	::testing::GTEST_FLAG(filter) = "FormatTest.*";
	::testing::GTEST_FLAG(filter) = "StrTest.*";
	::testing::GTEST_FLAG(filter) = "LiteralsTest.*";

	::testing::GTEST_FLAG(filter) = "CmdLineParser_Test.*";
	::testing::GTEST_FLAG(filter) = "JsonParser_Test.*";
	::testing::GTEST_FLAG(filter) = "XmlParser_Test.*";
	::testing::GTEST_FLAG(filter) = "Timecode_Test.*";
	::testing::GTEST_FLAG(filter) = "BofScriptLauncher_Test.*";
	::testing::GTEST_FLAG(filter) = "String_Test.*";
	::testing::GTEST_FLAG(filter) = "System_Test.*";
	::testing::GTEST_FLAG(filter) = "BofThread_Test.*";
	::testing::GTEST_FLAG(filter) = "Async_Test.*";
[  FAILED  ] SocketTcp_Test.TcpClientTest
[  FAILED  ] SocketUdp_Test.ScatterGatherIo
[  FAILED  ] SocketUdp_Test.UdpClientTest
*/
//	::testing::GTEST_FLAG(filter) = "-BofIo_Test.*";

	//::testing::GTEST_FLAG(filter) = "Async_Test.*";
	//::testing::GTEST_FLAG(filter) = "SocketUdp_Test.ScatterGatherIo";


#if defined (_WIN32)
#else
  ::testing::GTEST_FLAG(filter) = "-Uart_Test.*";   //No hw
#endif
	testing::InitGoogleTest(&argc, argv);
	int    Rts_i;
//	::testing::GTEST_FLAG(filter) = "XmlParser_Test.Xml";
//	::testing::GTEST_FLAG(filter) = "JsonParser_Test.JsonCfg";
//	::testing::GTEST_FLAG(filter) = "JsonParser_Test.IpSenderDeser";
//	::testing::GTEST_FLAG(filter) = "JsonParser_Test.*:JsonWriter_Test.*"; //JsonCfg";
//	::testing::GTEST_FLAG(filter) ="JsonParser_Test.JsonCfg";
//	::testing::GTEST_FLAG(filter) = "JsonWriter_Test.IpSwitcherSerDeser";
//	::testing::GTEST_FLAG(filter) ="Timecode_Test.*:CmdLineParser_Test.CmdLine";
//	::testing::GTEST_FLAG(filter) ="CmdLineParser_Test.CmdLine";
//	::testing::GTEST_FLAG(filter) = "Bit_Test.ErrorCode";	//test dailed on tc (file/tempo/...
	::testing::GTEST_FLAG(filter) = "Enum_Test.*";
		Rts_i = RUN_ALL_TESTS();

	Sts_E = Bof_Shutdown();
	BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);

#if defined(DEBUG)
	std::cout << "\nPress any key followed by enter to to quit ..." << std::endl;
//	std::cin.ignore();
#endif

	return Rts_i;
}

