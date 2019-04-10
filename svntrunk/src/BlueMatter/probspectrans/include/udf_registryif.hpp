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
// File: udf_registryif.hpp
// Author: cwo 
// Date: June 23, 2006 
// Namespace: probspectrans
// Class: UDF_RegistryIF 
// Description: 
// ***********************************************************************
#ifndef INCLUDE_UDF_REGISTRYIF_HPP
#define INCLUDE_UDF_REGISTRYIF_HPP

#define SQLCMD_BUFSZ 512 
#define SQLVARCHAR_BUFSZ 255

#include <BlueMatter/probspectrans/runtimeparams.hpp>
#include <db2/stmthandle.hpp>
#include <db2/handleexception.hpp>
#include <db2/dbname.hpp>

#include <string>
#include <stdlib.h>
#include <limits.h>

namespace probspectrans 
{

  class UDF_RegistryIF
  {
    private:
      
      //////////////////////////////////////// 
      // global fields
      static UDF_RegistryIF *obj_instance;
      RuntimeParams *runtimeparams;
      db2::ConnHandle& db2conn;
      int sys_id;
      
      UDF_RegistryIF(std::string, int, RuntimeParams*);
    public:

      //////////////////////////////////////// 
      // Interface methods
      bool is_category_for_UDFcode(int, std::string);
      int get_number_of_UDFcodes(); 
      int get_UDFcode_by_name(std::string);
      std::string get_UDFname_by_code(int); 
      std::string get_structName_by_UDFcode(int);
      int get_Nsites_by_UDFcode(int); 
      int get_argcount_by_UDFcode(int);
      bool reports_energy(std::string);
      std::string get_arg_by_ord(int, int);

    
      //////////////////////////////////////// 
      // Constructor/Destructor 
      static UDF_RegistryIF* instance(std::string, int, RuntimeParams*);
      ~UDF_RegistryIF();
  };
}

#endif
