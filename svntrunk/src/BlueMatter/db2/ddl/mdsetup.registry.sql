-- This CLP file was created using DB2LOOK Version 8.2
-- Timestamp: Thu May 18 03:20:09 EDT 2006
-- Database Name: MDSETUP        
-- Database Manager Version: DB2/6000 Version 8.2.4        
-- Database Codepage: 819
-- Database Collating Sequence is: UNIQUE


CONNECT TO MDSETUP;



------------------------------------------------
-- DDL Statements for table "REGISTRY"."UDF_RECORD"
------------------------------------------------
 
 CREATE TABLE "REGISTRY"."UDF_RECORD"  (
		  "UDF_ID" INTEGER NOT NULL , 
		  "UDF_NAME" VARCHAR(50) NOT NULL , 
		  "ENTRY_DATE" DATE , 
		  "ENTRY_TIME" TIME , 
		  "ARG_CHECK_SUM" INTEGER , 
		  "BODY_CHECK_SUM" INTEGER , 
		  "SITE_COUNT" INTEGER , 
		  "PARAM_COUNT" INTEGER )   
		 IN "MDSHORT" ; 

-- DDL Statements for primary key on Table "REGISTRY"."UDF_RECORD"

ALTER TABLE "REGISTRY"."UDF_RECORD" 
	ADD PRIMARY KEY
		("UDF_ID");



------------------------------------------------
-- DDL Statements for table "REGISTRY"."UDF_CATEGORY"
------------------------------------------------
 
 CREATE TABLE "REGISTRY"."UDF_CATEGORY"  (
		  "UDF_ID" INTEGER NOT NULL , 
		  "CATEGORY_NAME" VARCHAR(60) NOT NULL )   
		 IN "MDSHORT" ; 

-- DDL Statements for primary key on Table "REGISTRY"."UDF_CATEGORY"

ALTER TABLE "REGISTRY"."UDF_CATEGORY" 
	ADD PRIMARY KEY
		("UDF_ID",
		 "CATEGORY_NAME");



------------------------------------------------
-- DDL Statements for table "REGISTRY"."UDF_VARIABLE"
------------------------------------------------
 
 CREATE TABLE "REGISTRY"."UDF_VARIABLE"  (
		  "UDF_ID" INTEGER NOT NULL , 
		  "VAR_NAME" VARCHAR(40) NOT NULL , 
		  "TYPE_DESC" VARCHAR(30) , 
		  "TYPE_SIZE" INTEGER )   
		 IN "MDSHORT" ; 

-- DDL Statements for primary key on Table "REGISTRY"."UDF_VARIABLE"

ALTER TABLE "REGISTRY"."UDF_VARIABLE" 
	ADD PRIMARY KEY
		("UDF_ID",
		 "VAR_NAME");








-- DDL Statements for foreign keys on Table "REGISTRY"."UDF_CATEGORY"

ALTER TABLE "REGISTRY"."UDF_CATEGORY" 
	ADD CONSTRAINT "SQL030401160542551" FOREIGN KEY
		("UDF_ID")
	REFERENCES "REGISTRY"."UDF_RECORD"
		("UDF_ID")
	ON DELETE CASCADE
	ON UPDATE NO ACTION
	ENFORCED
	ENABLE QUERY OPTIMIZATION;

-- DDL Statements for foreign keys on Table "REGISTRY"."UDF_VARIABLE"

ALTER TABLE "REGISTRY"."UDF_VARIABLE" 
	ADD CONSTRAINT "SQL030401160543521" FOREIGN KEY
		("UDF_ID")
	REFERENCES "REGISTRY"."UDF_RECORD"
		("UDF_ID")
	ON DELETE CASCADE
	ON UPDATE NO ACTION
	ENFORCED
	ENABLE QUERY OPTIMIZATION;


COMMIT WORK;

CONNECT RESET;

TERMINATE;
