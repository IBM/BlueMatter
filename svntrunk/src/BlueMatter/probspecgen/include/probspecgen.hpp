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
 This code is retired

// ************************************************************************
// File: probspecgen.hpp
// Author: AR
// Date: June 22, 2001
// Class:
// Description: This class uses the forcelist data structure to generate
// the file which contains the specification of the MD problem
// ************************************************************************

#ifndef __PROBSPECGEN_HPP__
#define __PROBSPECGEN_HPP__

// NEED TO GET THIS FROM THE UDF_RegistryIF
#define MAX_SITE_COUNT  4

struct UdfInvocation
{
  int udfCode;
  int size;
  int* siteTupleList;
  void* paramTupleList;
  
  inline
  int* 
  GetNthTuple(int SitesPerTuple, int tupleNumber) 
    {
      int offset = SitesPerTuple*tupleNumber;
      assert((tupleNumber < size) && (offset >= 0 ));
      return siteTupleList + offset;
    } 
  
};

struct IrreduciblePartition
{
  int partitionId;
  int startSiteId;
  int numSites;
};

class IrreduciblePartitionHelper
{
public:
  int partitionId;
  int absSiteId;

  IrreduciblePartitionHelper()
    {
      partitionId = -1;
      absSiteId = -1;
    }
  
  ~IrreduciblePartitionHelper()
    {}

  static
  int compareSiteId(const void* c1, const void* c2)
    {
      IrreduciblePartitionHelper* iph1 = (IrreduciblePartitionHelper *) c1;
      IrreduciblePartitionHelper* iph2 = (IrreduciblePartitionHelper *) c2;
      
      if ( iph1->absSiteId < iph2->absSiteId )
	return -1;
      else if ( iph1->absSiteId == iph2->absSiteId )
	return 0;
      else return 1;
    }
  

  static
  int compare(const void* c1, const void* c2)
    {
      IrreduciblePartitionHelper* iph1 = (IrreduciblePartitionHelper *) c1;
      IrreduciblePartitionHelper* iph2 = (IrreduciblePartitionHelper *) c2;
      
      if ( iph1->partitionId < iph2->partitionId )
	return -1;
      else if ( iph1->partitionId == iph2->partitionId )
	{
	  if ( iph1->absSiteId < iph2->absSiteId )
	    return -1;
	  else if ( iph1->absSiteId == iph2->absSiteId )
	    return 0;
	  else 
	    return 1;
	}
      else 
	return 1;
    }

  int operator==(const IrreduciblePartitionHelper& iph) const
    {
      return (partitionId == iph.partitionId);
    }

};

struct ExcludedPair14
{
  int site1;
  int site2;
};

class ExcludedPair14Help
{
public:
  int site1;
  int site2;

  int operator==(const ExcludedPair14Help& ep) const
    {
      if( (site1 == ep.site1) &&
          (site2 == ep.site2) )
        {
          return 1;
        }

      return 0;
    }

  static int compare(const void* si1, const void* si2)
    {
      ExcludedPair14Help* ep1 = (ExcludedPair14Help *) si1;
      ExcludedPair14Help* ep2 = (ExcludedPair14Help *) si2;

      if (ep1->site1 < ep2->site1)
          return -1;
      else if ( ep1->site1 == ep2->site1 )
          {
            if ( ep1->site2 < ep2->site2 )
              return -1;
            else if (ep1->site2 == ep2->site2)
              return 0;
            else
              return 1;
          }
      else
        return 1;
    }
};

struct LJPairChar
{
  char epsilon[64];
  char sigma[64];
};

struct WorkOrderMap
{
  int workOrderIndex;
  int workOrderOffset;
};

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
      qsort(sites,
	    numSites,
	    sizeof(int),
	    compareSites);
    }

};

struct WorkOrder
{
  int udfCode;
  int paramIndex;
  int site1;
  int site2;
  int site3;
  int site4;
};

class SiteInfoChar
{
public:
  int siteId;
  char mass[64];
  char charge[64];
  int ljIndex;
  int lj14Index;
  int type;

  SiteInfoChar()
    {
      type = lj14Index = ljIndex = siteId = -1;
    }

  int operator==(const SiteInfoChar& si) const
    {
      if ( ( strcmp(mass,si.mass) == 0 ) &&
           ( strcmp(charge, si.charge) == 0 ) &&
           ( ljIndex == si.ljIndex ) &&
           ( lj14Index == si.lj14Index ) )
        {
          return 1;
        }

      return 0;
    }

  static int compareSiteInfoChar(const void* si1, const void* si2)
    {
      SiteInfoChar* siteInfo1 = (SiteInfoChar *) si1;
      SiteInfoChar* siteInfo2 = (SiteInfoChar *) si2;

      if (strcmp(siteInfo1->mass, siteInfo2->mass) < 0 )
        {
          return -1;
        }
      else if (strcmp( siteInfo1->mass, siteInfo2->mass) == 0)
        {
          if (strcmp(siteInfo1->charge, siteInfo2->charge) < 0 )
            {
              return -1;
            }
          else if (strcmp(siteInfo1->charge, siteInfo2->charge) == 0)
            {
              if ( siteInfo1->ljIndex < siteInfo2->ljIndex )
                {
                  return -1;
                }
              else if ( siteInfo1->ljIndex == siteInfo2->ljIndex )
                {
                  if ( siteInfo1->lj14Index < siteInfo2->lj14Index )
                    return -1;
                  else if ( siteInfo1->lj14Index == siteInfo2->lj14Index )
                    return 0;
                  else
                    return 1;
                }
              else
                return 1;
            }
          else
            return 1;
        }
      else
        return 1;
    }

  static int compareSiteIdChar (const void* si1, const void* si2)
    {
      SiteInfoChar* siteInfo1 = (SiteInfoChar *) si1;
      SiteInfoChar* siteInfo2 = (SiteInfoChar *) si2;

      if (siteInfo1->siteId < siteInfo2->siteId)
        return -1;
      else if (siteInfo1->siteId == siteInfo2->siteId)
        return 0;
      else return 1;
    }

};

struct SiteInfo
{
  double mass;
  double halfInverseMass;
  double charge;
  int ljIndex;
  int lj14Index;
    // int chargeIndex; //This is the same for charge14  
};

/*class SiteInfo
{
public:
  int siteId;
  double mass;
  double charge;
  int ljIndex;
  int lj14Index;
  int type;

  SiteInfo()
    {
      type = lj14Index = ljIndex = mass = charge = siteId = -1;
    }

  int operator==(const SiteInfo& si) const
    {
      if ( ( mass == si.mass ) &&
           ( charge == si.charge ) &&
           ( ljIndex == si.ljIndex ) &&
           ( lj14Index == si.lj14Index ) )
        {
          return 1;
        }

      return 0;
    }


  static int compareSiteInfo(const void* si1, const void* si2)
    {
      SiteInfo* siteInfo1 = (SiteInfo *) si1;
      SiteInfo* siteInfo2 = (SiteInfo *) si2;

      if (siteInfo1->mass < siteInfo2->mass)
        {
          return -1;
        }
      else if (siteInfo1->mass == siteInfo2->mass)
        {
          if (siteInfo1->charge < siteInfo2->charge)
            {
              return -1;
            }
          else if (siteInfo1->charge == siteInfo2->charge)
            {
              if ( siteInfo1->ljIndex < siteInfo2->ljIndex )
                {
                  return -1;
                }
              else if ( siteInfo1->ljIndex == siteInfo2->ljIndex )
                {
                  if ( siteInfo1->lj14Index < siteInfo2->lj14Index )
                    return -1;
                  else if ( siteInfo1->lj14Index == siteInfo2->lj14Index )
                    return 0;
                  else
                    return 1;
                }
              else
                return 1;
            }
          else
            return 1;
        }
      else
        return 1;
    }

  static int compareSiteId (const void* si1, const void* si2)
    {
      SiteInfo* siteInfo1 = (SiteInfo *) si1;
      SiteInfo* siteInfo2 = (SiteInfo *) si2;

      if (siteInfo1->siteId < siteInfo2->siteId)
        return -1;
      else if (siteInfo1->siteId == siteInfo2->siteId)
        return 0;
      else return 1;
    }
    }; */

#endif
