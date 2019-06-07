#! /usr/bin/env python

from __future__ import print_function
from schemadb2 import *
import sys


if len(sys.argv) < 2:
    print(sys.argv[0], "dbName <schemaName> <tableName>\n")
    sys.exit(-1)

action = len(sys.argv)
dbName = sys.argv[1]
db = Database(dbName)

if action > 2:
    schemaName = sys.argv[2]
    sc = Schema(db, schemaName)
if action > 3:
    tableName = sys.argv[3]
    table = Table(sc, tableName)

if action == 2:
    schemaRec = db.schemaList()
    print('%32s %16s' % ('SCHEMANAME', 'OWNER'))
    print(' ')
    for rec in schemaRec:
        print('%32s %16s' % (rec[0], rec[1]))
elif action == 3:
    tableList = sc.tables()
    print('%32s %16s %20s' % ('TABLENAME', 'ROWCOUNT', 'TABLEID'))
    print(' ')
    for table in tableList:
        print('%32s %16i %20d' % (table[0], table[1], table[2]))
elif action == 4:
    colList = table.columns()
    print('%20s %20s %16s %5s' % ('COLNAME', 'TYPENAME', 'LENGTH', 'NULLS'))
    print(' ')
    for col in colList:
        print('%20s %20s %16d %5s' % (col[0], col[1], int(col[2]), col[3]))
    print('\n\n')
    print('primary key:')
    for col in table.primary():
        print('%20s ' % (str(col)), end=' ')
    print('\n\n')
    print('%20s %20s %20s' % ('REFSCHEMA', 'REFTABLE', 'REFKEYCOLS'))
    print(' ')
    for ref in table.references():
        print('%20s %20s %20s' % (ref.refSchema(), ref.refTable(), ref.refKeyCols()))
