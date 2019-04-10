#!/usr/bin/env python

from string import *
from runresults import *
import sys

try:
    import Gnuplot, Gnuplot.PlotItems, Gnuplot.funcutils
except ImportError:
    # kludge in case Gnuplot hasn't been installed as a module yet:
    import __init__
    Gnuplot = __init__
    import PlotItems
    Gnuplot.PlotItems = PlotItems
    import funcutils
    Gnuplot.funcutils = funcutils

def DoPlot(ewald, p3me):
    f = open("p.tmp", "w")
    for i in range(len(p3me)):
      f.write("%f %f\n" % (p3me[i][0], p3me[i][1]))
    f.close()

    f = open("e.tmp", "w")
    for i in range(len(ewald)):
      f.write("%f %f\n" % (1.0/ewald[i][0], ewald[i][1]))
    f.close()

    g = Gnuplot.Gnuplot(debug=0)
    g('set terminal png')
    g('set output "P3ME_Ewald.png"')
 
    g.set_boolean('x2tics', 1)
    g.ylabel('TotalEnergy')
    g.set_string('x2label', 'P3ME Grid Spacing')
    g.set_string('xlabel', '1/EwaldKMax')
    g.set_range('xtics',  'nomirror')
 
    g.plot(Gnuplot.File("p.tmp", using='1:2', with='linespoints', axes='x2y1', title='p3me'),
      Gnuplot.File("e.tmp", using='1:2', with='linespoints', axes='x1y1', title='ewald'))
    #raw_input("press return")


# load RunResults.txt
# Should contain N Ewald runs with increasing KMax, and N P3ME runs with increasing spacing

data = RunResults(sys.argv[1])

keys = data.keys()

all = []
ew = []
p3m = []

for i in range(len(keys)):
  # all contains the triplet of KMax, spacing, and CQ (Final value)
  all += [[data[keys[i]][0]['LRFMethod'], data[keys[i]][0]['EwaldKmax'], data[keys[i]][0]['P3MEinitSpacing'], data[keys[i]][0]['ConsQuant']]]
  if all[i][0] == 'P3ME':
    p3m += [[atof(all[i][2]), atof(all[i][3])]]
  else:
    ew += [[atof(all[i][1]), atof(all[i][3])]]

p3m.sort()
ew.sort()

DoPlot(ew, p3m)

ew.reverse()

ewdlt = abs(ew[1][1]-ew[0][1])
p3mdlt = abs(p3m[1][1]-p3m[0][1])
diff = abs(ew[0][1]-p3m[0][1])

if diff < ewdlt+p3mdlt:
  print "Pass"
else:
  print "Fail"


