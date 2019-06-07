#!/usr/bin/env python
from __future__ import print_function
import sys
import os
import os.path
import pwd
import DB2
from PerfUtils import *

# Takes a trace_group_id and a list of labels and creates data files + 
# command line args:
# dbName trace_group_id outFileNameBase label1 label2 ... labelN

if len(sys.argv) < 4:
    print(sys.argv[0], "dbName trace_group_id outFileNameBase label1 label2 ... labelN")
    sys.exit(-1)

dbName = sys.argv[1]
groupId = sys.argv[2]
outFileBase = sys.argv[3]

labels=[]

myconn=DB2.Connection(dbName)

# we will get all the labels by default
if len(sys.argv) == 4:
    mycur=myconn.cursor()
    cmd="select distinct trace_label from perf.trace_id where "+\
         "trace_set_id in (select trace_set_id from perf.trace_set "+\
         "where group_id=" + str(groupId) + ")"
    mycur.execute(cmd)
    foo = mycur.fetchone()
    while foo:
        labels.append(foo[0])
        foo = mycur.fetchone()
else:
    for i in range(4, len(sys.argv)-1):
        labels.append(sys.argv[i])

scalability(myconn, groupId, outFileBase, labels)
