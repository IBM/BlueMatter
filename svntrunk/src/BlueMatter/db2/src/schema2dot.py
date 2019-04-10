#! /usr/bin/env python
# creates dot format file for graphviz

from schemadb2 import *
import sys


if len(sys.argv) < 3:
    print sys.argv[0], "dbName <schemaName> <schemaName> .... <schemaName>\n"
    sys.exit(-1)

dbName = sys.argv[1]
schemaList = sys.argv[2:]

db = Database(dbName)
print "strict digraph", dbName, "{"
print "size = \"10,7\""
print "rotate = 90"
print "node [shape=record];"

for schemaName in schemaList:
    sc = Schema(db, schemaName)
    tableList = sc.tables()
    for t in tableList:
        print '\"%s\\n(%s)\" [label=\"{%s\\n(%s)' % (t[0], schemaName.strip().upper(), t[0], schemaName.strip().upper()),
        table = Table(sc, t[0])
        for col in table.columns():
            print '|%s\\n(%s)' % (col[0].strip().lower(), col[1].strip().lower()),
        print '}\"]'
        refs = table.references()
        for ref in refs:
            print '\"%s\\n(%s)\" -> \"%s\\n(%s)\";' % (t[0], schemaName.strip().upper(), ref.refTable().strip(), ref.refSchema().strip())
print "}"

