#! /usr/bin/env python

from __future__ import print_function
from DB2 import *
import sys
import os
import os.path
import pwd
from PerfUtils import *

# This script allows users to process an entire group of related traces into
# their default schema in db2.  

# command line arguments: dbName description clockSpeedMhz fileList

if len(sys.argv) < 5:
    print(sys.argv[0], "dbName description clockSpeedMhz fileList")
    sys.exit(-1)

dbName = sys.argv[1]
desc = sys.argv[2]
timeFactor = sys.argv[3]
fileList = sys.argv[4]
traceSet = makeFileList(fileList)

groupId=traceGroup(dbName, desc, timeFactor, traceSet)
print("Group ID: ", str(groupId))
