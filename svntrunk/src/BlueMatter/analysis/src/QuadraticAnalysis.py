from __future__ import print_function
from Numeric import *
import sys
import os
import time

# called with init, loop over runset, finalize

# Load runset file - Get list of runsets and master vars
# Start master html 
# Date and BG_BIN_DIR - get from RunEnv
# Invoke QuadraticCons on the rdg's - per runset
# Accumulate all values of rat and erat - per runset
# Make scatter plot of results - possibly only shortest values
# Build table of simplified results
# Build table of all build params - using first run - cull master vars

# For each runset:
# Make double plot of CQDel and E, using gnuplot to subtract CQ0 from each val and scale by ts - plus Temp.
# Show table of results/ratio for each run, including timestep
# Make RunSet.html and RunSet.gif

# AnalyzeRunSet(rs)
# Run QuadraticCons and gather results
# Build .gp to plot scaled cq's
# Create master html


class QuadraticAnalysis:
  ''
  def __init__(self):
    return
    
  def createPNGSummary(summary):
    'Assumes summary.dat already exists'
    # summary is a list of tuples.  The third element of the tuple is the
    # run name
    summaryPlot = open("summary.gp", 'w')
    summaryPlot.write('reset\n')
    summaryPlot.write('set size .8, .8\n')
    summaryPlot.write('set origin .1, .1\n')
    summaryPlot.write('set term png\n')
    summaryPlot.write('set output \'summary_base.png\'\n')
    summaryPlot.write('set logscale xy\n')
    summaryPlot.write('set xlabel \"CQRatio\"\n')
    summaryPlot.write('set ylabel \"abs(Ratio - 1)\"\n')
    summaryPlot.write('plot \'summary.dat\' using 2:1\n')
    summaryPlot.write('set output\n')
    summaryPlot.close()
    os.system('gnuplot summary.gp')

class AnalyzeRunSet:
    def __init__(self, rVector):
        self.__rVector = rVector
        tsArray = []
        tsCountArray = []
        QCTimeStepArray = []
        NormDEArray = []
        NormDEKEArray = []
        sdRelDepartArray = []
        RatioDiffDepartArray = []
        epsilon = 1E-10
        print(rVector)
        QCString = "MDQuadraticCons "

        rVector.sort(lambda a,b: cmp(float(a['OuterTimeStepInPicoSeconds']), float(b['OuterTimeStepInPicoSeconds'])))

        for run in rVector:
            tsArray.append(float(run['OuterTimeStepInPicoSeconds']))
            delaystats = 0
            if run.has_key('DelayStats'):
                delaystats = int(float(run['DelayStats']))
            tsCountArray.append(int(float(run['Step'])) - delaystats)
            QCString += "../%s/Run.rdg "%run['DirName']

        f = os.popen(QCString)
        QCLines = f.readlines()
        f.close()
        print(QCLines)
        for l in QCLines[1:]:
          s = l.strip().split()
          QCTimeStepArray.append(float(s[0]))
          NormDEArray.append(float(s[4]))
          NormDEKEArray.append(float(s[6]))
          sdRelDepartArray.append(float(s[8]))
          RatioDiffDepartArray.append(float(s[9]))

        self.__ts = array(tsArray, Float)
        minStep = min(tsArray)
        print("minStep:", minStep)
        self.__tsCount = array(tsCountArray, Float)

        self.__htmlFile = open(self.__rVector[0]['RunSet'] + '.html', 'w')
        self.__htmlFile.write('<HTML>\n<HEAD>\n')
        self.__htmlFile.write('<TITLE>')
        self.__htmlFile.write('Results for ' + self.__rVector[0]['RunSet'] + '</TITLE>\n')
        self.__htmlFile.write('</HEAD>\n<BODY>\n')
        self.__htmlFile.write('<H1>Results for ' + self.__rVector[0]['RunSet'] +'</H1>\n<br><br>\n')
        self.__htmlFile.write('<table border=2>\n')
        self.__htmlFile.write('<caption><H2>Run Results</H2></caption>\n')
        self.__htmlFile.write('<tr><th align=left>Directory Name</th>')
        self.__htmlFile.write('<th align=left>Time Step (ps)</th>')
        self.__htmlFile.write('<th align=left>Statistics Time (ps)</th>')
        self.__htmlFile.write('<th align=left>NormDE</th>')
        self.__htmlFile.write('<th align=left>NormDE/KE</th>')
        self.__htmlFile.write('<th align=left>sdRelDeparture</th>')
        self.__htmlFile.write('<th align=left>Ratio-1</th></tr>\n')
        totalTime = self.__ts*self.__tsCount
        for i in range(len(rVector)):
            self.__htmlFile.write('<tr>')
            self.__htmlFile.write('<td>')
            self.__htmlFile.write(self.__rVector[i]['DirName'])
            self.__htmlFile.write('</td>')
            self.__htmlFile.write('<td>')
            self.__htmlFile.write(str(self.__ts[i]))
            self.__htmlFile.write('</td>')
            self.__htmlFile.write('<td>')
            self.__htmlFile.write(str(totalTime[i]))
            self.__htmlFile.write('</td>')
            self.__htmlFile.write('<td>')
            self.__htmlFile.write('%g'%NormDEArray[i])
            self.__htmlFile.write('</td>')
            self.__htmlFile.write('<td>')
            self.__htmlFile.write('%g' % NormDEKEArray[i])
            self.__htmlFile.write('</td>')
            self.__htmlFile.write('<td>')
            self.__htmlFile.write('%g' % sdRelDepartArray[i])
            self.__htmlFile.write('</td>')
            self.__htmlFile.write('<td>')
            self.__htmlFile.write('%g' % abs(RatioDiffDepartArray[i]))
            self.__htmlFile.write('</td>')
            self.__htmlFile.write('<td>')
            self.__htmlFile.write('</td>')
            self.__htmlFile.write('</tr>\n')
        self.__htmlFile.write('</table>\n<br><br>\n')
        self.__htmlFile.write('<table border=2>\n')
        self.__htmlFile.write('<caption><H2>RunSet Parameter Values</H2></caption>\n')
        self.__htmlFile.write('<tr><th align=center>Parameter</th><th align=center>Value</th></tr>\n')
	params = self.__rVector[0].params()
        for (param, value) in params:
            self.__htmlFile.write('<tr>')
            self.__htmlFile.write('<td>')
            self.__htmlFile.write(param)
            self.__htmlFile.write('</td>')
            self.__htmlFile.write('<td>')
            self.__htmlFile.write(value)
            self.__htmlFile.write('</td>')
            self.__htmlFile.write('</tr>\n')
        self.__htmlFile.write('</table>\n<br><br>\n')
        
        
    def tsCount(self):
        return(self.__tsCount)
    def finalize(self):
        self.__htmlFile.write('</BODY>\n</HTML>\n')
        self.__htmlFile.close()
    def ratios(self):
	"""
        ts_rmsd = []
        #for i in range(len(self.__ts)):
        #    ts_rmsd.append((self.__ts[i], self.__cqrmsd[i]))
        ts_rmsd.sort()
        print "ts_rmsd", ts_rmsd
        r0 = ts_rmsd[1][1]/ts_rmsd[0][1]
        t0 = ts_rmsd[1][0]/ts_rmsd[0][0]
        r1 = ts_rmsd[2][1]/ts_rmsd[1][1]
        t1 = ts_rmsd[2][0]/ts_rmsd[1][0]
        self.__htmlFile.write('<br><br>\n')
        self.__htmlFile.write('r0/t0^2 = ' + str(r0/(t0*t0)) + '\n')
        self.__htmlFile.write('<br>\n')
        self.__htmlFile.write('r1/t1^2 = ' + str(r1/(t1*t1)) + '\n')
        self.__htmlFile.write('<br>\n')
        ratio = (r0, t0*t0, r1, t1*t1)
        print "ratio",ratio
        return(ratio)
	"""

class QuadraticConservation:
    def __init__(self, resultsDir = './analysis'):
        self.__results = {}
        self.__ratioArray = []

        # create directory to hold all of the analysis stuff
        print("mkdir", resultsDir)
        os.mkdir(resultsDir)
        print("chdir", resultsDir)
        os.chdir(resultsDir)
        self.__mainFile = open("index.html", 'w')
        self.mainHtmlSetup()

    def mainHtmlSetup(self):
        self.__mainFile.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">\n')
        self.__mainFile.write('<HTML>\n<HEAD>\n')
        titleString = 'Results from ' + time.asctime(time.localtime(time.time()))
        self.__mainFile.write('<TITLE>' + titleString + '</TITLE>\n</HEAD>\n<BODY>\n')
        self.__mainFile.write('<H1>' + titleString + '</H1>\n<br>\n')
        self.__mainFile.write('<EMBED SRC="summary.png" NAME="pngsummary" HEIGHT="480" WIDTH="600">\n<br>\n')
    def mainHtmlClose(self):
        self.__mainFile.write('<TABLE>\n')
        self.__mainFile.write('<CAPTION><h2>Chi2 Values for RunSets</h2></CAPTION>\n')
        self.__mainFile.write('<TR><TH ALIGN=left>RunSet</TH><TH ALIGN=center>Chi2</TH></TR>\n')
        # create summary data array
        summary = []
        summaryFile = open("summary.dat", 'w')
        summaryFile.write('# col 1: abs(ratio - 1)\n')
        summaryFile.write('# col 2: chi^2\n')
        summaryFile.write('# col 3: RunSetID\n')
        for i in range(len(self.__chi2array)):
            ratioDev = max(abs(self.__ratioArray[i][0][0]/self.__ratioArray[i][0][1] - 1), abs(self.__ratioArray[i][0][2]/self.__ratioArray[i][0][3] - 1))
            newElt = (ratioDev, self.__chi2array[i][0], self.__chi2array[i][1])
            summary.append(newElt)
            summaryFile.write(str(ratioDev) + ' ')
            summaryFile.write(str(self.__chi2array[i][0]) + ' ')
            summaryFile.write(str(self.__chi2array[i][1]) + '\n')
        summaryFile.close()
        createPNGSummary(summary)
        self.__chi2array.sort()
        for i in self.__chi2array:
            self.__mainFile.write('<TR><TD ALIGN=left>')
            self.__mainFile.write('<A HREF=')
            self.__mainFile.write('\"' + str(i[1]) + '.html\">')
            self.__mainFile.write(str(i[1]))
            self.__mainFile.write('</A>')
            self.__mainFile.write('</TD><TD ALIGN=left>')
            self.__mainFile.write(str(i[0]))
            self.__mainFile.write('</TD></TR>\n')
        self.__mainFile.write('</TABLE>\n<br><br>\n')
        ratio = []
        for i in range(len(self.__ratioArray)):
            ratio.append((self.__ratioArray[i][0][0]/self.__ratioArray[i][0][1], self.__ratioArray[i][0][2]/self.__ratioArray[i][0][3], self.__ratioArray[i][1]))
        self.__mainFile.write('<TABLE>\n')
        self.__mainFile.write('<CAPTION><h2>RMSD Values for RunSets</h2></CAPTION>\n')
        self.__mainFile.write('<TR><TH ALIGN=left>RunSet</TH><TH ALIGN=left>Ratio 0</TH><TH ALIGN=left>Ratio 1</TH></TR>\n')
        ratio.sort()
        for i in ratio:
            self.__mainFile.write('<TR><TD ALIGN=left>')
            self.__mainFile.write('<A HREF=')
            self.__mainFile.write('\"' + str(i[2]) + '.html\">')
            self.__mainFile.write(str(i[2]))
            self.__mainFile.write('</A>')
            self.__mainFile.write('</TD>\n')
            self.__mainFile.write('<TD ALIGN=left>')
            self.__mainFile.write(str(i[0]))
            self.__mainFile.write('</TD>')
            self.__mainFile.write('<TD ALIGN=left>')
            self.__mainFile.write(str(i[1]))
            self.__mainFile.write('</TD>')
            self.__mainFile.write('</TR>\n')
        self.__mainFile.write('</TABLE>\n<br><br>\n')
        self.__mainFile.write('</BODY>\n</HTML>\n')
        self.__mainFile.close()
        
    def finalize(self):
        self.mainHtmlClose()

    def runSet(self, rVector):
        rsAnalysis = AnalyzeRunSet(rVector)
        ratios = rsAnalysis.ratios()
        rsAnalysis.finalize()
        self.__ratioArray.append((ratios, rVector[0]['RunSet']))

