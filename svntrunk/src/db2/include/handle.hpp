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
// File: handle.hpp
// Date: December 1, 2001
// Author: RSG
// Namespace: db2
// Class: Handle
// Description:  Interface class for basic handle operations.  Base
//               class for environment handles, connection handles,
//               statement handles, etc. used by DB2 CLI.
// *********************************************************************

#ifndef INCLUDE_DB2_HANDLE
#define INCLUDE_DB2_HANDLE

#include <sqlcli1.h>
#include <stdlib.h>

namespace db2
{
  class NullHandle;

  class Handle
  {
  private:
    // data
    Handle* d_parent_p;
    SQLHANDLE* d_handle_p;
    SQLSMALLINT d_handleType;
  protected:
    // methods
    Handle(int, Handle*);
  public:
    Handle();
    // data
    static Handle NullHandle;
    // methods
    ~Handle();
    void free();
    inline SQLSMALLINT handleType() const;
    inline Handle* parent();
    inline SQLHANDLE* CLIHandle();
  };

  Handle* Handle::parent()
  {
    return(d_parent_p);
  }

  SQLHANDLE* Handle::CLIHandle()
  {
    return(d_handle_p);
  }

  SQLSMALLINT Handle::handleType() const
  {
    return(d_handleType);
  }
}


#endif

