/*
 * Copyright (Sts_i) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the BofCommandLineParser class
 *
 * Name:        bofcommandlineparser.cpp
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
#include <bofstd/bofcommandlineparser.h>
#include <bofstd/bofgetopt.h>

#include <regex>

#if defined( _WIN32 )
#include <ws2tcpip.h> // for sockaddrin_6
#define snprintf    _snprintf
#else
#include <stdlib.h>
#endif

BEGIN_BOF_NAMESPACE()

BofCommandLineParser::BofCommandLineParser()
{
}


BofCommandLineParser::~BofCommandLineParser()
{
}


BOFERR BofCommandLineParser::ToByte(const int _Argc_i, char *const *_ppArgv_c, const std::vector<BOFPARAMETER> &_rCommandLineOption_X, const BOFPARAMETER_PARSE_CALLBACK _ParseCallback_O,
                                    const BOFCOMMANDLINEPARSER_ERROR_CALLBACK _ErrorCallback_O)
{
  BOFERR Rts_E = BOF_ERR_PARSER;
  int i, Sts_i, IndexInShortOpt_i, NbLongOpt_i, IndexInLongOpt_i, IndexInCommandLineOption_i;
  char pGetOptShort_c[256], pError_c[256], ShortOpt_c;
  const char *pOpt_c, *pOptVal_c;
  struct option pGetOptLong_X[512];


  if ((_ppArgv_c) && (_Argc_i))
  {
    Rts_E = BOF_ERR_NO_ERROR;

    if (_Argc_i > 1)
    {
      IndexInShortOpt_i = 0;
      NbLongOpt_i = 0;

      for (i = 0; i < static_cast<int>(_rCommandLineOption_X.size()); i++)
      {
        if (_rCommandLineOption_X[i].Name_S.size() == 1)
        {
          //Short option
          if ((IndexInShortOpt_i < static_cast<int>((sizeof(pGetOptShort_c) - 3))) && (_rCommandLineOption_X[i].Name_S != "?")) //Cannot use '?' as opt (special return value of getopt)
          {
            pGetOptShort_c[IndexInShortOpt_i++] = _rCommandLineOption_X[i].Name_S[0];

            if (Bof_IsBitFlagSet(_rCommandLineOption_X[i].ArgFlag_E, BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG))
            {
              pGetOptShort_c[IndexInShortOpt_i++] = ':';
            }
          }
          else
          {
            Rts_E = BOF_ERR_PARSER;  //-2
            break;
          }
        }
        else
        {
          //Long option
          if (NbLongOpt_i < (static_cast<int>(sizeof(pGetOptLong_X) - 2)))
          {
            pGetOptLong_X[NbLongOpt_i].flag = nullptr;
            pGetOptLong_X[NbLongOpt_i].has_arg = static_cast<int>(_rCommandLineOption_X[i].ArgFlag_E);
            pGetOptLong_X[NbLongOpt_i].name = strdup(_rCommandLineOption_X[i].Name_S.c_str());
            pGetOptLong_X[NbLongOpt_i].val = 0;

            NbLongOpt_i++;
          }
          else
          {
            Rts_E = BOF_ERR_PARSER;  // -3;
            break;
          }
        }
      }

      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        pGetOptShort_c[IndexInShortOpt_i] = 0;

        pGetOptLong_X[NbLongOpt_i].flag = 0;
        pGetOptLong_X[NbLongOpt_i].has_arg = 0;
        pGetOptLong_X[NbLongOpt_i].name = nullptr;
        pGetOptLong_X[NbLongOpt_i].val = 0;

        do
        {
          opterr = 0;                                                                                                                                                   //Callers store zero here to inhibit the error message `getopt' prints for unrecognized options.
          IndexInLongOpt_i = -1;                                                                                                                                                  // getopt_long stores the option index here.
          Sts_i = getopt_long(_Argc_i, _ppArgv_c, pGetOptShort_c, pGetOptLong_X, &IndexInLongOpt_i);

          if (Sts_i != -1)
          {
            pOptVal_c = optarg;
            IndexInCommandLineOption_i = -1;

            if (IndexInLongOpt_i < 0)
            {
              ShortOpt_c = static_cast<char>(Sts_i);

              for (i = 0; i < static_cast<int>(_rCommandLineOption_X.size()); i++)
              {
                if (_rCommandLineOption_X[i].Name_S.size() == 1)
                {
                  if (_rCommandLineOption_X[i].Name_S[0] == ShortOpt_c)
                  {
                    IndexInCommandLineOption_i = i;

                    //-w1 == -w 1 == -w=1   pOptVal_c is null if BOFPARAMETER_ARG_FLAG::NO_ARGUMENT
                    if (pOptVal_c)
                    {
                      if ((*pOptVal_c == ' ') || (*pOptVal_c == '='))
                      {
                        pOptVal_c++;
                      }
                    }
                    break;
                  }
                }
              }
              if (IndexInCommandLineOption_i == -1)
              {
                optopt = 0;
              }
            }
            else
            {
              //LongOption:only --opt=...
              for (i = 0; i < static_cast<int>(_rCommandLineOption_X.size()); i++)
              {
                if (_rCommandLineOption_X[i].Name_S.size() != 1)
                {
                  if (!strcmp(_rCommandLineOption_X[i].Name_S.c_str(), pGetOptLong_X[IndexInLongOpt_i].name))
                  {
                    IndexInCommandLineOption_i = i;
                    break;
                  }
                }
              }
            }

            pError_c[0] = 0;

            if ((IndexInCommandLineOption_i < 0) || (IndexInCommandLineOption_i >= static_cast<int>(_rCommandLineOption_X.size())))
            {
              pOpt_c = (optopt == 0) ? _ppArgv_c[optind - 1] : _ppArgv_c[optind];
            }
            else
            {
              pOpt_c = _rCommandLineOption_X[IndexInCommandLineOption_i].Name_S.c_str();                                                                                                                                                       // _ppArgv_c[optind - 1];
            }

            if (Sts_i == '?')                                                                                                                                                                                        //unrecognized option in optopt
            {
              snprintf(pError_c, sizeof(pError_c), "%s", pOpt_c);
            }

            if ((IndexInCommandLineOption_i >= 0) && (IndexInCommandLineOption_i < static_cast<int>(_rCommandLineOption_X.size())))
            {
              BofParameter::S_Parse(0, _rCommandLineOption_X[IndexInCommandLineOption_i], pOptVal_c, _ParseCallback_O);
            }
            else
            {
              Rts_E = BOF_ERR_PARSER;  //-2;

              if (_ErrorCallback_O != nullptr)
              {
                //Call error callback
                _ErrorCallback_O(pError_c);
              }
            }
          }                     //if (Sts_i != -1)
        } while (Sts_i >= 0);
      }

      for (i = 0; i < NbLongOpt_i; i++)
      {
        if (pGetOptLong_X[i].name)
        {
          free((void *)pGetOptLong_X[i].name);
          pGetOptLong_X[i].name = nullptr;
        }
      }
    }
  }
  return Rts_E;
}


void BofCommandLineParser::BuildHelpString(const std::vector<BOFPARAMETER> &_rCommandLineOption_X, const std::string &_rTitle_S, std::string &_rHelpString_S)
{
  BofParameter::S_BuildHelpString(_rCommandLineOption_X, 0xFFFFFFFF, _rTitle_S, _rHelpString_S);
}


END_BOF_NAMESPACE()
