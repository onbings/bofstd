/*
 * Copyright (c) 2015-2025, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit tests of the xmlwriter library
 *
 * Name:        ut_parser.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */
#pragma once
/*** Include files ***********************************************************/

#include <bofstd/bofguid.h>
#include <bofstd/bofsocket.h>
#include <vector>

/*** Global variables ********************************************************/

/*** data model *****************************************************************/

struct OTHER
{
	uint32_t    a_U32;
	uint32_t    b_U32;
	uint32_t    c_U32;

	OTHER()
	{
		Reset();
	}
	void Reset()
	{
		a_U32 = 0;
		b_U32 = 0;
		c_U32 = 0;
	}
};

struct BOOKPARAM
{
	char       pId_c[16];
	char       pAuthor_c[64];
	char       pTitle_c[64];
	char       pGenre_c[32];
	float      Price_f;
	char       pBha1_c[16];
	char       pBha2_c[16];
	BOF_DATE_TIME    PublishDate_X;
	char       pDescription_c[256];

	BOOKPARAM()
	{
		Reset();
	}
	void Reset()
	{
		pId_c[0]          = 0;
		pAuthor_c[0]      = 0;
		pTitle_c[0]       = 0;
		pGenre_c[0]       = 0;
		Price_f           = 0.0f;
		pBha1_c[0]        = 0;
		pBha2_c[0]        = 0;
		memset(&PublishDate_X, 0, sizeof(PublishDate_X) );
		pDescription_c[0] = 0;
	}
};

struct APPPARAM
{
	BOF_SOCKET_ADDRESS    DeployIpAddress_X;
	uint16_t           DeployIpPort_U16;
	char               pDeployProtocol_c[16];
	char               pDeployDirectory_c[256];
	char               pLoginUser_c[32];
	char               pLoginPassword_c[32];
	char               pEmail_c[64];
	char               pUserName_c[16];
	char               pUserCompany_c[16];
	char               pToolChainBaseDirectory_c[256];
	char               pTemplateProjectBaseDirectory_c[256];
	BOOKPARAM          pBook_X[100];
	OTHER              pOther_X[4];

	APPPARAM()
	{
		Reset();
	}
	void Reset()
	{
		// Setup application default value
		memset(&DeployIpAddress_X, 0, sizeof(DeployIpAddress_X) );
		DeployIpPort_U16                   = 0;
		pDeployProtocol_c[0]               = 0;
		pDeployDirectory_c[0]              = 0;
		pLoginUser_c[0]                    = 0;
		pLoginPassword_c[0]                = 0;
		pEmail_c[0]                        = 0;
		pUserName_c[0]                     = 0;
		pUserCompany_c[0]                  = 0;
		pToolChainBaseDirectory_c[0]       = 0;
		pTemplateProjectBaseDirectory_c[0] = 0;

		memset(pBook_X, 0, sizeof(pBook_X) );
		memset(pOther_X, 0, sizeof(pOther_X) );
	}
};


struct APPPARAMVECTOR
{
	std::vector<std::string> StringCollection;
	std::vector<float> FloatCollection;
	std::vector<BOF_DATE_TIME> DateTimeCollection;
	std::vector<BOF_SOCKET_ADDRESS> Ip6Collection;
	std::vector<bool> BoolCollection;
	std::vector<std::string> IdCollection;
	APPPARAMVECTOR()
	{
		Reset();
	}
	void Reset()
	{
		StringCollection.clear();
		FloatCollection.clear();
		DateTimeCollection.clear();
		Ip6Collection.clear();
		BoolCollection.clear();
		IdCollection.clear();
	}
};


const uint32_t MAXIPSENDERTAG = 8;
struct IPSENDER
{
	std::string Description_S;
	std::string DeviceId_S;
	std::string FlowId_S;
	std::string Id_S;
	std::string Label_S;
	std::string HrefManifest_S;
	std::string pTag_S[MAXIPSENDERTAG];
	std::string Transport_S;
	std::string Version_S;
	IPSENDER()
	{
		Reset();
	}
	void Reset()
	{
		uint32_t i_U32;

		Description_S = "";
		DeviceId_S = "";
		FlowId_S = "";
		Id_S = "";
		Label_S = "";
		HrefManifest_S = "";
		for (i_U32 = 0; i_U32<MAXIPSENDERTAG; i_U32++)
		{
			pTag_S[i_U32] = "";
		}
		Transport_S = "";
		Version_S = "";
	}
};

/*
{"add_manifest":{"sdp":["v=0","o=- 0 0 IN IP4 18.52.86.2","s=OUT3","i=Player C","c=IN IP4 33.67.101.2/32","t=0 0","m=video 16386 RTP/AVP 96","a=rtpmap:96 raw/90000","a=fmtp:96 packetization-mode=1"],"sender_id":"81f4da21-c9ef-4b90-ab03-a4893cae32ed"}}
*/
const uint32_t MAXSDPLINE = 16;
struct IPMANIFEST
{
	std::string SenderId_S;
	std::string pSdpLine_S[MAXSDPLINE];
	IPMANIFEST()
	{
		Reset();
	}
	void Reset()
	{
		uint32_t i_U32;

		SenderId_S = "";
		for (i_U32 = 0; i_U32<MAXSDPLINE; i_U32++)
		{
			pSdpLine_S[i_U32] = "";
		}
	}
};


/*
{"connect":{"receiver_id":"1c513fc7-faa8-4a89-bd1c-56d8b00a1355","sdp":["v=0","o=- 0 0 IN IP4 18.52.86.0","s=OUT1","i=Player A","c=IN IP4 33.67.101.0/32","t=0 0","m=video 16384 RTP/AVP 96","a=rtpmap:96 raw/90000","a=fmtp:96 packetization-mode=1"]}}
*/
struct IPCONNECT
{
	std::string ReceiverId_S;
	std::vector<std::string> SdpCollection;
	IPCONNECT()
	{
		Reset();
	}
	void Reset()
	{
		ReceiverId_S = "";
		SdpCollection.clear();
	}
};

/*
{"add_device":{"id":"a8500668-9218-4063-ba36-9b4900b82e67","label":"","node_id":"00000000-0000-0000-0000-000000000000","receivers":[],"senders":[],"type":"","version":""}}
*/
struct ADD_DEVICE
{
	BOF_NAMESPACE::BofGuid Id;
	std::string Label_S;
	BOF_NAMESPACE::BofGuid NodeId;
	std::vector<std::string> ReceiverCollection;
	std::vector<std::string> SenderCollection;
	std::string Type_S;
	std::string Version_S;
	ADD_DEVICE()
	{
		Reset();
	}
	void Reset()
	{
		Id.Clear();
		Label_S = "";
		NodeId.Clear();
		ReceiverCollection.clear();
		SenderCollection.clear();
		Type_S = "";
		Version_S = "";
	}
};

/*
{"add_source":{"caps":{},"description":"OUT1","device_id":"a8500668-9218-4063-ba36-9b4900b82e67","format":"","id":"895085f3-76a1-4f09-b3dd-2aabeb230cd8","label":"Player A","parents":[],"tags":{"":[]},"version":""}}
{"add_source":{"caps":{},"description":"OUT2","device_id":"a8500668-9218-4063-ba36-9b4900b82e67","format":"","id":"8dab86c0-1ed8-4a27-b0c1-f5a6cb504b26","label":"Player B","parents":[],"tags":{"":[]},"version":""}}
{"add_source":{"caps":{},"description":"OUT3","device_id":"a8500668-9218-4063-ba36-9b4900b82e67","format":"","id":"dbf26b9e-e4d5-4f27-a0cf-86b6e8e505aa","label":"Player C","parents":[],"tags":{"":[]},"version":""}}
*/
struct ADD_SOURCE
{
	std::string Caps_S;
	std::string Description_S;
	BOF_NAMESPACE::BofGuid DeviceId;
	std::string Format_S;
	BOF_NAMESPACE::BofGuid Id;
	std::string Label_S;
	std::vector<std::string> ParentCollection;
	std::vector<std::string> TagCollection;
	std::string Version_S;
	ADD_SOURCE()
	{
		Reset();
	}
	void Reset()
	{
		Caps_S = "";
		Description_S = "";
		DeviceId.Clear();
		Format_S = "";
		Id.Clear();
		Label_S = "";
		ParentCollection.clear();
		TagCollection.clear();
		Version_S = "";
	}
};
/*
{"add_flow":{"description":"OUT1","format":"","id":"40b94c9d-9bf1-4721-95ae-4316b4e080ea","label":"Player A","parents":[],"source_id":"895085f3-76a1-4f09-b3dd-2aabeb230cd8","tags":{"":[]},"version":""}}
{"add_flow":{"description":"OUT2","format":"","id":"c24e8007-0de7-4935-ac73-d24ef6653192","label":"Player B","parents":[],"source_id":"8dab86c0-1ed8-4a27-b0c1-f5a6cb504b26","tags":{"":[]},"version":""}}
{"add_flow":{"description":"OUT3","format":"","id":"0de84872-e6fb-41cf-9f0f-bff71be6dac2","label":"Player C","parents":[],"source_id":"dbf26b9e-e4d5-4f27-a0cf-86b6e8e505aa","tags":{"":[]},"version":""}}
*/
struct ADD_FLOW
{
	std::string Description_S;
	std::string Format_S;
	BOF_NAMESPACE::BofGuid Id;
	std::string Label_S;
	std::vector<std::string> ParentCollection;
	BOF_NAMESPACE::BofGuid SourceId;
	std::vector<std::string> TagCollection;
	std::string Version_S;
	ADD_FLOW()
	{
		Reset();
	}
	void Reset()
	{
		Description_S = "";
		Format_S = "";
		Id.Clear();
		Label_S = "";
		ParentCollection.clear();
		SourceId.Clear();
		TagCollection.clear();
		Version_S = "";
	}
};


/*
{"add_sender":{"description":"OUT1","device_id":"a8500668-9218-4063-ba36-9b4900b82e67","flow_id":"40b94c9d-9bf1-4721-95ae-4316b4e080ea","id":"edbe7239-8659-46c9-a4f7-85b46a2efc73","label":"Player A","manifest_href":"","tags":{"":[]},"transport":"","version":""}}
{"add_sender":{"description":"OUT2","device_id":"a8500668-9218-4063-ba36-9b4900b82e67","flow_id":"c24e8007-0de7-4935-ac73-d24ef6653192","id":"8b2dfdef-45ab-44e1-ae0e-c3378134e4cf","label":"Player B","manifest_href":"","tags":{"":[]},"transport":"","version":""}}
{"add_sender":{"description":"OUT3","device_id":"a8500668-9218-4063-ba36-9b4900b82e67","flow_id":"0de84872-e6fb-41cf-9f0f-bff71be6dac2","id":"5556fa11-2824-4784-8305-65e4f3e1c1a3","label":"Player C","manifest_href":"","tags":{"":[]},"transport":"","version":""}}
*/
struct ADD_SENDER
{
	std::string Description_S;
	BOF_NAMESPACE::BofGuid DeviceId;
	BOF_NAMESPACE::BofGuid FlowId;
	BOF_NAMESPACE::BofGuid Id;
	std::string Label_S;
	std::string HrefManifest_S;
	std::vector<std::string> TagCollection;
	std::string Transport_S;
	std::string Version_S;
	ADD_SENDER()
	{
		Reset();
	}
	void Reset()
	{
		Description_S = "";
		DeviceId.Clear();
		FlowId.Clear();
		Id.Clear();
		Label_S = "";
		HrefManifest_S = "";
		TagCollection.clear();
		Transport_S = "";
		Version_S = "";
	}
};

/*
{"add_receiver":{"caps":{},"description":"IN1","device_id":"a8500668-9218-4063-ba36-9b4900b82e67","format":"","id":"998a20b2-2a8a-42c3-a527-46d1de96a9df","label":"Recorder A","subscription":{"sender_id":"00000000-0000-0000-0000-000000000000"},"tags":{"":[]},"transport":"","version":""}}
{"add_receiver":{"caps":{},"description":"IN2","device_id":"a8500668-9218-4063-ba36-9b4900b82e67","format":"","id":"6942c8f1-fa56-4db7-a741-ea2876b13744","label":"Recorder B","subscription":{"sender_id":"00000000-0000-0000-0000-000000000000"},"tags":{"":[]},"transport":"","version":""}}
{"add_receiver":{"caps":{},"description":"IN3","device_id":"a8500668-9218-4063-ba36-9b4900b82e67","format":"","id":"281f1cf0-6d65-4c09-ac27-d6a4a0f425fa","label":"Recorder C","subscription":{"sender_id":"00000000-0000-0000-0000-000000000000"},"tags":{"":[]},"transport":"","version":""}}
*/
struct ADD_RECEIVER
{
	std::string Caps_S;
	std::string Description_S;
	BOF_NAMESPACE::BofGuid DeviceId;
	std::string Format_S;
	BOF_NAMESPACE::BofGuid Id;
	std::string Label_S;
	BOF_NAMESPACE::BofGuid SubscriptionSenderId;
	std::vector<std::string> TagCollection;
	std::string Transport_S;
	std::string Version_S;
	ADD_RECEIVER()
	{
		Reset();
	}
	void Reset()
	{
		Caps_S = "";
		Description_S = "";
		DeviceId.Clear();
		Format_S = "";
		Id.Clear();
		Label_S = "";
		SubscriptionSenderId.Clear();
		TagCollection.clear();
		Transport_S = "";
		Version_S = "";
	}
};
/*
{"add_manifest":{"sdp":["v=0","o=- 0 0 IN IP4 18.52.86.0","s=OUT1","i=Player A","c=IN IP4 33.67.101.0/32","t=0 0","m=video 16384 RTP/AVP 96","a=rtpmap:96 raw/90000","a=fmtp:96 packetization-mode=1"],"sender_id":"edbe7239-8659-46c9-a4f7-85b46a2efc73"}}
{"add_manifest":{"sdp":["v=0","o=- 0 0 IN IP4 18.52.86.1","s=OUT2","i=Player B","c=IN IP4 33.67.101.1/32","t=0 0","m=video 16385 RTP/AVP 96","a=rtpmap:96 raw/90000","a=fmtp:96 packetization-mode=1"],"sender_id":"8b2dfdef-45ab-44e1-ae0e-c3378134e4cf"}}
{"add_manifest":{"sdp":["v=0","o=- 0 0 IN IP4 18.52.86.2","s=OUT3","i=Player C","c=IN IP4 33.67.101.2/32","t=0 0","m=video 16386 RTP/AVP 96","a=rtpmap:96 raw/90000","a=fmtp:96 packetization-mode=1"],"sender_id":"5556fa11-2824-4784-8305-65e4f3e1c1a3"}}
*/
struct ADD_MANIFEST
{
	BOF_NAMESPACE::BofGuid SenderId;
	std::vector<std::string> SdpLineCollection;
	ADD_MANIFEST()
	{
		Reset();
	}
	void Reset()
	{
		SenderId.Clear();
		SdpLineCollection.clear();
	}
};


/*
{"connect":{"receiver_id":"1c513fc7-faa8-4a89-bd1c-56d8b00a1355","sdp":["v=0","o=- 0 0 IN IP4 18.52.86.0","s=OUT1","i=Player A","c=IN IP4 33.67.101.0/32","t=0 0","m=video 16384 RTP/AVP 96","a=rtpmap:96 raw/90000","a=fmtp:96 packetization-mode=1"]}}
*/
struct CMD_CONNECT
{
	BOF_NAMESPACE::BofGuid  ReceiverId;
	std::vector<std::string> SdpLineCollection;

	CMD_CONNECT()
	{
		Reset();
	}
	void Reset()
	{
		ReceiverId.Clear();
		SdpLineCollection.clear();
	}
};

/*
{"status_receivers":""}}
*/
struct CMD_STATUS_RECEIVERS
{
	std::string Arg_S;

	CMD_STATUS_RECEIVERS()
	{
		Reset();
	}
	void Reset()
	{
		Arg_S = "";
	}
};
/*
{"status_receivers":[{"id":"1c513fc7-faa8-4a89-bd1c-56d8b00a1355","impaired":"true","ip":"18.52.86.0","present":"both","state":"AB"},{"id":"1c513fc7-faa8-4a89-bd1c-56d8b00adead","impaired":"false","ip":"18.52.86.2","present":"primary","state":"A"}]};
*/
struct REPLY_STATUS_RECEIVERS
{
	std::vector<BOF_NAMESPACE::BofGuid> IdCollection;
	std::vector<BOF_NAMESPACE::BOF_SOCKET_ADDRESS> IpCollection;
	std::vector<std::string> PresentCollection;
	std::vector<bool> ImpairedCollection;
	std::vector<std::string> StateCollection;

	REPLY_STATUS_RECEIVERS()
	{
		Reset();
	}
	void Reset()
	{
		IdCollection.clear();
		IpCollection.clear();
		PresentCollection.clear();
		ImpairedCollection.clear();
		StateCollection.clear();
	}
};
/*
{"status_senders":""}}
*/
struct CMD_STATUS_SENDERS
{
	std::string Arg_S;

	CMD_STATUS_SENDERS()
	{
		Reset();
	}
	void Reset()
	{
		Arg_S = "";
	}
};
/*
{\"status_senders\":[{\"id\":\"1c513fc7-faa8-4a89-bd1c-56d8b00a1356\",\"impaired\":\"true\",\"ip\":\"18.52.86.0:1\",\"present\":\"lost\",\"state\":\"B\"},{\"id\":\"1c513fc7-faa8-4a89-bd1c-56d8b00abeef\",\"impaired\":\"true\",\"ip\":\"18.52.86.222:235\",\"present\":\"primary\",\"state\":\"XA\"}]}
*/
struct REPLY_STATUS_SENDERS
{
	std::vector<BOF_NAMESPACE::BofGuid> IdCollection;
	std::vector<BOF_NAMESPACE::BOF_SOCKET_ADDRESS> IpCollection;
	std::vector<std::string> PresentCollection;
	std::vector<bool> ImpairedCollection;
	std::vector<std::string> StateCollection;

	REPLY_STATUS_SENDERS()
	{
		Reset();
	}
	void Reset()
	{
		IdCollection.clear();
		IpCollection.clear();
		PresentCollection.clear();
		ImpairedCollection.clear();
		StateCollection.clear();
	}
};