#! /usr/bin/env python

from DB2 import *
import sys
import os
import os.path
import pwd
from PerfUtils import *

# processes a group of trace files and generates distribution functions of intervals
# corresponding to each label at a specified time step
# command line args: dbName trace_group_id timeStep nBins outFileName

if len(sys.argv) < 6:
    print sys.argv[0], "dbName trace_group_id timeStep nBins outFileName"
    sys.exit(-1)
dbName = sys.argv[1]
groupId = sys.argv[2]
timeStep = sys.argv[3]
nBins = sys.argv[4]
outFileName = sys.argv[5]
traceGroupDistribution(dbName, groupId, timeStep, nBins, outFileName)
