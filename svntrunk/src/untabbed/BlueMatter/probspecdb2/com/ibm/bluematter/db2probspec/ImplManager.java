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

import java.util.*;
import java.sql.*;
import java.io.*;
import com.ibm.bluematter.db2probspec.*;

public class ImplManager 
{
    private static int implId = -1;
    private static Connection conn = null;
    private static Statement  stmt = null;

    public static void init( int impl_id, Connection con) 
    {
        implId = impl_id;
        conn = con;
        
        try {
            stmt = con.createStatement();
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(1);
        } 
    }

    public static String getFFTType() throws Exception
    {
        String fftType = "double";

        ResultSet rs = stmt.executeQuery("select value from experiment.impl_params where impl_id="+implId
                                         + " and literal LIKE '0' and name LIKE 'FFT'");       
        
        if( rs.next() )
          {
          String fftImplDB2 = rs.getString( 1 ).trim();
          
          int LastIndexOfDash = fftImplDB2.lastIndexOf("-");
          
          if( LastIndexOfDash < 0 )
            return "double";
              
          fftType = fftImplDB2.substring( LastIndexOfDash+1, fftImplDB2.length() );
          }        
        
        return fftType;
    }

    public static String getFFTProcessorMeshSize() throws Exception 
    {
        ResultSet rs = stmt.executeQuery("select value from experiment.impl_params where impl_id="+implId
                                         + " and literal LIKE '0' and name LIKE 'FFT'");       
        
        String procMesh = "";
        if(rs.next()) {
            String fftImplDB2 = rs.getString( 1 );
            if( fftImplDB2.startsWith("fftw") ) 
            {
                String string1 = fftImplDB2.trim();

                if( string1.equals("fftw") || string1.equals("fftw_mpi") )
                    procMesh = "";
                else
                {
                    String procsString = fftImplDB2.substring(fftImplDB2.indexOf(":")+1, fftImplDB2.length());
                    System.out.println(procsString);
                    
                    String procsX = procsString.substring( 0, procsString.indexOf("_"));
                    String procsY = procsString.substring( procsString.indexOf("_")+1, procsString.lastIndexOf("_"));

                    String procsZ = null;
                    if( procsString.lastIndexOf("-") < 0 )                
                        procsZ = procsString.substring( procsString.lastIndexOf("_")+1, procsString.length() );
                    else
                        procsZ = procsString.substring( procsString.lastIndexOf("_")+1, procsString.lastIndexOf("-") );
                    
                    procMesh = "enum { P_X = "+procsX+", P_Y = "+procsY+", P_Z = "+procsZ+" };";                    
                    procMesh += "\n #define MSD_DEFINED_PROC_MESH 1 ";
                }                                
            }
            else if( fftImplDB2.startsWith("spi_based") || fftImplDB2.startsWith("mpi_based")) {
                
                String procsString = fftImplDB2.substring(fftImplDB2.indexOf(":")+1, fftImplDB2.length());
                System.out.println(procsString);

                String procsX = procsString.substring( 0, procsString.indexOf("_"));
                String procsY = procsString.substring( procsString.indexOf("_")+1, procsString.lastIndexOf("_"));
                
                String procsZ = null;
                if( procsString.lastIndexOf("-") < 0 )                
                    procsZ = procsString.substring( procsString.lastIndexOf("_")+1, procsString.length() );
                else
                    procsZ = procsString.substring( procsString.lastIndexOf("_")+1, procsString.lastIndexOf("-") );
                
                procMesh = "enum { P_X = "+procsX+", P_Y = "+procsY+", P_Z = "+procsZ+" };";
                procMesh += " \n #define MSD_DEFINED_PROC_MESH 1 ";
            }
            else {
                throw new Exception("ERROR:: unsupported fft implemetation: " + fftImplDB2);
            }
        }
        else
            throw new Exception("ERROR:: No FFT implementation detected");

        return procMesh;
    }

    public static String getNSQImpl() throws Exception 
    {
        ResultSet rs = stmt.executeQuery("select value from experiment.impl_params where impl_id="+implId
                                         + " and literal LIKE '0' and name LIKE 'NSQ'");               
        
        if(rs.next()) {
            String nsqImpl = rs.getString(1);
            return nsqImpl;
        }
        else {
            return "NSQ";
        }                    
    }

    public static String getFFT3DImpl() throws Exception 
    {
        ResultSet rs = stmt.executeQuery("select value from experiment.impl_params where impl_id="+implId
                                         + " and literal LIKE '0' and name LIKE 'FFT3D'");               
        
        if(rs.next()) {
            String nsqImpl = rs.getString(1);
            return nsqImpl;
        }
        else {
            return "sourcecode";
        }                    
    }

    public static String getProfilingImpl() throws Exception 
    {
        ResultSet rs = stmt.executeQuery("select value from experiment.impl_params where impl_id="+implId
                                         + " and literal LIKE '0' and name LIKE 'PROFILING'");               
        
        if(rs.next()) {
            String nsqImpl = rs.getString(1);
            return nsqImpl;
        }
        else {
            return "none";
        }                    
    }

    public static String getIFPImpl() throws Exception 
    {
        ResultSet rs = stmt.executeQuery("select value from experiment.impl_params where impl_id="+implId
                                         + " and literal LIKE '0' and name LIKE 'IFP'");               

        if(rs.next()) {
            String ifpImpl = rs.getString(1);
            return ifpImpl;
        }
        else {
            return "IFPSimple";
        }            
    }

    public static String getFFTImpl() throws Exception 
    {
        String fftImpl = "";

        ResultSet rs = stmt.executeQuery("select value from experiment.impl_params where impl_id="+implId
                                         + " and literal LIKE '0' and name LIKE 'FFT'");       
        
        if(rs.next()) {
            String fftImplDB2 = rs.getString(1);
            if( fftImplDB2.startsWith("fftw") ) {
                fftImpl = "FFTW2P3ME";
            }
            else if( fftImplDB2.startsWith("spi_based") || fftImplDB2.startsWith("mpi_based") ) {
                fftImpl = "BGL3DFFT";
            }
            else {
                throw new Exception("ERROR:: unsupported fft implemetation: " + fftImplDB2);
            }
        }
        else
            throw new Exception("ERROR:: No FFT implementation detected");

        return fftImpl;
    }

    public static String getFFTLibrary() throws Exception 
    {
        String fftLibrary = "";

        ResultSet rs = stmt.executeQuery("select value from experiment.impl_params where impl_id="+implId
                                         + " and literal LIKE '0' and name LIKE 'FFT'");       
        
        if(rs.next()) {
            String fftImplDB2 = rs.getString(1);
            if( fftImplDB2.startsWith("spi_based_essl") || fftImplDB2.startsWith("mpi_based_essl") ) {
                fftLibrary = "essl";
            }
            else {
              fftLibrary = "fftw" ;
            }
        }
        else
            throw new Exception("ERROR:: No FFT implementation detected");

        return fftLibrary;
    }

    public static String getParallelPhase() throws Exception
    {        
        //         ResultSet rs = stmt.executeQuery("select value from experiment.impl_params where impl_id="+implId
        //                                          + " and literal LIKE '0' and name LIKE ''");       
        
        ResultSet rs = stmt.executeQuery( "select name from experiment.impl_params where impl_id=" + implId
                                          + " and (name LIKE '%PHASE4%')" );
        if( rs.next() ) 
            return "phase4";

        return "phase1_5";
        
//         String phaseString = "";
//         if( rs.next() )
//           {
//               phaseString = rs.getString("name");
//               phaseString = phaseString.trim();              

//               // System.out.println("phaseString: " + phaseString);

//               if( phaseString.equals("-DPK_PHASE4") )
//                   return "phase4";
//               else if( phaseString.equals("-DPK_PHASE1_5") )
//                   return "phase1.5";
//               else 
//                   return "phase1.5";
//           }
//         else
//             return "phase1.5";
    }

    public static String getFFTInclude() throws Exception 
    {
        String fftInclude = "";

        ResultSet rs = stmt.executeQuery("select value from experiment.impl_params where impl_id="+implId
                                         + " and literal LIKE '0' and name LIKE 'FFT'");       
        
        if(rs.next()) {
            String fftImplDB2 = rs.getString(1);
//            if( fftImplDB2.startsWith("fftw") ) {
//                fftInclude = "BlueMatter/fftw2p3me";
//            }
////             else if( fftImplDB2.startsWith("packet_based")) {
////                 fftInclude = "fft3d/fft3d_sender_computes";
////             }
//            else if( fftImplDB2.startsWith("spi_based")) {
//                fftInclude = "spi/fft3D_MPI";
//            }
//            else if( fftImplDB2.startsWith("mpi_based")) {
//                fftInclude = "MPI/fft3D_MPI";
//            }
//            else {
//                throw new Exception("ERROR:: unsupported fft implemetation: " + fftImplDB2);
//            }
              fftInclude = "fft3D" ;
        }
        else
            throw new Exception("ERROR:: No FFT implementation detected");

        return fftInclude;
    }
}
