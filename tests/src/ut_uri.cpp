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
  BOF_SOCKET_ADDRESS_COMPONENT SchemeAuthority_X;
  std::string SchemeAuthority_S, Path_S, QueryParamCollection_S, IpAddress_S, Fragment_S;
  BOF_SOCKET_ADDRESS IpAddress_X;
  BofPath Path;
  std::map<std::string, std::string> QueryParamCollection;

  BofUri Uri;
//  EXPECT_FALSE(Uri.IsValid());

  Uri = BofUri("myprotocol", "", "/forum/questions/file.txt", "tag=networking&order=newest;justkey", "top");
  EXPECT_TRUE(Uri.IsValid());
  EXPECT_STREQ(Uri.ToString().c_str(), "myprotocol:/forum/questions/file.txt?justkey&order=newest&tag=networking#top");

  Uri = BofUri("myprotocol", "john.doe:password@www.google.com:123", "/forum/questions/file.txt", "tag=networking&order=newest;justkey", "top");
  EXPECT_TRUE(Uri.IsValid());
  EXPECT_STREQ(Uri.ToString().c_str(), "myprotocol://john.doe:password@www.google.com:123/forum/questions/file.txt?justkey&order=newest&tag=networking#top");
  SchemeAuthority_X = Uri.SchemeAuthority(SchemeAuthority_S);
  EXPECT_STREQ(SchemeAuthority_X.User_S.c_str(), "john.doe");
  EXPECT_STREQ(SchemeAuthority_X.Password_S.c_str(), "password");

  Uri = BofUri("myprotocol","john.doe@www.google.com:123", "/forum/questions/file.txt", "tag=networking&order=newest;justkey", "top");
  EXPECT_TRUE(Uri.IsValid());
  EXPECT_STREQ(Uri.ToString().c_str(), "myprotocol://john.doe@www.google.com:123/forum/questions/file.txt?justkey&order=newest&tag=networking#top");

  SchemeAuthority_X = Uri.SchemeAuthority(SchemeAuthority_S);
  EXPECT_STREQ(SchemeAuthority_S.c_str(), "myprotocol://john.doe@www.google.com:123");
  EXPECT_STREQ(SchemeAuthority_X.Protocol_S.c_str(), "myprotocol");
  EXPECT_STREQ(SchemeAuthority_X.User_S.c_str(), "john.doe");
  EXPECT_STREQ(SchemeAuthority_X.IpAddress_S.c_str(), "www.google.com");
  EXPECT_EQ(SchemeAuthority_X.Port_U16, 123);

  Uri = BofUri("myprotocol://john.doe@www.google.com:123/forum/questions/file.txt?tag=networking&order=newest;justkey#top");
  EXPECT_TRUE(Uri.IsValid());
  //printf("Uri: '%s'\n", Uri.ToString().c_str());
  EXPECT_STREQ(Uri.ToString().c_str(),"myprotocol://john.doe@www.google.com:123/forum/questions/file.txt?justkey&order=newest&tag=networking#top");

  SchemeAuthority_X = Uri.SchemeAuthority(SchemeAuthority_S);
  EXPECT_STREQ(SchemeAuthority_S.c_str(), "myprotocol://john.doe@www.google.com:123");
  EXPECT_STREQ(SchemeAuthority_X.Protocol_S.c_str(), "myprotocol");
  EXPECT_STREQ(SchemeAuthority_X.User_S.c_str(), "john.doe");
  EXPECT_STREQ(SchemeAuthority_X.IpAddress_S.c_str(), "www.google.com");
  EXPECT_EQ(SchemeAuthority_X.Port_U16, 123);

  EXPECT_STREQ(Uri.Scheme().c_str(), "myprotocol");
  EXPECT_STREQ(Uri.Authority().c_str(), "john.doe@www.google.com:123");

  IpAddress_X = Uri.IpAddress(IpAddress_S);
  //Can chage (google.com) EXPECT_STREQ(IpAddress_S.c_str(), "myprotocol://142.251.36.4:123");
  EXPECT_EQ(IpAddress_X.Port(), 123);
  EXPECT_FALSE(IpAddress_X.IpV6_B);
  EXPECT_EQ(IpAddress_X.SocketType_E, BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN);
  //EXPECT_EQ(IpAddress_X.IpV4Address_X;              IpV6_B;

  Path = Uri.Path(Path_S);
  EXPECT_STREQ(Path_S.c_str(), "/forum/questions/file.txt");
  EXPECT_STREQ(Path.DirectoryName(false).c_str(), "/forum/questions/");
  EXPECT_STREQ(Path.FileNameWithoutExtension().c_str(), "file");
  EXPECT_STREQ(Path.Extension().c_str(), "txt");

  QueryParamCollection = Uri.QueryParamCollection(QueryParamCollection_S);
  EXPECT_STREQ(QueryParamCollection_S.c_str(), "justkey&order=newest&tag=networking");
  EXPECT_EQ(QueryParamCollection.size(), 3);
  EXPECT_STREQ(QueryParamCollection["justkey"].c_str(), "");
  EXPECT_STREQ(QueryParamCollection["order"].c_str(), "newest");
  EXPECT_STREQ(QueryParamCollection["tag"].c_str(), "networking");

  Fragment_S = Uri.Fragment();
  EXPECT_STREQ(Fragment_S.c_str(), "top");

  Uri = BofUri("myprotocol://john.doe@www.google.com:123/forum/questions/file.txt?tag=networking&order=newest;justkey");
  EXPECT_TRUE(Uri.IsValid());
  EXPECT_STREQ(Uri.ToString().c_str(), "myprotocol://john.doe@www.google.com:123/forum/questions/file.txt?justkey&order=newest&tag=networking");

  Uri = BofUri("myprotocol://john.doe@www.google.com:123/forum/questions/file.txt#top");
  EXPECT_TRUE(Uri.IsValid());
  EXPECT_STREQ(Uri.ToString().c_str(), "myprotocol://john.doe@www.google.com:123/forum/questions/file.txt#top");

  Uri = BofUri("myprotocol://john.doe@www.google.com:123/forum/questions/file.txt");
  EXPECT_TRUE(Uri.IsValid());
  EXPECT_STREQ(Uri.ToString().c_str(), "myprotocol://john.doe@www.google.com:123/forum/questions/file.txt");
}

TEST(Uri_Test, Set)
{
  BOF_SOCKET_ADDRESS_COMPONENT SchemeAuthority_X;
  std::string SchemeAuthority_S, Path_S, QueryParamCollection_S, IpAddress_S, Fragment_S;
  BOF_SOCKET_ADDRESS IpAddress_X;
  BofPath Path;
  std::map<std::string, std::string> QueryParamCollection;

  BofUri Uri;
  EXPECT_FALSE(Uri.IsValid());
  EXPECT_NE(Uri.SetQueryParamDelimiter(','), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Uri.SetQueryParamDelimiter('&'), BOF_ERR_NO_ERROR);
  EXPECT_EQ(Uri.SetQueryParamDelimiter(';'), BOF_ERR_NO_ERROR);
  EXPECT_TRUE(Uri.QueryParamDelimiter() == ';');

  EXPECT_EQ(Uri.SetScheme("myprotocol"), BOF_ERR_NO_ERROR);
  EXPECT_TRUE(Uri.IsValid());

  EXPECT_EQ(Uri.SetAuthority("john.doe@www.google.com:123"), BOF_ERR_NO_ERROR);
  EXPECT_TRUE(Uri.IsValid());

  EXPECT_EQ(Uri.SetPath(std::string("/forum/questions/file.txt")), BOF_ERR_NO_ERROR);
  EXPECT_TRUE(Uri.IsValid());

  EXPECT_EQ(Uri.SetQueryParamCollection("justkey;order=newest;tag=networking"), BOF_ERR_NO_ERROR);
  EXPECT_TRUE(Uri.IsValid());

  EXPECT_EQ(Uri.SetFragment("top"), BOF_ERR_NO_ERROR);
  EXPECT_TRUE(Uri.IsValid());

  EXPECT_STREQ(Uri.ToString().c_str(), "myprotocol://john.doe@www.google.com:123/forum/questions/file.txt?justkey;order=newest;tag=networking#top");
}

TEST(Uri_Test, QueryParamCollection)
{
  //BOF_SOCKET_ADDRESS_COMPONENT SchemeAuthority_X;
  std::string  QueryParamCollection_S;
  //BOF_SOCKET_ADDRESS IpAddress_X;
  //BofPath Path;
  std::map<std::string, std::string> QueryParamCollection;
  BofUri Uri;

  Uri = BofUri("myprotocol", "john.doe@www.google.com:123", "/forum/questions/file.txt", "tag=networking&order=newest;justkey", "top");
  EXPECT_TRUE(Uri.IsValid());
  EXPECT_EQ(Uri.SetQueryParamDelimiter('&'), BOF_ERR_NO_ERROR);

  EXPECT_EQ(Uri.RemoveFromQueryParamCollection("order"), BOF_ERR_NO_ERROR);
  QueryParamCollection = Uri.QueryParamCollection(QueryParamCollection_S);
  EXPECT_STREQ(QueryParamCollection_S.c_str(), "justkey&tag=networking");
  EXPECT_EQ(QueryParamCollection.size(), 2);
  EXPECT_STREQ(QueryParamCollection["justkey"].c_str(), "");
  EXPECT_STREQ(QueryParamCollection["tag"].c_str(), "networking");

  EXPECT_EQ(Uri.RemoveFromQueryParamCollection("tag"), BOF_ERR_NO_ERROR);
  QueryParamCollection = Uri.QueryParamCollection(QueryParamCollection_S);
  EXPECT_STREQ(QueryParamCollection_S.c_str(), "justkey");
  EXPECT_EQ(QueryParamCollection.size(), 1);
  EXPECT_STREQ(QueryParamCollection["justkey"].c_str(), "");

  EXPECT_NE(Uri.RemoveFromQueryParamCollection("tag"), BOF_ERR_NO_ERROR);

  EXPECT_EQ(Uri.RemoveFromQueryParamCollection("justkey"), BOF_ERR_NO_ERROR);
  QueryParamCollection = Uri.QueryParamCollection(QueryParamCollection_S);
  EXPECT_STREQ(QueryParamCollection_S.c_str(), "");
  EXPECT_EQ(QueryParamCollection.size(), 0);

  EXPECT_EQ(Uri.AddToQueryParamCollection("a", "1"), BOF_ERR_NO_ERROR);
  QueryParamCollection = Uri.QueryParamCollection(QueryParamCollection_S);
  EXPECT_STREQ(QueryParamCollection_S.c_str(), "a=1");
  EXPECT_EQ(QueryParamCollection.size(), 1);

  EXPECT_NE(Uri.AddToQueryParamCollection("a", "2"), BOF_ERR_NO_ERROR);
  QueryParamCollection = Uri.QueryParamCollection(QueryParamCollection_S);
  EXPECT_STREQ(QueryParamCollection_S.c_str(), "a=1");
  EXPECT_EQ(QueryParamCollection.size(), 1);

  EXPECT_EQ(Uri.AddToQueryParamCollection("b", ""), BOF_ERR_NO_ERROR);
  QueryParamCollection = Uri.QueryParamCollection(QueryParamCollection_S);
  EXPECT_STREQ(QueryParamCollection_S.c_str(), "a=1&b");
  EXPECT_EQ(QueryParamCollection.size(), 2);

  EXPECT_EQ(Uri.AddToQueryParamCollection("c", "3"), BOF_ERR_NO_ERROR);
  QueryParamCollection = Uri.QueryParamCollection(QueryParamCollection_S);
  EXPECT_STREQ(QueryParamCollection_S.c_str(), "a=1&b&c=3");
  EXPECT_EQ(QueryParamCollection.size(), 3);

}