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
 // ***********************************************************************
// File: dbname.hpp
// Author: RSG
// Date: April 30, 2002
// Namespace: db2
// Class: DBName
// Description: Singleton class encapsulating the name of the database
//              being used and the connection to it. Unlike most
//              singleton classes, this one requires explicit
//              initialization.
//
// 06-15-2006: (cwo) Moved to /db2 in src-tree hierarchy
// ***********************************************************************

#ifndef INCLUDE_DB2_DBNAME_HPP
#define INCLUDE_DB2_DBNAME_HPP

#include <db2/envhandle.hpp>
#include <db2/connhandle.hpp>

//#define DBNAME_SINGLETON_CLASS

namespace db2
{
  class DBName
  {
    private:
      // data
      char* d_name;
      db2::EnvHandle* d_env_p;
      db2::ConnHandle* d_conn_p;
#ifdef DBNAME_SINGLETON_CLASS
      static DBName* s_instance;
#endif
      // methods
      DBName();
    public:
      ~DBName();
      static DBName* instance(const char* = NULL);
      inline const char* name() const;
      db2::ConnHandle& connection();
  };

  const char* DBName::name() const
  {
    return(d_name);
  }
}
#endif
