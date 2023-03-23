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

#include <bofstd/bofparameter.h>

#include <functional>
#include <memory>
#include <vector>

/*
#include <fstream>
#include <json/json.h>
File:
  "JsonType": {
    "bool": true,
    "decimal": 10,
    "float": 3.14,
    "string": "Hello world !\n",
    "null": null,
    "employee": {
      "name": "John",
      "age": 30,
      "city": "New York"
    },
    "employees": [ "John", "Anna", "Peter" ]
  }

void PrintRoot(Json::Value _Root)
{
  Json::Value::iterator It = _Root.begin();
  std::string name;
  int i = 0;

  for (It = _Root.begin(); It != _Root.end(); It++)
  {
    name = _Root.getMemberNames()[i];
    if (_Root[name].isInt())
    {
      std::cout << "MemberName " << name << " int " << std::endl;
      std::cout << _Root[name].asInt() << std::endl;
    }
    else if (_Root[name].isDouble())
    {
      std::cout << "MemberName " << name << " double " << std::endl;
      std::cout << _Root[name].asDouble() << std::endl;
    }
    else if (_Root[name].isBool())
    {
      std::cout << "MemberName " << name << " bool " << std::endl;
      std::cout << _Root[name].asBool() << std::endl;
    }
    else if (_Root[name].isArray())
    {
      std::cout << "MemberName " << name << " array size " << _Root[name].size() << std::endl;
      int size = _Root[name].size();
      for (int i = 0; i < size; i++)
      {
        Json::Value dat1 = _Root[name];
        PrintRoot(dat1.get(i, _Root[name]));
      }
      std::cout << "MemberName " << name << " array is object " << std::endl;
    }
    else if (_Root[name].isInt64())
    {
      std::cout << "MemberName " << name << " array " << std::endl;
      std::cout << _Root[name].asInt64() << std::endl;
    }
    else if (_Root[name].isString())
    {
      std::cout << "MemberName " << name << " string " << std::endl;
      std::cout << _Root[name].asString() << std::endl;
    }
    else if (_Root[name].isObject())
    {
      std::cout << "MemberName " << name << " object " << std::endl;
      PrintRoot(_Root[name]);
      //std::cout << root[name].asCString() << std::endl;
    }
    else
    {
      std::cout << "MemberName " << name << " unknown type " << std::endl;
      //std::cout << root[name].asCString() << std::endl;
    }
    i++;
  }
}

TEST(JsonParser_Test, JsonType)
{
  Json::Value Root, Val;
  std::ifstream Ifs;
  Json::CharReaderBuilder JsonReader;
  JSONCPP_STRING JsonErr;

  Ifs.open("./data/jsonparser.json");
  JsonReader["collectComments"] = true;
  if (!parseFromStream(JsonReader, Ifs, &Root, &JsonErr))
  {
    std::cout << JsonErr << std::endl;
    Ifs.close();
  }
  else
  {
    Ifs.close();

    Val = Root["JsonType"];
    Val = Root["MmgwSetting"];
    PrintRoot(Root);
  }
}
*/

BEGIN_BOF_NAMESPACE()
typedef std::function<bool(BOFERR _Sts_E, const BOFPARAMETER &rJsonEntry_X, const char *_pValue_c)> BOFJSONPARSER_ERROR_CALLBACK;

class BOFSTD_EXPORT BofJsonParser
{
  // Opaque pointer design pattern: all public and protected stuff goes here ...
public:
  BofJsonParser(const std::string &_rJsonInput_S);

  BofJsonParser &operator=(const BofJsonParser &) = delete; // Disallow copying
  BofJsonParser(const BofJsonParser &) = delete;

  BOFERR ToByte(const std::vector<BOFPARAMETER> &_rJsonSchema_X, const BOFPARAMETER_PARSE_CALLBACK _ParseCallback_O, const BOFJSONPARSER_ERROR_CALLBACK _ErrorCallback_O);

  const char *GetFirstElementFromOid(const char *_pOid_c);

  const char *GetNextElementFromOid();

  bool IsValid();

  std::string RootName();                                       // From the parsed string
  static std::string S_RootName(const std::string &_rJsonIn_S); // From a string
  virtual ~BofJsonParser();

  BofJsonParser &operator=(const std::string &_rJsonInput_S);

  // Opaque pointer design pattern: opaque type here
private:
  class JsonParserImplementation;

  std::unique_ptr<JsonParserImplementation> mpuJsonParserImplementation;
};

END_BOF_NAMESPACE()