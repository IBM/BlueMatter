
alias svndiff="svn diff --diff-cmd /usr/gnu/bin/diff -x'-w' $*"

##############################################################################
# set up environment based on environment variables:
# If we are in ode (PACKAGEBASE defined), then use install image from ode sandbox
# Otherwise, if variable BG_INSTALL_BASE is defined, then use that install image
# Otherwise use the default install image (under /usr/opt/bluegene)
#
if [[ -n ${PACKAGEBASE} ]]; then
    export BG_INSTALL_BASE=${PACKAGEBASE}/shipdata
elif [[ -n ${BG_INSTALL_BASE} ]]; then
    echo "PACKAGEBASE not defined, using BG_INSTALL_BASE="$BG_INSTALL_BASE
    export BG_INSTALL_BASE=${BG_INSTALL_BASE}
else
    echo "Neither PACKAGEBASE, nor BG_INSTALL_BASE is defined, using installed base"
    export BG_INSTALL_BASE=
fi

export BG_BIN_DIR=${BG_INSTALL_BASE}/usr/opt/bluegene/bin
export CLASSPATH=${BG_INSTALL_BASE}/usr/opt/bluegene/bin/setup/db2probspec.jar:${CLASSPATH}
export PATH=${BG_BIN_DIR}/utils:${BG_BIN_DIR}/setup:${BG_BIN_DIR}/simulation:${BG_BIN_DIR}/analysis:${PATH}
export PYTHONPATH=${BG_INSTALL_BASE}/usr/opt/bluegene/lib/python

echo "SETTING MP_SHARED_MEMORY=yes "
export MP_SHARED_MEMORY=yes
export NLSPATH=/usr/lib/nls/msg/%L/%N:/usr/lib/nls/msg/%L/%N.cat
echo "SETTING NLSPATH to "
echo $NLSPATH
echo " "
#
#
################################################################################


echo BG_INSTALL_BASE: $BG_INSTALL_BASE
echo BG_BIN_DIR: $BG_BIN_DIR
echo CLASSPATH: $CLASSPATH
echo PATH: $PATH
echo PYTHONPATH: $PYTHONPATH

# the following is not necessary if a workon is issued and is meaningless if
# workon is not issued
#export CLASSPATH=${SANDBOXBASE}/export/classes:$CLASSPATH

# #####################################################
# set up db2 environment
#
. ~db2insts/sqllib/db2profile
#. /bgl/BlueLight/ppcfloor/bglsys/bin/db2profile 

# #####################################################
# fix up for 64-bit environment 
#
if [[ "${CONTEXT}" == "rios64_aix_5" ]]
then 
  LIBPATH="${DB2DIR}/lib64:${LIBPATH}"
elif [[ "${CONTEXT}" == "ppc64_linux_2" ]]
then
  LD_LIBRARY_PATH="/opt/IBM/db2/V8.1/lib64:${LD_LIBRARY_PATH}"
fi

###################### new ode stuff ###############
export PATH=/usr/opt/pkg_tools/bin/power:$PATH

#############probspec alias##################
alias xml2db2='perl ${BG_BIN_DIR}/setup/xml2db2.pl'
alias DumpSysId='java com.ibm.bluematter.utils.DumpSysId'
alias DropSysId='java com.ibm.bluematter.utils.DropSystem'
alias probspecdb2='java com.ibm.bluematter.db2probspec.ProbspecgenDB2'
alias rtp2db2='java com.ibm.bluematter.utils.Rtp2db2'
alias impl2db2='java com.ibm.bluematter.utils.Impl2db2'
alias plt2db2='java com.ibm.bluematter.utils.Plt2db2'
alias compileMSD='java com.ibm.bluematter.utils.CompileMSD'
export xml2db2
export DumpSysId
export DropSysId
export probspecdb2
export rtp2db2
export impl2db2
export plt2db2
export compileMSD

#############directory alias##################
alias bm_src='cd ${SANDBOXBASE}/src/BlueMatter'
alias bmsrc='cd ${SANDBOXBASE}/src/BlueMatter'
alias bm_bin='cd ${SANDBOXBASE}/obj/${CONTEXT}/BlueMatter'
alias bmbin='cd ${BG_BIN_DIR}'
alias bmsetup='cd ${BG_BIN_DIR}/setup'
alias mdtest='cd ${SANDBOXBASE}/src/mdtest'
alias bmdata='cd ${SANDBOXBASE}/src/mdtest/data/bluematter'
alias cdata='cd ${SANDBOXBASE}/src/mdtest/data/charmm22'
alias create='cd ${SANDBOXBASE}/src/mdtest/data/create/charmm22'
alias reg='cd ${SANDBOXBASE}/src/mdtest/regression'
alias val='cd ${SANDBOXBASE}/src/mdtest/validation'
alias pf='cd ${SANDBOXBASE}/src/mdtest/performance'
alias extreg='cd ${SANDBOXBASE}/src/mdtest/extreg'
export bm_src
export bmsrc
export bm_bin
export bmbin
export bmsetup
export mdtest
export bmdata
export cdata
export create
export reg
export val
export pf
export extreg
