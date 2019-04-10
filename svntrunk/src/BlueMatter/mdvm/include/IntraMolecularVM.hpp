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
 #ifndef _INTRAMOLECULARVM_HPP_
#define _INTRAMOLECULARVM_HPP_

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
//#define DUMP_STAT

//BGF: NEED: I don't see ANY reason for no coding this terms without
//           conditionals and macros.  We should decide fx at set up.
#include <BlueMatter/MSD_IF.hpp>
#include <BlueMatter/MDVM_IF.hpp>

#ifdef ADD_FORCES_IN_ORDER
class PartialForceUnit
{
public:
  XYZ mPartialForce[ MSD_IF::Access()::MaxRespaLevels ];
  int mSiteId;

  PartialForceUnit()
    {
      for (int i=0; i < MSD_IF::Access()::MaxRespaLevels; i++)
        {
          mPartialForce[i].Zero();
        }

      mSiteId = 0;
    }

  ~PartialForceUnit()
    {
    }
};

PartialForceUnit* partialForceList[ THREAD_NUM ];
#endif

template<class DynamicVariableManagerIF>
inline
void
IntraMolecularVM(Instruction*                aInstrs,
                 int                         aInstrsSize,
                 DynamicVariableManagerIF  & aDynVarMgrIF,
                 MDVM                      & aMDVM,
                 unsigned                    aSimTick
#ifdef ADD_FORCES_IN_ORDER
                 , PartialForceUnit*          aPartialForceList
#endif
                 )
{

  if ( aInstrs == NULL )
    return;

#ifdef DUMP_STAT
      aMDVM.clearStats();
#endif

  int i = 0;
  int k = 0;
  int j=0;
  int siteId;
  int tempReg;
  void* currentParameter = NULL;
  int* map = NULL;
  int prevNumSites=0;
  int prevRespaLevel=0;
  /////////////////////timebasestruct_t start, finish;


  int exit = 0;

#ifdef DO_PERFORMANCE_INTRA
  int enter1 = 1;
  int enter2 = 1;
  int enter3 = 1;
  int enter4 = 1;
#endif

  Instruction curInstr;
  MDVM::SiteReg* curSiteReg;
  XYZ *curForcePtr;
  int loadSiteCount=0;

  while ( !exit )
    {
      curInstr = aInstrs[i];

      switch( curInstr.instrId )
        {
        case LOAD_SITE:
          {

#ifdef DO_PERFORMANCE_INTRA
//////////            if (enter1)
//////////              ReadTime(GET_LOAD_SITE_TIME, start);
#endif

            siteId = curInstr.intInfo;
            tempReg = curInstr.charInfo;


            // The first time we get here we'll just add a zero force to
            // siteId 0, This is harmless.
            j=0;
            curSiteReg = aMDVM.AccessSiteReg( tempReg );
            curForcePtr = curSiteReg->mForce;

#ifdef ADD_FORCES_IN_ORDER
            int siteIdToAdd = curSiteReg->mSiteOffset;
            aPartialForceList[ loadSiteCount ].mSiteId = siteIdToAdd;
#endif
            while ( j < MSD_IF::Access().NumberOfRespaLevels)
              {
#ifdef ADD_FORCES_IN_ORDER
                aPartialForceList[ loadSiteCount++ ].mPartialForce[j] = curSiteReg->mForce[j];
#else
                aDynVarMgrIF.AddForce(  curSiteReg->mSiteOffset,
                                        aSimTick,
                                        j,
                                        curSiteReg->mForce[j]);
#endif
                curSiteReg->mForce[j].Zero();
                j++;
              }

            // For now load the positions in order without taking advantege of the map yet.
            curSiteReg->mPosition =  aDynVarMgrIF.GetPosition( siteId );
            curSiteReg->mSiteOffset = siteId;
            aMDVM.InvalidateDistanceForRegister( tempReg );

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
/////////// if (enter3)
///////////   ReadTime(GET_EXECUTE_TIME, start);
#endif



            map = curInstr.map;

            aMDVM.SetSiteForRegMap(0, map[0]);
            aMDVM.SetSiteForRegMap(1, map[1]);
            aMDVM.SetSiteForRegMap(2, map[2]);
            aMDVM.SetSiteForRegMap(3, map[3]);
            aMDVM.SetSiteForRegMap(4, map[4]);
            aMDVM.SetSiteForRegMap(5, map[5]);
            aMDVM.SetSiteForRegMap(6, map[6]);
            aMDVM.SetSiteForRegMap(7, map[7]);

            BegLogLine( PKFXLOG_INTRAMOLECULARVM )
              << "MDVM SET_MAP< : "
              << " map[0] " << map[0]  << " id " << aMDVM.AccessSiteOffsetRegister(map[0])
              << " map[1] " << map[1]  << " id " << aMDVM.AccessSiteOffsetRegister(map[1])
              << " map[2] " << map[2]  << " id " << aMDVM.AccessSiteOffsetRegister(map[2])
              << " map[3] " << map[3]  << " id " << aMDVM.AccessSiteOffsetRegister(map[3])
              << EndLogLine;


            BegLogLine( PKFXLOG_INTRAMOLECULARVM )
              << "MDVM EXECUTE START: "
              << " aInstr[ " << i << " ].shortInfo "
              << curInstr.shortInfo
              << " Param @" << (void *) currentParameter
              << EndLogLine;

            // Set the current respa Level
            aMDVM.SetRespaLevel( curInstr.charInfo );

#ifdef DO_PERFORMANCE_INTRA
/////////                PrintForPerformance ( GET_UDF_EXECUTE_TIME, "UDF_CODE: ", curInstr.shortInfo );
/////////                ReadTime( GET_UDF_EXECUTE_TIME, start);
                UDF_Binding::UDF_Execute(curInstr.shortInfo, aMDVM, currentParameter);
/////////                ReadTime( GET_UDF_EXECUTE_TIME, finish);
/////////                PrintTime( GET_UDF_EXECUTE_TIME, start, finish, "UDF_Execute Time: ");
#else
                UDF_Binding::UDF_Execute(curInstr.shortInfo, aMDVM, currentParameter);
#endif

            BegLogLine( PKFXLOG_INTRAMOLECULARVM )
              << "MDVM EXECUTE FINIS: "
              << " aInstr[ " << i << " ].shortInfo "
              << curInstr.shortInfo
              << " Param @" << (void *) currentParameter
              << EndLogLine;

            BegLogLine( PKFXLOG_INTRAMOLECULARVM )
              << "MDVM EXECUTE APPLY FORCE : "
              << "MDVM EXECUTE FINIS: "
              << " aInstr[ " << i << " ].shortInfo "
              << curInstr.shortInfo
              << " ENERGY " << aMDVM.AccessEnergy()
              << EndLogLine;

            aMDVM.ReportUdfEnergy( curInstr.shortInfo, aMDVM.GetEnergy() );
            map = NULL;
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
/////////// if(enter4)
///////////   ReadTime(GET_TERMINATION_TIME, start);
#endif

            MDVM::SiteReg* reg = NULL;
            k=0;
            while (k < SITE_REG_COUNT)
              {
                reg = aMDVM.AccessSiteReg( k );

                siteId = reg->mSiteOffset;

                j = 0;
                while ( j < MSD_IF::Access().NumberOfRespaLevels)
                  {
                    aDynVarMgrIF.AddForce( siteId, aSimTick, j, reg->mForce[ j ]);
                    reg->mForce[ j ].Zero();
                    j++;
                  }

                k++;
              }

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

#ifdef DUMP_STAT
  aMDVM.dumpStats();
#endif
}

#endif


