CREATE SCHEMA CHEM;
	 
CREATE TABLE CHEM.chemical_component  (  
 component_name VARCHAR(60) NOT NULL, 
 description VARCHAR(100) NOT NULL, 
 PRIMARY KEY ( component_name )) IN mdshort;

CREATE TABLE CHEM.molecule_type  (  		      
 class VARCHAR(60) NOT NULL, 
 description VARCHAR(100) NOT NULL, 
 PRIMARY KEY ( class )) IN mdshort;

CREATE TABLE CHEM.elements  (  
 atomic_number INT NOT NULL, 
 element_symbol VARCHAR(60) NOT NULL, 
 element_name VARCHAR(50) NOT NULL, 
 atomic_mass VARCHAR(30) NOT NULL, 
 PRIMARY KEY ( element_symbol )) IN mdshort;

CREATE TABLE CHEM.atom_tag_table  (  
 component_name VARCHAR(60) NOT NULL, 
 atom_tag       VARCHAR(60) NOT NULL, 
 PRIMARY KEY ( component_name, atom_tag ), 
 FOREIGN KEY ( component_name) REFERENCES CHEM.chemical_component ON DELETE CASCADE) IN mdshort;

CREATE TABLE CHEM.extent_table  (  
 component_name VARCHAR(60) NOT NULL, 
 atom_tag       VARCHAR(60) NOT NULL, 
 max_extent   VARCHAR(60) NOT NULL,
 PRIMARY KEY ( component_name ), 
 FOREIGN KEY ( component_name, atom_tag) REFERENCES CHEM.atom_tag_table ON DELETE CASCADE) IN mdshort;

CREATE TABLE CHEM.site_param_type  (  
 type_id        INT NOT NULL, 
 type_desc      VARCHAR(60) NOT NULL, 
 PRIMARY KEY ( type_id )) IN mdshort;
         
CREATE TABLE CHEM.water_parameters  (  
 water_model    VARCHAR(60) NOT NULL, 
 q1             VARCHAR(60) NOT NULL, 
 q2             VARCHAR(60) NOT NULL, 
 O_epsilon      VARCHAR(60) NOT NULL, 
 O_sigma        VARCHAR(60) NOT NULL, 
 H_epsilon      VARCHAR(60) NOT NULL, 
 H_sigma        VARCHAR(60) NOT NULL, 
 PRIMARY KEY ( water_model )) IN mdshort;
