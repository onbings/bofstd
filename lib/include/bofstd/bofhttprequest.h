/*
 * Copyright (c) 2013-2033, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bofhttprequest module of the bofstd library
 *
 * Name:        bofhttprequest.h
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

#include <map>
#include <string>

BEGIN_BOF_NAMESPACE()

/*
HTTP (Hypertext Transfer Protocol) defines several request methods or verbs that indicate the desired action to be performed on a resource identified by a URL. 
Each request type corresponds to a specific action. Some of the commonly used HTTP request methods are:
- GET: Used to retrieve data from the server. The request URL includes the data to be retrieved. It should not have any side effects on the server.
- POST: Used to submit data to be processed to the server. The data is sent in the request body. It can be used to create new resources or perform 
  other actions that might have side effects.
- PUT: Used to update a resource on the server. It usually updates an existing resource or creates a new one if it doesn't exist.
- PATCH: Similar to PUT, but it's used for partial updates to a resource. It updates only the fields provided in the request. Used to apply partial modifications to a resource.
- DELETE: Used to request the removal of a resource on the server.
- HEAD: Similar to GET, but it retrieves only the headers of the response, not the actual data. It's often used to check if a resource has changed.
- OPTIONS: Used to request information about the communication options for the target resource. It can be used to determine which HTTP methods are supported by the server.
- CONNECT: Used to establish a network connection to a resource, often used with proxy servers.
- TRACE: Used to retrieve a diagnostic trace of the actions performed by the intermediate servers.

These methods define the purpose of the request and how the server should handle it. When making HTTP requests, you choose the appropriate request method based on the action you want to perform on the server's resources.
*/

enum BOF_HTTP_REQUEST_TYPE
{
  BOF_HTTP_REQUEST_UNKNOWN = 0,
  BOF_HTTP_REQUEST_GET,
  BOF_HTTP_REQUEST_POST,
  BOF_HTTP_REQUEST_PUT,
  BOF_HTTP_REQUEST_PATH,
  BOF_HTTP_REQUEST_DELETE,
  BOF_HTTP_REQUEST_HEAD,
  BOF_HTTP_REQUEST_OPTIONS,
  BOF_HTTP_REQUEST_CONNECT,
  BOF_HTTP_REQUEST_TRACE,
  BOF_HTTP_REQUEST_DBG_ECHO,  //For user Debug Echo function
  BOF_HTTP_REQUEST_MAX,

};
/// @brief This class implements an http request parser. 
/// Cf BofHttpRequest
/// In brief it is a string formatted as follow:
/// Http Request = method (GET,POST,...) path ["?" query] ["#" fragment]
/// ex 
/// GET /forum/questions/?tag=networking&order=newest#top
/// GET /forum/questions/?tag=networking&order=newest,
/// GET /forum/questions
///	METHOD /forum/questions/?tag=networking&order=newest#top
/// └─┬─┘  └───────┬───────┘ └───────────┬─────────────┘ └┬┘
/// method        path                 query           fragment
///
/// 
class BOFSTD_EXPORT BofHttpRequest
{
private:
  std::string mMethod_S;
  BOF_HTTP_REQUEST_TYPE mMethod_E;
  BofPath mPath;
  std::map<std::string, std::string> mQueryParamCollection;
  std::string mFragment_S;

  char mQueryParamDelimiter_c = '&';
  bool mValid_B = false;

public:
  /// @brief Initializes a new instance of the HttpRequest class.
  /// @remarks Default constructor.
  BofHttpRequest();

  /// @brief Initializes a new instance of the HttpRequest class.
  /// @param _pHttpRequest_c A pointer to a char array containing the http request
  /// @remarks None.
  BofHttpRequest(const char *_pHttpRequest_c);

  /// @brief Initializes a new instance of the HttpRequest class.
  /// @param _rHttpRequest_S A string containing the http request
  /// @remarks None.
  BofHttpRequest(const std::string &_rHttpRequest_S);

  /// @brief Initializes a new instance of the HttpRequest class.
  /// @param _rMethod_S Specifies the Http request method.
  /// @param _rPath Specifies the path.
  /// @param _rQueryParamCollection Specifies the query collection.
  /// @param _rFragment_S Specifies the fragment.
  /// @remarks None.
  BofHttpRequest(const std::string &_rMethod_S, const BofPath &_rPath, std::map<std::string, std::string> &_rQueryParamCollection, const std::string &_rFragment_S);
  BofHttpRequest(BOF_HTTP_REQUEST_TYPE _Method_E, const BofPath &_rPath, std::map<std::string, std::string> &_rQueryParamCollection, const std::string &_rFragment_S);

  /// @brief Initializes a new instance of the HttpRequest class.
  /// @param _rOther_O A reference to another HttpRequest class instance.
  /// @remarks Copy constructor.
  BofHttpRequest(const BofHttpRequest &_rOther_O);

  /// @brief Initializes a new instance of the HttpRequest class.
  /// @param _rrOther_O A rvalue reference to another HttpRequest class instance.
  /// @remarks Move constructor.
  BofHttpRequest(BofHttpRequest &&_rrOther_O);

  /// @brief Initializes a new instance of the HttpRequest class.
  /// @param _rOther_O A reference to another HttpRequest class instance.
  /// @remarks  Copy assignment operator.
  BofHttpRequest &operator=(const BofHttpRequest &_rOther_O); // Copy assignment operator

  /// @brief Create a new http request by assigning a new string value.
  /// @param _rNewHttpRequest_S A reference to a string containing the new http request.
  /// @return a modified class instance
  /// @remarks Assignment operator.
  BofHttpRequest &operator=(const std::string &_rNewHttpRequest_S);

  /// @brief Create a new http request by assigning a new char array value.
  /// @param _pNewHttpRequest_c A pointer to a char array containing the new http request.
  /// @return a modified class instance
  /// @remarks Assignment operator.
  BofHttpRequest &operator=(const char *_pNewHttpRequest_c);

  /// @brief Initializes a new instance of the HttpRequest class.
  /// @param _rrOther_O A rvalue reference to another HttpRequest class instance.
  /// @remarks Move constructor.
  BofHttpRequest &operator=(BofHttpRequest &&_rrOther_O);

  /// @brief Release all class instance resources.
  /// @remarks Virtual destructor.
  virtual ~BofHttpRequest();

  /// @brief Compares two object to check for equality
  /// @param _rOther_O A reference to another HttpRequest class instance.
  /// @return true if both object are equal
  /// @remarks Equal operator.
  bool operator==(const BofHttpRequest &_rOther_O) const;

  /// @brief Compares two object to check for difference
  /// @param _rOther_O A reference to another HttpRequest class instance.
  /// @return true if the objects are different (not equal)
  /// @remarks Different operator.
  bool operator!=(const BofHttpRequest &_rOther_O) const;

  static BOF_HTTP_REQUEST_TYPE S_RequestType(const char *_pRequest_c);
  static std::string S_RequestString(BOF_HTTP_REQUEST_TYPE _Method_E);
  /// @brief Check if the http request instance is well formed.
  /// @return true if the http request is valid.
  /// @remarks None
  bool IsValid() const;

  /// @brief Set the character used as séparator in the query string.
  /// @param [in] _QueryDelimiter_c : specifies the character used as séparator in the query string. (Must be ';' or '&').
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks None
  BOFERR SetQueryParamDelimiter(char _QueryParamDelimiter_c);

  /// @brief Set the method of the http request
  /// @param [in] _rScheme_S : specifies a string representing the scheme.
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks None
  BOFERR SetMethod(const std::string &_rMethod_S);
  BOFERR SetMethod(BOF_HTTP_REQUEST_TYPE _Method_E);

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

  /// @brief Set the query param collection of the http request.
  /// @param [in] _rQueryParamCollection : specifies the query collection.
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks None
  BOFERR SetQueryParamCollection(const std::map<std::string, std::string> &_rQueryParamCollection);

  /// @brief Set the query collection of the http request.
  /// @param [in] _rQueryParam_S : specifies the query collection with each key-value element separated by ';' or '&'
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks None
  BOFERR SetQueryParamCollection(const std::string &_rQueryParam_S);

  /// @brief Add a key value pair to the http request query collection.
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

  /// @brief Set the fragment part of the http request.
  /// @param [in] _rFragment_S : specifies the fragment.
  /// @return A BOFERR value which is BOF_ERR_NO_ERROR if the call is successful.
  /// @remarks None
  BOFERR SetFragment(const std::string &_rFragment_S);

  /// @brief Returns the character used as séparator in the query string.
  /// @return The character used in the query string.
  /// @remarks Must be ';' or '&'
  char QueryParamDelimiter() const;

  /// @brief Returns the method of the http request.
  /// @return A string containing the scheme characteristics.
  /// @remarks None
  const std::string &Method(BOF_HTTP_REQUEST_TYPE &_rMethod_E) const;
  /// @brief Returns the path characteristics.
  /// @param [out] _rPath_S : returns a string representing the path.
  /// @return A BOF_SOCKET_ADDRESS_COMPONENT containing the scheme characteristics.
  /// @remarks None
  const BofPath &Path(std::string &_rPath_S) const;
  const BofPath &Path() const;

  /// @brief Returns the query characteristics.
  /// @param [out] _rQueryParam_S : return a string representing the query.
  /// @return A map of string containing the query parameters.
  /// @remarks None
  const std::map<std::string, std::string> &QueryParamCollection(std::string &_rQueryParam_S) const;
  const std::map<std::string, std::string> &QueryParamCollection() const;

  /// @brief Returns the fragment part of the http request.
  /// @return A string containing the fragment.
  /// @remarks None
  const std::string &Fragment() const;

  /// @brief Convert an http request instance into a printable string.
  /// @param [in] _QueryDelimiter_c specify the character to use to separate the different option.
  /// @return A string representing the uti instance.
  /// @remarks None.
  std::string ToString() const;

private:
  /// @brief Initializes all the class instance members.
  /// @param [in] _rHttpRequest_S A string containing the http request
  /// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
  /// @remarks None
  BOFERR InitHttpRequestField(const std::string &_rHttpRequest_S);

  /// @brief Deserialize a query param string into a map of parameter value.
  /// @param [in] _rQueryParam_S A string containing the query param
  /// @param [out] _rQueryParamCollection return a collection of query key values
  /// @return A BOFERR value (0 or BOF_ERR_NO_ERROR if successful)
  /// @remarks None
  BOFERR ExtractQueryParamIntoCollection(const std::string &_rQueryParam_S, std::map<std::string, std::string> &_rQueryParamCollection);
};
END_BOF_NAMESPACE()