/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bofguid class
 *
 * Name:        bofbit.h
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
#include <bofstd/bofstd.h>
#include <memory>
#include <vector>

BEGIN_BOF_NAMESPACE()

/*** Global variables ********************************************************/

/*** Definitions *************************************************************/

/*** Class *******************************************************************/

class BOFSTD_EXPORT BofGuid
{
		// Opaque pointer design pattern: all public and protected stuff goes here ...
public:
		BofGuid();

		BofGuid(const uint8_t *_pData_U8);

		BofGuid(const std::vector<uint8_t> &_rData);

		BofGuid(const std::string &_rUuidStr_S);  //_rUuidStr_S connonical or not
		virtual ~BofGuid();

		bool IsValid() const;

		void Clear();

		bool IsNull() const;

		const std::vector<uint8_t> Data() const;

		const uint8_t *Data(size_t &_rSize) const;

		std::string ToString(bool _Cannonical_B) const;

		BofGuid(const BofGuid &_rOther_O);

		BofGuid(BofGuid &&_rrOther_O);

		BofGuid &operator=(const BofGuid &_rOther_O);

		BofGuid &operator=(BofGuid &&_rrOther_O);

		BofGuid &operator=(const char *_pUuidStr_c);

		BofGuid &operator=(const std::string &_rUuidStr_S);

		bool operator<(const BofGuid &_rOther) const;

		bool operator<=(const BofGuid &_rOther) const;

		bool operator==(const BofGuid &_rOther) const;

		bool operator!=(const BofGuid &_rOther) const;

		bool operator>(const BofGuid &_rOther) const;

		bool operator>=(const BofGuid &_rOther) const;

		// Opaque pointer design pattern: opaque type here
private:
		class BofGuidImplementation;

		std::unique_ptr<BofGuidImplementation> mpuBofGuidImplementation;
};

END_BOF_NAMESPACE()