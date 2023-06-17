#include <bofstd/boffs.h>
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
#if defined(_WIN32)
#include <vld.h>
#endif
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
  //::testing::GTEST_FLAG(filter) = "Threading_Test.SharedMemory"; // XmlParser_Test.XmlVector

  //::testing::GTEST_FLAG(filter) = "XmlParser_Test.*"; // XmlParser_Test.XmlVector
  //::testing::GTEST_FLAG(filter) = "Threading_Test.ThreadParameterFromString";
  //::testing::GTEST_FLAG(filter) = "DateTime_Test.ValidateDateTime";
  //::testing::GTEST_FLAG(filter) = "Threading_Test.SingleThread";
  //::testing::GTEST_FLAG(filter) = "Threading_Test.MultiThread";
  //::testing::GTEST_FLAG(filter) = "Threading_Test.MultiThreadWithoutMutex";
  //::testing::GTEST_FLAG(filter) = "Threading_Test.MultiThreadWithMutex";
  //::testing::GTEST_FLAG(filter) = "System_Test.Rational";
  //::testing::GTEST_FLAG(filter) = "Threading_Test.Mutex"; // SingleThread
  //  ::testing::GTEST_FLAG(filter) = "SocketUdp_Test.UdpClientTest";
  // std::string CrtDir_S;
  // BOF::Bof_GetCurrentDirectory(CrtDir_S);
  // printf("-CrtDir_S->%s\n", CrtDir_S.c_str());
  Rts_i = RUN_ALL_TESTS();

  Sts_E = Bof_Shutdown();
  BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);

#if defined(NDEBUG) // We are in Release compil
#else
  std::string Buffer_S;
  std::cout << "\nPress any key followed by enter to to quit ..." << std::endl;
  std::getline(std::cin, Buffer_S);
#endif

  return Rts_i;
#endif
}

/*
* WINDOWS
[----------] Global test environment tear-down
[==========] 176 tests from 45 test suites ran. (153571 ms total)
[  PASSED  ] 176 tests.

  YOU HAVE 1 DISABLED TEST

  LINUX:

[==========] 170 tests from 43 test suites ran. (45271 ms total)
[  PASSED  ] 169 tests.
[  FAILED  ] 1 test, listed below:
[  FAILED  ] Threading_Test.SharedMemory

 1 FAILED TEST
  YOU HAVE 6 DISABLED TESTS

need to launch test with
su
root@ad6c87cec684:/home/bha/bld/bofstd/tests# setcap CAP_SYS_NICE+ep ./bofstd-tests
exit
./bofstd-tests

to be able to use the FIFO scheduler in my docker container
*/
/*
[==========] 167 tests from 44 test suites ran. (72028 ms total)
[  PASSED  ] 160 tests.
[  FAILED  ] 7 tests, listed below:
[  FAILED  ] SocketOs_Test.ListAndSetupInterface
[  FAILED  ] DateTime_Test.ValidateDateTime
[  FAILED  ] Threading_Test.SingleThread
[  FAILED  ] Threading_Test.MultiThread
[  FAILED  ] Threading_Test.MultiThreadWithoutMutex
[  FAILED  ] Threading_Test.MultiThreadWithMutex
[  FAILED  ] System_Test.Rational

 7 FAILED TESTS
  YOU HAVE 9 DISABLED TESTS
*/