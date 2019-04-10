#! /usr/bin/env python

# script to run runstats on all tables within the specified set of schemas

from DB2 import *
import os
import sys


if len(sys.argv) < 2:
    print sys.argv[0], "dbName <schemaName> <schemaName> .... <schemaName>\n"
    sys.exit(-1)

action = len(sys.argv)
dbName = sys.argv[1]
schema = sys.argv[2:]
conn = Connection(dbName)

cursor = conn.cursor()

db2Proc = os.popen("db2 -t ", 'w')
db2Proc.write("connect to " + str(dbName) + ";\n")
for schemaName in schema:
    cmd = "select tabname from syscat.tables where tabschema = \'" + str(schemaName) + "\'"
    cursor.execute(cmd.upper())
    foo = cursor.fetchone()
    while (foo):
        tableName = str(foo[0])
        runstatsCmd = "runstats on table " + str(schemaName) + "." + str(tableName) + " with distribution and detailed indexes all;\n"
        print runstatsCmd
        db2Proc.write(runstatsCmd.upper())
        foo = cursor.fetchone()
db2Proc.close()
cursor.close()
