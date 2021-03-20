/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 * Name:        BofrPC.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 23 2002  BHA : Initial release
 *
 *
 */

/*** Include Files **********************************************************/

#include <bofstd/bofrpc.h>
#include <stdarg.h>
BEGIN_BOF_NAMESPACE()

/*** Global variable ********************************************************/

const char *GL_pRpcType_c[] =          /*! RPC type of variable (in the same orger than RPCKEYWORD)*/
	{
		"RPCRETURNVALUE",
		"RPCSERVERARG",
		"RPCARRAYIN",
		"RPCARRAYOUT",
		"RPCARRAYINOUT",
		"RPCIN",
		"RPCOUT",
		"RPCINOUT",
		"RPCVOID",
		"RPCU8",
		"RPCSBYTE",
		"RPCU16",
		"RPCSWORD",
		"RPCU32",
		"RPCSLONG",
		"RPCSTRING",
		"RPCFLOAT",
		"RPCDOUBLE"
	};

uint16_t   BofRpc::S_mRpcCmdTag_U16 = 0;

/*** DumpRpcFrame ********************************************************************/

/*!
 * Description
 * The DumpRpcFrame function display debug information about incoming and outcoming RPC requests.
 *
 *
 * Parameters
 * Trx_B :       true if this frame is sent
 * Nb_U32 :       Specifies the number of byte to dump
 * pData_U8 :   Specifies a pointer to the binary RPC data to dump
 *
 * Returns
 * None
 *
 * Remarks
 * None
 */
void DumpRpcFrame(bool Trx_B, char *pTxt_c, uint32_t Nb_U32, uint8_t *pData_U8)
{
	char pWork_c[4096], *p_c;
	uint32_t i_U32, Size_U32, NbFree_U32;

	i_U32 = snprintf(pWork_c, sizeof(pWork_c), "[RPC]: %s %s ", Trx_B ? "TRX" : "RCV", pTxt_c);

	if (Nb_U32 > 512)
	{
		Nb_U32 = 512;
	}
	p_c = &pWork_c[i_U32];
	NbFree_U32 = static_cast<uint32_t>(sizeof(pWork_c)) - i_U32;

	for (i_U32 = 0; i_U32 < Nb_U32; i_U32++)
	{
		Size_U32 = snprintf(p_c, NbFree_U32, "%02X ", *pData_U8++);
		NbFree_U32 -= Size_U32;
		p_c += Size_U32;
	}
}


#ifdef BHBH
uint32_t Bof_BuildRpcFunctionTable(TCHAR *pFnIn_tc, TCHAR *pFnOut_tc, TCHAR *pVarName_tc, uint32_t *pErrorLine_U32)
{
	uint32_t    Rts_U32 = -1, NbArg_U32, Line_U32;
	FILE        *IoIn_X, *IoOut_X;
	TCHAR       pIn_tc[4096], pOut_tc[4096], pModifier_tc[32], *p_tc, pHelp_tc[RPC_MAXHELPSIZE + 1];
	uint32_t    i_U32, NextI_U32, NbToken_U32;
	PARSERTOKEN pToken_X[64];
	bool        Error_B, FirstVar_B;

/*
 * RPCFUNCTION GL_pRpcFct_X[]=
 * {
 * {"Rpc0",Rpc0,{RPC_INOUT|RPC_VAR_U8,RPC_INOUT|RPC_VAR_U16,RPC_INOUT|RPC_VAR_U32,RPC_INOUT|RPC_VAR_CHAR,RPC_INOUT|RPC_VAR_FLOAT,RPC_INOUT|RPC_VAR_DOUBLE},6,"Rpc0 Function"},
 * {"Rpc1",Rpc1,{RPC_IN|RPC_VAR_U8,RPC_IN|RPC_VAR_U16,RPC_IN|RPC_VAR_U32,RPC_IN|RPC_VAR_CHAR,RPC_IN|RPC_VAR_FLOAT,RPC_IN|RPC_VAR_DOUBLE},6,"Rpc1 Function"},
 * {"Rpc3",Rpc3,{RPC_INOUT|RPC_VAR_U8,RPC_OUT|RPC_VAR_ARRAY,RPC_INOUT|RPC_VAR_U32},3,"Rpc3 Function"}
 * };
 */
	*pErrorLine_U32 = 0xFFFFFFFF;

	if ( (IoIn_X = fopen(pFnIn_tc, "r") ) != nullptr)
	{
		if ( (IoOut_X = fopen(pFnOut_tc, "w") ) != nullptr)
		{
			_stprintf(pOut_tc, "RPCFUNCTION %s[]=\n{\n", pVarName_tc);

			if (fputs(pOut_tc, IoOut_X) >= 0)
			{
				Rts_U32  = 0;
				Error_B  = false;
				Line_U32 = 0;

				while ( (! Error_B)
								&& (fgets(pIn_tc, sizeof(pIn_tc), IoIn_X) )
								)
				{
					Line_U32++;
					NbToken_U32 = 64;

					if ( (Bof_Parser(pIn_tc, sizeof(GL_pRpcType_c) / sizeof(GL_pRpcType_c[0]), GL_pRpcType_c, &NbToken_U32, pToken_X, " (,)", (TCHAR *)GL_pRpcType_c[KW_RPCRETURNVALUE], ";") )
							 && (pToken_X[1].KeywordId_U32 == PARSER_SYMBOL)
							 )
					{
						NbArg_U32  = 0;
						p_tc       = pOut_tc;
						p_tc      += (_stprintf(p_tc, "   {\"%s\",%s,{", pToken_X[1].pSymbol_tc, pToken_X[1].pSymbol_tc) );
						i_U32      = 2;
						FirstVar_B = true;

						while ( (i_U32 < NbToken_U32 - 1) // last is ;
										&& (! Error_B)
										)
						{
							if (pToken_X[i_U32].KeywordId_U32 != PARSER_SYMBOL)
							{
								if (pToken_X[i_U32].KeywordId_U32 < KW_RPCVOID)
								{
									if (pToken_X[i_U32 + 1].KeywordId_U32 == PARSER_SYMBOL)
									{
										if (pToken_X[i_U32].KeywordId_U32 == KW_RPCARRAYIN)
										{
											_tcscpy(pModifier_tc, "RPC_IN");
										}
										else if (pToken_X[i_U32].KeywordId_U32 == KW_RPCARRAYOUT)
										{
											_tcscpy(pModifier_tc, "RPC_OUT");
										}
										else if (pToken_X[i_U32].KeywordId_U32 == KW_RPCARRAYINOUT)
										{
											_tcscpy(pModifier_tc, "RPC_INOUT");
										}
										else if (pToken_X[i_U32].KeywordId_U32 == KW_RPCSERVERARG)
										{
											_tcscpy(pModifier_tc, "RPC_SERVERARG");
										}
										else
										{
											Error_B = true;
										}
										NextI_U32 = i_U32 + 2;
									}
									else
									{
										Error_B = true;
									}
								}
								else
								{
									if ( (pToken_X[i_U32 + 1].KeywordId_U32 != PARSER_SYMBOL)
											 && (pToken_X[i_U32 + 2].KeywordId_U32 == PARSER_SYMBOL)
											 )
									{
										if (pToken_X[i_U32 + 1].KeywordId_U32 == KW_RPCIN)
										{
											_tcscpy(pModifier_tc, "RPC_IN");
										}
										else if (pToken_X[i_U32 + 1].KeywordId_U32 == KW_RPCOUT)
										{
											_tcscpy(pModifier_tc, "RPC_OUT");
										}
										else if (pToken_X[i_U32 + 1].KeywordId_U32 == KW_RPCINOUT)
										{
											_tcscpy(pModifier_tc, "RPC_INOUT");
										}
										else
										{
											Error_B = true;
										}
										NextI_U32 = i_U32 + 3;
									}
									else
									{
										if ( (pToken_X[i_U32].KeywordId_U32 == KW_RPCSTRING)
												 && (pToken_X[i_U32 + 1].KeywordId_U32 == PARSER_SYMBOL)
												 )
										{
											NextI_U32 = i_U32 + 2;
										}
										else
										{
											Error_B = true;
										}
									}
								}
							}
							else
							{
								Error_B = true;
							}

							if (! Error_B)
							{
								NbArg_U32++;

								if (! FirstVar_B)
								{
									*p_tc++ = ',';
								}

								switch (pToken_X[i_U32].KeywordId_U32)
								{
									case KW_RPCARRAYIN:
									case KW_RPCARRAYOUT:
									case KW_RPCARRAYINOUT:
									{
										p_tc += (_stprintf(p_tc, "%s|RPC_VAR_ARRAY", pModifier_tc) );
									}
									break;

									case KW_RPCU8:
									case KW_RPCSBYTE:
									{
										p_tc += (_stprintf(p_tc, "%s|RPC_VAR_U8", pModifier_tc) );
									}
									break;

									case KW_RPCU16:
									case KW_RPCSWORD:
									{
										p_tc += (_stprintf(p_tc, "%s|RPC_VAR_U16", pModifier_tc) );
									}
									break;

									case KW_RPCU32:
									case KW_RPCSLONG:
									{
										p_tc += (_stprintf(p_tc, "%s|RPC_VAR_U32", pModifier_tc) );
									}
									break;

									case KW_RPCSTRING:
									{
										p_tc += (_stprintf(p_tc, "%s|RPC_VAR_CHAR", pModifier_tc) );
									}
									break;

									case KW_RPCFLOAT:
									{
										p_tc += (_stprintf(p_tc, "%s|RPC_VAR_FLOAT", pModifier_tc) );
									}
									break;

									case KW_RPCDOUBLE:
									{
										p_tc += (_stprintf(p_tc, "%s|RPC_VAR_DOUBLE", pModifier_tc) );
									}
									break;

									case KW_RPCSERVERARG:
									{
										p_tc += (_stprintf(p_tc, "%s|RPC_VAR_U32", pModifier_tc) );
									}
									break;


									default:
									{
										Error_B = true;
									}
									break;
								}
							}
							else
							{
								break;
							}
							FirstVar_B = false;
							i_U32      = NextI_U32;
						}

						_tcsncpy(pHelp_tc, pIn_tc, RPC_MAXHELPSIZE - 1);
						pHelp_tc[RPC_MAXHELPSIZE - 1] = 0;
						p_tc                         += (_stprintf(p_tc, "},%d,\"%s\"},\n", NbArg_U32, pHelp_tc) );

						if (fputs(pOut_tc, IoOut_X) < 0)
						{
							if (! Error_B)
							{
								Error_B = true;
							}
						}
					}
				}

				if (Error_B)
				{
					*pErrorLine_U32 = Line_U32;
				}
				_stprintf(pOut_tc, "};\n");
				fputs(pOut_tc, IoOut_X);
			}
			fclose(IoOut_X);
		}
		fclose(IoIn_X);
	}
	return Rts_U32;
}
#endif

/*** BofRpc ********************************************************************/

/*!
 * Description
 * The BofRpc method is used to instanciate a BofRpc object.
 * The Client use the BofComChannel interface of the communication object to send and
 * receive Rpc request.
 * After connection a sonnect signature is sent by the client and aknowledged by the sever.
 * The aknowledge pattern is used to check is byte swapping on binary data must be applied or not.
 * After this tep the server sends to the client a table describing the Rpc functions available to the client.
 * This table is used by the server and the client to process Rpc requests.
 *
 * Parameters
 * pBofRpcParam_X:  Specifies the object creation parameter
 *
 * Returns
 * Nothing
 *
 * Remarks
 * None
 */

BofRpc::BofRpc(BOF_RPC_PARAM *_pBofRpcParam_X)
{
	uint32_t i_U32;
	uint16_t Sof_U16;
	uint8_t *pSof_U8;
	BOF_STACK_PARAM BofStackParam_X;

	mpRpcFct_X = nullptr;
	mpRpcReqStack_O = nullptr;
	mpRpcAnsStack_O = nullptr;
	memset(&mRpcState_X, 0, sizeof(mRpcState_X));
	if (_pBofRpcParam_X)
	{
		mBofRpcParam_X = *_pBofRpcParam_X;
		mRpcState_X.MaxStackSize_U32 = mBofRpcParam_X.MaxStackSize_U32;
		mRpcState_X.UseChecksum_B = mBofRpcParam_X.UseChecksum_B;
		mRpcState_X.MaxStringSize_U32 = BOF_RPC_MAXSTRINGSIZE;
		mRpcState_X.NbRpcFunction_U32 = mBofRpcParam_X.NbRpcFunction_U32;


		Sof_U16 = BOF_RPCMAGICNUMBER;
		pSof_U8 = (uint8_t *) &Sof_U16;

		if ((pSof_U8[0] == (BOF_RPCMAGICNUMBER >> 8))
		    && (pSof_U8[1] == (uint8_t) (BOF_RPCMAGICNUMBER & 0xFF))
			)
		{
			mLittleEndian_B = false;
		}
		else
		{
			mLittleEndian_B = true;
		}

		mBofRpcParam_X.ErrorCode_E = BOF_ERR_EINVAL;

		if (mBofRpcParam_X.NbRpcFunction_U32 < BOF_RPC_MAXFCT)
		{
			for (i_U32 = 0; i_U32 < mBofRpcParam_X.NbRpcFunction_U32; i_U32++)
			{
				if (mBofRpcParam_X.pRpcFct_X[i_U32].NbArg_U16 < BOF_RPC_MAXFCTARG)
				{}
				else
				{
					break;
				}
			}

			if (i_U32 == mBofRpcParam_X.NbRpcFunction_U32)
			{
				mBofRpcParam_X.ErrorCode_E = BOF_ERR_NO_ERROR;
			}
		}

		if (mBofRpcParam_X.ErrorCode_E == BOF_ERR_NO_ERROR)
		{
			BofStackParam_X.MaxStackSize_U32 = mBofRpcParam_X.MaxStackSize_U32;
			BofStackParam_X.pData = nullptr;
			BofStackParam_X.MultiThreadAware_B = mBofRpcParam_X.MultiThreadAware_B;
			BofStackParam_X.SwapByte_B = false;
			mpRpcReqStack_O = new BofStack(BofStackParam_X);

			if ((mpRpcReqStack_O) && (mBofRpcParam_X.ErrorCode_E == BOF_ERR_NO_ERROR))
			{
				mpRpcAnsStack_O = new BofStack(BofStackParam_X);
			}
		}
		_pBofRpcParam_X->ErrorCode_E = mBofRpcParam_X.ErrorCode_E;
		mBofRpcParam_X.MaxStackSize_U32 = _pBofRpcParam_X->MaxStackSize_U32;
		mBofRpcParam_X.UseChecksum_B = _pBofRpcParam_X->UseChecksum_B;
		mBofRpcParam_X.NbRpcFunction_U32 = _pBofRpcParam_X->NbRpcFunction_U32;
	}
}


/*** ~BofRpc ********************************************************************/

/*!
 * Description
 * The ~BofRpc method delete a previously created BofRpc object. All
 * dynamically allocated resources are released here
 *
 * Parameters
 * None
 *
 * Returns
 * Nothing
 *
 * Remarks
 * None
 */

BofRpc::~BofRpc()
{
	BOF_SAFE_DELETE(mpRpcReqStack_O);
	BOF_SAFE_DELETE(mpRpcAnsStack_O);
}


/*** BuildRpcRequest ********************************************************************/

/*!
 * Description
 * The BuildRpcRequest method build a RPC request frame
 *
 * Parameters
 * _FctId_U32:      Specifies the rpc function id to execute
 * _TimeOutInMs_U16: Specfy the command execution timeout
 * _pMaxRpcFrameSizeInByte_U32: Specify the Rpc max frame size and return the resulting rpc frame size (_pRpcFrameResultBuffer_U8)
 * _pRpcFrameResultBuffer_U8: Specify where to store the resulting buffer
 * ...:  Specifies the Rpc parameter to send with the Rpc requests
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * None
 */
uint32_t BofRpc::BuildRpcRequest(BOF_RPC_PARSER_RESULT *_pRpcRequest_X, ...)
{
	uint32_t Rts_U32 = BOF_ERR_TOO_BIG;
	uint32_t i_U32, Cs_U32, Val_U32, *pVal_U32;
	uint8_t Val_U8, *pVal_U8;
	uint16_t Val_U16, *pVal_U16, Flag_U16;
	uint64_t Val_U64, *pVal_U64;
	bool Sts_B = false, RpcIn_B;
	BOF_RPC_FUNCTION *pRpcFct_X;
	BOF_RPC_VAR_TYPE VarType_E;
	float Val_f, *pVal_f;
	double Val_ff, *pVal_ff;
	va_list VaList_X;

	va_start(VaList_X, _pRpcRequest_X);

	if ((_pRpcRequest_X)
	    && (_pRpcRequest_X->FctId_U32 < BOF_RPC_MAXFCT)
	    && (mpRpcReqStack_O)
		)
	{
		_pRpcRequest_X->pRpcStack_U8 = mpRpcReqStack_O->GetStackBuffer();
		pRpcFct_X = &mBofRpcParam_X.pRpcFct_X[_pRpcRequest_X->FctId_U32];
		mpRpcReqStack_O->SetSwapByte(false);
		ResetStackPointerToRpcPayload(mpRpcReqStack_O);

		for (i_U32 = 0; i_U32 < pRpcFct_X->NbArg_U16; i_U32++)
		{
			VarType_E = (BOF_RPC_VAR_TYPE) (pRpcFct_X->pArg_U16[i_U32] & BOF_RPC_VARTYPEMASK);
			RpcIn_B = ((pRpcFct_X->pArg_U16[i_U32] & BOF_RPC_IN)
			           || (pRpcFct_X->pArg_U16[i_U32] & BOF_RPC_INOUT)
			);

			switch (VarType_E)
			{
				case BOF_RPC_VAR_VOID:
				{
					Sts_B = true;
				}
					break;

				case BOF_RPC_VAR_CHAR:
				{
					pVal_U8 = (uint8_t *) va_arg(VaList_X, char *);
					Sts_B = (RpcIn_B) ? mpRpcReqStack_O->Push((char *) pVal_U8) : true;
				}
					break;

				case BOF_RPC_VAR_U8:
				case BOF_RPC_VAR_S8:
				{
					if (pRpcFct_X->pArg_U16[i_U32] & BOF_RPC_IN)
					{
						Val_U8 = (uint8_t) va_arg(VaList_X, uint32_t);
						Sts_B = (RpcIn_B) ? mpRpcReqStack_O->Push(Val_U8) : true;
					}
					else
					{
						pVal_U8 = va_arg(VaList_X, uint8_t *);
						Sts_B = (RpcIn_B) ? mpRpcReqStack_O->Push(*pVal_U8) : true;
					}
				}
					break;

				case BOF_RPC_VAR_U16:
				case BOF_RPC_VAR_S16:
				{
					if (pRpcFct_X->pArg_U16[i_U32] & BOF_RPC_IN)
					{
						Val_U16 = (uint16_t) va_arg(VaList_X, uint32_t);
						Sts_B = (RpcIn_B) ? mpRpcReqStack_O->Push(Val_U16) : true;
					}
					else
					{
						pVal_U16 = va_arg(VaList_X, uint16_t *);
						Sts_B = (RpcIn_B) ? mpRpcReqStack_O->Push(*pVal_U16) : true;
					}
				}
					break;

				case BOF_RPC_VAR_U32:
				case BOF_RPC_VAR_S32:
				{
					if (pRpcFct_X->pArg_U16[i_U32] & BOF_RPC_IN)
					{
						Val_U32 = va_arg(VaList_X, uint32_t);
						Sts_B = (RpcIn_B) ? mpRpcReqStack_O->Push(Val_U32) : true;
					}
					else
					{
						pVal_U32 = va_arg(VaList_X, uint32_t *);
						Sts_B = (RpcIn_B) ? mpRpcReqStack_O->Push(*pVal_U32) : true;
					}
				}
					break;

				case BOF_RPC_VAR_U64:
				case BOF_RPC_VAR_S64:
				{
					if (pRpcFct_X->pArg_U16[i_U32] & BOF_RPC_IN)
					{
						Val_U64 = va_arg(VaList_X, uint64_t);
						Sts_B = (RpcIn_B) ? mpRpcReqStack_O->Push(Val_U64) : true;
					}
					else
					{
						pVal_U64 = va_arg(VaList_X, uint64_t *);
						Sts_B = (RpcIn_B) ? mpRpcReqStack_O->Push(*pVal_U64) : true;
					}
				}
					break;

				case BOF_RPC_VAR_FLOAT:
				{
					if (pRpcFct_X->pArg_U16[i_U32] & BOF_RPC_IN)
					{
						Val_f = (float) va_arg(VaList_X, double);
						Sts_B = (RpcIn_B) ? mpRpcReqStack_O->Push(Val_f) : true;
					}
					else
					{
						pVal_f = (float *) va_arg(VaList_X, double *);
						Sts_B = (RpcIn_B) ? mpRpcReqStack_O->Push(*pVal_f) : true;
					}
				}
					break;

				case BOF_RPC_VAR_DOUBLE:
				{
					if (pRpcFct_X->pArg_U16[i_U32] & BOF_RPC_IN)
					{
						Val_ff = va_arg(VaList_X, double);
						Sts_B = (RpcIn_B) ? mpRpcReqStack_O->Push(Val_ff) : true;
					}
					else
					{
						pVal_ff = va_arg(VaList_X, double *);
						Sts_B = (RpcIn_B) ? mpRpcReqStack_O->Push(*pVal_ff) : true;
					}
				}
					break;

				case BOF_RPC_VAR_ARRAY:
				{
					pVal_U8 = (uint8_t *) va_arg(VaList_X, char *);
					Sts_B = (RpcIn_B) ? Push(mpRpcReqStack_O, true, (BOF_RPC_VAR_ARRAYINOUT) pVal_U8) : true;
				}
					break;

				default:
					Sts_B = false;
					break;
			}

			if (!Sts_B)
			{
				break;
			}
		}

		if (Sts_B)
		{
			Sts_B = false;
			Flag_U16 = 0;

			if (mBofRpcParam_X.UseChecksum_B)
			{
				Flag_U16 |= BOF_RPC_FLAG_USECHECKSUM;
			}

			if (mpRpcReqStack_O->Push(static_cast< uint16_t > (0)))                                                                                                                          // Dumy rts
			{
				if (mpRpcReqStack_O->Push(static_cast< uint16_t > (_pRpcRequest_X->FctId_U32)))
				{
					S_mRpcCmdTag_U16++;

					if (!S_mRpcCmdTag_U16)
					{
						S_mRpcCmdTag_U16 = 1;
					}

					if (mBofRpcParam_X.UseChecksum_B)
					{
						if (PushTopRpcControlParam(mpRpcReqStack_O, BOF_RPCMAGICNUMBER, Flag_U16, (uint16_t) _pRpcRequest_X->TimeOut_U32, S_mRpcCmdTag_U16,
						                           mpRpcReqStack_O->GetStackPointer() + 2)) // Insert Sof and length for Mtx comptutation
						{
							Cs_U32 = 0;

							for (i_U32 = 0; i_U32 < mpRpcReqStack_O->GetStackPointer(); i_U32++)
							{
								Cs_U32 += _pRpcRequest_X->pRpcStack_U8[i_U32];
							}
							Sts_B = mpRpcReqStack_O->Push(static_cast< uint16_t > (Cs_U32));
						}
					}
					else
					{
						Sts_B = true;
					}

					if (Sts_B)
					{
						_pRpcRequest_X->RpcStackLen_U32 = mpRpcReqStack_O->GetStackPointer();

						if (PushTopRpcControlParam(mpRpcReqStack_O, BOF_RPCMAGICNUMBER, Flag_U16, (uint16_t) _pRpcRequest_X->TimeOut_U32, S_mRpcCmdTag_U16, _pRpcRequest_X->RpcStackLen_U32))        // Push size
						{
							Rts_U32 = BOF_ERR_NO_ERROR;
						}
					}
				}
			}
		}
	}
	va_end(VaList_X);

	return Rts_U32;
}


/*** BuildRpcAnswer ********************************************************************/

/*!
 * Description
 * The BuildRpcAnswer method build a RPC answer frame
 *
 * Parameters
 * _pMaxRpcFrameSizeInByte_U32: Specify the Rpc max frame size and return the resulting rpc frame size (_pRpcFrameResultBuffer_U8)
 * _pRpcFrameResultBuffer_U8: Specify where to store the resulting buffer
 * _Tag_U16: Specify the Rpc answer tag value (0 ignore)
 * _pRpcParserResult_X: Specifies the characteristics of the receiver thread that has received the Rpc request
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * None
 */
uint32_t BofRpc::BuildRpcAnswer(BOF_RPC_PARSER_RESULT *_pRpcParserResult_X)
{
	uint32_t Rts_U32 = BOF_ERR_TOO_BIG;
	uint32_t i_U32, Cs_U32;
	uint16_t Flag_U16;
	bool Sts_B = false, RpcOut_B;
	BOF_RPC_FUNCTION *pRpcFct_X;
	BOF_RPC_VAR_TYPE VarType_E;

	if ((mpRpcAnsStack_O)
	    && (_pRpcParserResult_X)
	    && (_pRpcParserResult_X->FctId_U32 < BOF_RPC_MAXFCT)
		)
	{
		_pRpcParserResult_X->pRpcStack_U8 = mpRpcAnsStack_O->GetStackBuffer();
		pRpcFct_X = &mBofRpcParam_X.pRpcFct_X[_pRpcParserResult_X->FctId_U32];

		mpRpcAnsStack_O->SetSwapByte(false);
		ResetStackPointerToRpcPayload(mpRpcAnsStack_O);

		for (i_U32 = 0; i_U32 < pRpcFct_X->NbArg_U16; i_U32++)
		{
			VarType_E = (BOF_RPC_VAR_TYPE) (pRpcFct_X->pArg_U16[i_U32] & BOF_RPC_VARTYPEMASK);
			RpcOut_B = ((pRpcFct_X->pArg_U16[i_U32] & BOF_RPC_OUT)
			            || (pRpcFct_X->pArg_U16[i_U32] & BOF_RPC_INOUT)
			);

			switch (VarType_E)
			{
				case BOF_RPC_VAR_VOID:
				{
					Sts_B = true;
				}
					break;

				case BOF_RPC_VAR_CHAR:
				{
					Sts_B = (RpcOut_B) ? mpRpcAnsStack_O->Push(_pRpcParserResult_X->pRpcVar_X[i_U32].VALUE.pVal_c) : true;
				}
					break;

				case BOF_RPC_VAR_U8:
				case BOF_RPC_VAR_S8:
				{
					Sts_B = (RpcOut_B) ? mpRpcAnsStack_O->Push(_pRpcParserResult_X->pRpcVar_X[i_U32].VALUE.Val_U8) : true;
				}
					break;

				case BOF_RPC_VAR_U16:
				case BOF_RPC_VAR_S16:
				{
					Sts_B = (RpcOut_B) ? mpRpcAnsStack_O->Push(_pRpcParserResult_X->pRpcVar_X[i_U32].VALUE.Val_U16) : true;
				}
					break;

				case BOF_RPC_VAR_U32:
				case BOF_RPC_VAR_S32:
				{
					Sts_B = (RpcOut_B) ? mpRpcAnsStack_O->Push(_pRpcParserResult_X->pRpcVar_X[i_U32].VALUE.Val_U32) : true;
				}
					break;

				case BOF_RPC_VAR_U64:
				case BOF_RPC_VAR_S64:
				{
					Sts_B = (RpcOut_B) ? mpRpcAnsStack_O->Push(_pRpcParserResult_X->pRpcVar_X[i_U32].VALUE.Val_U64) : true;
				}
					break;

				case BOF_RPC_VAR_FLOAT:
				{
					Sts_B = (RpcOut_B) ? mpRpcAnsStack_O->Push(_pRpcParserResult_X->pRpcVar_X[i_U32].VALUE.Val_f) : true;
				}
					break;

				case BOF_RPC_VAR_DOUBLE:
				{
					Sts_B = (RpcOut_B) ? mpRpcAnsStack_O->Push(_pRpcParserResult_X->pRpcVar_X[i_U32].VALUE.Val_ff) : true;
				}
					break;

				case BOF_RPC_VAR_ARRAY:
				{
					Sts_B = (RpcOut_B) ? Push(mpRpcAnsStack_O, true, &_pRpcParserResult_X->pRpcVar_X[i_U32].VALUE.Val_X) : true;
				}
					break;

				default:
					Sts_B = false;
					break;
			}

			if (!Sts_B)
			{
				break;
			}
		}

		if (Sts_B)
		{
			Sts_B = false;
			Flag_U16 = 0;

			if (mBofRpcParam_X.UseChecksum_B)
			{
				Flag_U16 |= BOF_RPC_FLAG_USECHECKSUM;
			}

			if (mpRpcAnsStack_O->Push(static_cast< uint16_t > (_pRpcParserResult_X->RpcRts_U32)))
			{
				if (mpRpcAnsStack_O->Push(static_cast< uint16_t > (_pRpcParserResult_X->FctId_U32)))
				{
					if (mBofRpcParam_X.UseChecksum_B)
					{
						if (PushTopRpcControlParam(mpRpcAnsStack_O, BOF_RPCMAGICNUMBER, Flag_U16, 0, _pRpcParserResult_X->RpcTag_U16, mpRpcAnsStack_O->GetStackPointer() + 2))
						{
							Cs_U32 = 0;

							for (i_U32 = 0; i_U32 < mpRpcAnsStack_O->GetStackPointer(); i_U32++)
							{
								Cs_U32 += _pRpcParserResult_X->pRpcStack_U8[i_U32];
							}
							Sts_B = mpRpcAnsStack_O->Push(static_cast< uint16_t > (Cs_U32));
						}
					}
					else
					{
						Sts_B = true;
					}

					if (Sts_B)
					{
						_pRpcParserResult_X->RpcStackLen_U32 = mpRpcAnsStack_O->GetStackPointer();

						if (PushTopRpcControlParam(mpRpcAnsStack_O, BOF_RPCMAGICNUMBER, Flag_U16, 0, _pRpcParserResult_X->RpcTag_U16, _pRpcParserResult_X->RpcStackLen_U32)) // Push size
						{
							Rts_U32 = BOF_ERR_NO_ERROR;
						}
					}
				}
			}
		}
	}
	return Rts_U32;
}


/*** ParseRpcFrame ********************************************************************/

/*!
 * Description
 * The ParseRpcRequest method parse e received request or answer rpc frame
 *
 * Parameters
 * _RpcAnswer_B:           true if the frame is a rpc result frame (false for rpc request)
 * _pRpcFrameBuffer_U8:    Specify the Rpc stack frame buffer
 * _pTag_U16:             if not null, specify the expected Rpc answer tag value if _RpcAnswer_B is true (0 ignore) and returns it to caller
 * _pRpcParserResult_X:     \Returns Specifies the characteristics of the rpc data stored in _pRpcFrameBuffer_U8
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 *      None
 */

uint32_t BofRpc::ParseRpcFrame(bool _RpcAnswer_B, uint8_t *_pRpcFrameBuffer_U8, BOF_RPC_PARSER_RESULT *_pRpcParserResult_X)
{
	uint32_t Rts_U32 = BOF_ERR_EINVAL, RpcFrameSizeInByte_U32 = 0, TheCs_U32, Cs_U32;
	uint32_t Nb_U32;
	int32_t i_S32;
	uint16_t Sof_U16, Flag_U16, TimeOutInMs_U16, TheTag_U16, ExpectedTag_U16, TheCs_U16, FctId_U16, RpcRts_U16;
	bool Sts_B = false, LittleEndian_B, MustSwap_B, RpcParam_B;
	BOF_RPC_FUNCTION *pRpcFct_X = nullptr;
	BofStack *pRpcReqStack_O = nullptr;

	if ((_pRpcFrameBuffer_U8)
	    && (_pRpcParserResult_X)
	    && (mpRpcReqStack_O)
	    && (mpRpcAnsStack_O)
		)
	{
		if (_RpcAnswer_B)
		{
			pRpcReqStack_O = mpRpcAnsStack_O;
		}
		else
		{
			pRpcReqStack_O = mpRpcReqStack_O;
			_pRpcParserResult_X->Reset();
		}
		_pRpcParserResult_X->pRpcStack_U8 = pRpcReqStack_O->GetStackBuffer();

		Flag_U16 = 0;
		TheTag_U16 = 0;
		memcpy(_pRpcParserResult_X->pRpcStack_U8, _pRpcFrameBuffer_U8, 16);                         // Put rpc header on stack
		pRpcReqStack_O->SetStackPointer(16);

		if (IsRpcLittleEndian(_pRpcParserResult_X->pRpcStack_U8, &LittleEndian_B, &MustSwap_B) == BOF_ERR_NO_ERROR)
		{
			pRpcReqStack_O->SetSwapByte(MustSwap_B);
			PopTopRpcControlParam(pRpcReqStack_O, &Sof_U16, &Flag_U16, &TimeOutInMs_U16, &TheTag_U16, &RpcFrameSizeInByte_U32);
			_pRpcParserResult_X->TimeOut_U32 = TimeOutInMs_U16;
			Rts_U32 = BOF_ERR_TOO_BIG;

			if ((Sof_U16 == BOF_RPCMAGICNUMBER)
			    && (RpcFrameSizeInByte_U32 <= pRpcReqStack_O->GetStackSize())
				)
			{
				_pRpcParserResult_X->RpcStackLen_U32 = RpcFrameSizeInByte_U32;
				memcpy(_pRpcParserResult_X->pRpcStack_U8, _pRpcFrameBuffer_U8, RpcFrameSizeInByte_U32); // Copy all Rpc header on stack
				pRpcReqStack_O->SetStackPointer(RpcFrameSizeInByte_U32);

				Sts_B = true;

				if (Flag_U16 & BOF_RPC_FLAG_USECHECKSUM)
				{
					Sts_B = false;
					Cs_U32 = 0;

					if (pRpcReqStack_O->Pop(&TheCs_U16))
					{
						TheCs_U32 = TheCs_U16;
						for (i_S32 = 0; i_S32 < (int32_t) pRpcReqStack_O->GetStackPointer(); i_S32++)
						{
							Cs_U32 += _pRpcParserResult_X->pRpcStack_U8[i_S32];
						}

						Rts_U32 = BOF_ERR_INVALID_CHECKSUM;

						if (Cs_U32 == TheCs_U32)
						{
							Sts_B = true;
						}
					}
				}
				ExpectedTag_U16 = _pRpcParserResult_X->RpcTag_U16;

				_pRpcParserResult_X->RpcTag_U16 = TheTag_U16;

				if ((_RpcAnswer_B)
				    && (ExpectedTag_U16)
					)
				{
					Sts_B = false;
					Rts_U32 = BOF_ERR_NOT_FOUND;

					if (TheTag_U16 == ExpectedTag_U16)
					{
						Sts_B = true;
					}
				}

				if (Sts_B)
				{
					Sts_B = false;

					if (pRpcReqStack_O->Pop(&FctId_U16))
					{
						_pRpcParserResult_X->FctId_U32 = FctId_U16;
						Rts_U32 = BOF_ERR_INVALID_COMMAND;

						if (_pRpcParserResult_X->FctId_U32 < BOF_RPC_MAXFCT)
						{
							pRpcFct_X = &mBofRpcParam_X.pRpcFct_X[_pRpcParserResult_X->FctId_U32];

// memcpy(_pRpcParserResult_X->pRpcFct_X,pRpcFct_X,sizeof(BOFRPCFUNCTION));
							if (pRpcReqStack_O->Pop(&RpcRts_U16))
							{
								_pRpcParserResult_X->RpcRts_U32 = RpcRts_U16;
								Sts_B = true;
							}
						}
					}
				}

				if ((Sts_B) && (pRpcFct_X))
				{
					for (i_S32 = pRpcFct_X->NbArg_U16 - 1; i_S32 >= 0; i_S32--)
					{
						if (_RpcAnswer_B)
						{
							RpcParam_B = ((pRpcFct_X->pArg_U16[i_S32] & BOF_RPC_OUT)
							              || (pRpcFct_X->pArg_U16[i_S32] & BOF_RPC_INOUT)
							);
						}
						else
						{
							RpcParam_B = ((pRpcFct_X->pArg_U16[i_S32] & BOF_RPC_IN)
							              || (pRpcFct_X->pArg_U16[i_S32] & BOF_RPC_INOUT)
							);
						}
						_pRpcParserResult_X->pRpcVar_X[i_S32].Arg_U16 = pRpcFct_X->pArg_U16[i_S32];
						_pRpcParserResult_X->pRpcVar_X[i_S32].Type_E = (BOF_RPC_VAR_TYPE) (pRpcFct_X->pArg_U16[i_S32] & BOF_RPC_VARTYPEMASK);

						switch (_pRpcParserResult_X->pRpcVar_X[i_S32].Type_E)
						{
							case BOF_RPC_VAR_VOID:
							{
								Sts_B = true;
							}
								break;

							case BOF_RPC_VAR_CHAR:
							{
								Sts_B = (RpcParam_B) ? pRpcReqStack_O->Pop(_pRpcParserResult_X->pRpcVar_X[i_S32].VALUE.pVal_c) : true;
							}
								break;

							case BOF_RPC_VAR_U8:
							case BOF_RPC_VAR_S8:
							{
								Sts_B = (RpcParam_B) ? pRpcReqStack_O->Pop(&_pRpcParserResult_X->pRpcVar_X[i_S32].VALUE.Val_U8) : true;
							}
								break;

							case BOF_RPC_VAR_U16:
							case BOF_RPC_VAR_S16:
							{
								Sts_B = (RpcParam_B) ? pRpcReqStack_O->Pop(&_pRpcParserResult_X->pRpcVar_X[i_S32].VALUE.Val_U16) : true;
							}
								break;

							case BOF_RPC_VAR_U32:
							case BOF_RPC_VAR_S32:
							{
								Sts_B = (RpcParam_B) ? pRpcReqStack_O->Pop(&_pRpcParserResult_X->pRpcVar_X[i_S32].VALUE.Val_U32) : true;
							}
								break;

							case BOF_RPC_VAR_FLOAT:
							{
								Sts_B = (RpcParam_B) ? pRpcReqStack_O->Pop(&_pRpcParserResult_X->pRpcVar_X[i_S32].VALUE.Val_f) : true;
							}
								break;

							case BOF_RPC_VAR_DOUBLE:
							{
								Sts_B = (RpcParam_B) ? pRpcReqStack_O->Pop(&_pRpcParserResult_X->pRpcVar_X[i_S32].VALUE.Val_ff) : true;
							}
								break;

							case BOF_RPC_VAR_ARRAY:
							{
								Sts_B = (RpcParam_B) ? Pop(pRpcReqStack_O, true, &_pRpcParserResult_X->pRpcVar_X[i_S32].VALUE.Val_X, &Nb_U32) : true;
							}
								break;
							default:
								Sts_B = false;
								break;
						}

						if (!Sts_B)
						{
							break;
						}
					}
				}
			}
		}

// if (Rts_U32==BOF_ERR_NO_ERROR)
		if (_RpcAnswer_B)
		{
			memset(mRpcState_X.pAnsRpcStack_U8, 0, sizeof(mRpcState_X.pAnsRpcStack_U8));
			mRpcState_X.AnsFctId_U32 = _pRpcParserResult_X->FctId_U32;
			mRpcState_X.AnsRpcRts_U32 = _pRpcParserResult_X->RpcRts_U32;
			mRpcState_X.AnsRpcStackLen_U32 = _pRpcParserResult_X->RpcStackLen_U32;
			mRpcState_X.AnsTimeOut_U32 = _pRpcParserResult_X->TimeOut_U32;
			mRpcState_X.AnsTag_U16 = TheTag_U16;
			mRpcState_X.AnsFlag_U16 = Flag_U16;
			memcpy(mRpcState_X.pAnsRpcStack_U8, _pRpcParserResult_X->pRpcStack_U8,
			       (mRpcState_X.AnsRpcStackLen_U32 < sizeof(mRpcState_X.pAnsRpcStack_U8)) ? mRpcState_X.AnsRpcStackLen_U32 : sizeof(mRpcState_X.pAnsRpcStack_U8));
		}
		else
		{
			memset(mRpcState_X.pReqRpcStack_U8, 0, sizeof(mRpcState_X.pReqRpcStack_U8));
			mRpcState_X.ReqFctId_U32 = _pRpcParserResult_X->FctId_U32;
			mRpcState_X.ReqRpcRts_U32 = _pRpcParserResult_X->RpcRts_U32;
			mRpcState_X.ReqRpcStackLen_U32 = _pRpcParserResult_X->RpcStackLen_U32;
			mRpcState_X.ReqTimeOut_U32 = _pRpcParserResult_X->TimeOut_U32;
			mRpcState_X.ReqTag_U16 = TheTag_U16;
			mRpcState_X.ReqFlag_U16 = Flag_U16;
			memcpy(mRpcState_X.pReqRpcStack_U8, _pRpcParserResult_X->pRpcStack_U8,
			       (mRpcState_X.ReqRpcStackLen_U32 < sizeof(mRpcState_X.pReqRpcStack_U8)) ? mRpcState_X.ReqRpcStackLen_U32 : sizeof(mRpcState_X.pReqRpcStack_U8));
		}

// if (Rts_U32==BOF_ERR_NO_ERROR)
	}

	if ((Sts_B) && (pRpcReqStack_O))
	{
		Rts_U32 = CheckAndResetStackPointerToRpcControlSize(pRpcReqStack_O);
	}

	return Rts_U32;
}


/*** IsRpcLittleEndian ********************************************************************/

/*!
 * Description
 * The BuildRpcAnswer method build a RPC answer frame
 *
 * Parameters
 * _pRpcFrameBuffer_U8: Specify the Rpc stack frame buffer
 * _pLittleEndian_B: \Return true if the _pRpcFrameBuffer_U8 contains little endian data
 * _pMustSwap_B:  \Return true if the Rpc frame endianess is not the same than the Rpc cpu
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * None
 */
uint32_t BofRpc::IsRpcLittleEndian(uint8_t *_pRpcFrameBuffer_U8, bool *_pLittleEndian_B, bool *_pMustSwap_B)
{
	uint32_t Rts_U32 = BOF_ERR_EINVAL;

	if ((_pRpcFrameBuffer_U8)
	    && (_pLittleEndian_B)
	    && (_pMustSwap_B)
		)
	{
		Rts_U32 = BOF_ERR_NO_ERROR;

		if ((_pRpcFrameBuffer_U8[0] == (BOF_RPCMAGICNUMBER >> 8))
		    && (_pRpcFrameBuffer_U8[1] == (uint8_t) (BOF_RPCMAGICNUMBER & 0xFF))
			)
		{
			*_pLittleEndian_B = false;
			*_pMustSwap_B = mLittleEndian_B;
		}
		else if ((_pRpcFrameBuffer_U8[1] == (BOF_RPCMAGICNUMBER >> 8))
		         && (_pRpcFrameBuffer_U8[0] == (uint8_t) (BOF_RPCMAGICNUMBER & 0xFF))
			)
		{
			*_pLittleEndian_B = true;
			*_pMustSwap_B = !mLittleEndian_B;
		}
		else
		{
			Rts_U32 = BOF_ERR_INTERNAL;
		}
	}
	return Rts_U32;
}


/*** WaitForRpcFrame ********************************************************************/

/*!
 * Description
 * The WaitForRpcFrame method scan a  BofComChannel to get an incoming rpc request
 *
 * Parameters
 * _pBofComChannel_O: Specify the Rpc stack frame buffer
 * _TimeoutInMs_U32: Specify the maximum time allowed to proces teh request
 * _pMaxRpcFrameSizeInByte_U32: Specify the Rpc max frame size and return the resulting rpc frame size (_pRpcFrame_U8)
 * _pRpcFrame_U8: Specify where to store the resulting buffer
 *
 * Returns
 * uint32_t: BOF_ERR_NO_ERROR if the operation is successful
 *
 * Remarks
 * None
 */
uint32_t BofRpc::WaitForRpcFrame(BofComChannel *_pBofComChannel_O, uint32_t _TimeoutInMs_U32, uint32_t *_pMaxRpcFrameSizeInByte_U32, uint8_t *_pRpcFrame_U8)
{
	uint32_t Rts_U32 = BOF_ERR_EINVAL, Nb_U32, RpcFrameSizeInByte_U32 = 0, HeaderLen_U32;
	uint16_t Sof_U16, Flag_U16, TimeOutInMs_U16, TheTag_U16;
	uint8_t *pStack_U8;
	bool LittleEndian_B, MustSwap_B;


	if ((_pBofComChannel_O)
	    && (_pMaxRpcFrameSizeInByte_U32)
	    && (_pRpcFrame_U8)
	    && (mpRpcReqStack_O)
		)
	{
		HeaderLen_U32 = 2 + 2 + 2 + 2 + 2;                      // Read Sof,Flag,To,Tag and Stack len
		Nb_U32 = HeaderLen_U32;
		Rts_U32 = _pBofComChannel_O->V_ReadData(_TimeoutInMs_U32, Nb_U32, _pRpcFrame_U8);

		if ((Rts_U32 == BOF_ERR_NO_ERROR)
		    && (Nb_U32 == HeaderLen_U32)
			)
		{
			pStack_U8 = mpRpcReqStack_O->GetStackBuffer();
			memcpy(pStack_U8, _pRpcFrame_U8, Nb_U32);             // Put rpc header on stack
			mpRpcReqStack_O->SetStackPointer(Nb_U32);

			if (IsRpcLittleEndian(pStack_U8, &LittleEndian_B, &MustSwap_B) == BOF_ERR_NO_ERROR)
			{
				mpRpcReqStack_O->SetSwapByte(MustSwap_B);
				PopTopRpcControlParam(mpRpcReqStack_O, &Sof_U16, &Flag_U16, &TimeOutInMs_U16, &TheTag_U16, &RpcFrameSizeInByte_U32);
				Rts_U32 = BOF_ERR_TOO_BIG;

				if ((Sof_U16 == BOF_RPCMAGICNUMBER)
				    && (RpcFrameSizeInByte_U32 <= mpRpcReqStack_O->GetStackSize())
					)
				{
					Nb_U32 = RpcFrameSizeInByte_U32 - HeaderLen_U32; // Read Stack
					Rts_U32 = _pBofComChannel_O->V_ReadData(_TimeoutInMs_U32, Nb_U32, &_pRpcFrame_U8[HeaderLen_U32]);

					if (Rts_U32 == BOF_ERR_NO_ERROR)
					{
						if (Nb_U32 != (RpcFrameSizeInByte_U32 - HeaderLen_U32))
						{
							Rts_U32 = BOF_ERR_TOO_BIG;
						}
					}
				}
			}
		}
	}
	return Rts_U32;
}


/*** CheckAndResetStackPointerToRpcControlSize ********************************************************************/

/*!
 * Description
 * The CheckAndResetStackPointerToRpcControlSize method checks the RPC stack for stack underflow or overflow
 *
 * Parameters
 * None
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * When all the argument have been popped out it must remain a byte that
 * contains the original stack size.
 */
uint32_t BofRpc::CheckAndResetStackPointerToRpcControlSize(BofStack *_pStack)
{
	uint32_t Rts_U32 = BOF_ERR_EINVAL;

	if (_pStack)
	{
		_pStack->LockStack();
		Rts_U32 = BOF_ERR_NO_ERROR;
		if (_pStack->GetStackPointer() != (2 + 2 + 2 + 2 + 2)) // 2+2+2: Sof,Flag,To,Tag
		{
			_pStack->SetStackPointer(2 + 2);
			Rts_U32 = BOF_ERR_FORMAT;
		}
		_pStack->UnlockStack();
	}
	return Rts_U32;
}


/*** ResetStackPointerToRpcPayload ********************************************************************/

/*!
 * Description
 * The ResetStackPointerToRpcPayload method reset the stack pointer to Rpc payload offset
 *
 * Parameters
 * None
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
uint32_t BofRpc::ResetStackPointerToRpcPayload(BofStack *_pStack)
{
	uint32_t Rts_U32 = BOF_ERR_EINVAL;

	if (_pStack)
	{
		_pStack->LockStack();
		Rts_U32 = BOF_ERR_NO_ERROR;
		_pStack->SetStackPointer(2 + 2 + 2 + 2 + 2);            // 2+2+2: Sof,Flag,To,Tag,Len
		_pStack->UnlockStack();
	}
	return Rts_U32;
}

/*** Push ********************************************************************/

/*!
 * Description
 * The Push method writes (pushs) a given number of bytes on the stack
 *
 * Parameters
 * PushData_B :  true if array data must be pushed on the stack
 * pRpcArray:  Specify the array to push
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * Array data are copied first followed by a long value containing the array
 * length.
 */
bool BofRpc::Push(BofStack *_pStack, bool PushData_B, BOF_RPC_VAR_ARRAYINOUT pRpcArray)
{
	bool Rts_B = false;
	uint32_t Nb_U32;

	if (_pStack)
	{
		_pStack->LockStack();

		Nb_U32 = pRpcArray->NbItem_U32 * pRpcArray->ItemSize_U32;

		if ((_pStack->GetStackPointer() + Nb_U32) <= _pStack->GetStackSize())
		{
			if (PushData_B)
			{
				memcpy(_pStack->GetCurrentStackBufferLocation(), pRpcArray->pItem, Nb_U32);
				_pStack->AdjustStackBufferLocation(Nb_U32);
			}

			if (_pStack->Push(static_cast< uint16_t > (pRpcArray->ItemSize_U32)))
			{
				Rts_B = _pStack->Push(static_cast< uint16_t > (pRpcArray->NbItem_U32));
			}
		}
		_pStack->UnlockStack();
	}
	return Rts_B;
}

/*** PushTopRpcControlParam ********************************************************************/

/*!
 * Description
 * The PushTop method writes (push) a variable on the Rpc stack. The Push is made on
 * the top of the stack (location 0) whatever the current stack pointer value is
 *
 * Parameters
 * Sof_U16:       Specify the start of frame value (It is also used to detect endianess)
 * Flags_U16:       Specify the rpc frame flag
 * TimeOutInMs_U16: Specify command execution timeout in ms
 * Tag_U16:       Specify the rpc tag value
 * Val_U32 :      Specifies the value to be pushed on the stack
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
bool BofRpc::PushTopRpcControlParam(BofStack *_pStack, uint16_t Sof_U16, uint16_t Flags_U16, uint16_t TimeOutInMs_U16, uint16_t Tag_U16, uint32_t Val_U32)
{
	bool Rts_B = false;
	uint32_t Ptr_U32;

	if (_pStack)
	{
		_pStack->LockStack();
		Ptr_U32 = _pStack->GetStackPointer();
		_pStack->SetStackPointer(0);
		Rts_B = _pStack->Push(Sof_U16);

		if (Rts_B)
		{
			Rts_B = _pStack->Push(Flags_U16);

			if (Rts_B)
			{
				Rts_B = _pStack->Push(TimeOutInMs_U16);

				if (Rts_B)
				{
					Rts_B = _pStack->Push(Tag_U16);

					if (Rts_B)
					{
						Rts_B = _pStack->Push(static_cast< uint16_t > (Val_U32));
					}
				}
			}
		}
		_pStack->SetStackPointer(Ptr_U32);
		_pStack->UnlockStack();
	}
	return Rts_B;
}

/*** Pop ********************************************************************/

/*!
 * Description
 * The Pop method reads (pops) a byte array variable
 * from the stack
 *
 * Parameters
 * PopData_B :  true if array data must be poped from the stack
 * pRpcArray:  \Return the array data
 * pNb_U32:     \Returns the number of byte poped from the stack
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * Array buffer is dynamically allocated by this function and must be
 * released when the caller doesn't need it anymore (BOF_RPC_DELETEARRAY)
 */
bool BofRpc::Pop(BofStack *_pStack, bool PopData_B, BOF_RPC_VAR_ARRAYINOUT pRpcArray, uint32_t *pNb_U32)
{
	bool Rts_B = false;
	uint32_t Nb_U32;

	if (_pStack)
	{
		pRpcArray->NbItem_U32 = 0;
		*pNb_U32 = 0;

		_pStack->LockStack();
		if (_pStack->Pop(&pRpcArray->NbItem_U32))
		{
			if (_pStack->Pop(&pRpcArray->ItemSize_U32))
			{
				Nb_U32 = pRpcArray->NbItem_U32 * pRpcArray->ItemSize_U32;
				pRpcArray->pItem = new uint8_t[Nb_U32];

				if (pRpcArray->pItem)
				{
					if (PopData_B)
					{
						_pStack->AdjustStackBufferLocation(Nb_U32);
						memcpy(pRpcArray->pItem, _pStack->GetCurrentStackBufferLocation(), Nb_U32);
					}
					*pNb_U32 = Nb_U32 + 2 + 2 + static_cast<uint32_t>(sizeof(void *));
					Rts_B = true;
				}
			}
		}
		_pStack->UnlockStack();
	}
	return Rts_B;
}


/*** PopTopRpcControlParam ********************************************************************/

/*!
 * Description
 * The PopTop method reads (pop) a variable from the Rpc stack. The Pop is made on
 * the top of the stack (location 0) whatever the current stack pointer value is
 *
 * Parameters
 * pSof_U16:       \Returns the start of frame value (It is also used to detect endianess)
 * pFlags_U16:	\Returns frame flags
 * pTimeOutInMs_U16:       \Returns the command execution timeout in ms
 * pTag_U16:       \Returns the rpc tag value
 * pVal_U32 :      \Returns the value poped from the stack
 *
 * Returns
 * bool: true if the operation is successful
 *
 * Remarks
 * None
 */
bool BofRpc::PopTopRpcControlParam(BofStack *_pStack, uint16_t *pSof_U16, uint16_t *pFlags_U16, uint16_t *pTimeOutInMs_U16, uint16_t *pTag_U16, uint32_t *pVal_U32)
{
	bool Rts_B = false;
	uint32_t Ptr_U32;
	uint16_t Val_U16;

	if ((_pStack) && (pVal_U32))
	{
		_pStack->LockStack();
		Ptr_U32 = _pStack->GetStackPointer();
		_pStack->SetStackPointer(2 + 2 + 2 + 2 + 2); // 2+2+2: Sof,Flag,To,Tag
		Rts_B = _pStack->Pop(&Val_U16);

		if (Rts_B)
		{
			*pVal_U32 = Val_U16;
			Rts_B = _pStack->Pop(pTag_U16);

			if (Rts_B)
			{
				Rts_B = _pStack->Pop(pTimeOutInMs_U16);

				if (Rts_B)
				{
					Rts_B = _pStack->Pop(pFlags_U16);

					if (Rts_B)
					{
						Rts_B = _pStack->Pop(pSof_U16);
					}
				}
			}
		}
		_pStack->SetStackPointer(Ptr_U32);
		_pStack->UnlockStack();
	}
	return Rts_B;
}
END_BOF_NAMESPACE()