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

import java.sql.*;
import java.text.*;
import java.io.*;

public class DropSchema {

   static {
      try {
         //  register the driver with DriverManager 
         //  The newInstance() call is needed for the sample to work with 
         //  JDK 1.1.1 on OS/2, where the Class.forName() method does not 
         //  run the static initializer. For other JDKs, the newInstance 
         //  call can be omitted. 
         Class.forName("COM.ibm.db2.jdbc.app.DB2Driver").newInstance();
      } catch (Exception e) {
         e.printStackTrace();
      }
   }

/**
*   The main program sets up all the neccesary tables for  probspecgen
*/
   public static void main(String argv[]) {

      int argc = argv.length;
      if(argc < 1 || argv[0].equals("-usage") ) {
    usage();
    return;
      }


      int j=0;
      while(j < argc) {
          System.out.println("argv[" + j + "] = " + argv[j]);
    j++;
      }      

      String schemaName = "";
      String databaseName = new String("mdsetup");
      
      boolean dropAll = false;
      
      int i=0;
      while (i < argc) {
    if( argv[i].equals("-schemaname")){
        if((i != argc-1) || argv[i+1].startsWith("-")) {
      schemaName = argv[i+1];
        }
        else {
      usage();
      return;
        }
    }
    else if( argv[i].equals("-all")){
              dropAll = true;
    }
    else if( argv[i].equals("-database")) {
        if((i != argc-1) || argv[i+1].startsWith("-")) {
      databaseName = argv[i+1];
        }
        else {
      usage();
      return;
        }
    }
    i++;
      }
      if (!dropAll && schemaName.equals("") ){
    usage();
      }
      
            //  URL is jdbc:db2:dbname 
      Connection con = null;
      String url = "jdbc:db2:" + databaseName ;

      try {
         
         //  connect with default id/password 
         con = DriverManager.getConnection(url);
   Statement stmt = con.createStatement();

         if(!dropAll) {
             String tempName = schemaName.toUpperCase();
             schemaName = "\"" + schemaName.toUpperCase() + "\"";
             
             ResultSet rs = stmt.executeQuery("SELECT SCHEMANAME FROM SYSCAT.SCHEMATA WHERE SCHEMANAME='" 
                                              + tempName + "'");
             if(!rs.next()) {
                 System.out.println("Shemaname : " + tempName + " is not found in the database");
                 return;
             }
         }
   

         if( dropAll || schemaName.equals("\"MDSYSTEM\"")) {
             try {
                 stmt.execute("DROP TABLE MDSYSTEM.System");                
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }
             
             try {
                 stmt.execute("DROP TABLE MDSYSTEM.chain");                
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }

             try {
                 stmt.execute("DROP TABLE MDSYSTEM.chem_comp_instance");                
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }

             try {
                 stmt.execute("DROP TABLE MDSYSTEM.specific_molecule");                
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }

             try {
                 stmt.execute("DROP TABLE MDSYSTEM.molecule_instance");                
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }

             try {
                 stmt.execute("DROP TABLE MDSYSTEM.monomer_instance");                
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }

             try {
                 stmt.execute("DROP TABLE MDSYSTEM.ParamTupleList");                
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }
             
             try {
                 stmt.execute("DROP TABLE MDSYSTEM.ParamTupleSpec"); 
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }
             
             try {
                 stmt.execute("DROP TABLE MDSYSTEM.ParamTuple");             
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }	 
             
             try {
                 stmt.execute("DROP TABLE MDSYSTEM.ParamTupleData");         
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }
             
             try {
                 stmt.execute("DROP TABLE MDSYSTEM.SiteTupleList");          
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }
             
             try {
                 stmt.execute("DROP TABLE MDSYSTEM.SiteTuple");              
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }	 
             
             try {
                 stmt.execute("DROP TABLE MDSYSTEM.SiteTupleData");          
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }
             
             try {
                 stmt.execute("DROP TABLE MDSYSTEM.UDF_Invocation_Table");  
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }
             
//              try {
//                  stmt.execute("DROP TABLE MDSYSTEM.LJ_Type");               
//              } catch (Exception e) {
//                 System.out.println(e.getMessage()); 
//                 }
             
//              try {
//                  stmt.execute("DROP TABLE MDSYSTEM.LJ_Pair_Parameter");     
//              } catch (Exception e) {
//                 System.out.println(e.getMessage()); 
//                 }
             
//              try {
//                  stmt.execute("DROP TABLE MDSYSTEM.LJ_Pair_Parameter_Type_List"); 
//              } catch (Exception e) {
//                 System.out.println(e.getMessage()); 
//                 }
             
//              try {
//                  stmt.execute("DROP TABLE MDSYSTEM.LJ14_Type");   
//              } catch (Exception e) {
//                 System.out.println(e.getMessage()); 
//                 }
             
//              try {
//                  stmt.execute("DROP TABLE MDSYSTEM.LJ14_Pair_Parameter");              
//              } catch (Exception e) {
//                 System.out.println(e.getMessage()); 
//                 }
             
//              try {
//                  stmt.execute("DROP TABLE MDSYSTEM.LJ14_Pair_Parameter_Type_List"); 
//              } catch (Exception e) {
//                 System.out.println(e.getMessage()); 
//                 }
             
             try {
                 stmt.execute("DROP TABLE MDSYSTEM.Site");                       
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }
             
             try {
                 stmt.execute("DROP TABLE MDSYSTEM.Bond");                       
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }

             try {
                 stmt.execute("DROP TABLE MDSYSTEM.group_site");                       
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }

             try {
                 stmt.execute("DROP TABLE MDSYSTEM.group");                       
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }

             try {
                 stmt.execute("DROP TABLE MDSYSTEM.site_param_spec");                       
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }

             try {
                 stmt.execute("DROP TABLE MDSYSTEM.site_param");                       
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }

//              try {
//                  stmt.execute("DROP TABLE MDSYSTEM.Bond_Site");                  
//              } catch (Exception e) {
//                 System.out.println(e.getMessage()); 
//                 }
             
             try {
                 stmt.execute("DROP TABLE MDSYSTEM.Global_Parameter_Type");      
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }
             
             try {
                 stmt.execute("DROP TABLE MDSYSTEM.Global_Parameter");           
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }
             
             try {
                 stmt.execute("DROP SCHEMA MDSYSTEM RESTRICT");             
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }

         }

         if( dropAll || schemaName.equals("\"REGISTRY\"")) {

             try {
                 stmt.execute("DROP TABLE Registry.UDF_Record");           
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }

             try {
                 stmt.execute("DROP TABLE Registry.UDF_Category");           
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }

             try {
                 stmt.execute("DROP TABLE Registry.UDF_Variable");           
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }

             try {
                 stmt.execute("DROP SCHEMA Registry RESTRICT");             
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }

         }

         if( dropAll || schemaName.equals("\"CHEM\"")) {
             try {
                 stmt.execute("DROP TABLE CHEM.chemical_component");           
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }

             try {
                 stmt.execute("DROP TABLE CHEM.molecule_type");           
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }

             try {
                 stmt.execute("DROP TABLE CHEM.elements");           
             } catch (Exception e) {
                System.out.println(e.getMessage()); 
                }

             try {
                 stmt.execute("DROP TABLE CHEM.atom_tag_table");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }
             
             try {
                 stmt.execute("DROP TABLE CHEM.site_param_type");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }

             try {
                 stmt.execute("DROP TABLE CHEM.water_parameters");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }

             try {
                 stmt.execute("DROP SCHEMA CHEM RESTRICT");             
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }
         }

         if( dropAll || schemaName.equals("\"EXPERIMENT\"")) {

//              try {
//                  stmt.execute("DROP TABLE EXPERIMENT.system_class");           
//              } catch (Exception e) {
//                  System.out.println(e.getMessage()); 
//              }

             try {
                 stmt.execute("DROP TABLE EXPERIMENT.ctp_table");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }
             try {
                 stmt.execute("DROP TABLE EXPERIMENT.vocab");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }
             try {
                 stmt.execute("DROP TABLE EXPERIMENT.ct_params");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }
             try {
                 stmt.execute("DROP TABLE EXPERIMENT.system_source");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }
             try {
                 stmt.execute("DROP TABLE EXPERIMENT.compiler");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }
             try {
                 stmt.execute("DROP TABLE EXPERIMENT.probspec");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }
             try {
                 stmt.execute("DROP TABLE EXPERIMENT.platform");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }
             try {
                 stmt.execute("DROP TABLE EXPERIMENT.executable");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }
             try {
                 stmt.execute("DROP TABLE EXPERIMENT.rtp_table");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }
             try {
                 stmt.execute("DROP TABLE EXPERIMENT.rt_params");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }
             try {
                 stmt.execute("DROP TABLE EXPERIMENT.snapshot");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }

             try {
                 stmt.execute("DROP TABLE EXPERIMENT.coords_table");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }

             try {
                 stmt.execute("DROP TABLE EXPERIMENT.restart_table");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }

             try {
                 stmt.execute("DROP TABLE EXPERIMENT.run");           
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }

             try {
                 stmt.execute("DROP SCHEMA EXPERIMENT RESTRICT");             
             } catch (Exception e) {
                 System.out.println(e.getMessage()); 
             }
         }

   System.out.println("Schema dropped. Success.");
   stmt.close();
   con.close();
      } catch (Exception e){
    e.printStackTrace();
      }
   }
    
    private static void usage() {
  System.out.println("Input: [flags]");
  System.out.println("flags: -schemaname <schema_name> --> drops the schema/tables for a specific system.");
  System.out.println("flags: -database <database_name> --> \"bluegene\" by default");
  System.out.println("flags: -all                      --> drop all the schemas (MDSYSTEM, REGISTRY, CHEM)");
  return;
    }
  }
