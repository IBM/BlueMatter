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
 package com.ibm.bluematter.db2probspec;
import java.io.*;
import java.util.*;


public class IntegerPairRep implements Comparable {

    public int element1;
    public int element2;
    private int optElement;
    
    public IntegerPairRep(int partId, int startId ) {
  element1 = partId;
  element2 = startId;
    }

    public IntegerPairRep(int partId, int startId, int opt ) {
  element1 = partId;
  element2 = startId;
        optElement = opt;
    }

    public IntegerPairRep() {
  element1 = -99999;
  element2 = -99999;
    }	

    public int getFirst() {
  return element1;
    }
    
    public int getSecond() {
  return element2;
    }

    public int getOptElement() {
  return optElement;
    }	

    public void setFirst(int e) {
  element1 = e;
    }
    
    public void incSecond()
    {
        element2++;
    }

    public void setSecond(int e) {
  element2 = e;
    }

    public void setOptElement(int e) {
  optElement = e;
    }

    public int compareTo( Object obj ) {
  IntegerPairRep ip = (IntegerPairRep) obj;
        
  if(element1 < ip.getFirst()) 
      return -1;
  else if ( element1 == ip.getFirst()) {
      if (element2 < ip.getSecond())
    return -1;
      else if( element2 == ip.getSecond())
    return 0;
      else return 1;
  } else {
      return 1;
  }
    }

    public boolean equalTo(IntegerPairRep ip) {
        if(ip == null)
            return false;
        
        return ( ip.getOptElement() == this.optElement );            
    }
    
}
