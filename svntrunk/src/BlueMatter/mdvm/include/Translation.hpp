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
 /************************************************************************
Name:           Alex Rayshubskiy
Program:        Translate.hpp
Date:           06/04/01
Company:        IBM Corp.
************************************************************************/
#ifndef _TRANSLATION_HPP_
#define _TRANSLATION_HPP_

#ifndef PKFXLOG_TRANSLATION
#define PKFXLOG_TRANSLATION  (0)
#endif

#ifndef PKFXLOG_WORKORDER
#define PKFXLOG_WORKORDER  (0)
#endif

//#define GET_UDF_COUNT_PER_THREAD

#include <BlueMatter/UDF_Binding.hpp>


/// REMOVE: as soon as Frank puts in IsUDFCodeCategory() into UDF_Binding.hpp
#include <BlueMatter/UDF_RegistryIF.hpp>

#include <BlueMatter/MSD_IF.hpp>
#include <BlueMatter/InstructionSet.hpp>
#include <BlueMatter/WorkOrderRep.hpp>
#include <BlueMatter/RunTimeGlobals.hpp>
#include <BlueMatter/MDVM_IF.hpp>

#define MAX_SITE_COUNT   4
#define CORRELATED_TERM  -1

extern RegRep* regTable;

/**************************************************
 *   THIS IS HERE FOR NOW. Until we figure out 
 *   where if anywhere the cost attribute should live
 *   There's a high chance that there will not be 
 *   a need for this in the fullness of time, because
 *   of dynamic load balancing. THIS IS JUST FOR NOW.
 **************************************************/
#define MAXCOST 5

static inline int GetCostByUDFCode(const int Code)
{
  switch (Code)
    {
    case UDF_Binding::DefaultForce_Code:
      return 0;
      break;
    case UDF_Binding::StdHarmonicBondForce_Code:
      return 2;
      break;
    case UDF_Binding::StdHarmonicAngleForce_Code:
      return 3;
      break;
    case UDF_Binding::SwopeTorsionForce_Code:
      return 4;
      break;
    case UDF_Binding::UreyBradleyForce_Code:
      return 3;
      break;
    case UDF_Binding::OPLSTorsionForce_Code:
      return 4;
      break;
    case UDF_Binding::OPLSImproperForce_Code:
      return 4;
      break;
    case UDF_Binding::ImproperDihedralForce_Code:
      return 4;
      break;
    case UDF_Binding::LennardJonesForce_Code:
      return 2;
      break;
    case UDF_Binding::StdChargeForce_Code:
      return 2;
      break;
    case UDF_Binding::TIP3PForce_Code:
      return 5;
      break;
    case UDF_Binding::KineticEnergy_Code:
      return 1;
      break;
    case UDF_Binding::WaterRigid3SiteShake_Code:
      return 2;
      break;
    case UDF_Binding::WaterRigid3SiteRattle_Code:
      return 2;
      break;
    case UDF_Binding::SPCForce_Code:
      return 2;
      break;
    case UDF_Binding::Coulomb14_Code:
      return 2;
      break;
    default: {
      return 2;
      break;
      }
    }
  return -1;
}



class SortHelper
{
public:
  int term[ MAX_SITE_COUNT ];
  WorkOrderHelp* workOrderPtr;

  SortHelper(){
    workOrderPtr = NULL;
  }

  ~SortHelper(){}

};

int compareNum(const void* num1, const void* num2)
{
  int i = *(int *)num1;
  int j = *(int *)num2;

  if (i < j)
    return -1;
  else
    return 1;
}

int compareSortHelper(const void* sortHelper1, const void* sortHelper2)
{
  return memcmp(sortHelper1, sortHelper2, sizeof(SortHelper) - sizeof(WorkOrderHelp *));
}

void sortWorkOrder(WorkOrderHelp* inputWorkOrder, const int workOrderSize, WorkOrderHelp*& outputWorkOrder)
{
  int j = 0;
  int n;
  int siteNum = 0;
  int paramNum = 0;
  int totalCount = 0;
  int myRespaLevel = 0;
  int l = 0;
  int udfCode;

  SortHelper* sortHelp = new SortHelper[ workOrderSize ];
  assert( sortHelp != NULL );


  // outPut list is an array of pointers
  int opc = 0;
  
  while( j < workOrderSize )
    {
      udfCode = inputWorkOrder[j].udfCode;
      sortHelp[j].workOrderPtr = &inputWorkOrder[j];
      opc++;

      BegLogLine( PKFXLOG_TRANSLATION )
        << "\t udfCode: " << sortHelp[j].workOrderPtr->udfCode
        << EndLogLine;

#if 0
      if (udfCode == 0)
        {
          // when udfCode == 0, we have a terminator code

          for(n = 0; n < MAX_SITE_COUNT; n++)
            {
              sortHelp[j].term[n] = CORRELATED_TERM;

              BegLogLine( PKFXLOG_TRANSLATION )
                << "\t codes: "
                << sortHelp[j].term[n] << "\t"
                << EndLogLine;
            }

          break;
        }
#endif

      siteNum = UDF_Binding::GetNumSitesByUDFCode(udfCode);
      paramNum = UDF_Binding::GetNumParamsByUDFCode(udfCode);
      totalCount = siteNum + paramNum;

      for (n = 0; n < siteNum; n++)
        {
          sortHelp[j].term[n] = inputWorkOrder[j].sites[n];
          BegLogLine( PKFXLOG_TRANSLATION )
            << sortHelp[j].term[n] << "\t"
            << EndLogLine;
        }

      Platform::Algorithm::Qsort(sortHelp[j].term,
            siteNum,
            sizeof(int),
            compareNum);

      BegLogLine( PKFXLOG_TRANSLATION )
        << " :: "
        << EndLogLine;

      for (int z = 0; z < siteNum; z++)
        {
          BegLogLine( PKFXLOG_TRANSLATION )
            << sortHelp[j].term[z] << "\t"
            << EndLogLine;
        }

      BegLogLine( PKFXLOG_TRANSLATION )
        << EndLogLine;


      l = 0;
      while ( l < (MAX_SITE_COUNT - siteNum) )
        {
          sortHelp[j].term[n] = CORRELATED_TERM;
          BegLogLine( PKFXLOG_TRANSLATION )
            << sortHelp[j].term[n] << "\t"
            << EndLogLine;
          n++;
          l++;
        }

      BegLogLine( PKFXLOG_TRANSLATION )
        << EndLogLine;
      j++;
    }

  /**************************************************************
   *                        Sorting Phase
   *************************************************************/

  Platform::Algorithm::Qsort( sortHelp,
         j,
         sizeof(SortHelper),
         compareSortHelper);


  /**************************************************************
   * Now that the sortHelp array is sorted we can write out
   * the sorted array.
   *************************************************************/
  j = 0;
  opc = 0;

  while(j < workOrderSize )
      {
      udfCode = sortHelp[j].workOrderPtr->udfCode;
      outputWorkOrder[j] = *(sortHelp[j].workOrderPtr);
      opc++;

      BegLogLine( PKFXLOG_TRANSLATION )
        << "\tUdfCode: " << outputWorkOrder[j].udfCode
        << EndLogLine;

      if (udfCode == 0)
        {
          // when udfCode == 0, we have a terminator code
          BegLogLine( PKFXLOG_TRANSLATION )
            << "\t Number of upcodes: " << opc
            << EndLogLine;
	  assert(1);
          break;
        }

      BegLogLine( PKFXLOG_TRANSLATION )
        <<  "\t codes: "
        << EndLogLine;

      for (int z = 0; z < outputWorkOrder[j].numSites; z++)
        {
          BegLogLine( PKFXLOG_TRANSLATION )
            << outputWorkOrder[j].sites[z] << "\t"
            << EndLogLine;
        }

      BegLogLine( PKFXLOG_TRANSLATION )
        << EndLogLine;

      j++;;
    }

  // SortHelper is no longer needed.
  delete sortHelp;
}

void testInstrs(Instruction *instrs, int size, int respaLevel, int threadId){

  ofstream os("translationTest.txt", ios::app);

  int n = 0 ;
  int opCodeCount = 0;

  os << "Respa Level: " << respaLevel << " ThreadId: " << threadId << endl;

  while (n < size) {

    switch(instrs[n].instrId)
      {
      case LOAD_SITE:
        {
          os << "\t load site: " << instrs[n].intInfo << endl;
          break;
        }
      case LOAD_PARAM:
        {
          os << "\t load param: " << instrs[n].intInfo << endl;
          break;
        }
      case EXECUTE:
        {

          os << "\t set map:";

          int k=0;
          while ( k < instrs[n].intInfo )
            {
              os << "\t " << instrs[n].map[k];
              k++;
            }
          os << endl;
          opCodeCount++;
          os << "\t execute UDF Code: " << instrs[n].shortInfo << "\n" <<endl;
          break;
        }
      case TERMINATION:
        {
          opCodeCount++;
          os << "\t terminate" << endl;
          os << "\n" << endl;
          os << "\t OpCodeCount = " << opCodeCount << endl;

          break;
        }
      default:
        {
          assert(0);
        }
      }
    n++;
  }
  os.close();
}

void
ClearRegTable()
{
  assert( regTable != NULL );

  for(int i=0; i < SITE_REG_COUNT; i++)
    {
      regTable[i].Clear();
    } 
}

void 
inline
AddToWorkOrder(WorkOrderHelp*& workOrder, 
	       int&            startIndex,
	       UdfInvocation   invocationRecord,
               int udfCode)
{
  int siteListSize = invocationRecord.size;
//  int udfCode      = invocationRecord.udfCode;  

  int i=0;
  int siteCount;
  int* siteTuple;// = invocationRecord.siteTupleList;
  
  while ( i < siteListSize )
    {
      workOrder[ startIndex ] = *(new WorkOrderHelp());
      siteCount = UDF_Binding::GetNumSitesByUDFCode( udfCode );

      workOrder[ startIndex ].sites = new int[ UDF_Binding::MAXSITECOUNT ];
      workOrder[ startIndex ].udfCode = udfCode;
      workOrder[ startIndex ].paramIndex = i;
      workOrder[ startIndex ].numSites = siteCount;
	
      siteTuple = invocationRecord.GetNthTuple(siteCount, i);
      
      for (int j=0; j <  UDF_Binding::MAXSITECOUNT; j++)
	{
	  if ( j < siteCount )
	    workOrder[ startIndex ].sites[ j ] = siteTuple[j];
	  else
	    workOrder[ startIndex ].sites[ j ] = -1;
	}
      
      startIndex++;
      i++;
    }
}

/************************************************************************
Description: The function translates the worklists into a list of
             instruction understood by the MDVM

Input:       1. array of code lists - codeLists
             2. array of sizes for each codeList - codeListSizes
             3. The size of the 2 arrays above.
             4. the size of the array and a respa level

Output:      A stream of instructions

Process:     The function is divided into 2 phases: Merging, Translating
             In the merging phase the worklists are merged into one
             long worklist, removing redundant lists. The translation
             phase then looks at the instruction codes and translates the
             instruction into a proper opcode, which will then be interpreted
             by the MDVM. The respa level determines the respa level at which
             we would like to count down from when computing the instruction
             stream. For instance we might only want to calculate for bond and
             angle and skip everything else.

NOTE:        1. There is room for optimization in the way that we choose the
             order in the worklist.
             2. In the future there will be capability for returning multiple
             instruction streams, BUT for now we just have 1.
*************************************************************************/

void Translate( Instruction* instrs[ THREAD_NUM ],
		int instrsSizes[ THREAD_NUM ],
		char* mode,
		int respaLevel)
{
  
  /*********************************************************
   *                        INIT
   ********************************************************/

  int forceMode             = 0;

  if(strcmp(mode, "UpdateForce") == 0)
   {
     forceMode = 1;
   }
  
  UdfInvocation* udfInvTable = MSD_IF::GetUdfInvocationTable();
  int udfInvTableSize        = MSD_IF::GetUdfInvocationTableSize();
  
  int i=0;
  int doUdf=0;
  int udfCode;    
  int workOrderSize = 0;
  
  while ( i < udfInvTableSize )
   {
        doUdf = 0;
   
	/// CHANGE: to UDF_Binding:: once implemented there.
        int tmpUdfCode = udfInvTable[i].udfCode;
                
        UDF_RegistryIF::IsUDFCodeCategory( tmpUdfCode, mode, doUdf );
        
      if (doUdf)
	workOrderSize += udfInvTable[i].size;
      
      i++;
    } 
  
  if(workOrderSize == 0)
    {
      // no udfs/work orders fo this mode
      i=0;
      while ( i < THREAD_NUM )
	{
	  instrsSizes[i] = 0;
	  i++;
	}
      return;
    }
  
  WorkOrderHelp* workOrder = new WorkOrderHelp[ workOrderSize ];
  assert( workOrder != NULL );
  int startIndex = 0;

  i=0;
  while ( i < udfInvTableSize )
    {
      /// CHANGE: to UDF_Binding:: once implemented there.
      doUdf = 0;
      int tmpUdfCode = udfInvTable[i].udfCode;
      
      UDF_RegistryIF::IsUDFCodeCategory( tmpUdfCode, mode, doUdf );
      if (doUdf)
	{
	  assert( startIndex < workOrderSize );
	  AddToWorkOrder(workOrder, startIndex, udfInvTable[i], tmpUdfCode);
	}
      i++;
    }

  WorkOrderHelp* workOrderOut =  NULL;
  if(forceMode)
    {
      // This is the table which contains the contents of the
      // registers. Needed for optimization
      regTable = new RegRep[ SITE_REG_COUNT ];
      assert ( regTable != NULL );
      assert( respaLevel >= 0 );
    
#define DO_SORT
#ifdef DO_SORT
      assert ( workOrderSize > 0 );
      workOrderOut = new WorkOrderHelp[ workOrderSize ];
      assert ( workOrderOut != NULL);
      sortWorkOrder(workOrder, workOrderSize, workOrderOut);
      workOrder = workOrderOut;
#endif
    }
  
  WorkOrderRep* finalWorkOrder = new WorkOrderRep[ workOrderSize + 1 ];
  assert ( finalWorkOrder != NULL );

  int j = 0;
  int k = 0;
  int siteNum = 0;
  int paramNum = 0;
  int totalCount = 0;
  int myRespaLevel = 0;
  int totalCost = 0;

  while( j < workOrderSize )
    {

      udfCode = workOrder[j].udfCode;

      if (forceMode)
	{
	  myRespaLevel = MSD_IF::Access().RespaLevelByUDFCode[ udfCode ];
	  assert( myRespaLevel >= 0 );
	  
	  if( myRespaLevel > respaLevel )
	    {
	      // If the respa level of this "opcode" is
	      // not in the range of desired respaLevel then
	      // skip this workorder from the list of codeReps printf("codeList[ %d ].mValue: %d\n", i, codeList[i].mValue);
	      j++;
	      continue;
	    }
	}

      siteNum = UDF_Binding::GetNumSitesByUDFCode(udfCode);
      paramNum = UDF_Binding::GetNumParamsByUDFCode(udfCode);

      finalWorkOrder[k] = *(new WorkOrderRep(udfCode, siteNum, paramNum));
      finalWorkOrder[k].sites = new int[ siteNum ];

      if (forceMode)
	finalWorkOrder[k].respaLevel = myRespaLevel;
      
      finalWorkOrder[k].workOrderId = k;

      for ( int i=0; i < siteNum; i++)
	{
	  finalWorkOrder[k].sites[i] = workOrder[j].sites[i];
	}
      
      finalWorkOrder[k].paramIndex = workOrder[j].paramIndex;

      totalCost += GetCostByUDFCode(udfCode);
      
      k++;
      j++;
    }
  
  // Once we reached the end of the workorder insert 
  // the termination instruction and exit
  //finalWorkOrder[k] = *(new WorkOrderRep(0,0,0));
  //  k++;
  

  /******************************************************
   *               THE TRANSLATION PHASE
   *****************************************************/
  // First figure out the numebr of instructions on each thread

  // The termination instruction should not be counted
  //  k--;  

  int tempSize[THREAD_NUM];
  j = 0;
  int threadCount = 0;
  int workOrderCount = 0;
  int limitCost0 = (int) (totalCost / THREAD_NUM);
  int limitCost1 = (int) (totalCost / THREAD_NUM) + MAXCOST;
  int curCost = 0;

#ifdef GET_UDF_COUNT_PER_THREAD
  int infoTemp[ THREAD_NUM ][ UDF_Binding::UDF_MAX_CODE+1 ];
  for(int ii=0; ii < THREAD_NUM; ii++)
    {
      for (int jj=0; jj < UDF_Binding::UDF_MAX_CODE+1 ; jj++)
	{
	  infoTemp[ii][jj] = 0;
	}
    }
#endif
  
  while( j < k )
    {
      udfCode = finalWorkOrder[ k - 1 - j ].udfCode;

#ifdef GET_UDF_COUNT_PER_THREAD
      infoTemp[ threadCount ][ udfCode ]++;
#endif

      curCost += GetCostByUDFCode( udfCode );
      
      
      if( (threadCount == 0) && ( curCost < limitCost0) )
	{
	  workOrderCount++; 
	}
      else if ( curCost < limitCost1 )
	{
	  workOrderCount++; 
	}
      else
	{
	  assert ( threadCount < THREAD_NUM );
	  workOrderCount++;
	  instrsSizes[ threadCount ] = workOrderCount;
	  //	  tempSize[ threadCount ] = workOrderCount;
	  //	  cerr <<  instrsSizes[ threadCount ] << endl;
	  threadCount++;
	  curCost = 0;
	  workOrderCount = 0;
	}
      j++;
    }
  instrsSizes[ threadCount ] = workOrderCount;

#if GET_UDF_COUNT_PER_THREAD
  cerr <<  instrsSizes[ threadCount ] << endl;


  for(int ii=0; ii < THREAD_NUM; ii++)
    {
      cerr << "Thread Num: " << ii << endl;
      for (int jj=0; jj < UDF_Binding::UDF_MAX_CODE+1 ; jj++)
	{
	  cerr << "UdfCode: " << jj << " Count: " << infoTemp[ii][jj] << endl;
	}
    }
#endif
  
  assert(threadCount+1 == THREAD_NUM);
  
  int sum=0;
  for(int zz=0; zz <= threadCount; zz++)
    {
      sum +=   instrsSizes[ zz ];
    }

  assert( sum == k );

  int p = 0;

  // The worst case is if we load 4 sites, 1 param and call execute.
  // This amounts to 6 instructions.
  int instrSize = 7*workOrderSize / THREAD_NUM;
  
  // At this point k is the length of the finalCodesList minus the termination code
  int index = 0;
  int n;
  p=0;

  j = k-1; 
  while ( p < THREAD_NUM )
    {
      instrs[ p ] = new Instruction[ instrSize ];
      assert ( instrs[ p ] != NULL );
      index = 0;
      n = 0;

      // Very important!!!
      if(forceMode)
	ClearRegTable();

      while( n < instrsSizes[ p ] )
	{
	  assert( j >= 0 );
	  if(forceMode)
	    finalWorkOrder[j].translateForce( instrs[ p ], index, instrSize);
	  else
	    finalWorkOrder[j].translateGeneral( instrs[ p ], index, instrSize);
	  j--;
	  n++;
	}
      instrs[ p ][ index++ ] =  *(new Instruction(TERMINATION, 0));
      instrsSizes[ p ] = index;
#if 0
      testInstrs( instrs[ p ], index, respaLevel, p );
#endif      
      p++;
    }
  
  // Translate the termination code
  //  finalWorkOrder[k-1].translate(instrs, index, instrSize);
  //  instrsSize = k;
  
  if (forceMode)
    delete [] regTable;

  delete [] finalWorkOrder;

#ifdef DO_SORT
  if (forceMode &&  (workOrderOut != NULL ))
    {
      delete [] workOrderOut;
      workOrderOut = NULL;
    }
#endif
}
#endif
