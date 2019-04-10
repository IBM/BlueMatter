
##############################################################################
# set up environment based on environment variables:
# If we are in ode (PACKAGEBASE defined), then use install image from ode sandbox
# Otherwise, if variable BG_INSTALL_BASE is defined, then use that install image
# Otherwise use the default install image (under /usr/opt/bluegene)
#
if ( ${?PACKAGEBASE} != "0" ) then
    setenv BG_INSTALL_BASE ${PACKAGEBASE}/shipdata
    echo "using BG_INSTALL_BASE = $BG_INSTALL_BASE"
else 
    if ( ${?BG_INSTALL_BASE} != "0" ) then
         echo "using BG_INSTALL_BASE = $BG_INSTALL_BASE"
    else
         setenv BG_INSTALL_BASE ""
         echo "Neither PACKAGEBASE nor BG_INSTALL_BASE is defined, using installed base"
    endif
endif

setenv BG_BIN_DIR ${BG_INSTALL_BASE}/usr/opt/bluegene/bin
setenv CLASSPATH ${BG_INSTALL_BASE}/usr/opt/bluegene/bin/setup/db2probspec.jar:${CLASSPATH}
setenv PATH ${BG_BIN_DIR}/utils:${BG_BIN_DIR}/setup:${BG_BIN_DIR}/simulation:${BG_BIN_DIR}/analysis:${PATH}
setenv PYTHONPATH ${BG_INSTALL_BASE}/usr/opt/bluegene/lib/python


# the following is not necessary if a workon is issued and is meaningless if
# workon is not issued
#setenv CLASSPATH ${SANDBOXBASE}/export/classes:$CLASSPATH

# #####################################################
# set up db2 environment
#
source ~db2insts/sqllib/db2cshrc

# #####################################################
# fix up for 64-bit environment 
#
if ( ${?CONTEXT} == "rios64_aix_5" ) then
  setenv LIBPATH ${DB2DIR}/lib64:${LIBPATH}
else 
  if ( ${?CONTEXT} == "ppc64_linux_2" ) then
    setenv LD_LIBRARY_PATH /opt/IBM/db2/V8.1/lib64:${LD_LIBRARY_PATH}
  endif
endif

###################### new ode stuff ###############  
setenv PATH /usr/opt/pkg_tools/bin/power:$PATH

#############probspec alias##################
alias xml2db2.pl perl ${BG_BIN_DIR}/setup/xml2db2.pl
alias DumpSysId java com.ibm.bluematter.utils.DumpSysId
alias DropSysId java com.ibm.bluematter.utils.DropSystem
alias probspecdb2 java com.ibm.bluematter.db2probspec.ProbspecgenDB2
alias rtp2db2 java com.ibm.bluematter.utils.Rtp2db2
alias impl2db2 java com.ibm.bluematter.utils.Impl2db2
alias plt2db2 java com.ibm.bluematter.utils.Plt2db2
alias compileMSD java com.ibm.bluematter.utils.CompileMSD

alias js python $BG_BIN_DIR/utils/jobstatus.py  
alias jc python $BG_BIN_DIR/utils/jobcontrol.py  
#############directory alias##################
if ( ${?PACKAGEBASE} != "0" ) then
    alias bm_src   cd ${SANDBOXBASE}/src/BlueMatter
    alias bmsrc    cd ${SANDBOXBASE}/src/BlueMatter
    alias bm_bin   cd ${SANDBOXBASE}/obj/${CONTEXT}/BlueMatter
    alias mdtest   cd ${SANDBOXBASE}/src/mdtest
    alias bmdata   cd ${SANDBOXBASE}/src/mdtest/data/bluematter
    alias cdata    cd ${SANDBOXBASE}/src/mdtest/data/charmm22
    alias create   cd ${SANDBOXBASE}/src/mdtest/data/create/charmm22
    alias reg      cd ${SANDBOXBASE}/src/mdtest/regression
    alias val      cd ${SANDBOXBASE}/src/mdtest/validation
    alias pf       cd ${SANDBOXBASE}/src/mdtest/performance
    alias extreg   cd ${SANDBOXBASE}/src/mdtest/extreg
endif

alias bmbin    cd ${BG_BIN_DIR}
alias bmsetup  cd ${BG_BIN_DIR}/setup


