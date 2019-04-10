#!/usr/bin/ksh

# script to set up directory to use Blue Matter install image under path
# passed as command line argument


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


# ###########################################################################
#
# Environment Setup (we are assuming that we are not inside ODE)
#
# ###########################################################################

umask 022

# set PATH to include python
export PATH=:/usr/java131/bin:/u/germain/projects/cbc/local/bin:/usr/agora/bin:/usr/local/bin:/usr/bin:/usr/lpp/pd/bin:/usr/dt/bin:/usr/bin/X11:/etc:/usr/sbin:/usr/ucb:/sbin:/usr/gnu/bin:.


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


# attempt to set db2 environment for db2 instance named db2instl
if [[ -f ~db2instl/sqllib/db2profile ]]; then
    . ~db2instl/sqllib/db2profile
else
    print "ERROR: No db2 instance named db2instl on this machine"
    return -1
fi
