# computes variance of variance (variance2) of total energies from selected run
# directory

from arrayread import *
from runlist import RunList
import sys
import os
import warnings

def var3(runDir, corrTime):

    os.chdir(runDir)
    # open file to drive gnuplot
    runList = RunList("RunList.txt")
    tsArray = []
    eavgArray = []
    ermsdArray = []
    ermsdrmsdArray = []
    for i in range(len(runList)):
	if i == 0:
	    minstep = float(runList[i][runList.columnDict()['OuterTimeStepInPicoSeconds']])
	    maxstepCount = int(runList[i][runList.columnDict()['NumberOfOuterTimeSteps']])
	else:
	    minstep = min(minstep, float(runList[i][runList.columnDict()['OuterTimeStepInPicoSeconds']]))
	    maxstepCount = max(maxstepCount, int(runList[i][runList.columnDict()['NumberOfOuterTimeSteps']]))

    outFile = open('run.var2','w')
    outFile.write('# column 1: time step size (picoseconds)\n')
    outFile.write('# column 2: average energy\n')
    outFile.write('# column 3: energy variance (average((E-Eavg)**2))\n')
    outFile.write('# column 4: variance of energy variance (var((E-Eavg)**2)) \n')
    outFile.write('# column 5: rmsd of energy (sqrt(energy variance)) \n')
    outFile.write('# column 6: rmsd of energy rmsd (computed by propagation of errors) \n')
    outFile.write('# column 7: uncertainty in energy rmsd (assuming independence of measurements) \n')

    for i in range(len(runList)):
	timeStepSize = float(runList[i][runList.columnDict()['OuterTimeStepInPicoSeconds']])
	tsMult = int((float(runList[i][runList.columnDict()['OuterTimeStepInPicoSeconds']])/minstep)+0.5)
	stepCount = int(runList[i][runList.columnDict()['NumberOfOuterTimeSteps']])
	if (stepCount*tsMult != maxstepCount):
	    warnings.warning("inconsistent stepCount for " + runList.data("DirName")[i] + " in " +runDir)
	mdlogFile = './' + runList[i][runList.columnDict()['DirName']] + '/Run.mdlog'
	mdData = FileArray(mdlogFile)
	print i, runDir + '/' + runList[i][runList.columnDict()['DirName']], tsMult, stepCount
	outFile.write(str(timeStepSize))
	outFile.write(' ')
	tsArray.append(timeStepSize)
	# compute average
	evec = mdData.data('TotalE')
	avgE = sum(evec)/float(len(evec))
	outFile.write(str(avgE))
	outFile.write(' ')
	eavgArray.append(avgE)
	# compute energy variance
	evar = evec - avgE
	evar = evar*evar
	avgEvar = sum(evar)/float(len(evec))
	outFile.write(str(avgEvar))
	outFile.write(' ')
	# compute variance of variance
	evarvar = evar - avgEvar
	evarvar = evarvar*evarvar
	avgEvarvar = sum(evarvar)/float(len(evarvar))
	outFile.write(str(avgEvarvar))
	outFile.write(' ')
        rmsde = sqrt(avgEvar)
	ermsdArray.append(rmsde)
	outFile.write(str(rmsde))
	outFile.write(' ')
        errorRMSDE = 0.5*rmsde*sqrt(avgEvarvar)/avgEvar
        outFile.write(str(errorRMSDE))
        errorRMSDE=errorRMSDE*sqrt(float(corrTime)/float(tsMult))/sqrt(float(len(evar)))
	outFile.write(' ')
        outFile.write(str(errorRMSDE))
	outFile.write('\n')
	ermsdrmsdArray.append(errorRMSDE)
    outFile.close()
    return (tsArray, eavgArray, ermsdArray, ermsdrmsdArray)



