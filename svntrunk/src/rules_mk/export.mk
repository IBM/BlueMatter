################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines the global "pass" targets:
#  Targets defined:
#    export_all
#    setup_all
#    expinc_<header file to export>
#  Some variables used:
#
################################################################################

.if !defined(__EXPORT_MK_)
__EXPORT_MK_=

.if !defined(EXPORTBASE)

export_all: .SPECTARG
	${You must define EXPORTBASE to do an ${.TARGET}:L:a&STDOUT}

.if !empty(_EXPORT_TARGETS_:Mexport_*)
${_EXPORT_TARGETS_:Mexport_*}:
	${You must define EXPORTBASE to do an ${.TARGET}:L:a&STDOUT}
.endif

.else

.if defined(SPECIAL_PASSES)
export_all: EXPORT .SPECTARG
.else
export_all: $${_all_targets_} .SPECTARG
.endif

################################################################################
#
# Export program rules
#
_EXPORT_TARGETS_PROGRAMS_ = ${_EXPORT_TARGETS_:Mexppgm_*}
${_EXPORT_TARGETS_PROGRAMS_}: $${_EXPPGMFILES_} .SPECTARG;@

${_EXPORT_TARGETS_PROGRAMS_:S/^exppgm_//g:@_EXPPGM_TARGET@${_EXPPGMFILES_}@}: $${.TARGET:T}
	${.TARGET:H:C}
	${.TARGET:rm-}
.if !empty(MACHINE:M*_openvms_*)
	${CP} ${_CPFLAGS_} ${.ALLSRC:@.PATH.@${VMS_PATH_CONV}@} \
        ${.TARGET:@.PATH.@${VMS_PATH_CONV}@}
.elifndef UNIX
	${CP} ${_CPFLAGS_} ${EXP_QUOTE}${.ALLSRC:S|/|\\|g}${EXP_QUOTE} ${EXP_QUOTE}${.TARGET:S|/|\\|g}${EXP_QUOTE}
.else # Unix rules follow
	${.TARGET:rm-}
.if defined(EXPORT_USING_COPY)
	${CP} ${_CPFLAGS_} ${EXP_QUOTE}${.ALLSRC}${EXP_QUOTE} ${EXP_QUOTE}${.TARGET}${EXP_QUOTE}
.elif defined(EXPORT_USING_TAR)
	(${CHDIR} ${.ALLSRC:H}; ${TAR} ${_C_TARFLAGS_} - \
  ${EXP_QUOTE}${.ALLSRC:T}${EXP_QUOTE}) | \
	(${CHDIR} ${.TARGET:H}; ${TAR} ${_X_TARFLAGS_} -)
.else
.ifdef EXPLINK_ABSPATH
	${LN} ${_LNFLAGS_} ${EXP_QUOTE}${.ALLSRC}${EXP_QUOTE} \
        ${EXP_QUOTE}${.TARGET}${EXP_QUOTE}
.else
	${LN} ${_LNFLAGS_} ${EXP_QUOTE}${_RELATIVE_EXPPGM_TARGET_}${EXP_QUOTE} \
        ${EXP_QUOTE}${.TARGET}${EXP_QUOTE}
.endif # EXPLINK_ABSPATH
.endif # EXPORT_USING_TAR
.ifdef CHMOD_ON_EXPORT
	${CHMOD} ${${.ALLSRC:T}_EMODE:U${EMODE}} ${EXP_QUOTE}${.TARGET}${EXP_QUOTE}
.endif
.ifdef CHOWN_ON_EXPORT
	${CHOWN} ${${.ALLSRC:T}_EOWNER:U${EOWNER}}:${${.ALLSRC:T}_EGROUP:U${EGROUP}} \
           ${EXP_QUOTE}${.TARGET}${EXP_QUOTE}
.endif
.endif # UNIX

################################################################################
#
# Export library rules
#
_EXPORT_TARGETS_LIBS_ = ${_EXPORT_TARGETS_:Mexport_*}
${_EXPORT_TARGETS_LIBS_:Mexport_*}: $${_EXPFILES_} .SPECTARG;@

${_EXPORT_TARGETS_LIBS_:S/^export_//g:@_EXPORT_TARGET@${_EXPFILES_}@}:\
		$${.TARGET:T}
	${.TARGET:H:C}
	${.TARGET:rm-}
.if !empty(MACHINE:M*_openvms_*)
	${CP} ${_CPFLAGS_} ${.ALLSRC:@.PATH.@${VMS_PATH_CONV}@} \
        ${.TARGET:@.PATH.@${VMS_PATH_CONV}@}
.elifndef UNIX
	${CP} ${_CPFLAGS_} ${EXP_QUOTE}${.ALLSRC:S|/|\\|g}${EXP_QUOTE} \
        ${EXP_QUOTE}${.TARGET:S|/|\\|g}${EXP_QUOTE}
.else # Unix rules follow
	${.TARGET:rm-}
.if defined(EXPORT_USING_COPY)
	${CP} ${_CPFLAGS_} ${EXP_QUOTE}${.ALLSRC}${EXP_QUOTE} \
        ${EXP_QUOTE}${.TARGET}${EXP_QUOTE}
.elif defined(EXPORT_USING_TAR)
	(${CHDIR} ${.ALLSRC:H}; ${TAR} ${_C_TARFLAGS_} - \
  ${EXP_QUOTE}${.ALLSRC:T}${EXP_QUOTE}) | \
	(${CHDIR} ${.TARGET:H}; ${TAR} ${_X_TARFLAGS_} -)
.else
.ifdef EXPLINK_ABSPATH
	${LN} ${_LNFLAGS_} ${EXP_QUOTE}${.ALLSRC}${EXP_QUOTE} \
        ${EXP_QUOTE}${.TARGET}${EXP_QUOTE}
.else
	${LN} ${_LNFLAGS_} ${EXP_QUOTE}${_RELATIVE_EXPLIB_TARGET_}${EXP_QUOTE} \
        ${EXP_QUOTE}${.TARGET}${EXP_QUOTE}
.endif # EXPLINK_ABSPATH
.endif # EXPORT_USING_TAR
.ifdef CHMOD_ON_EXPORT
	${CHMOD} ${${.ALLSRC:T}_EMODE:U${EMODE}} ${EXP_QUOTE}${.TARGET}${EXP_QUOTE}
.endif
.ifdef CHOWN_ON_EXPORT
	${CHOWN} ${${.ALLSRC:T}_EOWNER:U${EOWNER}}:${${.ALLSRC:T}_EGROUP:U${EGROUP}} \
           ${EXP_QUOTE}${.TARGET}${EXP_QUOTE}
.endif
.endif # UNIX

################################################################################
#
# Export header files rules
#
_EXPORT_TARGETS_INCLUDES_ = ${_EXPORT_TARGETS_:Mexpinc_*}
${_EXPORT_TARGETS_INCLUDES_}: $${_EXPINCFILES_} .SPECTARG;@

${_EXPORT_TARGETS_INCLUDES_:S/^expinc_//g:@_EXPINC_TARGET@${_EXPINCFILES_}@}: $${.TARGET:T}
	${.TARGET:H:C}
	${.TARGET:rm-}
.if !empty(MACHINE:M*_openvms_*)
	${CP} ${_CPFLAGS_} ${.ALLSRC:@.PATH.@${VMS_PATH_CONV}@} \
        ${.TARGET:@.PATH.@${VMS_PATH_CONV}@}
.elifndef UNIX
	${CP} ${_CPFLAGS_} ${EXP_QUOTE}${.ALLSRC:S|/|\\|g}${EXP_QUOTE} ${EXP_QUOTE}${.TARGET:S|/|\\|g}${EXP_QUOTE}
.else # Unix rules follow
	${.TARGET:rm-}
.if defined(EXPORT_USING_COPY)
	${CP} ${_CPFLAGS_} ${EXP_QUOTE}${.ALLSRC}${EXP_QUOTE} ${EXP_QUOTE}${.TARGET}${EXP_QUOTE}
.elif defined(EXPORT_USING_TAR)
	(${CHDIR} ${.ALLSRC:H}; ${TAR} ${_C_TARFLAGS_} - \
  ${EXP_QUOTE}${.ALLSRC:T}${EXP_QUOTE}) | \
	(${CHDIR} ${.TARGET:H}; ${TAR} ${_X_TARFLAGS_} -)
.else
.ifdef EXPLINK_ABSPATH
	${LN} ${_LNFLAGS_} ${EXP_QUOTE}${.ALLSRC}${EXP_QUOTE} \
        ${EXP_QUOTE}${.TARGET}${EXP_QUOTE}
.else
	${LN} ${_LNFLAGS_} ${EXP_QUOTE}${_RELATIVE_EXPINC_TARGET_}${EXP_QUOTE} \
        ${EXP_QUOTE}${.TARGET}${EXP_QUOTE}
.endif # EXPLINK_ABSPATH
.endif # EXPORT_USING_TAR
.ifdef CHMOD_ON_EXPORT
	${CHMOD} ${${.ALLSRC:T}_EMODE:U${EMODE}} ${EXP_QUOTE}${.TARGET}${EXP_QUOTE}
.endif
.ifdef CHOWN_ON_EXPORT
	${CHOWN} ${${.ALLSRC:T}_EOWNER:U${EOWNER}}:${${.ALLSRC:T}_EGROUP:U${EGROUP}} \
           ${EXP_QUOTE}${.TARGET}${EXP_QUOTE}
.endif
.endif # UNIX

.endif
#endif defined(EXPORT_BASE)

################################################################################
#
# Setup rules
#

.if defined(SPECIAL_PASSES)
setup_all: SETUP .SPECTARG
.else
setup_all: $${_all_targets_} .SPECTARG;@
.endif

${_SETUP_TARGETS_:Msetup_*}: $${_SETUPFILES_};@

${_SETUP_TARGETS_:Msetup_*:S/^setup_//g:@_SETUP_TARGET@${_SETUPFILES_}@} : \
	       $${_SETUP_TARGETS_:Msetup_*:S|^setup_|/|g:xM|.*[/\\]$${.TARGET:T}$$$$|:S|^/||g}
	${.TARGET:H:C}
	${.TARGET:rm-}
.if !empty(MACHINE:M*_openvms_*)
	${CP} ${_CPFLAGS_} ${.ALLSRC:@.PATH.@${VMS_PATH_CONV}@} \
        ${.TARGET:@.PATH.@${VMS_PATH_CONV}@}
.elif ${BACKSLASH_PATHS}
	${CP} ${_CPFLAGS_} ${SETUP_QUOTE}${.ALLSRC:S|/|\\|g}${SETUP_QUOTE} ${SETUP_QUOTE}${.TARGET:S|/|\\|g}${SETUP_QUOTE}
.else
	${CP} ${_CPFLAGS_} ${SETUP_QUOTE}${.ALLSRC}${SETUP_QUOTE} \
        ${SETUP_QUOTE}${.TARGET}${SETUP_QUOTE}
.endif

help::
	@${ECHO} Target: export_all - Places any target file to be exported \
	in the export directory.
	@${ECHO} Target: setup_all - Place preliminary [pre-build] files into \
	a specified directory.

.endif # __EXPORT_MK_
