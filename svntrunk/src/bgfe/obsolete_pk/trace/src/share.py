#! /usr/bin/env python

# creates a gnuplot-readable data file (sent to stdout) from a dataset in db2
# the data file is of the form created by trace_group_multi_stats.py
# command line args: dbName trace_group_id minTimeStep <bracketName = TimeStep>


from DB2 import *
import sys
import os

if len(sys.argv) < 3:
    print sys.argv[0], "dbName trace_group_id"
    sys.exit(-1)
dbName = sys.argv[1]
groupId = sys.argv[2]
oldTraceLbl = ""
ref = {}
conn = Connection(dbName)
cursor0 = conn.cursor()
cursor = conn.cursor()
cmd0 = "select " +\
       "ts.node_count, " +\
       "p.avg " +\
       "from performance p, trace_set ts " +\
       "where ts.trace_set_id=p.trace_set_id and p.trace_set_id in " +\
       "(select trace_set_id from trace_set where group_id = " +\
       str(groupId) + ") " +\
       "and p.trace_label=\'TimeStep\'" +\
       "order by ts.node_count for read only with cs;"
cursor0.execute(cmd0)
row = cursor0.fetchone()
while (row):
    ref[str(row[0])] = row[1]
    row = cursor0.fetchone()

cmd = "select " +\
      "p.trace_set_id, " +\
      "ts.node_count, " +\
      "p.trace_label, " +\
      "p.avg, " +\
      "p.std_dev, " +\
      "p.min, " +\
      "p.max, " +\
      "p.count " +\
      "from performance p, trace_set ts " +\
      "where ts.trace_set_id=p.trace_set_id and p.trace_set_id in " +\
      "(select trace_set_id from trace_set where group_id = " +\
      str(groupId) + ") " +\
      "order by p.trace_label, ts.node_count for read only with cs;\n"
cursor.execute(cmd)
foo = cursor.fetchone()
print '# Fractional contributions to total timestep for trace_group_id ' +\
      str(groupId)
print "# column 1: node count"
print "# column 2: fraction of total time step"
while (foo):
    if str(foo[2]) != oldTraceLbl:
        print "\n"
        oldTraceLbl = str(foo[2])
        print "# results for " + str(oldTraceLbl)
    share = foo[3]/ref[str(foo[1])]
    print str(foo[1]) + "\t" +\
          str(share)
    foo = cursor.fetchone()


