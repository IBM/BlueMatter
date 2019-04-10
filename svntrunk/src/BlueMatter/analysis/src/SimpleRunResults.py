#!/usr/bin/env python

import sys

class SimpleRunResults:
    'Simple class that loads run results as list of independent name/value mappings'
    def __init__(self, fname):
	self.runs = []
        try:
	  inFile = open(fname, 'r')
	  lines = inFile.readlines()
          inFile.close()
        except:
          print "Error reading file " + fname
          sys.exit(1)
        for i in range(0,len(lines),5):
            data = {}
            labels = lines[i+0].split() + lines[i+2].split()
            values = lines[i+1].split() + lines[i+3].split()
            for j in range(len(labels)):
                data[labels[j]] = values[j]
            self.runs.append(data)

if __name__ == '__main__':
    if len(sys.argv) < 2:
      print sys.argv[0] + " RunResults.txt varname1 varname2 ..."
      print "Dumps labelled column of results parsed from a RunResults file"
      sys.exit(1)
    res = SimpleRunResults(sys.argv[1])
    for name in sys.argv[2:]:
      print name,
    print
    for r in res.runs:
      for name in sys.argv[2:]:
        print r[name],
      print
        