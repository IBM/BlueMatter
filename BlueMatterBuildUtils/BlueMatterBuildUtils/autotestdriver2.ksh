#!/usr/bin/ksh
export SYS_NAME=`uname -s | tr [A-Z] [a-z]`
export SYS_VERSION=`uname -v | tr [A-Z] [a-z]`
export SYS_PLATFORM=${SYS_NAME}${SYS_VERSION}

# modified script to run tests on existing sandbox
sandboxName=$1
START_TIME=`date`
echo "Build started at $START_TIME"
#TESTDIR=/sandboxes/rgermain/test
TESTDIR=`pwd`

export BL_INSTALL=/bgl/BlueLight/ppcfloor
echo "Starting tests ...."

cd $TESTDIR
# time $TESTDIR/test-o-matic.ksh $TESTDIR/sandboxes/$sandboxName/inst.images/rios_aix_5/shipdata > $TESTDIR/logs/test$sandboxName 2>&1
time $TESTDIR/BlueMatterBuildUtils/autotest-o-matic.ksh $TESTDIR/sandboxes/$sandboxName/inst.images/rios_${SYS_NAME}_${SYS_VERSION}/shipdata > $TESTDIR/logs/test$sandboxName 2>&1

END_TIME=`date`
echo "Tests ended at $END_TIME"

