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
#include <bofstd/bofsocketos.h>
#include <bofstd/bofstring.h>
#include <bofstd/bofsystem.h>

#include <map>
#include <regex>
#if defined(_WIN32)
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa365915(v=vs.85).aspx
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <winsock2.h>
#define WORKING_BUFFER_SIZE 0x4000;
#else
#include <arpa/inet.h>
#include <ifaddrs.h>
#if defined(__EMSCRIPTEN__)
#else
#include <linux/rtnetlink.h>
#endif
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSIZE 8192

struct route_info
{
  struct in_addr dstAddr;
  struct in_addr srcAddr;
  struct in_addr gateWay;
  char ifName[IF_NAMESIZE];
};
#endif

BEGIN_BOF_NAMESPACE()

BOFERR Bof_GetIpMaskGw(const char *_pName_c, BOF_SOCKADDR *_pIpAddress_X, BOF_SOCKADDR *_pNetMask_X, BOF_SOCKADDR *_pBroadcast_X, BOF_NETWORK_INTERFACE_PARAM &_rNetworkInterfaceParam_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  int Sts_i;
  char *pAsciiFriendlyName_c, pText_c[NI_MAXHOST];

  _rNetworkInterfaceParam_X.Reset();
  if ((_pName_c) && (_pIpAddress_X) && (_pNetMask_X) && (_pBroadcast_X))
  {
    _rNetworkInterfaceParam_X.Name_S = _pName_c;
    Rts_E = BOF_ERR_INTERNAL;
    Sts_i = getnameinfo(_pIpAddress_X, _pIpAddress_X->sa_family == AF_INET ? sizeof(BOF_SOCKADDR_IN) : sizeof(BOF_SOCKADDR_IN6), pText_c, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);
    if (Sts_i == 0)
    {
      //  Some platform's getnameinfo, like Solaris, appear not to append the
      //  interface name when parsing a link-local IPv6 address. These addresses
      //  cannot be used without the interface, so we must append it manually.
      if (_pIpAddress_X->sa_family == AF_INET6 && IN6_IS_ADDR_LINKLOCAL(&((BOF_SOCKADDR_IN6 *)_pIpAddress_X)->sin6_addr) && !strchr(pText_c, '%'))
      {
        strcat(pText_c, "%");
        strcat(pText_c, _pName_c);
      }
      _rNetworkInterfaceParam_X.IpAddress_S = pText_c;
      Sts_i = getnameinfo(_pNetMask_X, _pNetMask_X->sa_family == AF_INET ? sizeof(BOF_SOCKADDR_IN) : sizeof(BOF_SOCKADDR_IN6), pText_c, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);
      if (Sts_i == 0)
      {
        _rNetworkInterfaceParam_X.IpMask_S = pText_c;
        if (_pIpAddress_X->sa_family == AF_INET)
        {
          //  If the returned broadcast address is the same as source
          //  address, build the broadcast address from the source
          //  address and netmask.
          if (((BOF_SOCKADDR_IN *)_pIpAddress_X)->sin_addr.s_addr == ((BOF_SOCKADDR_IN *)_pBroadcast_X)->sin_addr.s_addr)
          {
            ((BOF_SOCKADDR_IN *)_pBroadcast_X)->sin_addr.s_addr |= ~(((BOF_SOCKADDR_IN *)_pNetMask_X)->sin_addr.s_addr);
          }
          Sts_i = getnameinfo(_pBroadcast_X, sizeof(BOF_SOCKADDR_IN), pText_c, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);
          if (Sts_i == 0)
          {
            _rNetworkInterfaceParam_X.IpBroadcast_S = pText_c;
          }
        }
        else
        {
          //  The default is link-local all-node multicast group fe02::1
          _rNetworkInterfaceParam_X.IpBroadcast_S = "ff02:0:0:0:0:0:0:1";
        }
      }
      _rNetworkInterfaceParam_X.IpV6_B = ((_pIpAddress_X->sa_family == AF_INET6) ? true : false);
      _rNetworkInterfaceParam_X.Info_X.MacAddress.clear();
      Rts_E = Bof_GetNetworkInterfaceInfo(_rNetworkInterfaceParam_X.Name_S, _rNetworkInterfaceParam_X.Info_X);
    }
  }
  return Rts_E;
}

#if defined(_WIN32)
BOFERR Bof_GetNetworkInterfaceInfo(const std::string _rInterfaceName_S, BOF_INTERFACE_INFO &_rInterfaceInfo_X)
{
  BOFERR Rts_E = BOF_ERR_ENOMEM;
  DWORD Sts_DW;
  ULONG Try_UL, i_U32, Flag_UL = GAA_FLAG_INCLUDE_GATEWAYS | GAA_FLAG_INCLUDE_ALL_INTERFACES | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_MULTICAST;
  PIP_ADAPTER_ADDRESSES pAdapterAddressArray_X;
  PIP_ADAPTER_ADDRESSES pCrtAdapterAddress_X;
  ULONG OutputBufferLen_UL;
  WCHAR pInterfaceName_wc[0x200];
  char pIp_c[0x200];
  LPSOCKADDR pSockeckAddress_X;
  sockaddr_in *pSaInV4_X;
  sockaddr_in6 *pSaInV6_X;

  Bof_MultiByteToWideChar(_rInterfaceName_S.c_str(), sizeof(pInterfaceName_wc) / sizeof(WCHAR), pInterfaceName_wc);
  OutputBufferLen_UL = WORKING_BUFFER_SIZE;
  Try_UL = 0;
  _rInterfaceInfo_X.MacAddress.clear();
  Sts_DW = ERROR_BUFFER_OVERFLOW;
  do
  {
    pAdapterAddressArray_X = (IP_ADAPTER_ADDRESSES *)malloc(OutputBufferLen_UL);
    if (pAdapterAddressArray_X == nullptr)
    {
      break;
    }
    Sts_DW = GetAdaptersAddresses(AF_UNSPEC, Flag_UL, nullptr, pAdapterAddressArray_X, &OutputBufferLen_UL);
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
  } while ((Sts_DW == ERROR_BUFFER_OVERFLOW) && (Try_UL < 4));

  if (Sts_DW == NO_ERROR)
  {
    Rts_E = BOF_ERR_NOT_FOUND;
    pCrtAdapterAddress_X = pAdapterAddressArray_X;
    while (pCrtAdapterAddress_X)
    {
      if (!wcscmp(pInterfaceName_wc, pCrtAdapterAddress_X->FriendlyName))
      {
        Rts_E = BOF_ERR_NO_ERROR;
        _rInterfaceInfo_X.InterfaceFlag_E = BOF_NETWORK_INTERFACE_FLAG::BOF_IFF_NONE;
        if (pCrtAdapterAddress_X->OperStatus == IfOperStatusUp)
        {
          _rInterfaceInfo_X.InterfaceFlag_E = _rInterfaceInfo_X.InterfaceFlag_E | BOF_NETWORK_INTERFACE_FLAG::BOF_IFF_UP;
        }
        _rInterfaceInfo_X.MtuSize_U32 = pCrtAdapterAddress_X->Mtu;
        if (pCrtAdapterAddress_X->PhysicalAddressLength == 0) // 127.0.0.1
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
        _rInterfaceInfo_X.IpGateway_S = "";
        if (pCrtAdapterAddress_X->FirstGatewayAddress)
        {
          pSockeckAddress_X = pCrtAdapterAddress_X->FirstGatewayAddress->Address.lpSockaddr;
          if ((pSockeckAddress_X) && (pCrtAdapterAddress_X->FirstGatewayAddress->Address.iSockaddrLength))
          {
            Rts_E = BOF_ERR_TOO_SMALL;
            if (pSockeckAddress_X->sa_family == AF_INET)
            {
              pSaInV4_X = (sockaddr_in *)pSockeckAddress_X;
              if (inet_ntop(AF_INET, &(pSaInV4_X->sin_addr), pIp_c, sizeof(pIp_c)))
              {
                _rInterfaceInfo_X.IpGateway_S = pIp_c;
                Rts_E = BOF_ERR_NO_ERROR;
              }
            }
            else if (pSockeckAddress_X->sa_family == AF_INET6)
            {
              pSaInV6_X = (sockaddr_in6 *)pSockeckAddress_X;
              if (inet_ntop(AF_INET6, &(pSaInV6_X->sin6_addr), pIp_c, sizeof(pIp_c)))
              {
                _rInterfaceInfo_X.IpGateway_S = pIp_c;
                Rts_E = BOF_ERR_NO_ERROR;
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

BOFERR Bof_SetNetworkInterfaceParam(const std::string _rInterfaceName_S, BOF_NETWORK_INTERFACE_PARAM &_rNewInterfaceParam_X)
{
  BOFERR Rts_E;
  std::string Cmd_S, Output_S;
  int32_t ExitCode_S32;
  std::string IpAddress_S, IpMask_S, IpGateway_S;

  // TODO: Implement IpV6 BofSocket support
  IpAddress_S = (_rNewInterfaceParam_X.IpAddress_S == "") ? "0.0.0.0" : _rNewInterfaceParam_X.IpAddress_S.c_str();
  IpMask_S = (_rNewInterfaceParam_X.IpMask_S == "") ? "0.0.0.0" : _rNewInterfaceParam_X.IpMask_S.c_str();
  IpGateway_S = (_rNewInterfaceParam_X.Info_X.IpGateway_S == "") ? "0.0.0.0" : _rNewInterfaceParam_X.Info_X.IpGateway_S.c_str();

  // std::cerr << "[          ] You must be admin to run this command" << std::endl;
  Cmd_S = Bof_Sprintf("netsh interface ip set address name=\"%s\" static %s %s %s 1", _rInterfaceName_S.c_str(), IpAddress_S.c_str(), IpMask_S.c_str(), IpGateway_S.c_str());
  Rts_E = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);
  if ((Rts_E == BOF_ERR_NO_ERROR) && (ExitCode_S32))
  {
    Rts_E = BOF_ERR_INTERNAL;
  }
  if ((Rts_E == BOF_ERR_NO_ERROR) && (_rInterfaceName_S != _rNewInterfaceParam_X.Name_S))
  {
    Cmd_S = Bof_Sprintf("netsh interface set interface name=\"%s\" newname = \"%s\"", _rInterfaceName_S.c_str(), _rNewInterfaceParam_X.Name_S.c_str());
    Rts_E = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);
    if ((Rts_E == BOF_ERR_NO_ERROR) && (ExitCode_S32))
    {
      Rts_E = BOF_ERR_INTERNAL;
    }
  }
  return Rts_E;
}
#else
// TODO: Remove IsFlagValid at the end
bool IsFlagValid(uint16_t _Flag_U16, bool _IpV6_B)
{
  return (_Flag_U16 & IFF_UP)                              //  Only use interfaces that are running
         && !(_Flag_U16 & IFF_LOOPBACK)                    //  Ignore loopback interface
         && ((_IpV6_B || (_Flag_U16 & IFF_BROADCAST))      //  Only use interfaces that have BROADCAST
             && (!_IpV6_B || (_Flag_U16 & IFF_MULTICAST))) //  or IPv6 and MULTICAST
#if defined(IFF_SLAVE)
         && !(_Flag_U16 & IFF_SLAVE) //  Ignore devices that are bonding slaves.
#endif
         && !(_Flag_U16 & IFF_POINTOPOINT); //  Ignore point to point interfaces.
}

#if defined(__EMSCRIPTEN__)
int readNlSock(int sockFd, char *bufPtr, size_t buf_size, int seqNum, int pId)
{
  return -1;
}
int parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo, int _Family_i)
{
  return -1;
}  
#else
int readNlSock(int sockFd, char *bufPtr, size_t buf_size, int seqNum, int pId)
{
  struct nlmsghdr *nlHdr;
  int readLen = 0, msgLen = 0;

  do
  {
    /* Recieve response from the kernel */
    if ((readLen = static_cast<int>(recv(sockFd, bufPtr, buf_size - msgLen, 0))) < 0)
    {
      // perror ( "SOCK READ: " );
      return -1;
    }

    nlHdr = (struct nlmsghdr *)bufPtr;

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
  struct rtmsg *rtMsg;
  struct rtattr *rtAttr;
  int rtLen;
  int Ret_i = 0;

  rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);

  /* If the route is not for AF_INET or does not belong to main routing table then return. */
  if ((rtMsg->rtm_family == _Family_i) && (rtMsg->rtm_table == RT_TABLE_MAIN))
  {
    /* get the rtattr field */
    rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
    rtLen = static_cast<int>(RTM_PAYLOAD(nlHdr));

    for (; RTA_OK(rtAttr, rtLen); rtAttr = RTA_NEXT(rtAttr, rtLen))
    {
      switch (rtAttr->rta_type)
      {
        case RTA_OIF:
          {
            if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
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
#endif

BOFERR Bof_GetNetworkInterfaceInfo(const std::string _rInterfaceName_S, BOF_INTERFACE_INFO &_rInterfaceInfo_X)
{
  BOFERR Rts_E = BOF_ERR_CREATE;
  struct nlmsghdr *nlMsg;
  //	struct rtmsg      *rtMsg;
  struct route_info RouteInfo_X;
  int Socket_i, Sts_i;
  int len = 0;
  int msgSeq = 0;
  char msgBuf[BUFSIZE]; // pretty large buffer
  char pIp_c[0x200];
  struct ifreq IfReq_X;
  uint32_t i_U32;

  Socket_i = socket(AF_INET, SOCK_DGRAM, 0);
  if (Socket_i >= 0)
  {
    Rts_E = BOF_ERR_NO_ERROR;
    memset(&IfReq_X, 0, sizeof(IfReq_X));
    IfReq_X.ifr_addr.sa_family = AF_INET;
    strncpy(IfReq_X.ifr_name, _rInterfaceName_S.c_str(), IFNAMSIZ - 1);
    IfReq_X.ifr_name[IFNAMSIZ - 1] = 0;

    _rInterfaceInfo_X.InterfaceFlag_E = BOF_NETWORK_INTERFACE_FLAG::BOF_IFF_NONE;
    BOF_IOCTL(Socket_i, SIOCGIFFLAGS, sizeof(IfReq_X), &IfReq_X, 0, nullptr, Sts_i)
    if (Sts_i >= 0)
    {
      if (IfReq_X.ifr_flags & IFF_UP)
      {
        _rInterfaceInfo_X.InterfaceFlag_E = _rInterfaceInfo_X.InterfaceFlag_E | BOF_NETWORK_INTERFACE_FLAG::BOF_IFF_UP;
      }
    }

    BOF_IOCTL(Socket_i, SIOCGIFMTU, sizeof(IfReq_X), &IfReq_X, 0, nullptr, Sts_i);
    if (Sts_i >= 0)
    {
      _rInterfaceInfo_X.MtuSize_U32 = IfReq_X.ifr_mtu;
    }

    BOF_IOCTL(Socket_i, SIOCGIFHWADDR, sizeof(IfReq_X), &IfReq_X, 0, nullptr, Sts_i);
    if (Sts_i >= 0)
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
#if defined(__EMSCRIPTEN__)
#else    
    if ((Socket_i = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) >= 0)
    {
      /* Initialize the buffer */
      memset(msgBuf, 0, sizeof(msgBuf));

      /* point the header and the msg structure pointers into the buffer */
      nlMsg = (struct nlmsghdr *)msgBuf;
      //			rtMsg              = (struct rtmsg *)NLMSG_DATA(nlMsg);

      /* Fill in the nlmsg header*/
      nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
      nlMsg->nlmsg_type = RTM_GETROUTE;                      // Get the routes from kernel routing table .
      nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;       // The message is a request for dump.
      nlMsg->nlmsg_seq = msgSeq++;                           // Sequence of the message packet.
      nlMsg->nlmsg_pid = getpid();                           // PID of process sending the request.

      /* Send the request */
      Rts_E = BOF_ERR_WRITE;
      if (send(Socket_i, nlMsg, nlMsg->nlmsg_len, 0) >= 0)
      {
        /* Read the response */
        Rts_E = BOF_ERR_WRITE;
        if ((len = readNlSock(Socket_i, msgBuf, sizeof(msgBuf), msgSeq, getpid())) >= 0)
        {
          /* Parse and print the response */
          for (; NLMSG_OK(nlMsg, len); nlMsg = NLMSG_NEXT(nlMsg, len))
          {
            memset(&RouteInfo_X, 0, sizeof(route_info));

            if (parseRoutes(nlMsg, &RouteInfo_X, AF_INET) < 0)
            {
              continue; // don't check route_info if it has not been set up
            }

            // Check if default gateway
            if (strstr((char *)inet_ntoa(RouteInfo_X.dstAddr), "0.0.0.0"))
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
#endif  
  }
  return Rts_E;
}

BOFERR Bof_SetNetworkInterfaceParam(const std::string _rInterfaceName_S, BOF_NETWORK_INTERFACE_PARAM &_rNewInterfaceParam_X)
{
  BOFERR Rts_E;
  std::string Cmd_S, Output_S, Name_S;
  int32_t ExitCode_S32, CidrMask_S32;
  std::string IpAddress_S, IpMask_S, IpGateway_S;

  // TODO: Implement IpV6 BofSocket support
  //  std::cerr << "[          ] You must be admin to run this command" << std::endl;
  IpAddress_S = (_rNewInterfaceParam_X.IpAddress_S == "") ? "0.0.0.0" : _rNewInterfaceParam_X.IpAddress_S.c_str();
  IpMask_S = (_rNewInterfaceParam_X.IpMask_S == "") ? "0.0.0.0" : _rNewInterfaceParam_X.IpMask_S.c_str();
  IpGateway_S = (_rNewInterfaceParam_X.Info_X.IpGateway_S == "") ? "0.0.0.0" : _rNewInterfaceParam_X.Info_X.IpGateway_S.c_str();

  // Unconditionally down
  // Cmd_S = Bof_Sprintf("ip link set %s down", _rInterfaceName_S.c_str());
  // Rts_E = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);

  // Unconditionally remove all ip
  Cmd_S = Bof_Sprintf("ip addr flush dev %s", _rInterfaceName_S.c_str());
  Rts_E = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);

  CidrMask_S32 = Bof_Compute_CidrMask(IpMask_S.c_str());
  Cmd_S = Bof_Sprintf("ip address add %s/%d dev %s", IpAddress_S.c_str(), CidrMask_S32, _rInterfaceName_S.c_str());
  Rts_E = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);
  if ((Rts_E == BOF_ERR_NO_ERROR) && (ExitCode_S32)) //|| (Output_S != "") )
  {
    Rts_E = BOF_ERR_INTERNAL;
  }

  if ((Rts_E == BOF_ERR_NO_ERROR) && (!Bof_IsIpAddressNull(IpGateway_S)))
  {
    Cmd_S = Bof_Sprintf("ip route del default via %s", IpGateway_S.c_str());
    Rts_E = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);

    Cmd_S = Bof_Sprintf("ip route add default via %s", IpGateway_S.c_str());
    Rts_E = Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);
    if ((Rts_E == BOF_ERR_NO_ERROR) && (ExitCode_S32)) //|| (Output_S != "") )
    {
      Rts_E = BOF_ERR_INTERNAL;
    }
  }

  Name_S = _rInterfaceName_S;
  if ((Rts_E == BOF_ERR_NO_ERROR) && (!ExitCode_S32)) //|| (Output_S != "") )
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
  // Cmd_S = Bof_Sprintf("ip link set %s up", Name_S.c_str());
  // Bof_Exec(Cmd_S, &Output_S, ExitCode_S32);

  return Rts_E;
}

#endif

BOFERR Bof_GetListOfNetworkInterface(std::vector<BOF_NETWORK_INTERFACE_PARAM> &_rListOfNetworkInterface_X)
{
  BOFERR Rts_E = BOF_ERR_INTERNAL;
  BOF_NETWORK_INTERFACE_PARAM NetworkInterfaceParam_X;

#if defined(_WIN32)
  // TODO: Implement IpV6 BofSocket support
  ULONG AddressSize_U32 = 0;
  DWORD rc;
  int NbIter_i = 0;
  IP_ADAPTER_ADDRESSES *pIpAddresses_X, *pCrtIpAddress_X;
  IP_ADAPTER_UNICAST_ADDRESS *pUnicastAddress_X;
  IP_ADAPTER_PREFIX *pAdapterPrefix_X;
  WCHAR *pFriendlyName_wc;
  size_t AsciiSize;
  char *pAsciiFriendlyName_c, pText_c[NI_MAXHOST];
  bool Valid_B;
  BOF_SOCKADDR_IN IpAddress_X, NetMask_X, Broadcast_X;

  rc = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, nullptr, nullptr, &AddressSize_U32);
  if (rc == ERROR_BUFFER_OVERFLOW)
  {
    do
    {
      pIpAddresses_X = (IP_ADAPTER_ADDRESSES *)malloc(AddressSize_U32);
      rc = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, nullptr, pIpAddresses_X, &AddressSize_U32);
      if (rc == ERROR_BUFFER_OVERFLOW)
      {
        BOF_SAFE_FREE(pIpAddresses_X);
      }
      else
      {
        break;
      }
      ++NbIter_i;
    } while ((rc == ERROR_BUFFER_OVERFLOW) && (NbIter_i < 3));

    if (rc == NO_ERROR)
    {
      Rts_E = BOF_ERR_NO_ERROR;
      pCrtIpAddress_X = pIpAddresses_X;
      while ((pCrtIpAddress_X) && (Rts_E == BOF_ERR_NO_ERROR))
      {
        pUnicastAddress_X = pCrtIpAddress_X->FirstUnicastAddress;
        pAdapterPrefix_X = pCrtIpAddress_X->FirstPrefix;

        pFriendlyName_wc = pCrtIpAddress_X->FriendlyName;
        AsciiSize = wcstombs(0, pFriendlyName_wc, 0) + 1;
        pAsciiFriendlyName_c = (char *)malloc(AsciiSize);
        wcstombs(pAsciiFriendlyName_c, pFriendlyName_wc, AsciiSize);

        Valid_B = (pCrtIpAddress_X->OperStatus == IfOperStatusUp) && (pUnicastAddress_X) && (pAdapterPrefix_X) && ((pUnicastAddress_X->Address.lpSockaddr->sa_family == AF_INET) || (pUnicastAddress_X->Address.lpSockaddr->sa_family == AF_INET6)) &&
                  (pAdapterPrefix_X->PrefixLength <= 32);

        if (Valid_B)
        {
          IpAddress_X = *(BOF_SOCKADDR_IN *)pUnicastAddress_X->Address.lpSockaddr;
          IpAddress_X.sin_family = pUnicastAddress_X->Address.lpSockaddr->sa_family;
          NetMask_X.sin_addr.s_addr = htonl((0xffffffffU) << (32 - pAdapterPrefix_X->PrefixLength));
          NetMask_X.sin_family = pUnicastAddress_X->Address.lpSockaddr->sa_family;
          Broadcast_X = IpAddress_X;
          if (pUnicastAddress_X->Address.lpSockaddr->sa_family == AF_INET)
          {
            Broadcast_X.sin_addr.s_addr |= ~(NetMask_X.sin_addr.s_addr);
          }
          else
          {
            Broadcast_X.sin_addr.s_addr |= ~(NetMask_X.sin_addr.s_addr);
          }

          Rts_E = Bof_GetIpMaskGw(pAsciiFriendlyName_c, (BOF_SOCKADDR *)&IpAddress_X, (BOF_SOCKADDR *)&NetMask_X, (BOF_SOCKADDR *)&Broadcast_X, NetworkInterfaceParam_X);
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            _rListOfNetworkInterface_X.push_back(NetworkInterfaceParam_X);
          }
        }
        BOF_SAFE_FREE(pAsciiFriendlyName_c);
        pCrtIpAddress_X = pCrtIpAddress_X->Next;
      }
      BOF_SAFE_FREE(pIpAddresses_X);
    }
  }
#else
  // #if defined(ANDROID)
  // #else
  struct ifaddrs *pListOfInterfaces;
  struct ifaddrs *pInterface_X;
  bool IpV6_B;
  char pHost_c[NI_MAXHOST];
  int Family_i, Sts_i;
  char pIp_c[INET6_ADDRSTRLEN];
  uint32_t MskNetIfOk_U32, MskNetIfBad_U32;
  BOFERR Sts_E;

  if (getifaddrs(&pListOfInterfaces) == 0)
  {
    pInterface_X = pListOfInterfaces;
    Rts_E = BOF_ERR_NO_ERROR;
    while ((pInterface_X) && (Rts_E == BOF_ERR_NO_ERROR))
    {
      if (pInterface_X->ifa_addr)
      {
        Family_i = pInterface_X->ifa_addr->sa_family;
        printf("--- Interface: %s Family is %d, accept only %d or %d-----------------------------\n", pInterface_X->ifa_name, Family_i, AF_INET, AF_INET6);
        if ((Family_i == AF_INET) || (Family_i == AF_INET6))
        {
          Sts_i = getnameinfo(pInterface_X->ifa_addr,
                              (Family_i == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
                              pHost_c, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

          printf("getnameinfo sts %d\n", Sts_i);
          if (Sts_i == 0)
          {
            IpV6_B = (Family_i == AF_INET6);

            printf("Address: %s\n", pHost_c);
            printf("Family: %s\n", IpV6_B ? "IPv6" : "IPv4");
            inet_ntop(Family_i, pInterface_X->ifa_addr->sa_data, pIp_c, INET6_ADDRSTRLEN);
            printf("ifa_addr: %s\n", pIp_c);

            // Display ifa_netmask
            if (pInterface_X->ifa_netmask)
            {
              inet_ntop(Family_i, pInterface_X->ifa_netmask->sa_data, pIp_c, INET6_ADDRSTRLEN);
              printf("ifa_netmask: %s\n", pIp_c);
            }

            // Display ifu_broadaddr
            if (pInterface_X->ifa_broadaddr)
            {
              inet_ntop(Family_i, pInterface_X->ifa_broadaddr->sa_data, pIp_c, INET6_ADDRSTRLEN);
              printf("ifu_broadaddr: %s\n", pIp_c);
            }
            // Display ifa_flags
            printf("ifa_flags: 0x%X ", pInterface_X->ifa_flags);

            if (pInterface_X->ifa_flags & IFF_UP)
              printf("UP ");
            if (pInterface_X->ifa_flags & IFF_BROADCAST)
              printf("BROADCAST ");
            if (pInterface_X->ifa_flags & IFF_DEBUG)
              printf("DEBUG ");
            if (pInterface_X->ifa_flags & IFF_LOOPBACK)
              printf("LOOPBACK ");
            if (pInterface_X->ifa_flags & IFF_POINTOPOINT)
              printf("POINTOPOINT ");
            if (pInterface_X->ifa_flags & IFF_RUNNING)
              printf("RUNNING ");
            if (pInterface_X->ifa_flags & IFF_NOARP)
              printf("NOARP ");
            if (pInterface_X->ifa_flags & IFF_PROMISC)
              printf("PROMISC ");
            if (pInterface_X->ifa_flags & IFF_MULTICAST)
              printf("MULTICAST ");
            printf("\n");

            // if (IsFlagValid(pInterface_X->ifa_flags, IpV6_B))
            MskNetIfOk_U32 = IFF_UP | IFF_BROADCAST | IFF_MULTICAST;
            MskNetIfBad_U32 = IFF_LOOPBACK | IFF_SLAVE | IFF_POINTOPOINT;
            printf("  ->Check flag for bad capabilities  %08X AND %08X, result is %08X and should be 00000000\n", pInterface_X->ifa_flags, MskNetIfBad_U32, pInterface_X->ifa_flags & MskNetIfBad_U32);
            if ((pInterface_X->ifa_flags & MskNetIfBad_U32) == 0)
            {
              printf("  ->Check flag for good capabilities %08X AND %08X, result is %08X and should be %08X\n", pInterface_X->ifa_flags, MskNetIfOk_U32, pInterface_X->ifa_flags & MskNetIfOk_U32, MskNetIfOk_U32);

              if ((pInterface_X->ifa_flags & MskNetIfOk_U32) == MskNetIfOk_U32)
              {
                Sts_E = Bof_GetIpMaskGw(pInterface_X->ifa_name, pInterface_X->ifa_addr, pInterface_X->ifa_netmask, pInterface_X->ifa_broadaddr, NetworkInterfaceParam_X);
                printf("  ->Final interface status: %s\n", (Rts_E == BOF_ERR_NO_ERROR) ? "validated" : "rejected");
                if (Sts_E == BOF_ERR_NO_ERROR)
                {
                  _rListOfNetworkInterface_X.push_back(NetworkInterfaceParam_X);
                }
              }
            }
          } // if getnameinfo
        }   // if ((Family_i == AF_INET) || (Family_i == AF_INET6))
      }     // if (pInterface_X->ifa_addr)
      pInterface_X = pInterface_X->ifa_next;
    } // while ((pInterface_X) && (Rts_E == BOF_ERR_NO_ERROR))
    freeifaddrs(pListOfInterfaces);
  }
#endif
  return (Rts_E);
}

int32_t Bof_Compute_CidrMask(const std::string &_rIpV4Address_S)
{
  int32_t Rts_S32 = -1;
  int i, pIpVal_i[4];
  bool Finish_B;

  if (sscanf(_rIpV4Address_S.c_str(), "%d.%d.%d.%d", &pIpVal_i[0], &pIpVal_i[1], &pIpVal_i[2], &pIpVal_i[3]) == 4)
  {
    Finish_B = false;
    Rts_S32 = 0;
    for (i = 0; ((i < 4) && (!Finish_B)); i++)
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

std::string Bof_SocketAddressToString(const BOF_SOCKET_ADDRESS &_rIpAddress_X, bool _ShowProtocol_B, bool _ShowPortNumber_B)
{
  std::string Rts_S;

  if (_ShowProtocol_B)
  {
    Rts_S = _rIpAddress_X.Protocol_S + "://";
    //Rts_S = std::string(_rIpAddress_X.pProtocol_c) + "://";
    /*
    if (_rIpAddress_X.SocketType_E == BOF_SOCK_TYPE::BOF_SOCK_TCP)
    {
      Rts_S += "tcp://";
    }
    else if (_rIpAddress_X.SocketType_E == BOF_SOCK_TYPE::BOF_SOCK_UDP)
    {
      Rts_S += "udp://";
    }
    else
    {
      Rts_S += "???://";
    }
    */
  }
  if (_rIpAddress_X.IpV6_B)
  {
    Rts_S += Bof_SockAddrInToString(_rIpAddress_X.IpV6Address_X, _ShowPortNumber_B);
  }
  else
  {
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
  char *p_c, pToString_c[0x100];

  if (inet_ntop(AF_INET6, (void *)(&_rSockAddressIn_X.sin6_addr), &pToString_c[1], sizeof(pToString_c))) // (8 * 4) + 7 + 1))
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
  BOFERR Rts_E = BOF_ERR_EINVAL;
  struct addrinfo Hint_X, *pServerInfo_X, *pInfo_X;
  BOF_SOCKET_ADDRESS Ip_X;
  std::string IpAddress_S, TheIpAddress_S;
  uint16_t Port_U16;

  _rListOfIpAddress_X.clear();
  IpAddress_S = Bof_StringTrim(_rIpOrUrlAddress_S);
  if (IpAddress_S != "")
  {
    // Bof_IpAddressToSocketAddress call Bof_UrlAddressToSocketAddressCollection if needed and return array index 0
    /*
        Rts_E = Bof_IpAddressToSocketAddress(IpAddress_S, Ip_X, &TheIpAddress_S);
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          _rListOfIpAddress_X.push_back(Ip_X);
        }
        else
    */
    {
      Port_U16 = Ip_X.Port();
      if (Ip_X.IpV6_B)
      {
        memset(&Hint_X, 0, sizeof Hint_X);
        Hint_X.ai_family = AF_INET6; // AF_UNSPEC; // use AF_INET6 to force IPv6
        // Hint_X.ai_socktype = SOCK_STREAM;
        // Hint_X.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
        // Hint_X.ai_protocol = IPPROTO_TCP;		//0;          /* Any protocol */

        //        Rts_E = (getaddrinfo(IpV6Ipaddress_S.c_str(), nullptr, &Hint_X, &pServerInfo_X) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
        Rts_E = (getaddrinfo(IpAddress_S.c_str() /*TheIpAddress_S.c_str()*/, nullptr, &Hint_X, &pServerInfo_X) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          Rts_E = BOF_ERR_NOT_FOUND;
          for (pInfo_X = pServerInfo_X; pInfo_X != nullptr; pInfo_X = pInfo_X->ai_next)
          {
            Ip_X.IpV6Address_X = *reinterpret_cast<BOF_SOCKADDR_IN6 *>(pInfo_X->ai_addr);
            Ip_X.IpV6Address_X.sin6_port = htons(Port_U16);
            _rListOfIpAddress_X.push_back(Ip_X);
          }
          Rts_E = BOF_ERR_NO_ERROR;

          freeaddrinfo(pServerInfo_X); // all done with this structure
        }
      }
      else
      {
        memset(&Hint_X, 0, sizeof Hint_X);
        Hint_X.ai_family = AF_INET; // AF_UNSPEC; // use AF_INET6 to force IPv6
        // Hint_X.ai_socktype = SOCK_STREAM;
        // Hint_X.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
        // Hint_X.ai_protocol = IPPROTO_TCP;		//0;          /* Any protocol */
        // Hint_X.ai_flags=AI_NUMERICHOST;

        Rts_E = (getaddrinfo(IpAddress_S.c_str() /*TheIpAddress_S.c_str()*/, nullptr, &Hint_X, &pServerInfo_X) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL;
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          Rts_E = BOF_ERR_NOT_FOUND;
          for (pInfo_X = pServerInfo_X; pInfo_X != nullptr; pInfo_X = pInfo_X->ai_next)
          {
            Ip_X.IpV4Address_X = *reinterpret_cast<BOF_SOCKADDR_IN *>(pInfo_X->ai_addr);
            Ip_X.IpV4Address_X.sin_port = htons(Port_U16);
            _rListOfIpAddress_X.push_back(Ip_X);
          }
          Rts_E = BOF_ERR_NO_ERROR;

          freeaddrinfo(pServerInfo_X); // all done with this structure
        }
      }
    }
  }
  return Rts_E;
}
BOFERR Bof_IpAddressToSocketAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS &_rIpAddress_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  std::string::size_type PosColon, PosStartBracket, PosEndBracket, PosProtocol;
  std::string IpAddress_S;
  uint16_t Port_U16;
  BOF_SOCKET_ADDRESS Ip_X;
  int Port_i;
  std::vector<BOF_SOCKET_ADDRESS> ListOfIpAddress_X;
  //uint32_t ProtoNameSize_U32;

  IpAddress_S = Bof_StringTrim(_rIpAddress_S);
  if (IpAddress_S != "")
  {
    Rts_E = BOF_ERR_NO_ERROR;
    Port_U16 = 0;
    PosColon = IpAddress_S.rfind(':');
    PosStartBracket = IpAddress_S.find('[');
    PosEndBracket = IpAddress_S.rfind(']');

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
            Port_U16 = static_cast<uint16_t>(Port_i);
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
      Ip_X.IpV6_B = false;
      Ip_X.SocketType_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;

      if (IpAddress_S.back() == ']')
      {
        if ((PosStartBracket != std::string::npos) && (PosEndBracket != std::string::npos) && (PosStartBracket + 2 < PosEndBracket)) // At least [::]
        {
          Ip_X.IpV6_B = true;
          IpAddress_S = IpAddress_S.substr(PosStartBracket + 1, PosEndBracket - PosStartBracket - 1);
        }
      }

      PosProtocol = IpAddress_S.find("://");
      if (PosProtocol != std::string::npos)
      {
        if (!IpAddress_S.compare(0, 6, "tcp://"))
        {
          Ip_X.SocketType_E = BOF_SOCK_TYPE::BOF_SOCK_TCP;
          Ip_X.Protocol_S = "tcp";
//          strcpy(Ip_X.pProtocol_c, "tcp");
          IpAddress_S = IpAddress_S.substr(6);
        }
        else if (!IpAddress_S.compare(0, 6, "udp://"))
        {
          Ip_X.SocketType_E = BOF_SOCK_TYPE::BOF_SOCK_UDP;
          Ip_X.Protocol_S = "udp";
//          strcpy(Ip_X.pProtocol_c, "udp");
          IpAddress_S = IpAddress_S.substr(6);
        }
        else
        {
          Ip_X.SocketType_E = BOF_SOCK_TYPE::BOF_SOCK_TCP;
          Ip_X.Protocol_S = IpAddress_S.substr(0, PosProtocol);
          //ProtoNameSize_U32 = ((PosProtocol - 1) < sizeof(Ip_X.pProtocol_c)) ? (PosProtocol - 1) : (sizeof(Ip_X.pProtocol_c)-1);
          //memcpy(Ip_X.pProtocol_c, IpAddress_S.c_str(), ProtoNameSize_U32);
          //Ip_X.pProtocol_c[ProtoNameSize_U32] = 0;
          IpAddress_S = IpAddress_S.substr(PosProtocol+3);
        }
      }
      if (Ip_X.IpV6_B)
      {
        Ip_X.IpV6Address_X.sin6_port = htons(Port_U16);
      }
      else
      {
        Ip_X.IpV4Address_X.sin_port = htons(Port_U16);
      }
      _rIpAddress_X = Ip_X; // Return partial result to caller

      if (Ip_X.IpV6_B)
      {
        if (inet_pton(AF_INET6, IpAddress_S.c_str(), &Ip_X.IpV6Address_X.sin6_addr.s6_addr) == 1)
        {
          Ip_X.IpV6Address_X.sin6_family = AF_INET6;
          _rIpAddress_X = Ip_X;
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
  bool Rts_B;
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
    Rts_B = (i_U32 == 16);
  }
  else
  {
    Rts_B = (_rIpAddress_X.IpV4Address_X.sin_addr.s_addr == 0); // Port is not used for null
  }
  return Rts_B;
}

bool Bof_IsIpAddressNull(const std::string &_rIpAddress_S)
{
  bool Rts_B = true;
  BOF_SOCKET_ADDRESS IpAddress_X;

  if (Bof_IpAddressToSocketAddress(_rIpAddress_S, IpAddress_X) == BOF_ERR_NO_ERROR)
  {
    Rts_B = Bof_IsIpAddressNull(IpAddress_X);
  }
  return Rts_B;
}

bool Bof_IsIpAddressLocalHost(const BOF_SOCKET_ADDRESS &_rIpAddress_X)
{
  bool Rts_B = false;
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
  bool Rts_B = true;
  BOF_SOCKET_ADDRESS IpAddress_X;

  if (Bof_IpAddressToSocketAddress(_rIpAddress_S, IpAddress_X) == BOF_ERR_NO_ERROR)
  {
    Rts_B = Bof_IsIpAddressLocalHost(IpAddress_X);
  }
  return Rts_B;
}

// std::vector<uint16_t> Bof_IpAddressToBin(const std::string &_rIpAddress_S, bool &_rIsIpV6_B)
BOFERR Bof_IpAddressToBin(const std::string &_rIpAddress_S, bool &_rIsIpV6_B, std::vector<uint16_t> &_rIpBinDigitCollection)
{
  BOFERR Rts_E;
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
  bool Rts_B = false;
  BOFERR Sts_E;
  uint32_t NbElem_U32;
  std::vector<uint16_t> IpDigitCollection;
  bool IsIpV6_B;

  Sts_E = Bof_SplitIpAddress(_rIpAddress_S, _rInterfaceIpAddressComponent_X, _rIpAddressComponent_X);
  if (Sts_E == BOF_ERR_NO_ERROR)
  {
    Bof_IpAddressToBin(_rIpAddressComponent_X.IpAddress_S, IsIpV6_B, IpDigitCollection);
    NbElem_U32 = IsIpV6_B ? 8 : 4;
    if (IpDigitCollection.size() == NbElem_U32)
    {
      Rts_B = IsIpV6_B ? ((IpDigitCollection[0] & 0xFF00) == 0xFF00) : ((IpDigitCollection[0] & 0xF0) == 0xE0);
    }
  }
  return Rts_B;
}

/*
   uri:
   https://snipplr.com/view/6889/regular-expressions-for-uri-validationparsing
   myprotocol:/forum/questions/file.txt?justkey;order=newest&tag=networking#HashTag
   myprotocol://www.google.com:123/forum/questions/file.txt?justkey&order=newest;tag=networking#top
   myprotocol://john.doe:password@www.google.com:123/forum/questions/file.txt?justkey&order=newest;tag=networking#top
*/
#if 0
//replace() can be used to parse the URI. For example, to get the path:
//  path = uri.replace(regexUri, "$7$9");

//Validate a URI
//- The different parts are kept in their own groups and can be recombined
//  depending on the scheme:
//  - http as $1://$2$7?$11#$12 or $1://$5:$6$7?$11#$12
//  - ftp as $1://$2$7 or $1://$4@$5:$6$7
//  - mailto as $1:$9?$11
//- groups are as follows:
//  1   == scheme
//  2   == authority
//  4   == userinfo
//  5   == host (loose check to allow for IPv6 addresses)
//  6   == port
//  7,9 == path (7 if it has an authority, 9 if it doesn't)
//  11   == query
//  12   == fragment

var regexUri = / ^ ([a - z][a - z0 - 9 + . - ] *) : (? : \ / \ / ((? : (? = ((? : [a - z0 - 9 - ._~!$ & '()*+,;=:]|%[0-9A-F]{2})*))(\3)@)?(?=(\[[0-9A-F:.]{2,}\]|(?:[a-z0-9-._~!$&'() * +, ; =] | % [0 - 9A - F]{ 2 }) *))\5(? ::(? = (\d *))\6) ? )(\ / (? = ((? : [a - z0 - 9 - ._~!$ & '()*+,;=:@\/]|%[0-9A-F]{2})*))\8)?|(\/?(?!\/)(?=((?:[a-z0-9-._~!$&'() * +, ; = :@\ / ] | % [0 - 9A - F]{ 2 }) *))\10) ? )(? : \ ? (? = ((? : [a - z0 - 9 - ._~!$ & '()*+,;=:@\/?]|%[0-9A-F]{2})*))\11)?(?:#(?=((?:[a-z0-9-._~!$&'() * +, ; = :@\ / ? ] | % [0 - 9A - F]{ 2 }) *))\12) ? $ / i;
/*composed as follows:
  ^
  ([a-z][a-z0-9+.-]*):									#1 scheme
  (?:
    \/\/										it has an authority:

    (										#2 authority
      (?:(?=((?:[a-z0-9-._~!$&'()*+,;=:]|%[0-9A-F]{2})*))(\3)@)?		#4 userinfo
      (?=(\[[0-9A-F:.]{2,}\]|(?:[a-z0-9-._~!$&'()*+,;=]|%[0-9A-F]{2})*))\5	#5 host (loose check to allow for IPv6 addresses)
      (?::(?=(\d*))\6)?							#6 port
    )

    (\/(?=((?:[a-z0-9-._~!$&'()*+,;=:@\/]|%[0-9A-F]{2})*))\8)?			#7 path

    |										it doesn't have an authority:

    (\/?(?!\/)(?=((?:[a-z0-9-._~!$&'()*+,;=:@\/]|%[0-9A-F]{2})*))\10)?		#9 path
  )
  (?:
    \?(?=((?:[a-z0-9-._~!$&'()*+,;=:@\/?]|%[0-9A-F]{2})*))\11			#11 query string
  )?
  (?:
    #(?=((?:[a-z0-9-._~!$&'()*+,;=:@\/?]|%[0-9A-F]{2})*))\12			#12 fragment
  )?
  $
*/

//** Validate a URI (includes delimiters in groups) **//
//- The different parts--along with their delimiters--are kept in their own
//  groups and can be recombined as $1$6$2$3$4$5$7$8$9
//- groups are as follows:
//  1,6 == scheme:// or scheme:
//  2   == userinfo@
//  3   == host
//  4   == :port
//  5,7 == path (5 if it has an authority, 7 if it doesn't)
//  8   == ?query
//  9   == #fragment

var regexUriDelim = / ^ (? : ([a - z0 - 9 + . - ] + :\ / \ / )((? : (? : [a - z0 - 9 - ._~!$ & '()*+,;=:]|%[0-9A-F]{2})*)@)?((?:[a-z0-9-._~!$&'() * +, ; =] | % [0 - 9A - F]{ 2 }) *)(:(? : \d *)) ? (\ / (? : [a - z0 - 9 - ._~!$ & '()*+,;=:@\/]|%[0-9A-F]{2})*)?|([a-z0-9+.-]+:)(\/?(?:[a-z0-9-._~!$&'() * +, ; = :@] | % [0 - 9A - F]{ 2 }) + (? : [a - z0 - 9 - ._~!$ & '()*+,;=:@\/]|%[0-9A-F]{2})*)?)(\?(?:[a-z0-9-._~!$&'() * +, ; = :\ / ? @] | % [0 - 9A - F]{ 2 }) *) ? (#(? : [a - z0 - 9 - ._~!$ & () * +, ; = :\ / ? @] | % [0 - 9A - F]{ 2 }) *) ? $ / i;

//Validate a URL
//Validates a URI with an http or https scheme.
//- The different parts are kept in their own groups and can be recombined as
//  $1://$2:$3$4?$5#$6
//- Does not validate the host portion (domain); just makes sure the string
//  consists of valid characters (does not include IPv6 nor IPvFuture
//  addresses as valid).

var regexUrl = / ^ (https ? ) : \ / \ / ((? : [a - z0 - 9. - ] | % [0 - 9A - F]{ 2 }) { 3, })(? ::(\d + )) ? ((? : \ / (? : [a - z0 - 9 - ._~!$ & () * +, ; = :@] | % [0 - 9A - F]{ 2 }) *) *)(? : \ ? ((? : [a - z0 - 9 - ._~!$ & '() * +, ; = :\ / ? @] | % [0 - 9A - F]{ 2 }) *)) ? (? : #((? : [a - z0 - 9 - ._~!$ & '() * +, ; = :\ / ? @] | % [0 - 9A - F]{ 2 }) *)) ? $ / i;

//Validate a Mailto
//Validates a URI with a mailto scheme.
//- The different parts are kept in their own groups and can be recombined as
//  $1:$2?$3
//- Does not validate the email addresses themselves.

var regexMailto = / ^ (mailto) : ((? : [a - z0 - 9 - ._~!$ & '()*+,;=:@]|%[0-9A-F]{2})+)?(?:\?((?:[a-z0-9-._~!$&'() * +, ; = :\ / ? @] | % [0 - 9A - F]{ 2 }) *)) ? $ / i;

#endif
// TODO: Implement IpV6 BofSocket support : is sep for port and :: is for ipv6

BOFERR Bof_SplitUri(const std::string &_rUri_S, BOF_SOCKET_ADDRESS_COMPONENT &_rUri_X, std::string &_rPath_S, std::string &_rQuery_S, std::string &_rFragment_S)
{
  BOFERR Rts_E = BOF_ERR_FORMAT;
  static const std::regex S_RegExUri(
      "^([a-z][a-z0-9+.-]*):(?:\\/\\/((?:(?=((?:[a-z0-9-._~!$&'()*+,;=:]|%[0-9A-F]{2})*))(\\3)@)?(?=(\\[[0-9A-F:.]{2,}\\]|(?:[a-z0-9-._~!$&'()*+,;=]|%[0-9A-F]{2})*))\\5(?::(?=(\\d*))\\6)?)(\\/(?=((?:[a-z0-9-._~!$&'()*+,;=:@\\/]|%[0-9A-F]{2})*))\\8)?|(\\/"
      "?(?!\\/)(?=((?:[a-z0-9-._~!$&'()*+,;=:@\\/]|%[0-9A-F]{2})*))\\10)?)(?:\\?(?=((?:[a-z0-9-._~!$&'()*+,;=:@\\/?]|%[0-9A-F]{2})*))\\11)?(?:#(?=((?:[a-z0-9-._~!$&'()*+,;=:@\\/?]|%[0-9A-F]{2})*))\\12)?$"); // Static as it can takes time (on gcc 4.9 for
                                                                                                                                                                                                               // example)
  std::smatch MatchString;
  std::string::size_type PosColumn;
  std::vector<BOF_SOCKET_ADDRESS> ListOfIpAddress_X;

  _rUri_X.Reset();
  if (std::regex_search(_rUri_S, MatchString, S_RegExUri))
  {
    /*
    printf("%ld result:\n", MatchString.size());
    for (auto i = 0; i < MatchString.size(); i++)
    {
      printf("[%02d]='%s'\n", i, MatchString[i].str().c_str());
    }
    */
    if (MatchString.size() == 13)
    {
      Rts_E = BOF_ERR_NO_ERROR;
      _rUri_X.Protocol_S = MatchString[1].str();
      PosColumn = MatchString[4].str().find(':');
      if (PosColumn == std::string::npos)
      {
        _rUri_X.User_S = MatchString[4].str();
        _rUri_X.Password_S = "";
      }
      else
      {
        _rUri_X.User_S = MatchString[4].str().substr(0, PosColumn);
        _rUri_X.Password_S = MatchString[4].str().substr(PosColumn + 1);
        ;
      }
      // printf("[5]='%s' [6]='%s' -> %d\n", MatchString[5].str().c_str(), MatchString[6].str().c_str(), std::stoi(MatchString[6].str()));
      _rUri_X.IpAddress_S = MatchString[5].str();
      _rPath_S = (MatchString[2].str() == "") ? MatchString[9].str() : MatchString[7].str();
      _rQuery_S = MatchString[11].str();
      _rFragment_S = MatchString[12].str();

      try
      {
        if ((_rUri_X.IpAddress_S != "") && (MatchString[6].str() != ""))
        {
          _rUri_X.Port_U16 = static_cast<uint16_t>(std::stoi(MatchString[6].str()));
          Rts_E = Bof_IpAddressToSocketAddress(_rUri_X.IpAddress_S, _rUri_X.Ip_X);
          if (Rts_E != BOF_ERR_NO_ERROR)
          {
            Rts_E = Bof_UrlAddressToSocketAddressCollection(_rUri_X.IpAddress_S, ListOfIpAddress_X);
            if (Rts_E == BOF_ERR_NO_ERROR)
            {
              _rUri_X.Ip_X = ListOfIpAddress_X[0];
            }
          }
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            _rUri_X.Ip_X.Port(_rUri_X.Port_U16);
          }
        }
      }
      catch (const std::exception &)
      {
        Rts_E = BOF_ERR_EINVAL;
      }
    }
  }

  return Rts_E;
}
//GET /forum/questions/?tag=networking&order=newest#top
BOFERR Bof_SplitHttpRequest(const std::string &_rUri_S, std::string &_rMethod_S, std::string &_rPath_S, std::string &_rQuery_S, std::string &_rFragment_S)
{
  BOFERR Rts_E = BOF_ERR_FORMAT;
  static const std::regex S_RegExHttpRequest("([A-Z]*)\\s+(/[^?#]+)?(\\?[^#]*)?(#.*)?");
  std::smatch MatchString;
  std::string::size_type PosColumn;

  if (std::regex_search(_rUri_S, MatchString, S_RegExHttpRequest))
  {
    /*
    printf("%ld result:\n", MatchString.size());
    for (auto i = 0; i < MatchString.size(); i++)
    {
      printf("[%02d]='%s'\n", i, MatchString[i].str().c_str());
    }
    */
    if (MatchString.size() == 5)
    {
      _rMethod_S = MatchString[1].str();
      _rPath_S = MatchString[2].str();
      _rQuery_S = MatchString[3].str();
      if (_rQuery_S != "")
      {
        _rQuery_S = _rQuery_S.substr(1);//Remove ?
      }
      _rFragment_S = MatchString[4].str();
      if (_rFragment_S != "")
      {
        _rFragment_S = _rFragment_S.substr(1);//Remove #
      }
      Rts_E = BOF_ERR_NO_ERROR;
    }
  }
  return Rts_E;
}

/*
   zmq style address: REPLACE ; by BOF_INTERFACE_ADDRESS_SEPARATOR

   tcp://127.0.0.1:*														bind to first free port from C000 up
   tcp://127.0.0.1:!													bind to random port from C000 to FFFF
   tcp://127.0.0.1:*[60000-]									bind to first free port from 60000 up
   tcp://127.0.0.1:![-60000]									bind to random port from C000 to 60000
   tcp://127.0.0.1:![55000-55999]							bind to random port from 55000 to 55999
   pgm://192.168.1.1>239.192.1.1:5555"					multicast address from local interface 192.168.1.1 to group 239.192.1.1 on port 5555
   pgm://192.168.1.1:1234>239.192.1.1:5555"		multicast address from local interface 192.168.1.1 port 1234 to group 239.192.1.1 on port 5555
   myprotocol://john.doe:password@www.google.com:123
 */
// TODO: Implement IpV6 BofSocket support : is sep for port and :: is for ipv6

BOFERR ParseIpAddress(const std::string &_rIpAddress_S, const std::string &_rProtocol_S, BOF_SOCKET_ADDRESS_COMPONENT &_rSockAddressComponent_X)
{
  BOFERR Rts_E = BOF_ERR_TOO_SMALL;
  std::string::size_type PosEop, PosAfter, PosSeparator, Pos, PosColonPass, PosColonPort, PosAt, PosEndBracket;
  std::vector<BOF_SOCKET_ADDRESS> ListOfIpAddress_X;

  if (_rIpAddress_S != "")
  {
    Rts_E = BOF_ERR_NO_ERROR;
    PosAt = _rIpAddress_S.find('@');
    if (PosAt != std::string::npos)
    {
      Pos = _rIpAddress_S.find(':');
      if (Pos != std::string::npos)
      {
        if (Pos < PosAt)
        {
          PosColonPass = Pos;
          PosColonPort = _rIpAddress_S.find(':', Pos + 1);
        }
        else
        {
          PosColonPass = std::string::npos;
          PosColonPort = Pos;
        }
      }
      else
      {
        PosColonPass = std::string::npos;
        PosEndBracket = _rIpAddress_S.rfind(']'); // IPV6
        PosColonPort = _rIpAddress_S.rfind(':');
        if ((PosColonPort != std::string::npos) && (PosEndBracket != std::string::npos) && (PosColonPort < PosEndBracket))
        {
          PosColonPort = std::string::npos;
        }
      }
      if (PosColonPass != std::string::npos)
      {
        _rSockAddressComponent_X.User_S = _rIpAddress_S.substr(0, PosColonPass);
        _rSockAddressComponent_X.Password_S = _rIpAddress_S.substr(PosColonPass + 1, PosAt - PosColonPass - 1);
      }
      else
      {
        _rSockAddressComponent_X.User_S = _rIpAddress_S.substr(0, PosAt);
        _rSockAddressComponent_X.Password_S = "";
      }
      if (PosColonPort != std::string::npos)
      {
        _rSockAddressComponent_X.IpAddress_S = _rIpAddress_S.substr(PosAt + 1, PosColonPort - PosAt - 1);
      }
      else
      {
        _rSockAddressComponent_X.IpAddress_S = _rIpAddress_S.substr(PosAt + 1);
      }
    }
    else
    {
      _rSockAddressComponent_X.User_S = "";
      _rSockAddressComponent_X.Password_S = "";

      PosEndBracket = _rIpAddress_S.rfind(']'); // IPV6
      PosColonPort = _rIpAddress_S.rfind(':');
      if ((PosColonPort != std::string::npos) && ((PosColonPort > PosEndBracket) || (PosEndBracket == std::string::npos)))
      {
        _rSockAddressComponent_X.IpAddress_S = _rIpAddress_S.substr(0, PosColonPort);
      }
      else
      {
        _rSockAddressComponent_X.IpAddress_S = _rIpAddress_S;
      }
    }

    _rSockAddressComponent_X.Protocol_S = _rProtocol_S;
    Rts_E = Bof_IpAddressToSocketAddress(_rSockAddressComponent_X.IpAddress_S, _rSockAddressComponent_X.Ip_X);
    if (Rts_E != BOF_ERR_NO_ERROR)
    {
      Rts_E = Bof_UrlAddressToSocketAddressCollection(_rSockAddressComponent_X.IpAddress_S, ListOfIpAddress_X);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        _rSockAddressComponent_X.Ip_X = ListOfIpAddress_X[0];
      }
    }
    if (PosColonPort != std::string::npos)
    {
      try
      {
        _rSockAddressComponent_X.Port_U16 = static_cast<uint16_t>(std::stoi(_rIpAddress_S.substr(PosColonPort + 1)));
        _rSockAddressComponent_X.Ip_X.Port(_rSockAddressComponent_X.Port_U16);
      }
      catch (const std::exception &)
      {
        Rts_E = BOF_ERR_EINVAL;
      }
    }
    else
    {
      _rSockAddressComponent_X.Port_U16 = 0;
    }
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (_rProtocol_S == "tcp")
      {
        _rSockAddressComponent_X.Ip_X.SocketType_E = BOF_SOCK_TYPE::BOF_SOCK_TCP;
      }
      else if (_rProtocol_S == "udp")
      {
        _rSockAddressComponent_X.Ip_X.SocketType_E = BOF_SOCK_TYPE::BOF_SOCK_UDP;
      }
      else
      {
        _rSockAddressComponent_X.Ip_X.SocketType_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
      }
    }
  }
  return Rts_E;
}

BOF_SOCK_TYPE Bof_SocketType(const std::string &_rIpAddress_S)
{
  BOF_SOCK_TYPE Rts_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
  std::string Protocol_S;
  std::string::size_type PosEop;

  PosEop = _rIpAddress_S.find("://");
  if (PosEop != std::string::npos)
  {
    Protocol_S = _rIpAddress_S.substr(0, PosEop);
    if (Protocol_S == "tcp")
    {
      Rts_E = BOF_SOCK_TYPE::BOF_SOCK_TCP;
    }
    else if (Protocol_S == "udp")
    {
      Rts_E = BOF_SOCK_TYPE::BOF_SOCK_UDP;
    }
    else
    {
      Rts_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
    }
  }
  return Rts_E;
}

BOFERR Bof_SplitIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS_COMPONENT &_rInterfaceIpAddress_X, BOF_SOCKET_ADDRESS_COMPONENT &_rIpAddress_X)
{
  BOFERR Rts_E = BOF_ERR_TOO_SMALL;
  std::string Protocol_S, Address_S, Interface_S, TheAddress_S;
  std::string::size_type PosEop, PosAfter, PosSeparator, PosColon, PosAt;
  //uint32_t ProtoNameSize_U32;

  _rInterfaceIpAddress_X.Reset();
  _rIpAddress_X.Reset();
  TheAddress_S = _rIpAddress_S;
  if (TheAddress_S.size() > 6)
  {
    Rts_E = BOF_ERR_FORMAT;
    PosEop = TheAddress_S.find("://");
    if (PosEop == std::string::npos)
    {
      TheAddress_S = "???://" + TheAddress_S;
      PosEop = TheAddress_S.find("://");
    }
    if (PosEop != std::string::npos)
    {
      Protocol_S = TheAddress_S.substr(0, PosEop);
      PosAfter = PosEop + 3;

      PosSeparator = TheAddress_S.find(BOF_INTERFACE_ADDRESS_SEPARATOR, PosAfter);
      if (PosSeparator != std::string::npos)
      {
        Interface_S = TheAddress_S.substr(PosAfter, PosSeparator - PosAfter);
        Address_S = TheAddress_S.substr(PosSeparator + 1);
      }
      else
      {
        Interface_S = "";
        Address_S = TheAddress_S.substr(PosAfter);
      }
      Rts_E = BOF_ERR_NO_ERROR;
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
        Rts_E = ParseIpAddress(Interface_S, Protocol_S, _rInterfaceIpAddress_X);
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          _rInterfaceIpAddress_X.Protocol_S = Protocol_S;
          _rInterfaceIpAddress_X.Ip_X.Protocol_S = Protocol_S;
          //ProtoNameSize_U32 = (Protocol_S.size() < sizeof(_rInterfaceIpAddress_X.Ip_X.pProtocol_c)) ? Protocol_S.size() : (sizeof(_rInterfaceIpAddress_X.Ip_X.pProtocol_c)-1);
          //memcpy(_rInterfaceIpAddress_X.Ip_X.pProtocol_c, Protocol_S.c_str(), ProtoNameSize_U32);
          //_rInterfaceIpAddress_X.Ip_X.pProtocol_c[ProtoNameSize_U32] = 0;
        }
      }
    }

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (Address_S != "")
      {
        Rts_E = ParseIpAddress(Address_S, Protocol_S, _rIpAddress_X);
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          _rIpAddress_X.Protocol_S = Protocol_S;
          _rIpAddress_X.Ip_X.Protocol_S = Protocol_S;
          //ProtoNameSize_U32 = (Protocol_S.size() < sizeof(_rIpAddress_X.Ip_X.pProtocol_c)) ? Protocol_S.size() : (sizeof(_rIpAddress_X.Ip_X.pProtocol_c)-1);
          //memcpy(_rIpAddress_X.Ip_X.pProtocol_c, Protocol_S.c_str(), ProtoNameSize_U32);
          //_rIpAddress_X.Ip_X.pProtocol_c[ProtoNameSize_U32] = 0;
        }
      }
    }
  }
  /*
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    if ((_rIpAddress_X.Protocol_S == "") || (_rIpAddress_X.IpAddress_S == ""))
    {
      printf("jj");
    }
    else
    {
      printf("A %s P %s I %s\n", _rIpAddress_S.c_str(), _rIpAddress_X.Protocol_S.c_str(), _rIpAddress_X.IpAddress_S.c_str());
    }
  }
  */
  return Rts_E;
}

BOFERR Bof_SplitIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS_COMPONENT &_rIpAddress_X)
{
  BOF_SOCKET_ADDRESS_COMPONENT InterfaceIpAddress_X;

  return Bof_SplitIpAddress(_rIpAddress_S, InterfaceIpAddress_X, _rIpAddress_X);
}

BOFERR Bof_SplitIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS &_rInterfaceIpAddress_X, BOF_SOCKET_ADDRESS &_rIpAddress_X)
{
  BOFERR Rts_E;
  BOF_SOCKET_ADDRESS_COMPONENT InterfaceIpAddressComponent_X, IpAddressComponent_X;
  bool InterfaceIsIpV6_B, IsIpV6_B;
  BOF_SOCK_TYPE InterfaceSockType_E, SockType_E;
  std::vector<uint16_t> IpDigitCollection;
  uint32_t i_U32, Index_U32, pIp_U32[4];

  Rts_E = Bof_SplitIpAddress(_rIpAddress_S, InterfaceIpAddressComponent_X, IpAddressComponent_X);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    if (InterfaceIpAddressComponent_X.IpAddress_S != "")
    {
      InterfaceSockType_E = BOF_SOCK_TYPE::BOF_SOCK_TCP;
      Rts_E = Bof_IpAddressToBin(InterfaceIpAddressComponent_X.IpAddress_S, InterfaceIsIpV6_B, IpDigitCollection);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        Rts_E = BOF_ERR_FORMAT;
        if (IpDigitCollection.size() >= 4)
        {
          if (InterfaceIsIpV6_B)
          {
            Index_U32 = 0;
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
      InterfaceIsIpV6_B = false;
      InterfaceSockType_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
      pIp_U32[0] = 0;
      pIp_U32[1] = 0;
      pIp_U32[2] = 0;
      pIp_U32[3] = 0;
      InterfaceIpAddressComponent_X.Port_U16 = 0;
    }
    _rInterfaceIpAddress_X.Set(InterfaceIsIpV6_B, InterfaceSockType_E, pIp_U32[0], pIp_U32[1], pIp_U32[2], pIp_U32[3], InterfaceIpAddressComponent_X.Port_U16);

    if (IpAddressComponent_X.IpAddress_S != "")
    {
      IpDigitCollection.clear();
      SockType_E = BOF_SOCK_TYPE::BOF_SOCK_TCP;
      Rts_E = Bof_IpAddressToBin(IpAddressComponent_X.IpAddress_S, IsIpV6_B, IpDigitCollection);
      if (IpDigitCollection.size() >= 4)
      {
        if (IsIpV6_B)
        {
          Index_U32 = 0;
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
        InterfaceIsIpV6_B = false;
        InterfaceSockType_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
        pIp_U32[0] = 0;
        pIp_U32[1] = 0;
        pIp_U32[2] = 0;
        pIp_U32[3] = 0;
        InterfaceIpAddressComponent_X.Port_U16 = 0;
      }
      _rIpAddress_X.Set(IsIpV6_B, SockType_E, pIp_U32[0], pIp_U32[1], pIp_U32[2], pIp_U32[3], IpAddressComponent_X.Port_U16);
    }
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

BOFERR Bof_SplitIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS &_rIpAddress_X)
{
  BOF_SOCKET_ADDRESS InterfaceIpAddress_X;

  return Bof_SplitIpAddress(_rIpAddress_S, InterfaceIpAddress_X, _rIpAddress_X);
}

BOFERR Bof_ResolveIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS &_rInterfaceIpAddress_X, BOF_SOCKET_ADDRESS &_rIpAddress_X)
{
  BOFERR Rts_E = BOF_ERR_TOO_SMALL;
  std::string Protocol_S, Address_S, Interface_S;
  std::string::size_type PosEop, PosAfter, PosSeparator;
  BOF_SOCK_TYPE Type_E;

  if (_rIpAddress_S.size() > 6)
  {
    Type_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
    _rInterfaceIpAddress_X.IpV6_B = false;
    _rInterfaceIpAddress_X.SocketType_E = Type_E;
    _rInterfaceIpAddress_X.IpV4Address_X.sin_addr.s_addr = 0;
    _rInterfaceIpAddress_X.IpV4Address_X.sin_port = 0;
    _rInterfaceIpAddress_X.IpV4Address_X.sin_family = 0;
    _rIpAddress_X.IpV6_B = false;
    _rIpAddress_X.SocketType_E = Type_E;
    _rIpAddress_X.IpV4Address_X.sin_addr.s_addr = 0;
    _rIpAddress_X.IpV4Address_X.sin_port = 0;
    _rIpAddress_X.IpV4Address_X.sin_family = 0;
    Rts_E = BOF_ERR_FORMAT;
    PosEop = _rIpAddress_S.find("://");
    Protocol_S = "";
    if (PosEop != std::string::npos)
    {
      Protocol_S = _rIpAddress_S.substr(0, PosEop);
      _rIpAddress_X.Protocol_S = Protocol_S;
      if (Protocol_S == "tcp")
      {
        Type_E = BOF_SOCK_TYPE::BOF_SOCK_TCP;
      }
      else if (Protocol_S == "udp")
      {
        Type_E = BOF_SOCK_TYPE::BOF_SOCK_UDP;
      }
      else
      {
        Type_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
      }
      PosAfter = PosEop + 3;

      PosSeparator = _rIpAddress_S.find(BOF_INTERFACE_ADDRESS_SEPARATOR, PosAfter);
      if (PosSeparator != std::string::npos)
      {
        Interface_S = _rIpAddress_S.substr(PosAfter, PosSeparator - PosAfter);
        Address_S = _rIpAddress_S.substr(PosSeparator + 1);
      }
      else
      {
        Interface_S = "";
        Address_S = _rIpAddress_S.substr(PosAfter);
      }
      Rts_E = BOF_ERR_NO_ERROR;
    }
    else
    {
      Protocol_S = "???";
      PosAfter = 0;
      Interface_S = "";
      Address_S = _rIpAddress_S.substr(PosAfter);
      Rts_E = BOF_ERR_NO_ERROR;
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
        _rInterfaceIpAddress_X.SocketType_E = Type_E;
        _rInterfaceIpAddress_X.Protocol_S = Protocol_S;
      }
    }

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (Address_S != "")
      {
        Rts_E = Bof_IpAddressToSocketAddress(Address_S, _rIpAddress_X);
        _rIpAddress_X.SocketType_E = Type_E;
        _rIpAddress_X.Protocol_S = Protocol_S;
      }
    }
  }
  return Rts_E;
}

std::string Bof_BuildIpAddress(const BOF_SOCKET_ADDRESS &_rInterfaceIpAddress_X, const BOF_SOCKET_ADDRESS &_rIpAddress_X)
{
  std::string Rts_S;

  Rts_S = Bof_SocketAddressToString(_rInterfaceIpAddress_X, true, true);
  if (!Bof_IsIpAddressNull(_rIpAddress_X))
  {
    Rts_S += BOF_INTERFACE_ADDRESS_SEPARATOR + Bof_SocketAddressToString(_rIpAddress_X, false, true);
  }
  return Rts_S;
}

BOFERR Bof_GetCompatibleIpAddress(const std::vector<BOF_NETWORK_INTERFACE_PARAM> &_rListOfNetworkInterface_X, const BOF_SOCKET_ADDRESS &_rIpAddress_X, BOF_SOCKET_ADDRESS &_rCompatibleIpAddress_X)
{
  BOFERR Rts_E = BOF_ERR_NOT_FOUND;
  uint32_t i_U32, j_U32, TargetIp_U32, Ip_U32, Mask_U32, *pIp_U32, *pMask_U32;
  const uint32_t *pTargetIp_U32;
  BOF_SOCKET_ADDRESS Ip_X, Mask_X, Gw_X;
  BOF_INTERFACE_INFO InterfaceInfo_X;

  _rCompatibleIpAddress_X.IpV6_B = _rIpAddress_X.IpV6_B;
  Ip_X.IpV6_B = _rIpAddress_X.IpV6_B;
  Mask_X.IpV6_B = _rIpAddress_X.IpV6_B;
  Gw_X.IpV6_B = _rIpAddress_X.IpV6_B;
  pTargetIp_U32 = nullptr;
  TargetIp_U32 = 0;
  _rCompatibleIpAddress_X.Reset();
  if (_rIpAddress_X.IpV6_B)
  {
    pTargetIp_U32 = reinterpret_cast<const uint32_t *>(_rIpAddress_X.IpV6Address_X.sin6_addr.s6_addr);
  }
  else
  {
    TargetIp_U32 = ntohl(_rIpAddress_X.IpV4Address_X.sin_addr.s_addr);
  }
  for (i_U32 = 0; i_U32 < _rListOfNetworkInterface_X.size(); i_U32++)
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
          pMask_U32 = reinterpret_cast<uint32_t *>(Mask_X.IpV6Address_X.sin6_addr.s6_addr);
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
  BOFERR Rts_E = BOF_ERR_EINVAL;
  uint32_t i_U32;

  if (_rIpAddress_X.IpV6_B)
  {
    uint16_t *pIp_U16, Val_U16;

    //		BOF_ASSERT(_rBinFormat.size() >= 16);
    pIp_U16 = reinterpret_cast<uint16_t *>(&_rIpAddress_X.IpV6Address_X.sin6_addr.s6_addr); // it is in network order->big endian
    if (pIp_U16)
    {
      for (i_U32 = 0; i_U32 < 8; i_U32++)
      {
        Val_U16 = ntohs(pIp_U16[i_U32]);
        _rBinFormat.push_back(Val_U16);
      }
      //			_rBinFormat.resize(i_U32);
      Rts_E = BOF_ERR_NO_ERROR;
    }
  }
  else
  {
    uint8_t *pIp_U8;

    //		BOF_ASSERT(_rBinFormat.size() >= 4);
    pIp_U8 = reinterpret_cast<uint8_t *>(&_rIpAddress_X.IpV4Address_X.sin_addr.s_addr); // it is in network order->big endian
    if (pIp_U8)
    {
      for (i_U32 = 0; i_U32 < 4; i_U32++)
      {
        _rBinFormat.push_back(pIp_U8[i_U32]);
      }
      //			_rBinFormat.resize(4);
      Rts_E = BOF_ERR_NO_ERROR;
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
    uint16_t *pIp_U16; // , Val_U16;

    pIp_U16 = reinterpret_cast<uint16_t *>(_rBinFormat.data()); // it is in network order-<big endian
    if (pIp_U16)
    {
      _rIpAddress_X.SocketType_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
      _rIpAddress_X.IpV6Address_X.sin6_family = AF_INET6;
      _rIpAddress_X.IpV6Address_X.sin6_port = htons(0);
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
    uint8_t *pTargetIp_U8;
    int i;

    pIp_U16 = reinterpret_cast<uint16_t *>(_rBinFormat.data()); // it is in network order-<big endian
    if (pIp_U16)
    {
      _rIpAddress_X.SocketType_E = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
      _rIpAddress_X.IpV4Address_X.sin_family = AF_INET;
      _rIpAddress_X.IpV4Address_X.sin_port = htons(0);
      pTargetIp_U8 = reinterpret_cast<uint8_t *>(&_rIpAddress_X.IpV4Address_X.sin_addr);
      for (i = 0; i < 4; i++) // U16 value
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
  BOFERR Rts_E = BOF_ERR_EINVAL;
  uint32_t i_U32, NbREvent_U32;
  int Sts_i;

  _rNbPollSet_U32 = 0;
  NbREvent_U32 = 0;
  if (_pListOfPollOp_X)
  {
    Rts_E = BOF_ERR_EACCES;
    // It is not necessary to clear revents for any element prior to calling WSAPoll.
    /*
    for (i_U32 = 0; i_U32 < _NbPollOpInList_U32; i_U32++)
    {
      _pListOfPollOp_X[i_U32].Revent_U16 = 0;
    }
    */
#if defined(_WIN32)
    static_assert(sizeof(WSAPOLLFD) == sizeof(BOF_POLL_SOCKET), "Binary size differs !");
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
      Rts_E = BOF_ERR_NO_ERROR;
      _rNbPollSet_U32 = static_cast<uint32_t>(Sts_i);
    }
#endif
    // Sometime under windows at least in an ever looping BofIo_Test.OpenCloseSession test the _rNbPollSet_U32 (sts_i) value in not equal to the number of mpPollOp_X[i].Revent_U16 entries != from 0
    // We check it here and adjust
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      NbREvent_U32 = 0;
      for (i_U32 = 0; i_U32 < _NbPollOpInList_U32; i_U32++)
      {
        if (_pListOfPollOp_X[i_U32].Revent_U16)
        {
          if (_pListOfPollOp_X[i_U32].Revent_U16 & BOF_POLL_NVAL) // Put -1 in Fd to disable entry
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
    // BOF_DBG_PRINTF("@@@%s Bof_Poll %d/%d poll set->rts %X\n", _rName_S.c_str(), _rNbPollSet_U32, NbREvent_U32, Rts_E);
    for (i_U32 = 0; i_U32 < _NbPollOpInList_U32; i_U32++)
    {
      if (_pListOfPollOp_X[i_U32].Revent_U16)
      {
        // BOF_DBG_PRINTF("@@@%s Bof_Poll Fd[%d]=%08X evt %X\n", _rName_S.c_str(), i_U32, _pListOfPollOp_X[i_U32].Fd, _pListOfPollOp_X[i_U32].Revent_U16);
      }
    }
  }
  return Rts_E;
}
END_BOF_NAMESPACE()