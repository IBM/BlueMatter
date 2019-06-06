#! /usr/bin/env python

from __future__ import print_function
from DB2 import *
import sys
import os
import os.path
import pwd
from PerfUtils import *

# processes a group of trace files and generates "timing diagrams" for the
# start/finish trace pairs (using dist(t) for start trace points and 1-dist(t)
# for finish trace points corresponding to each label
# command line args: dbName trace_set_id nBins syncLabel

if len(sys.argv) < 5:
    print(sys.argv[0], "dbName trace_set_id nBins syncLabel")
    sys.exit(-1)
dbName = sys.argv[1]
traceSetId = sys.argv[2]
nBins = sys.argv[3]
syncLabel = sys.argv[4]
conn = Connection(dbName)
cur = conn.cursor()
cmd = "select trace_id from perf.trace_id where trace_set_id = " +\
      traceSetId + " and trace_type = 0 and trace_label = \'" +\
      syncLabel + "\'"
cur.execute(cmd)
foo = cur.fetchone()
if not foo:
    sys.stderr.write(" Error getting trace id for sync label " +\
                     str(syncLabel) + "\n")
    sys.exit(-1)
syncId = foo[0]
label=tracePointList(conn, traceSetId)
traceSetTimingDiagram(conn, traceSetId, label, nBins, syncId)
