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
 package com.ibm.bluematter.utils;

import java.util.*;
import java.io.*;

public class SimpleImplParser {

    String filename = null;
    BufferedReader fIn = null;
    int lineCount = 0;
    Hashtable impl = null;


    public SimpleImplParser(String file) {
	filename = file;
        impl = new Hashtable();

	try {
	    fIn = new BufferedReader(new FileReader(file));
	} catch (FileNotFoundException e) {
            System.err.println(e.getMessage());
            System.exit(1);
        }
    }

    public Hashtable parse() throws Exception {
	String line = null;
        try {
            line         = fIn.readLine();
        } catch (IOException e) {
            e.printStackTrace();
            System.out.println("SimpleImplParser::parser():: Error reading the rtp file");
            System.exit(1); 
        }

	lineCount = 0;
	while(line != null) {
            
	    line.trim();	    
	    if(line.startsWith("#") || line.equals("")) {
                line = fIn.readLine();
                lineCount++;
                continue; // skip comments and blank lines
            }
	    StringTokenizer tokenLine = new StringTokenizer( line );

	    String name = getNextToken( tokenLine );

            if(name.startsWith("-D")) {
                // We have a define
                impl.put(name, name+":1");
            }
            else if( name.equals("NSQ") ) {
                String value = getNextToken( tokenLine );
                impl.put(name, value+":0");
            }
            else if( name.equals("IFP") ) {
                String value = getNextToken( tokenLine );
                impl.put(name, value+":0");
            }
            else if( name.equals("TIMESTEPS_PER_RDG") ) {
                String value = getNextToken( tokenLine );
                impl.put(name, value+":0");
            }
            else if( name.equals("FFT3D") ) {
                String value = getNextToken( tokenLine );
                impl.put(name, value+":0");
            }
            else if( name.equals("FFT") ) {
                String value = getNextToken( tokenLine );
                if( value.equals("fftw") || value.equals("fftw_threaded") || 
                    value.equals("fftw_mpi") || value.equals("spi_based") || value.equals("mpi_based") ||
                    value.equals("spi_based_essl") || value.equals("mpi_based_essl")
                    ) {
                    
                    if( value.equals("spi_based") || value.equals("mpi_based") ||
                    	value.equals("spi_based_essl") || value.equals("mpi_based_essl")
                    		) 
                      {
                          String procSize1 = getNextToken( tokenLine );
                          String procSize2 = getNextToken( tokenLine );
                          String procSize3 = getNextToken( tokenLine );
                          
                          value = value + ":" + procSize1 + "_"+ procSize2 + "_"+ procSize3;
                          
                          if( tokenLine.hasMoreElements() )
                            {
                            String fftPrecisionType = getNextToken( tokenLine );
                            if( fftPrecisionType.equals("float") ||
                                fftPrecisionType.equals("double") )
                              {
                              value += "-" + fftPrecisionType;
                              }
                            }
                          else
                            value += "-double";
                      }
                    
                    impl.put(name, value+":0");
                }
                else
                    throw new Exception("ERROR:: Can not recognize value: " + value + " on line: "+lineCount);
            }
            else if( name.equals("pktrace")) {
                impl.put(name,name+":0");
            }
            else if( name.equals("blnie")) {
                impl.put(name,name+":0");
            }
            else if( name.equals("bgl")) {
                impl.put(name,name+":0");
            }
            else if( name.equals("bgp")) {
                impl.put(name,name+":0");
            }
            else {
       		throw new Exception("ERROR:: Can not recognize name: " + name + " on line: "+lineCount);
            }
            
            line = fIn.readLine();
	    lineCount++;
	}
    
        return impl;
    }

    private String getNextToken(StringTokenizer st) throws Exception {
	if(!st.hasMoreElements())
	    throw new Exception("Corrupted line: " + lineCount);

	return st.nextToken();
    }
}
