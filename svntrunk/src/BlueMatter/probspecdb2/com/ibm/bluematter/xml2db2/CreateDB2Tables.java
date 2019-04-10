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
 package com.ibm.bluematter.xml2db2;

import java.sql.*;
import java.text.*;
import java.io.*;

class CreateDB2Tables {
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
    
    public static void main(String argv[]) {
	
	int argc = argv.length;
	
	int i = 0;
	
	String databaseName = new String("mdsetup");

	while ( i < argc ) {
	     if (argv[i].equals("-database")) {
		 if((i==argc-1) || argv[i+1].startsWith("-")) {
		     usage();
		     return;
		 }
		 databaseName = argv[i+1];
	     } 
	     i++;
	}

        System.out.println("Database name: " + databaseName);

	Connection con = null;
	      String url = "jdbc:db2:" + databaseName;

      try {
         
            //  connect with default id/password 
         con = DriverManager.getConnection(url);
         
         Statement stmt = con.createStatement();
         Statement stmt1 = con.createStatement();
	 ResultSet rs = null;
	 ResultSet rs1 = null;

         /**************************************************************************
          *  Create the Experiment schema and tables
          **************************************************************************/
         /**************************************************************************
          *  Create the Registry schema and tables
          **************************************************************************/

	 stmt.execute("CREATE SCHEMA Registry ");	 
	 stmt.execute(" CREATE TABLE Registry.UDF_Record (udf_id     INT NOT NULL,"
		      + "udf_name   VARCHAR(50) NOT NULL,"
		      + "entry_date DATE,"
		      + "entry_time TIME,"
		      + "arg_check_sum INT,"
		      + "body_check_sum INT,"
		      + "site_count     INT,"
		      + "param_count    INT,"
		      + "PRIMARY KEY (udf_id))");

	 
	 stmt.execute("	CREATE TABLE Registry.UDF_Category (udf_id     INT NOT NULL,"
		      + "category_name    VARCHAR(60) NOT NULL,"
		      + "PRIMARY KEY (udf_id, category_name),"
		      + "FOREIGN KEY (udf_id) REFERENCES Registry.UDF_Record ON DELETE CASCADE);");
	 
	 stmt.execute("	CREATE TABLE Registry.UDF_Variable (udf_id     INT NOT NULL, "
		      + "var_name   VARCHAR(40) NOT NULL, "
		      + "type_desc  VARCHAR(30), "
		      + "type_size  INT, "
		      + "PRIMARY KEY (udf_id , var_name), "
		      + "FOREIGN KEY (udf_id) REFERENCES Registry.UDF_Record ON DELETE CASCADE);");


         /**************************************************************************
          *  Create the CHEM schema and tables
          **************************************************************************/
	 stmt.execute("CREATE SCHEMA CHEM");
	 
	 stmt.execute("	CREATE TABLE CHEM.chemical_component ( " 
		      + "component_name VARCHAR(60) NOT NULL, "
		      + "description VARCHAR(100) NOT NULL, "
		      + "PRIMARY KEY ( component_name ));");

	 stmt.execute("	CREATE TABLE CHEM.molecule_type ( " 		      + "class VARCHAR(60) NOT NULL, "
		      + "description VARCHAR(100) NOT NULL, "
		      + "PRIMARY KEY ( class ));");

	 stmt.execute("	CREATE TABLE CHEM.elements ( " 
		      + "atomic_number INT NOT NULL, "
		      + "element_symbol VARCHAR(60) NOT NULL, "
                      + "element_name VARCHAR(50) NOT NULL, "
                      + "atomic_mass VARCHAR(30) NOT NULL, "
		      + "PRIMARY KEY ( element_symbol ));");

	 stmt.execute("	CREATE TABLE CHEM.atom_tag_table ( " 
		      + "component_name VARCHAR(60) NOT NULL, "
		      + "atom_tag       VARCHAR(60) NOT NULL, "
		      + "PRIMARY KEY ( component_name, atom_tag ), "
                      + "FOREIGN KEY ( component_name) REFERENCES CHEM.chemical_component ON DELETE CASCADE);");

	 stmt.execute("	CREATE TABLE CHEM.site_param_type ( " 
		      + "type_id        INT NOT NULL, "
		      + "type_desc      VARCHAR(60) NOT NULL, "
		      + "PRIMARY KEY ( type_id ));");
         
	 stmt.execute("	CREATE TABLE CHEM.water_parameters ( " 
		      + "water_model    VARCHAR(60) NOT NULL, "
		      + "q1             VARCHAR(60) NOT NULL, "
		      + "q2             VARCHAR(60) NOT NULL, "
		      + "O_epsilon      VARCHAR(60) NOT NULL, "
		      + "O_sigma        VARCHAR(60) NOT NULL, "
		      + "H_epsilon      VARCHAR(60) NOT NULL, "
		      + "H_sigma        VARCHAR(60) NOT NULL, "
		      + "PRIMARY KEY ( water_model ));");
         /**************************************************************************/


         /**************************************************************************
          *  Create the MDSYSTEM schema and tables
          **************************************************************************/
	 stmt.execute("CREATE SCHEMA MDSYSTEM");

	 stmt.execute("CREATE TABLE MDSYSTEM.System ( " 
		      + "sys_id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 0), "
		      + "sys_desc VARCHAR(30) NOT NULL, " 
                      + "creator VARCHAR(40) NOT NULL, "
                      + "created TIMESTAMP NOT NULL,"
                      + "sys_class VARCHAR(60) NOT NULL,"
		      + "PRIMARY KEY ( sys_id ), "
                      + "FOREIGN KEY ( sys_class ) REFERENCES EXPERIMENT.system_class);");

         /********************************************************************************
          *  From chem.ps
          *******************************************************************************/
         stmt.execute("CREATE TABLE MDSYSTEM.chain ("
                      + "sys_id            INT NOT NULL, "
                      + "chain_id          INT NOT NULL, "
                      + "chain_desc        VARCHAR(100),"
                      + "PRIMARY KEY ( sys_id, chain_id ),"
                      + "FOREIGN KEY ( sys_id) REFERENCES MDSYSTEM.System ON DELETE CASCADE);");

         stmt.execute("CREATE TABLE MDSYSTEM.chem_comp_instance ("
                      + "sys_id            INT NOT NULL, "
                      + "cc_instance_id    INT NOT NULL, "
                      + "component_name    VARCHAR(100) NOT NULL, "
                      + "PRIMARY KEY ( sys_id, cc_instance_id ),"
                      + "FOREIGN KEY ( sys_id) REFERENCES MDSYSTEM.System ON DELETE CASCADE,"
                      + "FOREIGN KEY ( component_name) REFERENCES CHEM.chemical_component ON DELETE CASCADE);");

         stmt.execute("CREATE TABLE MDSYSTEM.specific_molecule ("
                      + "sys_id            INT NOT NULL, "
                      + "mol_id            INT NOT NULL,"
                      + "molecule_name     VARCHAR(60) NOT NULL, "
                      + "class             VARCHAR(60) NOT NULL, "
                      + "generic_name      VARCHAR(60), "
                      + "pdb_id            VARCHAR(60), "
                      + "PRIMARY KEY ( sys_id, mol_id ),"
                      + "FOREIGN KEY ( sys_id) REFERENCES MDSYSTEM.System ON DELETE CASCADE,"
                      + "FOREIGN KEY ( class ) REFERENCES CHEM.molecule_type ON DELETE CASCADE);");

         stmt.execute("CREATE TABLE MDSYSTEM.molecule_instance ("
                      + "sys_id            INT NOT NULL, "
                      + "mol_instance_id   INT NOT NULL, "
                      + "mol_id            INT NOT NULL, "
                      + "PRIMARY KEY ( sys_id, mol_instance_id ),"
                      + "FOREIGN KEY ( sys_id) REFERENCES MDSYSTEM.System ON DELETE CASCADE,"
                      + "FOREIGN KEY ( sys_id, mol_id ) REFERENCES MDSYSTEM.specific_molecule ON DELETE CASCADE);");

         stmt.execute("CREATE TABLE MDSYSTEM.monomer_instance ("
                      + "sys_id            INT NOT NULL, "
                      + "chain_id          INT NOT NULL, "
                      + "monomer_ordinal   INT NOT NULL, "
                      + "cc_instance_id    INT NOT NULL, "
                      + "PRIMARY KEY ( sys_id, chain_id, monomer_ordinal ),"
                      + "FOREIGN KEY ( sys_id ) REFERENCES MDSYSTEM.System ON DELETE CASCADE,"
                      + "FOREIGN KEY ( sys_id, chain_id ) REFERENCES MDSYSTEM.chain ON DELETE CASCADE,"
                      + "FOREIGN KEY ( sys_id, cc_instance_id ) REFERENCES MDSYSTEM.chem_comp_instance ON DELETE CASCADE);");

         /*******************************************************************************/
	 stmt.execute("CREATE TABLE MDSYSTEM.ParamTupleList ( " 
		      + "sys_id INT NOT NULL, "
		      + "param_tuple_list_id INT NOT NULL,"
		      + "param_tuple_desc VARCHAR(40),"
		      + "PRIMARY KEY ( sys_id, param_tuple_list_id ) , "
		      + "FOREIGN KEY ( sys_id ) REFERENCES MDSYSTEM.System ON DELETE CASCADE);");

   	 stmt.execute("CREATE TABLE MDSYSTEM.ParamTupleSpec ( "
		      + "sys_id INT NOT NULL, "
		      + "param_tuple_list_id INT NOT NULL," 
		      + "param_id            INT NOT NULL," 
		      + "param_type          INT," 
		      + "param_name          VARCHAR(40)," 
		      + "PRIMARY KEY ( sys_id, param_tuple_list_id, param_id ), "
		      + "FOREIGN KEY ( sys_id, param_tuple_list_id ) REFERENCES MDSYSTEM.ParamTupleList ON DELETE CASCADE);");

  	 stmt.execute("CREATE TABLE MDSYSTEM.ParamTuple ("
 		      + "sys_id INT NOT NULL, "
 		      + "param_tuple_list_id INT NOT NULL, "
 		      + "param_tuple_id      INT NOT NULL, "
 		      + "PRIMARY KEY ( sys_id, param_tuple_list_id, param_tuple_id ),"
 		      + "FOREIGN KEY ( sys_id, param_tuple_list_id ) REFERENCES MDSYSTEM.ParamTupleList ON DELETE CASCADE);");


     	 stmt.execute("CREATE TABLE MDSYSTEM.ParamTupleData ( "
 		      + "sys_id INT NOT NULL, "
 		      + "param_tuple_list_id INT NOT NULL, "
 		      + "param_tuple_id      INT NOT NULL,"
 		      + "param_id            INT NOT NULL,"
 		      + "param_value         VARCHAR(40), "
 		      + "PRIMARY KEY ( sys_id, param_tuple_list_id, param_tuple_id, param_id ), "
 		      + "FOREIGN KEY ( sys_id, param_tuple_list_id ) REFERENCES MDSYSTEM.ParamTupleList ON DELETE CASCADE);");

// // 	 stmt.execute("	CREATE TABLE UDF_Registry (udf_id INT NOT NULL, "
// // 		      + " PRIMARY KEY (udf_id));");
	 
         stmt.execute("	CREATE TABLE MDSYSTEM.SiteTupleList ( "		      
		      + "sys_id INT NOT NULL, "
		      + "site_tuple_list_id INT NOT NULL, "
		      + "site_tuple_desc VARCHAR(40),"
		      + "PRIMARY KEY (sys_id, site_tuple_list_id), "
		      + "FOREIGN KEY ( sys_id ) REFERENCES MDSYSTEM.System ON DELETE CASCADE);");

	 stmt.execute("CREATE TABLE MDSYSTEM.UDF_Invocation_Table( "
		      + " sys_id INT NOT NULL, "		      
		      + " udf_id VARCHAR(60) NOT NULL,"
		      + " site_tuple_list_id  INT NOT NULL,"
		      + " param_tuple_list_id INT,"
		      + " PRIMARY KEY (sys_id, udf_id, site_tuple_list_id),"
		      + " FOREIGN KEY ( sys_id, site_tuple_list_id ) REFERENCES MDSYSTEM.SiteTupleList  ON DELETE CASCADE,"
		      + " FOREIGN KEY ( sys_id, param_tuple_list_id )REFERENCES MDSYSTEM.ParamTupleList ON DELETE CASCADE);");
	 
	 stmt.execute("	CREATE TABLE MDSYSTEM.SiteTuple ( " 
		      + " sys_id INT NOT NULL, "
		      + " site_tuple_list_id INT NOT NULL, "
		      + " site_tuple_id      INT NOT NULL,"
		      + " PRIMARY KEY ( sys_id, site_tuple_list_id, site_tuple_id ),"
		      + " FOREIGN KEY ( sys_id, site_tuple_list_id ) REFERENCES MDSYSTEM.SiteTupleList ON DELETE CASCADE);");

	 stmt.execute(" CREATE TABLE MDSYSTEM.Site ("
		      + "sys_id INT NOT NULL, "
		      + "site_id         INT NOT NULL, "
		      + "cc_instance_id    INT,"
                      + "mol_instance_id INT,"
                      + "epsilon         VARCHAR(50),"
                      + "rmin            VARCHAR(50),"
                      + "epsilon14       VARCHAR(50),"
                      + "rmin14          VARCHAR(50),"
                      + "atomic_mass     VARCHAR(50),"
                      + "charge          VARCHAR(60),"
                      + "atom_tag        VARCHAR(60),"
                      + "component_name  VARCHAR(60),"
                      + "element_symbol  VARCHAR(10),"
                      + "PRIMARY KEY ( sys_id, site_id ),"
		      + "FOREIGN KEY ( sys_id ) REFERENCES MDSYSTEM.System ON DELETE CASCADE,"
                      + "FOREIGN KEY ( sys_id, mol_instance_id ) REFERENCES MDSYSTEM.molecule_instance ON DELETE CASCADE,"
                      + "FOREIGN KEY ( sys_id, cc_instance_id ) REFERENCES MDSYSTEM.chem_comp_instance ON DELETE CASCADE,"
                      + "FOREIGN KEY ( element_symbol ) REFERENCES CHEM.elements ON DELETE CASCADE,"
                      + "FOREIGN KEY ( component_name, atom_tag ) REFERENCES CHEM.atom_tag_table ON DELETE CASCADE)");
                      
 	 stmt.execute("	CREATE TABLE MDSYSTEM.SiteTupleData ( "
 		      + "sys_id INT NOT NULL, "
 		      + "site_tuple_list_id INT NOT NULL,"
 		      + "site_tuple_id      INT NOT NULL,"
 		      + "site_ordinal       INT NOT NULL,"
 		      + "site_id            INT NOT NULL,"
 		      + "PRIMARY KEY ( sys_id, site_tuple_list_id, site_tuple_id, site_ordinal ),"
 		      + "FOREIGN KEY ( sys_id, site_tuple_list_id ) REFERENCES MDSYSTEM.SiteTupleList ON DELETE CASCADE,"
 		      + "FOREIGN KEY ( sys_id, site_id )            REFERENCES MDSYSTEM.Site          ON DELETE CASCADE);");

         stmt.execute("CREATE TABLE MDSYSTEM.group ("
                      + "sys_id INT NOT NULL,"
                      + "group_id INT NOT NULL,"
                      + "group_type VARCHAR(60) NOT NULL,"
                      + "PRIMARY KEY ( sys_id, group_id ))");

         stmt.execute("CREATE TABLE MDSYSTEM.group_site ("
                      + "sys_id INT NOT NULL,"
                      + "group_id INT NOT NULL,"
                      + "site_id INT NOT NULL,"
                      + "PRIMARY KEY (sys_id, group_id, site_id),"
                      + "FOREIGN KEY ( sys_id, site_id )            REFERENCES MDSYSTEM.Site          ON DELETE CASCADE,"
                      + "FOREIGN KEY ( sys_id, group_id )           REFERENCES MDSYSTEM.group         ON DELETE CASCADE);");

         stmt.execute("CREATE TABLE MDSYSTEM.site_param_spec ("
                      + "sys_id       INT NOT NULL,"
                      + "param_id     INT NOT NULL,"
                      + "type_id      INT NOT NULL,"
                      + "name      VARCHAR(60) NOT NULL,"
                      + "PRIMARY KEY (sys_id, param_id ),"
                      + "FOREIGN KEY ( sys_id )            REFERENCES MDSYSTEM.System          ON DELETE CASCADE,"
                      + "FOREIGN KEY ( type_id )           REFERENCES CHEM.site_param_type     ON DELETE CASCADE)");

         stmt.execute("CREATE TABLE MDSYSTEM.site_param ("
                      + "sys_id    INT NOT NULL,"
                      + "site_id    INT NOT NULL,"
                      + "param_id    INT NOT NULL,"
                      + "value    VARCHAR(60) NOT NULL,"
                      + "PRIMARY KEY ( sys_id, site_id, param_id ),"
                      + "FOREIGN KEY ( sys_id, param_id ) REFERENCES MDSYSTEM.site_param_spec ON DELETE CASCADE,"
                      + "FOREIGN KEY ( sys_id, site_id ) REFERENCES MDSYSTEM.Site ON DELETE CASCADE)");


	 stmt.execute("	CREATE TABLE MDSYSTEM.Bond ( "
		      + "sys_id INT NOT NULL, "		      
		      + "bond_id INT NOT NULL,"
		      + "site_id1 INT NOT NULL,"
		      + "site_id2 INT NOT NULL,"
		      + "bond_type INT,"
		      + "PRIMARY KEY ( sys_id, bond_id), "
		      + "FOREIGN KEY ( sys_id ) REFERENCES MDSYSTEM.System ON DELETE CASCADE);");
	 
	 stmt.execute("	CREATE TABLE MDSYSTEM.Global_Parameter_Type ( "
		      + "sys_id INT NOT NULL, "
		      + "param_type INT NOT NULL, "
		      + "param_desc VARCHAR(60), "
		      + "PRIMARY KEY ( sys_id, param_type ), "
		      + "FOREIGN KEY ( sys_id ) REFERENCES MDSYSTEM.System ON DELETE CASCADE);");

	 stmt.execute("	CREATE TABLE MDSYSTEM.Global_Parameter ( " 
		      + "sys_id INT NOT NULL, "
		      + "param_id VARCHAR(20) NOT NULL, "
		      + "param_type INT NOT NULL, "
		      + "symbolic_constant INT, "
		      + "param_value VARCHAR(60), "
		      + "PRIMARY KEY ( sys_id, param_id  ), "
		      + "FOREIGN KEY ( sys_id, param_type ) REFERENCES MDSYSTEM.Global_Parameter_Type ON DELETE CASCADE);");


	 System.out.println("Tables created successfully.");

	 stmt.close();
	 con.close();
      } catch (Exception e) {
	  e.printStackTrace();
      }
    }
	
    private static void usage() {
	System.out.println( "Input: [optinal flags]" );
	System.out.println("flags: -database <database_name>         \"bluegene\" by default");
    }
    

}
