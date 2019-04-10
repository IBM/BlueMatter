#!/usr/bin/ksh

# ksh script to checkout and build entire BlueMatter application and utilities
# this script also sets up its own environment

# arguments:
# $1 = sandbox directory
# $2 = sandbox name
# $3 = SVN symbolic tag <default = none (bleeding edge)>

sandboxDir=$PWD
sandboxName=sb$$

umask 022

svntag="none"
if [[ $# = 3 ]]; then
    svntag=$3
    sandboxName=$2
    sandboxDir=$1
elif [[ $# = 2 ]]; then
    sandboxName=$2
    sandboxDir=$1
else
    print "$0 sandboxDir sandboxName <svntag=none>"
    return -1
fi

print "tag = $svntag"
print "module = $moduleName"
print "sandbox directory = $sandboxDir"
print "sandbox name = $sandboxName"

# ###########################################################################
#
# Environment Setup
#
# ###########################################################################

unset LIBPATH
unset LD_LIBRARY_PATH

# set SVNREPOS
export SVNREPOS=http://corey.watson.ibm.com/svn/bluematter

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

# ###########################################################################
#
# ODE Sandbox Creation (backing build)
#
# ###########################################################################

if mkbb -dir $sandboxDir -m rios_aix_5:rios64_aix_5:ppc_linux_2:bgl_cnk_1:x86_linux_2:x86_nt_4:ppc64_linux_2 $sandboxName
then
    print "Created sandbox in $sandboxDir/$sandboxName"
else
    print "Error creating sandbox"
    return -1
fi

# ###########################################################################
#
# svn checkout
#
# ###########################################################################

if cd $sandboxDir/$sandboxName
then
    print "Now in $PWD"
else
    print "Error in cd to $sandboxDir/$sandboxName"
    return -1
fi

if [[ $# = 2 ]]; then
    if svn checkout $SVNREPOS/trunk/dev src
    then
        if [[ -n "$DO_MDTEST" ]]; then
          cd src
          if svn checkout $SVNREPOS/trunk/mdtest
          then
              print "Checked out mdtest successfully from Subversion"
          else
              print "Error checking out mdtest"
              return -1
          fi
        fi
        print "Checked out HEAD successfully from Subversion"
    else
        print "Error in Subversion checkout"
        return -1
    fi
else
    if svn checkout $SVNREPOS/tags/$svntag/dev src
    then
        if [[ -n "$DO_MDTEST" ]]; then
          cd src
          if svn checkout $SVNREPOS/tags/$svntag/mdtest
          then
              print "Checked out mdtest in tag $svntag successfully from Subversion"
          else
              print "Error checking out mdtest"
              return -1
          fi
        fi
        print "Checked out tag $svntag successfully from Subversion"
    else
        print "Error in checkout"
        return -1
    fi
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


