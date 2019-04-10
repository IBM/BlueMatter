#!/usr/bin/env python
import os
import sys

if len(sys.args) < 7:
  print "QuadraticPlot.py RunSetName NTimes dir1 dir2 ... dirN t1 t2 ... tN (short to long)"
  sys.exit(-1)

RunSetName = sys.args[1]
NTimes = int(sys.args[2])

dirs = []
ts = []
for i in range[NTimes]:
  dirs.append(sys.args[3+i])
  ts.append(float(sys.args[3+NTimes+i]))

f = open("%s/Run.mdlog"%dirs[0], "r")
f.readline()
CQ0 = f.readline().split()[6]
f.close()

f = open("%s.gp"%RunSetName, 'w')

factor = map(lambda x: (0.001/x)**2, ts)

PlotStr = "plot"

for i in range(0, len(ts)):
  if i != 0:
    PlotStr += ", "
  PlotStr += "'%s/Run.mdlog' using (%s*$1):(($7-%s)*%f) t '' w l"%(dirs[i], ts[i], CQ0, factor[i])

f.write("""
set terminal png
set output "%s.png"
set bmargin 0
set tmargin 2
set lmargin 15
set rmargin 15
set size 1.0, 1.0
set multiplot
set size 1.0, 0.5
set origin 0.0, 0.5
set noxtics
set ylabel "ConsQuant"
"""%RunSetName)

f.write(PlotStr)

f.write("""
set title ""
set size 1.0, 0.5
set origin 0.0, 0.0
set bmargin 2
set tmargin 0
set xtics
set xlabel "Time (ps)"
set ylabel "Temperature"
set y2label "Total Energy"
set y2tics
set ytics nomirror
set nolabel
plot '%s/Run.mdlog' using (%f*$1):($6) t "Temp" w l 3, '%s/Run.mdlog' using (%f*$1):($2) t "E" axes x1y2 w l 2
set nomultiplot
set output
set terminal x11
#replot
#pause -1
"""%(dirs[0], ts[0], dirs[0], ts[0]) )

f.close()

os.popen("echo 'gnuplot QC.gp >& gp.log' | csh -s")
