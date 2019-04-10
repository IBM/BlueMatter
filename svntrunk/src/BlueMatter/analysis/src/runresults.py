# object to encapsulate data in RunResults.txt file (new format)

import re

class RunRecord:
    def __init__(self, lines):
	ws = re.compile(r'\s+')
        self.__data = {}
	self.__params = []
	paramlabels = lines[0].split()
	paramvalues = lines[1].split()
	for i in range(len(paramlabels)):
	    if (paramlabels[i] != 'DirName' and paramlabels[i] != 'RunSet' and paramlabels[i] != 'OuterTimeStepInPicoSeconds'
	            and paramlabels[i] != 'NumberOfOuterTimeSteps' and paramlabels[i] != 'DelayStats' and paramvalues[i] != '_'):
		self.__params.append((paramlabels[i], paramvalues[i]))
        header = ws.split(lines[0].strip()) + ws.split(lines[2].strip())
        data   = ws.split(lines[1].strip()) + ws.split(lines[3].strip())
        for i in range(len(header)):
            self.__data[header[i]] = data[i]
    def __str__(self):
        return str(self.__data)
    def __getitem__(self, index):
	return self.__data[index]
    def __len__(self):
	return len(self.__data)
    def has_key(self, key):
        return self.__data.has_key(key)
    def items(self):
        return self.__data.items()
    def values(self):
        return self.__data.values()
    def keys(self):
        return self.__data.keys()
    def params(self):
        return self.__params
        
        
class RunResults:
    def __init__(self, fname):
	self.__data = {}
	inFile = open(fname, 'r')
	self.__mastervars = inFile.readline().strip().split()
	lines = inFile.readlines()
        for i in range(0,len(lines),5):
            data = lines[i:i+6]
            record = RunRecord(data)
            # print 'record[', record['RunSet'], ']:', record
            # #######################################################
            # Only include SUCCESSFUL (RunStatus = 0) runs
            # #######################################################
            if int(record['RunStatus']) == 0:
                if self.__data.has_key(record['RunSet']):
                    self.__data[record['RunSet']].append(record)
                else:
                    self.__data[record['RunSet']] = [record]
    def __str__(self):
        return str(self.__data)
    def __getitem__(self, index):
	return self.__data[index]
    def __len__(self):
	return len(self.__data)
    def has_key(self, key):
        return self.__data.has_key(key)
    def items(self):
        return self.__data.items()
    def values(self):
        return self.__data.values()
    def keys(self):
        return self.__data.keys()

