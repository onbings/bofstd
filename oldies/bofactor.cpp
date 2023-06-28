/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implement routines for creating and managing an actor. It is
 * based on czmq zactor
 *
 * Name:        BofActor.cpp
 * Author:      Bernard HARMEL: b.harmel@belgacom.net
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Mar 11 2002  BHA : Initial release
 */

/*** Include files ***********************************************************/
#include <atomic>
#include <bofstd/bofactor.h>
#include <czmq.h>


/*** Global variables ********************************************************/

/*** BofActor *****************************************************************/

BEGIN_BOF_NAMESPACE()

const char ACTOR_CMD_SEPARATOR = ';';

static void S_BofActor(zsock_t *_pPipe_X, void *_pArg);

// Opaque pointer design pattern: defined privately here all private data and functions: all of these can now change without recompiling callers ...
class BofActor::ActorImplementation
{
private:
  std::string mName_S;
  zactor_t *mpActor_X;
  BofActorFunction mActorFunction;
  void *mpUserContext;
  std::atomic<bool> mCommandRunning;
  std::atomic<uint32_t> mNbOfCommandPending;
  std::atomic<uint32_t> mTicket;

public:
  ActorImplementation(const std::string &_rName_S, BofActorFunction _ActorFunction, void *_pUserContext)
  {
    mName_S = _rName_S;
    mActorFunction = _ActorFunction;
    mpUserContext = _pUserContext;
    mCommandRunning = false;
    mNbOfCommandPending = 0;
    mTicket.store(0);
    mpActor_X = zactor_new(S_BofActor, this);
    BOF_ASSERT(mpActor_X != nullptr);
  }

  ~ActorImplementation()
  {
    zactor_destroy(&mpActor_X);
  }

  void OnProcessing(zsock_t *_pPipe_X)
  {
    BOF_ASSERT(_pPipe_X != nullptr);
    bool Finish_B = false;
    zmsg_t *pMsg_X;
    char *pCmd_c, *p_c;
    std::string Reply_S;
    int Sts_i;
    uint32_t Ticket_U32;

    zsock_signal(_pPipe_X, 0); // Sync with caller
    while (!Finish_B)
    {
      pMsg_X = zmsg_recv(_pPipe_X);
      if (pMsg_X)
      {
        Sts_i = -1;
        mCommandRunning = true;
        pCmd_c = zmsg_popstr(pMsg_X);
        //  All actors must handle $TERM in this way
        if (!strcmp(pCmd_c, "$TERM"))
        {
          Sts_i = 1;
          Finish_B = true;
        }
        else
        {
          p_c = strrchr(pCmd_c, ACTOR_CMD_SEPARATOR);
          if (p_c)
          {
            *p_c++ = 0;
            Ticket_U32 = std::atoi(p_c);
          }
          else
          {
            Ticket_U32 = 0;
          }
          if (mActorFunction != nullptr)
          {
            Reply_S = mActorFunction(pCmd_c, mpUserContext) + ACTOR_CMD_SEPARATOR + std::to_string(Ticket_U32);
            Sts_i = zmsg_pushstr(pMsg_X, Reply_S.c_str());
            Sts_i |= zmsg_send(&pMsg_X, _pPipe_X);
            BOF_ASSERT(Sts_i == 0);
          }
        }
        free(pCmd_c);
        zmsg_destroy(&pMsg_X);
        mCommandRunning = false;
        if (Sts_i == 0)
        {
          mNbOfCommandPending++;
        }
      }
      else
      {
        Finish_B = true; //  Interrupted
      }
    }
  }

  BOFERR SendCommandToActor(const std::string &_rCmd_S, uint32_t &_rTicket_U32)
  {
    BOFERR Rts_E = BOF_ERR_WRITE;
    int Sts_i;
    std::string Command_S;
    uint32_t Ticket_U32;

    Ticket_U32 = ++mTicket;
    Command_S = _rCmd_S + ACTOR_CMD_SEPARATOR + std::to_string(Ticket_U32);
    Sts_i = zstr_send(mpActor_X, Command_S.c_str());
    if (!Sts_i)
    {
      _rTicket_U32 = Ticket_U32;
      Rts_E = BOF_ERR_NO_ERROR;
    }
    return Rts_E;
  }

  BOFERR WaitForReplyFromActor(std::string &_rReply_S, uint32_t &_rTicket_U32)
  {
    BOFERR Rts_E;
    char *pReply_c, *p_c;

    pReply_c = zstr_recv(mpActor_X);
    if (pReply_c)
    {
      p_c = strrchr(pReply_c, ACTOR_CMD_SEPARATOR);
      if (p_c)
      {
        *p_c++ = 0;
        _rTicket_U32 = std::atoi(p_c);
      }
      else
      {
        _rTicket_U32 = 0;
      }
      _rReply_S = pReply_c;
      free(pReply_c);
    }
    else
    {
      _rReply_S = "";
    }

    Rts_E = BOF_ERR_NO_ERROR;
    mNbOfCommandPending--;
    return Rts_E;
  }

  bool IsCommandRunning()
  {
    return mCommandRunning;
  }

  bool IsReplyPending()
  {
    return (mNbOfCommandPending != 0);
  }
};

//  Actor
//  must call zsock_signal (pipe) when initialized
//  must listen to pipe and exit on $TERM command

static void S_BofActor(zsock_t *_pPipe_X, void *_pContext)
{
  BofActor::ActorImplementation *pActorImplementation = reinterpret_cast<BofActor::ActorImplementation *>(_pContext);
  BOF_ASSERT(pActorImplementation);
  pActorImplementation->OnProcessing(_pPipe_X);
}

// Opaque pointer design pattern: ... set Implementation values ...
BofActor::BofActor(const std::string &_rName_S, BofActorFunction _ActorFunction, void *_pUserContext)
    : mpuActorImplementation(new ActorImplementation(_rName_S, _ActorFunction, _pUserContext))
{
}

BofActor::~BofActor()
{
}

BOFERR BofActor::SendCommandToActor(const std::string &_rCmd_S, uint32_t &_rTicket_U32)
{
  return mpuActorImplementation->SendCommandToActor(_rCmd_S, _rTicket_U32);
}

BOFERR BofActor::WaitForReplyFromActor(std::string &_rReply_S, uint32_t &_rTicket_U32)
{
  return mpuActorImplementation->WaitForReplyFromActor(_rReply_S, _rTicket_U32);
}

bool BofActor::IsCommandRunning()
{
  return mpuActorImplementation->IsCommandRunning();
}

bool BofActor::IsReplyPending()
{
  return mpuActorImplementation->IsReplyPending();
}
END_BOF_NAMESPACE()