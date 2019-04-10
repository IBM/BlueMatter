#!/usr/bin/env python
import sys
import os
import os.path
import pwd
import DB2
import re
import PerfUtils
from xreadlines import *

# ##############################################################################
# Script to take a trace_group_id and plot
# scalability data for a specified set of trace points
#
# command line arguments: dbName groupId trcPtList
#                         dbName (database name)
#                         groupId (trace group id)
#                         trcPtList (path to file containg the list of
#                                    trace points for which data is to
#                                    be plotted)
# ##############################################################################


def makeLabelList(trcPtList):
    labelList = []
    tl = open(trcPtList, 'r')
    if not tl:
        print "Unable to open file", trcPtList
        sys.exit(-1)
    ws = re.compile(r'\s+')
    for line in xreadlines(tl):
        rec = ws.split(str(line))
        labelList.append(rec[0])
    return labelList


if len(sys.argv) < 5:
    print sys.argv[0], "dbName groupId trcPtList outFileBase"
    sys.exit(-1)
dbName = sys.argv[1]
groupId = sys.argv[2]
trcPtList = sys.argv[3]
outFileBase = sys.argv[4]

labels = makeLabelList(trcPtList)
print labels
PerfUtils.scalability(dbName, groupId, outFileBase, labels)
