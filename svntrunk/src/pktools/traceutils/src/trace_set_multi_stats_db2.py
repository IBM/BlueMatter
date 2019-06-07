#! /usr/bin/env python

from __future__ import print_function
import DB2
import sys
import os
import os.path
import pwd
from PerfUtils import *

# processes a single trace file and generates statistics
# command line args: dbName trace_set_id minTimeStep <bracketName = TimeStep__0>

bracketName = "TimeStep__0"
skipLastN = 0

if len(sys.argv) < 5:
    print(sys.argv[0], "dbName trace_set_id outFileBaseName minTimeStep <skipLastNSteps = 0> <bracketName = TimeStep__0>")
    sys.exit(-1)
dbName = sys.argv[1]
traceSetId = sys.argv[2]
outFileBase = sys.argv[3]
minTimeStep = sys.argv[4]
if len(sys.argv) > 6:
    bracketName = sys.argv[6]
if len(sys.argv) > 5:
    skipLastN = sys.argv[5]

conn=DB2.Connect(dbName)
lset=tracePointList(conn, traceSetId)

traceSetMultiStats(dbName, conn, traceSetId, minTimeStep, skipLastN, lset, bracketName)
