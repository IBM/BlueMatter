# computes variance of variance (variance2) of total energies from selected run
# directory

from arrayread import *
from runlist import RunList
import sys
import os
import warnings

def var2(runDir, maxWidth):

    os.chdir(runDir)
    # open file to drive gnuplot
    gpFile = open('plotvarest.gp','w')
    gpFile.write('reset\n')
    gpFile.write('inverse(x) = 1.0/x\n')
    runList = RunList("RunList.txt")

    for i in range(len(runList)):
	if i == 0:
	    minstep = float(runList[i][runList.columnDict()['OuterTimeStepInPicoSeconds']])
	    maxstepCount = int(runList[i][runList.columnDict()['NumberOfOuterTimeSteps']])
	else:
	    minstep = min(minstep, float(runList[i][runList.columnDict()['OuterTimeStepInPicoSeconds']]))
	    maxstepCount = max(maxstepCount, int(runList[i][runList.columnDict()['NumberOfOuterTimeSteps']]))

    gpFile.write('deltat=' + str(minstep) + '\n')

    for i in range(len(runList)):
	tsMult = int((float(runList[i][runList.columnDict()['OuterTimeStepInPicoSeconds']])/minstep)+0.5)
	stepCount = int(runList[i][runList.columnDict()['NumberOfOuterTimeSteps']])
	if (stepCount*tsMult != maxstepCount):
	    warnings.warning("inconsistent stepCount for " + runList.data("DirName")[i] + " in " +runDir)
	mdlogFile = './' + runList[i][runList.columnDict()['DirName']] + '/Run.mdlog'
	mdData = FileArray(mdlogFile)
	print i, runDir + '/' + runList[i][runList.columnDict()['DirName']], tsMult, stepCount
	normvar = 1.0
	currentFile = runList[i][runList.columnDict()['DirName']] + '.varest'
	outFile = open(currentFile,'w')
	outFile.write('# column 1: window width\n')
	outFile.write('# column 2: window count\n')
	outFile.write('# column 3: variance computed using specified window\n')
	outFile.write('# column 4: rmsd computed using specified window\n')
	outFile.write('# column 5: normalized variance (normalized by width=1 variance\n')
	outFile.write('# column 6: ratio of nWindow*varEw/varE (A&T eqn. 6.16)\n')
	evec = array(mdData.data('TotalE'),Float)
	eavg = sum(evec)/float(len(evec))
	e2vec = evec - eavg
	e2vec = (e2vec*e2vec)
	for width in range(1,maxWidth/tsMult):
	    e2vecw = histogram(e2vec, width)
	    myvar = variance(e2vecw)
            normvar = variance(e2vec, width*len(e2vecw))
            normvar = 1.0/normvar
	    outFile.write(str(width))
	    outFile.write(' ')
	    outFile.write(str(len(e2vecw)))
	    outFile.write(' ')
	    outFile.write(str(myvar))
	    outFile.write(' ')
	    outFile.write(str(sqrt(myvar)))
	    outFile.write(' ')
	    outFile.write(str(myvar*normvar))
	    outFile.write(' ')
            outFile.write(str(float(width)*myvar*normvar))
	    outFile.write('\n')
	outFile.close()
	if i == 0:
	    gpFile.write('plot ')
	else:
	    gpFile.write(',\\\n')
	gpFile.write('\'' + currentFile + '\' using (inverse($1)):(')
#	gpFile.write('\'' + currentFile + '\' using (inverse(' + str(tsMult) + '*$1)):(')
#	gpFile.write(str(tsMult) + '*$1*$4) with points')
	gpFile.write('$6) with points')
    gpFile.write('\n')
    gpFile.close()


