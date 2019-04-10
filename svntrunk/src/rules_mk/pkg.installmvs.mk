################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines packaging install targets for MVS.
#  Targets defined:
#    INSTALLMVS_TARGETS
#
################################################################################
.if !defined(_PKG_INSTALLMVS_MK_)
_PKG_INSTALLMVS_MK_=

.if defined(INSTALLMVS)
_INSTALLMVSFLAGS_ ?= \
        -DPKG_CLASS=${PKG_CLASS} \
        -DPKG_EVENT="gather" \
        -DPKG_CONTROL_DIR=${PKG_CONTROL_DIR} \
        -DTOSTAGE=${TOSTAGE} \
        -DPKG_OUTPUT_DIR=${PKG_OUTPUT_DIR} \
        -DPKG_TEMP_DIR=${PKG_TEMP_DIR} \
        -DMVS_JOBCARD=${MVS_JOBCARD} \
        -DMVS_USERID=${MVS_USERID} \
        -DMVS_PASSWORD=${MVS_PASSWORD} \
        -DMVS_EXEC_DATASET=${MVS_EXEC_DATASET} \
        -DMVS_JOBMONITOR_TIME=${MVS_JOBMONITOR_TIME} \
        -DMVS_DELETE_OUTPUT=${MVS_DELETE_OUTPUT} \
        -DMVS_DISPLAY_OUTPUT=${MVS_DISPLAY_OUTPUT} \
        -DMVS_SAVE_OUTPUT_FILE=${MVS_SAVE_OUTPUT_FILE}

INSTALLMVS_TARGETS: .SPECTARG
	${JVM} ${_PKG_JVM_FLAGS_} ${_INSTALLMVSFLAGS_} \
	  com.ibm.ode.pkg.pkgMvs.RunMvsPkg

.else # Nothing to install
INSTALLMVS_TARGETS: .SPECTARG
.endif # defined(INSTALLMVS)
.endif # defined(_PMG_INSTALLMVS_MK_)

