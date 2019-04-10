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
// File: runMD.cpp
// Author: cwo 
// Date: August 14, 2006
// Namespace: concept_dbalg
// Description: Simple test run of the containers MD loop
// ***********************************************************************

#include <BlueMatter/concept_dbalg/RuntimeParams.hpp>
#include <BlueMatter/concept_dbalg/TimeStep.hpp>


void display_arguments()
  {
  std::cout << "Usage: runMD [ arguments ]" << std::endl;
  std::cout << "  [-db     <string>]  : Name of DB to connect to" << std::endl;
  std::cout << "  [-pstid  <int>   ]  : PST ID to run with" << std::endl;
  std::cout << "  [-dvs    <string>]  : Name of input DVS file" << std::endl;
  std::cout << "  [-rdg    <string>]  : Name of output RDG file" << std::endl;
  std::cout << "  [-cutoff <double>]  : NSQ Forces cutoff radius in A" << std::endl;
  std::cout << "  [-verb   <int>   ]  : Verbosity level [0-5]" << std::endl;
  } //end display_arguments()

int main(int argc, char* argv[])
{
  
  ////////////////////////////////////////////////////////////////////////////
  // Set command line parameter defaults 
  ////////////////////////////////////////////////////////////////////////////
  int pst_id = -1;
  int verbosity = 1;
  double cutoff = 10;
  
  char dbname[100];
  strncpy(dbname, "mdtest", 100);
  
  char dvs_name[256];
  strncpy(dvs_name, "NoDVSFilenameProvided", 100);
  char rdg_name[256];
  strncpy(rdg_name, "RDG_OUTPUT.rdg", 100);

  
  if (argc < 6)
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
    else if (strcmp("-dvs", argv[i])==0)
      {
      i++;
      strncpy(dvs_name, argv[i], 100);
      }
    else if (strcmp("-rdg", argv[i])==0)
      {
      i++;
      strncpy(rdg_name, argv[i], 100);
      }
    else if (strcmp("-cutoff", argv[i])==0)
      {
      i++;
      cutoff = atof(argv[i]);
      }
    else
      {
      std::cout << "Invalid Arguments." << std::endl;
      display_arguments();
      exit(1);
      }
    } //end for each argument

  concept_dbalg::RuntimeParams rt_params(dbname);

  // Params that come from PST
  rt_params.sys_id = -1;
  rt_params.water_method = std::string("UNDEF_WATERMETHOD");
  rt_params.doShakeRattleWater = -1;
  rt_params.doShakeRattleProtein = -1;
  //rt_params.set_params_from_pst_id( pst_id ); //Don't want to connect to DB2

  // Simulation parameters 
  rt_params.pst_id = pst_id;
  rt_params.verbosity = verbosity;
  rt_params.dvs_filename = std::string(dvs_name);
  rt_params.rdg_filename = std::string(rdg_name);
  rt_params.timestep_length = 0.0409;  // 0.002fs / TimeConversionFactor=0.04882
  rt_params.cutoff_dist = cutoff;
  rt_params.switch_width = 1;

  //rt_params.set_params_from_pst_id( pst_id );

  ////////////////////////////////////////////////////////////////////////////
  // Perform Transformation 
  ////////////////////////////////////////////////////////////////////////////
  // Display settings
  std::cout << "=============================================" << std::endl;
  std::cout << "BlueMatter" << std::endl;
  std::cout << "Pure Containers MD Simulation" << std::endl;
  std::cout << "=============================================" << std::endl;
  std::cout << "DB Name                 : " << dbname << std::endl;
  std::cout << "PST ID                  : " << rt_params.pst_id << std::endl;
  std::cout << "System ID               : " << rt_params.sys_id << std::endl;
  std::cout << "Input DVS File          : " << rt_params.dvs_filename << std::endl;
  std::cout << "Output RDG File         : " << rt_params.rdg_filename << std::endl;
  std::cout << "Do Shake/Rattle Water   : " << rt_params.doShakeRattleWater << std::endl;
  std::cout << "Do Shake/Rattle Protein : " << rt_params.doShakeRattleProtein << std::endl;
  std::cout << "Water Method            : " << rt_params.water_method << std::endl;
  std::cout << "NSQ Cutoff Radius       : " << rt_params.cutoff_dist << std::endl;
  std::cout << "Verbosity               : " << rt_params.verbosity << std::endl;
  std::cout << "=============================================" << std::endl;

  // Create objects 
 
  concept_dbalg::TimeStep *myTimeStep = 
          concept_dbalg::TimeStep::instance(&rt_params);

  myTimeStep->core_loop(2);

  return 0;
} //end main()

