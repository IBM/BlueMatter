#! /usr/bin/env python

from __future__ import print_function
from DB2 import *
import sys

if len(sys.argv) < 2:
    print(sys.argv[0], "dbName <schemaName> <tableName>\n")
    sys.exit(-1)

action = len(sys.argv)
dbName = sys.argv[1]
conn = Connection(dbName)

cursor = conn.cursor()
cursor2 = conn.cursor()

if action > 2:
    schemaName = sys.argv[2]
if action > 3:
    tableName = sys.argv[3]

if action == 2:
    cursor.execute("select schemaname, owner from syscat.schemata")
    print('%32s %16s' % ('SCHEMANAME', 'OWNER'))
    print(' ')
    foo = cursor.fetchone()
    while (foo):
        print('%32s %16s' % (foo[0], foo[1]))
        foo = cursor.fetchone()
elif action == 3:
    print('%32s %16s %20s' % ('TABLENAME', 'ROWCOUNT', 'TABLEID'))
    print(' ')
    cmd = "select tabname, card, tableid from syscat.tables where tabschema = \'" + str(schemaName) + "\'"
    cursor.execute(cmd.upper())
    foo = cursor.fetchone()
    while (foo):
        countCmd = "select count(*) from " + schemaName + "." + foo[0]
#        print countCmd
#        cursor2.execute(countCmd)
#        bar = cursor2.fetchone()
#        print '%32s %16i %20d' % (foo[0], bar[0], foo[2])
        print('%32s %16i %20d' % (foo[0], foo[1], foo[2]))
        foo = cursor.fetchone()
elif action == 4:
    print('%16s %16s %16s' % ('COLNAME', 'TYPENAME', 'LENGTH'))
    print(' ')
    cmd = "select colname, typename, length from syscat.columns where tabschema = \'" + str(schemaName) + "\' and tabname = \'" + str(tableName) + "\'"
    cursor.execute(cmd.upper())
    foo = cursor.fetchone()
    while (foo):
        print('%20s %20s %16d' % (foo[0], foo[1], foo[2]))
        foo = cursor.fetchone()
    print('\n%16s %20s %16s %16s\n' % ('INDSCHEMA', 'IDXNAME', 'DEFINER', 'COLUMNS'))
    idxCmd = "select indschema, indname, definer from syscat.indexes where tabschema = \'" + str(schemaName) + "\' and tabname = \'" + str(tableName) + "\'"
    cursor2.execute(idxCmd.upper())
    bar = cursor2.fetchone()
    cursor3 = conn.cursor()
    while(bar):
        print('%16s %20s %16s' % (bar[0], bar[1], bar[2]), end=' ')
        idxColCmd = "select colname from syscat.indexcoluse where indschema = \'" + str(bar[0]) + "\' and indname = \'" + str(bar[1]) + "\' order by colseq"
        cursor3.execute(idxColCmd.upper())
        glub = cursor3.fetchone()
        while(glub):
            print(' %16s' % (glub[0]), end=' ')
            glub = cursor3.fetchone()
        bar = cursor2.fetchone()
else:
    print("Invalid action = ", action)
conn.close()
