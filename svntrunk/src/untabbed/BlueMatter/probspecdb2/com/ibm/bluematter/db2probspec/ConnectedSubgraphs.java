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

public class ConnectedSubgraphs {

    private Vector adjList[] = null;
    private int visited[];
    private int adjListSize;
    private int UNSEEN = -1000000000;
    private int currentPartition;

    public ConnectedSubgraphs( BondListAsGraph blsg ) {
  adjList = blsg.getAdjList();

  adjListSize = blsg.getAdjListSize();
  visited = new int [ adjListSize ];
  classify();
    }

    private void classify() {
  for( int i=0; i < adjListSize; i++ ) {
      visited[i] = UNSEEN;
  } 
    
  currentPartition = 0;
  
  Stack touched = new Stack();
  visit(0, touched);
    }

    private void visit(int nodeId, Stack touched) {
  int newPartition = 0;
  
  for( int i=0; i< adjListSize; i++ ) {
      touched.push(new Integer(i));
      while (!touched.empty()) {
    int newNode = ((Integer) touched.pop()).intValue();
    if ( visited [ newNode ] != UNSEEN )
        continue;
    if (newPartition == 1) {
        ++currentPartition;
        newPartition = 0;
    }
      
    visited [ newNode ] = currentPartition;
    
    Vector adjListForNode = (Vector)  adjList[ newNode ];
    for (int j=0; j < adjListForNode.size(); j++) {
        int siteIdNode = ((SiteRep) adjListForNode.elementAt(j)).siteId;
        if(visited[ siteIdNode ] == UNSEEN)
      touched.push(new Integer(siteIdNode));
    }
      } // end of iteration over current connected subgraph
      newPartition = 1;
  }
    }

    // Returns the number of sites in the system
    public int nodeCount() {
  return visited.length;
    }
    
    public int partitionCount() {
  return currentPartition + 1;
    }

    public int subgraphLabel( int nodeId ) {
  return visited[ nodeId ];
    }
}
