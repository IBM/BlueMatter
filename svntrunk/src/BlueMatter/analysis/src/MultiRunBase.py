from Numeric import *
import os
import sys
import socket
import re
import thread
import time
import socket
import shutil
import bgutils
from types import *

"""
  This file supports MultiRun.py, which launches a system of MD runs specified by a MultiRun.rtp "meta-rtp" file.
  The required associated files are:
    MultiRun.rtp
    LaunchRun, or whatever, specifed in the rtp file as RunCommand

  In addition, the file RTPMachines.txt should contain the list of machines to use for
  the runs in the following format:

    octopus3  8  1.0
    locutus   4  1.5

  Here the first column is the machine name, the second the number of cpus, and the third
  is an approximation of the machine's relative speed.  This is all for load balancing.

  If this file (RTPMachines.txt) is found in the current directory, it is used.  Otherwise it will use
  the file specified by the RTPMACHINESFILE env. variable.

  The rtp file should contain an estimate of the relative work done in a run as RunCost.

  The RunCommand is responsible for launching the job and creating Run.mdlog, and Run.done or Run.failed.

  After the run is complete, the Run.final file is merged into the RunResults.txt file.

  Then the script specified by the AnalysisCommand variable, if specified in the rtp file, is executed.
"""

Machines = []


def IsPosix():
  if os.name == 'posix':
    return 1
  else:
    return 0

def CleanDirName(s):
  if s == None:
    return '.'
  if len(s) < 1:
    return s
  if s[-1] == '/':
    return s[:-1]
  return s

def SubstituteStrings(s, vars):
  if type(s) != StringType:
    return s
  for (n, v) in vars.items():
    s = s.replace(n, v)
  return s

class ValueExpression:
  def __init__(self, aname, aexprstr, aargument=0, amaster='', ainternal=0, acompound=0):
    if aname.startswith('var:'):
      aargument = 0
      aname = aname[4:]
    elif aname.startswith('arg:'):
      aargument = 1
      aname = aname[4:]
    self.name = aname
    self.exprstr = aexprstr
    self.master = amaster
    self.argument = aargument
    self.lastvalue = ''
    self.isstring = 0
    self.internal = ainternal
    self.compound = acompound

# vname is first token on line, and Str is remaining string
# tstep [0.0, 1.0, 2.0]
# was:
# tstep list
# now:
# list <empty>
def GetValueExpressions(lstr, valstartindex, rtpfile, vname, globdict, argument=0, master='', internal=0, compound=0):
  first = 1
  valexprs = []
  nvals = 0
  splitline = re.split("\s+", lstr, valstartindex)
  if len(splitline) > valstartindex:
    Str = splitline[valstartindex]
    valexprs.append(ValueExpression(vname, Str, argument, master, acompound=compound))
  else: 
    if vname != "list":
      print 'error parsing %s' % lstr
      os.exit(-1)

    l = rtpfile.readline()

    # get var names from first line
    for s in l.split():
      valexprs.append(ValueExpression(s, '', argument, master, acompound=compound))
    nvals = len(valexprs)

    # slave non-first columns to first column if first column is not slave
    if master =='' and nvals>1:
      for ve in valexprs[1:]:
        ve.master = valexprs[0].name
    l = rtpfile.readline()

    # start getting the values
    while l != '' and not l.startswith("#endlist"): 
      sl = l.strip()
      # get next line now
      l = rtpfile.readline()
      if sl == '' or sl.startswith("#"):
	continue
      i = 0
      if first:
	first = 0
	# init each expr based on type
	for s in sl.split():
	  ve = valexprs[i]
  	  try:
	    eval(str(s), globdict)
	  except:
	    ve.isstring = 1
	  if ve.isstring:
	    ve.exprstr = '["%s"' % s
	  else:
	    ve.exprstr = 'array([%s' % s
	  i += 1
      else:
	# not first, so fill in the exprs
	# If it's not a repeat, save the new token in lastvalue
	i = 0
	for s in sl.split():
	  ve = valexprs[i]
	  if s != '' and s != '*':
	    ve.lastvalue = s
	  i += 1
	# now use the value stored in lastvalue
	for ve in valexprs:
	  if ve.isstring:
            ve.exprstr += ',"%s"' % ve.lastvalue
	  else:
	    ve.exprstr += ',%s' % str(ve.lastvalue)
    # close the list string
    for ve in valexprs:
      if ve.isstring:
	ve.exprstr += "]"
      else:
	ve.exprstr += "])"

  return valexprs
    

# name is the name of the variable
# values is the list of values
# mainList is the global list of master variables
# master specifies the master of this (slave) variable
class RTPVar:
  def __init__(self, name, values, mainList, allList, orderedList, exprstr, isstring, argument=0, master='', compound=0):
    self.name = name.split('.')[0]
    self.master = master
    self.slaves = []
    self.values = values
    self.argument = argument
    self.exprstr = exprstr
    self.isstring = isstring
    self.compound = compound
    # print "RTPVar ", self.name, self.master, self.values, self.exprstr, self.compound
    # this is a hack to make sure values are sequence-like
    try:
      dummy=len(values)
      self.values = values
    except:
      self.values = [values]
    # does this variable count as a new ensemble dimension?
    self.isdimension = master == '' and len(values) > 1
    allList[name] = self
    orderedList.append(name)
    if master != '':
      mainList[master].AddSlave(self)
    elif compound == 0:
      mainList[name] = self

  def AddSlave(self, slave):
    self.slaves.append(slave)


# A specific set of variables for a run
# Assign run cost if in the values
class Run:
  def __init__(self, NameStem, SystemID, Values, SubVals):
    self.NameStem = NameStem
    self.SystemID = SystemID
    self.Values = Values
    self.Procs = 1
    for n in self.Values.keys():
      self.Values[n] = SubstituteStrings(self.Values[n], SubVals)
    if "procs" in self.Values.keys():
      self.Procs = self.Values['procs']
    self.AssignedMachine = None
    self.ParallelMachines = []
    self.pid = 0
    cost = 1
    if Values.has_key('RunCost'):
      cost = Values['RunCost']
    self.Cost = float(cost)


# Describe the basic properties of a machine
# and keep track of work assigned to it
# Workload should decrease when jobs complete. Do Later...
class Machine:
  def __init__(self, InitString):
    s = InitString.split()
    self.name = s[0]
    self.ncpus = int(s[1])
    self.speed = float(s[2])
    if socket.gethostname().split('.')[0] == self.name:
      self.islocal = 1
    else:
      self.islocal = 0
    self.jobsrunning = 0
    self.currentload = 0
    self.dirs = []
    self.pids = []
    # Next line could call Usage to set initial usage
    self.initialusage = 0
    self.currentusage = self.initialusage
    self.usagerunning = 0
    self.usagedone = 1

  def DoRun(self, Dir, RunStr, BuildStr, Cost=0, Launch=1, CmdName='Run.cmd', userArgs='', BuildLocal=1, NProcs=1, NoLog=0):
    # Need to find global name for cwd, and os.cwd() may give the local mount version
    # This requires PWD to be defined properly...
    pid = -1
    try:
      localdir = os.getenv('PWD')
      # pf = os.popen('/usr/bin/pwd')
      # localdir = pf.readline().split()[0]
      # pf.close()
    except:
      pass
    if localdir == None or localdir == '':
      localdir = os.getcwd()
    fname = "%s/%s/%s" % (localdir, Dir, CmdName)
    if CmdName == 'Run.cmd' and userArgs == '':
      os.system("rm -f %s %s/Run.pid %s/RawDatagramOutput %s/Run.final %s/Run.cmd %s/core %s/Run.done %s/Run.log %s/Run.mdlog %s/Run.failed %s/Run.pass" % (fname, Dir, Dir, Dir, Dir, Dir, Dir, Dir, Dir, Dir, Dir))
    else:
      os.system("rm -f %s %s/Run.pid %s/core" % (fname, Dir, Dir))
    f = open(fname, "w")
    f.write("#!/usr/bin/ksh\n")
    f.write("\n")
    f.write("umask 022\n")
    f.write("\n")
    f.write("if cd %s/%s\n" % (localdir, Dir))
    f.write("then\n")
    f.write("  . ../RunEnv\n")

    if BuildStr != '':
      if NoLog == 0:
	  f.write("  %s $* > Run.build 2>&1 \n" % BuildStr)
      else:
	  f.write("  %s $*  \n" % BuildStr)
      f.write("  exename=`ls *.exe *.rts`\n")
      f.write("  if [[ -s $exename ]]; then\n")
      if self.islocal or NProcs>1:
        f.write("    %s/%s/Run.ksh $*\n" % (localdir, Dir))
      else:
        f.write("    rsh %s -n %s/%s/Run.ksh $*\n" % (self.name, localdir, Dir))
      f.write("  else\n")
      f.write("    echo 'Build failed for %s'\n" % Dir)
      f.write("    touch Run.failed\n")
      f.write("    touch Run.done\n")
      f.write("    exit -1\n")
      f.write("  fi\n")
    else:
      if self.islocal or NProcs>1:
        f.write("    %s/%s/Run.ksh $*\n" % (localdir, Dir))
      else:
        f.write("    rsh %s -n %s/%s/Run.ksh $*\n" % (self.name, localdir, Dir))
    f.write("else\n")
    f.write("  echo 'Cannot cd to run directory %s/%s.  Aborting'\n" % (localdir, Dir))
    f.write("fi\n")
    f.close()
    os.chmod(fname, 511)

    kshname = "%s/%s/Run.ksh" % (localdir, Dir)
    f = open(kshname, "w")
    f.write("#!/usr/bin/ksh\n")
    f.write("cd %s/%s\n" % (localdir, Dir))
    f.write(". ../RunEnv\n")
    f.write("%s $*\n" % RunStr)
    f.close()
    os.chmod(kshname, 511)

    arg=['']*6
    if userArgs != '' :
      toks = userArgs.split()
      for i in range(len(toks)):
	arg[i] = toks[i]

    if Launch and IsPosix():
      # Always launch locally since build is local - never rsh the command here
      # print "Launching in %s" % Dir
      for trycount in range(5):
	try:
          if self.islocal or BuildLocal:
            pid = os.spawnlp(os.P_NOWAIT, fname, fname, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5])
          else:
            pid = os.spawnlp(os.P_NOWAIT, "rsh", "rsh", self.name, "-n", fname, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5])
	except:
	  print 'error launching in %s' % Dir
	if pid > 0:
	  break
        time.sleep(1.0)
      if pid < 0:
	print 'failed attempt to launch in %s' % Dir
	return

      #self.jobsrunning += NProcs
      self.currentload += Cost
      # print '%s has %d jobs' % (self.name, self.jobsrunning)

    self.dirs.append(Dir)
    return pid

  def KillRuns(self):
    if not IsPosix():
      return
    if len(self.pids) < 1:
      return
    if self.islocal:
      killstr = "kill -9 "
      for p in self.pids:
        killstr += " %s" % p
      os.system(killstr)
    else:
      killargs = ["rsh", self.name, "-n"]
      killargs.append(self.pids)
      os.spawnv(os.P_NOWAIT, "rsh", killargs)

  def UsageThread(self): 
    self.usagerunning=1
    self.usagedone=0
    if self.islocal and 0:  # can't get popen to close correctly locally
      s = os.popen('vmstat 1')
    else:
      s = os.popen('rsh %s -n vmstat 1' % self.name)
    s.readline()
    s.readline()
    s.readline()
    while self.usagerunning == 1:
      time.sleep(0.01)
      l = s.readline().split()
      tmpusage = self.currentusage
      try:
        tmpusage = 100 - int(l[15])
      except:
        tmpusage = self.currentusage
      self.currentusage = tmpusage
    s.close()
    self.usagedone = 1

  def WaitUsage(self):
    self.usagerunning = 0
    i = 0
    while i < 100 and self.usagedone == 0:
      time.sleep(0.1)
      i += 1
    if (self.usagedone == 0):
      print self.name + " vmstat still running"


  def Usage(self):
    if not IsPosix():
      return 0
    if self.usagerunning == 0:
      thread.start_new_thread(self.UsageThread, ())
      self.usagerunning = 1
    return self.currentusage

  def CurrentUsage(self):
    return self.currentusage


# Enumerate all run combinations given the variable list
# Slaves do not pop a new dimension in the run parameter space
# LocalVars are the local variables picked up so far
# Recursive function
def Enumerate(FileStem, SystemID, IndepVar, i, LocalVars, Vars, OrderedVars, AllVars, Runs, globdic, SubVals):
  #print "enumerate ", FileStem, SystemID, IndepVar, i, LocalVars, Vars
  if i >= len(Vars):
    if FileStem == '':
      FileStem = '0'
    if SystemID == '':
      SystemID = '0'

    # put master values into dictionary so slaves will evaluate correctly
    for n in OrderedVars:
      if AllVars[n].compound:
        continue

      vv = LocalVars[n]
      try:
        exec("%s = %s" % (n,vv), globdic)
      except:
        exec("%s = '%s'" % (n, vv), globdic)

    # now assign the values of the slaves.
    for n in OrderedVars:
      if AllVars[n].compound == 0:
        continue

      vv = AllVars[n]
      try:
        veval = eval(vv.exprstr, globdic)
      except:
        print "cannot eval slave string %s" % vv.exprstr
        veval = eval("'%s'" % vv.exprstr, globdic)
        print "... forced it to ", veval
      try:
        LocalVars[n] = veval
      except:
        print "could not parse ", vv.exprstr
      try:
        exec("%s = %s" % (n, vv.exprstr), globdic)
      except:
        exec("%s = '%s'" % (n, vv.exprstr), globdic)

    doit = 1
    if 'DoRun' in LocalVars.keys():
      doit=LocalVars['DoRun']
    if doit:
      Runs.append(Run(FileStem, SystemID, LocalVars, SubVals))
  else:
    ii = 0
    for name in OrderedVars:
      if name in Vars.keys():
	if ii == i:
	  break
	ii += 1
    var = Vars[name]
    for k in range(len(var.values)):
      p = "%s" % FileStem
      id = "%s" % SystemID
      # need local copy
      v = LocalVars.copy()
      # v = LocalVars
      # v.append((name, var.values[k]))
      v[name] = var.values[k]
      # don't pop a filename dimension for singleton variables
      if len(var.values) > 1:
        if FileStem == '':
	  p = "%d" % k
        else:
          p = FileStem + "_%d" % k

        IsIndep = name != IndepVar
	if IsIndep:
	  for slave in var.slaves:
	    if slave.name == IndepVar:
	      IsIndep = 1
	      break
        if IsIndep:
          if SystemID == '':
	    id = "%d" % k
	  else:
	    id = SystemID + "_%d" % k


      for slave in var.slaves:
        v[slave.name] = slave.values[k]

      Enumerate(p, id, IndepVar, i+1, v, Vars, OrderedVars, AllVars, Runs, globdic, SubVals)
      
# Simple way to find out if any runs have not completed.
# LaunchRun currently creates the Run.done file
def stillrunning(Runs):
  for r in Runs:
    fname = "Run_%s/Run.done" % r.NameStem
    if os.access(fname, os.F_OK) != 1:
      return 1
  return 0

# Check if any in Runs have completed - normally or abnormally
def anydone(Runs):
  for r in Runs:
    fname = "Run_%s/Run.done" % r.NameStem
    if os.access(fname, os.F_OK) == 1:
      return r
    fname = "Run_%s/Run.failed" % r.NameStem
    if os.access(fname, os.F_OK) == 1:
      return r
    fname = "Run_%s/core" % r.NameStem
    if os.access(fname, os.F_OK) == 1:
      return r
  return ''


# Simple heuristic for getting the next most available mcahine
def GetNextMachine():
  MinUtilization = 9999999
  BestMachine = Machines[0]
  for m in Machines:
      NFreeCPUs = m.ncpus*float(m.speed)*(100.0 - m.initialusage)/100.0
      utilization = (1.0 + float(m.currentload))/NFreeCPUs
      if utilization < MinUtilization:
	MinUtilization = utilization
	BestMachine = m
  return BestMachine

def GetNextAvailMachine(Running=[]):
  for m in Machines:
    if m.jobsrunning < m.ncpus:
      return m
  if len(Running) < 1:
    print "No cpus available and no jobs running.  Exiting."
    sys.exit(-1)
  r = anydone(Running)
  while r == '':
    time.sleep(1.0)
    r = anydone(Running)   
  if r.pid > 0:
    os.waitpid(r.pid, os.WNOHANG)
  Running.remove(r)
  if r.Procs < 0:
    r.AssignedMachine.jobsrunning -= 1
    print 'job complete on %s, now has %d running' % (r.AssignedMachine.name, r.AssignedMachine.jobsrunning)
    return r.AssignedMachine
  else:
    for m in r.ParallelMachines:
      m.jobsrunning -= 1
    return r.ParallelMachines[0]


def AddMachineThread(MachineLine):
  Machines.append(Machine(MachineLine))


def LoadMachines(mrdir='./'):
  RTPMachinesFileName = mrdir + '/RTPMachines.txt'

  f = None

  try:
    f = open(RTPMachinesFileName, 'r')
  except:
    RTPMachinesFileName = os.getenv('RTPMACHINESFILE')
    if RTPMachinesFileName == None:
      print 'File RTPMachines.txt not found and RTPMACHINESFILE not set.  Running locally.'
    else:
      try:
        f = open(RTPMachinesFileName, 'r')
      except:
        print 'File %s not found.  Aborting runs.' % RTPMachinesFileName
        sys.exit(-1)

  MachineLines = []
  if f != None:
    for l in f.readlines():
      if len(l.split()) == 3:
        MachineLines.append(l)
    f.close()
  else:
    localname = socket.gethostname().split('.')[0]
    MachineLines.append(localname + ' 1 1.0')

  for ml in MachineLines:
    # thread.start_new_thread(AddMachineThread, (ml,))
    Machines.append(Machine(ml))

  #while len(MachineLines) != len(Machines):
  #  time.sleep(0.1)


def KillRuns():
  if not IsPosix():
    return
  pids = {}
  try:
    RunFile = open('RunListAll.txt', 'r')
  except:
    print 'Error opening RunListAll.txt'
    sys.exit(-1)
  # skip over first line
  RunFile.readline()
  for l in RunFile.readlines():
    s = l.split()
    if len(s) > 1:
      dir = s[0]
      # name = s[2]
      pid = " "
      if os.access('%s/Run.done' % dir, os.F_OK):
        continue
      try:
        f = open('%s/Run.pid' % dir)
        name = f.readline().strip()
	for pid in f.readlines():
	  pid = pid.strip()
          if pids.has_key(name):
            pids[name].append(pid)
          else:
            pids[name] = [pid]
        f.close()
      except:
        continue

  RunFile.close()
  for m in pids.keys():
    pidlist = pids[m]
    if socket.gethostname().split('.')[0] == m:
      killstr = "killpidtree.ksh -self "
      for p in pidlist:
        killstr += " %s" % p
      os.system(killstr)
    else:
      killstr = "rsh %s -n killpidtree.ksh -self " % name
      for p in pidlist:
        killstr += " %s" % p
      print "kill str is " + killstr
      os.system(killstr)


def ListRuns(Runs, AllVars):
  try:
    RunFile = open('RunList.txt', "w")
  except:
    print 'Error creating RunList.txt'
    sys.exit(-1)

  RunFile.write("DirName RunSet MachineName")
  for v in Runs[0].Values.keys():
    if v != 'RunCost':
      RunFile.write(" %s" % v)
  RunFile.write("\n")

  for r in Runs:
    dirname = "Run_%s" % r.NameStem
    RunFile.write("%s RunSet_%s %s" % (dirname, r.SystemID, 'localhost'))
    # print r.Values.keys()
    for v in r.Values.keys():
      # print r.Values[v]
      if r.Values[v] == 'NULL':
        RunFile.write(" _")
	continue
      if v != 'RunCost':
        # Here we replace the _s_'s with spaces
        lft = str(v).replace("_s_", " ")
        rgt = str(r.Values[v]).replace("_s_", " ")
        RunFile.write(" %s" % r.Values[v])
    RunFile.write("\n")

  RunFile.close()


def LaunchRuns(Runs, RunCommand,  BuildCommand, RTPMaster, AllVars, OrderedVars, ListOnly, RunCount, DoDir, DoDirCmd, userArgs, InternalVars, SubVals, BuildLocal=1, LaunchNoLog=0):
  # Fill in the list of runs, with machine assignment
  try:
    RunFile = open('RunListAll.txt', "w")
  except:
    print 'Error creating RunListAll.txt'
    sys.exit(-1)

  # on first line output master indep vars
  for v in Runs[0].Values.keys():
    if v != 'RunCost':
      if AllVars[v].master=='' and AllVars[v].compound==0 and len(AllVars[v].values)>1:
        RunFile.write(" %s" % v)
  RunFile.write("\n")
	

  RunFile.write("DirName RunSet MachineName")
  for v in Runs[0].Values.keys():
    if v != 'RunCost':
      RunFile.write(" %s" % v)
	
  RunFile.write("\n")
  RunFile.close()

  # For each run, create a directory and corresponding
  # rtp file.  Then launch run locally in each directory.
  # Use most available machine at the current time.
  # Runs are already sorted, in decreasing order of cost
  # Note that RunCommand is specified in the MultiRun.rtp file.
  Running = []
  for r in Runs:
    dirname = "Run_%s" % r.NameStem
    if os.access(dirname, os.F_OK) != 1:
      os.mkdir(dirname)
    else:
      if os.access("%s/Run.done"%dirname, os.F_OK) == 1 and os.access("%s/Run.failed"%dirname, os.F_OK) != 1:
	continue
      else:
        shutil.rmtree(dirname)
	os.mkdir(dirname)
    filename = "%s/Run.rtp" % dirname
    f = open(filename, "w")
    f.write(RTPMaster)

    # previously made host file only if procs > 1, but 1 is useful
    if r.Procs < 1:
      # this may delay until a cpu is available
      #print "Getting machine..."
      machine = GetNextAvailMachine(Running)
      #print "Got machine %s" % machine.name
      r.AssignedMachine = machine
      machine.jobsrunning += 1
    else:
      hfile = open("%s/host.list"%dirname, "w")
      for nprocs in range(r.Procs):
        machine = GetNextAvailMachine(Running)
        r.ParallelMachines.append(machine)
        hfile.write(machine.name+"\n")
        machine.jobsrunning += 1
      hfile.close()

    try:
      RunFile = open('RunListAll.txt', "a")
    except:
      print 'Error creating RunListAll.txt'
      sys.exit(-1)

    RunFile.write("%s RunSet_%s %s" % (dirname, r.SystemID, machine.name))
    VarFile=open("%s/Run.vars" % dirname, "w")
    for v in OrderedVars:
      if AllVars[v].master=='' and AllVars[v].compound==0 and len(AllVars[v].values)>1:
        VarFile.write(str(r.Values[v]))
        VarFile.write(" ")
    VarFile.write("\n")
    VarFile.close()
    for v in r.Values.keys():
      if r.Values[v] == 'NULL':
        RunFile.write(" _")
	continue
      if AllVars[v].argument == 1:
        f.write("#")
      if v != 'RunCost':
        # Here we replace the _s_'s with spaces
        lft = str(v).replace("_s_", " ")
        rgt = str(r.Values[v]).replace("_s_", " ")
        f.write("%s  %s\n" % (lft, rgt))
        RunFile.write(" %s" % r.Values[v])
    f.close()
    RunFile.write("\n")
    RunStr = ""
    RunCmd = "Run.cmd"
    for s in RunCommand.split():
      if s in r.Values.keys():
        RunStr = RunStr + ' ' + str(r.Values[s])
      else:
        RunStr = RunStr + ' ' + s
    DVSFileName = RunStr.split()[-1]
    BuildStr = ''
    if BuildCommand != '':
      BuildStr = BuildCommand.split()[0] + " -dvs " + DVSFileName + " "
      # print "BuildCommand is ", BuildCommand
      for s in BuildCommand.split()[1:]:
        if s in r.Values.keys():
          BuildStr = BuildStr + ' ' + str(r.Values[s])
        elif s in InternalVars.keys():
	  BuildStr = BuildStr + ' ' + str(InternalVars[s])
        else:
          BuildStr = BuildStr + ' ' + s
        # print BuildStr
    # print "Build Str is ", BuildStr
    if DoDir:
      RunStr = DoDirCmd
      RunCmd = "Do.cmd"
    time.sleep(5.0)  # wait a bit to avoid db2 collisions
    r.pid = machine.DoRun(dirname, RunStr, BuildStr, r.Cost, not ListOnly, RunCmd, userArgs, BuildLocal, r.Procs, NoLog=LaunchNoLog)
    Running.append(r)
    RunCount += 1
    RunFile.close()

def TrimFileNames(s):
  l = ''
  for n in s.split():
    if n.rfind('/'):
      l += n[n.rfind('/')+1:]
    else:
      l += n
    l += ' '
  return l
    

def SummarizeResults():
  try:
    f = open('RunListAll.txt', 'r')
  except:
    print "Can't open RunListAll.txt"
    sys.exit(-1)

  try:
    fout = open('RunResults.txt', 'w')
  except:
    print "Can't open RunResults.txt for writing'"
    sys.exit(-1)

  MasterVarLine = f.readline().strip()
  MasterVars = MasterVarLine.split()
  fout.write(MasterVarLine)
  fout.write("\n")
  # Get the universal run parameter header labels that apply to all runs
  HeaderLine = f.readline().rstrip()
  for l in f.readlines():
    # Begin the 4-line output per run

    # Run parameter labels
    fout.write("%s\n" % HeaderLine)

    # Run parameter values
    #fout.write("%s\n" % l.rstrip())
    fout.write("%s\n" % TrimFileNames(l.rstrip()))

    # Get the directory name
    RunDir = l.split()[0]

    RealTime   = '0.0'
    UserTime   = '0.0'
    SystemTime = '0.0'
    TimePerTS  = '0.0'

    try:
      TimeFile = open("%s/Run.log" % RunDir, 'r')
      for tfl in TimeFile.readlines():
	t = tfl.split()
	if len(t) > 1:
  	  if t[0] == 'Real':
	    RealTime = t[1]
	  elif t[0] == 'User':
	    UserTime = t[1]
	  elif t[0] == 'System':
	    SystemTime = t[1]
      TimeFile.close()
    except:
      pass

    try:
      TimeStepFile = open("%s/Run.time" % RunDir, 'r')
      t = TimeStepFile.readline()
      TimePerTS = t.split()[1]
      TimeStepFile.close()
    except:
      pass

    failed = 0
    fname = "%s/Run.failed" % RunDir
    if os.access(fname, os.F_OK) == 1:
      failed = 1

    SystemID = '0'
    CTPID = '0'
    SRCID = '0'
    try:
      sysprefix = '//%system_id:'
      ctpprefix = '//%ctp_id:'
      srcprefix = '//%source_id:'
      p = os.popen('grep _id: %s/*.msd' % RunDir)
      l = p.readline()
      while l != '':
	l = l.strip()
	if l.startswith(sysprefix):
	  SystemID = l[len(sysprefix):]
	if l.startswith(ctpprefix):
	  CTPID = l[len(ctpprefix):]
	if l.startswith(srcprefix):
	  SRCID = l[len(srcprefix):]
        l = p.readline()
      p.close()
    except:
      print 'System and Source IDs not found in cpp file'
	
    try:
      LogFile = open("%s/Run.final" % RunDir, 'r')
      line1 = LogFile.readline().rstrip()
      line2 = LogFile.readline().rstrip()
      if len(line1.split()) < 1 or len(line1.split()) != len(line2.split()):
        failed = 1
        fout.write("RealTime UserTime SystemTime SystemID SourceID RunStatus TimePerTS\n")
        fout.write("%8s %8s %10s %9s %9s %9s %9s\n" % (RealTime, UserTime, SystemTime, SystemID, SRCID, failed, TimePerTS))
        fout.write("\n")
      else:
        # Output final mdlog labels and values, which may be different for each run
        fout.write("%s RealTime UserTime SystemTime SystemID SourceID RunStatus TimePerTS\n" % line1)
        fout.write("%s %8s %8s %10s %9s %9s %9s %9s\n" % (line2, RealTime, UserTime, SystemTime, SystemID, SRCID, failed, TimePerTS))
        fout.write("\n")
      LogFile.close()
    except:
      failed = 1
      fout.write("RealTime UserTime SystemTime SystemID SourceID RunStatus TimePerTS\n")
      fout.write("%8s %8s %10s %9s %9s %9s %9s\n" % (RealTime, UserTime, SystemTime, SystemID, SRCID, failed, TimePerTS))
      fout.write("\n")
  fout.close()
  f.close()

