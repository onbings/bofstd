/*
 * Copyright (c) 2015-2025, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit tests of the xmlparser library
 *
 * Name:        ut_xmlparser.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */
#include <bofstd/bofdatetime.h>
#include <bofstd/boffs.h>
#include <bofstd/bofsocket.h>
#include <bofstd/bofxmlparser.h>
#include <bofstd/bofxmlwriter.h>

#include "gtestrunner.h"

USE_BOF_NAMESPACE()

#include "ut_parser.h"

BOFERR XmlParseResultUltimateCheck(uint32_t /*_Index_U32*/, const BOFPARAMETER & /*_rBofCommandlineOption_X*/, const bool _CheckOk_B, const char * /*_pOptNewVal_c*/)
{
  BOFERR Rts_E = _CheckOk_B ? BOF_ERR_NO_ERROR : BOF_ERR_NO;
  return Rts_E;
}

bool XmlParseError(int /*_Sts_i*/, const BOFPARAMETER & /*_rXmlEntry_X*/, const char * /*_pValue*/)
{
  bool Rts_B = true;
  return Rts_B;
}

BOFERR XmlWriteResultUltimateCheck(uint32_t /*_Index_U32*/, const BOFPARAMETER & /*_rBofCommandlineOption_X*/, const bool _CheckOk_B, const char * /*_pOptNewVal_c*/)
{
  BOFERR Rts_E = _CheckOk_B ? BOF_ERR_NO_ERROR : BOF_ERR_NO;
  return Rts_E;
}

bool XmlWriteError(int /*_Sts_i*/, const BOFPARAMETER & /*_rXmlEntry_X*/, const char * /*_pValue*/)
{
  bool Rts_B = true;
  return Rts_B;
}
static PARSER_APPPARAM S_AppParamXml_X;

static std::vector<BOFPARAMETER> S_pOptionXml_X = {
    {nullptr, "DeployIpAddress", "DeployIpAddress", "", "MulFtpUserSetting", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AppParamXml_X.DeployIpAddress_X, IPV4, 0, 0)},
    {nullptr, "DeployIpPort", "DeployIpPort", "", "MulFtpUserSetting", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AppParamXml_X.DeployIpPort_U16, UINT16, 0, 0)},
    {nullptr, "DeployProtocol", "DeployProtocol", "", "MulFtpUserSetting", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AppParamXml_X.pDeployProtocol_c, CHARSTRING, 1, sizeof(S_AppParamXml_X.pDeployProtocol_c) - 1)},
    {nullptr, "DeployDirectory", "DeployDirectory", "", "MulFtpUserSetting", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AppParamXml_X.pDeployDirectory_c, CHARSTRING, 1, sizeof(S_AppParamXml_X.pDeployDirectory_c) - 1)},
    {nullptr, "LoginUser", "LoginUser", "", "MulFtpUserSetting", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AppParamXml_X.pLoginUser_c, CHARSTRING, 1, sizeof(S_AppParamXml_X.pLoginUser_c) - 1)},
    {nullptr, "LoginPassword", "LoginPassword", "", "MulFtpUserSetting", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AppParamXml_X.pLoginPassword_c, CHARSTRING, 1, sizeof(S_AppParamXml_X.pLoginPassword_c) - 1)},
    {nullptr, "Email", "Email", "", "MulFtpUserSetting", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AppParamXml_X.pEmail_c, CHARSTRING, 1, sizeof(S_AppParamXml_X.pEmail_c) - 1)},
    {nullptr, "UserName", "UserName", "", "MulFtpUserSetting", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AppParamXml_X.pUserName_c, CHARSTRING, 1, sizeof(S_AppParamXml_X.pUserName_c) - 1)},
    {nullptr, "UserCompany", "UserCompany", "", "MulFtpUserSetting", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AppParamXml_X.pUserCompany_c, CHARSTRING, 1, sizeof(S_AppParamXml_X.pUserCompany_c) - 1)},
    {nullptr, "ToolChainBaseDirectory", "ToolChainBaseDirectory", "", "MulFtpUserSetting", BOFPARAMETER_ARG_FLAG::NONE,
     BOF_PARAM_DEF_VARIABLE(S_AppParamXml_X.pToolChainBaseDirectory_c, CHARSTRING, 1, sizeof(S_AppParamXml_X.pToolChainBaseDirectory_c) - 1)},
    {nullptr, "TemplateProjectBaseDirectory", "TemplateProjectBaseDirectory", "", "MulFtpUserSetting", BOFPARAMETER_ARG_FLAG::NONE,
     BOF_PARAM_DEF_VARIABLE(S_AppParamXml_X.pTemplateProjectBaseDirectory_c, CHARSTRING, 1, sizeof(S_AppParamXml_X.pTemplateProjectBaseDirectory_c) - 1)},

    // For xml serialize xml array entry must be contiguous MulFtpUserSetting.catalog.book
    {nullptr, "id", "id", "", "MulFtpUserSetting.catalog.book", BOFPARAMETER_ARG_FLAG::XML_ATTRIBUTE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM, S_AppParamXml_X.pBook_X, pId_c, CHARSTRING, 1, sizeof(S_AppParamXml_X.pBook_X[0].pId_c) - 1)},
    {nullptr, "author", "author", "", "MulFtpUserSetting.catalog.book", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM, S_AppParamXml_X.pBook_X, pAuthor_c, CHARSTRING, 1, sizeof(S_AppParamXml_X.pBook_X[0].pAuthor_c) - 1)},

    {nullptr, "title", "title", "", "MulFtpUserSetting.catalog.book", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM, S_AppParamXml_X.pBook_X, pTitle_c, CHARSTRING, 1, sizeof(S_AppParamXml_X.pBook_X[0].pTitle_c) - 1)},
    {nullptr, "genre", "genre", "", "MulFtpUserSetting.catalog.book", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM, S_AppParamXml_X.pBook_X, pGenre_c, CHARSTRING, 1, sizeof(S_AppParamXml_X.pBook_X[0].pGenre_c) - 1)},
    {nullptr, "price", "price", "", "MulFtpUserSetting.catalog.book", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM, S_AppParamXml_X.pBook_X, Price_f, FLOAT, 0, 0)},

    // BAD for json  { nullptr, "tag", "dbg1", "", "MulFtpUserSetting.catalog.book.bhaattr", BOFPARAMETER_ARG_FLAG::XML_ATTRIBUTE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM, S_AppParamXml_X.pBook_X, pBha1_c, CHARSTRING, 0, 0) },

    {nullptr, "bha", "dbg2", "", "MulFtpUserSetting.catalog.book", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM, S_AppParamXml_X.pBook_X, pBha2_c, CHARSTRING, 0, 0)},
    {nullptr, "publish_date", "publish_date", "%Y-%m-%d", "MulFtpUserSetting.catalog.book", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM, S_AppParamXml_X.pBook_X, PublishDate, DATE, 0, 0)},
    {nullptr, "description", "description", "", "MulFtpUserSetting.catalog.book", BOFPARAMETER_ARG_FLAG::NONE,
     BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM, S_AppParamXml_X.pBook_X, pDescription_c, CHARSTRING, 1, sizeof(S_AppParamXml_X.pBook_X[0].pDescription_c) - 1)},

    {nullptr, "a", "other a", "", "MulFtpUserSetting.arrayofother.other", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(OTHER, S_AppParamXml_X.pOther_X, a_U32, UINT32, 0, 0)},
    {nullptr, "b", "other b", "", "MulFtpUserSetting.arrayofother.other", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(OTHER, S_AppParamXml_X.pOther_X, b_U32, UINT32, 0, 0)},
    {nullptr, "c", "other c", "", "MulFtpUserSetting.arrayofother.other", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(OTHER, S_AppParamXml_X.pOther_X, c_U32, UINT32, 0, 0)},

};
TEST(XmlParser_Test, Xml)
{
  int Sts_i;
  BofXmlParser *pBofXmlParser_O;
  const char *pValue_c;
  BofPath CrtDir, Path;
  std::string XmlData_S;

  S_AppParamXml_X.Reset();
  Bof_GetCurrentDirectory(CrtDir);
#if defined(_WIN32)    
  Path = CrtDir + "../binaries/bin/data/xmlparser.xml";
#else
  Path = CrtDir + "./data/xmlparser.xml";	
#endif	
  printf("-PATH-XML->%s\n", Path.ToString(false).c_str());
  EXPECT_EQ(Bof_ReadFile(Path, XmlData_S), BOF_ERR_NO_ERROR);

  pBofXmlParser_O = new BofXmlParser(XmlData_S);
  EXPECT_TRUE(pBofXmlParser_O != nullptr);
  Sts_i = pBofXmlParser_O->ToByte(S_pOptionXml_X, XmlParseResultUltimateCheck, XmlParseError);
  EXPECT_EQ(Sts_i, 0);

  // Check S_AppParam_X content
  pValue_c = pBofXmlParser_O->GetFirstElementFromOid(false, "MulFtpUserSetting.DeployIpAddress");
  EXPECT_TRUE(pValue_c != nullptr);
  pValue_c = pBofXmlParser_O->GetNextElementFromOid();
  EXPECT_TRUE(pValue_c == nullptr);

  pValue_c = pBofXmlParser_O->GetFirstElementFromOid(true, "MulFtpUserSetting.catalog.book.id");
  EXPECT_TRUE(pValue_c != nullptr);
  pValue_c = pBofXmlParser_O->GetNextElementFromOid();
  EXPECT_TRUE(pValue_c != nullptr);

  pValue_c = pBofXmlParser_O->GetFirstElementFromOid(true, "MulFtpUserSetting.catalog.book.bhaattr.tag");
  EXPECT_TRUE(pValue_c != nullptr);
  pValue_c = pBofXmlParser_O->GetNextElementFromOid();
  EXPECT_TRUE(pValue_c != nullptr);
  pValue_c = pBofXmlParser_O->GetNextElementFromOid();
  EXPECT_TRUE(pValue_c != nullptr);
  pValue_c = pBofXmlParser_O->GetNextElementFromOid();
  EXPECT_TRUE(pValue_c == nullptr);

  BOF_SAFE_DELETE(pBofXmlParser_O);
}

static APPPARAMVECTOR S_AppParamVector_X;
static std::vector<BOFPARAMETER> S_pOptionVector_X = {
    {nullptr, "id", "id", "", "XmlVector.catalog.book", BOFPARAMETER_ARG_FLAG::XML_ATTRIBUTE, BOF_PARAM_DEF_VECTOR(S_AppParamVector_X.IdCollection, STDSTRING, 1, 0x1000)},
    {nullptr, "author", "Pure vector of std::string.", "", "XmlVector.catalog.book", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_AppParamVector_X.StringCollection, STDSTRING, 1, 0x1000)},
    {nullptr, "price", "Pure vector of float.", "", "XmlVector.catalog.book", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_AppParamVector_X.FloatCollection, FLOAT, 0, 0)},
    {nullptr, "publish_date", "Pure vector of struct tm.", "%Y-%m-%d", "XmlVector.catalog.book", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_AppParamVector_X.DateTimeCollection, DATE, 0, 0)},
};
TEST(XmlParser_Test, XmlVector)
{
  int Sts_i;
  BofXmlParser *pBofXmlParser_O;
  BofPath CrtDir, Path;
  std::string XmlData_S;

  S_AppParamVector_X.Reset();
  // Should be /home/bha/bld/Tge2-Debug/bofstd/tests/data/xmlvectorparser.xml
  Bof_GetCurrentDirectory(CrtDir);
#if defined(_WIN32)    
  Path = CrtDir + "../binaries/bin/data/xmlvectorparser.xml";
#else
  Path = CrtDir + "./data/xmlvectorparser.xml";
#endif	
  EXPECT_EQ(Bof_ReadFile(Path, XmlData_S), BOF_ERR_NO_ERROR);

  pBofXmlParser_O = new BofXmlParser(XmlData_S);
  EXPECT_TRUE(pBofXmlParser_O != nullptr);

  std::string XmlIn_S = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<--Comment-->\r\n<MulFtpUserSetting\r\n\t   xmlns : xsi = \"http://www.w3.org/2001/XMLSchema-instance\" xmlns : xsd = \"http://www.w3.org/2001/XMLSchema\">";
  EXPECT_STREQ("MulFtpUserSetting", BofXmlParser::S_RootName(XmlIn_S).c_str());

  EXPECT_TRUE(pBofXmlParser_O->IsValid());
  EXPECT_STREQ(pBofXmlParser_O->RootName().c_str(), "XmlVector");
  EXPECT_STREQ(pBofXmlParser_O->RootName().c_str(), BofXmlParser::S_RootName(XmlData_S).c_str());

  Sts_i = pBofXmlParser_O->ToByte(S_pOptionVector_X, XmlParseResultUltimateCheck, XmlParseError);
  EXPECT_EQ(Sts_i, 0);

  BOF_SAFE_DELETE(pBofXmlParser_O);
}
TEST(XmlWriter_Test, XmlVector)
{
  int Sts_i;
  BofXmlParser *pBofXmlParser_O;
  BofPath CrtDir, Path;
  std::string XmlData_S, XmlOut_S;
  BofXmlWriter BofXmlWriter_O;

  S_AppParamVector_X.Reset();
  Bof_GetCurrentDirectory(CrtDir);
#if defined(_WIN32)   
  Path = CrtDir + "../binaries/bin/data/xmlvectorparser.xml";
#else
  Path = CrtDir + "./data/xmlvectorparser.xml";
#endif	
  // printf("Crtdir %s path %s\n", CrtDir.FullPathName(false).c_str(), Path.FullPathName(false).c_str());
  ASSERT_EQ(Bof_ReadFile(Path, XmlData_S), BOF_ERR_NO_ERROR);

  pBofXmlParser_O = new BofXmlParser(XmlData_S);
  EXPECT_TRUE(pBofXmlParser_O != nullptr);

  XmlOut_S = "";
  //	Sts_i = BofXmlWriter_O.FromByte("<?xml version='1.0' encoding=\"utf-8\"?>\r\n<!--This is a comment-->\n", S_pOptionVector_X, XmlOut_S);
  // EXPECT_EQ(Sts_i, 0);

  Sts_i = pBofXmlParser_O->ToByte(S_pOptionVector_X, XmlWriteResultUltimateCheck, XmlWriteError);
  EXPECT_EQ(Sts_i, 0);

  XmlOut_S = "";
  Sts_i = BofXmlWriter_O.FromByte("<?xml version='1.0' encoding=\"utf-8\"?>\r\n<!--This is a comment-->\r\n", S_pOptionVector_X, XmlOut_S);
  EXPECT_EQ(Sts_i, 0);

  std::string Res_S = "<?xml version='1.0' encoding=\"utf-8\"?>\r\n"
                      "<!--This is a comment-->\r\n"
                      "<XmlVector xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n"
                      "<catalog>\r\n"
                      "<book id=\"bk101\">\r\n"
                      "<author>Gambardella, Matthew</author>\r\n"
                      "<price>44.950001</price>\r\n"
                      "<publish_date>2016-05-26</publish_date>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk102\">\r\n"
                      "<author>Ralls, Kim</author>\r\n"
                      "<price>5.950000</price>\r\n"
                      "<publish_date>2000-12-16</publish_date>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk103\">\r\n"
                      "<author>Corets, Eva</author>\r\n"
                      "<price>5.950000</price>\r\n"
                      "<publish_date>2000-11-17</publish_date>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk104\">\r\n"
                      "<author>Corets, Eva</author>\r\n"
                      "<price>5.950000</price>\r\n"
                      "<publish_date>2001-03-10</publish_date>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk105\">\r\n"
                      "<author>Corets, Eva</author>\r\n"
                      "<price>5.950000</price>\r\n"
                      "<publish_date>2001-09-10</publish_date>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk106\">\r\n"
                      "<author>Randall, Cynthia</author>\r\n"
                      "<price>4.950000</price>\r\n"
                      "<publish_date>2000-09-02</publish_date>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk107\">\r\n"
                      "<author>Thurman, Paula</author>\r\n"
                      "<price>4.950000</price>\r\n"
                      "<publish_date>2000-11-02</publish_date>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk108\">\r\n"
                      "<author>Knorr, Stefan</author>\r\n"
                      "<price>4.950000</price>\r\n"
                      "<publish_date>2000-12-06</publish_date>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk109\">\r\n"
                      "<author>Kress, Peter</author>\r\n"
                      "<price>6.950000</price>\r\n"
                      "<publish_date>2000-11-02</publish_date>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk110\">\r\n"
                      "<author>O&apos;Brien, Tim</author>\r\n"
                      "<price>36.950001</price>\r\n"
                      "<publish_date>2000-12-09</publish_date>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk111\">\r\n"
                      "<author>O&apos;Brien, Tim</author>\r\n"
                      "<price>36.950001</price>\r\n"
                      "<publish_date>2000-12-01</publish_date>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk112\">\r\n"
                      "<author>Galos, Mike</author>\r\n"
                      "<price>49.950001</price>\r\n"
                      "<publish_date>2001-04-16</publish_date>\r\n"
                      "</book>\r\n"
                      "<book></book>\r\n"
                      "</catalog>\r\n"
                      "</XmlVector>\r\n";
  /*
  const char *p = XmlOut_S.c_str();
  const char *q = Res_S.c_str();
  //while (*p == *q) { printf("%c", *p); p++; q++; }
  while (*p == *q) { p++; q++; }
  */
  EXPECT_STREQ(XmlOut_S.c_str(), Res_S.c_str());
  BOF_SAFE_DELETE(pBofXmlParser_O);
}
TEST(XmlWriter_Test, Xml)
{
  int Sts_i;
  BofXmlWriter BofXmlWriter_O;
  BofXmlParser *pBofXmlParser_O;
  BofPath CrtDir, Path;
  std::string XmlData_S, XmlOut_S;

  S_AppParamXml_X.Reset();
  // Should be /home/bha/bld/Tge2-Debug/bofstd/tests/data/xmlparser.xml
  Bof_GetCurrentDirectory(CrtDir);
#if defined(_WIN32)    
  Path = CrtDir + "../binaries/bin/data/xmlparser.xml";
#else
  Path = CrtDir + "./data/xmlparser.xml";
#endif	
  printf("-->%s\n", Path.ToString(false).c_str());
  EXPECT_EQ(Bof_ReadFile(Path, XmlData_S), BOF_ERR_NO_ERROR);

  pBofXmlParser_O = new BofXmlParser(XmlData_S);
  EXPECT_TRUE(pBofXmlParser_O != nullptr);
  Sts_i = pBofXmlParser_O->ToByte(S_pOptionXml_X, XmlWriteResultUltimateCheck, XmlWriteError);
  EXPECT_EQ(Sts_i, 0);

  Sts_i = BofXmlWriter_O.FromByte("<?xml version='1.0' encoding=\"utf-8\"?>\r\n<!--This is a comment-->\r\n", S_pOptionXml_X, XmlOut_S);
  EXPECT_EQ(Sts_i, 0);

  std::string Res_S = "<?xml version='1.0' encoding=\"utf-8\"?>\r\n"
                      "<!--This is a comment-->\r\n"
                      "<MulFtpUserSetting xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n"
                      "<DeployIpAddress>192.168.1.21</DeployIpAddress>\r\n"
                      "<DeployIpPort>2000</DeployIpPort>\r\n"
                      "<DeployProtocol>ssh</DeployProtocol>\r\n"
                      "<DeployDirectory>/tmp/</DeployDirectory>\r\n"
                      "<LoginUser>root</LoginUser>\r\n"
                      "<LoginPassword>a</LoginPassword>\r\n"
                      "<Email>b.harmel@gmail.com</Email>\r\n"
                      "<UserName>b.harmel</UserName>\r\n"
                      "<UserCompany>OnBings</UserCompany>\r\n"
                      "<ToolChainBaseDirectory>D:\\SysGCC\\</ToolChainBaseDirectory>\r\n"
                      "<TemplateProjectBaseDirectory>D:\\cloudstation\\pro\\vsmake-project-template\\</TemplateProjectBaseDirectory>\r\n"
                      "<catalog>\r\n"
                      "<book id=\"bk101\">\r\n"
                      "<author>Gambardella, Matthew</author>\r\n"
                      "<title>XML Developer&apos;s Guide</title>\r\n"
                      "<genre>Computer</genre>\r\n"
                      "<price>44.950001</price>\r\n"
                      "<bha>AZ</bha>\r\n"
                      "<publish_date>2016-05-26</publish_date>\r\n"
                      "<description>\n\t\t\t\tAn in-depth look at creating applications\n\t\t\t\twith XML.\n\t\t\t</description>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk102\">\r\n"
                      "<author>Ralls, Kim</author>\r\n"
                      "<title>Midnight Rain</title>\r\n"
                      "<genre>Fantasy</genre>\r\n"
                      "<price>5.950000</price>\r\n"
                      "<bha>QS</bha>\r\n"
                      "<publish_date>2000-12-16</publish_date>\r\n"
                      "<description>\n\t\t\t\tA former architect battles corporate zombies,\n\t\t\t\tan evil sorceress, and her own childhood to become queen\n\t\t\t\tof the world.\n\t\t\t</description>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk103\">\r\n"
                      "<author>Corets, Eva</author>\r\n"
                      "<title>Maeve Ascendant</title>\r\n"
                      "<genre>Fantasy</genre>\r\n"
                      "<price>5.950000</price>\r\n"
                      "<bha>WX</bha>\r\n"
                      "<publish_date>2000-11-17</publish_date>\r\n"
                      "<description>\n\t\t\t\tAfter the collapse of a nanotechnology\n\t\t\t\tsociety in England, the young survivors lay the\n\t\t\t\tfoundation for a new society.\n\t\t\t</description>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk104\">\r\n"
                      "<author>Corets, Eva</author>\r\n"
                      "<title>Oberon&apos;s Legacy</title>\r\n"
                      "<genre>Fantasy</genre>\r\n"
                      "<price>5.950000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date>2001-03-10</publish_date>\r\n"
                      "<description>\n\t\t\t\tIn post-apocalypse England, the mysterious\n\t\t\t\tagent known only as Oberon helps to create a new life\n\t\t\t\tfor the inhabitants of London. Sequel to Maeve\n\t\t\t\tAscendant.\n\t\t\t</description>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk105\">\r\n"
                      "<author>Corets, Eva</author>\r\n"
                      "<title>The Sundered Grail</title>\r\n"
                      "<genre>Fantasy</genre>\r\n"
                      "<price>5.950000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date>2001-09-10</publish_date>\r\n"
                      "<description>\n\t\t\t\tThe two daughters of Maeve, half-sisters,\n\t\t\t\tbattle one another for control of England. Sequel to\n\t\t\t\tOberon&apos;s Legacy.\n\t\t\t</description>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk106\">\r\n"
                      "<author>Randall, Cynthia</author>\r\n"
                      "<title>Lover Birds</title>\r\n"
                      "<genre>Romance</genre>\r\n"
                      "<price>4.950000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date>2000-09-02</publish_date>\r\n"
                      "<description>\n\t\t\t\tWhen Carla meets Paul at an ornithology\n\t\t\t\tconference, tempers fly as feathers get ruffled.\n\t\t\t</description>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk107\">\r\n"
                      "<author>Thurman, Paula</author>\r\n"
                      "<title>Splish Splash</title>\r\n"
                      "<genre>Romance</genre>\r\n"
                      "<price>4.950000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date>2000-11-02</publish_date>\r\n"
                      "<description>\n\t\t\t\tA deep sea diver finds true love twenty\n\t\t\t\tthousand leagues beneath the sea.\n\t\t\t</description>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk108\">\r\n"
                      "<author>Knorr, Stefan</author>\r\n"
                      "<title>Creepy Crawlies</title>\r\n"
                      "<genre>Horror</genre>\r\n"
                      "<price>4.950000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date>2000-12-06</publish_date>\r\n"
                      "<description>\n\t\t\t\tAn anthology of horror stories about roaches,\n\t\t\t\tcentipedes, scorpions  and other insects.\n\t\t\t</description>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk109\">\r\n"
                      "<author>Kress, Peter</author>\r\n"
                      "<title>Paradox Lost</title>\r\n"
                      "<genre>Science Fiction</genre>\r\n"
                      "<price>6.950000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date>2000-11-02</publish_date>\r\n"
                      "<description>\n\t\t\t\tAfter an inadvertant trip through a Heisenberg\n\t\t\t\tUncertainty Device, James Salway discovers the problems\n\t\t\t\tof being quantum.\n\t\t\t</description>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk110\">\r\n"
                      "<author>O&apos;Brien, Tim</author>\r\n"
                      "<title>Microsoft .NET: The Programming Bible</title>\r\n"
                      "<genre>Computer</genre>\r\n"
                      "<price>36.950001</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date>2000-12-09</publish_date>\r\n"
                      "<description>\n\t\t\t\tMicrosoft&apos;s .NET initiative is explored in\n\t\t\t\tdetail in this deep programmer&apos;s reference.\n\t\t\t</description>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk111\">\r\n"
                      "<author>O&apos;Brien, Tim</author>\r\n"
                      "<title>MSXML3: A Comprehensive Guide</title>\r\n"
                      "<genre>Computer</genre>\r\n"
                      "<price>36.950001</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date>2000-12-01</publish_date>\r\n"
                      "<description>\n\t\t\t\tThe Microsoft MSXML3 parser is covered in\n\t\t\t\tdetail, with attention to XML DOM interfaces, XSLT processing,\n\t\t\t\tSAX and more.\n\t\t\t</description>\r\n"
                      "</book>\r\n"
                      "<book id=\"bk112\">\r\n"
                      "<author>Galos, Mike</author>\r\n"
                      "<title>Visual Studio 7: A Comprehensive Guide</title>\r\n"
                      "<genre>Computer</genre>\r\n"
                      "<price>49.950001</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date>2001-04-16</publish_date>\r\n"
                      "<description>\n\t\t\t\tMicrosoft Visual Studio 7 is explored in depth,\n\t\t\t\tlooking at how Visual Basic, Visual C++, C#, and ASP+ are\n\t\t\t\tintegrated into a comprehensive development\n\t\t\t\tenvironment.\n\t\t\t</description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book id=\"\">\r\n"
                      "<author></author>\r\n"
                      "<title></title>\r\n"
                      "<genre></genre>\r\n"
                      "<price>0.000000</price>\r\n"
                      "<bha></bha>\r\n"
                      "<publish_date></publish_date>\r\n"
                      "<description></description>\r\n"
                      "</book>\r\n"
                      "<book></book>\r\n"
                      "</catalog>\r\n"
                      "<arrayofother>\r\n"
                      "<other>\r\n"
                      "<a>1</a>\r\n"
                      "<b>2</b>\r\n"
                      "<c>3</c>\r\n"
                      "</other>\r\n"
                      "<other>\r\n"
                      "<a>4</a>\r\n"
                      "<b>5</b>\r\n"
                      "<c>6</c>\r\n"
                      "</other>\r\n"
                      "<other>\r\n"
                      "<a>7</a>\r\n"
                      "<b>8</b>\r\n"
                      "<c>9</c>\r\n"
                      "</other>\r\n"
                      "<other>\r\n"
                      "<a>10</a>\r\n"
                      "<b>11</b>\r\n"
                      "<c>12</c>\r\n"
                      "</other>\r\n"
                      "<other></other>\r\n"
                      "</arrayofother>\r\n"
                      "</MulFtpUserSetting>\r\n";

#if 0 // To track issue but crash when there is no problem
	const char *p = XmlOut_S.c_str();
	const char *q = Res_S.c_str();
	while (*p == *q) { p++; q++; }
	//printf("%d ok on %d\r\n", static_cast<uint32_t>(p - XmlOut_S.c_str()), static_cast<uint32_t>(XmlOut_S.size()));
	uint32_t Start_U32, Nb_U32, OffsetFirstDiff_U32 = static_cast<uint32_t>(p - XmlOut_S.c_str());
	if (OffsetFirstDiff_U32 > 128)
	{
		Start_U32 = OffsetFirstDiff_U32;
		Nb_U32 = 128;
	}
	else
	{
		Start_U32 = 0;
		Nb_U32 = OffsetFirstDiff_U32;
	}
	std::string Out_S = XmlOut_S.substr(Start_U32, Nb_U32);
	std::string Expect_S = Res_S.substr(Start_U32, Nb_U32);
	//printf("OUT:    '%s'\r\n", Out_S.c_str());
	//printf("EXPECT: '%s'\r\n", Expect_S.c_str());

	Out_S = XmlOut_S.substr(OffsetFirstDiff_U32, 32);
	Expect_S = Res_S.substr(OffsetFirstDiff_U32, 32);
	//printf("DiffOUT:      '%s'\r\n", Out_S.c_str());
	//printf("DiffEXPECTED: '%s'\r\n", Expect_S.c_str());
	const char *pOut_c = Out_S.c_str();
	const char *pExpected_c = Expect_S.c_str();
#endif

  EXPECT_STREQ(XmlOut_S.c_str(), Res_S.c_str());

  Sts_i = BofXmlWriter_O.FromByte("", S_pOptionXml_X, XmlOut_S);
  EXPECT_EQ(Sts_i, 0);

  BOF_SAFE_DELETE(pBofXmlParser_O);
}