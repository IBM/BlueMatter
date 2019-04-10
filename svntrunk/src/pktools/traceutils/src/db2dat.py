#!/usr/bin/env python
import sys
import os
import os.path
import pwd
from DB2 import *
from PerfUtils import *

# Takes a trace_group_id and a list of labels and creates data files + 
# command line args:
# dbName trace_group_id trcPtListFile outFileNameBase

if len(sys.argv) < 4:
    print sys.argv[0], "dbName trace_group_id outFileNameBase <trcPtListFile=all>\n"
    sys.exit(-1)

dbName = sys.argv[1]
groupId = sys.argv[2]
outFileBase = sys.argv[3]

conn=Connection(dbName)
cursor=conn.cursor()
cursor.execute("select trace_set_id from perf.trace_set where group_id=" +\
               str(groupId))
foo=cursor.fetchone()
traceSetId=foo[0]
lset=tracePointList(conn, traceSetId)
labels=lset.keys()

if len(sys.argv) > 4:
    trcPtListFile = sys.argv[4]
    labels = makeLabelList(trcPtListFile)

db2dat(conn, groupId, outFileBase, labels)
