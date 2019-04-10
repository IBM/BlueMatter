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
// File: udf_registryif.cpp
// Author: cwo 
// Date: June 23, 2006
// Namespace: probspectrans 
// Description: 
// ***********************************************************************

#include <BlueMatter/probspectrans/udf_registryif.hpp>

#include <stdlib.h>
#include <assert.h>
#include <iostream.h>

namespace probspectrans
{

//****************************************************************************
// Constructors/Destructors
//****************************************************************************
  UDF_RegistryIF *UDF_RegistryIF::obj_instance = NULL;

  UDF_RegistryIF::UDF_RegistryIF(
          std::string dbname,
          int sys_id,
          RuntimeParams *runtimeparams)
    :  db2conn (db2::DBName::instance(dbname.c_str())->connection()),
       sys_id (sys_id),
       runtimeparams (runtimeparams)
    {
    //empty
    }
  UDF_RegistryIF::~UDF_RegistryIF()
    {
    delete obj_instance;
    obj_instance = NULL;
    }

  UDF_RegistryIF* UDF_RegistryIF::instance(
          std::string dbname,
          int sys_id,
          RuntimeParams *runtimeparams)
    {
    if (obj_instance == NULL)
      {
      obj_instance = new UDF_RegistryIF(dbname, sys_id, runtimeparams);
      assert(obj_instance != NULL);
      }
    return (obj_instance);
    }

//****************************************************************************
//****************************************************************************

  bool UDF_RegistryIF::is_category_for_UDFcode(int code, std::string category)
    {
    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[SQLCMD_BUFSZ];
    snprintf(sqlcmd1, SQLCMD_BUFSZ, "SELECT udf_id FROM Registry.UDF_Category "
            "WHERE udf_id=%d and category_name LIKE '%s' ", code, category.c_str());
    stmt1.prepare(sqlcmd1);

    int ret_code = -1;
    stmt1.bindCol(1, (SQLINTEGER&)ret_code);
    try { stmt1.execute(); stmt1.fetch(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
    
    if (ret_code != -1)
      {
      return true;
      }
    else
      {
      return false;
      }
 
    } //end 


//****************************************************************************
  int UDF_RegistryIF::get_number_of_UDFcodes()
    {
    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[SQLCMD_BUFSZ];
    snprintf(sqlcmd1, SQLCMD_BUFSZ, "SELECT COUNT(*) FROM Registry.UDF_Record");
    stmt1.prepare(sqlcmd1);

    int ret_count = -1;
    stmt1.bindCol(1, (SQLINTEGER&)ret_count);
    try { stmt1.execute(); stmt1.fetch(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

    return ret_count;
    } //end 


//****************************************************************************
  int UDF_RegistryIF::get_UDFcode_by_name(std::string name)
    {
    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[SQLCMD_BUFSZ];
    snprintf(sqlcmd1, SQLCMD_BUFSZ, "SELECT udf_id FROM Registry.UDF_Record "
               " WHERE udf_name LIKE '%s'", name.c_str());
    stmt1.prepare(sqlcmd1);

    int ret_UDFcode = -1;
    stmt1.bindCol(1, (SQLINTEGER&)ret_UDFcode);
    try { stmt1.execute(); stmt1.fetch(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
    
    return ret_UDFcode;
    } //end 


//****************************************************************************
  std::string UDF_RegistryIF::get_UDFname_by_code(int code)
    {
    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[SQLCMD_BUFSZ];
    snprintf(sqlcmd1, SQLCMD_BUFSZ, "SELECT udf_name FROM Registry.UDF_Record "
               " WHERE udf_id=%d ", code);
    stmt1.prepare(sqlcmd1);

    char ret_UDFname[SQLVARCHAR_BUFSZ];
    ret_UDFname[0] = '\n';
    stmt1.bindCol(1, (SQLCHAR*)&ret_UDFname[0], SQLVARCHAR_BUFSZ);
    try { stmt1.execute(); stmt1.fetch(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
 
    std::string ret_str(ret_UDFname);
    return ret_str;
    } //end 


//****************************************************************************
  std::string UDF_RegistryIF::get_structName_by_UDFcode(int code)
    {

    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[SQLCMD_BUFSZ];
    snprintf(sqlcmd1, SQLCMD_BUFSZ, "SELECT udf_name FROM Registry.UDF_Record "
               " WHERE udf_id=%d ", code);
    stmt1.prepare(sqlcmd1);

    char ret_struct_name[SQLVARCHAR_BUFSZ];
    ret_struct_name[0] = '\n';
    stmt1.bindCol(1, (SQLCHAR*)&ret_struct_name[0], SQLVARCHAR_BUFSZ);
    try { stmt1.execute(); stmt1.fetch(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
 
    std::string ret_str(ret_struct_name);
    return ret_str.append("_Params");
    } //end 


//****************************************************************************
  int UDF_RegistryIF::get_Nsites_by_UDFcode(int code) 
    {
    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[SQLCMD_BUFSZ];
    snprintf(sqlcmd1, SQLCMD_BUFSZ, "SELECT site_count FROM Registry.UDF_Record "
               " WHERE udf_id=%d ",code);
    stmt1.prepare(sqlcmd1);

    int ret_count = -1;
    stmt1.bindCol(1, (SQLINTEGER&)ret_count);
    try { stmt1.execute(); stmt1.fetch(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

    return ret_count;
    } //end 


//****************************************************************************
  int UDF_RegistryIF::get_argcount_by_UDFcode(int code)
    {
    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[SQLCMD_BUFSZ];
    snprintf(sqlcmd1, SQLCMD_BUFSZ, "SELECT COUNT( var_name ) FROM "
            " Registry.UDF_Variable WHERE udf_id =%d ", code);
    stmt1.prepare(sqlcmd1);

    int ret_count = -1;
    stmt1.bindCol(1, (SQLINTEGER&)ret_count);
    try { stmt1.execute(); stmt1.fetch(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
    
    return ret_count;
    } //end 


//****************************************************************************
  bool UDF_RegistryIF::reports_energy(std::string name)
    {
    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[SQLCMD_BUFSZ];
    snprintf(sqlcmd1, SQLCMD_BUFSZ, "SELECT count( distinct ruc.udf_id) "
            " FROM registry.udf_record as rur, registry.udf_category as ruc " 
            " WHERE rur.udf_name='%s' and ruc.udf_id=rur.udf_id "
            " and ruc.category_name='ReportsEnergy' ", name.c_str());
    stmt1.prepare(sqlcmd1);

    int ret_count = -1;
    stmt1.bindCol(1, (SQLINTEGER&)ret_count);
    try { stmt1.execute(); stmt1.fetch(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
    
    if (ret_count > 0)
      {
      return true;
      }
    else 
      {
      return false;
      }
    } //end 


//****************************************************************************
  std::string UDF_RegistryIF::get_arg_by_ord(int code, int arg_index)
    {
    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[SQLCMD_BUFSZ];
    snprintf(sqlcmd1, SQLCMD_BUFSZ, "SELECT var_name, type_desc FROM Registry.UDF_Variable " 
            " WHERE udf_id = %d ORDER BY type_size DESC, var_name ASC;", code);
    stmt1.prepare(sqlcmd1);

    char ret_name[SQLVARCHAR_BUFSZ];
    ret_name[0] = '\n';
    stmt1.bindCol(1, (SQLCHAR*)&ret_name[0], SQLVARCHAR_BUFSZ);
    stmt1.execute();

    int count = 0;
    while(stmt1.fetch())
      {
      if (count == arg_index)
        {
        std::string ret_str(ret_name);
        return ret_str;
        }
      count++;
      } 
    
    return ""; 
    } //end 



} //end namespace probspectrans

