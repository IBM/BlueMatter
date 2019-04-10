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
// File: probspectrans.cpp
// Author: cwo 
// Date: June 15, 2006
// Namespace: probspectrans 
// Description: Main driver that starts and runs the DB transformations
//              of probspectrans
// ***********************************************************************

#include <BlueMatter/probspectrans/probspectrans.hpp>


static void display_arguments()
  {
  std::cout << "Usage: probspectrans [ arguments ]" << std::endl;
  std::cout << "  [-db    <string>]  : Name of DB to connect to" << std::endl;
  std::cout << "  [-sysid <int>   ]  : System ID to transform" << std::endl;
  std::cout << "  [-pstid <int>   ]  : PST ID to transform" << std::endl;
  std::cout << "  [-o     <string>]  : Name of output MSD file ('nil' = no output)" << std::endl;
  std::cout << "  [-wm    <string>]  : Name of water method to use" << std::endl;
  std::cout << "  [-verb  <int>   ]  : Verbosity level [0-5]" << std::endl;
  std::cout << "  [-no_srw        ]  : Turn off Shake/Rattle for waters" << std::endl;
  std::cout << "  [-no_srp        ]  : Turn off Shake/Rattle for proteins" << std::endl;
  } //end display_arguments()

int main(int argc, char* argv[])
{
  
  ////////////////////////////////////////////////////////////////////////////
  // Set command line parameter defaults 
  ////////////////////////////////////////////////////////////////////////////
//int system_id = -1;

  int pst_id = -1;
  int sys_id = -1;
  int verbosity = 1;
  int doShakeRattleWater = 1;
  int doShakeRattleProtein = 1;
  std::string water_method("TIP3P");
  
  char dbname[100];
  strncpy(dbname, "mdtest", 100);
  
  char outfile_name[100];
  strncpy(outfile_name, "probspectrans_output.msd", 100);

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
    if (strcmp("-sysid", argv[i])==0)
      {
      i++;
      int sys_id = atoi(argv[i]);
      }
    else if (strcmp("-pstid", argv[i])==0)
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
      strncpy(outfile_name, argv[i], 100);
      }
    else if (strcmp("-no_srw", argv[i])==0)
      {
      doShakeRattleWater = 0;
      }
    else if (strcmp("-no_srp", argv[i])==0)
      {
      doShakeRattleProtein = 0;
      }
    else if (strcmp("-wm", argv[i])==0)
      {
      i++;
      std::string input_method(argv[i]);
      if (probspectrans::RuntimeParams::check_if_valid_watermethod(input_method))
        {
        water_method = input_method;
        }
      else
        {
        probspectrans::RuntimeParams::display_invalid_watermethod_errormsg(std::cerr);
        exit(1);
        }
      }
    else
      {
      std::cout << "Invalid Arguments." << std::endl;
      display_arguments();
      exit(1);
      }
    } //end for each argument

  probspectrans::RuntimeParams rt_params( dbname );
  rt_params.sys_id = sys_id;
  rt_params.verbosity = verbosity;
  rt_params.doShakeRattleWater = doShakeRattleWater;
  rt_params.doShakeRattleProtein = doShakeRattleProtein;
  rt_params.water_method = water_method;

  if( pst_id == -1 )    
    pst_id = rt_params.get_pst_id_from_sysid_and_run_time_options();
  else
    {
    rt_params.set_params_from_pst_id( pst_id );
    }

  ////////////////////////////////////////////////////////////////////////////
  // Perform Transformation 
  ////////////////////////////////////////////////////////////////////////////
  // Display settings
  std::cout << "=============================================" << std::endl;
  std::cout << "BlueMatter" << std::endl;
  std::cout << "Running Problem Specficiation Transformations" << std::endl;
  std::cout << "=============================================" << std::endl;
  std::cout << "DB Name      : " << dbname << std::endl;
  std::cout << "PST ID    : " << pst_id << std::endl;
  std::cout << "System ID    : " << rt_params.sys_id << std::endl;
  if (strcmp("nil", outfile_name)!=0)
    {
    std::cout << "Output File  : " << outfile_name << std::endl;
    }
  std::cout << "Water Method : " << rt_params.water_method << std::endl;
  std::cout << "Shake / Rattle Water : " << rt_params.doShakeRattleWater << std::endl;
  std::cout << "Shake / Rattle Protein : " << rt_params.doShakeRattleProtein << std::endl;
  std::cout << "Verbosity    : " << rt_params.verbosity << std::endl;
  std::cout << "=============================================" << std::endl;

  // Create objects 
  probspectrans::UDF_RegistryIF *udf_registry =
        probspectrans::UDF_RegistryIF::instance(dbname, rt_params.sys_id, &rt_params);
  
  probspectrans::TransformSysParams *xform = 
        probspectrans::TransformSysParams::instance(dbname, pst_id, rt_params.sys_id, &rt_params,
        udf_registry);

  probspectrans::ProbSpecTransToMSD *db_to_MSD =
        probspectrans::ProbSpecTransToMSD::instance( dbname, pst_id, rt_params.sys_id );

  // Check if DB ok to write to 
  bool dirty_DB = xform->check_for_dirty_DB();
  if ( dirty_DB )
    {
    std::cout << "INFO: DB already contains MSD entries for system ID " << rt_params.sys_id
              << " and pst_id: " << pst_id << std::endl;
    }
  else
    {
    // Perform transformation 
    xform->transform_all_sysparams();
    }

  // Output transformation 
  if (strcmp("nil", outfile_name)!=0)
    {
    std::ofstream file_ostream;
    file_ostream.open(outfile_name, std::ios::out);
    db_to_MSD->output_entire_MSD(file_ostream);
    file_ostream.close();  
    }


} //end main()
