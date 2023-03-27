#include <bofstd/bofsystem.h>

#include "../include/gtestrunner.h"

#include "fmt/format.h"
#include <iostream>

#if defined(_WIN32)
// #define _CRTDBG_MAP_ALLOC
// #include <stdlib.h>
// #include <crtdbg.h>
#else
#include <malloc.h>
#endif

#if 0
//#include "asio.hpp"
#include <bofstd/bofsocketio.h>
//Bof_GetNetworkInterfaceInfo
void f()
{
  BOFSTDPARAM StdParam_X;
  BOF::Bof_Initialize(StdParam_X);
  std::vector<BOF::BOF_NETWORK_INTERFACE_PARAM> NetworkInterfaceCollection;
  BOFERR e = BOF::Bof_GetListOfNetworkInterface(NetworkInterfaceCollection);
  printf("Bof_GetListOfNetworkInterface found %zd Network board (%s)\n", NetworkInterfaceCollection.size(), BOF::Bof_ErrorCode(e));
  for (auto &rIt : NetworkInterfaceCollection)
  {
    printf("name %s ip %s msk %s brd %s %s\n", rIt.Name_S.c_str(), rIt.IpAddress_S.c_str(), rIt.IpMask_S.c_str(), rIt.IpBroadcast_S.c_str(), rIt.IpV6_B ? "IpV6":"IpV4");
    printf("Info: flg %X Gw %s Mtu %d Mac %02X:%02X:%02X:%02X:%02X:%02X\n", rIt.Info_X.InterfaceFlag_E, rIt.Info_X.IpGateway_S.c_str(), rIt.Info_X.MtuSize_U32, 
      rIt.Info_X.MacAddress[0], rIt.Info_X.MacAddress[1], rIt.Info_X.MacAddress[2], rIt.Info_X.MacAddress[3], rIt.Info_X.MacAddress[4], rIt.Info_X.MacAddress[5]);
  }
/*
  asio::io_context io;
  asio::ip::tcp::resolver resolver(io);
  asio::ip::tcp::resolver::query query(asio::ip::host_name(), "");
  asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
  while (it != asio::ip::tcp::resolver::iterator())
  {
    asio::ip::address addr = (it++)->endpoint().address();
    if (addr.is_v6())
    {
      std::cout << "ipv6 address: ";
    }
    else
      std::cout << "ipv4 address: ";

    std::cout << addr.to_string() << std::endl;

  }
  */
}
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
  // Still this issue https://github.com/fmtlib/fmt/issues/
  // look for #if 00 in ut_stringformatter and active them when fixed
  // link fails with
  //  1>fmtd.lib(fmtd.dll) : error LNK2005: "protected: __cdecl fmt::v8::detail::buffer<char>::buffer<char>(char *,unsigned __int64,unsigned __int64)" (??0?$buffer@D@detail@v8@fmt@@IEAA@PEAD_K1@Z) already defined in boflogchannel_spdlog.obj
  // if you activate the following line
//  f();
#if defined(_WIN32)
#else
#if defined(__ANDROID__)
#else
  mallopt(M_CHECK_ACTION, 3);
#endif
  signal(SIGPIPE, SIG_IGN);
#endif
  BOFSTDPARAM StdParam_X;
  StdParam_X.AssertInRelease_B = true;
  StdParam_X.AssertCallback = AppBofAssertCallback;
  Sts_E = Bof_Initialize(StdParam_X);
  BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);
  std::cout << "BofStd version " << Bof_GetVersion() << std::endl;

#if defined(_WIN32)
#else
  ::testing::GTEST_FLAG(filter) = "-Uart_Test.*"; // No hw
#endif
  testing::InitGoogleTest(&argc, argv);
  int Rts_i;
  ::testing::GTEST_FLAG(filter) = "XmlWriter_Test.Xml";
  //::testing::GTEST_FLAG(filter) = "SocketUdp_Test.*:BofIo_Test.OpenCloseCmdSession";

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
* WINDOWS
[----------] Global test environment tear-down
[==========] 162 tests from 43 test suites ran. (54946 ms total)
[  PASSED  ] 162 tests.

  YOU HAVE 2 DISABLED TESTS
*/