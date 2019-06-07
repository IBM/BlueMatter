#! /usr/bin/env python

# ###################################################################
#
# This utility builds an executable using a source file, compiler and
# build flags passed to it as command line arguments.  It also will
# connect with db2 to get an executable id and record the parameters
# of the build.
#
# build2db2 sourceFile buildOpts <targetPlatform=0 (aix/rs6k)> <install_image_path = $SHIPDATA_INCLUDE> <compiler=xlC_r> <executable extension=smp.aix.exe>
#
# ###################################################################

from __future__ import print_function
from DB2 import *
import mx.DateTime
import time
import sys
import bgutils
import re
import os.path
import pwd

if len(sys.argv) < 3:
    print(sys.argv[0], "sourceFile buildOpts <targetPlatform = 0 (rs/6000 aix)> <install_image_path=$PACKAGEBASE/shipdata/usr/opt/bluegene>  <compiler=xlC_r> <executable_extension=smp.aix.exe>")
    sys.exit(-1)

sourceFile = sys.argv[1]
buildOpts = sys.argv[2]
targetPlatform = 0
installImagePath = bgutils.installbase() + 'usr/opt/bluegene'

compilerFamily = "xlC_r"
exeExtension = "smp.aix.exe"
objExtension = "o"

print("buildOpts:\n", buildOpts)

if (len(sys.argv) > 3):
    targetPlatform = sys.argv[3]
if (len(sys.argv) > 4):
    installImagePath = sys.argv[4]
if (len(sys.argv) > 5):
    compilerFamily = sys.argv[5]
if (len(sys.argv) > 6):
    exeExtension = sys.argv[6]

rootfilepat = re.compile(r'(.+)\.cpp$')
fmatch = re.match(rootfilepat, sourceFile)
if fmatch == None:
    print("Error: Invalid file name", sourceFile)
    sys.exit(-1)

rootFileName = fmatch.expand(r'\1')
print("rootFileName =", rootFileName)

# database name
dbpat = re.compile(r'^//%database_name:\s*(\w+)')
sourcepat = re.compile(r'^//%source_id:\s*(\d+)')
syspat = re.compile(r'^//%system_id:\s*(\d+)')
ctppat = re.compile(r'^//%ctp_id:\s*(\d+)')
psversionpat = re.compile(r'^//%probspec_version:\s*([\w\.]+)')
pstagpat = re.compile(r'^//%probspec_tag:([\w ]+)')
magicnopat = re.compile(r'^//%magic_number:([\w]+)')

inFile = open(sourceFile)

matchTotal = 7
matchCount = 0
for line in inFile.xreadlines():
    ma = re.match(dbpat, line)
    mb = re.match(sourcepat, line)
    mc = re.match(syspat, line)
    md = re.match(ctppat, line)
    me = re.match(psversionpat, line)
    mf = re.match(pstagpat, line)
    mg = re.match(magicnopat, line)

    if  ma != None:
        ++matchCount
        dbName = ma.expand(r'\1')
        print("database name =", ma.expand(r'\1'))
    elif mb != None:
        ++matchCount
        sourceId = mb.expand(r'\1')
        print("source_id =", mb.expand(r'\1'))
    elif mc != None:
        ++matchCount
        sysId = mc.expand(r'\1')
        print("sys_id =", mc.expand(r'\1'))
    elif md != None:
        ++matchCount
        ctpId = md.expand(r'\1')
        print("ctp_id =", md.expand(r'\1'))
    elif me != None:
        ++matchCount
        probspecVersion = me.expand(r'\1')
        print("probspec_version =", me.expand(r'\1'))
    elif mf != None:
        ++matchCount
        probspecTag = mf.expand(r'\1')
        print("probspec_tag =", mf.expand(r'\1'))
    elif mg != None:
        ++matchCount
        magicNumber = mg.expand(r'\1')
        print("magic number =", mg.expand(r'\1'))
    if (matchCount >= matchTotal):
        break
inFile.close()

conn = Connection(dbName)
cursor = conn.cursor()

# get the probspec version information based on the probspecTag
cmd = "select probspec_version from experiment.probspec where tag = \'" \
      + probspecTag + "\'"
cursor.execute(cmd)
foo = cursor.fetchone()
if foo:
    probspecVersion = foo[0]
else:
    print("ERROR: Unable to find probspec version corresponding to tag", probspecTag)
    sys.exit(-2)


# fetch and print platform information based on target platform id
cmd = "select platform_id, hardware, os_family, os_version from experiment.platform where platform_id = " + str(targetPlatform)
print(cmd)
cursor.execute(cmd)
res = cursor.fetchone()
print("Platform:", res[0], res[1], res[2], res[3])

# we're going to check the version files under both BlueMatter and bgfe and
# verify that they contain the same version (if any).
blueMatterVersion = None
bgfeVersion = None
versionpat = re.compile(r'^\$Name:\s*(\w+)')
inFile = open(installImagePath + "/BlueMatter.version")
for line in inFile.xreadlines():
    mVer = re.match(versionpat, line)
    if mVer != None:
        blueMatterVersion = mVer.expand(r'\1')
        print("blueMatterVersion =", blueMatterVersion)
        break;
inFile.close()

inFile = open(installImagePath + "/bgfe.version")
for line in inFile.xreadlines():
    mVer = re.match(versionpat, line)
    if mVer != None:
        bgfeVersion = mVer.expand(r'\1')
        print("bgfeVersion =", bgfeVersion)
        break;
inFile.close()

tagVersion = bgfeVersion
if bgfeVersion == None or blueMatterVersion == None or bgfeVersion != blueMatterVersion:
    tagVersion = None

if tagVersion == None:
    print("tagVersion: NONE")
else:
    print("tagVersion:", tagVersion)

# now get the relevant information about compiler levels and such.
# The following is AIX-specific:

compilerInfo = os.popen('lslpp -L -c vacpp.cmp.C vacpp.cmp.batch vacpp.cmp.core vacpp.cmp.include memdbg.adt bos.adt.include', 'r')

compilerVersion = ''
for line in compilerInfo.xreadlines():
    compilerVersion = compilerVersion + line
print(compilerVersion, end=' ')

cmd = "select count(*) from experiment.compiler where family = " + "\'" + str(compilerFamily) + "\' and version = \'" + str(compilerVersion) + "\'"
print(cmd)
cursor.execute(cmd)

foo = cursor.fetchone();
print(foo[0],"rows returned")

if foo[0] > 1:
    print("WARNING: multiple entries in compiler table for " + compilerFamily)
    print("version\n" + compilerVersion)
elif int(foo[0]) == 0:
    cmd = "insert into experiment.compiler (compiler_id, family, version) values (DEFAULT, " \
    + "\'" + str(compilerFamily) + "\', " \
    + "\'" + str(compilerVersion) + "\')"
    print(cmd)
    cursor.execute(cmd)
cmd = "select compiler_id from experiment.compiler where family = " \
+ "\'" + str(compilerFamily) + "\' and version = \'" + str(compilerVersion) + "\'"
print(cmd)
cursor.execute(cmd)
foo = cursor.fetchone()
if foo:
    compilerId = foo[0]
    print("compilerId:", compilerId)
else:
    print("ERROR: Could not fetch compiler_id from db2")
    sys.exit(-2)


# machine identifier
compileHost = os.uname()[4];

print(compileHost)

# Because we are having db2 generate the executable_id which we must retrieve
# prior to the build, we need to have a way to uniquely identify the row that
# we are adding.  Since we will be updateing the build_begin and build_end
# columns after the build completes we can use them to ensure that the row we
# add can be specified uniquely without knowing the executable_id.  We'll do
# this by setting build_begin to
# mx.DateTime.TimestampFromTicks(time.time())
# and setting build_end to
# mx.DateTime.TimestampFromTicks(os.getpid())

buildBegin = mx.DateTime.TimestampFromTicks(time.time())
buildEnd = mx.DateTime.TimestampFromTicks(os.getpid())
builder = pwd.getpwuid(os.getuid())[0]

# initial insertion
cmd = "insert into experiment.executable (executable_id, build_begin, build_end, build_machine, build_opts, builder, compiler_id, install_image_path, install_image_tag, platform_id, probspec_version, source_id) " \
+ "values (DEFAULT, " \
+ "\'" + str(buildBegin) + "\', " \
+ "\'" + str(buildEnd) + "\', " \
+ "\'" + str(compileHost) + "\', " \
+ "\'" + str(buildOpts) + "\', " \
+ "\'" + str(pwd.getpwuid(os.getuid())[0]) + "\', " \
+ str(compilerId) + ", " \
+ "\'" + str(installImagePath) + "\', " \
+ "\'" + str(tagVersion) + "\', " \
+ str(targetPlatform) + ", " \
+ str(probspecVersion) + ", " \
+ str(sourceId) + ")"

print(cmd)
cursor.execute(cmd)

cmd = \
    "select executable_id from experiment.executable where " +\
    "build_begin = \'" + str(buildBegin) + "\' and " +\
    "build_end = \'" + str(buildEnd) + "\' and " +\
    "build_machine = \'" + str(compileHost) + "\' and " +\
    "build_opts = \'" + str(buildOpts) + "\' and " +\
    "builder = \'" + str(pwd.getpwuid(os.getuid())[0]) + "\' and " +\
    "compiler_id = " + str(compilerId) + " and " +\
    "install_image_path = \'" + str(installImagePath) + "\' and " +\
    "install_image_tag = \'" + str(tagVersion) + "\' and " +\
    "platform_id = " + str(targetPlatform) + " and " +\
    "probspec_version = " + str(probspecVersion) + " and " +\
    "source_id = " + str(sourceId)

print(cmd)
cursor.execute(cmd)
foo = cursor.fetchone()

executableId = -1

if foo:
    executableId = foo[0]
else:
    print("ERROR: Could not retrieve executable id from table")
    sys.exit(-2)

print("executableId =", executableId)

allOpts = buildOpts + " -DEXECUTABLEID=" + str(executableId)
##buildCmd = str(compilerFamily) + " " + str(allOpts) + " " +\
##           str(sourceFile) + " -o " + str(rootFileName) +\
##           "." + str(exeExtension)
#buildCmd = str(compilerFamily) + " " + str(allOpts) + " -c " +\
#           str(sourceFile) + ";" +\
#           str(compilerFamily) + " " + str(allOpts) +\
#           " -bloadmap:" + str(rootFileName) + ".map" +\
#           " -o " + str(rootFileName) +\
#           "." + str(exeExtension) +\
#           " " + str(rootFileName) + "." + str(objExtension)
# Build with profiling
buildCmd = \
           str(compilerFamily) + " " + str(allOpts) + " -P " +\
           str(sourceFile) + "&&" +\
           str(compilerFamily) + " -+ " + str(allOpts) + " -qlinedebug -c " +\
           str(rootFileName) + ".i &&" +\
           str(compilerFamily) + " -pg " + str(allOpts) +\
           " -bloadmap:" + str(rootFileName) + ".map" +\
           " -o " + str(rootFileName) +\
           "." + str(exeExtension) +\
           " " + str(rootFileName) + "." + str(objExtension)
print(buildCmd)
buildBegin = mx.DateTime.TimestampFromTicks(time.time())
ret = os.system(buildCmd)
buildEnd = mx.DateTime.TimestampFromTicks(time.time())

if (ret == 0):
    cmd = "update experiment.executable set (build_begin, build_end) = (\'" +\
          str(buildBegin) + "\', \'" + str(buildEnd) +\
          "\') where executable_id=" + str(executableId)

    print(cmd)
    cursor.execute(cmd)
else:
    print("ERROR: Compile failed with return code =", str(ret))
    sys.exit(-4)

conn.close()
