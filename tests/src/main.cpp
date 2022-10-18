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
//  	::testing::GTEST_FLAG(filter) = "DateTime_Test.*";
/*
[==========] 151 tests from 41 test suites ran. (56031 ms total)
[  PASSED  ] 151 tests.

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
bha@3f67ce95f2c5:~/bld/bofstd$ export LD_LIBRARY_PATH=/home/bha/bld/evs-muse/vcpkg_installed/evs-x64-swx-linux-dynamic/debug/lib:/home/bha/bld/vcpkg_installed/evs-x64-swx-linux-dynamic/lib:$LD_LIBRARY_PATH
bha@3f67ce95f2c5:~/bld/bofstd$ ctest .
Test project /home/bha/bld/bofstd
        Start   1: bofstd_Bof2d_Test.MediaDetectorParam
  1/158 Test   #1: bofstd_Bof2d_Test.MediaDetectorParam ....................................................................................................***Failed    1.59 sec
        Start   2: bofstd_Bof2d_Test.MediaDetectorParse
  2/158 Test   #2: bofstd_Bof2d_Test.MediaDetectorParse ....................................................................................................***Failed    1.06 sec
        Start   3: bofstd_Bof2d_Test.MediaDetectorParseBuffer
  3/158 Test   #3: bofstd_Bof2d_Test.MediaDetectorParseBuffer ..............................................................................................***Failed    1.06 sec
        Start   4: bofstd_Bof2d_Test.MediaDetectorQuery
  4/158 Test   #4: bofstd_Bof2d_Test.MediaDetectorQuery ....................................................................................................***Failed    1.06 sec
        Start   5: bofstd_Bof2d_Test.MediaDetectorToJson
  5/158 Test   #5: bofstd_Bof2d_Test.MediaDetectorToJson ...................................................................................................***Failed    1.06 sec
        Start   6: bofstd_Bof2d_Test.MediaDetectorFromJson
  6/158 Test   #6: bofstd_Bof2d_Test.MediaDetectorFromJson .................................................................................................   Passed    1.06 sec
        Start   7: bofstd_Bof2d_Test.UyvyToBgra
  7/158 Test   #7: bofstd_Bof2d_Test.UyvyToBgra ............................................................................................................***Failed    1.05 sec
        Start   8: bofstd_Bof2d_Test.BgraToUyvy
  8/158 Test   #8: bofstd_Bof2d_Test.BgraToUyvy ............................................................................................................***Failed    1.06 sec
        Start   9: bofstd_Api_Test.Version
  9/158 Test   #9: bofstd_Api_Test.Version .................................................................................................................   Passed    1.04 sec
        Start  10: bofstd_Api_Test.ErrorCode
 10/158 Test  #10: bofstd_Api_Test.ErrorCode ...............................................................................................................   Passed    1.04 sec
        Start  11: bofstd_Api_Test.Exception
 11/158 Test  #11: bofstd_Api_Test.Exception ...............................................................................................................   Passed    1.04 sec
        Start  12: bofstd_Bit_Test.BitSetPosition
 12/158 Test  #12: bofstd_Bit_Test.BitSetPosition ..........................................................................................................   Passed    1.04 sec
        Start  13: bofstd_CallbackCollection_Test.Void
 13/158 Test  #13: bofstd_CallbackCollection_Test.Void .....................................................................................................   Passed    1.04 sec
        Start  14: bofstd_CallbackCollection_Test.Constructor
 14/158 Test  #14: bofstd_CallbackCollection_Test.Constructor ..............................................................................................***Failed    1.04 sec
        Start  15: bofstd_CmdLineParser_Test.CmdLine
 15/158 Test  #15: bofstd_CmdLineParser_Test.CmdLine .......................................................................................................   Passed    1.07 sec
        Start  16: bofstd_Pipe_Test.UdpPipe
 16/158 Test  #16: bofstd_Pipe_Test.UdpPipe ................................................................................................................   Passed    2.04 sec
        Start  17: bofstd_SocketOs_Test.SocketAddress
 17/158 Test  #17: bofstd_SocketOs_Test.SocketAddress ......................................................................................................***Failed    1.07 sec
        Start  18: bofstd_SocketOs_Test.ListAndSetupInterface
 18/158 Test  #18: bofstd_SocketOs_Test.ListAndSetupInterface ..............................................................................................   Passed    1.04 sec
        Start  19: bofstd_SocketOs_Test.IpAddressBinSer
 19/158 Test  #19: bofstd_SocketOs_Test.IpAddressBinSer ....................................................................................................   Passed    1.04 sec
        Start  20: bofstd_SocketTcp_Test.TcpClientTest
 20/158 Test  #20: bofstd_SocketTcp_Test.TcpClientTest .....................................................................................................***Failed   10.40 sec
        Start  21: bofstd_SocketUdp_Test.FilterMulticastOnIpAddress
 21/158 Test  #21: bofstd_SocketUdp_Test.FilterMulticastOnIpAddress ........................................................................................***Failed    1.05 sec
        Start  22: bofstd_SocketUdp_Test.ScatterGatherIo
 22/158 Test  #22: bofstd_SocketUdp_Test.ScatterGatherIo ...................................................................................................***Failed    1.05 sec
        Start  23: bofstd_SocketUdp_Test.UdpClientTest
 23/158 Test  #23: bofstd_SocketUdp_Test.UdpClientTest .....................................................................................................***Failed   10.30 sec
        Start  24: bofstd_BofNaryTreeKv_Test.Create
 24/158 Test  #24: bofstd_BofNaryTreeKv_Test.Create ........................................................................................................   Passed    1.04 sec
        Start  25: bofstd_BofCircularBufferTemplate_Test.TheTest<char>
 25/158 Test  #25: bofstd_BofCircularBufferTemplate_Test.TheTest<char> .....................................................................................   Passed    1.04 sec
        Start  26: bofstd_BofCircularBufferTemplate_Test.TheTest<unsigned char>
 26/158 Test  #26: bofstd_BofCircularBufferTemplate_Test.TheTest<unsigned char> ............................................................................   Passed    1.04 sec
        Start  27: bofstd_BofCircularBufferTemplate_Test.TheTest<unsigned short>
 27/158 Test  #27: bofstd_BofCircularBufferTemplate_Test.TheTest<unsigned short> ...........................................................................   Passed    1.04 sec
        Start  28: bofstd_BofCircularBuffer_Test.PopExternalStorage
 28/158 Test  #28: bofstd_BofCircularBuffer_Test.PopExternalStorage ........................................................................................   Passed    1.04 sec
        Start  29: bofstd_BofCircularBuffer_Test.IsEntryFree
 29/158 Test  #29: bofstd_BofCircularBuffer_Test.IsEntryFree ...............................................................................................   Passed    1.04 sec
        Start  30: bofstd_BofCircularBuffer_Test.LockUnlock
 30/158 Test  #30: bofstd_BofCircularBuffer_Test.LockUnlock ................................................................................................   Passed    1.04 sec
        Start  31: bofstd_BofCircularBuffer_Test.BlockingMode
 31/158 Test  #31: bofstd_BofCircularBuffer_Test.BlockingMode ..............................................................................................   Passed   12.21 sec
        Start  32: bofstd_BofCircularBuffer_Test.StdString
 32/158 Test  #32: bofstd_BofCircularBuffer_Test.StdString .................................................................................................   Passed   13.46 sec
        Start  33: bofstd_BofRawCircularBufferNoSlotsize_Test.ByteBuffer
 33/158 Test  #33: bofstd_BofRawCircularBufferNoSlotsize_Test.ByteBuffer ...................................................................................   Passed    1.05 sec
        Start  34: bofstd_BofRawCircularBufferWithSlotsize_Test.CheckByteBuffer
 34/158 Test  #34: bofstd_BofRawCircularBufferWithSlotsize_Test.CheckByteBuffer ............................................................................   Passed    1.05 sec
        Start  35: bofstd_BofStringCircularBuffer_Test.ByteBuffer
 35/158 Test  #35: bofstd_BofStringCircularBuffer_Test.ByteBuffer ..........................................................................................   Passed    1.04 sec
        Start  36: bofstd_BofPot_Test.PotWithMagic
 36/158 Test  #36: bofstd_BofPot_Test.PotWithMagic .........................................................................................................   Passed    1.04 sec
        Start  37: bofstd_BofPot_Test.PotWithoutMagic
 37/158 Test  #37: bofstd_BofPot_Test.PotWithoutMagic ......................................................................................................   Passed    1.04 sec
        Start  38: bofstd_BofPot_Test.BlockingMode
 38/158 Test  #38: bofstd_BofPot_Test.BlockingMode .........................................................................................................   Passed    1.47 sec
        Start  39: bofstd_Queue_Test.Queue
 39/158 Test  #39: bofstd_Queue_Test.Queue .................................................................................................................   Passed    1.14 sec
        Start  40: bofstd_BofCrypto_Test.Sha1
 40/158 Test  #40: bofstd_BofCrypto_Test.Sha1 ..............................................................................................................   Passed    1.10 sec
        Start  41: bofstd_BofRamDb_Test.Init
 41/158 Test  #41: bofstd_BofRamDb_Test.Init ...............................................................................................................   Passed    1.04 sec
        Start  42: bofstd_BofRamDb_Test.Cursor
 42/158 Test  #42: bofstd_BofRamDb_Test.Cursor .............................................................................................................   Passed    1.04 sec
        Start  43: bofstd_BofRamDb_Test.Insert
 43/158 Test  #43: bofstd_BofRamDb_Test.Insert .............................................................................................................   Passed    1.04 sec
        Start  44: bofstd_Async_Test.AsyncMulticastDelegateLib
 44/158 Test  #44: bofstd_Async_Test.AsyncMulticastDelegateLib .............................................................................................   Passed    2.72 sec
        Start  45: bofstd_Async_Test.MulticastDelegatePerf
 45/158 Test  #45: bofstd_Async_Test.MulticastDelegatePerf .................................................................................................   Passed    1.51 sec
        Start  46: bofstd_Async_Test.AsyncNotifier
 46/158 Test  #46: bofstd_Async_Test.AsyncNotifier .........................................................................................................   Passed    1.78 sec
        Start  47: bofstd_Async_Test.SyncNotifier
 47/158 Test  #47: bofstd_Async_Test.SyncNotifier ..........................................................................................................   Passed    1.08 sec
        Start  48: bofstd_Enum_Test.WithDefaultValue
 48/158 Test  #48: bofstd_Enum_Test.WithDefaultValue .......................................................................................................   Passed    1.04 sec
        Start  49: bofstd_Enum_Test.WithoutDefaultValue
 49/158 Test  #49: bofstd_Enum_Test.WithoutDefaultValue ....................................................................................................   Passed    1.04 sec
        Start  50: bofstd_Path_Test.PathConstructorDestructorWindows
 50/158 Test  #50: bofstd_Path_Test.PathConstructorDestructorWindows .......................................................................................   Passed    1.04 sec
        Start  51: bofstd_Path_Test.PathConstructorDestructorLinux
 51/158 Test  #51: bofstd_Path_Test.PathConstructorDestructorLinux .........................................................................................   Passed    1.04 sec
        Start  52: bofstd_Path_Test.PathParsing
 52/158 Test  #52: bofstd_Path_Test.PathParsing ............................................................................................................   Passed    1.04 sec
        Start  53: bofstd_Uri_Test.UrlEscape
 53/158 Test  #53: bofstd_Uri_Test.UrlEscape ...............................................................................................................   Passed    1.04 sec
        Start  54: bofstd_Uri_Test.UriConstructorDestructor
 54/158 Test  #54: bofstd_Uri_Test.UriConstructorDestructor ................................................................................................   Passed    1.06 sec
        Start  55: bofstd_Uri_Test.Set
 55/158 Test  #55: bofstd_Uri_Test.Set .....................................................................................................................   Passed    1.05 sec
        Start  56: bofstd_Uri_Test.QueryParamCollection
 56/158 Test  #56: bofstd_Uri_Test.QueryParamCollection ....................................................................................................   Passed    1.05 sec
        Start  57: bofstd_Fs_Test.DirectoryManagement
 57/158 Test  #57: bofstd_Fs_Test.DirectoryManagement ......................................................................................................   Passed    1.05 sec
        Start  58: bofstd_Fs_Test.FileManagement
 58/158 Test  #58: bofstd_Fs_Test.FileManagement ...........................................................................................................   Passed    1.04 sec
        Start  59: bofstd_Fs_Test.EntireFile
 59/158 Test  #59: bofstd_Fs_Test.EntireFile ...............................................................................................................   Passed    1.04 sec
        Start  60: bofstd_Fs_Test.FileLayout
 60/158 Test  #60: bofstd_Fs_Test.FileLayout ...............................................................................................................   Passed    1.12 sec
        Start  61: bofstd_Fs_Test.DirEnum
 61/158 Test  #61: bofstd_Fs_Test.DirEnum ..................................................................................................................   Passed    1.14 sec
        Start  62: bofstd_Fs_Test.CreateTempFile
 62/158 Test  #62: bofstd_Fs_Test.CreateTempFile ...........................................................................................................   Passed    1.05 sec
        Start  63: bofstd_Fs_Test.ResetFileContentReOpenMode
 63/158 Test  #63: bofstd_Fs_Test.ResetFileContentReOpenMode ...............................................................................................   Passed    1.04 sec
        Start  64: bofstd_Fs_Test.ResetFileContent
 64/158 Test  #64: bofstd_Fs_Test.ResetFileContent .........................................................................................................   Passed    1.04 sec
        Start  65: bofstd_util_test.bit_cast
 65/158 Test  #65: bofstd_util_test.bit_cast ...............................................................................................................   Passed    1.04 sec
        Start  66: bofstd_util_test.increment
 66/158 Test  #66: bofstd_util_test.increment ..............................................................................................................   Passed    1.04 sec
        Start  67: bofstd_util_test.parse_nonnegative_int
 67/158 Test  #67: bofstd_util_test.parse_nonnegative_int ..................................................................................................   Passed    1.04 sec
        Start  68: bofstd_util_test.utf8_to_utf16
 68/158 Test  #68: bofstd_util_test.utf8_to_utf16 ..........................................................................................................   Passed    1.04 sec
        Start  69: bofstd_util_test.utf8_to_utf16_empty_string
 69/158 Test  #69: bofstd_util_test.utf8_to_utf16_empty_string .............................................................................................   Passed    1.04 sec
        Start  70: bofstd_Logger_Test.LoggerInit
 70/158 Test  #70: bofstd_Logger_Test.LoggerInit ...........................................................................................................   Passed    1.05 sec
        Start  71: bofstd_Logger_Test.LoggerMultiChannel
 71/158 Test  #71: bofstd_Logger_Test.LoggerMultiChannel ...................................................................................................***Failed    1.70 sec
        Start  72: bofstd_XmlParser_Test.Xml
 72/158 Test  #72: bofstd_XmlParser_Test.Xml ...............................................................................................................***Failed    1.04 sec
        Start  73: bofstd_XmlParser_Test.XmlVector
 73/158 Test  #73: bofstd_XmlParser_Test.XmlVector .........................................................................................................***Failed    1.05 sec
        Start  74: bofstd_XmlWriter_Test.XmlVector
 74/158 Test  #74: bofstd_XmlWriter_Test.XmlVector .........................................................................................................***Failed    1.05 sec
        Start  75: bofstd_XmlWriter_Test.Xml
 75/158 Test  #75: bofstd_XmlWriter_Test.Xml ...............................................................................................................***Failed    1.16 sec
        Start  76: bofstd_JsonParser_Test.JsonTypeNative
 76/158 Test  #76: bofstd_JsonParser_Test.JsonTypeNative ...................................................................................................   Passed    1.04 sec
        Start  77: bofstd_JsonParser_Test.JsonType
 77/158 Test  #77: bofstd_JsonParser_Test.JsonType .........................................................................................................***Exception: SegFault  0.21 sec
        Start  78: bofstd_JsonParser_Test.Json
 78/158 Test  #78: bofstd_JsonParser_Test.Json .............................................................................................................***Failed    1.05 sec
        Start  79: bofstd_JsonParser_Test.IpSenderDeser
 79/158 Test  #79: bofstd_JsonParser_Test.IpSenderDeser ....................................................................................................   Passed    1.05 sec
        Start  80: bofstd_JsonParser_Test.ManifestDeser
 80/158 Test  #80: bofstd_JsonParser_Test.ManifestDeser ....................................................................................................   Passed    1.04 sec
        Start  81: bofstd_JsonParser_Test.ConnectDeser
 81/158 Test  #81: bofstd_JsonParser_Test.ConnectDeser .....................................................................................................   Passed    1.04 sec
        Start  82: bofstd_JsonWriter_Test.Json
 82/158 Test  #82: bofstd_JsonWriter_Test.Json .............................................................................................................***Failed    1.07 sec
        Start  83: bofstd_JsonWriter_Test.IpSenderSer
 83/158 Test  #83: bofstd_JsonWriter_Test.IpSenderSer ......................................................................................................   Passed    1.05 sec
        Start  84: bofstd_JsonWriter_Test.ManifestSer
 84/158 Test  #84: bofstd_JsonWriter_Test.ManifestSer ......................................................................................................   Passed    1.05 sec
        Start  85: bofstd_JsonWriter_Test.ConnectSer
 85/158 Test  #85: bofstd_JsonWriter_Test.ConnectSer .......................................................................................................   Passed    1.04 sec
        Start  86: bofstd_JsonWriter_Test.IpSwitcherSerDeser
 86/158 Test  #86: bofstd_JsonWriter_Test.IpSwitcherSerDeser ...............................................................................................   Passed    1.05 sec
        Start  87: bofstd_BofIo_Test.CreateAndDestroyBofSocketServer
 87/158 Test  #87: bofstd_BofIo_Test.CreateAndDestroyBofSocketServer .......................................................................................***Not Run (Disabled)   0.00 sec
        Start  88: bofstd_BofIo_Test.SendPollSocketCommand
 88/158 Test  #88: bofstd_BofIo_Test.SendPollSocketCommand .................................................................................................***Not Run (Disabled)   0.00 sec
        Start  89: bofstd_BofIo_Test.OpenCloseCmdSession
 89/158 Test  #89: bofstd_BofIo_Test.OpenCloseCmdSession ...................................................................................................***Not Run (Disabled)   0.00 sec
        Start  90: bofstd_BofIo_Test.OpenCloseCmdDataSession
 90/158 Test  #90: bofstd_BofIo_Test.OpenCloseCmdDataSession ...............................................................................................***Not Run (Disabled)   0.00 sec
        Start  91: bofstd_BofIo_Test.ServerOpenCloseCmdDataSession
 91/158 Test  #91: bofstd_BofIo_Test.ServerOpenCloseCmdDataSession .........................................................................................***Not Run (Disabled)   0.00 sec
        Start  92: bofstd_BofScriptLauncher_Test.Execute_popen
 92/158 Test  #92: bofstd_BofScriptLauncher_Test.Execute_popen .............................................................................................   Passed    1.05 sec
        Start  93: bofstd_BofScriptLauncher_Test.Execute_vfork
 93/158 Test  #93: bofstd_BofScriptLauncher_Test.Execute_vfork .............................................................................................   Passed    7.04 sec
        Start  94: bofstd_BofScriptLauncher_Test.Execute_posix_spawn
 94/158 Test  #94: bofstd_BofScriptLauncher_Test.Execute_posix_spawn .......................................................................................   Passed    5.05 sec
        Start  95: bofstd_System_Test.TickDelta
 95/158 Test  #95: bofstd_System_Test.TickDelta ............................................................................................................   Passed    2.04 sec
        Start  96: bofstd_System_Test.EnvVar
 96/158 Test  #96: bofstd_System_Test.EnvVar ...............................................................................................................   Passed    1.04 sec
        Start  97: bofstd_System_Test.DumpMemoryZone
 97/158 Test  #97: bofstd_System_Test.DumpMemoryZone .......................................................................................................   Passed    1.04 sec
        Start  98: bofstd_System_Test.Util_Sprintf
 98/158 Test  #98: bofstd_System_Test.Util_Sprintf .........................................................................................................   Passed    1.04 sec
        Start  99: bofstd_System_Test.Util_AlignedAlloc
 99/158 Test  #99: bofstd_System_Test.Util_AlignedAlloc ....................................................................................................***Failed    1.04 sec
        Start 100: bofstd_System_Test.NanoTickCount
100/158 Test #100: bofstd_System_Test.NanoTickCount ........................................................................................................   Passed    2.04 sec
        Start 101: bofstd_System_Test.MsTickCount
101/158 Test #101: bofstd_System_Test.MsTickCount ..........................................................................................................   Passed    2.04 sec
        Start 102: bofstd_System_Test.Eol
102/158 Test #102: bofstd_System_Test.Eol ..................................................................................................................   Passed    1.04 sec
        Start 103: bofstd_System_Test.Random
103/158 Test #103: bofstd_System_Test.Random ...............................................................................................................   Passed    1.80 sec
        Start 104: bofstd_System_Test.RandomString
104/158 Test #104: bofstd_System_Test.RandomString .........................................................................................................   Passed    1.14 sec
        Start 105: bofstd_System_Test.Exec
105/158 Test #105: bofstd_System_Test.Exec .................................................................................................................   Passed    1.05 sec
        Start 106: bofstd_String_Test.MultiByteToWideChar
106/158 Test #106: bofstd_String_Test.MultiByteToWideChar ..................................................................................................   Passed    1.04 sec
        Start 107: bofstd_String_Test.WideCharToMultiByte
107/158 Test #107: bofstd_String_Test.WideCharToMultiByte ..................................................................................................   Passed    1.04 sec
        Start 108: bofstd_String_Test.StringTrim
108/158 Test #108: bofstd_String_Test.StringTrim ...........................................................................................................   Passed    1.04 sec
        Start 109: bofstd_String_Test.StringRemove
109/158 Test #109: bofstd_String_Test.StringRemove .........................................................................................................   Passed    1.05 sec
        Start 110: bofstd_String_Test.StringReplace
110/158 Test #110: bofstd_String_Test.StringReplace ........................................................................................................   Passed    1.04 sec
        Start 111: bofstd_String_Test.StringIsPresent
111/158 Test #111: bofstd_String_Test.StringIsPresent ......................................................................................................   Passed    1.04 sec
        Start 112: bofstd_String_Test.StringIsAllTheSameChar
112/158 Test #112: bofstd_String_Test.StringIsAllTheSameChar ...............................................................................................   Passed    1.04 sec
        Start 113: bofstd_String_Test.StringSplit
113/158 Test #113: bofstd_String_Test.StringSplit ..........................................................................................................   Passed    1.04 sec
        Start 114: bofstd_String_Test.StringJoin
114/158 Test #114: bofstd_String_Test.StringJoin ...........................................................................................................   Passed    1.04 sec
        Start 115: bofstd_String_Test.Bof_RemoveDuplicateSuccessiveCharacter
115/158 Test #115: bofstd_String_Test.Bof_RemoveDuplicateSuccessiveCharacter ...............................................................................   Passed    1.04 sec
        Start 116: bofstd_String_Test.MultipleKeyValueString
116/158 Test #116: bofstd_String_Test.MultipleKeyValueString ...............................................................................................   Passed    1.04 sec
        Start 117: bofstd_String_Test.Bof_FastZeroStrncpy
117/158 Test #117: bofstd_String_Test.Bof_FastZeroStrncpy ..................................................................................................   Passed    1.04 sec
        Start 118: bofstd_Timecode_Test.TcBin
118/158 Test #118: bofstd_Timecode_Test.TcBin ..............................................................................................................   Passed    1.04 sec
        Start 119: bofstd_Timecode_Test.Construct
119/158 Test #119: bofstd_Timecode_Test.Construct ..........................................................................................................   Passed    1.04 sec
        Start 120: bofstd_Timecode_Test.Operator
120/158 Test #120: bofstd_Timecode_Test.Operator ...........................................................................................................   Passed    1.04 sec
        Start 121: bofstd_Guid_Test.Guid
121/158 Test #121: bofstd_Guid_Test.Guid ...................................................................................................................   Passed    1.04 sec
        Start 122: bofstd_DateTime_Test.ValidateDateTime
122/158 Test #122: bofstd_DateTime_Test.ValidateDateTime ...................................................................................................   Passed    1.04 sec
        Start 123: bofstd_DateTime_Test.IsLeapYear
123/158 Test #123: bofstd_DateTime_Test.IsLeapYear .........................................................................................................   Passed    1.04 sec
        Start 124: bofstd_DateTime_Test.GetDateTime
124/158 Test #124: bofstd_DateTime_Test.GetDateTime ........................................................................................................   Passed    1.04 sec
        Start 125: bofstd_DateTime_Test.SetDateTime
125/158 Test #125: bofstd_DateTime_Test.SetDateTime ........................................................................................................   Passed    1.04 sec
        Start 126: bofstd_DateTime_Test.DeltaMsToHms
126/158 Test #126: bofstd_DateTime_Test.DeltaMsToHms .......................................................................................................   Passed    1.04 sec
        Start 127: bofstd_DateTime_Test.DiffDateTime
127/158 Test #127: bofstd_DateTime_Test.DiffDateTime .......................................................................................................   Passed    1.04 sec
        Start 128: bofstd_DateTime_Test.NbDaySinceUnixEpoch
128/158 Test #128: bofstd_DateTime_Test.NbDaySinceUnixEpoch ................................................................................................   Passed    1.04 sec
        Start 129: bofstd_DateTime_Test.StringDateTime
129/158 Test #129: bofstd_DateTime_Test.StringDateTime .....................................................................................................   Passed    1.04 sec
        Start 130: bofstd_DateTime_Test.TickSleep
130/158 Test #130: bofstd_DateTime_Test.TickSleep ..........................................................................................................   Passed    5.76 sec
        Start 131: bofstd_BofThread_Test.VirtualThreadTrue
*/