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
// File: probspectrans_to_MSD.cpp
// Author: cwo 
// Date: July 13, 2006
// Namespace: probspectrans 
// Class: ProbSpecTransToMSD
// Description: Reads DB entry from a run of probspectrans and outputs
//              strings with array in MSD file format
// ***********************************************************************

#include <BlueMatter/probspectrans/probspectrans_to_MSD.hpp>
#include <BlueMatter/probspectrans/transform_sysparams.hpp>

//////////////////////////////////////////
// Static globals for this file only

#define SQLCMD_BUFSZ 512
#define SQLVARCHAR_BUFSZ 255
#define STREAM_PRECISION 16

static int verbosity = 0;



static void display_arguments()
  {
  std::cout << "Usage: probspectrans_to_MSD [ arguments ]" << std::endl;
  std::cout << "  [-db    <string>]  : Name of DB to connect to" << std::endl;
  std::cout << "  [-sysid <int>   ]  : System ID to transform" << std::endl;
  std::cout << "  [-verb  <int>   ]  : Verbosity level" << std::endl;
  std::cout << "  [-o     <string>]  : Name of Output file" << std::endl;
  } //end display_arguments()

int get_sys_id_from_pst_id( char* dbname, int pst_id )
  {
  db2::ConnHandle& db2conn = db2::DBName::instance( dbname )->connection();
  
  db2::StmtHandle stmt1( db2conn );
  char sqlcmd1[ SQLCMD_BUFSZ ];
  snprintf( sqlcmd1, SQLCMD_BUFSZ, "select sys_id from MDSYSTEM.PSTID_to_exp "
            " where pst_id=%d FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare( sqlcmd1 );
  
  int sys_id = -1;
  stmt1.bindCol(1, (SQLINTEGER&)sys_id);

  try { stmt1.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
  
  if( !stmt1.fetch() )
    {
    std::cerr << "ERROR:: sys_id is not found for pst_id: " << pst_id << std::endl;
    }
    
  return sys_id;
  }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
  
  ////////////////////////////////////////////////////////////////////////////
  // Set command line parameter defaults 
  ////////////////////////////////////////////////////////////////////////////
  std::string output_filename = "probspectrans_to_MSD.output";
  char dbname[100];
  strncpy(dbname, "mdtest", 100);
  verbosity = 0;
  // int _id = 19531;
  int pst_id = -1;

  if (argc < 2)
    {
    display_arguments();
    exit(0);
    }

  ////////////////////////////////////////////////////////////////////////////
  // Read command line parameters
  ////////////////////////////////////////////////////////////////////////////
  for (int i=1; (i<argc) && ((argv[i])[0])=='-'; i++)
    {
    if (strcmp("-pstid", argv[i])==0)
      {
      i++;
      pst_id = atoi(argv[i]);
      }
    else if (strcmp("-verb", argv[i])==0)
      {
      i++;
      verbosity = atoi(argv[i]);
      }
    else if (strcmp("-db", argv[i])==0)
      {
      i++;
      strncpy(dbname, argv[i], 100);
      }
    else if (strcmp("-o", argv[i])==0)
      {
      i++;
      output_filename = std::string(argv[i]);
      }
    else
      {
      std::cout << "Invalid Arguments." << std::endl;
      display_arguments();
      exit(1);
      }
    } //end for each argument

  ////////////////////////////////////////////////////////////////////////////
  // Perform Actual Verification 
  ////////////////////////////////////////////////////////////////////////////
  int sys_id = get_sys_id_from_pst_id( dbname, pst_id );

  std::cout << "=====================================" << std::endl;
  std::cout << "BlueMatter" << std::endl;
  std::cout << "Problem Specficiation Transformations" << std::endl;
  std::cout << "DB to MSD Conversion Tool" << std::endl;
  std::cout << "=====================================" << std::endl;
  std::cout << "DB Name      : " << dbname << std::endl;
  std::cout << "PST ID    : " << pst_id << std::endl;
  std::cout << "SYS ID    : " << sys_id << std::endl;
  std::cout << "Verbosity    : " << verbosity << std::endl;
  std::cout << "Output File  : " << output_filename << std::endl;
  std::cout << "=====================================" << std::endl;  

  // int pst_id = rt_params.get_pst_id_from_sysid_and_run_time_options();

  probspectrans::ProbSpecTransToMSD *db_to_MSD = 
        probspectrans::ProbSpecTransToMSD::instance(dbname, pst_id, sys_id);

  std::ofstream file_ostream;
  file_ostream.open(output_filename.c_str(), std::ios::out);

  db_to_MSD->output_entire_MSD(file_ostream);
     
  file_ostream.close();  
 
} //end main()




