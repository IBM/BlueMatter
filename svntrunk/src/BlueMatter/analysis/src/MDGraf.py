#!/usr/bin/env python
import os
import sys
import glob
import MDPlot2

ts = 1
ens = ''
lrf = ''
wat = ''
temp = ''
tsstr = ''
tempcntrl = ''
multiplot = 1

if len(sys.argv) > 1:
  os.chdir(sys.argv[1])

f = open("Run.rtp", 'r')
lines = f.readlines()
for l in lines:
  s = l.split()
  if s[0] == 'OuterTimeStepInPicoSeconds':
    tsstr = s[1]
    ts = float(s[1])
  elif s[0] == 'Ensemble':
    ens = s[1]
  elif s[0] == 'VelocityResampleTargetTemperature':
    temp = s[1]
  elif s[0] == 'LRFMethod':
    lrf = s[1]
  elif s[0] == 'WaterModel':
    wat = s[1]
  elif s[0] == 'TempControl':
    tempcntrl = s[1]

#if ens != 'NVE':
#  multiplot = 1

labs = {}
udfs = []
startvals = {}
ensemble = MDPlot2.Ensemble()
range = [0, 0]
MDPlot2.ParseColumns('Run.mdlog', ensemble, labs, udfs, startvals, range)
minstep = range[0]
xmin = ts*(minstep + 1)
maxstep = MDPlot2.GetMaxStep('Run.mdlog')
xmax = ts*maxstep

f = open("varlab.gp", 'w')
f.write("""
save var "vars.dat"
!awk  '/^$0 /{printf("set label \\"%s = %8g\\" at graph %s, %s\\n", $$1, $$3, $1, $2)}' vars.dat > labval.gp
load "labval.gp"
!rm -f vars.dat
!rm -f labval.gp
""")
f.close()

f = open("Run.gp", 'w')
f.write("ts = %g\n" % ts)
f.write("xxmin = %g\n" % xmin)
f.write("xxmax = %g\n" % xmax)
f.write("""
set terminal png
set output "Run.png"
f(x) = slope*x + intercept
fit [xxmin:xxmax] f(x) 'Run.mdlog' using (ts*$1):7 via slope, intercept
""")

try:
  exestr = glob.glob("*.exe")[0]
except:
  exestr = ''
exeindex = exestr.rfind('.aix.exe')
if exeindex > 0:
  exestr = exestr[:exeindex]
titlestr = "%s ..%s" % (exestr, os.getcwd()[-50:])
f.write('set title "%s"\n' % titlestr)

print "Graphing %s" % titlestr
  
h = 0.05
if multiplot:
  dh = 0.06
else:
  dh = 0.04
l = 0.05
h += dh  # for the slope label
f.write('set label "dT = %s" at graph %f , %f\n' % (tsstr, l, h))
h += dh

if ens != '':
  f.write('set label "%s" at graph %f , %f\n' % (ens, l, h))
  h += dh
if ens == 'NPT' or ens == 'NVT' and temp != 0:
  f.write('set label "T = %s" at graph %f , %f\n' % (temp, l, h))
  h += dh
if wat != '':
  f.write('set label "Water = %s" at graph %f , %f\n' % (wat, l, h))
  h += dh
if lrf != '':
  f.write('set label "LRF = %s" at graph %f , %f\n' % (lrf, l, h))
  h += dh
if tempcntrl != '' and ens != 'NVE' and ens != 'NPH':
  f.write('set label "TempCntrl = %s" at graph %f , %f\n' % (tempcntrl, l, h))
  h += dh

if multiplot:
  f.write("set xrange [%f:%f]" % (xmin, xmax))
  f.write("""
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
  set y2label "TotalEnergy"
  set y2tics
  set ytics nomirror
  call "varlab.gp" slope 0.05 0.05
  plot [xxmin:xxmax] 'Run.mdlog' using (ts*$1):7 t "ConsQuant" w l 3, f(x) t "CQ Fit" w l 12, 'Run.mdlog' using (ts*$1):2 axes x1y2 t "TotalE" w l 6
  set title ""
  set size 1.0, 0.5
  set origin 0.0, 0.0
  set xtics
  set xlabel "Time (ps)"
  set ylabel "Temperature"
  set tmargin 0
  set bmargin 5
  set nolabel
  fit [xxmin:xxmax] f(x) 'Run.mdlog' using (ts*$1):6 via slope, intercept
  call "varlab.gp" slope 0.05 0.05
""")

if ensemble.ConstP:
  f.write('set y2label "Volume"\n')
  f.write('plot [xxmin:xxmax] "Run.mdlog" using (ts*$1):%d t "Temp" w l 6, "Run.mdlog" using (ts*$1):%d axes x1y2 t "Volume" w l 3, f(x) t "Temp Fit" w l 12' % (labs["Temp"], labs["Volume"]))
else:
  f.write('set noy2tics\n')
  f.write('set y2label ""\n')
  f.write('plot [xxmin:xxmax] "Run.mdlog" using (ts*$1):%d t "Temp" w l 6, f(x) t "Temp Fit" w l 12' % labs['Temp'])
f.write("""
set nomultiplot
set output
set terminal x11
#replot
#pause -1
""")
f.close()

os.popen("echo 'gnuplot Run.gp >& gp.log' | csh -s")
