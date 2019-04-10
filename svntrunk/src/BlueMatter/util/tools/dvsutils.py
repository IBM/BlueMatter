
import string
import sys
import os
from veclib import Vec




#####################################################################
#####################################################################
def GetOutput(cmd):
    fstr = os.popen(cmd)
    lines = fstr.readlines()
    fstr.close()
    return lines


#####################################################################
#####################################################################
def GetDVS(fname):
    cmd = 'dvs2txt ' + fname
    fstr = os.popen(cmd)
    lines = fstr.readlines()
    fstr.close()
    return lines


#####################################################################
#####################################################################

def ReadPosVels(lines , crds, vls):

    nl = len(lines)
    for i in range( nl ):
	  l = lines[i]
	  vals = l.split()     
	  v = Vec( [float(vals[2]),float(vals[3]),float(vals[4])] ) 
	  crds += [ v ]
	  v = Vec( [float(vals[5]),float(vals[6]),float(vals[7])] ) 
	  vls   += [ v ]
	  i += 1;
    return nl

#####################################################################
#####################################################################
def WriteDVSfile(cords,vel,fname):

    try:
	file = open("tmp_mergedvs_out.txt","w")
    except IOError, message:
        print >> sys.stderr, "File could not be opened:" , message
	sys.exit(1)
    OutputDvsTxt(cords, vel, file )
    file.close()

    cmd = "dvsconv tmp_mergedvs_out.txt " + fname
    os.system(cmd)
    if os.access(fname,os.R_OK) == 0:
       print "Failed to write file " + fname
       sys.exit(1)
    print "Wrote dvs file " + fname
    return

#####################################################################
#####################################################################
def OutputDvsTxt( crds, vls , file):

    nl = len(crds)
    file.write(str(nl))
    i = 0
    while i < nl:
	  lin =  "%15s  %15s  %15s \n" % \
		(str(crds[i][0]),str(crds[i][1]),str(crds[i][2]))
	  file.write(lin)
	  i += 1      
   
    file.write( str(nl))
    i = 0
    while i < nl:
	  lin =  "%15s  %15s  %15s \n" % \
		(str(vls[i][0]), str(vls[i][1]), str(vls[i][2]))
	  file.write(lin)
	  i += 1      

    return




