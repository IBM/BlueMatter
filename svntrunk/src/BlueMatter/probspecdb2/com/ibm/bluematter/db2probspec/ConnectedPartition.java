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


public class ConnectedPartition implements Comparable {

    private int partitionId;
    private int firstSiteId;
    private int numberOfSites;
    
    public ConnectedPartition(int partId, int startId ) {
	partitionId = partId;
	firstSiteId = startId;
    }

    public ConnectedPartition(int partId, int startId, int count ) {
	partitionId = partId;
	firstSiteId = startId;
        numberOfSites = count;
    }

    public ConnectedPartition() {
	partitionId = -99999;
	firstSiteId = -99999;
    }	

    public int getPartitionId() {
	return partitionId;
    }
    
    public int getFirstSiteId() {
	return firstSiteId;
    }

    public int getNumberOfSites() {
	return numberOfSites;
    }	

    public void setNumberOfSites(int e) {
	numberOfSites = e;
    }

    public int compareTo( Object obj ) {
	ConnectedPartition ip = (ConnectedPartition) obj;
        
	if( partitionId < ip.getPartitionId() ) 
	    return -1;
	else if ( partitionId == ip.getPartitionId()) {
	    if ( firstSiteId < ip.getFirstSiteId())
		return -1;
	    else if( firstSiteId == ip.getFirstSiteId())
		return 0;
	    else return 1;
	} else {
	    return 1;
	}
    }

    public boolean equalTo(ConnectedPartition ip) {
        if(ip == null)
            return false;

        // This is a hack for water
        return ( ip.getNumberOfSites() == this.numberOfSites ) && ( this.numberOfSites == 3);            
    }
    
}
