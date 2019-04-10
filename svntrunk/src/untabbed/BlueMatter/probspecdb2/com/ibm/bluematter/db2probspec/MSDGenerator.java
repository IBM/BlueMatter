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
 /*************************************************************************
 * Trouble spots for RIGID_PROTEIN
 * IsSiteInWater
 * IsProtein()
 * getNumberOfConstraints()
 ************************************************************************/

package com.ibm.bluematter.db2probspec;

import java.sql.*;
import java.io.*;
import java.util.*;

import com.ibm.bluematter.db2probspec.parser.*;

public class MSDGenerator {

    private Connection     con                         = null;
    private UDF_RegistryIF udfRegistry                 = null;
    private boolean        doCharge14                  = false;
    private boolean        doLJ14                      = false;
    private Vector         paramTupleArray             = new Vector();
    private Vector         siteTupleArray              = new Vector();
    private int            siteTupleIdArray[]          = null;
    private int            sizeTupleArray[]            = null;
    private int            siteIDSiteTypeMap[]         = null;
    private Vector         siteTypeList                = new Vector();
    private int            udfCodeArray[]              = null;


    private Vector         graphSepList                = null;

    private Vector         pair14List                  = new Vector();
    private int            pair14Count                 = 0;

    private Vector         excludedList                = new Vector();
    private int            excludedCount               = 0;

    private int            lj14LowerTriangularMatrix[]  = null;
    private int            ljNsqLowerTriangularMatrix[] = null;

    private int            udfTableSize                = 0;

    private Vector         nsqSiteTypeList             = new Vector();
    private int            nsqSiteIDSiteTypeMap[]      = null;

    private String         chargeNsqParamName          = null;
    private String         ljNsqParamName              = null;
    private String         ljStructName                = null;
    private String         stdChargeName                = null;

    private int            systemId                    = -1;
    private String         ffFamily                    = null;


    private long           timeS, timeF;

    private ConnectedPartition irrPart[]               = null;

    private Vector         mFragmentTypeList           = null;
    private int            mNumberOfSitePerFragmentType[] = null;

    private int            numNodes                    = -1;
    private int            partCount                   = -1;
    private int            irrPartCount                = -1;

    private int            stdHarmonicBondTupleMask[]   = null;
    private Hashtable      stdHarmonicBondExcludedSites = new Hashtable();
    //    private RunTimeParameters rtp                = null;

    private Statement      stmt = null;
    private Statement      stmt1 = null;
    private Statement      stmt2 = null;
    private Statement      stmt3 = null;

    private Vector          udfInvocationTable     = new Vector();

    private Vector           pair1234               = new Vector();

    private Hashtable        aminoAcidAtomTagIndex  = new Hashtable();
    private Hashtable        aminoAcidMaximumExtent = new Hashtable();


    // By default leave it at 3
    private int              numberOfConstraints = 3;

    private Vector           rigidSites[]  = new Vector[ 4 ];
    private Vector           rigidParams[] = new Vector[ 3 ];
    private Vector           crumbList = new Vector();

    private int[]            externalToInternalSiteIdMap;
    private int[]            internalToExternalSiteIdMap;

    private String CurrentWaterMethod = "";
    private int    OxygenPositionInWater = 0;

    // NOTE:: This array is indexed by external site id
    private char WaterProteinOtherForSiteId[] = null;

    private final int PROTEIN = 0;
    private final int WATER = 1;
    private final int OTHER = 2;
    private final int PROTEIN_H = 3;

    private final double DEG2RAD = Math.PI / 180.0;

    public MSDGenerator( Connection c, UDF_RegistryIF udfReg ) {
  con = c;
  udfRegistry = udfReg;
  //rtp = rtpIn;
    }

    private String EvaluateString( String aStr ) throws Exception
    {
        String lStr = aStr.trim();


        if( lStr.lastIndexOf("cos") != -1 )
          {
              // get everything between
              String value = lStr.substring( lStr.indexOf( "(" )+1,
                                             lStr.indexOf( "*" ) );

              double dVal = Double.valueOf( value ).doubleValue() * DEG2RAD;
              double cdval = Math.cos(  dVal );

              return Double.toString( cdval );

          }
        else if( lStr.lastIndexOf("sin") != -1 )
          {
              String value = lStr.substring( lStr.indexOf( "(" )+1,
                                             lStr.indexOf( "*" ) );

              double dVal = Double.valueOf( value ).doubleValue() * DEG2RAD;
              double sdval = Math.sin(  dVal );

              return Double.toString( sdval );
          }
        else if( lStr.lastIndexOf("Math::Deg2Rad" ) != -1 )
          {
              String value = lStr.substring( lStr.indexOf( "(" )+1,
                                             lStr.indexOf( "*" ) );

              double dVal = Double.valueOf( value ).doubleValue() * DEG2RAD;

              return Double.toString( dVal );
          }
        else
          {
              return aStr;
          }
    }

    private void dumpSymbolicConstants(FileWriter out, int systemId)  throws Exception {
  /*******************************************************************************
   *   Output all symbolic constants from the headerParamer as #defines
   ******************************************************************************/
  if(ProbspecgenDB2.verbose)
      System.out.println("Printing out symbolic constants...\n\n");
  
  ResultSet rs = stmt.executeQuery("SELECT param_id, param_value FROM " +
           "MDSYSTEM.Global_Parameter WHERE sys_id=" + systemId +" AND symbolic_constant=1 FOR READ ONLY WITH UR");
  
  while(rs.next()) {
      String paramIdent = rs.getString("param_id");
      String paramValue = rs.getString("param_value");
      out.write("#define " + paramIdent + "  " + paramValue + "\n");
  }
  
  out.write("\n\n");
  /*******************************************************************************/
    }


    private void dumpStructureByUDFName(FileWriter out, String udfName) throws Exception {
  /*******************************************************************************
   *  Getting out the ljStructure, once the non-bonded stuff are generic
   *  there will be no need for this here.
   ******************************************************************************/
  int     udfCode = -1;
  String  udfStruct    = null;
  String  udfStructName = null;
  
  udfCode = udfRegistry.GetUDFCodeByName(udfName);
  udfStructName = udfRegistry.GetStructureNameByUDFCode(udfCode);
  udfStruct = udfRegistry.GetStructureStringByUDFCode(udfCode);

  out.write( udfStruct + "\n");
  /*******************************************************************************/
    }

    private void dumpAllParticipatingStructures(FileWriter out) throws Exception {
  /*******************************************************************************
   *  Printout all the structures participating pertinent to the specified system
   ******************************************************************************/
  if(ProbspecgenDB2.verbose)
      System.out.println( "Printing out the udf structures...\n\n" );
  
  int                         udfCode;
  String                      structString  =  null;
  String                      udfName       =  null;
  // This invocationRecord is used through out the file
  
  ResultSet invRecord     =  stmt.executeQuery("SELECT * FROM "
                 + "MDSYSTEM.UDF_Invocation_Table WHERE sys_id="+ systemId + " order by site_tuple_list_id FOR READ ONLY WITH UR");
  
  int paramListId = -1;
  while(invRecord.next()) {
  
      paramListId = invRecord.getInt("param_tuple_list_id");
  
      if( paramListId == -1 )
    continue;
  
      String udfCodeString = invRecord.getString("udf_id");
      udfCode = udfRegistry.GetUDFCodeByName( udfCodeString );
      structString = udfRegistry.GetStructureStringByUDFCode(udfCode);
      out.write( structString + "\n");
  }
  /******************************************************************************/
    }

    private void dumpMSDClass(FileWriter out) throws Exception {
  /*******************************************************************************
   *  Printout the fields of MSD
   ******************************************************************************/
  if(ProbspecgenDB2.verbose)
      System.out.println( "Printing out the MSD class... \n\n" );
  
  ResultSet rs, rs1, rs2, rs3;
    
  out.write( "class MSD\n");
  out.write( "{\n" );
  out.write( "public:\n\n" );

  out.write("enum { NSQ0 = 0, NSQ1 = 1, NSQ2 = 2};\n");
  out.write("enum { Exclusion = 0, Inclusion = 1};\n");
  out.write("enum { WATER = 0, AMINOACID = 1, OTHER=2};\n");
  out.write("enum { NORM = 0, COMB = 1};\n");

  // out.write(rtp.getRTPEnumAsString());

  out.write("\n");
  RunTimeParameters.dumpRTPParamsDeclaration(out);
  out.write("\n");
  

  out.write( "static const double TimeConversionFactorMsd;\n" );
  out.write( "static UdfInvocation UdfInvocationTable[];\n" );
  out.write( "static const int UdfInvocationTableSize;\n\n" );

  out.write( "static const int magicNumber;\n\n" );
  out.write( "static const int sourceId;\n\n" );
  
  /*****************************************************************/
  out.write( "static const SiteInfo SiteInformation[];\n" );
  out.write( "static const int SiteInformationSize;\n\n" );
  
  out.write( "static const int SiteIDtoSiteTypeMap[];\n" );
  out.write( "static const int SiteIDtoSiteTypeMapSize;\n\n" );
  
  int nsqLJ_code = udfRegistry.GetUDFCodeByName("NSQLennardJones");
        int nsqCharge_code = udfRegistry.GetUDFCodeByName("NSQCoulomb");
  
  String udfBindingPrefix = new String("UDF_Binding::");
  
  chargeNsqParamName = udfBindingPrefix + udfRegistry.GetStructureNameByUDFCode(nsqCharge_code);
  ljNsqParamName = udfBindingPrefix + udfRegistry.GetStructureNameByUDFCode(nsqLJ_code);
  
  out.write( "static const " + chargeNsqParamName + " ChargeNsqParams[];\n");
  out.write( "static const " + ljNsqParamName + " LJNsqParams[];\n");
  out.write( "static const int NumberOfNsqSiteTypes;\n\n" );
  
  out.write("static const int NsqSiteIDtoSiteTypeMap[];\n");
  out.write("static const int NsqSiteIDtoSiteTypeMapSize;\n\n");	
  
  out.write( "static const SiteId ExclusionList1_2_3_4[];\n" );
  out.write( "static const int ExclusionList1_2_3_4Size;\n\n" );
  
  out.write("static const NsqUdfInvocation NsqInvocationTable[];\n");
  out.write("static const int NsqInvocationTableSize;\n\n");

  int ljCode = udfRegistry.GetUDFCodeByName("LennardJonesForce");
  ljStructName = udfBindingPrefix + udfRegistry.GetStructureNameByUDFCode(ljCode);

  int chargeCode = udfRegistry.GetUDFCodeByName("StdChargeForce");
  stdChargeName = udfBindingPrefix + udfRegistry.GetStructureNameByUDFCode(chargeCode);
  
  if (!ProbspecgenDB2.no14) {
      out.write( "static const " + ljStructName + " LJ14PairTable[];\n" );
      out.write( "static const int LJ14PairTableSize;\n\n" );
  
      int charge14Code = udfRegistry.GetUDFCodeByName("Coulomb14");
      String charge14Name = udfBindingPrefix + udfRegistry.GetStructureNameByUDFCode(charge14Code);
        
      out.write( "static const " + charge14Name + " Charge14PairTable[];\n" );
      out.write( "static const int Charge14PairTableSize;\n\n" );
  
      out.write( "static const SiteId Pair14SiteList[];\n" );
      out.write( "static const int Pair14SiteListSize;\n\n" );
  }
  
  out.write( "static const IrreduciblePartitionType IrreduciblePartitionTypeList[];\n" );
  out.write( "static const int IrreduciblePartitionTypeListSize;\n\n" );
  
  out.write( "static const int IrreduciblePartitionCount;\n\n" );

        out.write( "static const IrreduciblePartitionMap IrreduciblePartitionIndexToTypeMap[];\n");
        out.write( "static const int IrreduciblePartitionIndexToTypeMapSize;\n\n" );

        out.write( "static const SiteId ExternalToInternalSiteIdMap[];\n");
        out.write( "static const int ExternalToInternalSiteIdMapSize;\n\n");

        out.write( "static const SiteId InternalToExternalSiteIdMap[];\n");
        out.write( "static const int InternalToExternalSiteIdMapSize;\n\n");

        out.write( "inline static int GetNumberOfSimTicks();\n\n" );
        out.write( "static const int NumberOfEnergyReportingUDFs;\n\n" );

  if(RunTimeParameters.getWaterModel() != null && waterSitesPresent() ) {
      out.write( "static const SiteId Water3sites[];\n\n" );
  }
  
        if( RunTimeParameters.doShakeRattleProtein() ||
           ( ProbspecgenDB2.fragment_mode == ProbspecgenDB2.RIGID_GROUP_FRAGMENTATION ) ) {

            out.write( "static const SiteId Rigid2Sites[];\n" );
      out.write( "static const SiteId Rigid3Sites[];\n" );
      out.write( "static const SiteId Rigid4Sites[];\n\n" );

            out.write( "static const UDF_Binding::ShakeGroup2_Params Rigid2Params[];\n" );
      out.write( "static const UDF_Binding::ShakeGroup3_Params Rigid3Params[];\n" );
      out.write( "static const UDF_Binding::ShakeGroup4_Params Rigid4Params[];\n\n" );
        }

  /******************************************************************************/
  
  
  /*******************************************************************************
   *  Printout the field names of the parameter arrays
   ******************************************************************************/
  
  rs = stmt.executeQuery("SELECT COUNT(*) FROM MDSYSTEM.UDF_Invocation_Table WHERE sys_id=" + systemId);
  rs.next();
  udfTableSize = rs.getInt(1);
  
  int udfCode;
  int i=0;
  if ( udfTableSize != 0) {
  
      int paramTupleListId;
      int siteTupleListId;
      Vector siteTupleListIds = new Vector();
  
      rs = stmt.executeQuery("SELECT * FROM "
           + "MDSYSTEM.UDF_Invocation_Table WHERE sys_id=" + systemId + " order by site_tuple_list_id FOR READ ONLY WITH UR");
      String siteTupleDesc  = null;
      String paramTupleDesc = null;
  
      while (rs.next()) {
    int tmp = (new Integer(rs.getInt("site_tuple_list_id"))).intValue();
    siteTupleListIds.add(new Integer(rs.getInt("site_tuple_list_id")));
      }
  
      rs = stmt.executeQuery("SELECT * FROM "
           + "MDSYSTEM.UDF_Invocation_Table WHERE sys_id=" + systemId + " order by site_tuple_list_id FOR READ ONLY WITH UR");
      boolean printSites;
      String structName = null;
  
  
      i = 0;
      while (rs.next()) {
    String udfCodeString = rs.getString("udf_id");
    udfCode = udfRegistry.GetUDFCodeByName( udfCodeString );
    siteTupleListId = rs.getInt("site_tuple_list_id");
    paramTupleListId = rs.getInt("param_tuple_list_id");
    
    printSites = true;
    
                // This check is here so that we can use the same site tuple list for several udfs.
                //
    for (int zz = 0 ; zz < i; zz++) {
        int tempInt = ((Integer) (siteTupleListIds.get(zz))).intValue();
        if( tempInt == siteTupleListId)
      printSites = false;
    }
    
    rs1 = stmt1.executeQuery("SELECT site_tuple_desc FROM "
           + "MDSYSTEM.SiteTupleList WHERE sys_id=" + systemId + " AND site_tuple_list_id="
           + siteTupleListId + " FOR READ ONLY WITH UR" );
    while(rs1.next()) {
        siteTupleDesc = rs1.getString("site_tuple_desc");
    }
    
    if(siteTupleDesc == null)
        throw new Exception("No description was found for site_tuple_list_id: " + siteTupleListId);
    
                if(ProbspecgenDB2.debug)
                    System.out.println("printSites: " + printSites );

    if (printSites)
        out.write("static const SiteId " + siteTupleDesc+ "SiteTupleList[];\n");
    
    
    if (paramTupleListId == -1) {
        i++;
        continue;
    }
    
    rs1 = stmt1.executeQuery("SELECT param_tuple_desc FROM "
           + "MDSYSTEM.ParamTupleList WHERE sys_id=" + systemId + " AND param_tuple_list_id="
           + paramTupleListId + " FOR READ ONLY WITH UR" );
    rs1.next();
    paramTupleDesc = rs1.getString("param_tuple_desc");
    
    if(paramTupleDesc == null)
        throw new Exception("No description was found for param_tuple_list_id: " + paramTupleListId);
    
    structName = "UDF_Binding::" + udfRegistry.GetStructureNameByUDFCode(udfCode);
    out.write("static const " + structName + " " + paramTupleDesc + "ParamTupleList[];\n\n");
    i++;
      }
        }

        // Closing the MSD class
        out.write( "};\n\n");
        /*****************************************************************************/
    }


    private void createIrreduciblePartitionTable(BondListAsGraph blGraph) throws Exception {
  /******************************************************************************
   *             Create the irreducible partition table
   *****************************************************************************/
  timeS = System.currentTimeMillis();
  ConnectedSubgraphs partition = new ConnectedSubgraphs(blGraph);
  timeF = System.currentTimeMillis();
  
  if(ProbspecgenDB2.debug)
      System.out.println("Time to make the ConnectedSeparation: " + (timeF-timeS));
  
  numNodes       = partition.nodeCount();
  irrPartCount   = -1;
  irrPart = new ConnectedPartition[ numNodes ];
  
  int                         partitionCount = partition.partitionCount();
  int                         curRunCount    = 0;

  partCount      = 0;
  
  Vector iph = new Vector();
  ConnectedPartition curIPH = null;
  ConnectedPartition prevIPH = new ConnectedPartition();
  
  int i=0;
  while( i < numNodes ) {
      iph.add(new ConnectedPartition( partition.subgraphLabel( i ), i ));
      i++;
  }
  
  /******************************************************************************
   *           Sort the iph list first by partitionId then by siteId
   *****************************************************************************/
  Collections.sort(iph);
  
  i=0;
  while ( i < numNodes ) {
      curIPH = (ConnectedPartition) iph.elementAt(i);;

            // Check if the sites are in the same connected partition
            if ( curIPH.getPartitionId() == prevIPH.getPartitionId() ) {

    if( (curIPH.getFirstSiteId() - prevIPH.getFirstSiteId()) == 1 ) {
        curRunCount++;
        irrPart[irrPartCount].setNumberOfSites(curRunCount);
    } else {
        irrPartCount++;
        irrPart[irrPartCount] = new ConnectedPartition(curIPH.getPartitionId(),curIPH.getFirstSiteId(), 1);
        curRunCount = 1;
    }
      } else {
    partCount++;
    irrPartCount++;
                irrPart[irrPartCount] = new ConnectedPartition(curIPH.getPartitionId(),curIPH.getFirstSiteId(), 1);
    curRunCount = 1;
      }
      prevIPH = curIPH;
      i++;
  }
    }

    private void dumpIrreduciblePartition(FileWriter out, BondListAsGraph blGraph) throws Exception {

  int i=0;
  
  if(irrPart == null)
      throw new Exception("ProbspecGenerator::dumpIrreduciblePartition::irrPart == null");
  
        int numberOfSitesInSystem = getNumberOfSitesInSystem();
        //        if( ProbspecgenDB2.debug )
        System.out.println("numberOfSitesInSystem: " + numberOfSitesInSystem );

        externalToInternalSiteIdMap = new int[ numberOfSitesInSystem ];
        if(externalToInternalSiteIdMap==null) {
            throw new Exception("ERROR: Not enough memory to allocate externalToInternalSiteIdMap");
        }

        // The default case is one to one
        for( int s=0; s < numberOfSitesInSystem; s++ ) {
            externalToInternalSiteIdMap[ s ]= s;
        }

  switch(ProbspecgenDB2.fragment_mode) {
  case ProbspecgenDB2.ALL_SITES_IN_ONE_FRAGMENT:
  case ProbspecgenDB2.ALL_MOLECULES_IN_ONE_FRAGMENT:
      {
                throw new Exception("Fragmentation scheme all sites in one fragment are no longer supported");

// 		/******************************************************************************
// 		 *           Printout the Irreducible partition
// 		 *****************************************************************************/
// 		if(ProbspecgenDB2.verbose)
// 		    System.out.println ( "Printing out the Irreducible Partition List... All sites/molecules in ONE fragment \n\n");
    
// 		out.write( "// All sites/molecules in one fragment\n\n" );
// 		out.write( "const IrreduciblePartitionType MSD::IrreduciblePartitionTypeList[] = \n");
// 		out.write( "{\n" );

//                 //                String centerOfMassMass = getCenterOfMass(0, numNodes);

// 		int numConst = getNumberOfConstraints( 0, numNodes );

// 		out.write( "{ " + numNodes + " , MSD::NON_WATER, 0, " + numConst + ", 99999999.0 }\n");
// 		out.write("};\n\n");
    
// 		out.write( "const int MSD::IrreduciblePartitionTypeListSize = 1;\n\n" );
// 		out.write( "const int MSD::IrreduciblePartitionCount = 1;\n\n" );
//                 ProbspecgenDB2.globalDefines.put("NUMBER_OF_FRAGMENTS", new String( 1 + ""));
// 		/*****************************************************************************/

// 		out.write( "// {start_site , ip_type} \n");
// 		out.write( "const IrreduciblePartitionMap MSD::IrreduciblePartitionIndexToTypeMap[] = \n");
// 		out.write( "{\n" );
//                 out.write( "{ 0, 0, 0, 0}" );
// 		out.write("};\n\n");

// 		out.write( "const int MSD::IrreduciblePartitionIndexToTypeMapSize = 1;\n\n" );

// 		break;
      }
  case ProbspecgenDB2.ALL_SITES_IN_THEIR_OWN_FRAGMENT:
      {
    /******************************************************************************
     *           Printout the Irreducible partition
     *****************************************************************************/
    if(ProbspecgenDB2.verbose)
        System.out.println( "Printing out the Irreducible Partition List... All sites in their own fragment \n\n");
    
    out.write( "// All sites in thier own fragment\n");
                out.write( "// Center of Mass is 0.0 since pressure control will not be used with this fragmentation scheme\n");
    out.write( "const IrreduciblePartitionType MSD::IrreduciblePartitionTypeList[] = \n");
    out.write( "{\n" );
                out.write( "{ 1, MSD::NON_WATER, 0, 0, 0.0 }");
    out.write( "};\n\n");


    out.write( "const int MSD::IrreduciblePartitionTypeListSize = 1;\n\n");
    
    i=0;


    int startSiteId = 0;
    out.write( "// {start_site , ip_type} \n");
    out.write( "const IrreduciblePartitionMap MSD::IrreduciblePartitionIndexToTypeMap[] = \n");
    out.write( "{\n" );

    while (i <= irrPartCount) {
        startSiteId =  irrPart[ i ].getFirstSiteId();		
                        out.write( "{ " + startSiteId + ", 0, 0, 0 }" );

        if (i != irrPartCount )
      out.write( ",\n");
    
        i++;
    
    }	

    out.write("};\n\n");
                out.write( "const int MSD::IrreduciblePartitionCount = " + irrPartCount + ";\n\n" );
                ProbspecgenDB2.globalDefines.put("NUMBER_OF_FRAGMENTS", new String( irrPartCount + ""));
    out.write( "const int MSD::IrreduciblePartitionIndexToTypeMapSize = 1;\n\n" );

    /*****************************************************************************/
    break;
      }
  case ProbspecgenDB2.ALL_MOLECULES_IN_THEIR_OWN_FRAGMENT: {
            throw new Exception("Fragmentation scheme all molecules in their fragment is no longer supported");

//             /******************************************************************************
//              *           Printout the Irreducible partition
//              *****************************************************************************/

// 		if(ProbspecgenDB2.verbose)
// 		    System.out.println( "Printing out the Irreducible Partition List... "
// 					+"All molecules in their own fragment (default setting)\n\n");
    
// 		out.write( "// All molecules in their own fragment\n" );
// 		out.write( "const IrreduciblePartitionType MSD::IrreduciblePartitionTypeList[] = \n");
// 		out.write("{\n");
    
//                 boolean firstEntry = true;
//                 int oxygenPosition = -1;
// 		i=0;
//                 boolean isWater = false;


//                 IntegerPairRep[] fragmentIndexToTypeMap = new IntegerPairRep[ irrPartCount+1 ];

//                 ConnectedPartition current;
//                 ConnectedPartition prev=null;
//                 int fragmentType = -1;

//                 int MAXFRAGMENTS = 4096;


//                 String[] fragmentTypes = new String[ MAXFRAGMENTS ];

// 		while (i <= irrPartCount) {
//                     current = irrPart[ i ];

//                     if(!current.equalTo(prev)) {
//                         fragmentType++;

//                         int firstSiteInFragment    = current.getFirstSiteId();
//                         int numberOfSitesInFragment = current.getNumberOfSites();

//                         fragmentIndexToTypeMap[ i ] = new IntegerPairRep( firstSiteInFragment, fragmentType );

//                         fragmentTypes[ fragmentType ] = getFragmentString(firstSiteInFragment, numberOfSitesInFragment, "NONE");
//                     }
//                     else {
//                         fragmentIndexToTypeMap[ i ] = new IntegerPairRep( current.getFirstSiteId(), fragmentType );
//                     }

//                     prev = current;
// 		    i++;
// 		}

//                 for( int j=0; j < fragmentType+1; j++) {
//                     out.write(fragmentTypes[ j ]);
//                     if(j != fragmentType )
//                         out.write(",");
//                     out.write("\n");
//                 }
    
// 		out.write( "};\n\n");
    
// 		// irrPartCount++;
// 		out.write( "const int MSD::IrreduciblePartitionTypeListSize = " + (fragmentType+1)  + ";\n\n" );
// 		out.write( "const int MSD::IrreduciblePartitionCount = " + (irrPartCount+1) + ";\n\n");
//                 ProbspecgenDB2.globalDefines.put("NUMBER_OF_FRAGMENTS", new String( (irrPartCount+1) + "" ));
// 		/*****************************************************************************/


// 		out.write( "// {start_site , ip_type, molecule_id, fragments_index_in_molecule} \n");
// 		out.write( "const IrreduciblePartitionMap MSD::IrreduciblePartitionIndexToTypeMap[] = \n");
// 		out.write( "{\n" );
//                 i=0;
// 		while (i <= irrPartCount) {
//                     out.write( "{ " + fragmentIndexToTypeMap[ i ].getFirst() + ", "
//                                + fragmentIndexToTypeMap[ i ].getSecond() + ", "
//                                + String.valueOf(i) + ", 0 }" );

//                     if(i != irrPartCount)
//                         out.write(",");
//                     out.write("\n");
//                     i++;
//                 }
// 		out.write("};\n\n");

// 		out.write( "const int MSD::IrreduciblePartitionIndexToTypeMapSize = "+(irrPartCount+1)+";\n\n" );



//            break;
        }
  case ProbspecgenDB2.RESIDUE_FRAGMENTATION:
      {
    /******************************************************************************
     *           Printout the Irreducible partition
     *****************************************************************************/
    if(ProbspecgenDB2.verbose)
        System.out.println( "Printing out the Irreducible Partition List... "
          +"All molecules in their own fragment (default setting)\n\n");
    
    out.write( "// All molecules in their own fragment\n" );
    out.write( "const IrreduciblePartitionType MSD::IrreduciblePartitionTypeList[] = \n");
    out.write("{\n");
    
                boolean firstEntry = true;
                int oxygenPosition = -1;
    i=0;
                boolean isWater = false;


                // Vector fragmentIndexToTypeMap = new Vector();
                mFragmentTypeList = new Vector();
                Vector fragmentTypes = new Vector();

                ConnectedPartition current;
                ConnectedPartition prev=null;
                int fragmentType = -1;

                int MAXFRAGMENTS = 256;

                int connectedPartitionId = 0;

                if(ProbspecgenDB2.debug)
                    System.out.println("Number of Connected Partition: " + irrPartCount );

                int moleculeId = 0;

    while ( connectedPartitionId <= irrPartCount ) {

                    current = irrPart[ connectedPartitionId ];
                    if(ProbspecgenDB2.debug)
                        System.out.println("ConnectedPartition: irrPart["+connectedPartitionId+"]\n" + current);

                    if(!current.equalTo(prev)) {
                        fragmentType++;
                        if(ProbspecgenDB2.debug)
                            System.out.println("Fragment Type: " + fragmentType);


                        int firstSiteInFragment    = current.getFirstSiteId();
                        int numberOfSitesInFragment = current.getNumberOfSites();

                        if(ProbspecgenDB2.debug)
                            System.out.println("firstSiteInFragment: " + firstSiteInFragment
                                               + " numberOfSitesInFragment: " + numberOfSitesInFragment);

                        if( IsProtein( firstSiteInFragment ) ) {

                            Vector fragmentedProtein = fragmentProtein(firstSiteInFragment, numberOfSitesInFragment);

                            int numberOfNewFragments = fragmentedProtein.size();

                            Hashtable aminoAcidNameToTypeMap = new Hashtable();

                            if(ProbspecgenDB2.debug)
                                System.out.println("NumberOfNewFragment = " + numberOfNewFragments );

                            if(numberOfNewFragments != 0)
                                fragmentType--;

                            for(int j=0; j < numberOfNewFragments; j++) {
                                 Fragment currentFragment = (Fragment) fragmentedProtein.elementAt( j );

                                 if(ProbspecgenDB2.debug)
                                     System.out.println("CurrentFragment = " + currentFragment);

                                 int firstSiteInProteinFragment = currentFragment.getFirstSiteId();
                                 int numberOfSites = currentFragment.getNumberOfSites();
                                 String aminoAcidName = currentFragment.getAminoAcidName().trim();

                                 String aminoAcidKey = aminoAcidName + ":" + numberOfSites;

                                 Integer type = (Integer) aminoAcidNameToTypeMap.get(aminoAcidKey);
                                 int currentFragmentType = -1;
                                 if(type == null) {
                                     fragmentType++;
                                     currentFragmentType = fragmentType;
                                     aminoAcidNameToTypeMap.put( aminoAcidKey , new Integer(fragmentType) );
                                     //if(j != numberOfNewFragments-1)
                                     //  fragmentType++;
                                 }
                                 else {
                                     currentFragmentType = type.intValue();
                                 }

                                 if(ProbspecgenDB2.debug)
                                     System.out.println("CurrentFragmenType = " + currentFragmentType);

                                 mFragmentTypeList.add(new IntegerPairRep( firstSiteInProteinFragment,
                                                                                currentFragmentType,
                                                                                moleculeId));
                                 // i++;

                                 if(type == null) {
                                     String fragmentTypeString = getFragmentString(firstSiteInProteinFragment, numberOfSites, aminoAcidName);
                                     fragmentTypes.add( fragmentTypeString );
                                 }
                            }

                            moleculeId++;

                            // irrPartCount += numberOfNewFragments - 1;
                            // i += numberOfNewFragments - 1;
                         }
                         else {
                             if(ProbspecgenDB2.debug)
                                 System.out.println("Non a protein: firstSite: " + firstSiteInFragment
                                                    + " fragmentType: " + fragmentType);

                             mFragmentTypeList.add( new IntegerPairRep( firstSiteInFragment, fragmentType, moleculeId++ ));
                             String fragmentTypeString = getFragmentString(firstSiteInFragment, numberOfSitesInFragment, "NONE");
                             fragmentTypes.add( fragmentTypeString );

                             if(ProbspecgenDB2.debug)
                                 System.out.println("Non a protein: numberOfFragments: " + mFragmentTypeList.size()
                                                    + " numberOfFragmentTypes: " + fragmentTypes.size());

                         }
                    }
                    else {
                        if(ProbspecgenDB2.debug)
                            System.out.println("Fragment type already exists: type: "
                                               + fragmentType + " first site id: " + current.getFirstSiteId());

                        mFragmentTypeList.add( new IntegerPairRep( current.getFirstSiteId(), fragmentType, moleculeId++ ) );
                    }

                    prev = current;
        connectedPartitionId++;
    }

                for( int j=0; j < fragmentTypes.size(); j++) {
                    String fragType = (String) fragmentTypes.elementAt( j );
                    out.write(fragType);
                    if(j != fragmentTypes.size()-1 )
                        out.write(",");
                    out.write("\n");
                }
    
    out.write( "};\n\n");
    
    // irrPartCount++;
    out.write( "const int MSD::IrreduciblePartitionTypeListSize = " + fragmentTypes.size() + ";\n\n" );
    out.write( "const int MSD::IrreduciblePartitionCount = " + mFragmentTypeList.size() + ";\n\n");
                ProbspecgenDB2.globalDefines.put("NUMBER_OF_FRAGMENTS", new String( mFragmentTypeList.size() + ""));
    /*****************************************************************************/


    out.write( "// {start_site , ip_type, molecule_id, fragments_index_in_molecule} \n");
    out.write( "const IrreduciblePartitionMap MSD::IrreduciblePartitionIndexToTypeMap[] = \n");
    out.write( "{\n" );


                int indexInMolecule = 0;
                int curMolId = 0;
                int prevMolId = 0;

    for (int j=0; j < mFragmentTypeList.size(); j++) {

                    IntegerPairRep rep = (IntegerPairRep) mFragmentTypeList.elementAt(j);

                    curMolId = rep.getOptElement();

                    if(curMolId != prevMolId) {
                        indexInMolecule = 0;
                    }

                    out.write( "{ " + rep.getFirst() + ", " + rep.getSecond() + ", "
                               + curMolId + ", " + indexInMolecule + " }" );
                    indexInMolecule++;

                    if(j != mFragmentTypeList.size()-1)
                        out.write(",");
                    out.write("\n");

                    prevMolId = curMolId;
                }
    out.write("};\n\n");

    out.write( "const int MSD::IrreduciblePartitionIndexToTypeMapSize = "+mFragmentTypeList.size()+";\n\n" );

    break;
      }
        case ProbspecgenDB2.RIGID_GROUP_FRAGMENTATION:
            {
            // This fragmentation scheme splits up the protein chain only
            // making sure that the rigid bonds do not cross fragments.
            // The algorithm relies on the Rigid{n}Site lists and the crumbList

            // 1. Put the list into one data structure
            // 2. Sort each row of the data structure merge the crumbs favoring the smaller groups
            // only if the crumb connects with that group

                out.flush();

//                 if(!RunTimeParameters.doShakeRattleProtein())
//                     throw new Exception("ERROR: RigidGroup fragmentation is defined, but rigid protein is NOT on");

                // This is a vector of VectorComparables
                Vector proteinFragmentation = new Vector();
                mFragmentTypeList = new Vector();

                if(proteinFragmentation == null) {
                    System.out.println("ERROR:: Not enough memory for proteinFragmentation");
                    System.exit(1);
                }

                for(int p=0; p<3; p++) {
                    int numberOfSitesPerRow = p+2;

                    if(rigidSites[p] == null) {
                        System.out.println("ERROR:: rigidSites[ p ] == null");
                        System.exit(1);
                    }

                    int numberOfSites = rigidSites[p].size();
                    int numberOfRows = numberOfSites / numberOfSitesPerRow;

                    for(int row=0; row < numberOfRows; row++) {
                        Vector toAdd = new Vector();
                        for(int col=0; col < numberOfSitesPerRow; col++) {

                            int baseIndex = row * numberOfSitesPerRow;

                            Integer siteId = (Integer)rigidSites[p].elementAt( baseIndex + col );
                            toAdd.add(siteId);
                        }
                        proteinFragmentation.add(new VectorComparable(toAdd));
                    }
                }

                // System.out.println("Got here1");

                for(int row=0; row < crumbList.size(); row++) {
                    Integer siteId = (Integer) crumbList.elementAt(row);
                    Vector tempVect = new Vector();
                    tempVect.add(siteId);
                    proteinFragmentation.add(new VectorComparable(tempVect));
                }

                Collections.sort(proteinFragmentation);

                Vector prevGroup = null;
                Vector curGroup = null;
                Vector nextGroup = null;

                Vector finalListOfFragments = new Vector();

                // Collect the crumbs
                for(int p=0; p < proteinFragmentation.size(); p++) {
                    curGroup = ((VectorComparable) proteinFragmentation.elementAt(p)).data;
                    if( p != proteinFragmentation.size()-1 )
                        nextGroup = ((VectorComparable) proteinFragmentation.elementAt(p+1)).data;
                    else
                        nextGroup = null;

                    if(curGroup.size() == 1) {
                        // We got ourselves a crumb
                        // The crumb get's merged with the smallest group which it has a connection with.
                        int curSiteId = ((Integer)curGroup.elementAt(0)).intValue();
                        int prevSize = (prevGroup==null) ? 99999999 : prevGroup.size();
                        int nextSize = (nextGroup==null) ? 99999999 : nextGroup.size();
                        if( prevGroup != null && prevSize < nextSize && isConnectedToGroup(curSiteId, prevGroup, blGraph)) {
                            // Add the crumb to the end of the prev
                            int finalListLastIndex = finalListOfFragments.size()-1;
                            ((Vector)finalListOfFragments.elementAt(finalListLastIndex)).add(new Integer( curSiteId ));
                            prevGroup = (Vector) finalListOfFragments.elementAt(finalListLastIndex);
                        }
                        else if(nextGroup != null && isConnectedToGroup(curSiteId, nextGroup,  blGraph)) {
                            // Add the crumb to the beginning of next
                           ((VectorComparable) proteinFragmentation.elementAt(p+1)).insertAtBeg(new Integer(curSiteId));
                           //                           prevGroup = curGroup;
                        }
                        else {
                            // This is the situation when the crumb got sorted into a contiguous order,
                            // but is not connected to either the previous or next group;
                            finalListOfFragments.add(curGroup);
                            prevGroup = curGroup;
                        }
                    } else {
                        finalListOfFragments.add(curGroup);
                        prevGroup = curGroup;
                    }
                }

                int maxSitesInFragment = 0;

                Vector typeStrings  =  new Vector();
                System.out.println("FinalListOfFragments.size() = " +  finalListOfFragments.size());
                for(i=0; i < finalListOfFragments.size(); i++) {
                    int firstSiteId = ((Integer)((Vector)finalListOfFragments.elementAt(i)).elementAt(0)).intValue();
                    int numberOfSites = ((Vector)finalListOfFragments.elementAt(i)).size();

                    String typeString = getFragmentString(firstSiteId, numberOfSites,"NONE");
                    typeStrings.add(typeString);

                    if( maxSitesInFragment < numberOfSites )
                        maxSitesInFragment = numberOfSites;
                }

                // System.out.println("Got here2");
                // Collect the lone ions
                int connectedPartitionId = 0;
                ConnectedPartition current;
                System.out.println( "irrPartCount: " + irrPartCount );
    while ( connectedPartitionId <= irrPartCount ) {
                    current = irrPart[ connectedPartitionId ];

                    int firstSiteInFragment    = current.getFirstSiteId();
                    int numberOfSitesInFragment = current.getNumberOfSites();

                    if(!IsProtein(firstSiteInFragment) && !IsSiteInWater( firstSiteInFragment )) {
                        String typeString = getFragmentString(firstSiteInFragment, numberOfSitesInFragment, "NONE");
                        typeStrings.add(typeString);

                        if( maxSitesInFragment < numberOfSitesInFragment )
                            maxSitesInFragment = numberOfSitesInFragment;

                        Vector toAdd = new Vector();
                        toAdd.add(new Integer(firstSiteInFragment));
                        finalListOfFragments.add(toAdd);
                    }
                    connectedPartitionId++;
                }

                // System.out.println("Got here3");
                // Collect the water
                int numberOfWaters = getNumberOfWaters();
                int waterTypeIndex = -1;
                int firstSiteIdOfWater=-1;

                if( ProbspecgenDB2.debug )
                    System.out.println( "numberOfWaters: " + numberOfWaters );

                if( numberOfWaters != 0 ) {
                    // Get the first water site
                    firstSiteIdOfWater = getfirstSiteIdOfWater();

                    if( ProbspecgenDB2.debug )
                        System.out.println( "RIGID_GROUP:: firstSiteIdOfWater " + firstSiteIdOfWater );

                    String waterTypeString = getFragmentString(firstSiteIdOfWater, 3, "NONE");

                    if( maxSitesInFragment < 3 )
                        maxSitesInFragment = 3;

                    typeStrings.add(waterTypeString);
                    waterTypeIndex = typeStrings.size()-1;
                }
                // System.out.println("Got here4");

    if(ProbspecgenDB2.verbose)
        System.out.println( "Printing out the Irreducible Partition List... "
          +"All molecules in their own fragment (default setting)\n\n");
    
                // out.write( "#define MAX_SITES_IN_FRAGMENT " +  maxSitesInFragment + "\n");
                if( maxSitesInFragment > 4 )
                    throw new Exception( "ERROR: MaxSitesInFragment: " + maxSitesInFragment + " > 4 " );

                ProbspecgenDB2.globalDefines.put( "MAX_SITES_IN_FRAGMENT", new String( maxSitesInFragment + ""));

    out.write( "// All molecules in their own fragment\n" );
    out.write( "const IrreduciblePartitionType MSD::IrreduciblePartitionTypeList[] = \n");
    out.write("{\n");

                for( int j=0; j < typeStrings.size(); j++) {
                    String fragType = (String) typeStrings.elementAt( j );
                    out.write(fragType);
                    if(j != typeStrings.size()-1 )
                        out.write(",");
                    out.write("\n");
                }

                out.write("};\n");
                out.write( "const int MSD::IrreduciblePartitionTypeListSize = " + typeStrings.size() + ";\n\n" );

                out.flush();


                // At this point our protein is fragmented. Now gather the lone ions and water.
                if(1==0) {
                    for(int pp=0; pp<finalListOfFragments.size(); pp++) {
                        Vector curFrag = (Vector) finalListOfFragments.elementAt(pp);
                        for (int xx=0; xx < curFrag.size(); xx++) {
                            Integer siteId = (Integer) curFrag.elementAt(xx);
                            System.out.print(siteId.intValue());
                            if( xx != curFrag.size()-1 )
                                System.out.print(" , ");
                        }
                    if (pp != finalListOfFragments.size()-1)
                        System.out.println();
                    }
                }

                try {

                // ATTENTION!!!
                // This is where we create the mapping from external to internal site ids
                // The finalListOfFragments does not contain water

                if( ProbspecgenDB2.debug )
                    System.out.println( "RIGID_GROUP:: About to create the external to internal site mapping" );

                int internalSiteId = 0;
                boolean firstEntry=true;
                for(int fragment=0; fragment < finalListOfFragments.size(); fragment++) {
                    Vector curFrag = (Vector) finalListOfFragments.elementAt( fragment );

                    for (int siteOrd=0; siteOrd < curFrag.size(); siteOrd++) {

                        Integer siteId = (Integer) curFrag.elementAt( siteOrd );
                        if(siteId == null) {
                            System.out.println("ERROR:: siteId:(fragment:ord): " + fragment + ":" + siteOrd);
                        }

                        externalToInternalSiteIdMap[ siteId.intValue() ] = internalSiteId;
                        // System.out.println("(extSiteId:"+ siteId.intValue() +")="+internalSiteId );
                        if( siteId.intValue() != internalSiteId && firstEntry ) {
                            System.out.println("INFO:Non contiguous sites detected");
                            firstEntry=false;
                        }
                        internalSiteId++;
                    }
                }

//                 for(int j=300; j < 350; j++) {
//                     System.out.println( "j: " + j + "," + externalToInternalSiteIdMap[ j ] );
//                 }

                out.flush();
                Vector fragmentStrings = new Vector();
                int indexInMol = 0;
                int curMolId = 0;

                int curFirstSiteIdInConnectedComponent = irrPart[ curMolId ].getFirstSiteId();
                int curNumberOfSitesInConnectedComponent = irrPart[ curMolId ].getNumberOfSites();

                if( ProbspecgenDB2.debug )
                    System.out.println( "RIGID_GROUP:: Setting up the list of fragments for protein" );

                for(i=0; i<finalListOfFragments.size(); i++) {
                    int firstSiteId = ((Integer)((Vector)finalListOfFragments.elementAt(i)).elementAt(0)).intValue();

                    // Detect the change in molecule... Needed to increment the indexInMol id
                    if(firstSiteId >= curFirstSiteIdInConnectedComponent + curNumberOfSitesInConnectedComponent) {
                        curMolId++;

                        if(curMolId > irrPartCount)
                            throw new Exception("INTERNAL ERROR: (curMolId > irrPartCount)");

                        curFirstSiteIdInConnectedComponent = irrPart[ curMolId ].getFirstSiteId();
                        curNumberOfSitesInConnectedComponent = irrPart[ curMolId ].getNumberOfSites();
                        indexInMol = 0;
                    }

                    int internalSiteIdCur = externalToInternalSiteIdMap[ firstSiteId ];

                    String fragmentString = "{" + internalSiteIdCur +", " + i +", " + curMolId + ", " + indexInMol+ "}";
                    indexInMol++;
                    fragmentStrings.add(fragmentString);
                }

                int waterMoleculeId=curMolId+1;

                if( ProbspecgenDB2.debug )
                    {
                    System.out.println( "RIGID_GROUP:: Setting up the list of fragments for water: water_id=" + waterMoleculeId);
                    System.out.println( "RIGID_GROUP:: firstSiteIdOfWater: " + firstSiteIdOfWater );
                    }

                for(i=0; i<numberOfWaters; i++) {
                    int firstSiteId = firstSiteIdOfWater + 3*i;
                    externalToInternalSiteIdMap[ firstSiteId ] = firstSiteId;
                    // System.out.println("(extSiteId:"+ firstSiteId +")=" + firstSiteId );
                    String fragmentString = "{" + firstSiteId +", " + waterTypeIndex
                        +", " + waterMoleculeId + ", 0}";
                    waterMoleculeId++;
                    fragmentStrings.add(fragmentString);
                }

                if( ProbspecgenDB2.debug )
                    System.out.println( "RIGID_GROUP:: Reporting the IrreduciblePartitionIndexToTypeMap" );

    out.write( "// {start_site , ip_type, molecule_id, fragments_index_in_molecule} \n");
    out.write( "const IrreduciblePartitionMap MSD::IrreduciblePartitionIndexToTypeMap[] = \n");
    out.write( "{\n" );

                for( int j=0; j < fragmentStrings.size(); j++) {
                    String fragType = (String) fragmentStrings.elementAt( j );
                    out.write(fragType);
                    if(j != fragmentStrings.size()-1 )
                        out.write(",");
                    out.write("\n");
                }

                out.write("};\n\n");

                out.write( "const int MSD::IrreduciblePartitionIndexToTypeMapSize = "+fragmentStrings.size()+";\n\n" );
                out.write( "const int MSD::IrreduciblePartitionCount = " + fragmentStrings.size() + ";\n\n");
                ProbspecgenDB2.globalDefines.put("NUMBER_OF_FRAGMENTS", new String( fragmentStrings.size() + ""));


                mNumberOfSitePerFragmentType = new int[ typeStrings.size() ];

                for( int k=0; k<typeStrings.size();k++)
                  {
                    String fragType = (String) typeStrings.elementAt( k );
                    int BeginCount = fragType.indexOf("{") + 1;
                    int EndCount   = fragType.indexOf(",");

                    String CountStr = fragType.substring( BeginCount, EndCount );
                    CountStr = CountStr.trim();

                    int Count       = (Integer.valueOf( CountStr )).intValue();
                    mNumberOfSitePerFragmentType[ k ] = Count;
                  }


                for( int j=0; j < fragmentStrings.size(); j++ )
                  {
                    String fragType = (String) fragmentStrings.elementAt( j );

//                     System.out.println("fragType: " + fragType );

                    int BeginIndexOfFirstSiteId = fragType.indexOf("{") + 1;
                    int EndIndexOfFirstSiteId   = fragType.indexOf(",");
                    int EndIndexOfCount   = fragType.indexOf("," , EndIndexOfFirstSiteId+1 );

//                     System.out.println("BeginIndexOfFirstSiteId: " + BeginIndexOfFirstSiteId );
//                     System.out.println("EndIndexOfFirstSiteId: " + EndIndexOfFirstSiteId );
//                     System.out.println("EndIndexOfCount: " + EndIndexOfCount );

                    String FirstSiteIdInFragStr = fragType.substring( BeginIndexOfFirstSiteId, EndIndexOfFirstSiteId );
                    FirstSiteIdInFragStr = FirstSiteIdInFragStr.trim();

                    String CountStr = fragType.substring( EndIndexOfFirstSiteId+1, EndIndexOfCount );
                    CountStr = CountStr.trim();

//                     System.out.println("FirstSiteIdInFragStr: " + FirstSiteIdInFragStr);
//                     System.out.println("CountStr: " + CountStr );

                    int FirstSiteId = (Integer.valueOf( FirstSiteIdInFragStr )).intValue();
                    int Count       = (Integer.valueOf( CountStr )).intValue();

                    mFragmentTypeList.add(new IntegerPairRep( FirstSiteId,
                                                              Count,
                                                              0) );
                  }

                } catch (Exception e) {
                  e.printStackTrace(System.out);
                  System.out.println("Error Message: " +e.getMessage()+"\n");
                }

                break;
            }
  default:
      {
    throw new Exception ("ERROR:: Unknown fragmentation mode.");
      }
  }

        dumpExternalInternalSiteMaps( out );
    }

    private void dumpExternalInternalSiteMaps( FileWriter out ) throws Exception {
        int numberOfSitesInSystem = getNumberOfSitesInSystem();

        internalToExternalSiteIdMap = new int[ numberOfSitesInSystem ];


        out.write("const SiteId MSD::ExternalToInternalSiteIdMap[] = \n");
        out.write( "{\n" );
        for (int i=0; i < numberOfSitesInSystem; i++) {
            internalToExternalSiteIdMap[ externalToInternalSiteIdMap[ i ] ] = i;

            out.write( String.valueOf( externalToInternalSiteIdMap[ i ] ) );

            if( i!=numberOfSitesInSystem-1)
                out.write(",");

            if( i % 30 == 0 )
                out.write("\n");

        }
        out.write( "};\n" );
        out.write( "const int MSD::ExternalToInternalSiteIdMapSize = "+ numberOfSitesInSystem+";\n\n");


        out.write("const SiteId MSD::InternalToExternalSiteIdMap[] = \n");
        out.write( "{\n" );
        for (int i=0; i < numberOfSitesInSystem; i++) {

            out.write( String.valueOf( internalToExternalSiteIdMap[ i ] ) );

            if( i!=numberOfSitesInSystem-1)
                out.write(",");

            if( i % 30 == 0 )
                out.write("\n");
        }
        out.write( "};\n" );
        out.write( "const int MSD::InternalToExternalSiteIdMapSize = "+ numberOfSitesInSystem+";\n\n");
    }

    private void dumpBondedSitesParams(FileWriter out) throws Exception {
        /******************************************************************************
         * Instantiate the paramArray, sizeOfParams and cardinalityOfParams
         * data structures
         *****************************************************************************/
        int    udfInvocationTableSize;

        ResultSet rs, rs1, rs2;
        if ( udfTableSize != 0) {

            rs = stmt.executeQuery("SELECT COUNT(*) FROM "
                                   + "MDSYSTEM.UDF_Invocation_Table WHERE sys_id=" + systemId + " FOR READ ONLY WITH UR" ) ;
            rs.next();
            udfInvocationTableSize = rs.getInt(1);


            sizeTupleArray = new int[ udfInvocationTableSize ];
            udfCodeArray = new int[  udfInvocationTableSize ];
            siteTupleIdArray = new int [ udfInvocationTableSize ];
            int i=0;
            for (i = 0; i < udfInvocationTableSize; i++) {
                udfCodeArray[i] = -1;
                sizeTupleArray[i] = 0;
                siteTupleIdArray[i] = -1;
            }
            /*****************************************************************************/


            /******************************************************************************
             * Output the contents of the compressed parameters as well as setup
             * the apropriate data structures for outputing workorders
             *****************************************************************************/


            rs = stmt.executeQuery("SELECT * FROM "
                                   + "MDSYSTEM.UDF_Invocation_Table WHERE sys_id=" + systemId + " order by site_tuple_list_id FOR READ ONLY WITH UR");

            int siteCount;
            String paramStructName = null;
            Vector siteTupleListStringRep =  new Vector();
            Vector paramTupleListStringRep =  new Vector();

            for( i = 0;  rs.next(); i++) {
                String udfCodeString     = rs.getString("udf_id");
                int udfCode = udfRegistry.GetUDFCodeByName( udfCodeString );
                int siteTupleListId = rs.getInt("site_tuple_list_id");
                int paramTupleListId = rs.getInt("param_tuple_list_id");

                siteCount = udfRegistry.GetNSitesByUDFCode(udfCode);
    
                if ( siteTupleListId != -1 ) {
                    udfCodeArray[i] = udfCode;
    
                    rs2 = stmt2.executeQuery("SELECT COUNT(site_tuple_id) FROM "
                                             + "MDSYSTEM.SiteTuple WHERE sys_id=" + systemId
                                             + " AND site_tuple_list_id=" + siteTupleListId + " FOR READ ONLY WITH UR");
                    rs2.next();
                    int siteTupleListSize = rs2.getInt(1);
                    sizeTupleArray[i] = siteTupleListSize;
    
                    rs2 = stmt2.executeQuery("SELECT site_tuple_desc FROM "
                                             + "MDSYSTEM.SiteTupleList "
                                             + "WHERE sys_id="+systemId+" AND site_tuple_list_id="+ siteTupleListId + " FOR READ ONLY WITH UR");
    
                    rs2.next();
                    boolean listExists = false;
                    for(  int zz = 0; zz < i; zz++) {
                        if( siteTupleIdArray[zz] == siteTupleListId ) {
                            listExists = true;
                        }	
                    }

                    String siteTupleDescName = rs2.getString("site_tuple_desc");
    
                    siteTupleIdArray[i] = siteTupleListId;
                    siteTupleArray.add(new String( siteTupleDescName + "SiteTupleList"));
    
                    if(listExists) {
                        continue;
                    }
    
                    out.write( "/*\n" );
                    out.write( "These are the sites.\n" );
                    out.write( "*/\n" );
    
                    if(ProbspecgenDB2.verbose)
                        System.out.println( "Printing out SiteTupleListId: " + siteTupleListId + " ...\n\n" );
    
                    siteCount = udfRegistry.GetNSitesByUDFCode(udfCode);
    
                    String siteTupleDesc = rs2.getString("site_tuple_desc");
                    out.write("const SiteId MSD::" + siteTupleDesc  + "SiteTupleList[] = {\n");
    
                    rs2 = stmt2.executeQuery("SELECT site_id, site_tuple_id FROM "
                                             + "MDSYSTEM.SiteTupleData WHERE sys_id=" + systemId + " AND site_tuple_list_id="
                                             + siteTupleListId +" ORDER BY site_tuple_id, site_ordinal FOR READ ONLY WITH UR");
    

                    if( RunTimeParameters.doShakeRattleProtein() && siteTupleDescName.startsWith( "StdHarmonicBond" ) )
                        stdHarmonicBondTupleMask = new int[ siteTupleListSize ];

                    int jj = 0;
                    int zz = 0;
                    // System.out.println("Number of siteTupleListSize: " + siteTupleListSize );
                    int numberOfFinalSiteTuples=0;

                     if( stdHarmonicBondTupleMask == null ) {
                         while(jj < siteTupleListSize) {
                             zz = 0;
                             while( zz < siteCount ) {

                                 if(!rs2.next())
                                     throw new Exception("ERROR:: Problem reading the site tuple list for system id: " + systemId);

                                 int siteId = externalToInternalSiteIdMap[ rs2.getInt("site_id") ];
                                 out.write(String.valueOf(siteId));
                                 if(zz != siteCount-1)
                                     out.write(",");
                                 zz++;
                             }

                             if(jj != siteTupleListSize-1)
                                 out.write(",\n");
                             jj++;
                         }
                         numberOfFinalSiteTuples = siteTupleListSize;
                     }
                     else {
                         Vector bondedSites = new Vector();
                         while( jj < siteTupleListSize ) {
                         // We are dealing with StdHarmonicBond list 2 sites
                             if(!rs2.next())
                                 throw new Exception("ERROR:: Problem reading the site tuple list for system id: " + systemId);

                             int externalSiteId1 = rs2.getInt("site_id");
                             int siteId1 = externalToInternalSiteIdMap[ externalSiteId1 ];

                             if( !rs2.next() )
                                 throw new Exception("ERROR:: Problem reading the site tuple list for system id: " + systemId);

                             int externalSiteId2 = rs2.getInt("site_id");
                             int siteId2 = externalToInternalSiteIdMap[ externalSiteId2 ];

                             String queryReq = "";
                             if(externalSiteId1 < externalSiteId2)
                                 queryReq = externalSiteId1 + ":" + externalSiteId2;
                             else
                                 queryReq = externalSiteId2 + ":" + externalSiteId1;

                             /// NOTE: The stdHarmonicBoncExecludedSites are in the External Sites!!!
                             String reqString = (String) stdHarmonicBondExcludedSites.get(queryReq);

                            if(ProbspecgenDB2.debug)
                                System.out.println("queryReq: " + queryReq + "\treqString: " + reqString);

                            if(reqString == null) {
//                                 out.write(String.valueOf(siteId1) + "," + String.valueOf(siteId2));
//                                 // These sites should be included
//                                 if(jj != siteTupleListSize-1)
//                                     out.write(",\n");
                                bondedSites.add(new String(String.valueOf(siteId1) + "," + String.valueOf(siteId2)));
                                stdHarmonicBondTupleMask[ jj ] = 1;
                            } else
                                stdHarmonicBondTupleMask[ jj ] = 0;
                            jj++;
                         }

//                          int bondedListSize = 0;
//                          jj=0;
//                          while( jj < siteTupleListSize ) {
//                              if( stdHarmonicBondTupleMask[ jj] == 1)
//                                  bondedListSize++;
//                              jj++;
//                          }

                         if(ProbspecgenDB2.debug)
                             System.out.println("bondSites.size(): " + bondedSites.size());

                         jj=0;
                         while( jj < bondedSites.size() ) {
                             out.write((String) bondedSites.elementAt(jj));
                             numberOfFinalSiteTuples++;

                             // These sites should be included
                             if(jj != bondedSites.size()-1) {
                                 out.write(",\n");
                                 // numberOfFinalSiteTuples++;
                             }
                             jj++;
                         }
                         numberOfFinalSiteTuples = jj;
                         sizeTupleArray[i] = bondedSites.size();
                     }

                    out.flush();
                    out.write("};\n");

                    out.write("\n#define NUMBER_OF_" + siteTupleDesc + "_SITE_TUPLES " + numberOfFinalSiteTuples + "\n\n");
                }

                if ( paramTupleListId != -1 ) {

                    paramStructName = udfRegistry.GetStructureNameByUDFCode(udfCode);
                    rs1 = stmt1.executeQuery("SELECT param_tuple_desc FROM "
                                             + "MDSYSTEM.ParamTupleList WHERE sys_id=" + systemId + " AND param_tuple_list_id="
                                             + paramTupleListId + " FOR READ ONLY WITH UR");
                    rs1.next();
                    String paramName = rs1.getString("param_tuple_desc");

                    paramTupleArray.add(new String(paramName + "ParamTupleList"));

                    siteTupleListStringRep.add(new String( paramName + "ParamTupleList" ));
                    udfCodeArray[i] = udfCode;
                    rs1 = stmt1.executeQuery("SELECT COUNT(*) FROM "
                                             + "MDSYSTEM.ParamTuple WHERE sys_id=" + systemId + " AND param_tuple_list_id="
                                             + paramTupleListId + " FOR READ ONLY WITH UR");
                    rs1.next();
                    // sizeTupleArray[ i ] = rs1.getInt( 1 );
    
                    out.write( "/*\n" );
                    out.write( "The parameters are in the order of the structs above.\n" );
                    out.write( "*/\n" );
    
                    if(ProbspecgenDB2.verbose)
                        System.out.println( "Printing out " + paramName + "ParamTupleList...\n\n");
    
                    int argCount = udfRegistry.GetArgCountByUDFCode(udfCode);
    
                    boolean mustEnter;
                    int paramMap[] = new int[ argCount ];
    
                    int j = 0;		
                    while ( j < argCount ) {
                        String ordName = udfRegistry.GetArgByOrd(udfCode, j);
                        mustEnter = true;
                        rs1 = stmt1.executeQuery("SELECT param_name, param_id FROM "
                                                 + "MDSYSTEM.ParamTupleSpec "
                                                 + "WHERE sys_id=" + systemId + " AND param_tuple_list_id=" + paramTupleListId + " FOR READ ONLY WITH UR");
      
                        int z = 0;
      
                        while(rs1.next()) {
                            String paramNameInTuple = rs1.getString("param_name");
                            if ( paramNameInTuple.equals( ordName ) ) {
        // paramMap[j] = rs1.getInt("param_id");
                                int paramId = rs1.getInt("param_id");
                                paramMap[ paramId ] = j;
                                mustEnter = false;
                                break;
                            }
                            z++;
                        }
      
                        if(mustEnter)
                            throw new Exception("The arg name: " + ordName + " is not found in the list of parameters");
                        j++;
                    }
    
                    out.write("const UDF_Binding::" + paramStructName + " MSD::" + paramName + "ParamTupleList[] = {\n");
                    // 		    rs1 = stmt1.executeQuery("SELECT param_tuple_id FROM "
                    // 					     + "MDSYSTEM.ParamTuple "
                    // 					     +"WHERE sys_id=" + systemId + " AND param_tuple_list_id=" + paramTupleListId + " FOR READ ONLY WITH UR");
    
                    rs2 = stmt2.executeQuery("SELECT COUNT(param_tuple_id) FROM "
                                             + "MDSYSTEM.ParamTuple "
                                             +"WHERE sys_id=" + systemId + " AND param_tuple_list_id=" + paramTupleListId + " FOR READ ONLY WITH UR");
    
                    rs2.next();
                    int parameterTupleCount = rs2.getInt(1);
                    int numberOfFinalParamTuples = 0;

                    rs2 = stmt2.executeQuery("select param_value, param_tuple_id, param_id from mdsystem.paramtupledata"
                                             + " where sys_id = " + systemId
                                             + " and param_tuple_list_id = " + paramTupleListId + " order by param_tuple_id, param_id FOR READ ONLY WITH UR");

                    // Additional detail for the evaluation hack

                    int k=0;
                    int n=0;
                    String[] tempParamValue = new String[ argCount ];
                    // System.out.println("Number of parameterTupleCount: " + parameterTupleCount );
                    while( k < parameterTupleCount ) {

                        n=0;
                        while( n < argCount) {
                            if(!rs2.next())
                                throw new Exception("ERROR: Problem occured while reading parameter tuple list for system: " + systemId);

                            int paramId = rs2.getInt("param_id");
                            String value = rs2.getString("param_value");
                            tempParamValue[ paramMap[ paramId ] ] = value;
                            n++;
                        }

                        // Skip the excluded parameters
                        if(RunTimeParameters.doShakeRattleProtein() && paramName.startsWith("StdHarmonicBond") ) {
                            if(stdHarmonicBondTupleMask[ k ] == 0) {
                                if(k == parameterTupleCount-1)
                                    stdHarmonicBondTupleMask = null;

                                k++;
                                continue;
                            }

                            if(k == parameterTupleCount-1)
                                stdHarmonicBondTupleMask = null;
                        }

                        n=0;
                        numberOfFinalParamTuples++;
                        out.write("{ ");
                        while( n < argCount ) {

                            // This is where we evaluate "*Math::Deg2Rad"
                            String paramString = EvaluateString( tempParamValue[ n ] );

                            out.write( paramString );
                            if( n != argCount-1 ) {
                                out.write(" , ");
                            }
                            n++;
                        }
                        out.write(" }");

                        if( k != parameterTupleCount-1)
                            out.write(",\n");
                        k++;
                    }
                    out.write(" };\n");

                    out.write("\n#define NUMBER_OF_" + paramName + "_PARAM_TUPLES " + numberOfFinalParamTuples + "\n\n");

                    out.flush();		
                } else {
                    paramTupleArray.add(new String("NULL"));		
                }
            }
            /*****************************************************************************/
        }
    }

    private int createSiteInformation () throws Exception {
  /******************************************************************************
   * Site Information Translation
   ******************************************************************************/
  if(ProbspecgenDB2.verbose)
      System.out.println("Printing out SystemInformation Table...\n\n");
  
// 	ResultSet rs = stmt.executeQuery("SELECT EPSILON14, RMIN14, ATOMIC_MASS, CHARGE "
// 					 + "FROM "
// 					 + "MDSYSTEM.SITE WHERE sys_id=" + systemId
// 					 + " GROUP BY EPSILON14, RMIN14, "
// 					 + "ATOMIC_MASS, CHARGE");
  
  ResultSet rs1 = stmt1.executeQuery("SELECT COUNT( * ) "
             + "FROM "
             + "MDSYSTEM.SITE WHERE sys_id=" + systemId
             + " GROUP BY  EPSILON14, RMIN14, "
             + "ATOMIC_MASS, CHARGE FOR READ ONLY WITH UR");
  
  int numSiteTypes = 0;
  while(rs1.next()) {
      numSiteTypes++;
  }
  
  rs1 = stmt1.executeQuery("SELECT COUNT(*) FROM "
         + "MDSYSTEM.SITE WHERE sys_id="+ systemId + " FOR READ ONLY WITH UR");
  rs1.next();
  int numSites = rs1.getInt(1);
  siteIDSiteTypeMap = new int[ numSites ];

  
        String currentWaterMethod = RunTimeParameters.getWaterModel();
        String oCharge = "NULL";
        String hCharge = "NULL";

        if(currentWaterMethod != null) {
            ResultSet rs2 = stmt2.executeQuery("SELECT q1, q2 from chem.water_parameters where water_model='" + currentWaterMethod+"' FOR READ ONLY WITH UR");
            if(!rs2.next())
                throw new Exception ("Water charges (q1,q2) are not defined for water model: " + currentWaterMethod);

            oCharge = rs2.getString("q1") + " * ChargeConversion";
            hCharge = rs2.getString("q2") + " * ChargeConversion";
        }

        ResultSet rs11 = stmt1.executeQuery("SELECT site_id, epsilon14, rmin14, atomic_mass, charge, element_symbol FROM "
                                            + "MDSYSTEM.SITE "
                                            + "WHERE sys_id=" + systemId
                                            + " ORDER BY  epsilon14, rmin14, atomic_mass, charge FOR READ ONLY WITH UR;");
  int i=0;

        String currentType = "";
        String prevType    = "";
  int siteInfoIndex = -1;
  while(rs11.next()) {
            int siteId           = externalToInternalSiteIdMap[ rs11.getInt("site_id") ];
            String epsilon14     = rs11.getString("epsilon14").trim();
            String rmin14        = rs11.getString("rmin14").trim();
      String atomicMass    = rs11.getString("atomic_mass").trim();
      String charge        = rs11.getString("charge").trim();
            String elementSymbol = rs11.getString("element_symbol").trim();

            currentType = epsilon14 + ":" + rmin14 + ":" + atomicMass + ":" + charge;
  
            if( !currentType.equals(prevType) ) {

// 	    rs1 = stmt1.executeQuery("SELECT SITE_ID, element_symbol FROM "
// 				     + "MDSYSTEM.SITE "
// 				     + "WHERE sys_id=" + systemId
//                                      + " AND EPSILON14='" + epsilon14 + "'"
//                                      + " AND RMIN14='" + rmin14 + "'"
// 				     + " AND ATOMIC_MASS='" + atomicMass + "'"
// 				     + " AND CHARGE='" + charge + "'");
              
                // String elementSymbol = null;
                // while(rs1.next()) {
                // elementSymbol = (rs1.getString("element_symbol")).trim();
    // int siteId1 = rs1.getInt("SITE_ID");
                // }	
  
                if(charge.lastIndexOf("NULL") != -1 ) {

                    if(currentWaterMethod == null)
                        throw new Exception("ERROR: There are NULLs assigned to charges in xml, but no water method defined in rtp");

                    if( elementSymbol.equals("H"))
                        charge = hCharge;
                    else if( elementSymbol.equals("O"))
                        charge = oCharge;
                    else
                        throw new Exception("Charge parameter is NULL, but the site is neither an \"H\" nor an \"O\"");
                }

                if(epsilon14.lastIndexOf("NULL") != -1) {

                    if(currentWaterMethod == null)
                        throw new Exception("ERROR: There are NULLs assigned to epsilon14 in xml, but no water method defined in rtp");
                    epsilon14 = "0.0";
                }

                if(rmin14.lastIndexOf("NULL") != -1) {
                    if(currentWaterMethod == null)
                        throw new Exception("ERROR: There are NULLs assigned to epsilon14 in xml, but no water method defined in rtp");

                    rmin14 = "0.0";
                }

                siteTypeList.add(new SiteTypeRep(atomicMass, charge, epsilon14, rmin14));
                siteInfoIndex++;
            }

            prevType = currentType;
            siteIDSiteTypeMap[ siteId ] = siteInfoIndex;

  }


  return numSites;
  /****************************************************************************/
    }

    private void getPair14ExcludedLists() throws Exception {
  /******************************************************************************
   *    Setup the pair14/Excluded lists
   *****************************************************************************/
  
  Vector adjListForNode = null;
  IntegerPairRep curPair = null;

  for (int i=0; i < graphSepList.size(); i++ ) {
      adjListForNode = (Vector) graphSepList.elementAt(i);
      for(int j=0; j < adjListForNode.size(); j++) {
    curPair = (IntegerPairRep) adjListForNode.elementAt(j);
    if(curPair.getFirst() == 3) {
      if ( i < curPair.getSecond()) {
          pair14List.add(new IntegerPairRep(i,curPair.getSecond()));
      } else {
          pair14List.add(new IntegerPairRep(curPair.getSecond(), i));
      }
      pair14Count++;
    }
    else if ((curPair.getFirst() == 2) || (curPair.getFirst() == 1)) {
        if ( i < curPair.getSecond()) {
      excludedList.add(new IntegerPairRep(i,curPair.getSecond()));
        } else {
      excludedList.add(new IntegerPairRep(curPair.getSecond(), i));
        }
        excludedCount++;
    }
      }
  }
    }

    private void createRigidSitesParamsState(BondListAsGraph blGraph, FileWriter out) throws Exception {

        Vector adjList[] = blGraph.getAdjList();

        for(int i=0; i < 3; i++) {
            rigidSites[i] = new Vector();
            rigidParams[i] = new Vector();
        }

        ResultSet rs1 = stmt1.executeQuery("select distinct mstd1.site_id, mstd2.site_id, mptd.param_value "
                                           +"  from mdsystem.sitetuplelist as mstl, "
                                           +"       mdsystem.sitetupledata as mstd1, "
                                           +"       mdsystem.sitetupledata as mstd2, "
                                           +"       mdsystem.paramtuplelist as mptl, "
                                           +"       mdsystem.paramtuplespec as mpts, "
                                           +"       mdsystem.paramtupledata as mptd "
                                           +" where mstl.sys_id="+systemId+" and "
                                           +"       mptd.sys_id="+systemId+" and "
                                           +"       mstd1.sys_id="+systemId+" and "
                                           +"       mstd2.sys_id="+systemId+" and "
                                           +"       mptd.sys_id="+systemId+" and "
                                           +"       mpts.sys_id="+systemId+" and "
                                           +"       mstl.site_tuple_desc='StdHarmonicBondForce_sites' and "
                                           +"       mptl.param_tuple_desc='StdHarmonicBondForce_params' and "
                                           +"       mptd.param_tuple_list_id=mptl.param_tuple_list_id and "
                                           +"       mpts.param_tuple_list_id=mptl.param_tuple_list_id and "
                                           +"       mpts.param_name='r0' and "
                                           +"       mptd.param_id=mpts.param_id and "
                                           +"       mstd1.site_tuple_list_id=mstl.site_tuple_list_id and "
                                           +"       mstd2.site_tuple_list_id=mstl.site_tuple_list_id and "
                                           +"       mstd1.site_tuple_id=mstd2.site_tuple_id and "
                                           +"       mptd.param_tuple_id=mstd1.site_tuple_id and "
                                           +"       mstd1.site_id < mstd2.site_id "
                                           +" order by mstd1.site_id FOR READ ONLY WITH UR; ");

        Hashtable reqForSites = new Hashtable();
        while(rs1.next()) {
            int siteid1 = rs1.getInt(1) ;
            int siteid2 = rs1.getInt(2) ;
            String reqString = rs1.getString(3);
            String siteComb = siteid1+":"+siteid2;
            reqForSites.put(siteComb, reqString);
        }

        ResultSet rs = stmt.executeQuery("SELECT site_id, component_name, element_symbol from mdsystem.site where sys_id="+systemId  + " order by site_id FOR READ ONLY WITH UR");

        while(rs.next()) {
            int siteId =  rs.getInt( 1 ) ;
            String componentName = rs.getString(2).trim();
            String elementSymbol = rs.getString(3).trim();

            // make sure that the site is a protein site
            if(!componentName.equals("HOH") && !componentName.equals("NAP")) {
                if( !elementSymbol.equals("H") ) {
//                    elementSymbol.equals("N") ||
//                    elementSymbol.equals("O") ||
//                    elementSymbol.equals("P") ||
//                    elementSymbol.equals("S")) {
                    Vector adjListForNode = adjList[ siteId ];

                    int siteIdsForRigid[] = new int[4];
                    String reqForRigid[] = new String[3];
                    int numberOfHydrogens=0;

                    // String site36Str="";
                    for(int i=0; i<adjListForNode.size(); i++) {
                        int siteIdRigid = ((SiteRep)adjListForNode.elementAt(i)).siteId;
                        String bondedElementSymbol = ((SiteRep)adjListForNode.elementAt(i)).elementSymbol;

                        if(bondedElementSymbol.equals("H")) {
                            siteIdsForRigid[ numberOfHydrogens++ ] = siteIdRigid;

                            String queryReq = "";
                            if(siteId < siteIdRigid)
                                queryReq = siteId + ":" +siteIdRigid;
                            else
                                queryReq = siteIdRigid + ":" +siteId;

                            String reqString = (String) reqForSites.get( queryReq );

//                             if( queryReq.equals("35:36") ) {
//                                 System.out.println("Req for 35:36: " + reqString );
//                                 site36Str=queryReq;
//                             }

                            if(reqString == null)
                                throw new Exception("ERROR: No rEQ found for sites: {"+siteId+","+siteIdRigid+"}");

                            stdHarmonicBondExcludedSites.put(queryReq, queryReq);
                            reqForRigid[numberOfHydrogens-1] = reqString;
                        }
                    }

                    if( numberOfHydrogens > 0 ) {
                        rigidSites[ numberOfHydrogens-1 ].add( new Integer( siteId ) );

                        for(int i=0; i < numberOfHydrogens; i++) {
                            rigidSites[ numberOfHydrogens-1 ].add(new Integer(siteIdsForRigid[i]));

//                             if( site36Str.equals("35:36") ) {
//                                 System.out.println("Req for 35:36 put into rigidParams: " + reqForRigid[ i ]
//                                                    + " numberOfHydrogens=" + numberOfHydrogens );

//                                 System.out.println("Before add to rigid Params size=" + rigidParams[ numberOfHydrogens-1 ].size() );
//                             }

                            rigidParams[ numberOfHydrogens-1 ].add(reqForRigid[i]);
                        }
                    }
                    else {
                        // We found a crumb
                        crumbList.add(new Integer(siteId));
                    }
                }
            }
        }
    }

    void dumpRigidSitesParamsState( FileWriter out ) throws Exception {

        for(int i=0; i < 3; i++) {
            int numberOfSites = rigidSites[i].size();
            
            int numberOfRows = numberOfSites / (i+2);
            out.write("\n\n#define RIGID" + (i+2) + "SITES_TUPLE_CNT " + numberOfRows);

            if( numberOfSites == 0 ){

                String rigidListName = "Rigid" + (i+2) + "Sites";
                out.write("\n\nconst SiteId MSD::" + rigidListName + "[] = { -1 }; \n");

                rigidListName = "Rigid" + (i+2) + "Params";
                String rigidParamType = "UDF_Binding::ShakeGroup" + (i+2) + "_Params";
                out.write("\n\nconst " + rigidParamType + " MSD::" + rigidListName + "[] = { { {1.0} }  }; \n");

                continue;
            }

            if(ProbspecgenDB2.debug)
                System.out.println("NumberOfConstraints (before): " + numberOfConstraints);

            if(RunTimeParameters.doShakeRattleProtein())
                numberOfConstraints += (numberOfRows*(i+1));

            if(ProbspecgenDB2.debug)
                System.out.println("NumberOfConstraints (after): " + numberOfConstraints);

            String rigidListName = "Rigid" + (i+2) + "Sites";
            out.write("\n\nconst SiteId MSD::" + rigidListName + "[] = {\n");

            int site36TupleId=0;

            for(int j=0; j < numberOfSites; j++) {
                int externalSiteId = ((Integer)rigidSites[i].elementAt(j)).intValue();
                if( externalSiteId == 36 )
                    site36TupleId = j / 3;

                int siteId = externalToInternalSiteIdMap[ externalSiteId ];
                out.write(String.valueOf(siteId));
                if(j!=rigidSites[i].size()-1) {
                    out.write(",");
                    if(((j+1)%(i+2))==0)
                        out.write("\n");
                }
            }
            out.write("};");

            rigidListName = "Rigid" + (i+2) + "Params";
            String rigidParamType = "UDF_Binding::ShakeGroup" + (i+2) + "_Params";
            out.write("\n\nconst " + rigidParamType + " MSD::" + rigidListName + "[] = {\n");
            int numberOfParamTuples = (rigidParams[ i ].size())/(i+1);

            for(int j=0; j < numberOfParamTuples; j++) {
                out.write("{{ ");
                for(int z=0; z < (i+1); z++) {
                    String paramValue = (String) rigidParams[i].elementAt( (i+1)*j + z );
//                     if (j == site36TupleId && i == 1 )
//                         System.out.println("site36TupleId params: " + paramValue + " site36TupleId="+((i+1)*j + z));

                    out.write(paramValue);
                    if(z!=i)
                        out.write(",");
                }
                out.write("}} ");
                if(j != numberOfParamTuples-1)
                    out.write(",\n");
            }
            out.write("};");
        }
    }

    private void dumpWater3SiteList(FileWriter out) throws Exception {
  ResultSet rs = stmt.executeQuery("SELECT site_tuple_list_id FROM MDSYSTEM.SiteTupleList "
           + "WHERE sys_id=" + systemId
           + " AND site_tuple_desc LIKE 'Water3sites' FOR READ ONLY WITH UR");
  if(!rs.next())
      throw new Exception("dumpWater3SiteList is called but no \"Water3sites\" defined in xml/db2");
  
  int siteTupleListId = rs.getInt(1);
  ResultSet rs2 = stmt2.executeQuery("SELECT COUNT(site_tuple_id) FROM "
         + "MDSYSTEM.SiteTuple WHERE sys_id=" + systemId
         + " AND site_tuple_list_id=" + siteTupleListId + " FOR READ ONLY WITH UR");
  rs2.next();
  int siteTupleListSize = rs2.getInt(1);
  
// 	rs2 = stmt2.executeQuery("SELECT site_tuple_id FROM "
// 					     + "MDSYSTEM.SiteTuple WHERE sys_id=" + systemId + " AND site_tuple_list_id="
// 					     + siteTupleListId + " FOR READ ONLY WITH UR");


        rs2 = stmt2.executeQuery("SELECT site_id, site_tuple_id FROM "
                                 + "MDSYSTEM.SiteTupleData WHERE sys_id=" + systemId + " AND site_tuple_list_id="
                                 + siteTupleListId +" ORDER BY site_tuple_id, site_ordinal FOR READ ONLY WITH UR");



  out.write("const SiteId MSD::Water3sites[] = {\n");

        int siteCount = 3;
        int jj = 0;
        int zz = 0;
        while(jj < siteTupleListSize) {
            zz = 0;
            while( zz < siteCount ) {

                if(!rs2.next())
                    throw new Exception("ERROR:: Problem reading the water 3 site list for system id: " + systemId);

                int siteId = externalToInternalSiteIdMap[ rs2.getInt("site_id") ];
                out.write(String.valueOf(siteId));
                if(zz != siteCount-1)
                    out.write(",");
                zz++;
            }

            if(jj != siteTupleListSize-1)
                out.write(",\n");
            jj++;
        }

// 	int siteCount = 3;
// 	int jj=0;
// 	while( rs2.next() ) {
// 	    int siteTupleId = rs2.getInt("site_tuple_id");
// 	    ResultSet rs3 = stmt3.executeQuery("SELECT site_id FROM "
// 				     + "MDSYSTEM.SiteTupleData WHERE sys_id=" + systemId
// 				     +  " AND site_tuple_list_id=" + siteTupleListId
// 				     + " AND site_tuple_id=" + siteTupleId + " FOR READ ONLY WITH UR");
  
// 	    int z = 0;			
// 	    while(rs3.next()) {
// 		int siteId = rs3.getInt("site_id");
// 		out.write(String.valueOf(siteId));
// 		if( z != siteCount-1 )
// 		    out.write(",");
// 		z++;
// 	    }
// 	    if(jj < siteTupleListSize-1 )
// 		out.write(" ,\n");
// 	    else
// 		out.write("\n");
  
// 	    jj++;
// 	}
  out.write("};\n\n");
    }

    private Vector compressPairList(Vector pairList) throws Exception {
  
  /**************************************************************************
   *                  Compress the Excluded List
   *************************************************************************/
  Vector pairListCompressed = new Vector();
  timeS = System.currentTimeMillis();
  Collections.sort(pairList);
  timeF = System.currentTimeMillis();
  //System.out.println("Time to sort the excluded: " + (timeF-timeS));
  
  IntegerPairRep prev = new IntegerPairRep();
  IntegerPairRep cur;

  for(int i=0; i < pairList.size(); i++) {
      cur = (IntegerPairRep) pairList.elementAt(i);
  
      if (cur.compareTo(prev) != 0) {
    pairListCompressed.add(cur);
      }
      prev = cur;
  }
  
  return pairListCompressed;
    }

    private void dumpPair14SiteList(FileWriter out, Vector pair14Compressed) throws Exception {
  
  if(pair14Count!=0) {
  
      /**************************************************************************
       *                  Print the Pair14 List
       *************************************************************************/
      doCharge14 = true; //!ProbspecgenDB2.doEwald;
      doLJ14     = !ProbspecgenDB2.ljOff;
  
      out.write( "\nconst SiteId MSD::Pair14SiteList[] = \n");
      out.write( "{\n" );
  
            ///            FileWriter fw = new FileWriter("pairlist14.test");

      for (int i=0; i < pair14Compressed.size(); i++) {

                int siteId1 = ((IntegerPairRep) pair14Compressed.elementAt(i)).getFirst();
                int siteId2 = ((IntegerPairRep) pair14Compressed.elementAt(i)).getSecond();

                int extSiteId1 = internalToExternalSiteIdMap[ siteId1 ];
                int extSiteId2 = internalToExternalSiteIdMap[ siteId2 ];

                /// fw.write("internal("+siteId1+" , " + siteId2+") = "+ "("+ extSiteId1 +" , "+ extSiteId2 +")\n");

    out.write(((IntegerPairRep) pair14Compressed.elementAt(i)).getFirst() +
        " , " + ((IntegerPairRep) pair14Compressed.elementAt(i)).getSecond());
    if ( i != pair14Compressed.size() - 1 )
        out.write(",");
    out.write("\n");
      }
            ///            fw.close();
      out.write("};\n\n");
      out.write( "const int MSD::Pair14SiteListSize = " + pair14Compressed.size() +  ";\n\n" );

            ProbspecgenDB2.globalDefines.put("PAIR14_SITE_LIST_SIZE", new String( pair14Compressed.size() + ""));
  }
  else {
      /***************************************************************************
       *            If there are no pair14 entries printout { -1, -1 }
       **************************************************************************/
      out.write( "const SiteId MSD::Pair14SiteList[] = \n");
      out.write( "{ \n" );
      out.write( -1 + "," + -1 + "\n" );
      out.write("};\n\n");
  
      out.write( "const int MSD::Pair14SiteListSize = " +  1 + ";\n\n");
            ProbspecgenDB2.globalDefines.put("PAIR14_SITE_LIST_SIZE", new String( 1 + ""));
      /**************************************************************************/
  }
    }

    private void dumpCharge14Params(FileWriter out, Vector pair14Compressed ) throws Exception {

  int chargeCode = udfRegistry.GetUDFCodeByName("Coulomb14");
  stdChargeName = "UDF_Binding::" + udfRegistry.GetStructureNameByUDFCode(chargeCode);

  if(doCharge14) {
  
      out.write( "const " + stdChargeName +" MSD::Charge14PairTable[] = \n");
      out.write( "{ \n" );
  
      int site1 = 0;
      int site2 = 0;
  
            String curString  = "";
            String prevString = "";


            Vector charge14Params = new Vector();

      for(int i=0; i<pair14Compressed.size(); i++) {

                IntegerPairRep curPairRep =  (IntegerPairRep) pair14Compressed.elementAt(i);

    site1 = curPairRep.getFirst();
    site2 = curPairRep.getSecond();
    
    int siteType1  = siteIDSiteTypeMap[ site1 ];
    int siteType2  = siteIDSiteTypeMap[ site2 ];
    
    String charge1 = ((SiteTypeRep) siteTypeList.elementAt(siteType1)).getCharge();
    String charge2 = ((SiteTypeRep) siteTypeList.elementAt(siteType2)).getCharge();
    
    // out.write("{ " + charge1 + "*" + charge2 + "*charge14scale }");

                curString = "{ " + charge1 + " * " + charge2 + " * charge14scale }";
                // charge14Params.add(new ParamSortHelper(curString, curPairRep));
                out.write(curString);

                if( pair14Compressed.size()-1 != i )
                    out.write(",");
                out.write("\n");
            }

//             Collections.sort(charge14Params);

//             int paramIndexToCompressedIndexMap = new int[ pair14Compressed.size() ];
//             int compressedIndex = -1;

//             for (int i=0; i < charge14Params.size()) {
//                 ParamSortHelper curSH = (ParamSortHelper) charge14Params.elementAt(i);

//                 pair14Compressed.setElementAt( curSH.getIntegerPairRep(), i );

//                 curString = curSH.getString();

//                 if(!curString.equals(prevString)) {
//                     paramIndexToCompressedIndexMap[ i ] = compressedIndex;
//                 }
//                 else {
//                     compressedIndex++;
//                     paramIndexToCompressedIndexMap[ i ] = compressedIndex;
//                     out.write( curString );
//                 }

// 		if(i!=pair14Compressed.size()-1)
// 		    out.write(",\n");

//                 prevString = curString;
//             }

  
      out.write("};\n\n");
      out.write( "const int MSD::Charge14PairTableSize = " +  pair14Compressed.size() + ";\n\n");
  
  } else {
      out.write( "const " + stdChargeName +" MSD::Charge14PairTable[] = \n");
      out.write( "{ \n" );
      out.write( "{ -1 }\n" );
      out.write("};\n\n");
  
      out.write( "const int MSD::Charge14PairTableSize = 1;\n\n");
  }
    }

    private void dumpLJ14Params(FileWriter out, Vector pair14Compressed) throws Exception {

  ResultSet rs = null;

  if(doLJ14) {

      out.write( "const " + ljStructName +" MSD::LJ14PairTable[] = \n");
      out.write( "{ \n" );
  
      for(int i=0; i < siteTypeList.size(); i++) {
    String epsilon14 = ((SiteTypeRep) siteTypeList.elementAt(i)).getEpsilon();
    String rmin14 = ((SiteTypeRep) siteTypeList.elementAt(i)).getRmin();
    

// 		rs = stmt.executeQuery("SELECT EPSILON, SIGMA FROM "
// 				       + "MDSYSTEM.LJ14_PAIR_PARAMETER AS lj14pp, "
// 				       +" MDSYSTEM.LJ14_PAIR_PARAMETER_TYPE_LIST AS lj14pptl "
// 				       + " WHERE lj14pp.sys_id=" + systemId
// 				       + " AND lj14pptl.sys_id=" + systemId
// 				       + " AND lj14pp.PARAM_ID=lj14pptl.PARAM_ID "
// 				       + " AND lj14pptl.LJ14_TYPE=" + lj14Type );
// 		rs.next();
// 		String epsilon = rs.getString("EPSILON");
// 		String sigma   = rs.getString("SIGMA");
    
    
    out.write("{ " + epsilon14  + " , " + rmin14 + " }");
    if(i != siteTypeList.size()-1)
        out.write(",\n");

      }

      out.write("};\n\n");
      out.write( "const int MSD::LJ14PairTableSize = " + siteTypeList.size() + ";\n\n");
  
  } else {
      out.write( "const " + ljStructName +" MSD::LJ14PairTable[] = \n");
      out.write( "{ \n" );
      out.write( "{ -1 , -1 }\n" );
      out.write("};\n\n");
  
      out.write( "const int MSD::LJ14PairTableSize = " +  1 + ";\n\n");
  }
    }


//     private void createLJ14LowerTriangularMatrix() throws Exception {

// 	ResultSet rs = stmt.executeQuery("SELECT COUNT(*) FROM "
// 					 + "MDSYSTEM.LJ14_TYPE WHERE sys_id="+systemId);
// 	ResultSet rs1, rs2;

// 	rs.next();
// 	int num14Types = rs.getInt(1);
  
// 	int lj14ArraySize = (num14Types*num14Types + num14Types)/2;
  
// 	lj14LowerTriangularMatrix = new int [ lj14ArraySize ];
  
// 	rs = stmt.executeQuery("SELECT PARAM_ID FROM "
// 			       + "MDSYSTEM.LJ14_PAIR_PARAMETER WHERE sys_id="+systemId);
  
// 	int lj14Type1;
// 	int lj14Type2;		
// 	while(rs.next()) {
// 	    int parameterId = rs.getInt("PARAM_ID");
// 	    rs1 = stmt1.executeQuery("SELECT COUNT(LJ14_TYPE) FROM "
// 				     + "MDSYSTEM.LJ14_PAIR_PARAMETER_TYPE_LIST WHERE sys_id=" + systemId + " AND PARAM_ID ="
// 				     + parameterId);
// 	    rs2 = stmt2.executeQuery("SELECT LJ14_TYPE FROM "
// 				     + "MDSYSTEM.LJ14_PAIR_PARAMETER_TYPE_LIST WHERE sys_id=" + systemId + " AND PARAM_ID ="
// 				     + parameterId +" ORDER BY LJ14_TYPE");
  
// 	    rs1.next();
// 	    int resCount = rs1.getInt(1);
  
// 	    if(resCount == 1) {
// 		rs2.next();
// 		lj14Type1 = rs2.getInt("lj14_type");
// 		lj14Type2 = rs2.getInt("lj14_type");
// 	    } else if ( resCount == 2) {
// 		rs2.next();
// 		lj14Type1 = rs2.getInt("lj14_type");
// 		rs2.next();
// 		lj14Type2 = rs2.getInt("lj14_type");
// 	    } else {
// 		throw new Exception("Too many lj14_types exist for param_id: " + parameterId);
// 	    }
  
// 	    int index = -1;
// 	    if(lj14Type1 <= lj14Type2)
// 		index = lj14Type2*(lj14Type2+1)/2 + lj14Type1;
// 	    else
// 		index = lj14Type1*(lj14Type1+1)/2 + lj14Type2;
  
// 	    lj14LowerTriangularMatrix[ index ] = parameterId;
  
// 	}
  
//     }

    private void dumpUdfInvocationTable(FileWriter out, int num14SitePairs ) throws Exception {
  if(udfTableSize!=0) {
      /*****************************************************************************
       *  Output the array with pointers to parameter tables
       *****************************************************************************/
      out.write( "/* \n" );
      out.write( "UdfCode, size of tupleList, siteTupleListPtr, paramTupleListPtr, CombinationRule, ToCombine\n" );
      out.write( "*/\n\n" );
      out.write( "UdfInvocation MSD::UdfInvocationTable[] = \n" );
      out.write( "{\n" );
  

      int tip3pShakeCode = udfRegistry.GetUDFCodeByName("WaterTIP3PShake");
      int spcShakeCode = udfRegistry.GetUDFCodeByName("WaterSPCShake");
  
      int extraUdfs = 0;
  
      int i=0;
      while ( i < udfTableSize ) {
    
    if(ProbspecgenDB2.doTIP3P || ProbspecgenDB2.doSPC ) {
        int curUdfCode = udfCodeArray[i];
        int waterInitCode = -1;
    
        if(curUdfCode == tip3pShakeCode ) {
      waterInitCode = udfRegistry.GetUDFCodeByName("WaterTIP3PInit");
      out.write("{ ");
      out.write( waterInitCode + " , " );
      out.write( sizeTupleArray[i] + " , " );
      out.write( "(SiteId*) " + ((String ) siteTupleArray.elementAt(i)) + ", ");
      out.write( "(void*) " +((String) paramTupleArray.elementAt(i)) + ", ");
      out.write( " MSD::NORM, ");
      out.write( " -1 },\n");

      udfInvocationTable.add( new UDFInvocation(waterInitCode, (String) siteTupleArray.elementAt(i),
                  (String) paramTupleArray.elementAt(i), udfRegistry, sizeTupleArray[i] ));
      extraUdfs++;
        }
        else if (curUdfCode == spcShakeCode) {
      waterInitCode = udfRegistry.GetUDFCodeByName("WaterSPCInit");
      out.write("{ ");
      out.write( waterInitCode + " , " );
      out.write( sizeTupleArray[i] + " , " );
      out.write( "(SiteId*) " + ((String ) siteTupleArray.elementAt(i)) + ", ");
      out.write( "(void*) " +((String) paramTupleArray.elementAt(i)) + ", ");
      out.write( " MSD::NORM, ");
      out.write( " -1 },\n");
      extraUdfs++;
      udfInvocationTable.add( new UDFInvocation(waterInitCode, (String) siteTupleArray.elementAt(i),
                  (String) paramTupleArray.elementAt(i), udfRegistry, sizeTupleArray[i] ));
            
        }
    }		
    
    out.write("{ ");
    out.write( udfCodeArray[i] + " , " );
    out.write( sizeTupleArray[i] + " , " );
    out.write( "(SiteId*) " + ((String ) siteTupleArray.elementAt(i)) + ", ");
    out.write( "(void*) " +((String) paramTupleArray.elementAt(i)) + ", ");
    out.write( " MSD::NORM, ");
    out.write( " -1 ");
    out.write( " }");
    udfInvocationTable.add( new UDFInvocation(udfCodeArray[i], (String) siteTupleArray.elementAt(i),
                (String) paramTupleArray.elementAt(i), udfRegistry, sizeTupleArray[i] ));
          
    
    if ( i != udfTableSize-1) {
        out.write(",\n");
    }
    i++;
      }	
  
    
      if(!ProbspecgenDB2.no14) {
    if ( doLJ14 || doCharge14)
        out.write(",\n");
    
    if( doCharge14 ) {
        extraUdfs++;
        int stdChargeUdfCode = udfRegistry.GetUDFCodeByName("Coulomb14");
    
        out.write("{ " + stdChargeUdfCode + " , " + num14SitePairs
            + " , (SiteId*) Pair14SiteList , (void*)Charge14PairTable, MSD::NORM, -1 }");
        udfInvocationTable.add( new UDFInvocation(stdChargeUdfCode, "Pair14SiteList",
                    "Charge14PairTable", udfRegistry, num14SitePairs ));
    
    }
    
    if( doLJ14 ) {
        extraUdfs++;
    
        if( doCharge14 )
      out.write( ",\n" );
    
        int ljUdfCode = udfRegistry.GetUDFCodeByName("LennardJonesForce");
        int lj14Combiner = -1;
    
        if (ProbspecgenDB2.charmm) {
      lj14Combiner = udfRegistry.GetUDFCodeByName("LJStdCombiner");
        }
        else if(ProbspecgenDB2.oplsaa) {
                        lj14Combiner = udfRegistry.GetUDFCodeByName("LJStdCombiner");
      //lj14Combiner = udfRegistry.GetUDFCodeByName("LJ14OPLSCombiner");
        }
        else if( ProbspecgenDB2.amber) {
                        lj14Combiner = udfRegistry.GetUDFCodeByName("LJStdCombiner");
                    }
                    else
      throw new Exception("Force Field not supported.");

        out.write("{ " + ljUdfCode + " , " + num14SitePairs
            + " , (SiteId*)Pair14SiteList , (void*)LJ14PairTable, MSD::COMB, "+ lj14Combiner+" }");
    }
      }
  
      out.write( "\n};\n\n" );
  
      out.write( "const int MSD::UdfInvocationTableSize = " + (udfTableSize+extraUdfs) + ";\n\n" );
      /*****************************************************************************/
  }
  else {
      out.write( "/*\n ");
      out.write( "UdfCode, size of tupleList, siteTupleListPtr, paramTupleListPtr\n" );
      out.write( "*/\n\n");
      out.write( "UdfInvocation MSD::UdfInvocationTable[] = \n" );
      out.write( "{\n" );
  
      out.write( "{ -1, 0, NULL, NULL, MSD::NORM, -1 }" );
  
      out.write( "\n};\n");
      out.write( "const int MSD::UdfInvocationTableSize = 0;\n\n" );
  }
  /******************************************************************************/
    }

    private int createNsqSiteTypes () throws Exception {
  
  /*************************************************************************************
   *       Here begins the quest for Nsq generality.
   **************************************************************************************/
// 	ResultSet rs = stmt.executeQuery("SELECT  CHARGE, EPSILON, RMIN, ELEMENT_SYMBOL "
// 			       + "FROM "
// 			       + "MDSYSTEM.SITE WHERE sys_id=" + systemId
// 			       + " GROUP BY CHARGE, EPSILON, RMIN, ELEMENT_SYMBOL");
  
  ResultSet rs1 = stmt1.executeQuery("SELECT COUNT( * ) "
         + "FROM "
         + "MDSYSTEM.SITE WHERE sys_id=" + systemId
         + " GROUP BY CHARGE, EPSILON, RMIN, ELEMENT_SYMBOL");
  int numSiteTypes = 0;
  while(rs1.next()) {
      numSiteTypes++;
  }
  
  rs1 = stmt1.executeQuery("SELECT COUNT(*) FROM "
         + "MDSYSTEM.SITE WHERE sys_id=" + systemId + " FOR READ ONLY WITH UR");
  rs1.next();
  int numSites = rs1.getInt(1);
  
  nsqSiteIDSiteTypeMap = new int[ numSites ];

        if (nsqSiteIDSiteTypeMap == null)
           throw new Exception ("Out of enough memory...");

        String currentWaterMethod = RunTimeParameters.getWaterModel();
        String oCharge = "NULL";
        String hCharge = "NULL";
        String oEpsilon = "NULL";
        String hEpsilon = "NULL";
        String oSigma   = "NULL";
        String hSigma   = "NULL";

        if(currentWaterMethod != null) {
           ResultSet rs2 = stmt2.executeQuery("SELECT q1, q2, H_EPSILON, O_EPSILON, H_SIGMA, O_SIGMA "
                                     +" from chem.water_parameters where water_model='" + currentWaterMethod+"' FOR READ ONLY WITH UR");
            if(!rs2.next())
                throw new Exception ("Water charges (q1,q2) are not defined for water model: " + currentWaterMethod);

            oCharge = rs2.getString("q1") + " * ChargeConversion";
            hCharge = rs2.getString("q2") + " * ChargeConversion";
            oEpsilon = rs2.getString("O_EPSILON");
            hEpsilon = rs2.getString("H_EPSILON");
            oSigma = rs2.getString("O_SIGMA");
            hSigma = rs2.getString("H_SIGMA");
        }

  
  int i=0;
        String atomicMass   = new String("0.0");

        ResultSet rs11 = stmt1.executeQuery("SELECT site_id, epsilon, rmin, charge, element_symbol FROM "
                                            + "MDSYSTEM.SITE "
                                            + "WHERE sys_id=" + systemId
                                            + " ORDER BY  epsilon, rmin, charge, element_symbol FOR READ ONLY WITH UR;");
        String currentType = "";
        String prevType    = "";
  int nsqSiteInfoIndex = -1;

  while(rs11.next()) {
            int siteId            = externalToInternalSiteIdMap[ rs11.getInt("site_id") ];
      String epsilon        = rs11.getString("epsilon").trim();
      String rmin           = rs11.getString("rmin").trim();
      String charge         = rs11.getString("charge").trim();
            String elementSymbol  = rs11.getString("element_symbol").trim();

            currentType = epsilon + ":" + rmin + ":" + elementSymbol + ":" + charge;	
            if( !currentType.equals(prevType) ) {
// 	    rs1 = stmt1.executeQuery("SELECT SITE_ID FROM "
// 				     + "MDSYSTEM.SITE "
// 				     + "WHERE sys_id=" + systemId
// 				     + " AND EPSILON='" + epsilon + "'"
// 				     + " AND RMIN='" +  rmin + "'"
// 				     + " AND ELEMENT_SYMBOL='" +  elementSymbol + "'"
// 				     + " AND CHARGE='" + charge + "' FOR READ ONLY WITH UR");
  
// 	    while(rs1.next()) {
// 		int siteId1 = rs1.getInt("SITE_ID");
// 		nsqSiteIDSiteTypeMap[ siteId1 ] = nsqSiteInfoIndex;
// 	    }	
  

                if(charge.lastIndexOf("NULL") != -1 ) {
                    if(currentWaterMethod == null)
                        throw new Exception("ERROR: There are NULLs assigned to charges in xml, but no water method defined in rtp");

                    if (elementSymbol.equals("O"))
                        charge = oCharge;
                    else if(elementSymbol.equals("H"))
                        charge = hCharge;
                    else
                        throw new Exception("Charge parameter is NULL, but the site is neither an \"H\" nor an \"O\"");
                }

                if (epsilon.lastIndexOf("NULL") != -1) {

                    if(currentWaterMethod == null)
                        throw new Exception("ERROR: There are NULLs assigned to epsilon in xml, but no water method defined in rtp");

                    if(!currentWaterMethod.equals("CHARMM_TIP3P"))
                        if (elementSymbol.equals("O"))
                            epsilon = oEpsilon;
                        else if(elementSymbol.equals("H"))
                            epsilon = hEpsilon;
                        else
                            throw new Exception("Epsilon parameter is NULL, but the site is neither an \"H\" nor an \"O\"");
                    else
                        if (elementSymbol.equals("O"))
                            epsilon = "0.15207259450172";
                        else if(elementSymbol.equals("H"))
                            epsilon = "0.0";
                        else
                            throw new Exception("Epsilon parameter is NULL, but the site is neither an \"H\" nor an \"O\"");
                }

                if ( rmin.lastIndexOf("NULL") != -1 ) {

                    if(currentWaterMethod == null)
                        throw new Exception("ERROR: There are NULLs assigned to rmin in xml, but no water method defined in rtp");

                    if(!currentWaterMethod.equals("CHARMM_TIP3P"))
                        if (elementSymbol.equals("O"))
                            rmin = oSigma + " * SciConst::SixthRootOfTwo";
                        else if(elementSymbol.equals("H"))
                            rmin = hSigma + " * SciConst::SixthRootOfTwo";
                        else
                            throw new Exception("Rmin parameter is NULL, but the site is neither an \"H\" nor an \"O\"");
                    else
                        if (elementSymbol.equals("O")) {
                            rmin = "2*1.76824595565275";
                        }
                        else if(elementSymbol.equals("H")) {
                            // rmin = hSigma;
                            rmin = "0.0";
                        }
                        else
                            throw new Exception("Rmin parameter is NULL, but the site is neither an \"H\" nor an \"O\"");
                }


                nsqSiteTypeList.add(new SiteTypeRep(atomicMass, charge, epsilon, rmin));
                nsqSiteInfoIndex++;
            }

            nsqSiteIDSiteTypeMap[ siteId ] = nsqSiteInfoIndex;
            prevType = currentType;
  }

  return numSites;
  /*****************************************************************************/
    }

    private void dumpNsqChargeParams( FileWriter out ) throws Exception {
  /******************************************************************************
   * Site Information Translation
   ******************************************************************************/
  if(ProbspecgenDB2.verbose)
      System.out.println( "Printing out the NsqSiteInformation Table...\n\n" );
  
  out.write("const " + chargeNsqParamName + " MSD::ChargeNsqParams[] = \n");
  out.write("{\n");
  
  for(int j=0; j < nsqSiteTypeList.size(); j++) {
  
      SiteTypeRep curRep = (SiteTypeRep) nsqSiteTypeList.elementAt(j);
      String charge     = curRep.getCharge();
  
      out.write("{ " + charge + " }");
  
      if(j != nsqSiteTypeList.size() - 1)
    out.write(",");
      out.write("\n");
  }
  out.write("};\n");
    }

    private void dumpNumberOfNsqSiteTypes(FileWriter out) throws Exception {
        int temp = nsqSiteTypeList.size();
  out.write("const int MSD::NumberOfNsqSiteTypes = " + temp + ";\n\n");
        ProbspecgenDB2.globalDefines.put("NUMBER_OF_NSQ_SITE_TYPES", new String(temp + ""));
    }

//     private void createLJLowerTriangularMatrix()  throws Exception {
  
// 	ResultSet rs1, rs2;
  
// 	ResultSet rs = stmt.executeQuery("SELECT COUNT(*) FROM "
// 					 + "MDSYSTEM.LJ_TYPE WHERE sys_id="+systemId + " FOR READ ONLY WITH UR");
  
// 	rs.next();
// 	int numLJNsqTypes = rs.getInt(1);
  
// 	int ljNsqArraySize = (numLJNsqTypes*numLJNsqTypes + numLJNsqTypes)/2;
  
// 	ljNsqLowerTriangularMatrix = new int [ ljNsqArraySize ];
  
// 	rs = stmt.executeQuery("SELECT PARAM_ID FROM "
// 			       + "MDSYSTEM.LJ_PAIR_PARAMETER WHERE sys_id="+systemId);
  
// 	int ljNsqType1;
// 	int ljNsqType2;		
// 	while(rs.next()) {
// 	    int parameterId = rs.getInt("PARAM_ID");
// 	    rs1 = stmt1.executeQuery("SELECT COUNT(LJ_TYPE) FROM "
// 				     + "MDSYSTEM.LJ_PAIR_PARAMETER_TYPE_LIST WHERE sys_id=" + systemId + " AND PARAM_ID ="
// 				     + parameterId);
// 	    rs2 = stmt2.executeQuery("SELECT LJ_TYPE FROM "
// 				     + "MDSYSTEM.LJ_PAIR_PARAMETER_TYPE_LIST WHERE sys_id=" + systemId + " AND PARAM_ID ="
// 				     + parameterId +" ORDER BY LJ_TYPE");
  
// 	    rs1.next();
// 	    int resCount = rs1.getInt(1);
  
// 	    if(resCount == 1) {
// 		rs2.next();
// 		ljNsqType1 = rs2.getInt("lj_type");
// 		ljNsqType2 = rs2.getInt("lj_type");
// 	    } else if ( resCount == 2) {
// 		rs2.next();
// 		ljNsqType1 = rs2.getInt("lj_type");
// 		rs2.next();
// 		ljNsqType2 = rs2.getInt("lj_type");
// 	    } else {
// 		throw new Exception("Too many lj_types exist for param_id: " + parameterId);
// 	    }
  
// 	    int index = -1;
// 	    if(ljNsqType1 <= ljNsqType2)
// 		index = ljNsqType2*(ljNsqType2+1)/2 + ljNsqType1;
// 	    else
// 		index = ljNsqType1*(ljNsqType1+1)/2 + ljNsqType2;
  
// 	    ljNsqLowerTriangularMatrix[ index ] = parameterId;
// 	}
//     }

    private void dumpLJNsqParams( FileWriter out ) throws Exception {
    
  out.write("const " + ljNsqParamName + " MSD::LJNsqParams[] = \n");
  out.write("{\n");
  
  for(int j=0; j < nsqSiteTypeList.size(); j++) {
  
      SiteTypeRep curRep = (SiteTypeRep) nsqSiteTypeList.elementAt(j);
      String    epsilon   = curRep.getEpsilon();
      String    rmin      = curRep.getRmin();
  
// 	    ResultSet rs = stmt.executeQuery("SELECT EPSILON, SIGMA FROM "
// 					     + "MDSYSTEM.LJ_PAIR_PARAMETER AS ljpp, "
// 					     +" MDSYSTEM.LJ_PAIR_PARAMETER_TYPE_LIST AS ljpptl "
// 					     + " WHERE ljpp.sys_id=" + systemId
// 					     + " AND ljpptl.sys_id=" + systemId
// 					     + " AND ljpp.PARAM_ID=ljpptl.PARAM_ID "
// 					     + " AND ljpptl.LJ_TYPE=" + ljTypeId);
// 	    rs.next();
// 	    String epsilon = rs.getString("EPSILON");
// 	    String sigma   = rs.getString("SIGMA");
  
  
      out.write("{ " + epsilon  + " , " + rmin + " }");
      if(j != nsqSiteTypeList.size()-1)
    out.write(",\n");
  }
  
  out.write("\n};\n");
    }

    private void dumpNsqSiteIDtoSiteTypeMap( FileWriter out, int numSites ) throws Exception {
  out.write("const int MSD::NsqSiteIDtoSiteTypeMap[] = \n");
  out.write("{\n");
  
  int i=0;
  while( i < numSites ) {
      out.write(String.valueOf(nsqSiteIDSiteTypeMap[i]));
      if(i != numSites-1)
    out.write(",");
      out.write("\n");
      i++;
  }
  
  out.write("};\n");
  out.write("const int MSD::NsqSiteIDtoSiteTypeMapSize = " + numSites + ";\n\n");	
  /******************************************************************************/
    }

    private void dumpTimeConversionFactor( FileWriter out ) throws Exception {
  out.write("const double MSD::TimeConversionFactorMsd = TimeConversionFactor;\n");
    }

    private void dumpExclusionList1234( FileWriter out, Vector pair14Compressed, Vector excludedCompressed ) throws Exception {
  // Vector pair1234 = new Vector();
  pair1234.addAll(pair14Compressed);
  pair1234.addAll(excludedCompressed);
  int siteTupleCount = pair1234.size();
  
  if(siteTupleCount != 0) {
  
      out.write("const SiteId MSD::ExclusionList1_2_3_4[] = \n");
      out.write("{\n");
  
      timeS = System.currentTimeMillis();
      Collections.sort(pair1234);
      timeF = System.currentTimeMillis();
      //System.out.println("Time to sort the pair1234: " + (timeF-timeS));
  
      for (int i=0; i < pair1234.size(); i++) {
    out.write(((IntegerPairRep) pair1234.elementAt(i)).getFirst() +
        " , " + ((IntegerPairRep) pair1234.elementAt(i)).getSecond());
    if (i != pair1234.size() - 1 )
        out.write(",");
    out.write("\n");
      }
  
      out.write("};\n");
      out.write("const int MSD::ExclusionList1_2_3_4Size = "
          + siteTupleCount
          + ";\n\n");
            ProbspecgenDB2.globalDefines.put("EXCLUSION_LIST1_2_3_4", new String( siteTupleCount + ""));
  }
  else {
      out.write("const SiteId MSD::ExclusionList1_2_3_4[] = \n");
      out.write("{ -1, -1 };\n");
      out.write("const int MSD::ExclusionList1_2_3_4Size = 0;\n\n");
            ProbspecgenDB2.globalDefines.put("EXCLUSION_LIST1_2_3_4", new String( 0 + ""));
  }

        int numberOfSitesInSystem = getNumberOfSitesInSystem();

        int SiteIdToFragIdMap[] = new int[ numberOfSitesInSystem ];

        // Generate the Exclusion bit table
        for (int i=0; i < mFragmentTypeList.size(); i++)
          {
            IntegerPairRep rep = (IntegerPairRep) mFragmentTypeList.elementAt(i);

            int FirstSiteId = rep.getFirst();
            int TypeIndex   = rep.getSecond();
            int Count = mNumberOfSitePerFragmentType[ TypeIndex ];

            for( int SiteId = FirstSiteId; SiteId < FirstSiteId+Count; SiteId++ )
              {
                // System.out.println("SiteId: " + SiteId );
                SiteIdToFragIdMap[ SiteId ] = i;
              }
          }

        int MaxFragDiff = -999999;

        Vector ExclusionBitTable = new Vector();

        IntegerPairRep SiteIdToExclusionBitPortionMap[] = new IntegerPairRep[ numberOfSitesInSystem ];

        int CurrentSiteId = -1;
        int PrevSiteId = -1;

        for( int i=0; i < pair1234.size(); i++ )
          {
            CurrentSiteId = ((IntegerPairRep) pair1234.elementAt(i)).getFirst();
            int SiteIdB = ((IntegerPairRep) pair1234.elementAt(i)).getSecond();

//             System.out.println("CurrentSiteId: " + CurrentSiteId );
//             System.out.println("SiteIdB: " + SiteIdB );

            int FragA = SiteIdToFragIdMap[ CurrentSiteId ];
            int FragB = SiteIdToFragIdMap[ SiteIdB ];

            int Diff = Math.abs( FragA - FragB );
            if( Diff > MaxFragDiff )
                MaxFragDiff = Diff;

//             if( CurrentSiteId != PrevSiteId )
//               {
//                 // First in a run of site ids
//                 // First is the min value
//                 SiteIdToExclusionBitPortionMap[ CurrentSiteId ].SetFirst( SiteIdB );
//                 SiteIdToExclusionBitPortionMap[ CurrentSiteId ].SetSecond( 0 );
//                 SiteIdToExclusionBitPortionMap[ CurrentSiteId ].incSecond();
//               }

//             SiteIdToExclusionBitPortionMap[ CurrentSiteId ].incSecond();


//             PrevSiteId = CurrentSiteId;
          }

        ProbspecgenDB2.globalDefines.put("MAX_FRAGID_DELTA_TO_HAVE_EXCLUSIONS", new String( MaxFragDiff + ""));
    }

    private void dumpNsqInvocationTable (FileWriter out, int siteTupleCount, Statement stmt) throws Exception {
  /******************************************************************************
   *     Write the nsqInvocationTable
   ******************************************************************************/
  out.write("const NsqUdfInvocation MSD::NsqInvocationTable[] = \n");
  out.write("{\n");
  
  
  // These are the contents of the NsqUdfInvocationRecord
  // udfcode, udfType, number_of_sites(non-zero if siteTuplePtr is not null),
  // Exclusive/Inclusive/None, SiteTupleList, ParamTupleList
  
  int nsqUdfCount = 0;
  if ( ProbspecgenDB2.doEwald || RunTimeParameters.getLongRangeMethod().equals("LEKNER") ) {
      int nsqEwaldRealSpace = udfRegistry.GetUDFCodeByName("NSQEwaldRealSpace");
      // int nsqEwaldCorrection = udfRegistry.GetUDFCodeByName("NSQEwaldCorrection");
  
      out.write("{ -1, -1, MSD::NSQ2, " + siteTupleCount
          + ", MSD::Exclusion, (SiteTuplePair *)ExclusionList1_2_3_4, (void *)ChargeNsqParams  } \n");
  
// 	    out.write("{ " + nsqEwaldCorrection + ", -1, MSD::NSQ2, " + siteTupleCount
// 		      + ", MSD::Inclusion, (SiteTuplePair *)ExclusionList1_2_3_4, (void *)ChargeNsqParams  } ");
  
      nsqUdfCount += 1;
  
  }
  else {
      int nsqChargeCode = udfRegistry.GetUDFCodeByName("NSQCoulomb");
      out.write("{ " + nsqChargeCode + ", -1, MSD::NSQ2, " + siteTupleCount
          + ", MSD::Exclusion, (SiteTuplePair *)ExclusionList1_2_3_4, (void *)ChargeNsqParams  } ");
      nsqUdfCount += 1;
  }
  
  if (!ProbspecgenDB2.ljOff) {
      out.write(",\n");

      int nsqLJCombinationCode;

      if(ProbspecgenDB2.debug)
    System.out.println(ffFamily);

      if(ffFamily.equals("CHARMM")) {
    nsqLJCombinationCode = udfRegistry.GetUDFCodeByName("LJStdCombiner");
      }
      else if (ffFamily.equals("OPLSAA")){
    nsqLJCombinationCode = udfRegistry.GetUDFCodeByName("LJStdCombiner");
      }
      else if (ffFamily.equals("AMBER")){
    nsqLJCombinationCode = udfRegistry.GetUDFCodeByName("LJStdCombiner");
      }
      else {
    throw new Exception ("Force Field is not defined");
      }
  
      int nsqLennardJonesCode = udfRegistry.GetUDFCodeByName("NSQLennardJones");

      out.write("{ " + nsqLennardJonesCode +", "+nsqLJCombinationCode+", MSD::NSQ1, " + siteTupleCount
          + ", MSD::Exclusion, (SiteTuplePair *)ExclusionList1_2_3_4,  (void *)LJNsqParams } \n");
      nsqUdfCount += 1;
  }

        if(RunTimeParameters.doRespa() &&
           ( ProbspecgenDB2.doEwald || RunTimeParameters.getLongRangeMethod().equals("LEKNER") )) {
      out.write(", { -1, -1, MSD::NSQ2, " + siteTupleCount
          + ", MSD::Exclusion, (SiteTuplePair *)ExclusionList1_2_3_4, (void *)ChargeNsqParams  } \n");
      nsqUdfCount += 1;
        }
  
  out.write("};\n");
  
  out.write("const int MSD::NsqInvocationTableSize = " + nsqUdfCount +";\n\n");
    }

    private void dumpSiteInformationTable(FileWriter out) throws Exception {
  /******************************************************************************
   * Site Information Translation
   ******************************************************************************/
  if(ProbspecgenDB2.verbose)
      System.out.println( "Printing out the SiteInformation Table...\n\n" );
  
  out.write( "/*\n" );
  out.write( "Indexed by site type  = { mass, halfInverseMass, charge, lj_type, lj_14_type };\n" );
  out.write( "*/\n\n" );
  
  
  out.write("const SiteInfo MSD::SiteInformation[] = \n");
  out.write("{\n");
  
  // This is a vector of 3 elements.
  // Element1: Is the siteTypeToChargeTypeMap
  // Element2: Is the full compressed table of the chargeTypes
  // Element3: Is the full compressed table of the charge14Types
  //	    Vector chargeContainer   = getChargeContainer(stmt1, schemaName,  numSiteTypes);
  
  
  for(int j=0; j < siteTypeList.size(); j++) {
  
      SiteTypeRep curRep = (SiteTypeRep) siteTypeList.elementAt(j);
      String atomicMass = curRep.getAtomicMass();
      String charge     = curRep.getCharge();
// 	    int    ljTypeId   = curRep.getLJType();
// 	    int    lj14TypeId = curRep.getLJ14Type();
  
      out.write("{ " + atomicMass + " , " + "1.0/(2.0 * " + atomicMass +") , "
          + charge + " }");
  
      if(j != siteTypeList.size()-1)
    out.write(",");
      out.write("\n");
  
  }
  
  out.write("};\n");
  
  out.write("const int MSD::SiteInformationSize = " + siteTypeList.size() + ";\n\n");
    }

    private void dumpSiteIDtoSiteTypeMap(FileWriter out, int numSites) throws Exception {

  /*****************************************************************************/
  out.write("const int MSD::SiteIDtoSiteTypeMap[] = \n");
  out.write("{\n");
  
  int i=0;
  while( i < numSites ) {
      out.write(String.valueOf(siteIDSiteTypeMap[i]));
      if(i != numSites-1)
    out.write(",");
      out.write("\n");
      i++;
  }
  
  out.write("};\n");
  
  out.write("const int MSD::SiteIDtoSiteTypeMapSize = " + numSites + ";\n\n");
        ProbspecgenDB2.globalDefines.put("NUMBER_OF_SITES", new String( numSites + ""));
  /*****************************************************************************/
    }

    private boolean IsSiteInWater(int SiteId) throws Exception {

        //        int externalSiteId = internalToExternalSiteIdMap [ SiteId ];

// 	ResultSet rs = stmt2.executeQuery("SELECT std.site_id FROM MDSYSTEM.SiteTupleData AS std, " +
// 					  " MDSYSTEM.SiteTupleList AS stl  WHERE stl.sys_id=" + systemId
// 					  +" AND (stl.site_tuple_desc LIKE 'Water3sites')"
// 					  + " AND std.sys_id="+ systemId
// 					  + " AND stl.site_tuple_list_id=std.site_tuple_list_id "
// 					  + " AND std.site_id=" + SiteId );
  
// 	return rs.next();
        return ( WaterProteinOtherForSiteId[ SiteId ] == WATER );
    }

    private boolean isConnectedToGroup(int siteId, Vector group, BondListAsGraph blGraph) {
        Vector adjList[] = blGraph.getAdjList();
        Vector adjListForNode = adjList[ siteId ];

        for(int i=0; i < group.size(); i++) {
            int siteIdI = ((Integer) group.elementAt(i)).intValue();
            for(int j=0;  j < adjListForNode.size(); j++) {
                // check if the intersection of group and adjListForNode != 0
                int siteIdJ = ((SiteRep) adjListForNode.elementAt(j)).siteId;
                if( siteIdI == siteIdJ ) {
                    return true;
                }
            }
        }

        return false;
    }

    private Vector fragmentProtein(int startSiteId, int numberOfSites) throws Exception {

        if(ProbspecgenDB2.debug)
            System.out.println("fragmentProtein() args: " + startSiteId + " " + numberOfSites);

        Vector result = new Vector();

        String aminoAcids[] = new String[ numberOfSites ];
        //        int externalSiteId = internalToExternalSiteIdMap [ startSiteId ];
        ResultSet rs = stmt.executeQuery("SELECT component_name, cc_instance_id from mdsystem.site "
                                         +" where sys_id=" + systemId
                                         + " and site_id >= " + startSiteId
                                         + " and site_id <" + (startSiteId+numberOfSites) + " FOR READ ONLY WITH UR");

        int i=0;
        while(rs.next()) {
            String componentName = rs.getString(1);
            String ccInstanceId  = rs.getString(2);
            aminoAcids[ i ] =  componentName +":"+ ccInstanceId ;
            i++;
        }

        String currentAminoAcid = "";
        String prevAminoAcid = "";
        Fragment curFrag = null;
        int fragmentIndex = -1;

        if(ProbspecgenDB2.debug)
            for(int j=0; j < i; j++)
                System.out.println("AminoAcid["+j+"]=" + aminoAcids[ j ] );


        for( i = 0; i < numberOfSites; i++) {
            currentAminoAcid = aminoAcids[ i ].trim();

            if(!currentAminoAcid.equals(prevAminoAcid)) {
                int firstSiteId = i + startSiteId;
                String aminoAcidName = currentAminoAcid.substring(0, currentAminoAcid.indexOf(":"));
                curFrag = new Fragment(firstSiteId, aminoAcidName );
                result.add(curFrag);
                fragmentIndex++;
            }
            else {
                ((Fragment)result.elementAt( fragmentIndex )).incNumberOfSites();
            }
            prevAminoAcid = currentAminoAcid;
        }

        return result;
    }

    private boolean IsProtein(int siteId) throws Exception {

//         if(ProbspecgenDB2.debug)
//             System.out.println("IsProtein() args: " + siteId);


//         ResultSet rs = stmt.executeQuery("select sp.class from mdsystem.site AS ms,"
//                                          + "mdsystem.molecule_instance AS mi,mdsystem.specific_molecule AS sp"
//                                          + " where sp.sys_id=" + systemId
//                                          + " and ms.sys_id=" + systemId
//                                          + " and mi.sys_id=" + systemId
//                                          + " and ms.site_id=" + siteId
//                                          + " and ms.mol_instance_id=mi.mol_instance_id "
//                                          + " and sp.mol_id=mi.mol_id");
//         if(!rs.next())
//             throw new Exception("ERROR:: System class identification does not exist for site id=" + siteId);

//         String result = rs.getString(1).trim();
//         result = result.toUpperCase();

//        return (result.equals("PROTEIN"));

        //        int externalSiteId = internalToExternalSiteIdMap [ siteId ];

//         ResultSet rs = stmt.executeQuery("select component_name from mdsystem.site "
//                                          +" where sys_id=" + systemId
//                                          + " and site_id=" + siteId);
//         if(!rs.next())
//             throw new Exception("ERROR:: System class identification does not exist for site id=" + siteId);

//         String componentName = rs.getString(1).trim();
//         return (!componentName.equals("HOH") && !componentName.equals("NAP"));
        return (( WaterProteinOtherForSiteId[ siteId ] == PROTEIN )
                || ( WaterProteinOtherForSiteId[ siteId ] == PROTEIN_H ) );
    }

    private String getFragmentString( int firstSiteId, int numberOfSitesInFragment, String aminoAcid ) throws Exception {

        String result =  "/*"+aminoAcid+"*/ { " + numberOfSitesInFragment + " , ";

        boolean isWater = false;
        int oxygenPosition = -1;

        if( numberOfSitesInFragment == 3 ) {
            if(IsSiteInWater( firstSiteId )) {
                result += " MSD::WATER, ";
                isWater = true;
//                 oxygenPosition = getOxygenPosition( firstSiteId );
//                 if(oxygenPosition == -1)
//                     throw new Exception ("ERROR: Water detected, but oxygen position could not be determined");
            } else
                result += " MSD::OTHER, ";
        }
        else if(!aminoAcid.equals("NONE"))
            result += " MSD::AMINOACID, ";
        else
            result += " MSD::OTHER, ";

        // Oxygen site Id comes next
        if(isWater)
            result += String.valueOf( OxygenPositionInWater );
        else if ( !aminoAcid.equals("NONE") )
            result += getTagAtomIndexForAminoAcid( aminoAcid );
        else
            result += "0";

        /****
         * Number of constraints per fragment
         **/
        result += " , " + getNumberOfConstraints( firstSiteId, numberOfSitesInFragment );
        /******/

        // String currentWaterMethod = RunTimeParameters.getCurrentWaterMethod();

        if(isWater) {
            if( CurrentWaterMethod == null )
                throw new Exception("ERROR: Water is detected, but water method is not set in RTP.");

            // Suits: This is a bogus heuristic but it should work. Positions should be monitored
            // by analysis. Run haulted if they exceed.

            if(CurrentWaterMethod.equals("TIP3P") || CurrentWaterMethod.equals("CHARMM_TIP3P"))
                result += ", 1.1 * 0.9572 ";
            else if(CurrentWaterMethod.equals("SPC") || CurrentWaterMethod.equals("SPCE"))
                result += ", 1.1 * 1.0000 ";
            else if (CurrentWaterMethod.equals("TIP3PFLOPPY") || CurrentWaterMethod.equals("CHARMM_TIP3PFLOPPY"))
                result += ", 1.5 * 0.9572 ";
            else if(CurrentWaterMethod.equals("SPCFLOPPY") || CurrentWaterMethod.equals("SPCEFLOPPY"))
                result += ", 1.5 * 1.0000 ";
            else
                throw new Exception ("Internal Error: Add extent distance for: " + CurrentWaterMethod);
        }
        else if(!aminoAcid.equals("NONE")) {
            String maxExtent = getMaxExtentForAminoAcid( aminoAcid );
            result += ", " + maxExtent + " ";
        }
        else {
            if( numberOfSitesInFragment == 1 )
                result += ", 0.0 ";
            else {
                if ( ProbspecgenDB2.fragment_mode == ProbspecgenDB2.RIGID_GROUP_FRAGMENTATION ) {
                    // System.out.println("WARNING:: Using RIGID_GROUP extents 3.0 (default)!!! Make sure this is what you want!!!");
                    result += ", 3.0 ";
                }
                else
                    result += ", 99999999.0 ";
            }
        }

//         String centerOfMassMass = getCenterOfMass( firstSiteId,
//                                                    numberOfSitesInFragment );

//         result += centerOfMassMass;

        result += "}";

        return result;
    }

    private int getNumberOfConstraints(int start, int count) throws Exception {

        int numWaters = 0;
        int numConst  = 0;

        if( RunTimeParameters.doShakeRattleWater() ) {

            //             ResultSet rs = stmt.executeQuery("select count(*) from mdsystem.site where sys_id="+systemId
            //                                              + " and component_name='HOH' "
            //                                              + " and site_id >= " + start
            //                                              + " and site_id < " + ( start + count ));

            int cnt=0;
            for( int sId = start; sId < start+count; sId++ ) {
                if( WaterProteinOtherForSiteId[ sId ] == WATER )
                    cnt++;
            }

            //            if(rs.next()) {
            numWaters = (int) ( cnt / 3 );
            numConst += numWaters * 3;
            //             }
            //             else
            //                 throw new Exception("ERROR:: number of constraints could not be determined");
        }

        if( RunTimeParameters.doShakeRattleProtein() ) {
            // Get the number of non-water hydrogens
//             ResultSet rs = stmt.executeQuery("select count(*) from mdsystem.site where sys_id=" + systemId
//                                    + " and element_symbol='H' and component_name !='HOH'  "
//                                    + " and site_id >= " + start
//                                    + " and site_id < " + ( start + count ));

//             if(rs.next()) {
            //                int numH = rs.getInt(1);

            int cnt=0;
            for( int sId = start; sId < start+count; sId++ ) {
                if( WaterProteinOtherForSiteId[ sId ] == PROTEIN_H )
                    cnt++;
            }

            numConst += cnt;
//             }
//             else
//                 throw new Exception("ERROR:: number of constraints could not be determined");
        }

        return numConst;
    }


    private String getTagAtomIndexForAminoAcid( String aminoAcid ) throws Exception {

        String index = (String) aminoAcidAtomTagIndex.get( aminoAcid );
        if(index == null) {
            ResultSet rs = stmt.executeQuery("SELECT atom_tag from chem.extent_table"
                                             + " where component_name='"+ aminoAcid +"' FOR READ ONLY WITH UR");
            if( !rs.next() )
                throw new Exception( "ERROR:: No tag atom for amino acid: " + aminoAcid );

            String tagAtom = rs.getString(1).trim();

            rs = stmt.executeQuery("SELECT atom_tag from mdsystem.site "
                                   + " where sys_id="+ systemId
                                   + " and component_name='"+aminoAcid+"' FOR READ ONLY WITH UR");

            int i=0;
            while( rs.next() ) {
                String atomTag = rs.getString(1).trim();
                if(atomTag.equals(tagAtom)) {
                    String returnIndex = String.valueOf(i);
                    aminoAcidAtomTagIndex.put( aminoAcid, returnIndex );
                    return returnIndex;
                }
                i++;
            }

            throw new Exception("ERROR:: No tag atom index found for amino acid: " + aminoAcid);
        }
        else
            return index;
    }

    private String getMaxExtentForAminoAcid( String aminoAcid ) throws Exception {
        String maxExtent = (String) aminoAcidMaximumExtent.get( aminoAcid );

        if( maxExtent == null ) {
            ResultSet rs = stmt.executeQuery("SELECT max_extent from chem.extent_table "
                                             +" where component_name='"+aminoAcid+"' FOR READ ONLY WITH UR");
            if(!rs.next())
                throw new Exception(" ERROR:: No max extent found for amino acid: " + aminoAcid );

            String toReturn = rs.getString(1).trim();
            aminoAcidMaximumExtent.put( aminoAcid, toReturn );
            return toReturn;
        }
        else
            return maxExtent;
    }

    private String getCenterOfMass(int startSiteId, int numberOfSites) {

        int[] activeSiteTypes = new int [ siteTypeList.size() ];
        for(int i=0; i<siteTypeList.size(); i++)
            activeSiteTypes[ i ] = 0;

        for(int i=0; i < numberOfSites; i++) {
            int siteType  = siteIDSiteTypeMap[ startSiteId+i ];
            activeSiteTypes[ siteType ]++;
        }

        String result = " ( ";
        boolean firstElement = true;
        for(int siteType=0; siteType<siteTypeList.size(); siteType++) {

            if( activeSiteTypes[ siteType ] != 0 ) {

                SiteTypeRep stRep = (SiteTypeRep) siteTypeList.elementAt( siteType );

                String mass = stRep.getAtomicMass();
                if(firstElement) {
                    result += (" (" + activeSiteTypes[ siteType ] + "*" + mass +")");
                    firstElement = false;
                }
                else
                    result += (" + (" + activeSiteTypes[ siteType ] + "*" + mass +")");
            }
        }

        result += " )";
        return result;
    }

    private int getOxygenPosition( int position ) throws Exception {

        //        int externalSiteId = internalToExternalSiteIdMap [ position ];

        int i=0;
        while (i < 3) {
            ResultSet rs = stmt.executeQuery("SELECT atom_tag FROM MDSYSTEM.SITE "
                                             + " WHERE sys_id="+systemId
                                             + " AND site_id="+(position+i) + " FOR READ ONLY WITH UR");

            if(!rs.next())
                throw new Exception ("ERROR: System Id: " + (position+i) + " not found.");

            String atomTag = rs.getString("atom_tag");

            if(atomTag.startsWith("O"))
                return i;

            i++;
        }

        return -1;
    }


    private void checkSystemIntegrity() throws Exception {

        /*****************************************************
         * Check that the number of site tuples is equal to
         * the number of param tuple
         ****************************************************/
        ResultSet rs = stmt.executeQuery("SELECT * from mdsystem.udf_invocation_table "
                                         + " WHERE sys_id=" + systemId + " FOR READ ONLY WITH UR");

        while(rs.next()) {
            int siteTupleList = rs.getInt("site_tuple_list_id");
            int paramTupleList = rs.getInt("param_tuple_list_id");

            if( paramTupleList != -1 ) {
                ResultSet rs1 = stmt1.executeQuery("SELECT COUNT(*) from mdsystem.sitetuple "
                                                  + " WHERE sys_id=" + systemId
                                                  + " AND site_tuple_list_id=" + siteTupleList + " FOR READ ONLY WITH UR");

                ResultSet rs2 = stmt2.executeQuery("SELECT COUNT(*) from mdsystem.paramtuple "
                                                  + " WHERE sys_id=" + systemId
                                                  + " AND param_tuple_list_id=" + paramTupleList + " FOR READ ONLY WITH UR");


                if (rs1.next() && rs2.next()) {
                    int numberOfSiteTuples = rs1.getInt(1);
                    int numberOfParamTuples = rs2.getInt(1);

                    if( numberOfSiteTuples != numberOfParamTuples ) {
                        rs1 = stmt1.executeQuery("SELECT site_tuple_desc  from mdsystem.sitetuplelist "
                                                           + " WHERE sys_id=" + systemId
                                                           + " AND site_tuple_list_id=" + siteTupleList + " FOR READ ONLY WITH UR");

                        rs2 = stmt2.executeQuery("SELECT param_tuple_desc from mdsystem.paramtuplelist "
                                                           + " WHERE sys_id=" + systemId
                                                           + " AND param_tuple_list_id=" + paramTupleList + " FOR READ ONLY WITH UR");

                        if (rs1.next() && rs2.next()) {
                            String siteTupleDesc = rs1.getString(1);
                            String paramTupleDesc = rs2.getString(1);
                            throw new Exception ("ERROR: Site tuple list: "
                                                 + siteTupleDesc
                                                 + " and param tuple list: "
                                                 + paramTupleDesc + " are not of the same size");
                        }
                        else {
                            throw new Exception ("ERROR occured while reading data from db2");
                        }
                    }
                }


            }
        }
    }

    public boolean waterSitesPresent() throws Exception {
        ResultSet rs = stmt.executeQuery("SELECT site_tuple_list_id FROM MDSYSTEM.SiteTupleList "
                                         + "WHERE sys_id=" + systemId
           + " AND site_tuple_desc LIKE 'Water3sites' FOR READ ONLY WITH UR");

        return rs.next();
    }


    private int getfirstSiteIdOfWater() throws Exception {
        ResultSet rs = stmt.executeQuery("select std.site_id from mdsystem.sitetuplelist as stl, "
                                         + " mdsystem.sitetuple as st,  "
                                         + " mdsystem.sitetupledata as std  "
                                         + " where st.sys_id="+systemId
                                         + " and stl.sys_id="+systemId
                                         + " and std.sys_id="+systemId
                                         + " and stl.site_tuple_desc LIKE 'Water3sites'"
                                         + " and st.site_tuple_list_id=std.site_tuple_list_id"
                                         + " and stl.site_tuple_list_id=std.site_tuple_list_id"
                                         + " and std.site_tuple_id=0"
                                         + " order by std.site_id asc"
                                         + " FOR READ ONLY WITH UR");

        if(!rs.next())
            return -1;
            // throw new Exception("ERROR: number of waters is not 0, but yet none are found.");

        int firstSiteIdOfWater = rs.getInt(1);
        return firstSiteIdOfWater;
    }

    private int getNumberOfWaters() throws Exception {

  ResultSet rs = stmt.executeQuery("SELECT site_tuple_list_id FROM MDSYSTEM.SiteTupleList "
           + "WHERE sys_id=" + systemId
           + " AND site_tuple_desc LIKE 'Water3sites' FOR READ ONLY WITH UR");
  if(!rs.next())
            return 0;
      // throw new Exception("dumpWater3SiteList is called but no \"Water3sites\" defined in xml/db2");
  
  int siteTupleListId = rs.getInt(1);
  ResultSet rs2 = stmt2.executeQuery("SELECT COUNT(site_tuple_id) FROM "
         + "MDSYSTEM.SiteTuple WHERE sys_id=" + systemId
         + " AND site_tuple_list_id=" + siteTupleListId + " FOR READ ONLY WITH UR");
  rs2.next();
  int siteTupleListSize = rs2.getInt(1);

        return siteTupleListSize;
    }

//         ResultSet rs = stmt.executeQuery("SELECT count(distinct mst.site_tuple_id) "
//                                          +" FROM MDSYSTEM.SiteTupleList as mstl, MDSYSTEM.SiteTuple as mst "
//                                          + "WHERE mstl.sys_id=" + systemId
//                                          + " AND mst.sys_id=" + systemId
// 					 + " AND mstl.site_tuple_desc LIKE 'Water3sites'");

//         if(!rs.next())
//             throw new Exception("ERROR:: No water 3 sites were found for this system.");

//         return rs.getInt(1);
//     }

    private int getNumberOfSitesInSystem() throws Exception {
        ResultSet rs = stmt.executeQuery("select count(*) from mdsystem.site where sys_id="+systemId + " FOR READ ONLY WITH UR");
        rs.next();
        int count = rs.getInt(1);
        return count;
    }


    public void dumpCpp(int sys_Id, int sourceId, String magicNumber,  FileWriter out) throws Exception {
  
  try {
            systemId = sys_Id;

      stmt  = con.createStatement();
      stmt1  = con.createStatement();
      stmt2  = con.createStatement();
      stmt3  = con.createStatement();
  
      ResultSet rs = null;
      ResultSet rs1 = null;
      ResultSet rs2 = null;
      ResultSet rs3 = null;

            //            if(ProbspecgenDB2.checkSystemIntegrity)
            checkSystemIntegrity();

            rs = stmt.executeQuery("SELECT param_value FROM MDSYSTEM.GLOBAL_PARAMETER WHERE sys_id=" +systemId
                              + " AND param_id LIKE 'ff_family' FOR READ ONLY WITH UR");

            if(!rs.next())
                throw new Exception ("Force field is not set.");
            ffFamily = rs.getString(1);

      out.write("#ifndef __MSD_HPP__\n");
      out.write("#define __MSD_HPP__\n");

            out.write("#ifndef EXECUTABLEID \n");
            out.write("#define EXECUTABLEID -1\n");
            out.write("#endif \n");

//             java.util.Date now = new java.util.Date();
//             out.write("//File generated on: " + now +"\n");
//             out.write("//%database_name:" + databaseName + "\n");
//             out.write("//%source_id:" + sourceId + "\n");
//             out.write("//%system_id:" + systemId + "\n");
//             out.write("//%ctp_id:" + ctpId + "\n");
//             out.write("//%probspec_version:v1.0.0\n");
//             out.write("//%probspec_tag:RTP file is comming in from db2\n");
//             out.write("//%magic_number:"+magicNumber+"\n");

            out.write("#ifndef MSD_COMPILE_DATA_ONLY\n");
            out.write("#include <pk/platform.hpp>\n");
            out.write("#include <pk/fxlogger.hpp>\n");
            out.write("#include <pk/pktrace.hpp>  // Needed by the pk tracing instrumentation\n");
            out.write("#endif\n");

// 	    out.write("#include <pk/platform.hpp>\n");
// 	    out.write("#include <pk/fxlogger.hpp>\n");
// 	    out.write("\n");
            out.write("#include <BlueMatter/pktrace_control.hpp>\n");
      out.write("#include <BlueMatter/XYZ.hpp>\n");
      out.write("#include <BlueMatter/BoundingBox.hpp>\n");
      out.write("\n");
  

//             out.write("#include <BlueMatter/RunTimeGlobals.hpp>\n");
// 	    out.write("RunTimeGlobals RTG;\n");
// 	    out.write("\n");
  
            out.write("#ifndef assert\n");
            out.write("#include <assert.h>\n");
            out.write("#endif\n");

            out.write("#ifndef MSD_COMPILE_DATA_ONLY\n");
            out.write("#include <BlueMatter/RunTimeGlobals.hpp>\n");
            ////////////////            out.write("RunTimeGlobals RTG;\n");
            out.write("#endif\n");

            out.write("#include <BlueMatter/UDF_Binding.hpp>\n");
      out.write("#include <BlueMatter/MSD_Prefix.hpp>\n");


            CurrentWaterMethod = RunTimeParameters.getCurrentWaterMethod();

            int firstSiteIdOfWater = getfirstSiteIdOfWater();

            if( firstSiteIdOfWater != -1 )
                OxygenPositionInWater = getOxygenPosition ( firstSiteIdOfWater );

            int nSites = getNumberOfSitesInSystem();

            // NOTE:: This array is indexed by external site id
            WaterProteinOtherForSiteId = new char[ nSites ];
            ResultSet rs11 = stmt.executeQuery("select site_id, component_name, element_symbol from mdsystem.site "
                                             +" where sys_id=" + systemId + " order by site_id FOR READ ONLY WITH UR");
            // + " and site_id=" + siteId);
            // int siteId=0;
            while( rs11.next() )
              {
                  String componentName = rs11.getString( 2 ).trim();
                  String elemSymbol = rs11.getString( 3 ).trim();
                  int siteId = rs11.getInt( 1 );

                  if(  componentName.equals("HOH") || componentName.equals("OHH") || componentName.equals("HHO")  )
                      WaterProteinOtherForSiteId[ siteId ] = WATER;
                  else if ( componentName.equals("NAP") )
                      WaterProteinOtherForSiteId[ siteId ] = OTHER;
                  else if( elemSymbol.equals("H")) {
                      // Protein H is needed in the RIGID GROUP constraint opt
                      WaterProteinOtherForSiteId[ siteId ] = PROTEIN_H;
                  }
                  else
                      WaterProteinOtherForSiteId[ siteId ] = PROTEIN;
              }


      String defines = RunTimeParameters.getRTPDefinesAsString();
      out.write(defines);


      rs = stmt.executeQuery("SELECT param_value FROM MDSYSTEM.GLOBAL_PARAMETER WHERE sys_id=" + systemId
           + " AND param_id LIKE 'ff_family' FOR READ ONLY WITH UR");

      rs.next();
      String paramVal = rs.getString(1);
      if(paramVal.equals("CHARMM"))
    ProbspecgenDB2.charmm = true;
      else if(paramVal.equals("OPLSAA"))
    ProbspecgenDB2.oplsaa = true;
      else if(paramVal.equals("AMBER"))
    ProbspecgenDB2.amber = true;
      else
    throw new Exception("Force Field " + paramVal + " not supported yet.");
  
  
      dumpSymbolicConstants(out, systemId);

// 	    dumpStructureByUDFName( out, "LennardJonesForce" );
// 	    dumpStructureByUDFName( out, "StdChargeForce" );

// 	    dumpAllParticipatingStructures( out, stmt);
  
      dumpMSDClass(out);

            out.write("#ifndef MSD_COMPILE_CODE_ONLY\n");

      /*********************************************************************************/
      timeS = System.currentTimeMillis();
      BondListAsGraph blGraph = new BondListAsGraph(con, systemId);
      timeF = System.currentTimeMillis();
      //System.out.println("Time to make the BondListAsGraph: " + (timeF-timeS));	    	   	
      /*********************************************************************************/

            /****
             *  These are the fragments with contiguous sites. The sites are made cotiguous using
             *  a map.
             **/
            if(RunTimeParameters.doShakeRattleProtein() ||
               (ProbspecgenDB2.fragment_mode == ProbspecgenDB2.RIGID_GROUP_FRAGMENTATION )) {
                createRigidSitesParamsState( blGraph, out );
            }

            createIrreduciblePartitionTable( blGraph );	
      dumpIrreduciblePartition( out, blGraph );
            /*********************************************************************************/

            if(RunTimeParameters.doShakeRattleProtein() ||
               (ProbspecgenDB2.fragment_mode == ProbspecgenDB2.RIGID_GROUP_FRAGMENTATION )) {
                dumpRigidSitesParamsState(out);
            }

            // FROM HERE ON ALL THE DB2 site tables need to be using the MAP created above
            // WE ARE USING THE INTERNAL SITES FROM HERE ON
            blGraph.mapSites( externalToInternalSiteIdMap );

            // int siteid_q = 24970;
            // System.out.println( "internalToExternalSiteIdMap[ " + siteid_q + " ]: " + internalToExternalSiteIdMap[ siteid_q ] );

            timeS = System.currentTimeMillis();
      GraphSeparation gs = new GraphSeparation(blGraph, 3);
      timeF = System.currentTimeMillis();
            graphSepList = gs.getGraphSepList();

      if(ProbspecgenDB2.debug)
                System.out.println("Time to make the GraphSeparation: " + (timeF-timeS));

            dumpBondedSitesParams( out );

      dumpTimeConversionFactor( out );	

      out.write("\n");
      RunTimeParameters.dumpRTPParamsInitiation( out, udfRegistry, ProbspecgenDB2.globalDefines );
      out.write("\n");

      int numSites = createSiteInformation();
  
      dumpSiteInformationTable(out);

      getPair14ExcludedLists();

      Vector excludedCompressed = compressPairList( excludedList );
      Vector pair14Compressed   = compressPairList( pair14List );
        
      if( !ProbspecgenDB2.no14 ) {

                dumpPair14SiteList( out, pair14Compressed );
                dumpCharge14Params( out, pair14Compressed );
    dumpLJ14Params( out, pair14Compressed );
      }

      int num14SitePairs = pair14Compressed.size();	    	    	
      dumpUdfInvocationTable( out, num14SitePairs );

      /**
       *  Start NSQ Generation
       **/
      int numNsqSites = createNsqSiteTypes();

      dumpNsqChargeParams(out);
  
      dumpNumberOfNsqSiteTypes(out);

      dumpLJNsqParams(out);
  
      dumpNsqSiteIDtoSiteTypeMap( out, numNsqSites );
  
      dumpExclusionList1234(out, pair14Compressed, excludedCompressed);

      int siteTupleCount = pair14Compressed.size() + excludedCompressed.size();
  
      dumpNsqInvocationTable( out, siteTupleCount, stmt );
  
      dumpSiteIDtoSiteTypeMap( out, numSites);

      if(RunTimeParameters.getWaterModel() != null ) {

                if(waterSitesPresent())
                    dumpWater3SiteList(out);
                else
                    System.out.println("WARNING: WaterModel defined in RTP, but no water defined in db2/xml");
      }
  
      out.write("\n\n");

            if(RunTimeParameters.doShakeRattleWater()) {
                int numberOfWaters = getNumberOfWaters();

                if(ProbspecgenDB2.debug)
                    System.out.println("NumberOfConstraints (before): " + numberOfConstraints);

                numberOfConstraints += 3*numberOfWaters;

                if(ProbspecgenDB2.debug) {
                    System.out.println( "NumberOfConstraints (after): " + numberOfConstraints );
                    System.out.println( "numberOfWaters: " + numberOfWaters );
                }
            }

            if( RunTimeParameters.DoAndersonTempControl() )
                numberOfConstraints -= 3;

            out.write( "const int MSD::NumberOfConstraints =" + numberOfConstraints + ";\n\n" );

            out.write( "const int MSD::magicNumber = "+magicNumber+";\n\n" );
            out.write( "const int MSD::sourceId = "+sourceId+";\n\n" );

      out.write("MSD MolecularSystemDef;\n\n");

            out.write("#endif // MSD_COMPILE_CODE_ONLY\n\n");

            out.write("// This is the common area, defines go here...\n");
            Enumeration keys = ProbspecgenDB2.globalDefines.keys();
            while( keys.hasMoreElements() ) {
                String key = (String) keys.nextElement();

                String val =  (String) ProbspecgenDB2.globalDefines.get(key);

                out.write("#define " + key + " " + val + "\n");
            }

            out.write("\n#ifndef MSD_COMPILE_DATA_ONLY\n\n");

            out.write("extern MSD MolecularSystemDef;\n\n");
            out.write("struct RTG_PARAMETERS\n");
            out.write("{\n");
            out.write("  enum { NUM_OF_SHELLS               = NUMBER_OF_SHELLS }; \n");
            out.write("  enum { NUM_OF_THERMOSTATS          = 2 }; \n");
            out.write("  enum { NUM_OF_FRAGMENTS            = NUMBER_OF_FRAGMENTS }; \n");
            if(RunTimeParameters.doNoseHoover())
                out.write("  enum { NUM_OF_SITES_PER_THERMOSTAT = NUMBER_OF_SITES_PER_THERMOSTAT }; \n");
            else
                out.write("  enum { NUM_OF_SITES_PER_THERMOSTAT = 1 }; \n");
            out.write("};\n\n");
            out.write("extern RunTimeGlobals< RTG_PARAMETERS > RTG;\n");
            out.write("#if !defined(LEAVE_SYMS_EXTERN)\n");
            out.write("RunTimeGlobals< RTG_PARAMETERS > RTG;\n");
            out.write("#endif\n\n");

//                 out.write("/****************************************************************************\n");
//                 out.write(" *  Payload logic \n");
//                 out.write(" * \n");
//                 out.write(" * The confusion is due to our commitment to support both phase 1 and phase 2 \n");
//                 out.write(" ****************************************************************************/ \n");
//                 //out.write("#include <math.h>\n");
//                 out.write("#include <pk/pkmath.h>\n\n");
//
//                 out.write("#define EXPECTED_PAYLOAD_SUM ( 0xffffffff >> 1 ) // max unsigned int divided by 2 \n");
//     //          out.write("#define UDF_PAYLOAD          ( rint( EXPECTED_PAYLOAD_SUM / Platform::Topology::GetAddressSpaceCount() ) )\n");
//     //          out.write("#define EXPECTED_PAYLOAD_SUM ( 0x40000000 >> 1 ) // max unsigned int divided by 2 \n");
//                 out.write("#define UDF_PAYLOAD          (  EXPECTED_PAYLOAD_SUM / Platform::Topology::GetAddressSpaceCount() )\n");
//
//
//                 out.write("#define EmitEnergyDistributed( TimeStep, UdfCode, Energy ) \\ \n");
//                 out.write("  ED_Emit_UDFd1( TimeStep, UdfCode, UDF_PAYLOAD,  Energy) \n");
//
//                 out.write("#define EmitEnergyUni(TimeStep, UdfCode, Energy) ED_Emit_UDFd1( TimeStep, UdfCode, EXPECTED_PAYLOAD_SUM,  Energy)\n");
//
//                 out.write("#ifdef RDG_TO_DEVNULL\n");
//                 out.write("#define EmitEnergy(TimeStep, UdfCode, Energy, UniContext) {}\n");
//                 out.write("#else\n");
//                 out.write("#ifdef PK_PARALLEL\n");
//
//                 out.write("#define EmitEnergy(TimeStep, UdfCode, Energy, UniContext) \\ \n");
//                 out.write("{ \\ \n");
//                 out.write("if( UniContext == 1 ) \\ \n");
//                 out.write("  EmitEnergyUni( TimeStep, UdfCode, Energy ); \\ \n");
//                 out.write("else \\ \n");
//                 //out.write("  EmitEnergyDistributed( TimeStep, UdfCode, Energy ); \\ \n");
//
//                 out.write("  { \\ \n");
//                 if( PltManager.IsSpiBuild() )
//                   {
//                       out.write("   EmitEnergyDistributed( TimeStep, UdfCode, Energy ); \\  \n");
//                   }
//                 else
//                   {
//                       out.write("  double TempEnergy = 0.0; \\ \n");
//                       out.write("  double LocalEnergy = Energy; \\ \n");
//                       out.write("  MPI_Allreduce( &LocalEnergy, &TempEnergy, 1, MPI_DOUBLE, MPI_SUM, Platform::Topology::cart_comm ); \\ \n");
//                       out.write("  if( Platform::Topology::GetAddressSpaceId() == 0 ) \\ \n");
//                       out.write("   {                                                       \\  \n");
//                       out.write("   EmitEnergyUni( TimeStep, UdfCode, TempEnergy ); \\  \n");
//                       out.write("   } \\ \n");
//                   }
//                 out.write("  } \\  \n");
//
//
//                 out.write("} \n");
//
//                 // out.write("#endif \n");
//
//                 out.write("#else\n");
//
//                 out.write("#define EmitEnergy(TimeStep, UdfCode, Energy, UniContext) EmitEnergyUni( TimeStep, UdfCode, Energy )\n");
//
//                 out.write("#endif\n");
//                 out.write("#endif\n");
//                 out.write("/**********************************************************************/\n");
//
//                 out.write("#if defined( NO_TIME_STAMPS )\n");
//                 out.write("#define EmitTimeStamp(TimeStep, TypeTag, ToEmit)   {}     \n");
//                 out.write("#else \n");
//                 out.write("#define EmitTimeStamp(TimeStep, TypeTag, ToEmit)           \\ \n");
//                 out.write(" if( ToEmit )                                              \\ \n");
//                 out.write(" if( Platform::Topology::GetAddressSpaceId() == 0 ) \\ \n");
//                 out.write("   {                                                       \\ \n");
//                 out.write("   TimeValue TimeNow = Platform::Clock::GetTimeValue();    \\ \n");
//                 out.write("   ED_Emit_ControlTimeStamp(TimeStep, TypeTag, Platform::Topology::GetAddressSpaceId(), \\ \n");
//                 out.write("                            TimeNow.Seconds, TimeNow.Nanoseconds );                     \\ \n");
//                 out.write("   } \n");
//                 out.write("#endif \n");


      out.write("#include <BlueMatter/BMM_RDG_EmitWrappers.hpp>\n\n");

      out.write("#include <BlueMatter/UDF_BindingWrapper.hpp>\n\n");

            String parallelPhase = ImplManager.getParallelPhase();
            if( parallelPhase.equals("phase4") )
                out.write("#include <BlueMatter/DynamicVariableManager_phase4.hpp>\n\n");
            else
                out.write("#include <BlueMatter/DynamicVariableManager.hpp>\n\n");


            out.write("typedef TDynamicVariableManager< NUMBER_OF_SITES,\n");
            out.write("                                 NUMBER_OF_FRAGMENTS,\n");
            out.write("                                 NUMBER_OF_RESPA_LEVELS\n");
            out.write("                               > DynamicVariableManager;\n");

            out.write("extern DynamicVariableManager DynVarMgrIF;\n");
            out.write("#if !defined(LEAVE_SYMS_EXTERN)\n");
            out.write("DynamicVariableManager DynVarMgrIF;\n");
            out.write("#endif\n\n");

            if( parallelPhase.equals("phase4") )
                {
                    out.write("#include <BlueMatter/ORBManagerIF.hpp>\n\n");

                    out.write("typedef T_ORBManagerIF<DynamicVariableManager> T_ORB_Manager_IF;\n\n");

                    out.write("extern T_ORB_Manager_IF ORBManagerIF ;\n");
                  out.write("#if !defined(LEAVE_SYMS_EXTERN)\n");
                    out.write("T_ORB_Manager_IF ORBManagerIF ;\n");
                    out.write("#endif\n\n");
                }
            else
                {
                    out.write( "template<class dummy> class T_ORBManagerIF {};\n" );
                    out.write("typedef T_ORBManagerIF<DynamicVariableManager> T_ORB_Manager_IF;\n\n");
                    out.write("extern T_ORB_Manager_IF ORBManagerIF ;\n");
                  out.write("#if !defined(LEAVE_SYMS_EXTERN)\n");
                    out.write("T_ORB_Manager_IF ORBManagerIF ;\n");
                    out.write("#endif\n\n");
                }

            out.write("#include <BlueMatter/RandomNumberGenerator.hpp>\n\n");
            out.write("extern RandomNumberGenerator RanNumGenIF;\n");
            out.write("#if !defined(LEAVE_SYMS_EXTERN)\n");
            out.write("RandomNumberGenerator RanNumGenIF;\n");
            out.write("#endif\n\n");

      out.write("#include <BlueMatter/MDVM_IF.hpp>\n\n");
  
      out.write("#include <BlueMatter/LocalFragmentDirectDriver.hpp>\n\n");
  
      MSD_WrapperGenerator.dump(out, udfInvocationTable, udfRegistry,
                                      doLJ14, ffFamily, pair1234, rigidSites, getNumberOfWaters() );

            out.write("#include <BlueMatter/MSD_IF.hpp>\n\n");

            out.write("#include <BlueMatter/RunTimeGlobalsInit.hpp>\n\n");

//            out.write("#include <pk/AtomicOps.hpp>\n");
//            out.write("#include <pk/ThreadCreate.hpp>\n");

            // #ifdef PK_MPI_ALL
            // #include <pk/globject.hpp>
            // // Declare the globalized type to the globject system.
            // typedef RunTimeGlobals<NUMBER_OF_SHELLS> RTGType;
            // template< class RTGType > GCM_TypeFreeBase * GCM< RTGType >::SingletonPtr = NULL;  //A98 required NULL def.
            // #endif

//            out.write("#include <pk/Barrier.hpp>\n");
            // out.write("#include <BlueMatter/random.hpp>\n");

            // out.write("#include <fstream>\n");
            out.write("using namespace std ;\n");


            out.write("#include <BlueMatter/PeriodicImage.hpp>\n\n");

            out.write("#include <BlueMatter/RandomVelocity.hpp>\n\n");

            // More globals - not long for this world!
            // NEED: find the right home for this global!!!
            // unsigned StartingTimeStep = 0; // If respa is being used, this is the outer most timestep or lowest frequency iterator
            out.write("XYZ  *StartingVelocities;\n");
            out.write("XYZ  *StartingPositions;\n");
            out.write("int NoseHooverRestart;\n");

            out.write("#include <BlueMatter/DVSFile.hpp>\n");


            // out.write("#include <pk/platform.hpp>\n");

            int longRangeForceMethod = RunTimeParameters.longRangeForceMethod;

            //#ifndef MSDDEF_NO_LRF
            if( longRangeForceMethod != RunTimeParameters.NO_LRF ) {

                int NsqControlFlag = 1;

                String nsqImpl = ImplManager.getNSQImpl();


                String ifpImpl = ImplManager.getIFPImpl();

                if( RunTimeParameters.doRespa() && ifpImpl.equals("IFPSimple"))
                    throw new Exception("ERROR:: Respa(ctp) and IFPSimple (impl) can not be specified at the same time ");

                out.write("#include <BlueMatter/" + nsqImpl + ".hpp>\n");
                out.write("#include <BlueMatter/" + ifpImpl + ".hpp>\n");

                out.write("typedef " + ifpImpl + "< NsqUdfGroup,\n");
                out.write("                         UDF_SwitchFunction,\n");
                out.write("                         UDF_SwitchFunctionForce,\n");
                out.write("                         NUMBER_OF_SHELLS,\n");
                out.write("                      " + NsqControlFlag + "\n");
                out.write("                       > IFP_TYPE; \n\n");

                // if( parallelPhase.equals("phase4") )
                    {
                        out.write("typedef NSQAnchor<\n");
                        out.write("                  DynamicVariableManager,\n");
                        out.write("                  IFP_TYPE,\n");
                        out.write("                  T_ORB_Manager_IF\n");
                        out.write("                 > PNBE_TYPE;\n\n");
                    }
//                  else
//                      {
//                          out.write("typedef NSQAnchor<\n");
//                          out.write("                  DynamicVariableManager,\n");
//                          out.write("                  IFP_TYPE,\n");
//                          out.write("                  T_ORB_Manager_IF // this is just a dummy\n");
//                          out.write("                 > PNBE_TYPE;\n\n");
//                      }

                out.write("PNBE_TYPE PNBE;\n");

                // #if MSDDEF_DoRespa
                if( RunTimeParameters.doRespa() ) {
                    // This is the NSQ driver for the last respa level (LRL)
                    // We need to execute a special udf on the last respa level (NSQEwaldRespaCorrection)


                    out.write("typedef " + ifpImpl + "< NsqUdfGroup1,\n");
                    out.write("                         UDF_SwitchFunction,\n");
                    out.write("                         UDF_SwitchFunctionForce,\n");
                    out.write("                         NUMBER_OF_SHELLS,\n");
                    out.write("                      " + NsqControlFlag + "\n");
                    out.write("                       > IFP_TYPE1; \n\n");

                    out.write("typedef NSQAnchor<\n");
                    out.write("                  DynamicVariableManager,\n");
                    out.write("                  IFP_TYPE1\n");
                    out.write("                 > PNBE_TYPE_LRL;\n\n");
                    out.write("PNBE_TYPE_LRL PNBE_LRL;\n\n");
                }
                // #endif

                // #ifdef MSDDEF_EWALD
                if( longRangeForceMethod == RunTimeParameters.EWALD ) {
                    out.write("#include <BlueMatter/Ewald.hpp>\n");
                    out.write("#include <BlueMatter/EwaldKSpaceEngine.hpp>\n");
                    out.write("EwaldKSpaceAnchor<DynamicVariableManager> KSpace_NBE;\n\n");
                }
                // #endif

                // #ifdef MSDDEF_LEKNER
                else if( longRangeForceMethod == RunTimeParameters.LEKNER ) {
                    out.write("#include <BlueMatter/Lekner.hpp>\n");
                    out.write("LeknerAnchor<DynamicVariableManager> Lekner_NBE;\n");
                }
                // #endif

                // #ifdef MSDDEF_P3ME
                else if( longRangeForceMethod == RunTimeParameters.P3ME ) {
                    out.write("#define PK_FORWARD -1\n");
                    out.write("#define PK_REVERSE  1\n");

                    String fftImpl = ImplManager.getFFTImpl();
                    String fftInclude = ImplManager.getFFTInclude();
                    String fftType    = ImplManager.getFFTType();

                    out.write("#include <BlueMatter/ArrayHelper.hpp>\n\n");

                    // Shold be FFT3D_STRUCT_ALIGNMENT , but we need both while we flush the typo out
                    out.write("#define FFT3D_STUCT_ALIGNMENT __attribute__ (( aligned( (0x01 << 6) ) ))\n");
                    out.write("#define FFT3D_STRUCT_ALIGNMENT __attribute__ (( aligned( (0x01 << 6) ) ))\n");


                    /*************************************************************
                     * These lines get generated.
                     *************************************************************/
                    RunTimeParameters.dumpFFTPlan(out);

                    // struct FFT_PLAN
                    // {
                    //   enum {
                    //     GLOBAL_SIZE_X = 63,
                    //     GLOBAL_SIZE_Y = 63,
                    //     GLOBAL_SIZE_Z = 63
                    //   };
                    // };
                    out.write("#include <"+fftInclude+".hpp>\n\n");

                    out.write("typedef " + fftType + " FFT_TYPE;\n\n");

                    out.write("typedef " + fftImpl + "< FFT_PLAN, PK_FORWARD, FFT_TYPE, " + fftType + " complex > P3ME_FFT_FWD_TO_USE FFT3D_STRUCT_ALIGNMENT;\n\n");
                    out.write("typedef " + fftImpl + "< FFT_PLAN, PK_REVERSE, FFT_TYPE, " + fftType + " complex > P3ME_FFT_REV_TO_USE FFT3D_STRUCT_ALIGNMENT;\n\n");

                    out.write("class P3ME_PLAN_IF\n");
                    out.write("{\n");
                    out.write("public:\n");
                    out.write("  enum { SerializeReverseFFTsFlag = 1 };\n");
                    out.write("typedef P3ME_FFT_FWD_TO_USE FFT_FWD_TYPE;\n");
                    out.write("typedef P3ME_FFT_REV_TO_USE FFT_REV_TYPE;\n");
                    out.write("};\n\n");
                    /*************************************************************/

                    out.write("#include <BlueMatter/P3MEKSpaceEngine.hpp>\n\n");
//                     out.write("#ifdef PK_PHASE5_SPLIT_COMMS\n");
//                     out.write("P3MEKSpaceAnchor< DynamicVariableManager, P3ME_PLAN_IF, KspaceCommManagerIF > KSpace_NBE;\n");
//                     out.write("#else \n");
//                     out.write("class KspaceDummy {};\n");
                    out.write("P3MEKSpaceAnchor< DynamicVariableManager, P3ME_PLAN_IF, KSpaceCommMIF > KSpace_NBE;\n");
//                     out.write("#endif");
                }
            }

      out.write("\n\n#include <BlueMatter/p2.hpp>\n\n");

      out.write("#endif\n");
      out.write("#endif\n");
  
      // Finito
      stmt.close();
      stmt1.close();
      stmt2.close();
            //	    out.close();
  
      if(ProbspecgenDB2.verbose)
    System.out.println( "Success.\n" );
  
  }
  catch( Exception e ) {
      e.printStackTrace();
  }
    }
}

