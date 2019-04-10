#!/usr/bin/env python

#from xreadlines import *
import sys
import os
import time
import popen2
import time 

#####################################################################
#####################################################################
def GetOutput(cmd):
    fstr = os.popen(cmd)
    lines = fstr.readlines()
    fstr.close()
    return lines
#####################################################################
#####################################################################
def OutputLines(lines,file):
    for i in range(len(lines)):
	file.write(lines[i])

#####################################################################
#####################################################################
def ReportSRDG(wdir,rdgdir,seed,block,seq,ser):

    if(ser == 'NA'):
	rdgname = wdir + rdgdir + seed +  '.' + seq + '*.srdg'
#	rdgname = wdir + rdgdir + seed +'.' + block +  '.' + seq + '*.srdg'
    else:
	rdgname = wdir + rdgdir + '/' + ser + '/' + seed + '_' + ser  + '.' + seq + '*.srdg'
#	rdgname = wdir + rdgdir + '/' + ser + '/' + seed + '_' + ser  + '.' + block + '.' + seq + '*.srdg'
#	rdgname = wdir + rdgdir + '/' + seed + '_' + ser  + '.' + block + '.' + seq + '*.srdg'
#	rdgname = wdir + rdgdir + '/' + seed + '_' + ser  + '.' + seq + '*.srdg'
    cmd = 'ls -ltr ' + rdgname
    lines = GetOutput(cmd)
    SRDG_COUNT = len(lines)
    nlines= lines
    if(SRDG_COUNT > 0):
	for i in range(SRDG_COUNT):
	    cols = lines[i].split()
	    #print cols
	    #print "len cols = %d" % (len(cols))
	    nlines[i] = cols[8] + ' ' + \
		    cols[4] + ' ' + \
		    cols[5] + ' ' + \
		    cols[6] + ' ' + \
		    cols[7]
	    print nlines[i]
    else:
	print "No srdg files of the name " + rdgname + " exist (yet)"
    return SRDG_COUNT
#####################################################################
#####################################################################
def ReportMDL(wdir,mdldir,seed,block,seq,ser):
    if(ser == 'NA'):
	mdlname = wdir + mdldir + seed + '.' + seq + '.mdl'
    else:
	mdlname = wdir + mdldir + '/' + ser + '/' + seed + '_' + ser + '.' + seq + '.mdl'
    
    if (os.access(mdlname,os.F_OK)):
	cmd = 'wc -l ' + mdlname
	linecountstr = GetOutput(cmd)
	linecountcols = linecountstr[0].split()
	linecount = linecountcols[0]
	cmd = 'ls -ltr ' + mdlname
	lines = GetOutput(cmd)
	nlines= lines
	cols = lines[0].split()
        #print cols
        #print "len cols = %d" % (len(cols))
	cmd = "tail -1 " + mdlname
	lines = GetOutput(cmd)
	ln = '0.0'
	if len(lines) > 0 :
	    ln = lines[0].split()
	currts = float(ln[0])
	totsimtim = str(SimStepInPs * (currts - SimStart))
	simtim = str(float(linecount) * SimEnergyMod * SimStepInPs)
	out        = "      Name    : \t" + cols[8] + '\n' + \
		     "      Size    : \t" + cols[4] + '\n' + \
		     "      Date    : \t" + cols[5] + ' ' + cols[6] + ' ' + cols[7] + '\n' \
	             "      Sim Time: \t" + simtim + ' ps \n' +\
	             "Total Sim Time: \t" + totsimtim + ' ps \n'
	print out
    else:
	print "MDLog file " + mdlname + " does not exist (yet)."
	linecount = 0
    return linecount
	
#####################################################################
#####################################################################
def LaunchMDLogger(wdir,rdir,mdir,ddir,seed,block,seq,ser,nprocs):
    if(ser == 'NA'):
	wddir = wdir + ddir 
	tempname = wdir + rdir + seed + '.' + seq + '.' + '%04d.0000.'+ nprocs + '.1024.srdg'
#	tempname = wdir + rdir + seed + '.' + block + '.' + seq + '.' + '%04d.0000.' + nprocs + '.1024.srdg'
	mdlname = wdir + mdir  + seed  + '.' + seq + '.mdl'	    
	dname = seed + '.'
    else:
	wddir = wdir + ddir + '/' + ser + '/' 
	tempname = wdir + rdir + '/' + ser + '/' + seed + '_' + ser +  '.' + seq + '.' + '%04d.0000.' + nprocs + '.1024.srdg'
#	tempname = wdir + rdir + '/' + seed + '_' + ser +  '.' + seq + '.' + '%04d.0000.' + nprocs + '.1024.srdg'
#	tempname = wdir + rdir + '/' + ser + '/' + seed + '_' + ser +  '.' + block + '.' +  seq + '.' + '%04d.0000.' + nprocs + '.1024.srdg'
#	tempname = wdir + rdir + '/' + seed + '_' + ser +  '.' + block + '.' +  seq + '.' + '%04d.0000.' + nprocs + '.1024.srdg'
	mdlname = wdir + mdir  + '/' + ser + '/' + seed + '_' + ser + '.' + seq + '.mdl'	            
	dname = seed + '_' + ser + '.' 

    fname = wddir + 'launchMDL_' + seed + '.sh'
    fil = open(fname,"w")
    cmd = 'cd ' + wddir + '\n'
    fil.write(cmd)
    sname = dname + seq + '.stop'
#    cmd =  bdir + 'MDLogger -seekahead 10000000 -timeout 420 ' +  '\"' + tempname + '\" > ' + mdlname  + '\n'
    cmd =  bdir + 'MDLogger ' +  ' \"' + tempname + '\" -snapnamestem ' + sname + ' > ' + mdlname  + '\n'
    fil.write(cmd)
    cmd = 'sleep 10 \n'
    fil.write(cmd)
    cmd = wddir + 'xferdvs_' + seed + '.sh' + ' > ' + wddir + 'xferdvs_' + seed + '.out'
    fil.write(cmd)
    fil.close()
    cmd = 'chmod +x ' + fname
    os.system(cmd)
    fname2 = wddir + 'xferdvs_' + seed + '.sh'
    fil = open(fname2,"w")
#    cmd = 'cp Snapshot.dvs ' + dname + seq + '.stop.dvs' + '\n'
#    fil.write(cmd)
    num = int(seq)
    num = num+1
    nseq = str(num)
    startname =  dname + nseq + '.start.dvs'
    cmd = 'cp ' + wddir + sname + '.dvs  '  + startname + '\n'
    fil.write(cmd)
    cmd = 'cp ' + startname + ' ' + StartDvsDir  + '\n'
    fil.write(cmd)
    cmd = 'chmod a+rx ' + StartDvsDir + startname + '\n'
    fil.write(cmd)
    cmd = 'echo \"prepared \"' + startname + ' > MDL_finished_' + seed + '.' + nseq + '.txt' + '\n'
    fil.write(cmd)
    fil.close()
    cmd = 'chmod +x ' + fname2
    os.system(cmd)
    cmd =  fname + ' &'
    print cmd
    os.system(cmd)

#####################################################################
#####################################################################
def StartDvsName(sddir,seed,seq,ser):
    if(ser == 'NA'):
	startingdvsname = sddir + seed + "." + seq + ".start.dvs"
    else:
	startingdvsname = sddir + seed + "_" + ser + "." + seq + ".start.dvs"
    return startingdvsname

#####################################################################
#####################################################################
def XferDVS(srcdir,destdir,seed,seq,ser):
    sname = StartDvsName(srcdir,seed,seq,ser)
    dname = StartDvsName(destdir,seed,seq,ser)
    snpname = srcdir + 'Snapshot.dvs'
    snpfname = srcdir + 'SnapshotFinal.dvs'
    if(os.access(sname,os.R_OK)):
	cmd = 'cp ' + sname + ' ' + dname
	os.system(cmd)
    elif (os.access(snpname,os.R_OK)):
	cmd = 'cp ' + snpname + ' ' + sname
	os.system(cmd)		
	cmd = 'cp ' + sname + ' ' + dname
	os.system(cmd)
    elif (os.access(snpfname,os.R_OK)):
	cmd = 'cp ' + snpfname + ' ' + sname
	os.system(cmd)		
	cmd = 'cp ' + sname + ' ' + dname
	os.system(cmd)
    else:
	print "No record of any previous snapshot"
	return 0
    if(os.access(dname,os.R_OK)):
	return 1
    else:
	return 0

#####################################################################
#####################################################################
def LaunchRun(sdir,dvdir,rdgdir,seed,seq,ser,midplane,launchcmd,rtsname,driver):
    # sdir      : directory for starting dvs
    # dvdir     : full directory of previous dvs runs
    # rdgdir    : subdirectory for rdgs 
    # seed      : seedname
    # seq       : sequence
    # midplane  : script to rsh into and launch ciorun
    # launchcmd : name of the rts
    # driver    : driver to use

    startingdvsname = StartDvsName(sdir,seed,seq,ser)
    if(os.access(startingdvsname,os.R_OK)):
	clear_to_launch = 1
    else:
	print "Starting DVS file is not in place: " + startingdvsname
	print "Attempting Transfer " + startingdvsname
	clear_to_launch = XferDVS(dvdir,StartDvsDir,seed,fseq)
    if(clear_to_launch):
        launchcmd = launchcmd + ' ' + seed + ' ' + rtsname + ' ' + seq + ' ' + rdgdir + ' ' + driver + ' ' + midplane
	Run(launchcmd)
	print "Executed Launch"
    else:		
	print "Launch Failed because starting dvs could not be put in place: " + startingdvsname +" !!!!!"
    return clear_to_launch
#####################################################################
#####################################################################
def  LaunchFullRestart(cmds,rstname):

    tfile = open(rstname,'w')
    tfile.write(cmds)
    tfile.close()

    if(os.access(rstname,os.R_OK)):
	cmd = sys.argv[0] + ' ' +  sys.argv[1] + ' < ' + rstname + '  > ' + rstname + '.out'
	print cmd    
	RunPy(cmd)
	print "Executed Launch Restart. Output is in file " + rstname + '.out'
	return 1
    else:
	print "Could not write restart file: " + rstname
	print "Restart Failed !!!!"
	return 0
#####################################################################
#####################################################################

def  BootMidplane(bootname,slp,DRIVER,midplane):
    cmd = '/usr/bin/csh ' + bootname + ' ' + str(slp) + ' ' + DRIVER + ' ' + midplane + ' > ' + bootname + '.out &'
    print cmd
    os.system(cmd)
    
#####################################################################
#####################################################################
def Run(runcmd):
    cmd = '/usr/bin/csh ' + runcmd + '  &'
    print cmd
    os.system(cmd)

#####################################################################
#####################################################################
def Sleep(sec):
    time.sleep(float(sec))
#####################################################################
#####################################################################
def RunPy(runcmd):
    cmd = '/usr/local/bin/python ' + runcmd + '  &'
    print cmd
    os.system(cmd)

#####################################################################
#####################################################################
def KillPids(str1,str2):
    cmd = './get_pid.sh ' + str1 + ' ' + str2
    lins = GetOutput(cmd)
    for i in range(len(lins)):
	pids = lins[i]
	cmd = 'kill -9 ' + pids
	os.system(cmd)
#####################################################################
#####################################################################




expfname = sys.argv[1]
print "Experiment : %s" % expfname
keytab = {}

lines = GetOutput('cat ' + expfname)
for i in range(len(lines)):
    vals = lines[i].split()
    if (len(vals) > 1):
	keytab[vals[0]] = vals[1]
#	print "key %s = %s" % (vals[0],vals[1])

	
wdir = keytab['BGL_WDIR']
fseq = keytab['BGL_FWD_SEQ']
seed = keytab['BGL_SEEDNAME']
bdir = keytab['BGL_BIN']
rdir = keytab['BGL_RDGDIR']
mdir = keytab['BGL_MDLDIR']
ddir = keytab['BGL_DVSDIR']
fwdrdir = rdir 
fwdmdir = mdir 
fwdddir = ddir 
launchfwd = keytab['BGL_RUN_FWD']
fwdbootname = keytab['BGL_BOOT_FWD']
rtsname = keytab['BGL_RTSNAME']
bootsleep = 1000000
DRIVER = keytab['BGL_DRIVER']
StartDvsDir = keytab['BGL_DVS_START_DIR'] 
SimStart = float(keytab['BGL_SIMULATION_START'])
SimStepInPs = float(keytab['BGL_SIMULATION_STEP_IN_PS'])
SimEnergyMod = float(keytab['BGL_SIMULATION_ENERGY_MODULO'])
BGL_BLOCK = keytab['BGL_BLOCK']
Series = keytab['BGL_SERIES']
nprocs = keytab['BGL_NPROCS']

help = 1 
while 1:
    if(len(sys.argv) <=  2): 
	print "\t"
    	print "\t               Command Options"
    	print "\t================================================= "
    	print "\t1  print params"
    	print "\t2  check srdg"
    	print "\t4  check mdl"
    	print "\t6  Launch MDLogger"
    	print "\t8  Boot "
    	print "\t10  Launch run"
    	print "\t12  Full Restart "
    	print "\t14  Advance Sequence"
    	print "\t16  write params"
    	print "\t17  kill pids (str1,str2) "
	print "\t18  check if starting dvs is in place"
	print "\t20  transfer  starting dvs"
    	print "\t1000  sleep"
    	print "\t0  Exit"

    if(len(sys.argv) <=  2): 
        print "Enter cmd number :"
    inplin = sys.stdin.readline()
    cmds = inplin.split()
    if len(cmds) > 0:	
	if(cmds[0].isdigit()):
	    cmdnum = int(cmds[0])
	else:
	    cmdnum = 0
	#print "cmdnum = %d" % cmdnum
    	if(len(sys.argv) <=  2): 
	    sys.stdout.write(inplin)
	if cmdnum == 1:
	    for i in keytab.items():
		print "\t%20s \t= \t%s" % (i[0],i[1])
	elif cmdnum == 2:
	    ReportSRDG(wdir,fwdrdir,seed,BGL_BLOCK,fseq,Series)
	elif cmdnum == 4:
	    ReportMDL(wdir,fwdmdir,seed,BGL_BLOCK,fseq,Series)
	elif cmdnum == 6:
	    LaunchMDLogger(wdir,fwdrdir,fwdmdir,fwdddir,seed,BGL_BLOCK,fseq,Series,nprocs)
	elif cmdnum == 8:
	    print " Booting midplane %s" % ( cmds[1])
	    BootMidplane(fwdbootname,bootsleep,DRIVER,cmds[1])
	elif cmdnum == 10:
	    tdir = wdir + fwdddir
	    LaunchRun(StartDvsDir,tdir,fwdrdir,seed,fseq,Series,cmds[1],launchfwd,rtsname,DRIVER)

	elif cmdnum == 12:	# Launch Full Restart fwd    
	    str1 = "1 \n8 " + cmds[1]
	    str2 = "\n1000 250 \n10 " + cmds[1]
	    str3 = "\n1000 600 \n6 \n1000 60 \n2 \n4 \n1000 600 \n2 \n4 \n1000 600 \n2 \n4 \n0 \n"
	    str4 = str1 + str2 + str3
	    rstname = bdir + 'restartF.cmd'
	    LaunchFullRestart(str4,rstname)

	elif cmdnum == 14:	    
	    fseq =  str(int(fseq) + 1)
	    keytab['BGL_FWD_SEQ'] = fseq
	elif cmdnum == 16:	    
	    fil = open(expfname,"w")
	    for i in keytab.items():
		print >> fil, "%s %s" % (i[0],i[1])
	    fil.close()
	elif cmdnum == 1000:
	    print "Sleeping for %s" % cmds[1]
	    Sleep(cmds[1])
	    print "...woke up"
	elif cmdnum == 18:
	    startingdvsname = StartDvsName(StartDvsDir,seed,fseq,BGL_SERIES)
	    if(os.access(startingdvsname,os.R_OK)):
		print("Starting dvs " + startingdvsname + " is in place")
	    else:
		print("Starting dvs " + startingdvsname + " is NOT in place")
	elif cmdnum == 20:
	    tdir = wdir + fwdddir
	    dname = StartDvsName(StartDvsDir,seed,fseq,Series)
	    code = XferDVS(tdir,StartDvsDir,seed,fseq,Series)
	    if(code):
		print "Transfered starting DVS. Starting DVS " + dname + " is now in place"
	    else:
		print "Failed to transfer starting DVS"
	elif cmdnum == 0:
	    sys.exit(0)


