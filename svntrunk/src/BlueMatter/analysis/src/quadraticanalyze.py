#!/usr/bin/env python

from QuadraticAnalysis import *
from runresults import *

import sys

testDir = "./"

if len(sys.argv) > 1:
    testDir = sys.argv[1]
    os.chdir(testDir)

runResults = RunResults('RunResults.txt')

qc = QuadraticConservation()

# now we can spool through each entry in this new dictionary to analyze
# each RunSet
for elt in runResults.keys():
    print "run set =", elt
    qc.runSet(runResults[elt])

qc.finalize()

