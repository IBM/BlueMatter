import sys
from var2 import *

if len(sys.argv) < 3:
    print argv[0], "runDir maxWidth"
    sys.exit(-1)
var2(sys.argv[1], int(sys.argv[2]))

