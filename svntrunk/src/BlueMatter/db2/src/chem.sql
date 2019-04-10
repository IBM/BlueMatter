-- This CLP file was created using DB2LOOK Version 7.2
-- Timestamp: Sun Jul 21 21:48:47 EDT 2002
-- Database Name: MDSETUP        
-- Database Manager Version: DB2/6000 Version 7.2.3        
-- Database Codepage: 819


CONNECT TO MDSETUP;



------------------------------------------------
-- DDL Statements for table "CHEM    "."CHEMICAL_COMPONENT"
------------------------------------------------
 
 CREATE TABLE "CHEM    "."CHEMICAL_COMPONENT"  (
		  "COMPONENT_NAME" VARCHAR(60) NOT NULL , 
		  "DESCRIPTION" VARCHAR(100) NOT NULL )   
		 IN "MDSHORT" ; 

-- DDL Statements for primary key on Table "CHEM    "."CHEMICAL_COMPONENT"

ALTER TABLE "CHEM    "."CHEMICAL_COMPONENT" 
	ADD PRIMARY KEY
		("COMPONENT_NAME");



------------------------------------------------
-- DDL Statements for table "CHEM    "."MOLECULE_TYPE"
------------------------------------------------
 
 CREATE TABLE "CHEM    "."MOLECULE_TYPE"  (
		  "CLASS" VARCHAR(60) NOT NULL , 
		  "DESCRIPTION" VARCHAR(100) NOT NULL )   
		 IN "MDSHORT" ; 

-- DDL Statements for primary key on Table "CHEM    "."MOLECULE_TYPE"

ALTER TABLE "CHEM    "."MOLECULE_TYPE" 
	ADD PRIMARY KEY
		("CLASS");



------------------------------------------------
-- DDL Statements for table "CHEM    "."ELEMENTS"
------------------------------------------------
 
 CREATE TABLE "CHEM    "."ELEMENTS"  (
		  "ATOMIC_NUMBER" INTEGER NOT NULL , 
		  "ELEMENT_SYMBOL" VARCHAR(60) NOT NULL , 
		  "ELEMENT_NAME" VARCHAR(50) NOT NULL , 
		  "ATOMIC_MASS" VARCHAR(30) NOT NULL )   
		 IN "MDSHORT" ; 

-- DDL Statements for primary key on Table "CHEM    "."ELEMENTS"

ALTER TABLE "CHEM    "."ELEMENTS" 
	ADD PRIMARY KEY
		("ELEMENT_SYMBOL");



------------------------------------------------
-- DDL Statements for table "CHEM    "."ATOM_TAG_TABLE"
------------------------------------------------
 
 CREATE TABLE "CHEM    "."ATOM_TAG_TABLE"  (
		  "COMPONENT_NAME" VARCHAR(60) NOT NULL , 
		  "ATOM_TAG" VARCHAR(60) NOT NULL )   
		 IN "MDSHORT" ; 

-- DDL Statements for primary key on Table "CHEM    "."ATOM_TAG_TABLE"

ALTER TABLE "CHEM    "."ATOM_TAG_TABLE" 
	ADD PRIMARY KEY
		("COMPONENT_NAME",
		 "ATOM_TAG");



------------------------------------------------
-- DDL Statements for table "CHEM    "."SITE_PARAM_TYPE"
------------------------------------------------
 
 CREATE TABLE "CHEM    "."SITE_PARAM_TYPE"  (
		  "TYPE_ID" INTEGER NOT NULL , 
		  "TYPE_DESC" VARCHAR(60) NOT NULL )   
		 IN "MDSHORT" ; 

-- DDL Statements for primary key on Table "CHEM    "."SITE_PARAM_TYPE"

ALTER TABLE "CHEM    "."SITE_PARAM_TYPE" 
	ADD PRIMARY KEY
		("TYPE_ID");



------------------------------------------------
-- DDL Statements for table "CHEM    "."WATER_PARAMETERS"
------------------------------------------------
 
 CREATE TABLE "CHEM    "."WATER_PARAMETERS"  (
		  "WATER_MODEL" VARCHAR(60) NOT NULL , 
		  "Q1" VARCHAR(60) NOT NULL , 
		  "Q2" VARCHAR(60) NOT NULL , 
		  "O_EPSILON" VARCHAR(60) NOT NULL , 
		  "O_SIGMA" VARCHAR(60) NOT NULL , 
		  "H_EPSILON" VARCHAR(60) NOT NULL , 
		  "H_SIGMA" VARCHAR(60) NOT NULL )   
		 IN "MDSHORT" ; 

-- DDL Statements for primary key on Table "CHEM    "."WATER_PARAMETERS"

ALTER TABLE "CHEM    "."WATER_PARAMETERS" 
	ADD PRIMARY KEY
		("WATER_MODEL");



------------------------------------------------
-- DDL Statements for table "CHEM    "."EXTENT_TABLE"
------------------------------------------------
 
 CREATE TABLE "CHEM    "."EXTENT_TABLE"  (
		  "COMPONENT_NAME" VARCHAR(60) NOT NULL , 
		  "ATOM_TAG" VARCHAR(60) NOT NULL , 
		  "MAX_EXTENT" VARCHAR(60) NOT NULL )   
		 IN "MDSHORT" ; 

-- DDL Statements for primary key on Table "CHEM    "."EXTENT_TABLE"

ALTER TABLE "CHEM    "."EXTENT_TABLE" 
	ADD PRIMARY KEY
		("COMPONENT_NAME");







-- DDL Statements for foreign keys on Table "CHEM    "."ATOM_TAG_TABLE"

ALTER TABLE "CHEM    "."ATOM_TAG_TABLE" 
	ADD CONSTRAINT "SQL020508213229180" FOREIGN KEY
		("COMPONENT_NAME")
	REFERENCES "CHEM    "."CHEMICAL_COMPONENT"
		("COMPONENT_NAME")
	ON DELETE CASCADE
	ON UPDATE NO ACTION;




-- DDL Statements for foreign keys on Table "CHEM    "."EXTENT_TABLE"

ALTER TABLE "CHEM    "."EXTENT_TABLE" 
	ADD CONSTRAINT "SQL020717183813390" FOREIGN KEY
		("COMPONENT_NAME",
		 "ATOM_TAG")
	REFERENCES "CHEM    "."ATOM_TAG_TABLE"
		("COMPONENT_NAME",
		 "ATOM_TAG")
	ON DELETE CASCADE
	ON UPDATE NO ACTION;



COMMIT WORK;

CONNECT RESET;

TERMINATE;
