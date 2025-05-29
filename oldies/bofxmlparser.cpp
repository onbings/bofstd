/*
 * Copyright (Sts_i) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the BofXmlParser class
 *
 * Name:        bofxmlparser.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#include <bofstd/bofstring.h>
#include <bofstd/bofxmlparser.h>

#include <ixml.h>

#include <regex>

BEGIN_BOF_NAMESPACE()

// Opaque pointer design pattern: defined privately here all private data and functions: all of these can now change without recompiling callers ...
class BofXmlParser::XmlParserImplementation
{
public:
  IXML_Document *mpXmlDoc_X;
  int mSts_i;
  char mpTag_c[1024];
  std::vector<std::string> mSubTagList;
  IXML_Element *mpLastXmlElementFound_X;
  IXML_Node *mpLastXmlNodeFound_X;
  bool mLastElementWasAnXmlAttribute_B;

  XmlParserImplementation(const std::string &_rXmlInput_S)
  {
    Open(_rXmlInput_S);
  }

  ~XmlParserImplementation()
  {
    Close();
  }

  void Open(const std::string &_rXmlInput_S)
  {
    mpLastXmlElementFound_X = nullptr;
    mpLastXmlNodeFound_X = nullptr;
    mLastElementWasAnXmlAttribute_B = false;
    mpXmlDoc_X = nullptr;
    mSts_i = ixmlParseBufferEx(_rXmlInput_S.c_str(), &mpXmlDoc_X);
  }

  void Close()
  {
    if (mpXmlDoc_X)
    {
      ixmlDocument_free(mpXmlDoc_X);
    }
    mpXmlDoc_X = nullptr;
  }

  BOFERR ToByte(const std::vector<BOFPARAMETER> &_rXmlSchema_X, const BOFPARAMETER_PARSE_CALLBACK _ParseCallback_O, const BOFXMLPARSER_ERROR_CALLBACK _ErrorCallback_O)
  {
    BOFERR Rts_E = BOF_ERR_INVALID_STATE;
    uint32_t i_U32, Index_U32;
    char pOid_c[1024];
    bool Finish_B;
    const char *pXmlValue_c;
    BOFPARAMETER XmlParam_X;

    if (mpXmlDoc_X)
    {
      for (i_U32 = 0; i_U32 < _rXmlSchema_X.size(); i_U32++)
      {
        Index_U32 = 0;
        snprintf(pOid_c, sizeof(pOid_c), "%s.%s", _rXmlSchema_X[i_U32].Path_S.c_str(), _rXmlSchema_X[i_U32].Name_S.c_str());
        pXmlValue_c = GetFirstElementFromOid(Bof_IsAnyBitFlagSet(_rXmlSchema_X[i_U32].ArgFlag_E, BOFPARAMETER_ARG_FLAG::XML_ATTRIBUTE), pOid_c);
        Finish_B = false;

        Rts_E = BOF_ERR_NO_ERROR;
        while ((!Finish_B) && (pXmlValue_c) && (Rts_E == BOF_ERR_NO_ERROR))
        {
          Finish_B = true;
          XmlParam_X = _rXmlSchema_X[i_U32];
          XmlParam_X.ArgFlag_E |= BOFPARAMETER_ARG_FLAG::COMA_IS_NOT_A_SEPARATOR;
          Rts_E = BofParameter::S_Parse(Index_U32, XmlParam_X, pXmlValue_c, _ParseCallback_O);

          if (Rts_E != BOF_ERR_NO_ERROR)
          {
            if (_ErrorCallback_O != nullptr)
            {
              _ErrorCallback_O(Rts_E, _rXmlSchema_X[i_U32], pXmlValue_c);
            }
          }
          else
          {
            if (_rXmlSchema_X[i_U32].ArrayCapacity_U32) // Array
            {
              Index_U32++;
              if (Index_U32 < _rXmlSchema_X[i_U32].ArrayCapacity_U32) // NbEntry is 0 for non array descriptor
              {
                pXmlValue_c = GetNextElementFromOid();

                if (pXmlValue_c)
                {
                  Finish_B = false;
                }
              }
              else
              {
                Rts_E = BOF_ERR_NO_ERROR; // End of arraybo
              }
            }
          }
        } // while
        if (Rts_E != BOF_ERR_NO_ERROR)
        {
          break;
        }
      } // for
    }
    return Rts_E;
  }

  // Description is an oid (. separated path) to the xml element->oid leave between () is a root mlielement or an attribute

  /*
   * _pOid_c is a string containing a dot separated list of xml tags which describe the path to reach an XML element from the root node
   * ex: MulFtpUserSetting.catalog.book.publish_date points to the first occurrence of <publish_date>2000-10-01</publish_date>
   *
   * <MulFtpUserSetting xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
   * <DeployIpAddress>192.168.1.21</DeployIpAddress>
   * <catalog>
   *  <book id="bk101">
   *    <author>Gambardella, Matthew</author>
   *    <publish_date>2000-10-01</publish_date>    <<<<<<------------------MulFtpUserSetting.catalog.book.publish_date
   *  </book>
   * </catalog>
   * </MulFtpUserSetting>
   *
   */

  const char *GetFirstElementFromOid(bool _ItIsAnAttributte_B, const char *_pOid_c)
  {
    const char *pRts_c = nullptr;
    IXML_Element *pXmlElement_X;
    IXML_Node *pXmlNode_X;
    uint32_t i_U32;

    if ((mpXmlDoc_X) && (_pOid_c) && (strlen(_pOid_c) < sizeof(mpTag_c)))
    {
      // mNbSubTag_U32           = 0;
      mpLastXmlElementFound_X = nullptr;
      mpLastXmlNodeFound_X = nullptr;

      strcpy(mpTag_c, _pOid_c);
      // mNbSubTag_U32 = sizeof( mppSubTag_c ) / sizeof( mppSubTag_c[0] );

      // if (Bof_SplitString(mpTag_c, '.', &mNbSubTag_U32, &mppSubTag_c[0]) == 0)
      mSubTagList = Bof_StringSplit(mpTag_c, ".");
      if (mSubTagList.size() > 0)
      {
        pXmlElement_X = reinterpret_cast<IXML_Element *>(mpXmlDoc_X);
        pXmlNode_X = nullptr;

        for (i_U32 = 0; i_U32 < mSubTagList.size(); i_U32++)
        {
          if (i_U32 == (mSubTagList.size() - 1))
          {
            mLastElementWasAnXmlAttribute_B = _ItIsAnAttributte_B;
            if (_ItIsAnAttributte_B)
            {
              if (ixmlElement_getAttribute(pXmlElement_X, mSubTagList[mSubTagList.size() - 1].c_str()))
              {
                if ((pXmlElement_X->n.firstAttr) && (pXmlElement_X->n.firstAttr->nodeType == eATTRIBUTE_NODE))
                {
                  pXmlNode_X = pXmlElement_X->n.firstAttr;
                }
              }
            }
            else
            {
              pXmlElement_X = ixmlDocument_getElementById(reinterpret_cast<IXML_Document *>(pXmlElement_X), mSubTagList[i_U32].c_str());
              if (pXmlElement_X)
              {
                if ((pXmlElement_X->n.firstChild) && (pXmlElement_X->n.firstChild->nodeType == eTEXT_NODE))
                {
                  pXmlNode_X = pXmlElement_X->n.firstChild;
                }
              }
            }
          }
          else
          {
            pXmlElement_X = ixmlDocument_getElementById(reinterpret_cast<IXML_Document *>(pXmlElement_X), mSubTagList[i_U32].c_str());
          }

          if (!pXmlElement_X)
          {
            break;
          }
        } // for (i_U32 = 0; i_U32 < NbSubTag_U32; i_U32++)
        mpLastXmlElementFound_X = pXmlElement_X;
        mpLastXmlNodeFound_X = pXmlNode_X;

        if (pXmlNode_X)
        {
          pRts_c = pXmlNode_X->nodeValue;
        }
      } // if (Bof_SplitString(pTag_c, '.', &NbSubTag_U32, &ppSubTag_c) == 0)
    }   // if ((mpXmlDoc_X) && (_pOid_c) && (strlen(_pOid_c) < sizeof(pTag_c)))
    return pRts_c;
  }

  const char *GetNextElementFromOid()
  {
    const char *pRts_c = nullptr;
    IXML_Element *pXmlElement_X, *pParentXmlElement_X, *pSiblingXmlElement_X;
    IXML_Node *pXmlNode_X;
    bool NextNodeExist_B;

    if ((mpXmlDoc_X) && (mpLastXmlNodeFound_X) && (mpLastXmlElementFound_X))
    {
      pXmlNode_X = nullptr;

      pXmlElement_X = mpLastXmlElementFound_X;

      NextNodeExist_B = false;
      pParentXmlElement_X = reinterpret_cast<IXML_Element *>(pXmlElement_X->n.parentNode);

      if (pParentXmlElement_X)
      {
        /* case loop on book author path
         * <catalog>
         * <book id = "bk101">
         *  <author>Gambardella, Matthew< / author> <<<<<<<<<<<<<<<<<<<---------------------
         * </book>
         */
        pSiblingXmlElement_X = reinterpret_cast<IXML_Element *>(pParentXmlElement_X->n.nextSibling);

        if (pSiblingXmlElement_X)
        {
          if (strcmp(pSiblingXmlElement_X->n.nodeName, pXmlElement_X->n.nodeName))
          {
            pXmlElement_X = pSiblingXmlElement_X;
            NextNodeExist_B = true;
          }
        }
        else
        {
          /* case loop on book id path
           * <catalog>
           * <book id = "bk101">      <<<<<<<<<<<<<<<<<<<---------------------
           *    <author>Gambardella, Matthew< / author>
           * </book>
           */
          pSiblingXmlElement_X = reinterpret_cast<IXML_Element *>(pXmlElement_X->n.nextSibling);

          if (pSiblingXmlElement_X)
          {
            if (!strcmp(pSiblingXmlElement_X->n.nodeName, pXmlElement_X->n.nodeName)) // <book id = "bk101">
            {
              pXmlElement_X = pSiblingXmlElement_X;
              NextNodeExist_B = true;
            }
          }
        }
      }

      if (NextNodeExist_B)
      {
        if (mLastElementWasAnXmlAttribute_B)
        {
          pXmlElement_X = ixmlDocument_getElementById(reinterpret_cast<IXML_Document *>(pXmlElement_X), mSubTagList[mSubTagList.size() - 2].c_str());

          if (pXmlElement_X)
          {
            if (!ixmlElement_getAttribute(pXmlElement_X, mSubTagList[mSubTagList.size() - 1].c_str()))
            {
              pXmlElement_X = nullptr;
            }
            else
            {
              if ((pXmlElement_X->n.firstAttr) && (pXmlElement_X->n.firstAttr->nodeType == eATTRIBUTE_NODE))
              {
                pXmlNode_X = pXmlElement_X->n.firstAttr;
              }
            }
          }
        }
        else
        {
          pXmlElement_X = ixmlDocument_getElementById(reinterpret_cast<IXML_Document *>(pXmlElement_X), mSubTagList[mSubTagList.size() - 1].c_str());

          if (pXmlElement_X)
          {
            if ((pXmlElement_X->n.firstChild) && (pXmlElement_X->n.firstChild->nodeType == eTEXT_NODE))
            {
              pXmlNode_X = pXmlElement_X->n.firstChild;
            }
          }
        }
        mpLastXmlElementFound_X = pXmlElement_X;
        mpLastXmlNodeFound_X = pXmlNode_X;

        if (pXmlNode_X)
        {
          pRts_c = pXmlNode_X->nodeValue;
        }
      }
    }
    return pRts_c;
  }

  bool IsValid()
  {
    return ((mSts_i == IXML_SUCCESS) && (mpXmlDoc_X != nullptr));
  }

  std::string RootName()
  {
    std::string Rts_S;

    if ((mpXmlDoc_X) && (mpXmlDoc_X->n.firstChild) && (mpXmlDoc_X->n.firstChild->nodeName))
    {
      Rts_S = mpXmlDoc_X->n.firstChild->nodeName;
    }
    return Rts_S;
  }

  void OperatorAssign(const std::string &_rXmlInput_S)
  {
    Close();
    Open(_rXmlInput_S);
  }
};

// Opaque pointer design pattern: ... set Implementation values ...
BofXmlParser::BofXmlParser(const std::string &_rXmlInput_S) : mpXmlParserImplementation_O(new XmlParserImplementation(_rXmlInput_S))
{
}

BofXmlParser::~BofXmlParser()
{
}

BOFERR BofXmlParser::ToByte(const std::vector<BOFPARAMETER> &_rXmlSchema_X, const BOFPARAMETER_PARSE_CALLBACK _ParseCallback_O, const BOFXMLPARSER_ERROR_CALLBACK _ErrorCallback_O)
{
  return mpXmlParserImplementation_O->ToByte(_rXmlSchema_X, _ParseCallback_O, _ErrorCallback_O);
}

const char *BofXmlParser::GetFirstElementFromOid(bool _ItIsAnAttributte_B, const char *_pOid_c)
{
  return mpXmlParserImplementation_O->GetFirstElementFromOid(_ItIsAnAttributte_B, _pOid_c);
}

const char *BofXmlParser::GetNextElementFromOid()
{
  return mpXmlParserImplementation_O->GetNextElementFromOid();
}

bool BofXmlParser::IsValid()
{
  return mpXmlParserImplementation_O->IsValid();
}

std::string BofXmlParser::RootName()
{
  return mpXmlParserImplementation_O->RootName();
}

//<?xml version="1.0" encoding="utf-8"?>
//<--Comment-->
//<MulFtpUserSetting xmlns : xsi = "http://www.w3.org/2001/XMLSchema-instance" xmlns : xsd = "http://www.w3.org/2001/XMLSchema">
std::string BofXmlParser::S_RootName(const std::string &_rXmlIn_S)
{
  std::string Rts_S;
  static std::regex S_RegExXmlTagExcludingCommentPrologOrEnd("<([^-\?/][^ \t\n\v\f\r>]*)"); // Static as it can takes time (on gcc 4.9 for example)
  std::smatch MatchString;

  if (std::regex_search(_rXmlIn_S, MatchString, S_RegExXmlTagExcludingCommentPrologOrEnd))
  {
    if (MatchString.size() == 2) // Size=2 because we have a capture group ()
    {
      Rts_S = MatchString[1].str();
    }
  }
  //	Rts_S=Bof_StringTrim(Rts_S);
  return Rts_S;
}

BofXmlParser &BofXmlParser::operator=(const std::string &_rXmlInput_S)
{
  mpXmlParserImplementation_O->OperatorAssign(_rXmlInput_S);
  return *this;
}
END_BOF_NAMESPACE()