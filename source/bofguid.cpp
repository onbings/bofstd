/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bofguid lass
 *
 * Name:        bofguid.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         None
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */

/*** Include files ***********************************************************/
#include <bofstd/bofguid.h>
#include <czmq.h>
#if 0
/*  =========================================================================
	zuuid - UUID support class

	Copyright (c) the Contributors as noted in the AUTHORS file.
	This file is part of CZMQ, the high-level C binding for 0MQ:
	http://czmq.zeromq.org.

	This Source Code Form is subject to the terms of the Mozilla Public
	License, v. 2.0. If a copy of the MPL was not distributed with this
	file, You can obtain one at http://mozilla.org/MPL/2.0/.
	=========================================================================
*/

/*
@header
	The zuuid class generates UUIDs and provides methods for working with
	them. If you build CZMQ with libuuid, on Unix/Linux, it will use that
	library. On Windows it will use UuidCreate(). Otherwise it will use a
	random number generator to produce convincing imitations of UUIDs.
@discuss
@end
*/
#include <assert.h>
#if defined(_WIN32)
#include <Windows.h>
#endif
//  Structure of our class

#define ZUUID_LEN     16
#define ZUUID_STR_LEN (ZUUID_LEN * 2)
struct _zuuid_t {
	uint8_t uuid[ZUUID_LEN];              //  Binary UUID
	char str[ZUUID_STR_LEN + 1];       //  Printable UUID
	char* str_canonical;                //  Canonical UUID, if any
};

typedef struct _zuuid_t zuuid_t;
void zuuid_set(zuuid_t* self, const byte* source);
void zstr_free(char** string_p)
{
	assert(string_p);
	free(*string_p);
	*string_p = NULL;
}
//  --------------------------------------------------------------------------
//  Constructor

zuuid_t*
zuuid_new(void)
{
	zuuid_t* self = (zuuid_t*)malloc(sizeof(zuuid_t));
	assert(self);

#if defined (_WIN32)
	//  Windows always has UUID support
	UUID uuid;
	assert(sizeof(uuid) == ZUUID_LEN);
	UuidCreate(&uuid);
	zuuid_set(self, (uint8_t*)&uuid);
#elif defined (__UTYPE_ANDROID) || !defined (HAVE_UUID)
	//  No UUID system calls, so generate a random string
	uint8_t uuid[ZUUID_LEN];

	int fd = open("/dev/urandom", O_RDONLY);
	if (fd != -1) {
		ssize_t bytes_read = read(fd, uuid, ZUUID_LEN);
		assert(bytes_read == ZUUID_LEN);
		close(fd);
		zuuid_set(self, uuid);
	}
	else {
		//  We couldn't read /dev/urandom and we have no alternative
		//  strategy
		zsys_error(strerror(errno));
		assert(false);
	}
#elif defined (__UTYPE_OPENBSD) || defined (__UTYPE_FREEBSD) || defined (__UTYPE_NETBSD)
	uuid_t uuid;
	uint32_t status = 0;
	uuid_create(&uuid, &status);
	if (status != uuid_s_ok) {
		zuuid_destroy(&self);
		return NULL;
	}
	uint8_t buffer[ZUUID_LEN];
	uuid_enc_be(&buffer, &uuid);
	zuuid_set(self, buffer);
#elif defined (__UTYPE_LINUX) || defined (__UTYPE_OSX) || defined (__UTYPE_GNU)
	uuid_t uuid;
	assert(sizeof(uuid) == ZUUID_LEN);
	uuid_generate(uuid);
	zuuid_set(self, (uint8_t*)uuid);
#else
#   error "Unknow UNIX TYPE"
#endif
	return self;
}


//  -----------------------------------------------------------------
//  Destructor

void
zuuid_destroy(zuuid_t** self_p)
{
	assert(self_p);
	if (*self_p) {
		zuuid_t* self = *self_p;
		free(self->str_canonical);
		free(self);
		*self_p = NULL;
	}
}


//  -----------------------------------------------------------------
//  Create UUID object from supplied ZUUID_LEN-octet value

zuuid_t*
zuuid_new_from(const uint8_t* source)
{
	zuuid_t* self = (zuuid_t*)malloc(sizeof(zuuid_t));
	assert(self);
	zuuid_set(self, source);
	return self;
}


//  -----------------------------------------------------------------
//  Set UUID to new supplied ZUUID_LEN-octet value

void
zuuid_set(zuuid_t* self, const uint8_t* source)
{
	assert(self);
	memcpy(self->uuid, source, ZUUID_LEN);
	char hex_char[] = "0123456789ABCDEF";
	int byte_nbr;
	for (byte_nbr = 0; byte_nbr < ZUUID_LEN; byte_nbr++) {
		uint32_t val = (self->uuid)[byte_nbr];
		self->str[byte_nbr * 2 + 0] = hex_char[val >> 4];
		self->str[byte_nbr * 2 + 1] = hex_char[val & 15];
	}
	self->str[ZUUID_LEN * 2] = 0;
	zstr_free(&self->str_canonical);
}


//  -----------------------------------------------------------------
//  Set UUID to new supplied string value skipping '-' and '{' '}'
//  optional delimiters. Return 0 if OK, else returns -1.

int
zuuid_set_str(zuuid_t* self, const char* source)
{
	assert(self);
	assert(source);

	uint32_t byte_nbr = 0;
	while (*source) {
		if (*source == '-' || *source == '{' || *source == '}')
			source++;
		else {
			//  Get two hex digits
			uint32_t value;
			if (sscanf(source, "%02x", &value) != 1)
				return -1;
			if (byte_nbr < ZUUID_LEN) {
				self->uuid[byte_nbr] = (uint8_t)value;
				self->str[byte_nbr * 2 + 0] = toupper(*source++);
				self->str[byte_nbr * 2 + 1] = toupper(*source++);
				byte_nbr++;
			}
			else
				return -1;
		}
	}
	zstr_free(&self->str_canonical);
	return 0;
}


//  -----------------------------------------------------------------
//  Return UUID binary data

const uint8_t*
zuuid_data(zuuid_t* self)
{
	assert(self);
	return self->uuid;
}


//  -----------------------------------------------------------------
//  Return UUID binary size

size_t
zuuid_size(zuuid_t* self)
{
	assert(self);
	return ZUUID_LEN;
}


//  -----------------------------------------------------------------
//  Returns UUID as string

const char*
zuuid_str(zuuid_t* self)
{
	assert(self);
	return self->str;
}


//  -----------------------------------------------------------------
//  Return UUID in the canonical string format: 8-4-4-4-12, in lower
//  case. Caller does not modify or free returned value. See
//  http://en.wikipedia.org/wiki/Universally_unique_identifier

const char*
zuuid_str_canonical(zuuid_t* self)
{
	assert(self);
	if (!self->str_canonical)
		self->str_canonical = (char*)malloc(8 + 4 + 4 + 4 + 12 + 5);
	*self->str_canonical = 0;
	strncat(self->str_canonical, self->str, 8);
	strcat(self->str_canonical, "-");
	strncat(self->str_canonical, self->str + 8, 4);
	strcat(self->str_canonical, "-");
	strncat(self->str_canonical, self->str + 12, 4);
	strcat(self->str_canonical, "-");
	strncat(self->str_canonical, self->str + 16, 4);
	strcat(self->str_canonical, "-");
	strncat(self->str_canonical, self->str + 20, 12);

	int char_nbr;
	for (char_nbr = 0; char_nbr < 36; char_nbr++)
		self->str_canonical[char_nbr] = tolower(self->str_canonical[char_nbr]);
	return self->str_canonical;
}


//  -----------------------------------------------------------------
//  Store UUID blob into a target array

void
zuuid_export(zuuid_t* self, uint8_t* target)
{
	assert(self);
	memcpy(target, self->uuid, ZUUID_LEN);
}


//  -----------------------------------------------------------------
//  Check if UUID is same as supplied value

bool
zuuid_eq(zuuid_t* self, const uint8_t* compare)
{
	assert(self);
	return (memcmp(self->uuid, compare, ZUUID_LEN) == 0);
}


//  -----------------------------------------------------------------
//  Check if UUID is different from supplied value

bool
zuuid_neq(zuuid_t* self, const uint8_t* compare)
{
	assert(self);
	return (memcmp(self->uuid, compare, ZUUID_LEN) != 0);
}


//  --------------------------------------------------------------------------
//  Make copy of UUID object; if uuid is null, or memory was exhausted,
//  returns null.

zuuid_t*
zuuid_dup(zuuid_t* self)
{
	if (self)
		return zuuid_new_from(zuuid_data(self));
	else
		return NULL;
}


//  --------------------------------------------------------------------------
//  Print properties of the zuuid object.

void
zuuid_print(zuuid_t* self)
{
	printf("%s", zuuid_str_canonical(self));
}
#endif


BEGIN_BOF_NAMESPACE()
// Opaque pointer design pattern: defined privately here all private data and functions: all of these can now change without recompiling callers ...
class BofGuid::BofGuidImplementation
{
public:
		zuuid_t *mpUuid;
		bool mGuidValid_B;

		BofGuidImplementation()
		{
			mpUuid = zuuid_new();
			mGuidValid_B = true;
		}

		BofGuidImplementation(const uint8_t *_pData_U8)
		{
			mpUuid = zuuid_new();
			if (_pData_U8)
			{
				zuuid_set(mpUuid, _pData_U8);
			}
			mGuidValid_B = true;
		}

		BofGuidImplementation(const std::vector<uint8_t> &_rData)
		{
			mpUuid = zuuid_new();
			mGuidValid_B = (_rData.size() >= Size());
			zuuid_set(mpUuid, _rData.data());
		}

		BofGuidImplementation(const std::string &_rUuidStr_S)  //_rUuidStr_S connonical or not
		{
			int Sts_i;

			mpUuid = zuuid_new();
			Sts_i = zuuid_set_str(mpUuid, _rUuidStr_S.c_str());
			mGuidValid_B = (Sts_i == 0);
		}

		~BofGuidImplementation()
		{
			zuuid_destroy(&mpUuid);
		}

		size_t Size() const
		{
			return mpUuid ? zuuid_size(mpUuid) : 0;
		}

		const std::vector<uint8_t> Data() const
		{
			std::vector<uint8_t> Rts;

			if (mpUuid)
			{
				const uint8_t *pData_U8 = zuuid_data(mpUuid);
				Rts = std::vector<uint8_t>(pData_U8, pData_U8 + Size());
			}
			return Rts;
		}

		const uint8_t *Data(size_t &_rSize) const
		{
			const uint8_t *pRts_U8 = mpUuid ? zuuid_data(mpUuid) : nullptr;
			_rSize = Size();
			return pRts_U8;
		}

		std::string ToString(bool _Cannonical_B) const
		{
			std::string Rts_S;

			if (mpUuid)
			{
				if (_Cannonical_B)
				{
					Rts_S = zuuid_str_canonical(mpUuid);    //6ba39f88-ed13-8c48-a765-a8738dba383e
				}
				else
				{
					Rts_S = zuuid_str(mpUuid);          //6BA39F88ED138C48A765A8738DBA383E
				}
			}
			return Rts_S;
		}

		void OperatorAssign(const BofGuidImplementation *_pOther)
		{
			if (_pOther)
			{
				zuuid_destroy(&mpUuid);
				mpUuid = zuuid_dup(_pOther->mpUuid);
				//  mpUuid = mpUuid ? zuuid_dup(_pOther->mpUuid) : nullptr;
			}
		}

		void OperatorAssign(const char *_pUuidStr_c)
		{
			mpUuid ? zuuid_set_str(mpUuid, _pUuidStr_c) : -1;
		}

		bool OperatorEqual(const BofGuidImplementation *_pOther) const
		{
			bool Rts_B = false;
			size_t Size;
			if ((_pOther) && (mpUuid))
			{
				const uint8_t *pData_U8 = _pOther->Data(Size);
				Rts_B = zuuid_eq(mpUuid, pData_U8);
			}
			return Rts_B;
		}

		bool OperatorLower(const BofGuidImplementation *_pOther) const
		{
			bool Rts_B = false;

			if (_pOther)
			{
				Rts_B = ToString(true) < _pOther->ToString(true);
			}
			return Rts_B;
		}

		bool IsValid() const
		{
			return mGuidValid_B;
		}

		void Clear()
		{
//		size_t t = Size();	//16
			uint8_t pData_U8[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
			zuuid_set(mpUuid, pData_U8);
		}

		bool IsNull() const
		{
			size_t i, Len;
			const uint8_t *pData_U8 = Data(Len);
			for (i = 0; i < Len; i++)
			{
				if (pData_U8[i])
				{
					break;
				}
			}
			return (i == Len);
		}
};

// Opaque pointer design pattern: ... set Implementation values ...
BofGuid::BofGuid()
	: mpuBofGuidImplementation(new BofGuidImplementation())
{}

BofGuid::BofGuid(const uint8_t *_pData_U8)
	: mpuBofGuidImplementation(new BofGuidImplementation(_pData_U8))
{}

BofGuid::BofGuid(const std::vector<uint8_t> &_rData)
	: mpuBofGuidImplementation(new BofGuidImplementation(_rData))
{}

BofGuid::BofGuid(const std::string &_rUuidStr_S)
	: mpuBofGuidImplementation(new BofGuidImplementation(_rUuidStr_S))
{}

BofGuid::BofGuid(const BofGuid &_rOther)
	: mpuBofGuidImplementation(new BofGuidImplementation(_rOther.Data()))
{}

BofGuid::BofGuid(BofGuid &&_rrOther)
	: mpuBofGuidImplementation(new BofGuidImplementation(_rrOther.Data()))
{}

BofGuid::~BofGuid()
{}

const std::vector<uint8_t> BofGuid::Data() const
{
	return mpuBofGuidImplementation->Data();
}

const uint8_t *BofGuid::Data(size_t &_rSize) const
{
	return mpuBofGuidImplementation->Data(_rSize);
}

std::string BofGuid::ToString(bool _Cannonical_B) const
{
	return mpuBofGuidImplementation->ToString(_Cannonical_B);
}

BofGuid &BofGuid::operator=(const BofGuid &_rOther)
{
	mpuBofGuidImplementation->OperatorAssign(_rOther.mpuBofGuidImplementation.get());
	return *this;
}

BofGuid &BofGuid::operator=(BofGuid &&_rrOther)
{
	if (this != &_rrOther)
	{
		mpuBofGuidImplementation->OperatorAssign(_rrOther.mpuBofGuidImplementation.get());
		zuuid_destroy(&_rrOther.mpuBofGuidImplementation->mpUuid);
//		BOF_SAFE_DELETE(_rrOther.mpuBofGuidImplementation);
	}
	return *this;
}

BofGuid &BofGuid::operator=(const char *_pUuidStr_c)
{
	mpuBofGuidImplementation->OperatorAssign(_pUuidStr_c);
	return *this;
}

BofGuid &BofGuid::operator=(const std::string &_rUuidStr_S)
{
	mpuBofGuidImplementation->OperatorAssign(_rUuidStr_S.c_str());
	return *this;
}

bool BofGuid::operator<(const BofGuid &_rOther) const
{

	bool Rts_B = mpuBofGuidImplementation->OperatorEqual(_rOther.mpuBofGuidImplementation.get());
	if (Rts_B)
	{
		Rts_B = false;
	}
	else
	{
		Rts_B = mpuBofGuidImplementation->OperatorLower(_rOther.mpuBofGuidImplementation.get());
	}
//  printf("%s < %s %d\n", this->ToString(true).c_str(), _rOther.ToString(true).c_str(), Rts_B);
	return Rts_B;
}

bool BofGuid::operator<=(const BofGuid &_rOther) const
{
	bool Rts_B = mpuBofGuidImplementation->OperatorEqual(_rOther.mpuBofGuidImplementation.get());
	if (!Rts_B)
	{
		Rts_B = mpuBofGuidImplementation->OperatorLower(_rOther.mpuBofGuidImplementation.get());
	}
//  printf("%s <= %s %d\n", this->ToString(true).c_str(), _rOther.ToString(true).c_str(), Rts_B);
	return Rts_B;
}

bool BofGuid::operator==(const BofGuid &_rOther) const
{
	bool Rts_B = mpuBofGuidImplementation->OperatorEqual(_rOther.mpuBofGuidImplementation.get());
//  printf("%s == %s %d\n", this->ToString(true).c_str(), _rOther.ToString(true).c_str(), Rts_B);
	return Rts_B;
}

bool BofGuid::operator!=(const BofGuid &_rOther) const
{
	bool Rts_B = !(*this == _rOther);
//  printf("%s != %s %d\n", this->ToString(true).c_str(), _rOther.ToString(true).c_str(), Rts_B);
	return Rts_B;
}

bool BofGuid::operator>(const BofGuid &_rOther) const
{
	bool Rts_B = mpuBofGuidImplementation->OperatorEqual(_rOther.mpuBofGuidImplementation.get());
	if (Rts_B)
	{
		Rts_B = false;
	}
	else
	{
		Rts_B = !mpuBofGuidImplementation->OperatorLower(_rOther.mpuBofGuidImplementation.get());
	}
//  printf("%s > %s %d\n", this->ToString(true).c_str(), _rOther.ToString(true).c_str(), Rts_B);
	return Rts_B;
}

bool BofGuid::operator>=(const BofGuid &_rOther) const
{
	bool Rts_B = mpuBofGuidImplementation->OperatorEqual(_rOther.mpuBofGuidImplementation.get());
	if (!Rts_B)
	{
		Rts_B = !mpuBofGuidImplementation->OperatorLower(_rOther.mpuBofGuidImplementation.get());
	}
//  printf("%s >= %s %d\n", this->ToString(true).c_str(), _rOther.ToString(true).c_str(), Rts_B);
	return Rts_B;
}

bool BofGuid::IsValid() const
{
	return mpuBofGuidImplementation->IsValid();
}

void BofGuid::Clear()
{
	mpuBofGuidImplementation->Clear();
}

bool BofGuid::IsNull() const
{
	return (mpuBofGuidImplementation->IsNull());
}
END_BOF_NAMESPACE()