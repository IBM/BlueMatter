#!/usr/bin/env python

import string
import os
import sys
import bgutils

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



if len(sys.argv) < 4:
    print
    print 'Usage: xml2exe.py xmlname rtpname outputtag <BuildMode> <BuildOpts>'
    print '   Program will produce <outputtag>.msd and <outputtag>.smp.aix.exe'
    sys.exit(-1)

xmlname = sys.argv[1]
rtpname = sys.argv[2]
dest = sys.argv[3]

buildmode = "Default"
if len(sys.argv) > 4:
  buildmode = sys.argv[4]

buildopts = "Default"
if len(sys.argv) > 5:
  buildopts = sys.argv[5]

if os.access(xmlname,os.R_OK) == 0:
   print 'cannot read ' + xmlname
   sys.exit(-1)
if os.access(rtpname,os.R_OK) == 0:
   print 'cannot read ' + rtpname
   sys.exit(-1)

#SB = os.environ['SANDBOXBASE']
#BG = SB + '/inst.images/rios_aix_4/shipdata/usr/opt/bluegene'
BG = bgutils.installbase() + 'usr/opt/bluegene'
BG_SETUP = BG + '/bin/setup'
BG_UTIL = BG + '/bin/util'

# Make the generated files readable by others and the web server
os.umask(022)


#########################################################
#                     xml2db2
#########################################################
xmlcom = "perl " + BG_SETUP + "/xml2db2.pl " + xmlname 
print xmlcom

wcf = os.popen(xmlcom)
XMLLines = wcf.readlines()
wcf.close()
AddLog("xml2db2", XMLLines)

nlines = len(XMLLines)
for i in range(nlines):
    thisline = XMLLines[i].split()
#    print thisline
    for j in range(len(thisline)):        
        if(thisline[j] == 'SystemId:'):
            SysId = thisline[j+1]
	    print "System ID: " + SysId
        if(thisline[j] == 'ERROR:'):
	    DumpAndExit("ERROR:  xml2db2 returned an error.  Please check the XML.")
        if(thisline[j] == 'SUCCESS:'):
            print XMLLines[i]
            
#########################################################
msdfile = dest + '.msd'
exefile = dest + '.smp.aix.exe'
rmmsd = 'rm -f ' + msdfile
rmexe = 'rm -f ' + exefile

#### Kill previous msd and exe files
os.system(rmmsd)
os.system(rmexe)

#########################################################
#                     rtp2db2
#########################################################
rtp2 =  'java com.ibm.bluematter.utils.Rtp2db2 '
rtp2com = rtp2 + rtpname
print rtp2com

wcf = os.popen(rtp2com)
RTPLines = wcf.readlines()
wcf.close()
AddLog("rtp2db2", RTPLines)

nlines = len(RTPLines)
CtpId = -1
for i in range(nlines):
    thisline = RTPLines[i].split()
#    print thisline
    for j in range(len(thisline)):
	if(thisline[j] == 'CTP_ID:'):
	    CtpId = thisline[j+1]
	    print "CTP ID: " + CtpId

if CtpId == -1:
    DumpAndExit("ERROR:  rtp2db2 did not return a CtpId")

#########################################################
#               ProbspecDB2
#########################################################
cmd = " cat " + BG_SETUP + "/" + buildmode + ".ref"
wcf = os.popen(cmd)
XMLLines = wcf.readlines()
wcf.close()
AddLog("xml2db2", XMLLines)

thisline = XMLLines[0].split()
implId = thisline[0]
pltId = thisline[1]

pdb2 =  'java com.ibm.bluematter.db2probspec.ProbspecgenDB2 '
pdb2com = pdb2 + SysId + " " + CtpId + " " + implId + " " + pltId + " " + dest + " -v"
print pdb2com

wcf = os.popen(pdb2com)
PSPLines = wcf.readlines()
wcf.close()
AddLog("ProbspecDB2", PSPLines)

####  Check success of msd transliteration

if os.access(msdfile,os.R_OK) == 0:
    DumpAndExit("ERROR: ProbspecDB2 did not generate the msd")

#########################################################
#                 compile_msd
#########################################################
msd =  'time java com.ibm.bluematter.utils.CompileMSD '
msdcom = msd + dest + ".msd "
print msdcom

wcf = os.popen(msdcom)
COMLines = wcf.readlines()
wcf.close()
AddLog("compileMSD", COMLines)

nlines = len(COMLines)
exeId = -1
for i in range(nlines):
    thisline = COMLines[i].split()
#    print thisline
    for j in range(len(thisline)):
	if(thisline[j].strip() == 'Executable'):
	    exeId = int(thisline[j+2])
	    print "Executable ID::" + str(exeId)

print "exeId: " + str(exeId)
if exeId == -1:
    DumpAndExit("ERROR:: Compilation failed, sorry pal :(")

####  Check success of msd compilation
print 'EXE name : ' + exefile
print "%s successfully created by xml2exe.py" % exefile

sys.exit( 0 )


