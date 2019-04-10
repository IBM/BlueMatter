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
// File: stmthandle.hpp
// Date: December 1, 2001
// Author: RSG
// Namespace: db2
// Class: StmtHandle
// Inheritance: Handle
// Description:  Interface class for connection handle operations. 
//
// 06-2006: (cwo) Added bindParm(), bindParamArray() input bindings
// *********************************************************************


#ifndef INCLUDE_DB2_STMTHANDLE_HPP
#define INCLUDE_DB2_STMTHANDLE_HPP

#include <db2/handle.hpp>
#include <db2/connhandle.hpp>
#include <db2/handleexception.hpp>
#include <stdlib.h>

namespace db2
{

   class StmtHandle : public Handle
   {
      private:
      public:
         StmtHandle(ConnHandle&);
         ~StmtHandle();
         void executeDirect(const char*);
         void prepare(const char*);
         void execute();
         void close();
         int fetch();
         // args: column no. (from 1), variable to be bound, array size
         // (where applicable) 
         void bindCol(int, SQLCHAR*, int);
         void bindCol(int, SQLSCHAR&);
         void bindCol(int, SQLSMALLINT&);
         void bindCol(int, SQLINTEGER&);
         void bindCol(int, SQLDOUBLE&);
         void bindCol(int, SQLREAL&);
         void bindCol(int, DATE_STRUCT&);
         void bindCol(int, TIME_STRUCT&);
         void bindCol(int, TIMESTAMP_STRUCT&);


         //args: (column no. (staring with 1), variable to bind)
         void bindParam(int, SQLINTEGER&);
         void bindParam(int, SQLDOUBLE&);
//         void bindParam(int, TIMESTAMP_STRUCT&);  
         //args: (column no. (staring with 1), variable to bind, size of buffer)
         void bindParam(int, SQLCHAR*, int);

         //args: (column no. (staring with 1), variable to bind, size of array)
         void bindParamArray(int, SQLINTEGER*, int);
         void bindParamArray(int, SQLDOUBLE*, int);
         //args: (column no. (staring with 1), variable to bind, size of array,
         //       size of each buffer in array)
         void bindParamArray(int, SQLCHAR**, int, int);

   };


}

#endif
