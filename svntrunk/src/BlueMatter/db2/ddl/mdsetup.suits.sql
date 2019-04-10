-- This CLP file was created using DB2LOOK Version 8.2
-- Timestamp: Thu May 18 03:20:28 EDT 2006
-- Database Name: MDSETUP        
-- Database Manager Version: DB2/6000 Version 8.2.4        
-- Database Codepage: 819
-- Database Collating Sequence is: UNIQUE


CONNECT TO MDSETUP;



------------------------------------------------
-- DDL Statements for table "SUITS   "."COMMON_NAME"
------------------------------------------------
 
 CREATE TABLE "SUITS   "."COMMON_NAME"  (
		  "NAME" VARCHAR(100) NOT NULL , 
		  "SYS_ID" INTEGER , 
		  "CREATED" TIMESTAMP )   
		 IN "USERSPACE1" ; 

-- DDL Statements for primary key on Table "SUITS   "."COMMON_NAME"

ALTER TABLE "SUITS   "."COMMON_NAME" 
	ADD PRIMARY KEY
		("NAME");








-- DDL Statements for foreign keys on Table "SUITS   "."COMMON_NAME"

ALTER TABLE "SUITS   "."COMMON_NAME" 
	ADD CONSTRAINT "SQL030205164004490" FOREIGN KEY
		("SYS_ID")
	REFERENCES "MDSYSTEM"."SYSTEM"
		("SYS_ID")
	ON DELETE CASCADE
	ON UPDATE NO ACTION
	ENFORCED
	ENABLE QUERY OPTIMIZATION;


COMMIT WORK;

CONNECT RESET;

TERMINATE;
