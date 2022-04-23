/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implement os dependent routines for creating and managing socket
 *
 * Name:        bofsocketos.cpp
 * Author:      Bernard HARMEL: b.harmel@belgacom.net
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Aug 21 2001  BHA : Initial release
 */

/*** Include files ***********************************************************/

#include <bofstd/bofsocketos.h>
#include <bofstd/bofsystem.h>
#include <bofstd/bofstring.h>
#include <bofstd/bofstringformatter.h>
#include <czmq.h>
#include <map>

#if defined (_WIN32)
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa365915(v=vs.85).aspx
#include <winsock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <bofstd/bofstring.h>
#define WORKING_BUFFER_SIZE    0x4000;
#else

#include <poll.h>
#include <linux/rtnetlink.h>

#define BUFSIZE                8192

struct route_info
{
  struct in_addr dstAddr;
  struct in_addr srcAddr;
  struct in_addr gateWay;
  char           ifName[IF_NAMESIZE];
};
#endif

BEGIN_BOF_NAMESPACE()
static std::map<std::string, BOF_PROTOCOL_TYPE> S_ToProtocolType =
                                                  {
                                                    {"???",   BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN},
                                                    {"tcp",   BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TCP},
                                                    {"udp",   BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP},
                                                    {"ftp",   BOF_PROTOCOL_TYPE::BOF_PROTOCOL_FTP},
                                                    {"http",  BOF_PROTOCOL_TYPE::BOF_PROTOCOL_HTTP},
                                                    {"https", BOF_PROTOCOL_TYPE::BOF_PROTOCOL_HTTPS},
                                                    {"tel",   BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TELNET},
                                                    {"ssh",   BOF_PROTOCOL_TYPE::BOF_PROTOCOL_SSH},
                                                    {"pgm",   BOF_PROTOCOL_TYPE::BOF_PROTOCOL_ZMQ_PGM},
                                                    {"pipe",  BOF_PROTOCOL_TYPE::BOF_PROTOCOL_PIPE},
                                                    {"ram",   BOF_PROTOCOL_TYPE::BOF_PROTOCOL_SHARED_RAM},
                                                  };

static std::map<BOF_PROTOCOL_TYPE, std::string> S_ToProtocolTypeString =
                                                  {
                                                    {BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN,    "???"},
                                                    {BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TCP,        "tcp"},
                                                    {BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP,        "udp"},
                                                    {BOF_PROTOCOL_TYPE::BOF_PROTOCOL_FTP,        "ftp"},
                                                    {BOF_PROTOCOL_TYPE::BOF_PROTOCOL_HTTP,       "http"},
                                                    {BOF_PROTOCOL_TYPE::BOF_PROTOCOL_HTTPS,      "https"},
                                                    {BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TELNET,     "tel"},
                                                    {BOF_PROTOCOL_TYPE::BOF_PROTOCOL_SSH,        "ssh"},
                                                    {BOF_PROTOCOL_TYPE::BOF_PROTOCOL_ZMQ_PGM,    "pgm"},
                                                    {BOF_PROTOCOL_TYPE::BOF_PROTOCOL_PIPE,       "pipe"},
                                                    {BOF_PROTOCOL_TYPE::BOF_PROTOCOL_SHARED_RAM, "ram"},
                                                  };
#if defined (_WIN32)
BOFERR Bof_GetNetworkInterfaceInfo(const std::string _rInterfaceName_S, BOF_INTERFACE_INFO & _rInterfaceInfo_X)
{
  BOFERR                Rts_E = BOF_ERR_ENOMEM;
  DWORD                 Sts_DW;
  ULONG                 Try_UL, i_U32, Flag_UL = GAA_FLAG_INCLUDE_GATEWAYS | GAA_FLAG_INCLUDE_ALL_INTERFACES | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_MULTICAST;
  PIP_ADAPTER_ADDRESSES pAdapterAddressArray_X;
  PIP_ADAPTER_ADDRESSES pCrtAdapterAddress_X;
  ULONG                 OutputBufferLen_UL;
  WCHAR                 pInterfaceName_wc[0x200];
  char                  pIp_c[0x200];
  LPSOCKADDR            pSockeckAddress_X;
  sockaddr_in           *pSaInV4_X;
  sockaddr_in6          *pSaInV6_X;

  Bof_MultiByteToWideChar(_rInterfaceName_S.c_str(), sizeof(pInterfaceName_wc) / sizeof(WCHAR), pInterfaceName_wc);
  OutputBufferLen_UL = WORKING_BUFFER_SIZE;
  Try_UL             = 0;
  _rInterfaceInfo_X.MacAddress.clear();
  Sts_DW             = ERROR_BUFFER_OVERFLOW;
  do
  {
    pAdapterAddressArray_X = (IP_ADAPTER_ADDRESSES *)malloc(OutputBufferLen_UL);
    if (pAdapterAddressArray_X == nullptr)
    {
      break;
    }
    Sts_DW                 = GetAdaptersAddresses(AF_UNSPEC, Flag_UL, nullptr, pAdapterAddressArray_X, &OutputBufferLen_UL);
    if (Sts_DW == ERROR_BUFFER_OVERFLOW)
    {
      free(pAdapterAddressArray_X);
      pAdapterAddressArray_X = nullptr;
    }
    else
    {
      break;
    }
    Try_UL++;
  }
  while ( (Sts_DW == ERROR_BUFFER_OVERFLOW) && (Try_UL < 4) );

  if (Sts_DW == NO_ERROR)
  {
    Rts_E                = BOF_ERR_NOT_FOUND;
    pCrtAdapterAddress_X = pAdapterAddressArray_X;
    while (pCrtAdapterAddress_X)
    {
      if (! wcscmp(pInterfaceName_wc, pCrtAdapterAddress_X->FriendlyName) )
      {
        Rts_E                             = BOF_ERR_NO_ERROR;
        _rInterfaceInfo_X.InterfaceFlag_E = BOF_NETWORK_INTERFACE_FLAG::BOF_IFF_NONE;
        if (pCrtAdapterAddress_X->OperStatus == IfOperStatusUp)
        {
          _rInterfaceInfo_X.InterfaceFlag_E = _rInterfaceInfo_X.InterfaceFlag_E | BOF_NETWORK_INTERFACE_FLAG::BOF_IFF_UP;
        }
        _rInterfaceInfo_X.MtuSize_U32     = pCrtAdapterAddress_X->Mtu;
        if (pCrtAdapterAddress_X->PhysicalAddressLength==0)	//127.0.0.1
        {
          for (i_U32 = 0; i_U32 < 6; i_U32++)
          {
            _rInterfaceInfo_X.MacAddress.push_back(0);
          }

        }
        else
        {
          for (i_U32 = 0; i_U32 < pCrtAdapterAddress_X->PhysicalAddressLength; i_U32++)
          {
            _rInterfaceInfo_X.MacAddress.push_back(pCrtAdapterAddress_X->PhysicalAddress[i_U32]);
          }
        }
        _rInterfaceInfo_X.IpGateway_S     = "";
        if (pCrtAdapterAddress_X->FirstGatewayAddress)
        {
          pSockeckAddress_X = pCrtAdapterAddress_X->FirstGatewayAddress->Address.lpSockaddr;
          if ( (pSockeckAddress_X) && (pCrtAdapterAddress_X->FirstGatewayAddress->Address.iSockaddrLength) )
          {
            Rts_E = BOF_ERR_TOO_SMALL;
            if (pSockeckAddress_X->sa_family == AF_INET)
            {
              pSaInV4_X = (sockaddr_in *)pSockeckAddress_X;
              if (inet_ntop(AF_INET, &(pSaInV4_X->sin_addr), pIp_c, sizeof(pIp_c) ) )
              {
                _rInterfaceInfo_X.IpGateway_S = pIp_c;
                Rts_E                         = BOF_ERR_NO_ERROR;
              }
            }
            else if (pSockeckAddress_X->sa_family == AF_INET6)
            {
              pSaInV6_X = (sockaddr_in6 *)pSockeckAddress_X;
              if (inet_ntop(AF_INET6, &(pSaInV6_X->sin6_addr), pIp_c, sizeof(pIp_c) ) )
              {
                _rInterfaceInfo_X.IpGateway_S = pIp_c;
                Rts_E                         = BOF_ERR_NO_ERROR;
              }
            }
            else
            {
              Rts_E = BOF_ERR_INTERNAL;
            }
          }
        }
        break;
      }
      pCrtAdapterAddress_X = pCrtAdapterAddress_X->Next;
    }
    free(pAdapterAddressArray_X);
  }
  return Rts_E;
}

BOFERR Bof_SetNetworkInterfaceParam(const std::string _rInterfaceName_S, BOF_NETWORK_INTERFACE_PARAM & _rNewInterfaceParam_X)
{
  BOFERR      Rts_E;
  std::string Cmd_S, Output_S;
  int32_t     ExitCode_S32;
  std::string IpAddress_S, IpMask_S, IpGateway_S;

// TODO IpV6 version
  IpAddress_S = (_rNewInterfaceParam_X.IpAddress_S == "") ? "0.0.0.0" : _rNewInterfaceParam_X.IpAddress_S.c_str();
  IpMask_S    = (_rNewInterfaceParam_X.IpMask_S == "") ? "0.0.0.0" : _rNewInterfaceParam_X.IpMask_S.c_str();
  IpGateway_S = (_rNewInterfaceParam_X.Info_X.IpGateway_S == "") ? "0.0.0.0" : _rNewInterfaceParam_X.Info_X.IpGateway_S.c_str();

  // std::cerr << "[          ] You must be admin to run this command" << std::endl;
  Cmd_S       = Bof_Sprintf("netsh interface ip set address name=\"%s\" static %s %s %s 1", _rInterfaceName_S.c_str(), IpAddress_S.c_str(), IpMask_S.c_str(), IpGateway_S.c_str() );
  Rts_E       = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);
  if ((Rts_E == BOF_ERR_NO_ERROR) && (ExitCode_S32))
  {
    Rts_E = BOF_ERR_INTERNAL;
  }
  if ( (Rts_E == BOF_ERR_NO_ERROR) && (_rInterfaceName_S != _rNewInterfaceParam_X.Name_S) )
  {
    Cmd_S = Bof_Sprintf("netsh interface set interface name=\"%s\" newname = \"%s\"", _rInterfaceName_S.c_str(), _rNewInterfaceParam_X.Name_S.c_str() );
    Rts_E = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);
    if ((Rts_E == BOF_ERR_NO_ERROR) && (ExitCode_S32))
    {
        Rts_E = BOF_ERR_INTERNAL;
    }
  }
  return Rts_E;
}
#if 0
uint32_t           IpAddress_U32, IpMask_U32, SrcGateway_U32, Sts_U32, Size_U32, i_U32;
ULONG              NteContext_UL, NteInstance_UL;
MIB_IPFORWARDTABLE *pIpForwardTable_X;
MIB_IPFORWARDROW   IpForwardRow_X;

if ( (_pSrcIpParam_X) &&
     (_pDstIpParam_X) &&
     (_pDstIpParam_X->IndexId_U32 == _pSrcIpParam_X->IndexId_U32)
     )
{
  /*
   * MaxIpParamEntry_U32 = sizeof( pIpParam_X ) / sizeof( pIpParam_X[0] );
   * Rts_E             = S_GetIpParameterList(&MaxIpParamEntry_U32, pIpParam_X);
   *
   * if ( Rts_E == BOF_ERR_NO_ERROR )
   * {
   *      Rts_E = BOF_ERR_NOT_FOUND;
   *
   *      for ( i_U32 = 0; i_U32 < MaxIpParamEntry_U32; i_U32++ )
   *      {
   *              if ( !memcmp(_pSrcIpParam_X, &pIpParam_X[i_U32], sizeof( BOFSOCKETIPPARAM ) ) )
   *              {
   *                      Rts_E = BOF_ERR_NO_ERROR;
   *                      break;
   *              }
   *      }
   *      if (Rts_E == NO_ERROR)
   *      {
   */
  if ( (! BofSocket::S_IsIpAddressEqual(true, false, &_pSrcIpParam_X->IpAddress_X, &_pDstIpParam_X->IpAddress_X) ) ||
       (! BofSocket::S_IsIpAddressEqual(true, false, &_pSrcIpParam_X->IpMask_X, &_pDstIpParam_X->IpMask_X) )
       )
  {
    Sts_U32 = DeleteIPAddress(_pSrcIpParam_X->Context_U32);

    BOF_SET_SOCKET_ADDRESS32(IpAddress_U32, _pDstIpParam_X->IpAddress_X);
    BOF_SET_SOCKET_ADDRESS32(IpMask_U32, _pDstIpParam_X->IpMask_X);
    Rts_E   = AddIPAddress(IpAddress_U32, IpMask_U32, _pDstIpParam_X->IndexId_U32, &NteContext_UL, &NteInstance_UL);

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      _pDstIpParam_X->Context_U32 = NteContext_UL;
    }
    else
    {
      if (Rts_E == ERROR_OBJECT_ALREADY_EXISTS)
      {
        Rts_E = BOF_ERR_NO_ERROR;
      }
      else
      {
        BOF_SET_SOCKET_ADDRESS32(IpAddress_U32, _pSrcIpParam_X->IpAddress_X);
        BOF_SET_SOCKET_ADDRESS32(IpMask_U32, _pSrcIpParam_X->IpMask_X);
        Sts_U32 = AddIPAddress(IpAddress_U32, IpMask_U32, _pDstIpParam_X->IndexId_U32, &NteContext_UL, &NteInstance_UL);
      }
    }
  }

  if ( (Rts_E == NO_ERROR) &&
       (! BofSocket::S_IsIpAddressEqual(true, false, &_pSrcIpParam_X->Gateway_X, &_pDstIpParam_X->Gateway_X) )
       )
  {
    Rts_E             = BOF_ERR_ENOMEM;
    Size_U32          = 0;
    pIpForwardTable_X = nullptr;

    if (GetIpForwardTable(pIpForwardTable_X, (ULONG *)&Size_U32, false) == ERROR_INSUFFICIENT_BUFFER)
    {
      // Allocate the memory for the table
      pIpForwardTable_X = (MIB_IPFORWARDTABLE *)malloc(Size_U32);

      if (pIpForwardTable_X)
      {
        Rts_E = BOF_ERR_INTERNAL;

        if (GetIpForwardTable(pIpForwardTable_X, (ULONG *)&Size_U32, false) == ERROR_SUCCESS)
        {
          Rts_E                           = BOF_ERR_NO_ERROR;
          memset(&IpForwardRow_X, 0, sizeof(IpForwardRow_X) );
          IpForwardRow_X.dwForwardIfIndex = _pDstIpParam_X->IndexId_U32;

          // NO               IpForwardRow_X.dwForwardMask=_pDstIpParam_X->Mask_U32;
          BOF_SET_SOCKET_ADDRESS32(IpForwardRow_X.dwForwardNextHop, _pDstIpParam_X->Gateway_X);

          // IpForwardRow_X.dwForwardNextHop = _pDstIpParam_X->Gateway_X;
          IpForwardRow_X.dwForwardType    = 3; // 4;     //MIB_IPROUTE_TYPE_DIRECT/MIB_IPROUTE_TYPE_INDIRECT;
          IpForwardRow_X.dwForwardProto   = 3; // RouteProtocolNetMgmt;
          IpForwardRow_X.dwForwardAge     = 0x2f8;
          IpForwardRow_X.dwForwardMetric1 = 0xA;

          BOF_SET_SOCKET_ADDRESS32(SrcGateway_U32, _pSrcIpParam_X->Gateway_X);

          for (i_U32 = 0 ; i_U32 < pIpForwardTable_X->dwNumEntries ; i_U32++)
          {
            if ( (pIpForwardTable_X->table[i_U32].dwForwardDest == 0) &&

                                               // ( pIpForwardTable_X->table[i_U32].dwForwardNextHop == _pSrcIpParam_X->Gateway_X ) &&
                 (pIpForwardTable_X->table[i_U32].dwForwardNextHop == SrcGateway_U32) &&
                 (pIpForwardTable_X->table[i_U32].dwForwardIfIndex == _pSrcIpParam_X->IndexId_U32)
                 )
            {
              Rts_E = BOF_ERR_INTERNAL;

              if (DeleteIpForwardEntry(&pIpForwardTable_X->table[i_U32]) == ERROR_SUCCESS)
              {
                IpForwardRow_X = pIpForwardTable_X->table[i_U32];
                BOF_SET_SOCKET_ADDRESS32(IpForwardRow_X.dwForwardNextHop, _pDstIpParam_X->Gateway_X);

                // IpForwardRow_X.dwForwardNextHop = _pDstIpParam_X->Gateway_X;
                Rts_E          = BOF_ERR_NO_ERROR;
              }
            }
          }

          // Create a new route entry for the default gateway.
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            Rts_E = BOF_ERR_INTERNAL;

            if (CreateIpForwardEntry(&IpForwardRow_X) == NO_ERROR)
            {
              Rts_E = BOF_ERR_NO_ERROR;
            }
          }
        }
      }
    }
  }
}
#endif

#else

int readNlSock(int sockFd, char *bufPtr, size_t buf_size, int seqNum, int pId)
{
  struct nlmsghdr *nlHdr;
  int             readLen = 0, msgLen = 0;

  do
  {
    /* Recieve response from the kernel */
    if ((readLen = static_cast<int>(recv(sockFd, bufPtr, buf_size - msgLen, 0))) < 0)
    {
// perror ( "SOCK READ: " );
      return -1;
    }

    nlHdr = (struct nlmsghdr *) bufPtr;

    /* Check if the header is valid */
    if ((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
    {
// perror ( "Error in received packet" );
      return -1;
    }

    /* Check if the its the last message */
    if (nlHdr->nlmsg_type == NLMSG_DONE)
    {
      break;
    }
    else
    {
      /* Else move the pointer to buffer appropriately */
      bufPtr += readLen;
      msgLen += readLen;
    }

    /* Check if its a multi part message */
    if ((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0)
    {
      /* return if its not */
      break;
    }
  } while ((nlHdr->nlmsg_seq != static_cast<uint32_t>(seqNum)) || (nlHdr->nlmsg_pid != static_cast<uint32_t>(pId)));

  return msgLen;
}

/* parse the route info returned */
int parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo, int _Family_i)
{
  struct rtmsg  *rtMsg;
  struct rtattr *rtAttr;
  int           rtLen;
  int           Ret_i = 0;

  rtMsg = (struct rtmsg *) NLMSG_DATA(nlHdr);

  /* If the route is not for AF_INET or does not belong to main routing table then return. */
  if ((rtMsg->rtm_family == _Family_i) && (rtMsg->rtm_table == RT_TABLE_MAIN))
  {
    /* get the rtattr field */
    rtAttr = (struct rtattr *) RTM_RTA(rtMsg);
    rtLen  = static_cast<int>(RTM_PAYLOAD(nlHdr));

    for (; RTA_OK(rtAttr, rtLen); rtAttr = RTA_NEXT(rtAttr, rtLen))
    {
      switch (rtAttr->rta_type)
      {
        case RTA_OIF:
        {
          if_indextoname(*(int *) RTA_DATA(rtAttr), rtInfo->ifName);
          break;
        }

        case RTA_GATEWAY:
        {
          memcpy(&rtInfo->gateWay, RTA_DATA(rtAttr), sizeof(rtInfo->gateWay));
          break;
        }

        case RTA_PREFSRC:
        {
          memcpy(&rtInfo->srcAddr, RTA_DATA(rtAttr), sizeof(rtInfo->srcAddr));
          break;
        }

        case RTA_DST:
        {
          memcpy(&rtInfo->dstAddr, RTA_DATA(rtAttr), sizeof(rtInfo->dstAddr));
          break;
        }
      }
    }
  }
  else
  {
    Ret_i = -1;
  }

  return Ret_i;
}

BOFERR Bof_GetNetworkInterfaceInfo(const std::string _rInterfaceName_S, BOF_INTERFACE_INFO &_rInterfaceInfo_X)
{
  BOFERR            Rts_E  = BOF_ERR_CREATE;
  struct nlmsghdr   *nlMsg;
//	struct rtmsg      *rtMsg;
  struct route_info RouteInfo_X;
  int               Socket_i;
  int               len    = 0;
  int               msgSeq = 0;
  char              msgBuf[BUFSIZE];   // pretty large buffer
  char              pIp_c[0x200];
  struct ifreq      IfReq_X;
  uint32_t          i_U32;

  Socket_i = socket(AF_INET, SOCK_DGRAM, 0);
  if (Socket_i >= 0)
  {
    Rts_E = BOF_ERR_NO_ERROR;
    memset(&IfReq_X, 0, sizeof(IfReq_X));
    IfReq_X.ifr_addr.sa_family = AF_INET;
    strncpy(IfReq_X.ifr_name, _rInterfaceName_S.c_str(), IFNAMSIZ - 1);
    IfReq_X.ifr_name[IFNAMSIZ - 1] = 0;

    _rInterfaceInfo_X.InterfaceFlag_E = BOF_NETWORK_INTERFACE_FLAG::BOF_IFF_NONE;
    if (ioctl(Socket_i, SIOCGIFFLAGS, &IfReq_X) >= 0)
    {
      if (IfReq_X.ifr_flags & IFF_UP)
      {
        _rInterfaceInfo_X.InterfaceFlag_E = _rInterfaceInfo_X.InterfaceFlag_E | BOF_NETWORK_INTERFACE_FLAG::BOF_IFF_UP;
      }
    }

    if (ioctl(Socket_i, SIOCGIFMTU, &IfReq_X) >= 0)
    {
      _rInterfaceInfo_X.MtuSize_U32 = IfReq_X.ifr_mtu;
    }

    if (ioctl(Socket_i, SIOCGIFHWADDR, &IfReq_X) >= 0)
    {
      for (i_U32 = 0; i_U32 < 6; i_U32++)
      {
        _rInterfaceInfo_X.MacAddress.push_back(IfReq_X.ifr_hwaddr.sa_data[i_U32]);
      }
    }
    close(Socket_i);
  }

  _rInterfaceInfo_X.IpGateway_S = "";
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    if ((Socket_i = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) >= 0)
    {
      /* Initialize the buffer */
      memset(msgBuf, 0, sizeof(msgBuf));

      /* point the header and the msg structure pointers into the buffer */
      nlMsg = (struct nlmsghdr *) msgBuf;
//			rtMsg              = (struct rtmsg *)NLMSG_DATA(nlMsg);

      /* Fill in the nlmsg header*/
      nlMsg->nlmsg_len   = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
      nlMsg->nlmsg_type  = RTM_GETROUTE;                        // Get the routes from kernel routing table .
      nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;          // The message is a request for dump.
      nlMsg->nlmsg_seq   = msgSeq++;                            // Sequence of the message packet.
      nlMsg->nlmsg_pid   = getpid();                            // PID of process sending the request.

      /* Send the request */
      Rts_E = BOF_ERR_WRITE;
      if (send(Socket_i, nlMsg, nlMsg->nlmsg_len, 0) >= 0)
      {
        /* Read the response */
        Rts_E    = BOF_ERR_WRITE;
        if ((len = readNlSock(Socket_i, msgBuf, sizeof(msgBuf), msgSeq, getpid())) >= 0)
        {
          /* Parse and print the response */
          for (; NLMSG_OK(nlMsg, len); nlMsg = NLMSG_NEXT(nlMsg, len))
          {
            memset(&RouteInfo_X, 0, sizeof(route_info));

            if (parseRoutes(nlMsg, &RouteInfo_X, AF_INET) < 0)
            {
              continue;                                         // don't check route_info if it has not been set up
            }

            // Check if default gateway
            if (strstr((char *) inet_ntoa(RouteInfo_X.dstAddr), "0.0.0.0"))
            {
              // It's the interface we are looking for
              if (strcmp(RouteInfo_X.ifName, _rInterfaceName_S.c_str()) == 0)
              {
                // copy it over
                inet_ntop(AF_INET, &RouteInfo_X.gateWay, pIp_c, sizeof(pIp_c));
                _rInterfaceInfo_X.IpGateway_S = pIp_c;
                Rts_E = BOF_ERR_NO_ERROR;
                break;
              }
            }
          }
        }
      }
      close(Socket_i);
      // For gw, in all case returns BOF_ERR_NO_ERROR (if previous op was good)
      Rts_E = BOF_ERR_NO_ERROR;

    }
  }
  return Rts_E;
}

BOFERR Bof_SetNetworkInterfaceParam(const std::string _rInterfaceName_S, BOF_NETWORK_INTERFACE_PARAM &_rNewInterfaceParam_X)
{
  BOFERR      Rts_E;
  std::string Cmd_S, Output_S, Name_S;
  int32_t     ExitCode_S32, CidrMask_S32;
  std::string IpAddress_S, IpMask_S, IpGateway_S;

  // TODO IpV6 version
  // std::cerr << "[          ] You must be admin to run this command" << std::endl;
  IpAddress_S = (_rNewInterfaceParam_X.IpAddress_S == "") ? "0.0.0.0" : _rNewInterfaceParam_X.IpAddress_S.c_str();
  IpMask_S    = (_rNewInterfaceParam_X.IpMask_S == "") ? "0.0.0.0" : _rNewInterfaceParam_X.IpMask_S.c_str();
  IpGateway_S = (_rNewInterfaceParam_X.Info_X.IpGateway_S == "") ? "0.0.0.0" : _rNewInterfaceParam_X.Info_X.IpGateway_S.c_str();

  // Unconditionally down
  //Cmd_S = Bof_Sprintf("ip link set %s down", _rInterfaceName_S.c_str());
  //Rts_E = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);

  // Unconditionally remove all ip
  Cmd_S = Bof_Sprintf("ip addr flush dev %s", _rInterfaceName_S.c_str());
  Rts_E = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);

  CidrMask_S32 = Bof_Compute_CidrMask(IpMask_S.c_str());
  Cmd_S        = Bof_Sprintf("ip address add %s/%d dev %s", IpAddress_S.c_str(), CidrMask_S32, _rInterfaceName_S.c_str());
  Rts_E        = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);
  if ((Rts_E == BOF_ERR_NO_ERROR) && (ExitCode_S32))  //|| (Output_S != "") )
  {
    Rts_E = BOF_ERR_INTERNAL;
  }

  if ((Rts_E == BOF_ERR_NO_ERROR) && (!Bof_IsIpAddressNull(IpGateway_S)))
  {
    Cmd_S = Bof_Sprintf("ip route del default via %s", IpGateway_S.c_str());
    Rts_E = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);

    Cmd_S = Bof_Sprintf("ip route add default via %s", IpGateway_S.c_str());
    Rts_E = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);
    if ((Rts_E == BOF_ERR_NO_ERROR) && (ExitCode_S32))  //|| (Output_S != "") )
    {
      Rts_E = BOF_ERR_INTERNAL;
    }
  }

  Name_S = _rInterfaceName_S;
  if ((Rts_E == BOF_ERR_NO_ERROR) && (!ExitCode_S32))  //|| (Output_S != "") )
  {
    if (_rNewInterfaceParam_X.Name_S != "")
    {
      Cmd_S = Bof_Sprintf("ip link set dev %s name %s", _rInterfaceName_S.c_str(), _rNewInterfaceParam_X.Name_S.c_str());
      Rts_E = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);
      if ((Rts_E == BOF_ERR_NO_ERROR) && (ExitCode_S32))
      {
        Rts_E = BOF_ERR_INTERNAL;
      }
      else
      {
        Name_S = _rNewInterfaceParam_X.Name_S;
      }
    }
  }
  // Unconditionally up
  //Cmd_S = Bof_Sprintf("ip link set %s up", Name_S.c_str());
  //Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);

  return Rts_E;
}

#if 0
uint32_t       BroadcastAddress_U32, BroadcastMask_U32;
int            Sts_i, Socket_i;
struct ifreq   IfReq_X;
struct rtentry Route_X;
bool           ChangeBroadcast_B, ChangeMask_B;
SOCKADDR_IN    IpBroadcast_X, *pIp_X;

// http://www-01.ibm.com/support/knowledgecenter/ssw_aix_61/com.ibm.aix.commtrf2/ioctl_socket_control_operations.htm
// http://linux.die.net/man/7/netdevice
// https://gist.github.com/lbuchy/1888469

if ( (_pSrcIpParam_X) && (_pDstIpParam_X) && (_pSrcIpParam_X->IndexId_U32 == _pDstIpParam_X->IndexId_U32) )
{
  Socket_i = socket(AF_INET, SOCK_DGRAM, 0);

  if (Socket_i >= 0)
  {
    memset(&IfReq_X, 0, sizeof(IfReq_X) );
    IfReq_X.ifr_ifindex = _pSrcIpParam_X->IndexId_U32;
    Sts_i               = ioctl(Socket_i, SIOCGIFNAME, &IfReq_X);
    ChangeBroadcast_B   = false;

    if (Sts_i >= 0)
    {
      if ( (! BofSocket::S_IsIpAddressEqual(true, false, &_pSrcIpParam_X->IpAddress_X, &_pDstIpParam_X->IpAddress_X) ) && (! BofSocket::S_IsIpAddressNull(&_pDstIpParam_X->IpAddress_X) ) )
      {
        *(SOCKADDR_IN *) & IfReq_X.ifr_addr = _pDstIpParam_X->IpAddress_X;
        Sts_i                               = ioctl(Socket_i, SIOCSIFADDR, &IfReq_X);
        ChangeBroadcast_B                   = true;
        ChangeMask_B                        = true; // Mask is resetted
      }

      if (Sts_i >= 0)
      {
        if ( (ChangeMask_B) || (! BofSocket::S_IsIpAddressEqual(true, false, &_pSrcIpParam_X->IpMask_X, &_pDstIpParam_X->IpMask_X) ) )
        {
          *(SOCKADDR_IN *) & IfReq_X.ifr_netmask = _pDstIpParam_X->IpMask_X;
          Sts_i                                  = ioctl(Socket_i, SIOCSIFNETMASK, &IfReq_X);
          ChangeBroadcast_B                      = true;
        }
      }

      if (Sts_i >= 0)
      {
        if (ChangeBroadcast_B)
        {
          BroadcastAddress_U32                     = _pDstIpParam_X->IpAddress_X.sin_addr.s_addr & _pDstIpParam_X->IpMask_X.sin_addr.s_addr;
          BroadcastMask_U32                        = 0xFFFFFFFF ^ _pDstIpParam_X->IpMask_X.sin_addr.s_addr;
          BroadcastAddress_U32                     = BroadcastAddress_U32 | BroadcastMask_U32;
          IpBroadcast_X                            = _pDstIpParam_X->IpAddress_X;
          IpBroadcast_X.sin_addr.s_addr            = BroadcastAddress_U32;
          *(SOCKADDR_IN *) & IfReq_X.ifr_broadaddr = _pDstIpParam_X->IpMask_X;

          Sts_i                                    = ioctl(Socket_i, SIOCSIFBRDADDR, &IfReq_X);
        }
      }

      if (Sts_i >= 0)
      {
        if (! BofSocket::S_IsIpAddressEqual(true, false, &_pSrcIpParam_X->Gateway_X, &_pDstIpParam_X->Gateway_X) )
        {
          // First delete old gateway
          if (! BofSocket::S_IsIpAddressNull(&_pSrcIpParam_X->Gateway_X) )
          {
            memset(&Route_X, 0, sizeof(Route_X) );
            pIp_X                  = (SOCKADDR_IN *)&Route_X.rt_gateway;
            pIp_X->sin_family      = AF_INET;
            pIp_X->sin_addr.s_addr = _pSrcIpParam_X->Gateway_X.sin_addr.s_addr;
            pIp_X                  = (SOCKADDR_IN *)&Route_X.rt_dst;
            pIp_X->sin_family      = AF_INET;
            pIp_X->sin_addr.s_addr = INADDR_ANY;
            pIp_X                  = (SOCKADDR_IN *)&Route_X.rt_genmask;
            pIp_X->sin_family      = AF_INET;
            pIp_X->sin_addr.s_addr = INADDR_ANY;

            Route_X.rt_dev         = _pSrcIpParam_X->pName_c;
            Route_X.rt_flags       = RTF_UP | RTF_GATEWAY;
            Route_X.rt_metric      = 0;
            Sts_i                  = ioctl(Socket_i, SIOCDELRT, &Route_X);
          }

          // In all case we define the new gw
          // if ( Sts_i >= 0 )
          {
            if (! BofSocket::S_IsIpAddressNull(&_pDstIpParam_X->Gateway_X) )
            {
              memset(&Route_X, 0, sizeof(Route_X) );
              pIp_X                  = (SOCKADDR_IN *)&Route_X.rt_gateway;
              pIp_X->sin_family      = AF_INET;
              pIp_X->sin_addr.s_addr = _pDstIpParam_X->Gateway_X.sin_addr.s_addr;
              pIp_X                  = (SOCKADDR_IN *)&Route_X.rt_dst;
              pIp_X->sin_family      = AF_INET;
              pIp_X->sin_addr.s_addr = INADDR_ANY;
              pIp_X                  = (SOCKADDR_IN *)&Route_X.rt_genmask;
              pIp_X->sin_family      = AF_INET;
              pIp_X->sin_addr.s_addr = INADDR_ANY;

              Route_X.rt_dev         = _pDstIpParam_X->pName_c;
              Route_X.rt_flags       = RTF_UP | RTF_GATEWAY;
              Route_X.rt_metric      = 0;
              Sts_i                  = ioctl(Socket_i, SIOCADDRT, &Route_X);
            }
          }
        }
      }
    }
  }
  close(Socket_i);

  if (Sts_i >= 0)
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
}
#endif
#endif


BOFERR Bof_GetListOfNetworkInterface(std::vector<BOF_NETWORK_INTERFACE_PARAM> &_rListOfNetworkInterface_X)
{
  BOFERR                      Rts_E = BOF_ERR_ENOMEM;
  ziflist_t                   *pIfList_X;
  const char                  *p;
  BOF_NETWORK_INTERFACE_PARAM NetworkInterface_X;

  _rListOfNetworkInterface_X.clear();
  // TODO BHA ziflist_new only support ipv4

//BHA path czmq ziflist.c: static bool s_valid_flags(short flags)
  pIfList_X = ziflist_new();
  if (pIfList_X)
  {
    Rts_E = BOF_ERR_NO_ERROR;
    // ziflist_print(pIfList_X);
    p     = ziflist_first(pIfList_X);
    while (p)
    {
      NetworkInterface_X.IpV6_B = false;

      NetworkInterface_X.Name_S = p;
      p = ziflist_address(pIfList_X);
      NetworkInterface_X.IpAddress_S = p;
      p = ziflist_broadcast(pIfList_X);
      NetworkInterface_X.IpBroadcast_S = p;
      p = ziflist_netmask(pIfList_X);
      NetworkInterface_X.IpMask_S = p;
      NetworkInterface_X.Info_X.MacAddress.clear();
      Rts_E = Bof_GetNetworkInterfaceInfo(NetworkInterface_X.Name_S, NetworkInterface_X.Info_X);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        _rListOfNetworkInterface_X.push_back(NetworkInterface_X);
        p = ziflist_next(pIfList_X);
      }
      else
      {
        break;
      }
    }
    ziflist_destroy(&pIfList_X);
  }

  return (Rts_E);

}

BOFERR Bof_GetNetworkInterfaceParam(const std::string _rInterfaceName_S, BOF_NETWORK_INTERFACE_PARAM &_rNewInterfaceParam_X)
{
  BOFERR                      Rts_E = BOF_ERR_ENOMEM;
  ziflist_t                   *pIfList_X;
  const char                  *p;
  BOF_NETWORK_INTERFACE_PARAM NetworkInterface_X;

  // TODO BHA ziflist_new only support ipv4
  pIfList_X = ziflist_new();
  if (pIfList_X)
  {
    Rts_E = BOF_ERR_NOT_FOUND;
    // ziflist_print(pIfList_X);
    p     = ziflist_first(pIfList_X);
    while (p)
    {
      if (strcmp(p, _rInterfaceName_S.c_str()) == 0)
      {
        _rNewInterfaceParam_X.IpV6_B = false;
        _rNewInterfaceParam_X.Name_S = p;
        p = ziflist_address(pIfList_X);
        _rNewInterfaceParam_X.IpAddress_S = p;
        p = ziflist_broadcast(pIfList_X);
        _rNewInterfaceParam_X.IpBroadcast_S = p;
        p = ziflist_netmask(pIfList_X);
        _rNewInterfaceParam_X.IpMask_S = p;
        Rts_E = Bof_GetNetworkInterfaceInfo(NetworkInterface_X.Name_S, _rNewInterfaceParam_X.Info_X);
        break;
      }
      p = ziflist_next(pIfList_X);
    }
    ziflist_destroy(&pIfList_X);
  }
  return (Rts_E);

}

int32_t Bof_Compute_CidrMask(const std::string &_rIpV4Address_S)
{
  int32_t Rts_S32 = -1;
  int     i, pIpVal_i[4];
  bool    Finish_B;

  if (sscanf(_rIpV4Address_S.c_str(), "%d.%d.%d.%d", &pIpVal_i[0], &pIpVal_i[1], &pIpVal_i[2], &pIpVal_i[3]) == 4)
  {
    Finish_B = false;
    Rts_S32  = 0;
    for (i   = 0; ((i < 4) && (!Finish_B)); i++)
    {
      switch (pIpVal_i[i])
      {
        case 0x80:
          Rts_S32 += 1;
          break;

        case 0xC0:
          Rts_S32 += 2;
          break;

        case 0xE0:
          Rts_S32 += 3;
          break;

        case 0xF0:
          Rts_S32 += 4;
          break;

        case 0xF8:
          Rts_S32 += 5;
          break;

        case 0xFC:
          Rts_S32 += 6;
          break;

        case 0xFE:
          Rts_S32 += 7;
          break;

        case 0xFF:
          Rts_S32 += 8;
          break;

        default:
          Finish_B = true;
          break;
      }
    }
  }
  return Rts_S32;
}

std::string Bof_ProtocolTypeToString(BOF_PROTOCOL_TYPE _ProtocolType_E)
{
  std::string Rts_S;

  Rts_S = S_ToProtocolTypeString[_ProtocolType_E];

  return Rts_S;
}

std::string Bof_SocketAddressToString(const BOF_SOCKET_ADDRESS &_rIpAddress_X, bool _ShowType_B, bool _ShowPortNumber_B)
{
  std::string Rts_S;

  if (_rIpAddress_X.IpV6_B)
  {
    if (_ShowType_B)
    {
      Rts_S = S_ToProtocolTypeString[_rIpAddress_X.ProtocolType_E] + "://";
    }
    Rts_S += Bof_SockAddrInToString(_rIpAddress_X.IpV6Address_X, _ShowPortNumber_B);
  }
  else
  {
    if (_ShowType_B)
    {
      Rts_S = S_ToProtocolTypeString[_rIpAddress_X.ProtocolType_E] + "://";
    }
    Rts_S += Bof_SockAddrInToString(_rIpAddress_X.IpV4Address_X, _ShowPortNumber_B);
  }
  return Rts_S;
}
/*
std::string Bof_SockAddrInToString(const struct sockaddr &_rSockAddress_X, bool _ShowPortNumber_B)
{
	BOF_SOCKADDR_IN SockAddressIn_X;

	SockAddressIn_X = *reinterpret_cast<const BOF_SOCKADDR_IN *>(&_rSockAddress_X);
	return (Bof_SockAddrInToString(SockAddressIn_X, _ShowPortNumber_B));
}
*/
std::string Bof_SockAddrInToString(const BOF_SOCKADDR_IN &_rSockAddressIn_X, bool _ShowPortNumber_B)
{
  std::string Rts_S;

  Rts_S += Bof_Sprintf("%s", inet_ntoa(_rSockAddressIn_X.sin_addr));
  if (_ShowPortNumber_B)
  {
    Rts_S += Bof_Sprintf(":%d", ntohs(_rSockAddressIn_X.sin_port));
  }
  return Rts_S;
}

std::string Bof_SockAddrInToString(const BOF_SOCKADDR_IN6 &_rSockAddressIn_X, bool _ShowPortNumber_B)
{
  std::string Rts_S;
  char        *p_c, pToString_c[0x100];

  if (inet_ntop(AF_INET6, (void *) (&_rSockAddressIn_X.sin6_addr), &pToString_c[1], sizeof(pToString_c))) // (8 * 4) + 7 + 1))
  {
    pToString_c[0] = '[';
    p_c = &pToString_c[strlen(pToString_c)];
    if (_ShowPortNumber_B)
    {
      sprintf(p_c, "]:%d", ntohs(_rSockAddressIn_X.sin6_port));
    }
    else
    {
      strcat(p_c, "]");
    }
    Rts_S = pToString_c;
  }
  return Rts_S;
}

BOFERR Bof_UrlAddressToSocketAddressCollection(const std::string &_rIpOrUrlAddress_S, std::vector<BOF_SOCKET_ADDRESS> &_rListOfIpAddress_X)
{
  BOFERR             Rts_E = BOF_ERR_EINVAL;
  struct addrinfo    Hint_X, *pServerInfo_X, *pInfo_X;
  BOF_SOCKET_ADDRESS Ip_X;
  std::string        IpAddress_S, TheIpAddress_S;
  uint16_t           Port_U16;

  _rListOfIpAddress_X.clear();
  IpAddress_S = Bof_StringTrim(_rIpOrUrlAddress_S);
  if (IpAddress_S != "")
  {
    Rts_E = Bof_IpAddressToSocketAddress(IpAddress_S, Ip_X, &TheIpAddress_S);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      _rListOfIpAddress_X.push_back(Ip_X);
    }
    else
    {
      Port_U16 = Ip_X.Port();
      if (Ip_X.IpV6_B)
      {
        memset(&Hint_X, 0, sizeof Hint_X);
        Hint_X.ai_family = AF_INET6;      // AF_UNSPEC; // use AF_INET6 to force IPv6
        // Hint_X.ai_socktype = SOCK_STREAM;
        // Hint_X.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
        // Hint_X.ai_protocol = IPPROTO_TCP;		//0;          /* Any protocol */

//        Rts_E = (getaddrinfo(IpV6Ipaddress_S.c_str(), nullptr, &Hint_X, &pServerInfo_X) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
        Rts_E = (getaddrinfo(TheIpAddress_S.c_str(), nullptr, &Hint_X, &pServerInfo_X) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          Rts_E        = BOF_ERR_NOT_FOUND;
          for (pInfo_X = pServerInfo_X; pInfo_X != nullptr; pInfo_X = pInfo_X->ai_next)
          {
            Ip_X.IpV6Address_X = *reinterpret_cast<BOF_SOCKADDR_IN6 *> (pInfo_X->ai_addr);
            Ip_X.IpV6Address_X.sin6_port = htons(Port_U16);
            _rListOfIpAddress_X.push_back(Ip_X);
          }
          Rts_E = BOF_ERR_NO_ERROR;

          freeaddrinfo(pServerInfo_X);   // all done with this structure
        }
      }
      else
      {
        memset(&Hint_X, 0, sizeof Hint_X);
        Hint_X.ai_family = AF_INET;      // AF_UNSPEC; // use AF_INET6 to force IPv6
        // Hint_X.ai_socktype = SOCK_STREAM;
        // Hint_X.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
        // Hint_X.ai_protocol = IPPROTO_TCP;		//0;          /* Any protocol */
        //Hint_X.ai_flags=AI_NUMERICHOST;

        Rts_E = (getaddrinfo(TheIpAddress_S.c_str(), nullptr, &Hint_X, &pServerInfo_X) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          Rts_E        = BOF_ERR_NOT_FOUND;
          for (pInfo_X = pServerInfo_X; pInfo_X != nullptr; pInfo_X = pInfo_X->ai_next)
          {
            Ip_X.IpV4Address_X = *reinterpret_cast<BOF_SOCKADDR_IN *> (pInfo_X->ai_addr);
            Ip_X.IpV4Address_X.sin_port = htons(Port_U16);
            _rListOfIpAddress_X.push_back(Ip_X);
          }
          Rts_E = BOF_ERR_NO_ERROR;

          freeaddrinfo(pServerInfo_X);   // all done with this structure
        }
      }
    }
  }
  return Rts_E;
}
BOFERR Bof_IpAddressToSocketAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS &_rIpAddress_X, std::string *_pIpAddress_S)
{
  BOFERR                 Rts_E = BOF_ERR_EINVAL;
  std::string::size_type PosColon, PosStartBracket, PosEndBracket;
  std::string            IpAddress_S;
  uint16_t               Port_U16;
  BOF_SOCKET_ADDRESS     Ip_X;
  int                    Port_i;

  IpAddress_S = Bof_StringTrim(_rIpAddress_S);
  if (IpAddress_S != "")
  {
    Rts_E           = BOF_ERR_NO_ERROR;
    Port_U16        = 0;
    PosColon        = IpAddress_S.rfind(':');
    PosStartBracket = IpAddress_S.find('[');
    PosEndBracket   = IpAddress_S.rfind(']');

    if ((PosColon != std::string::npos) && ((PosColon > PosEndBracket) || (PosEndBracket == std::string::npos)))
    {
      if (PosColon == IpAddress_S.size() - 1)
      {
        Rts_E = BOF_ERR_EINVAL;
      }
      else
      {
        Rts_E = BOF_ERR_NO_ERROR;
        try
        {
//          Port_U16 = static_cast<uint16_t> (std::atoi(IpAddress_S.substr(PosColon + 1).c_str()));
          Port_i = std::stoi(IpAddress_S.substr(PosColon + 1));
          if ((Port_i >= 0) && (Port_i < 0x10000))
          {
            IpAddress_S = IpAddress_S.substr(0, PosColon);
            Port_U16    = static_cast<uint16_t>(Port_i);
          }
        }
        catch (const std::exception &)
        {

        }
      }
    }
    if ((Rts_E == BOF_ERR_NO_ERROR) && (IpAddress_S != ""))
    {
//"tcp://[102:3c0:405:6a8:708:901:a0b:c01]:1234"->IpAddress_S=tcp://[102:3c0:405:6a8:708:901:a0b:c01]
      Ip_X.IpV6_B       = false;
      Ip_X.SocketType_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
      Bof_ProtocolType(IpAddress_S, Ip_X.ProtocolType_E); //BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN;

      if (IpAddress_S.back() == ']')
      {
        if ((PosStartBracket != std::string::npos) && (PosEndBracket != std::string::npos) && (PosStartBracket + 2 < PosEndBracket)) //At least [::]
        {
          Ip_X.IpV6_B = true;
          IpAddress_S = IpAddress_S.substr(PosStartBracket + 1, PosEndBracket - PosStartBracket - 1);
        }
      }
      if (!IpAddress_S.compare(0, 6, "tcp://"))
      {
        Ip_X.SocketType_E = BOF_SOCK_TYPE::BOF_SOCK_TCP;
        IpAddress_S = IpAddress_S.substr(6);
      }
      else if (!IpAddress_S.compare(0, 6, "udp://"))
      {
        Ip_X.SocketType_E = BOF_SOCK_TYPE::BOF_SOCK_UDP;
        IpAddress_S = IpAddress_S.substr(6);
      }

      if (Ip_X.IpV6_B)
      {
        Ip_X.IpV6Address_X.sin6_port = htons(Port_U16);
      }
      else
      {
        Ip_X.IpV4Address_X.sin_port = htons(Port_U16);
      }
      _rIpAddress_X = Ip_X; //Return partial result to caller
      if (_pIpAddress_S)
      {
        *_pIpAddress_S = IpAddress_S;
      }
      if (Ip_X.IpV6_B)
      {
        if (inet_pton(AF_INET6, IpAddress_S.c_str(), &Ip_X.IpV6Address_X.sin6_addr.s6_addr) == 1)
        {
          Ip_X.IpV6Address_X.sin6_family = AF_INET6;

          _rIpAddress_X = Ip_X;
          // Rts_E = BOF_ERR_NO_ERROR;
        }
        else
        {
          Rts_E = BOF_ERR_FORMAT;
        }
      }
      else
      {
        if (inet_pton(AF_INET, IpAddress_S.c_str(), &Ip_X.IpV4Address_X.sin_addr) == 1)
        {
          Ip_X.IpV4Address_X.sin_family = AF_INET;

          _rIpAddress_X = Ip_X;
          // Rts_E = BOF_ERR_NO_ERROR;
        }
        else
        {
          Rts_E = BOF_ERR_FORMAT;
        }
      }
    }
  }
  return Rts_E;
}

bool Bof_IsIpAddressEqual(bool CheckType_B, bool _CheckFamily_B, bool _CheckPort_B, const BOF_SOCKET_ADDRESS &_rSrcIpAddress_X, const BOF_SOCKET_ADDRESS &_rDstIpAddress_X)
{
  bool Rts_B = false;

  if (_rSrcIpAddress_X.IpV6_B == _rDstIpAddress_X.IpV6_B)
  {
    if (_rSrcIpAddress_X.IpV6_B)
    {
      Rts_B = (memcmp(_rSrcIpAddress_X.IpV6Address_X.sin6_addr.s6_addr, _rDstIpAddress_X.IpV6Address_X.sin6_addr.s6_addr, 16) == 0);

      if ((Rts_B) && (CheckType_B))
      {
        Rts_B = (_rSrcIpAddress_X.SocketType_E == _rDstIpAddress_X.SocketType_E);
      }

      if ((Rts_B) && (_CheckFamily_B))
      {
        Rts_B = (_rSrcIpAddress_X.IpV6Address_X.sin6_family == _rDstIpAddress_X.IpV6Address_X.sin6_family);
      }

      if ((Rts_B) && (_CheckPort_B))
      {
        Rts_B = (_rSrcIpAddress_X.IpV6Address_X.sin6_port == _rDstIpAddress_X.IpV6Address_X.sin6_port);
      }
    }
    else
    {
      Rts_B = (_rSrcIpAddress_X.IpV4Address_X.sin_addr.s_addr == _rDstIpAddress_X.IpV4Address_X.sin_addr.s_addr);

      if ((Rts_B) && (CheckType_B))
      {
        Rts_B = (_rSrcIpAddress_X.SocketType_E == _rDstIpAddress_X.SocketType_E);
      }

      if ((Rts_B) && (_CheckFamily_B))
      {
        Rts_B = (_rSrcIpAddress_X.IpV4Address_X.sin_family == _rDstIpAddress_X.IpV4Address_X.sin_family);
      }

      if ((Rts_B) && (_CheckPort_B))
      {
        Rts_B = (_rSrcIpAddress_X.IpV4Address_X.sin_port == _rDstIpAddress_X.IpV4Address_X.sin_port);
      }
    }
  }
  return Rts_B;
}

bool Bof_IsIpAddressNull(const BOF_SOCKET_ADDRESS &_rIpAddress_X)
{
  bool     Rts_B;
  uint32_t i_U32;

  if (_rIpAddress_X.IpV6_B)
  {
    for (i_U32 = 0; i_U32 < 16; i_U32++)
    {
      if (_rIpAddress_X.IpV6Address_X.sin6_addr.s6_addr[i_U32])
      {
        break;
      }
    }
    Rts_B      = (i_U32 == 16);
  }
  else
  {
    Rts_B = (_rIpAddress_X.IpV4Address_X.sin_addr.s_addr == 0);  //Port is not used for null
  }
  return Rts_B;
}

bool Bof_IsIpAddressNull(const std::string &_rIpAddress_S)
{
  bool               Rts_B = true;
  BOF_SOCKET_ADDRESS IpAddress_X;

  if (Bof_IpAddressToSocketAddress(_rIpAddress_S, IpAddress_X) == BOF_ERR_NO_ERROR)
  {
    Rts_B = Bof_IsIpAddressNull(IpAddress_X);
  }
  return Rts_B;
}

bool Bof_IsIpAddressLocalHost(const BOF_SOCKET_ADDRESS &_rIpAddress_X)
{
  bool     Rts_B = false;
  uint32_t i_U32;

  if (_rIpAddress_X.IpV6_B)
  {
//::1 = 0:0:0:0:0:0:0:1
    for (i_U32 = 0; i_U32 < 7; i_U32++)
    {
#if defined(_WIN32)
      if (_rIpAddress_X.IpV6Address_X.sin6_addr.u.Word[i_U32])
#else
      if (_rIpAddress_X.IpV6Address_X.sin6_addr.s6_addr16[i_U32])
#endif
      {
        break;
      }
    }
    if (i_U32 == 7)
    {
#if defined(_WIN32)
      Rts_B = (ntohs(_rIpAddress_X.IpV6Address_X.sin6_addr.u.Word[7]) == 1);
#else
      Rts_B = (ntohs(_rIpAddress_X.IpV6Address_X.sin6_addr.s6_addr16[7]) == 1);
#endif
    }
  }
  else
  {
    Rts_B = (ntohl(_rIpAddress_X.IpV4Address_X.sin_addr.s_addr) == 0x7F000001);
  }
  return Rts_B;
}

bool Bof_IsIpAddressLocalHost(const std::string &_rIpAddress_S)
{
  bool               Rts_B = true;
  BOF_SOCKET_ADDRESS IpAddress_X;

  if (Bof_IpAddressToSocketAddress(_rIpAddress_S, IpAddress_X) == BOF_ERR_NO_ERROR)
  {
    Rts_B = Bof_IsIpAddressLocalHost(IpAddress_X);
  }
  return Rts_B;
}


BOF_SOCK_TYPE Bof_ProtocolToSocketType(BOF_PROTOCOL_TYPE _Protocol_E)
{
  BOF_SOCK_TYPE Rts_E;

  if ((_Protocol_E == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP) || (_Protocol_E == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_TFTP) || (_Protocol_E == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_ZMQ_PGM))
  {
    Rts_E = BOF_SOCK_TYPE::BOF_SOCK_UDP;
  }
  else if ((_Protocol_E == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_PIPE) || (_Protocol_E == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_SHARED_RAM) || (_Protocol_E == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN))
  {
    Rts_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
  }
  else
  {
    Rts_E = BOF_SOCK_TYPE::BOF_SOCK_TCP;
  }
  return Rts_E;
}

BOFERR Bof_ProtocolType(const std::string &_rIpAddress_S, BOF_PROTOCOL_TYPE &_rProtocolType_E)
{
  BOFERR                 Rts_E = BOF_ERR_DONT_EXIST;
  std::string::size_type Pos;
  std::string            Protocol_S;

  _rProtocolType_E = BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN;
  Pos              = _rIpAddress_S.find("://");
  if (Pos != std::string::npos)
  {
    Protocol_S = _rIpAddress_S.substr(0, Pos);
  }
  else
  {
    Protocol_S = _rIpAddress_S;
  }
  auto It = S_ToProtocolType.find(Protocol_S);
  if (It != S_ToProtocolType.end())
  {
    _rProtocolType_E = It->second;
    Rts_E            = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

//std::vector<uint16_t> Bof_IpAddressToBin(const std::string &_rIpAddress_S, bool &_rIsIpV6_B)
BOFERR Bof_IpAddressToBin(const std::string &_rIpAddress_S, bool &_rIsIpV6_B, std::vector<uint16_t> &_rIpBinDigitCollection)
{
  BOFERR             Rts_E;
  BOF_SOCKET_ADDRESS InterfaceIpAddress_X, IpAddress_X;

  _rIsIpV6_B = false;
  _rIpBinDigitCollection.clear();
  Rts_E = Bof_ResolveIpAddress(_rIpAddress_S, InterfaceIpAddress_X, IpAddress_X);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Rts_E = Bof_SocketAddressToBin(IpAddress_X, _rIpBinDigitCollection);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      _rIsIpV6_B = IpAddress_X.IpV6_B;
    }
  }

  return Rts_E;
}

/*
IPv4 multicast addresses are defined by the leading address bits of 1110, originating from the classful network design of the early Internet when this group 
of addresses was designated as Class D. The Classless Inter-Domain Routing (CIDR) prefix of this group is 224.0.0.0/4. The group includes the addresses from 
224.0.0.0 (0xE0) to 239.255.255.255 (0xEF). Address assignments from within this range are specified in RFC 5771, an Internet Engineering Task Force (IETF) Best Current 
Practice document (BCP 51).

The address range is divided into blocks each assigned a specific purpose or behavior.
IP multicast address range	Description	Routable
224.0.0.0 to 224.0.0.255	Local subnetwork[1]	No
224.0.1.0 to 224.0.1.255	Internetwork control	Yes
224.0.2.0 to 224.0.255.255	AD-HOC block 1[2]	Yes
224.3.0.0 to 224.4.255.255	AD-HOC block 2[3]	Yes
232.0.0.0 to 232.255.255.255	Source-specific multicast[1]	Yes
233.0.0.0 to 233.255.255.255	GLOP addressing[1]	Yes
233.252.0.0 to 233.255.255.255	AD-HOC block 3[4]	Yes
234.0.0.0 to 234.255.255.255	Unicast-prefix-based	Yes
239.0.0.0 to 239.255.255.255	Administratively scoped[1]	Yes
 */
bool Bof_IsMulticastIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS_COMPONENT &_rInterfaceIpAddressComponent_X, BOF_SOCKET_ADDRESS_COMPONENT &_rIpAddressComponent_X)
{
  bool                  Rts_B = false;
  BOFERR                Sts_E;
  uint32_t              NbElem_U32;
  std::vector<uint16_t> IpDigitCollection;
  bool                  IsIpV6_B;
  BOF_PROTOCOL_TYPE     InterfaceProtocolType_E, ProtocolType_E;

  Sts_E = Bof_SplitIpAddress(_rIpAddress_S, _rInterfaceIpAddressComponent_X, _rIpAddressComponent_X);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    Sts_E = BOF_ERR_INTERFACE;
    Bof_ProtocolType(_rInterfaceIpAddressComponent_X.Protocol_S, InterfaceProtocolType_E);
    if (InterfaceProtocolType_E == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN)
    {
      InterfaceProtocolType_E = BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP;
    }
    if ((InterfaceProtocolType_E == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP) || (InterfaceProtocolType_E == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_ZMQ_PGM))
    {
      Sts_E = BOF_ERR_PROTOCOL;
      Bof_ProtocolType(_rIpAddressComponent_X.Protocol_S, ProtocolType_E);
      /*no
      if (ProtocolType_E == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN)
      {
        ProtocolType_E = BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP;
      }
      */
      if ((ProtocolType_E == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UDP) || (ProtocolType_E == BOF_PROTOCOL_TYPE::BOF_PROTOCOL_ZMQ_PGM))
      {
        if (InterfaceProtocolType_E == ProtocolType_E)
        {
          Bof_IpAddressToBin(_rIpAddress_S, IsIpV6_B, IpDigitCollection);
          NbElem_U32 = IsIpV6_B ? 8 : 4;
          if (IpDigitCollection.size() == NbElem_U32)
          {
            Rts_B = IsIpV6_B ? ((IpDigitCollection[0] & 0xFF00) != 0) : ((IpDigitCollection[0] & 0x00E0) != 0);
          }
        }
      }
    }
  }
  return Rts_B;
}

/*
   zmq style address:

   tcp://127.0.0.1:*														bind to first free port from C000 up
   tcp://127.0.0.1:!													bind to random port from C000 to FFFF
   tcp://127.0.0.1:*[60000-]									bind to first free port from 60000 up
   tcp://127.0.0.1:![-60000]									bind to random port from C000 to 60000
   tcp://127.0.0.1:![55000-55999]							bind to random port from 55000 to 55999
   pgm://192.168.1.1;239.192.1.1:5555"					multicast address from local interface 192.168.1.1 to group 239.192.1.1 on port 5555
   pgm://192.168.1.1:1234;239.192.1.1:5555"		multicast address from local interface 192.168.1.1 port 1234 to group 239.192.1.1 on port 5555
 */
//TODO faire ipv6 version : is sep for port and :: is for ipv6
BOFERR Bof_SplitIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS_COMPONENT &_rInterfaceIpAddress_X, BOF_SOCKET_ADDRESS_COMPONENT &_rIpAddress_X)
{
  BOFERR                 Rts_E = BOF_ERR_TOO_SMALL;
  std::string            Protocol_S, Address_S, Interface_S, TheAddress_S;
  std::string::size_type PosEop, PosAfter, PosSemiColon, PosColon;
  BOF_PROTOCOL_TYPE      Protocol_E;
  BOF_SOCKET_ADDRESS     IpAddress_X;

  _rInterfaceIpAddress_X.Reset();
  _rIpAddress_X.Reset();
  TheAddress_S = _rIpAddress_S;
  if (TheAddress_S.size() > 6)
  {
    Rts_E  = BOF_ERR_FORMAT;
    PosEop = TheAddress_S.find("://");
    if (PosEop == std::string::npos)
    {
      TheAddress_S = "???://" + TheAddress_S;
      PosEop       = TheAddress_S.find("://");
    }
    if (PosEop != std::string::npos)
    {
      Protocol_S = TheAddress_S.substr(0, PosEop);
      PosAfter   = PosEop + 3;

      PosSemiColon = TheAddress_S.find(';', PosAfter);
      if (PosSemiColon != std::string::npos)
      {
        Interface_S = TheAddress_S.substr(PosAfter, PosSemiColon - PosAfter);
        Address_S   = TheAddress_S.substr(PosSemiColon + 1);
      }
      else
      {
        Interface_S = "";
        Address_S   = TheAddress_S.substr(PosAfter);
      }
      Rts_E        = Bof_ProtocolType(Protocol_S, Protocol_E);
    }

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if ((Interface_S == "") && (Address_S == ""))
      {
        Rts_E = BOF_ERR_ADDRESS;
      }
    }

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (Interface_S != "")
      {
        _rInterfaceIpAddress_X.Protocol_S = Protocol_S;
        PosColon = Interface_S.rfind(':');
        if (PosColon != std::string::npos)
        {
          _rInterfaceIpAddress_X.IpAddress_S = Interface_S.substr(0, PosColon);
          try
          {
            _rInterfaceIpAddress_X.Port_U16 = static_cast<uint16_t>(std::stoi(Interface_S.substr(PosColon + 1)));
          }
          catch (const std::exception &)
          {
            Rts_E = BOF_ERR_EINVAL;
          }
        }
        else
        {
          _rInterfaceIpAddress_X.IpAddress_S = Interface_S.substr(0, PosColon);
          _rInterfaceIpAddress_X.Port_U16    = 0;
        }
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          Rts_E = Bof_IpAddressToSocketAddress(_rInterfaceIpAddress_X.IpAddress_S, IpAddress_X);
        }
      }
    }

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (Address_S != "")
      {
        _rIpAddress_X.Protocol_S = Protocol_S;
        PosColon = Address_S.rfind(':');
        if (PosColon != std::string::npos)
        {
          _rIpAddress_X.IpAddress_S = Address_S.substr(0, PosColon);
          try
          {
            _rIpAddress_X.Port_U16 = static_cast<uint16_t>(std::stoi(Address_S.substr(PosColon + 1)));
          }
          catch (const std::exception &)
          {
            Rts_E = BOF_ERR_EINVAL;
          }
        }
        else
        {
          _rIpAddress_X.IpAddress_S = Address_S.substr(0, PosColon);
          _rIpAddress_X.Port_U16    = 0;
        }
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          Rts_E = Bof_IpAddressToSocketAddress(_rIpAddress_X.IpAddress_S, IpAddress_X);
        }
      }
    }
  }
  return Rts_E;
}

BOFERR Bof_SplitIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS &_rInterfaceIpAddress_X, BOF_SOCKET_ADDRESS &_rIpAddress_X)
{
  BOFERR                       Rts_E;
  BOF_SOCKET_ADDRESS_COMPONENT InterfaceIpAddressComponent_X, IpAddressComponent_X;
  bool                         InterfaceIsIpV6_B, IsIpV6_B;
  BOF_PROTOCOL_TYPE            InterfaceProtocolType_E = BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN, ProtocolType_E;
  BOF_SOCK_TYPE                InterfaceSockType_E, SockType_E;
  std::vector<uint16_t>        IpDigitCollection;
  uint32_t                     i_U32, Index_U32, pIp_U32[4];

  Rts_E = Bof_SplitIpAddress(_rIpAddress_S, InterfaceIpAddressComponent_X, IpAddressComponent_X);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    if (InterfaceIpAddressComponent_X.IpAddress_S != "")
    {
      Bof_ProtocolType(InterfaceIpAddressComponent_X.Protocol_S, InterfaceProtocolType_E);
      InterfaceSockType_E = Bof_ProtocolToSocketType(InterfaceProtocolType_E);
      Rts_E               = Bof_IpAddressToBin(Bof_ProtocolTypeToString(InterfaceProtocolType_E) + "://" + InterfaceIpAddressComponent_X.IpAddress_S, InterfaceIsIpV6_B, IpDigitCollection);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Rts_E = BOF_ERR_FORMAT;
        if (IpDigitCollection.size() >= 4)
        {
          if (InterfaceIsIpV6_B)
          {
            Index_U32  = 0;
            for (i_U32 = 0; i_U32 < 4; i_U32++)
            {
              //	        pIp_U32[i_U32] = ((static_cast<uint32_t>(IpDigitCollection[Index_U32++]) << 16) | (IpDigitCollection[Index_U32++]));
              pIp_U32[i_U32] = static_cast<uint32_t>(IpDigitCollection[Index_U32++]) << 16;
              pIp_U32[i_U32] |= static_cast<uint32_t>(IpDigitCollection[Index_U32++]);
            }
          }
          else
          {
            for (i_U32 = 0; i_U32 < 4; i_U32++)
            {
              pIp_U32[i_U32] = IpDigitCollection[i_U32];
            }
          }
        }
      }
    }
    else
    {
      InterfaceIsIpV6_B   = false;
      InterfaceSockType_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
      pIp_U32[0] = 0;
      pIp_U32[1] = 0;
      pIp_U32[2] = 0;
      pIp_U32[3] = 0;
      InterfaceIpAddressComponent_X.Port_U16 = 0;
    }
    _rInterfaceIpAddress_X.Set(InterfaceIsIpV6_B, InterfaceSockType_E, InterfaceProtocolType_E, pIp_U32[0], pIp_U32[1], pIp_U32[2], pIp_U32[3], InterfaceIpAddressComponent_X.Port_U16);

    if (IpAddressComponent_X.IpAddress_S != "")
    {
      IpDigitCollection.clear();
      Bof_ProtocolType(IpAddressComponent_X.Protocol_S, ProtocolType_E);
      SockType_E = Bof_ProtocolToSocketType(ProtocolType_E);
      Bof_IpAddressToBin(Bof_ProtocolTypeToString(ProtocolType_E) + "://" + IpAddressComponent_X.IpAddress_S, IsIpV6_B, IpDigitCollection);
      if (IpDigitCollection.size() >= 4)
      {
        if (IsIpV6_B)
        {
          Index_U32  = 0;
          for (i_U32 = 0; i_U32 < 4; i_U32++)
          {
            //	          pIp_U32[i_U32] = ((static_cast<uint32_t>(IpDigitCollection[Index_U32++]) << 16) | (IpDigitCollection[Index_U32++]));
            pIp_U32[i_U32] = static_cast<uint32_t>(IpDigitCollection[Index_U32++]) << 16;
            pIp_U32[i_U32] |= static_cast<uint32_t>(IpDigitCollection[Index_U32++]);
          }
        }
        else
        {
          for (i_U32 = 0; i_U32 < 4; i_U32++)
          {
            pIp_U32[i_U32] = IpDigitCollection[i_U32];
          }
        }
      }
      else
      {
        InterfaceIsIpV6_B   = false;
        InterfaceSockType_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
        pIp_U32[0] = 0;
        pIp_U32[1] = 0;
        pIp_U32[2] = 0;
        pIp_U32[3] = 0;
        InterfaceIpAddressComponent_X.Port_U16 = 0;
      }
      _rIpAddress_X.Set(IsIpV6_B, SockType_E, ProtocolType_E, pIp_U32[0], pIp_U32[1], pIp_U32[2], pIp_U32[3], IpAddressComponent_X.Port_U16);
    }
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

BOFERR Bof_SplitIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS_COMPONENT &_rIpAddress_X)
{
  BOF_SOCKET_ADDRESS_COMPONENT InterfaceIpAddress_X;

  return Bof_SplitIpAddress(_rIpAddress_S, InterfaceIpAddress_X, _rIpAddress_X);
}

BOFERR Bof_SplitIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS &_rIpAddress_X)
{
  BOF_SOCKET_ADDRESS InterfaceIpAddress_X;

  return Bof_SplitIpAddress(_rIpAddress_S, InterfaceIpAddress_X, _rIpAddress_X);
}


BOFERR Bof_ResolveIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS &_rInterfaceIpAddress_X, BOF_SOCKET_ADDRESS &_rIpAddress_X)
{
  BOFERR                 Rts_E = BOF_ERR_TOO_SMALL;
  std::string            Protocol_S, Address_S, Interface_S;
  std::string::size_type PosEop, PosAfter, PosSemiColon;
  BOF_SOCK_TYPE          Type_E;
  BOF_PROTOCOL_TYPE      Protocol_E;

  if (_rIpAddress_S.size() > 6)
  {
    Type_E     = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
    Protocol_E = BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN;

    _rInterfaceIpAddress_X.IpV6_B                        = false;
    _rInterfaceIpAddress_X.SocketType_E                  = Type_E;
    _rInterfaceIpAddress_X.ProtocolType_E                = Protocol_E;
    _rInterfaceIpAddress_X.IpV4Address_X.sin_addr.s_addr = 0;
    _rInterfaceIpAddress_X.IpV4Address_X.sin_port        = 0;
    _rInterfaceIpAddress_X.IpV4Address_X.sin_family      = 0;
    _rIpAddress_X.IpV6_B                                 = false;
    _rIpAddress_X.SocketType_E                           = Type_E;
    _rIpAddress_X.ProtocolType_E                         = Protocol_E;
    _rIpAddress_X.IpV4Address_X.sin_addr.s_addr          = 0;
    _rIpAddress_X.IpV4Address_X.sin_port                 = 0;
    _rIpAddress_X.IpV4Address_X.sin_family               = 0;
    Rts_E  = BOF_ERR_FORMAT;
    PosEop = _rIpAddress_S.find("://");

    if (PosEop != std::string::npos)
    {
      Protocol_S = _rIpAddress_S.substr(0, PosEop);
      PosAfter   = PosEop + 3;

      PosSemiColon = _rIpAddress_S.find(';', PosAfter);
      if (PosSemiColon != std::string::npos)
      {
        Interface_S = _rIpAddress_S.substr(PosAfter, PosSemiColon - PosAfter);
        Address_S   = _rIpAddress_S.substr(PosSemiColon + 1);
      }
      else
      {
        Interface_S = "";
        Address_S   = _rIpAddress_S.substr(PosAfter);
      }
      Rts_E        = Bof_ProtocolType(Protocol_S, Protocol_E);
      Type_E       = Bof_ProtocolToSocketType(Protocol_E);
    }
    else
    {
      Protocol_S  = "???";
      PosAfter    = 0;
      Interface_S = "";
      Address_S   = _rIpAddress_S.substr(PosAfter);
      Rts_E       = Bof_ProtocolType(Protocol_S, Protocol_E);
      Type_E      = Bof_ProtocolToSocketType(Protocol_E);
    }

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if ((Interface_S == "") && (Address_S == ""))
      {
        Rts_E = BOF_ERR_ADDRESS;
      }
    }

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (Interface_S != "")
      {
        Rts_E = Bof_IpAddressToSocketAddress(Interface_S, _rInterfaceIpAddress_X);
        _rInterfaceIpAddress_X.SocketType_E   = Type_E;
        _rInterfaceIpAddress_X.ProtocolType_E = Protocol_E;
      }
    }

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (Address_S != "")
      {
        Rts_E = Bof_IpAddressToSocketAddress(Address_S, _rIpAddress_X);
        _rIpAddress_X.SocketType_E   = Type_E;
        _rIpAddress_X.ProtocolType_E = Protocol_E;
      }
    }
  }
  return Rts_E;
}

std::string Bof_BuildIpAddress(const BOF_SOCKET_ADDRESS &_rInterfaceIpAddress_X, const BOF_SOCKET_ADDRESS &_rIpAddress_X)
{
  std::string Rts_S;


  //NO INADDR_ANY	if (! Bof_IsIpAddressNull(_rInterfaceIpAddress_X) )
  {
    Rts_S = Bof_Sprintf("%s://%s", S_ToProtocolTypeString[_rInterfaceIpAddress_X.ProtocolType_E].c_str(), Bof_SocketAddressToString(_rInterfaceIpAddress_X, false, true).c_str());
  }
  if (!Bof_IsIpAddressNull(_rIpAddress_X))
  {
    if (!Bof_IsIpAddressNull(_rInterfaceIpAddress_X))
    {
      Rts_S += Bof_Sprintf(";%s", Bof_SocketAddressToString(_rIpAddress_X, false, true).c_str());
    }
    else
    {
      Rts_S = Bof_Sprintf("%s://%s", S_ToProtocolTypeString[_rIpAddress_X.ProtocolType_E].c_str(), Bof_SocketAddressToString(_rIpAddress_X, false, true).c_str());
    }
  }

  return Rts_S;
}

BOFERR Bof_GetCompatibleIpAddress(const std::vector<BOF_NETWORK_INTERFACE_PARAM> &_rListOfNetworkInterface_X, const BOF_SOCKET_ADDRESS &_rIpAddress_X, BOF_SOCKET_ADDRESS &_rCompatibleIpAddress_X)
{
  BOFERR             Rts_E = BOF_ERR_NOT_FOUND;
  uint32_t           i_U32, j_U32, TargetIp_U32, Ip_U32, Mask_U32, *pIp_U32, *pMask_U32;
  const uint32_t     *pTargetIp_U32;
  BOF_SOCKET_ADDRESS Ip_X, Mask_X, Gw_X;
  BOF_INTERFACE_INFO InterfaceInfo_X;

  _rCompatibleIpAddress_X.IpV6_B = _rIpAddress_X.IpV6_B;
  Ip_X.IpV6_B                    = _rIpAddress_X.IpV6_B;
  Mask_X.IpV6_B                  = _rIpAddress_X.IpV6_B;
  Gw_X.IpV6_B                    = _rIpAddress_X.IpV6_B;
  pTargetIp_U32 = nullptr;
  TargetIp_U32  = 0;
  _rCompatibleIpAddress_X.Reset();
  if (_rIpAddress_X.IpV6_B)
  {
    pTargetIp_U32 = reinterpret_cast<const uint32_t *>(_rIpAddress_X.IpV6Address_X.sin6_addr.s6_addr);
  }
  else
  {
    TargetIp_U32 = ntohl(_rIpAddress_X.IpV4Address_X.sin_addr.s_addr);
  }
  for (i_U32    = 0; i_U32 < _rListOfNetworkInterface_X.size(); i_U32++)
  {
    if (Bof_IsIpAddressNull(Gw_X))
    {
      if (Bof_GetNetworkInterfaceInfo(_rListOfNetworkInterface_X[i_U32].Name_S, InterfaceInfo_X) == BOF_ERR_NO_ERROR)
      {
        if (Bof_IpAddressToSocketAddress(InterfaceInfo_X.IpGateway_S, Gw_X) == BOF_ERR_NO_ERROR)
        {

        }
      }
    }
    if (Bof_IpAddressToSocketAddress(_rListOfNetworkInterface_X[i_U32].IpAddress_S, Ip_X) == BOF_ERR_NO_ERROR)
    {
      if (_rIpAddress_X.IpV6_B)
      {
        pIp_U32 = reinterpret_cast<uint32_t *>(Ip_X.IpV6Address_X.sin6_addr.s6_addr);
        if (Bof_IpAddressToSocketAddress(_rListOfNetworkInterface_X[i_U32].IpMask_S, Mask_X) == BOF_ERR_NO_ERROR)
        {
          pMask_U32  = reinterpret_cast<uint32_t *>(Mask_X.IpV6Address_X.sin6_addr.s6_addr);
          for (j_U32 = 0; j_U32 < 4; j_U32++)
          {
            if ((pMask_U32[j_U32] & pIp_U32[j_U32]) != (pMask_U32[j_U32] & pTargetIp_U32[j_U32]))
            {
              break;
            }
          }
          if (j_U32 == 4)
          {
            _rCompatibleIpAddress_X = Ip_X;
            break;
          }
        }
      }
      else
      {
        Ip_U32 = ntohl(Ip_X.IpV4Address_X.sin_addr.s_addr);
        if (Bof_IpAddressToSocketAddress(_rListOfNetworkInterface_X[i_U32].IpMask_S, Mask_X) == BOF_ERR_NO_ERROR)
        {
          Mask_U32 = ntohl(Mask_X.IpV4Address_X.sin_addr.s_addr);
          if ((Mask_U32 & Ip_U32) == (Mask_U32 & TargetIp_U32))
          {
            _rCompatibleIpAddress_X = Ip_X;
            break;
          }
        }
      }
    }
  }

  if (Bof_IsIpAddressNull(_rCompatibleIpAddress_X))
  {
    _rCompatibleIpAddress_X = Gw_X;
  }
  if (!Bof_IsIpAddressNull(_rCompatibleIpAddress_X))
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

BOFERR Bof_SocketAddressToBin(BOF_SOCKET_ADDRESS &_rIpAddress_X, std::vector<uint16_t> &_rBinFormat)
{
  BOFERR   Rts_E = BOF_ERR_EINVAL;
  uint32_t i_U32;

  if (_rIpAddress_X.IpV6_B)
  {
    uint16_t *pIp_U16, Val_U16;

//		BOF_ASSERT(_rBinFormat.size() >= 16);
    pIp_U16 = reinterpret_cast<uint16_t *>(&_rIpAddress_X.IpV6Address_X.sin6_addr.s6_addr); //it is in network order->big endian
    if (pIp_U16)
    {
      for (i_U32 = 0; i_U32 < 8; i_U32++)
      {
        Val_U16 = ntohs(pIp_U16[i_U32]);
        _rBinFormat.push_back(Val_U16);
      }
//			_rBinFormat.resize(i_U32);
      Rts_E      = BOF_ERR_NO_ERROR;
    }
  }
  else
  {
    uint8_t *pIp_U8;

//		BOF_ASSERT(_rBinFormat.size() >= 4);
    pIp_U8 = reinterpret_cast<uint8_t *>(&_rIpAddress_X.IpV4Address_X.sin_addr.s_addr); //it is in network order->big endian
    if (pIp_U8)
    {
      for (i_U32 = 0; i_U32 < 4; i_U32++)
      {
        _rBinFormat.push_back(pIp_U8[i_U32]);
      }
//			_rBinFormat.resize(4);
      Rts_E      = BOF_ERR_NO_ERROR;
    }
  }
  return Rts_E;
}

BOFERR Bof_BinToSocketAddress(std::vector<uint16_t> &_rBinFormat, BOF_SOCKET_ADDRESS &_rIpAddress_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if (_rIpAddress_X.IpV6_B)
  {
    BOF_ASSERT(_rBinFormat.size() >= 8);
    uint16_t *pIp_U16;  // , Val_U16;

    pIp_U16 = reinterpret_cast<uint16_t *>(_rBinFormat.data()); //it is in network order-<big endian
    if (pIp_U16)
    {
      _rIpAddress_X.ProtocolType_E            = BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN;
      _rIpAddress_X.SocketType_E              = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
      _rIpAddress_X.IpV6Address_X.sin6_family = AF_INET6;
      _rIpAddress_X.IpV6Address_X.sin6_port   = htons(0);
//      Val_U16 = pIp_U16[i_U32];
      //?? Val_U16=htons(Val_U16);

      memcpy(&_rIpAddress_X.IpV6Address_X.sin6_addr, pIp_U16, 16);
      Rts_E = BOF_ERR_NO_ERROR;
    }
  }
  else
  {
    BOF_ASSERT(_rBinFormat.size() >= 4);
    uint16_t *pIp_U16;
    uint8_t  *pTargetIp_U8;
    int      i;

    pIp_U16 = reinterpret_cast<uint16_t *>(_rBinFormat.data()); //it is in network order-<big endian
    if (pIp_U16)
    {
      _rIpAddress_X.ProtocolType_E           = BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN;
      _rIpAddress_X.SocketType_E             = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
      _rIpAddress_X.IpV4Address_X.sin_family = AF_INET;
      _rIpAddress_X.IpV4Address_X.sin_port   = htons(0);
      pTargetIp_U8 = reinterpret_cast<uint8_t *>(&_rIpAddress_X.IpV4Address_X.sin_addr);
      for (i       = 0; i < 4; i++) //U16 value
      {
        pTargetIp_U8[i] = static_cast<uint8_t>(pIp_U16[i]);
      }
      Rts_E = BOF_ERR_NO_ERROR;
    }
  }
  return Rts_E;
}

BOFERR Bof_Poll(uint32_t _TimeoutInMs_U32, uint32_t _NbPollOpInList_U32, BOF_POLL_SOCKET *_pListOfPollOp_X, uint32_t &_rNbPollSet_U32)
{
  BOFERR   Rts_E = BOF_ERR_EINVAL;
  uint32_t i_U32, NbREvent_U32;
  int      Sts_i;

  _rNbPollSet_U32 = 0;
  NbREvent_U32    = 0;
  if (_pListOfPollOp_X)
  {
    Rts_E = BOF_ERR_EACCES;
    //It is not necessary to clear revents for any element prior to calling WSAPoll.
    /*
    for (i_U32 = 0; i_U32 < _NbPollOpInList_U32; i_U32++)
    {
      _pListOfPollOp_X[i_U32].Revent_U16 = 0;
    }
    */
#if defined (_WIN32)
    static_assert(sizeof(WSAPOLLFD) == sizeof(BOF_POLL_SOCKET),"Binary size differs !");
    INT Timeout = static_cast<int>(_TimeoutInMs_U32);
    Sts_i = WSAPoll(reinterpret_cast<LPWSAPOLLFD>(_pListOfPollOp_X), _NbPollOpInList_U32, Timeout);
    if (Sts_i != SOCKET_ERROR)
    {
      Rts_E = BOF_ERR_NO_ERROR;
      _rNbPollSet_U32 = static_cast<uint32_t>(Sts_i);
    }
    else
    {
      int32_t Error_S32;
      Sts_i = Bof_GetLastError(true, &Error_S32);
      _rNbPollSet_U32 = 0;
    }
#else
    //	printf("S1 %d/%d S2 %d/%d\n", sizeof(struct pollfd),sizeof(BOF_POLL_SOCKET),sizeof(int),sizeof(BOFSOCKET));
    static_assert(sizeof(struct pollfd) == sizeof(BOF_POLL_SOCKET), "Binary size differs !");
    int Timeout = static_cast<int>(_TimeoutInMs_U32);
    Sts_i = poll(reinterpret_cast<struct pollfd *>(_pListOfPollOp_X), _NbPollOpInList_U32, Timeout);
    if (Sts_i >= 0)
    {
      Rts_E           = BOF_ERR_NO_ERROR;
      _rNbPollSet_U32 = static_cast<uint32_t>(Sts_i);
    }
#endif
    //Sometime under windows at least in an ever looping BofIo_Test.OpenCloseSession test the _rNbPollSet_U32 (sts_i) value in not equal to the number of mpPollOp_X[i].Revent_U16 entries != from 0
    //We check it here and adjust
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      NbREvent_U32 = 0;
      for (i_U32   = 0; i_U32 < _NbPollOpInList_U32; i_U32++)
      {
        if (_pListOfPollOp_X[i_U32].Revent_U16)
        {
          if (_pListOfPollOp_X[i_U32].Revent_U16 & BOF_POLL_NVAL)      //Put -1 in Fd to disable entry
          {
            _pListOfPollOp_X[i_U32].Revent_U16 = 0;
          }
          else
          {
            NbREvent_U32++;
          }
        }
        else
        {
        }
      }
      if (NbREvent_U32 != _rNbPollSet_U32)
      {
        ////BOF_DBG_PRINTF("@@@%s Bof_Poll WARNING %d returned but %d set->Adjust to %d !!!\n", _rName_S.c_str(), _rNbPollSet_U32, NbREvent_U32, NbREvent_U32);
        _rNbPollSet_U32 = NbREvent_U32;
      }
    }
  }

  if (NbREvent_U32)
  {
    //BOF_DBG_PRINTF("@@@%s Bof_Poll %d/%d poll set->rts %X\n", _rName_S.c_str(), _rNbPollSet_U32, NbREvent_U32, Rts_E);
    for (i_U32 = 0; i_U32 < _NbPollOpInList_U32; i_U32++)
    {
      if (_pListOfPollOp_X[i_U32].Revent_U16)
      {
        //BOF_DBG_PRINTF("@@@%s Bof_Poll Fd[%d]=%08X evt %X\n", _rName_S.c_str(), i_U32, _pListOfPollOp_X[i_U32].Fd, _pListOfPollOp_X[i_U32].Revent_U16);
      }
    }
  }
  return Rts_E;
}
END_BOF_NAMESPACE()