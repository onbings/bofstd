/*
 * Copyright (c) 2013-2033, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bofuri module of the bofstd library
 *
 * Name:        bofuri.h
 * Author:      Bernard HARMEL: onbings@gmail.com
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         None
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#pragma once

#include <bofstd/bofsocketos.h>
#include <bofstd/bofpath.h>
#include <string>
#include <map>

BEGIN_BOF_NAMESPACE()
/// @brief This class implements an Uri parser. An uri is defined in https://en.wikipedia.org/wiki/Uniform_Resource_Identifier
/// In brief it is a string formatted as follow:
/// URI = scheme ":" ["//" authority] path ["?" query] ["#" fragment]
/// ex storage://10.129.4.172:11000/mnt/file/cg2/data1
///         userinfo       host      port
///    ┌──┴───┐ ┌──────┴──────┐ ┌┴┐
///    https ://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top
///    └─┬─┘   └───────────┬──────────────┘└───────┬───────┘ └───────────┬─────────────┘ └┬┘
///    scheme          authority                  path                 query           fragment
///    
///    ldap ://[2001:db8::7]/c=GB?objectClass?one
///    └┬─┘   └─────┬─────┘└─┬─┘ └──────┬──────┘
///    scheme   authority   path      query
///    
///    mailto : John.Doe@example.com
///    └─┬──┘ └────┬─────────────┘
///    scheme     path
///    
///    news : comp.infosystems.www.servers.unix
///    └┬─┘ └─────────────┬─────────────────┘
///    scheme            path
///    
///    tel : +1 - 816 - 555 - 1212
///    └┬┘ └──────┬──────┘
///    scheme    path
///    
///    telnet ://192.0.2.16:80/
///    └─┬──┘   └─────┬─────┘│
///    scheme     authority  path
///    
///    urn : oasis:names:specification:docbook:dtd:xml:4.1.2
///    └┬┘ └──────────────────────┬──────────────────────┘
///    scheme                    path
///
/// For bofstd it is:
/// - a protocol string (scheme)		-> BOF_SOCKET_ADDRESS_COMPONENT
/// - an address string (authority)	-> BOF_SOCKET_ADDRESS_COMPONENT
/// - a path (path)									-> BofPath
/// - an query parameter string			-> std::map<std::string, std::string>
///   Start from the last ? in the sring after the path. Its syntax is not well defined, but by 
///   convention is most often a sequence of attribute–value pairs separated by a delimiter.
///		Query delimiter		Example
///		Ampersand(&)			key1 = value1 & key2 = value2
///		Semicolon(;)  		key1 = value1; key2 = value2
/// - An optional fragment component -> std::string
///   It is preceded by a hash(#).The fragment contains a fragment identifier providing direction 
///   to a secondary resource, such as a section heading in an article identified by the remainder 
///   of the URI.When the primary resource is an HTML document, the fragment is often an id 
///   attribute of a specific element, andweb browsers will scroll this element into view.
/// 

class BOFSTD_EXPORT BofUri
{
private:
	BOF_SOCKET_ADDRESS_COMPONENT				mScheme_X;
	BOF_SOCKET_ADDRESS									mIpAddress_X;
	BofPath															mPath;
	std::map<std::string, std::string>	mQueryCollection;
	std::string												  mFragment_S;
	bool																mValid_B = false;
	char																mQueryDelimiter_c = ';';
public:
		/// @brief Initializes a new instance of the Uri class.
		/// @remarks Default constructor.
		BofUri();

		/// @brief Initializes a new instance of the Uri class.
		/// @param _pUri_c A pointer to a char array containing the uri
		/// @remarks None.
		BofUri(const char *_pUri_c);

		/// @brief Initializes a new instance of the Uri class.
		/// @param _rUri_S A string containing the uri
		/// @remarks None.
		BofUri(const std::string &_rUri_S);

		/// @brief Initializes a new instance of the Uri class.
		/// @param _rScheme_X Specifies the scheme.
		/// @param _rPath Specifies the path.
		/// @param _rQueryCollection Specifies the query collection.
		/// @param _rFragment_S Specifies the fragment.
		/// @remarks None.
		BofUri(const BOF_SOCKET_ADDRESS_COMPONENT &_rScheme_X, const BofPath &_rPath, std::map<std::string, std::string>	&_rQueryCollection, const std::string &_rFragment_S);

		/// @brief Initializes a new instance of the Uri class.
		/// @param _rOther_O A reference to another Uri class instance.
		/// @remarks Copy constructor.
		BofUri(const BofUri &_rOther_O);

		/// @brief Initializes a new instance of the Uri class.
		/// @param _rrOther_O A rvalue reference to another Uri class instance.
		/// @remarks Move constructor.
		BofUri(BofUri &&_rrOther_O);

		/// @brief Initializes a new instance of the Uri class.
		/// @param _rOther_O A reference to another Uri class instance.
		/// @remarks  Copy assignment operator.
		BofUri &operator=(const BofUri &_rOther_O); // Copy assignment operator

		/// @brief Create a new uri by assigning a new string value.
		/// @param _rNewUri_S A reference to a string containing the new uri.
		/// @return a modified class instance
		/// @remarks Assignment operator.
		BofUri &operator=(const std::string &_rNewUri_S);

		/// @brief Create a new uri by assigning a new char array value.
		/// @param _pNewUri_c A pointer to a char array containing the new uri.
		/// @return a modified class instance
		/// @remarks Assignment operator.
		BofUri &operator=(const char *_pNewUri_c);

		/// @brief Initializes a new instance of the Uri class.
		/// @param _rrOther_O A rvalue reference to another Uri class instance.
		/// @remarks Move constructor.
		BofUri &operator=(BofUri &&_rrOther_O);

		/// @brief Release all class instance resources.
		/// @remarks Virtual destructor.
		virtual ~BofUri();

		/// @brief Compares two object to check for equality
		/// @param _rOther_O A reference to another Uri class instance.
		/// @return true if both object are equal
		/// @remarks Equal operator. 
		bool operator==(const BofUri &_rOther_O) const;

		/// @brief Compares two object to check for difference
		/// @param _rOther_O A reference to another Uri class instance.
		/// @return true if the objects are different (not equal)
		/// @remarks Different operator.
		bool operator!=(const BofUri &_rOther_O) const;

		/// @brief Check if the uri instance is well formed.
		/// @return true if the uri is valid.
		/// @remarks None
		bool IsValid() const;

		/// @brief Set the scheme characteristics of the uri
		/// @param [in] _rScheme_S : specifies a string representing the scheme.
		/// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
		/// @remarks None
		BOFERR SetScheme(const std::string &_rScheme_S);

		/// @brief Set the scheme characteristics of the uri
		/// @param [in] _rScheme_X : specifies a BOF_SOCKET_ADDRESS_COMPONENT representing the scheme.
		/// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
		/// @remarks None
		BOFERR SetScheme(const BOF_SOCKET_ADDRESS_COMPONENT &_rScheme_X);

		/// @brief Set the path characteristics of the usri
		/// @param [in] _rPath_S : specifies a string representing the path.
		/// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
		/// @remarks Must be a linux (not windows) path type
		BOFERR SetPath(const std::string &_rPath_S);

		/// @brief Set the path characteristics of the usri
		/// @param [in] _rBofPath : specifies a BofPath representing the path.
		/// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
		/// @remarks Must be a linux (not windows) path type
		BOFERR SetPath(const BofPath &_rBofPath);

		/// @brief Set the query collection of the uri.
		/// @param [in] _rQueryCollection : specifies the query collection.
		/// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
		/// @remarks None
		BOFERR SetQueryCollection(const std::map<std::string, std::string> &_rQueryCollection);

		/// @brief Set the query collection of the uri.
		/// @param [in] _rQueryString_S : specifies the query collection.
		/// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
		/// @remarks None
		BOFERR SetQueryCollection(const std::string &_rQueryString_S);

		/// @brief Add a key value pair to the uri query collection.
		/// @param [in] _rQuery : specifies the query to add to the collection.
		/// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
		/// @remarks None
		BOFERR AddToQueryCollection(const std::pair<std::string, std::string> &_rQuery);

		/// @brief Add a key value pair to the uri query collection.
		/// @param [in] _rQuery_S : specifies the query to add to the collection.
		/// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
		/// @remarks None
		BOFERR AddToQueryCollection(const std::string &_rQuery_S);

		/// @brief Remove a key value pair from the usri query collection.
		/// @param [in] _rQueryKeyToRemove_S : specifies the query key to remove from the collection.
		/// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
		/// @remarks None
		BOFERR RemoveFromQueryCollection(const std::string &_rQueryKeyToRemove_S);

		/// @brief Set the fragment part of the uri.
		/// @param [in] _rFragment_S : specifies the fragment.
		/// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
		/// @remarks None
		BOFERR SetFragment(const std::string &_rFragment_S);

		/// @brief Returns the scheme characteristics.
		/// @param [out] _rScheme_S : returns a string representing the scheme.
		/// @return A BOF_SOCKET_ADDRESS_COMPONENT containing the scheme characteristics.
		/// @remarks None
		const BOF_SOCKET_ADDRESS_COMPONENT &Scheme(std::string &_rScheme_S) const;

		/// @brief Returns the ip address corresponding to the current scheme.
		/// @param [out] _rIpAddress_S : returns a string representing the ip address.
		/// @return A BOF_SOCKET_ADDRESS containing the scheme ip address.
		/// @remarks None
		const BOF_SOCKET_ADDRESS &IpAddress(std::string &_rIpAddress_S) const;

		/// @brief Returns the path characteristics.
		/// @param [out] _rPath_S : returns a string representing the path.
		/// @return A BOF_SOCKET_ADDRESS_COMPONENT containing the scheme characteristics.
		/// @remarks None
		const BofPath &Path(std::string &_rPath_S) const;

		/// @brief Returns the path characteristics.
		/// @param [out] _rQueryCollection_S : return a string representing the query.
		/// @return A map of string containing the query parameters.
		/// @remarks None
		const std::map<std::string, std::string> &QueryCollection(std::string &_rQueryCollection_S) const;

		/// @brief Returns the fragment part of the uri.
		/// @return A string containing the fragment.
		/// @remarks None
		const std::string	&Fragment() const;

		/// @brief Convert an uri instance into a printable string.
		/// @param [in] _QueryDelimiter_c specify the character to use to separate the different option.
		/// @return A string representing the uti instance.
		/// @remarks None.
		std::string	ToString() const;

		/// @brief Esccape "special" characters in url
		/// @param _rIn_S : Specifies the url to encode.
		/// @return The encoded string
		/// @remarks https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
		//           https://docs.microfocus.com/OMi/10.62/Content/OMi/ExtGuide/ExtApps/URL_encoding.htm
		static std::string S_UrlEncode(const std::string &_rIn_S);

		/// @brief De-Esccape "special" characters in url
		/// @param _rIn_S : Specifies the url to decode.
		/// @return The decoded string
		/// @remarks https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
		//           https://docs.microfocus.com/OMi/10.62/Content/OMi/ExtGuide/ExtApps/URL_encoding.htm
		static std::string S_UrlDecode(const std::string &_rIn_S);

private:
		/// @brief Initializes the class instance members.
		/// @param _rUri_S A string containing the uri
		/// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
		/// @remarks None
		BOFERR InitUriField(const std::string &_rUri_S);

		/// @brief Check if a uri contains forbidden char such as "<>:\"\\|?*\a\f\n\r\t\v"
		/// @param _rUri_S : Specifies the uri to check
		/// @return true if the uri contains forbidden char
		/// @remarks None
		bool IsForbiddenChar(const std::string &_rUri_S);
};
END_BOF_NAMESPACE()