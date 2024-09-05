/*
 * Copyright (c) 2013-2033, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bofserviceendpoint module of the bofstd library
 *
 * Name:        bofserviceendpoint.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Aug 15 2024  BHA : Initial release
 */
#include <bofstd/bofserviceendpoint.h>

BEGIN_BOF_NAMESPACE()

BofServiceEndpoint::BofServiceEndpoint():mUri()
{
}
BofServiceEndpoint::BofServiceEndpoint(const std::string &_rServiceUrl_S) :mUri(_rServiceUrl_S)
{
}
BofServiceEndpoint::BofServiceEndpoint(const std::string &_rServiceName_S, const std::string &_rServiceInstance_S, const std::string &_rServiceUrl_S,
                                       const std::set<std::string> &_rServiceTagCollection)
  : mServiceName_S(_rServiceName_S), mServiceInstance_S(_rServiceInstance_S), mUri(_rServiceUrl_S),mServiceTagCollection(_rServiceTagCollection)
{
}
BofServiceEndpoint::~BofServiceEndpoint() 
{
}
bool BofServiceEndpoint::IsValid()
{
  return mUri.IsValid();
}
std::string BofServiceEndpoint::ToString(bool _ShowName_B)
{
  std::string Rts;

  if (_ShowName_B)
  {
      Rts = mServiceName_S + '(' + mServiceInstance_S + ")@";
  }
  Rts += mUri.ToString();
  return Rts;
}
void BofServiceEndpoint::Reset()
{
  mUri.Reset();
  mServiceName_S="";
  mServiceInstance_S="";
  mServiceTagCollection.clear();
  mpServiceEndpointMetaData = nullptr;
}
bool BofServiceEndpoint::SetServiceMetaData(void *_pServiceEndpointMetaData)
{
  mpServiceEndpointMetaData = _pServiceEndpointMetaData;
  return true;
}
bool BofServiceEndpoint::SetServiceName(const std::string &_rServiceName_S)
{
  mServiceName_S = _rServiceName_S;
  return true;
}

bool BofServiceEndpoint::SetServiceInstance(const std::string &_rServiceInstance_S)
{
  mServiceInstance_S = _rServiceInstance_S;
  return true;
}

bool BofServiceEndpoint::SetServiceUrl(const std::string &_rServiceUrl_S)
{
  return (mUri.SetAuthority(_rServiceUrl_S) == BOF_ERR_NO_ERROR);
}

bool BofServiceEndpoint::SetServiceTagCollection(const std::set<std::string> &_rServiceTagCollection)
{
  mServiceTagCollection = _rServiceTagCollection;
  return true;
}
void *BofServiceEndpoint::GetServiceMetaData() const
{
  return mpServiceEndpointMetaData;
}
std::string BofServiceEndpoint::GetServiceName() const
{
  return mServiceName_S;
}

std::string BofServiceEndpoint::GetServiceInstance() const
{
  return mServiceInstance_S;
}

std::string BofServiceEndpoint::GetServiceUrl() const
{
  std::string Rts_S;
  BOF::BOF_SOCKET_ADDRESS SocketAddress_X;

  SocketAddress_X = mUri.IpAddress(Rts_S);
  return Rts_S;
}

std::set<std::string> BofServiceEndpoint::GetServiceTagCollection() const
{
  return mServiceTagCollection;
}

END_BOF_NAMESPACE()