from __future__ import print_function
# takes a data file suitable for a plotting program like gnuplot
# in which the columns are whitespace delimited and puts out lines
# suitable for use in a LaTeX tabular environment (fields separated by
# tab (&) characters and ending with \\
# input lines beginning with # are ignored
# Eventually, this utility should support formatting only selected
# columns specified by command line parameters

import re
import sys

if len(sys.argv) < 2:
    print(sys.argv[0], "fileName <col1> <col2> ... <colN>")
    sys.exit(-1)

fname = sys.argv[1]

colList = []
for i in range(len(sys.argv)-2):
    colList.append(int(sys.argv[i+2])-1)

cols = []
commentLine = re.compile(r'^\#')
ws = re.compile(r'\s+')
floatPat = re.compile(r'(-?\d+\.\d+)[eE](\+)(-?)0*(\d+)')
inFile = open(fname, 'r')
for i in inFile.xreadlines():
    i=i.lstrip()
    i=i.rstrip()
    m = commentLine.match(i)
    if m:
        continue
    curCols = ws.split(i)
    if len(colList) == 0:
        colList = range(len(curCols))

    for j in range(len(curCols)):
        cols.append(curCols[j])

    for j in range(len(colList)):
        if j > 0:
            print("\t&", end=' ')
        print('$', end=' ')
        x = curCols[colList[j]];
        y = re.sub(floatPat, r'\1\\times 10^{\3\4}', x)
        print(y, end=' ')
        print('$', end=' ')
        # print curCols[colList[j]],
    print(" \\\\")
    
