#!/usr/bin/ksh

# script to drive basic tests on the BlueMatter code base using a specified
# install image.  Tests are run in or relative to the current working directory

# arguments:
# $1 location of blue matter install image root (everything before /usr/opt/bluegene/...
# $2 build mode to use (default is TestProduction)
installroot=/

buildMode=TestProduction

if [[ $# -eq 1 ]]; then
    installroot=$1
elif [[ $# -eq 2 ]]; then
    installroot=$1
    buildMode=$2
else
    print "$0 installRootPath <buildMode=TestProduction>"
    return -1
fi

export BG_INSTALL_BASE=$installroot

timestamp=`date +%Y_%m_%d_%H_%M`

# ###########################################################################
#
# Environment Setup (we are assuming that we are not inside ODE)
#
# ###########################################################################

umask 022
case "$(uname -s)" in
AIX)    export PATH=/usr/local/bin:/usr/ode/bin:/usr/java14/bin:/usr/bin:/bin ;;
Linux)  export PATH=/usr/local/bin:/usr/bin:/bin:/usr/lib/java/bin ;;
*)      echo "ERROR: Not expecting to build on $(uname -s) . Leaving PATH=${PATH}"
esac

# set CLASSPATH to include ode tools
if [[ -r /usr/ode/bin/ode_tools.jar ]]
then
  ODE_TOOLS_JAR=/usr/ode/bin/ode_tools.jar
else if [[ -r /gsa/yktgsa/home/t/j/tjcw/ode/ode5.0/rules_tools/ode5.0_b20041004.0559_tools.jar ]]
     then
       ODE_TOOLS_JAR=/gsa/yktgsa/home/t/j/tjcw/ode/ode5.0/rules_tools/ode5.0_b20041004.0559_tools.jar
     else
       print "ERROR: No ODE tools are installed. Packaging will not work."
       ODE_TOOLS_JAR=
     fi
fi

export CLASSPATH=${ODE_TOOLS_JAR}

# attempt to set db2 environment for db2 instance named db2insts
if [[ -r ~db2insts/sqllib/db2profile ]]; then
    . ~db2insts/sqllib/db2profile
else
    print "ERROR: No db2 instance named db2insts on this machine"
#    return -1 
fi

## set PATH to include python
#export PATH=:/usr/java131/bin:/u/germain/projects/cbc/local/bin:/usr/agora/bin:/usr/local/bin:/usr/bin:/bin:/usr/lpp/pd/bin:/usr/dt/bin:/usr/bin/X11:/etc:/usr/sbin:/usr/ucb:/sbin:/usr/gnu/bin:.


unset LIBPATH
unset LD_LIBRARY_PATH
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


## attempt to set db2 environment for db2 instance named db2instl
#if [[ -f ~db2instl/sqllib/db2profile ]]; then
#    . ~db2instl/sqllib/db2profile
#else
#    print "ERROR: No db2 instance named db2instl on this machine"
##    return -1
#fi

# ###########################################################################
#
# Create test directory and run tests
#
# ###########################################################################

testDir=test_$timestamp

(
	mkdir $testDir
	
	cd $testDir
	
	# Brooks suite external regression test
	# CHARMM
	(
		mkdir charmm22_brooks
		cd charmm22_brooks
		
		$BG_INSTALL_BASE/usr/opt/bluegene/mdtest/drivers/external/charmm22/brooks/DoBrookList.py $buildMode > test.log 2>&1
		if (( $? )); then
		  echo "FAILED: charmm22 energy regression test"
		  echo "See directory $PWD"
		  cat test.log
		else
		  echo "PASSED: charmm22 energy regression test"
		fi
	)
	
	#OPLSAA
	(
		mkdir oplsaa_brooks
		cd oplsaa_brooks
		
		$BG_INSTALL_BASE/usr/opt/bluegene/mdtest/drivers/external/oplsaa/brooks/dotest.ksh $buildMode
	)
)

