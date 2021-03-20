/*
 * Copyright (Sts_i) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the BofJsonWriter class
 *
 * Name:        bofjsonwriter.cpp
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

#include <bofstd/bofjsonwriter.h>
#include <bofstd/bofstring.h>
#include <json/json.h>
#include <sstream>

BEGIN_BOF_NAMESPACE()
/*** Global variables ********************************************************/

/*** Definitions *************************************************************/

/*** Class *******************************************************************/

// Opaque pointer design pattern: defined privately here all private data and functions: all of these can now change without recompiling callers ...
class BofJsonWriter::JsonWriterImplementation
{
public:

		JsonWriterImplementation()
		{
		}

		~JsonWriterImplementation()
		{
		}

		// if _MaxElemSize_U32 a stack based 64K buffer per element is used*/
		BOFERR FromByte(bool _ShortFormat_B, const std::vector<BOFPARAMETER> &_rJsonSchema_X, std::string &_rJsonOutput_S)
		{
			BOFERR Rts_E = BOF_ERR_INVALID_STATE;
			uint32_t i_U32, Index_U32, NbActiveArrayElement_U32, VectorCapacity_U32; // , NbSubTag_U32;
			char pOid_c[4096];
			bool Finish_B, ItIsAnArray_B;
			char pJsonValue_c[0x10000];
			Json::StyledStreamWriter JsonStyledStreamWriter_O;
			Json::FastWriter JsonFastWriter_O;
			Json::Value Root_O(Json::objectValue);
			std::vector<std::string> SubTagList;

			Rts_E = BOF_ERR_NO_ERROR;

			// Root_O.append(Json::Value::null);
			for (i_U32 = 0; i_U32 < _rJsonSchema_X.size(); i_U32++)
			{
				Index_U32 = 0;
				snprintf(pOid_c, sizeof(pOid_c), "%s.%s", _rJsonSchema_X[i_U32].Path_S.c_str(), _rJsonSchema_X[i_U32].Name_S.c_str());

				// NbSubTag_U32 = sizeof(ppSubTag_c) / sizeof(ppSubTag_c[0]);
				// if (Bof_SplitString(pOid_c, '.', &NbSubTag_U32, &ppSubTag_c[0]) == 0)
				SubTagList = Bof_StringSplit(pOid_c, ".");
				if (SubTagList.size() > 0)
				{
					if (_rJsonSchema_X[i_U32].ArrayCapacity_U32)
					{
						ItIsAnArray_B = true;
						NbActiveArrayElement_U32 = _rJsonSchema_X[i_U32].ArrayCapacity_U32;
					}
					else
					{
						ItIsAnArray_B = false;
						NbActiveArrayElement_U32 = 0;
					}
					if (BofParameter::S_ParameterToString(Index_U32, _rJsonSchema_X[i_U32], false, sizeof(pJsonValue_c), pJsonValue_c, VectorCapacity_U32))
					{
						if (NbActiveArrayElement_U32 == 0xFFFFFFFF)
						{
							NbActiveArrayElement_U32 = VectorCapacity_U32;
						}
						do
						{
							Finish_B = true;

							switch (SubTagList.size())
							{
								case 1:
								{
									if (ItIsAnArray_B)
									{
										if (SubTagList[0] == "")
										{
											Root_O[Index_U32] = Json::Value(pJsonValue_c);
										}
										else
										{
											Root_O[SubTagList[0]][Index_U32] = Json::Value(pJsonValue_c);
										}
									}
									else
									{
										Root_O[SubTagList[0]] = Json::Value(pJsonValue_c);
									}
								}
									break;

								case 2:
								{
									if (ItIsAnArray_B)
									{
										if (SubTagList[1] == "")
										{
											Root_O[SubTagList[0]][Index_U32] = Json::Value(pJsonValue_c);
										}
										else
										{
											Root_O[SubTagList[0]][Index_U32][SubTagList[1]] = Json::Value(pJsonValue_c);
										}
									}
									else
									{
										Root_O[SubTagList[0]][SubTagList[1]] = Json::Value(pJsonValue_c);
									}
								}
									break;

								case 3:
								{
									if (ItIsAnArray_B)
									{
										if (SubTagList[2] == "")
										{
											Root_O[SubTagList[0]][SubTagList[1]][Index_U32] = Json::Value(pJsonValue_c);
										}
										else
										{
											Root_O[SubTagList[0]][SubTagList[1]][Index_U32][SubTagList[2]] = Json::Value(pJsonValue_c);
										}
									}
									else
									{
										Root_O[SubTagList[0]][SubTagList[1]][SubTagList[2]] = Json::Value(pJsonValue_c);
									}
								}
									break;

								case 4:
								{
									if (ItIsAnArray_B)
									{
										if (SubTagList[3] == "")
										{
											Root_O[SubTagList[0]][SubTagList[1]][SubTagList[2]][Index_U32] = Json::Value(pJsonValue_c);
										}
										else
										{
											Root_O[SubTagList[0]][SubTagList[1]][SubTagList[2]][Index_U32][SubTagList[3]] = Json::Value(pJsonValue_c);
										}
									}
									else
									{
										Root_O[SubTagList[0]][SubTagList[1]][SubTagList[2]][SubTagList[3]] = Json::Value(pJsonValue_c);
									}
								}
									break;

								case 5:
								{
									if (ItIsAnArray_B)
									{
										if (SubTagList[4] == "")
										{
											Root_O[SubTagList[0]][SubTagList[1]][SubTagList[2]][SubTagList[3]][Index_U32] = Json::Value(pJsonValue_c);
										}
										else
										{
											Root_O[SubTagList[0]][SubTagList[1]][SubTagList[2]][SubTagList[3]][Index_U32][SubTagList[4]] = Json::Value(pJsonValue_c);
										}
									}
									else
									{
										Root_O[SubTagList[0]][SubTagList[1]][SubTagList[2]][SubTagList[3]][SubTagList[4]] = Json::Value(pJsonValue_c);
									}
								}
									break;

								case 6:
								{
									if (ItIsAnArray_B)
									{
										if (SubTagList[5] == "")
										{
											Root_O[SubTagList[0]][SubTagList[1]][SubTagList[2]][SubTagList[3]][SubTagList[4]][Index_U32] = Json::Value(pJsonValue_c);
										}
										else
										{
											Root_O[SubTagList[0]][SubTagList[1]][SubTagList[2]][SubTagList[3]][SubTagList[4]][Index_U32][SubTagList[5]] = Json::Value(pJsonValue_c);
										}
									}
									else
									{
										Root_O[SubTagList[0]][SubTagList[1]][SubTagList[2]][SubTagList[3]][SubTagList[4]][SubTagList[5]] = Json::Value(pJsonValue_c);
									}
								}
									break;

								case 7:
								{
									if (ItIsAnArray_B)
									{
										if (SubTagList[6] == "")
										{
											Root_O[SubTagList[0]][SubTagList[1]][SubTagList[2]][SubTagList[3]][SubTagList[4]][SubTagList[5]][Index_U32] = Json::Value(pJsonValue_c);
										}
										else
										{
											Root_O[SubTagList[0]][SubTagList[1]][SubTagList[2]][SubTagList[3]][SubTagList[4]][SubTagList[5]][Index_U32][SubTagList[6]] = Json::Value(pJsonValue_c);
										}
									}
									else
									{
										Root_O[SubTagList[0]][SubTagList[1]][SubTagList[2]][SubTagList[3]][SubTagList[4]][SubTagList[5]][SubTagList[6]] = Json::Value(pJsonValue_c);
									}
								}
									break;

								default:
								{
								}
									break;
							}

							if (ItIsAnArray_B)                          // array
							{
								Index_U32++;

								if (Index_U32 < NbActiveArrayElement_U32) // NbEntry is 0 for non array descriptor
								{
									if (BofParameter::S_ParameterToString(Index_U32, _rJsonSchema_X[i_U32], false, sizeof(pJsonValue_c), pJsonValue_c, VectorCapacity_U32))
									{
										// if (pJsonValue_c[0])	//Stop array iat the first empty elem
										{
											Finish_B = false;
										}
									}
								}
							}
						} while (!Finish_B);
					}                                               // if (BofParameter::S_ParameterToString(Index_U32, _rJsonSchema_X[i_U32], false, sizeof(pJsonValue_c), pJsonValue_c))
					else
					{
						NbActiveArrayElement_U32 = 0;
					}
				}                                                 // if ( Bof_SplitString(pOid_c, '.', &NbSubTag_U32, &ppSubTag_c[0]) == 0 )
			}                                                   // for (i_U32 = 0; i_U32 < _rJsonSchema_X.size(); i_U32++)
			if (_ShortFormat_B)
			{
				_rJsonOutput_S = JsonFastWriter_O.write(Root_O);
			}
			else
			{
				std::ostringstream OutputStream;
				JsonStyledStreamWriter_O.write(OutputStream, Root_O);
				_rJsonOutput_S = OutputStream.str();
			}
			return Rts_E;
		}
};

// Opaque pointer design pattern: ... set Implementation values ...
BofJsonWriter::BofJsonWriter()
	: mpuJsonWriterImplementation(new JsonWriterImplementation())
{}


BofJsonWriter::~BofJsonWriter()
{}


BOFERR BofJsonWriter::FromByte(bool _ShortFormat_B, const std::vector<BOFPARAMETER> &_rJsonSchema_X, std::string &_rJsonOutput_S)
{
	return mpuJsonWriterImplementation->FromByte(_ShortFormat_B, _rJsonSchema_X, _rJsonOutput_S);
}

END_BOF_NAMESPACE()