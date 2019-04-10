#! /usr/bin/env python

from DB2 import *
import sys

# key1, key2 are the values of column[0] that will be used to select two
# different results sets from tableName for comparison using column[1]..column[n]

def difftable(key1, key2, column, tableName, dbName):
    conn = Connection(dbName)
    selCmd = "select "
    for i in range(1,len(column)-1):
        selCmd = selCmd + str(column[i]) + ", "
    selCmd = selCmd + str(column[len(column)-1]) + " "
    selCmd = selCmd + "from " + str(tableName) + " where " + str(column[0]) + "="
    cmd = selCmd + str(key1)
    cmd = cmd + " except ("
    cmd = cmd + selCmd + str(key2) + ")"

    cursor = conn.cursor()
#    print "cmd", cmd
    cursor.execute(cmd)
    foo = cursor.fetchone()
    headers = 0
    if (foo):
        for i in range(len(column)):
            print '%32s' % (column[i]),
        print ' '
        headers = 1

    while(foo):
        print '%32s' % (key1),
        for i in range(len(foo)):
            print '%32s' % (foo[i]),
        print ' '
        foo = cursor.fetchone()
    cmd = selCmd + str(key2)
    cmd = cmd + " except "
    cmd = cmd + selCmd + str(key1)
    cursor.execute(cmd)
    foo = cursor.fetchone()
    if (foo and headers != 1):
        for i in range(len(column)):
            print '%32s' % (column[i]),
        print ' '
        headers = 1
    while(foo):
        print '%32s' % (key2),
        for i in range(len(foo)):
            print '%32s' % (foo[i]),
        print ' '
        foo = cursor.fetchone()
    cursor.close()
    conn.close()
        

if len(sys.argv) < 4:
    print sys.argv[0], "dbName key1 key2"
    sys.exit(-1)

dbName = sys.argv[1]
key1 = sys.argv[2]
key2 = sys.argv[3]

bondColumn = ('sys_id', 'bond_id', 'bond_type', 'site_id1', 'site_id2')
bondTable = 'mdsystem.bond'
print "DIFFS FOR:", bondTable
difftable(key1, key2, bondColumn, bondTable, dbName)

udfInvocationColumn = ('sys_id', 'udf_id', 'param_tuple_list_id', 'site_tuple_list_id')
udfInvocationTable = 'mdsystem.udf_invocation_table'
print "DIFFS FOR:", udfInvocationTable
difftable(key1, key2, udfInvocationColumn, udfInvocationTable, dbName)

siteTupleListColumn = ('sys_id', 'site_tuple_list_id', 'site_tuple_desc')
siteTupleListTable = 'mdsystem.sitetuplelist'
print "DIFFS FOR:", siteTupleListTable
difftable(key1, key2, siteTupleListColumn, siteTupleListTable, dbName)

siteTupleDataColumn = ('sys_id', 'site_id', 'site_ordinal', 'site_tuple_id', 'site_tuple_list_id')
siteTupleDataTable = 'mdsystem.sitetupledata'
print "DIFFS FOR:", siteTupleDataTable
difftable(key1, key2, siteTupleDataColumn, siteTupleDataTable, dbName)

siteTupleColumn = ('sys_id', 'site_tuple_id', 'site_tuple_list_id')
siteTupleTable = 'mdsystem.sitetuple'
print "DIFFS FOR:", siteTupleTable
difftable(key1, key2, siteTupleColumn, siteTupleTable, dbName)

paramTupleListColumn = ('sys_id', 'param_tuple_list_id', 'param_tuple_desc')
paramTupleListTable = 'mdsystem.paramtuplelist'
print "DIFFS FOR:", paramTupleListTable
difftable(key1, key2, paramTupleListColumn, paramTupleListTable, dbName)

paramTupleDataColumn = ('sys_id', 'param_id', 'param_tuple_id', 'param_tuple_list_id', 'param_value')
paramTupleDataTable = 'mdsystem.paramtupledata'
print "DIFFS FOR:", paramTupleDataTable
difftable(key1, key2, paramTupleDataColumn, paramTupleDataTable, dbName)

paramTupleColumn = ('sys_id', 'param_tuple_id', 'param_tuple_list_id')
paramTupleTable = 'mdsystem.paramtuple'
print "DIFFS FOR:", paramTupleTable
difftable(key1, key2, paramTupleColumn, paramTupleTable, dbName)

pTupleSpecColumn = ('sys_id', 'param_id', 'param_name', 'param_tuple_list_id', 'param_type')
pTupleSpecTable = 'mdsystem.paramtuplespec'
print "DIFFS FOR:", pTupleSpecTable
difftable(key1, key2, pTupleSpecColumn, pTupleSpecTable, dbName)

globalParamColumn = ('sys_id', 'param_id', 'param_type', 'param_value', 'symbolic_constant')
globalParamTable = 'mdsystem.global_parameter'
print "DIFFS FOR:", globalParamTable
difftable(key1, key2, globalParamColumn, globalParamTable, dbName)

siteColumn = ('sys_id', 'site_id', 'atom_tag', 'atomic_mass', 'charge', 'epsilon', 'epsilon14', 'rmin', 'rmin14')
siteTable = 'mdsystem.site'
print "DIFFS FOR:",siteTable
difftable(key1, key2, siteColumn, siteTable, dbName)

