#! /usr/bin/env python

from __future__ import print_function
from DB2 import *
import sys
import os
import pwd

# This script allows users to update the sys_class column of the mdsystem.system
# table in db2.  Internally, it will attempt to check that the userid of the
# person updating the column is the same as that of the creator of the system.

# command line arguments: dbName sysId new_sys_class

if len(sys.argv) < 4:
    print(sys.argv[0], "dbName sysId new_sys_class=(scratch|testing|production)")
    sys.exit(-1)

dbName = sys.argv[1]
sysId = sys.argv[2]
newSysClass = sys.argv[3]

conn = Connection(dbName);
cursor = conn.cursor();

cmd = "select sys_id, sys_desc, sys_class, creator, created from mdsystem.system where sys_id = " + sysId

cursor.execute(cmd)

foo = cursor.fetchone()
if not foo:
    print("row with sys_id = ", sysId, " does not exist")
    cursor.close()
    conn.close()
    sys.exit(-2)

print("Before update:")
print('%8s %16s %10s %16s %16s' % ('sys_id','sys_desc','sys_class','creator','created'))
print('%8s %16s %10s %16s %16s' % foo)

if (pwd.getpwuid(os.getuid())[0].upper() != foo[3].upper()):
    print("System created by", foo[3].upper(), "not you")
    cursor.close()
    conn.close()
    sys.exit(-3)

update = "update mdsystem.system set sys_class = \'" + newSysClass + "\' where sys_id = " + sysId

cursor.execute(update)

print("After update:")

cursor.execute(cmd)
foo = cursor.fetchone()
if not foo:
    print("Error in select after update")
else:
    print('%8s %16s %10s %16s %16s' % ('sys_id','sys_desc','sys_class','creator','created'))
    print('%8s %16s %10s %16s %16s' % foo)

cursor.close()
conn.close()
