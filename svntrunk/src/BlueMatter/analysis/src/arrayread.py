# reads a file in the format of Run.mdlog and provides access to the
# columns as NumPy arrays

from Numeric import *
import re

class FileArray:
    def __init__(self, fname):
	self.__cols = {}
	self.__data = []
	self.__lineCount = -1
	inFile = open(fname, 'r')
	for i in inFile.xreadlines():
	    self.__lineCount = self.__lineCount+1
	inFile.seek(0,0)
	head = inFile.readline()
	head=head.lstrip()
	head=head.rstrip()
	ws = re.compile(r'\s+')
	cols = ws.split(head)
	for i in range(len(cols)):
            if self.__cols.has_key(cols[i]): # deal with duplicate column names
                self.__cols[str(cols[i])+'_'+str(i)]=i
            else:
                self.__cols[cols[i]]=i
	self.__data = range(len(self.__cols))
	for i in self.__cols.keys():
	    self.__data[self.__cols[i]] = array(range(self.__lineCount), Float)
	inFile.seek(0,0)
	index = -1
	for i in inFile.xreadlines():
	    if index < 0:
		index=index+1
		continue
	    i=i.lstrip()
	    i=i.rstrip()
	    data = ws.split(i)
	    for j in range(len(data)):
		self.__data[j][index] = float(data[j])
	    index=index+1

    def data(self, colName):
	return self.__data[self.__cols[colName]]
    def cols(self):
	return self.__cols
    def lineCount(self):
	return self.__lineCount


def histogram(data, width):
    newLen = int(len(data)/width)
#    if ((len(data) % width) != 0):
#	 newLen = newLen+1
    newData = array(range(newLen), Float)
    lw = 0
    norm = 1.0/width
    for i in range(len(newData)):
	rw = lw + width
	newData[i] = sum(data[lw:rw])*norm
	lw = rw
    return newData

def variance(data, length = -1):
    if length == -1:
        length = len(data)
    newData = data[0:length]
    avg = sum(newData)/float(length)
    var = sum((newData-avg)*(newData-avg))/float(length)
    return var
