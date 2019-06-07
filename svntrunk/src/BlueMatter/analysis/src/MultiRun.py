#!/usr/bin/env python

"""
  See MultiRunBase.py for description
"""
from __future__ import print_function

from Numeric import *
import os
import sys
import time
import re
import bgutils
from MultiRunBase import *

# Some globals
Vars = {}
OrderedVars = []
CompoundVars = []
InternalVars = {}
SubstitutionVars = {}
AllVars = {}
Runs = []
AnalysisCommand = ''
InitCommand = ''
RTPMaster = ''
BuildCommand = ''
IndependentVariable = 'OuterTimeStepInPicoSeconds'
DoSummary = 0
DoDir = 0
DoDirCmd = ''
UserArgs = ''
DoRun = 1
InternalVars["DoRun"] = 1
DataDirSet = 0
BuildLocal = 1
TestOnly = 0
NoLog = 0

localdir = CleanDirName(os.getenv('PWD'))
MultiRunDirectory = localdir
DataDirectory = localdir

BG_INSTALL_BASE = CleanDirName(bgutils.installbase())
BG_MDTEST_DIR   = BG_INSTALL_BASE+"/usr/opt/bluegene/mdtest"

InternalVars["BG_INSTALL_BASE"] = BG_INSTALL_BASE
InternalVars["BG_MDTEST_DIR"]   = BG_MDTEST_DIR

# Make the generated files readable by others and the web server
os.umask(0o22)

ListOnly = 0
argnum = 1
nargs = len(sys.argv)
while argnum < nargs:
  if sys.argv[argnum] == '-listonly':
    ListOnly = 1
  elif sys.argv[argnum] == '-summarize':
    DoSummary = 1
  elif sys.argv[argnum] == '-nolog':
    NoLog = 1
  elif sys.argv[argnum] == '-testonly':
    TestOnly = 1
  elif sys.argv[argnum] == '-distributebuild':
    BuildLocal = 0
  elif sys.argv[argnum] == '-dir':
    MultiRunDirectory = sys.argv[argnum+1]
    argnum += 1
  elif sys.argv[argnum] == '-set':
    (n,v) = sys.argv[argnum+1].split('=')
    try:
      x = float(v)
      exec("%s=%s"%(n,v))
    except:
      exec("%s='%s'"%(n,v))
    argnum += 1
  elif sys.argv[argnum] == '-datadir':
    DataDirectory = sys.argv[argnum+1]
    DataDirSet = 1
    argnum += 1
  elif sys.argv[argnum] == '-do':
    DoDir = 1
    DoDirCmd = ' '.join(sys.argv[argnum+1:])
    argnum += 1
  elif sys.argv[argnum] == '-analyzeonly':
    UserArgs += " " + sys.argv[argnum]
  elif sys.argv[argnum] == '-buildonly':
    UserArgs += " " + sys.argv[argnum]
  elif sys.argv[argnum] == '-killall':
    KillRuns()
    sys.exit(1)
  elif sys.argv[argnum] == '-runonly':
    UserArgs += " " + sys.argv[argnum]
  else:
    print("Command option %s not recognized" % sys.argv[argnum])
    print("Usage:  MultiRun.py [-listonly] [-summarize] [-dir DIR] [-do cmd] [-analyzeonly] [-buildonly] [-killall]")
    sys.exit(-1)
  argnum += 1
  if DoDir:
    break

if DoSummary:
  SummarizeResults()
  sys.exit(0)

MultiRunDirectory = CleanDirName(MultiRunDirectory)
SubstitutionVars['MRSOURCEDIR'] = MultiRunDirectory
if DataDirSet:
  SubstitutionVars['MRDATADIR'] = CleanDirName(DataDirectory)
else:
  SubstitutionVars['MRDATADIR'] = MultiRunDirectory
SubstitutionVars['MRDESTDIR']   = localdir
SubstitutionVars['BG_INSTALL_BASE'] = BG_INSTALL_BASE
SubstitutionVars['BG_MDTEST_DIR'] = BG_MDTEST_DIR

cmdstr = """env | grep -v "^SHELL=" | grep -v "^LOGIN=" | grep -v "^LOGNAME=" | grep -v "{NL}" | grep -v "^MACHINE=" | grep -v "^HOST" | grep -v "^PWD=" | grep -v "^LS_COLORS=" | awk '{print "export \\\"" $1 "\\\""}' > RunEnv"""
if IsPosix():
    os.system(cmdstr)

# The following loads the rtp files and parses the variables
# This cannot be in a separate subroutine because the exec's
# would only happen in local scope

# load the master rtp file
try:
  f = open(MultiRunDirectory + '/MultiRun.rtp')
except:
  print("Can't find file %s/MultiRun.rtp" % MultiRunDirectory)
  sys.exit(-1)

# read and build the corresponding RTPVars
# also accumulate the common lines into RTPMaster
line = None
while line != '':
  line = f.readline()
  l = line.strip()

  # each #python line may be loading a single variable or several lists in columns
  # thus valexprs returns 1 or more new variable definitions
  if l.startswith('#python'):
    try:
      valexprs = []
      s = l.split()
      if s[1] == 'const' or s[1] == 'constant':
	try:
	  v = re.split("\s+", l, 3)[3]
	  exec(s[2] + '=' + v)
	  ival = eval(v)
	  InternalVars[s[2]] = ival
	except:
	  print("Trouble parsing %s" % l)
	  os.exit(-1)
      if s[1] == 'default':
        try:    # is it already defined from command line?
          ival = eval(s[2])
          InternalVars[s[2]] = ival
        except: # no - 
  	  try:
	    v = re.split("\s+", l, 3)[3]
	    exec(s[2] + '=' + v)
	    ival = eval(v)
	    InternalVars[s[2]] = ival
	  except:
            v = "'%s'" % v
            exec(s[2] + '=' + v)
            ival = eval(v)
            InternalVars[s[2]] = ival
	    #print "Trouble parsing %s" % l
	    #os.exit(-1)
      elif s[1] == 'var' or s[1] == 'variable':
        valexprs = GetValueExpressions(l, 3, f, s[2], globals())
      elif s[1] == 'argument' or s[1] == 'arg':
        valexprs = GetValueExpressions(l, 3, f, s[2], globals(), argument=1)
      elif s[1] == 'master':
        if s[3] != 'argument' and s[3] != 'arg':
          valexprs = GetValueExpressions(l, 4, f, s[3], globals(), master=s[2])
        else:
          valexprs = GetValueExpressions(l, 5, f, s[4], globals(), argument=1, master=s[2])
      elif s[1] == 'compound':
        if s[2] != 'argument' and s[2] != 'arg':
          valexprs = GetValueExpressions(l, 3, f, s[2], globals(), argument=0, compound=1)
        else:
          valexprs = GetValueExpressions(l, 4, f, s[3], globals(), argument=1, compound=1)
      elif s[1] == 'internal' or s[1] == 'intern':
        t = "%s = " % s[2]
        #t = t + re.split("\s+", l, 3)[3]
        expr = re.split("\s+", l, 3)[3]
	t += expr
	#valexprs = [t]
	try:
	  exec(t)
	  # print "just executed internal command ", t
	  ival = eval(expr)
	  InternalVars[s[2]] = ival
	except:
	  print("Trouble parsing %s" % l)
	  os.exit(-1)
    except:
      print("Trouble parsing: %s\n" % l)
      sys.exit(-1)

    # now you have all the variables for a given #python line so create RTPVariables for them
    # and define them in the global dictionary
    if len(valexprs) > 0:
      try:
	for l in valexprs:
          if (l.compound == 0):
	    exstr = "%s = %s" % (l.name, l.exprstr)
	    eval(l.exprstr)
	    exec(exstr)
	    RTPVar(l.name, eval(l.exprstr), Vars, AllVars, OrderedVars, l.exprstr, l.isstring, l.argument, l.master, l.compound)
          else:
            RTPVar(l.name, [0], Vars, AllVars, OrderedVars, l.exprstr, l.isstring, l.argument, l.master, l.compound)
      except:
	print("Trouble parsing: %s\n" % exstr)
	sys.exit(-1)
	  
  else:
    if (l != '') and (l != '\n') and not l.startswith("#"):
      RTPMaster += l+"\n"

f.close()

LoadMachines(MultiRunDirectory)

# Enumerate all runs
ss = ''
id = ''
IndepVar = IndependentVariable

Enumerate(ss, id, IndepVar, 0, {}, Vars, OrderedVars, AllVars, Runs, globals(), SubstitutionVars)

# Sort runs in decreasing order of cost
Runs.sort(lambda x,y: cmp(y.Cost, x.Cost))

RunCount = 0

if ListOnly:
  ListRuns(Runs, AllVars)
  sys.exit(0)

# Launch analysis
if InitCommand != '':
  InitCommand = SubstituteStrings(InitCommand, SubstitutionVars)
  print('Initializing')
  os.system(InitCommand)
  print('Initialization Complete')

if TestOnly:
  Runs = Runs[:1]

LaunchRuns(Runs, RunCommand, BuildCommand, RTPMaster, AllVars, OrderedVars, ListOnly, RunCount, DoDir, DoDirCmd, UserArgs, InternalVars, SubstitutionVars, BuildLocal, NoLog)

# Poll for completion of all runs
if not ListOnly and IsPosix():
  # poll till all runs complete
  while stillrunning(Runs):
    time.sleep(0.5)


print()
print("Runs Complete")
print()


# Now pull the results together
# Get last mdlog line from each run
# and place into RunResults.txt

if not ListOnly:
  SummarizeResults()

# Launch analysis
if AnalysisCommand != '':
  AnalysisCommand = SubstituteStrings(AnalysisCommand, SubstitutionVars)
  print('Launching Analysis')
  os.system(AnalysisCommand)
  print('Analysis Complete')

print("MultiRun Complete")
