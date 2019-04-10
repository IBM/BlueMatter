#!/usr/bin/env python

from xreadlines import *
import sys
import os
import time


"""
if len(sys.argv) < 4:
    print
    print 'Usage: launch.py <directory> '
    sys.exit(1)
"""

dir = sys.argv[1]

seqnum = 1
found = 0

print "dir %s" % dir
##############  
#  Determine sequence number
#
while (not found):
      name = dir + "/data/mdlip." + str(seqnum)
#      print "checking existence of %s" % name
      if os.access(name,os.F_OK):
	 seqnum = seqnum + 1
      else:
	 found = 1

print "sequence number = %d" % seqnum
running = 0
jobname = "nojob"
##############
#   Check if process is running
#
idname = dir + "/id.txt"
if os.access(idname,os.F_OK):
   fil = open(idname,'r')
   lines = xreadlines(fil)
   fields = lines[0].split()
##   print fields
   job = fields[3]
   jobidstr = job.split('.')[4]
   jobid = int(jobidstr.split('"')[0])
   print job ,jobid

###  Get running job id list
   cmd = "rsh sp1login /usr/lpp/LoadL/full/bin/llq | grep pitman"
   fstr = os.popen(cmd)
   lines = fstr.readlines()
   fstr.close()  
#   print lines
   for i in range(len(lines)):
       tmp = lines[i].split()[0]
       tmpp = int(tmp.split('.')[1]) 
       if(jobid == tmpp):
	  running = 1
	  jobname = tmp
	  break

killjob = 0
currentlog = 0
logexists = 0
datagramexists = 0
naninside = 0
restart = 0
startlogging = 0

logname = dir + "/mdlip." + str(seqnum)
if (os.access(logname, os.F_OK)):
   logexists = 1

datagramname = dir + "/RawDatagramOutput"
if (os.access(datagramname, os.F_OK)):
   datagramexists = 1

if (running):
   print "Process %s is currently running" % jobname
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
      print "Last rdg at %s %s %s : %s" % (month,day,hour,min)

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
	 print "Last log at %s %d %d : %d" % (lmonth,lday,lhour,lmin)
	 mindif = abs(hour*60 + min - (lhour*60 + lmin))
	 if (month == lmonth) and (day == lday) and (mindif <= 1):
		print "logging in synch"
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

print "##############################################"
print "########   Job status for %s %s  " %(dir , jobname)
if (not running): 
   restart = 1
   print "job is not running - plan on restarting"

if (running and not datagramexists): 
   killjob = 1
   print "job is running but no datagram - plan on killing"

if (running and naninside): 
   killjob = 1
   print "job is running but log has nans - plan on killing"

if (running and (logexists and not currentlog)): 
   killjob = 1
   print "job is running but log is not current - plan on killing"

if (running and not logexists and not killjob):
	startlogging = 1
	print "job is running without logging - plan on logging"

if (killjob): 
   restart = 1
   print "will restart after killing job"

if(running and currentlog and not killjob):
   print "Status OK - job is running and log is current"


print "###########    Status summary:"
print "Running = %d" % running
print "Logexists = %d" % logexists 
print "Datagramexists = %d" % datagramexists
print "Currentlog = %d" % currentlog
print "Naninside = %d" % naninside
print "Killjob = %d" % killjob
print "Restart = %d" % restart
print "Startlogging = %d" % startlogging



if (killjob):
   print "killing job %s" % jobname
   cmd = "rsh sp1login /usr/lpp/LoadL/full/bin/llcancel " + jobname
   print cmd
   print "kill result : %s" % os.system(cmd)


if(restart):
   if (datagramexists):
      cmd = "mv " + datagramname + "  " + dir + "/datagrams/rdg." + str(seqnum)
      print cmd
      print "rdg rename result: %s" % os.system(cmd)


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
	    print cmd
	    print "mv result : %s " % os.system(cmd)


	cmd = "mv " + logname + " " +  dir + "/data/"
	print cmd
	print "mv result : %s " % os.system(cmd)

		     
	cmd = "cp " + dir + "/LipSnapshot.dvs " +  dir + "/Snapshot.dvs"
	print cmd
	print "cp result : %s " % os.system(cmd)





