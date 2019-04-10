################################################################################
# Version: ODEi2.5 (Build 15, 20-Feb-2001)
#
# This file defines parsing targets.
#  Targets defined:
#    PARSE_TARGETS
#
################################################################################
.if !defined(_PKG_PTFNUM_MK_)
_PKG_PTFNUM_MK_=

_SVCMVSFLAGS_ ?= \
      -DB390_DIR=${B390_DIR} \
      -DB390_SCRIPT=${B390_SCRIPT} \
      -DPKG_CONTROL_DIR=${PKG_CONTROL_DIR} \
      -DPKG_API_URL=${PKG_API_URL} \
      -DCMVC_LEVEL=${CMVC_LEVEL} \
      -DCMVC_RELEASE=${CMVC_RELEASE} \
      -DCMVC_FAMILY=${CMVC_FAMILY} \
      -DPKG_EVENT="getptfnums"

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
.endif  #if OS2/NetBSD/Linux/SCO

.else  # nothing to parse
getptfnums_all: .SPECTARG
.endif # defined(PKG_PTFNUM_MK)

