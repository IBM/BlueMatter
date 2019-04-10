#!/usr/bin/env python

from Tkinter import *
from plot import *
from xreadlines import *
import Pmw
import sys
import os

SkipStart=0

if len(sys.argv) < 2:
    print
    print 'Usage: MDPlot.py FileName.mdlog [-relative] [-skip N]'
    print '   or: MDPlot.py -    to read from stdin'
    sys.exit(1)

Relative = 0
if len(sys.argv) > 2: 
   if sys.argv[2] == '-skip':
      SkipStart = int(sys.argv[3])
   if sys.argv[2] == '-relative':
      Relative = 1


root = Tk()

FName = sys.argv[1]

text = Pmw.ScrolledText(root, hscrollmode='static', text_wrap='none',
    text_font='fixedsys', usehullsize=1, hull_width=200, hull_height=100)

balloon = Pmw.Balloon(root)

if FName == '-':
    file = sys.stdin
else:
    file = open(FName, 'r')

wcf = os.popen('wc %s' % FName)
NLines = int(wcf.readline().split()[0]) - 1
wcf.close()

NSkip = 0
if NLines > 800:
  NSkip = int(int(NLines)/800 - 1)
if NSkip < 0:
  NSkip = 0

first = 1
data = []
SkipRange = range(NSkip)
lines = xreadlines(file)
i = 0
while i < NLines:
    l = lines[i]
    text.insert(END, l)
    if first == 1:
        labels = l.split()
        first = 0
        for num in range(len(labels)):
####
	    if labels[num] == 'ConsQuant':
	       CQcol = num
####
            data.append([])
        i += 1
        l = lines[i]
        text.insert(END, l)
    if i < SkipStart:
      i += 1
      continue
    s = l.split()
    col = 0
    for num in s:
        data[col].append(float(num))
        col = col + 1
    for r in SkipRange:
      i += 1
      if i < NLines:
        lines[i] 
    i += 1

file.close()

if Relative:
   for i in range(len(data)):
	if i == 0 :
	   continue
	for j in range(len(data[i])):
	    delta = data[i][-1] - data[CQcol][-1]
	    data[i][j] -= delta
#	    labels[i] = labels[i] + " - " + str(delta)


text.pack(fill=X)

shifts = []


MainLabels = ['TotalE', 'IntraE', 'InterE', 'KE','ConsQuant']
SpecialLabels = ['ConsQuant', 'TotalE']
IgnoreLabels = ['Step', 'Temp']
PressureLabels = ['Volume', 'VolumeVeloc', 'Virial', 'IntPressure']

colors = ['black', 'blue', 'red', 'darkgreen', 'darkred', 'lightblue']

GraphColumns = [[], [], [], []]
i = 0
for eachLabel in labels:
    if eachLabel == 'Step':
        StepData = data[i]
        NSteps = len(StepData)
    if eachLabel in MainLabels:
        GraphColumns[0].append(i)
    elif eachLabel not in IgnoreLabels:
        GraphColumns[1].append(i)
    if eachLabel in SpecialLabels:
        GraphColumns[2].append(i)
    if eachLabel == 'ConsQuant':
	GraphColumns[3].append(i)
    i = i+1

graphObjects = []
graphs = []

for igraph in range(4):
    UDFLines = []
    graphs.append(GraphBase(root, 500, 180, relief=SUNKEN, border=2))
    for i in GraphColumns[igraph]:
        UDFData = []
        for j in range(NSteps):
            UDFData.append((StepData[j], data[i][j]))
        UDFLines.append(GraphLine(UDFData, labels[i], smooth=0, color=colors[i%len(colors)]))
        balloon.tagbind(graphs[-1].canvas, labels[i], labels[i])
    graphObjects.append(GraphObjects(UDFLines))
    graphs[-1].pack(side=TOP, fill=BOTH, expand=YES)
    graphs[-1].draw(graphObjects[-1], 'automatic', 'automatic')

root.mainloop()
