#!/usr/bin/ksh
export SYS_NAME=`uname -s | tr [A-Z] [a-z]`
export SYS_VERSION=`uname -v | tr [A-Z] [a-z]`
export SYS_PLATFORM=${SYS_NAME}${SYS_VERSION}

# wrapper driver to be called by cron
sandboxName=sb`date +%Y%m%d%H%M`
START_TIME=`date`
echo "Build started at $START_TIME"
TESTDIR=/sandboxes/rgermain/test
cd $TESTDIR
time $TESTDIR/build-o-matic.ksh $TESTDIR/sandboxes $sandboxName ode_cron > $TESTDIR/logs/build.out 2>&1

if (( $? )); then
  echo "Build failed, exiting..."
  return -1
fi

BUILD_END=`date`
echo "Build ended at $BUILD_END -- Starting tests"

cd $TESTDIR
# time $TESTDIR/test-o-matic.ksh $TESTDIR/sandboxes/$sandboxName/inst.images/rios_aix_5/shipdata > $TESTDIR/logs/test$sandboxName 2>&1
time $TESTDIR/test-o-matic.ksh $TESTDIR/sandboxes/$sandboxName/inst.images/rios_${SYS_NAME}_${SYS_VERSION}/shipdata > $TESTDIR/logs/test$sandboxName 2>&1

END_TIME=`date`
echo "Tests ended at $END_TIME"

