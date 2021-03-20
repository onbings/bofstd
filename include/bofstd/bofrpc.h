/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * Name:        BofRpc.h
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
 * The goal of the Remote Procedure Call (RPC) system is to enable distant execution of
 * a function. A cpu (client) sends a request to another one (server). This request contains
 * the function id to execute and its associated parameters (argument).
 * The caller sends these data using a communication channel, the target receives them,
 * process the RPC request and returns the results (arguments) and its status.
 *
 * Each parameter (argument) of the remote function is pushed on a 'virtual' stack by means of Push
 * functions. The virtual stack is a array of byte which is sent to the remote computer using our
 * RPC protocol. Some extra word are added to this stack before transmission:
 *
 * Cmd: Send by client to server:
 *    Item Name                    Size
 *    0    Sof/Endian              Word (Start of Frame and endianess detector): BOF_RPCMAGICNUMBER
 *    1    Flag                    Word Frame flags (use checksum,...)
 *    2    TimeOutInMs             Word Cmd Timeout exec
 *    3    Tag                     Word Cmd Id (unique id linked to this command and used to tag the rpc answer) CAN'T be zero !
 *    4    stack len							 Word: total number of byte including from item 0 up to k+6
 *    5    In and InOut Arg 1      x
 *    6    In and InOut Arg 2      x
 *
 *    k+4  In and InOut Arg k      x
 *
 *    k+5  Default function return value    Word
 *    k+6  Function Id                      Word
 *    k+7  Mtx from 0 to n+1                 Word
 *
 *    - If 'UseChecksum_B' is false item k+7 is not computed,checked or inserted
 *
 * Ans: Send by server to client:
 *    Item Name                    Size
 *    0    Sof/Endian              Word (Start of Frame and endianess detector): BOF_RPCMAGICNUMBER
 *    1    Flag                    Word Frame flags (use checksum,...)
 *    2    NotUsed                 Word Cmd Timeout exec
 *    3    Tag                     Word Answer Id (unique id linked to this answer and used to link it to its rpc command) CAN'T be zero !
 *    4    stack len (n)           Word total number of byte including from item 0 up to k+6
 *    5    Out and InOut Arg 1     x
 *    6    Out and InOut Arg 2     x
 *
 *    k+4  Out and InOut Arg k     x
 *
 *    k+5  Default function return value    Word
 *    k+6  Function Id                      Word
 *    k+7  Mtx from 0 to n+1                 Word
 *
 *    - If 'UseChecksum_B' is false item k+7 is not computed,checked or inserted
 *
 * The remote system pops each parameter using Pop function, executes the request and sends back
 * results to the caller by using also a virtual stack. This one is received by the client and the
 * results are poped and returned back to the caller.
 */

/*** Include **********************************************************/
#pragma once

#include <bofstd/bofstd.h>
#include <bofstd/bofstack.h>
#include <bofstd/bofcomchannel.h>
#include <string.h>

BEGIN_BOF_NAMESPACE()

/*** Defines ****************************************************************/

#define BOF_RPC_MAXFCT           256    /*!<Max number of Rpc function*/
#define BOF_RPC_MAXFCTARG        16     /*!<Max number of argument per RPC function*/
#define BOF_RPC_VARTYPEMASK      0xFF
#define BOF_RPC_MAXSTRINGSIZE    256
#define BOF_RPC_MAXHELPSIZE      64
#define BOF_RPCIN
#define BOF_RPCINOUT             *
#define BOF_RPCOUT               *
#define BOF_RPCMAGICNUMBER       0x4248 // BH

/*** Macro ******************************************************************/
/*
   #define BOF_RPC_ALLOCATEARRAY(Var, NbItem, ItemSize)																																																\
   { Var = new BOF_RPC_VAR_BINARY; if (Var) { Var->pItem = new uint8_t[NbItem * ItemSize]; Var->ItemSize_U32 = ItemSize; Var->NbItem_U32 = NbItem; }	\
   }
   #define BOF_RPC_SETARRAYENTRY(Var, Index, pVal)    { memcpy( (uint8_t *)Var->pItem + (Var->ItemSize_U32 * Index), pVal, Var->ItemSize_U32); }
   #define BOF_RPC_GETARRAYENTRY(Var, Index, pVal)    { memcpy(pVal, (uint8_t *)Var->pItem + (Var->ItemSize_U32 * Index), Var->ItemSize_U32); }
   #define BOF_RPC_DELETEARRAY(Var)                   { BOF_SAFE_DELETE(Var->pItem); }
 */
/*** enum *******************************************************************/

typedef enum
{
		BOF_RPC_VAR_UNDEF = 0,
		BOF_RPC_VAR_VOID,
		BOF_RPC_VAR_CHAR,
		BOF_RPC_VAR_U8,
		BOF_RPC_VAR_S8,
		BOF_RPC_VAR_U16,
		BOF_RPC_VAR_S16,
		BOF_RPC_VAR_U32,
		BOF_RPC_VAR_S32,
		BOF_RPC_VAR_U64,
		BOF_RPC_VAR_S64,
		BOF_RPC_VAR_SLONG,
		BOF_RPC_VAR_FLOAT,
		BOF_RPC_VAR_DOUBLE,
		BOF_RPC_VAR_ARRAY,
		RPCVARTYPE_ALIGNON32BITS = 0xFFFFFFFF
}
	BOF_RPC_VAR_TYPE;

typedef enum
{
		BOF_RPC_IN = 0x0100,
		BOF_RPC_OUT = 0x0200,
		BOF_RPC_INOUT = 0x0400,
		BOF_RPCQUALIFIER_ALIGNON32BITS = 0xFFFFFFFF
}
	BOFRPCQUALIFIER;

typedef enum
{
		BOF_RPC_FLAG_USECHECKSUM = 0x0001,
		BOF_RPCFLAG_ALIGNON16BITS = 0xFFFF
}
	BOFRPCFLAG;
typedef struct
{
		uint32_t ItemSize_U32;
		uint32_t NbItem_U32;
		void *pItem;                  // This item must be the last one
}
	BOF_RPC_VAR_BINARY, *BOF_RPC_VAR_ARRAYIN, *BOF_RPC_VAR_ARRAYOUT, *BOF_RPC_VAR_ARRAYINOUT;
/*** struct *******************************************************************/

typedef void BOFRPCVOID;
typedef uint8_t BOFRPCU8;
typedef int8_t BOFRPCSBYTE;
typedef uint16_t BOFRPCU16;
typedef int16_t BOFRPCSWORD;
typedef uint32_t BOFRPCU32;
typedef int32_t BOFRPCSLONG;
typedef char *BOFRPCSTRING;
typedef float BOFRPCFLOAT;
typedef double BOFRPCDOUBLE;
typedef void *BOFRPCSERVERARG;

#define BOFRPCSTRING(Var, Len)    char Var[Len]

#pragma pack(1)


struct BOF_RPC_FUNCTION
{
		uint16_t NbArg_U16;
		uint16_t pArg_U16[BOF_RPC_MAXFCTARG];

		BOF_RPC_FUNCTION()
		{
			Reset();
		}

		BOF_RPC_FUNCTION(uint16_t _NbArg_U16, uint16_t a1, uint16_t a2, uint16_t a3, uint16_t a4, uint16_t a5, uint16_t a6, uint16_t a7, uint16_t a8, uint16_t a9, uint16_t a10, uint16_t a11, uint16_t a12,
		                 uint16_t a13, uint16_t a14, uint16_t a15, uint16_t a16)
		{
			NbArg_U16 = _NbArg_U16;
			pArg_U16[0] = a1;
			pArg_U16[1] = a2;
			pArg_U16[2] = a3;
			pArg_U16[3] = a4;
			pArg_U16[4] = a5;
			pArg_U16[5] = a6;
			pArg_U16[6] = a7;
			pArg_U16[7] = a8;
			pArg_U16[8] = a9;
			pArg_U16[9] = a10;
			pArg_U16[10] = a11;
			pArg_U16[11] = a12;
			pArg_U16[12] = a13;
			pArg_U16[13] = a14;
			pArg_U16[14] = a15;
			pArg_U16[15] = a16;
		}

		void Reset()
		{
			NbArg_U16 = 0;
			memset(pArg_U16, 0, sizeof(pArg_U16));
		}
};

struct BOFRPCVAR
{
		union
		{
				void *pVal;
				uint8_t Val_U8;
				int8_t Val_S8;
				uint16_t Val_U16;
				int16_t Val_S16;
				uint32_t Val_U32;
				int32_t Val_S32;
				uint64_t Val_U64;
				int64_t Val_S64;
				float Val_f;
				double Val_ff;
				char pVal_c[BOF_RPC_MAXSTRINGSIZE];
				BOF_RPC_VAR_BINARY Val_X;
		} VALUE;
		BOF_RPC_VAR_TYPE Type_E;
		uint16_t Arg_U16;

		BOFRPCVAR()
		{
			Reset();
		}

		void Reset()
		{
			VALUE.Val_S64 = 0;
			memset(VALUE.pVal_c, 0, sizeof(VALUE.pVal_c));

			Type_E = BOF_RPC_VAR_UNDEF;
			Arg_U16 = 0;
		}
};

struct BOF_RPC_PARSER_RESULT
{
		uint32_t FctId_U32;
		uint32_t RpcRts_U32;
		uint32_t RpcStackLen_U32;
		uint32_t TimeOut_U32;
		uint16_t RpcTag_U16;
		uint8_t *pRpcStack_U8;
		BOFRPCVAR pRpcVar_X[BOF_RPC_MAXFCTARG];

		BOF_RPC_PARSER_RESULT()
		{
			Reset();
		}

		void Reset()
		{
			uint32_t i_U32;

			FctId_U32 = 0;
			RpcRts_U32 = 0;
			RpcStackLen_U32 = 0;
			TimeOut_U32 = 0;
			RpcTag_U16 = 0;
			*pRpcStack_U8 = 0;
			for (i_U32 = 0; i_U32 < BOF_RPC_MAXFCTARG; i_U32++)
			{
				pRpcVar_X[i_U32].Reset();
			}
		}
};

// class BofRpc;  //to be deleted

struct BOF_RPC_PARAM
{
		BOFERR ErrorCode_E;        /*! Return an optional error code */
		bool MultiThreadAware_B;
		uint32_t MaxStackSize_U32;   /*! Specify the rpc stack max size*/
		uint32_t UseChecksum_B;      /*! True if checksum computaion, insertion and control must be used during transfert: Not bool to be sure of structure size and aligment on different cpu*/
		uint32_t NbRpcFunction_U32;
		BOF_RPC_FUNCTION pRpcFct_X[BOF_RPC_MAXFCT];

		BOF_RPC_PARAM()
		{
			Reset();
		}

		void Reset()
		{
			uint32_t i_U32;

			ErrorCode_E = BOF_ERR_NO_ERROR;
			MultiThreadAware_B = false;
			MaxStackSize_U32 = 0;
			UseChecksum_B = false;
			NbRpcFunction_U32 = 0;
			for (i_U32 = 0; i_U32 < BOF_RPC_MAXFCT; i_U32++)
			{
				pRpcFct_X[i_U32].Reset();
			}
		}
};

// 212	 bytes
struct BOF_RPC_STATE
{
// BOFRPCPARAM
/* 000 */  uint32_t MaxStackSize_U32;   /*! Specify the rpc stack max size*/
/* 004 */ uint32_t RpcControlParamSize_U32; /*! No more used in bofstd ->keep for bofmp compat -> to be removed laterSpecify the size of the basic Rpc data object (1, 2 or 4 bytes)*/
/* 008 */ uint32_t UseChecksum_B;      /*! True if checksum computaion, insertion and control must be used during transfert: Not bool to be sure of structure size and aligment on different cpu*/
/* 012 */ uint32_t MaxStringSize_U32;
/* 016 */ uint32_t NbRpcFunction_U32;

// BOFRPCPARSERRESULT
/* 020 */ uint32_t ReqFctId_U32;
/* 024 */ uint32_t ReqRpcRts_U32;
/* 028 */ uint32_t ReqRpcStackLen_U32;
/* 032 */ uint32_t ReqTimeOut_U32;
/* 036 */ uint16_t ReqTag_U16;
/* 038 */ uint16_t ReqFlag_U16;
/* 040 */ uint8_t pReqRpcStack_U8[64];
/* 104 */ uint32_t AnsFctId_U32;
/* 108 */ uint32_t AnsRpcRts_U32;
/* 112 */ uint32_t AnsRpcStackLen_U32;
/* 116 */ uint32_t AnsTimeOut_U32;
/* 120 */ uint32_t RpcFifoLevel_U32;
/* 124 */ uint32_t RpcMaxFifoLevel_U32;
/* 128 */ uint16_t AnsTag_U16;
/* 130 */ uint16_t AnsFlag_U16;
/* 132 */ uint8_t pAnsRpcStack_U8[64];

// GBEUCPCCMDCHANNEL
/* 196 */ uint8_t Flag_U8;            /*! Communication handshake control*/
/* 197 */ uint8_t GbeCmd_U8;          /*! Command id*/
/* 198 */ uint8_t Sts_U8;             /*! command status */
/* 199 */ uint8_t Spare_U8;
/* 200 */ uint32_t CommandTimeout_U32; /*! timeout linked to pending rpc request->0 if none*/
/* 204 */ uint32_t CmdTimer_U32;
/* 208 */ uint16_t SendTcpRpcResultSize_U16;
/* 210 */ uint16_t Spare_U16;
/*member of an union->no user defined constructor
	BOF_RPC_STATE()
	{
		Reset();
	}
	void                Reset()
	{
		MaxStackSize_U32         = 0;
		UseChecksum_B            = false;
		MaxStringSize_U32        = 0;
		NbRpcFunction_U32        = 0;

		ReqFctId_U32             = 0;
		ReqRpcRts_U32            = 0;
		ReqRpcStackLen_U32       = 0;
		ReqTimeOut_U32           = 0;
		ReqTag_U16               = 0;
		ReqFlag_U16              = 0;
		memset(pReqRpcStack_U8, 0, sizeof(pReqRpcStack_U8) );
		AnsFctId_U32             = 0;
		AnsRpcRts_U32            = 0;
		AnsRpcStackLen_U32       = 0;
		AnsTimeOut_U32           = 0;
		RpcFifoLevel_U32         = 0;
		RpcMaxFifoLevel_U32      = 0;
		AnsTag_U16               = 0;
		AnsFlag_U16              = 0;
		memset(pAnsRpcStack_U8, 0, sizeof(pAnsRpcStack_U8) );


		Flag_U8                  = 0;
		GbeCmd_U8                = 0;
		Sts_U8                   = 0;
		Spare_U8                 = 0;
		CommandTimeout_U32       = 0;
		CmdTimer_U32             = 0;
		SendTcpRpcResultSize_U16 = 0;
		Spare_U16                = 0;
	}
	*/
};

#pragma pack()

/*** Function ***************************************************************/

void DumpRpcFrame(bool Trx_B, char *pTxt_c, uint32_t Nb_U32, uint8_t *pData_U8);

// uint32_t Bof_BuildRpcFunctionTable(TCHAR *pFnIn_tc,TCHAR *pFnOut_tc,TCHAR *pVarName_tc,uint32_t *pErrorLine_U32);

/*** Class definition *******************************************************/

/*** BofRpc ****************************************************************/

class BofRpc
{
private:
		BOF_RPC_PARAM mBofRpcParam_X;
		BOF_RPC_FUNCTION *mpRpcFct_X;
		BofStack *mpRpcReqStack_O;
		BofStack *mpRpcAnsStack_O;
		bool mLittleEndian_B;
		static uint16_t S_mRpcCmdTag_U16;
protected:
		BOF_RPC_STATE mRpcState_X;
public:
		BofRpc(BOF_RPC_PARAM *pBofRpcClientParam_X);

		~BofRpc();

		BOF_RPC_STATE *GetRpcState()
		{ return &mRpcState_X; }

		uint32_t GetNumberOfRpcFunction()
		{ return mBofRpcParam_X.NbRpcFunction_U32; }

		uint32_t GetFunctionCharacteristicByIndex(uint32_t Index_U32, BOF_RPC_FUNCTION *pRpcFct_X)
		{
			if (Index_U32 < mBofRpcParam_X.NbRpcFunction_U32)
			{
				*pRpcFct_X = mBofRpcParam_X.pRpcFct_X[Index_U32];
				return BOF_ERR_NO_ERROR;
			}
			else
			{ return BOF_ERR_NOT_FOUND; }
		}

		uint32_t BuildRpcRequest(BOF_RPC_PARSER_RESULT *_pRpcRequest_X, ...);

		uint32_t BuildRpcAnswer(BOF_RPC_PARSER_RESULT *_pRpcParserResult_X);

		uint32_t ParseRpcFrame(bool _RpcAnswer_B, uint8_t *_pRpcFrameBuffer_U8, BOF_RPC_PARSER_RESULT *_pRpcParserResult_X);

		uint32_t IsRpcLittleEndian(uint8_t *_pRpcFrameBuffer_U8, bool *_pLittleEndian_B, bool *_pMustSwap_B);

		uint32_t WaitForRpcFrame(BofComChannel *_pBofComChannel_O, uint32_t _TimeoutInMs_U32, uint32_t *_pMaxRpcFrameSizeInByte_U32, uint8_t *_pRpcFrame_U8);


		uint32_t CheckAndResetStackPointerToRpcControlSize(BofStack *_pStack);

		uint32_t ResetStackPointerToRpcPayload(BofStack *_pStack);

		bool Push(BofStack *_pStack, bool PushData_B, BOF_RPC_VAR_ARRAYINOUT pRpcArray);

		bool PushTopRpcControlParam(BofStack *_pStack, uint16_t Sof_U16, uint16_t Flags_U16, uint16_t TimeOutInMs_U16, uint16_t Tag_U16, uint32_t Val_U32);

		bool Pop(BofStack *_pStack, bool PopData_B, BOF_RPC_VAR_ARRAYINOUT pRpcArray, uint32_t *pNb_U32);

		bool PopTopRpcControlParam(BofStack *_pStack, uint16_t *pSof_U16, uint16_t *pFlags_U16, uint16_t *pTimeOutInMs_U16, uint16_t *pTag_U16, uint32_t *pVal_U32);
};

END_BOF_NAMESPACE()