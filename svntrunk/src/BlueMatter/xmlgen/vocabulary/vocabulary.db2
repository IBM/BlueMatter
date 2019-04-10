create table molecule_type (
  class varchar(32) not null primary key,
  description varchar(128) not null
 )
  partitioning key (class) using hashing;

create table chemical_component (
  component_name char(8) not null primary key,
  description varchar(128)
 )
  partitioning key (component_name) using hashing;

create table atom_tag_tbl (
  atom_tag char(4) not null,
  component_name char(3) not null,
  constraint CHEM_COMP foreign key (component_name)
    references CHEMICAL_COMPONENT (component_name),
  primary key (component_name, atom_tag)
 )
  partitioning key (component_name) using hashing;

create table elements (
  atomic_number integer not null,
  element_symbol char(4) not null primary key,
  element_name varchar(32) not null,
  mass varchar(32) not null
 )
  partitioning key (element_symbol) using hashing;
