from __future__ import print_function

from schema2graph import *
from graphutils import *
import sys
import os

# target is a fileName to be matched
def visitImport(table, dirName, names):
    target = table[0].lower() + '.' + table[1].upper() + '.ixf'
    for n in names:
        if n == target:
            print('import from %s/%s of ixf lobs from %s/lobs/ modified by lobsinfile commitcount 100000 insert into %s.%s;' %\
                  (dirName, target, dirName, table[0], table[1]))
            return

if len(sys.argv) < 3:
    print(sys.argv[0], "importRootPath dbName <schemaName> <schemaName> .... <schemaName>\n")
    sys.exit(-1)

rootPath = sys.argv[1]
dbName = sys.argv[2]
schemaList = sys.argv[3:]
print("--schemaList: ", str(schemaList))

print("connect to", dbName, ";")
graph = schema2graph(dbName, schemaList)
for n in graph.keys():
    print("--node:", n, "| adj:", graph[n])
d = DepthFirst(graph)

for table in d.visitList():
    print("--table: ", str(table))
    os.path.walk(rootPath, visitImport, table)

print("commit work;")
print("connect reset;")
print("terminate;")
