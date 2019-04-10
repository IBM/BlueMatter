#!/usr/bin/ksh

#db2move mdsetup2 EXPORT -sn chem,registry,experiment,suits,mdsystem,perf -l /backup01/db2/pepsi/mdsetup2/export/20060907/lob

db2look -d mdsetup2 -e -o mdsetup2.sql -l 
