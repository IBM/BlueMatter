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
 #ifndef __PARSER_H__
#define __PARSER_H__

/*******************************************************************
*
*
* Parser:
*
* This File contains a parser utility which can be used to convert
* a string to tokens and also parse a file and call a callback
* function which then can do something with the silly token vector.
*
* $Id: parser.h 4625 2003-01-31 18:22:00Z blake $
*
* $Log$
* Revision 1.1  2003/01/31 18:21:59  blake
* add interesting parts of blrudp
*
* Revision 1.3  2002/10/30 16:59:54  ralphbel
* Add changes and fixes for L3 fence test
*
* Revision 1.2  2002/09/22 13:27:10  ralphbel
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



#include <vector>
#include <string>

class Parser
{
public:
    Parser() {};
    virtual ~Parser() {};

    typedef std::vector<std::string> TokenVector;   // vector of tokens.
    ////////////////////////////////////////////////////////////////////
    typedef int (PARSEFILE_CALLBACK) (TokenVector &rTokens,
                                      unsigned long ulLine,
                                      void *pUserData);
    //
    // Callback function for ParseFile call.  
    // for each non comment line in the file it calls the callback
    // function with a vector of tokens it has parsed.
    //
    // inputs:
    //    rTokens -- vector of toeksn from the resultant parse.
    //    ulLine -- line number tokens were read from.
    //    pUserData -- void pointer parameters passed into the ParseFile call.
    // outputs:
    //    return -- BLERR_SUCCESS to continue the parse.
    //              != BLERR_SUCCESS to terminate the parse.
    /////////////////////////////////////////////////////////////////////
    typedef PARSEFILE_CALLBACK *PFN_PARSEFILE_CALLBACK;
    

    ////////////////////////////////////////////////////////////
    int ParseString(const char *pszString,
                    TokenVector &rTokens,
                    char cFirstSeparator = ' ');
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
    
    //////////////////////////////////////////////////////////////////////////////
    int ParseFile(const char *pszFileName,
                  PFN_PARSEFILE_CALLBACK pfnCallBack,
                  void *pUserData,
                  char cFirstSeparator = ' ');
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
    
    //////////////////////////////////////////////////////////////////////////////
    static int ParseBoolValue(const char *pszInValue,
                              bool &bValue);
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
    
    
    
protected:
private:

};

#endif

