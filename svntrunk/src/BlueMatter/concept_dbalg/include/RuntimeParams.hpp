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
// File: RuntimeParams.hpp
// Author: cwo 
// Date: August 14, 2006 
// Namespace: concept_dbalg
// Class: RuntimeParams 
// Description: Global runtime param class, determines what options 
//              runMD should use - copied from probspectrans
// FIXME -- Some functions need to be changed to not talk directly to 
//          DB2 but to go through containers
// ***********************************************************************
#ifndef INCLUDE_RUNTIMEPARAMS_HPP
#define INCLUDE_RUNTIMEPARAMS_HPP

#include <db2/dbname.hpp>
#include <db2/stmthandle.hpp>
#include <db2/handleexception.hpp>

#define SQLCMD_BUFSZ 512
#define SQLVARCHAR_BUFSZ 255

#define IMPORT_PREDICATE_BUFFER_SZ 40 

#include <string>
#include <ostream>






namespace concept_dbalg
{

  static const int POSSIBLE_WATERMETHOD_STRINGS_CNT = 9;
  static const int POSSIBLE_WATERMETHOD_STRINGS_SZ = 20;
  static const char POSSIBLE_WATERMETHOD_STRINGS[9][20] = {
    "NONE", 
    "TIP3P", 
    "CHARMM_TIP3P", 
    "SPC", 
    "SPCE", 
    "TIP3PFLOPPY", 
    "CHARMM_TIP3PFLOPPY", 
    "SPCFLOPPY", 
    "SPCEFLOPPY"
    };

  class RuntimeParams 
  {
    private:
    public:

      //************************************* 
      // Parameters that can be filled in from PST
      int sys_id;
      int doShakeRattleWater;
      int doShakeRattleProtein;
      std::string water_method;
      
      //************************************* 
      // True runtime parameters for simulation
      int pst_id;
      unsigned int verbosity;
      std::string dvs_filename;
      std::string rdg_filename;
      double timestep_length;
      double cutoff_dist;
      double switch_width;

      double boundingbox_dim_x;
      double boundingbox_dim_y;
      double boundingbox_dim_z;
      double inv_boundingbox_dim_x;
      double inv_boundingbox_dim_y;
      double inv_boundingbox_dim_z;
      
      //************************************* 
      // Class member variables 
      //db2::ConnHandle& db2conn;

      //**********************************************************************
      //**********************************************************************
      RuntimeParams( std::string dbname )
          {
          //this->db2conn = db2::DBName::instance(dbname.c_str())->connection();
          } 

      //**********************************************************************
      //**********************************************************************
      bool check_if_valid_watermethod(std::string wm)
        {
        bool valid = false;
        for (int i=0; i < POSSIBLE_WATERMETHOD_STRINGS_CNT; ++i)
          {
          if (strcmp(POSSIBLE_WATERMETHOD_STRINGS[i], wm.c_str())==0)
            {
            valid = true;
            }
          }
        return valid;
        } //end get_watermethod_string()

      //**********************************************************************
      //**********************************************************************
      void display_invalid_watermethod_errormsg(std::ostream& out)
        {
        out << "Invalid Water Method name, acceptable method names are:" << std::endl;
        for (int i=0; i < POSSIBLE_WATERMETHOD_STRINGS_CNT; ++i)
          {
          out << "   \"" << POSSIBLE_WATERMETHOD_STRINGS[i] << "\"" << std::endl; 
          }
        }

      //**********************************************************************
      //**********************************************************************
      /*
      void set_params_from_pst_id( int pst_id )
        {
        db2::StmtHandle stmt0( db2conn );
        char sqlcmd0[ SQLCMD_BUFSZ ];
        snprintf(sqlcmd0, SQLCMD_BUFSZ, "select "
                 " a1.paramvalue "
                 " from mdsystem.PST_Exp_Params as a1, "
                 " mdsystem.PST_Exp as a2, "
                 " mdsystem.PSTID_to_exp as a3 "
                 " where a3.pst_id=%d "
                 " and a3.exp_id=a2.exp_id "
                 " and a2.exp_param_id=a1.exp_param_id "
                 " and a1.paramname like 'water_method' "
                 " FOR READ ONLY WITH UR", pst_id );
        stmt0.prepare(sqlcmd0);
                
        char water_method_char[ SQLVARCHAR_BUFSZ ];
        
        stmt0.bindCol(1, (SQLCHAR*)&water_method_char[0], SQLVARCHAR_BUFSZ);

        try { stmt0.execute(); }
        catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

        if( !stmt0.fetch() )
          {
          std::cerr << "ERROR:: No water method defined for pst_id: " << pst_id
                    << std::endl;
          exit( 1 );
          }
        
        water_method = std::string( water_method_char );

        db2::StmtHandle stmt1( db2conn );
        char sqlcmd1[ SQLCMD_BUFSZ ];
        snprintf(sqlcmd1, SQLCMD_BUFSZ, "select "
                 " a1.paramvalue "
                 " from mdsystem.PST_Exp_Params as a1, "
                 " mdsystem.PST_Exp as a2, "
                 " mdsystem.PSTID_to_exp as a3 "
                 " where a3.pst_id=%d "
                 " and a3.exp_id=a2.exp_id "
                 " and a2.exp_param_id=a1.exp_param_id "
                 " and a1.paramname like 'shake_rattle_protein' "
                 " FOR READ ONLY WITH UR", pst_id );
        stmt1.prepare( sqlcmd1 );
                
        char sr_protein_char[ SQLVARCHAR_BUFSZ ];
                
        stmt1.bindCol( 1, (SQLCHAR *) &sr_protein_char[0], SQLVARCHAR_BUFSZ );

        try { stmt1.execute(); }
        catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

        if( !stmt1.fetch() )
          {
          std::cerr << "ERROR:: No shake/rattle protein defined for pst_id: " << pst_id
                    << std::endl;
          exit( 1 );
          }
        
        doShakeRattleProtein = atoi( sr_protein_char );
        
        db2::StmtHandle stmt2( db2conn );
        char sqlcmd2[ SQLCMD_BUFSZ ];
        snprintf(sqlcmd2, SQLCMD_BUFSZ, "select "
                 " a1.paramvalue "
                 " from mdsystem.PST_Exp_Params as a1, "
                 " mdsystem.PST_Exp as a2, "
                 " mdsystem.PSTID_to_exp as a3 "
                 " where a3.pst_id=%d "
                 " and a3.exp_id=a2.exp_id "
                 " and a2.exp_param_id=a1.exp_param_id "
                 " and a1.paramname like 'shake_rattle_water' "
                 " FOR READ ONLY WITH UR", pst_id );
        stmt2.prepare( sqlcmd2 );
                
        char sr_water_char[ SQLVARCHAR_BUFSZ ];
                
        stmt2.bindCol( 1, (SQLCHAR *) &sr_water_char[0], SQLVARCHAR_BUFSZ );

        try { stmt2.execute(); }
        catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

        if( !stmt2.fetch() )
          {
          std::cerr << "ERROR:: No shake/rattle water defined for pst_id: " << pst_id
                    << std::endl;
          exit( 1 );
          }
        
        doShakeRattleWater = atoi( sr_water_char );        

        db2::StmtHandle stmt3( db2conn );
        char sqlcmd3[ SQLCMD_BUFSZ ];
        snprintf(sqlcmd3, SQLCMD_BUFSZ, "select "
                 " a1.sys_id from "
                 " mdsystem.PSTID_to_exp as a1 "
                 " where a1.pst_id=%d FOR READ ONLY WITH UR", pst_id );
        stmt3.prepare( sqlcmd3 );
                                        
        stmt3.bindCol(1, (SQLINTEGER&) sys_id );

        try { stmt3.execute(); }
        catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

        if( !stmt3.fetch() )
          {
          std::cerr << "ERROR:: No system id defined for pst_id: " << pst_id
                    << std::endl;
          exit( 1 );
          }        
        }
      */
      //**********************************************************************

      //**********************************************************************
      //**********************************************************************
      /*
      int get_pst_id_from_sysid_and_run_time_options()
        {        
        db2::StmtHandle stmt0( db2conn );
        char sqlcmd0[ SQLCMD_BUFSZ ];
        snprintf(sqlcmd0, SQLCMD_BUFSZ, "select "
                 " a1.exp_param_id "
                 " from mdsystem.PST_Exp_Params as a1, mdsystem.PST_Exp_Params as a2, mdsystem.PST_Exp_Params as a3 "
                 " where a1.exp_param_id=a2.exp_param_id "
                 " and a2.exp_param_id=a3.exp_param_id "
                 " and a1.paramname like 'water_method' "
                 " and a1.paramvalue like '%s' "
                 " and a2.paramname like 'shake_rattle_protein' "
                 " and a2.paramvalue like '%d'"
                 " and a3.paramname like 'shake_rattle_water' "
                 " and a3.paramvalue like '%d'"
                 " FOR READ ONLY WITH UR", water_method.c_str(), doShakeRattleProtein, doShakeRattleWater );
        stmt0.prepare(sqlcmd0);
        
        int exp_param_id = -1;
        stmt0.bindCol(1, (SQLINTEGER&) exp_param_id );

        try { stmt0.execute(); }
        catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

        if( !stmt0.fetch() )
          {          
          db2::StmtHandle stmt1( db2conn );
          char sqlcmd1[ SQLCMD_BUFSZ ];
          
          snprintf(sqlcmd1, SQLCMD_BUFSZ, "select "
                   " max( exp_param_id ) "
                   " from mdsystem.PST_Exp_Params "
                   " FOR READ ONLY WITH UR");

          stmt1.prepare( sqlcmd1 );

          int max_exp_param_id = -1;
          stmt1.bindCol(1, (SQLINTEGER&) max_exp_param_id );

          try { stmt1.execute(); stmt1.fetch(); }
          catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
          
          int new_exp_param_id = max_exp_param_id + 1;
          
          db2::StmtHandle input_stmt0(this->db2conn);
          char input_sqlcmd0[SQLCMD_BUFSZ];

          snprintf( input_sqlcmd0, SQLCMD_BUFSZ, " insert into mdsystem.PST_Exp_Params "
                    " ( exp_param_id, paramname, paramvalue ) "
                    "   values (%d,'%s','%s')", 
                    new_exp_param_id, 
                    "water_method", water_method.c_str() );
          
          input_stmt0.prepare( input_sqlcmd0 );
          
          try { input_stmt0.execute(); }
          catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

          snprintf( input_sqlcmd0, SQLCMD_BUFSZ, " insert into mdsystem.PST_Exp_Params "
                    " ( exp_param_id, paramname, paramvalue ) "
                    "   values (%d,'%s','%d')", 
                    new_exp_param_id, 
                    "shake_rattle_protein", doShakeRattleProtein );
          
          input_stmt0.prepare( input_sqlcmd0 );
          
          try { input_stmt0.execute(); }
          catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

          snprintf( input_sqlcmd0, SQLCMD_BUFSZ, " insert into mdsystem.PST_Exp_Params "
                    " ( exp_param_id, paramname, paramvalue ) "
                    "   values (%d,'%s','%d')", 
                    new_exp_param_id, 
                    "shake_rattle_water", doShakeRattleWater );
          
          input_stmt0.prepare( input_sqlcmd0 );
          
          try { input_stmt0.execute(); }
          catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

          exp_param_id = new_exp_param_id;
          }

        
        db2::StmtHandle stmt2( db2conn );
        char sqlcmd2[ SQLCMD_BUFSZ ];
        snprintf(sqlcmd2, SQLCMD_BUFSZ, "select "
                 " exp_id "
                 " from mdsystem.PST_Exp "
                 " where exp_param_id=%d FOR READ ONLY WITH UR", exp_param_id );
        stmt2.prepare(sqlcmd2);

        int exp_id = -1;
        stmt2.bindCol(1, (SQLINTEGER&) exp_id );

        try { stmt2.execute(); }
        catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
        
        if( !stmt2.fetch() )
          {
          db2::StmtHandle input_stmt0(this->db2conn);
          char input_sqlcmd0[SQLCMD_BUFSZ];

          snprintf( input_sqlcmd0, SQLCMD_BUFSZ, " insert into mdsystem.PST_Exp "
                    " ( exp_param_id ) "
                    "   values ( %d )", 
                    exp_param_id );
          
          input_stmt0.prepare( input_sqlcmd0 );
          
          try { input_stmt0.execute(); }
          catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }                    
          
          db2::StmtHandle stmt3( db2conn );
          char sqlcmd3[ SQLCMD_BUFSZ ];
          snprintf(sqlcmd3, SQLCMD_BUFSZ, "select "
                   " exp_id "
                   " from mdsystem.PST_Exp "
                   " where exp_param_id=%d FOR READ ONLY WITH UR", exp_param_id );
          stmt3.prepare( sqlcmd3 );
          
          exp_id = -1;
          stmt3.bindCol(1, (SQLINTEGER&) exp_id );
          
          try { stmt3.execute();  }
          catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

          if( !stmt3.fetch() )
            {
            std::cerr << "ERROR:: DB2 is not generating us a new exp_id." << std::endl;
            exit( 1 );
            }
          }
        
        // Get the pst_id
        db2::StmtHandle stmt3( db2conn );
        char sqlcmd3[ SQLCMD_BUFSZ ];
        snprintf(sqlcmd3, SQLCMD_BUFSZ, "select "
                 " pst_id "
                 " from mdsystem.PSTID_to_exp "
                 " where exp_id=%d and sys_id=%d FOR READ ONLY WITH UR", exp_id, sys_id );
        stmt3.prepare( sqlcmd3 );

        int pst_id = -1;
        stmt3.bindCol( 1, (SQLINTEGER&) pst_id );

        try { stmt3.execute(); }        
        catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

        if( !stmt3.fetch() )
          {
          db2::StmtHandle input_stmt0(this->db2conn);
          char input_sqlcmd0[SQLCMD_BUFSZ];
          
          snprintf( input_sqlcmd0, SQLCMD_BUFSZ, " insert into mdsystem.PSTID_to_exp "
                    " ( exp_id, sys_id ) "
                    "   values ( %d, %d )", 
                    exp_id, sys_id );
          
          input_stmt0.prepare( input_sqlcmd0 );
          
          try { input_stmt0.execute(); }
          catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
          
          db2::StmtHandle stmt4( db2conn );
          char sqlcmd4[ SQLCMD_BUFSZ ];
          snprintf(sqlcmd4, SQLCMD_BUFSZ, "select "
                   " pst_id "
                   " from mdsystem.PSTID_to_exp "
                   " where exp_id=%d and sys_id=%d FOR READ ONLY WITH UR", exp_id, sys_id );
          stmt4.prepare( sqlcmd4 );
          
          pst_id = -1;
          stmt4.bindCol(1, (SQLINTEGER&) pst_id );
          
          try { stmt4.execute();  }
          catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

          if( !stmt4.fetch() )
            {
            std::cerr << "ERROR:: DB2 is not generating us a new pst_id." << std::endl;
            exit( 1 );
            }
          }
        
        if( pst_id < 0 )
          {
          std::cerr << "ERROR:: Invalid pst_id." << std::endl;
          exit( 1 );
          }     
        
        return pst_id;
        }
      */
      //**********************************************************************


  }; //end class
 
} //end namespace

#endif
