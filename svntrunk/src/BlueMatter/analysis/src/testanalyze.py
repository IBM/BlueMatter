#!/usr/bin/env python
# driver for new multianalyze.py and analyzeconservation.py 

from multianalyze import *
from analyzeconservation import *

import sys

if len(sys.argv) < 2:
    print sys.argv[0], "runDir"
    sys.exit(-1)

os.chdir(sys.argv[1])
corrTime = 400*6.25e-5 # correlation time in picoseconds
a1 = AnalyzeConservation(corrTime)
multianalyze(a1)
