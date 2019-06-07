#!/usr/bin/env python
from __future__ import print_function
import sys
import os

if len(sys.argv) < 2:
    print()
    print('Usage: hpp2msd.py <hpp name without extension>')
    print()
    sys.exit(1)

hppname = sys.argv[1] + ".hpp"
msdname = sys.argv[1] + ".msd"
msddir = " ${SANDBOXBASE}/export/rios_aix_4/usr/include/BlueMatter/"
msdpref = msddir + "msdprefix.hpp "
msdsuf = msddir + "msdsuffix.hpp "
os.system("cat " + msdpref  + hppname +  msdsuf + " >msd.cpp");

print()
print("************* compiling " + hppname + " ****************")
print()

cargs = '-g -DPK_AIX -DPK_SMP -DNO_PK_PLATFORM -o msd.exe -I${SANDBOXBASE}/export/rios_aix_4/usr/include '

os.system( "xlC_r " + cargs + " msd.cpp"); 

os.system("msd.exe");
os.system("mv output.msd " + msdname);






