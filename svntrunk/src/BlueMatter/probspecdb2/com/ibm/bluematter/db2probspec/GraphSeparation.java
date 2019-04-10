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

public class GraphSeparation {
  
    private int maxSep = 0;
    private int visited[];
    private Vector adjList[] = null;
    private Vector graphSepList = new Vector();
    private int adjListSize;
    private int UNSEEN = -1000000000;

    public GraphSeparation(BondListAsGraph blag, int maxS)  throws Exception {
	this.maxSep = maxS;
	adjList = blag.getAdjList();
	
	adjListSize = blag.getAdjListSize();

	visited = new int [ adjListSize ];
	
	for (int i=0; i< adjListSize; i++)
	    graphSepList.add(new Vector());
	

	for (int i=0; i < adjListSize; i++) {
	    search(i);
	}
    }

    public Vector getGraphSepList() {
	return graphSepList;
    }

    private void search(int node) throws Exception {
	for( int i=0; i < adjListSize; i++) {
	    visited[i] = UNSEEN;
	} 
	
	LinkedList touched = new LinkedList();
	visit( node, 0, touched );
	for (int i = 0; i < visited.length; ++i) {
		if (visited[i] != UNSEEN) {
		    ((Vector) graphSepList.elementAt(node)).add(new IntegerPairRep(visited[i], i));
		}
	}
	Collections.sort( (Vector) graphSepList.elementAt(node) );
    }


    private void visit( int nodeId, int linkCount, LinkedList touched ) throws Exception {
	touched.addLast(new Integer(nodeId));
	visited[nodeId] = linkCount == 0 ? 0 : -linkCount;

	while(!touched.isEmpty()) {
	    int newNode = ((Integer) touched.removeFirst()).intValue();
	    
	    if (visited[ newNode ] > 0 )
		continue;

	    visited[ newNode ] = visited[ newNode ] == 0 ? 0 : -visited[ newNode ];
	    if (visited [ newNode ] < maxSep ) {
		int childLinkCount = visited[newNode] + 1;
		Vector adjListAtNode = (Vector) adjList[ newNode ];
		
		if (adjListAtNode == null) 
		    throw new Exception("GraphSeparation::visit(): adjListAtNode is NULL, newNode = " + newNode);
		    

		for (int i=0; i<adjListAtNode.size(); i++) {

		    int siteIdValue = ((SiteRep) adjListAtNode.elementAt(i)).siteId;

		    visited[ siteIdValue ] = visited[ siteIdValue ] > -childLinkCount ? 
			visited[siteIdValue] : -childLinkCount;

		    if( visited[ siteIdValue ] < 0 )
			touched.addLast(new Integer(siteIdValue));
		}
	    }
	}
    }
}
