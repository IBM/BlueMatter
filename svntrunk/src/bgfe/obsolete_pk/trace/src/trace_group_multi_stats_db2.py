#! /usr/bin/env python

from DB2 import *
import sys
import os
import os.path
import pwd
from PerfUtils import *

# processes a group of trace files and generates statistics
# command line args: dbName trace_group_id minTimeStep <bracketName = TimeStep__0>

bracketName = "TimeStep__0"
skipLastN = 0

if len(sys.argv) < 5:
    print sys.argv[0], "dbName trace_group_id outFileBaseName minTimeStep <skipLastNSteps = 0> <bracketName = TimeStep__0>"
    sys.exit(-1)
dbName = sys.argv[1]
groupId = sys.argv[2]
outFileBase = sys.argv[3]
minTimeStep = sys.argv[4]
if len(sys.argv) > 6:
    bracketName = sys.argv[6]
if len(sys.argv) > 5:
    skipLastN = sys.argv[5]
traceGroupMultiStatsDb2(dbName, groupId, outFileBase, minTimeStep, skipLastN, bracketName)

conn=Connection(dbName)
cursor=conn.cursor()
cursor.execute("select trace_set_id from perf.trace_set where group_id=" +\
               str(groupId))
foo=cursor.fetchone()
traceSetId=foo[0]
lset=tracePointList(conn, traceSetId)
labels=lset.keys()
db2dat(conn, groupId, outFileBase, labels)
