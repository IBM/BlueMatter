#! /usr/bin/env python

from __future__ import print_function
from DB2 import *
import sys
import os
import os.path
import pwd
from PerfUtils import *

# processes a single trace files and generates statistics
# command line args: traceFileName timeFactor minTimeStep <bracketName = TimeStep>

bracketName = "TimeStep"
if len(sys.argv) < 5:
    print(sys.argv[0], "traceFileName nodeCount timeFactor minTimeStep <bracketName = TimeStep>")
    sys.exit(-1)
traceFileName=sys.argv[1]
nodeCount = int(sys.argv[2])
timeFactor=float(sys.argv[3])
minTimeStep=int(sys.argv[4])
if len(sys.argv) > 5:
    bracketName = sys.argv[5]
singleTrace(timeFactor, nodeCount, traceFileName, minTimeStep, bracketName)
