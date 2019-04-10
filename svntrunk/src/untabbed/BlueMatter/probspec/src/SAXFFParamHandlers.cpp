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
 #ifndef PKFXLOG_PROBSPEC
#define PKFXLOG_PROBSPEC (0)
#endif

#include <util/XMLUni.hpp>
#include <stdlib.h>
#include <assert.h>
#include <sax/AttributeList.hpp>
#include "BlueMatter/SAXFFParamHandlers.hpp"
#include <iostream.h>

// fxlogger has its own version of assert, so undef the system one
//#undef assert
//#include <pk/fxlogger.hpp>

class TString
{
  friend ostream& operator<<(ostream&, const TString&);
private:
  char* d_string;
public:
  inline TString(char* string) : d_string(string)
    {}
  inline TString(const XMLCh* instr) : d_string(transcode(instr))
    {}
  inline ~TString()
    {
      delete [] d_string;
    }
  inline char* asChar()
    {
      return d_string;
    }
  static inline char* transcode(const XMLCh* toTranscode)
    {
      return XMLString::transcode(toTranscode);
    }
};

ostream& operator<<(ostream& os, const TString& tstr)
{
  os << tstr.d_string;
  return os;
}

// ---------------------------------------------------------------------------
//  SAXFFParamHandlers: Constructors and Destructor
// ---------------------------------------------------------------------------
SAXFFParamHandlers::SAXFFParamHandlers() : d_handler_p(NULL), d_depth(-1)
{

}

SAXFFParamHandlers::~SAXFFParamHandlers()
{
}


// ---------------------------------------------------------------------------
//  SAXFFParamHandlers: Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
void SAXFFParamHandlers::error(const SAXParseException& e)
{
    cerr << "\nError at (file " << TString(e.getSystemId())
                 << ", line " << e.getLineNumber()
                 << ", char " << e.getColumnNumber()
         << "): " << TString(e.getMessage()) << endl;
}

void SAXFFParamHandlers::fatalError(const SAXParseException& e)
{
    cerr << "\nFatal Error at (file " << TString(e.getSystemId())
                 << ", line " << e.getLineNumber()
                 << ", char " << e.getColumnNumber()
         << "): " << TString(e.getMessage()) << endl;
}

void SAXFFParamHandlers::warning(const SAXParseException& e)
{
    cerr << "\nWarning at (file " << TString(e.getSystemId())
                 << ", line " << e.getLineNumber()
                 << ", char " << e.getColumnNumber()
         << "): " << TString(e.getMessage()) << endl;
}


// ---------------------------------------------------------------------------
//  SAXFFParamHandlers: Overrides of the SAX DTDHandler interface
// ---------------------------------------------------------------------------
void SAXFFParamHandlers::unparsedEntityDecl(const     XMLCh* const name
                                          , const   XMLCh* const publicId
                                          , const   XMLCh* const systemId
                                          , const   XMLCh* const notationName)
{
    // Not used at this time
}


void SAXFFParamHandlers::notationDecl(                                               const   XMLCh* const name
                                    , const XMLCh* const publicId
                                    , const XMLCh* const systemId)
{
    // Not used at this time
}


// ---------------------------------------------------------------------------
//  SAXFFParamHandlers: Overrides of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------
void SAXFFParamHandlers::characters(const     XMLCh* const    chars
                                  , const   unsigned int    length)
{
  if (d_handler_p != NULL)
    {
      TString tdata(chars);
      char* tmp = new char[(strlen(tdata.asChar())+1)];
      assert(tmp != NULL);
      strcpy(tmp, tdata.asChar());
      d_handler_p->charData(tmp);
#if defined(USE_PKFXLOG)
      BegLogLine( PKFXLOG_PROBSPEC )
  << "character data: " << tmp
  << " being processed by handler of type: "
  << d_handler_p->getType() << EndLogLine;
#endif
    }
}


void SAXFFParamHandlers::endDocument()
{
}

void SAXFFParamHandlers::endElement(const XMLCh* const name)
{
  if (d_handler_p != NULL)
    {
      TString tempX(name);
      d_handler_p->endElement(tempX.asChar());
#if defined(USE_PKFXLOG)
      BegLogLine( PKFXLOG_PROBSPEC )
        << "</" << tempX.asChar() << ">" << EndLogLine;

#endif
      if (XMLString::compareIString(d_handler_p->getType(),
                                    tempX.asChar()) == 0)
        {

#if defined(USE_PKFXLOG)
          BegLogLine( PKFXLOG_PROBSPEC )
            << "handler type: " << d_handler_p->getType()
            << " calling finalize " << EndLogLine;
#endif
          d_handler_p->finalize();
          delete d_handler_p;
          d_handler_p = NULL;
        }

      if (d_depth >= 0)
        {
          d_handler_p = d_stack[d_depth];
          d_depth--;
          if (d_handler_p != NULL)
            {
#if defined(USE_PKFXLOG)
              BegLogLine( PKFXLOG_PROBSPEC )
                << "handler type: " << d_handler_p->getType()
                << " just popped off stack"
                << EndLogLine;
#endif
            }
        }
    }
}

void SAXFFParamHandlers::ignorableWhitespace( const   XMLCh* const chars
                                            ,const  unsigned int length)
{
}

void SAXFFParamHandlers::processingInstruction(const  XMLCh* const target
                                            , const XMLCh* const data)
{
}

void SAXFFParamHandlers::startDocument()
{
}

void SAXFFParamHandlers::startElement(const   XMLCh* const    name
                                    ,       AttributeList&  attributes)
{
  TString tempX(TString::transcode(name));
#if defined(USE_PKFXLOG)
  BegLogLine( PKFXLOG_PROBSPEC ) << "<" << tempX.asChar()
                                 << ">"  << EndLogLine;
#endif
  FFHandler* tHandler_p = FFHandler::createHandler(tempX.asChar());
  if (tHandler_p != NULL)
    {

#if defined(USE_PKFXLOG)
      BegLogLine( PKFXLOG_PROBSPEC ) << "handler created of type: "
                                     << tHandler_p->getType()
                                     << EndLogLine;
#endif
      tHandler_p->initialize(); // hook for initialization
    }

  if (d_handler_p != NULL) // we have an existing handler
    {
      d_handler_p->startElement(tempX.asChar());
#if defined(USE_PKFXLOG)
      BegLogLine( PKFXLOG_PROBSPEC ) << "existing handler called of type: "
                                     << d_handler_p->getType()
                                     << EndLogLine;
#endif
      if (tHandler_p != NULL)
        {

          d_depth++; // push onto stack
          assert(d_depth < MAX_STACK);
          d_stack[d_depth] = d_handler_p;

#if defined(USE_PKFXLOG)
          BegLogLine( PKFXLOG_PROBSPEC ) << "handler of type: "
                                         << d_handler_p->getType()
                                         << " pushed onto stack"
                                         << EndLogLine;
#endif
        }
    }

  if (tHandler_p != NULL)
    {
      tHandler_p->startElement(tempX.asChar());
#if defined(USE_PKFXLOG)
      BegLogLine( PKFXLOG_PROBSPEC ) << "new handler called of type: "
                                     << tHandler_p->getType()
                                     << EndLogLine;
#endif
      d_handler_p = tHandler_p;
    }

}

