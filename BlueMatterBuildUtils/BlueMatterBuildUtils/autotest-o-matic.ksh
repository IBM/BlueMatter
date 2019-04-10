#!/usr/bin/ksh

# script to drive basic tests on the BlueMatter code base using a specified
# install image.  Tests are run in or relative to the current working directory

# arguments:
# $1 location of blue matter install image root (everything before /usr/opt/bluegene/...

installroot=/

if [[ $# = 1 ]]; then
    installroot=$1
else
    print "$0 installRootPath"
    return -1
fi

export BG_INSTALL_BASE=$installroot
echo "Using install base $BG_INSTALL_BASE"
timestamp=`date +%Y_%m_%d_%H_%M`

# ###########################################################################
#
# Environment Setup (we are assuming that we are not inside ODE)
#
# ###########################################################################

umask 022

# construct string with operating system name and major version
# e.g. aix4
export SYS_NAME=`/usr/bin/uname -s | /usr/bin/tr [A-Z] [a-z]`
#export SYS_VERSION=`/usr/bin/uname -v | /usr/bin/tr [A-Z] [a-z]`
export SYS_VERSION=5
export SYS_PLATFORM=${SYS_NAME}${SYS_VERSION}

# set PATH to include python
# export PATH=:/usr/java131/bin:/gsa/watgsa/projects/b/bluematter/local/$SYS_PLATFORM/bin:/gsa/watgsa/projects/b/bluematter/local/bin:/usr/agora/bin:/usr/local/bin:/usr/bin:/usr/lpp/pd/bin:/usr/dt/bin:/usr/bin/X11:/etc:/usr/sbin:/usr/ucb:/sbin:/usr/gnu/bin:.
export PATH=:/usr/java131/bin:/usr/agora/bin:/usr/local/bin:/usr/bin:/usr/lpp/pd/bin:/usr/dt/bin:/usr/bin/X11:/etc:/usr/sbin:/usr/ucb:/sbin:/usr/gnu/bin:.


unset LIBPATH
# ensure that signatures of ODE are not available
unset PACKAGEBASE
unset SANDBOXBASE
unset SANDBOX
unset SANDBOXDIR
unset BACKED_SANDBOXDIR

export BG_BIN_DIR=${BG_INSTALL_BASE}/usr/opt/bluegene/bin
export CLASSPATH=${BG_INSTALL_BASE}/usr/opt/bluegene/bin/setup/db2probspec.jar:${CLASSPATH}
export PATH=${BG_BIN_DIR}/utils:${BG_BIN_DIR}/setup:${BG_BIN_DIR}/simulation:${BG_BIN_DIR}/analysis:${PATH}
export PYTHONPATH=${BG_INSTALL_BASE}/usr/opt/bluegene/lib/python

# export LIBPATH=/usr/lib:/gsa/watgsa/projects/b/bluematter/local/lib:/usr/gnu/lib
export LIBPATH=/usr/lib:/usr/gnu/lib

# attempt to set db2 environment for db2 instance named db2instl
if [[ -f ~db2instl/sqllib/db2profile ]]; then
    . ~db2instl/sqllib/db2profile
else
    print "ERROR: No db2 instance named db2instl on this machine"
    return -1
fi

# ###########################################################################
#
# Create test directory and run tests
#
# ###########################################################################

#export PATH=/.../torolab.ibm.com/fs/projects/vabld/run/vacpp/dev/aix/latest/bin:$PATH

mainDir=`pwd`

testDir=test_$timestamp

mkdir $testDir

cd $testDir


#export RTPMACHINESFILE=/var/quad/test/periodic/rtp4cpus
export RTPMACHINESFILE=$mainDir/BlueMatterBuildUtils/rtp4cpus
mkdir regrun_solv_frag
cd regrun_solv_frag
echo ""
echo "regrun_solv_frag"
MultiRun.py  -dir $BG_INSTALL_BASE/usr/opt/bluegene/mdtest/regrun/solv_frag
cat PassFailSummary.out

cd ..
echo ""

#export RTPMACHINESFILE=/var/quad/test/periodic/rtp1cpus
export RTPMACHINESFILE=$mainDir/BlueMatterBuildUtils/rtp4cpus
mkdir regdev_perf_design20
cd regdev_perf_design20
echo ""
echo "regdev_perf_design20"
#MultiRun.py -dir $BG_INSTALL_BASE/usr/opt/bluegene/mdtest/regdev/perf_design20 > test.log 2>&1
cat RunTiming.txt 

cd ..
echo ""

#export RTPMACHINESFILE=/var/quad/test/periodic/rtp2cpus
export RTPMACHINESFILE=$mainDir/BlueMatterBuildUtils/rtp4cpus
mkdir valrun_multi_cons
cd valrun_multi_cons
echo ""
echo "valrun_multi_cons"
#MultiRun.py -dir $BG_INSTALL_BASE/usr/opt/bluegene/mdtest/valrun/multi_cons > test.log 2>&1
cat PassFailSummary.out

cd ..
echo ""

#export RTPMACHINESFILE=/var/quad/test/periodic/rtp4cpus
export RTPMACHINESFILE=$mainDir/BlueMatterBuildUtils/rtp4cpus

mkdir valrun_multi_restart
cd valrun_multi_restart
echo ""
echo "valrun_multi_restart"
#MultiRun.py -dir $BG_INSTALL_BASE/usr/opt/bluegene/mdtest/valrun/multi_restart > test.log 2>&1
cat DiffSummary.txt

cd ..
echo ""

#export RTPMACHINESFILE=/var/quad/test/periodic/rtp4cpus
export RTPMACHINESFILE=$mainDir/BlueMatterBuildUtils/rtp4cpus

mkdir regrun_energy
cd regrun_energy
echo ""
echo "regrun_energy"
MultiRun.py -dir $BG_INSTALL_BASE/usr/opt/bluegene/mdtest/regrun/energy > test.log 2>&1
cat PassFailSummary.out

echo ""
cd ..


mkdir charmm22_brooks
cd charmm22_brooks

echo "Using install base $BG_INSTALL_BASE"
#$BG_INSTALL_BASE/usr/opt/bluegene/mdtest/drivers/external/charmm22/brooks/DoBrookList.py > test.log 2>&1
if (( $? )); then
  echo "FAILED: charmm22 energy regression test"
  echo "See directory $PWD"
  cat test.log
else
  echo "PASSED: charmm22 energy regression test"
fi

cd ..
echo ""


echo "Tests completed"


