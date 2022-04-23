#include <bofstd/bofgateway.h>


#if defined ( __linux__ )

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <net/if.h>
#include <linux/rtnetlink.h>

#include <unistd.h>
#include <arpa/inet.h>

BEGIN_BOF_NAMESPACE () ;

#define BUFSIZE    8192

struct route_info
{
		struct in_addr dstAddr;
		struct in_addr srcAddr;
		struct in_addr gateWay;
		char ifName[IF_NAMESIZE];
};


int readNlSock(int sockFd, char *bufPtr, size_t buf_size, int seqNum, int pId)
{
	struct nlmsghdr *nlHdr;
	int readLen = 0, msgLen = 0;

	do
	{
		/* Recieve response from the kernel */
		if ((readLen = recv(sockFd, bufPtr, buf_size - msgLen, 0)) < 0)
		{
// perror ( "SOCK READ: " );
			return -1;
		}

		nlHdr = (struct nlmsghdr *) bufPtr;

		/* Check if the header is valid */
		if ((NLMSG_OK (nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
		{
// perror ( "Error in recieved packet" );
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
	} while ((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));

	return msgLen;
}


/* parse the route info returned */
int parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo, int _Family_i)
{
	struct rtmsg *rtMsg;
	struct rtattr *rtAttr;
	int rtLen;
	int Ret_i = 0;

	rtMsg = (struct rtmsg *) NLMSG_DATA (nlHdr);

	/* If the route is not for AF_INET or does not belong to main routing table then return. */
	if ((rtMsg->rtm_family == _Family_i) && (rtMsg->rtm_table == RT_TABLE_MAIN))
	{
		/* get the rtattr field */
		rtAttr = (struct rtattr *) RTM_RTA (rtMsg);
		rtLen = RTM_PAYLOAD (nlHdr);

		for (; RTA_OK (rtAttr, rtLen); rtAttr = RTA_NEXT (rtAttr, rtLen))
		{
			switch (rtAttr->rta_type)
			{
				case RTA_OIF:
				{
					if_indextoname(*(int *) RTA_DATA (rtAttr), rtInfo->ifName);
					break;
				}

				case RTA_GATEWAY:
				{
					memcpy(&rtInfo->gateWay, RTA_DATA (rtAttr), sizeof(rtInfo->gateWay));
					break;
				}

				case RTA_PREFSRC:
				{
					memcpy(&rtInfo->srcAddr, RTA_DATA (rtAttr), sizeof(rtInfo->srcAddr));
					break;
				}

				case RTA_DST:
				{
					memcpy(&rtInfo->dstAddr, RTA_DATA (rtAttr), sizeof(rtInfo->dstAddr));
					break;
				}
			}
		}
	}
	else
	{
		Ret_i = -1;
	}

	return 0;
}


// meat
int Bof_GetGatewayIpAddress(const char *_pInterface_c, char *_pGatewayIp_c, uint32_t _Size_i, int _Family_i)
{
	int found_gatewayip = 0;
	struct nlmsghdr *nlMsg;
	struct rtmsg *rtMsg;
	struct route_info RouteInfo_X;
	int sock = 0;
	int len = 0;
	int msgSeq = 0;
	char msgBuf[BUFSIZE];                                        // pretty large buffer

	if ((_pInterface_c == 0) || (_pGatewayIp_c == 0))
	{
		// perror ( "Invalid arguments" );
		return -1;
	}

	/* Create Socket */
	if ((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
	{
		// perror ( "Socket Creation: " );
		return -1;
	}

	/* Initialize the buffer */
	memset(msgBuf, 0, sizeof(msgBuf));

	/* point the header and the msg structure pointers into the buffer */
	nlMsg = (struct nlmsghdr *) msgBuf;
	rtMsg = (struct rtmsg *) NLMSG_DATA (nlMsg);

	/* Fill in the nlmsg header*/
	nlMsg->nlmsg_len = NLMSG_LENGTH (sizeof(struct rtmsg)); // Length of message.
	nlMsg->nlmsg_type = RTM_GETROUTE;                             // Get the routes from kernel routing table .
	nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;               // The message is a request for dump.
	nlMsg->nlmsg_seq = msgSeq++;                                 // Sequence of the message packet.
	nlMsg->nlmsg_pid = getpid();                               // PID of process sending the request.

	/* Send the request */
	if (send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0)
	{
// fprintf ( stderr, "Write To Socket Failed...\n" );
		return -1;
	}

	/* Read the response */
	if ((len = readNlSock(sock, msgBuf, sizeof(msgBuf), msgSeq, getpid())) < 0)
	{
// fprintf ( stderr, "Read From Socket Failed...\n" );
		return -1;
	}

	/* Parse and print the response */
	for (; NLMSG_OK (nlMsg, len); nlMsg = NLMSG_NEXT (nlMsg, len))
	{
		memset(&RouteInfo_X, 0, sizeof(route_info));

		if (parseRoutes(nlMsg, &RouteInfo_X, _Family_i) < 0)
		{
			continue;                                                  // don't check route_info if it has not been set up
		}

		// Check if default gateway
		if (strstr((char *) inet_ntoa(RouteInfo_X.dstAddr), "0.0.0.0"))
		{
			// It's the interface we are looking for
			if (strcmp(RouteInfo_X.ifName, _pInterface_c) == 0)
			{
				// copy it over
				inet_ntop(AF_INET, &RouteInfo_X.gateWay, _pGatewayIp_c, _Size_i);
				found_gatewayip = 1;
				break;
			}
		}
	}

	close(sock);

	return found_gatewayip;
}
END_BOF_NAMESPACE ();

#else
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa365915(v=vs.85).aspx
#include <winsock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <bofstd/bofstring.h>
BEGIN_BOF_NAMESPACE ( );
#define WORKING_BUFFER_SIZE    0x20000
int Bof_GetGatewayIpAddress ( const char *_pInterface_c, char *_pGatewayIp_c, uint32_t _Size_i, int _Family_i )
{
	int Rts_i																		 = BOFERR_INVALID_PARAM;
	DWORD Sts_DW;
	ULONG Flag_UL																 = GAA_FLAG_INCLUDE_GATEWAYS | GAA_FLAG_INCLUDE_ALL_INTERFACES | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_MULTICAST;
	uint8_t pBuffer_U8[ WORKING_BUFFER_SIZE ];
	PIP_ADAPTER_ADDRESSES pAdapterAddressArray_X = ( PIP_ADAPTER_ADDRESSES ) pBuffer_U8;
	ULONG OutputBufferLen_UL;
	WCHAR pInterfaceName_wc[ 0x1000 ];
	PIP_ADAPTER_ADDRESSES pCrtAdapterAddress_X;
	LPSOCKADDR pSockeckAddress_X;
	sockaddr_in *pSaInV4_X;
	sockaddr_in6 *pSaInV6_X;

	if ( ( _pInterface_c ) && ( _pGatewayIp_c ) && ( _Size_i ) )
	{
		Rts_i							 = BOFERR_NOT_FOUND;
		Bof_MultiByteToWideChar(_pInterface_c, sizeof(pInterfaceName_wc) / sizeof(WCHAR), pInterfaceName_wc);
		OutputBufferLen_UL = WORKING_BUFFER_SIZE;
		Sts_DW						 = GetAdaptersAddresses ( _Family_i, Flag_UL, nullptr, pAdapterAddressArray_X, & OutputBufferLen_UL ); // WORKING_BUFFER_SIZE should be enough
		if ( Sts_DW == NO_ERROR )
		{
			pCrtAdapterAddress_X = pAdapterAddressArray_X;
			while ( pCrtAdapterAddress_X )
			{
				if ( ! wcscmp ( pInterfaceName_wc, pCrtAdapterAddress_X->FriendlyName ) )
				{
					if ( pCrtAdapterAddress_X->FirstGatewayAddress ) 
					{
						pSockeckAddress_X = pCrtAdapterAddress_X->FirstGatewayAddress->Address.lpSockaddr;
						if ((pSockeckAddress_X) && (pCrtAdapterAddress_X->FirstGatewayAddress->Address.iSockaddrLength))
						{
							Rts_i = BOFERR_TOO_SMALL;
							if (pSockeckAddress_X->sa_family == AF_INET)
							{
								pSaInV4_X = (sockaddr_in *)pSockeckAddress_X;
								if (inet_ntop(AF_INET, &(pSaInV4_X->sin_addr), _pGatewayIp_c, _Size_i))
								{
									Rts_i = BOFERR_NO_ERROR;
								}
							}
							else if (pSockeckAddress_X->sa_family == AF_INET6)
							{
								pSaInV6_X = (sockaddr_in6 *)pSockeckAddress_X;
								if (inet_ntop(AF_INET6, &(pSaInV6_X->sin6_addr), _pGatewayIp_c, _Size_i))
								{
									Rts_i = BOFERR_NO_ERROR;
								}
							}
							else
							{
								Rts_i = BOFERR_INTERNAL_ERROR;
							}
						}
					}
					break;
				}
				pCrtAdapterAddress_X = pCrtAdapterAddress_X->Next;
			}
		}
	}
	return Rts_i;
}

END_BOF_NAMESPACE ( );
#endif

