#include <bofstd/bofsystem.h>

#include "../include/gtestrunner.h"

#include "fmt/format.h"
#include <iostream>
#if defined(_WIN32)
// #define _CRTDBG_MAP_ALLOC
// #include <stdlib.h>
// #include <crtdbg.h>
#if defined(NDEBUG) // We are in Release compil
#else
#include <vld.h>
#endif

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
  printf("Assert in %s line %d Msg %s\n", _rFile_S.c_str(), _Line_U32, _rMasg_S.c_str());
  return BOF_ERR_NO_ERROR;
}

int main(int argc, char *argv[])
{
#if 0
  char *p = new char[5];
  return 0;
#else
  int Rts_i;
  BOFERR Sts_E;
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
  //::testing::GTEST_FLAG(filter) = "Threading_Test.SingleThread";
  //::testing::GTEST_FLAG(filter) = "SocketUdp_Test.*:BofIo_Test.OpenCloseCmdSession";
  //::testing::GTEST_FLAG(filter) = "Pipe_Test.UdpPipeSingle";

  Rts_i = RUN_ALL_TESTS();

  Sts_E = Bof_Shutdown();
  BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);

#if defined(NDEBUG) // We are in Release compil
#else
  std::string Buffer_S;
  std::cout << "\nPress any key followed by enter to to quit ..." << std::endl;
  //  std::getline(std::cin, Buffer_S);
#endif

  return Rts_i;
#endif
}

/*
* WINDOWS
[----------] Global test environment tear-down
[==========] 176 tests from 45 test suites ran. (131092 ms total)
[  PASSED  ] 176 tests.

  YOU HAVE 1 DISABLED TEST


  LINUX:

[----------] Global test environment tear-down
[==========] 164 tests from 43 test suites ran. (72546 ms total)
[  PASSED  ] 164 tests.

  YOU HAVE 2 DISABLED TESTS

need to launch test with
su
root@ad6c87cec684:/home/bha/bld/bofstd/tests# setcap CAP_SYS_NICE+ep ./bofstd-tests
exit
./bofstd-tests

to be able to use the FIFO scheduler in my docker container
*/