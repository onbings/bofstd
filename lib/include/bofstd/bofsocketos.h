/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines os dependent routines for creating and managing socket
 *
 * Name:        bofsocketos.h
 * Author:      Bernard HARMEL: b.harmel@belgacom.net
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Aug 21 2001  BHA : Initial release
 */
#pragma once

/*** Include ***********************************************************************************************************************/
#include <bofstd/bofstd.h>
#include <bofstd/bofflag.h>
//#include <bofstd/bofsocket.h>
#include <cstdint>
#include <vector>
#include <string.h>

#if defined (_WIN32)
#include <WinSock2.h>
#include <ws2tcpip.h>
#define BOF_POLL_RDHUP 0	//does not exist in _WIN32
#else
#define SOCKET_ERROR                 -1

#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <poll.h>

#define BOF_POLL_RDHUP POLLRDHUP
#endif
#define BOF_POLL_IN     POLLIN
#define BOF_POLL_OUT   POLLOUT
#define BOF_POLL_HUP   POLLHUP
#define BOF_POLL_ERR   POLLERR
#define BOF_POLL_NVAL  POLLNVAL

BEGIN_BOF_NAMESPACE()
  class BofSocketIo;

#define BOFSOCKET_INVALID           static_cast<BOFSOCKET>(-1)        //INVALID_SOCKET
#if defined(_WIN32)
  typedef SOCKET BOFSOCKET;
  typedef WSABUF  SCATTER_GATHER_BUFFER;
#else
  typedef int          BOFSOCKET;
  typedef struct iovec SCATTER_GATHER_BUFFER;
#endif

  struct BOF_POLL_SOCKET
  {
    BOFSOCKET Fd;
    uint16_t  Event_U16;
    uint16_t  Revent_U16;

    BOF_POLL_SOCKET()
    {
      Reset();
    }

    void Reset()
    {
      Fd         = (BOFSOCKET) -1;
      Event_U16  = 0;
      Revent_U16 = 0;
    }
  };

  enum class BOF_POLL_SOCKET_OP : uint8_t
  {
    BOF_POLL_SOCKET_OP_NONE         = ' ',
    //BOF_POLL_SOCKET_OP_CANCEL_WAIT = 'C',
    BOF_POLL_SOCKET_OP_ADD_ENTRY    = 'A',
    BOF_POLL_SOCKET_OP_QUIT         = 'Q',
    BOF_POLL_SOCKET_OP_REMOVE_ENTRY = 'R',
    BOF_POLL_SOCKET_OP_TEST         = 'T',
  };

#pragma pack(1)

  struct BOF_POLL_SOCKET_CMD
  {
    BOF_POLL_SOCKET_OP SocketOp_E;
    uint8_t            pSpare_U8[3];
    BOFSOCKET          SessionId;
    uint32_t           AnswerTicket_U32;    //If not zero a BOF_POLL_SOCKET_CMD is sent back by the processing thread to the caller to provide sync mecanism. If 0 no result back (full async without synchro)
    uint32_t           AnswerArg_U32;        //Optional arg value used by the snt back value.

    BOF_POLL_SOCKET_CMD()
    {
      Reset();
    }

    void Reset()
    {
      SocketOp_E       = BOF_POLL_SOCKET_OP::BOF_POLL_SOCKET_OP_NONE;
      SessionId        = BOFSOCKET_INVALID;
      AnswerTicket_U32 = 0;
      AnswerArg_U32    = 0;
    }
  };

#pragma pack()

#define BOF_U32IPADDR_TO_U8IPADDR(Address, Ip1, Ip2, Ip3, Ip4)  {uint32_t Ip=htonl(Address);Ip1=static_cast<uint8_t>(Ip>>24);Ip2=static_cast<uint8_t>(Ip>>16);Ip3=static_cast<uint8_t>(Ip>>8);Ip4=static_cast<uint8_t>(Ip);}

  enum class BOF_NETWORK_INTERFACE_FLAG : uint32_t
  {
    BOF_IFF_NONE        = 0x00000000,    /*! Nothing.*/
    BOF_IFF_UP          = 0x00000001,    /*! Interface is running.*/
    BOF_IFF_BROADCAST   = 0x00000002,    /*! Valid broadcast address set.*/
    BOF_IFF_LOOPBACK    = 0x00000004,    /*! Interface is a loopback interface.*/
    BOF_IFF_POINTOPOINT = 0x00000008,    /*! Interface is a point - to - point link.*/
    BOF_IFF_MULTICAST   = 0x00000010,    /*! Supports multicast*/
  };
  template<>
  struct IsItAnEnumBitFLag<BOF_NETWORK_INTERFACE_FLAG> : std::true_type
  {
  };

  enum class BOF_SOCK_TYPE : int32_t
  {
//		BOF_SOCK_UNDEF = 0,                /*! None */
      BOF_SOCK_UNKNOWN = 0,                /*! None */
      BOF_SOCK_TCP,                     /*! stream socket */
      BOF_SOCK_UDP,                      /*! datagram socket */
//	BOF_SOCK_RAW,                        /*! raw-protocol interface */
//	BOF_SOCK_RDM,                        /*! reliably-delivered message */
//	BOF_SOCK_SEQPACKET,                  /*! sequenced packet stream */
  };
  enum class BOF_PROTOCOL_TYPE : int32_t
  {
    BOF_PROTOCOL_UNKNOWN = 0,                /*! None */
    BOF_PROTOCOL_TCP,                 /*! stream protocol */
    BOF_PROTOCOL_UDP,                  /*! datagram protocol */
    BOF_PROTOCOL_FTP,                    /*! ftp protocol */
    BOF_PROTOCOL_TFTP,                   /*! ftp protocol */
    BOF_PROTOCOL_HTTP,                   /*! http protocol */
    BOF_PROTOCOL_HTTPS,                  /*! https protocol */
    BOF_PROTOCOL_TELNET,                 /*! telnet protocol */
    BOF_PROTOCOL_SSH,                    /*! ssh protocol */
    BOF_PROTOCOL_ZMQ_PGM,                /*! Zmq pgm protocol */
    BOF_PROTOCOL_PIPE,                   /*! Named Pipe(windows) / Unix domain socket */
    BOF_PROTOCOL_SHARED_RAM,             /*! Shared memory */
    BOF_PROTOCOL_MAX
  };
/*** Typedef ***********************************************************************************************************************/

  typedef struct sockaddr     BOF_SOCKADDR;
  typedef struct sockaddr_in  BOF_SOCKADDR_IN;
  typedef struct sockaddr_in6 BOF_SOCKADDR_IN6;
  typedef struct in_addr      BOF_IN_ADDR;
  typedef struct in6_addr     BOF_IN_ADDR6;

/*** Structures ********************************************************************************************************************/

  struct BOF_SOCKET_ADDRESS_COMPONENT
  {
    std::string Protocol_S;
    std::string IpAddress_S;
    uint16_t    Port_U16;

    BOF_SOCKET_ADDRESS_COMPONENT()
    {
      Reset();
    }

    void Reset()
    {
      Protocol_S  = "";
      IpAddress_S = "";
      Port_U16    = 0;
    }
  };

  struct BOF_SOCKET_ADDRESS
  {
    bool              IpV6_B;
    BOF_SOCK_TYPE     SocketType_E;
    BOF_PROTOCOL_TYPE ProtocolType_E;
    BOF_SOCKADDR_IN   IpV4Address_X;
    BOF_SOCKADDR_IN6  IpV6Address_X;

    BOF_SOCKET_ADDRESS()
    {
      Reset();
    }

    BOF_SOCKET_ADDRESS(bool _IpV6_B, BOF_SOCK_TYPE _SocketType_E, BOF_PROTOCOL_TYPE _ProtocolType_E, uint32_t _Ip1_U32, uint32_t _Ip2_U32, uint32_t _Ip3_U32, uint32_t _Ip4_U32, uint16_t _Port_U16)
    {
      Set(_IpV6_B, _SocketType_E, _ProtocolType_E, _Ip1_U32, _Ip2_U32, _Ip3_U32, _Ip4_U32, _Port_U16);
    }
//TODO a faire 
//  void Set(const std::string &_rAddress);

    void Set(bool _IpV6_B, BOF_SOCK_TYPE _SocketType_E, BOF_PROTOCOL_TYPE _ProtocolType_E, uint32_t _Ip1_U32, uint32_t _Ip2_U32, uint32_t _Ip3_U32, uint32_t _Ip4_U32, uint16_t _Port_U16)
    {
      IpV6_B = _IpV6_B;
      if (_IpV6_B)
      {
        uint16_t *pIp_U16, Val_U16;

        IpV6Address_X.sin6_family   = AF_INET6;
        IpV6Address_X.sin6_port     = htons(_Port_U16);
        IpV6Address_X.sin6_flowinfo = 0;
        IpV6Address_X.sin6_scope_id = 0;
        pIp_U16 = reinterpret_cast<uint16_t *>(&IpV6Address_X.sin6_addr.s6_addr);
        Val_U16 = htons(static_cast<uint16_t>(_Ip1_U32 >> 16));
        *pIp_U16++ = Val_U16;
        Val_U16 = htons(static_cast<uint16_t>(_Ip1_U32));
        *pIp_U16++ = Val_U16;
        Val_U16 = htons(static_cast<uint16_t>(_Ip2_U32 >> 16));
        *pIp_U16++ = Val_U16;
        Val_U16 = htons(static_cast<uint16_t>(_Ip2_U32));
        *pIp_U16++ = Val_U16;
        Val_U16 = htons(static_cast<uint16_t>(_Ip3_U32 >> 16));
        *pIp_U16++ = Val_U16;
        Val_U16 = htons(static_cast<uint16_t>(_Ip3_U32));
        *pIp_U16++ = Val_U16;
        Val_U16 = htons(static_cast<uint16_t>(_Ip4_U32 >> 16));
        *pIp_U16++ = Val_U16;
        Val_U16 = htons(static_cast<uint16_t>(_Ip4_U32));
        *pIp_U16++ = Val_U16;

      }
      else
      {
        uint8_t *pIp_U8;

        IpV4Address_X.sin_family = AF_INET;
        IpV4Address_X.sin_port   = htons(_Port_U16);
        pIp_U8 = reinterpret_cast<uint8_t *>(&IpV4Address_X.sin_addr.s_addr);
        *pIp_U8++ = static_cast<uint8_t>(_Ip1_U32);
        *pIp_U8++ = static_cast<uint8_t>(_Ip2_U32);
        *pIp_U8++ = static_cast<uint8_t>(_Ip3_U32);
        *pIp_U8++ = static_cast<uint8_t>(_Ip4_U32);
      }
      SocketType_E   = _SocketType_E;
      ProtocolType_E = _ProtocolType_E;
    }

    void Inc(int32_t _Inc_S32)
    {
      uint32_t                         i_U32;
      int32_t                          Val_S32;
      uint16_t                         *pIp_U16;
      uint8_t                          *pIp_U8;

      if (IpV6_B)
      {
        if ((_Inc_S32 > -0xFFFE) && (_Inc_S32 < 0xFFFE))
        {
          pIp_U16    = reinterpret_cast<uint16_t *>(&IpV6Address_X.sin6_addr.s6_addr);
          for (i_U32 = 0; i_U32 < 8; i_U32++)
          {
            Val_S32 = ntohs(static_cast<uint16_t>(pIp_U16[7 - i_U32]));
            Val_S32 += _Inc_S32;
            if (Val_S32 >= 0xFFFE)
            {
              pIp_U16[7 - i_U32] = htons(static_cast<uint16_t>(Val_S32 - 0xFFFE));
            }
            else if (Val_S32 <= 0)
            {
              pIp_U16[7 - i_U32] = htons(static_cast<uint16_t>(Val_S32 + 0xFFFE));
            }
            else
            {
              pIp_U16[7 - i_U32] = htons(static_cast<uint16_t>(Val_S32));
              break;
            }
          }
        }
      }
      else
      {
        if ((_Inc_S32 > -0xFE) && (_Inc_S32 < 0xFE))
        {
          pIp_U8    = reinterpret_cast<uint8_t *>(&IpV4Address_X.sin_addr.s_addr);
          for (i_U32 = 0; i_U32 < 4; i_U32++)
          {
            Val_S32 = static_cast<uint8_t>(pIp_U8[3 - i_U32]);
            Val_S32 += _Inc_S32;
            if (Val_S32 >= 0xFE)
            {
              pIp_U8[3 - i_U32] = static_cast<uint8_t>(Val_S32 - 0xFE);
            }
            else if (Val_S32 <= 0)
            {
              pIp_U8[3 - i_U32] = static_cast<uint8_t>(Val_S32 + 0xFE);
            }
            else
            {
              pIp_U8[3 - i_U32] = static_cast<uint8_t>(Val_S32);
              break;
            }
          }
        }
      }
    }

    void Set(BOF_SOCK_TYPE _SocketType_E, BOF_PROTOCOL_TYPE _ProtocolType_E, uint32_t _IpV4_U32, uint16_t _Port_U16)
    {
      IpV6_B = false;

      IpV4Address_X.sin_family      = AF_INET;
      IpV4Address_X.sin_port        = htons(_Port_U16);
      IpV4Address_X.sin_addr.s_addr = htonl(_IpV4_U32);
      SocketType_E   = _SocketType_E;
      ProtocolType_E = _ProtocolType_E;
    }

    uint16_t Port() const
    {
      uint16_t Rts_U16 = IpV6_B ? IpV6Address_X.sin6_port : IpV4Address_X.sin_port;
      return ntohs(Rts_U16);
    }
		void Port(uint16_t _Port_U16)
		{
    	if (IpV6_B)
			{
				IpV6Address_X.sin6_port        = htons(_Port_U16);
			}
    	else
			{
				IpV4Address_X.sin_port        = htons(_Port_U16);
			}
		}

    uint32_t IpV4Address() const
    {
      uint32_t Rts_U32 = IpV4Address_X.sin_addr.s_addr;
      return ntohl(Rts_U32);
    }
    void Parse(BOF_SOCK_TYPE &_rSocketType_E, BOF_PROTOCOL_TYPE &_rProtocolType_E, uint32_t &_rIp1_U32, uint32_t &_rIp2_U32, uint32_t &_rIp3_U32, uint32_t &_rIp4_U32, uint16_t &_rPort_U16)
    {
      if (IpV6_B)
      {
        uint32_t *pIp_U32;

        _rPort_U16 = ntohs(IpV6Address_X.sin6_port);
        pIp_U32    = reinterpret_cast<uint32_t *>(&IpV6Address_X.sin6_addr.s6_addr);
        _rIp1_U32  = ntohl(pIp_U32[0]);
        _rIp2_U32  = ntohl(pIp_U32[1]);
        _rIp3_U32  = ntohl(pIp_U32[2]);
        _rIp4_U32  = ntohl(pIp_U32[3]);
      }
      else
      {
        uint8_t *pIp_U8;

        _rPort_U16 = ntohs(IpV4Address_X.sin_port);
        pIp_U8     = reinterpret_cast<uint8_t *>(&IpV4Address_X.sin_addr.s_addr);
        _rIp1_U32  = *pIp_U8++;
        _rIp2_U32  = *pIp_U8++;
        _rIp3_U32  = *pIp_U8++;
        _rIp4_U32  = *pIp_U8++;
      }
      _rSocketType_E   = SocketType_E;
      _rProtocolType_E = ProtocolType_E;
    }

    void Parse(uint16_t &_rPort_U16)
    {
      if (IpV6_B)
      {
        _rPort_U16 = ntohs(IpV6Address_X.sin6_port);
      }
      else
      {
        _rPort_U16 = ntohs(IpV4Address_X.sin_port);
      }
    }

    void Reset()
    {
      IpV6_B         = false;
      SocketType_E   = BOF_SOCK_TYPE::BOF_SOCK_UNKNOWN;
      ProtocolType_E = BOF_PROTOCOL_TYPE::BOF_PROTOCOL_UNKNOWN;
      IpV4Address_X.sin_addr.s_addr = 0;
      IpV4Address_X.sin_family      = AF_UNSPEC;
      IpV4Address_X.sin_port        = 0;

      memset(IpV6Address_X.sin6_addr.s6_addr, 0, 16);
      IpV6Address_X.sin6_family   = AF_UNSPEC;
      IpV6Address_X.sin6_flowinfo = 0;
      IpV6Address_X.sin6_port     = 0;
      IpV6Address_X.sin6_scope_id = 0;
      //IpV6Address_X.sin6_scope_struct = 0;
    }

  };

  struct BOF_INTERFACE_INFO
  {
    std::string                IpGateway_S;
    BOF_NETWORK_INTERFACE_FLAG InterfaceFlag_E;
    uint32_t                   MtuSize_U32;
    std::vector<uint8_t>       MacAddress;

    BOF_INTERFACE_INFO()
    {
      Reset();
    }

    void Reset()
    {
      IpGateway_S     = "";
      InterfaceFlag_E = BOF_NETWORK_INTERFACE_FLAG::BOF_IFF_NONE;
      MtuSize_U32     = 0;
      MacAddress.clear();
    }
  };

  struct BOF_NETWORK_INTERFACE_PARAM
  {
    bool        IpV6_B;        // Only ipV4 supported for now
    std::string Name_S;        // 16:IF_NAMESIZE linux 260: MAX_ADAPTER_NAME_LENGTH+4 win3�
    std::string IpAddress_S;
    std::string IpMask_S;
    std::string IpBroadcast_S;

    BOF_INTERFACE_INFO Info_X;

    BOF_NETWORK_INTERFACE_PARAM()
    {
      Reset();
    }

    void Reset()
    {
      IpV6_B        = false;
      Name_S        = "";
      IpAddress_S   = "";
      IpMask_S      = "";
      IpBroadcast_S = "";
    }
  };

/*** Constants *********************************************************************************************************************/

/*** Prototypes ********************************************************************************************************************/

  BOFERR Bof_GetNetworkInterfaceInfo(const std::string _rInterfaceName_S, BOF_INTERFACE_INFO &_rInterfaceInfo_X);

  BOFERR Bof_SetNetworkInterfaceParam(const std::string _rInterfaceName_S, BOF_NETWORK_INTERFACE_PARAM &_rNewInterfaceParam_X);  //TODO IpV6 version

  BOFERR Bof_GetListOfNetworkInterface(std::vector<BOF_NETWORK_INTERFACE_PARAM> &_rListOfNetworkInterface_X); //TODO IpV6 version
  BOFERR Bof_GetNetworkInterfaceParam(const std::string _rInterfaceName_S, BOF_NETWORK_INTERFACE_PARAM &_rNewInterfaceParam_X); //TODO IpV6 version

  int32_t Bof_Compute_CidrMask(const std::string &_rIpV4Address_S);  //TODO IpV6 version
  std::string Bof_ProtocolTypeToString(BOF_PROTOCOL_TYPE _ProtocolType_E);

  std::string Bof_SocketAddressToString(const BOF_SOCKET_ADDRESS &_rIpAddress_X, bool _ShowType_B, bool _ShowPortNumber_B);

//std::string Bof_SockAddrInToString(const struct sockaddr &_rSockAddress_X, bool _ShowPortNumber_B);

  std::string Bof_SockAddrInToString(const BOF_SOCKADDR_IN &_rSockAddressIn_X, bool _ShowPortNumber_B);

  std::string Bof_SockAddrInToString(const BOF_SOCKADDR_IN6 &_rSockAddressIn_X, bool _ShowPortNumber_B);

  BOFERR Bof_UrlAddressToSocketAddressCollection(const std::string &_rIpOrUrlAddress_S, std::vector<BOF_SOCKET_ADDRESS> &_rListOfIpAddress_X);

  BOFERR Bof_IpAddressToSocketAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS &_rIpAddress_X, std::string *_pIpAddress_S= nullptr);

  BOFERR Bof_SocketAddressToBin(BOF_SOCKET_ADDRESS &_rIpAddress_X, std::vector<uint16_t> &_rBinFormat);

  BOFERR Bof_BinToSocketAddress(std::vector<uint16_t> &_rBinFormat, BOF_SOCKET_ADDRESS &_rIpAddress_X);

  BOFERR Bof_ProtocolType(const std::string &_rIpAddress_S, BOF_PROTOCOL_TYPE &_rProtocolType_E);

  BOFERR Bof_IpAddressToBin(const std::string &_rIpAddress_S, bool &_rIsIpV6_B, std::vector<uint16_t> &_rIpBinDigitCollection);

  BOF_SOCK_TYPE Bof_ProtocolToSocketType(BOF_PROTOCOL_TYPE _Protocol_E);

  BOFERR Bof_SplitIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS_COMPONENT &_rInterfaceIpAddress_X, BOF_SOCKET_ADDRESS_COMPONENT &_rIpAddress_X);

  BOFERR Bof_SplitIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS &_rInterfaceIpAddress_X, BOF_SOCKET_ADDRESS &_rIpAddress_X);

  BOFERR Bof_SplitIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS_COMPONENT &_rIpAddress_X);

  BOFERR Bof_SplitIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS &_rIpAddress_X);

  BOFERR Bof_ResolveIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS &_rInterfaceIpAddress_X, BOF_SOCKET_ADDRESS &_rIpAddress_X);

  std::string Bof_BuildIpAddress(const BOF_SOCKET_ADDRESS &_rInterfaceIpAddress_X, const BOF_SOCKET_ADDRESS &_rIpAddress_X);

  bool Bof_IsMulticastIpAddress(const std::string &_rIpAddress_S, BOF_SOCKET_ADDRESS_COMPONENT &_rInterfaceIpAddressComponent_X, BOF_SOCKET_ADDRESS_COMPONENT &_rIpAddressComponent_X);

  bool Bof_IsIpAddressEqual(bool CheckType_i, bool _CheckFamily_B, bool _CheckPort_B, const BOF_SOCKET_ADDRESS &_rSrcIpAddress_X, const BOF_SOCKET_ADDRESS &_rDstIpAddress_X);

  bool Bof_IsIpAddressNull(const BOF_SOCKET_ADDRESS &_rIpAddress_X);

  bool Bof_IsIpAddressNull(const std::string &_rIpAddress_S);

  bool Bof_IsIpAddressLocalHost(const BOF_SOCKET_ADDRESS &_rIpAddress_X);

  bool Bof_IsIpAddressLocalHost(const std::string &_rIpAddress_S);

  BOFERR Bof_GetCompatibleIpAddress(const std::vector<BOF_NETWORK_INTERFACE_PARAM> &_rListOfNetworkInterface_X, const BOF_SOCKET_ADDRESS &_rIpAddress_X, BOF_SOCKET_ADDRESS &_rCompatibleIpAddress_X);

  BOFERR Bof_Poll(uint32_t _TimeoutInMs_U32, uint32_t _NbPollOpInList_U32, BOF_POLL_SOCKET *_pListOfPollOp_X, uint32_t &_rNbPollSet_U32);

/*** Classes ***********************************************************************************************************************/

END_BOF_NAMESPACE()