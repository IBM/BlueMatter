#! /usr/bin/env python

from __future__ import print_function
from DB2 import *
import sys
import os
import os.path
import pwd
from PerfUtils import *

# This script allows users to process a selected group of traces and
# add them to an existing trace_group in db2

# command line arguments: dbName groupID timeFactor fileList

if len(sys.argv) < 5:
    print(sys.argv[0], "dbName groupID timeFactor fileList")
    sys.exit(-1)

dbName = sys.argv[1]
groupId = sys.argv[2]
timeFactor = sys.argv[3]
fileList = sys.argv[4]
traceSet = makeFileList(fileList)

traceGroupAppend(dbName, groupId, timeFactor, traceSet)
