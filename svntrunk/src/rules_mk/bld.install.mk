################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines installation rules.
#  Targets defined:
#    install_all
#    instdep_all
#  Some variables used:
#    TOSTAGE, IDIR
#    IOWNER, IGROUP, IMODE
#    NO_CHOWN, NO_CHMOD, CHMOD, CHOWN
################################################################################
.if !defined(__BLD_INSTALL_MK_)
__BLD_INSTALL_MK_=

TOSTAGE ?= \
  ${SANDBOXBASE}${DIRSEP}${INSTALLATION_DIR}${DIRSEP}${CONTEXT}${DIRSEP}shipdata

.if make(instdep_all)
_IDIR_    = ${${_INSTDEP_TARGET}_IDIR:U${IDIR:U/_MISSING_IDIR_/}}
_TOSTAGE_ = ${${_INSTDEP_TARGET}_TOSTAGE:U${TOSTAGE}}
.else
_IDIR_    = ${${_INSTALL_TARGET}_IDIR:U${IDIR:U/_MISSING_IDIR_/}}
_TOSTAGE_ = ${${_INSTALL_TARGET}_TOSTAGE:U${TOSTAGE}}
.endif # make(instdep_all)

_IDIRINSTALLTARGET_ = ${_IDIR_:@_IDIR_@${INST_QUOTE}${_TOSTAGE_}${_IDIR_}${_INSTALL_TARGET:T}${INST_QUOTE}@}

.if defined(SPECIAL_PASSES)
instdep_all: INSTDEP .SPECTARG
install_all: INSTALL .SPECTARG
.endif

install_all instdep_all : $${_all_targets_} .SPECTARG;@

.if defined(UNIX) || !empty(MACHINE:M*_openvms_*)
_INSTALLRELDIR_	= ${_TOSTAGE_:xs|^${SANDBOXBASE}|${MAKETOP}..|c}${_IDIR_}
_INSTALLABSDIR_ = ${_IDIR_:@_IDIR_@${_TOSTAGE_}${_IDIR_}@}
_TARGET_PATH_   = ${OBJECTDIRTOP}${.TARGET}
_ALLSRC_OSPATH_	= ${INST_QUOTE}${.ALLSRC}${INST_QUOTE}
.else
_INSTALLRELDIR_	= ${_TOSTAGE_:s|\\|/|g:xs|^${SANDBOXBASE:s|\\|/|g}|${MAKETOP}..|c}${_IDIR_}
_INSTALLABSDIR_	= ${_IDIR_:@_IDIR_@${_TOSTAGE_:s|\\|/|g}${_IDIR_}@}
_TARGET_PATH_   = ${OBJECTDIRTOP:s|/|\\|g}${.TARGET:s|/|\\|g}
_ALLSRC_OSPATH_	= ${INST_QUOTE}${.ALLSRC:s|/|\\|g}${INST_QUOTE}
.endif

_TARGET_OSPATH_   = ${INST_QUOTE}${_TARGET_PATH_}${INST_QUOTE}
_TARGET_RELDIR_   = ${_TARGET_PATH_:H}
_TARGET_OSRELDIR_ = ${_TARGET_RELDIR_}${DIRSEP}.

.if !empty(MACHINE:M*_openvms_*)
_ALLSRC_OSPATH_   = ${.ALLSRC:@.PATH.@${VMS_PATH_CONV}@}
_TARGET_OSPATH_   = ${_TARGET_PATH_:@.PATH.@${VMS_PATH_CONV}@}
_TARGET_OSRELDIR_ = ${_TARGET_RELDIR_:@.PATH.@${VMS_PATH_CONV}@}
.endif

.if defined(INSTALL_USING_DEPS) || make(instdep_all)

.if make(instdep_all)
_INSTDEPFILES2_	= ${_INSTALLRELDIR_}${_INSTDEP_TARGET}
.else
_INSTDEPFILES2_	= ${_INSTALLRELDIR_}${_INSTALL_TARGET}
.endif # make(instdep_all)
_INSTDEPFILES_	= ${_IDIR_:@_IDIR_@${_INSTDEPFILES2_:xs|[/\\]+|/|ge}@}

.if make(instdep_all)
  _INSTDEP_TARGETS_INSTALL_	= ${_INSTDEP_TARGETS_:Minstdep_*}
.else
  _INSTALL_TARGETS_INSTDEP_	= ${_INSTALL_TARGETS_:Minstdep_*}
.endif # make(instdep_all)

.if !empty(_INSTDEP_TARGETS_INSTALL_) || !empty(_INSTALL_TARGETS_INSTDEP_)

.if make(instdep_all)
${_INSTDEP_TARGETS_INSTALL_}	: $${_INSTDEPFILES_} .SPECTARG;@
.else
${_INSTALL_TARGETS_INSTDEP_}	: $${_INSTDEPFILES_} .SPECTARG;@
.endif # make(instdep_all)


.if make(instdep_all)
${_INSTDEP_TARGETS_INSTALL_:S/^instdep_//g:@_INSTDEP_TARGET@${_INSTDEPFILES_}@}: $${.TARGET:T}
.else
${_INSTALL_TARGETS_INSTDEP_:S/^instdep_//g:@_INSTALL_TARGET@${_INSTDEPFILES_}@}: $${.TARGET:T}
.endif # make(instdep_all)
	${_TARGET_RELDIR_:C-}
.if defined(INSTALL_USING_CRLFCON)
	${_CRLFCON_} ${_CRLFFLAGS_} ${_ALLSRC_OSPATH_} ${_TARGET_OSPATH_}
.else
	${CP} ${_CPFLAGS_} ${_ALLSRC_OSPATH_} ${_TARGET_OSRELDIR_}
.endif # defined(INSTALL_USING_CRLFCON)
.if defined(UNIX)
.ifndef NO_CHMOD
.if make(instdep_all)
	${CHMOD} ${${_INSTDEP_TARGET}_IMODE:U${IMODE}} ${_TARGET_OSPATH_}
.else
	${CHMOD} ${${_INSTALL_TARGET}_IMODE:U${IMODE}} ${_TARGET_OSPATH_}
.endif # make(instdep_all)
.endif # NO_CHMOD
.ifndef NO_CHOWN
.if make(instdep_all)
	${CHOWN} ${${_INSTDEP_TARGET}_IOWNER:U${IOWNER}}:${${_INSTDEP_TARGET}_IGROUP:U${IGROUP}} \
	   ${_TARGET_OSPATH_}
.else
	${CHOWN} ${${_INSTALL_TARGET}_IOWNER:U${IOWNER}}:${${_INSTALL_TARGET}_IGROUP:U${IGROUP}} \
	   ${_TARGET_OSPATH_}
.endif # make(instdep_all)
.endif # NO_CHOWN
.endif # defined(UNIX)

.endif # !empty(_INSTDEP_TARGETS_INSTALL_) || !empty(_INSTALL_TARGETS_INSTDEP_)


.else  # !defined(INSTALL_USING_DEPS) or !make(instdep_all)


.if !empty(_INSTALL_TARGETS_:Minstall_*)

${_INSTALL_TARGETS_:Minstall_*}: .SPECTARG
	${_INSTALLABSDIR_:C-}
.if defined(INSTALL_USING_CRLFCON)
	${_IDIRINSTALLTARGET_:@_IDIRINSTALLTARGET_@${DUMMY:!${_CRLFCON_} \
    ${_CRLFFLAGS_} ${INST_QUOTE}${_INSTALL_TARGET:p}${INST_QUOTE} \
    ${_IDIRINSTALLTARGET_}!e}@:b}
.else
.if !empty(MACHINE:M*_openvms_*)
	${_IDIR_:@_IDIR_@${DUMMY:!${CP} ${_CPFLAGS_} \
    ${_INSTALL_TARGET:p:@.PATH.@${VMS_PATH_CONV}@} \
    ${${_TOSTAGE_}${_IDIR_}:L:@.PATH.@${VMS_PATH_CONV}@}!e}@:b}
.elif ${BACKSLASH_PATHS}
	${_IDIR_:@_IDIR_@${DUMMY:!${CP} ${_CPFLAGS_} \
    ${INST_QUOTE}${_INSTALL_TARGET:p:S|/|\\|g}${INST_QUOTE} \
    ${_TOSTAGE_:S|/|\\|g}${_IDIR_:S|/|\\|g}.!e}@:b}
.else
	${_IDIR_:@_IDIR_@${DUMMY:!${CP} ${_CPFLAGS_} \
    ${INST_QUOTE}${_INSTALL_TARGET:p}${INST_QUOTE} \
    ${_TOSTAGE_}${_IDIR_}.!e}@:b}
.endif
.endif # defined(INSTALL_USING_CRLFCON)
.ifdef UNIX
.ifndef NO_CHMOD
	${CHMOD} ${${_INSTALL_TARGET}_IMODE:U${IMODE}} ${_IDIRINSTALLTARGET_}
.endif
.ifndef NO_CHOWN
	${CHOWN} ${${_INSTALL_TARGET}_IOWNER:U${IOWNER}}:${${_INSTALL_TARGET}_IGROUP:U${IGROUP}} ${_IDIRINSTALLTARGET_}
.endif
.endif # UNIX

.endif # !empty(_INSTALL_TARGETS_:Minstall_*)

.endif # defined(INSTALL_USING_DEPS) or make(instdep_all)

.endif

help::
.if defined(INSTALL_USING_DEPS) || make(instdep_all)
	@${ECHO} Target: instdep_all - Places updated files in a directory they will occupy in an installed system.
.else
	@${ECHO} Target: install_all - Places files in a directory they will occupy in an installed system.
.endif # defined(INSTALL_USING_DEPS)

