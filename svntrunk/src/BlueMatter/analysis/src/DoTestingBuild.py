#!/usr/bin/env python

# #################################################################
#
# File: DoTestingBuild.py
#
# Description: Script to be used in building test systems from one of:
#              base package (charmm | amber | oplsaa) or xml or db2.
#              Default behavior is to:

#              (1) attempt to find the fully # qualified system name
#                  in the db2 table experiment.test_sys.
#              (2) find the appropriate xml file within the current
#                  install image based on the fully qualified system
#                  name.
#              (3) construct the system from the original force field
#                  assignment data (created by the appropriate package)
#
#
# Usage: DoTestingBuild.py [options] sysName [rtpName=Run.rtp]
#
# #################################################################

from __future__ import print_function
from optik import OptionParser
import sys
import BuildUtils

def main():
    usage = "usage: %prog [options] sysName [ctp_id | rtpFileName=Run.rtp] [buildMode] [buildOpts]"
    dataSource = {"db2":0,"xml":1,"ff":2}
    
    parser = OptionParser(usage)
    rtpName = 'Run.rtp'

    # sys_class option:
    parser.add_option("-c", "--sys_class", type="string", action="store",
                      dest="sys_class", default="testing",
                      help="(scratch | [testing] | production)")
    # source options:
    parser.add_option("-d", "--from-database", action="store_const",
                      dest="source", const=dataSource["db2"],
                      default=dataSource["db2"],
                      help="look for system in database")
    
    parser.add_option("-x", "--from-xml", action="store_const",
                      dest="source", const=dataSource["xml"],
                      help="look for system in xml file")
    
    parser.add_option("-p", "--from-package", action="store_const",
                      dest="source", const=dataSource["ff"],
                      help="look for system in force field files")

    # force field identifier option (only used if source=package)
    parser.add_option("-f", "--forcefield", action="store",
                      dest="forceField", default="unknown",
                      help="define force field of system")

    # system id option (only used if source=db2)
    parser.add_option("-i", "--system-id", action="store",
                      dest="sysId", default="-1",
                      help="identify system in db2")

    # verbose/quiet options
    parser.add_option("-v", "--verbose", action="store_true",
                      dest="verbose", default="store_false",
                      help="sets output to be verbose")

    parser.add_option("-q", "--quiet", action="store_false",
                      dest="verbose", help="sets output to be quiet")
    
    
    (options, args) = parser.parse_args()

    buildMode="Opt"
    buildOpts=''
    if len(args) < 1:
        parser.error("incorrect number of args")
    sysName = args[0]
    if len(args) == 2:
        rtpName = args[1]
    if len(args) == 3:
        buildMode=args[2]
    if len(args) == 4:
        buildOpts=args[3]
    if options.verbose:
        print("sys_class:", options.sys_class)
        print("source:", options.source)
        print("rtp file:", rtpName)
        print("force field:", options.forceField)
    # set xml creation object based on force field setting
    creator = None
    if options.forceField=='charmm':
        creator = BuildUtils.createXmlForCharmm()
    elif options.forceField=='oplsaa':
        creator = BuildUtils.createXmlForOplsaa()
    else:
        create = BuildUtils.createXml()
    sysId = options.sysId
    ctpId = -1
    if options.source == dataSource["ff"]:
        if options.verbose:
            print("calling creator.fromSource()")
        creator.fromSource(sysName, None, options.verbose)
    if options.source == dataSource["xml"] or \
           options.source == dataSource["ff"]:
        if options.verbose:
            print("calling BuildUtils.xml2db2(), BuildUtils.rtp2db2()")
        sysId=BuildUtils.xml2db2(sysName + '.' + options.forceField, \
                                 options.verbose)
        if options.verbose:
            print("sysId =", str(sysId))
    if rtpName.isdigit():
        ctpId=rtpName
    else:
        ctpId=BuildUtils.rtp2db2(rtpName, options.verbose)
    if options.source == dataSource["xml"] or \
           options.source == dataSource["ff"] or \
           options.source == dataSource["db2"]:
        if options.verbose:
            print("calling BuildUtils.db2cpp(), BuildUtils.cpp2exe()")
        BuildUtils.db2cpp(str(sysName) + '.' + str(options.forceField), sysId, ctpId, options.verbose)
        BuildUtils.cpp2exe(str(sysName) + '.' + str(options.forceField), buildMode, buildOpts, options.verbose)
  
    return(0)


main()
