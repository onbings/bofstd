/*
 * Copyright (Sts_i) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the BofJsonParser class
 *
 * Name:        bofjsonparser.cpp
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
#include <regex>

#include <bofstd/bofjsonparser.h>
#include <bofstd/bofstring.h>
#include <json/json.h>

BEGIN_BOF_NAMESPACE()

/*** Global variables ********************************************************/

/*** Definitions *************************************************************/

/*** Class *******************************************************************/

// Opaque pointer design pattern: defined privately here all private data and functions: all of these can now change without recompiling callers ...
class BofJsonParser::JsonParserImplementation
{
public:
		Json::Value mRoot_O;  // will contains the mRoot_O value after parsing.
		bool mJsonFileOk_B;
		std::string mLastError_S;
		char *mpJsonBuffer_c;
		Json::Value mLastParentJsonValue_O;
		Json::Value mLastJsonValue_O;
		uint32_t mCrtJsonIndex_U32;
		char mpTag_c[1024];
		std::vector<std::string> mSubTagList;

		JsonParserImplementation(const std::string &_rJsonInput_S)
		{
			Open(_rJsonInput_S);
		}

		~JsonParserImplementation()
		{
			Close();
		}

		void Open(const std::string &_rJsonInput_S)
		{
			Json::Reader JsonReader_O;
			int BufferLength_i = static_cast<int>(_rJsonInput_S.size() + 1);

			mpJsonBuffer_c = nullptr;
			mCrtJsonIndex_U32 = 0;

			mpJsonBuffer_c = new char[BufferLength_i];

			if (mpJsonBuffer_c)
			{
				memcpy(mpJsonBuffer_c, _rJsonInput_S.c_str(), BufferLength_i - 1);
				mpJsonBuffer_c[BufferLength_i - 1] = 0;
				mJsonFileOk_B = JsonReader_O.parse(mpJsonBuffer_c, mRoot_O, false);
				if ((!mJsonFileOk_B) || (!JsonReader_O.good()))
				{
					mLastError_S = JsonReader_O.getFormattedErrorMessages();
				}
			}
		}

		void Close()
		{
			BOF_SAFE_DELETE_ARRAY(mpJsonBuffer_c);
		}

		BOFERR ToByte(const std::vector<BOFPARAMETER> &_rJsonSchema_X, const BOFPARAMETER_PARSE_CALLBACK _ParseCallback_O, const BOFJSONPARSER_ERROR_CALLBACK _ErrorCallback_O)
		{
			BOFERR Rts_E = BOF_ERR_INVALID_STATE;
			uint32_t i_U32, Index_U32;
			char pOid_c[1024];
			bool Finish_B;
			const char *pJsonValue_c;
			BOFPARAMETER JsonParam_X;

			if (mJsonFileOk_B)
			{
				for (i_U32 = 0; i_U32 < _rJsonSchema_X.size(); i_U32++)
				{
					Index_U32 = 0;
					snprintf(pOid_c, sizeof(pOid_c), "%s.%s", _rJsonSchema_X[i_U32].Path_S.c_str(), _rJsonSchema_X[i_U32].Name_S.c_str());
					pJsonValue_c = GetFirstElementFromOid(pOid_c);
					Finish_B = false;
					Rts_E = BOF_ERR_NO_ERROR;
					while ((!Finish_B) && (pJsonValue_c) && (Rts_E == BOF_ERR_NO_ERROR))
					{
						Finish_B = true;
						JsonParam_X = _rJsonSchema_X[i_U32];
						JsonParam_X.ArgFlag_E |= BOFPARAMETER_ARG_FLAG::COMA_IS_NOT_A_SEPARATOR;
						Rts_E = BofParameter::S_Parse(Index_U32, JsonParam_X, pJsonValue_c, _ParseCallback_O);

						if (Rts_E != BOF_ERR_NO_ERROR)
						{
							if (_ErrorCallback_O != nullptr)
							{
								_ErrorCallback_O(Rts_E, _rJsonSchema_X[i_U32], pJsonValue_c);
							}
						}
						else
						{
							if (Index_U32 < _rJsonSchema_X[i_U32].ArrayCapacity_U32) // NbEntry is 0 for non array descriptor
							{
								pJsonValue_c = GetNextElementFromOid();

								if (pJsonValue_c)
								{
									Index_U32++;
									Finish_B = false;
								}
							}
							else
							{
								Rts_E = BOF_ERR_NO_ERROR;                               // End of array
							}
						}
					}                                                            // while
					if (Rts_E != BOF_ERR_NO_ERROR)
					{
						break;
					}
				}                                                              // for
			}
			return Rts_E;
		}

		BOFERR Serialize(const std::vector<BOFPARAMETER> &_rJsonSchema_X)
		{
			BOFERR Rts_E = BOF_ERR_INVALID_STATE;
			uint32_t i_U32, Index_U32;
			char pOid_c[1024];
			bool Finish_B;
			const char *pJsonValue_c;

			if (mJsonFileOk_B)
			{
				for (i_U32 = 0; i_U32 < _rJsonSchema_X.size(); i_U32++)
				{
					Index_U32 = 0;
					snprintf(pOid_c, sizeof(pOid_c), "%s.%s", _rJsonSchema_X[i_U32].Path_S.c_str(), _rJsonSchema_X[i_U32].Name_S.c_str());
					pJsonValue_c = GetFirstElementFromOid(pOid_c);
					Finish_B = false;
					Rts_E = BOF_ERR_NO_ERROR;
					while ((!Finish_B) && (pJsonValue_c) && (Rts_E == BOF_ERR_NO_ERROR))
					{
						Finish_B = true;
						// Rts_i    = BofParameter::S_Parse(Index_U32, _rJsonSchema_X[i_U32], pJsonValue_c, _ParseCallback_O);

						if (Rts_E != BOF_ERR_NO_ERROR)
						{}
						else
						{
							if (Index_U32 < _rJsonSchema_X[i_U32].ArrayCapacity_U32) // NbEntry is 0 for non array descriptor
							{
								pJsonValue_c = GetNextElementFromOid();

								if (pJsonValue_c)
								{
									Index_U32++;
									Finish_B = false;
								}
							}
							else
							{
								Rts_E = BOF_ERR_PARSER;
							}
						}
					}                                                            // While
					if (Rts_E != BOF_ERR_NO_ERROR)
					{
						break;
					}
				}                                                              // for
			}
			return Rts_E;
		}

		// Description is an oid (. separated path) to the json element->oid leave between () is a root element

		/*
		 * _pOid_c is a string containing a dot separated list of json tags which describe the path to reach a JSON element from the mRoot_O node
		 * ex: MulFtpUserSetting.ToolChainBaseDirectory points to the first occurrence of  "ToolChainBaseDirectory": "D:\\SysGCC\\",
		 *
		 * {
		 * "MulFtpUserSetting": {
		 *       "ToolChainBaseDirectory": "D:\\SysGCC\\",
		 *       "TemplateProjectBaseDirectory": "D:\\cloudstation\\pro\\vsmake-project-template\\",
		 *       "book": "bha",
		 *       "catalog": {
		 *               "book": [
		 *               {
		 *               "id": "bk101",
		 *               "publish_date": "2016-05-26",
		 *
		 */

		const char *GetFirstElementFromOid(const char *_pOid_c)
		{
			const char *pRts_c = nullptr;
			uint32_t i_U32;

			if ((mJsonFileOk_B) && (_pOid_c) && (strlen(_pOid_c) < sizeof(mpTag_c)))
			{
				mCrtJsonIndex_U32 = 0;
				strcpy(mpTag_c, _pOid_c);

				mSubTagList = Bof_StringSplit(mpTag_c, ".");
				if (mSubTagList.size() > 0)
				{
					mLastJsonValue_O = mRoot_O[mSubTagList[0]];
					mLastParentJsonValue_O = mLastJsonValue_O;

					if (!mLastJsonValue_O.isNull())
					{
						for (i_U32 = 1; i_U32 < mSubTagList.size(); i_U32++)
						{
							if ((i_U32 == (mSubTagList.size() - 1))) //|| (mSubTagList[i_U32] == ""))	//Pure array
							{
								/*
								Json::Value xx, yy,zz;
								xx = mRoot_O["status_senders"];
								yy = mRoot_O["status_senders"]["ip"];
								zz = mRoot_O["status_senders"]["ip"][""];
								pRts_c = yy.asCString();
								*/
								if (mLastJsonValue_O.isArray())
								{
									if (mSubTagList[i_U32] == "")  //Pure array
									{
										mLastJsonValue_O = mLastJsonValue_O[0];
									}
									else
									{
										mLastJsonValue_O = mLastJsonValue_O[0][mSubTagList[i_U32]];
									}
									mCrtJsonIndex_U32 = 1;
								}
								else
								{
									mLastJsonValue_O = mLastJsonValue_O[mSubTagList[i_U32]];
									/*
									if (mLastJsonValue_O.isArray())
									{
										//mLastParentJsonValue_O = mLastJsonValue_O;
										mLastJsonValue_O = mLastJsonValue_O[0];
										mCrtJsonIndex_U32 = 1;
									}
									*/
								}

								// We only manage pure text json file->convertion to binary is made by bofparameter
								if (mLastJsonValue_O.isString())
								{
									pRts_c = mLastJsonValue_O.asCString();
								}
								/* Start of native type integration but we need to refactor serializer -W not now
								if (mLastJsonValue_O.isBool())
								{
									pRts_c = mLastJsonValue_O.asBool() ? "true":"false";
								}
								*/
							}
							else
							{
								if (mLastJsonValue_O.isArray())
								{
									mLastJsonValue_O = mLastJsonValue_O[0][mSubTagList[i_U32]];
								}
								else
								{
									mLastJsonValue_O = mLastJsonValue_O[mSubTagList[i_U32]];
								}
								mLastParentJsonValue_O = mLastJsonValue_O;
							}

							if (mLastJsonValue_O.isNull())
							{
								break;
							}
						}                            // for (i_U32 = 0; i_U32 < NbSubTag_U32; i_U32++)
					}
				}                                // if (Bof_SplitString(pTag_c, '.', &NbSubTag_U32, &ppSubTag_c, &Attribute_B) == 0)
			}                                  // if ((mpXmlDoc_X) && (_pOid_c) && (strlen(_pOid_c) < sizeof(pTag_c)) && (_ppNode_X))
			return pRts_c;
		}

		const char *GetNextElementFromOid()
		{
			const char *pRts_c = nullptr;


			if ((mJsonFileOk_B) && (!mLastJsonValue_O.isNull()) && (!mLastParentJsonValue_O.isNull()))
			{
				mLastJsonValue_O = mLastParentJsonValue_O;

				if (mLastJsonValue_O.isArray())
				{
					if (mCrtJsonIndex_U32 < mLastJsonValue_O.size())
					{
						if (mSubTagList[mSubTagList.size() - 1] == "")  //Pure array
						{
							mLastJsonValue_O = mLastParentJsonValue_O[mCrtJsonIndex_U32];
						}
						else
						{
							mLastJsonValue_O = mLastParentJsonValue_O[mCrtJsonIndex_U32][mSubTagList[mSubTagList.size() - 1]];
						}
						if (!mLastJsonValue_O.isNull())
						{
							pRts_c = mLastJsonValue_O.asCString();
							/* Start of native type integration but we need to refactor serializer -W not now
							if (mLastJsonValue_O.isString())
							{
								pRts_c = mLastJsonValue_O.asCString();
							}
							if (mLastJsonValue_O.isBool())
							{
								pRts_c = mLastJsonValue_O.asBool() ? "true" : "false";
							}
							*/
						}
						mCrtJsonIndex_U32++;
					}
				}
			}
			return pRts_c;
		}

		bool IsValid()
		{
			return mJsonFileOk_B;
		}

		std::string RootName()
		{
			std::string Rts_S;
			Json::Value::iterator It = mRoot_O.begin();
			if (It != mRoot_O.end())
			{
				Rts_S = mRoot_O.begin().name();
			}
			return Rts_S;
		}

		void OperatorAssign(const std::string &_rJsonInput_S)
		{
			Close();
			Open(_rJsonInput_S);
		}
};

// Opaque pointer design pattern: ... set Implementation values ...
BofJsonParser::BofJsonParser(const std::string &_rJsonInput_S)
	: mpuJsonParserImplementation(new JsonParserImplementation(_rJsonInput_S))
{}

BofJsonParser::~BofJsonParser()
{}


BOFERR BofJsonParser::ToByte(const std::vector<BOFPARAMETER> &_rJsonSchema_X, const BOFPARAMETER_PARSE_CALLBACK _ParseCallback_O, const BOFJSONPARSER_ERROR_CALLBACK _ErrorCallback_O)
{
	return mpuJsonParserImplementation->ToByte(_rJsonSchema_X, _ParseCallback_O, _ErrorCallback_O);
}


const char *BofJsonParser::GetFirstElementFromOid(const char *_pOid_c)
{
	return mpuJsonParserImplementation->GetFirstElementFromOid(_pOid_c);
}


const char *BofJsonParser::GetNextElementFromOid()
{
	return mpuJsonParserImplementation->GetNextElementFromOid();
}

bool BofJsonParser::IsValid()
{
	return mpuJsonParserImplementation->IsValid();
}

std::string BofJsonParser::RootName()
{
	return mpuJsonParserImplementation->RootName();
}

//{"add_source":{
std::string BofJsonParser::S_RootName(const std::string &_rJsonIn_S)
{
	std::string Rts_S;

#if 1
	static const std::regex S_RegExJsonFirstTag("\\{[^\"]*\"([^\t\n\v\f\r\"]*)");  //Static as it can takes time (on gcc 4.9 for example)
	std::smatch MatchString;

	if (std::regex_search(_rJsonIn_S, MatchString, S_RegExJsonFirstTag))
	{
		if (MatchString.size() == 2)  //Size=2 because we have a capture group ()
		{
			Rts_S = MatchString[1].str();
		}
	}
	//	Rts_S=Bof_StringTrim(Rts_S);
	return Rts_S;
#else
	std::string::size_type PosOpenBrace1, PosOpenBrace2, PosOpenQuote, PosCloseQuote, PosColon;
	PosOpenBrace1 = _rJsonIn_S.find('{', 0);
	if (PosOpenBrace1 != std::string::npos) 
	{
		PosOpenQuote = _rJsonIn_S.find('"', PosOpenBrace1 + 1);
		if (PosOpenQuote != std::string::npos)
		{
			PosCloseQuote = _rJsonIn_S.find('"', PosOpenQuote + 1);
			if (PosCloseQuote != std::string::npos)
			{
				PosColon = _rJsonIn_S.find(':', PosOpenBrace1 + 1);
				if (PosColon != std::string::npos)
				{
					PosOpenBrace2 = _rJsonIn_S.find('{', PosOpenBrace1 + 1);
					if (PosOpenBrace2 != std::string::npos)
					{
						if ((PosOpenBrace1 < PosOpenQuote) && (PosOpenQuote < PosCloseQuote) && (PosCloseQuote < PosColon) && (PosColon < PosOpenBrace2))
						{
							Rts_S = _rJsonIn_S.substr(PosOpenQuote + 1, PosCloseQuote - PosOpenQuote - 1);
						}
					}
				}
			}
		}
	}
	Rts_S= Bof_StringTrim(Rts_S);
	return Rts_S;
#endif
}

BofJsonParser &BofJsonParser::operator=(const std::string &_rJsonInput_S)
{
	mpuJsonParserImplementation->OperatorAssign(_rJsonInput_S);
	return *this;
}
END_BOF_NAMESPACE()