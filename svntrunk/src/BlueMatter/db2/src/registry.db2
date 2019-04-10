CREATE SCHEMA Registry; 	 

CREATE TABLE Registry.UDF_Record (
 udf_id     INT NOT NULL,
 udf_name   VARCHAR(50) NOT NULL,
 entry_date DATE,
 entry_time TIME,
 arg_check_sum INT,
 body_check_sum INT,
 site_count     INT,
 param_count    INT,
 PRIMARY KEY (udf_id)) IN mdshort;
	 
CREATE TABLE Registry.UDF_Category (
 udf_id     INT NOT NULL,
 category_name    VARCHAR(60) NOT NULL,
 PRIMARY KEY (udf_id, category_name),
 FOREIGN KEY (udf_id) REFERENCES Registry.UDF_Record ON DELETE CASCADE) IN mdshort;
 
CREATE TABLE Registry.UDF_Variable (
 udf_id     INT NOT NULL, 
 var_name   VARCHAR(40) NOT NULL, 
 type_desc  VARCHAR(30), 
 type_size  INT, 
 PRIMARY KEY (udf_id , var_name), 
 FOREIGN KEY (udf_id) REFERENCES Registry.UDF_Record ON DELETE CASCADE) IN mdshort;
