#!/usr/bin/env python

import sys
import os

if len(sys.argv) < 2:
    print sys.argv[0], " dbName <schemaName> <schemaName> .... <schemaName>\n"
    sys.exit(-1)

action = len(sys.argv)
dbName = sys.argv[1]
schema = sys.argv[2:]

for s in schema:
    cmd = "db2look -d " + dbName + " -z " + s + " -e -o " + dbName + "." + s + ".sql"
    os.system(cmd)

