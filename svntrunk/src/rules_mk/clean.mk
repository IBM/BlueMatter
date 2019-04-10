################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines clean rules.
#  Targets defined:
#    clean_all
#    rmtarget_all
#    clobber_all
#  Some variables NOT used:
#    RM - the command which deletes files (del, rm, etc.)
#    _RMFLAGS_ - flags for the RM command
#    Instead, the :rm- variable modifier is used to erase files.
#
################################################################################
.if !defined(__CLEAN_MK_)
__CLEAN_MK_=

#
#  Clean up rules
#
.if defined(SPECIAL_PASSES)
clean_all: CLEAN .SPECTARG
.else
clean_all: $${_all_targets_} .SPECTARG
.endif # SPECIAL_PASSES
.if defined(UNIX)
	${Erasing core:L:a&STDOUT}
	-${core:L:rm-}
.endif # UNIX

.if !empty(_CLEAN_TARGETS_:Mclean_*)
${_CLEAN_TARGETS_:Mclean_*}: .SPECTARG
	${Erasing ${_CLEANFILES_}:L:a&STDOUT}
	${_CLEANFILES_:rm-}
.endif

.if defined(SPECIAL_PASSES)
rmtarget_all: RMTARGET .SPECTARG
.else
rmtarget_all: $${_all_targets_} .SPECTARG
.endif # SPECIAL_PASSES
.if defined(UNIX)
	${Erasing core:L:a&STDOUT}
	-${core:rm-}
.endif # UNIX

.if !empty(_RMTARGET_TARGETS_:Mrmtarget_*)
${_RMTARGET_TARGETS_:Mrmtarget_*}: .SPECTARG
	${Erasing ${_RMTARGET_TARGET}:L:a&STDOUT}
	${_RMTARGET_TARGET:rm-}
.endif # empty(_RMTARGET_TARGETS_:Mrmtarget_*)

.if defined(SPECIAL_PASSES)
clobber_all: CLOBBER .SPECTARG
.else
clobber_all: $${_all_targets_} .SPECTARG
.endif # SPECIAL_PASSES
.if defined(UNIX)
	${Erasing core:L:a&STDOUT}
	${core:L:rm-}
.endif # UNIX
	${Erasing depend.mk:L:a&STDOUT}
	${depend.mk:L:rm-}

.if !empty(_CLOBBER_TARGETS_:Mclobber_*)
${_CLOBBER_TARGETS_:Mclobber_*}: .SPECTARG
	${Erasing ${_CLOBBERFILES_}:L:a&STDOUT}
	${_CLOBBERFILES_:rm-}
.endif # empty(_CLOBBER_TARGETS_:Mclobber_*)

help::
	@${ECHO} Target: rmtarget_all - Removes all targets \
		[excluding OFILES dependents] except include headers.
	@${ECHO} Target: clean_all    - Removes all targets \
		[including OFILES dependents] except include headers.
	@${ECHO} Target: clobber_all  - Removes all targets \
		[including OFILES dependents and include headers].

.endif # __CLEAN_MK

