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
 #ifndef _UPDATE__VM_HPP_
#define _UPDATE__VM_HPP_

#ifndef PKFXLOG_INTRAMOLECULARVM
#define PKFXLOG_INTRAMOLECULARVM  (0)
#endif

#ifndef PKFXLOG_UDF
#define PKFXLOG_UDF (0)
#endif

#define DO_PERFORMANCE_INTRA 1

#ifndef GET_LOAD_SITE_TIME
#define GET_LOAD_SITE_TIME 0
#endif
#ifndef GET_LOAD_PARAM_TIME
#define GET_LOAD_PARAM_TIME 0
#endif
#ifndef GET_EXECUTE_TIME
#define GET_EXECUTE_TIME 1
#endif
#ifndef GET_TERMINATION_TIME
#define GET_TERMINATION_TIME 0
#endif
#ifndef GET_UDF_EXECUTE_TIME
#define GET_UDF_EXECUTE_TIME 0
#endif

/* Intra molecular virtual machine
 *
 *      This toxic little number traverses the code list,
 * evaluating each term and sums the forces and energies.
 * The Beginning of each pass is a code type. The type dictates
 * How many operands follow. static registers are kept to
 * minimize the number of changes to the internal state.
 * The mChangeFlag of each code dictates when a new value
 * must be loaded.
 *
 *
 * This is a state machine which reads a list of
 * Intramolecular work orders, probably for a single
 * fragment and executes the work.
 *
 *
 * While this machine goes in order, that is not
 * garanteed by future versions.
 *
 */

#include <BlueMatter/UDF_Binding.hpp>
#include <BlueMatter/InstructionSet.hpp>

//BGF: NEED: I don't see ANY reason for no coding this terms without
//           conditionals and macros.  We should decide fx at set up.
#include <BlueMatter/MSD_IF.hpp>
#include <BlueMatter/MDVM_UVP_IF.hpp>

template<class DynamicVariableManagerIF>
inline
void
UpdateVPVM(Instruction*                aInstrs,
           int                         aInstrsSize,
           DynamicVariableManagerIF  & aDynVarMgrIF,
           MDVM_UVP                  & aMDVM,
           unsigned                    aSimTick )
{

  if ( aInstrs == NULL )
    return;

  int i = 0;
  int k = 0;
  int j=0;
  int siteId;
  int tempReg;
  void* currentParameter = NULL;
  int* map = NULL;
  int prevNumSites=0;
  int prevRespaLevel=0;
  ////////////////////timebasestruct_t start, finish;


  int exit = 0;

#ifdef DO_PERFORMANCE_INTRA
  int enter1 = 1;
  int enter2 = 1;
  int enter3 = 1;
  int enter4 = 1;
#endif

  Instruction curInstr;
  MDVM_UVP::SiteReg* curSiteReg;
  XYZ *curForcePtr;

  while ( !exit )
    {
      curInstr = aInstrs[i];
      switch( curInstr.instrId )
        {
        case LOAD_SITE:
          {

#ifdef DO_PERFORMANCE_INTRA
////////////if (enter1)
////////////  ReadTime(GET_LOAD_SITE_TIME, start);
#endif

            siteId = curInstr.intInfo;
            tempReg = curInstr.charInfo;

            aMDVM.SetSiteIdInRegister( tempReg, siteId );
            aMDVM.SetPositionInRegister( tempReg, aDynVarMgrIF.GetPosition( siteId ));
            aMDVM.SetVelocityInRegister( tempReg, aDynVarMgrIF.GetVelocity( siteId ));

#ifdef  DO_PERFORMANCE_INTRA
////////////if(enter1)
////////////  {
////////////     ReadTime(GET_LOAD_SITE_TIME, finish);
////////////     PrintTime(GET_LOAD_SITE_TIME, start, finish, "LOAD_SITE: ");
////////////     enter1 = 0;
////////////  }
#endif
            break;
          }

        case LOAD_PARAM:
          {

#ifdef DO_PERFORMANCE_INTRA
////////////if (enter2)
////////////  ReadTime(GET_LOAD_PARAM_TIME, start);
#endif

            MSD_IF::GetUDFParameterAddress(curInstr.shortInfo, curInstr.intInfo, &currentParameter);

#ifdef DO_PERFORMANCE_INTRA
////////////if(enter2)
////////////  {
////////////    ReadTime(GET_LOAD_PARAM_TIME, finish);
////////////    PrintTime(GET_LOAD_PARAM_TIME, start, finish, "LOAD_PARAM: ");
////////////    enter2 = 0;
////////////  }
#endif

            break;
          }

        case EXECUTE:
          {
#ifdef DO_PERFORMANCE_INTRA
////////////if (enter3)
////////////  ReadTime(GET_EXECUTE_TIME, start);
#endif

            BegLogLine( PKFXLOG_INTRAMOLECULARVM )
              << "MDVM EXECUTE START: "
              << " aInstr[ " << i << " ].shortInfo "
              << curInstr.shortInfo
              << " Param @" << (void *) currentParameter
              << EndLogLine;

#ifdef DO_PERFORMANCE_INTRA
////////////////PrintForPerformance ( GET_UDF_EXECUTE_TIME, "UDF_CODE: ", curInstr.shortInfo );
////////////////ReadTime( GET_UDF_EXECUTE_TIME, start);
            UDF_Binding::UDF_Execute(curInstr.shortInfo, aMDVM, currentParameter);
////////////////ReadTime( GET_UDF_EXECUTE_TIME, finish);
////////////////PrintTime( GET_UDF_EXECUTE_TIME, start, finish, "UDF_Execute Time: ");
#else
                UDF_Binding::UDF_Execute(curInstr.shortInfo, aMDVM, currentParameter);
#endif

                int siteCount;
                siteCount = UDF_Binding::GetNumSitesByUDFCode(curInstr.shortInfo);

                int zz=0;
                while( zz < siteCount )
                  {
                    aDynVarMgrIF.SetVelocity( aMDVM.GetSiteId(zz), aMDVM.GetVelocity(zz) );
                    aDynVarMgrIF.SetPosition( aMDVM.GetSiteId(zz), aMDVM.GetPosition(zz) );
                    zz++;
                  }

                BegLogLine( PKFXLOG_INTRAMOLECULARVM )
                  << "MDVM EXECUTE FINIS: "
                  << " aInstr[ " << i << " ].shortInfo "
                  << curInstr.shortInfo
                  << " Param @" << (void *) currentParameter
                  << EndLogLine;

                currentParameter = NULL;

#ifdef DO_PERFORMANCE_INTRA
//////////if(enter3)
//////////  {
//////////    ReadTime(GET_EXECUTE_TIME, finish);
//////////    PrintTime(GET_EXECUTE_TIME, start, finish, "EXECUTE: ");
//////////    enter3 = 0;
//////////  }
#endif
          break;
          }
        case TERMINATION:
          {

#ifdef DO_PERFORMANCE_INTRA
////////////if(enter4)
////////////  ReadTime(GET_TERMINATION_TIME, start);
#endif
            exit = 1;

#ifdef DO_PERFORMANCE_INTRA
////////////if(enter4)
////////////  {
////////////    ReadTime(GET_TERMINATION_TIME, finish);
////////////    PrintTime(GET_TERMINATION_TIME, start, finish, "TERMINATE: ");
////////////    enter4 = 0;
////////////  }
#endif
            break;
          }
        default:
          {
            assert(0);
            break;
          }
        }

      i++;
    }
}

#endif


