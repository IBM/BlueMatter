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
# Master script to take a set of trace output files, analyze them, and plot
# scalability data for a specified set of trace points
#
# command line arguments: dbName description timeConv fileList trcPtList
#                         dbName (database name)
#                         description (quoted)
#                         timeConv (time conversion factor, 1000 for BG/L
#                         fileList (path to file containing list of trace output
#                                   files to be processed along with the
#                                   corresponding node counts---each line is of
#                                   the form: nodeCount trcFile)
#                         trcPtList (path to file containg the list of
#                                    trace points for which data is to
#                                    be plotted)
# ##############################################################################

#

if len(sys.argv) < 8:
    print sys.argv[0], "dbName description timeConv fileList "+\
          "trcPtList minTimeStep outFileBase"
    sys.exit(-1)
dbName = sys.argv[1]
desc = sys.argv[2]
timeConv = sys.argv[3]
fileList = sys.argv[4]
trcPtList = sys.argv[5]
minTimeStep = sys.argv[6]
outFileBase = sys.argv[7]

traceSet = PerfUtils.makeFileList(fileList)
labels = PerfUtils.makeLabelList(trcPtList)

groupId = PerfUtils.traceGroup(dbName, desc, timeConv, traceSet)
bracketName='TimeStep__0'
outFileBase='stats'
PerfUtils.traceGroupMultiStatsDb2(dbName, groupId, outFileBase, minTimeStep,
                                  bracketName)
PerfUtils.scalability(dbName, groupId, outFileBase, labels)
