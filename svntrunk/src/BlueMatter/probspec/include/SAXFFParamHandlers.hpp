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
 // ***************************************************************************
// File: SAXFFParamHandlers.hpp
// Date: October 4, 2000
// Author: RSG (adapted from Xerces parser example SAXPrintHandlers.hpp
// Description: Implementation of call-back interface required to
//		support the ffparam file format and drive information
//		parsed from files obeying that DTD into Mike Pitman's
//		setup program.
// ***************************************************************************

#ifndef INCLUDE_SAXFFPARAMHANDLERS_HPP
#define INCLUDE_SAXFFPARAMHANDLERS_HPP

#include    <sax/HandlerBase.hpp>
#include "BlueMatter/ffhandler.hpp"

class SAXFFParamHandlers : public HandlerBase
{

private :
  // enums
  enum {MAX_STACK = 256};
  // private data
  FFHandler* d_handler_p;
  int d_mode;
  FFHandler* d_stack[MAX_STACK];
  int d_depth;

public:
    // -----------------------------------------------------------------------
    //	Constructors
    // -----------------------------------------------------------------------
    SAXFFParamHandlers();
    ~SAXFFParamHandlers();


    // -----------------------------------------------------------------------
    //	Implementations of the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    void endDocument();

    void endElement(const XMLCh* const name);

    void characters(const XMLCh* const chars, const unsigned int length);

    void ignorableWhitespace
    (
	const	XMLCh* const	chars
	, const unsigned int	length
    );

    void processingInstruction
    (
	const	XMLCh* const	target
	, const XMLCh* const	data
    );

    void startDocument();

    void startElement(const XMLCh* const name, AttributeList& attributes);



    // -----------------------------------------------------------------------
    //	Implementations of the SAX ErrorHandler interface
    // -----------------------------------------------------------------------
    void warning(const SAXParseException& exception);
    void error(const SAXParseException& exception);
    void fatalError(const SAXParseException& exception);



    // -----------------------------------------------------------------------
    //	Implementation of the SAX DTDHandler interface
    // -----------------------------------------------------------------------
    void notationDecl
    (
	const	XMLCh* const	name
	, const XMLCh* const	publicId
	, const XMLCh* const	systemId
    );

    void unparsedEntityDecl
    (
	const	XMLCh* const	name
	, const XMLCh* const	publicId
	, const XMLCh* const	systemId
	, const XMLCh* const	notationName
    );

};

#endif
