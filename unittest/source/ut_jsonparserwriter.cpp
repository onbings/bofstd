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

/*** Global variables ********************************************************/

/*** Factory functions called at the beginning/end of each test case **********/

/*** Test case ******************************************************************/

//BOFERR JsonParseResultUltimateCheck(uint32_t /*_Index_U32*/, const BOFPARAMETER & /*_rBofCommandlineOption_X*/, const bool _CheckOk_B, const char * /*_pOptNewVal_c*/)
BOFERR JsonParseResultUltimateCheck(uint32_t /*_Index_U32*/, const BOFPARAMETER & _rBofCommandlineOption_X, const bool _CheckOk_B, const char *_pOptNewVal_c)
{
  BOFERR Rts_E = _CheckOk_B ? BOF_ERR_NO_ERROR:BOF_ERR_NO;

	printf("Check is '%s'\r\n", _CheckOk_B ? "TRUE" : "FALSE");
	printf("Op pUser %p Name %s Tp %d OldVal %p NewVal %s\r\n", _rBofCommandlineOption_X.pUser, _rBofCommandlineOption_X.Name_S.c_str(), static_cast<uint32_t>(_rBofCommandlineOption_X.ArgType_E), _rBofCommandlineOption_X.pValue, _pOptNewVal_c ? _pOptNewVal_c : "nullptr");

	return Rts_E;
}

//bool JsonParseError(int /*_Sts_i*/, const BOFPARAMETER & /*_rJsonEntry_X*/, const char * /*_pValue*/)
bool JsonParseError(int _Sts_i, const BOFPARAMETER & _rJsonEntry_X, const char *_pValue)
{
	bool Rts_B = true;

	printf("JSON error %d on entry pUser %p value %s\r\n", _Sts_i, _rJsonEntry_X.pUser, _pValue ? _pValue : "nullptr");
	return Rts_B;
}

BOFERR JsonWriteResultUltimateCheck(uint32_t /*_Index_U32*/, const BOFPARAMETER & /*_rBofCommandlineOption_X*/, const bool _CheckOk_B, const char * /*_pOptNewVal_c*/)
{
  BOFERR Rts_E = _CheckOk_B ? BOF_ERR_NO_ERROR:BOF_ERR_NO;

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
static std::vector< BOF_NAMESPACE::BOFPARAMETER > S_IpManifestOptionCollection =
{
	{ nullptr,  "",                 "",              "",         "add_manifest.sdp",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_ARRAY(S_IpManifestParam_X.pSdpLine_S,   STDSTRING,                  0,              0) },
	{ nullptr,  "sender_id",            "",              "",         "add_manifest",                    BOFPARAMETER_ARG_FLAG::NONE,          BOF_PARAM_DEF_VARIABLE(S_IpManifestParam_X.SenderId_S,               STDSTRING,                  0,              0) },
};
static IPCONNECT                   S_IpConnectParam_X;
static std::vector< BOF_NAMESPACE::BOFPARAMETER > S_IpConnectOptionCollection =
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
	const char    *pValue_c;
	int           Sts_i;
	BofPath CrtDir, Path;
	std::string JsonData_S;


	std::string JsonIn_S = "        {    \r\n \"add_source\"    \t :\t\r{kjlkjljl";
	EXPECT_STREQ("add_source", BofJsonParser::S_RootName(JsonIn_S).c_str());



	S_AppParamJson_X.Reset();

	Bof_GetCurrentDirectory(CrtDir);

	Path = CrtDir + "jsonparser.json";
	EXPECT_EQ(Bof_ReadFile(Path, JsonData_S), BOF_ERR_NO_ERROR);

	pBofJsonParser_O = new BofJsonParser(JsonData_S);
	EXPECT_TRUE(pBofJsonParser_O != nullptr);
	Sts_i            = pBofJsonParser_O->ToByte(S_OptionJsonCollection, JsonParseResultUltimateCheck, JsonParseError);
	EXPECT_EQ(Sts_i, 0);


// Check S_AppParam_X content
	pValue_c         = pBofJsonParser_O->GetFirstElementFromOid("MulFtpUserSetting.DeployIpAddress");
	EXPECT_TRUE(pValue_c != nullptr);
	pValue_c         = pBofJsonParser_O->GetNextElementFromOid();
	EXPECT_TRUE(pValue_c == nullptr);

	pValue_c         = pBofJsonParser_O->GetFirstElementFromOid("MulFtpUserSetting.catalog.book.id");
	EXPECT_TRUE(pValue_c != nullptr);
	pValue_c         = pBofJsonParser_O->GetNextElementFromOid();
	EXPECT_TRUE(pValue_c != nullptr);

	BOF_SAFE_DELETE(pBofJsonParser_O);
}


TEST(JsonParser_Test, IpSenderDeser)
{
	const char    *pValue_c;
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
	const char    *pValue_c;
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
	const char    *pValue_c;
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

	Bof_GetCurrentDirectory(CrtDir);

	//memset(&S_AppParam_X, 0, sizeof(S_AppParam_X) );

	Path = CrtDir + "jsonparser.json";
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
	std::string JsonIn_S="{\"add_sender\":{\"description\":\"OUT1\",\"device_id\":\"f98f5eaa-cda5-4396-b975-407748ba513b\",\"flow_id\":\"a3fc4d73-6fdc-4221-ae10-e1b6e006ac5c\",\"id\":\"a8a0b68c-0fbd-4b94-ad95-f637d4c4c6a3\",\"label\":\"Player A\",\"manifest_href\":\"http:/bha.txt\",\"tags\":{\"\":[\"1\",\"2\",\"3\",\"\",\"\",\"\",\"\",\"\"]},\"transport\":\"\",\"version\":\"\"}}\n";
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
	std::string JsonIn_S="{\"add_manifest\":{\"sdp\":[\"v=0\",\"o=- 0 0 IN IP4 18.52.86.2\",\"s=OUT3\",\"i=Player C\",\"c=IN IP4 33.67.101.2/32\",\"t=0 0\",\"m=video 16386 RTP/AVP 96\",\"a=rtpmap:96 raw/90000\",\"a=fmtp:96 packetization-mode=1\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"],\"sender_id\":\"81f4da21-c9ef-4b90-ab03-a4893cae32ed\"}}\n";

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
	std::string JsonIn_S="{\"connect\":{\"receiver_id\":\"1c513fc7-faa8-4a89-bd1c-56d8b00a1355\",\"sdp\":[\"v=0\",\"o=- 0 0 IN IP4 18.52.86.0\",\"s=OUT1\",\"i=Player A\",\"c=IN IP4 33.67.101.0/32\",\"t=0 0\",\"m=video 16384 RTP/AVP 96\",\"a=rtpmap:96 raw/90000\",\"a=fmtp:96 packetization-mode=1\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"]}}\n";

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
	printf("Id   %s%s", S_AddDevice_X.Id.ToString(true).c_str(),Bof_Eol());
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
	JsonParser=JsonIn_S;

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
	for (auto Item : S_ReplyStatusReceiver_X.ImpairedCollection) printf("Sts Rcv Impaired %s%s", Item ? "True":"False", Bof_Eol());
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


typedef void *MMGW_API_HANDLE;
typedef void *demux_t;
typedef enum
{
	MMGWDRV_BOARD_TYPE_SDI_3G = 0,
	MMGWDRV_BOARD_TYPE_SDI_12G,
	MMGWDRV_BOARD_TYPE_IP,
	MMGWDRV_BOARD_TYPE_MAX,
	MMGWDRV_BOARD_TYPE_UNKNOWN
} MMGWDRV_BOARD_TYPE;
#define MMGWDRV_MAX_NUMBER_OF_DEVICE 4
#define MMGWDRV_MAX_NUMBER_OF_INPUT                 8
#define MMGWDRV_MAX_NUMBER_OF_OUTPUT                4
#define MMGWDRV_MAX_AUDIO_CHANNEL_PER_PAGE 16 //Total audio channel size is MMGWDRV_MAX_AUDIO_CHANNEL_PER_PAGE * MMGWDRV_AUDIO_CHANNEL_BUFFER_SIZE = 64KB


struct VLC_BOARD
{
	pthread_t thread;
	int evfd;
	VLC_BOARD()
	{
		Reset();
	}
	void Reset()
	{
		thread=-1;
		evfd=-1;
	}
};
struct VLC_CHANNEL
{
	/* picture decoding */
//	mtime_t      i_next_vdate, i_next_adate;
	int          i_incr, i_aincr;
	/* ES stuff */
	//es_out_id_t *p_es_video;
	//es_out_id_t *p_es[MMGWDRV_MAX_AUDIO_CHANNEL_PER_PAGE];
#if defined(METATITLE)
	unsigned int i_title;
	unsigned int i_longest_title;
	input_title_t **pp_title;

	int i_attachments;
	input_attachment_t **attachments;
	int i_cover_idx;
	float f_fps;
	const META_DL *p_meta;
/* Used to store bluray disc path */
	char *psz_bd_path;
#endif
	VLC_CHANNEL()
	{
		Reset();
	}
	void Reset()
	{
		uint32_t i_U32;

//		i_next_vdate=0;
//		i_next_adate=0;
		i_incr=0;
		i_aincr=0;
	//	p_es_video=nullptr;
		for (i_U32=0;i_U32<MMGWDRV_MAX_AUDIO_CHANNEL_PER_PAGE;i_U32++)
		{
		//	p_es[i_U32]=nullptr;
		}
	}
};

struct MMGW_VLC_STREAM_PARAM;
struct MMGW_BOARD_CHANNEL
{
	//Not exported in .json cfg file
	VLC_CHANNEL VlcChannel_X;
	uint32_t Index_U32;
	bool Input_B;
	//MMGW_CHANNEL_HANDLE     *ppChannelHandle_X[MMGWDRV_STREAM_CHANNEL_TYPE_MAX];
	//MMGW_STREAM_HANDLE      *pStreamHandle_X;
	MMGW_VLC_STREAM_PARAM		*pMmgwVlcStreamParam_X;

//Exported in .json cfg file
	bool IoActive_B;
	char pIoName_c[32];
	BOF_NAMESPACE::BofVideoStandard				VideoStandard;
	BOF_NAMESPACE::BofAudioStandard				AudioStandard;
//Sdi specific

//IP specific
	BOF_NAMESPACE::BOF_SOCKET_ADDRESS    VideoIpAddress_X;
	BOF_NAMESPACE::BOF_SOCKET_ADDRESS    pAudioIpAddress_X[MMGWDRV_MAX_AUDIO_CHANNEL_PER_PAGE];
	BOF_NAMESPACE::BOF_SOCKET_ADDRESS    AncIpAddress_X;

	uint32_t 	Bit_U32;
	BOF_NAMESPACE::BOF_SIZE  ProxyRes_X;

	MMGW_BOARD_CHANNEL()
	{
		Reset();
	}
	void Reset()
	{
		uint32_t i_U32;

		VlcChannel_X.Reset();
		Index_U32=0;
		Input_B = false;
		/*
		for (i_U32=0;i_U32<MMGWDRV_STREAM_CHANNEL_TYPE_MAX;i_U32++)
		{
			ppChannelHandle_X[i_U32] = nullptr;
		}
		pStreamHandle_X=nullptr;
		 */
		pMmgwVlcStreamParam_X=nullptr;
		IoActive_B=false;
		pIoName_c[0]=0;
		VideoIpAddress_X.Reset();
		VideoIpAddress_X.Reset();
		for (i_U32=0;i_U32<MMGWDRV_MAX_AUDIO_CHANNEL_PER_PAGE;i_U32++)
		{
			pAudioIpAddress_X[i_U32].Reset();
		}
		AncIpAddress_X.Reset();
		Bit_U32=0;
		ProxyRes_X.Reset();
	}
};

struct MMGW_BOARD_STATE
{
//Not exported in .json cfg file
	VLC_BOARD		VlcBoard_X;
	uint32_t Index_U32;
/*
	MMGW_BOARD_HANDLE        *pBoardHandle_X;
	MMGW_BOARD_STATIC_INFO   BoardStaticInfo_X;
	MMGW_PTP_INFO            PtpInfo_X;
*/
//Exported in .json cfg file
	bool BrdActive_B;
	char pBrdName_c[32];
	BOF_NAMESPACE::BofVideoStandard				VideoStandard;
	MMGWDRV_BOARD_TYPE  BoardType_E;		//Used as it by simulator OR overwritten by Mmgw_OpenBoard

//Sdi specific

//IP specific
	BOF_NAMESPACE::BOF_SOCKET_ADDRESS    PtpIpAddress_X;
	BOF_NAMESPACE::BOF_SOCKET_ADDRESS    PtpIpMask_X;
	BOF_NAMESPACE::BOF_SOCKET_ADDRESS    PtpIpGw_X;
	uint8_t															 PtpDomain_U8;

	MMGW_BOARD_CHANNEL pHrInput_X[MMGWDRV_MAX_NUMBER_OF_INPUT];
	MMGW_BOARD_CHANNEL pHrOutput_X[MMGWDRV_MAX_NUMBER_OF_OUTPUT];
	MMGW_BOARD_STATE()
	{
		Reset();
	}
	void Reset()
	{
		uint32_t i_U32;
		VlcBoard_X.Reset();
		Index_U32=0;

		BoardType_E=MMGWDRV_BOARD_TYPE::MMGWDRV_BOARD_TYPE_UNKNOWN;
		PtpIpAddress_X.Reset();
		PtpIpMask_X.Reset();
		PtpIpGw_X.Reset();
		PtpDomain_U8=0;
		for (i_U32=0;i_U32<MMGWDRV_MAX_NUMBER_OF_INPUT;i_U32++)
		{
			pHrInput_X[i_U32].Reset();
		}
		for (i_U32=0;i_U32<MMGWDRV_MAX_NUMBER_OF_OUTPUT;i_U32++)
		{
			pHrOutput_X[i_U32].Reset();
		}

	}
};
struct MMGW_API_STATE
{
//Not exported in .json cfg file
	bool 						SimulatorOn_B;
	bool 						LogOn_B;
	MMGW_API_HANDLE *pApiHandle_X;
	char            pVersion_c[0x100];
	uint32_t NbBoard_U32;

//Exported in .json cfg file
	BOF_NAMESPACE::BofVideoStandard	VideoStandard;
	MMGW_BOARD_STATE  pBoardState_X[MMGWDRV_MAX_NUMBER_OF_DEVICE];


	MMGW_API_STATE()
	{
		Reset();
	}
	void Reset()
	{
		uint32_t i_U32;
		SimulatorOn_B=false;
		LogOn_B=false;
		pApiHandle_X=nullptr;
		pVersion_c[0]=0;
		NbBoard_U32=0;
		for (i_U32=0;i_U32<MMGWDRV_MAX_NUMBER_OF_DEVICE;i_U32++)
		{
			pBoardState_X[i_U32].Reset();
		}
	}
};


struct demux_sys_t
{
	MMGW_API_STATE *pMmgwApiState_X;


#if 0
	/* video device reader */
	 int          i_vfd;
	 unsigned int i_link;
	 unsigned int i_standard;
#ifdef HAVE_MMAP_SDIVIDEO
	 uint8_t      **pp_vbuffers;
    unsigned int i_vbuffers, i_current_vbuffer;
#endif
	 unsigned int i_vbuffer_size;

	 /* audio device reader */
	 int          i_afd;
	 int          i_max_channel;
	 unsigned int i_sample_rate;
#ifdef HAVE_MMAP_SDIAUDIO
	 uint8_t      **pp_abuffers;
    unsigned int i_abuffers, i_current_abuffer;
#endif
	 unsigned int i_abuffer_size;

	 /* picture decoding */
	 unsigned int i_frame_rate, i_frame_rate_base;
	 unsigned int i_width, i_height, i_aspect, i_forced_aspect;
	 unsigned int i_vblock_size, i_ablock_size;
	 mtime_t      i_next_vdate, i_next_adate;
	 int          i_incr, i_aincr;

	 /* ES stuff */
	 int          i_id_video;
	 es_out_id_t  *p_es_video;
	 hdsdi_audio_t p_audios[MAX_AUDIOS];

	 pthread_t thread;
	 int evfd;

	 unsigned int        i_title;
	 unsigned int        i_longest_title;
	 input_title_t       **pp_title;

	 int                 i_attachments;
	 input_attachment_t  **attachments;
	 int                 i_cover_idx;
	 float f_fps;
	 const META_DL       *p_meta;

	 /* Used to store bluray disc path */
	 char                *psz_bd_path;
#endif
};

struct MMGW_VLC_STREAM_PARAM
{
	demux_t *pDemux_X;
	demux_sys_t *pDemuxSys_X;
	MMGW_API_STATE *pMmgwApiState_X;
	MMGW_BOARD_STATE  *pBoardState_X;
	MMGW_BOARD_CHANNEL *pBoardChannel_X;
	VLC_CHANNEL *pVlcChannel_X;

	MMGW_VLC_STREAM_PARAM()
	{
		pDemux_X=nullptr;
		pDemuxSys_X=nullptr;
		pMmgwApiState_X=nullptr;
		pBoardState_X=nullptr;
		pBoardChannel_X=nullptr;
		pVlcChannel_X=nullptr;
	}
	MMGW_VLC_STREAM_PARAM(uint32_t _BoardIndex_U32, bool _Input_B, uint32_t _Channel_U32, demux_t *_pDemux_X)
	{
		Reset(_BoardIndex_U32, _Input_B, _Channel_U32, _pDemux_X);
	}
	void Reset(uint32_t _BoardIndex_U32, bool _Input_B, uint32_t _Channel_U32, demux_t *_pDemux_X)
	{
		pDemux_X=_pDemux_X;
		BOF_ASSERT(pDemux_X);
//		pDemuxSys_X=pDemux_X->p_sys;
//		BOF_ASSERT(pDemuxSys_X);
		pMmgwApiState_X=pDemuxSys_X->pMmgwApiState_X;
		BOF_ASSERT(pMmgwApiState_X);
		BOF_ASSERT(_BoardIndex_U32 < MMGWDRV_MAX_NUMBER_OF_DEVICE);
		pBoardState_X=&pMmgwApiState_X->pBoardState_X[_BoardIndex_U32];
		BOF_ASSERT(pBoardState_X);
		if (_Input_B)
		{
			BOF_ASSERT(_Channel_U32 < MMGWDRV_MAX_NUMBER_OF_INPUT);
			pBoardChannel_X = &pBoardState_X->pHrInput_X[_Channel_U32];
			BOF_ASSERT(pBoardChannel_X);
		}
		else
		{
			BOF_ASSERT(_Channel_U32 < MMGWDRV_MAX_NUMBER_OF_OUTPUT);
			pBoardChannel_X = &pBoardState_X->pHrOutput_X[_Channel_U32];
			BOF_ASSERT(pBoardChannel_X);
		}
		pVlcChannel_X=&pBoardChannel_X->VlcChannel_X;
		BOF_ASSERT(pVlcChannel_X);
	}
};

static MMGW_API_STATE S_MmgwApiState_X;
static std::vector< BOF_NAMESPACE::BOFPARAMETER > S_CfgJsonCollection2 =
				{
								{nullptr, "AudioStandard", "AudioStandard", "", "MmgwSetting.Board.InHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
								 BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrInput_X, AudioStandard, AUDIOSTANDARD, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL, 0)},
				};
static std::vector< BOF_NAMESPACE::BOFPARAMETER > S_CfgJsonCollection =
				{
//MmgwSetting
								{ nullptr, "VideoStandard", "VideoStandard", "", "MmgwSetting", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_VARIABLE(S_MmgwApiState_X.VideoStandard, VIDEOSTANDARD, 0, 0 ) },

//MmgwSetting.Board
								{ nullptr, "Active", "Active", "", "MmgwSetting.Board", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_ARRAY_OF_STRUCT(MMGW_BOARD_STATE, S_MmgwApiState_X.pBoardState_X, BrdActive_B, BOOL, 0, 0) },
								{ nullptr, "Name", "Name", "", "MmgwSetting.Board", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_ARRAY_OF_STRUCT(MMGW_BOARD_STATE, S_MmgwApiState_X.pBoardState_X, pBrdName_c, CHARSTRING, 0, sizeof(S_MmgwApiState_X.pBoardState_X[0].pBrdName_c)-1) },

								{ nullptr, "BoardType", "BoardType", "", "MmgwSetting.Board", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_ARRAY_OF_STRUCT(MMGW_BOARD_STATE, S_MmgwApiState_X.pBoardState_X, BoardType_E, UINT32, MMGWDRV_BOARD_TYPE_SDI_3G, MMGWDRV_BOARD_TYPE_UNKNOWN) },

								{ nullptr, "VideoStandard", "VideoStandard", "", "MmgwSetting.Board", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X, VideoStandard, VIDEOSTANDARD, 0, 0) },

								{ nullptr, "PtpIpAddress", "PtpIpAddress", "", "MmgwSetting.Board", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PROTOCOL | BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PORT,
												BOF_PARAM_DEF_ARRAY_OF_STRUCT(MMGW_BOARD_STATE, S_MmgwApiState_X.pBoardState_X, PtpIpAddress_X, IPV4, 0, 0) },

								{ nullptr, "PtpIpMask", "PtpIpMask", "", "MmgwSetting.Board", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PROTOCOL | BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PORT,
												BOF_PARAM_DEF_ARRAY_OF_STRUCT(MMGW_BOARD_STATE, S_MmgwApiState_X.pBoardState_X, PtpIpMask_X, IPV4, 0, 0) },

								{ nullptr, "PtpIpGw", "PtpIpGw", "", "MmgwSetting.Board", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PROTOCOL | BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PORT,
												BOF_PARAM_DEF_ARRAY_OF_STRUCT(MMGW_BOARD_STATE, S_MmgwApiState_X.pBoardState_X, PtpIpGw_X, IPV4, 0, 0) },

								{ nullptr, "PtpDomain", "PtpDomain", "", "MmgwSetting.Board", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_ARRAY_OF_STRUCT(MMGW_BOARD_STATE, S_MmgwApiState_X.pBoardState_X, PtpDomain_U8, UINT8, 0, 127) },

//MmgwSetting.Board.InHr
								{ nullptr, "Active", "Active", "", "MmgwSetting.Board.InHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrInput_X, IoActive_B, BOOL, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },
								{ nullptr, "Name", "Name", "", "MmgwSetting.Board.InHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrInput_X, pIoName_c, CHARSTRING, 0, sizeof(S_MmgwApiState_X.pBoardState_X[0].pHrInput_X[0].pIoName_c)-1, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },

								{ nullptr, "VideoStandard", "VideoStandard", "", "MmgwSetting.Board.InHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrInput_X, VideoStandard, VIDEOSTANDARD, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },
								{ nullptr, "VideoIpAddress", "VideoIpAddress", "", "MmgwSetting.Board.InHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PROTOCOL | BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PORT,
												BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrInput_X, VideoIpAddress_X, IPV4, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },

								{ nullptr, "AudioStandard", "AudioStandard", "", "MmgwSetting.Board.InHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrInput_X, AudioStandard, AUDIOSTANDARD, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },
								{ nullptr, "", "", "", "MmgwSetting.Board.InHr.AudioIpAddress", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PROTOCOL | BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PORT,
												BOF_PARAM_DEF_MULTI_ARRAY(S_MmgwApiState_X.pBoardState_X[0].pHrInput_X[0].pAudioIpAddress_X, IPV4, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },

								{ nullptr, "AncIpAddress", "AncIpAddress", "", "MmgwSetting.Board.InHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PROTOCOL | BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PORT,
												BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrInput_X, AncIpAddress_X, IPV4, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },
								{ nullptr, "Bit", "Bit", "", "MmgwSetting.Board.InHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrInput_X, Bit_U32, UINT32, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },
								{ nullptr, "ProxyRes", "ProxyRes", "", "MmgwSetting.Board.InHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrInput_X, ProxyRes_X, SIZE2D, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },

//MmgwSetting.Board.OutHr
								{ nullptr, "Active", "Active", "", "MmgwSetting.Board.OutHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrOutput_X, IoActive_B, BOOL, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },
								{ nullptr, "Name", "Name", "", "MmgwSetting.Board.OutHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrOutput_X, pIoName_c, CHARSTRING, 0, sizeof(S_MmgwApiState_X.pBoardState_X[0].pHrOutput_X[0].pIoName_c)-1, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },

								{ nullptr, "VideoStandard", "VideoStandard", "", "MmgwSetting.Board.OutHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrOutput_X, VideoStandard, VIDEOSTANDARD, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },
								{ nullptr, "VideoIpAddress", "VideoIpAddress", "", "MmgwSetting.Board.OutHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PROTOCOL | BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PORT,
												BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrOutput_X, VideoIpAddress_X, IPV4, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },

								{ nullptr, "AudioStandard", "AudioStandard", "", "MmgwSetting.Board.OutHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrOutput_X, AudioStandard, AUDIOSTANDARD, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },
								{ nullptr, "", "", "", "MmgwSetting.Board.OutHr.AudioIpAddress", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PROTOCOL | BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PORT,
												BOF_PARAM_DEF_MULTI_ARRAY(S_MmgwApiState_X.pBoardState_X[0].pHrOutput_X[0].pAudioIpAddress_X, IPV4, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },

								{ nullptr, "AncIpAddress", "AncIpAddress", "", "MmgwSetting.Board.OutHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PROTOCOL | BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::IP_FORMAT_PORT,
												BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrOutput_X, AncIpAddress_X, IPV4, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },
								{ nullptr, "Bit", "Bit", "", "MmgwSetting.Board.OutHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrOutput_X, Bit_U32, UINT32, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },
								{ nullptr, "ProxyRes", "ProxyRes", "", "MmgwSetting.Board.OutHr", BOF_NAMESPACE::BOFPARAMETER_ARG_FLAG::NONE,
												BOF_PARAM_DEF_MULTI_ARRAY_OF_STRUCT(MMGW_BOARD_CHANNEL, S_MmgwApiState_X.pBoardState_X[0].pHrOutput_X, ProxyRes_X, SIZE2D, 0, 0, MMGWDRV_MAX_NUMBER_OF_DEVICE, MMGW_BOARD_STATE, MMGW_BOARD_CHANNEL,0) },
				};

#define MMGWVLC_DBG_LOG printf
BOFERR DisplayConfig()
{
	BOFERR Rts_E=BOF_ERR_NO_ERROR;

	uint32_t i_U32,j_U32,k_U32;
	char pTxt_c[0x400];

	MMGWVLC_DBG_LOG("Simul    %s\n", S_MmgwApiState_X.SimulatorOn_B ? "True":"False");
	MMGWVLC_DBG_LOG("Log      %s\n", S_MmgwApiState_X.LogOn_B ? "True":"False");
	MMGWVLC_DBG_LOG("ApiHndl  %p\n", static_cast<void *>(S_MmgwApiState_X.pApiHandle_X));
	MMGWVLC_DBG_LOG("Version  '%s'\n", S_MmgwApiState_X.pVersion_c);
	MMGWVLC_DBG_LOG("Nb Board %d\n", S_MmgwApiState_X.NbBoard_U32);
	MMGWVLC_DBG_LOG("Vs       %s: %s\n", S_MmgwApiState_X.VideoStandard.IdTxt(), S_MmgwApiState_X.VideoStandard.Description());

	for (i_U32=0;i_U32<MMGWDRV_MAX_NUMBER_OF_DEVICE;i_U32++)
	{
		MMGWVLC_DBG_LOG("Board %d Name %s Active %d Vs %s: %s\n", i_U32, S_MmgwApiState_X.pBoardState_X[i_U32].pBrdName_c,S_MmgwApiState_X.pBoardState_X[i_U32].BrdActive_B, S_MmgwApiState_X.pBoardState_X[i_U32].VideoStandard.IdTxt(), S_MmgwApiState_X.pBoardState_X[i_U32].VideoStandard.Description());
		MMGWVLC_DBG_LOG("        Type   %d\n", static_cast<int>(S_MmgwApiState_X.pBoardState_X[i_U32].BoardType_E));
		MMGWVLC_DBG_LOG("        PtpIp  %s\n", Bof_SocketAddressToString(S_MmgwApiState_X.pBoardState_X[i_U32].PtpIpAddress_X,true,true).c_str());
		MMGWVLC_DBG_LOG("        PtpMsk %s\n", Bof_SocketAddressToString(S_MmgwApiState_X.pBoardState_X[i_U32].PtpIpMask_X,true,true).c_str());
		MMGWVLC_DBG_LOG("        PtpGw  %s\n", Bof_SocketAddressToString(S_MmgwApiState_X.pBoardState_X[i_U32].PtpIpGw_X,true,true).c_str());

		for (j_U32=0;j_U32< MMGWDRV_MAX_NUMBER_OF_INPUT;j_U32++)
		{
			MMGWVLC_DBG_LOG("   Inp %d Name %s Active %d Vs  %s: %s\n", j_U32, S_MmgwApiState_X.pBoardState_X[i_U32].pHrInput_X[j_U32].pIoName_c,S_MmgwApiState_X.pBoardState_X[i_U32].pHrInput_X[j_U32].IoActive_B,S_MmgwApiState_X.pBoardState_X[i_U32].pHrInput_X[j_U32].VideoStandard.IdTxt(), S_MmgwApiState_X.pBoardState_X[i_U32].pHrInput_X[j_U32].VideoStandard.Description());
			MMGWVLC_DBG_LOG("         Vip %s\n", Bof_SocketAddressToString(S_MmgwApiState_X.pBoardState_X[i_U32].pHrInput_X[j_U32].VideoIpAddress_X,true,true).c_str());

			MMGWVLC_DBG_LOG("         As  %s\n", S_MmgwApiState_X.pBoardState_X[i_U32].pHrInput_X[j_U32].AudioStandard.ToString().c_str());
			pTxt_c[0]=0;
			for (k_U32=0;k_U32< MMGWDRV_MAX_AUDIO_CHANNEL_PER_PAGE;k_U32++)
			{
				if (Bof_IsIpAddressNull(S_MmgwApiState_X.pBoardState_X[i_U32].pHrInput_X[j_U32].pAudioIpAddress_X[k_U32]))
				{
					break;
				}
				strcat(pTxt_c, Bof_SocketAddressToString(S_MmgwApiState_X.pBoardState_X[i_U32].pHrInput_X[j_U32].pAudioIpAddress_X[k_U32],true,true).c_str());
			}
			MMGWVLC_DBG_LOG("%s\n", pTxt_c);

			MMGWVLC_DBG_LOG("         aip %s\n", Bof_SocketAddressToString(S_MmgwApiState_X.pBoardState_X[i_U32].pHrInput_X[j_U32].AncIpAddress_X,true,true).c_str());
			MMGWVLC_DBG_LOG("         Bit %d\n", S_MmgwApiState_X.pBoardState_X[i_U32].pHrInput_X[j_U32].Bit_U32);
			MMGWVLC_DBG_LOG("         Sz  %dx%d\n", S_MmgwApiState_X.pBoardState_X[i_U32].pHrInput_X[j_U32].ProxyRes_X.Width_U32, S_MmgwApiState_X.pBoardState_X[i_U32].pHrInput_X[j_U32].ProxyRes_X.Height_U32);
		}

		for (j_U32=0;j_U32< MMGWDRV_MAX_NUMBER_OF_OUTPUT;j_U32++)
		{
			MMGWVLC_DBG_LOG("   Out %d Name %s Active %d Vs  %s: %s\n", j_U32, S_MmgwApiState_X.pBoardState_X[i_U32].pHrOutput_X[j_U32].pIoName_c,S_MmgwApiState_X.pBoardState_X[i_U32].pHrOutput_X[j_U32].IoActive_B, S_MmgwApiState_X.pBoardState_X[i_U32].pHrOutput_X[j_U32].VideoStandard.IdTxt(), S_MmgwApiState_X.pBoardState_X[i_U32].pHrOutput_X[j_U32].VideoStandard.Description());
			MMGWVLC_DBG_LOG("         Vip %s\n", Bof_SocketAddressToString(S_MmgwApiState_X.pBoardState_X[i_U32].pHrOutput_X[j_U32].VideoIpAddress_X,true,true).c_str());

			MMGWVLC_DBG_LOG("         As  %s\n", S_MmgwApiState_X.pBoardState_X[i_U32].pHrOutput_X[j_U32].AudioStandard.ToString().c_str());
			pTxt_c[0]=0;
			for (k_U32=0;k_U32< MMGWDRV_MAX_AUDIO_CHANNEL_PER_PAGE;k_U32++)
			{
				if (Bof_IsIpAddressNull(S_MmgwApiState_X.pBoardState_X[i_U32].pHrOutput_X[j_U32].pAudioIpAddress_X[k_U32]))
				{
					break;
				}
				strcat(pTxt_c, Bof_SocketAddressToString(S_MmgwApiState_X.pBoardState_X[i_U32].pHrOutput_X[j_U32].pAudioIpAddress_X[k_U32],true,true).c_str());
			}
			MMGWVLC_DBG_LOG("%s\n", pTxt_c);

			MMGWVLC_DBG_LOG("         aip %s\n", Bof_SocketAddressToString(S_MmgwApiState_X.pBoardState_X[i_U32].pHrOutput_X[j_U32].AncIpAddress_X,true,true).c_str());
			MMGWVLC_DBG_LOG("         Bit %d\n", S_MmgwApiState_X.pBoardState_X[i_U32].pHrOutput_X[j_U32].Bit_U32);
			MMGWVLC_DBG_LOG("         Sz  %dx%d\n", S_MmgwApiState_X.pBoardState_X[i_U32].pHrOutput_X[j_U32].ProxyRes_X.Width_U32, S_MmgwApiState_X.pBoardState_X[i_U32].pHrOutput_X[j_U32].ProxyRes_X.Height_U32);
		}
	}
	return Rts_E;
}
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
	Sts_i            = pBofJsonParser_O->ToByte(S_CfgJsonCollection, JsonParseResultUltimateCheck, JsonParseError);
	EXPECT_EQ(Sts_i, 0);

	DisplayConfig();


//	EXPECT_EQ(BofJsonWriter_O.FromByte(false, S_CfgJsonCollection, JsonOut_S), BOF_ERR_NO_ERROR);
//printf("%s\n",JsonOut_S.c_str());

	BOF_SAFE_DELETE(pBofJsonParser_O);
}
