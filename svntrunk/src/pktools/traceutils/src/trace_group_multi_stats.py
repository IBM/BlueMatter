#! /usr/bin/env python

from DB2 import *
import sys
import os
import os.path
import pwd

# processes a group of trace files and generates statistics
# command line args: dbName trace_group_id outFileNameBase minTimeStep <bracketName = TimeStep__0>

bracketName = "TimeStep__0"
if len(sys.argv) < 5:
    print sys.argv[0], "dbName trace_group_id outFileBaseName minTimeStep <bracketName = TimeStep__0>"
    sys.exit(-1)
dbName = sys.argv[1]
groupId = sys.argv[2]
outFileBase = sys.argv[3]
minTimeStep = sys.argv[4]
if len(sys.argv) > 5:
    bracketName = sys.argv[5]
dataFile = outFileBase + '.dat'
gpFile = outFileBase + '.gp'
result = {}
nodeCount = []
label = []
conn = Connection(dbName)
cursor = conn.cursor()
cursor2 = conn.cursor()
cmd = "select trace_set_id, node_count, bin_file from trace_set where group_id =" + groupId
cursor.execute(cmd)
foo = cursor.fetchone()
while (foo):
    cmd2 = "select trace_id from trace_id where trace_set_id = " +\
           str(foo[0]) + " and trace_type=0 and trace_label =\'" + bracketName +\
           "\' group by trace_id"
    cursor2.execute(cmd2)
    bar = cursor2.fetchone()
    if bar:
        bracketId = bar[0]
    else:
        print "No bracketId found for", bracketName
        sys.exit(-1)
    if (nodeCount.count(foo[1]) == 0):
        nodeCount.append(foo[1])
    ts = os.popen2("timestamp2groupedintervalstats " + str(foo[2]) + " " +\
                  str(foo[1]) + " " + str(minTimeStep) + " " + str(bar[0]), 't', 1)
    tsIn = ts[0]
    tsOut = ts[1]
    cmd2 = "with st (start_id, trace_label) as " +\
           "(select distinct trace_id, trace_label from trace_id " +\
           "where trace_type = 0 and trace_set_id = " + str(foo[0]) + "), " +\
           "fi (finish_id, trace_label) as " +\
           "(select distinct trace_id, trace_label from trace_id " +\
           "where trace_type = 1 and trace_set_id = " + str(foo[0]) + ") " +\
           "select start_id, finish_id, st.trace_label " +\
           "from st, fi " +\
           "where st.trace_label = fi.trace_label " +\
           "order by st.trace_label"
    cursor2.execute(cmd2)
    bar = cursor2.fetchone()
    while bar:
        if label.count(bar[2]) == 0:
            label.append(bar[2])
        tsIn.write(str(bar[0]) + " " + str(bar[1]) + " " + str(bar[2]) + "\n")
        tsIn.flush()
        bar = cursor2.fetchone()
    tsIn.close()
    for line in tsOut.xreadlines():
        line.strip()
        field = line.split()
        key = (field[6], int(field[0]))
        result[key] = field
    tsOut.close()
    foo = cursor.fetchone()
fd = open(dataFile, 'w')
fg = open(gpFile, 'w')
fd.write('# Statistics for grouped trace points from trace_group_id ' + str(groupId) +\
         "\n")
fg.write('# Statistics for grouped trace points from trace_group_id ' + str(groupId) +\
         "\n")
fd.write("# column 1: node count\n")
fd.write("# column 2: average\n")
fd.write("# column 3: std. dev.\n")
fd.write("# column 4: min()\n")
fd.write("# column 5: max()\n")
fd.write("# column 6: count()\n")
nodeCount.sort()
label.sort()
for l in label:
    fd.write("# results for " + str(l) + "\n")
    for nc in nodeCount:
        key = (l, nc)
        if result.has_key(key):
            r = result[key]
            fd.write(result[key][0] + "\t" +\
                     result[key][1] + "\t" +\
                     result[key][2] + "\t" +\
                     result[key][3] + "\t" +\
                     result[key][4] + "\t" +\
                     result[key][5] + "\n")
    fd.write("\n\n")
fd.close()
fg.write("reset\n")
fg.write("set logscale xy\n")
fg.write("set key outside\n")
for i in range(1, len(label)+1):
    fg.write("set linestyle " + str(i) + "\n")
fg.write("set xlabel \'Node Count\'\n")
fg.write("set ylabel \'Elapsed Time (pclocks)\'\n")
fg.write("plot ")
for i in range(len(label)):
    fg.write("\'" + dataFile +"\' index " + str(i) + " using 1:2:4:5 title \'" +\
             label[i] + "\' with errorbars linestyle " + str(i+1) +",\\\n")
for i in range(len(label)):
    ip = i+1
    fg.write("\'" + dataFile +"\' index " + str(i) + " using 1:2 notitle with lines linestyle " + str(ip))
    if (i+1 == len(label)):
        fg.write("\n")
    else:
        fg.write(",\\\n")
fg.close()
print "label: ", label
print "node count: ", nodeCount
print "result:", result
