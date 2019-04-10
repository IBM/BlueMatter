#!/usr/bin/ksh

# ksh script to checkout and build entire BlueMatter application and utilities
# based on version of code in tree at 0100 local time today
# this script also sets up its own environment

# arguments:
# $1 = module name <default = ode_all>
# $2 = sandbox directory <default = $PWD>
# $3 = backing build directory <default = none>

moduleName="ode_all"
sandboxDir=$PWD
bbDir=-1
sandboxName=`date +%Y%m%d`
isodate=`date +%Y-%m-%d`
umask 022

if [[ $# = 3 ]]; then
    bbDir=$3
    sandboxDir=$2
    moduleName=$1
elif [[ $# = 2 ]]; then
    sandboxDir=$2
    moduleName=$1
elif [[ $# = 1 ]]; then
    moduleName=$1
else
    print "$0 <module=ode_all> <sandboxDir=PWD> <backingBuildPath=none>"
fi

print "module = $moduleName"
print "sandbox directory = $sandboxDir"
print "sandbox name = $sandboxName"

# ###########################################################################
#
# Environment Setup
#
# ###########################################################################

# set PATH to include ode and python
export PATH=:/usr/ode/bin:/usr/opt/pkgtools/bin:/usr/java131/bin:/u/germain/projects/cbc/local/bin:/usr/agora/bin:/usr/local/bin:/usr/bin:/bin:/usr/lpp/pd/bin:/usr/dt/bin:/usr/bin/X11:/etc:/usr/sbin:/usr/ucb:/sbin:/usr/gnu/bin:.

# set CLASSPATH to include ode tools
export CLASSPATH=/usr/ode/bin/ode_tools.jar

unset LIBPATH
unset LD_LIBRARY_PATH

# attempt to set db2 environment for db2 instance named db2instl
if [[ -f ~db2instl/sqllib/db2profile ]]; then
    . ~db2instl/sqllib/db2profile
else
    print "ERROR: No db2 instance named db2instl on this machine"
#    return -1
fi

# ###########################################################################
#
# ODE Sandbox Creation (backing build)
#
# ###########################################################################

if [[ $bbDir = -1 ]]; then
    if mkbb -dir $sandboxDir -m rios_aix_5:ppc_linux_2:bgl_cnk_1:x86_linux_2:x86_nt_4 $sandboxName
    then
        print "Created sandbox in $sandboxDir/$sandboxName"
    else
        print "Error creating sandbox"
        return -1
    fi
else
    if mksb -dir $sandboxDir -back $bbDir -m rios_aix_5:ppc_linux_2:bgl_cnk_1:x86_linux_2:x86_nt_4 $sandboxName
    then
        print "Created sandbox in $sandboxDir/$sandboxName backed by $bbDir"
    else
        print "Error creating sandbox"
        return -1
    fi
fi

# ###########################################################################
#
# cvs checkout of yesterday's code base
#
# ###########################################################################

if cd $sandboxDir/$sandboxName
then
    print "Now in $PWD"
else
    print "Error in cd to $sandboxDir/$sandboxName"
    return -1
fi

# checkout code as of 17:00 local time today

#if cvs checkout -D "$isodate 17:00" $moduleName
if cvs checkout $moduleName
then
    print "checkout of code version as of $isodate 17:00 of $moduleName was successful"
else
    print "Error in checkout"
    return -1
fi

# ###########################################################################
#
# do the build in ode
#
# ###########################################################################

if echo 'build -j4 build_all install_all' | workon -sb $sandboxName
then
    print "Build Successful"
else
    print "Error in Build!!"
    return -1
fi

return 0


