# object to encapsulate data in RunList.txt file

import re

class RunList:
    def __init__(self, fname):
	self.__cols = {}
	self.__data = []
	inFile = open(fname, 'r')
	lines = inFile.readlines()
	lines[0] = lines[0].lstrip()
	lines[0] = lines[0].rstrip()
	ws = re.compile(r'\s+')
	cols = ws.split(lines[0])
	for i in range(len(cols)):
	    self.__cols[cols[i]]=i
	for i in range(1,len(lines)):
	    lines[i]=lines[i].lstrip()
	    lines[i]=lines[i].rstrip()
	    row = ws.split(lines[i])
	    self.__data.append(row)
    def columnDict(self):
	return self.__cols
    def __getitem__(self, index):
	return self.__data[index]
    def __len__(self):
	return len(self.__data)

