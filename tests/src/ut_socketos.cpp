/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the comchannel class
 *
 * Name:        ut_socketos.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#include <bofstd/bofsocketos.h>
#include "bofstd/bofstring.h"
#include "bofstd/bofbinserializer.h"

#include "gtestrunner.h"

USE_BOF_NAMESPACE()

//#define BOF_UT_INTERNET_AVAILABLE

void TestSocketAddress(bool _IsIpV6_B)
{
  BOFERR                            Sts_E;
  BOF_SOCKET_ADDRESS                pIpAddress_X[2], HostIpAddress_X, InterfaceIpAddress_X, IpAddress_X;
  bool                              Sts_B, IsIpV6_B;
  std::string                       Ip_S, HostName_S;
  uint32_t                          pIp_U32[4];
  BOF_SOCK_TYPE                     SocketType_E;
  std::vector< BOF_SOCKET_ADDRESS > ListOfIp_X;
  uint16_t                          Port_U16;
  BOF_SOCKET_ADDRESS_COMPONENT			InterfaceIpAddressComponent_X, IpAddressComponent_X;
  std::vector<uint16_t> IpDigitCollection;
  //	Sts_E=Bof_IpAddressToSocketAddressCollection("tcp://[102:3c0:405:6a8:708:901:a0b:c01]:1234", HostIpAddress_X);

  IpAddress_X = BOF_SOCKET_ADDRESS("1.2.3.4:8765");
  Ip_S = IpAddress_X.ToString(false, true);
  EXPECT_STREQ(Ip_S.c_str(), "1.2.3.4:8765");
  Ip_S = IpAddress_X.ToString(false, false);
  EXPECT_STREQ(Ip_S.c_str(), "1.2.3.4");
  Ip_S = IpAddress_X.ToString(true, false);
  EXPECT_STREQ(Ip_S.c_str(), "???://1.2.3.4");
  Ip_S = IpAddress_X.ToString(true, true);
  EXPECT_STREQ(Ip_S.c_str(), "???://1.2.3.4:8765");

  pIpAddress_X[0].Set(_IsIpV6_B, BOF_SOCK_TYPE::BOF_SOCK_TCP, 1, 2, 3, 4, 1234);
  pIpAddress_X[1].Set(_IsIpV6_B, BOF_SOCK_TYPE::BOF_SOCK_UDP, 0, 0, 0, 0, 4321);

  HostIpAddress_X = pIpAddress_X[0];
  Sts_B = Bof_IsIpAddressNull(HostIpAddress_X);
  EXPECT_FALSE(Sts_B);

  //Sts_E                                          = HostIpAddress_X.Reset();
  //EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_B = Bof_IsIpAddressNull(HostIpAddress_X);
  EXPECT_FALSE(Sts_B);

  Sts_B = Bof_IsIpAddressNull(pIpAddress_X[0]);
  EXPECT_FALSE(Sts_B);
  Sts_B = Bof_IsIpAddressNull(pIpAddress_X[1]);
  EXPECT_TRUE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(true, true, true, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_FALSE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(true, true, false, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_FALSE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(true, false, true, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_FALSE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(true, false, false, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_FALSE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(false, true, true, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_FALSE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(false, true, false, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_FALSE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(false, false, true, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_FALSE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(false, false, false, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_FALSE(Sts_B);
  Ip_S = Bof_SocketAddressToString(pIpAddress_X[0], true, true);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(Ip_S, "tcp://[0:1:0:2:0:3:0:4]:1234");
    Sts_E = Bof_IpAddressToBin(Ip_S, IsIpV6_B, IpDigitCollection);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(IpDigitCollection.size(), 8);
    EXPECT_EQ(IpDigitCollection[0], 0);
    EXPECT_EQ(IpDigitCollection[1], 1);
    EXPECT_EQ(IpDigitCollection[2], 0);
    EXPECT_EQ(IpDigitCollection[3], 2);
    EXPECT_EQ(IpDigitCollection[4], 0);
    EXPECT_EQ(IpDigitCollection[5], 3);
    EXPECT_EQ(IpDigitCollection[6], 0);
    EXPECT_EQ(IpDigitCollection[7], 4);
    Sts_B = Bof_IsMulticastIpAddress(Ip_S, InterfaceIpAddressComponent_X, IpAddressComponent_X);
    EXPECT_FALSE(Sts_B);
    Sts_B = Bof_IsMulticastIpAddress("udp://[FF00:1:0:2:0:3:0:4]:1234", InterfaceIpAddressComponent_X, IpAddressComponent_X);
    EXPECT_TRUE(Sts_B);
    Sts_B = Bof_IsMulticastIpAddress("udp://[FF00:1:0:2:0:3:0:4]", InterfaceIpAddressComponent_X, IpAddressComponent_X);
    EXPECT_TRUE(Sts_B);
    Sts_B = Bof_IsMulticastIpAddress("udp://[0:1:0:2:0:3:0:4]:1234>[FF00:1:0:2:0:3:0:4]:1234", InterfaceIpAddressComponent_X, IpAddressComponent_X);
    EXPECT_TRUE(Sts_B);

  }
  else
  {
    EXPECT_EQ(Ip_S, "tcp://1.2.3.4:1234");
    Sts_E = Bof_IpAddressToBin(Ip_S, IsIpV6_B, IpDigitCollection);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    EXPECT_EQ(IpDigitCollection.size(), 4);
    EXPECT_EQ(IpDigitCollection[0], 1);
    EXPECT_EQ(IpDigitCollection[1], 2);
    EXPECT_EQ(IpDigitCollection[2], 3);
    EXPECT_EQ(IpDigitCollection[3], 4);
    Sts_B = Bof_IsMulticastIpAddress(Ip_S, InterfaceIpAddressComponent_X, IpAddressComponent_X);
    EXPECT_FALSE(Sts_B);
    Sts_B = Bof_IsMulticastIpAddress("pgm://238.2.3.4:1234", InterfaceIpAddressComponent_X, IpAddressComponent_X);
    EXPECT_TRUE(Sts_B);
    //BOF_INTERFACE_ADDRESS_SEPARATOR is >
    Sts_B = Bof_IsMulticastIpAddress("pgm://192.168.1.1:1234>239.192.1.1:5555", InterfaceIpAddressComponent_X, IpAddressComponent_X);
    EXPECT_TRUE(Sts_B);
  }


  Ip_S = Bof_SocketAddressToString(pIpAddress_X[1], true, false);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(Ip_S, "udp://[::]");
  }
  else
  {
    EXPECT_EQ(Ip_S, "udp://0.0.0.0");
  }
  pIpAddress_X[1] = pIpAddress_X[0];
  Sts_B = Bof_IsIpAddressNull(pIpAddress_X[0]);
  EXPECT_FALSE(Sts_B);
  Sts_B = Bof_IsIpAddressNull(pIpAddress_X[1]);
  EXPECT_FALSE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(true, true, true, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_TRUE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(true, true, false, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_TRUE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(true, false, true, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_TRUE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(true, false, false, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_TRUE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(false, true, true, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_TRUE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(false, true, false, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_TRUE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(false, false, true, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_TRUE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(false, false, false, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_TRUE(Sts_B);

  if (_IsIpV6_B)
  {
    pIpAddress_X[1].IpV6Address_X.sin6_port = 4321;
  }
  else
  {
    pIpAddress_X[1].IpV4Address_X.sin_port = 4321;
  }
  Sts_B = Bof_IsIpAddressEqual(true, true, true, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_FALSE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(true, true, false, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_TRUE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(true, false, true, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_FALSE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(true, false, false, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_TRUE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(false, true, true, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_FALSE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(false, true, false, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_TRUE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(false, false, true, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_FALSE(Sts_B);
  Sts_B = Bof_IsIpAddressEqual(false, false, false, pIpAddress_X[0], pIpAddress_X[1]);
  EXPECT_TRUE(Sts_B);
  pIpAddress_X[0].Parse(SocketType_E, pIp_U32[0], pIp_U32[1], pIp_U32[2], pIp_U32[3], Port_U16);

  EXPECT_EQ(pIp_U32[0], 1);
  EXPECT_EQ(pIp_U32[1], 2);
  EXPECT_EQ(pIp_U32[2], 3);
  EXPECT_EQ(pIp_U32[3], 4);

  EXPECT_EQ(Port_U16, 1234);
  EXPECT_EQ(SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_TCP);

  Sts_E = Bof_IpAddressToSocketAddress("10.131.125", HostIpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_IpAddressToSocketAddress("10.131.125,250", HostIpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_IpAddressToSocketAddress("10.131.125.ab", HostIpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_IpAddressToSocketAddress("10.131.125.250", HostIpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_IpAddressToSocketAddress("tcp://10.131.125.250", HostIpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_IpAddressToSocketAddress("tcp://1.2.3.4:5", HostIpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_IpAddressToSocketAddress("", IpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_IpAddressToSocketAddress("a.b.c.d", IpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_IpAddressToSocketAddress("1.2.3.4", IpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(IpAddress_X.SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN);
  Ip_S = Bof_SocketAddressToString(IpAddress_X, true, true);
  EXPECT_EQ(Ip_S, "???://1.2.3.4:0");

  Sts_E = Bof_IpAddressToSocketAddress("1.2.3.4:", IpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_IpAddressToSocketAddress("1.2.3.4:5", IpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(IpAddress_X.IpV6Address_X.sin6_port, 0); //Bof_IsCpuLittleEndian() ? 5 * 256 : 5);
  }
  else
  {
    EXPECT_EQ(IpAddress_X.IpV4Address_X.sin_port, Bof_IsCpuLittleEndian() ? 5 * 256 : 5);
  }
  Ip_S = Bof_SocketAddressToString(IpAddress_X, false, true);
  EXPECT_EQ(Ip_S, "1.2.3.4:5");
#if defined (BOF_UT_INTERNET_AVAILABLE)
  Sts_E = Bof_UrlAddressToSocketAddressCollection("www.google.com:1", ListOfIp_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_TRUE(ListOfIp_X.size() >= 1);
  EXPECT_EQ(ListOfIp_X[0].ProtocolType_E, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN);
  EXPECT_EQ(ListOfIp_X[0].SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(ListOfIp_X[0].IpV6Address_X.sin6_port, 0);	// Bof_IsCpuLittleEndian() ? 1 * 256 : 1);
  }
  else
  {
    EXPECT_EQ(ListOfIp_X[0].IpV4Address_X.sin_port, Bof_IsCpuLittleEndian() ? 1 * 256 : 1);
  }
  Ip_S = Bof_SocketAddressToString(ListOfIp_X[0], false, true);
  // Change EXPECT_EQ(Ip_S, "216.58.204.68:1");

  Sts_E = Bof_UrlAddressToSocketAddressCollection("www.google.com", ListOfIp_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_TRUE(ListOfIp_X.size() >= 1);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(ListOfIp_X[0].IpV6Address_X.sin6_port, 0);
  }
  else
  {
    EXPECT_EQ(ListOfIp_X[0].IpV4Address_X.sin_port, 0);
  }
  Ip_S = Bof_SocketAddressToString(ListOfIp_X[0], false, true);
  // Change EXPECT_EQ(Ip_S, "216.58.204.68:0");

  Sts_E = Bof_UrlAddressToSocketAddressCollection("www.yahoo.com:2124", ListOfIp_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_GE(ListOfIp_X.size(), (uint32_t)1);
  EXPECT_EQ(ListOfIp_X[0].ProtocolType_E, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN);
  EXPECT_EQ(ListOfIp_X[0].SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(ListOfIp_X[0].IpV6Address_X.sin6_port, 0);	//Bof_IsCpuLittleEndian() ? 0x4C08 : 2124);
  }
  else
  {
    EXPECT_EQ(ListOfIp_X[0].IpV4Address_X.sin_port, Bof_IsCpuLittleEndian() ? 0x4C08 : 2124);
  }
  Ip_S = Bof_SocketAddressToString(ListOfIp_X[0], true, true);

  Sts_E = Bof_UrlAddressToSocketAddressCollection("www.msn.com", ListOfIp_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_GE(ListOfIp_X.size(), 1);
  EXPECT_EQ(ListOfIp_X[0].ProtocolType_E, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN);
  EXPECT_EQ(ListOfIp_X[0].SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(ListOfIp_X[0].IpV6Address_X.sin6_port, 0);
  }
  else
  {
    EXPECT_EQ(ListOfIp_X[0].IpV4Address_X.sin_port, 0);
  }
  Ip_S = Bof_SocketAddressToString(ListOfIp_X[0], true, false);
#if defined(_WIN32)
  EXPECT_EQ(Ip_S, "???://204.79.197.203");
#else
  EXPECT_EQ(Ip_S, "???://204.79.197.203");
#endif
#endif

  Sts_E = Bof_IpAddressToSocketAddress("", IpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_IpAddressToSocketAddress("a.b.c.d", IpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_IpAddressToSocketAddress("1.2.3.4", IpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(IpAddress_X.SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(IpAddress_X.IpV6Address_X.sin6_port, 0);
  }
  else
  {
    EXPECT_EQ(IpAddress_X.IpV4Address_X.sin_port, 0);
  }
  Ip_S = Bof_SocketAddressToString(IpAddress_X, true, true);
  EXPECT_EQ(Ip_S, "???://1.2.3.4:0");

  Sts_E = Bof_IpAddressToSocketAddress("1.2.3.4:", IpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_IpAddressToSocketAddress("1.2.3.4:5", IpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(IpAddress_X.SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(IpAddress_X.IpV6Address_X.sin6_port, 0); // Bof_IsCpuLittleEndian() ? 5 * 256 : 5);
  }
  else
  {
    EXPECT_EQ(IpAddress_X.IpV4Address_X.sin_port, Bof_IsCpuLittleEndian() ? 5 * 256 : 5);
  }
  Ip_S = Bof_SocketAddressToString(IpAddress_X, false, true);
  EXPECT_EQ(Ip_S, "1.2.3.4:5");
#if defined (BOF_UT_INTERNET_AVAILABLE)       // need internet
  std::vector<BOF_SOCKET_ADDRESS> ListOfIpAddress_X;

  Sts_E = Bof_UrlAddressToSocketAddressCollection("www.google.com:1", ListOfIpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(IpAddress_X.ProtocolType_E, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN);
  EXPECT_EQ(IpAddress_X.SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(IpAddress_X.IpV6Address_X.sin6_port, 0);	// Bof_IsCpuLittleEndian() ? 1 * 256 : 1);
  }
  else
  {
    EXPECT_EQ(IpAddress_X.IpV4Address_X.sin_port, Bof_IsCpuLittleEndian() ? 1 * 256 : 1);
  }
  Ip_S = Bof_SocketAddressToString(IpAddress_X, false, true);
  // Change EXPECT_EQ(Ip_S, "216.58.204.68:1");

  Sts_E = Bof_UrlAddressToSocketAddressCollection("www.google.com", ListOfIpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  IpAddress_X = ListOfIpAddress_X[0];
  EXPECT_EQ(IpAddress_X.ProtocolType_E, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN);
  EXPECT_EQ(IpAddress_X.SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(IpAddress_X.IpV6Address_X.sin6_port, 0);
  }
  else
  {
    EXPECT_EQ(IpAddress_X.IpV4Address_X.sin_port, 0);
  }
  Ip_S = Bof_SocketAddressToString(IpAddress_X, false, true);
  // Change EXPECT_EQ(Ip_S, "216.58.204.68:0");

  Sts_E = Bof_UrlAddressToSocketAddressCollection("www.yahoo.com:2124", ListOfIpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  IpAddress_X = ListOfIpAddress_X[0];
  EXPECT_EQ(IpAddress_X.ProtocolType_E, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN);
  EXPECT_EQ(IpAddress_X.SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(IpAddress_X.IpV6Address_X.sin6_port, 0);	// Bof_IsCpuLittleEndian() ? 0x4C08 : 2124);
  }
  else
  {
    EXPECT_EQ(IpAddress_X.IpV4Address_X.sin_port, Bof_IsCpuLittleEndian() ? 0x4C08 : 2124);
  }
  Ip_S = Bof_SocketAddressToString(IpAddress_X, true, true);

  Sts_E = Bof_UrlAddressToSocketAddressCollection("www.msn.com", ListOfIpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  IpAddress_X = ListOfIpAddress_X[0];
  EXPECT_EQ(IpAddress_X.ProtocolType_E, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN);
  EXPECT_EQ(IpAddress_X.SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(IpAddress_X.IpV6Address_X.sin6_port, 0);
  }
  else
  {
    EXPECT_EQ(IpAddress_X.IpV4Address_X.sin_port, 0);
  }
  Ip_S = Bof_SocketAddressToString(IpAddress_X, true, false);
  EXPECT_EQ(Ip_S, "???://204.79.197.203");

#endif
  //Bof_IpAddressToSocketAddress


  BOF_SOCKET_ADDRESS_COMPONENT Uri_X;
  std::string Path_S, Query_S, Fragment_S;
myprotocol://john.doe:password@www.google.com:123/forum/questions/file.txt?justkey&order=newest;tag=networking#top
///myprotocol: / forum / questions / file.txt ? justkey & order = newest; tag = networking#top
  Sts_E = Bof_SplitUri("myprotocol://john.doe:password@www.google.com:123/forum/questions/file.txt?justkey&order=newest;tag=networking#top", Uri_X, Path_S, Query_S, Fragment_S);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Uri_X.Protocol_S.c_str(), "myprotocol");
  EXPECT_STREQ(Uri_X.User_S.c_str(), "john.doe");
  EXPECT_STREQ(Uri_X.Password_S.c_str(), "password");
  EXPECT_STREQ(Uri_X.IpAddress_S.c_str(), "www.google.com");
  EXPECT_EQ(Uri_X.Port_U16, 123);
  EXPECT_STREQ(Path_S.c_str(), "/forum/questions/file.txt");
  EXPECT_STREQ(Query_S.c_str(), "justkey&order=newest;tag=networking");
  EXPECT_STREQ(Fragment_S.c_str(), "top");

  Sts_E = Bof_SplitUri("protocol:/forum/questions/thefile.txt?justkey;order=newest&tag=networking#top", Uri_X, Path_S, Query_S, Fragment_S);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Uri_X.Protocol_S.c_str(), "protocol");
  EXPECT_STREQ(Uri_X.User_S.c_str(), "");
  EXPECT_STREQ(Uri_X.Password_S.c_str(), "");
  EXPECT_STREQ(Uri_X.IpAddress_S.c_str(), "");
  EXPECT_EQ(Uri_X.Port_U16, 0);
  EXPECT_STREQ(Path_S.c_str(), "/forum/questions/thefile.txt");
  EXPECT_STREQ(Query_S.c_str(), "justkey;order=newest&tag=networking");
  EXPECT_STREQ(Fragment_S.c_str(), "top");

  Sts_E = Bof_SplitIpAddress("10.131.125", HostIpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_SplitIpAddress("10.131.125,250", HostIpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);
  Sts_E = Bof_SplitIpAddress("10.131.125.ab", HostIpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_SplitIpAddress("tcp://192.168.1.1:1234", InterfaceIpAddressComponent_X, IpAddressComponent_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_STREQ(InterfaceIpAddressComponent_X.Protocol_S.c_str(), "");
  EXPECT_STREQ(InterfaceIpAddressComponent_X.IpAddress_S.c_str(), "");
  EXPECT_EQ(InterfaceIpAddressComponent_X.Port_U16, 0);
  EXPECT_STREQ(IpAddressComponent_X.Protocol_S.c_str(), "tcp");
  EXPECT_STREQ(IpAddressComponent_X.IpAddress_S.c_str(), "192.168.1.1");
  EXPECT_EQ(IpAddressComponent_X.Port_U16, 1234);
  Ip_S = Bof_SocketAddressToString(InterfaceIpAddressComponent_X.Ip_X, false, true);
  EXPECT_EQ(Ip_S, "0.0.0.0:0");
  Ip_S = Bof_SocketAddressToString(IpAddressComponent_X.Ip_X, false, true);
  EXPECT_EQ(Ip_S, "192.168.1.1:1234");

  Sts_E = Bof_SplitIpAddress("udp://193.169.2.3:456>194.170.3.4:7", InterfaceIpAddressComponent_X, IpAddressComponent_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_STREQ(InterfaceIpAddressComponent_X.Protocol_S.c_str(), "udp");
  EXPECT_STREQ(InterfaceIpAddressComponent_X.IpAddress_S.c_str(), "193.169.2.3");
  EXPECT_EQ(InterfaceIpAddressComponent_X.Port_U16, 456);
  EXPECT_STREQ(IpAddressComponent_X.Protocol_S.c_str(), "udp");
  EXPECT_STREQ(IpAddressComponent_X.IpAddress_S.c_str(), "194.170.3.4");
  EXPECT_EQ(IpAddressComponent_X.Port_U16, 7);
  Ip_S = Bof_SocketAddressToString(InterfaceIpAddressComponent_X.Ip_X, false, true);
  EXPECT_EQ(Ip_S, "193.169.2.3:456");
  Ip_S = Bof_SocketAddressToString(IpAddressComponent_X.Ip_X, false, true);
  EXPECT_EQ(Ip_S, "194.170.3.4:7");

  Sts_E = Bof_SplitIpAddress("udp://193.169.2.3>194.170.3.4:7", InterfaceIpAddressComponent_X, IpAddressComponent_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_STREQ(InterfaceIpAddressComponent_X.Protocol_S.c_str(), "udp");
  EXPECT_STREQ(InterfaceIpAddressComponent_X.IpAddress_S.c_str(), "193.169.2.3");
  EXPECT_EQ(InterfaceIpAddressComponent_X.Port_U16, 0);
  EXPECT_STREQ(IpAddressComponent_X.Protocol_S.c_str(), "udp");
  EXPECT_STREQ(IpAddressComponent_X.IpAddress_S.c_str(), "194.170.3.4");
  EXPECT_EQ(IpAddressComponent_X.Port_U16, 7);
  Ip_S = Bof_SocketAddressToString(InterfaceIpAddressComponent_X.Ip_X, false, true);
  EXPECT_EQ(Ip_S, "193.169.2.3:0");
  Ip_S = Bof_SocketAddressToString(IpAddressComponent_X.Ip_X, false, true);
  EXPECT_EQ(Ip_S, "194.170.3.4:7");

  Sts_E = Bof_SplitIpAddress("udp://193.169.2.3:456>194.170.3.4", InterfaceIpAddressComponent_X, IpAddressComponent_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_STREQ(InterfaceIpAddressComponent_X.Protocol_S.c_str(), "udp");
  EXPECT_STREQ(InterfaceIpAddressComponent_X.IpAddress_S.c_str(), "193.169.2.3");
  EXPECT_EQ(InterfaceIpAddressComponent_X.Port_U16, 456);
  EXPECT_STREQ(IpAddressComponent_X.Protocol_S.c_str(), "udp");
  EXPECT_STREQ(IpAddressComponent_X.IpAddress_S.c_str(), "194.170.3.4");
  EXPECT_EQ(IpAddressComponent_X.Port_U16, 0);
  Ip_S = Bof_SocketAddressToString(InterfaceIpAddressComponent_X.Ip_X, false, true);
  EXPECT_EQ(Ip_S, "193.169.2.3:456");
  Ip_S = Bof_SocketAddressToString(IpAddressComponent_X.Ip_X, false, true);
  EXPECT_EQ(Ip_S, "194.170.3.4:0");

  Sts_E = Bof_SplitIpAddress("udp://193.169.2.3:0>194.170.3.4:0", InterfaceIpAddressComponent_X, IpAddressComponent_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_STREQ(InterfaceIpAddressComponent_X.Protocol_S.c_str(), "udp");
  EXPECT_STREQ(InterfaceIpAddressComponent_X.IpAddress_S.c_str(), "193.169.2.3");
  EXPECT_EQ(InterfaceIpAddressComponent_X.Port_U16, 0);
  EXPECT_STREQ(IpAddressComponent_X.Protocol_S.c_str(), "udp");
  EXPECT_STREQ(IpAddressComponent_X.IpAddress_S.c_str(), "194.170.3.4");
  EXPECT_EQ(IpAddressComponent_X.Port_U16, 0);
  Ip_S = Bof_SocketAddressToString(InterfaceIpAddressComponent_X.Ip_X, false, true);
  EXPECT_EQ(Ip_S, "193.169.2.3:0");
  Ip_S = Bof_SocketAddressToString(IpAddressComponent_X.Ip_X, false, true);
  EXPECT_EQ(Ip_S, "194.170.3.4:0");

  Sts_E = Bof_SplitIpAddress("bha://john.doe:password@1.2.3.4:5", IpAddressComponent_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_STREQ(IpAddressComponent_X.Protocol_S.c_str(), "bha");
  EXPECT_STREQ(IpAddressComponent_X.IpAddress_S.c_str(), "1.2.3.4");
  EXPECT_EQ(IpAddressComponent_X.Port_U16, 5);
  EXPECT_STREQ(IpAddressComponent_X.User_S.c_str(), "john.doe");
  EXPECT_STREQ(IpAddressComponent_X.Password_S.c_str(), "password");
  Ip_S = Bof_SocketAddressToString(IpAddressComponent_X.Ip_X, true, true);
  EXPECT_EQ(Ip_S, "???://1.2.3.4:5");

  Sts_E = Bof_SplitIpAddress("bha://john.doe@1.2.3.4:5", IpAddressComponent_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_STREQ(IpAddressComponent_X.Protocol_S.c_str(), "bha");
  EXPECT_STREQ(IpAddressComponent_X.IpAddress_S.c_str(), "1.2.3.4");
  EXPECT_EQ(IpAddressComponent_X.Port_U16, 5);
  EXPECT_STREQ(IpAddressComponent_X.User_S.c_str(), "john.doe");
  EXPECT_STREQ(IpAddressComponent_X.Password_S.c_str(), "");
  Ip_S = Bof_SocketAddressToString(IpAddressComponent_X.Ip_X, true, true);
  EXPECT_EQ(Ip_S, "???://1.2.3.4:5");

  Sts_E = Bof_SplitIpAddress("bha://john.doe@1.2.3.4", IpAddressComponent_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_STREQ(IpAddressComponent_X.Protocol_S.c_str(), "bha");
  EXPECT_STREQ(IpAddressComponent_X.IpAddress_S.c_str(), "1.2.3.4");
  EXPECT_EQ(IpAddressComponent_X.Port_U16, 0);
  EXPECT_STREQ(IpAddressComponent_X.User_S.c_str(), "john.doe");
  EXPECT_STREQ(IpAddressComponent_X.Password_S.c_str(), "");
  Ip_S = Bof_SocketAddressToString(IpAddressComponent_X.Ip_X, true, true);
  EXPECT_EQ(Ip_S, "???://1.2.3.4:0");

  Sts_E = Bof_SplitIpAddress("udp://193.169.2.3:0>john.doe:password@www.google.com:123", InterfaceIpAddressComponent_X, IpAddressComponent_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_STREQ(InterfaceIpAddressComponent_X.Protocol_S.c_str(), "udp");
  EXPECT_STREQ(InterfaceIpAddressComponent_X.IpAddress_S.c_str(), "193.169.2.3");
  EXPECT_EQ(InterfaceIpAddressComponent_X.Port_U16, 0);
  EXPECT_STREQ(IpAddressComponent_X.User_S.c_str(), "john.doe");
  EXPECT_STREQ(IpAddressComponent_X.Password_S.c_str(), "password");
  EXPECT_EQ(IpAddressComponent_X.Port_U16, 123);
  Ip_S = Bof_SocketAddressToString(InterfaceIpAddressComponent_X.Ip_X, false, true);
  EXPECT_EQ(Ip_S, "193.169.2.3:0");
  Ip_S = Bof_SocketAddressToString(IpAddressComponent_X.Ip_X, false, true);
  //EXPECT_EQ(Ip_S, "142.251.36.4:123"); can change www.google.com


  Sts_E = Bof_ResolveIpAddress("tcp://192.168.1.1:1234", InterfaceIpAddress_X, IpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  InterfaceIpAddress_X.SocketType_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
  if (_IsIpV6_B)
  {
    InterfaceIpAddress_X.Set(_IsIpV6_B, BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN, 0, 0, 0, 0, 0);
    InterfaceIpAddress_X.IpV6Address_X.sin6_family = 0;
  }
  else
  {
    InterfaceIpAddress_X.Set(_IsIpV6_B, BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN, 0, 0, 0, 0, 0);
    InterfaceIpAddress_X.IpV4Address_X.sin_family = 0;
  }
  Ip_S = Bof_SocketAddressToString(InterfaceIpAddress_X, true, true);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(Ip_S, "???://[::]:0");
  }
  else
  {
    EXPECT_EQ(Ip_S, "???://0.0.0.0:0");
  }
  Ip_S = Bof_BuildIpAddress(InterfaceIpAddress_X, IpAddress_X);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(Ip_S, "???://[::]:0>192.168.1.1:1234");
  }
  else
  {
    EXPECT_EQ(Ip_S, "???://0.0.0.0:0>192.168.1.1:1234");
  }

  if (_IsIpV6_B)
  {
    InterfaceIpAddress_X.Set(_IsIpV6_B, BOF_SOCK_TYPE::BOF_SOCK_TCP, 0x010203C0, 0x040506A8, 0x07080901, 0x0A0B0C01, 1234);
  }
  else
  {
    InterfaceIpAddress_X.Set(_IsIpV6_B, BOF_SOCK_TYPE::BOF_SOCK_TCP, 192, 168, 1, 1, 1234);
  }
  Ip_S = Bof_SocketAddressToString(IpAddress_X, true, true);
  EXPECT_EQ(Ip_S, "tcp://192.168.1.1:1234");
  Ip_S = Bof_BuildIpAddress(InterfaceIpAddress_X, IpAddress_X);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(Ip_S, "tcp://[102:3c0:405:6a8:708:901:a0b:c01]:1234>192.168.1.1:1234");
  }
  else
  {
    EXPECT_EQ(Ip_S, "tcp://192.168.1.1:1234>192.168.1.1:1234");
  }
  Sts_E = Bof_ResolveIpAddress("pgm://192.168.10.1:1235>239.192.1.1:5555", InterfaceIpAddress_X, IpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  Ip_S = Bof_SocketAddressToString(InterfaceIpAddress_X, true, true);
  EXPECT_EQ(Ip_S, "???://192.168.10.1:1235");
  Ip_S = Bof_SocketAddressToString(IpAddress_X, true, true);
  EXPECT_EQ(Ip_S, "???://239.192.1.1:5555");

  if (_IsIpV6_B)
  {
    InterfaceIpAddress_X.Set(_IsIpV6_B, BOF_SOCK_TYPE::BOF_SOCK_UDP, 0x010203C0, 0x040506A8, 0x0708090A, 0x0A0B0C01, 1235);
  }
  else
  {
    InterfaceIpAddress_X.Set(_IsIpV6_B, BOF_SOCK_TYPE::BOF_SOCK_UDP, 192, 168, 10, 1, 1235);
  }
  Ip_S = Bof_SocketAddressToString(InterfaceIpAddress_X, true, true);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(Ip_S, "udp://[102:3c0:405:6a8:708:90a:a0b:c01]:1235");
  }
  else
  {
    EXPECT_EQ(Ip_S, "udp://192.168.10.1:1235");
  }
  Ip_S = Bof_BuildIpAddress(InterfaceIpAddress_X, IpAddress_X);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(Ip_S, "udp://[102:3c0:405:6a8:708:90a:a0b:c01]:1235>239.192.1.1:5555");
  }
  else
  {
    EXPECT_EQ(Ip_S, "udp://192.168.10.1:1235>239.192.1.1:5555");
  }
  if (_IsIpV6_B)
  {
    IpAddress_X.Set(_IsIpV6_B, BOF_SOCK_TYPE::BOF_SOCK_TCP, 0x010203EF, 0x040506C0, 0x07080901, 0x0A0B0C01, 5555);
  }
  else
  {
    IpAddress_X.Set(_IsIpV6_B, BOF_SOCK_TYPE::BOF_SOCK_TCP, 239, 192, 1, 1, 5555);
  }
  Ip_S = Bof_SocketAddressToString(IpAddress_X, true, true);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(Ip_S, "tcp://[102:3ef:405:6c0:708:901:a0b:c01]:5555");
  }
  else
  {
    EXPECT_EQ(Ip_S, "tcp://239.192.1.1:5555");
  }
  Ip_S = Bof_BuildIpAddress(InterfaceIpAddress_X, IpAddress_X);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(Ip_S, "udp://[102:3c0:405:6a8:708:90a:a0b:c01]:1235>[102:3ef:405:6c0:708:901:a0b:c01]:5555");
  }
  else
  {
    EXPECT_EQ(Ip_S, "udp://192.168.10.1:1235>239.192.1.1:5555");
  }
  Sts_E = Bof_ResolveIpAddress("udp://192.168.10.1:1235;239.192.1.1:5555", InterfaceIpAddress_X, IpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_ResolveIpAddress("PGM://192.168.10.1,1235>239.192.1.1:5555", InterfaceIpAddress_X, IpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_ResolveIpAddress("PGM://192.168.10.1:1235>239.192.1.1,5555", InterfaceIpAddress_X, IpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_ResolveIpAddress("PGM://192.168.10.1:>239.192.1.1:5555", InterfaceIpAddress_X, IpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_ResolveIpAddress("PGM://192.168.10.1:1235>239.192.1.1:", InterfaceIpAddress_X, IpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_ResolveIpAddress("PGM://192.168.10.1:1235>239.192.1.1:5555", InterfaceIpAddress_X, IpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_ResolveIpAddress("pgm://192.168.10.1:1235239.192.1.1:5555", InterfaceIpAddress_X, IpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_ResolveIpAddress("pgm:/192.168.10.1:1235>239.192.1.1:5555", InterfaceIpAddress_X, IpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

  Sts_E = Bof_ResolveIpAddress("pgm://192.168.a.1:1235>239.192.1.1:5555", InterfaceIpAddress_X, IpAddress_X);
  EXPECT_NE(Sts_E, BOF_ERR_NO_ERROR);

#if defined (BOF_UT_INTERNET_AVAILABLE)       // need internet
  Sts_E = Bof_UrlAddressToSocketAddressCollection("http://www.google.com:1", ListOfIpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  IpAddress_X = ListOfIpAddress_X[0];
  EXPECT_EQ(IpAddress_X.ProtocolType_E, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_HTTP);
  EXPECT_EQ(IpAddress_X.SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_TCP);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(IpAddress_X.IpV6Address_X.sin6_port, 0);	// Bof_IsCpuLittleEndian() ? 1 * 256 : 1);
  }
  else
  {
    EXPECT_EQ(IpAddress_X.IpV4Address_X.sin_port, Bof_IsCpuLittleEndian() ? 1 * 256 : 1);
  }
  EXPECT_FALSE(Bof_IsIpAddressNull(IpAddress_X));

  Sts_E = Bof_UrlAddressToSocketAddressCollection("http://www.google.com", ListOfIpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  IpAddress_X = ListOfIpAddress_X[0];
  EXPECT_EQ(IpAddress_X.ProtocolType_E, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_HTTP);
  EXPECT_EQ(IpAddress_X.SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_TCP);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(IpAddress_X.IpV6Address_X.sin6_port, 0);
  }
  else
  {
    EXPECT_EQ(IpAddress_X.IpV4Address_X.sin_port, 0);
  }
  EXPECT_FALSE(Bof_IsIpAddressNull(IpAddress_X));

  Sts_E = Bof_UrlAddressToSocketAddressCollection("http://www.yahoo.com:2124", ListOfIpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  IpAddress_X = ListOfIpAddress_X[0];
  EXPECT_EQ(IpAddress_X.ProtocolType_E, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_HTTP);
  EXPECT_EQ(IpAddress_X.SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_TCP);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(IpAddress_X.IpV6Address_X.sin6_port, 0);	// Bof_IsCpuLittleEndian() ? 0x4C08 : 2124);
  }
  else
  {
    EXPECT_EQ(IpAddress_X.IpV4Address_X.sin_port, Bof_IsCpuLittleEndian() ? 0x4C08 : 2124);
  }
  EXPECT_FALSE(Bof_IsIpAddressNull(IpAddress_X));

  Sts_E = Bof_UrlAddressToSocketAddressCollection("http://www.msn.com", ListOfIpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  IpAddress_X = ListOfIpAddress_X[0];
  EXPECT_EQ(IpAddress_X.ProtocolType_E, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_HTTP);
  EXPECT_EQ(IpAddress_X.SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_TCP);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(IpAddress_X.IpV6Address_X.sin6_port, 0);
  }
  else
  {
    EXPECT_EQ(IpAddress_X.IpV4Address_X.sin_port, 0);
  }
  EXPECT_FALSE(Bof_IsIpAddressNull(IpAddress_X));
#endif

}

TEST(SocketOs_Test, SocketAddress)
{
  TestSocketAddress(false);
  TestSocketAddress(true);
}

void TestListAndSetupInterface(bool _IsIpV6_B)
{
  BOFERR                                     Sts_E;
  std::vector< BOF_NETWORK_INTERFACE_PARAM > ListOfNetworkInterface_X;
  BOF_INTERFACE_INFO                         InterfaceInfo_X;
  BOF_NETWORK_INTERFACE_PARAM                InitialInterfaceParam_X, NewInterfaceParam_X;
  uint32_t                                   i_U32, FoundIndex_U32;
  int32_t                                    CidrMask_S32;
  BOF_SOCKET_ADDRESS												 IpAddress_X, CompatibleIpAddress_X;

  CidrMask_S32 = Bof_Compute_CidrMask("");
  EXPECT_EQ(CidrMask_S32, -1);

  CidrMask_S32 = Bof_Compute_CidrMask("0.0.0.");
  EXPECT_EQ(CidrMask_S32, -1);

  CidrMask_S32 = Bof_Compute_CidrMask("255.0.0.0");
  EXPECT_EQ(CidrMask_S32, 8);

  CidrMask_S32 = Bof_Compute_CidrMask("255.127.0.0");
  EXPECT_EQ(CidrMask_S32, 8);

  CidrMask_S32 = Bof_Compute_CidrMask("255.128.0.0");
  EXPECT_EQ(CidrMask_S32, 9);

  CidrMask_S32 = Bof_Compute_CidrMask("255.255.0.0");
  EXPECT_EQ(CidrMask_S32, 16);

  CidrMask_S32 = Bof_Compute_CidrMask("255.255.255.0");
  EXPECT_EQ(CidrMask_S32, 24);


  Sts_E = Bof_GetListOfNetworkInterface(ListOfNetworkInterface_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_FALSE(ListOfNetworkInterface_X.size() == 0);


  IpAddress_X.Set(_IsIpV6_B, BOF_SOCK_TYPE::BOF_SOCK_TCP, 127, 0, 0, 1, 5555);
  Sts_E = Bof_GetCompatibleIpAddress(ListOfNetworkInterface_X, IpAddress_X, CompatibleIpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  IpAddress_X.Set(_IsIpV6_B, BOF_SOCK_TYPE::BOF_SOCK_TCP, 192, 168, 0, 150, 5555);
  Sts_E = Bof_GetCompatibleIpAddress(ListOfNetworkInterface_X, IpAddress_X, CompatibleIpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  for (i_U32 = 0; i_U32 < ListOfNetworkInterface_X.size(); i_U32++)
  {
    Bof_IpAddressToSocketAddress(ListOfNetworkInterface_X[i_U32].IpAddress_S, IpAddress_X);
    Sts_E = Bof_GetCompatibleIpAddress(ListOfNetworkInterface_X, IpAddress_X, CompatibleIpAddress_X);
    EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
    //si le dhcp nous donne 168.1.2.3/16 et 168.1.20.30/16 	CompatibleIpAddress_X sera egal a 168.1.2.3/16 pour 168.1.20.30/16 	EXPECT_TRUE(Bof_IsIpAddressEqual(false, false, false, CompatibleIpAddress_X, IpAddress_X));

  }

  FoundIndex_U32 = 0xFFFFFFFF;
  for (i_U32 = 0; i_U32 < ListOfNetworkInterface_X.size(); i_U32++)
  {
    EXPECT_FALSE(ListOfNetworkInterface_X[i_U32].IpAddress_S == "");
    EXPECT_FALSE(ListOfNetworkInterface_X[i_U32].IpMask_S == "");
    EXPECT_FALSE(ListOfNetworkInterface_X[i_U32].IpV6_B);
    if (ListOfNetworkInterface_X[i_U32].Info_X.MtuSize_U32 != 0xFFFFFFFF)
    {
      EXPECT_EQ(ListOfNetworkInterface_X[i_U32].Info_X.MacAddress.size(), 6);
      EXPECT_FALSE(ListOfNetworkInterface_X[i_U32].Info_X.MtuSize_U32 == 0);
      EXPECT_NE(ListOfNetworkInterface_X[i_U32].Info_X.InterfaceFlag_E, BOF_NETWORK_INTERFACE_FLAG::BOF_IFF_NONE);
    }
    if (ListOfNetworkInterface_X[i_U32].IpAddress_S == "127.0.0.1")
    {
      FoundIndex_U32 = i_U32;
    }
  }
  if (FoundIndex_U32 == 0xFFFFFFFF)
  {
    FoundIndex_U32 = 0;
  }
  EXPECT_LT(FoundIndex_U32, ListOfNetworkInterface_X.size());
  InitialInterfaceParam_X = ListOfNetworkInterface_X[FoundIndex_U32];

  Sts_E = Bof_GetNetworkInterfaceInfo(ListOfNetworkInterface_X[FoundIndex_U32].Name_S, InterfaceInfo_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_NE(InterfaceInfo_X.InterfaceFlag_E, BOF_NETWORK_INTERFACE_FLAG::BOF_IFF_NONE);
  // EXPECT_EQ(InterfaceInfo_X.IpGateway_S, "");
  EXPECT_FALSE(InterfaceInfo_X.MtuSize_U32 == 0);
  EXPECT_TRUE(InterfaceInfo_X.MacAddress.size() == 6);

#if 0
  NewInterfaceParam_X = InitialInterfaceParam_X;
  // NewInterfaceParam_X.Name_S = "bha";
  NewInterfaceParam_X.IpMask_S = "255.255.0.0";
  Sts_E = Bof_SetNetworkInterfaceParam(InitialInterfaceParam_X.Name_S, NewInterfaceParam_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  NewInterfaceParam_X.IpMask_S = "0.0.0.0";
//Removed  Sts_E = Bof_GetNetworkInterfaceParam(InitialInterfaceParam_X.Name_S, NewInterfaceParam_X);
  // EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_TRUE(NewInterfaceParam_X.IpMask_S == InitialInterfaceParam_X.IpMask_S);

  NewInterfaceParam_X = InitialInterfaceParam_X;
  Sts_E = Bof_SetNetworkInterfaceParam(InitialInterfaceParam_X.Name_S, NewInterfaceParam_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
#endif
}

TEST(SocketOs_Test, ListAndSetupInterface)
{
  TestListAndSetupInterface(false);
  TestListAndSetupInterface(true);
}

void TestIpAddressBinSer(bool _IsIpV6_B)
{
  BOFERR  Sts_E;
  BOF_SOCKET_ADDRESS IpAddress_X;
  std::vector<uint16_t> BinFormat;

  Sts_E = Bof_SocketAddressToBin(IpAddress_X, BinFormat);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_EQ(BinFormat[0], 0);
  EXPECT_EQ(BinFormat[1], 0);
  EXPECT_EQ(BinFormat[2], 0);
  EXPECT_EQ(BinFormat[3], 0);

  IpAddress_X.Reset();
  Sts_E = Bof_BinToSocketAddress(BinFormat, IpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_TRUE(Bof_IsIpAddressNull(IpAddress_X));
  if (_IsIpV6_B)
  {
    Sts_E = Bof_IpAddressToSocketAddress("[1:2:3:4:5:6:7:8]:5", IpAddress_X);
  }
  else
  {
    Sts_E = Bof_IpAddressToSocketAddress("1.2.3.4:5", IpAddress_X);
  }
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  BinFormat.clear();
  Sts_E = Bof_SocketAddressToBin(IpAddress_X, BinFormat);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(BinFormat.size(), 8);
    for (int i = 0; i < 8; i++)
    {
      EXPECT_EQ(BinFormat[i], i + 1);
    }
  }
  else
  {
    EXPECT_EQ(BinFormat.size(), 4);
    EXPECT_EQ(BinFormat[0], 1);
    EXPECT_EQ(BinFormat[1], 2);
    EXPECT_EQ(BinFormat[2], 3);
    EXPECT_EQ(BinFormat[3], 4);
  }
  IpAddress_X.Reset();
  IpAddress_X.IpV6_B = _IsIpV6_B;
  Sts_E = Bof_BinToSocketAddress(BinFormat, IpAddress_X);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_FALSE(Bof_IsIpAddressNull(IpAddress_X));
  EXPECT_EQ(IpAddress_X.SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN);
  if (_IsIpV6_B)
  {
    EXPECT_EQ(IpAddress_X.IpV6Address_X.sin6_family, AF_INET6);
    EXPECT_EQ(IpAddress_X.IpV6Address_X.sin6_port, 0);
    uint16_t *pIp_U16 = reinterpret_cast<uint16_t *>(&IpAddress_X.IpV6Address_X.sin6_addr.s6_addr);

    for (int i = 0; i < 8; i++)
    {
      EXPECT_EQ(pIp_U16[i], (uint16_t)(i + 1));
    }
  }
  else
  {
    EXPECT_EQ(IpAddress_X.IpV4Address_X.sin_family, AF_INET);
    EXPECT_EQ(IpAddress_X.IpV4Address_X.sin_port, 0);
    EXPECT_EQ(IpAddress_X.IpV4Address_X.sin_addr.s_addr, htonl(0x01020304));
  }
}

TEST(SocketOs_Test, IpAddressBinSer)
{
  TestIpAddressBinSer(false);
  TestIpAddressBinSer(true);
}