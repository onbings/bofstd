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

#include <bofstd/bofpath.h>
#include <bofstd/bofsocketos.h>

#include <map>
#include <string>

BEGIN_BOF_NAMESPACE()
/// @brief This class implements an Uri parser. An uri is defined in https://en.wikipedia.org/wiki/Uniform_Resource_Identifier
/// In brief it is a string formatted as follow:
/// URI = scheme ":" ["//" authority] path ["?" query] ["#" fragment]
/// ex storage://10.129.4.172:11000/mnt/file/cg2/data1
///					    userinfo       host      port
///             ┌──┴───┐ ┌──────┴──────┐ ┌┴┐
///    https ://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top
///    └─┬─┘    └───────────┬──────────────┘└───────┬───────┘ └───────────┬─────────────┘ └┬┘
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
/// - a protocol string (scheme)		-> scheme + authority: BOF_SOCKET_ADDRESS_COMPONENT
/// - an address string (authority)	-> scheme + authority: BOF_SOCKET_ADDRESS_COMPONENT
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
/// Upper/lower case: So on both server types, it’s best to make sure you only use lower-case characters in your internal
/// links — on Apache, you do it to avoid file not found errors, and on IIS you do it to avoid dupe content.
/// To be on the safe side, it’s also a good idea on IIS to rewrite requests for URLs that contain upper-case characters
/// to the lower-case equivalent.
class BOFSTD_EXPORT BofUri
{
private:
  BOF_SOCKET_ADDRESS_COMPONENT mSchemeAuthority_X;
  BofPath mPath;
  std::map<std::string, std::string> mQueryParamCollection;
  std::string mFragment_S;

  char mQueryParamDelimiter_c = '&';
  bool mValid_B = false;

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
  /// @param _rSchemeAuthority_X Specifies the scheme and the authority.
  /// @param _rPath Specifies the path.
  /// @param _rQueryParamCollection Specifies the query collection.
  /// @param _rFragment_S Specifies the fragment.
  /// @remarks None.
  BofUri(const BOF_SOCKET_ADDRESS_COMPONENT &_rSchemeAuthority_X, const BofPath &_rPath, std::map<std::string, std::string> &_rQueryParamCollection, const std::string &_rFragment_S);

  /// @brief Initializes a new instance of the Uri class.
  /// @param _rScheme_S Specifies the scheme.
  /// @param _rAuthority_S Specifies the authority.
  /// @param _rPath_S Specifies the path.
  /// @param _rQueryParam_S Specifies the query collection.
  /// @param _rFragment_S Specifies the fragment.
  /// @remarks None.
  BofUri(const std::string &_rScheme_S, const std::string &_rAuthority_S, const std::string &_rPath_S, const std::string &_rQueryParam_S, const std::string &_rFragment_S);

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

  /// @brief Set the character used as séparator in the query string.
  /// @param [in] _QueryDelimiter_c : specifies the character used as séparator in the query string. (Must be ';' or '&').
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks None
  BOFERR SetQueryParamDelimiter(char _QueryParamDelimiter_c);

  /// @brief Set the scheme characteristics of the uri
  /// @param [in] _rScheme_S : specifies a string representing the scheme.
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks None
  BOFERR SetScheme(const std::string &_rScheme_S);

  /// @brief Set the authority characteristics of the uri
  /// @param [in] _rAuthority_S : specifies a string representing the authority.
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks None
  BOFERR SetAuthority(const std::string &_rAuthority_S);

  /// @brief Set the scheme and authority characteristics of the uri
  /// @param [in] _rSchemeAuthority_S : specifies a string representing the scheme and the authority.
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks None
  BOFERR SetSchemeAuthority(const std::string &_rSchemeAuthority_S);

  /// @brief Set the scheme and authority characteristics of the uri
  /// @param [in] _rScheme_X : specifies a BOF_SOCKET_ADDRESS_COMPONENT representing the scheme and the authority.
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks None
  BOFERR SetSchemeAuthority(const BOF_SOCKET_ADDRESS_COMPONENT &_rSchemeAuthority_X);

  /// @brief Set the path characteristics of the usri
  /// @param [in] _rPath_S : specifies a string representing the path.
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks Must be a linux (not windows) path type
  // BOFERR SetPath(const std::string &_rPath_S);

  /// @brief Set the path characteristics of the usri
  /// @param [in] _rPath : specifies a BofPath representing the path.
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks Must be a linux (not windows) path type
  BOFERR SetPath(const BofPath &_rPath);

  /// @brief Set the query param collection of the uri.
  /// @param [in] _rQueryParamCollection : specifies the query collection.
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks None
  BOFERR SetQueryParamCollection(const std::map<std::string, std::string> &_rQueryParamCollection);

  /// @brief Set the query collection of the uri.
  /// @param [in] _rQueryParam_S : specifies the query collection with each key-value element separated by ';' or '&'
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks None
  BOFERR SetQueryParamCollection(const std::string &_rQueryParam_S);

  /// @brief Add a key value pair to the uri query collection.
  /// @param [in] _rKey_S : specifies the query key to add to the collection.
  /// @param [in] _rValue_S : specifies the query key to add to the collection.
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks None
  BOFERR AddToQueryParamCollection(const std::string &_rKey_S, const std::string &_rValue_S);

  /// @brief Remove a key value pair from the usri query collection.
  /// @param [in] _rQueryParamKeyToRemove_S : specifies the query key to remove from the collection.
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks None
  BOFERR RemoveFromQueryParamCollection(const std::string &_rQueryParamKeyToRemove_S);

  /// @brief Set the fragment part of the uri.
  /// @param [in] _rFragment_S : specifies the fragment.
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks None
  BOFERR SetFragment(const std::string &_rFragment_S);

  /// @brief Returns the character used as séparator in the query string.
  /// @return The character used in the query string.
  /// @remarks Must be ';' or '&'
  char QueryParamDelimiter() const;

  /// @brief Returns the scheme characteristics.
  /// @return A string containing the scheme characteristics.
  /// @remarks None
  const std::string &Scheme() const;

  /// @brief Returns the authority characteristics.
  /// @return A string containing the authority characteristics.
  /// @remarks None
  std::string Authority() const;

  /// @brief Returns the scheme and authority characteristics.
  /// @param [out] _rScheme_S : returns a string representing the scheme.
  /// @return A BOF_SOCKET_ADDRESS_COMPONENT containing the scheme characteristics.
  /// @remarks None
  const BOF_SOCKET_ADDRESS_COMPONENT &SchemeAuthority(std::string &_rSchemeAuthority_S) const;

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

  /// @brief Returns the query characteristics.
  /// @param [out] _rQueryParam_S : return a string representing the query.
  /// @return A map of string containing the query parameters.
  /// @remarks None
  const std::map<std::string, std::string> &QueryParamCollection(std::string &_rQueryParam_S) const;
  const std::map<std::string, std::string> &QueryParamCollection() const;

  /// @brief Returns the fragment part of the uri.
  /// @return A string containing the fragment.
  /// @remarks None
  const std::string &Fragment() const;

  /// @brief Convert an uri instance into a printable string.
  /// @param [in] _QueryDelimiter_c specify the character to use to separate the different option.
  /// @return A string representing the uti instance.
  /// @remarks None.
  std::string ToString() const;

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
  /// @brief Initializes all the class instance members.
  /// @param [in] _rUri_S A string containing the uri
  /// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
  /// @remarks None
  BOFERR InitUriField(const std::string &_rUri_S);

  /// @brief Deserialize a query param string into a map of parameter value.
  /// @param [in] _rQueryParam_S A string containing the query param
  /// @param [out] _rQueryParamCollection return a collection of query key values
  /// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
  /// @remarks None
  BOFERR ExtractQueryParamIntoCollection(const std::string &_rQueryParam_S, std::map<std::string, std::string> &_rQueryParamCollection);
};
END_BOF_NAMESPACE()