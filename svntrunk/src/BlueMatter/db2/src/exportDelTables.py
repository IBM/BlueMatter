#!/usr/local/bin/python
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
cursor3 = conn.cursor()

exportProc = os.popen("db2 -t ", 'w')
exportProc.write("connect to " + str(dbName) + ";\n")

# First export tables that do not have a sys_id column
exportDir=targetDir
lobsDir = exportDir + "/" "lobs"
if not os.path.exists(lobsDir):
    os.mkdir(lobsDir)
for schemaName in schema:
    cmd = "select tabname from syscat.tables where tabschema = \'" +\
          str(schemaName) +\
          "\' except (select tabname from syscat.columns where tabschema=\'" +\
          str(schemaName) +\
          "\' and colname = \'SYS_ID\')"
    cursor.execute(cmd.upper())
    tableList = cursor.fetchall()
    conn.commit()
    for foo in tableList:
        tableName = str(foo[0])
        exportCmd = "export to " + exportDir + "/" + str(schemaName) + "." + str(tableName) + ".ixf of ixf lobs to " + lobsDir + "/ lobfile " + str(schemaName) + "." + str(tableName) + " modified by lobsinfile select * from " + str(schemaName) + "." + str(tableName)
        exportCmd = exportCmd + ";\n"
        print(exportCmd)
        print("commit;\n")
        exportProc.write(exportCmd)
        exportProc.write("commit;\n")

idCmd = "select sys_id from mdsystem.system where sys_class <> \'scratch\'"
#idCmd = "select sys_id from mdsystem.system"
cursor3.execute(idCmd)
sysList = cursor3.fetchall()
conn.commit()
for sysLine in sysList:
    sysId = str(sysLine[0])
    print("SYS_ID = ", sysId)
    exportDir = targetDir + "/" + sysId
    if not os.path.exists(exportDir):
        os.mkdir(exportDir)
    lobsDir = exportDir + "/" "lobs"
    if not os.path.exists(lobsDir):
        os.mkdir(lobsDir)
    for schemaName in schema:
        cmd = "select tabname from syscat.columns where tabschema = \'" + str(schemaName) + "\' and colname = \'SYS_ID\'"
        cursor.execute(cmd.upper())
        tableList = cursor.fetchall()
        conn.commit()
        for foo in tableList:
            tableName = str(foo[0])
            exportCmd = "export to " + exportDir + "/" + str(schemaName) + "." + str(tableName) + ".ixf of ixf lobs to " + lobsDir + "/ lobfile " + str(schemaName) + "." + str(tableName) + " modified by lobsinfile select * from " + str(schemaName) + "." + str(tableName)
            exportCmd = exportCmd + " where sys_id = " + sysId
            exportCmd = exportCmd + ";\n"
            print(exportCmd)
            print("commit;\n")
            exportProc.write(exportCmd)
            exportProc.write("commit;\n")
exportProc.write("connect reset;\n")
exportProc.close()
conn.commit()
