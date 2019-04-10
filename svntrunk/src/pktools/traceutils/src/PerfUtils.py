# This module contains subroutines that are used to analyze trace data

from DB2 import *
import sys
import os
import os.path
import pwd
import re

# ############################################################################
# fileName2Block
# takes a filename of the form block_id-job_id.extension and parses out
# block_id, job_id and returns them as the tuple (block_id, job_id)
# args: fileName
# ############################################################################

def fileName2Block(fileName):
    blk = re.compile(r'(\A.+)\-(\d+)\..*')
    m = re.search(blk,fileName)
    if m:
        return m.groups([1,2])
    return None

# ############################################################################
# blockInfo
# retrieves information about block_name (returns None if not found) from db2
# args: dbName, block_name
# ############################################################################

def blockInfo(dbName, blockName):
    conn = Connection(dbName);
    cursor = conn.cursor();
    cmd = "select px, py, pz from perf.blocks where block_id=\'" +\
          str(blockName) + "\'"
    cursor.execute(cmd)
    foo = cursor.fetchone();
    return foo

# ############################################################################
# groupDesc
# retrieves description from db2 for specified groupId
# Args: conn
#       groupId
# ############################################################################

def groupDesc(conn, groupId):
    cursor = conn.cursor();
    cmd = "select desc from perf.trace_group where group_id = " +\
          str(groupId)
    cursor.execute(cmd)
    foo = cursor.fetchone()
    return(foo[0])

# ############################################################################
# parseIdFile
# parses DEL file containing id information as produced by fxlog2bin
# 
# returns dictionary keyed by trace label where data items are tuples:
# (startId, finishId, desc)
# ############################################################################

def parseIdFile(idFile):
    idf = open(idFile, 'r')
    if not idf:
        print "Unable to open file", idFile
        sys.exit(-1)
    pat = re.compile(r'"?\s*,\s*"?')
    id = {}
    for line in idf:
        rec=pat.split(line, 4)
        curId = [-1, -1, ""]
        if id.has_key(rec[2]):
            curId=id[rec[2]]
        else:
            curId[2] = rec[3]
        if int(rec[1]) == 0:
            curId[0] = int(rec[0])
        else:
            curId[1] = int(rec[0])
        id[rec[2]] = curId
    return id

# ############################################################################
# makeFileList
# creates list of tuples (nodeCount, fileName) from file with
# whitespace delimited fields
# ############################################################################

def makeFileList(fileList):
    traceSet = []
    fl = open(fileList, 'r')
    if not fl:
        print "Unable to open file", fileList
        sys.exit(-1)
    ws = re.compile(r'\s+')
    for line in fl:
        print str(line)
        rec = ws.split(str(line))
        tuple = (rec[0], rec[1])
        traceSet.append(tuple)
    return traceSet

# ############################################################################
# makeNameValuePairs
# creates list of tuples containing name, value pairs from a file with
# colon (:) delimited records
# ############################################################################
def makeNameValuePairs(nameValueFile):
    dataList = []
    tl = open(nameValueFile, 'r')
    if not tl:
        print "Unable to open file", trcPtList
        sys.exit(-1)
    delim = re.compile(r':')
    for line in tl:
        rec = delim.split(str(line[0:len(line)-1]))
        foo = (rec[0], rec[1])
        dataList.append(foo)
    return dataList

# ############################################################################
# readNameValuePairs
# reads name value pairs (creates vector of tuples for particular
# trace_set_id from db2)
# args: conn, trace_set_id
# returns: list of tuples (name, value) derived from the
#          perf.trace_set_data table
# ############################################################################
def readNameValuePairs(conn, traceSetId):
    nvPairs = []
    cursor = conn.cursor()
    cmd = "select name, value from perf.trace_set_data where trace_set_id=" +\
          str(traceSetId) + " order by name for read only with ur"
    cursor.execute(cmd)
    foo = cursor.fetchone();
    while(foo):
        nv = (foo[0], foo[1])
        nvPairs.append(nv)
        foo = cursor.fetchone()
    return nvPairs

# ############################################################################
# makeLabelList
# creates list of trace points from file trcPtList
# ############################################################################
def makeLabelList(trcPtList):
    labelList = []
    tl = open(trcPtList, 'r')
    if not tl:
        print "Unable to open file", trcPtList
        sys.exit(-1)
    ws = re.compile(r'\s+')
    for line in tl:
        rec = ws.split(str(line))
        labelList.append(rec[0])
    return labelList

# ############################################################################
# This subroutine allows users process a single trace file and generate
# statistics
# args: timeFactor (conversion, typically 1000 for BG/L to give time in ns)
#       nodeCount
#       traceFileName
#
# Returns: Dictionary of tuples where index is trace identifier and tuple is
#          (avg, ....)
# ############################################################################

def singleTrace(timeFactor, nodeCount, traceFileName, minTimeStep, bracketId):
    base = os.path.splitext(traceFileName)[0]
    idNameBase = base + ".id.del"
    dataName = base + ".data.del"
    binName = base + ".bin"
    cmd = "fxlog2bin " + str(timeFactor) + " " + idNameBase + " " + base + " < " + traceFileName + " > /dev/null"
    rc = os.system(cmd)
    if (rc != 0):
        print "Error on return from: " + cmd
        sys.exit(-1)
    trcpts = parseIdFile(idNameBase)
    ts = os.popen2("timestamp2groupedintervalstats " + binName + " " +\
                   str(nodeCount) + " " + str(minTimeStep) +\
                   " " + str(bracketId), 't', 1)
    tsIn = ts[0]
    tsOut = ts[1]
    for trcpt in trcpts.keys():
        trcrec=trcpts[trcpt]
        print str(trcrec[0]), str(trcrec[1]), str(trcpt)
        tsIn.write(str(trcrec[0]) + " " + str(trcrec[1]) + " " +\
                   str(trcpt) + "\n")
        tsIn.flush()
    tsIn.close()
    result={}
    for line in tsOut:
        line.strip()
        print line
        # field[0] = nodeCount
        # field[1] = avg
        # field[2] = stddev
        # field[3] = min()
        # field[4] = max()
        # field[5] = minDev from avg value (computed for each ts)
        # field[6] = maxDev from avg value (computed for each ts)
        # field[7] = count()
        # field[8] = trace label
        # print "Line: ", line
        field = line.split()
        print "Fields: ", field
        key = field[8]
        result[key] = field
    tsOut.close()
    statsFile = base + ".stats"
    fd = open(statsFile, 'w')
    fd.write("Statistics for node count = " + str(nodeCount) + "\n")
    fd.write("%30s %12s %12s %12s %12s %12s %12s %12s\n" % ("label", "average", "std_dev", "min", "max", "minDev", "maxDev", "count"))
    print result.keys()
    for label in result.keys():
        rec=result[label]
        print rec
        fd.write("%30s %12g %12g %12g %12g %12g %12g %12g\n" % (label, float(rec[1]), float(rec[2]), float(rec[3]), float(rec[4]), float(rec[5]), float(rec[6]), float(rec[7])))
    fd.close()


# ############################################################################
# This subroutine allows users to append a group of related traces to an
# existing group within db2
# args: dbName (database name)
#       groupId
#       timeFactor (conversion, typically 1000 for BG/L to give time in ns)
#       traceSet (list of tuples of the form (nodeCount, traceFileName)
#
# Returns: nothing
# ############################################################################

def traceGroupAppend(dbName, groupId, timeFactor, traceSet):
    conn = Connection(dbName);
    cursor = conn.cursor();

    userId = pwd.getpwuid(os.getuid())[0].upper()

    for tset in traceSet:
        base = os.path.basename(tset[1])
        idNameBase = base + ".id.del"
        dataName = base + ".data.del"
        nameValueFile = base + ".nvp"
        cmd = "fxlog2bin " + timeFactor + " " + idNameBase + " " + base + " " + tset[1] + " < " + tset[1] + " > /dev/null"
        rc = os.system(cmd)
        if (rc != 0):
            print "Error on return from: " + cmd
            cursor.close()
            conn.close()
            sys.exit(-1)
        nameValue = makeNameValuePairs(nameValueFile)
        cmd = "insert into perf.trace_set (group_id, executable_id, node_count, bin_file) values (" + str(groupId) + ", -1, " + str(tset[0]) + ", \'" + os.path.abspath(base + ".bin") + "\' " + ")"
        # print cmd
        cursor.execute(cmd)
        cmd = "select trace_set_id from perf.trace_set where group_id = " + str(groupId) + " order by trace_set_id desc for read only with ur"
        cursor.execute(cmd)
        foo = cursor.fetchone()
        traceSetId = foo[0]
        # print "trace_set_id =",str(traceSetId),"for node_count=",str(tset[0])
        for nvp in nameValue:
            cmd = "insert into perf.trace_set_data (trace_set_id, name, value) values (" +\
                  str(traceSetId) + ", \'" + str(nvp[0]) + "\', \'" +\
                  str(nvp[1]) + "\')"
            print cmd
            cursor.execute(cmd)
        df = open(idNameBase)
        idName = idNameBase + ".withId"
        of = open(idName, 'w')
        for line in df:
            of.write(str(traceSetId) + ", " + line)
        of.close()
        df.close()
        cmd = "db2 \"connect to " + dbName + "\"; db2 \"import from " + idName + " of DEL method p (1,2,3,4,5) commitcount 5000 insert into perf.trace_id (trace_set_id, trace_id, trace_type, trace_label, trace_desc)\"; db2 connect reset "
        rc = os.system(cmd)
    cursor.close()
    conn.close()
    
# ############################################################################
# This subroutine allows users to process an entire group of related traces into
# their default schema in db2.  
# args: dbName (database name)
#       desc (description of trace group)
#       timeFactor (conversion, typically 1000 for BG/L to give time in ns)
#       traceSet (list of tuples of the form (nodeCount, traceFileName)
#
#
# Returns: groupId
# ############################################################################
def traceGroup(dbName, desc, timeFactor, traceSet):
    conn = Connection(dbName);
    cursor = conn.cursor();

    userId = pwd.getpwuid(os.getuid())[0].upper()
    cmd = "insert into perf.trace_group (creator, created, desc) values (\'" + userId + "\', current timestamp, \'" + desc + "\')"
    
    cursor.execute(cmd)
    
    cmd = "select group_id from perf.trace_group where creator = \'" + userId + "\' order by group_id desc for read only with ur"
    cursor.execute(cmd)
    foo = cursor.fetchone()
    groupId = foo[0]
    cursor.close()
    conn.close()
    # just call the append routine
    traceGroupAppend(dbName, groupId, timeFactor, traceSet)
    return groupId

# ############################################################################
# This subroutine creates a dictionary indexed by trace label that contains
# the start and finish ids of that trace label for all trace labels present
# in a specified trace set. The elements of the dictionary are tuples of the
# form (start_id, finish_id)
# args: conn -- db connection
#       traceSetId
#
# returns dictionary
# ############################################################################

def tracePointList(conn, traceSetId):
    label={}
    cursor=conn.cursor()
    cmd2 = "with st (start_id, trace_label) as " +\
           "(select trace_id, trace_label from perf.trace_id " +\
           "where trace_type = 0 and trace_set_id = " + str(traceSetId) + "), "+\
           "fi (finish_id, trace_label) as " +\
           "(select trace_id, trace_label from perf.trace_id " +\
           "where trace_type = 1 and trace_set_id = " + str(traceSetId) + ") " +\
           "select start_id, finish_id, st.trace_label " +\
           "from st, fi " +\
           "where st.trace_label = fi.trace_label " +\
           "order by st.trace_label for read only with ur"
    cursor.execute(cmd2)
    bar = cursor.fetchone()
    while bar:
        if not label.has_key(bar[2]):
            label[bar[2]]=(bar[0], bar[1])
        bar = cursor.fetchone()
    return label


# ############################################################################
# this routine processes a single trace file and generates "timing" diagrams
# for a list of labels (output to stdout)
# args: conn db2 connection object
#       trace_set_id
#       label (dictionary of trace labels containing (start_id, finish_id)
#       nBins (number of bins to use for distribution function)
#       syncLabel (label to be used to synchronize timestamp traces)
#
# Returns: Nothing
#
# ############################################################################

def traceSetTimingDiagram(conn, traceSetId, label, nBins, syncId):
    cmd = "select node_count, bin_file from perf.trace_set where trace_set_id="+\
          str(traceSetId)
    cursor=conn.cursor()
    cursor.execute(cmd)
    foo=cursor.fetchone()
    binFile=foo[1]
    nodeCount=foo[0]
    if not foo:
        print >>sys.stderr, "traceSetDistribution: error selecting trace_set record"
        sys.exit(-1)
    sys.stdout.write("# Distribution Functions for trace set id: " + str(traceSetId) + ", Node count = " + str(nodeCount) + "\n#\n")
    ts = os.popen2("distributiontraces " + str(binFile) + " " +\
                   str(nodeCount) + " " + str(syncId) + " " +\
                   str(nBins), 't', 1)
    tsIn = ts[0]
    tsOut = ts[1]
    for l in label.keys():
        sys.stderr.write(str(label[l][0]) + " " + str(label[l][1]) + " " +  str(l))
        tsIn.write(str(label[l][0]) + " " + str(label[l][1]) +\
                   " " + str(l) + "\n")
        tsIn.flush()
    tsIn.close()
    for line in tsOut:
        print line.strip()
    tsOut.close()


# ############################################################################
# this routine processes a single trace file and generates the distribution
# function for a list of labels
# args: dbName
#       conn db2 connection object
#       trace_set_id
#       timeStep (time step of data for which a distribution function
#                 over nodes is to be calculated)
#       label (dictionary of trace labels containing (start_id, finish_id)
#       nBins (number of bins to use for distribution function)
#       idx (index to start at for labeling traces)
#       fd (filehandle for output)
#
# Returns: Dictionary keyed by (nodeCount, trace_label, timeStep, percentile[i]) containing
#          tuples comprising fields 1-4 defined as follows:
#          field[1] = distribution[i]
#          field[2] = distribution[i]-distribution[i-1]
#          field[3] = distribution[i]/total
#          field[4] = (distribution[i]-distribution[i-1])/total
#
# ############################################################################

def traceSetDistribution(dbName, conn, traceSetId, timeStep, label, nBins, idx, fd):
    cmd = "select node_count, bin_file from perf.trace_set where trace_set_id="+\
          str(traceSetId)
    cursor=conn.cursor()
    cursor.execute(cmd)
    foo=cursor.fetchone()
    binFile=foo[1]
    nodeCount=foo[0]
    if not foo:
        print >>sys.stderr, "traceSetDistribution: error selecting trace_set record"
        sys.exit(-1)
    fd.write("# Distribution Functions for trace set id: " + str(traceSetId) + ", Node count = " + str(nodeCount) + "\n#\n")
    ts = os.popen2("tracepoint2distribution " + str(binFile) + " " +\
                   str(nodeCount) + " " + str(timeStep) + " " + str(nBins), 't', 1)
    tsIn = ts[0]
    tsOut = ts[1]
    for l in label.keys():
        print str(label[l][0]), str(label[l][1]), str(l)
        tsIn.write(str(label[l][0]) + " " + str(label[l][1]) +\
                   " " + str(l) + "\n")
        tsIn.flush()
    tsIn.close()
    currLabel = None;
    currIndex = idx;
    for line in tsOut:
        line.strip()
        # field[0] = percentile[i]
        # field[1] = distribution[i]
        # field[2] = distribution[i]-distribution[i-1]
        # field[3] = distribution[i]/total
        # field[4] = (distribution[i]-distribution[i-1])/total
        # field[5] = timeStep
        # field[6] = trace label
        # print "Line: ", line
        field = line.split()
        # print "Fields: ", field
        if currLabel and (currLabel == field[6]):
            fd.write(line.strip() + "\n")
        else:
            if (currLabel):
                fd.write("\n\n")
            currLabel = field[6]
            fd.write("#Index " + str(currIndex) + ": " + str(currLabel) + "\n")
            fd.write(line.strip() + "\n")
            currIndex = currIndex + 1
    tsOut.close()
    fd.write("\n\n")
    return currIndex


# ############################################################################
# this routine processes a group of trace files and generates distribution
# functions for each start/finish trace point pair at a specified time-step
# args: dbName
#       groupId
#       timeStep
#       nBins (for distribution function)
# ############################################################################

def traceGroupDistribution(dbName, groupId, timeStep, nBins, outFileName):
    result = {}
    nodeCount = []
    fd = open(outFileName, 'w')
    traceSetIdList = []
    conn = Connection(dbName)
    cursor = conn.cursor()
    cmd = "select trace_set_id from perf.trace_set where group_id =" +\
          str(groupId) +\
          " order by node_count, trace_set_id for read only with ur"
    cursor.execute(cmd)
    fd.write("# Distribution functions for group id = " + str(groupId) + "\n")
    fd.write("# Distribution functions reported using " + str(nBins) + " bins\n")
    fd.write("# column 1: percentile[i] (value)\n")
    fd.write("# column 2: distribution[i]\n")
    fd.write("# column 3: distribution[i]-distribution[i-1]\n")
    fd.write("# column 4: distribution[i]/total\n")
    fd.write("# column 5: (distribution[i]-distribution[i-1])/total\n")
    fd.write("# column 6: timeStep\n")
    fd.write("# column 7: trace label\n#\n")
    
    foo = cursor.fetchone()
    idx = 0;
    while (foo):
        traceSetId = foo[0]
        traceSetIdList.append(traceSetId)
        label=tracePointList(conn, traceSetId)
        idx = traceSetDistribution(dbName, conn, traceSetId, timeStep, label, nBins, idx, fd)
        foo = cursor.fetchone()
    fd.close()

# ############################################################################
# this routine processes a single trace file and generates statistics
# which are loaded into db2
# args: dbName
#       conn db2 connection object
#       trace_set_id
#       minTimeStep (when to start computing statistics)
#       skipLastNSteps (how many steps to skip at the end)
#       label (dictionary of trace labels containing (start_id, finish_id)
#       bracketName (by default None which causes timestamp2intervalstats
#       to be invoked rather than timestamp2grouped intervalstats)
# ############################################################################

def traceSetMultiStats(dbName, conn, traceSetId, minTimeStep, skipLastN, label, bracketName):
    delFile = "statistics_" + str(traceSetId) + ".del"
    result = {}
    cmd = "select node_count, bin_file from perf.trace_set where trace_set_id="+\
          str(traceSetId)
    cursor=conn.cursor()
    cursor.execute(cmd)
    foo=cursor.fetchone()
    binFile=foo[1]
    nodeCount=foo[0]
    if not foo:
        print >>sys.stderr, "traceSetMultiStats: error selecting trace_set record"
        sys.exit(-1)
    if bracketName:
        cmd = "select trace_id from perf.trace_id where trace_set_id = " +\
              str(traceSetId) + " and trace_type=0 and trace_label =\'" +\
              bracketName + "\' for read only with ur"
        cursor.execute(cmd)
        bar = cursor.fetchone()
        if bar:
            bracketId = bar[0]
        else:
            print >>sys.stderr, "No bracketId found for", bracketName
            sys.exit(-1)
        ts = os.popen2("timestamp2groupedintervalstats " + str(binFile) + " " +\
                       str(nodeCount) + " " + str(minTimeStep) + " " +\
                       str(skipLastN) + " " + str(bracketId), 't', 1)
    else:
        ts = os.popen2("timestamp2groupedintervalstats " + str(binFile) + " " +\
                       str(nodeCount) + " " + str(minTimeStep) + " " +\
                       str(skipLastN), 't', 1)
        
    tsIn = ts[0]
    tsOut = ts[1]
    for l in label.keys():
        print str(label[l][0]), str(label[l][1]), str(l)
        tsIn.write(str(label[l][0]) + " " + str(label[l][1]) +\
                   " " + str(l) + "\n")
        tsIn.flush()
    tsIn.close()
    for line in tsOut:
        line.strip()
        # field[0] = nodeCount
        # field[1] = avg
        # field[2] = stddev
        # field[3] = min()
        # field[4] = max()
        # field[5] = minDev from avg value (computed for each ts)
        # field[6] = maxDev from avg value (computed for each ts)
        # field[7] = count()
        # field[8] = trace label
        # print "Line: ", line
        field = line.split()
        # print "Fields: ", field
        key = (field[8])
        result[key] = field
    tsOut.close()

    delFile = "statistics_" + str(traceSetId) + ".del"
    fd = open(delFile, 'w')
    # column 1: trace_set_id
    # column 2: trace_label
    # column 3: average
    # column 4: std. dev
    # column 5: min()
    # column 6: max()
    # column 7: minDev (deviation from average value computed for each ts)
    # column 8: maxDev (deviation from average value computed for each ts)
    # column 9: count()
    for l in label.keys():
        # fd.write("# results for " + str(l) + "\n")
        fd.write(str(traceSetId) + ", " +\
                 "\"" + str(result[l][8]) + "\"" + ", " +\
                         str(result[l][1]) + ", " +\
                         str(result[l][2]) + ", " +\
                         str(result[l][3]) + ", " +\
                         str(result[l][4]) + ", " +\
                         str(result[l][5]) + ", " +\
                         str(result[l][6]) + ", " +\
                         str(result[l][7]) + "\n")
    fd.close()
    imp = os.popen("db2 -t", 'w');
    imp.write("connect to " + str(dbName) + ";" + "\n")
    imp.write("import from " + str(delFile) +\
              " of del method p (1,2,3,4,5,6,7,8,9) commitcount 100 " +\
              "insert into perf.performance " +\
              "(trace_set_id, trace_label, avg, std_dev, min, max, min_dev, max_dev, count);" +\
              "\n")
    imp.write("connect reset;\n")
    rc = imp.close()

# ############################################################################
# this routine processes a group of trace files and generates statistics
# which are loaded into db2
# args: dbName
#       groupId
#       minTimeStep (when to start computing statistics)
#       bracketName (typically TimeStep)
# ############################################################################

def traceGroupMultiStatsDb2(dbName, groupId, outFileBase, minTimeStep, skipLastN, bracketName):
    delFile = outFileBase + '.del'
    result = {}
    nodeCount = []
    traceSetIdList = []
    conn = Connection(dbName)
    cursor = conn.cursor()
    cursor2 = conn.cursor()
    cmd = "select trace_set_id from perf.trace_set where group_id =" +\
          str(groupId) +\
          " order by node_count, trace_set_id for read only with ur"
    cursor.execute(cmd)
    foo = cursor.fetchone()
    while (foo):
        traceSetId = foo[0]
        traceSetIdList.append(traceSetId)
        label=tracePointList(conn, traceSetId)
        traceSetMultiStats(dbName, conn, traceSetId, minTimeStep, skipLastN, label, bracketName)
        foo = cursor.fetchone()

# ############################################################################
# db2results
# Creates a results object containing statistical data for a specified
# trace_group_id about a specified list of trace labels
# The data are provided in a file format suitable for use by gnuplot
# args: conn
#       groupId
#       labels
#
# Returns: dictionary indexed by trace label that provides statistical
#          information as a function of node count.  Each dictionary
#          entry is a tuple comprising: node count, average, stddev,
#          min, max, count (number of data points contributing to stats),
#
# ############################################################################

def db2results(conn, groupId, labels):
    cur=conn.cursor()
    result = {}
    for label in labels:
        result[label]=[]
        cmd="select perf.performance.trace_set_id, node_count, avg, std_dev, min, max, count, min_dev, max_dev from perf.trace_set, perf.performance "+\
             "where trace_label='" + str(label) + "' "+\
             "and perf.trace_set.trace_set_id=perf.performance.trace_set_id "+\
             "and perf.trace_set.trace_set_id "+\
             "in (select trace_set_id from perf.trace_set where group_id="+\
             str(groupId) + ") order by node_count"
        # print cmd
        cur.execute(cmd)
        foo=cur.fetchone()
        while foo:
            # print str(foo)
            result[label].append(foo)
            foo=cur.fetchone()
    return(result)

# ############################################################################
# db2dat
# Creates a single data file (usable by gnuplot) with data
# from a trace_group_id and a list of labels
# args: conn (db2 connection object)
#       groupId
#       outFileNameBase (base name for output file)
#       labels (list of labels)
# ############################################################################

def db2dat(conn, groupId, outFileBase, labels):
    datFile = "stdout.dat"
    if outFileBase == '-':
        fd=sys.stdout
    else:
        datFile = outFileBase + ".dat"
        fd = open(datFile, 'w')
    fd.write('# Statistics for grouped trace points from trace_group_id ' +\
             str(groupId) + "\n")
    desc = groupDesc(conn, groupId)
    cursor = conn.cursor()
    cursor2 = conn.cursor()
    cmd = "select trace_set_id from perf.trace_set where group_id ="+\
       str(groupId) +\
       " order by node_count, trace_set_id for read only with ur"
    cursor.execute(cmd)
    fd.write("# TraceSetId:Name:Value\n")
    foo = cursor.fetchone()
    while (foo):
        traceSetId = foo[0]
        nvPair = readNameValuePairs(conn, traceSetId)
        for nv in nvPair:
            fd.write('# ' + str(traceSetId) + ":" + str(nv[0]) + ":" +\
                     str(nv[1]) + "\n")
        foo = cursor.fetchone()
    fd.write("#\n")

    fd.write("# All times are in nanoseconds\n")
    fd.write("# " + str(desc) + "\n")
    fd.write("# column 1: trace set id\n")
    fd.write("# column 2: node count\n")
    fd.write("# column 3: average\n")
    fd.write("# column 4: std. dev.\n")
    fd.write("# column 5: min()\n")
    fd.write("# column 6: max()\n")
    fd.write("# column 7: min_dev (from avg, computed for each ts)\n")
    fd.write("# column 8: max_dev (from avg, computed for each ts)\n#\n#\n")
    fd.write("# column 9: count()\n")
    result = db2results(conn, groupId, labels)
    idx = 0
    sortedKeys = result.keys()
    sortedKeys.sort()
    for label in sortedKeys:
        fd.write("# index = " + str(idx) + " for label " + str(label) +\
                 " \n")
        idx = idx + 1
        for foo in result[label]:
            fd.write(str(foo[0]) + " \t")
            fd.write(str(foo[1]) + " \t")
            fd.write(str(foo[2]) + " \t")
            fd.write(str(foo[3]) + " \t")
            fd.write(str(foo[4]) + " \t")
            fd.write(str(foo[5]) + " \t")
            fd.write(str(foo[7]) + " \t")
            fd.write(str(foo[8]) + " \t")
            fd.write(str(foo[6]) + "\n")
        fd.write("\n\n")
    fd.close()
    return datFile

# ############################################################################
# Creates a pair of files:gnuplot command file and data file
# from a trace_group_id and a list of labels
# args: dbName
#       groupId
#       outFileNameBase (base name for output file)
#       labels (list of labels)
# ############################################################################

def scalability(conn, groupId, outFileBase, labels):
    if outFileBase == '-':
        fg=sys.stdout
        datFile = "stdout.dat"
    else:
        gpFile = outFileBase + ".gp"
        fg = open(gpFile, 'w')
    datFile = db2dat(conn, groupId, outFileBase, labels)
    fg.write('# Statistics for grouped trace points from trace_group_id ' + str(groupId) +\
             "\n")
    fg.write("# data read from file\n")
    result = db2results(conn, groupId, labels)
    fg.write("reset\n")
    fg.write("set logscale xy\n")
    fg.write("set key outside\n")
    for i in range(1, len(result.keys())+1):
        fg.write("set linestyle " + str(i) + "\n")
    fg.write("set xlabel \'Node Count\'\n")
    fg.write("set ylabel \'Elapsed Time (nsec.)\'\n")
    fg.write("plot ")
    i=1
    sortedKeys = result.keys()
    sortedKeys.sort()
    for label in sortedKeys:
        fg.write("\'" + datFile +"\' index " + str(i-1) + " using 2:3:5:6 title \'" +\
                 label + "\' with errorbars linestyle " + str(i))
        fg.write(",\\\n")
        i=i+1
    i=1
    for label in sortedKeys:
        fg.write("\'" + datFile + "\' index " + str(i-1) + " using 2:3 notitle with lines linestyle " + str(i))
        if (i == len(result.keys())):
            fg.write("\n")
        else:
            fg.write(",\\\n")
        i=i+1
    fg.close()

# ############################################################################
# Creates a mapping where the array index is the node count and the data are
# tuples of statistical information for a specific label.  All these data are
# gathered for a specific trace_group_id and database connection
# args: dbConn
#       groupId
#       label
# returns: mapping (dictionary) indexed by node_count where the data are
#          tuples of the form (avg, std_dev, max, min, max_dev, min_dev, count)
# ############################################################################

def statsForLabel(dbConn, groupId, label):
    cur=dbConn.cursor()
    cmd="select node_count, avg, std_dev, min, max, min_dev, max_dev, count from perf.trace_set, perf.performance "+\
         "where trace_label='" + str(label) + "' "+\
         "and perf.trace_set.trace_set_id=perf.performance.trace_set_id "+\
         "and perf.trace_set.trace_set_id "+\
         "in (select trace_set_id from perf.trace_set where group_id="+\
         str(groupId) + ") order by node_count"
    # print cmd
    cur.execute(cmd)
    foo=cur.fetchone()
    while foo:
        # print str(foo)
        key=int(foo[0])
        data = (float(foo[1]), float(foo[2]), float(foo[3]),\
                float(foo[4]), float(foo[5]), float(foo[6]), int(foo[7]))
        result[key]=data
        foo=cur.fetchone()
    return result

