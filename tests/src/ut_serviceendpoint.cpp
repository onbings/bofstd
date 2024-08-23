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
 * Name:        ut_serviceendpoint.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Aug 15 2024  BHA : Initial release
 */
#include "bofstd/bofserviceendpoint.h"

#include "gtestrunner.h"

USE_BOF_NAMESPACE()

TEST(ServiceEndPoint_Test, ServiceEndpoint)
{
  BOF::BofServiceEndpoint Endpoint;
  std::set<std::string> TagCollection;

  BOF_SOCKET_ADDRESS_COMPONENT Uri_X;
  std::string Path_S, Query_S, Fragment_S;
  BOFERR Sts_E;
  // john.doe:password@www.google.com:123/forum/questions/file.txt?justkey&order=newest;tag=networking#top
  /// myprotocol: / forum / questions / file.txt ? justkey & order = newest; tag = networking#top
  Sts_E = Bof_SplitUri("myprotocol://john.doe:password@1.2.3.4:123/forum/questions/file.txt?justkey&order=newest;tag=networking#top", Uri_X, Path_S, Query_S, Fragment_S);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);
  EXPECT_STREQ(Uri_X.Protocol_S.c_str(), "myprotocol");
  EXPECT_STREQ(Uri_X.User_S.c_str(), "john.doe");
  EXPECT_STREQ(Uri_X.Password_S.c_str(), "password");
  EXPECT_STREQ(Uri_X.IpAddress_S.c_str(), "1.2.3.4");
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

  
  EXPECT_FALSE(Endpoint.IsValid());
  EXPECT_STREQ(Endpoint.ToString(false).c_str(), "");
  EXPECT_STREQ(Endpoint.ToString(true).c_str(), "()@");
  EXPECT_STREQ(Endpoint.GetServiceName().c_str(), "");
  EXPECT_STREQ(Endpoint.GetServiceInstance().c_str(), "");
  EXPECT_STREQ(Endpoint.GetServiceUrl().c_str(), "://0.0.0.0:0");
  TagCollection = Endpoint.GetServiceTagCollection();
  EXPECT_EQ(TagCollection.size(), 0);
  
  Endpoint = BOF::BofServiceEndpoint("http://10.129.170.14");
  EXPECT_TRUE(Endpoint.IsValid());
  EXPECT_STREQ(Endpoint.ToString(false).c_str(), "http://10.129.170.14");
  EXPECT_STREQ(Endpoint.ToString(true).c_str(), "()@http://10.129.170.14");
  EXPECT_STREQ(Endpoint.GetServiceName().c_str(), "");
  EXPECT_STREQ(Endpoint.GetServiceInstance().c_str(), "");
  EXPECT_STREQ(Endpoint.GetServiceUrl().c_str(), "http://10.129.170.14:0");
  TagCollection = Endpoint.GetServiceTagCollection();
  EXPECT_EQ(TagCollection.size(), 0);

  Endpoint = BOF::BofServiceEndpoint("http://10.129.170.14:8500");
  EXPECT_TRUE(Endpoint.IsValid());
  EXPECT_STREQ(Endpoint.ToString(false).c_str(), "http://10.129.170.14:8500");
  EXPECT_STREQ(Endpoint.ToString(true).c_str(), "()@http://10.129.170.14:8500");
  EXPECT_STREQ(Endpoint.GetServiceName().c_str(), "");
  EXPECT_STREQ(Endpoint.GetServiceInstance().c_str(), "");
  EXPECT_STREQ(Endpoint.GetServiceUrl().c_str(), "http://10.129.170.14:8500");
  TagCollection = Endpoint.GetServiceTagCollection();
  EXPECT_EQ(TagCollection.size(), 0);

  TagCollection.insert("Tag1");
  TagCollection.insert("Tag2=Deux");
  TagCollection.insert("Tag3:Three");
  Endpoint = BOF::BofServiceEndpoint("ServiceName", "ServiceInstance","http://10.129.170.14:8500", TagCollection);
  EXPECT_TRUE(Endpoint.IsValid());
  EXPECT_STREQ(Endpoint.ToString(false).c_str(), "http://10.129.170.14:8500");
  EXPECT_STREQ(Endpoint.ToString(true).c_str(), "ServiceName(ServiceInstance)@http://10.129.170.14:8500");
  EXPECT_STREQ(Endpoint.GetServiceName().c_str(), "ServiceName");
  EXPECT_STREQ(Endpoint.GetServiceInstance().c_str(), "ServiceInstance");
  EXPECT_STREQ(Endpoint.GetServiceUrl().c_str(), "http://10.129.170.14:8500");
  TagCollection = Endpoint.GetServiceTagCollection();
  EXPECT_EQ(TagCollection.size(), 3);
  auto It = TagCollection.find("Tag1");
  EXPECT_TRUE(It != TagCollection.end());
  It = TagCollection.find("Tag2=Deux");
  EXPECT_TRUE(It != TagCollection.end());
  It = TagCollection.find("Tag3:Three");
  EXPECT_TRUE(It != TagCollection.end());
}

