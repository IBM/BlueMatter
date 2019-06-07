from __future__ import print_function
# #################################################################
#
# File: BuildUtils.py
#
# Description: Collection of python utilities (classes/functions) to
#              encapsulate operations associated with building systems
#              for BlueMatter
#
# #################################################################

import bgutils
import commands
import re
import os

# base class defining interface to create xml file for system specified
# by a system name
class createXml:
    def __init__(self):
        self.installRoot = bgutils.installbase() + 'usr/opt/bluegene'
    def fromSource(self, sysName, dataDir=None, verbose=0): # derived classes must overload this
        raise RuntimeError('createXML.from() called from base class')

# derived class implementing from() method for CHARMM
class createXmlForCharmm(createXml):
    def __init__(self):
        createXml.__init__(self)
    def fromSource(self, sysName, dataDir=None, verbose=0):
        toppardir=self.installRoot + '/forcefields/charmm22'
        if dataDir == None:
            datadir=self.installRoot + '/mdtest/data/charmm22'
        topfile="top_all22_prot_nacl.inp"
        parfile="par_all22_prot_nacl.inp"
        rc = commands.getstatusoutput('bunzip2 -kc ' + datadir +
                                      '/' + sysName +'.psf.bz2 > ./'
                                      + sysName + '.psf')
        if rc[0] != 0:
            raise RuntimeError('createXML.fromCharmm: ' + str(rc[0]) + '\n' + rc[1])
        
        
        rc = commands.getstatusoutput('psf2xml.pl ' + toppardir + '/' +
                                      topfile + ' ' + toppardir + '/' +
                                      parfile + ' ./' + sysName + '.psf')
        if rc[0] != 0:
            raise RuntimeError('createXML.fromCharmm: ' + str(rc[0]) + '\n' + rc[1])

#derived class implementing from() method for OPLSAA
class createXmlForOplsaa(createXml):
    def __init__(self):
        createXml.__init__(self)
    def fromSource(self, sysName, dataDir=None, verbose=0):
        if dataDir==None:
            datadir=self.installRoot + '/mdtest/data/oplsaa'
        rc = commands.getstatusoutput('bunzip2 -kc ' + datadir + '/'
                                      + sysName + '.out.bz2 > ./'
                                      + sysName + '.out')
        if rc[0] != 0:
            raise RuntimeError('createXML.fromOplsas: ' + str(rc[0]) + '\n' + rc[1])

        rc = commands.getstatusoutput('bunzip2 -kc ' + datadir + '/'
                                      + sysName + '.pdb.bz2 > ./'
                                      + sysName + '.pdb')
        if rc[0] != 0:
            raise RuntimeError('createXML.fromOplsas: ' + str(rc[0]) + '\n' + rc[1])
        
        rc = commands.getstatusoutput('opls2xml.pl ./' + sysName + '.out'
                                      + ' ./' + sysName + '.pdb')
        if rc[0] != 0:
            raise RuntimeError('createXML.fromOplsas: ' + str(rc[0]) + '\n' + rc[1])


        
def xml2db2(xmlBase, verbose=0):
    script = bgutils.installbase() + 'usr/opt/bluegene/bin/setup/xml2db2.pl'
    rc = commands.getstatusoutput("perl " + script + " " + xmlBase + ".xml")
    sysId = '-1'
    syspat = re.compile(r'^SystemId:\s+(\d+)')
    successpat = re.compile(r'SUCCESS:')
    errorpat = re.compile(r'ERROR:')
    xmlLines = rc[1].split('\n')
    for xmlLine in xmlLines:
        if verbose:
            print("xml2db2:",xmlLine)
        ma = re.match(syspat, xmlLine)
        mb = re.match(successpat, xmlLine)
        mc = re.match(errorpat, xmlLine)
        if ma != None:
            sysId = ma.expand(r'\1')
        elif mb != None:
            if verbose:
                print(xmlLine)
        elif mc != None:
            raise RuntimeError('xml2db2: Error loading xml into db2. Please check the XML.\n' + xmlLine)
                
    if rc[0] != 0:
        raise RuntimeError('xml2db2 error')
    return sysId

def rtp2db2(rtpName, verbose=0):
    rtpCmd =  'java com.ibm.bluematter.utils.Rtp2db2 ' + rtpName
    if verbose:
        print(rtpCmd)
    ctpId = '-1'
    ctppat = re.compile(r'^CTP_ID:\s+(\d+)')
    rc = commands.getstatusoutput(rtpCmd)
    rtpLines = rc[1].split('\n')
    for rtpLine in rtpLines:
        ma = re.match(ctppat, rtpLine)
        if ma != None:
            ctpId = ma.expand(r'\1')
    if ctpId == -1:
        raise RuntimeError('rtp2db2 did not return a ctp_id')
    if rc[0] != 0:
        raise RuntimeError('rtp2db2 non-zero return code')
    return ctpId
    
def db2cpp(dest, sysId, ctpId, verbose=0):
    os.system('rm -f ' + dest + '.cpp')
    cmd = 'java com.ibm.bluematter.db2probspec.ProbspecgenDB2 ' \
          + str(sysId) + ' ' + str(ctpId) + ' ' + dest + ' -v'
    if verbose:
        print(cmd)
    rc = commands.getstatusoutput(cmd)
    if verbose:
        print("db2cpp:\n", rc[1])
    if rc[0] != 0:
        raise RuntimeError('db2cpp non-zero return code')
    if os.access(dest + '.cpp', os.R_OK) == 0:
        raise RuntimeError('db2cpp did not generate the cpp')

def cpp2exe(dest, buildmode, buildopts, verbose=0):
    exeFile = dest + '.smp.aix.exe'
    os.system('rm -f ' + exeFile)
    script = str(bgutils.installbase()) + \
             'usr/opt/bluegene/bin/setup/compile_msd.sh '
    cmd = 'time ' + script + dest + ' ' + buildmode + ' ' + buildopts
    rc = commands.getstatusoutput(cmd)
    if rc[0] != 0:
        raise RuntimeError('cpp2exe non-zero return code')
    if os.access(exeFile, os.R_OK) == 0:
        raise RuntimeError('cpp2exe did not generate the exe')
    if verbose:
        print(exeFile, "successfully created by cpp2exe")
