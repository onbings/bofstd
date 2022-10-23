/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the BofXmlWriter class
 *
 * Name:        bofxmlwriter.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#pragma once

#include <bofstd/bofparameter.h>

#include <memory>
#include <vector>
#include <functional>

BEGIN_BOF_NAMESPACE()

class BOFSTD_EXPORT BofXmlWriter
{
  // Opaque pointer design pattern: all public and protected stuff goes here ...
public:
  BofXmlWriter();

  BofXmlWriter &operator=(const BofXmlWriter &) = delete;     // Disallow copying
  BofXmlWriter(const BofXmlWriter &) = delete;

  BOFERR FromByte(const std::string &_rXmlDeclare_S, const std::vector<BOFPARAMETER> &_rXmlSchema_X, std::string &_rXmlOutput_S);

  virtual ~BofXmlWriter();

  // Opaque pointer design pattern: opaque type here
private:
  class XmlWriterImplementation;

  std::unique_ptr<XmlWriterImplementation> mpuXmlWriterImplementation;
};

END_BOF_NAMESPACE()