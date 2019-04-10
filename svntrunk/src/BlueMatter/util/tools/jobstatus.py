#!/usr/bin/env python

from xreadlines import *
import sys
import os
import time



#####################################################################
#####################################################################
def GetSequenceNumber(wdir,mdldir,seed):
    found = 0
    seq = 1
    while (not found):
	name = wdir + mdldir + "/" + seed + "."  + str(seq) + ".mdl"
	#print "checking existence of %s" % name
	if os.access(name,os.F_OK):
	    seq = seq + 1
	else:
	    found = 1
    return seq
#####################################################################
#####################################################################
def GetJobId(idname):
    if os.access(idname,os.F_OK):
	fil = open(idname,'r')
	lines = xreadlines(fil)
	fields = lines[0].split()
	job = fields[3]
	jobidstr = job.split('.')[4]
	jobid = int(jobidstr.split('"')[0])
	#print job ,jobid
    else:
	print "!!!!   Failed to parse jobId for |%s|" % idname
	jobid = 0
    return jobid
#####################################################################
#####################################################################
def GetJobName(jobid):
    running = 0
    user = os.environ['USER']
    #print "checking running jobs for %s" % user
    cmd = "rsh sp1login /usr/lpp/LoadL/full/bin/llq | grep " + user
    fstr = os.popen(cmd)
    lines = fstr.readlines()
    fstr.close()  
    #print lines
    jobname = 'not running'
    for i in range(len(lines)):
	tmp = lines[i].split()[0]
	tmpp = int(tmp.split('.')[1]) 
	if(jobid == tmpp):
	    running = 1
	    jobname = tmp
	    #print "Job %s is running" % jobname
	    break
	#else:
	#    print "%d not equal %d" % (jobid,tmpp)
    return jobname

#####################################################################
#####################################################################
def WriteLLScript(llfile,wdir,seedname,seqnum,cls,exename,dvsname,tasks,blck):

    pred = '# @ '
    outline = pred + "class = " + cls + "\n"
    llfile.write(outline)
    outline = pred + "initialdir = " + wdir  + "\n"
    llfile.write(outline)
    outline = pred + "executable = /usr/bin/poe" + "\n"
    llfile.write(outline)
    outline = pred + "arguments = " + exename + " " + dvsname + "\n"
    llfile.write(outline)
    outline = pred + "output = " + seedname + "." + str(seqnum) + ".log" + "\n"
    llfile.write(outline)
    outline = pred + "error =  " + seedname + "." + str(seqnum) + ".err" + "\n"
    llfile.write(outline)
    outline = pred + "network.MPI = css0, shared, US" + "\n"
    llfile.write(outline)
    outline = pred + "job_type = parallel" + "\n"
    llfile.write(outline)
    outline = pred + "environment = COPY_ALL" + "\n"
    llfile.write(outline)
    outline = pred + "requirements = (Machine != \"sp1n89en0\" )" + "\n"
    llfile.write(outline)
    if(blck == 1):
	outline = pred + "total_tasks = " + str(tasks)  + "\n" 
	llfile.write(outline)
	outline = pred + "blocking = unlimited\n"
	llfile.write(outline)
    else:
	outline = pred + "tasks_per_node = 4"  + "\n"
	llfile.write(outline)
	outline = pred + "node = " + str(tasks/4) + "\n"
	llfile.write(outline)
    outline = pred + "queue" + "\n"
    llfile.write(outline)
    return 1

#####################################################################
#####################################################################
def CheckRunLength(logname):
    if(os.access(logname, os.F_OK)):
	cmd = "head -2 " + logname + " | tail -1 | awk '{print $1}' > t.jnk" 
	os.system(cmd)
	cmd = "tail -1 " + logname + " | awk '{print $1}' >> t.jnk" 
	os.system(cmd)
	fil = open("t.jnk","r")
	rec = fil.readlines()
	fil.close()
	if len(rec) > 0:
	    l0 = rec[0].split()
	    l1 = rec[1].split()
	    T = int(l1[0]) - int(l0[0])
	    print "\t\t\t\t===> Current job has completed %d timesteps  (%f ps) " % (T, 0.002 * float(T) )


#####################################################################
#####################################################################
def CheckTotalRunLength(wdir,mdldir,seed,seqnum,logname):
    if (seqnum > 1):
	firstlog = wdir + mdldir + "/" + seed + ".1.mdl"
    else:
	firstlog = logname
    if( os.access(firstlog, os.F_OK) and os.access(logname, os.F_OK)):
	cmd = "head -2 " + firstlog + " | tail -1 | awk '{print $1}' > t.jnk" 
	os.system(cmd)
	cmd = "tail -1 " + logname + " | awk '{print $1}' >> t.jnk" 
	os.system(cmd)
	fil = open("t.jnk","r")
	rec = fil.readlines()
	fil.close()
	if len(rec) > 0:
	    l0 = rec[0].split()
	    if len(rec) > 1:
		l1 = rec[1].split()
		T = int(l1[0]) - int(l0[0])
		print "\t\t\t\t===> All jobs have completed total of %d timesteps  (%f ps) " % (T, 0.002 * float(T) )
	    else:
		print "\t\t\t\t===> No timesteps in current log"


#####################################################################
#####################################################################
#####################################################################
#####################################################################
#####################################################################
#####################################################################

if len(sys.argv) < 3:
    print
    print 'Usage: jobstatus.py <directory> <seed name>'
    print 
    print 'job identification is held by <dir>id.txt'
    print 'mdlog files are in <dir>/mdl/<seed>.#.mdl'
    sys.exit(1)

dir = sys.argv[1]
seedname = sys.argv[2]

MDLdir = "/MDL/"
RDGdir = "/RDG/"
Rawname = "RawDatagramOutput"
RDGName = Rawname
IDname = "id.txt"

while 1:
    seqnum = 1
    found = 0
    killjob = 0
    currentlog = 0
    logexists = 0
    datagramexists = 0
    naninside = 0
    restart = 0
    restartfileexists = 0
    startlogging = 0
    running = 0

    print "\n\n"
    #    	    
    #   Determine sequence Number
    #
    seqnum = GetSequenceNumber(dir,MDLdir,seedname)
    #print "Sequence number:   %d" % seqnum
	    
    #    	    
    #   Get job name and id
    #
    idname = dir + "/" + IDname
    jobid = GetJobId(idname)
    jobname = GetJobName(jobid)

    #    	    
    #   Determine if Job is running
    #    
    if (jobname != 'not running'):
	running = 1

    #    	    
    #   Check if log exists
    #    	    
    logname = dir + seedname + "." + str(seqnum) + ".mdl"
    if (os.access(logname, os.F_OK)):
	logexists = 1
	#print "logname %s exists" % logname

    #    	    
    #   Check if datagram exists
    #    	    
    datagramname = dir + "/" + Rawname
    if (os.access(datagramname, os.F_OK)):
	datagramexists = 1
	#print "datagram %s exists" % datagramname

    if (running):
	#print "Process %s is currently running" % jobname
        ####################
	#  Check if process is being logged
	#
	if (datagramexists):
	    cmd = "ls -l " + datagramname
	    fstr = os.popen(cmd)
	    lines = fstr.readlines()
	    fstr.close()  
	    lin = lines[0].split()
	    month = lin[5]
	    day = int(lin[6])
	    hour = int (lin[7].split(':')[0])
	    min = int (lin[7].split(':')[1])
	    #	    print "Last update to rdg was at %s %s %s : %s" % (month,day,hour,min)
	
	if (logexists):
	    cmd = "ls -l " + logname
	    fstr = os.popen(cmd)
	    lines = fstr.readlines()
	    fstr.close()  
	    lin = lines[0].split()
	    lmonth = lin[5]
	    lday = int(lin[6])
	    lhour = int (lin[7].split(':')[0])
	    lmin = int (lin[7].split(':')[1])
	    # print "Last update to log was at %s %d %d : %d" % (lmonth,lday,lhour,lmin)
	    mindif = abs(hour*60 + min - (lhour*60 + lmin))
	    if (month == lmonth) and (day == lday) and (mindif <= 1):
		#print "logging in synch"
		currentlog = 1
		


    if (logexists):
	cmd = "tail -2 " + logname + " | grep -i \"nan\" "
	fstr = os.popen(cmd)
	lines = fstr.readlines()
	fstr.close()  
	if ( len(lines) > 0):
	    print lines
	    #      print "#########    NAN Inside   #######"
	    naninside = 1

    print "\t##############################################"
    print "\t### Directory       %s" % dir 
    print "\t### Seedname        %s" %  seedname
    print "\t### Sequence Number %d" %  seqnum
    print "\t### SP2 job         %s" %  jobname
    print "\t### "

    if (not running): 
	restart = 1
	print "\t\t\t\t===>"
	print "\t\t\t\t===>job is not running - plan on restarting"
	print "\t\t\t\t===>"

    if (running and not datagramexists): 
	killjob = 1
	print "\t\t\t\t===>"
	print "\t\t\t\t===>job is running but no datagram - plan on killing"
	print "\t\t\t\t===>"

    if (running and naninside): 
	killjob = 1
	print "\t\t\t\t===>"
	print "\t\t\t\t===>job is running but log has nans - plan on killing"
	print "\t\t\t\t===>"

    if (running and (logexists and not currentlog)): 
	killjob = 1
	print "\t\t\t\t===>"
	print "\t\t\t\t===>job is running but log is not current - plan on killing"
	print "\t\t\t\t===>"

    if (running and not logexists and not killjob):
	startlogging = 1
	print "\t\t\t\t===>"
	print "\t\t\t\t===>job is running without logging - plan on logging"
	print "\t\t\t\t===>"

    if (killjob): 
	restart = 1
	print "\t\t\t\t===>"
	print "\t\t\t\t===>Consider restart after killing job"
	print "\t\t\t\t===>"

    if(running and currentlog and not killjob):
	print "\t\t\t\t===>"
	print "\t\t\t\t===> Status OK - job is running and log is current"
	CheckRunLength(logname)
	CheckTotalRunLength(dir,MDLdir,seedname,seqnum,logname)
	print "\t\t\t\t===> "


    print "\t               State Flags"
    print "\t================================================= "
    stsflgs =  "\tRunning = %d" % running + "\tLogfile = %d" % logexists + "\tDatagram = %d" % datagramexists + "\tCurrentlog = %d" % currentlog
    print stsflgs

    stsflgs =  "\tNaninside = %d" % naninside + "\tKill = %d" % killjob + "\tRestart = %d" % restart + "\tStartlog = %d" % startlogging
    print stsflgs



    print "\t"
    print "\t               Command Options"
    print "\t================================================= "
    print "\t1  Start Logging"
    print "\t2  Prepare for Restart <exename> <class> <tasks> (0/1 blocking)"
    print "\t3  Start Running"
    print "\t4  Kill job"
    print "\t5  check user jobs"
    print "\t6  check other user's jobs"
    print "\t7  check class availability"
    print "\t8  N timesteps"
    print "\t0  Exit"

    print "Enter cmd number :"
    inplin = sys.stdin.readline()
    cmds = inplin.split()
    if len(cmds) > 0:	
	cmdnum = int(cmds[0])
	print "cmdnum = %d" % cmdnum
	sys.stdout.write(inplin)
	if cmdnum == 1:
	    # 
	    #Start logging
	    #
	    cdcmd = "cd " + dir 
	    oscmd = cdcmd + ";MDLogger -- < " + dir + RDGName + " > " +logname +  " &"
	    print oscmd
	    os.system(oscmd)
	elif cmdnum == 2:
	    # 
	    #           Prepare for Restart <exename> <class> <tasks> (0/1 blocking)
	    #

	    if len(cmds) < 5:
		print "Usage:  2  <exename> <class> <tasks> (0/1 blocking)"
	    else:
		#
		exename = cmds[1] 
		cls = cmds[2]
		tasks = int(cmds[3])
		blck = int(cmds[4])
		dvsname = seedname + "." + str(seqnum) + ".start.dvs"

		#                      Move RDG
		# mv RDGname RDG/seedname.seqnum.rdg
		#
		if datagramexists:
		    oscmd = "mv " + dir + RDGName + " " + dir + RDGdir + seedname + "." + str(seqnum) + ".rdg"
		    os.system(oscmd)
		#                     Move mdl
		# mv logname MDL/
		#
		if logexists:
		    oscmd = "mv " + logname + " " + dir + MDLdir 
		    os.system(oscmd)
		    seqnum = seqnum + 1
		    dvsname = seedname + "." + str(seqnum) + ".start.dvs"
		    #
		    #                     copy dvs
		    # cp Snapshot.dvs RDG/seedname.seqnum.stop.dvs
		    # cp Snapshot.dvs RDG/seedname.seqnum+1.start.dvs
		    #
		    oscmd = "cp " + dir + "Snapshot.dvs " + dir + RDGdir + seedname + "." + str(seqnum-1) + ".stop.dvs"
		    os.system(oscmd)
		    oscmd = "cp " + dir + "Snapshot.dvs " + dir + dvsname
		    os.system(oscmd)
		#
		#         Write load leveler script for restart of next sequence
		#
		llname = dir + seedname + "." + str(seqnum) + ".restart.lls"
		llfile = open(llname,"w");
		WriteLLScript(llfile,dir,seedname,seqnum,cls,exename,dvsname,tasks,blck)
		llfile.close()
	elif cmdnum == 3:
	    # 
	    llname = dir + seedname + "." + str(seqnum) + ".restart.lls"
	    if (os.access(llname, os.F_OK)):
		cmd = "rsh sp1login /usr/lpp/LoadL/full/bin/llsubmit " + llname + " > " + idname
		print "submit result : %s" % os.system(cmd)
	elif cmdnum == 4:
	    if (running):
		print "killing job %s" % jobname
		cmd = "rsh sp1login /usr/lpp/LoadL/full/bin/llcancel " + jobname
		print "kill result : %s" % os.system(cmd)
	    else:
		print "!!!!!!!!    Job is not running\n"

	elif cmdnum == 5:
	    #
	    #                  check user jobs
	    #
	    cmd = "rsh sp1login /usr/lpp/LoadL/full/bin/llq | grep " + os.environ['USER']
	    print "---------------------------------------------------------------------------"
	    os.system(cmd)
	    print "---------------------------------------------------------------------------"
	elif cmdnum == 6:
	    #
	    #                  check other user's jobs
	    #
	    cmd = "rsh sp1login /usr/lpp/LoadL/full/bin/llq | grep " + cmds[1]
	    print "---------------------------------------------------------------------------"
	    os.system(cmd)
	    print "---------------------------------------------------------------------------"
	elif cmdnum == 7:
	    #
	    #                  check class availability
	    #
	    cmd = "rsh sp1login /usr/lpp/LoadL/full/bin/llclass "
	    print "---------------------------------------------------------------------------"
	    os.system(cmd)
	    print "---------------------------------------------------------------------------"
	elif cmdnum == 8:
	    #
	    #                  check number of timesteps
	    #
	    CheckRunLength(logname)
	    CheckTotalRunLength(dir,MDLdir,seedname,seqnum,logname)
    else:
	sys.stdout.flush()
	break



sys.exit()





if(restart):
   if (datagramexists):
      cmd = "mv " + datagramname + "  " + dir + "/datagrams/rdg." + str(seqnum)
      print "rdg rename result: " % os.system(cmd)


   if(logexists):
	fillist = ( "CH2_Hist.dat","CH3_Hist.dat",  \
		    "CHvec_10.dat","CHvec_11.dat",  \
		    "CHvec_12.dat","CHvec_13.dat",   \
		    "CHvec_14.dat","CHvec_15.dat",  \
		    "CHvec_16.dat","CHvec_17.dat", \
		    "CHvec_18.dat","CHvec_2.dat",  \
		    "CHvec_3.dat", "CHvec_4.dat",  \
		    "CHvec_5.dat", "CHvec_6.dat",  \
		    "CHvec_7.dat", "CHvec_8.dat",  \
		    "CHvec_9.dat", "Carb_Hist.dat",  \
		    "Chol_Hist.dat","Gly_Hist.dat",  \
		    "H2O_Hist.dat", "LipidOrderParam.dat",  \
		    "Ole_Hist.dat", "Phos_Hist.dat" )
        for i in range(len(fillist)):
	    cmd = "mv " + dir + "/" + fillist[i] + " " + dir + \
		      "/data/" + fillist[i] + "." + str(seqnum)
	    cmdls = "ls -l " + dir + "/" + fillist[i] + " " 
	    print "mv result : %s " % os.system(cmd)


	cmd = "mv " + logname + " " +  dir + "/data/"
	print "mv result : %s " % os.system(cmd)

		     
	cmd = "cp " + dir + "/LipSnapshot.dvs " +  dir + "/Snapshot.dvs"
	print "cp result : %s " % os.system(cmd)





