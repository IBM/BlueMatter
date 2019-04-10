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

public class BondListAsGraph {
    
    private Connection con = null;
    private Vector adjList[] = null; // Vector of vectors
    private int adjListSize;


    public BondListAsGraph(Connection c, int systemId) {
  con = c;
  
  try {
      Statement stmt = con.createStatement();
      Statement stmt1 = con.createStatement();
      ResultSet rs1 = null;
      ResultSet rs = stmt.executeQuery("SELECT COUNT(*) FROM "
           + "MDSYSTEM.SITE WHERE sys_id=" + systemId);
      rs.next();
      adjListSize = rs.getInt(1);


      long timeS = System.currentTimeMillis();
      adjList = new Vector[ adjListSize ];
      long timeF = System.currentTimeMillis();
      if(ProbspecgenDB2.debug)
    System.out.println("BondListAsGraph::Time to init the adjList vector: " + (timeF-timeS));


      timeS = System.currentTimeMillis();
      for (int i = 0; i < adjListSize; i++) {
    adjList [ i ] = new Vector();
      }
      timeF = System.currentTimeMillis();
      if(ProbspecgenDB2.debug)
    System.out.println("BondListAsGraph::Time to init the adjList vector: " + (timeF-timeS));

      // select site1.site_id, site2.site_id 
      //   from enkp017.bond_site as site1,
      //        enkp017.bond_site as site2
      //   where site1.bond_id=site2.bond_id
      //         and site1.site_id<>site2.site_id
      //   order by site1.site_id;

      timeS = System.currentTimeMillis();
      //System.out.println("Default Fetch Size: " + stmt.getFetchSize());
      //stmt.setFetchSize(10000000);
// 	    rs = stmt.executeQuery("SELECT SITE1.SITE_ID, SITE2.SITE_ID "
// 				   + "FROM MDSYSTEM.BOND_SITE AS SITE1, "
// 				   + "MDSYSTEM.BOND_SITE AS SITE2 "
// 				   + "WHERE SITE1.sys_id=" + systemId   
// 				   + " AND SITE2.sys_id=" + systemId
// 				   + " AND SITE1.BOND_ID=SITE2.BOND_ID AND "
// 				   + "SITE1.SITE_ID<>SITE2.SITE_ID "
// 				   + "ORDER BY SITE1.SITE_ID");
      //	    rs.setFetchSize(10000000);
// 	    rs = stmt.executeQuery("SELECT distinct mb.site_id1, site_id2 "
// 				   + "FROM MDSYSTEM.BOND "
// 				   + "WHERE sys_id=" + systemId
// 				   + " ORDER BY site_id1");

            rs = stmt.executeQuery("select distinct  mb.site_id1, ms1.element_symbol, mb.site_id2, ms2.element_symbol "
                                   +"  from mdsystem.bond as mb, "
                                   +"        mdsystem.site as ms1,"
                                   +"        mdsystem.site as ms2"
                                   +" where ms1.sys_id="+systemId+" and "
                                   +"       ms2.sys_id="+systemId+" and "
                                   +"       mb.sys_id="+systemId+" and "
                                   +"       mb.site_id1=ms1.site_id and "
                                   +"       mb.site_id2=ms2.site_id"
                                   +" order by mb.site_id1;");


      timeF = System.currentTimeMillis(); 
      if(ProbspecgenDB2.debug)
    System.out.println("BondListAsGraph::Time to invoke the SQL stmt: " + (timeF - timeS));

      timeS = System.currentTimeMillis();
      long timePrev = System.currentTimeMillis();
      long timeCurrent;
      while(rs.next()) {
    int siteId1 = rs.getInt(1);
                String elementSymbol1 = rs.getString(2);
    int siteId2 = rs.getInt(3);
                String elementSymbol2 = rs.getString(4);
    //((Vector) adjList.elementAt(siteId1)).add(new Integer (siteId2));
    adjList[ siteId1 ].add(new SiteRep( siteId2, elementSymbol2 ));
    adjList[ siteId2 ].add(new SiteRep( siteId1, elementSymbol1 ));
    //timeCurrent = System.currentTimeMillis();
    //		System.out.println("DeltaT: " + (timeCurrent - timePrev));
    //timePrev = timeCurrent;
      }
      timeF = System.currentTimeMillis();
      if(ProbspecgenDB2.debug)
    System.out.print("BondListAsGraph::Time in the while loop : " + (timeF - timeS));


//             for( int k=0; k < adjListSize; k++ )
//                 {
//                     System.out.print("BondListAsGraph:: site="+k+" { ");
//                     for( int kk=0; kk < adjList[ k ].size(); kk++ )
//                         {
//                             System.out.print( ((SiteRep)adjList[ k ].elementAt( kk )).siteId + " " );
//                         }
//                     System.out.println(" }\n");
//                 }

      stmt.close();
      stmt1.close();
  } catch ( Exception e ) {
      e.printStackTrace();
      System.exit(1);
  }
    }

    public void mapSites( int[] externalToInternalMap ) {

        for( int i=0; i<adjListSize; i++) {            
            for( int j=0; j < adjList[i].size(); j++ ) {
                SiteRep externalSiteRep = (SiteRep) adjList[ i ].elementAt( j );
                int internalSiteId = externalToInternalMap[ externalSiteRep.getSiteId() ];
                externalSiteRep.setSiteId( internalSiteId );
                adjList[i].setElementAt( externalSiteRep, j );
            }
        }        

//         int i = 0;
//         int curSiteId = 0;
//         Vector savedVectorPrev = adjList[ curSiteId ];        
//         while( i < adjListSize ) {
//             int internalSiteId = externalToInternalMap[ curSiteId ];            
//             Vector savedVectorCur = adjList[ internalSiteId ];
//             adjList[ internalSiteId ] = savedVectorPrev;
//             savedVectorPrev = savedVectorCur;
//             curSiteId = internalSiteId;
//             i++;
//         }        
        
        Vector adjListTemp[] = new Vector[ adjListSize ];
        for( int i=0; i<adjListSize; i++) {
            int internalSiteId = externalToInternalMap[ i ];
            adjListTemp[ internalSiteId ] = (Vector) (adjList[ i ].clone());
        }
        
        adjList = adjListTemp;
        
//         for( int k=0; k < adjListSize; k++ )
//             {
//                 System.out.print("BondListAsGraph::mapSites site="+k+" { ");
//                 for( int kk=0; kk < adjList[ k ].size(); kk++ )
//                     {
//                         System.out.print( ((SiteRep)adjList[ k ].elementAt( kk )).siteId + " " );
//                     }
//                 System.out.println(" }\n");
//             }

    }

    public Vector[]  getAdjList() {
  return adjList;
    }

    public int getAdjListSize(){
  return adjListSize;
    }
}

