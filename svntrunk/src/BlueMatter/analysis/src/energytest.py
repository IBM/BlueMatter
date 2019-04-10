# routine to extract and fit variances from RunResults.txt
from Numeric import *
from LinearAlgebra import *
from runlist import *
from polyfit import *
from var3 import *
import types
import re
import sys
import os
import warnings

def energytest(runDir, corrTime):

    def sum(x,y):
        return x+y
    
    polydeg = 2
    mask = [0, 0, 1] # try to fit only with a + b*x*x
    rcond = 1e-10

    corrTime = int(corrTime)
    os.chdir(runDir)

    varTuple = var3('.', corrTime)
    tsArray = varTuple[0]
    eavgArray = varTuple[1]
    ermsdArray = varTuple[2]
    ermsdrmsdArray = varTuple[3]
    print "ermsdArray = ", ermsdArray
    print "ermsdrmsdArray = ", ermsdrmsdArray

    # we can use the RunList class to contain RunResults as well
    runResults = RunList('RunResults.txt')

    ts = array(tsArray, Float)
    ermsd = array(ermsdArray, Float)
    # we're going to use this as the uncertainty in the rmsd of the total energy
    # BUT to get a more accurate idea of the uncertainty, we would have to know how many
    # correlation times the data were gathered over.
    # Since all runs took place for the same amount of simulation time,
    # this factor, although unknown, is the same for all of the runs.
    # Therefore this quantity still can be used to weight the data going into the fit
    htmlFile = open('results.html', 'w')
    
    sigmaArray = ermsdrmsdArray
    rc = polyfit(ts, ermsd, sigmaArray, polydeg, mask, rcond)

    # print "parameters:\n",rc[0]
    coeff = []
    index = 0
    for i in range(polydeg + 1):
        if mask[i]:
            coeff.append(rc[0][index])
            index = index + 1
        else:
            coeff.append(0.0)

    #print "covariance matrix:\n",rc[1]

    res2 = []
    for i in range(len(ts)):
        res = ermsd[i] - polyeval(coeff,ts[i])
        res = res/sigmaArray[i]
        res2.append(res*res)

    chi2 = reduce(sum, res2)
    print "Chi Squared:\n", chi2
    htmlFile.write("Chi Squared: " + str(chi2) + "\n")
    htmlFile.write("<br><br>\n")
    print "parameters and estimated uncertainties:"
    htmlFile.write("parameters and estimated uncertainties:\n<br><br>\n")
    index = 0
    for i in range(len(coeff)):
        if mask[i]:
            print "a[",i,"] =",rc[0][index], "+-",sqrt(rc[1][index][index])
            htmlFile.write("a["+str(i)+"] = "+str(rc[0][index])+" +- ")
            htmlFile.write(str(sqrt(rc[1][index][index])))
            htmlFile.write("\n<br>\n")
            index = index+1

    htmlFile.write("<br><br>\n")
    
    vres = array(res2, Float)
    sum = add.reduce(vres)
    sum = sqrt(sum/float(len(ts) - len(rc[0])))
    print "average rmsd of fit = ", sum
    htmlFile.write("average rmsd of fit = " + str(sum) + "\n<br>\n")
    eFile = open('ermsd.dat', 'w')
    for i in range(len(ts)):
        eFile.write(str(ts[i]))
        eFile.write(' ')
        eFile.write(str(ermsd[i]))
        eFile.write(' ')
        eFile.write(str(sigmaArray[i]))
        eFile.write('\n')
    eFile.close()

    gpFile = open('plotfit.gp', 'w')
    gpFile.write('reset\n')
    gpFile.write('set term png\n')
    gpFile.write('set output \'plotfit.png\'\n')
    gpFile.write('ermsd(x) = 0 ')
    for i in range(len(coeff)):
        gpFile.write('+ ')
        gpFile.write(str(coeff[i]))
        for j in range(i):
            gpFile.write('*x')
    gpFile.write('\n')
    gpFile.write('set logscale xy\n')
    gpFile.write('set xlabel \"time step (picoseconds)\"\n')
    gpFile.write('set ylabel \"Ermsd\"\n')
    gpFile.write('set label \"')
    gpFile.write('a2 = ' + str(coeff[2]))
    gpFile.write('\" at graph .1, .8\n')
    gpFile.write('set label \"')
    gpFile.write('Chi^2 = ' + str(chi2))
    gpFile.write('\" at graph .1, .7\n')
    gpFile.write('plot \"ermsd.dat\" using 1:2:3 with yerrorbars, ermsd(x) with lines\n')
    gpFile.write('set output\n')
    gpFile.close()

    htmlFile.write("<img src=\"plotfit.png\" alt=\"plot of data + fit\">\n")
    htmlFile.close()

    os.system('gnuplot plotfit.gp')

