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
  // std::cout << "BofStd version " << Bof_GetVersion() << std::endl;
  printf("Running BofStd V %s on %s\n", StdParam_X.Version_S.c_str(), StdParam_X.ComputerName_S.c_str());
  // for (int i = 0; i < 7; i++)
  //{
  //   printf("hello world %d\n", i);
  // }
  //  const char *pp = StdParam_X.ComputerName_S.c_str();

#if defined(_WIN32)
#else
  // Ok on tge2, there is an uart  ::testing::GTEST_FLAG(filter) = "-Uart_Test.*"; // No hw
#endif
  testing::InitGoogleTest(&argc, argv);
  // Dont touch
  //::testing::GTEST_FLAG(filter) = "Api_Test.*";
  //::testing::GTEST_FLAG(filter) = "Async_Test.*";
  //::testing::GTEST_FLAG(filter) = "AsyncMuticastDelegate_Test.*";
  //::testing::GTEST_FLAG(filter) = "BinSerializer_Test.*";
  //::testing::GTEST_FLAG(filter) = "Bit_Test.*";
  //::testing::GTEST_FLAG(filter) = "ConIo_Test.*";
  //::testing::GTEST_FLAG(filter) = "CallbackCollection_Test.*";
  //::testing::GTEST_FLAG(filter) = "CircularBuffer_Test.*";
  //::testing::GTEST_FLAG(filter) = "CmdLineParser_Test.*";
  //::testing::GTEST_FLAG(filter) = "Crypto_Test.*";
  //::testing::GTEST_FLAG(filter) = "DateTime_Test.*";
  //::testing::GTEST_FLAG(filter) = "Enum_Test.*";
  //::testing::GTEST_FLAG(filter) = "Fs_Test.*";
  //::testing::GTEST_FLAG(filter) = "Guid_Test.*";
  //::testing::GTEST_FLAG(filter) = "JsonParser_Test.*";
  //::testing::GTEST_FLAG(filter) = "Logger_Test.*";
  //::testing::GTEST_FLAG(filter) = "NaryTreeKv_Test.*";
  //::testing::GTEST_FLAG(filter) = "Path_Test.*";
  //::testing::GTEST_FLAG(filter) = "Pipe_Test.*";
  //::testing::GTEST_FLAG(filter) = "Pot_Test.*";
  //::testing::GTEST_FLAG(filter) = "Process_Test.*";
  //::testing::GTEST_FLAG(filter) = "Queue_Test.*";
  //::testing::GTEST_FLAG(filter) = "RamDb_Test.*";
  //::testing::GTEST_FLAG(filter) = "RawCircularBufferNoSlotsize_Test.*";
  //::testing::GTEST_FLAG(filter) = "RawCircularBufferWithSlotsize_Test.*";
  //::testing::GTEST_FLAG(filter) = "Shell_Test.*";
  //::testing::GTEST_FLAG(filter) = "SocketOs_Test.ListAndSetupInterface"; //*";
  //::testing::GTEST_FLAG(filter) = "SocketTcp_Test.*";
  //::testing::GTEST_FLAG(filter) = "SocketUdp_Test.*";
  //::testing::GTEST_FLAG(filter) = "SockIo_Client_Server_Test.*";
  //::testing::GTEST_FLAG(filter) = "SockIo_Test.*";
  //::testing::GTEST_FLAG(filter) = "String_Test.*";
  //::testing::GTEST_FLAG(filter) = "StringCircularBuffer_Test.*";
  ////::testing::GTEST_FLAG(filter) = "stringformatter a fixer.*";
  //::testing::GTEST_FLAG(filter) = "System_Test.*";
  //::testing::GTEST_FLAG(filter) = "Threading_Test.*";
  //::testing::GTEST_FLAG(filter) = "Timecode_Test.*";
  //::testing::GTEST_FLAG(filter) = "Uart_Test.*";
  //::testing::GTEST_FLAG(filter) = "Uri_Test.*";
  //::testing::GTEST_FLAG(filter) = "XmlParser_Test.*";

  // Use these one
  //::testing::GTEST_FLAG(filter) = "Threading_Test.SingleThread";
  //::testing::GTEST_FLAG(filter) = "Threading_Test.MultiThread";
  //::testing::GTEST_FLAG(filter) = "Threading_Test.MultiThreadWithoutMutex";
  //::testing::GTEST_FLAG(filter) = "Threading_Test.MultiThreadWithMutex";
  //::testing::GTEST_FLAG(filter) = "System_Test.Rational";
  //::testing::GTEST_FLAG(filter) = "Bit_Test.*";
  //::testing::GTEST_FLAG(filter) = "Pipe_Test.*";
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
[==========] 174 tests from 46 test suites ran. (82449 ms total)
[  PASSED  ] 174 tests.

  YOU HAVE 7 DISABLED TESTS

  LINUX:

[==========] 170 tests from 43 test suites ran. (45271 ms total)
[  PASSED  ] 170 tests.

  YOU HAVE 6 DISABLED TESTS

need to launch test with
su
root@ad6c87cec684:/home/bha/bld/bofstd/tests# setcap CAP_SYS_NICE+ep ./bofstd-tests
exit
./bofstd-tests

to be able to use the FIFO scheduler in my docker container

On Tge2:
[==========] 174 tests from 46 test suites ran. (45466 ms total)
[  PASSED  ] 174 tests.

  YOU HAVE 7 DISABLED TESTS
*/
/*
root@evstge2:/root# ifconfig
eth-1gb0  Link encap:Ethernet  HWaddr 00:1C:F3:0A:20:44
          inet addr:10.129.171.112  Bcast:10.129.171.255  Mask:255.255.255.0
          inet6 addr: fe80::21c:f3ff:fe0a:2044/64 Scope:Link
          UP BROADCAST RUNNING MULTICAST  MTU:9000  Metric:1
          RX packets:10 errors:0 dropped:1 overruns:0 frame:0
          TX packets:32 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:10000
          RX bytes:1336 (1.3 KiB)  TX bytes:5334 (5.2 KiB)
          Memory:1ae4000-1ae4fff

eth-1gb1  Link encap:Ethernet  HWaddr 00:1C:F3:0A:20:45
          inet addr:10.129.172.21  Bcast:10.129.172.255  Mask:255.255.255.0
          inet6 addr: fe80::21c:f3ff:fe0a:2045/64 Scope:Link
          UP BROADCAST RUNNING MULTICAST  MTU:9000  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:29 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:10000
          RX bytes:0 (0.0 B)  TX bytes:5207 (5.0 KiB)
          Memory:1ae6000-1ae6fff

lo        Link encap:Local Loopback
          inet addr:127.0.0.1  Mask:255.0.0.0
          inet6 addr: ::1/128 Scope:Host
          UP LOOPBACK RUNNING  MTU:65536  Metric:1
          RX packets:360 errors:0 dropped:0 overruns:0 frame:0
          TX packets:360 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:28323 (27.6 KiB)  TX bytes:28323 (27.6 KiB)

root@evstge2:/root# ^C
root@evstge2:/root# ^C
root@evstge2:/root# ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host
       valid_lft forever preferred_lft forever
2: sit0@NONE: <NOARP> mtu 1480 qdisc noop state DOWN group default qlen 1000
    link/sit 0.0.0.0 brd 0.0.0.0
3: eth-1gb0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 9000 qdisc prio state UP group default qlen 10000
    link/ether 00:1c:f3:0a:20:44 brd ff:ff:ff:ff:ff:ff
    inet 10.129.171.112/24 brd 10.129.171.255 scope global eth-1gb0
       valid_lft forever preferred_lft forever
    inet6 fe80::21c:f3ff:fe0a:2044/64 scope link
       valid_lft forever preferred_lft forever
4: eth-1gb1: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 9000 qdisc prio state UP group default qlen 10000
    link/ether 00:1c:f3:0a:20:45 brd ff:ff:ff:ff:ff:ff
    inet 10.129.172.21/24 brd 10.129.172.255 scope global eth-1gb1
       valid_lft forever preferred_lft forever
    inet6 fe80::21c:f3ff:fe0a:2045/64 scope link
       valid_lft forever preferred_lft forever
5: eth-10gb0: <BROADCAST,MULTICAST> mtu 9000 qdisc noop state DOWN group default qlen 1000
    link/ether 00:1c:f3:0a:20:46 brd ff:ff:ff:ff:ff:ff
6: eth-10gb1: <BROADCAST,MULTICAST> mtu 9000 qdisc noop state DOWN group default qlen 1000
    link/ether 00:1c:f3:0a:20:47 brd ff:ff:ff:ff:ff:ff
root@evstge2:/root#

*/