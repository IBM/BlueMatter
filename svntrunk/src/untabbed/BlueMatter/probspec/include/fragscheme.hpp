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
 
#ifndef _INCLUDED_FRAGSCHEME_HPP
#define _INCLUDED_FRAGSCHEME_HPP

#include "list.hpp"


//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~
//  ~~~  Fragment:  Used to hold the minimal spec of which atoms
//  ~~~             belong in which fragment. 
//  ~~~
//  ~~~  FragmentationScheme:   A container class for Fragments
//  ~~~
//  ~~~ Author: MCP   
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

///////////////////////////////////////////////////////////
//             Fragment 
///////////////
class Fragment : public List
{
public:
  int NumberInternalSites;
  int *InternalSites;
  Fragment()
    {	   
      NumberInternalSites = 0;
      InternalSites = NULL;
    }
  ~Fragment()
    {
      if(InternalSites != NULL)
  { delete [] InternalSites;}      
    }

  virtual Fragment * Next(){return (Fragment *)d_next;}
  virtual Fragment * Prev(){return (Fragment *)d_prev;}
  Fragment & operator=(const Fragment &f)
    {
      if(InternalSites != NULL)
      delete [] InternalSites;
      NumberInternalSites = f.NumberInternalSites;
      if(NumberInternalSites == 0)
        InternalSites = NULL;
      else
      {
       InternalSites = new int[NumberInternalSites];
       for(int i=0;i<NumberInternalSites;i++)
       {  InternalSites[i] = f.InternalSites[i];	}
      }
      return *this;      
    }
};

/////////////////////////////////////////////////////////////
//                 FragmentationScheme 
//////////////////
class FragmentationScheme 
{
private:
  Fragment *d_fraglist;
  int d_nfrags;
  
public:

  FragmentationScheme()
    {
      d_fraglist = NULL;
      d_nfrags = 0;
    }
  ~FragmentationScheme()
    {
      if(d_fraglist != NULL)
  delete d_fraglist;
    }

  Fragment * GetFragList(){return d_fraglist;}

  int NFrags(){return d_nfrags;}
  int AddFragment(Fragment *f)
    {
      assert(f != NULL);
      if(d_fraglist == NULL)
  d_fraglist = f;
      else
  d_fraglist->Append(f);
      d_nfrags++;
    return d_nfrags; 
    }

};

#endif
