#! /usr/bin/env python

import time
import math
import sys
import Numeric
import os

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


def wait(str=None, prompt='Press return to close window...\n'):
    if str is not None:
        print str
    raw_input(prompt)

class Ensemble:
    def __init__(self, ens="NVE"):
      self.Name = ens;
      self.ConstT = 0
      self.ConstP = 0
      self.ConstV = 1
      self.ConstE = 1
      self.ConstN = 1
      if (ens == 'NVT' or ens == 'NPT'):
	self.ConstT = 1
	self.ConstE = 0
      if (ens == 'NPH' or ens == 'NPT'):
	self.ConstP = 1
	self.ConstV = 0
	self.ConstE = 0

    def setEnsemble(self):
      self.Name = "NVE"
      if self.ConstT:
	if self.ConstP:
	  self.Name = "NPT"
	else:
	  self.Name = "NVT"
      else:
	if self.ConstP:
	  self.Name = "NPH"

def GetMaxStep(lf):
    tries = 0
    while tries < 300:
      tries += 1
      try:
        f = os.popen('wc %s' % lf)
        l = f.readline()
	f.close()
	nlines = l.split()[0]
	# wait for at least 4 lines
	if nlines < 4:
	  time.sleep(5)
	  continue
      except:
	time.sleep(5)
	continue
      try:
        f = os.popen('tail -1 %s' % lf)
        l = f.readline()
        f.close()
        return int(l.split()[0])
      except:
	time.sleep(5)
	continue
    return -1 
     
def ParseColumns(lf, ens, labs, udfs, startvals, range):
    nonudfs = ['Step', 'TotalE', 'IntraE', 'InterE', 'KE', 'Temp', 'ConsQuant', 'CQRMSDEst', 'IntPressure', 'Virial', 'Volume', 'VolVeloc']
    f = open(lf)
    l = f.readline()
    v = f.readline()
    f.close()
    toks = l.split()
    vals = v.split()
    i = 1
    for t in toks:
      if t == 'ELoss':
	ens.ConstT = 1
      elif t == 'Volume':
	ens.ConstP = 1
      labs[t] = i;
      startvals[t] = vals[i-1]
      if not t in nonudfs:
	udfs.append(t)
      i += 1

    ens.setEnsemble()
    range[0] = int(startvals['Step'])
    range[1] = GetMaxStep(lf)


def main():
    """MDPlot2.py mdlogfile -udfs -poll -polltime 60 -ts 0.001 """

    nargs = len(sys.argv)
    if nargs < 2:
      print "MDPlot2.py mdlogfile -udfs -poll -polltime 60 -ts 0.001"
      sys.exit()

    showudfs = 0
    poll = 0
    polltime = 60
    ts = 0.001

    logfile = sys.argv[1]

    i = 2

    while i < nargs:
      s = sys.argv[i]
      if s == '-udfs':
	showudfs = 1
      elif s == '-poll':
	poll = 1
      elif s == '-ts':
	i += 1
	ts = float(sys.argv[i])
      elif s == '-polltime':
	i += 1
	polltime = float(sys.argv[i])
	poll = 1
      i += 1

    hfrac = 0.5
    if showudfs:
      hfrac = 1.0/3.0
    ens = Ensemble()

    labs = {}
    udfs = []
    startvals = {}

    if GetMaxStep(logfile) < 0:
      print 'timeout waiting for 3 lines in ', logfile
      sys.exit(-1)
    range = [0, 0]
    ParseColumns(logfile, ens, labs, udfs, startvals, range) 
    minstep = range[0]
    oldmaxstep = -1000000
    # skip first data value so it doesn't throw off scales
    xmin = ts*(minstep + 1)
    g = Gnuplot.Gnuplot(debug=0)
    g.set_range('lmargin', '15')
    g.set_range('rmargin', '15')
    doplot = 1
    #g('set term postscript eps enhanced dashed')
    #g("set output 'MDPlot.eps'")

    while poll or doplot :
      maxstep = GetMaxStep(logfile)
      if maxstep == oldmaxstep:
	time.sleep(polltime)
	continue
      oldmaxstep = maxstep
      xmax = ts*maxstep
      g('set bmargin 0')
      g('set tmargin 2')
      g.set_range('size', '1.0, 1.0')
      g.set_boolean('multiplot', 1)
      g.set_range('size', '1.0, %f' % (hfrac))
      g.set_range('origin', '0.0, %f' % (1.0-hfrac))
      g.set_boolean('xtics', 0)
      #g1= Gnuplot.Gnuplot(debug=1)
  
      g.set_boolean('y2tics', 1)
      g.ylabel('TotalEnergy')
      g.set_string('y2label',  'ConsQuant')
      g.set_range('ytics',  'nomirror')
  
      g.set_range('xrange', '[%f:%f]'%(xmin, xmax))

      g.plot(Gnuplot.File(logfile, using='(%f*$1):%d'%(ts, labs['TotalE']), with='lines', title='TotalEnergy'),
             Gnuplot.File(logfile, using='(%f*$1):%d'%(ts, labs['ConsQuant']), axes='x1y2', with='lines', title='ConsQuant'))

    
      g.set_range('size', '1.0,%f'%(hfrac))
      g.set_range('origin', '0.0, %f' % (1.0-2*hfrac))
      g.set_range('xrange', '[%f:%f]'%(xmin, xmax))
      g('set tmargin 0')
      if not showudfs:
        g.set_boolean('xtics', 1)
	g('set bmargin 2')
      g.ylabel('')
      if ens.ConstP:
        g.set_string('y2label', 'Volume')
        g.plot(Gnuplot.File(logfile, using='(%f*$1):%d'%(ts, labs['Temp']), with='lines', title='Temperature'),
	     Gnuplot.File(logfile, using='(%f*$1):%d'%(ts, labs['Volume']), axes='x1y2', with='lines', title='Volume'))
      else:
	g.set_string('y2label', '')
	g.set_boolean('y2tics', 0)
        g.plot(Gnuplot.File(logfile, using='(%f*$1):%d'%(ts,labs['Temp']), with='lines', title='Temperature'))
	
      if showudfs:
        g.set_range('size', '1.0,%f'%(hfrac))
        g.set_range('origin', '0.0, 0.0')
        g.set_range('xrange', '[%f:%f]'%(xmin, xmax))
        g.set_boolean('xtics', 1)
        g.ylabel('')
        g.set_string('y2label', '')
        g.set_boolean('y2tics', 0)
        g.set_boolean('ytics', 1)
        g('set key below spacing 0.25')

        i = 1
        g._clear_queue()
        pitems = []
        for u in udfs:
          g._add_to_queue([Gnuplot.File(logfile, using='(%f*$1):($%d-%s)'%(ts, labs[u], startvals[u]), with='lines %d'%i, title=u)])
	  i += 1
        g.plotcmd = 'plot'
        g.refresh()
      
      g.set_boolean('multiplot', 0)
      doplot = 0
      if poll:
        time.sleep(polltime)

    wait('pause')
    #g('set output')
    #g('set term x11')


# when executed, just run main():
if __name__ == '__main__':
    main()

