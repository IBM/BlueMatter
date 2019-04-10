#!/usr/bin/env python
#
# utility to generate .cpp and .exe from executable id
# command line args: dbName exeId dest optionPkg

import string
import os
import sys
import bgutils
from DB2 import *

RunningLog = []

def DumpAndExit(msg):
  global RunningLog
  print ""
  print msg + "  Details follow..."
  print ""
  for l in RunningLog:
    print l.strip()  
  print ""
  print ""
  sys.exit(-1)
  return

def AddLog(title, log):
  global RunningLog
  header = "============== %s ==============" % title
  RunningLog = RunningLog + [header] + log + [header]
  return


BG = bgutils.installbase() + 'usr/opt/bluegene'
BG_SETUP = BG + '/bin/setup'
BG_UTIL = BG + '/bin/util'

# Make the generated files readable by others and the web server
os.umask(022)

if len(sys.argv) < 5:
    print sys.argv[0], "dbName exeId dest optionPkg"
    sys.exit(-1)

dbName = sys.argv[1]
exeId = sys.argv[2]
dest = sys.argv[3]
optionPkg = sys.argv[4]

cppfile = dest + '.cpp'
exefile = dest + '.smp.aix.exe'

foo = bgutils.exeInfo(dbName, exeId)
sysId = foo[0]
ctpId = foo[1]

pltInfo = bgutils.PlatformInfo(dbName)
foo = pltInfo.choices()[optionPkg]
implId = foo[0]
pltId = foo[1]

pdb2 =  'java com.ibm.bluematter.db2probspec.ProbspecgenDB2 '
pdb2com = pdb2 + str(sysId) + " " + str(ctpId) + " " + str(implId) + " " + str(pltId) + " " + dest + " -v"
print pdb2com

wcf = os.popen(pdb2com)
PSPLines = wcf.readlines()
wcf.close()
AddLog("ProbspecDB2", PSPLines)


####  Check success of cpp transliteration

if os.access(cppfile,os.R_OK) == 0:
    DumpAndExit("ERROR: ProbspecDB2 did not generate the cpp")

#########################################################
#                 compile_msd
#########################################################
msd =  'time java com.ibm.bluematter.utils.CompileMSD '
msdcom = msd + dest + ".cpp "
print msdcom

wcf = os.popen(msdcom)
COMLines = wcf.readlines()
wcf.close()
AddLog("compileMSD", COMLines)

nlines = len(COMLines)
newExeId = -1
for i in range(nlines):
    thisline = COMLines[i].split()
#    print thisline
    for j in range(len(thisline)):
	if(thisline[j].strip() == 'Executable'):
	    exeId = int(thisline[j+2])
	    print "Executable ID::" + str(exeId)

print "newExeId: " + str(newExeId)
if newExeId == -1:
    DumpAndExit("ERROR:: Compilation failed, sorry pal :(")

####  Check success of cpp compilation
print 'EXE name : ' + exefile
print "%s successfully created by xml2exe.py" % exefile

sys.exit( 0 )
