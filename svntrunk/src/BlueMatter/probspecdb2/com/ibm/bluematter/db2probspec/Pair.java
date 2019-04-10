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


public class Pair implements Comparable {

    private Comparable elem1;
    private Comparable elem2;

    public Pair(Comparable e1, Comparable e2) {
	elem1 = e1;
	elem2 = e2;
    }
    
    public Comparable getFirst() {
	return elem1;
    }

    public Comparable getSecond() {
	return elem2;
    }
    
    public int compareTo( Object obj ) {
	Pair p  = (Pair) obj;
	
	if(elem1.compareTo(p.getFirst()) < 0)
	    return -1;
	else if(elem1.compareTo(p.getFirst()) == 0)
	    if (elem2.compareTo(p.getSecond()) < 0)
		return -1;
	    else if (elem2.compareTo(p.getSecond()) == 0)
		return 0;
	    else 
		return 1;
	else
	    return 1;
    }
}
