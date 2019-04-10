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

public class SimplePltParser {

    String filename = null;
    BufferedReader fIn = null;
    int lineCount = 0;
    Hashtable plt = null;


    public SimplePltParser(String file) {
  filename = file;
        plt = new Hashtable();

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
            System.out.println("SimplePltParser::parser():: Error reading the plt file");
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
      StringTokenizer tokenLine = new StringTokenizer(line);

      String name = getNextToken(tokenLine);

            if(name.equals("Compiler")) {
                // We have a define                
                String value = getNextToken(tokenLine);
                plt.put(name, value.trim());
            }
            else if( name.equals("CompilerHost") ) {
                // We have a define                
                String value = getNextToken(tokenLine);
                plt.put(name, value.trim());
            }
            else if( name.startsWith("-DPARTITION_SIZE") ) {
                String value_x = getNextToken(tokenLine);
                String value_y = getNextToken(tokenLine);
                String value_z = getNextToken(tokenLine);
                plt.put( "PARTITION_SIZE", value_x +":"+ value_y +":"+ value_z );
            }
            else if(name.startsWith("-q") || name.startsWith("-z") || name.startsWith("-O") || 
                    name.startsWith("-B") || name.startsWith("-t") ||
                    name.startsWith("-g") || name.startsWith("-D") ||
                    name.startsWith("-l") || name.startsWith("-b") || 
                    name.startsWith("-I") || name.startsWith("-L") ) {
                plt.put(name, name);
            }
            else if(name.equals("profiled")) {
                plt.put(name, name);
            }
            else if(name.equals("dump_lst")) {
                plt.put(name, name);
            }
            else if(name.equals("mpi")) {
                plt.put(name, name);
            }
            else if(name.equals("spi")) {
                plt.put(name, name);
            }
            else if(name.equals("blrts")) {
                plt.put(name, name);
            }
            else if(name.equals("double_hummer_3phase_pass")) {
                plt.put(name, name);
            }
            else if(name.equals("blade")) {
                plt.put(name, name);
            }
            else if(name.equals("Pk")) {
                plt.put(name, name);
            }
            else {
          throw new Exception("Can not recognize name: " + name + " on line: "+lineCount);
            }
            
            line = fIn.readLine();
      lineCount++;
  }
    
        return plt;
    }

    private String getNextToken(StringTokenizer st) throws Exception {
  if(!st.hasMoreElements())
      throw new Exception("Corrupted line: " + lineCount);

  return st.nextToken();
    }
}
