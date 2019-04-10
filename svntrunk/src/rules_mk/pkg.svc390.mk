################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines targets for various 390 service packaging tasks
#
################################################################################
.if !defined(_PKG_SVC390_MK_)
_PKG_SVC390_MK_=

_SVCMVSFLAGS_ ?= \
      -DB390_PARM_FILE=${B390_PARM_FILE} \
      -DB390_PATH=${B390_PATH} \
      -DB390_TRACE_ONLY=${B390_TRACE_ONLY} \
      -DB390_DEBUG_ON=${B390_DEBUG_ON} \
      -DPKG_CONTROL_DIR=${PKG_CONTROL_DIR} \
      -DPKG_API_URL=${PKG_API_URL} \
      -DCMVC_LEVEL=${CMVC_LEVEL} \
      -DCMVC_RELEASE=${CMVC_RELEASE} \
      -DCMVC_FAMILY=${CMVC_FAMILY} \
      -DSUPERSEDE_PTFS=${SUPERSEDE_PTFS} \
      -DMVS_DISPLAY_OUTPUT=${MVS_DISPLAY_OUTPUT} \
      -Djava.security.policy=${JAVA_SECURITY_POLICY_FILE} \
      -DPKG_EVENT="getptfnums" \
      -DPKG_CLASS="sp"

getptfnums_all: .SPECTARG
.ifdef PKG_IS_SUPPORTED   # see pkg.std.mk
	${PKG_CONTROL_DIR:C}
	${PKG_TEMP_DIR:C}
	${PKG_OUTPUT_DIR:C}
	${TOSTAGE:C}
.if empty(MACHINE:Mx86_nt_*) && empty(MACHINE:Mx86_95_*)
	${JVM} ${_PKG_JVM_FLAGS_} ${_SVCMVSFLAGS_} \
	  com.ibm.ode.pkg.pkgMvs.service.RunMvsSvc
.endif # not Windows
.else
	@${ECHO} ERROR: ODE Packaging tasks are not supported on platform ${MACHINE}.
.endif

## createsetup_all target
.if (${B390_DEBUG_ON:l} == "yes")
_DEBUGFLAG_ = /DEBUG1
.endif
createsetup_all: .SPECTARG
.if (!empty(MACHINE:Mmvs390_oe_*))
.if (${B390_TRACE_ONLY:l} != "yes")
	${JVM} -DMBGlobals.Build390_path=${B390_PATH} Build390.MBClient \
	  CREATESETUP REPLACE=YES PARMFILE=${B390_PARM_FILE} ${_DEBUGFLAG_}
.else
	@${ECHO} ${JVM} -DMBGlobals.Build390_path=${B390_PATH} Build390.MBClient \
	  CREATESETUP REPLACE=YES PARMFILE=${B390_PARM_FILE} ${_DEBUGFLAG_}
.endif
.else
	@${ECHO} ERROR: createsetup_all target not supported on ${MACHINE}.
.endif

.endif # defined(PKG_SVC390_MK)

