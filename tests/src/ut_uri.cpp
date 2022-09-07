/*
 * Copyright (c) 2013-2033, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bofpath class
 *
 * Name:        ut_Fs.cpp
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

/*** Include files ***********************************************************/

#include "gtestrunner.h"
#include <bofstd/bofuri.h>

USE_BOF_NAMESPACE()
/*
Timelesschunk:  storage://10.129.4.172:11000/5/file/cg2/data1

storage ip: 10.129.4.172

api port: 11000

Partition: 5

Timelesschunk: file

Timelesschunk name: cg2

Timelesschunk sub: data1
*/

//https://www.urldecoder.org/
TEST(Uri_Test, UrlEscape)
{
  EXPECT_STREQ(BofUri::S_UrlEncode("").c_str(), "");
  EXPECT_STREQ(BofUri::S_UrlDecode("").c_str(), "");

  EXPECT_STREQ(BofUri::S_UrlEncode("Hello World !").c_str(), "Hello%20World%20%21");
  EXPECT_STREQ(BofUri::S_UrlDecode("Hello%20World%20%21").c_str(), "Hello World !");

  EXPECT_STREQ(BofUri::S_UrlEncode(R"( <>#%+{}|\^~[]`;/?:@=&)").c_str(), "%20%3C%3E%23%25%2B%7B%7D%7C%5C%5E~%5B%5D%60%3B%2F%3F%3A%40%3D%26");
  EXPECT_STREQ(BofUri::S_UrlDecode( "%20%3C%3E%23%25%2B%7B%7D%7C%5C%5E~%5B%5D%60%3B%2F%3F%3A%40%3D%26").c_str(), R"( <>#%+{}|\^~[]`;/?:@=&)");

  EXPECT_STREQ(BofUri::S_UrlDecode(R"(title%20EQ%20"%3CMy%20title%3E")").c_str(), "title EQ \"<My title>\"");
  EXPECT_STREQ(BofUri::S_UrlEncode("title EQ \"<My title>\"").c_str(), R"(title%20EQ%20%22%3CMy%20title%3E%22)");
}
TEST(Uri_Test, UriConstructorDestructor)
{
  BofUri Uri;
  BOF_SOCKET_ADDRESS_COMPONENT SchemeSocketAddressComponent_X;
  std::string Scheme_S, Path_S, QueryCollection_S, IpAddress_S, Fragment_S;
  BOF_SOCKET_ADDRESS IpAddress_X;
  BofPath Path;
  std::map<std::string, std::string> QueryCollection;

  EXPECT_FALSE(Uri.IsValid());
  Uri = BofUri("myprotocol://john.doe@www.google.com:123/forum/questions/file.txt?tag=networking&order=newest;justkey#top");
  EXPECT_TRUE(Uri.IsValid());
  //printf("Uri: '%s'\n", Uri.ToString().c_str());
  EXPECT_STREQ(Uri.ToString().c_str(),"myprotocol://john.doe@www.google.com:123/forum/questions/file.txt?justkey;order=newest;tag=networking#top");

  SchemeSocketAddressComponent_X = Uri.Scheme(Scheme_S);
  EXPECT_STREQ(Scheme_S.c_str(), "myprotocol://john.doe@www.google.com:123");
  EXPECT_STREQ(SchemeSocketAddressComponent_X.Protocol_S.c_str(), "myprotocol");
  EXPECT_STREQ(SchemeSocketAddressComponent_X.User_S.c_str(), "john.doe");
  EXPECT_STREQ(SchemeSocketAddressComponent_X.IpAddress_S.c_str(), "www.google.com");
  EXPECT_EQ(SchemeSocketAddressComponent_X.Port_U16, 123);

  IpAddress_X = Uri.IpAddress(IpAddress_S);
  EXPECT_STREQ(IpAddress_S.c_str(), "myprotocol://142.251.36.4:123");
  EXPECT_EQ(IpAddress_X.Port(), 123);
  EXPECT_FALSE(IpAddress_X.IpV6_B);
  EXPECT_EQ(IpAddress_X.SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN);
  EXPECT_EQ(IpAddress_X.ProtocolType_E, BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN);
  //EXPECT_EQ(IpAddress_X.IpV4Address_X;              IpV6_B;

  Path = Uri.Path(Path_S);
  EXPECT_STREQ(Path_S.c_str(), "/forum/questions/file.txt");
  EXPECT_STREQ(Path.DirectoryName(false).c_str(), "/forum/questions/");
  EXPECT_STREQ(Path.FileNameWithoutExtension().c_str(), "file");
  EXPECT_STREQ(Path.Extension().c_str(), "txt");

  QueryCollection = Uri.QueryCollection(QueryCollection_S);
  EXPECT_STREQ(QueryCollection_S.c_str(), "justkey;order=newest;tag=networking");
  EXPECT_EQ(QueryCollection.size(), 3);
  EXPECT_STREQ(QueryCollection["justkey"].c_str(), "");
  EXPECT_STREQ(QueryCollection["order"].c_str(), "newest");
  EXPECT_STREQ(QueryCollection["tag"].c_str(), "networking");

  Fragment_S = Uri.Fragment();
  EXPECT_STREQ(Fragment_S.c_str(), "top");

  Uri = BofUri("myprotocol://john.doe@www.google.com:123/forum/questions/file.txt?tag=networking&order=newest;justkey");
  EXPECT_TRUE(Uri.IsValid());
  //printf("Uri: '%s'\n", Uri.ToString().c_str());
  EXPECT_STREQ(Uri.ToString().c_str(), "myprotocol://john.doe@www.google.com:123/forum/questions/file.txt?justkey;order=newest;tag=networking");


  Uri = BofUri("myprotocol://john.doe@www.google.com:123/forum/questions/file.txt#top");
  EXPECT_TRUE(Uri.IsValid());
  //printf("Uri: '%s'\n", Uri.ToString().c_str());
  EXPECT_STREQ(Uri.ToString().c_str(), "myprotocol://john.doe@www.google.com:123/forum/questions/file.txt#top");

  Uri = BofUri("myprotocol://john.doe@www.google.com:123/forum/questions/file.txt");
  EXPECT_TRUE(Uri.IsValid());
  //printf("Uri: '%s'\n", Uri.ToString().c_str());
  EXPECT_STREQ(Uri.ToString().c_str(), "myprotocol://john.doe@www.google.com:123/forum/questions/file.txt");


}