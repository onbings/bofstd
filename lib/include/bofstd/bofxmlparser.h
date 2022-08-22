/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the BofXmlParser class
 *
 * Name:        bofxmlparser.h
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

/*** Include files ***********************************************************/

#include <vector>
#include <memory>
#include <functional>
#include <bofstd/bofstd.h>
#include <bofstd/bofparameter.h>
#include <bofstd/bofpath.h>
#include <bofstd/bofsocket.h>

/*** Global variables ********************************************************/

/*** Definitions *************************************************************/

/*** Class *******************************************************************/

BEGIN_BOF_NAMESPACE()
typedef std::function<bool(int __Sts_i, const BOFPARAMETER &rXmlEntry_X, const char *_pValue_c)> BOFXMLPARSER_ERROR_CALLBACK;

class BOFSTD_EXPORT BofXmlParser
{
		// Opaque pointer design pattern: all public and protected stuff goes here ...
public:
		BofXmlParser(const std::string &_rXmlInput_S);

		BofXmlParser &operator=(const BofXmlParser &) = delete;     // Disallow copying
		BofXmlParser(const BofXmlParser &) = delete;

		BOFERR ToByte(const std::vector<BOFPARAMETER> &_rXmlSchema_X, const BOFPARAMETER_PARSE_CALLBACK _ParseCallback_O, const BOFXMLPARSER_ERROR_CALLBACK _ErrorCallback_O);

		const char *GetFirstElementFromOid(bool _ItIsAnAttributte_B, const char *_pOid_c);

		const char *GetNextElementFromOid();

		bool IsValid();

		std::string RootName();                          //From the parsed string
		static std::string S_RootName(const std::string &_rXmlIn_S);    //From a string
		virtual ~BofXmlParser();

		BofXmlParser &operator=(const std::string &_rXmlInput_S);

		// Opaque pointer design pattern: opaque type here
private:
		class XmlParserImplementation;

		std::unique_ptr<XmlParserImplementation> mpXmlParserImplementation_O;
};

END_BOF_NAMESPACE()