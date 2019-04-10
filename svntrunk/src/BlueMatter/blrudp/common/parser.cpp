/* Copyright 2001, 2019 IBM Corporation
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
 * following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the 
 * following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the 
 * following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 /*******************************************************************
*
*
* Parser:
*
* This File contains a parser utility which can be used to convert
* a string to tokens and also parse a file and call a callback
* function which then can do something with the silly token vector.
*
* $Id: parser.cpp 4625 2003-01-31 18:22:00Z blake $
*
* $Log$
* Revision 1.1  2003/01/31 18:21:59  blake
* add interesting parts of blrudp
*
* Revision 1.5  2003/01/04 01:20:21  ralphbel
* switch aix mti code to use the visual age xlC compiler
*
* Revision 1.4  2002/11/26 19:45:33  ralphbel
* Get IPL software to compile on AIX again
*
* Revision 1.3  2002/10/30 16:59:53  ralphbel
* Add changes and fixes for L3 fence test
*
* Revision 1.2  2002/09/22 13:27:09  ralphbel
* add common parser code for test code
*
* Revision 1.1  2002/09/20 17:46:10  ralphbel
* empty
*
*
*
*
*
*******************************************************************/

#include <stdio.h>
#include <strings.h>

#include "parser.h"
#include "BlErrors.h"

#include "dbgPrint.h"

#define FAIL goto Fail
#define FAILERR(nErr, nValue) {(nErr) = (nValue); goto Fail;}



////////////////////////////////////////////////////////////
int Parser::ParseString(const char *pszString,
                        TokenVector &rTokens,
                        char cFirstSeparator)
//
//
// Parse a string into tokens.
//
//
// inputs:
//     pszLine -- string to parse intot tokens.
//     rTokens -- vector of return the tokens in.
//     cFirstSeparator -- first separator.
// outputs:
//     returns -- BLERR_SUCCESS if successful
//                BLERR_OUT_OF_MEMORY of not enough memory could be found
//                                  to parse the line.
/////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    char *pszTemp = NULL;

    enum {
        STATE_SPACE,
        STATE_CHAR,
        STATE_QUOTE
    };
    int nState = STATE_SPACE;
    int nNumTokens = 0;

    rTokens.clear();
    ASSERT(pszString);        // huh???
    if (pszString == NULL)
        FAIL;
    pszTemp = new char[strlen(pszString)+1];
    if (pszTemp == NULL)
        FAILERR(nErr, BLERR_NO_MEMORY);


    const char *psz;      // pointer to our token parser.
    char *pszToken;

    psz = pszString;
    pszToken = pszTemp;     


    bool bParseDone;
    bParseDone = false;
    //
    // Not the most efficient code, but for this test this does not matter.
    //
    while ( ! bParseDone)
    {
        char ch;
        ch = *psz++;
        if (ch == 0x0a)        // line feed is treated the same as a end of line.
            ch = 0;

        switch (nState)
        {
            case STATE_CHAR:
            {
                if ( (ch == ' ') || (ch == '\t') || (ch == 0) ||
                     ((nNumTokens == 0) && (ch == cFirstSeparator)) )
                {
                    nState = STATE_SPACE;
                    *pszToken++ = 0;        // string terminator
                    if ((pszTemp[0] == ';') || 
                        ((pszTemp[0] == '/') && (pszTemp[1] == '/')) )   // comment.
                    {
                        bParseDone = true;
                        break;
                    }
                    // valid token, add it to the list.
                    rTokens.push_back(pszTemp);         // push the token on the list.
                    nNumTokens++;
                    *pszTemp  = 0;
                    pszToken = pszTemp;                 // reset to the beginning.
                    nState = STATE_SPACE;
                }
                else
                    *pszToken++ = ch;
                break;
            }
            case STATE_QUOTE:
            {
                if (ch == '"')
                {
                    nState = STATE_CHAR;                // needs a following space to terminate it.
                    break;
                }
                if (ch == 0)                            // end of the line terminated with out quote character.
                {
                    printf("missing \" character");
                    FAILERR(nErr, BLERR_PARAM_ERR);
                }
                *pszToken++ = ch;
                break;
            }
            case STATE_SPACE:
            {
                if (ch == '"')
                {
                    nState = STATE_QUOTE;       // do the quoted string thing
                    break;
                }
                //
                // first token seperator can be a '=' character.
                //
                if ((nNumTokens == 1) && (ch == cFirstSeparator))
                    break;
                
                if ((ch != ' ') && (ch != '\t'))
                {
                    *pszToken++ = ch;
                    nState = STATE_CHAR;
                }
                                    
                break;
            }
            default:
            {
                //assert(0);
                break;
            };
        }   // switch nState
        if (ch == 0)                        // end of the line???
            bParseDone = true;;

    }


    delete [] pszTemp;    
    return(BLERR_SUCCESS);
Fail:
    if (pszTemp)
        delete [] pszTemp;
    return(nErr);
}

//////////////////////////////////////////////////////////////////////////////
int Parser::ParseFile(const char *pszFileName,
                      PFN_PARSEFILE_CALLBACK pfnCallBack,
                      void *pUserData,
                      char cFirstSeparator)
//
// Parse file callingback pfnCAllback for each non comment line.
//
// inputs:
//      pszFileName -- file name to open.
//      pfnCallBack -- pointer to callback function to call.
//      pUserData -- pointer to user data to pass back to the callback function.
//      cFirstSeperator.
// outputs:
//      returns -- BLERR_SUCCESS if parse was successful.
//
//////////////////////////////////////////////////////////////////////////////
{
    int nErr = BLERR_INTERNAL_ERR;
    enum {MAX_READ = 1024};     // read buffer
    char *pszData = NULL;
    TokenVector vectTokens;
    unsigned long nLines;
    
    FILE *pFile = NULL;
    
    if ((pszFileName == NULL) || (pfnCallBack == NULL))
        FAILERR(nErr, BLERR_PARAM_ERR);

    pszData = (char *)malloc(MAX_READ);
    if (pszData == NULL)
    {
        printf("out of memory\n");
        FAILERR(nErr, BLERR_NO_MEMORY);
    }


    pFile = fopen(pszFileName, "rt");       // open for reading, translated mode.
    if (pFile == NULL)
    {
        printf("Failed to open file %s\n", pszFileName);
        FAILERR(nErr, BLERR_FILEOPEN_ERR);
    }
    

    nLines = 0;
    while (true) 
    {
        int nRet;
        char *pRead;
        pRead = fgets(pszData,          //*string
                     MAX_READ,          // n
                     pFile);            //*stream
        if (pRead == NULL)
            break;                      // end of the line.
        nLines++;
        nRet = ParseString(pszData,                 // convert strings to tokens.
                           vectTokens,              // rTokens
                           cFirstSeparator);        // cFirst Separator.
        if (nRet != BLERR_SUCCESS)
        {
            printf("Error parsing tokens\n");
            FAILERR(nErr, nRet);
        } 
        if (vectTokens.size() == 0)        // blank line.
            continue;
        if (vectTokens[0][0] == '#')        // comment line...
            continue;
        if (vectTokens[0][0] == ';')        // comment line...
            continue;

        nRet = (*pfnCallBack)(vectTokens,
                               nLines,
                               pUserData);
        if (nRet != BLERR_SUCCESS)
            FAILERR(nErr, nRet);                               
    }    
    
    
    if (pszData)
        free(pszData);
    
    
    fclose(pFile);
    
    return(BLERR_SUCCESS);

Fail:
    if (pszData)
        free(pszData);
    if (pFile)
        fclose(pFile);      // clean up when we are done.
    return(nErr);
}

//////////////////////////////////////////////////////////////////////////////
int Parser::ParseBoolValue(const char *pszInValue,
                           bool &bValue)
//
// Parse boolean value [yes | no]
// and return the boolean result.
//
// inputs:
//    pszInValue - input value 
// outputs:
//    returns -- BLERR_SUCCESS if successful.
//               BLERR_PARAM_ERR if invalid value.
//    &bValue -- true or false depending on the value of pszInValue
//
////////////////////////////////////////////////////////////////////////////////
{
    int nRet = BLERR_PARAM_ERR;
    
    if (strcasecmp(pszInValue, "yes") == 0)
        bValue = true;
    else
    if (strcasecmp(pszInValue, "no") == 0)
        bValue = false;
    else
        FAIL;

    return(BLERR_SUCCESS);
Fail:
    return(nRet);
}


