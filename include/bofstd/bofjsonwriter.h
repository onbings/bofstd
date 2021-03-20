/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the BofJsonWriter class
 *
 * Name:        bofjsonwriter.h
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
#include <bofstd/bofpath.h>
#include "bofjsonparser.h"

/*** Global variables ********************************************************/

/*** Definitions *************************************************************/

/*** Class *******************************************************************/

BEGIN_BOF_NAMESPACE()

class BofJsonWriter
{
// Opaque pointer design pattern: all public and protected stuff goes here ...
public:
		BofJsonWriter();

		BofJsonWriter &operator=(const BofJsonWriter &) = delete;     // Disallow copying
		BofJsonWriter(const BofJsonWriter &) = delete;

		BOFERR FromByte(bool _ShortFormat_B, const std::vector<BOFPARAMETER> &_rJsonSchema_X, std::string &_rJsonOutput_S);

		bool IsValid();

		std::string RootName();

		virtual ~BofJsonWriter();

// Opaque pointer design pattern: opaque type here
private:
		class JsonWriterImplementation;

		std::unique_ptr<JsonWriterImplementation> mpuJsonWriterImplementation;
};

END_BOF_NAMESPACE()