/*
 * Copyright (Sts_i) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the BofXmlWriter class
 *
 * Name:        bofxmlwriter.cpp
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
#include <bofstd/bofxmlwriter.h>
#include <bofstd/bofstring.h>
#include <bofstd/boffs.h>

#include <ixml.h>

BEGIN_BOF_NAMESPACE()

// Opaque pointer design pattern: defined privately here all private data and functions: all of these can now change without recompiling callers ...
class BofXmlWriter::XmlWriterImplementation
{
public:
  XmlWriterImplementation()
  {
  }

  ~XmlWriterImplementation()
  {
  }

  BOFERR FromByte(const std::string &_rXmlDeclare_S, const std::vector<BOFPARAMETER> &_rXmlSchema_X, std::string &_rXmlOutput_S)
  {
    BOFERR Rts_E = BOF_ERR_EINVAL;
    uint32_t i_U32, j_U32, Index_U32, NbActiveArrayElement_U32, FirstArrayEntry_U32, LastArrayEntry_U32, VectorCapacity_U32;
    char pOid_c[1024];
    bool Finish_B, ItIsAnArray_B;
    char pXmlValue_c[0x10000];
    IXML_Element *ppXmlElementPerTag_X[128]; // see ppSubTag_c
    IXML_Document *pXmlDoc_X;
    IXML_Element *pXmlRoot_X, *pXmlElement_X, *pXmlParent_X, *pXmlArg_X;
    DOMString pXmlContent;
    std::vector<std::string> SubTagList;

    pXmlDoc_X = nullptr;
    pXmlRoot_X = nullptr;
    LastArrayEntry_U32 = 0;

    Rts_E = BOF_ERR_NO_ERROR;

    snprintf(pOid_c, sizeof(pOid_c), "%s", _rXmlSchema_X[0].Path_S.c_str());
    SubTagList = Bof_StringSplit(pOid_c, ".");
    // Get base xml key
    if (SubTagList.size() > 0)
    {
      // Create doc
      Rts_E = (ixmlDocument_createDocumentEx(&pXmlDoc_X) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_CREATE;
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        // create root
//				printf("DBG: create0 %s\r\n", SubTagList[0].c_str());
        Rts_E = (ixmlDocument_createElementEx(pXmlDoc_X, SubTagList[0].c_str(), &pXmlRoot_X) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_CREATE;
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          // Add root ele
          Rts_E = (ixmlElement_setAttribute(pXmlRoot_X, "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance") == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_SET;
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            Rts_E = (ixmlElement_setAttribute(pXmlRoot_X, "xmlns:xsd", "http://www.w3.org/2001/XMLSchema") == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_SET;
            if (Rts_E == BOF_ERR_NO_ERROR)
            {
              Rts_E = (ixmlNode_appendChild(reinterpret_cast<IXML_Node *> (pXmlDoc_X), reinterpret_cast<IXML_Node *> (pXmlRoot_X)) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_SET;
            }
          }
        }
      }
    }                                                           // if (Bof_SplitString(pOid_c, '.', &NbSubTag_U32, &ppSubTag_c[0]) == 0)

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      // for each entry in schema
      for (i_U32 = 0; i_U32 < _rXmlSchema_X.size(); i_U32++)
      {
        Rts_E = BOF_ERR_FORMAT;
        snprintf(pOid_c, sizeof(pOid_c), "%s", _rXmlSchema_X[i_U32].Path_S.c_str());
        SubTagList = Bof_StringSplit(pOid_c, ".");
        // Get base xml key
        if (SubTagList.size() > 0)
        {
          // create base xml key element
          ppXmlElementPerTag_X[0] = pXmlRoot_X;

          Rts_E = BOF_ERR_NO_ERROR;
          for (j_U32 = 1; j_U32 < SubTagList.size(); j_U32++)
          {
            //						printf("DBG: create[%d] %s\r\n", j_U32, SubTagList[j_U32].c_str());
            Rts_E = (ixmlDocument_createElementEx(pXmlDoc_X, SubTagList[j_U32].c_str(), &ppXmlElementPerTag_X[j_U32]) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_CREATE;
            if (Rts_E == BOF_ERR_NO_ERROR)
            {
              // Link it
              Rts_E = (ixmlNode_appendChild(reinterpret_cast<IXML_Node *> (ppXmlElementPerTag_X[j_U32 - 1]), reinterpret_cast<IXML_Node *> (ppXmlElementPerTag_X[j_U32])) == 0) ? BOF_ERR_NO_ERROR
                : BOF_ERR_CREATE;
            }
            if (Rts_E != BOF_ERR_NO_ERROR)
            {
              break;
            }
          }
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            pXmlParent_X = ppXmlElementPerTag_X[SubTagList.size() - 1]; // If array->parent of all element

            if (_rXmlSchema_X[i_U32].ArrayCapacity_U32)
            {
              // It is an array: element def should be continuous
              ItIsAnArray_B = true;
              NbActiveArrayElement_U32 = _rXmlSchema_X[i_U32].ArrayCapacity_U32;
              FirstArrayEntry_U32 = i_U32;
              LastArrayEntry_U32 = i_U32;

              for (j_U32 = i_U32 + 1; j_U32 < _rXmlSchema_X.size(); j_U32++)
              {
                if (!strcmp(_rXmlSchema_X[i_U32].Path_S.c_str(), _rXmlSchema_X[j_U32].Path_S.c_str()))
                {
                  LastArrayEntry_U32++;
                }
                else
                {
                  break;
                }
              }
            }                                                           // if (_rXmlSchema_X[i_U32].ArrayCapacity_U32)
            else
            {
              // Single element
              ItIsAnArray_B = false;
              NbActiveArrayElement_U32 = 0;
              FirstArrayEntry_U32 = i_U32;
              LastArrayEntry_U32 = i_U32;
            }

            Index_U32 = 0;

            do
            {
              Finish_B = true;
              // For each array entry
              for (j_U32 = FirstArrayEntry_U32; j_U32 <= LastArrayEntry_U32; j_U32++)
              {
                Rts_E = BOF_ERR_FORMAT;

                // get value
                if (BofParameter::S_ParameterToString(Index_U32, _rXmlSchema_X[j_U32], false, sizeof(pXmlValue_c), pXmlValue_c, VectorCapacity_U32))
                {
                  if (NbActiveArrayElement_U32 == 0xFFFFFFFF)
                  {
                    NbActiveArrayElement_U32 = VectorCapacity_U32;
                  }
                  if (Bof_IsBitFlagSet(_rXmlSchema_X[j_U32].ArgFlag_E, BOFPARAMETER_ARG_FLAG::XML_ATTRIBUTE))
                  {
                    Rts_E = (ixmlElement_setAttribute(ppXmlElementPerTag_X[SubTagList.size() - 1], _rXmlSchema_X[j_U32].Name_S.c_str(), pXmlValue_c) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_CREATE;
                  }
                  else
                  {
                    //										printf("DBG: createFirstLast[%d] %s\r\n", j_U32, _rXmlSchema_X[j_U32].Name_S.c_str());
                    Rts_E = (ixmlDocument_createElementEx(pXmlDoc_X, _rXmlSchema_X[j_U32].Name_S.c_str(), &pXmlElement_X) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_CREATE;
                    if (Rts_E == BOF_ERR_NO_ERROR)
                    {
                      Rts_E = (ixmlNode_appendChild(reinterpret_cast<IXML_Node *> (pXmlParent_X), reinterpret_cast<IXML_Node *> (pXmlElement_X)) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_CREATE;

                      if (Rts_E == BOF_ERR_NO_ERROR)
                      {
                        Rts_E = (ixmlDocument_createTextNodeEx(pXmlDoc_X, pXmlValue_c, reinterpret_cast<IXML_Node **> (&pXmlArg_X)) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_CREATE;
                        if (Rts_E == BOF_ERR_NO_ERROR)
                        {
                          Rts_E = (ixmlNode_appendChild(reinterpret_cast<IXML_Node *> (pXmlElement_X), reinterpret_cast<IXML_Node *> (pXmlArg_X)) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_CREATE;
                        }
                      }
                    }
                  }
                }
                else
                {
                  NbActiveArrayElement_U32 = 0;

                  int ClildIndex_i;
                  for (ClildIndex_i = static_cast<int>(SubTagList.size() - 1); ClildIndex_i > 0; ClildIndex_i--)
                  {
                    //										printf("DBG: remove %s\r\n", ppXmlElementPerTag_X[ClildIndex_i]->tagName);

                    ixmlNode_removeChild(reinterpret_cast<IXML_Node *>(ppXmlElementPerTag_X[ClildIndex_i - 1]), reinterpret_cast<IXML_Node *>(ppXmlElementPerTag_X[ClildIndex_i]), nullptr);
                    //										ixmlElement_free(ppXmlElementPerTag_X[j_U32]);
                  }
                  Rts_E = BOF_ERR_NO_ERROR;
                  break;
                }
                if (Rts_E != BOF_ERR_NO_ERROR)
                {
                  break;
                }
              }                                                                   // for (j_U32 = FirstArrayEntry_U32; j_U32 <= LastArrayEntry_U32; j_U32++)

              if (Rts_E == BOF_ERR_NO_ERROR)
              {
                if (ItIsAnArray_B)                                                // array
                {
                  if (Index_U32 < NbActiveArrayElement_U32)
                  {
                    //										printf("DBG: createArrayEntry[%d] %s\r\n", Index_U32, SubTagList[SubTagList.size() - 1].c_str());
                    Index_U32++;
                    // Create new array entry->new parent
                    Rts_E = (ixmlDocument_createElementEx(pXmlDoc_X, SubTagList[SubTagList.size() - 1].c_str(), &ppXmlElementPerTag_X[SubTagList.size() - 1]) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_CREATE;
                    if (Rts_E == BOF_ERR_NO_ERROR)
                    {
                      // Link it
                      Rts_E = (ixmlNode_appendChild(reinterpret_cast<IXML_Node *> (ppXmlElementPerTag_X[SubTagList.size() - 2]),
                               reinterpret_cast<IXML_Node *> (ppXmlElementPerTag_X[SubTagList.size() - 1])) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_CREATE;
                      if (Rts_E == BOF_ERR_NO_ERROR)
                      {
                        pXmlParent_X = ppXmlElementPerTag_X[SubTagList.size() - 1]; // If array->parent of all element

                        Finish_B = (Index_U32 == NbActiveArrayElement_U32);
                      }
                    }
                  }
                }
              }
            } while ((!Finish_B) && (Rts_E == BOF_ERR_NO_ERROR));
          } //if (Rts_E == BOF_ERR_NO_ERROR)
          if (Rts_E != BOF_ERR_NO_ERROR)
          {
            break;
          }
        }     //if (SubTagList.size() > 0)
        i_U32 = LastArrayEntry_U32;    //Normal et voulu
      }                                                                           // for (i_U32 = 0; i_U32 < _rXmlSchema_X.size(); i_U32++)

      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        // pXmlContent = ixmlPrintDocument(pXmlDoc_X);
        pXmlContent = ixmlNodetoString(reinterpret_cast<IXML_Node *> (pXmlDoc_X));

        if (pXmlContent != nullptr)
        {
          _rXmlOutput_S = (_rXmlDeclare_S != "") ? _rXmlDeclare_S : "<?xml version='1.0'?>\r\n";
          _rXmlOutput_S += pXmlContent;
          ixmlFreeDOMString(pXmlContent);
        }
      }                                                                           // if (Rts_i == 0)
    }                                                                             // if (Rts_i == 0)

    if (pXmlDoc_X)
    {
      ixmlDocument_free(pXmlDoc_X);
    }
    return Rts_E;
  }
};

// Opaque pointer design pattern: ... set Implementation values ...
BofXmlWriter::BofXmlWriter()
  : mpuXmlWriterImplementation(new XmlWriterImplementation())
{}


BofXmlWriter::~BofXmlWriter()
{}

BOFERR BofXmlWriter::FromByte(const std::string &_rXmlDeclare_S, const std::vector<BOFPARAMETER> &_rXmlSchema_X, std::string &_rXmlOutput_S)
{
  return mpuXmlWriterImplementation->FromByte(_rXmlDeclare_S, _rXmlSchema_X, _rXmlOutput_S);
}


END_BOF_NAMESPACE()