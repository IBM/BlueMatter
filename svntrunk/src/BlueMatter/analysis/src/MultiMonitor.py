#!/usr/bin/env python
from Tkinter import *
import os
import sys
import thread
import time
import Pmw
import ProgressBar
import socket
from MultiRunBase import *

bars = []
root = Tk()
running = 0

def GetMachines():
  RTPMachinesFileName = 'RTPMachines.txt'
  try:
    MachineFile = open(RTPMachinesFileName)
  except:
    RTPMachinesFileName = os.getenv('RTPMACHINESFILE')
    if RTPMachinesFileName == None:
      print "Can't find file RTPMachines.txt locally, and RTPMACHINESFILE is not set" 
      sys.exit(-1)
    try:
      MachineFile = open(RTPMachinesFileName)
    except:
      print "Can't find RTPMachines file %s" % RTPMachinesFileName
      sys.exit(-1)

  for l in MachineFile.readlines():
    if l.rstrip() == '':
      break
    Machines.append(Machine(l))
  havelocal = 0
  for m in Machines:
    if m.islocal != 0:
      havelocal = 1
  if havelocal == 0:
    Machines.insert(0, Machine("%s 1 1.0" % socket.gethostname().split('.')[0]))

  MachineFile.close()

def update():
  i = 0
  for b in bars:
    b.updateProgress(Machines[i].CurrentUsage())
    i += 1
  root.after(1000, update)

def UpdateThread():
  while running:
    for m in Machines:
      m.Usage()
      if not running:
	break
    time.sleep(0.01)

GetMachines()

for m in Machines:
  p = ProgressBar.ProgressBar(root, doLabel=0, labelColor="black",
    labelText=m.name,
    labelFormat="%s",
    fillColor="blue",
    width=180, height=15)
  p.frame.pack(side=TOP)
  p.updateProgress(m.initialusage, 100)
  bars.append(p)

# root.after(1000, update)

root.title('Usage Monitor')

update()

running = 1

thread.start_new_thread(UpdateThread, ())

root.mainloop()

running = 0

for m in Machines:
  m.usagerunning = 0

for m in Machines:
  m.WaitUsage()

time.sleep(1.0)

