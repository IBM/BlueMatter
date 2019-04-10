#!/usr/bin/ksh

# command line parameter is file name of .out file
#
# output goes to stdout (REMEMBER THIS!)
#
# $1 is input file name (.out file)
# $2 is clock speed in MHz
# $3 is base output fileName

fxlog2del $2 $3.id.del $3.data.del < $1 > /dev/null

sort -k1,1n -k2,2n -k3,3n traceData.del | trcdel2bin
