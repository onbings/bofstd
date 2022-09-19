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

#if 0
		const char *p,*q;
			Json::Value a,b,c;
			a=mRoot_O["MmgwSetting"]["Board"][0]["InHr"][0]["AudioIpAddress"];
			b=mRoot_O["MmgwSetting"]["Board"][0]["InHr"];
			c=mRoot_O["MmgwSetting"]["Board"];


			p=c[0]["InHr"][0]["AudioIpAddress"][0].asCString();
			q=c[0]["InHr"][0]["AudioIpAddress"][1].asCString();
			printf("%s/%s\n",p,q);
			p=c[0]["InHr"][1]["AudioIpAddress"][0].asCString();
			q=c[0]["InHr"][1]["AudioIpAddress"][1].asCString();
			printf("%s/%s\n",p,q);
			p=c[0]["InHr"][2]["AudioIpAddress"][0].asCString();
			q=c[0]["InHr"][2]["AudioIpAddress"][1].asCString();
			printf("%s/%s\n",p,q);
			p=c[1]["InHr"][0]["AudioIpAddress"][0].asCString();
			q=c[1]["InHr"][0]["AudioIpAddress"][1].asCString();
			printf("%s/%s\n",p,q);
			p=c[1]["InHr"][1]["AudioIpAddress"][0].asCString();
			q=c[1]["InHr"][1]["AudioIpAddress"][1].asCString();
			printf("%s/%s\n",p,q);
			p=c[1]["InHr"][2]["AudioIpAddress"][0].asCString();
			q=c[1]["InHr"][2]["AudioIpAddress"][1].asCString();
			printf("%s/%s\n",p,q);


			p=mRoot_O["MmgwSetting"]["Board"][0]["InHr"][0]["AudioIpAddress"][0].asCString();
			q=mRoot_O["MmgwSetting"]["Board"][0]["InHr"][0]["AudioIpAddress"][1].asCString();
			printf("%s/%s\n",p,q);
			p=mRoot_O["MmgwSetting"]["Board"][0]["InHr"][1]["AudioIpAddress"][0].asCString();
			q=mRoot_O["MmgwSetting"]["Board"][0]["InHr"][1]["AudioIpAddress"][1].asCString();
			printf("%s/%s\n",p,q);
			p=mRoot_O["MmgwSetting"]["Board"][0]["InHr"][2]["AudioIpAddress"][0].asCString();
			q=mRoot_O["MmgwSetting"]["Board"][0]["InHr"][2]["AudioIpAddress"][1].asCString();
			printf("%s/%s\n",p,q);

			p=mRoot_O["MmgwSetting"]["Board"][1]["InHr"][0]["AudioIpAddress"][0].asCString();
			q=mRoot_O["MmgwSetting"]["Board"][1]["InHr"][0]["AudioIpAddress"][1].asCString();
			printf("%s/%s\n",p,q);
			p=mRoot_O["MmgwSetting"]["Board"][1]["InHr"][1]["AudioIpAddress"][0].asCString();
			q=mRoot_O["MmgwSetting"]["Board"][1]["InHr"][1]["AudioIpAddress"][1].asCString();
			printf("%s/%s\n",p,q);
			p=mRoot_O["MmgwSetting"]["Board"][1]["InHr"][2]["AudioIpAddress"][0].asCString();
			q=mRoot_O["MmgwSetting"]["Board"][1]["InHr"][2]["AudioIpAddress"][1].asCString();
			printf("%s/%s\n",p,q);

			p=mRoot_O["MmgwSetting"]["Board"][0]["InHr"][0]["VideoStandard"].asCString();
			printf("%s\n",p);
			p=mRoot_O["MmgwSetting"]["Board"][0]["InHr"][1]["VideoStandard"].asCString();
			printf("%s\n",p);
			p=mRoot_O["MmgwSetting"]["Board"][0]["InHr"][2]["VideoStandard"].asCString();
			printf("%s\n",p);

			p=mRoot_O["MmgwSetting"]["Board"][1]["InHr"][0]["VideoStandard"].asCString();
			printf("%s\n",p);
			p=mRoot_O["MmgwSetting"]["Board"][1]["InHr"][1]["VideoStandard"].asCString();
			printf("%s\n",p);
			p=mRoot_O["MmgwSetting"]["Board"][1]["InHr"][2]["VideoStandard"].asCString();
			printf("%s\n",p);
#endif


struct JSON_OID_TAG
{
	uint32_t CrtArrayIndex_U32;
	uint32_t ArraySize_U32;
	uint32_t ArrayMaxSize_U32;
	uint32_t ElemSize_U32;
	std::string SubOid_S;
	std::string ArrayElem_S;
	JSON_OID_TAG()	{	}
	JSON_OID_TAG(const std::string &_rSubOid_S, const std::string &_rArrayElem_S)
	{
		ArraySize_U32=0;	//Init in GetFirstElementFromOid and in GetNExtArray
		ArrayMaxSize_U32=0;	//Init in ToByte
		SubOid_S=_rSubOid_S;
		CrtArrayIndex_U32=0;
		ElemSize_U32=0;	//Init in ToByte
		ArrayElem_S=_rArrayElem_S;	//If this is "" it is a raw array else [n].ArrayElem_S
	}
};
// Opaque pointer design pattern: defined privately here all private data and functions: all of these can now change without recompiling callers ...
class BofJsonParser::JsonParserImplementation
{
public:
		Json::Value mRoot_O;  // will contains the mRoot_O value after parsing.
		bool mJsonFileOk_B;
		std::string mLastError_S;
		char *mpJsonBuffer_c;
		Json::Value mLastJsonArray_O;
		Json::Value mLastJsonValue_O;
  	std::string mJsonArrayElem_S;
		uint32_t mCrtJsonArrayTagIndex_U32;
		uint32_t mFirstJsonArrayTagIndex_U32;
		uint32_t mSecondJsonArrayTagIndex_U32;
		std::vector<JSON_OID_TAG> mJsonOidTagCollection;

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
			uint32_t i_U32, j_U32, k_U32, Index_U32, ArrayIndex_U32;
			uint8_t *pArrayBaseAddress_U8;
			char pOid_c[1024];
			bool Finish_B;
			const char *pJsonValue_c;
			BOFPARAMETER JsonParam_X;
			std::string ValueAsString_S;

			if (mJsonFileOk_B)
			{
				for (i_U32 = 0; i_U32 < _rJsonSchema_X.size(); i_U32++)
				{
					JsonParam_X = _rJsonSchema_X[i_U32];
					JsonParam_X.ArgFlag_E |= BOFPARAMETER_ARG_FLAG::COMA_IS_NOT_A_SEPARATOR;

					snprintf(pOid_c, sizeof(pOid_c), "%s.%s", _rJsonSchema_X[i_U32].Path_S.c_str(), _rJsonSchema_X[i_U32].Name_S.c_str());
					pJsonValue_c = GetFirstElementFromOid(pOid_c, ValueAsString_S);
					ArrayIndex_U32 = 1;	//0 is nbmaxmultiarrayentry in BOF_PARAM_DEF_MULTI_ARRAY
					for (j_U32 = 0; j_U32 < mJsonOidTagCollection.size(); j_U32++)
					{
						if (mJsonOidTagCollection[j_U32].ArraySize_U32)
						{
							if (mJsonOidTagCollection[j_U32].ArraySize_U32 > JsonParam_X.ArrayCapacity_U32)
							{
								mJsonOidTagCollection[j_U32].ArraySize_U32 = JsonParam_X.ArrayCapacity_U32;
							}
							mJsonOidTagCollection[j_U32].ArrayMaxSize_U32 = JsonParam_X.ArrayCapacity_U32;

							if (j_U32 == static_cast<uint32_t>(mJsonOidTagCollection.size() - 1))
							{
								mJsonOidTagCollection[j_U32].ElemSize_U32 = JsonParam_X.ArrayElementSize_U32;
							}
							else
							{
								if (ArrayIndex_U32 < 3)	//BOF_NB_ELEM_IN_ARRAY(JsonParam_X.pExtraParam_U32))
								{
									mJsonOidTagCollection[j_U32].ElemSize_U32 = JsonParam_X.pExtraParam_U32[ArrayIndex_U32++];
								}
							}
						}
					} //for (j_U32 = 0; j_U32 < mJsonOidTagCollection.size(); j_U32++)

/*
 * At this point you should have:
 * mJsonOidTagCollection = {std::vector<onbings::bof::JSON_OID_TAG, std::allocator>}
 [0] = {onbings::bof::JSON_OID_TAG}
  CrtArrayIndex_U32 = {uint32_t} 0 [0x0]
  ArraySize_U32 = {uint32_t} 0 [0x0]
  ElemSize_U32 = {uint32_t} 0 [0x0]
  SubOid_S = {std::__cxx11::string} "MmgwSetting"
  ArrayElem_S = {std::__cxx11::string} ""
 [1] = {onbings::bof::JSON_OID_TAG}
  CrtArrayIndex_U32 = {uint32_t} 0 [0x0]
  ArraySize_U32 = {uint32_t} 2 [0x2]
  ElemSize_U32 = {uint32_t} 13116 [0x333c]
  SubOid_S = {std::__cxx11::string} "Board"
  ArrayElem_S = {std::__cxx11::string} ""
 [2] = {onbings::bof::JSON_OID_TAG}
  CrtArrayIndex_U32 = {uint32_t} 0 [0x0]
  ArraySize_U32 = {uint32_t} 3 [0x3]
  ElemSize_U32 = {uint32_t} 1076 [0x434]
  SubOid_S = {std::__cxx11::string} "InHr"
  ArrayElem_S = {std::__cxx11::string} "VideoStandard"

mJsonOidTagCollection = {std::vector<onbings::bof::JSON_OID_TAG, std::allocator>}
 [0] = {onbings::bof::JSON_OID_TAG}
  CrtArrayIndex_U32 = {uint32_t} 0 [0x0]
  ArraySize_U32 = {uint32_t} 0 [0x0]
  ElemSize_U32 = {uint32_t} 0 [0x0]
  SubOid_S = {std::__cxx11::string} "MmgwSetting"
  ArrayElem_S = {std::__cxx11::string} ""
 [1] = {onbings::bof::JSON_OID_TAG}
  CrtArrayIndex_U32 = {uint32_t} 0 [0x0]
  ArraySize_U32 = {uint32_t} 2 [0x2]
  ElemSize_U32 = {uint32_t} 13116 [0x333c]
  SubOid_S = {std::__cxx11::string} "Board"
  ArrayElem_S = {std::__cxx11::string} ""
 [2] = {onbings::bof::JSON_OID_TAG}
  CrtArrayIndex_U32 = {uint32_t} 0 [0x0]
  ArraySize_U32 = {uint32_t} 3 [0x3]
  ElemSize_U32 = {uint32_t} 1076 [0x434]
  SubOid_S = {std::__cxx11::string} "InHr"
  ArrayElem_S = {std::__cxx11::string} ""
 [3] = {onbings::bof::JSON_OID_TAG}
  CrtArrayIndex_U32 = {uint32_t} 1 [0x1]
  ArraySize_U32 = {uint32_t} 2 [0x2]
  ElemSize_U32 = {uint32_t} 56 [0x38]
  SubOid_S = {std::__cxx11::string} "AudioIpAddress"
  ArrayElem_S = {std::__cxx11::string} ""
 */

//Compatibility with non BOF_PARAM_DEF_MULTI_ARRAY type
					if (JsonParam_X.ArrayCapacity_U32 == 0)
					{
						JsonParam_X.ArrayCapacity_U32 = 1;
					}
					if (JsonParam_X.pExtraParam_U32[0] == 0)
					{
						JsonParam_X.pExtraParam_U32[0] = 1;
					}
					//printf("GetFirstElementFromOid %s='%s' base %p elemsz %d\n", pOid_c, pJsonValue_c, JsonParam_X.pValue, mJsonOidTagCollection[(mFirstJsonArrayTagIndex_U32 == 0xFFFFFFFF) ? 0 : mFirstJsonArrayTagIndex_U32].ElemSize_U32);

					for (k_U32=0;k_U32 < JsonParam_X.pExtraParam_U32[0];k_U32++)
					{
						Index_U32 = 0;
						for (j_U32 = 0; j_U32 < JsonParam_X.ArrayCapacity_U32; j_U32++)
						{
							Finish_B = false;
							Rts_E = BOF_ERR_NO_ERROR;

							while ((!Finish_B) && (pJsonValue_c) && (Rts_E == BOF_ERR_NO_ERROR))
							{
								Finish_B = true;
//								printf("Parse %s base at %p j %d elemsz %d Index %d->Add %p\n", pJsonValue_c, JsonParam_X.pValue, j_U32, JsonParam_X.ArrayElementSize_U32, Index_U32, (char *)JsonParam_X.pValue+(Index_U32*JsonParam_X.ArrayElementSize_U32));
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
										pJsonValue_c = GetNextElementFromOid(ValueAsString_S);
										//printf("GetNextElementFromOid %s\n", pJsonValue_c ? pJsonValue_c : "null");

										if (pJsonValue_c)
										{
											Index_U32++;
											Finish_B = false;
										}
										else
										{
											if (mSecondJsonArrayTagIndex_U32 == 0xFFFFFFFF)
											{
												Finish_B = true;
											}
											else
											{
												ResetAllLowerLevelIndex(mSecondJsonArrayTagIndex_U32);
												Finish_B = !GetNextArray(mSecondJsonArrayTagIndex_U32);
											}
											//printf("GetNextArray %s\n", Finish_B ? "No more" : "Ok got next");

											if (Finish_B)
											{
												break;
											}
											else
											{
												Index_U32 = 0;
												pJsonValue_c = mLastJsonValue_O.asCString();
												//printf("GetNextArray val %s\n", pJsonValue_c ? pJsonValue_c : "null");

												if (JsonParam_X.ArrayCapacity_U32 != 0xFFFFFFFF)
												{
													//Go to next array address, mSecondJsonArrayTagIndex_U32 is different from 0
													pArrayBaseAddress_U8 = reinterpret_cast<uint8_t *>(JsonParam_X.pValue) + mJsonOidTagCollection[mSecondJsonArrayTagIndex_U32].ElemSize_U32;
													//printf("GetNextArray ptr %p elemsize %d\n", pArrayBaseAddress_U8, mJsonOidTagCollection[mSecondJsonArrayTagIndex_U32].ElemSize_U32);
													JsonParam_X.pValue = pArrayBaseAddress_U8;
												}
											}
										} //else if if (pJsonValue_c)
									}
									else //if (Index_U32 < _rJsonSchema_X[i_U32].ArrayCapacity_U32)
									{
										Rts_E = BOF_ERR_NO_ERROR;                               // End of array
									}
								}
							}  // while ((!Finish_B) && (pJsonValue_c) && (Rts_E == BOF_ERR_NO_ERROR))                                                          // while
							if ((Rts_E != BOF_ERR_NO_ERROR) || (pJsonValue_c == nullptr))
							{
								break;
							}
						} //for (j_U32 = 0; j_U32 < JsonParam_X.ArrayCapacity_U32; j_U32++)

						if (mJsonOidTagCollection.size())
						{
							ResetAllLowerLevelIndex(mFirstJsonArrayTagIndex_U32);
							Finish_B = (mFirstJsonArrayTagIndex_U32 == 0xFFFFFFFF) ? true : !GetNextArray(mFirstJsonArrayTagIndex_U32);

							if (!Finish_B)
							{
								if ((mLastJsonValue_O.isNull()) || (!mLastJsonValue_O.asCString()))
								{
									Finish_B = true;
								}
							}
							//printf("Inner loop GetNextArray %s\n", Finish_B ? "No more" : "Ok got next");
							if (Finish_B)
							{
								break;
							}
							else
							{
								JsonParam_X.pValue = _rJsonSchema_X[i_U32].pValue;
								pArrayBaseAddress_U8 = reinterpret_cast<uint8_t *>(JsonParam_X.pValue) + ((k_U32+1) * mJsonOidTagCollection[(mFirstJsonArrayTagIndex_U32 == 0xFFFFFFFF) ? 0 : mFirstJsonArrayTagIndex_U32].ElemSize_U32);
								JsonParam_X.pValue = pArrayBaseAddress_U8;

								pJsonValue_c = mLastJsonValue_O.asCString();
							}
						}
					}
				} //for (i_U32 = 0; i_U32 < _rJsonSchema_X.size(); i_U32++)
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
			std::string ValueAsString_S;

			if (mJsonFileOk_B)
			{
				for (i_U32 = 0; i_U32 < _rJsonSchema_X.size(); i_U32++)
				{
					Index_U32 = 0;
					snprintf(pOid_c, sizeof(pOid_c), "%s.%s", _rJsonSchema_X[i_U32].Path_S.c_str(), _rJsonSchema_X[i_U32].Name_S.c_str());
					pJsonValue_c = GetFirstElementFromOid(pOid_c, ValueAsString_S);
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
								pJsonValue_c = GetNextElementFromOid(ValueAsString_S);
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

//New enhanced json parser with multi array def
//"MmgwSetting.Board.%.InHr.%" "VideoStandard"
//"MmgwSetting.Board%.InHr.%.AudioIpAddress.%" ""
//const char *p;
//p=mRoot_O["MmgwSetting"]["Board"][0]["InHr"][0]["AudioIpAddress"][0].asCString();
//p=mRoot_O["MmgwSetting"]["Board"][0]["InHr"][0]["AudioIpAddress"][1].asCString();
//p=mRoot_O["MmgwSetting"]["Board"][0]["InHr"][0]["VideoStandard"].asCString();
//p=mRoot_O["MmgwSetting"]["Board"][0]["InHr"][1]["VideoStandard"].asCString();
		const char *GetFirstElementFromOid(const char *_pOid_c, std::string &_rValueAsString_S)
		{
			const char *pRts_c = nullptr;
			uint32_t i_U32, NbArrayDetected_U32;
			std::string NextArrayElem_S, SubOid_S;
			Json::Value NextValue;
			std::vector<std::string> SubOidCollection;

			if ((mJsonFileOk_B) && (_pOid_c))
			{
				mCrtJsonArrayTagIndex_U32 = 0xFFFFFFFF;
				mFirstJsonArrayTagIndex_U32 = 0xFFFFFFFF;
				mSecondJsonArrayTagIndex_U32 = 0xFFFFFFFF;
				NbArrayDetected_U32=0;
				mJsonArrayElem_S="";
				mJsonOidTagCollection.clear();
				mLastJsonValue_O = Json::Value();
				mLastJsonArray_O = Json::Value();
				SubOidCollection = Bof_StringSplit(_pOid_c, ".");

				if (SubOidCollection.size() > 0)
				{
					mLastJsonValue_O = mRoot_O;
					for (i_U32 = 0; i_U32 < SubOidCollection.size(); i_U32++)
					{
		//				if (SubOidCollection[i_U32][SubOidCollection[i_U32].size() - 1] == '%')
		//				{
		//					SubOidCollection[i_U32].pop_back();
		//				}
						mLastJsonValue_O = mLastJsonValue_O[SubOidCollection[i_U32]];
						if (mLastJsonValue_O.isNull())
						{
							break;
						}
						else
						{
							if (mLastJsonValue_O.isArray())
							{
								mLastJsonArray_O = mLastJsonValue_O;
								NextArrayElem_S = "";
								SubOid_S = "NotEmpty";
								//printf("mJsonOidTagCollection push array sz %d of %s\n",mLastJsonArray_O.size(),SubOidCollection[i_U32-1].c_str());
//Last entry
								NbArrayDetected_U32++;
								if (NbArrayDetected_U32 > 3)
								{
									break;	//We only handle a max of 3 array and if the third one exist it must be a the last raw item in the suboid list
								}
								if (i_U32 == (SubOidCollection.size() - 1))
								{
									break;  //Not possible in our case raw array have a "" element name just after (see after)
								}

								if (i_U32 < (SubOidCollection.size() - 1))
								{
									SubOid_S = SubOidCollection[i_U32 + 1];
					//				if (SubOid_S[SubOid_S.size() - 1] == '%')
					//				{
					//					SubOid_S.pop_back();
					//				}
									if (SubOid_S != "")
									{
										NextValue = mLastJsonArray_O[0][SubOid_S];
										if ((!NextValue.isNull()) && ((NextValue.isString()) || (NextValue.isBool()) || (NextValue.isInt64()) || (NextValue.isUInt64()) || (NextValue.isDouble())))
										{
											NextArrayElem_S = SubOid_S;
										}
									}
								}

								mJsonArrayElem_S = NextArrayElem_S;
								mCrtJsonArrayTagIndex_U32 = static_cast<uint32_t>(mJsonOidTagCollection.size());
								if (mFirstJsonArrayTagIndex_U32==0xFFFFFFFF)
								{
									mFirstJsonArrayTagIndex_U32=mCrtJsonArrayTagIndex_U32;
								}
								else
								{
									if (mSecondJsonArrayTagIndex_U32==0xFFFFFFFF)
									{
										mSecondJsonArrayTagIndex_U32=mCrtJsonArrayTagIndex_U32;
									}
								}
								mJsonOidTagCollection.push_back(JSON_OID_TAG(SubOidCollection[i_U32], mJsonArrayElem_S));
								mJsonOidTagCollection[i_U32].ArraySize_U32 = mLastJsonArray_O.size();

								if ((NextArrayElem_S != "") || (SubOid_S == ""))
								{
									i_U32++;
								}
								if (mJsonArrayElem_S == "")
								{
									mLastJsonValue_O = mLastJsonArray_O[0];
								}
								else
								{
									mLastJsonValue_O = mLastJsonArray_O[0][mJsonArrayElem_S];
								}
							}
							else
							{
								mJsonOidTagCollection.push_back(JSON_OID_TAG(SubOidCollection[i_U32], ""));
							}
						}
					} //for (i_U32 = 0; i_U32 < mSubTagList.size(); i_U32++)

					if (i_U32 == SubOidCollection.size())
					{
					// We only manage pure text json file->convertion to binary is made by bofparameter
						if (!mLastJsonValue_O.isNull())
						{
							if (mCrtJsonArrayTagIndex_U32!=0xFFFFFFFF)
							{
								mJsonOidTagCollection[mCrtJsonArrayTagIndex_U32].CrtArrayIndex_U32++;
							}
							if (mLastJsonValue_O.isString())
							{
								pRts_c = mLastJsonValue_O.asCString();
							} 
							else if (mLastJsonValue_O.isBool())
							{
								_rValueAsString_S = mLastJsonValue_O.asBool() ? "true" : "false";
								pRts_c = _rValueAsString_S.c_str();
							}
							else if (mLastJsonValue_O.isInt64())
							{
								_rValueAsString_S = std::to_string(mLastJsonValue_O.asInt64());
								pRts_c = _rValueAsString_S.c_str();
							}
							else if (mLastJsonValue_O.isUInt64())
							{
								_rValueAsString_S = std::to_string(mLastJsonValue_O.asUInt64());
								pRts_c = _rValueAsString_S.c_str();
							}
							else if (mLastJsonValue_O.isDouble())
							{
								_rValueAsString_S = std::to_string(mLastJsonValue_O.asDouble());
								pRts_c = _rValueAsString_S.c_str();
							}
						}
					}
				}
			}
			return pRts_c;
		}

		const char *GetNextElementFromOid(std::string &_rValueAsString_S)
		{
			const char *pRts_c = nullptr;

			if ((mJsonFileOk_B) && (!mLastJsonValue_O.isNull()) && (!mLastJsonArray_O.isNull()))
			{
				if (mLastJsonArray_O.isArray())
				{
					if (mJsonOidTagCollection[mCrtJsonArrayTagIndex_U32].CrtArrayIndex_U32 < mJsonOidTagCollection[mCrtJsonArrayTagIndex_U32].ArraySize_U32)
					{
						if (mJsonArrayElem_S=="")
						{
							mLastJsonValue_O = mLastJsonArray_O[mJsonOidTagCollection[mCrtJsonArrayTagIndex_U32].CrtArrayIndex_U32];
						}
						else
						{
							mLastJsonValue_O = mLastJsonArray_O[mJsonOidTagCollection[mCrtJsonArrayTagIndex_U32].CrtArrayIndex_U32][mJsonArrayElem_S];
						}
						if (!mLastJsonValue_O.isNull())
						{
							mJsonOidTagCollection[mCrtJsonArrayTagIndex_U32].CrtArrayIndex_U32++;
							if (mLastJsonValue_O.isString())
							{
								pRts_c = mLastJsonValue_O.asCString();
							}
							else if (mLastJsonValue_O.isBool())
							{
								_rValueAsString_S = mLastJsonValue_O.asBool() ? "true" : "false";
								pRts_c = _rValueAsString_S.c_str();
							}
							else if (mLastJsonValue_O.isInt64())
							{
								_rValueAsString_S = std::to_string(mLastJsonValue_O.asInt64());
								pRts_c = _rValueAsString_S.c_str();
							}
							else if (mLastJsonValue_O.isUInt64())
							{
								_rValueAsString_S = std::to_string(mLastJsonValue_O.asUInt64());
								pRts_c = _rValueAsString_S.c_str();
							}
							else if (mLastJsonValue_O.isDouble())
							{
								_rValueAsString_S = std::to_string(mLastJsonValue_O.asDouble());
								pRts_c = _rValueAsString_S.c_str();
							}
						}
					}
				}
			}
			return pRts_c;
		}

		void ResetAllLowerLevelIndex(uint32_t _Level_U32)
		{
			uint32_t i_U32;
			JSON_OID_TAG JsonTagArray_X;

			if (_Level_U32 != 0xFFFFFFFF)
			{
				for (i_U32 = _Level_U32 + 1; i_U32 < mJsonOidTagCollection.size(); i_U32++)
				{
					JsonTagArray_X = mJsonOidTagCollection[i_U32];
					JsonTagArray_X.CrtArrayIndex_U32 = 0;
					mJsonOidTagCollection[i_U32] = JsonTagArray_X;
					/*
					if (JsonTagArray_X.ArrayElem_S=="")
					{
						JsonTagArray_X = mJsonOidTagCollection[i_U32-1];
						JsonTagArray_X.CrtArrayIndex_U32 = 0;
						mJsonOidTagCollection[i_U32-1] = JsonTagArray_X;
					}
					 */
				}
			}
		}
		bool GetNextArray(uint32_t _Level_U32)
		{
			bool Rts_B=false;
			JSON_OID_TAG JsonTagArray_X;
			uint32_t i_U32;

			if ((_Level_U32 < mJsonOidTagCollection.size()) && (mJsonOidTagCollection[_Level_U32].ArraySize_U32))
			{
				/*
				if ((mJsonOidTagCollection[_Level_U32].ArrayElem_S=="") && (_Level_U32))
				{
					_Level_U32--;
				}
				 */
				JsonTagArray_X = mJsonOidTagCollection[_Level_U32];
				JsonTagArray_X.CrtArrayIndex_U32++;
//printf("%d<%d\n",JsonTagArray_X.CrtArrayIndex_U32, JsonTagArray_X.ArraySize_U32);

				if (JsonTagArray_X.CrtArrayIndex_U32 < JsonTagArray_X.ArraySize_U32)
				{
					mJsonOidTagCollection[_Level_U32] = JsonTagArray_X;
					mLastJsonValue_O=mRoot_O;	//[mJsonOidTagCollection[0].SubOid_S];
					for (i_U32 = 0; i_U32 < mJsonOidTagCollection.size(); i_U32++)
					{
						mLastJsonValue_O = mLastJsonValue_O[mJsonOidTagCollection[i_U32].SubOid_S];
						//printf("[%s]", mJsonOidTagCollection[i_U32].SubOid_S.c_str());
						if (mLastJsonValue_O.isNull())
						{
							break;
						}
						else
						{
							if (mLastJsonValue_O.isArray())
							{
								mLastJsonArray_O = mLastJsonValue_O;
								mJsonOidTagCollection[i_U32].ArraySize_U32 = mLastJsonArray_O.size();
								if (mJsonOidTagCollection[i_U32].ArraySize_U32 > mJsonOidTagCollection[i_U32].ArrayMaxSize_U32)
								{
									mJsonOidTagCollection[i_U32].ArraySize_U32 = mJsonOidTagCollection[i_U32].ArrayMaxSize_U32;
								}

								if (i_U32 == _Level_U32)
								{
									mLastJsonValue_O = mLastJsonValue_O[JsonTagArray_X.CrtArrayIndex_U32];
									//printf("[%d]", JsonTagArray_X.CrtArrayIndex_U32);
								}
								else
								{
									mLastJsonValue_O = mLastJsonValue_O[mJsonOidTagCollection[i_U32].CrtArrayIndex_U32];
									//printf("[%d]", mJsonOidTagCollection[i_U32].CrtArrayIndex_U32);
								}
								if (mJsonOidTagCollection[i_U32].ArrayElem_S=="")
								{
								}
								else
								{
									mLastJsonValue_O = mLastJsonValue_O[mJsonOidTagCollection[i_U32].ArrayElem_S];
									//printf("[%s]", mJsonOidTagCollection[i_U32].ArrayElem_S.c_str());
								}
							}
						}
					}
					if (i_U32 == mJsonOidTagCollection.size())
					{
						if ((!mLastJsonValue_O.isNull()) && (mLastJsonValue_O.isString()))
						{
							mJsonOidTagCollection[mCrtJsonArrayTagIndex_U32].CrtArrayIndex_U32=1;
							Rts_B = true;
						}
					}
				}
			}
			//printf("%s", Rts_B ? " Ok\n":" Bad\n");
			return Rts_B;
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
	std::string ValueAsString_S;
	return mpuJsonParserImplementation->GetFirstElementFromOid(_pOid_c, ValueAsString_S);
}


const char *BofJsonParser::GetNextElementFromOid()
{
	std::string ValueAsString_S;
	return mpuJsonParserImplementation->GetNextElementFromOid(ValueAsString_S);
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