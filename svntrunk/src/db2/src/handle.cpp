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
 // *********************************************************************
// File: handle.cpp
// Date: December 1, 2001
// Author: RSG
// Namespace: db2
// Class: Handle
// Description:  Interface class for basic handle operations.  Base
//               class for environment handles, connection handles,
//               statement handles, etc. used by DB2 CLI.
// *********************************************************************

#include <db2/handle.hpp>
#include <db2/handleexception.hpp>
#include <iostream>

namespace db2
{
  Handle Handle::NullHandle;

  Handle::Handle()
    : d_handleType(SQL_NULL_HANDLE), d_handle_p(new SQLHANDLE), d_parent_p(NULL)
  {
    ASSERT(d_handle_p != NULL);
    *d_handle_p = SQL_NULL_HANDLE;
  }

  Handle::Handle(int handleType, Handle* parent_p)
    : d_handleType(handleType), d_handle_p(new SQLHANDLE), d_parent_p(parent_p)
  {
    ASSERT(d_handle_p != NULL);
    SQLRETURN rc = SQLAllocHandle(d_handleType,
				  *d_parent_p->CLIHandle(),
				  d_handle_p);
    //std::cerr << "rc = " << rc << std::endl;
    SQLTEST(rc, *this);
  }

  void Handle::free()
  {
    if ((d_handle_p != NULL) && (*d_handle_p != SQL_NULL_HANDLE))
      {
	SQLRETURN rc = SQLFreeHandle(d_handleType, *d_handle_p);
	SQLTEST(rc, *this);
      }
  }

  Handle::~Handle()
  {
    free();
    delete d_handle_p;
  }

}
