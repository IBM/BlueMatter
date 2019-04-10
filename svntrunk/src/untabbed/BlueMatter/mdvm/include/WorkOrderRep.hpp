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
 #ifndef __WORKORDER_REPRESENTATION__
#define __WORKORDER_REPRESENTATION__

#include <BlueMatter/InstructionSet.hpp>
#include <BlueMatter/MDVM_IF.hpp>
#include <BlueMatter/Translation.hpp>

#define BIG_NUM 1000000

class WorkOrderHelp
{
public:
  int udfCode;
  int paramIndex;
  int* sites;
  int numSites;
  int siteOffset; // this is the first site

  WorkOrderHelp()
    {
      udfCode = -1;
      paramIndex = -1;
      sites = NULL;
    }

  ~WorkOrderHelp(){
    if(sites != NULL)
      delete sites;
  }

  int operator==(const WorkOrderHelp& wo) const
    {
      if( (udfCode == wo.udfCode) &&
          (paramIndex == wo.paramIndex))

        {
          for (int i = 0; i < numSites; i++)
            {
              if (sites[i] != wo.sites[i])
                return 0;
            }

          return 1;
        }

      return 0;
    }

  static int compare(const void* p1, const void* p2)
    {
      WorkOrderHelp * wo1 = (WorkOrderHelp *) p1;
      WorkOrderHelp * wo2 = (WorkOrderHelp *) p2;

      if (wo1->udfCode < wo2->udfCode)
          return -1;
      else if (wo1->udfCode == wo2->udfCode)
        {
          if (wo1->paramIndex < wo2->paramIndex)
            return -1;
          else if ( wo1->paramIndex == wo2->paramIndex )
            {
              for (int i=0; i < wo1->numSites; i++)
                {
                  if (wo1->sites[i] < wo2->sites[i])
                    return -1;
                  else if (wo1->sites[i] == wo2->sites[i])
                    {
                      if (i == (wo1->numSites)-1)
                        return 0;
                      continue;
                    }
                  else return 1;
                }
            }
          else
            return 1;
        }
      else
        return 1;

      return 1;
    }
  
 static int compareSites( const void* site1, const void* site2)
    {
      int* s1 = (int *) site1;
      int* s2 = (int *) site2;

      if ( *s1 < *s2)
  return -1;
      else if ( *s1 == *s2 )
  return 0;
      else 
  return 1;
    }
  
  void sortSites()
    {
      Platform::Algorithm::Qsort(sites,
      numSites,
      sizeof(int),
      compareSites);
    }

};

class RegRep
{
public: 
  int pinOn;
  int regContent;
  int regSeqCount;
  
  RegRep() 
    {
      pinOn = 0;
      regContent = -1;
      regSeqCount = BIG_NUM;
    }
  
  void
  Clear()
    {
      pinOn = 0;
      regContent = -1;
      regSeqCount = BIG_NUM;
    }
};

RegRep* regTable;

//#define MAX_SITE_NUM 4

// Create an abstract base class for all types
// of code representation 
class WorkOrderRep 
{
public:
  // The codeType corresponds to UDF_Registry codes
  // ex. ANGLECODE, TORSIONCODE, etc...
  int udfCode;
  int workOrderId;
  int numSites;
  int numParams;
  int* sites;
  int paramIndex;
  int respaLevel;
  
  WorkOrderRep() 
    {
      workOrderId = -1;
      udfCode = 0;
      numSites = 0;
      numParams = 0;
      respaLevel = -1;
      sites = NULL;
      paramIndex = -1;
    }
  
  WorkOrderRep(int ct, int siteNum, int paramNum) 
    { 
      udfCode = ct;
      numSites = siteNum;
      numParams = paramNum;
    }
  
  ~WorkOrderRep(){
    if (sites != NULL) 
      {
  delete sites;
      }
  }

  inline
  void 
  translateGeneral(Instruction *& instrs, int & index, int size) 
    {
      int i = 0;
      while (i < numSites ) 
  {
    // Need to choose the register to load to.
    instrs[index] = *(new Instruction( LOAD_SITE, sites[i] ));
    instrs[index].charInfo = i;
    index++;
    i++;
  }
      
      int j = 0;
      while (j < numParams) 
  {
    instrs[index] = *(new Instruction(LOAD_PARAM, paramIndex));
    assert( udfCode != 0 );
    instrs[index].shortInfo = udfCode;
    index++;
    j++;
  }


      instrs[index] = *(new Instruction());
      instrs[index].instrId = EXECUTE;
      instrs[index].shortInfo = udfCode;
      instrs[index].intInfo = numSites;
      index++;
    }

  /******************************************************************
   * The translate function take a array of pointers to instructions
   * it begins to fill the array starting at the index. The index is
   * updated every time a new instruction is written to the array.
   *******************************************************************/
  
  inline
  void 
  translateForce(Instruction *& instrs, int & index, int size) 
    {
      assert(size > index);
    
      if (udfCode == 0) 
  {
    instrs[index] = *(new Instruction(TERMINATION, 0));
    index++;
    return;
  } 

      int i = 0;
      int maskReg = 0;
      int shifter = 0;

      // These are the registers which are pinned.
      int* regMapForSiteIndex = new int[SITE_REG_COUNT];
      assert ( regMapForSiteIndex != NULL );
      
      // Make a first pass seeing which registers need to be pinned.
      // i.e. which sites are already in registers.
      for (int k=0; k < numSites; k++) 
  {
    regMapForSiteIndex[k] = -1;
    
    for (int l=0; l < SITE_REG_COUNT; l++) 
      {
        if( sites[k] == regTable[l].regContent ) 
    {
      // Our site is in the register. Register l is not available for dumping
      
      // Set the Mask
      shifter = 1;
      shifter = shifter << l;
      maskReg |= shifter;
      
      // Set the site --> reg mapping
      regMapForSiteIndex[k] = l;
      
    }
      }
  }
      
      int reg;
      while (i < numSites ) 
  {
    
    // case 1: The siteId was not in any register
    if( regMapForSiteIndex[i] == -1 ) 
      { 
        reg = GetRegisterMRU(maskReg);
        
        // Need to choose the register to load to.
        instrs[index] = *(new Instruction( LOAD_SITE, sites[i] ));
        instrs[index].charInfo = reg;
        index++;
        
        regTable[reg].regContent = sites[i];
        regMapForSiteIndex[i] = reg;
      }
    
    regTable [ regMapForSiteIndex[i] ].regSeqCount = workOrderId;
    
    i++;
  }

      // At this point all the sites are given a register.
      // We are ready to set the map

      // Need to keep i around to correctly index into the codeList
      int j = 0;

      while (j < numParams) 
  {
    instrs[index] = *(new Instruction(LOAD_PARAM, paramIndex));
    assert( udfCode != 0 );
    instrs[index].shortInfo = udfCode;
    index++;
    j++;
  }


      instrs[index] = *(new Instruction());
      instrs[index].instrId = EXECUTE;
      instrs[index].shortInfo = udfCode;
      instrs[index].intInfo = numSites;
      instrs[index].map =  regMapForSiteIndex;
      instrs[index].charInfo = respaLevel;
      index++;
    
      //      delete [] regMapForSiteIndex;
      
    }

  inline
  int
  GetBits(unsigned x, unsigned p, unsigned n) /* get n bits from position p */
    {
      return ((x >> (p+1-n)) & ~(~0 << n));
    }


  inline
  int GetRegisterLRU(int & maskReg) 
    {
      int minCount = 1000000;
      int reg = -1;
      for(int m=0; m < SITE_REG_COUNT; m++) {
  if( !GetBits(maskReg, m, 1) && (regTable[m].regSeqCount < minCount )) 
    {
      minCount = regTable[m].regSeqCount;
      reg = m;
    }
      }
      
      // Mark the register is taken
      maskReg = maskReg | (1 << reg);
      
      return reg;
    }

  inline
  int GetRegisterMRU(int & maskReg) 
    {
      int maxCount = -1;
      int reg = -1;
      for(int m=0; m < SITE_REG_COUNT; m++) {
  if(!GetBits(maskReg, m, 1) && (regTable[m].regSeqCount > maxCount )) 
    {
      maxCount = regTable[m].regSeqCount;
      reg = m;
    }
      }
      
      // Mark the register is taken
      maskReg = maskReg | (1 << reg);
      
      return reg;
    }
};

#endif 
