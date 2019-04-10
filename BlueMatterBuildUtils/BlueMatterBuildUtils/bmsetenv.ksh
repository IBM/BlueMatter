#!/usr/bin/ksh

installroot=/gsa/yktgsa/projects/b/bluematter/autotest/RELEASE_CANDIDATE_040106/sandboxes/sb20060702/inst.images/rios_aix_5/shipdata

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
# export PATH=:/usr/java131/bin:/gsa/yktgsa/projects/b/bluematter/local/$SYS_PLATFORM/bin:/gsa/yktgsa/projects/b/bluematter/local/bin:/usr/agora/bin:/usr/local/bin:/usr/bin:/usr/lpp/pd/bin:/usr/dt/bin:/usr/bin/X11:/etc:/usr/sbin:/usr/ucb:/sbin:/usr/gnu/bin:.
export PATH=:/usr/java/bin:/usr/agora/bin:/usr/local/bin:/usr/bin:/usr/lpp/pd/bin:/usr/dt/bin:/usr/bin/X11:/etc:/usr/sbin:/usr/ucb:/sbin:/usr/gnu/bin:.


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

# export LIBPATH=/usr/lib:/gsa/yktgsa/projects/b/bluematter/local/lib:/usr/gnu/lib
export LIBPATH=/usr/lib:/usr/gnu/lib

# attempt to set db2 environment for db2 instance named db2instl
if [[ -f ~db2insts/sqllib/db2profile ]]; then
    . ~db2insts/sqllib/db2profile
else
    print "ERROR: No db2 instance named db2insts on this machine"
    return -1
fi
