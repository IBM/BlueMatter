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
 class PartitionIterator
{
public:
  int numSites;
  
  struct PartitionMember
  {
    int startSiteId; 
    int runCount;
  };

  PartitionMember* members;
  int memCount;
  int curMember;
  
  // The offset from the start site
  int curSiteOffset;

  PartitionIterator()
    {
      numSites = -1;
      members = NULL;
      memCount = 0;
      curMember = 0;
      curSiteOffset = 0;
    }
  
  ~PartitionIterator()
    {
      if ( members != NULL )
  delete members;
    }
  
  inline
  int
  HasMoreElements()
    {
      return !( ( curMember == memCount ) &&
         ( curSiteOffset == members[ curMember].runCount) );	
    }

  inline
  int 
  GetNextSite() 
    {
      if ( curSiteOffset == members[ curMember].runCount ) 
  {
    curMember++;
    curSiteOffset = 0;
  }
      
      int siteId = members[ curMember].startSiteId + curSiteOffset;
      curSiteOffset++;
      return siteId; 
      
    }

  inline
  void
  Reset()
    {
      curMember=0;
      curSiteOffset=0;
    }

  inline
  int 
  GetNumOfSites()
    {
      return numSites;
    }
};
