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
// File: dbname.cpp
// Author: RSG
// Date: April 30, 2002
// Namespace: db2
// Class: DBName
// Description: Singleton class encapsulating the name of the database
//              being used. Unlike most singleton classes, this one
//              requires explicit initialization.
//
// 06-15-2006: (cwo) Moved to /db2 in src-tree hierarchy
// ***********************************************************************

#include <db2/dbname.hpp>

#include <stdlib.h>
#include <assert.h>
#include <string.h>

namespace db2
{
#ifdef DBNAME_SINGLETON_CLASS
  DBName* DBName::s_instance = NULL;
#endif
  DBName::DBName()
  {}

  DBName::~DBName()
  {
    delete [] d_name;
    delete d_conn_p;
    d_conn_p = NULL;
    delete d_env_p;
    d_env_p = NULL;
#ifdef DBNAME_SINGLETON_CLASS
    delete s_instance;
    s_instance = NULL;
#endif
  }

  db2::ConnHandle& DBName::connection()
  {
    return(*d_conn_p);
  }

  DBName* DBName::instance(const char* name)
  {
#ifdef DBNAME_SINGLETON_CLASS
    if (s_instance == NULL)
    {
#else
    DBName* s_instance = NULL;
#endif
      s_instance = new DBName;
      assert(s_instance != NULL);
      assert(name != NULL);
      s_instance->d_name = new char[strlen(name)+1];
      assert(s_instance->d_name != NULL);
      strcpy(s_instance->d_name, name);
      s_instance->d_env_p = new db2::EnvHandle;
      assert(s_instance->d_env_p != NULL);
      s_instance->d_conn_p = new db2::ConnHandle(*s_instance->d_env_p);
      assert(s_instance->d_conn_p != NULL);
      s_instance->d_conn_p->connect(s_instance->d_name);
      return(s_instance);
#ifdef DBNAME_SINGLETON_CLASS
    }
    assert(name == NULL);
    return(s_instance);
#endif
  }
}

