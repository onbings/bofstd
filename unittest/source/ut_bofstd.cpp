#include <cstdint>
#include "../include/gtestrunner.h"

#include <bofstd/BofSystem.h>
#include <bofstd/BofUart.h>
#include <bofstd/BofSocket.h>

#if defined( __linux__ ) || defined( __APPLE__ )
#include <errno.h>
#include <arpa/inet.h>
#endif

USE_BOF_NAMESPACE()

namespace
{
  class BofStd_Test : public testing::Test
  {
  protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    virtual void SetUp() {}
    virtual void TearDown() {}
  private:
  };

#if (1)	//it is an integation test which should fail on build station defined(TARGET_PLATFORM_H3X) || defined(TARGET_PLATFORM_H3X_GBE) || defined(TARGET_PLATFORM_TABLET)  || defined(__APPLE__)
#else
  TEST_F(BofStd_Test, CheckUart)
  {
    BofUart *    pBofUart_O;
    BOFUARTPARAM Params_X;
    uint32_t          i_U32, j_U32, Sts_U32, Nb_U32, pBaud_U32[3];
    uint8_t           pBuffer_U8[128];

    pBaud_U32[0] = 9600;
    pBaud_U32[1] = 19200;
    pBaud_U32[2] = 38400;

    for (j_U32 = 0; j_U32 < 3; j_U32++)
    {
      // Initialize parameters
      memset(&Params_X, 0x00, sizeof(Params_X));
      Params_X.Baud_U32 = pBaud_U32[j_U32];
      Params_X.Port_U32 = COM_PORT2;
      Params_X.Data_U8 = 8;
      Params_X.Parity_U8 = NOPARITY;
      Params_X.Stop_U8 = ONESTOPBIT;
      Params_X.RtsCts_B = false;
      Params_X.XonXoff_B = false;
      Params_X.DtrDsr_B = false;

      Params_X.BufferInSize_U32 = 1024;
      Params_X.BufferOutSize_U32 = 1024;
      Params_X.SynchronousWrite_B = true;
      pBofUart_O = new BofUart(&Params_X);

      // The port has been opened
      ASSERT_TRUE(pBofUart_O != nullptr);
      ASSERT_EQ(Params_X.ErrorCode_U32, 0);
      Nb_U32 = sizeof(pBuffer_U8);

      for (i_U32 = 0; i_U32 < Nb_U32; i_U32++)
      {
        pBuffer_U8[i_U32] = (uint8_t)i_U32;
      }
      Sts_U32 = pBofUart_O->V_WriteData(1000, &Nb_U32, pBuffer_U8);
      ASSERT_EQ(Sts_U32, 0);
      ASSERT_EQ(Nb_U32, sizeof(pBuffer_U8));

      Nb_U32 = sizeof(pBuffer_U8);
      Sts_U32 = pBofUart_O->V_ReadData(0, &Nb_U32, pBuffer_U8);
      ASSERT_EQ(Sts_U32, BOFERR_READ);
      ASSERT_EQ(Nb_U32, 0);


      Sts_U32 = pBofUart_O->V_PurgeData(1000);
      ASSERT_EQ(Sts_U32, 0);
      SAFE_DELETE(pBofUart_O);
      ASSERT_TRUE(pBofUart_O == nullptr);
    }
  }
#endif

  TEST_F(BofStd_Test, StaticSocket)
  {
    int              Sts_i;
    SOCKADDR_IN      pIpAddress_X[2], HostIpAddress_X;
    uint32_t              pIpAddressInNetworkOrder_U32[2], Sts_U32;
    bool             Sts_B;
    char *           pIp_c, pIpAddress_c[64], pWork_c[64], pHostName_c[128];
    uint8_t               pIp_U8[2][4], pIpHost_U8[4];
    uint16_t              pPort_U16[2], PortHost_U16;
    uint32_t              MaxIpParamEntry_U32, NbIpAddress_U32;
    BOFSOCKETIPPARAM pSrcIpParam_X[32], pDstIpParam_X[32];  // , IpParam_X;


 
//    return;
#if 0 //test socket listen connect
    BOFSOCKETPARAM        BofSocketParam_X;
    BofSocket             *pListenSocket_O;
    void *                Client_h;
    BofSocket             *pClient_O;

    memset(&BofSocketParam_X, 0, sizeof(BofSocketParam_X));
    BofSocketParam_X.LocalIpAddress_X.sin_family = AF_INET;
#if defined( __linux__ ) || defined( __APPLE__ )
    BofSocketParam_X.LocalIpAddress_X.sin_addr.s_addr = INADDR_ANY;
#else
    BofSocketParam_X.LocalIpAddress_X.sin_addr.S_un.S_addr = INADDR_ANY;
#endif    
    BofSocketParam_X.LocalIpAddress_X.sin_port = htons(55000);
    BofSocketParam_X.Type_i = SOCK_STREAM;
    BofSocketParam_X.ListeningState_B = true;
    BofSocketParam_X.ListenBackLog_U32 = 8;
    BofSocketParam_X.Verbose_B = true;
    BofSocketParam_X.Blocking_B = false;

    strncpy(BofSocketParam_X.pName_c, "Listen", sizeof(BofSocketParam_X.pName_c));
    BofSocketParam_X.pName_c[sizeof(BofSocketParam_X.pName_c) - 1] = 0;
    pListenSocket_O = new BofSocket(&BofSocketParam_X);
    if ((pListenSocket_O)
      && (!BofSocketParam_X.ErrorCode_U32))
    {
      while (1)
      {
        Client_h = pListenSocket_O->V_Listen(1000, 0, nullptr, nullptr);
        if (Client_h)
        {
          pClient_O = static_cast<BofSocket *>(Client_h);
        }
      }
    }
#endif


#if 0 //test socket io
    SOCKADDR_IN           LinxServerAddress_X;
    BOFSOCKETPARAM        BofSocketParam_X;
    BofSocket                             *pEventMulticastSocket_O;
    BofSocket                             *pBroadcastSocket_O;
    BofSocket                             *pSocket_O;
    uint32_t Nb_U32;
    uint8_t pBuffer_U8[0x1000];


    memset(&BofSocketParam_X, 0, sizeof(BofSocketParam_X));

#if defined(WIN32)
    BOF_SET_SOCKET_ADDRESS(LinxServerAddress_X, 10, 129, 170, 30, 0);
#else //#if defined(WIN32)
//    BOF_SET_SOCKET_ADDRESS(LinxServerAddress_X, 10, 129, 170, 21, 0);
    BOF_SET_SOCKET_ADDRESS(LinxServerAddress_X, 10, 129, 170, 31, 0);
#endif

#if 0 //WIN32 Broadcast send  
    BofSocketParam_X.LocalIpAddress_X = LinxServerAddress_X;
    BofSocketParam_X.LocalIpAddress_X.sin_port = htons(50001);
    BofSocketParam_X.Broadcast_B = true;
    BofSocketParam_X.BroadcastPort_U16 = 50001;

    BofSocketParam_X.Type_i = SOCK_DGRAM;
    BofSocketParam_X.NoDelay_B = true;
    BofSocketParam_X.RcvBufferSize_U32 = 0x10000;
    BofSocketParam_X.SndBufferSize_U32 = 0x10000;

    BofSocketParam_X.ErrorCode_U32 = 0;
    strncpy(BofSocketParam_X.pName_c, "BrdLnx", sizeof(BofSocketParam_X.pName_c));
    BofSocketParam_X.pName_c[sizeof(BofSocketParam_X.pName_c) - 1] = 0;
    pBroadcastSocket_O = new BofSocket(&BofSocketParam_X);
    if ((pBroadcastSocket_O)
      && (!BofSocketParam_X.ErrorCode_U32))
    {
      Sts_U32=pBroadcastSocket_O->SetTtl(32);
      while (1)
      {
        Nb_U32 = sizeof(pBuffer_U8);
        memset(pBuffer_U8, 0x69, Nb_U32);
        Sts_U32 = pBroadcastSocket_O->V_WriteData(100, &Nb_U32, pBuffer_U8);
        if (Sts_U32 == 0)
        {
          if (Nb_U32)
          {
            Sts_U32 = 0;
          }
        }
        Bof_Sleep(1000);
      }
    }
#endif  //WIN32 Broadcast send  

#if 0 //WIN32 Multicast receive  
    BofSocketParam_X.LocalIpAddress_X = LinxServerAddress_X;
    BofSocketParam_X.LocalIpAddress_X.sin_port = htons(50002);
    BOF_SET_SOCKET_ADDRESS(BofSocketParam_X.MulticastIpAddress_X, 225, 0, 0, 64, 50002);
    BofSocketParam_X.MulticastServer_B = false;
    //  BofSocketParam_X.TTl_U32 = 8;

    BofSocketParam_X.Type_i = SOCK_DGRAM;

    BofSocketParam_X.NoDelay_B = true;
    BofSocketParam_X.RcvBufferSize_U32 = 0x10000;
    BofSocketParam_X.SndBufferSize_U32 = 0x10000;
    BofSocketParam_X.TTl_U32=255;
    BofSocketParam_X.ErrorCode_U32 = 0;
    strncpy(BofSocketParam_X.pName_c, "EvntLnx", sizeof(BofSocketParam_X.pName_c));
    BofSocketParam_X.pName_c[sizeof(BofSocketParam_X.pName_c) - 1] = 0;
    pEventMulticastSocket_O = new BofSocket(&BofSocketParam_X);
    if ((pEventMulticastSocket_O)
      && (!BofSocketParam_X.ErrorCode_U32))
    {
      while (1)
      {
        Nb_U32 = sizeof(pBuffer_U8);
        memset(pBuffer_U8, 0x69, Nb_U32);
        Sts_U32 = pEventMulticastSocket_O->V_ReadData(100, &Nb_U32, pBuffer_U8);
        if (Sts_U32 == 0)
        {
          if (Nb_U32)
          {
            Sts_U32 = 0;
          }
        }
        Bof_Sleep(1000);
      }
    }
#endif  //WIN32 Multicast receive  


#if 0 //PINGU Broadcast receive


    BOF_SET_SOCKET_ADDRESS(LinxServerAddress_X, 255, 255, 255, 255, 50001);

    BofSocketParam_X.LocalIpAddress_X = LinxServerAddress_X;
    //  BofSocketParam_X.LocalIpAddress_X.sin_port = htons(50001);

    BofSocketParam_X.Type_i = SOCK_DGRAM;
    BofSocketParam_X.Blocking_B = false;
    BofSocketParam_X.ListeningState_B = false;
    BofSocketParam_X.NoDelay_B = true;
    BofSocketParam_X.ListenBackLog_U32 = 0;
    BofSocketParam_X.RcvBufferSize_U32 = 0x10000;
    BofSocketParam_X.SndBufferSize_U32 = 0x10000;

    BofSocketParam_X.ErrorCode_U32 = 0;
    strncpy(BofSocketParam_X.pName_c, "sckt", sizeof(BofSocketParam_X.pName_c));
    BofSocketParam_X.pName_c[sizeof(BofSocketParam_X.pName_c) - 1] = 0;
    pSocket_O = new BofSocket(&BofSocketParam_X);
    if ((pSocket_O)
      && (!BofSocketParam_X.ErrorCode_U32))
    {
      while (1)
      {
        Nb_U32 = sizeof(pBuffer_U8);
        memset(pBuffer_U8, 0x69, Nb_U32);
        Sts_U32 = pSocket_O->V_ReadData(100, &Nb_U32, pBuffer_U8);
        if (Sts_U32 == 0)
        {
          if (Nb_U32)
          {
            Sts_U32 = 0;
          }
        }
        Bof_Sleep(1000);
      }
    }


#endif  //PINGU Broadcast receive

#if 0 //PINGU Multicast send  
    BofSocketParam_X.LocalIpAddress_X = LinxServerAddress_X;
    BofSocketParam_X.LocalIpAddress_X.sin_port = htons(50002);
    BOF_SET_SOCKET_ADDRESS(BofSocketParam_X.MulticastIpAddress_X, 225, 0, 0, 64, 50002);
    BofSocketParam_X.MulticastServer_B = true;
    BofSocketParam_X.TTl_U32 = 8;

    BofSocketParam_X.Type_i = SOCK_DGRAM;

    BofSocketParam_X.NoDelay_B = true;
    BofSocketParam_X.RcvBufferSize_U32 = 0x10000;
    BofSocketParam_X.SndBufferSize_U32 = 0x10000;

    BofSocketParam_X.ErrorCode_U32 = 0;
    strncpy(BofSocketParam_X.pName_c, "EvntLnx", sizeof(BofSocketParam_X.pName_c));
    BofSocketParam_X.pName_c[sizeof(BofSocketParam_X.pName_c) - 1] = 0;
    pEventMulticastSocket_O = new BofSocket(&BofSocketParam_X);
    if ((pEventMulticastSocket_O)
      && (!BofSocketParam_X.ErrorCode_U32))
    {
      while (1)
      {
        Nb_U32 = sizeof(pBuffer_U8);
        memset(pBuffer_U8, 0x69, Nb_U32);
        Sts_U32 = pEventMulticastSocket_O->V_WriteData(100, &Nb_U32, pBuffer_U8);
        if (Sts_U32 == 0)
        {
          if (Nb_U32)
          {
            Sts_U32 = 0;
          }
        }
        Bof_Sleep(1000);
      }
    }
#endif  //PINGU Multicast send  

#endif //if test socket io
    pIp_U8[0][0] = 0x01;
    pIp_U8[0][1] = 0x02;
    pIp_U8[0][2] = 0x03;
    pIp_U8[0][3] = 0x04;
    pPort_U16[0] = 0x1234;

    pIp_U8[1][0] = 0x00;
    pIp_U8[1][1] = 0x00;
    pIp_U8[1][2] = 0x00;
    pIp_U8[1][3] = 0x00;
    pPort_U16[1] = 0x0000;

    BOF_SET_SOCKET_ADDRESS(pIpAddress_X[0], pIp_U8[0][0], pIp_U8[0][1], pIp_U8[0][2], pIp_U8[0][3], pPort_U16[0]);
    BOF_SET_SOCKET_ADDRESS(pIpAddress_X[1], pIp_U8[1][0], pIp_U8[1][1], pIp_U8[1][2], pIp_U8[1][3], pPort_U16[1]);
    Sts_B = BofSocket::S_IsIpAddressNull(nullptr);
    ASSERT_FALSE(Sts_B);
    Sts_B = BofSocket::S_IsIpAddressNull(&pIpAddress_X[0]);
    ASSERT_FALSE(Sts_B);
    Sts_B = BofSocket::S_IsIpAddressNull(&pIpAddress_X[1]);
    ASSERT_TRUE(Sts_B);

    BOF_SET_SOCKET_ADDRESS(pIpAddress_X[0], pIp_U8[0][0], pIp_U8[0][1], pIp_U8[0][2], pIp_U8[0][3], pPort_U16[0]);
    pIp_c = inet_ntoa(pIpAddress_X[0].sin_addr);
    sprintf(pIpAddress_c, "%d.%d.%d.%d:%d", pIp_U8[0][0], pIp_U8[0][1], pIp_U8[0][2], pIp_U8[0][3], pPort_U16[0]);
    Sts_i = memcmp(pIp_c, pIpAddress_c, strlen(pIp_c));
    ASSERT_EQ(Sts_i, 0);
    pIp_c = BofSocket::S_IpAddressToString(nullptr, sizeof(pWork_c), pWork_c, true);
    ASSERT_TRUE(pIp_c == nullptr);
    pIp_c = BofSocket::S_IpAddressToString(&pIpAddress_X[0], 7, pWork_c, true);
    ASSERT_TRUE(pIp_c == nullptr);
    pIp_c = BofSocket::S_IpAddressToString(&pIpAddress_X[0], sizeof(pWork_c), nullptr, true);
    ASSERT_TRUE(pIp_c == nullptr);
    pIp_c = BofSocket::S_IpAddressToString(&pIpAddress_X[0], sizeof(pWork_c), pWork_c, true);
    ASSERT_TRUE(pIp_c == pWork_c);


    //#if 0

    Sts_i = memcmp(pIp_c, pIpAddress_c, strlen(pIp_c));
    ASSERT_EQ(Sts_i, 0);

    BOF_PARSE_SOCKET_ADDRESS(pIpAddress_X[0], pIp_U8[1][0], pIp_U8[1][1], pIp_U8[1][2], pIp_U8[1][3], pPort_U16[1]);
    Sts_i = memcmp(pIp_U8[0], pIp_U8[1], sizeof(pIp_U8[0]));
    ASSERT_EQ(Sts_i, 0);
    ASSERT_EQ(pPort_U16[0], pPort_U16[1]);

    BOF_SET_SOCKET_ADDRESS32(pIpAddressInNetworkOrder_U32[0], pIpAddress_X[0]);
    BOF_PARSE_SOCKET_ADDRESS32(pIpAddressInNetworkOrder_U32[0], pIpAddress_X[1]);
    Sts_B = BofSocket::S_IsIpAddressEqual(true, false, nullptr, &pIpAddress_X[1]);
    ASSERT_FALSE(Sts_B);
    Sts_B = BofSocket::S_IsIpAddressEqual(true, false, &pIpAddress_X[0], nullptr);
    ASSERT_FALSE(Sts_B);
    Sts_B = BofSocket::S_IsIpAddressEqual(true, false, nullptr, nullptr);
    ASSERT_FALSE(Sts_B);
    Sts_B = BofSocket::S_IsIpAddressEqual(true, true, &pIpAddress_X[0], &pIpAddress_X[1]);
    ASSERT_FALSE(Sts_B);
    Sts_B = BofSocket::S_IsIpAddressEqual(true, false, &pIpAddress_X[0], &pIpAddress_X[1]);
    ASSERT_TRUE(Sts_B);
    Sts_B = BofSocket::S_IsIpAddressEqual(false, false, &pIpAddress_X[0], &pIpAddress_X[1]);
    ASSERT_TRUE(Sts_B);
    Sts_B = BofSocket::S_IsIpAddressEqual(false, false, &pIpAddress_X[0], &pIpAddress_X[1]);
    ASSERT_TRUE(Sts_B);
    Sts_B = BofSocket::S_IsIpAddressEqual(false, true, &pIpAddress_X[0], &pIpAddress_X[1]);
    ASSERT_FALSE(Sts_B);


    pIp_c = inet_ntoa(pIpAddress_X[1].sin_addr);
    Sts_i = memcmp(pIp_c, pIpAddress_c, strlen(pIp_c));
    ASSERT_EQ(Sts_i, 0);
    pIp_c = BofSocket::S_IpAddressToString(&pIpAddress_X[0],sizeof(pWork_c),pWork_c, true );
    Sts_i = memcmp(pIp_c, pIpAddress_c, strlen(pIp_c));
    ASSERT_EQ(Sts_i, 0);
    Sts_U32 = BofSocket::S_GetDefaultIpAddress(nullptr, nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);
    Sts_U32 = BofSocket::S_GetDefaultIpAddress(&HostIpAddress_X, nullptr);
    ASSERT_EQ(Sts_U32, 0);
    Sts_U32 = BofSocket::S_GetDefaultIpAddress(&HostIpAddress_X, pHostName_c);
    ASSERT_EQ(Sts_U32, 0);
    pIp_c = BofSocket::S_IpAddressToString(&HostIpAddress_X, sizeof(pWork_c), pWork_c, true);
    ASSERT_TRUE(pIp_c == pWork_c);
    BOF_PARSE_SOCKET_ADDRESS(HostIpAddress_X, pIpHost_U8[0], pIpHost_U8[1], pIpHost_U8[2], pIpHost_U8[3], PortHost_U16);
    ASSERT_NE(pIpHost_U8[0], 0);
    ASSERT_EQ(PortHost_U16, 0);

    Sts_U32 = BofSocket::S_GetIpAddressFromName(0, nullptr, &pIpAddress_X[0]);
    ASSERT_NE(Sts_U32, (uint32_t)0);
    Sts_U32 = BofSocket::S_GetIpAddressFromName(0, "", nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);
    Sts_U32 = BofSocket::S_GetIpAddressFromName(0, pHostName_c, &pIpAddress_X[0]);
    ASSERT_EQ(Sts_U32, 0);
    Sts_B = BofSocket::S_IsIpAddressEqual(true, false, &pIpAddress_X[0], &HostIpAddress_X);
    ASSERT_TRUE(Sts_B);
    Sts_B = BofSocket::S_IsIpAddressEqual(true, true, &pIpAddress_X[0], &HostIpAddress_X);
    ASSERT_TRUE(Sts_B);
    pIp_c = BofSocket::S_IpAddressToString(&HostIpAddress_X, sizeof(pIpAddress_c), pIpAddress_c, false);
    ASSERT_TRUE(pIp_c == pIpAddress_c);
    Sts_U32 = BofSocket::S_GetIpAddressFromName(0, pIpAddress_c, &pIpAddress_X[0]);
    ASSERT_EQ(Sts_U32, 0);
    Sts_U32 = BofSocket::S_GetIpParameter("", nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);
    Sts_U32 = BofSocket::S_GetIpParameter("", pSrcIpParam_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);
    Sts_U32 = BofSocket::S_GetIpParameter("Mtpc.10.129.170.30", pSrcIpParam_X);

#if defined( __APPLE__ )

    //MAC OS does not support the full socket ioctl->for now this is not supported on mac
    //	Sts_U32 = BofSocket::S_GetIpParameter("en0", pSrcIpParam_X);
    //	ASSERT_EQ(Sts_U32, 0);
#else
#if defined( __linux__ )
    Sts_U32 = BofSocket::S_GetIpParameter("eth0", pSrcIpParam_X);
    if (Sts_U32)
    {
      Sts_U32 = BofSocket::S_GetIpParameter("pc_lan_1", pSrcIpParam_X);
    }
    ASSERT_EQ(Sts_U32, 0);
#endif

    Sts_U32 = BofSocket::S_GetIpParameter(1, nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);
    Sts_U32 = BofSocket::S_GetIpParameter((uint32_t)0, pSrcIpParam_X);
    ASSERT_EQ(Sts_U32, 0);
    MaxIpParamEntry_U32 = sizeof(pSrcIpParam_X) / sizeof(pSrcIpParam_X[0]);
    Sts_U32 = BofSocket::S_GetIpParameterList(&MaxIpParamEntry_U32, nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);
    MaxIpParamEntry_U32 = sizeof(pSrcIpParam_X) / sizeof(pSrcIpParam_X[0]);
    Sts_U32 = BofSocket::S_GetIpParameterList(&MaxIpParamEntry_U32, pSrcIpParam_X);
    ASSERT_EQ(Sts_U32, 0);

    Sts_U32 = BofSocket::S_SetIpParameter(nullptr, pDstIpParam_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);
    Sts_U32 = BofSocket::S_SetIpParameter(pSrcIpParam_X, nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);
#if defined( __linux__ ) || defined( __APPLE__ )
    pSrcIpParam_X[0] = pSrcIpParam_X[1];                //Entry 0 is interface lo on linux 1 is used for application and 2 is for debug->do not change debug link
#else
#endif
    pDstIpParam_X[0] = pSrcIpParam_X[0];
    BOF_PARSE_SOCKET_ADDRESS(pDstIpParam_X[0].IpAddress_X, pIp_U8[1][0], pIp_U8[1][1], pIp_U8[1][2], pIp_U8[1][3], pPort_U16[1]);
    BOF_SET_SOCKET_ADDRESS(pDstIpParam_X[0].IpAddress_X, pIp_U8[1][0], pIp_U8[1][1], pIp_U8[1][2], pIp_U8[1][3] + 6, pPort_U16[1])

      //Pas sur tc	Sts_U32 = BofSocket::S_SetIpParameter(pSrcIpParam_X, pDstIpParam_X);
      //	ASSERT_EQ(Sts_U32, 0);

      //pSrcIpParam_X[0] = pDstIpParam_X[0];		//Change done
      BOF_SET_SOCKET_ADDRESS(pDstIpParam_X[0].IpAddress_X, pIp_U8[1][0], pIp_U8[1][1], pIp_U8[1][2], pIp_U8[1][3], pPort_U16[1])

      //Pas sur tc	Sts_U32 = BofSocket::S_SetIpParameter(pSrcIpParam_X, pDstIpParam_X);
      //Pas sur tc ASSERT_EQ(Sts_U32, 0);
#endif

      // www.google.com 216.58.211.68
      // www.yahoo.com 46.228.47.114/115
      // www.msn.com 204.79.197.203
    Sts_i = BofSocket::S_HostnameToIpAddress("a.b.c.d", 0, &NbIpAddress_U32, &HostIpAddress_X);
    ASSERT_NE(Sts_i, 0);
    Sts_i = BofSocket::S_HostnameToIpAddress(nullptr, 0, &NbIpAddress_U32, &HostIpAddress_X);
    ASSERT_NE(Sts_i, 0);
    Sts_i = BofSocket::S_HostnameToIpAddress("1.2.3.4", 0, &NbIpAddress_U32, nullptr);
    ASSERT_NE(Sts_i, 0);
    Sts_i = BofSocket::S_HostnameToIpAddress("1.2.3.4", 0, &NbIpAddress_U32, &HostIpAddress_X);
    ASSERT_EQ(Sts_i, 0);
    Sts_i = BofSocket::S_HostnameToIpAddress("1.2.3.4:", 0, &NbIpAddress_U32, &HostIpAddress_X);
    ASSERT_EQ(Sts_i, 0);
    Sts_i = BofSocket::S_HostnameToIpAddress("1.2.3.4:5", 0, &NbIpAddress_U32, &HostIpAddress_X);
    ASSERT_EQ(Sts_i, 0);
    pIp_c = BofSocket::S_IpAddressToString(&HostIpAddress_X, sizeof(pHostName_c), pHostName_c, true);
    ASSERT_TRUE(pIp_c != nullptr);
#if 0 //need internet
    Sts_i = BofSocket::S_HostnameToIpAddress("www.google.com:1", 0, &NbIpAddress_U32, &HostIpAddress_X);
    ASSERT_EQ(Sts_i, 0);
    pIp_c = BofSocket::S_IpAddressToString(&HostIpAddress_X, sizeof(pHostName_c), pHostName_c, true);
    ASSERT_TRUE(pIp_c != nullptr);
    Sts_i = BofSocket::S_HostnameToIpAddress("www.yahoo.com:2124", 0, &NbIpAddress_U32, &HostIpAddress_X);
    ASSERT_EQ(Sts_i, 0);
    pIp_c = BofSocket::S_IpAddressToString(&HostIpAddress_X, sizeof(pHostName_c), pHostName_c, true);
    ASSERT_TRUE(pIp_c != nullptr);
    Sts_i = BofSocket::S_HostnameToIpAddress("www.msn.com", 0, &NbIpAddress_U32, &HostIpAddress_X);
    ASSERT_EQ(Sts_i, 0);
    pIp_c = BofSocket::S_IpAddressToString(&HostIpAddress_X, sizeof(pHostName_c), pHostName_c, true);
    ASSERT_TRUE(pIp_c != nullptr);
#endif
  }
  
  TEST_F(BofStd_Test, CheckTextFile)
  {
    uint32_t  Sts_U32, Nb_U32, Len_U32, NumberMaxOfBytesToRead_U32, NumberOfBytesRead_U32;
    bool Sts_B, FileLocked_B;

    char   pCrtDir_c[128], pFn_c[128], pWork_c[128], pRead_c[128];
    int    i;
    void * Io_h;
    
    Bof_RemoveDirectory("test", true);

    Sts_U32 = Bof_GetCurrentDirectory(sizeof(pCrtDir_c), pCrtDir_c);
    ASSERT_EQ(Sts_U32, 0);

    Sts_U32 = Bof_CreateDirectory("test", 0777);
    ASSERT_EQ(Sts_U32, 0);
    //Creating twice the same dir is ok
    Sts_U32 = Bof_CreateDirectory("test", 0777);
    ASSERT_EQ(Sts_U32, 0);

    sprintf(pFn_c, "%s%ctest%cTestBha.bha", pCrtDir_c, Bof_FilenameSeparator(), Bof_FilenameSeparator());
    Io_h = Bof_OpenFile(pFn_c, false, (S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH), &FileLocked_B);
    ASSERT_NE(Io_h, BOF_INVALID_HANDLE_VALUE);
    ASSERT_FALSE(FileLocked_B);

    for (i = 0; i < 10; i++)
    {
      sprintf(pWork_c, "ABCDEFGHIJKLMNOPQRSTUVWXYZsazertyuiopqsdfghjklmwxcvbn%s", Eol);
      Len_U32 = strlen(pWork_c) - i;
      Sts_U32 = Bof_WriteFile(Io_h, &pWork_c[i], Len_U32, &Nb_U32);
      ASSERT_EQ(Sts_U32, 0);
      ASSERT_EQ(Len_U32, Nb_U32);
    }
    Sts_U32 = Bof_CloseFile(Io_h);
    ASSERT_EQ(Sts_U32, 0);

    Io_h = Bof_OpenFile(pFn_c, true, 0, &FileLocked_B);
    ASSERT_NE(Io_h, BOF_INVALID_HANDLE_VALUE);
    ASSERT_FALSE(FileLocked_B);

    for (i = 0; i < 10; i++)
    {
      NumberMaxOfBytesToRead_U32 = sizeof(pRead_c);
      Sts_U32 = Bof_ReadTextLine(Io_h, pRead_c, NumberMaxOfBytesToRead_U32, &NumberOfBytesRead_U32);
      ASSERT_EQ(Sts_U32, 0);
      Len_U32 = strlen(pWork_c) - i;
      ASSERT_EQ(NumberOfBytesRead_U32, Len_U32);
      Sts_B = !strncmp(pRead_c, &pWork_c[i], Len_U32);
      ASSERT_TRUE(Sts_B);
    }
    Sts_U32 = Bof_CloseFile(Io_h);
    ASSERT_EQ(Sts_U32, 0);

    Io_h = Bof_OpenFile(pFn_c, true, 0, &FileLocked_B);
    ASSERT_NE(Io_h, BOF_INVALID_HANDLE_VALUE);
    ASSERT_FALSE(FileLocked_B);

    for (i = 0; i < 6; i++)
    {
      NumberMaxOfBytesToRead_U32 = 10;
      Sts_U32 = Bof_ReadTextLine(Io_h, pRead_c, NumberMaxOfBytesToRead_U32, &NumberOfBytesRead_U32);
      ASSERT_EQ(Sts_U32, 0);
      ASSERT_EQ(NumberOfBytesRead_U32, 9);
      Sts_B = !strncmp(pRead_c, &pWork_c[i * 9], 9);
      ASSERT_TRUE(Sts_B);
    }
    Sts_U32 = Bof_CloseFile(Io_h);
    ASSERT_EQ(Sts_U32, 0);

    Sts_U32 = Bof_RemoveDirectory("test", true);
    ASSERT_EQ(Sts_U32, 0);
  }

  TEST_F(BofStd_Test, CheckBof_TickDelta)
  {
    uint32_t Timer_U32, Delta_U32;

    BOF_TICKCOUNT(Timer_U32);

    do
    {
      BOF_TICKDELTA(Timer_U32, Delta_U32);
    } while (Delta_U32 < 1000);
    ASSERT_LT(Delta_U32, (uint32_t)1050);
    ASSERT_GE(Delta_U32, (uint32_t)1000);
  }

  static const char *S_pMonth_c[13] = { "None", "Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sept", "Oct", "Nov", "Dec" };
  TEST_F(BofStd_Test, CheckBof_GetMonthName)
  {
    uint32_t  Sts_U32;
    char pMonthName_c[16];
    int  i;
    bool Sts_B;

    Sts_U32 = Bof_GetMonthName(0, pMonthName_c);
    ASSERT_NE(Sts_U32, (uint32_t)0);
    Sts_U32 = Bof_GetMonthName(13, pMonthName_c);
    ASSERT_NE(Sts_U32, (uint32_t)0);
    Sts_U32 = Bof_GetMonthName(1, nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    for (i = 1; i < 13; i++)
    {
      Sts_U32 = Bof_GetMonthName(static_cast<uint8_t>(i), pMonthName_c);
      ASSERT_EQ(Sts_U32, 0);
      Sts_B = !strcmp(pMonthName_c, S_pMonth_c[i]);
      ASSERT_TRUE(Sts_B);
    }
  }

  TEST_F(BofStd_Test, CheckValidateDateTime)
  {
    uint32_t         Sts_U32;
    BOFDATETIME DateTime_X;

    memset(&DateTime_X, 0, sizeof(DateTime_X));

    //BAD
    Sts_U32 = Bof_ValidateDateTime(nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    //GOOD
    DateTime_X.Day_U8 = 1;
    DateTime_X.Month_U8 = 1;
    DateTime_X.Year_U16 = 1970;
    DateTime_X.DayOfWeek_U8 = 0;
    DateTime_X.Hour_U8 = 0;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    //DAY
    DateTime_X.Day_U8 = 0;
    DateTime_X.Month_U8 = 1;
    DateTime_X.Year_U16 = 1970;
    DateTime_X.DayOfWeek_U8 = 0;
    DateTime_X.Hour_U8 = 0;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Day_U8 = 32;
    DateTime_X.Month_U8 = 1;
    DateTime_X.Year_U16 = 1970;
    DateTime_X.DayOfWeek_U8 = 0;
    DateTime_X.Hour_U8 = 0;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 1;
    DateTime_X.Year_U16 = 1970;
    DateTime_X.DayOfWeek_U8 = 0;
    DateTime_X.Hour_U8 = 0;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    //MONTH
    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 0;
    DateTime_X.Year_U16 = 1970;
    DateTime_X.DayOfWeek_U8 = 0;
    DateTime_X.Hour_U8 = 0;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 13;
    DateTime_X.Year_U16 = 1970;
    DateTime_X.DayOfWeek_U8 = 0;
    DateTime_X.Hour_U8 = 0;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 1970;
    DateTime_X.DayOfWeek_U8 = 0;
    DateTime_X.Hour_U8 = 0;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    //YEAR
    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 1969;
    DateTime_X.DayOfWeek_U8 = 0;
    DateTime_X.Hour_U8 = 0;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2071;
    DateTime_X.DayOfWeek_U8 = 0;
    DateTime_X.Hour_U8 = 0;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2070;
    DateTime_X.DayOfWeek_U8 = 0;
    DateTime_X.Hour_U8 = 0;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    //DAYOFWEEK
    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2070;
    DateTime_X.DayOfWeek_U8 = 0xFF;
    DateTime_X.Hour_U8 = 0;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2070;
    DateTime_X.DayOfWeek_U8 = 7;
    DateTime_X.Hour_U8 = 0;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2070;
    DateTime_X.DayOfWeek_U8 = 6;
    DateTime_X.Hour_U8 = 0;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    //HOUR
    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2070;
    DateTime_X.DayOfWeek_U8 = 6;
    DateTime_X.Hour_U8 = 0xFF;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2070;
    DateTime_X.DayOfWeek_U8 = 6;
    DateTime_X.Hour_U8 = 24;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2070;
    DateTime_X.DayOfWeek_U8 = 6;
    DateTime_X.Hour_U8 = 23;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    //MINUTE
    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2070;
    DateTime_X.DayOfWeek_U8 = 6;
    DateTime_X.Hour_U8 = 23;
    DateTime_X.Minute_U8 = 0xFF;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2070;
    DateTime_X.DayOfWeek_U8 = 6;
    DateTime_X.Hour_U8 = 23;
    DateTime_X.Minute_U8 = 60;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2070;
    DateTime_X.DayOfWeek_U8 = 6;
    DateTime_X.Hour_U8 = 23;
    DateTime_X.Minute_U8 = 59;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    //SECOND
    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2070;
    DateTime_X.DayOfWeek_U8 = 6;
    DateTime_X.Hour_U8 = 23;
    DateTime_X.Minute_U8 = 59;
    DateTime_X.Second_U8 = 0xFF;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2070;
    DateTime_X.DayOfWeek_U8 = 6;
    DateTime_X.Hour_U8 = 23;
    DateTime_X.Minute_U8 = 59;
    DateTime_X.Second_U8 = 60;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2070;
    DateTime_X.DayOfWeek_U8 = 6;
    DateTime_X.Hour_U8 = 23;
    DateTime_X.Minute_U8 = 59;
    DateTime_X.Second_U8 = 59;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    //MILLISECOND
    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2070;
    DateTime_X.DayOfWeek_U8 = 6;
    DateTime_X.Hour_U8 = 23;
    DateTime_X.Minute_U8 = 59;
    DateTime_X.Second_U8 = 59;
    DateTime_X.Millisecond_U16 = 0xFFFF;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2070;
    DateTime_X.DayOfWeek_U8 = 6;
    DateTime_X.Hour_U8 = 23;
    DateTime_X.Minute_U8 = 59;
    DateTime_X.Second_U8 = 59;
    DateTime_X.Millisecond_U16 = 1000;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Day_U8 = 31;
    DateTime_X.Month_U8 = 12;
    DateTime_X.Year_U16 = 2070;
    DateTime_X.DayOfWeek_U8 = 6;
    DateTime_X.Hour_U8 = 23;
    DateTime_X.Minute_U8 = 59;
    DateTime_X.Second_U8 = 59;
    DateTime_X.Millisecond_U16 = 999;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    //LEAP
    DateTime_X.Day_U8 = 28;
    DateTime_X.Month_U8 = 2;
    DateTime_X.Year_U16 = 2000;
    DateTime_X.DayOfWeek_U8 = 0;
    DateTime_X.Hour_U8 = 0;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);

    ASSERT_EQ(Sts_U32, 0);
    DateTime_X.Day_U8 = 29;
    DateTime_X.Month_U8 = 2;
    DateTime_X.Year_U16 = 2000;
    DateTime_X.DayOfWeek_U8 = 0;
    DateTime_X.Hour_U8 = 0;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    DateTime_X.Day_U8 = 30;
    DateTime_X.Month_U8 = 2;
    DateTime_X.Year_U16 = 2000;
    DateTime_X.DayOfWeek_U8 = 0;
    DateTime_X.Hour_U8 = 0;
    DateTime_X.Minute_U8 = 0;
    DateTime_X.Second_U8 = 0;
    DateTime_X.Millisecond_U16 = 0;
    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);
  }

  TEST_F(BofStd_Test, CheckIsLeapYear)
  {
    bool Sts_B;

    Sts_B = Bof_IsLeapYear(1900);
    ASSERT_FALSE(Sts_B);

    Sts_B = Bof_IsLeapYear(2000);
    ASSERT_TRUE(Sts_B);

    Sts_B = Bof_IsLeapYear(2008);
    ASSERT_TRUE(Sts_B);
  }

  TEST_F(BofStd_Test, CheckGetDateTime)
  {
    uint32_t         Sts_U32;
    BOFDATETIME DateTime_X;

    /*
    * EXTERN_C uint32_t Bof_GetDateTime(BOFDATETIME *_pDateTime_X);
    * EXTERN_C uint32_t Bof_DiffDateTime(BOFDATETIME *_pFirstDateTime_X, BOFDATETIME *_pSecondDateTime_X, BOFDATETIME *_pDiffTime_X, uint32_t *_pDiffDay_U32);
    */

    memset(&DateTime_X, 0, sizeof(DateTime_X));

    //BAD
    Sts_U32 = Bof_GetDateTime(nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_GetDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    Sts_U32 = Bof_ValidateDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);
  }

  TEST_F(BofStd_Test, CheckSetDateTime)
  {
    uint32_t         Sts_U32;
    BOFDATETIME DateTime_X, DateTimeOrg_X;

    Sts_U32 = Bof_GetDateTime(&DateTimeOrg_X);
    ASSERT_EQ(Sts_U32, 0);

    DateTime_X = DateTimeOrg_X;


#if 0     //Disable setdate time to check if it can explain teamcity hangout
    DateTime_X.Day_U8 = 26;
    Sts_U32 = Bof_SetDateTime(&DateTime_X);
#if defined( __linux__ ) || defined( __APPLE__ )

    //  //The calling process has insufficient privilege to call settimeofday(); under Linux the CAP_SYS_TIME capability is required.
#else
#endif
    ASSERT_EQ(Sts_U32, 0);

    DateTime_X.Month_U8 = 13;
    Sts_U32 = Bof_SetDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Month_U8 = 12;
    Sts_U32 = Bof_SetDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    DateTime_X.Year_U16 = 12000;
    Sts_U32 = Bof_SetDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Year_U16 = 2014;
    Sts_U32 = Bof_SetDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    DateTime_X.Hour_U8 = 24;
    Sts_U32 = Bof_SetDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Hour_U8 = 23;
    Sts_U32 = Bof_SetDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    DateTime_X.Minute_U8 = 60;
    Sts_U32 = Bof_SetDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Minute_U8 = 59;
    Sts_U32 = Bof_SetDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    DateTime_X.Second_U8 = 60;
    Sts_U32 = Bof_SetDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Second_U8 = 59;
    Sts_U32 = Bof_SetDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    DateTime_X.Millisecond_U16 = 1000;
    Sts_U32 = Bof_SetDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    DateTime_X.Millisecond_U16 = 999;
    Sts_U32 = Bof_SetDateTime(&DateTime_X);
    ASSERT_EQ(Sts_U32, 0);

    DateTime_X.Day_U8 = 32;
    Sts_U32 = Bof_SetDateTime(&DateTime_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);


    Bof_Sleep(2000);     //We setup time to 23:59:59->wait to pass day
    Sts_U32 = Bof_GetDateTime(&DateTimeNew_X);
    ASSERT_EQ(Sts_U32, 0);
    ASSERT_EQ(DateTimeNew_X.Day_U8, 26 + 1);     //We setup time to 23:59:59->wait to pass day

    ASSERT_EQ(DateTimeNew_X.Month_U8, 12);
    ASSERT_EQ(DateTimeNew_X.Year_U16, 2014);

    //  ASSERT_EQ(DateTimeNew_X.DayOfWeek_U8, DateTime_X.DayOfWeek_U8);
    ASSERT_EQ(DateTimeNew_X.Hour_U8, 0);     //We setup time to 23:59:59->wait to pass day
    ASSERT_EQ(DateTimeNew_X.Minute_U8, 0);
    ASSERT_LT(DateTimeNew_X.Second_U8, 4);

    //  ASSERT_EQ(DateTimeNew_X.Millisecond_U16, DateTime_X.Millisecond_U16);

    Sts_U32 = Bof_SetDateTime(&DateTimeOrg_X);
    ASSERT_EQ(Sts_U32, 0);
#endif
  }
  TEST_F(BofStd_Test, CheckDiffDateTime)
  {
    uint32_t         Sts_U32, DiffDay_U32, DayNumber_U32;
    BOFDATETIME FirstDateTime_X, SecondDateTime_X, DiffTime_X;

    memset(&FirstDateTime_X, 0, sizeof(FirstDateTime_X));
    Sts_U32 = Bof_DateTimeToNumber(nullptr, &DayNumber_U32);
    ASSERT_NE(Sts_U32, (uint32_t)0);
    Sts_U32 = Bof_DateTimeToNumber(&FirstDateTime_X, nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);
    Sts_U32 = Bof_DateTimeToNumber(&FirstDateTime_X, &DayNumber_U32);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_DateTimeToNumber(&FirstDateTime_X, &DayNumber_U32);
    ASSERT_NE(Sts_U32, (uint32_t)0);
    FirstDateTime_X.Day_U8 = 26;
    FirstDateTime_X.Month_U8 = 5;
    FirstDateTime_X.Year_U16 = 2000;
    Sts_U32 = Bof_DateTimeToNumber(&FirstDateTime_X, &DayNumber_U32);
    ASSERT_EQ(Sts_U32, 0);
    ASSERT_EQ(DayNumber_U32, 730571);

    memset(&FirstDateTime_X, 0, sizeof(FirstDateTime_X));
    memset(&SecondDateTime_X, 0, sizeof(SecondDateTime_X));
    memset(&DiffTime_X, 0, sizeof(DiffTime_X));

    Sts_U32 = Bof_DiffDateTime(nullptr, &SecondDateTime_X, &DiffTime_X, &DiffDay_U32);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_DiffDateTime(&FirstDateTime_X, nullptr, &DiffTime_X, &DiffDay_U32);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_DiffDateTime(&FirstDateTime_X, &SecondDateTime_X, nullptr, &DiffDay_U32);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_DiffDateTime(&FirstDateTime_X, &SecondDateTime_X, &DiffTime_X, nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_DiffDateTime(&FirstDateTime_X, &SecondDateTime_X, &DiffTime_X, &DiffDay_U32);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    FirstDateTime_X.Day_U8 = 18;
    FirstDateTime_X.Month_U8 = 1;
    FirstDateTime_X.Year_U16 = 2000;
    FirstDateTime_X.Hour_U8 = 1;
    FirstDateTime_X.Minute_U8 = 30;
    FirstDateTime_X.Second_U8 = 20;
    FirstDateTime_X.Millisecond_U16 = 0;

    SecondDateTime_X = FirstDateTime_X;
    Sts_U32 = Bof_DiffDateTime(&FirstDateTime_X, &SecondDateTime_X, &DiffTime_X, &DiffDay_U32);
    ASSERT_EQ(Sts_U32, 0);
    ASSERT_EQ(DiffDay_U32, 0);
    ASSERT_EQ(DiffTime_X.Hour_U8, 0);
    ASSERT_EQ(DiffTime_X.Minute_U8, 0);
    ASSERT_EQ(DiffTime_X.Second_U8, 0);

    SecondDateTime_X.Year_U16 = FirstDateTime_X.Year_U16 + 1;
    Sts_U32 = Bof_DiffDateTime(&FirstDateTime_X, &SecondDateTime_X, &DiffTime_X, &DiffDay_U32);
    ASSERT_EQ(Sts_U32, 0);
    ASSERT_EQ(DiffDay_U32, 366);
    ASSERT_EQ(DiffTime_X.Hour_U8, 0);
    ASSERT_EQ(DiffTime_X.Minute_U8, 0);
    ASSERT_EQ(DiffTime_X.Second_U8, 0);

    SecondDateTime_X.Month_U8 = FirstDateTime_X.Month_U8 + 1;
    Sts_U32 = Bof_DiffDateTime(&FirstDateTime_X, &SecondDateTime_X, &DiffTime_X, &DiffDay_U32);
    ASSERT_EQ(Sts_U32, 0);
    ASSERT_EQ(DiffDay_U32, 366 + 31);
    ASSERT_EQ(DiffTime_X.Hour_U8, 0);
    ASSERT_EQ(DiffTime_X.Minute_U8, 0);
    ASSERT_EQ(DiffTime_X.Second_U8, 0);

    SecondDateTime_X.Day_U8 = FirstDateTime_X.Day_U8 + 7;
    Sts_U32 = Bof_DiffDateTime(&FirstDateTime_X, &SecondDateTime_X, &DiffTime_X, &DiffDay_U32);
    ASSERT_EQ(Sts_U32, 0);
    ASSERT_EQ(DiffDay_U32, 366 + 31 + 7);
    ASSERT_EQ(DiffTime_X.Hour_U8, 0);
    ASSERT_EQ(DiffTime_X.Minute_U8, 0);
    ASSERT_EQ(DiffTime_X.Second_U8, 0);

    FirstDateTime_X.Hour_U8 = 0;
    FirstDateTime_X.Minute_U8 = 0;
    FirstDateTime_X.Second_U8 = 0;
    FirstDateTime_X.Millisecond_U16 = 0;
    SecondDateTime_X = FirstDateTime_X;
    SecondDateTime_X.Second_U8 = 1;
    Sts_U32 = Bof_DiffDateTime(&FirstDateTime_X, &SecondDateTime_X, &DiffTime_X, &DiffDay_U32);
    ASSERT_EQ(Sts_U32, 0);
    ASSERT_EQ(DiffDay_U32, 0);
    ASSERT_EQ(DiffTime_X.Hour_U8, 0);
    ASSERT_EQ(DiffTime_X.Minute_U8, 0);
    ASSERT_EQ(DiffTime_X.Second_U8, 1);

    SecondDateTime_X.Minute_U8 = 2;
    Sts_U32 = Bof_DiffDateTime(&FirstDateTime_X, &SecondDateTime_X, &DiffTime_X, &DiffDay_U32);
    ASSERT_EQ(Sts_U32, 0);
    ASSERT_EQ(DiffDay_U32, 0);
    ASSERT_EQ(DiffTime_X.Hour_U8, 0);
    ASSERT_EQ(DiffTime_X.Minute_U8, 2);
    ASSERT_EQ(DiffTime_X.Second_U8, 1);

    SecondDateTime_X.Hour_U8 = 3;
    Sts_U32 = Bof_DiffDateTime(&FirstDateTime_X, &SecondDateTime_X, &DiffTime_X, &DiffDay_U32);
    ASSERT_EQ(Sts_U32, 0);
    ASSERT_EQ(DiffDay_U32, 0);
    ASSERT_EQ(DiffTime_X.Hour_U8, 3);
    ASSERT_EQ(DiffTime_X.Minute_U8, 2);
    ASSERT_EQ(DiffTime_X.Second_U8, 1);
  }


  TEST_F(BofStd_Test, TickSleep)
  {

    BOFDATETIME FirstDateTime_X, SecondDateTime_X, DiffTime_X;
    uint32_t         DiffDay_U32, Sts_U32, Start_U32, Delta_U32;

    Start_U32 = Bof_GetTickCount();
    Sts_U32 = Bof_GetDateTime(&FirstDateTime_X);
    ASSERT_EQ(Sts_U32, 0);
    Bof_Sleep(3000);
    /*
    uint32_t End_U32 = Bof_GetTickCount();
    uint32_t k = End_U32 - Start_U32;

    Start_U32 = 0xf7a0b107;
    End_U32 = 0x6d4;


    uint32_t now = 0x6d4;
    uint32_t start = 0xf7a0b107;

    uint32_t delta = 0xFFFFFFFF - start;
    delta = (now < start) ? 0xFFFFFFFF - start - now : now - start;
    */
    BOF_TICKDELTA(Start_U32, Delta_U32)
      Sts_U32 = Bof_GetDateTime(&SecondDateTime_X);
    ASSERT_EQ(Sts_U32, 0);
    Sts_U32 = Bof_DiffDateTime(&FirstDateTime_X, &SecondDateTime_X, &DiffTime_X, &DiffDay_U32);
    ASSERT_EQ(Sts_U32, 0);
    ASSERT_EQ(DiffDay_U32, 0);
    ASSERT_EQ(DiffTime_X.Hour_U8, 0);
    ASSERT_EQ(DiffTime_X.Minute_U8, 0);
    ASSERT_GE(DiffTime_X.Second_U8, 3);
    ASSERT_LE(DiffTime_X.Second_U8, 4);
    ASSERT_EQ(DiffTime_X.Millisecond_U16, 0);
    ASSERT_GT(Delta_U32, (uint32_t)2990);
    ASSERT_LT(Delta_U32, (uint32_t)3010);

    Start_U32 = Bof_GetTickCount();
    Sts_U32 = Bof_GetDateTime(&FirstDateTime_X);
    ASSERT_EQ(Sts_U32, 0);
    Bof_Sleep(100);
    BOF_TICKDELTA(Start_U32, Delta_U32)
      Sts_U32 = Bof_GetDateTime(&SecondDateTime_X);
    ASSERT_EQ(Sts_U32, 0);
    Sts_U32 = Bof_DiffDateTime(&FirstDateTime_X, &SecondDateTime_X, &DiffTime_X, &DiffDay_U32);
    ASSERT_EQ(Sts_U32, 0);
    ASSERT_EQ(DiffDay_U32, 0);
    ASSERT_EQ(DiffTime_X.Hour_U8, 0);
    ASSERT_EQ(DiffTime_X.Minute_U8, 0);
    //can be one	
    ASSERT_LE(DiffTime_X.Second_U8, 1);
    ASSERT_EQ(DiffTime_X.Millisecond_U16, 0);
    ASSERT_GE(Delta_U32, (uint32_t)90);
    ASSERT_LE(Delta_U32, (uint32_t)110);

    Start_U32 = Bof_GetTickCount();
    Sts_U32 = Bof_GetDateTime(&FirstDateTime_X);
    ASSERT_EQ(Sts_U32, 0);
    Bof_Sleep(30);      //Sleep can't be lower than the timer res (on some system it is 15 ms)
    BOF_TICKDELTA(Start_U32, Delta_U32)
      Sts_U32 = Bof_GetDateTime(&SecondDateTime_X);
    ASSERT_EQ(Sts_U32, 0);
    Sts_U32 = Bof_DiffDateTime(&FirstDateTime_X, &SecondDateTime_X, &DiffTime_X, &DiffDay_U32);
    ASSERT_EQ(Sts_U32, 0);
    ASSERT_EQ(DiffDay_U32, 0);
    ASSERT_EQ(DiffTime_X.Hour_U8, 0);
    ASSERT_EQ(DiffTime_X.Minute_U8, 0);
    //can be one	
    ASSERT_LE(DiffTime_X.Second_U8, 1);
    ASSERT_EQ(DiffTime_X.Millisecond_U16, 0);
    ASSERT_GE(Delta_U32, (uint32_t)15);
    ASSERT_LE(Delta_U32, (uint32_t)45);

  }

  int CheckSplit(char *_pLine_c, int _Nb_i, char **ppListOfString_c)
  {
    int i, Rts_i = 0;
    char c = 'a';

    if ((_pLine_c[0] != c) || (_pLine_c[1] != 0) || (_pLine_c != ppListOfString_c[0]))
    {
      Rts_i = -1;
    }
    if (Rts_i == 0)
    {
      for (i = 0; i < _Nb_i; i++)
      {
        if ((ppListOfString_c[i][0] != c) || (ppListOfString_c[i][1] != 0))
        {
          Rts_i = -2;
          break;
        }
        c++;
      }
    }
    return(Rts_i);
  }

  TEST_F(BofStd_Test, CheckFileFunction)
  {
    uint32_t    Sts_U32, Nb_U32, Len_U32;
    bool   FileLocked_B;
    char   pCrtDir_c[128], pFirstDir_c[128], pWork_c[128], pWork2_c[128], pFn_c[128];
    int    i;
    void * Io_h;


    /* should check on android/mac/...*/
    /*
    char pOutput_c[0x4000];
    uint32_t Size_U32;
    int Len_i;

    Size_U32 = sizeof(pOutput_c);
    pOutput_c[0] = 0;
    Sts_U32 = Bof_SpawnAndGetOutputStream("lbhkjh", &Size_U32, pOutput_c);
    ASSERT_EQ(Sts_U32, 0x7F);
    ASSERT_EQ(Size_U32, 0);
    Size_U32 = sizeof(pOutput_c);
    Sts_U32 = Bof_SpawnAndGetOutputStream("ls /llkjk", &Size_U32, pOutput_c);
    ASSERT_EQ(Sts_U32, 0x01);
    ASSERT_EQ(Size_U32, 0);
    Size_U32 = sizeof(pOutput_c);
    Sts_U32 = Bof_SpawnAndGetOutputStream("ls", &Size_U32, pOutput_c);
    Len_i = strlen(pOutput_c);
    ASSERT_EQ(Sts_U32, 0x00);
    ASSERT_NE(Size_U32, 0);
    ASSERT_EQ(Size_U32, Len_i);
    Size_U32 = sizeof(pOutput_c);
    Sts_U32 = Bof_SpawnAndGetOutputStream("ls -alR /", &Size_U32, pOutput_c);
    Len_i = strlen(pOutput_c);
 //   printf("%s", pOutput_c);
    ASSERT_EQ(Sts_U32, 0x00);
    ASSERT_NE(Size_U32, 0);
    ASSERT_EQ(Size_U32, Len_i);
    Sts_U32 = Bof_SpawnAndGetOutputStream("ls -al /dev", &Size_U32, pOutput_c);
    Len_i = strlen(pOutput_c);
 //   printf("%s", pOutput_c);
    ASSERT_EQ(Sts_U32, 0x00);
    ASSERT_NE(Size_U32, 0);
    ASSERT_EQ(Size_U32, Len_i);
*/
    Sts_U32 = Bof_GetCurrentDirectory(0, pFirstDir_c);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_GetCurrentDirectory(sizeof(pFirstDir_c), nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_GetCurrentDirectory(sizeof(pFirstDir_c), pFirstDir_c);
    ASSERT_EQ(Sts_U32, 0);

    Sts_U32 = Bof_CreateDirectory(nullptr, 0777);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_RemoveDirectory("test", false);
    Sts_U32 = Bof_CreateDirectory("test", 0777);
    ASSERT_EQ(Sts_U32, 0);
    Sts_U32 = Bof_CreateDirectory("test/a", 0777);
    ASSERT_EQ(Sts_U32, 0);
    Sts_U32 = Bof_CreateDirectory("test/a/b", 0777);
    ASSERT_EQ(Sts_U32, 0);

    Sts_U32 = Bof_RemoveDirectory("test", true);
    ASSERT_EQ(Sts_U32, 0);

    Sts_U32 = Bof_CreateDirectory("test", 0777);
    ASSERT_EQ(Sts_U32, 0);



    Sts_U32 = Bof_ChangeCurrentDirectory("testbabar");
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_ChangeCurrentDirectory(nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_ChangeCurrentDirectory("test");
    ASSERT_EQ(Sts_U32, 0);

    Sts_U32 = Bof_GetCurrentDirectory(sizeof(pCrtDir_c), pCrtDir_c);
    ASSERT_EQ(Sts_U32, 0);

    sprintf(pWork_c, "%s%c%s", pFirstDir_c, Bof_FilenameSeparator(), "test");
    ASSERT_STREQ(pWork_c, pCrtDir_c);

    for (i = 0; i < 10; i++)
    {
      sprintf(pFn_c, "Test%03d.bha", i);
      Io_h = Bof_OpenFile(pFn_c, false, (S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH), &FileLocked_B);
      ASSERT_NE(Io_h, BOF_INVALID_HANDLE_VALUE);
      ASSERT_FALSE(FileLocked_B);
      sprintf(pWork_c, "ABCDEFGHIJKLMNOPQRSTUVWXYZ%sazertyuiopqsdfghjklmwxcvbn", Eol);
      Len_U32 = strlen(pWork_c) + 1;
      Sts_U32 = Bof_WriteFile(Io_h, pWork_c, Len_U32, &Nb_U32);
      ASSERT_EQ(Sts_U32, 0);
      ASSERT_EQ(Len_U32, Nb_U32);

      //on linux this work but not on windows !   Sts_U32 = Bof_DeleteFile(pFn_c);
      //   ASSERT_NE(Sts_U32, (uint32_t)0);

      Sts_U32 = Bof_CloseFile(Io_h);
      ASSERT_EQ(Sts_U32, 0);
    }

    Sts_U32 = Bof_ChangeCurrentDirectory("..");
    ASSERT_EQ(Sts_U32, 0);

    Sts_U32 = Bof_GetCurrentDirectory(sizeof(pCrtDir_c), pCrtDir_c);
    ASSERT_EQ(Sts_U32, 0);
    ASSERT_STREQ(pFirstDir_c, pCrtDir_c);

    Sts_U32 = Bof_RemoveDirectory(nullptr, false);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_RemoveDirectory("testbabar", false);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_RemoveDirectory("test", false);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    for (i = 0; i < 10; i++)
    {
      sprintf(pFn_c, "test%cTest%03d.bha", Bof_FilenameSeparator(), i);

      Io_h = Bof_OpenFile(pFn_c, true, 0, &FileLocked_B);
      ASSERT_NE(Io_h, BOF_INVALID_HANDLE_VALUE);
      ASSERT_FALSE(FileLocked_B);
      sprintf(pWork_c, "ABCDEFGHIJKLMNOPQRSTUVWXYZ%sazertyuiopqsdfghjklmwxcvbn", Eol);
      Len_U32 = strlen(pWork_c) + 1;
      Sts_U32 = Bof_ReadFile(Io_h, pWork2_c, Len_U32, &Nb_U32);
      ASSERT_EQ(Sts_U32, 0);
      ASSERT_EQ(Len_U32, Nb_U32);
      ASSERT_STREQ(pWork2_c, pWork_c);
      Sts_U32 = Bof_CloseFile(Io_h);
      ASSERT_EQ(Sts_U32, 0);


      Sts_U32 = Bof_DeleteFile(pFn_c);
      ASSERT_EQ(Sts_U32, 0);
    }

    Sts_U32 = Bof_RemoveDirectory("test", false);
    ASSERT_EQ(Sts_U32, 0);

    Sts_U32 = Bof_ChangeCurrentDirectory("test");
    ASSERT_NE(Sts_U32, (uint32_t)0);
  }
  
  TEST_F(BofStd_Test, CheckDirEnum)
  {
    uint32_t          Sts_U32, Nb_U32, Len_U32, DiffDay_U32;
    bool         Sts_B, FileLocked_B;
    BOFFILEFOUND Result_X;
    BOFDATETIME  Time_X, Now_X, DiffTime_X;
    char         pCrtDir_c[128], *p_c, pFn_c[128], pWork_c[128];
    int          i;
    void *       Io_h;

    Bof_RemoveDirectory("test", true);

    Sts_U32 = Bof_GetCurrentDirectory(sizeof(pCrtDir_c), pCrtDir_c);
    ASSERT_EQ(Sts_U32, 0);

    Sts_U32 = Bof_CreateDirectory("test", 0777);
    ASSERT_EQ(Sts_U32, 0);

    Sts_U32 = Bof_GetDateTime(nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);
    Sts_U32 = Bof_GetDateTime(&Now_X);
    ASSERT_EQ(Sts_U32, 0);

    for (i = 0; i < 10; i++)
    {
      sprintf(pFn_c, "test%cTestBha%03d.bha", Bof_FilenameSeparator(), i);
      Io_h = Bof_OpenFile(pFn_c, false, (S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH), &FileLocked_B);
      ASSERT_NE(Io_h, BOF_INVALID_HANDLE_VALUE);
      ASSERT_FALSE(FileLocked_B);
      sprintf(pWork_c, "ABCDEFGHIJKLMNOPQRSTUVWXYZ%sazertyuiopqsdfghjklmwxcvbn", Eol);
      Len_U32 = strlen(pWork_c) + 1;
      Sts_U32 = Bof_WriteFile(Io_h, pWork_c, Len_U32, &Nb_U32);
      ASSERT_EQ(Sts_U32, 0);
      ASSERT_EQ(Len_U32, Nb_U32);
      Sts_U32 = Bof_CloseFile(Io_h);
      ASSERT_EQ(Sts_U32, 0);

      sprintf(pFn_c, "test%cTestFilters%03d.filters", Bof_FilenameSeparator(), i);
      Io_h = Bof_OpenFile(pFn_c, false, (S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH), &FileLocked_B);
      ASSERT_NE(Io_h, BOF_INVALID_HANDLE_VALUE);
      ASSERT_FALSE(FileLocked_B);
      sprintf(pWork_c, "ABCDEFGHIJKLMNOPQRSTUVWXYZ%sazertyuiopqsdfghjklmwxcvbn", Eol);
      Len_U32 = strlen(pWork_c) + 1;
      Sts_U32 = Bof_WriteFile(Io_h, pWork_c, Len_U32, &Nb_U32);
      ASSERT_EQ(Sts_U32, 0);
      ASSERT_EQ(Len_U32, Nb_U32);
      Sts_U32 = Bof_CloseFile(Io_h);
      ASSERT_EQ(Sts_U32, 0);
    }


    Sts_U32 = Bof_FindFileNext(&Result_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_FindFileClose(&Result_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_FindFileFirst(nullptr, "*.*", FA_DIRECTORY, &Result_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);
    Sts_U32 = Bof_FindFileFirst("test", nullptr, FA_DIRECTORY, &Result_X);
    ASSERT_NE(Sts_U32, (uint32_t)0);
    Sts_U32 = Bof_FindFileFirst("test", "*.*", FA_DIRECTORY, nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_FindFileFirst("test", "*.*", FA_DIRECTORY, &Result_X);
    ASSERT_EQ(Sts_U32, 0);

    //ASSERT_STREQ(Result_X.pName_c, ".");
    //ASSERT_TRUE(Result_X.Attrib_U8 & _A_SUBDIR);
    Sts_U32 = Bof_DiffDateTime(&Result_X.CreationTime_X, &Now_X, &DiffTime_X, &DiffDay_U32);
    ASSERT_EQ(Sts_U32, 0);
    ASSERT_EQ(DiffDay_U32, 0);

    //	ASSERT_EQ(DiffTime_X.Hour_U8, 0);
    //	ASSERT_LE(DiffTime_X.Minute_U8, 1);

    //ASSERT_EQ(Result_X.CreationTime_X.Second_U8, Now_X.Second_U8);
    //  ASSERT_EQ(Result_X.CreationTime_X.Millisecond_U16, Now_X.Millisecond_U16);

    Sts_U32 = Bof_FindFileFirst("test", "*.*", FA_DIRECTORY, nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_FindFileNext(nullptr);
    ASSERT_NE(Sts_U32, (uint32_t)0);

    Sts_U32 = Bof_FindFileNext(&Result_X);
    ASSERT_EQ(Sts_U32, 0);

    //ASSERT_STREQ(Result_X.pName_c, "..");
    //ASSERT_TRUE(Result_X.Attrib_U8 & _A_SUBDIR);

    Sts_U32 = Bof_FindFileNext(&Result_X);
    ASSERT_EQ(Sts_U32, 0);
    ASSERT_FALSE(Result_X.Attrib_U8 & _A_SUBDIR);
    ASSERT_NE(Result_X.Size_U32, (uint32_t)0);

    Sts_U32 = Bof_FindFileClose(&Result_X);
    ASSERT_EQ(Sts_U32, 0);

    Sts_B = Bof_FileTimeToSystemTime(0, nullptr);
    ASSERT_FALSE(Sts_B);

    Sts_B = Bof_FileTimeToSystemTime(0, &Time_X);
    ASSERT_TRUE(Sts_B);
#if defined( __linux__ ) || defined( __APPLE__ )
    ASSERT_EQ(Time_X.Day_U8, 1);
    ASSERT_EQ(Time_X.Month_U8, 1);
    ASSERT_EQ(Time_X.Year_U16, 1970);
    ASSERT_EQ(Time_X.DayOfWeek_U8, 4);

    //  ASSERT_EQ(Time_X.Hour_U8, 0); can be 1 if on other timezone
    ASSERT_EQ(Time_X.Minute_U8, 0);
    ASSERT_EQ(Time_X.Second_U8, 0);
    ASSERT_EQ(Time_X.Millisecond_U16, 0);
    Sts_B = Bof_FileTimeToSystemTime(0x54C763AD, &Time_X);
#else
    ASSERT_EQ(Time_X.Day_U8, 1);
    ASSERT_EQ(Time_X.Month_U8, 1);
    ASSERT_EQ(Time_X.Year_U16, 1601);
    ASSERT_EQ(Time_X.DayOfWeek_U8, 1);
    ASSERT_EQ(Time_X.Hour_U8, 0);
    ASSERT_EQ(Time_X.Minute_U8, 0);
    ASSERT_EQ(Time_X.Second_U8, 0);
    ASSERT_EQ(Time_X.Millisecond_U16, 0);
    Sts_B = Bof_FileTimeToSystemTime(0x01CFF7817D395771, &Time_X);
#endif


    //  int Sts_i=errno;
    //  ASSERT_EQ(Sts_i,0);


#if defined( __linux__ ) || defined( __APPLE__ )
    ASSERT_TRUE(Sts_B);
    ASSERT_EQ(Time_X.Day_U8, 27);
    ASSERT_EQ(Time_X.Month_U8, 1);
    ASSERT_EQ(Time_X.Year_U16, 2015);
    ASSERT_EQ(Time_X.DayOfWeek_U8, 2);

    // ASSERT_EQ(Time_X.Hour_U8, 10);   can be 11 if on other timezone
    ASSERT_EQ(Time_X.Minute_U8, 8);
    ASSERT_EQ(Time_X.Second_U8, 45);
    ASSERT_EQ(Time_X.Millisecond_U16, 0);

#else
    ASSERT_TRUE(Sts_B);
    ASSERT_EQ(Time_X.Day_U8, 3);
    ASSERT_EQ(Time_X.Month_U8, 11);
    ASSERT_EQ(Time_X.Year_U16, 2014);
    ASSERT_EQ(Time_X.DayOfWeek_U8, 1);
    ASSERT_EQ(Time_X.Hour_U8, 16);
    ASSERT_EQ(Time_X.Minute_U8, 16);
    ASSERT_EQ(Time_X.Second_U8, 14);
    ASSERT_EQ(Time_X.Millisecond_U16, 78);
#endif
    Sts_U32 = Bof_FindFileFirst("test", "*.filters", 0, &Result_X);
    ASSERT_EQ(Sts_U32, 0);
    p_c = strrchr(Result_X.pName_c, '.');
    ASSERT_TRUE(p_c != nullptr);
    ASSERT_STREQ(p_c, ".filters");
    ASSERT_FALSE(Result_X.Attrib_U8 & _A_SUBDIR);

    for (i = 0; i < 9; i++)
    {
      Sts_U32 = Bof_FindFileNext(&Result_X);
      p_c = strrchr(Result_X.pName_c, '.');
      ASSERT_TRUE(p_c != nullptr);
      ASSERT_STREQ(p_c, ".filters");
      ASSERT_FALSE(Result_X.Attrib_U8 & _A_SUBDIR);
    }
    Sts_U32 = Bof_FindFileClose(&Result_X);
    ASSERT_EQ(Sts_U32, 0);



    Sts_U32 = Bof_FindFileFirst("test", "*.bha", 0, &Result_X);
    ASSERT_EQ(Sts_U32, 0);
    p_c = strrchr(Result_X.pName_c, '.');
    ASSERT_TRUE(p_c != nullptr);
    ASSERT_STREQ(p_c, ".bha");
    ASSERT_FALSE(Result_X.Attrib_U8 & _A_SUBDIR);

    for (i = 0; i < 9; i++)
    {
      Sts_U32 = Bof_FindFileNext(&Result_X);
      p_c = strrchr(Result_X.pName_c, '.');
      ASSERT_TRUE(p_c != nullptr);
      ASSERT_STREQ(p_c, ".bha");
      ASSERT_FALSE(Result_X.Attrib_U8 & _A_SUBDIR);
    }
    Sts_U32 = Bof_FindFileClose(&Result_X);
    ASSERT_EQ(Sts_U32, 0);

    for (i = 0; i < 10; i++)
    {
      sprintf(pFn_c, "test%cTestBha%03d.bha", Bof_FilenameSeparator(), i);
      Sts_U32 = Bof_DeleteFile(pFn_c);
      ASSERT_EQ(Sts_U32, 0);
      sprintf(pFn_c, "test%cTestFilters%03d.filters", Bof_FilenameSeparator(), i);
      Sts_U32 = Bof_DeleteFile(pFn_c);
      ASSERT_EQ(Sts_U32, 0);
    }
    Sts_U32 = Bof_RemoveDirectory("test", false);
    ASSERT_EQ(Sts_U32, 0);
  }
}

TEST_F(BofStd_Test, CheckEnvVar)
{
  const char *pEnv_c;
  char pVal_c[128];
  int Sts_i;

  pEnv_c=Bof_GetEnvVar(nullptr);
  ASSERT_TRUE(pEnv_c==0);

  Sts_i= Bof_SetEnvVar(nullptr,pVal_c, 0);
  ASSERT_NE(Sts_i, 0);
  Sts_i = Bof_SetEnvVar("nullptr", nullptr, 0);
  ASSERT_NE(Sts_i, 0);
  Sts_i = Bof_SetEnvVar(nullptr, nullptr, 0);
  ASSERT_NE(Sts_i, 0);

  Sts_i = Bof_SetEnvVar("BHA_TEST_VAR", "1:BHA", 1);
  ASSERT_EQ(Sts_i, 0);
  Sts_i = Bof_SetEnvVar("BHA_TEST_VAR", "2:BHA", 0);
  ASSERT_NE(Sts_i, 0);
  Sts_i = Bof_SetEnvVar("BHA_TEST_VAR", "3:BHA", 1);
  ASSERT_EQ(Sts_i, 0);

  pEnv_c = Bof_GetEnvVar("BHA_TEST");
  ASSERT_TRUE(pEnv_c == 0);

  pEnv_c = Bof_GetEnvVar("BHA_TEST_VAR");
  ASSERT_TRUE(pEnv_c != 0);

  ASSERT_STREQ(pEnv_c, "3:BHA");

}

