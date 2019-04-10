#! /usr/bin/env python

# creates a gnuplot-readable data file (sent to stdout) from a dataset in db2
# the data file is of the form created by trace_group_multi_stats.py
# command line args: dbName trace_group_id minTimeStep <bracketName =TimeStep__0>


from DB2 import *
import sys
import os

if len(sys.argv) < 3:
    print sys.argv[0], "dbName trace_group_id"
    sys.exit(-1)
dbName = sys.argv[1]
groupId = sys.argv[2]
oldTraceLbl = ""
conn = Connection(dbName)
cursor = conn.cursor()
cursor2 = conn.cursor()
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
      "(select trace_set_id from trace_set where group_id =76) " +\
      "order by p.trace_label, ts.node_count for read only with cs;\n"
cursor.execute(cmd)
foo = cursor.fetchone()
print '# Statistics for grouped trace points from trace_group_id ' +\
      str(groupId)
print "# column 1: node count"
print "# column 2: average"
print "# column 3: std. dev."
print "# column 4: min()"
print "# column 5: max()"
print "# column 6: count()"

while (foo):
    if str(foo[2]) != oldTraceLbl:
        print "\n"
        oldTraceLbl = str(foo[2])
        print "# results for " + str(oldTraceLbl)
    print str(foo[1]) + "\t" +\
          str(foo[3]) + "\t" +\
          str(foo[4]) + "\t" +\
          str(foo[5]) + "\t" +\
          str(foo[6]) + "\t" +\
          str(foo[7])
    foo = cursor.fetchone()


