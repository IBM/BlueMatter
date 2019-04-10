#! /bin/ksh

db2 "connect to $1";

db2 -tvf drop_chem.db2;
db2 -tvf drop_registry.db2;
db2 -tvf drop_mdsystem.db2;
db2 -tvf drop_experiment.db2;

db2 "disconnect $1";
