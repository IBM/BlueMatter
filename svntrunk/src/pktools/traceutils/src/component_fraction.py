#!/usr/bin/env python
# generates data file suitable for gnuplot
# contains fraction of total time in each component as a function of total ts
# command line args:
# dataBase
# group_id
# tracePtFile (list of trace points)
# optional bracketId = "TimeStep"
from __future__ import print_function
import sys
import DB2
bracketName = "TimeStep"
if len(sys.argv) < 4:
    print(sys.argv[0], "dbName group_id tracePtFile <bracketId =", bracketName, ">")
    sys.exit(-1)
if len(sys.argv) > 4:
    bracketName=sys.argv[4]
dbName=sys.argv[1]
groupId=sys.argv[2]
trcPtFile=sys.argv[3]

f=open(trcPtFile)
trcPtLines=f.readlines()
trcPtList = ""
trcPts = []
for trcPt in trcPtLines:
    trcPt=trcPt.strip()
    trcPts.append(trcPt)
    trcPtList = trcPtList + "\'" + str(trcPt) + "\', "
trcPtList = trcPtList + "\'" + str(bracketName) + "\'"
trcPts.append(bracketName)
trcPts.sort()

conn=DB2.Connection(dbName)
cursor=conn.cursor()

cmd = "with d (set_id, node_count, trace_label, avg) as " +\
      "(select perf.performance.trace_set_id, node_count, trace_label, avg " +\
      "from perf.performance, perf.trace_set where perf.performance.trace_set_id in " +\
      "(select perf.trace_set.trace_set_id from perf.trace_set where group_id=" +\
      str(groupId) + ") and perf.performance.trace_set_id=perf.trace_set.trace_set_id)," +\
      "r (set_id, avg) as "+\
      "(select trace_set_id, avg from perf.performance where trace_set_id in "+\
      "(select trace_set_id from perf.trace_set where group_id=" +\
      str(groupId) + ") and trace_label=\'" + str(bracketName) + "\') "+\
      "select d.node_count, d.trace_label, d.avg from d,r " +\
      "where d.set_id=r.set_id and d.trace_label in (" + str(trcPtList) +\
      ") order by node_count, d.set_id, d.trace_label"
# print cmd
cursor.execute(cmd)
foo=cursor.fetchone()
oldnc=0
print("# group_id =", str(groupId))
print("# all times in nanoseconds")
print("# column 1: node count")
i = 2
for t in trcPts:
    print("# column ", str(i), ":", str(t))
    i=i+1
while foo:
    if foo[0] != oldnc:
        print('\n %6d \t' % (int(foo[0])), end=' ')
        oldnc = foo[0]
    #    print str(foo[0]), str(foo[1]), str(foo[2])
    if foo[0] and foo[1] and foo[2]:
        print('%10.3g \t' % (foo[2]), end=' ')
    else:
        print('%10d \t' % (-1), end=' ')
    foo=cursor.fetchone()
print("\n", end=' ')
