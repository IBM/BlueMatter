#! /bin/ksh

db2 "connect to $1";

db2 -tvf chem.db2;
db2 -tvf registry.db2;
db2 -tvf mdsystem.db2;
db2 -tvf experiment.db2;
db2 -tvf setpriv.db2;

db2 "disconnect $1";
