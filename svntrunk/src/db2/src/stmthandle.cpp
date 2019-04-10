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
// File: stmthandle.cpp
// Date: December 1, 2001
// Author: RSG
// Namespace: db2
// Class: StmtHandle
// Inheritance: Handle
// Description:  Interface class for connection handle operations. 
// *********************************************************************

#include <db2/stmthandle.hpp>
#include <db2/handleexception.hpp>

namespace db2
{
  StmtHandle::StmtHandle(ConnHandle& hdc) : Handle(SQL_HANDLE_STMT, &hdc)
  {}

  StmtHandle::~StmtHandle()
  {
    SQLRETURN rc = SQLFreeStmt(*CLIHandle(), SQL_CLOSE);
    SQLTEST(rc, *this);
  }

  void StmtHandle::executeDirect(const char* cmd)
  {
    SQLRETURN rc = SQLExecDirect(*CLIHandle(), (SQLCHAR*)cmd, SQL_NTS);
    SQLTEST(rc, *this);
  }

  void StmtHandle::prepare(const char* cmd)
  {
    SQLRETURN rc = SQLPrepare(*CLIHandle(), (SQLCHAR*)cmd, SQL_NTS);
    SQLTEST(rc, *this);
  }

  void StmtHandle::execute()
  {
    SQLRETURN rc = SQLExecute(*CLIHandle());
    SQLTEST(rc, *this);
  }

  int StmtHandle::fetch()
  {
    SQLRETURN rc = SQLFetch(*CLIHandle());
    if (rc == SQL_NO_DATA)
      {
	return(0);
      }
    SQLTEST(rc, *this);
    return(1);
  }

  void StmtHandle::close()
  {
    SQLRETURN rc = SQLFreeStmt(*CLIHandle(), SQL_CLOSE);
    SQLTEST(rc, *this);
  }

  //****************************************************************************
  // Functions for inserting inputs (binding parameters) 
  //****************************************************************************

     void StmtHandle::bindParam(int paramNo, SQLINTEGER& data)
     {
        SQLRETURN rc = SQLBindParameter(*CLIHandle(), 
              paramNo,
              SQL_PARAM_INPUT,
              SQL_C_LONG,
              SQL_INTEGER,
              0, //ignored for SQL_INTEGER
              0, //ignored for SQL_INTEGER
              &data,
              0, //ignored for SQL_INTEGER
              NULL);
        SQLTEST(rc, *this);
     } //end bindParam()

     void StmtHandle::bindParam(int paramNo, SQLDOUBLE& data)
     {
        SQLRETURN rc = SQLBindParameter(*CLIHandle(), 
              paramNo,
              SQL_PARAM_INPUT,
              SQL_C_DOUBLE,
              SQL_DOUBLE,
              0, //ignored for SQL_DOUBLE
              0, //ignored for SQL_DOUBLE
              &data,
              0, //ignored for SQL_DOUBLE
              NULL);
        SQLTEST(rc, *this);
     } //end bindParam()

     void StmtHandle::bindParam(int paramNo, SQLCHAR *buf, int buf_sz)
     {
        SQLRETURN rc = SQLBindParameter(*CLIHandle(), 
              paramNo,
              SQL_PARAM_INPUT,
              SQL_C_CHAR,
              SQL_VARCHAR,
              buf_sz,
              0, //ignored for SQL_VARCHAR
              buf,
              buf_sz, 
              NULL);
        SQLTEST(rc, *this);
     } //end bindParam()

  //   void StmtHandle::bindParam(int paramNo, TIMESTAMP_STRUCT& data)
  //   {
  //      SQLRETURN rc = SQLBindParameter(*CLIHandle(), 
  //            paramNo,
  //            SQL_PARAM_INPUT,
  //            SQL_C_TYPE_TIMESTAMP,
  //            SQL_TYPE_TIMESTAMP,
  //            0, //ignored for SQL_TYPE_TIMESTAMP
  //            3, //Precision for seconds (in decimal places) (FIXME??)
  //            &data,
  //            0, //ignored for SQL_TYPE_TIMESTAMP
  //            NULL);
  //      SQLTEST(rc, *this);
  //   } //end bindParam()

     void StmtHandle::bindParamArray(int paramNo, SQLCHAR *buf[], int array_sz, int buf_sz)
     {

        SQLRETURN rc = SQLSetStmtAttr(*CLIHandle(),
              SQL_ATTR_PARAMSET_SIZE,
              (SQLPOINTER)array_sz,
              0);
        SQLTEST(rc, *this);

        rc = SQLBindParameter(*CLIHandle(), 
              paramNo,
              SQL_PARAM_INPUT,
              SQL_C_CHAR,
              SQL_VARCHAR,
              buf_sz,
              0, //ignored for SQL_VARCHAR
              buf,
              buf_sz, 
              NULL);
        SQLTEST(rc, *this);
     } //end bindParamArray()

     void StmtHandle::bindParamArray(int paramNo, SQLINTEGER *data, int array_sz)
     {

        SQLRETURN rc = SQLSetStmtAttr(*CLIHandle(),
              SQL_ATTR_PARAMSET_SIZE,
              (SQLPOINTER)array_sz,
              0);
        SQLTEST(rc, *this);

        rc = SQLBindParameter(*CLIHandle(), 
              paramNo,
              SQL_PARAM_INPUT,
              SQL_C_LONG,
              SQL_INTEGER,
              0, //ignored for SQL_LONG
              0, //ignored for SQL_LONG
              data,
              0, //ignored for SQL_LONG
              NULL);
        SQLTEST(rc, *this);
     } //end bindParamArray()


     void StmtHandle::bindParamArray(int paramNo, SQLDOUBLE *data, int array_sz)
     {

        SQLRETURN rc = SQLSetStmtAttr(*CLIHandle(),
              SQL_ATTR_PARAMSET_SIZE,
              (SQLPOINTER)array_sz,
              0);
        SQLTEST(rc, *this);

        rc = SQLBindParameter(*CLIHandle(), 
              paramNo,
              SQL_PARAM_INPUT,
              SQL_C_DOUBLE,
              SQL_DOUBLE,
              0, //ignored for SQL_DOUBLE
              0, //ignored for SQL_DOUBLE
              data,
              0, //ignored for SQL_DOUBLE
              NULL);
        SQLTEST(rc, *this);
     } //end bindParamArray()

  //****************************************************************************
  // Functions for retrieving outputs (binding columns)
  //****************************************************************************

    void StmtHandle::bindCol(int colNo, SQLCHAR* buf, int bufSize)
    {
      SQLINTEGER buflen = 0;
      SQLRETURN rc = SQLBindCol(*CLIHandle(),
  			      colNo,
  			      SQL_C_CHAR,
  			      (SQLPOINTER)buf,
  			      bufSize,
  			      &buflen);
      SQLTEST(rc, *this);
    }

    void StmtHandle::bindCol(int colNo, SQLSCHAR& data)
    {
      SQLINTEGER buflen = 0;
      SQLRETURN rc = SQLBindCol(*CLIHandle(),
  			      colNo,
  			      SQL_C_TINYINT,
  			      (SQLPOINTER)&data,
  			      0,
  			      &buflen);
      SQLTEST(rc, *this);
    }

    void StmtHandle::bindCol(int colNo, SQLSMALLINT& data)
    {
      SQLINTEGER buflen = 0;
      SQLRETURN rc = SQLBindCol(*CLIHandle(),
  			      colNo,
  			      SQL_C_SHORT,
  			      (SQLPOINTER)&data,
  			      0,
  			      &buflen);
      SQLTEST(rc, *this);
    }

    void StmtHandle::bindCol(int colNo, SQLINTEGER& data)
    {
      SQLINTEGER buflen = 0;
      SQLRETURN rc = SQLBindCol(*CLIHandle(),
  			      colNo,
  			      SQL_C_LONG,
  			      (SQLPOINTER)&data,
  			      0,
  			      &buflen);
      SQLTEST(rc, *this);
    }

    void StmtHandle::bindCol(int colNo, SQLDOUBLE& data)
    {
      SQLINTEGER buflen = 0;
      SQLRETURN rc = SQLBindCol(*CLIHandle(),
  			      colNo,
  			      SQL_C_DOUBLE,
  			      (SQLPOINTER)&data,
  			      0,
  			      &buflen);
      SQLTEST(rc, *this);
    }

    void StmtHandle::bindCol(int colNo, SQLREAL& data)
    {
      SQLINTEGER buflen = 0;
      SQLRETURN rc = SQLBindCol(*CLIHandle(),
  			      colNo,
  			      SQL_C_FLOAT,
  			      (SQLPOINTER)&data,
  			      0,
  			      &buflen);
      SQLTEST(rc, *this);
    }

    void StmtHandle::bindCol(int colNo, DATE_STRUCT& data)
    {
      SQLINTEGER buflen = 0;
      SQLRETURN rc = SQLBindCol(*CLIHandle(),
  			      colNo,
  			      SQL_C_TYPE_DATE,
  			      (SQLPOINTER)&data,
  			      0,
  			      &buflen);
      SQLTEST(rc, *this);
    }


    void StmtHandle::bindCol(int colNo, TIME_STRUCT& data)
    {
      SQLINTEGER buflen = 0;
      SQLRETURN rc = SQLBindCol(*CLIHandle(),
  			      colNo,
  			      SQL_C_TYPE_TIME,
  			      (SQLPOINTER)&data,
  			      0,
  			      &buflen);
      SQLTEST(rc, *this);
    }

    void StmtHandle::bindCol(int colNo, TIMESTAMP_STRUCT& data)
    {
      SQLINTEGER buflen = 0;
      SQLRETURN rc = SQLBindCol(*CLIHandle(),
  			      colNo,
  			      SQL_C_TYPE_TIMESTAMP,
  			      (SQLPOINTER)&data,
  			      0,
  			      &buflen);
      SQLTEST(rc, *this);
    }
}
