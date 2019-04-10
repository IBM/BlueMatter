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

import java.sql.*;
import java.io.*;
import java.util.*;

import com.ibm.bluematter.db2probspec.parser.*;
import com.ibm.bluematter.db2probspec.*;

public class CompileScriptGenerator {

    private Connection     con                         = null;
    private Statement      stmt = null;
    private Statement      stmt1 = null;
    
    private int            sectionSequence = 0 ;

    public CompileScriptGenerator( Connection c ) {
        con = c;
    }

    public String tokenizeByLine( String in )
    {
        String out = "";
        
        StringTokenizer st = new StringTokenizer( in );

        while( st.hasMoreTokens() )
        {
            if ( st.countTokens() == 1 ) 
                out += st.nextToken();
            else 
                out += st.nextToken() + " \\\n";
        }

        return out;
    }

    public String tokenizeByLineStrippingIPA( String in )
    {
        String out = "";
        
        StringTokenizer st = new StringTokenizer( in );

        while( st.hasMoreTokens() )
        {
        	String MyToken = st.nextToken() ;
        	if ( ! MyToken.startsWith("-qipa"))
        	{
        	  out += " \\\n" + MyToken ;
        	}
        }

        return out;
    }

    	  // Construct the compile of one section
    public void ConstructSectionCompile(
    		FileWriter out,
    		String sectionName,
    		String sectionNameControl,
    		String optOpts,
    		String compilerOpts,
    		String macroDefines,
    		String projectIncludes,
    		String extInc,
    		String compiler
    		) throws Exception 
    {
	    String compilerOpts1 = compilerOpts + " " + optOpts + " -qarch=440 ";
	    String allOpts0 = macroDefines 
	                    + sectionNameControl 
	                    + " -DIFP_SEPARATE_EXECUTE -DORB_SEPARATE -DBONDED_SEPARATE_EXECUTE -DFFT_SEPARATE -DP3ME_SEPARATE " 
	                    + compilerOpts1 
	                    + " -I. " 
	                    + projectIncludes 
	                    + "  -I$SHIPDATA_INCLUDE " 
	                    + extInc 
	                    + "-DMSD_COMPILE_CODE_ONLY ../$1.msd ";
	
	    String firstCompLine0 = " " + allOpts0 + " -qpriority=" + (sectionSequence+2) + " -c -o ../$1." + sectionName + ".o";
	    String finalCmd0      = "( time " + compiler + " " + tokenizeByLine( firstCompLine0 ) + " >" + sectionName + ".log 2>&1;cat " + sectionName + ".log ) &";          
	    out.write( "cd $PWD && mkdir -p " + sectionName + " && cd " + sectionName + " && " + finalCmd0 + "\n" );
        sectionSequence += 1 ;
    }
    
    
    public void dumpScript(int implId, int pltId, FileWriter out ) throws Exception {

        stmt  = con.createStatement();
        stmt1  = con.createStatement();
        
        ResultSet rs = null;
        ResultSet rs1 = null;

        boolean blnieBuild = ( getParamFromDB2Impl( implId, "blnie" ) != null );
        boolean bglBuild = ( getParamFromDB2Impl( implId, "bgl" ) != null );
        boolean bgpBuild = ( getParamFromDB2Impl( implId, "bgp" ) != null );
        boolean bladeBuild = ( getParamFromDB2Platform( pltId, "blade" ) != null );
        boolean spiBuild = ( getParamFromDB2Platform( pltId, "spi" ) != null );
        boolean blrtsBuild = ( getParamFromDB2Platform( pltId, "blrts" ) != null );
        boolean PkBuild = ( getParamFromDB2Platform( pltId, "Pk" ) != null );
        
        String fft3DBuild = getParamFromDB2Impl( implId, "FFT3D" ) ;
        
  
        out.write("\n\n\n/** BEGIN_COMPILE_SCRIPT\n\n");
        out.write("#! /bin/ksh\n\n");
        
        // Set up the proper paths
        out.write("rm -rf *.o\n\n");
        out.write("rm -rf *.a\n\n");
        
        out.write("# pick out install base\n");
        out.write("SCRIPTED_INSTALL_BASE=\"$(installbase.py)\"\n") ;
        out.write("if [[ -n \"${SCRIPTED_INSTALL_BASE}\" ]]\n") ;
        out.write("then\n") ;
        out.write("  BG_INSTALL_BASE=\"${SCRIPTED_INSTALL_BASE%/}\"\n") ;
        out.write("fi\n") ;
        out.write("export SHIPDATA_DIR=$BG_INSTALL_BASE/usr/opt/bluegene\n");
        out.write("export SHIPDATA_INCLUDE=$SHIPDATA_DIR/include\n");
        out.write("export SHIPDATA_LIB=$SHIPDATA_DIR/lib\n");
        out.write("export SHIPDATA_TGTLIB=$SHIPDATA_DIR/tgtlib\n");
        out.write("export SHIPDATA_SRC=$SHIPDATA_DIR/src\n");
        out.write("export SHIPDATA_BIN=$SHIPDATA_DIR/bin\n\n\n");
        
        if( bladeBuild ) 
            {            
                out.write("export HW_INC_TOP=$BG_INSTALL_BASE/../../../../\n");
                out.write("export BLADE_INC=$HW_INC_TOP/blade/include\n\n\n");
            }
        else if ( blrtsBuild ) 
            {
    	        out.write(bgpBuild ? "export BGP_INSTALL=/bgsys/drivers/ppcfloor/\n" : "export BGL_INSTALL=${BL_INSTALL:=/bgl/BlueLight/ppcfloor}\n") ;
                if( spiBuild )
                  {
                  out.write( "LIB_BLADE=${SHIPDATA_SRC}/hoodcomms/libblade/\n\n" );
                  }
            }

        String compiler = getParamFromDB2Platform( pltId, "Compiler" );
        String compilerMachine = getParamFromDB2Platform( pltId, "CompilerHost" );       

        if( compilerMachine == null )
          {              
              compilerMachine = "localhost";
          }

        if(compiler == null)
            compiler = "xlC_r";
        
        if( compiler.equals( "crosscompiler" ) ) 
            throw new Exception("ERROR:: crosscompiler keyword is depricated.");

        out.write("# First compiler pass ( compile the DATA ONLY )\n");
        
        boolean crossComp = ( bladeBuild || blrtsBuild )  ;
        boolean dumpLst = ( getParamFromDB2Platform( pltId, "dump_lst" ) != null) ;

        boolean dvsBuiltIn = false;

// Include paths for non-BlueMatter libraries (e.g. GNU GSL) can be specified here
        String projectIncludes = "" ;

        String firstPassCommon = "-I. " + projectIncludes + " ";

        if( dumpLst )
            firstPassCommon += "-qsource -qlist -qshowinc \\\n";

        firstPassCommon += "-I$SHIPDATA_INCLUDE \\\n";
        firstPassCommon += "-qpriority=1 \\\n";
        firstPassCommon += "-DMSD_COMPILE_DATA_ONLY \\\n";

	// add macro to set EXECUTABLEID from environment variable
	firstPassCommon += "-DEXECUTABLEID=${executable_id:=-1} \\\n";

        firstPassCommon += "-+ \\\n";
        firstPassCommon += "../$1.msd \\\n";
        
        // String compilerMachine = "lipid";
        
        String commandPrefix = "" ; 
        String commandSuffix = "" ;
        
        if ( crossComp && (compilerMachine != "localhost") )
        {
        	commandPrefix = "PATH=/usr/bin:/bin ssh " + compilerMachine + " \" exec 2>&1;" ;
        	commandSuffix = "\" </dev/null" ;
        }
        
        out.write(commandPrefix) ;
        
        if( crossComp ) {
            out.write( "cd $PWD && mkdir -p data && cd data && " + compiler + " -qarch=440 ");
            out.write( firstPassCommon );
            out.write(" -DPK_BLADE -DPK_BGL \\\n");
            out.write( "-qsuppress=1540-1090:1540-1087:1540-1089:1540-1088 \\\n");
            out.write( "-qnoeh -qnounwind \\\n");
            out.write("-c -o ../$1.data.o  &\n");
        }
        else {
            out.write("cd $PWD && mkdir -p data && cd data && " + compiler + " ");
            out.write( firstPassCommon );
            out.write("-c -o ../$1.data.o  &\n");            
        }


        out.write("# Second compiler pass ( compile the CODE ONLY )\n");
        
        // 1. Get out the macros from the impl table
        rs = stmt.executeQuery("select name, value from experiment.impl_params where impl_id="+implId
                               + " and literal LIKE '1'");
        
        String macroDefines = "";       
        while( rs.next() ) {
            String value = rs.getString("value");                                    
            macroDefines += " "+value+"  ";            
            
            value = value.trim();
            if( value.equals("-DDVS_BUILT_IN") )
                dvsBuiltIn = true;
        }

        rs = stmt.executeQuery("select name, value from experiment.platform_params where platform_id="+pltId );
        
        while(rs.next()) {
            String name = rs.getString( 1 );
            if( name.startsWith("-D"))
                macroDefines += " " + name + " ";
        }
        
        String timestepsPerRdg = getParamFromDB2Impl(implId, "TIMESTEPS_PER_RDG");
        if(timestepsPerRdg != null)
            macroDefines += " -DTIMESTEPS_PER_RDG="+timestepsPerRdg +" ";
        
        // 2. Get out the symbolic specs from the platform table 

        boolean doMPI = (getParamFromDB2Platform(pltId,"mpi") != null);
        boolean doProfiled = (getParamFromDB2Platform(pltId,"profiled") != null);

        // 2. Get out the symbolic specs from the impl table (these effect the libraries which we compile in)

        Vector sourceNames = new Vector();
        Vector sourceNames_440d = new Vector();
        Vector sourceNames_gcc = new Vector();
        
// 3 files don't build with xlC v9 for BlueGene at the moment. Use gcc for parts on bgl; xlC for all on bgp
        boolean forceXLC = bgpBuild ;
        
        sourceNames.add(new String("pk/fxlogger.cpp"));
        sourceNames.add(new String("pk/platform.cpp"));
        sourceNames.add(new String("pk/pktrace.cpp"));
        sourceNames.add(new String("pk/pksincos.cpp"));       
        sourceNames.add(new String("BlueMatter/MRG32k3aEngine.cpp"));       
        
        if( bladeBuild ) 
        {
            sourceNames_440d.add( new String("pk/treebroadcast.cpp") );
            sourceNames_440d.add( new String("pk/treereduce.cpp") );
            sourceNames_440d.add( new String("pk/treereduce_int.cpp") );
        }

	if( spiBuild )
	    {
		(forceXLC ? sourceNames_440d : sourceNames_gcc ).add( new String("pk/a2a/packet_alltoallv.cpp") ) ;
		sourceNames_440d.add( new String("../include/spi/tree_reduce.cpp") ) ;
		(forceXLC ? sourceNames_440d : sourceNames_gcc ).add( new String("../include/spi/BGL_spi_wrapper.cpp") ) ;
		(forceXLC ? sourceNames : sourceNames_gcc ).add(new String("pk/PktThrobber.cpp"));
	    }
	
	
        if( !doMPI ) 
            sourceNames.add(new String("pk/coprocessor_on_coprocessor.cpp"));

        sourceNames.add(new String("BlueMatter/orbnode.cpp"));
        
        String libraries = "";      
        if( blrtsBuild )
          {
            libraries += ( bgpBuild ) ? " -L$BGP_INSTALL/arch/lib " : " -L$BGL_INSTALL/bglsys/lib ";
          }

        libraries += " -L$SHIPDATA_LIB ";             

        boolean ocoFFT = ( fft3DBuild == null || ! fft3DBuild.equals("sourcecode")) ;
        if( ocoFFT )
          {
        	macroDefines += " -DOCO_SRC ";
//        	libraries += " -L$SHIPDATA_TGTLIB ";        		
//        	if(doMPI)
//        	{
//            	libraries += " -locofft3dmpi.rts ";        		
//        	}
//        	else
//        	{
//        	    libraries += " -locofft3d.rts ";
//        	}
          }

	// add macro to set EXECUTABLEID from environment variable
	macroDefines += " -DEXECUTABLEID=${executable_id:=-1} ";
        
        if( spiBuild ) 
          {
            libraries += " -L$SHIPDATA_SRC/pk/bonb -lbonb.rts ";
	  }

        rs = stmt.executeQuery("select name, value from experiment.impl_params where impl_id="+implId
                               + " and literal LIKE '0'");       
        
        boolean doP3ME = RunTimeParameters.getLongRangeMethod().equals("P3ME");
        
        boolean doESSL = true ;

        boolean fftSet = false;
        while(rs.next()) {
            String name = rs.getString("name");
            String value = rs.getString("value");

            if( name.equals("FFT") ) {                
                fftSet = true;
                
                if( blrtsBuild )
                {
                    String fftType    = ImplManager.getFFTType();
                    String fftLib     = ImplManager.getFFTLibrary() ;
                    if ( PkBuild )
                    {
                    	doESSL = false ;
                        if( fftType.equals( "float" ))
                        {
                 	     libraries += " -l sfftwPk.rts " ;
                        }
                        else
                        {
                          libraries += " -l fftwPk.rts " ;
                        }
                    } else {
                       if( fftType.equals( "float" ))
                       {
                    	   if ( fftLib.equals("essl"))
                    	   {
                    		  libraries += " -L /opt/ibmmath/lib -L /opt/ibmcmp/xlf/bg/11.1/bglib -L /opt/ibmcmp/xlsmp/bg/1.7/lib -l esslbg -lxlfmath -lxlf90 -lxl -lxlomp_ser " ;
                    		  macroDefines += " -DFFT_ESSL " ;
                    	   } else {
                	          libraries += " -L /bgl/local/fftw-2.1.5/lib  -l sfftw " ;
                    		  macroDefines += " -DFFT_FFTW " ;
                    	   }
                       }
                       else
                       {
                    	   if ( fftLib.equals("essl"))
                    	   {
                     		  libraries += " -L /opt/ibmmath/lib -L /opt/ibmcmp/xlf/bg/11.1/bglib -L /opt/ibmcmp/xlsmp/bg/1.7/lib -l esslbg -l esslbg -lxlfmath -lxlf90 -lxl -lxlomp_ser " ;
                    		  macroDefines += " -DFFT_ESSL " ;
                    	   } else {
                           	  doESSL = false ;
                              libraries += " -L /bgl/local/fftw-2.1.5/lib/ -l fftw " ;
                    		  macroDefines += " -DFFT_FFTW " ;
                    	   }
                       }
                    }
                }
                else if(value.startsWith("fftw")) 
                {                    
                  doESSL = false ;
                  libraries += " $SHIPDATA_LIB/libfftw.a  ";
                  libraries += " $SHIPDATA_LIB/librfftw.a  ";
                  
                  if(value.equals("fftw_threaded")) {
                      libraries += " $SHIPDATA_LIB/libfftw_threads.a  ";
                      libraries += " $SHIPDATA_LIB/librfftw_threads.a  ";
                  }
                  else if(value.equals("fftw_mpi")) {
                      if(!doMPI)
                          throw new Exception("ERROR:: fftw_mpi defined, but mpi is not set in the platformfile.");
                      
                      if( blrtsBuild )
                          throw new Exception("ERROR:: building for the hardware, fftw_mpi is not supported");
                      
                      libraries += " $SHIPDATA_LIB/libfftw_mpi.a ";
                      libraries += " $SHIPDATA_LIB/librfftw_mpi.a ";
                  }               
                }
                else if(value.startsWith("packet_based")) {
                    
                	doESSL = false ;
                    // Might be using that 1-D fft for now
                    libraries += " $SHIPDATA_LIB/libfftw.a ";                    
                }
                else if( value.startsWith("mpi_based") ) {
                	doESSL = false ;
                    if( !(bglBuild || bgpBuild) )
                        libraries += " $SHIPDATA_LIB/libfftw.a ";
                }
                else
                    throw new Exception("ERROR:: FFT defined in the impl file, but no corresponding fft value");
            }
        }
        
        if(!fftSet && doP3ME) {
            System.out.println("WARNING:: P3ME is specified in the ctp but no FFT implementation specified in the impl");
            System.out.println("Setting FFT to fftw");
        	doESSL = false ;
            libraries += " $SHIPDATA_LIB/libfftw.a ";
            libraries += " $SHIPDATA_LIB/librfftw.a ";            
            
            if( doMPI ) {
                libraries += " $SHIPDATA_LIB/libfftw_mpi.a ";
                libraries += " $SHIPDATA_LIB/librfftw_mpi.a ";                
            }
        }
        
        String ocoLibraries = 
        	ocoFFT ? ( " -L$SHIPDATA_TGTLIB " + (
        			   doESSL ? ( doMPI ? "-locofft3dmpiessl.rts " : "-locofft3dessl.rts" ) 
        					  : ( doMPI ? "-locofft3dmpi.rts "     : "-locofft3d.rts " )
        			   )
        			 )
        		   : "" ;
        
        // 3. Get out the compiler options from the platform table.        
        rs = stmt.executeQuery("select name from experiment.platform_params where platform_id="+pltId
                               + " and name=value");
        String compilerOpts = "";
        String linkerOpts = "";
        
        String optIfp="";
        String optOrb="";
        String optBonded="";
        String optFft="";
        String optP3me="";
        String optNsq="";
        String optRest="" ;
        
        while( rs.next() ) {            
            String name = rs.getString("name");
            if ( name.startsWith("-Ovariable")) 
               {
            	  optIfp="-O3 -qipa=nohot" ;
                  optOrb="-O3 -qipa=nohot" ;
                  optBonded="-O3 -qipa=nohot" ;
                  optFft="-O3 -qipa=nohot" ;
                  optP3me="-O3 -qipa=nohot" ;
                  optNsq="-O" ;
                  optRest="-O3" ;
               }
            else
               {
	            if(name.startsWith("-O") || name.startsWith("-q") || name.startsWith("-z") || name.startsWith("-g") || 
	               name.startsWith("-B") || name.startsWith("-t") || name.startsWith("-b") )
	                compilerOpts += " "+name+" ";            
	            else if(name.startsWith("-l")) {
	                libraries += " "+name+" ";
	            }       	
               }
        }
        
        if ( doProfiled )
           {
        	compilerOpts += " -qlinedebug " ;
        	linkerOpts += " -pg " ;
           }
        	
        if( dumpLst )
            compilerOpts += " -qsource  -qlist  -qshowinc ";
        
        compilerOpts += " -+ ";

        String finalOpts="";        
        
        String extInc = ""; 
        if( bladeBuild )
            extInc = " -I$BLADE_INC  ";
        else if( blrtsBuild )
        {        	
            extInc = bgpBuild ? " -I$BGP_INSTALL/arch/include " : " -I$BGL_INSTALL/bglsys/include ";
        }
        
        
        finalOpts += " -I. " + projectIncludes + " -I$SHIPDATA_INCLUDE  "+extInc+" -DMSD_COMPILE_CODE_ONLY ";

        String extensionName="";

        if ( blrtsBuild ) {
            extensionName = "rts";
        }
        else if( doMPI ) {
            extensionName = "mpi.aix.exe";
        }
        else {
            extensionName = "smp.aix.exe";
        }               

        if( dumpLst )
            out.write("mv -f $1.lst $1.data.lst\n");

        //String sourceFiles = "$1.msd ";
        String oFiles = "" ;
        String sourceFiles = "" ;
        for( int i=0; i < sourceNames.size(); i++) 
           {
        	  String sourceName=(String) sourceNames.elementAt(i) ;
        	  String baseName=sourceName.substring(sourceName.lastIndexOf("/")+1) ;
        	  String fileName=baseName.substring(0,baseName.lastIndexOf(".")) ;
        	  sourceFiles += " $SHIPDATA_SRC/" + sourceName ;
        	  oFiles += " " + fileName + ".o" ;
           }
        String sourceFiles_440d = "" ;
        for( int i=0; i < sourceNames_440d.size(); i++) 
           {
        	  String sourceName=(String) sourceNames_440d.elementAt(i) ;
        	  String baseName=sourceName.substring(sourceName.lastIndexOf("/")+1) ;
        	  String fileName=baseName.substring(0,baseName.lastIndexOf(".")) ;
        	  sourceFiles_440d += " $SHIPDATA_SRC/" + sourceName ;
        	  oFiles += " " + fileName + ".o" ;
           }
        String sourceFiles_gcc = "" ;
        for( int i=0; i < sourceNames_gcc.size(); i++) 
           {
        	  String sourceName=(String) sourceNames_gcc.elementAt(i) ;
        	  String baseName=sourceName.substring(sourceName.lastIndexOf("/")+1) ;
        	  String fileName=baseName.substring(0,baseName.lastIndexOf(".")) ;
        	  sourceFiles_gcc += " $SHIPDATA_SRC/" + sourceName ;
        	  oFiles += " " + fileName + ".o" ;
           }

        String compilerOpts1 = compilerOpts + " -qarch=440 ";
        if ( blrtsBuild ) 
          {
          
          String dvs_file ="";          
          if ( dvsBuiltIn )
             { 
                dvs_file = "$1.dvs.cpp";
             }
          
		  if( spiBuild )
		    {
		      String g_plus_plus_compiler = bgpBuild ? "/opt/ibmcmp/vacpp/bg/9.0/bin/gxlC": "$BGL_INSTALL/blrts-gnu/bin/powerpc-bgl-blrts-gnu-g++";
		      
		      StringTokenizer md = new StringTokenizer( macroDefines );
		
		      String g_macro_defines = "";
		
		      while( md.hasMoreTokens() )
		          {
		              String token = md.nextToken();
		
		              if( !token.trim().startsWith("-DPK_XLC") )
		                  {
		                      g_macro_defines += token + " ";
		                  }
		          }
				
		      // Going to build parts of it with xlC
		      String final_throbber_c_plus_plus_cmd = compiler + " \\\n"
		      + macroDefines + " \\\n"
		      + ( bgpBuild ? "" : "-qarch=440d \\\n" )                  
		      + "-O3 \\\n"                  
		      + "-c \\\n"
		      + "-I$SHIPDATA_INCLUDE \\\n"
		      + "-I$LIB_BLADE \\\n"
		      + "-I$LIB_BLADE/include \\\n"
		      + (bgpBuild ? "-I$BGP_INSTALL/arch/include \\\n": "-I$BGL_INSTALL/bglsys/include \\\n")
		      + sourceFiles_440d
		      + " &" ;
		
		      out.write( "cd $PWD && ( set -x ; " + final_throbber_c_plus_plus_cmd + "\n ) >bits.log 2>&1; cat bits.log\n" );              
				      
		      // And other parts with g++
		      
		      if( sourceNames_gcc.size() > 0 )
		        {
			      String final_throbber_g_plus_plus_cmd = g_plus_plus_compiler + " \\\n"
			          + tokenizeByLine( g_macro_defines ) + " \\\n"
			          + "-O3 \\\n"                  
			          + "-c \\\n"
			          + "-I$SHIPDATA_INCLUDE \\\n"
			          + "-I$LIB_BLADE/ \\\n"
			          + "-I$LIB_BLADE/include \\\n"
			          + (bgpBuild ? "-I$BGP_INSTALL/arch/include \\\n": "-I$BGL_INSTALL/bglsys/include \\\n")
			          + sourceFiles_gcc ;
			
			      out.write( "cd $PWD && " + final_throbber_g_plus_plus_cmd + "\n" );
		        }
		    }

                // Construct the IFP compile
            ConstructSectionCompile(out,"ifp","-DIFP_COMPILE_EXECUTE -DIFP_NO_MAIN -DLEAVE_SYMS_EXTERN",compilerOpts,optIfp,macroDefines,projectIncludes,extInc,compiler) ;
            
            // Construct the ORB compile
            ConstructSectionCompile(out,"orb","-DORB_COMPILE -DIFP_NO_MAIN -DLEAVE_SYMS_EXTERN",compilerOpts,optOrb,macroDefines,projectIncludes,extInc,compiler) ;

            // Construct the Bonded compile
            ConstructSectionCompile(out,"bonded","-DBONDED_COMPILE_EXECUTE -DIFP_NO_MAIN -DLEAVE_SYMS_EXTERN",compilerOpts,optBonded,macroDefines,projectIncludes,extInc,compiler) ;

            // Construct the FFT compile
            ConstructSectionCompile(out,"fft","-DFFT_COMPILE -DIFP_NO_MAIN -DLEAVE_SYMS_EXTERN",compilerOpts,optFft,macroDefines,projectIncludes,extInc,compiler) ;

            // Construct the P3ME compile
            ConstructSectionCompile(out,"p3me","-DP3ME_COMPILE -DIFP_NO_MAIN -DLEAVE_SYMS_EXTERN",compilerOpts,optP3me,macroDefines,projectIncludes,extInc,compiler) ;


            // Construct the NSQ-and-up compile
            ConstructSectionCompile(out,"nsq","",compilerOpts,optNsq,macroDefines,projectIncludes,extInc,compiler) ;

            if ( ! PkBuild )
            {
              // Construct the 'everything else compile'
              String allOpts1       = macroDefines+" " + compilerOpts1 + " " + optRest + " " + finalOpts + "  -DIFP_SEPARATE_EXECUTE -DMSD_COMPILE_CODE_ONLY -DORB_SEPARATE -DBONDED_SEPARATE_EXECUTE -DFFT_SEPARATE -DLEAVE_SYMS_EXTERN -c " + sourceFiles ; 
            
              String firstCompLine1 = " " + allOpts1 ;
              String finalCmd1      = compiler + " " + tokenizeByLine( firstCompLine1 ) ;          
              out.write( "cd $PWD && ( set -x; " + finalCmd1 + ") >rest.log 2>&1; cat rest.log\n") ;
            }
            out.write("wait\n") ;
            String arCmd = "rm -f librest.rts.a; ar crs librest.rts.a " + oFiles ;
            out.write("cd $PWD && " + arCmd + "\n") ;
              // Construct the 'link'
            String allOptsLink       = linkerOpts + macroDefines+" " + compilerOpts1 + " "+finalOpts + " $1.data.o $1.nsq.o $1.ifp.o $1.orb.o $1.bonded.o $1.fft.o $1.p3me.o librest.rts.a " + ocoLibraries + libraries ;
            
	    if( doMPI )
		{
		    allOptsLink += " -lmpich.rts -lmsglayer.rts" ;
		}

	    allOptsLink += " -ldevices.rts -lrts.rts";

            String firstCompLineLink = " " + allOptsLink + " -o $1." + extensionName + " 2>&1";
            String finalCmd = compiler + " " + tokenizeByLineStrippingIPA( firstCompLineLink );
            
            out.write( "cd $PWD && " + finalCmd + "\n" );
          }
        else 
          {                    
          String allOpts = macroDefines + compilerOpts + " " + finalOpts + " " + sourceFiles + " $1.msd $1.data.o " + ocoLibraries + libraries;
          String firstCompLine = " " + allOpts + " -o $1." + extensionName;            
          String finalCmd      = compiler + " " + tokenizeByLine( firstCompLine );
          out.write( "wait;cd $PWD && " + finalCmd + ";\n" );
          }
         
        out.write(commandSuffix) ;
       
        if( ( blnieBuild || bladeBuild ) && !crossComp ) {
            out.write("\n\nar -v -q $1.a *.o\n");
            out.write("\nrm -f *.o\n");
        }
        

        out.write("\n\nEND_COMPILE_SCRIPT **/\n\n\n");
    }
    
    private String getParamFromDB2Platform( int pltId, String paramName ) throws Exception {
        ResultSet rs = null;
        
        rs = stmt.executeQuery("select value from experiment.platform_params"
                               + " where platform_id="+pltId
                               + " and name LIKE '"+paramName+"'");
        if(!rs.next()) 
            return null;
        
        return rs.getString("value");
    }

    private String getParamFromDB2Impl( int implId, String paramName ) throws Exception {
        ResultSet rs = null;
        
        rs = stmt.executeQuery("select value from experiment.impl_params"
                               + " where impl_id=" + implId
                               + " and name LIKE '" + paramName+"'");
        if(!rs.next()) 
            return null;
        
        return rs.getString("value");
    }
}
