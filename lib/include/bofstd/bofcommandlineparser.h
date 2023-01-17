/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the BofCommandLineParser class
 *
 * Name:        bofcommandlineparser.h
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

#include <vector>

BEGIN_BOF_NAMESPACE()

typedef std::function<BOFERR(const char *_pError_c)> BOFCOMMANDLINEPARSER_ERROR_CALLBACK;

class BOFSTD_EXPORT BofCommandLineParser
{
private:
public:
  BofCommandLineParser();

  virtual ~BofCommandLineParser();

  BOFERR ToByte(const std::string &_rOption_S, const std::vector<BOFPARAMETER> &_rCommandLineOption_X, const BOFPARAMETER_PARSE_CALLBACK _ParseCallback_O, const BOFCOMMANDLINEPARSER_ERROR_CALLBACK _ErrorCallback_O);

  BOFERR ToByte(const int _Argc_i, char *const *_ppArgv_c, const std::vector<BOFPARAMETER> &_rCommandLineOption_X, const BOFPARAMETER_PARSE_CALLBACK _ParseCallback_O,
                const BOFCOMMANDLINEPARSER_ERROR_CALLBACK _ErrorCallback_O);

  void BuildHelpString(const std::vector<BOFPARAMETER> &_rCommandLineOption_X, const std::string &_rTitle_S, std::string &_rHelpString_S);
};

END_BOF_NAMESPACE()