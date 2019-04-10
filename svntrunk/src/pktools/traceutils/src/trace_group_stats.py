#! /usr/bin/env python

from DB2 import *
import sys
import os
import os.path
import pwd

# processes a group trace files and generates statistics
# command line args: dbName, trace_group_id outFileName

if len(sys.argv) < 5:
    print sys.argv[0], "dbName trace_group_id outFileBaseName minTimeStep"
    sys.exit(-1)
dbName = sys.argv[1]
groupId = sys.argv[2]
outFileBase = sys.argv[3]
minTimeStep = sys.argv[4]
dataFile = outFileBase + '.dat'
gpFile = outFileBase + '.gp'
result = {}
nodeCount = []
label = []
label2 = []
conn = Connection(dbName)
cursor = conn.cursor()
cursor2 = conn.cursor()
cmd = "select trace_set_id, node_count, bin_file from perf.trace_set where group_id =" + groupId
cursor.execute(cmd)
foo = cursor.fetchone()
while (foo):
    if (nodeCount.count(foo[1]) == 0):
        nodeCount.append(foo[1])
    ts = os.popen2("timestamp2intervalstats " + str(foo[2]) + " " +\
                  str(foo[1]) + " " + str(minTimeStep), 't', 1)
    tsIn = ts[0]
    tsOut = ts[1]
    cmd2 = "with st (start_id, trace_label) as " +\
           "(select distinct trace_id, trace_label from perf.trace_id " +\
           "where trace_type = 0 and trace_set_id = " + str(foo[0]) + "), " +\
           "fi (finish_id, trace_label) as " +\
           "(select distinct trace_id, trace_label from perf.trace_id " +\
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
        tslabel = "TS_from_" + bar[2]
        if label2.count(tslabel) == 0:
            label2.append(tslabel)
        tsIn.write(str(bar[0]) + " " + str(bar[1]) + " " + str(bar[2]) + "\n")
        tsIn.flush()
        tsIn.write(str(bar[0]) + " " + str(bar[0]) + " " + tslabel + "\n")
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
fd.write('# Statistics for trace points from trace_group_id ' + str(groupId) +\
         "\n")
fg.write('# Statistics for trace points from trace_group_id ' + str(groupId) +\
         "\n")
fd.write("# column 1: node count\n")
fd.write("# column 2: average\n")
fd.write("# column 3: std. dev.\n")
fd.write("# column 4: min()\n")
fd.write("# column 5: max()\n")
fd.write("# column 6: count()\n")
nodeCount.sort()
label.sort()
label2.sort()
label = label + label2
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
