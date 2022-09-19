/*
 * Copyright (c) 2015-2025, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit tests of the jsonparser library
 *
 * Name:        ut_jsonparser.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */

 /*** Include files ***********************************************************/
#include <gtest/gtest.h>
#include <bofstd/bofjsonwriter.h>
#include <bofstd/bofjsonparser.h>
#include <bofstd/bofsocket.h>
#include <bofstd/boffs.h>
#include <bofstd/bofvideostandard.h>
#include <bofstd/bof2d.h>

USE_BOF_NAMESPACE()
#include "ut_parser.h"

#include <fstream> 
#include <json/json.h>
/*
  "JsonType": {
    "bool": true,
    "decimal": 10,
    "float": 3.14,
    "string": "Hello world !\n",
    "null": null,
    "employee": {
      "name": "John",
      "age": 30,
      "city": "New York"
    },
    "employees": [ "John", "Anna", "Peter" ]
  }
*/
void PrintRoot(Json::Value _Root)
{
  Json::Value::iterator It = _Root.begin();
  std::string name;
  int i = 0;

  for (It = _Root.begin(); It != _Root.end(); It++)
  {
    name = _Root.getMemberNames()[i];
    if (_Root[name].isInt())
    {
      std::cout << "MemberName " << name << " int " << std::endl;
      std::cout << _Root[name].asInt() << std::endl;
    }
    else if (_Root[name].isDouble())
    {
      std::cout << "MemberName " << name << " double " << std::endl;
      std::cout << _Root[name].asDouble() << std::endl;
    }
    else if (_Root[name].isBool())
    {
      std::cout << "MemberName " << name << " bool " << std::endl;
      std::cout << _Root[name].asBool() << std::endl;
    }
    else if (_Root[name].isArray())
    {
      std::cout << "MemberName " << name << " array size " << _Root[name].size() << std::endl;
      int size = _Root[name].size();
      for (int i = 0; i < size; i++)
      {
        Json::Value dat1 = _Root[name];
        PrintRoot(dat1.get(i, _Root[name]));
      }
      std::cout << "MemberName " << name << " array is object " << std::endl;
    }
    else if (_Root[name].isInt64())
    {
      std::cout << "MemberName " << name << " array " << std::endl;
      std::cout << _Root[name].asInt64() << std::endl;
    }
    else if (_Root[name].isString())
    {
      std::cout << "MemberName " << name << " string " << std::endl;
      std::cout << _Root[name].asString() << std::endl;
    }
    else if (_Root[name].isObject())
    {
      std::cout << "MemberName " << name << " object " << std::endl;
      PrintRoot(_Root[name]);
      //std::cout << root[name].asCString() << std::endl;
    }
    else
    {
      std::cout << "MemberName " << name << " unknown type " << std::endl;
      //std::cout << root[name].asCString() << std::endl;
    }
    i++;
  }
}

TEST(JsonParser_Test, JsonTypeNative)
{
  Json::Value Root, Val;
  std::ifstream Ifs;
  Json::CharReaderBuilder JsonReader;
  JSONCPP_STRING JsonErr;

  Ifs.open("./data/jsonparser.json");
  JsonReader["collectComments"] = true;
  if (!parseFromStream(JsonReader, Ifs, &Root, &JsonErr))
  {
    std::cout << JsonErr << std::endl;
    Ifs.close();
  }
  else
  {
    Ifs.close();

    Val = Root["JsonType"];
    Val = Root["MmgwSetting"];
    PrintRoot(Root);
  }
}

//BOFERR JsonParseResultUltimateCheck(uint32_t /*_Index_U32*/, const BOFPARAMETER & /*_rBofCommandlineOption_X*/, const bool _CheckOk_B, const char * /*_pOptNewVal_c*/)
BOFERR JsonParseResultUltimateCheck(uint32_t /*_Index_U32*/, const BOFPARAMETER &_rBofCommandlineOption_X, const bool _CheckOk_B, const char *_pOptNewVal_c)
{
  BOFERR Rts_E = _CheckOk_B ? BOF_ERR_NO_ERROR : BOF_ERR_NO;

  printf("Check is '%s'\r\n", _CheckOk_B ? "TRUE" : "FALSE");
  printf("Op pUser %p Name %s Tp %d OldVal %p NewVal %s\r\n", _rBofCommandlineOption_X.pUser, _rBofCommandlineOption_X.Name_S.c_str(), static_cast<uint32_t>(_rBofCommandlineOption_X.ArgType_E), _rBofCommandlineOption_X.pValue, _pOptNewVal_c ? _pOptNewVal_c : "nullptr");

  return Rts_E;
}

//bool JsonParseError(int /*_Sts_i*/, const BOFPARAMETER & /*_rJsonEntry_X*/, const char * /*_pValue*/)
bool JsonParseError(int _Sts_i, const BOFPARAMETER &_rJsonEntry_X, const char *_pValue)
{
  bool Rts_B = true;

  printf("JSON error %d on entry pUser %p value %s\r\n", _Sts_i, _rJsonEntry_X.pUser, _pValue ? _pValue : "nullptr");
  return Rts_B;
}

BOFERR JsonWriteResultUltimateCheck(uint32_t /*_Index_U32*/, const BOFPARAMETER & /*_rBofCommandlineOption_X*/, const bool _CheckOk_B, const char * /*_pOptNewVal_c*/)
{
  BOFERR Rts_E = _CheckOk_B ? BOF_ERR_NO_ERROR : BOF_ERR_NO;

  //	printf("Check is '%s'\r\n", _CheckOk_B ? "TRUE" : "FALSE");
  //	printf("Op pUser %p Name %s Tp %d OldVal %p NewVal %s\r\n", _rBofCommandlineOption_X.pUser, _rBofCommandlineOption_X.Name_S.c_str(), _rBofCommandlineOption_X.ArgType_E, _rBofCommandlineOption_X.pValue, _pOptNewVal_c ? _pOptNewVal_c : "nullptr");

  return Rts_E;
}

bool JsonWriteError(int /*_Sts_E*/, const BOFPARAMETER & /*_rJsonEntry_X*/, const char * /*_pValue*/)
{
  bool Rts_B = true;

  //	printf("JSON error %d on entry pUser %p value %s\r\n", _Sts_E, _rJsonEntry_X.pUser, _pValue ? _pValue : "nullptr");
  return Rts_B;
}
//{"JsonType": {"bool":true, "decimal":10, "float":3.14, "string":"Hello world !\n", "employee":{"name":"John", "age":30, "city":"New York"}, "employees":["John", "Anna", "Peter"] }}
JSON_TYPE       S_JsonType_X;
std::vector< BOFPARAMETER > S_JsonTypeSchemaCollection =
{
  { nullptr,  "bool",    "", "", "JsonType",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_JsonType_X.Bool_B, BOOL, 0, 0) },
  { nullptr,  "decimal", "", "", "JsonType",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_JsonType_X.Decimal_S64, INT64, 0, 0) },
  { nullptr,  "float",   "", "", "JsonType",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_JsonType_X.Double_lf, DOUBLE, 0, 0) },
  { nullptr,  "string",  "", "", "JsonType",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_JsonType_X.String_S, STDSTRING, 0, 0) },

  { nullptr,  "name",    "", "", "JsonType.employee",  BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_JsonType_X.Employee_X.Name_S, STDSTRING, 0, 0) },
  { nullptr,  "age",     "", "", "JsonType.employee",  BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_JsonType_X.Employee_X.Age_S32, INT16, 0, 0) },
  { nullptr,  "city",    "", "", "JsonType.employee",  BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_JsonType_X.Employee_X.City_S, STDSTRING, 0, 0) },

  { nullptr,  "name",    "", "", "JsonType.employees", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(JSON_OBJECT, S_JsonType_X.pEmployee_X, Name_S, STDSTRING, 0, 0) },
  { nullptr,  "age",     "", "", "JsonType.employees", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(JSON_OBJECT, S_JsonType_X.pEmployee_X, Age_S32, INT32, 0, 0) },
  { nullptr,  "city",    "", "", "JsonType.employees", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(JSON_OBJECT, S_JsonType_X.pEmployee_X, City_S, STDSTRING, 0, 0) },

  { nullptr,  "",  "",   "", "JsonType.strings",   BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_JsonType_X.StringCollection, STDSTRING, 0, 0) },
};
TEST(JsonParser_Test, JsonType)
{
  BofJsonParser *pBofJsonParser_O;
  const char *pValue_c;
  int           Sts_i;
  BofPath CrtDir, Path;
  std::string JsonData_S;

  S_JsonType_X.Reset();

  Bof_GetCurrentDirectory(CrtDir);
  Path = CrtDir + "data/jsonparser.json";
  EXPECT_EQ(Bof_ReadFile(Path, JsonData_S), BOF_ERR_NO_ERROR);

  pBofJsonParser_O = new BofJsonParser(JsonData_S);
  EXPECT_TRUE(pBofJsonParser_O != nullptr);
  Sts_i = pBofJsonParser_O->ToByte(S_JsonTypeSchemaCollection, JsonParseResultUltimateCheck, JsonParseError);
  EXPECT_EQ(Sts_i, 0);

  EXPECT_TRUE(S_JsonType_X.Bool_B);
  EXPECT_EQ(S_JsonType_X.Decimal_S64, 10);
  EXPECT_EQ(S_JsonType_X.Double_lf, 3.14);
  EXPECT_STREQ(S_JsonType_X.String_S.c_str(), "Hello world !\n");
  EXPECT_EQ(S_JsonType_X.Employee_X.Age_S32, 30);
  EXPECT_STREQ(S_JsonType_X.Employee_X.Name_S.c_str(), "John");
  EXPECT_STREQ(S_JsonType_X.Employee_X.City_S.c_str(), "New York");
  EXPECT_EQ(S_JsonType_X.pEmployee_X[0].Age_S32, 30);
  EXPECT_STREQ(S_JsonType_X.pEmployee_X[0].Name_S.c_str(), "John");
  EXPECT_STREQ(S_JsonType_X.pEmployee_X[0].City_S.c_str(), "New York");
  EXPECT_EQ(S_JsonType_X.pEmployee_X[1].Age_S32, 20);
  EXPECT_STREQ(S_JsonType_X.pEmployee_X[1].Name_S.c_str(), "Anna");
  EXPECT_STREQ(S_JsonType_X.pEmployee_X[1].City_S.c_str(), "LosAngeles");
  EXPECT_EQ(S_JsonType_X.pEmployee_X[2].Age_S32, 10);
  EXPECT_STREQ(S_JsonType_X.pEmployee_X[2].Name_S.c_str(), "Peter");
  EXPECT_STREQ(S_JsonType_X.pEmployee_X[2].City_S.c_str(), "Denver");
  EXPECT_EQ(S_JsonType_X.pEmployee_X[3].Age_S32, 0);
  EXPECT_STREQ(S_JsonType_X.pEmployee_X[3].Name_S.c_str(), "");
  EXPECT_STREQ(S_JsonType_X.pEmployee_X[3].City_S.c_str(), "");

  EXPECT_EQ(S_JsonType_X.StringCollection.size(), 3);
  EXPECT_STREQ(S_JsonType_X.StringCollection[0].c_str(), "John");
  EXPECT_STREQ(S_JsonType_X.StringCollection[1].c_str(), "Anna");
  EXPECT_STREQ(S_JsonType_X.StringCollection[2].c_str(), "Peter");


  /*
  pValue_c = pBofJsonParser_O->GetFirstElementFromOid("MulFtpUserSetting.DeployIpAddress");
  EXPECT_TRUE(pValue_c != nullptr);
  pValue_c = pBofJsonParser_O->GetNextElementFromOid();
  EXPECT_TRUE(pValue_c == nullptr);

  pValue_c = pBofJsonParser_O->GetFirstElementFromOid("MulFtpUserSetting.catalog.book.id");
  EXPECT_TRUE(pValue_c != nullptr);
  pValue_c = pBofJsonParser_O->GetNextElementFromOid();
  EXPECT_TRUE(pValue_c != nullptr);
*/
  BOF_SAFE_DELETE(pBofJsonParser_O);
}
static APPPARAM                    S_AppParamJson_X;
static std::vector< BOFPARAMETER > S_OptionJsonCollection =
{
  { nullptr, "id",                           "id",                           "",         "MulFtpUserSetting.catalog.book",       BOFPARAMETER_ARG_FLAG::XML_ATTRIBUTE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM,                             S_AppParamJson_X.pBook_X,  pId_c,          CHARSTRING, 1, sizeof(S_AppParamJson_X.pBook_X[0].pId_c) - 1) },

  { nullptr, "DeployIpAddress",              "DeployIpAddress",              "",         "MulFtpUserSetting",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AppParamJson_X.DeployIpAddress_X,               IPV4,                  0,              0) },
  { nullptr, "DeployIpPort",                 "DeployIpPort",                 "",         "MulFtpUserSetting",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AppParamJson_X.DeployIpPort_U16,                UINT16,                0,              0) },
  { nullptr, "DeployProtocol",               "DeployProtocol",               "",         "MulFtpUserSetting",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AppParamJson_X.pDeployProtocol_c,               CHARSTRING,            1,              sizeof(S_AppParamJson_X.pDeployProtocol_c) - 1) },
  { nullptr, "DeployDirectory",              "DeployDirectory",              "",         "MulFtpUserSetting",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AppParamJson_X.pDeployDirectory_c,              CHARSTRING,            1,              sizeof(S_AppParamJson_X.pDeployDirectory_c) - 1) },
  { nullptr, "LoginUser",                    "LoginUser",                    "",         "MulFtpUserSetting",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AppParamJson_X.pLoginUser_c,                    CHARSTRING,            1,              sizeof(S_AppParamJson_X.pLoginUser_c) - 1) },
  { nullptr, "LoginPassword",                "LoginPassword",                "",         "MulFtpUserSetting",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AppParamJson_X.pLoginPassword_c,                CHARSTRING,            1,              sizeof(S_AppParamJson_X.pLoginPassword_c) - 1) },
  { nullptr, "Email",                        "Email",                        "",         "MulFtpUserSetting",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AppParamJson_X.pEmail_c,                        CHARSTRING,            1,              sizeof(S_AppParamJson_X.pEmail_c) - 1) },
  { nullptr, "UserName",                     "UserName",                     "",         "MulFtpUserSetting",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AppParamJson_X.pUserName_c,                     CHARSTRING,            1,              sizeof(S_AppParamJson_X.pUserName_c) - 1) },
  { nullptr, "UserCompany",                  "UserCompany",                  "",         "MulFtpUserSetting",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AppParamJson_X.pUserCompany_c,                  CHARSTRING,            1,              sizeof(S_AppParamJson_X.pUserCompany_c) - 1) },
  { nullptr, "ToolChainBaseDirectory",       "ToolChainBaseDirectory",       "",         "MulFtpUserSetting",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AppParamJson_X.pToolChainBaseDirectory_c,       CHARSTRING,            1,              sizeof(S_AppParamJson_X.pToolChainBaseDirectory_c) - 1) },
  { nullptr, "TemplateProjectBaseDirectory", "TemplateProjectBaseDirectory", "",         "MulFtpUserSetting",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AppParamJson_X.pTemplateProjectBaseDirectory_c, CHARSTRING,            1,              sizeof(S_AppParamJson_X.pTemplateProjectBaseDirectory_c) - 1) },

  // For xml serialize xml array entry must be contiguous MulFtpUserSetting.catalog.book
  { nullptr, "id",                           "id",                           "",         "MulFtpUserSetting.catalog.book",       BOFPARAMETER_ARG_FLAG::XML_ATTRIBUTE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM,                             S_AppParamJson_X.pBook_X,  pId_c,          CHARSTRING, 1, sizeof(S_AppParamJson_X.pBook_X[0].pId_c) - 1) },
  { nullptr, "author",                       "author",                       "",         "MulFtpUserSetting.catalog.book",       BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM,                             S_AppParamJson_X.pBook_X,  pAuthor_c,      CHARSTRING, 1, sizeof(S_AppParamJson_X.pBook_X[0].pAuthor_c) - 1) },

  { nullptr, "title",                        "title",                        "",         "MulFtpUserSetting.catalog.book",       BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM,                             S_AppParamJson_X.pBook_X,  pTitle_c,       CHARSTRING, 1, sizeof(S_AppParamJson_X.pBook_X[0].pTitle_c) - 1) },
  { nullptr, "genre",                        "genre",                        "",         "MulFtpUserSetting.catalog.book",       BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM,                             S_AppParamJson_X.pBook_X,  pGenre_c,       CHARSTRING, 1, sizeof(S_AppParamJson_X.pBook_X[0].pGenre_c) - 1) },
  { nullptr, "price",                        "price",                        "",         "MulFtpUserSetting.catalog.book",       BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM,                             S_AppParamJson_X.pBook_X,  Price_f,        FLOAT, 0, 0) },


  // BAD for json  { nullptr, "tag", "dbg1", "", "MulFtpUserSetting.catalog.book.bhaattr", BOFPARAMETER_ARG_FLAG::XML_ATTRIBUTE, BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM, S_AppParamJson_X.pBook_X, pBha1_c, CHARSTRING, 0, 0) },

  { nullptr, "bha",                          "dbg2",                         "",         "MulFtpUserSetting.catalog.book",       BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM,                             S_AppParamJson_X.pBook_X,  pBha2_c,        CHARSTRING, 0, 0) },
  { nullptr, "publish_date",                 "publish_date",                 "%Y-%m-%d", "MulFtpUserSetting.catalog.book",       BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM,                             S_AppParamJson_X.pBook_X,  PublishDate_X,  DATE, 0, 0) },
  { nullptr, "description",                  "description",                  "",         "MulFtpUserSetting.catalog.book",       BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_ARRAY_OF_STRUCT(BOOKPARAM,                             S_AppParamJson_X.pBook_X,  pDescription_c, CHARSTRING, 1, sizeof(S_AppParamJson_X.pBook_X[0].pDescription_c) - 1) },

  { nullptr, "a",                            "other a",                      "",         "MulFtpUserSetting.arrayofother.other", BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_ARRAY_OF_STRUCT(OTHER,                                 S_AppParamJson_X.pOther_X, a_U32,          UINT32, 0, 0) },
  { nullptr, "b",                            "other b",                      "",         "MulFtpUserSetting.arrayofother.other", BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_ARRAY_OF_STRUCT(OTHER,                                 S_AppParamJson_X.pOther_X, b_U32,          UINT32, 0, 0) },
  { nullptr, "c",                            "other c",                      "",         "MulFtpUserSetting.arrayofother.other", BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_ARRAY_OF_STRUCT(OTHER,                                 S_AppParamJson_X.pOther_X, c_U32,          UINT32, 0, 0) },

};

static IPSENDER                    S_IpSenderParam_X;
static std::vector< BOFPARAMETER > S_IpSenderOptionCollection =
{
  { nullptr,  "description",           "",              "",         "add_sender",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_IpSenderParam_X.Description_S,               STDSTRING,                  0,              0) },
  { nullptr,  "device_id",            "",              "",         "add_sender",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_IpSenderParam_X.DeviceId_S,               STDSTRING,                  0,              0) },
  { nullptr,  "flow_id",              "",              "",         "add_sender",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_IpSenderParam_X.FlowId_S,               STDSTRING,                  0,              0) },
  { nullptr,  "id",                   "",              "",         "add_sender",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_IpSenderParam_X.Id_S,               STDSTRING,                  0,              0) },
  { nullptr,  "label",                "",              "",         "add_sender",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_IpSenderParam_X.Label_S,               STDSTRING,                  0,              0) },
  { nullptr,  "manifest_href",        "",              "",         "add_sender",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_IpSenderParam_X.HrefManifest_S,               STDSTRING,                  0,              0) },
  //	{ nullptr,  "e",                 "",              "",         "add_sender.tags.taglist",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_ARRAY(S_IpSenderParam_X.pTag_S,   STDSTRING,                  0,              0) },
  { nullptr,  "",                 "",              "",         "add_sender.tags.",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_ARRAY(S_IpSenderParam_X.pTag_S,   STDSTRING,                  0,              0) },
  { nullptr,  "transport",            "",              "",         "add_sender",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_IpSenderParam_X.Transport_S,               STDSTRING,                  0,              0) },
  { nullptr,  "version",              "",              "",         "add_sender",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_IpSenderParam_X.Version_S,               STDSTRING,                  0,              0) },
};
static IPMANIFEST                  S_IpManifestParam_X;
static std::vector< BOF::BOFPARAMETER > S_IpManifestOptionCollection =
{
  { nullptr,  "",                 "",              "",         "add_manifest.sdp",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_ARRAY(S_IpManifestParam_X.pSdpLine_S,   STDSTRING,                  0,              0) },
  { nullptr,  "sender_id",            "",              "",         "add_manifest",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_IpManifestParam_X.SenderId_S,               STDSTRING,                  0,              0) },
};
static IPCONNECT                   S_IpConnectParam_X;
static std::vector< BOF::BOFPARAMETER > S_IpConnectOptionCollection =
{
  { nullptr,  "receiver_id",            "",              "",         "connect",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_IpConnectParam_X.ReceiverId_S,               STDSTRING,                  0,              0) },
  { nullptr,  "",                 "",              "",         "connect.sdp",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VECTOR(S_IpConnectParam_X.SdpCollection,   STDSTRING,                  0,              0) },
};




//{"add_device":{"id":"a8500668-9218-4063-ba36-9b4900b82e67","label":"","node_id":"00000000-0000-0000-0000-000000000000","receivers":[],"senders":[],"type":"","version":""}}
ADD_DEVICE       S_AddDevice_X;
std::vector< BOFPARAMETER > S_AddDeviceSchemaCollection =
{
  { nullptr,  "id",       "", "", "add_device",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddDevice_X.Id,					GUID,  0, 0) },
  { nullptr,  "label",		"", "", "add_device",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddDevice_X.Label_S,       STDSTRING,  0, 0) },
  { nullptr,  "node_id",  "", "", "add_device",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddDevice_X.NodeId,GUID,  0, 0) },
  { nullptr,  "",         "", "", "add_device.receivers", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_AddDevice_X.ReceiverCollection,   STDSTRING,  0, 0) },
  { nullptr,  "",         "", "", "add_device.senders",   BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_AddDevice_X.SenderCollection,   STDSTRING,  0, 0) },
  { nullptr,  "type",     "", "", "add_device",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddDevice_X.Type_S,   STDSTRING,  0, 0) },
  { nullptr,  "version",  "", "", "add_device",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddDevice_X.Version_S,     STDSTRING,  0, 0) },
};

//{"add_source":{"caps":{},"description":"OUT1","device_id":"a8500668-9218-4063-ba36-9b4900b82e67","format":"","id":"895085f3-76a1-4f09-b3dd-2aabeb230cd8","label":"Player A","parents":[],"tags":{"":[]},"version":""}}
ADD_SOURCE       S_AddSource_X;
std::vector< BOFPARAMETER > S_AddSourceSchemaCollection =
{
  { nullptr,  "caps",       "", "", "add_source",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddSource_X.Caps_S,					STDSTRING,  0, 0) },
  { nullptr,  "description",		"", "", "add_source",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddSource_X.Description_S,       STDSTRING,  0, 0) },
  { nullptr,  "device_id",  "", "", "add_source",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddSource_X.DeviceId,GUID,  0, 0) },
  { nullptr,  "format",  "", "", "add_source",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddSource_X.Format_S,STDSTRING,  0, 0) },
  { nullptr,  "id",  "", "", "add_source",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddSource_X.Id,GUID,  0, 0) },
  { nullptr,  "label",  "", "", "add_source",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddSource_X.Label_S,STDSTRING,  0, 0) },
  { nullptr,  "",         "", "", "add_source.parents", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_AddSource_X.ParentCollection,   STDSTRING,  0, 0) },
  { nullptr,  "",         "", "", "add_source.tags.",   BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_AddSource_X.TagCollection,   STDSTRING,  0, 0) },
  { nullptr,  "version",  "", "", "add_source",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddSource_X.Version_S,     STDSTRING,  0, 0) },
};

//{"add_flow":{"description":"OUT1","format":"","id":"40b94c9d-9bf1-4721-95ae-4316b4e080ea","label":"Player A","parents":[],"source_id":"895085f3-76a1-4f09-b3dd-2aabeb230cd8","tags":{"":[]},"version":""}}
ADD_FLOW       S_AddFlow_X;
std::vector< BOFPARAMETER > S_AddFlowSchemaCollection =
{
  { nullptr,  "description",		"", "", "add_flow",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddFlow_X.Description_S,       STDSTRING,  0, 0) },
  { nullptr,  "format",  "", "", "add_flow",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddFlow_X.Format_S,STDSTRING,  0, 0) },
  { nullptr,  "id",  "", "", "add_flow",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddFlow_X.Id,GUID,  0, 0) },
  { nullptr,  "label",  "", "", "add_flow",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddFlow_X.Label_S,STDSTRING,  0, 0) },
  { nullptr,  "",         "", "", "add_flow.parents", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_AddFlow_X.ParentCollection,   STDSTRING,  0, 0) },
  { nullptr,  "source_id",  "", "", "add_flow",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddFlow_X.SourceId,GUID,  0, 0) },
  { nullptr,  "",         "", "", "add_flow.tags.",   BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_AddFlow_X.TagCollection,   STDSTRING,  0, 0) },
  { nullptr,  "version",  "", "", "add_flow",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddFlow_X.Version_S,     STDSTRING,  0, 0) },
};

//{"add_sender":{"description":"OUT1","device_id":"a8500668-9218-4063-ba36-9b4900b82e67","flow_id":"40b94c9d-9bf1-4721-95ae-4316b4e080ea","id":"edbe7239-8659-46c9-a4f7-85b46a2efc73","label":"Player A","manifest_href":"","tags":{"":[]},"transport":"","version":""}}
ADD_SENDER       S_AddSender_X;
std::vector< BOFPARAMETER > S_AddSenderSchemaCollection =
{
  { nullptr,  "description",           "",              "",         "add_sender",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AddSender_X.Description_S,               STDSTRING,                  0,              0) },
  { nullptr,  "device_id",            "",              "",         "add_sender",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AddSender_X.DeviceId,               GUID,                  0,              0) },
  { nullptr,  "flow_id",              "",              "",         "add_sender",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AddSender_X.FlowId,               GUID,                  0,              0) },
  { nullptr,  "id",                   "",              "",         "add_sender",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AddSender_X.Id,               GUID,                  0,              0) },
  { nullptr,  "label",                "",              "",         "add_sender",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AddSender_X.Label_S,               STDSTRING,                  0,              0) },
  { nullptr,  "manifest_href",        "",              "",         "add_sender",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AddSender_X.HrefManifest_S,               STDSTRING,                  0,              0) },
  { nullptr,  "",                 "",              "",         "add_sender.tags.",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VECTOR(S_AddSender_X.TagCollection,   STDSTRING,                  0,              0) },
  { nullptr,  "transport",            "",              "",         "add_sender",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AddSender_X.Transport_S,               STDSTRING,                  0,              0) },
  { nullptr,  "version",              "",              "",         "add_sender",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AddSender_X.Version_S,               STDSTRING,                  0,              0) },
};

//{"add_receiver":{"caps":{},"description":"IN1","device_id":"a8500668-9218-4063-ba36-9b4900b82e67","format":"","id":"998a20b2-2a8a-42c3-a527-46d1de96a9df","label":"Recorder A","subscription":{"sender_id":"00000000-0000-0000-0000-000000000000"},"tags":{"":[]},"transport":"","version":""}}
ADD_RECEIVER       S_AddReceiver_X;
std::vector< BOFPARAMETER > S_AddReceiverSchemaCollection =
{
  { nullptr,  "caps",       "", "", "add_receiver",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddReceiver_X.Caps_S,					STDSTRING,  0, 0) },
  { nullptr,  "description",		"", "", "add_receiver",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddReceiver_X.Description_S,       STDSTRING,  0, 0) },
  { nullptr,  "device_id",  "", "", "add_receiver",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddReceiver_X.DeviceId,GUID,  0, 0) },
  { nullptr,  "format",  "", "", "add_receiver",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddReceiver_X.Format_S,STDSTRING,  0, 0) },
  { nullptr,  "id",  "", "", "add_receiver",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddReceiver_X.Id,GUID,  0, 0) },
  { nullptr,  "label",  "", "", "add_receiver",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddReceiver_X.Label_S,STDSTRING,  0, 0) },
  { nullptr,  "sender_id",         "", "", "add_receiver.subscription", BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddReceiver_X.SubscriptionSenderId,   GUID,  0, 0) },
  { nullptr,  "",         "", "", "add_receiver.tags.",   BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_AddReceiver_X.TagCollection,   STDSTRING,  0, 0) },
  { nullptr,  "transport",  "", "", "add_receiver",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddReceiver_X.Transport_S,     STDSTRING,  0, 0) },
  { nullptr,  "version",  "", "", "add_receiver",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VARIABLE(S_AddReceiver_X.Version_S,     STDSTRING,  0, 0) },
};

//{"add_manifest":{"sdp":["v=0","o=- 0 0 IN IP4 18.52.86.0","s=OUT1","i=Player A","c=IN IP4 33.67.101.0/32","t=0 0","m=video 16384 RTP/AVP 96","a=rtpmap:96 raw/90000","a=fmtp:96 packetization-mode=1"],"sender_id":"edbe7239-8659-46c9-a4f7-85b46a2efc73"}}
ADD_MANIFEST       S_AddManifest_X;
std::vector< BOFPARAMETER > S_AddManifestSchemaCollection =
{
  { nullptr,  "sender_id",            "",              "",         "add_manifest",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_AddManifest_X.SenderId,               GUID,                  0,              0) },
  { nullptr,  "",                 "",              "",         "add_manifest.sdp",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VECTOR(S_AddManifest_X.SdpLineCollection,   STDSTRING,                  0,              0) },
};

//{"connect":{"receiver_id":"1c513fc7-faa8-4a89-bd1c-56d8b00a1355","sdp":["v=0","o=- 0 0 IN IP4 18.52.86.0","s=OUT1","i=Player A","c=IN IP4 33.67.101.0/32","t=0 0","m=video 16384 RTP/AVP 96","a=rtpmap:96 raw/90000","a=fmtp:96 packetization-mode=1"]}}
CMD_CONNECT       S_CmdConnect_X;
std::vector< BOFPARAMETER > S_CmdConnectSchemaCollection =
{
  { nullptr,  "receiver_id",            "",              "",         "connect",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_CmdConnect_X.ReceiverId,               GUID,                  0,              0) },
  { nullptr,  "",                 "",              "",         "connect.sdp",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VECTOR(S_CmdConnect_X.SdpLineCollection,   STDSTRING,                  0,              0) },
};

//{"status_receivers":{"arg":""}}
CMD_STATUS_RECEIVERS       S_CmdStatusReceiver_X;
std::vector< BOFPARAMETER > S_CmdStatusReceiverSchemaCollection =
{
  { nullptr,  "arg",                 "",              "",         "status_receivers",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_CmdStatusReceiver_X.Arg_S,   STDSTRING,                  0,              0) },
};
//{"status_receivers":{"":[{"id":"1c513fc7-faa8-4a89-bd1c-56d8b00a1355","ip":"18.52.86.0:1234","present":"both","impaired":"true","state":"AB"},{"id":"1c513fc7-faa8-4a89-bd1c-56d8b00adead","ip":"18.52.86.2:1235","present":"primary","impaired":"false","state":"A"}] }}
REPLY_STATUS_RECEIVERS     S_ReplyStatusReceiver_X;
std::vector< BOFPARAMETER > S_ReplyStatusReceiverSchemaCollection =
{
  { nullptr,  "id",       "", "", "status_receivers",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_ReplyStatusReceiver_X.IdCollection,					GUID,  0, 0) },
  { nullptr,  "ip",		"", "", "status_receivers",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_ReplyStatusReceiver_X.IpCollection,       IPV4,  0, 0) },
  { nullptr,  "present",  "", "", "status_receivers",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_ReplyStatusReceiver_X.PresentCollection,STDSTRING,  0, 0) },
  { nullptr,  "impaired",  "", "", "status_receivers",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_ReplyStatusReceiver_X.ImpairedCollection,BOOL,  0, 0) },
  { nullptr,  "state",  "", "", "status_receivers",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_ReplyStatusReceiver_X.StateCollection,STDSTRING,  0, 0) },
};

//{"status_senders":{"arg":""}}
CMD_STATUS_SENDERS       S_CmdStatusSender_X;
std::vector< BOFPARAMETER > S_CmdStatusSenderSchemaCollection =
{
  { nullptr,  "arg",                 "",              "",         "status_senders",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_CmdStatusSender_X.Arg_S,   STDSTRING,                  0,              0) },
};

//{"status_senders":{"":[{"id":"1c513fc7-faa8-4a89-bd1c-56d8b00a1356","ip":"118.52.86.0:1234","present":"lost","impaired":"true","state":"B"},{"id":"1c513fc7-faa8-4a89-bd1c-56d8b00abeef","ip":"18.52.86.222:235","present":"primary","impaired":"true","state":"XA"}] }}
REPLY_STATUS_SENDERS       S_ReplyStatusSender_X;
std::vector< BOFPARAMETER > S_ReplyStatusSenderSchemaCollection =
{
  { nullptr,  "id",       "", "", "status_senders",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_ReplyStatusSender_X.IdCollection,					GUID,  0, 0) },
  { nullptr,  "ip",		"", "", "status_senders",           BOFPARAMETER_ARG_FLAG::IP_FORMAT_PORT, BOF_PARAM_DEF_VECTOR(S_ReplyStatusSender_X.IpCollection,       IPV4,  0, 0) },
  { nullptr,  "present",  "", "", "status_senders",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_ReplyStatusSender_X.PresentCollection,STDSTRING,  0, 0) },
  { nullptr,  "impaired",  "", "", "status_senders",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_ReplyStatusSender_X.ImpairedCollection,BOOL,  0, 0) },
  { nullptr,  "state",  "", "", "status_senders",           BOFPARAMETER_ARG_FLAG::NONE, BOF_PARAM_DEF_VECTOR(S_ReplyStatusSender_X.StateCollection,STDSTRING,  0, 0) },
};

TEST(JsonParser_Test, Json)
{
  BofJsonParser *pBofJsonParser_O;
  const char *pValue_c;
  int           Sts_i;
  BofPath CrtDir, Path;
  std::string JsonData_S;


  std::string JsonIn_S = "        {    \r\n \"add_source\"    \t :\t\r{kjlkjljl";
  EXPECT_STREQ("add_source", BofJsonParser::S_RootName(JsonIn_S).c_str());

  S_AppParamJson_X.Reset();

  Bof_GetCurrentDirectory(CrtDir);
  Path = CrtDir + "data/jsonparser.json";
  EXPECT_EQ(Bof_ReadFile(Path, JsonData_S), BOF_ERR_NO_ERROR);

  pBofJsonParser_O = new BofJsonParser(JsonData_S);
  EXPECT_TRUE(pBofJsonParser_O != nullptr);
  Sts_i = pBofJsonParser_O->ToByte(S_OptionJsonCollection, JsonParseResultUltimateCheck, JsonParseError);
  EXPECT_EQ(Sts_i, 0);


  // Check S_AppParam_X content
  pValue_c = pBofJsonParser_O->GetFirstElementFromOid("MulFtpUserSetting.DeployIpAddress");
  EXPECT_TRUE(pValue_c != nullptr);
  pValue_c = pBofJsonParser_O->GetNextElementFromOid();
  EXPECT_TRUE(pValue_c == nullptr);

  pValue_c = pBofJsonParser_O->GetFirstElementFromOid("MulFtpUserSetting.catalog.book.id");
  EXPECT_TRUE(pValue_c != nullptr);
  pValue_c = pBofJsonParser_O->GetNextElementFromOid();
  EXPECT_TRUE(pValue_c != nullptr);

  BOF_SAFE_DELETE(pBofJsonParser_O);
}


TEST(JsonParser_Test, IpSenderDeser)
{
  const char *pValue_c;
  int           Sts_i;

  BofJsonParser BofJsonParser_O("{\"add_sender\":{\"description\":\"OUT1\",\"device_id\":\"f98f5eaa-cda5-4396-b975-407748ba513b\",\"flow_id\":\"a3fc4d73-6fdc-4221-ae10-e1b6e006ac5c\",\"id\":\"a8a0b68c-0fbd-4b94-ad95-f637d4c4c6a3\",\"label\":\"Player A\",\"manifest_href\":\"http:/bha.txt\",\"tags\":{\"\":[\"1\",\"2\",\"3\"]},\"transport\":\"\",\"version\":\"\"}}");


  Sts_i = BofJsonParser_O.ToByte(S_IpSenderOptionCollection, JsonParseResultUltimateCheck, JsonParseError);
  EXPECT_EQ(Sts_i, 0);

  // Check S_AppParam_X content
  pValue_c = BofJsonParser_O.GetFirstElementFromOid("add_sender.device_id");
  EXPECT_TRUE(pValue_c != nullptr);
  pValue_c = BofJsonParser_O.GetNextElementFromOid();
  EXPECT_TRUE(pValue_c == nullptr);

  pValue_c = BofJsonParser_O.GetFirstElementFromOid("add_sender.tags..");
  EXPECT_TRUE(pValue_c != nullptr);
  pValue_c = BofJsonParser_O.GetNextElementFromOid();
  EXPECT_TRUE(pValue_c != nullptr);

}
TEST(JsonParser_Test, ManifestDeser)
{
  const char *pValue_c;
  int           Sts_i;

  BofJsonParser BofJsonParser_O("{\"add_manifest\":{\"sdp\":[\"v=0\",\"o=- 0 0 IN IP4 18.52.86.2\",\"s=OUT3\",\"i=Player C\",\"c=IN IP4 33.67.101.2/32\",\"t=0 0\",\"m=video 16386 RTP/AVP 96\",\"a=rtpmap:96 raw/90000\",\"a=fmtp:96 packetization-mode=1\"],\"sender_id\":\"81f4da21-c9ef-4b90-ab03-a4893cae32ed\"}}");

  Sts_i = BofJsonParser_O.ToByte(S_IpManifestOptionCollection, JsonParseResultUltimateCheck, JsonParseError);
  EXPECT_EQ(Sts_i, 0);

  // Check S_AppParam_X content
  pValue_c = BofJsonParser_O.GetFirstElementFromOid("add_manifest.sender_id");
  EXPECT_TRUE(pValue_c != nullptr);
  pValue_c = BofJsonParser_O.GetNextElementFromOid();
  EXPECT_TRUE(pValue_c == nullptr);

  pValue_c = BofJsonParser_O.GetFirstElementFromOid("add_manifest.sdp.");
  EXPECT_TRUE(pValue_c != nullptr);
  pValue_c = BofJsonParser_O.GetNextElementFromOid();
  EXPECT_TRUE(pValue_c != nullptr);

}
TEST(JsonParser_Test, ConnectDeser)
{
  const char *pValue_c;
  int           Sts_i;

  BofJsonParser BofJsonParser_O("{\"connect\":{\"receiver_id\":\"1c513fc7-faa8-4a89-bd1c-56d8b00a1355\",\"sdp\":[\"v=0\",\"o=- 0 0 IN IP4 18.52.86.0\",\"s=OUT1\",\"i=Player A\",\"c=IN IP4 33.67.101.0/32\",\"t=0 0\",\"m=video 16384 RTP/AVP 96\",\"a=rtpmap:96 raw/90000\",\"a=fmtp:96 packetization-mode=1\"]}}");

  Sts_i = BofJsonParser_O.ToByte(S_IpConnectOptionCollection, JsonParseResultUltimateCheck, JsonParseError);
  EXPECT_EQ(Sts_i, 0);

  // Check S_AppParam_X content
  pValue_c = BofJsonParser_O.GetFirstElementFromOid("connect.receiver_id");
  EXPECT_TRUE(pValue_c != nullptr);
  pValue_c = BofJsonParser_O.GetNextElementFromOid();
  EXPECT_TRUE(pValue_c == nullptr);

  pValue_c = BofJsonParser_O.GetFirstElementFromOid("connect.sdp.");
  EXPECT_TRUE(pValue_c != nullptr);
  pValue_c = BofJsonParser_O.GetNextElementFromOid();
  EXPECT_TRUE(pValue_c != nullptr);

}

TEST(JsonWriter_Test, Json)
{
  BOFERR           Sts_E;
  std::string JsonOut_S;
  BofJsonWriter BofJsonWriter_O;
  BofJsonParser *pBofJsonParser_O;
  BofPath CrtDir, Path;
  std::string JsonData_S;

  //memset(&S_AppParam_X, 0, sizeof(S_AppParam_X) );
  Bof_GetCurrentDirectory(CrtDir);
  Path = CrtDir + "data/jsonparser.json";
  EXPECT_EQ(Bof_ReadFile(Path, JsonData_S), BOF_ERR_NO_ERROR);

  pBofJsonParser_O = new BofJsonParser(JsonData_S);
  EXPECT_TRUE(pBofJsonParser_O != nullptr);
  Sts_E = pBofJsonParser_O->ToByte(S_OptionJsonCollection, JsonWriteResultUltimateCheck, JsonWriteError);
  EXPECT_EQ(Sts_E, 0);

  Sts_E = BofJsonWriter_O.FromByte(true, S_OptionJsonCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);

  Sts_E = BofJsonWriter_O.FromByte(false, S_OptionJsonCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);

  BOF_SAFE_DELETE(pBofJsonParser_O);
}


TEST(JsonWriter_Test, IpSenderSer)
{
  BOFERR           Sts_E;
  std::string JsonOut_S;
  std::string JsonIn_S = "{\"add_sender\":{\"description\":\"OUT1\",\"device_id\":\"f98f5eaa-cda5-4396-b975-407748ba513b\",\"flow_id\":\"a3fc4d73-6fdc-4221-ae10-e1b6e006ac5c\",\"id\":\"a8a0b68c-0fbd-4b94-ad95-f637d4c4c6a3\",\"label\":\"Player A\",\"manifest_href\":\"http:/bha.txt\",\"tags\":{\"\":[\"1\",\"2\",\"3\",\"\",\"\",\"\",\"\",\"\"]},\"transport\":\"\",\"version\":\"\"}}\n";
  BofJsonWriter BofJsonWriter;
  S_IpSenderParam_X.Reset();
  BofJsonParser BofJsonParser_O(JsonIn_S);

  Sts_E = BofJsonParser_O.ToByte(S_IpSenderOptionCollection, JsonWriteResultUltimateCheck, JsonWriteError);
  EXPECT_EQ(Sts_E, 0);

  Sts_E = BofJsonWriter.FromByte(true, S_IpSenderOptionCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), JsonIn_S.c_str());
  /*
  std::string t="{\"add_sender\":{\"description\":\"OUT1\",\"device_id\":\"f98f5eaa-cda5-4396-b975-407748ba513b\",\"flow_id\":\"a3fc4d73-6fdc-4221-ae10-e1b6e006ac5c\",\"id\":\"a8a0b68c-0fbd-4b94-ad95-f637d4c4c6a3\",\"label\":\"Player A\",\"manifest_href\":\"http:/bha.txt\",\"tags\":{\"\":[{\"\":\"1\"},{\"\":\"2\"},{\"\":\"3\"},{\"\":\"\"},{\"\":\"\"},{\"\":\"\"},{\"\":\"\"},{\"\":\"\"},{\"\":\"\"},{\"\":\"\"},{\"\":\"\"},{\"\":\"\"},{\"\":\"\"},{\"\":\"\"},{\"\":\"\"},{\"\":\"\"}]},\"transport\":\"\",\"version\":\"\"}}\n";
  const char *p = JsonOut_S.c_str();
  const char *q = t.c_str();
  while (*p == *q) { p++; q++; }
  */
  Sts_E = BofJsonWriter.FromByte(false, S_IpSenderOptionCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);

}

TEST(JsonWriter_Test, ManifestSer)
{
  BOFERR           Sts_E;
  std::string JsonOut_S;
  std::string JsonIn_S = "{\"add_manifest\":{\"sdp\":[\"v=0\",\"o=- 0 0 IN IP4 18.52.86.2\",\"s=OUT3\",\"i=Player C\",\"c=IN IP4 33.67.101.2/32\",\"t=0 0\",\"m=video 16386 RTP/AVP 96\",\"a=rtpmap:96 raw/90000\",\"a=fmtp:96 packetization-mode=1\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"],\"sender_id\":\"81f4da21-c9ef-4b90-ab03-a4893cae32ed\"}}\n";

  BofJsonWriter BofJsonWriter;
  S_IpManifestParam_X.Reset();
  BofJsonParser BofJsonParser_O(JsonIn_S);

  Sts_E = BofJsonParser_O.ToByte(S_IpManifestOptionCollection, JsonWriteResultUltimateCheck, JsonWriteError);
  EXPECT_EQ(Sts_E, 0);

  Sts_E = BofJsonWriter.FromByte(true, S_IpManifestOptionCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), JsonIn_S.c_str());

  Sts_E = BofJsonWriter.FromByte(false, S_IpManifestOptionCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
}

TEST(JsonWriter_Test, ConnectSer)
{
  BOFERR           Sts_E;
  std::string JsonOut_S;
  std::string JsonIn_S = "{\"connect\":{\"receiver_id\":\"1c513fc7-faa8-4a89-bd1c-56d8b00a1355\",\"sdp\":[\"v=0\",\"o=- 0 0 IN IP4 18.52.86.0\",\"s=OUT1\",\"i=Player A\",\"c=IN IP4 33.67.101.0/32\",\"t=0 0\",\"m=video 16384 RTP/AVP 96\",\"a=rtpmap:96 raw/90000\",\"a=fmtp:96 packetization-mode=1\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"]}}\n";

  BofJsonWriter BofJsonWriter;
  S_IpConnectParam_X.Reset();
  BofJsonParser BofJsonParser_O(JsonIn_S);

  JsonOut_S = "";
  Sts_E = BofJsonWriter.FromByte(true, S_IpConnectOptionCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), "{\"connect\":{\"receiver_id\":\"\"}}\n");

  Sts_E = BofJsonParser_O.ToByte(S_IpConnectOptionCollection, JsonWriteResultUltimateCheck, JsonWriteError);
  EXPECT_EQ(Sts_E, 0);

  JsonOut_S = "";
  Sts_E = BofJsonWriter.FromByte(true, S_IpConnectOptionCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);

  /*
  std::string t = "{\"connect\":{\"receiver_id\":\"1c513fc7-faa8-4a89-bd1c-56d8b00a1355\",\"sdp\":[{\"\":\"v=0\"},{\"\":\"o=- 0 0 IN IP4 18.52.86.0\"},{\"\":\"s=OUT1\"},{\"\":\"i=Player A\"},{\"\":\"c=IN IP4 33.67.101.0/32\"},{\"\":\"t=0 0\"},{\"\":\"m=video 16384 RTP/AVP 96\"},{\"\":\"a=rtpmap:96 raw/90000\"},{\"\":\"a=fmtp:96 packetization-mode=1\"}]}}\n";
  const char *p = JsonOut_S.c_str();
  const char *q = t.c_str();
  while (*p == *q) { p++; q++; }
  */

  EXPECT_STREQ(JsonOut_S.c_str(), JsonIn_S.c_str());

  Sts_E = BofJsonWriter.FromByte(false, S_IpConnectOptionCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
}

TEST(JsonWriter_Test, IpSwitcherSerDeser)
{
  BOFERR        Sts_E;
  std::string		JsonIn_S, JsonOut_S;
  BofJsonWriter JsonWriter;
  //S_AddDevice_X
  S_AddDevice_X.Reset();
  JsonIn_S = "{\"add_device\":{\"id\":\"a8500668-9218-4063-ba36-9b4900b82e67\",\"label\":\"\",\"node_id\":\"00000000-0000-0000-0000-000000000000\",\"receivers\":[],\"senders\":[],\"type\":\"\",\"version\":\"\"}}\n";
  BofJsonParser JsonParser(JsonIn_S);
  //goto l;


  EXPECT_TRUE(JsonParser.IsValid());
  EXPECT_STREQ(JsonParser.RootName().c_str(), "add_device");
  EXPECT_STREQ(JsonParser.RootName().c_str(), BofJsonParser::S_RootName(JsonIn_S).c_str());

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_AddDeviceSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), "{\"add_device\":{\"id\":\"00000000-0000-0000-0000-000000000000\",\"label\":\"\",\"node_id\":\"00000000-0000-0000-0000-000000000000\",\"type\":\"\",\"version\":\"\"}}\n");

  Sts_E = JsonParser.ToByte(S_AddDeviceSchemaCollection, JsonWriteResultUltimateCheck, JsonWriteError);
  EXPECT_EQ(Sts_E, 0);
  printf("Id   %s%s", S_AddDevice_X.Id.ToString(true).c_str(), Bof_Eol());
  printf("Node %s%s", S_AddDevice_X.NodeId.ToString(true).c_str(), Bof_Eol());

  //\"receivers\":[],\"senders\":[], are null
  JsonIn_S = "{\"add_device\":{\"id\":\"a8500668-9218-4063-ba36-9b4900b82e67\",\"label\":\"\",\"node_id\":\"00000000-0000-0000-0000-000000000000\",\"type\":\"\",\"version\":\"\"}}\n";
  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_AddDeviceSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), JsonIn_S.c_str());

  //S_AddSource_X
  S_AddSource_X.Reset();
  JsonIn_S = "{\"add_source\":{\"caps\":{},\"description\":\"OUT1\",\"device_id\":\"a8500668-9218-4063-ba36-9b4900b82e67\",\"format\":\"\",\"id\":\"895085f3-76a1-4f09-b3dd-2aabeb230cd8\",\"label\":\"Player A\",\"parents\":[],\"tags\":{\"\":[]},\"version\":\"\"}}\n";
  JsonParser = JsonIn_S;

  EXPECT_TRUE(JsonParser.IsValid());
  EXPECT_STREQ(JsonParser.RootName().c_str(), "add_source");
  EXPECT_STREQ(JsonParser.RootName().c_str(), BofJsonParser::S_RootName(JsonIn_S).c_str());

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_AddSourceSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), "{\"add_source\":{\"caps\":\"\",\"description\":\"\",\"device_id\":\"00000000-0000-0000-0000-000000000000\",\"format\":\"\",\"id\":\"00000000-0000-0000-0000-000000000000\",\"label\":\"\",\"version\":\"\"}}\n");

  Sts_E = JsonParser.ToByte(S_AddSourceSchemaCollection, JsonWriteResultUltimateCheck, JsonWriteError);
  EXPECT_EQ(Sts_E, 0);
  printf("Id     %s%s", S_AddSource_X.Id.ToString(true).c_str(), Bof_Eol());
  printf("Device %s%s", S_AddSource_X.DeviceId.ToString(true).c_str(), Bof_Eol());

  //caps:{} -> caps:""
  //\"parents\":[],\"tags\":{\"\":[]} are null
  JsonIn_S = "{\"add_source\":{\"caps\":\"\",\"description\":\"OUT1\",\"device_id\":\"a8500668-9218-4063-ba36-9b4900b82e67\",\"format\":\"\",\"id\":\"895085f3-76a1-4f09-b3dd-2aabeb230cd8\",\"label\":\"Player A\",\"version\":\"\"}}\n";
  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_AddSourceSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), JsonIn_S.c_str());

  //S_AddFlow_X
  S_AddFlow_X.Reset();
  JsonIn_S = "{\"add_flow\":{\"description\":\"OUT1\",\"format\":\"\",\"id\":\"40b94c9d-9bf1-4721-95ae-4316b4e080ea\",\"label\":\"Player A\",\"parents\":[],\"source_id\":\"895085f3-76a1-4f09-b3dd-2aabeb230cd8\",\"tags\":{\"\":[]},\"version\":\"\"}}\n";
  JsonParser = JsonIn_S;

  EXPECT_TRUE(JsonParser.IsValid());
  EXPECT_STREQ(JsonParser.RootName().c_str(), "add_flow");
  EXPECT_STREQ(JsonParser.RootName().c_str(), BofJsonParser::S_RootName(JsonIn_S).c_str());

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_AddFlowSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), "{\"add_flow\":{\"description\":\"\",\"format\":\"\",\"id\":\"00000000-0000-0000-0000-000000000000\",\"label\":\"\",\"source_id\":\"00000000-0000-0000-0000-000000000000\",\"version\":\"\"}}\n");

  Sts_E = JsonParser.ToByte(S_AddFlowSchemaCollection, JsonWriteResultUltimateCheck, JsonWriteError);
  EXPECT_EQ(Sts_E, 0);
  printf("Id       %s%s", S_AddFlow_X.Id.ToString(true).c_str(), Bof_Eol());
  printf("SourceId %s%s", S_AddFlow_X.SourceId.ToString(true).c_str(), Bof_Eol());

  //\"parents\":[],\"tags\":{\"\":[]} are null
  JsonIn_S = "{\"add_flow\":{\"description\":\"OUT1\",\"format\":\"\",\"id\":\"40b94c9d-9bf1-4721-95ae-4316b4e080ea\",\"label\":\"Player A\",\"source_id\":\"895085f3-76a1-4f09-b3dd-2aabeb230cd8\",\"version\":\"\"}}\n";
  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_AddFlowSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), JsonIn_S.c_str());

  //S_AddSender_X
  S_AddSender_X.Reset();
  JsonIn_S = "{\"add_sender\":{\"description\":\"OUT1\",\"device_id\":\"a8500668-9218-4063-ba36-9b4900b82e67\",\"flow_id\":\"40b94c9d-9bf1-4721-95ae-4316b4e080ea\",\"id\":\"edbe7239-8659-46c9-a4f7-85b46a2efc73\",\"label\":\"Player A\",\"manifest_href\":\"\",\"tags\":{\"\":[]},\"transport\":\"\",\"version\":\"\"}}\n";
  JsonParser = JsonIn_S;

  EXPECT_TRUE(JsonParser.IsValid());
  EXPECT_STREQ(JsonParser.RootName().c_str(), "add_sender");
  EXPECT_STREQ(JsonParser.RootName().c_str(), BofJsonParser::S_RootName(JsonIn_S).c_str());

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_AddSenderSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), "{\"add_sender\":{\"description\":\"\",\"device_id\":\"00000000-0000-0000-0000-000000000000\",\"flow_id\":\"00000000-0000-0000-0000-000000000000\",\"id\":\"00000000-0000-0000-0000-000000000000\",\"label\":\"\",\"manifest_href\":\"\",\"transport\":\"\",\"version\":\"\"}}\n");

  Sts_E = JsonParser.ToByte(S_AddSenderSchemaCollection, JsonWriteResultUltimateCheck, JsonWriteError);
  EXPECT_EQ(Sts_E, 0);
  printf("Id       %s%s", S_AddFlow_X.Id.ToString(true).c_str(), Bof_Eol());
  printf("SourceId %s%s", S_AddFlow_X.SourceId.ToString(true).c_str(), Bof_Eol());

  //"tags" is null
  JsonIn_S = "{\"add_sender\":{\"description\":\"OUT1\",\"device_id\":\"a8500668-9218-4063-ba36-9b4900b82e67\",\"flow_id\":\"40b94c9d-9bf1-4721-95ae-4316b4e080ea\",\"id\":\"edbe7239-8659-46c9-a4f7-85b46a2efc73\",\"label\":\"Player A\",\"manifest_href\":\"\",\"transport\":\"\",\"version\":\"\"}}\n";
  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_AddSenderSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), JsonIn_S.c_str());

  //S_AddReceiver_X
  S_AddReceiver_X.Reset();
  JsonIn_S = "{\"add_receiver\":{\"caps\":{},\"description\":\"IN1\",\"device_id\":\"a8500668-9218-4063-ba36-9b4900b82e67\",\"format\":\"\",\"id\":\"998a20b2-2a8a-42c3-a527-46d1de96a9df\",\"label\":\"Recorder A\",\"subscription\":{\"sender_id\":\"00000000-0000-0000-0000-000000000000\"},\"tags\":{\"\":[]},\"transport\":\"\",\"version\":\"\"}}\n";
  JsonParser = JsonIn_S;

  EXPECT_TRUE(JsonParser.IsValid());
  EXPECT_STREQ(JsonParser.RootName().c_str(), "add_receiver");
  EXPECT_STREQ(JsonParser.RootName().c_str(), BofJsonParser::S_RootName(JsonIn_S).c_str());

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_AddReceiverSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), "{\"add_receiver\":{\"caps\":\"\",\"description\":\"\",\"device_id\":\"00000000-0000-0000-0000-000000000000\",\"format\":\"\",\"id\":\"00000000-0000-0000-0000-000000000000\",\"label\":\"\",\"subscription\":{\"sender_id\":\"00000000-0000-0000-0000-000000000000\"},\"transport\":\"\",\"version\":\"\"}}\n");

  Sts_E = JsonParser.ToByte(S_AddReceiverSchemaCollection, JsonWriteResultUltimateCheck, JsonWriteError);
  EXPECT_EQ(Sts_E, 0);
  printf("Id       %s%s", S_AddFlow_X.Id.ToString(true).c_str(), Bof_Eol());
  printf("SourceId %s%s", S_AddFlow_X.SourceId.ToString(true).c_str(), Bof_Eol());

  //caps:{} -> caps:""
  //\"tags\":{\"\":[]} is null
  JsonIn_S = "{\"add_receiver\":{\"caps\":\"\",\"description\":\"IN1\",\"device_id\":\"a8500668-9218-4063-ba36-9b4900b82e67\",\"format\":\"\",\"id\":\"998a20b2-2a8a-42c3-a527-46d1de96a9df\",\"label\":\"Recorder A\",\"subscription\":{\"sender_id\":\"00000000-0000-0000-0000-000000000000\"},\"transport\":\"\",\"version\":\"\"}}\n";

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_AddReceiverSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), JsonIn_S.c_str());

  //S_AddManifest_X

  S_AddManifest_X.Reset();
  JsonIn_S = "{\"add_manifest\":{\"sdp\":[\"v=0\",\"o=- 0 0 IN IP4 18.52.86.0\",\"s=OUT1\",\"i=Player A\",\"c=IN IP4 33.67.101.0/32\",\"t=0 0\",\"m=video 16384 RTP/AVP 96\",\"a=rtpmap:96 raw/90000\",\"a=fmtp:96 packetization-mode=1\"],\"sender_id\":\"edbe7239-8659-46c9-a4f7-85b46a2efc73\"}}\n";
  JsonParser = JsonIn_S;

  EXPECT_TRUE(JsonParser.IsValid());
  EXPECT_STREQ(JsonParser.RootName().c_str(), "add_manifest");
  EXPECT_STREQ(JsonParser.RootName().c_str(), BofJsonParser::S_RootName(JsonIn_S).c_str());

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_AddManifestSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), "{\"add_manifest\":{\"sender_id\":\"00000000-0000-0000-0000-000000000000\"}}\n");

  Sts_E = JsonParser.ToByte(S_AddManifestSchemaCollection, JsonWriteResultUltimateCheck, JsonWriteError);
  EXPECT_EQ(Sts_E, 0);
  printf("SenderId %s%s", S_AddManifest_X.SenderId.ToString(true).c_str(), Bof_Eol());

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_AddManifestSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), JsonIn_S.c_str());

  //S_CmdConnect_X
  S_CmdConnect_X.Reset();
  JsonIn_S = "{\"connect\":{\"receiver_id\":\"1c513fc7-faa8-4a89-bd1c-56d8b00a1355\",\"sdp\":[\"v=0\",\"o=- 0 0 IN IP4 18.52.86.0\",\"s=OUT1\",\"i=Player A\",\"c=IN IP4 33.67.101.0/32\",\"t=0 0\",\"m=video 16384 RTP/AVP 96\",\"a=rtpmap:96 raw/90000\",\"a=fmtp:96 packetization-mode=1\"]}}\n";
  JsonParser = JsonIn_S;

  EXPECT_TRUE(JsonParser.IsValid());
  EXPECT_STREQ(JsonParser.RootName().c_str(), "connect");
  EXPECT_STREQ(JsonParser.RootName().c_str(), BofJsonParser::S_RootName(JsonIn_S).c_str());

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_CmdConnectSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), "{\"connect\":{\"receiver_id\":\"00000000-0000-0000-0000-000000000000\"}}\n");

  Sts_E = JsonParser.ToByte(S_CmdConnectSchemaCollection, JsonWriteResultUltimateCheck, JsonWriteError);
  EXPECT_EQ(Sts_E, 0);
  printf("ReceiverId %s%s", S_CmdConnect_X.ReceiverId.ToString(true).c_str(), Bof_Eol());

  //caps:{} -> caps:""
  //\"tags\":{\"\":[]} is null
  JsonIn_S = "{\"connect\":{\"receiver_id\":\"1c513fc7-faa8-4a89-bd1c-56d8b00a1355\",\"sdp\":[\"v=0\",\"o=- 0 0 IN IP4 18.52.86.0\",\"s=OUT1\",\"i=Player A\",\"c=IN IP4 33.67.101.0/32\",\"t=0 0\",\"m=video 16384 RTP/AVP 96\",\"a=rtpmap:96 raw/90000\",\"a=fmtp:96 packetization-mode=1\"]}}\n";
  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_CmdConnectSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), JsonIn_S.c_str());


  //S_CmdStatusReceiver_X
  S_CmdStatusReceiver_X.Reset();
  JsonIn_S = "{\"status_receivers\":{\"arg\":\"\"}}\n";
  JsonParser = JsonIn_S;

  EXPECT_TRUE(JsonParser.IsValid());
  EXPECT_STREQ(JsonParser.RootName().c_str(), "status_receivers");
  EXPECT_STREQ(JsonParser.RootName().c_str(), BofJsonParser::S_RootName(JsonIn_S).c_str());

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_CmdStatusReceiverSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), JsonIn_S.c_str());

  Sts_E = JsonParser.ToByte(S_CmdStatusReceiverSchemaCollection, JsonWriteResultUltimateCheck, JsonWriteError);
  EXPECT_EQ(Sts_E, 0);

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_CmdStatusReceiverSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), JsonIn_S.c_str());

  //S_ReplyStatusReceiver_X
  S_ReplyStatusReceiver_X.Reset();

  JsonIn_S = "{\"status_receivers\":[{\"id\":\"1c513fc7-faa8-4a89-bd1c-56d8b00a1355\",\"impaired\":\"true\",\"ip\":\"18.52.86.0\",\"present\":\"both\",\"state\":\"AB\"},{\"id\":\"1c513fc7-faa8-4a89-bd1c-56d8b00adead\",\"impaired\":\"false\",\"ip\":\"18.52.86.2\",\"present\":\"primary\",\"state\":\"A\"}]}\n";
  JsonParser = JsonIn_S;

  EXPECT_TRUE(JsonParser.IsValid());
  EXPECT_STREQ(JsonParser.RootName().c_str(), "status_receivers");
  EXPECT_STREQ(JsonParser.RootName().c_str(), BofJsonParser::S_RootName(JsonIn_S).c_str());

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_ReplyStatusReceiverSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), "{}\n");

  Sts_E = JsonParser.ToByte(S_ReplyStatusReceiverSchemaCollection, JsonWriteResultUltimateCheck, JsonWriteError);
  EXPECT_EQ(Sts_E, 0);
  for (auto Item : S_ReplyStatusReceiver_X.IdCollection) printf("Sts Rcv Id %s%s", Item.ToString(true).c_str(), Bof_Eol());
  for (auto Item : S_ReplyStatusReceiver_X.IpCollection) printf("Sts Rcv Ip %s%s", Bof_SocketAddressToString(Item, false, true).c_str(), Bof_Eol());
  for (auto Item : S_ReplyStatusReceiver_X.PresentCollection) printf("Sts Rcv Present %s%s", Item.c_str(), Bof_Eol());
  for (auto Item : S_ReplyStatusReceiver_X.ImpairedCollection) printf("Sts Rcv Impaired %s%s", Item ? "True" : "False", Bof_Eol());
  for (auto Item : S_ReplyStatusReceiver_X.StateCollection) printf("Sts Rcv State %s%s", Item.c_str(), Bof_Eol());

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_ReplyStatusReceiverSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), JsonIn_S.c_str());

  //S_CmdStatusSender_X
  S_CmdStatusReceiver_X.Reset();
  JsonIn_S = "{\"status_senders\":{\"arg\":\"\"}}\n";

  JsonParser = JsonIn_S;

  EXPECT_TRUE(JsonParser.IsValid());
  EXPECT_STREQ(JsonParser.RootName().c_str(), "status_senders");
  EXPECT_STREQ(JsonParser.RootName().c_str(), BofJsonParser::S_RootName(JsonIn_S).c_str());

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_CmdStatusSenderSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), JsonIn_S.c_str());

  Sts_E = JsonParser.ToByte(S_CmdStatusSenderSchemaCollection, JsonWriteResultUltimateCheck, JsonWriteError);
  EXPECT_EQ(Sts_E, 0);

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_CmdStatusSenderSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), JsonIn_S.c_str());


  //S_ReplyStatusSender_X
//	l:
  S_ReplyStatusSender_X.Reset();

  JsonIn_S = "{\"status_senders\":[{\"id\":\"1c513fc7-faa8-4a89-bd1c-56d8b00a1356\",\"impaired\":\"true\",\"ip\":\"18.52.86.0:1\",\"present\":\"lost\",\"state\":\"B\"},{\"id\":\"1c513fc7-faa8-4a89-bd1c-56d8b00abeef\",\"impaired\":\"true\",\"ip\":\"18.52.86.222:235\",\"present\":\"primary\",\"state\":\"XA\"}]}\n";
  JsonParser = JsonIn_S;

  EXPECT_TRUE(JsonParser.IsValid());
  EXPECT_STREQ(JsonParser.RootName().c_str(), "status_senders");
  EXPECT_STREQ(JsonParser.RootName().c_str(), BofJsonParser::S_RootName(JsonIn_S).c_str());

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_ReplyStatusSenderSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), "{}\n");

  Sts_E = JsonParser.ToByte(S_ReplyStatusSenderSchemaCollection, JsonWriteResultUltimateCheck, JsonWriteError);
  EXPECT_EQ(Sts_E, 0);
  for (auto Item : S_ReplyStatusSender_X.IdCollection) printf("Sts Snd Id %s%s", Item.ToString(true).c_str(), Bof_Eol());
  for (auto Item : S_ReplyStatusSender_X.IpCollection) printf("Sts Snd Ip %s%s", Bof_SocketAddressToString(Item, false, true).c_str(), Bof_Eol());
  for (auto Item : S_ReplyStatusSender_X.PresentCollection) printf("Sts Snd Present %s%s", Item.c_str(), Bof_Eol());
  for (auto Item : S_ReplyStatusSender_X.ImpairedCollection) printf("Sts Snd Impaired %s%s", Item ? "True" : "False", Bof_Eol());
  for (auto Item : S_ReplyStatusSender_X.StateCollection) printf("Sts Snd State %s%s", Item.c_str(), Bof_Eol());

  JsonOut_S = "";
  Sts_E = JsonWriter.FromByte(true, S_ReplyStatusSenderSchemaCollection, JsonOut_S);
  EXPECT_EQ(Sts_E, 0);
  EXPECT_STREQ(JsonOut_S.c_str(), JsonIn_S.c_str());
}

#if 0
TEST(JsonParser_Test, JsonCfg)
{
  BofJsonParser *pBofJsonParser_O;
  int           Sts_i;
  BofPath CrtDir, Path;
  std::string JsonData_S, JsonOut_S;
  BofJsonWriter BofJsonWriter_O;
  std::string Cfg_S;
  /*
    printf("sz MMGW_API_STATE %d MMGW_BOARD_STATE %d MMGW_BOARD_CHANNEL %d BOF_SOCKET_ADDRESS %d\n",sizeof(MMGW_API_STATE), sizeof(MMGW_BOARD_STATE),sizeof(MMGW_BOARD_CHANNEL),sizeof(BOF_SOCKET_ADDRESS));
    printf("ONE O %p 1 %p 2 %p\n",  &S_MmgwApiState_X.pBoardState_X[0].pHrInput_X[0].IoActive_B,&S_MmgwApiState_X.pBoardState_X[0].pHrInput_X[1].IoActive_B,
          &S_MmgwApiState_X.pBoardState_X[0].pHrInput_X[2].IoActive_B,&S_MmgwApiState_X.pBoardState_X[0].pHrInput_X[3].IoActive_B);
    printf("TWO O %p 1 %p 2 %p\n",  &S_MmgwApiState_X.pBoardState_X[1].pHrInput_X[0].IoActive_B,&S_MmgwApiState_X.pBoardState_X[1].pHrInput_X[1].IoActive_B,
           &S_MmgwApiState_X.pBoardState_X[1].pHrInput_X[2].IoActive_B,&S_MmgwApiState_X.pBoardState_X[1].pHrInput_X[3].IoActive_B);

  */
  S_MmgwApiState_X.Reset();
  Bof_GetCurrentDirectory(CrtDir);
  Path = CrtDir + "jsonparser.json";
  EXPECT_EQ(Bof_ReadFile(Path, JsonData_S), BOF_ERR_NO_ERROR);

  pBofJsonParser_O = new BofJsonParser(JsonData_S);
  EXPECT_TRUE(pBofJsonParser_O != nullptr);
  Sts_i = pBofJsonParser_O->ToByte(S_CfgJsonCollection, JsonParseResultUltimateCheck, JsonParseError);
  EXPECT_EQ(Sts_i, 0);

  DisplayConfig();


  //	EXPECT_EQ(BofJsonWriter_O.FromByte(false, S_CfgJsonCollection, JsonOut_S), BOF_ERR_NO_ERROR);
  //printf("%s\n",JsonOut_S.c_str());

  BOF_SAFE_DELETE(pBofJsonParser_O);
}
#endif