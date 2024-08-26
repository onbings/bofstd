/*
 * Copyright (c) 2013-2033, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bofserviceendpoint module of the bofstd library
 *
 * Name:        bofserviceendpoint.h
 * Author:      Bernard HARMEL: onbings@gmail.com
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         None
 *
 * History:
 *
 * V 1.00  Aug 15 2024  BHA : Initial release
 */
#pragma once

#include <bofstd/bofuri.h>

BEGIN_BOF_NAMESPACE()

struct BOF_SERVICE_ENPOINT_META_DATA
{
  bool UserMaster_B;
  std::string UserArg_S;
  void *pUser;

  BOF_SERVICE_ENPOINT_META_DATA()
  {
    Reset();
  }
  void Reset()
  {
    UserMaster_B = false;
    UserArg_S = "";
    pUser = nullptr;
  }
};
class BOFSTD_EXPORT BofServiceEndpoint
{
public:
  BofServiceEndpoint();
  BofServiceEndpoint(const std::string &_rServiceUrl_S);
  BofServiceEndpoint(const std::string &_rServiceName_S, const std::string &_rServiceInstance_S, const std::string &_rServiceUrl_S,
                     const std::set<std::string> &_rServiceTagCollection);
  ~BofServiceEndpoint();
  bool IsValid();
  std::string ToString(bool _ShowName_B);

  bool SetServiceMetaData(const BOF_SERVICE_ENPOINT_META_DATA &_rServiceEndpointMetaData_X);
  bool SetServiceName(const std::string &_rServiceName_S);
  bool SetServiceInstance(const std::string &_rServiceInstance_S);
  bool SetServiceUrl(const std::string &_rServiceUrl_S);
  bool SetServiceTagCollection(const std::set<std::string> &_rServiceTagCollection);

  BOF_SERVICE_ENPOINT_META_DATA GetServiceMetaData() const;
  std::string GetServiceName() const;
  std::string GetServiceInstance() const;
  std::string GetServiceUrl() const;
  std::set<std::string> GetServiceTagCollection() const;

private:
  BofUri mUri;
  std::string mServiceName_S;
  std::string mServiceInstance_S;
  std::set<std::string> mServiceTagCollection;
  BOF_SERVICE_ENPOINT_META_DATA mServiceEndpointMetaData_X;
};

END_BOF_NAMESPACE()