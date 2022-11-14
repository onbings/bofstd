#include <bofstd/bofsystem.h>

#include "../include/gtestrunner.h"

#include <iostream>
#include "fmt/format.h"

#if defined(_WIN32)
//#define _CRTDBG_MAP_ALLOC  
//#include <stdlib.h>  
//#include <crtdbg.h> 
#else
#include <malloc.h>
#endif

USE_BOF_NAMESPACE()

BOFERR AppBofAssertCallback(const std::string &_rFile_S, uint32_t _Line_U32, const std::string &_rMasg_S)
{
  printf("Assert in %s line %d Msg %s\r\n", _rFile_S.c_str(), _Line_U32, _rMasg_S.c_str());
  return BOF_ERR_NO_ERROR;
}

int main(int argc, char *argv[])
{
  BOFERR Sts_E;
  //Still this issue https://github.com/fmtlib/fmt/issues/
  //look for #if 00 in ut_stringformatter and active them when fixed
  //link fails with 
  // 1>fmtd.lib(fmtd.dll) : error LNK2005: "protected: __cdecl fmt::v8::detail::buffer<char>::buffer<char>(char *,unsigned __int64,unsigned __int64)" (??0?$buffer@D@detail@v8@fmt@@IEAA@PEAD_K1@Z) already defined in boflogchannel_spdlog.obj
  //if you activate the following line

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
  //	::testing::GTEST_FLAG(filter) = "String_Test.*:XmlParser_Test.*:JsonParser_Test.*:Bof2d_Test.*:Fs_Test.*";
    //::testing::GTEST_FLAG(filter) = "SocketOs_Test.*";	// :Uri_Test.*";
    //	::testing::GTEST_FLAG(filter) = "BofNaryTreeKv_Test.*";
//  	::testing::GTEST_FLAG(filter) = "XmlWriter_Test.Xml";
  //	::testing::GTEST_FLAG(filter) = "SocketOs_Test.*:BofNaryTreeKv_Test.*:Path_Test.*:Uri_Test.*:Fs_Test.*";
    //	::testing::GTEST_FLAG(filter) = "BofNaryTreeKv_Test.*:SocketOs_Test.*:Uri_Test.*";
  //	::testing::GTEST_FLAG(filter) = "Uri_Test.*:CmdLineParser_Test.*";	// XmlWriter_Test.Xml";
  //	::testing::GTEST_FLAG(filter) = "Bof2d_Test.MediaDetectorToJson:Bof2d_Test.MediaDetectorFromJson";	// XmlWriter_Test.Xml";
  //	::testing::GTEST_FLAG(filter) = "Timecode_Test.*:CmdLineParser_Test.*:System_Test.*";
  //::testing::GTEST_FLAG(filter) = "Bof2d_Test.MediaDetectorParseBuffer";  // MediaDetectorParam";
//  ::testing::GTEST_FLAG(filter) = "BofIo_Test.*";
//  ::testing::GTEST_FLAG(filter) = "Fs_Test.*";
/*
* under linux:
[  PASSED  ] 144 tests.
[  FAILED  ] 9 tests, listed below:
[  FAILED  ] SocketOs_Test.SocketAddress
[  FAILED  ] SocketTcp_Test.TcpClientTest
[  FAILED  ] SocketUdp_Test.FilterMulticastOnIpAddress
[  FAILED  ] SocketUdp_Test.ScatterGatherIo
[  FAILED  ] SocketUdp_Test.UdpClientTest
[  FAILED  ] Logger_Test.LoggerMultiChannel
[  FAILED  ] XmlWriter_Test.Xml
[  FAILED  ] System_Test.Util_AlignedAlloc
[  FAILED  ] Threading_Test.SingleThread

 9 FAILED TESTS
  YOU HAVE 5 DISABLED TESTS

*/
  Rts_i = RUN_ALL_TESTS();

  Sts_E = Bof_Shutdown();
  BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);

#if !defined(NDEBUG)
  std::cout << "\nPress any key followed by enter to to quit ..." << std::endl;
  //	std::cin.ignore();
#endif

  return Rts_i;
}

/*

[==========] 153 tests from 41 test suites ran. (86966 ms total)
[  PASSED  ] 144 tests.
[  FAILED  ] 9 tests, listed below:
[  FAILED  ] SocketOs_Test.SocketAddress
[  FAILED  ] SocketTcp_Test.TcpClientTest
[  FAILED  ] SocketUdp_Test.FilterMulticastOnIpAddress
[  FAILED  ] SocketUdp_Test.ScatterGatherIo
[  FAILED  ] SocketUdp_Test.UdpClientTest
[  FAILED  ] Logger_Test.LoggerMultiChannel
[  FAILED  ] XmlWriter_Test.Xml
[  FAILED  ] System_Test.Util_AlignedAlloc
[  FAILED  ] Threading_Test.SingleThread

 9 FAILED TESTS
  YOU HAVE 5 DISABLED TESTS


Press any key followed by enter to to quit ...
bha@d8d8bb40e3bd:~/bld/bofstd/tests$

*/