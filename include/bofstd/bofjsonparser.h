/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the BofJsonParser class
 *
 * Name:        bofjsonparser.h
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
#include <memory>
#include <vector>
#include <functional>
#include <bofstd/bofstd.h>
#include <bofstd/bofparameter.h>

/*** Global variables ********************************************************/

/*** Definitions *************************************************************/

/*** Class *******************************************************************/

BEGIN_BOF_NAMESPACE()
typedef std::function<bool(BOFERR _Sts_E, const BOFPARAMETER &rJsonEntry_X, const char *_pValue_c)> BOFJSONPARSER_ERROR_CALLBACK;

class BofJsonParser
{
// Opaque pointer design pattern: all public and protected stuff goes here ...
public:
		BofJsonParser(const std::string &_rJsonInput_S);

		BofJsonParser &operator=(const BofJsonParser &) = delete;     // Disallow copying
		BofJsonParser(const BofJsonParser &) = delete;

		BOFERR ToByte(const std::vector<BOFPARAMETER> &_rJsonSchema_X, const BOFPARAMETER_PARSE_CALLBACK _ParseCallback_O, const BOFJSONPARSER_ERROR_CALLBACK _ErrorCallback_O);

		const char *GetFirstElementFromOid(const char *_pOid_c);

		const char *GetNextElementFromOid();

		bool IsValid();

		std::string RootName();                          //From the parsed string
		static std::string S_RootName(const std::string &_rJsonIn_S);    //From a string
		virtual ~BofJsonParser();

		BofJsonParser &operator=(const std::string &_rJsonInput_S);

// Opaque pointer design pattern: opaque type here
private:
		class JsonParserImplementation;

		std::unique_ptr<JsonParserImplementation> mpuJsonParserImplementation;
};

END_BOF_NAMESPACE()