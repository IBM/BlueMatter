#! /usr/bin/env python

from __future__ import print_function
from DB2 import *
import sys
import os

if len(sys.argv) < 3:
    print(sys.argv[0], " targetDir dbName <schemaName> <schemaName> .... <schemaName>\n")
    sys.exit(-1)

action = len(sys.argv)
targetDir = sys.argv[1]
dbName = sys.argv[2]
schema = sys.argv[3:]
conn = Connection(dbName)

cursor = conn.cursor()
cursor2 = conn.cursor()

exportProc = os.popen("db2 -t ", 'w')
exportProc.write("connect to " + str(dbName) + ";\n")
for schemaName in schema:
    cmd = "select tabname from syscat.tables where tabschema = \'" + str(schemaName) + "\'"
    cursor.execute(cmd.upper())
    foo = cursor.fetchone()
    while (foo):
        tableName = str(foo[0])
        exportCmd = "export to " + targetDir + "/" + str(schemaName) + "." + str(tableName) + ".ixf of ixf select * from " + str(schemaName) + "." + str(tableName)
        ckSysCmd = "select count(*) from syscat.columns where tabschema = \'" + str(schemaName) + "\' and tabname = \'" + str(tableName) + "\' and colname = \'SYS_ID\'"
        cursor2.execute(ckSysCmd.upper())
        bar = cursor2.fetchone()
        if bar[0] != 0:
            exportCmd = exportCmd + " where sys_id in (select sys_id from mdsystem.system where sys_class <> 'scratch')"
        exportCmd = exportCmd + ";\n"
        print(exportCmd)
        exportProc.write(exportCmd)
        foo = cursor.fetchone()
exportProc.write("connect reset;\n")
exportProc.close()
