#! /usr/bin/env python

from DB2 import *
import sys
import os
import os.path
import pwd
from PerfUtils import *

# This script allows users to process an entire group of related traces into
# their default schema in db2 and analyze their statistics

# command line arguments: dbName description clockSpeedMhz fileList outFileBaseName minTime Step <skipLastNSteps = 0> <bracketName =  TimeStep__0>

if len(sys.argv) < 7:
    print sys.argv[0], "dbName description clockSpeedMhz fileList outFileBaseName  minTimeStep <skipLastNSteps = 0> <bracketName = TimeStep__0>"
    sys.exit(-1)

bracketName = "TimeStep__0"
skipLastN = 0
dbName = sys.argv[1]
desc = sys.argv[2]
timeFactor = sys.argv[3]
fileList = sys.argv[4]
outFileBase = sys.argv[5]
minTimeStep = sys.argv[6]

if len(sys.argv) > 8:
    bracketName = sys.argv[8]
if len(sys.argv) > 7:
    skipLastN = sys.argv[7]

traceSet = makeFileList(fileList)

groupId=traceGroup(dbName, desc, timeFactor, traceSet)
print "Group ID: ", str(groupId)
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

