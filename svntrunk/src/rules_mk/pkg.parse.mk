################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines parsing targets.
#  Targets defined:
#    PARSE_TARGETS
#
################################################################################
.if !defined(_PKG_PARSE_MK_)
_PKG_PARSE_MK_=

.if defined(PARSE) || defined(PACKAGE)

_PARSERGENFLAGS_ ?= \
	-DTOSTAGE=${TOSTAGE} \
	-DPKG_CONTROL_DIR=${PKG_CONTROL_DIR} \
	-DPKG_CLASS=${PKG_CLASS} \
	-DPKG_TYPE=${PKG_TYPE} \
	-DPKG_CMF_FILE=${PKG_CMF_FILE}
.if ${PKGFAMILY:l}=="pkgmk"
  _PARSERGENFLAGS_ += \
        -DPATCH_NAME="${PATCH_NAME}" 
.endif # ${PKGFAMILY:l}=="pkgmk" 
.if ${PKG_CLASS:l}=="sp"
 .if !empty(MACHINE:Mmvs390_oe_2)
  _PARSERGENFLAGS_ += \
        -DPKG_FIX_STRATEGY=${PKG_FIX_STRATEGY} \
        -DPKG_API_URL=${PKG_API_URL} \
        -DAPAR=${APAR} \
        -DUSERMOD=${USERMOD} \
        -DB390_NEW_FILES="${B390_NEW_FILES}" \
        -DB390_DELETED_FILES="${B390_DELETED_FILES}" \
        -DB390_JCLIN_FILES="${B390_JCLIN_FILES}"
 .elif !empty(MACHINE:M*_solaris_*)
  _PARSERGENFLAGS_ += \
        -DPATCH_NEW_LINKS=${PATCH_NEW_LINKS} \
        -DPATCH_DELETED_FILES=${PATCH_DELETED_FILES}
 .elif (${MACHINE}==hp9000_ux_11) || !empty(MACHINE:Mia64_hpux_*)
  _PARSERGENFLAGS_ += \
        -DPATCH_NAME=${PATCH_NAME} \
        -DPATCH_NEW_FILESETS=${PATCH_NEW_FILESETS} \
        -DPATCH_ANCESTOR_LIST="${PATCH_ANCESTOR_LIST}" \
        -DPATCH_SUPERSEDES_LIST="${PATCH_SUPERSEDES_LIST}" \
        -DPATCH_NEW_LINKS=${PATCH_NEW_LINKS} \
        -DPATCH_NEW_DIRS=${PATCH_NEW_DIRS}
 .elif !empty(MACHINE:M*_linux_*)
  _PARSERGENFLAGS_ += \
        -DPATCH_NAME=${PATCH_NAME} \
        -DPATCH_REQUIRES_LIST="${PATCH_REQUIRES_LIST}" \
        -DPATCH_NEW_LINKS=${PATCH_NEW_LINKS} \
        -DPATCH_NEW_DIRS=${PATCH_NEW_DIRS}
 .endif # !empty(MACHINE:Mmvs390_oe_2)
.endif # ${PKG_CLASS:l}=="sp"

PARSE_TARGETS: .SPECTARG
.if (${PKG_CLASS:l}=="ipp" && defined(PKG_IS_SUPPORTED)) || \
    (${PKG_CLASS:l}=="sp" && defined(PKG_IS_SP_SUPPORTED)) || \
    (${PKG_CLASS:l}=="st")
	${PKG_CONTROL_DIR:C}
	${PKG_TEMP_DIR:C}
	${PKG_OUTPUT_DIR:C}
	${TOSTAGE:C}
 .if empty(MACHINE:Mx86_nt_*) && empty(MACHINE:Mx86_95_*)
	${JVM} ${_PKG_JVM_FLAGS_} ${_PARSERGENFLAGS_} \
	  com.ibm.ode.pkg.parserGenerator.ParserGeneratorInitiator
 .endif # not Windows
.elif ${PKG_CLASS:l}=="ipp" && !defined(PKG_IS_SUPPORTED)
	@${ECHO} ERROR: ODE Packaging tasks are not supported on platform ${MACHINE}.
.elif ${PKG_CLASS:l}=="sp" && !defined(PKG_IS_SP_SUPPORTED)
        @${ECHO} ERROR: ODE Service Packaging tasks are not supported on platform ${MACHINE}.
.endif  #if OS2/NetBSD/Linux/SCO

.else  # nothing to parse
PARSE_TARGETS: .SPECTARG
.endif # defined(PARSE) || defined(PACKAGE)
.endif # defined(PKG_PARSE_MK)

