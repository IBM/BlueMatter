################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines the passes and actions rules.
#  Included makefiles:
#    bld.passes.mk
#    pkg.passes.mk
#
#  Some variables used:
#    SPECIAL_PASSES
#	 If defined, then the rules to support the .PASSES special source will
#	 be used
#
################################################################################
.if !defined(__PASSES_MK_)
__PASSES_MK_=

# Reinit the _ALL_ACTIONS_ variable to be nothing. (not needed for new passes)
_ALL_ACTIONS_= ${EXTRA_ACTIONS}

# Include the different kinds of passes.
.if !empty(MACHINE:M*_openvms_*)
.include <bld_passes.mk>
.else
.include <bld.passes.mk>
.endif

.ifndef OMIT_PACKAGING_RULES
.if !empty(MACHINE:M*_openvms_*)
.include <pkg_passes.mk>
.else
.include <pkg.passes.mk>
.endif
.endif

# Include the project passes rules
.if defined(PROJECT_NAME)
.if !empty(MACHINE:M*_openvms_*)
.tryinclude <${PROJECT_NAME}_passes.mk>
.else
.tryinclude <${PROJECT_NAME}.passes.mk>
.endif
.endif

.if defined(SPECIAL_PASSES)
# This the only remains needed from the old passes rules
_TARGET_PASSES_=${_${MAKEACTION}_${MAKEPASS}_TARGETS_}
_TARGET_ACTIONS_=${_${MAKEACTION}_PASSES_}

.else
# The following is PASSES implemented with rules

#
#  Magic begins here...
#
#  This sequence of indirect macros basically performs the following
#  expansion inline to generate the correct dependents for each action
#
#  foreach pass (${_${action}_PASSES_})
#     foreach subdir (${_${pass}_SUBDIRS_})
#	 _SUBDIR/${action}/${pass}/${subdir}
#     end
#     ${_${action}_${pass}_TARGETS_}
#  end
#

_SUBDIR_PASSES_=${_${.PASS.}_SUBDIRS_:S;^;_SUBDIR/${.ACTION.}/${.PASS.}/;g}
_TARGET_PASSES_=${_${.ACTION.}_${.PASS.}_TARGETS_}
_PASS_ACTIONS_=${_${.ACTION.}_PASSES_:@.PASS.@${_SUBDIR_PASSES_} _TARGET/${.ACTION.}/${.PASS.}@}
_TARGET_ACTIONS_=${_${.ACTION.}_PASSES_:@.PASS.@${_TARGET_PASSES_}@}
_SUBDIR_ACTIONS_=${_${.ACTION.}_PASSES_:@.PASS.@${_SUBDIR_PASSES_}@}

_all_targets_=${_${.TARGET:S;_all$;;}_action_:@.ACTION.@${_PASS_ACTIONS_}@}

#
# If the user wishes to build subdirectories in parallel, do two things:
#
# 1. The .ORDER statements will NOT include the _SUBDIR/action/pass/subdir
#    targets.  This means there will no longer be any order imposed on
#    the subdirectories.
# 2. To make sure the subdirectories for each pass are traversed before the
#    targets in the current directory are built, we place dependencies from
#    the _TARGET targets to the _SUBDIR targets.
#
.ifdef DIRS_IN_PARALLEL
_ORDERED_PASS_ACTIONS_=${_${.ACTION.}_PASSES_:@.PASS.@_TARGET/${.ACTION.}/${.PASS.}@}
_ALL_ACTION_PASS_TARGETS_ = ${_ALL_ACTIONS_:@.ACTION.@${_PASS_ACTIONS_}@}
_ACTION_PASS_MATCH_       = ${.TARGET:S|^_TARGET/||g}
${_ALL_ACTION_PASS_TARGETS_:M_TARGET/*} : .SPECTARG \
    $${_ALL_ACTION_PASS_TARGETS_:M_SUBDIR/$${_ACTION_PASS_MATCH_}/*}
.else
_ORDERED_PASS_ACTIONS_=${_PASS_ACTIONS_}
.endif

#  Order the high-level pass information so that we do not make
#  things on the next pass until this pass is finished.
#  We need to only order actions that have multiple passes
#  An example of this, is when in a leaf directory (no subdirectories) and
#   there are header files to export before a file is compiled.

# For building
.ORDER: ${BUILD:L:@.ACTION.@${_ORDERED_PASS_ACTIONS_}@}
.ORDER: ${CLEAN:L:@.ACTION.@${_ORDERED_PASS_ACTIONS_}@}
.ORDER: ${CLOBBER:L:@.ACTION.@${_ORDERED_PASS_ACTIONS_}@}
.ORDER: ${COMP:L:@.ACTION.@${_ORDERED_PASS_ACTIONS_}@}
.ORDER: ${DEPEND:L:@.ACTION.@${_ORDERED_PASS_ACTIONS_}@}
.ORDER: ${EXPORT:L:@.ACTION.@${_ORDERED_PASS_ACTIONS_}@}
.ORDER: ${INSTALL:L:@.ACTION.@${_ORDERED_PASS_ACTIONS_}@}
.ORDER: ${INSTDEP:L:@.ACTION.@${_ORDERED_PASS_ACTIONS_}@}
.ORDER: ${JAVADOC:L:@.ACTION.@${_ORDERED_PASS_ACTIONS_}@}
.ORDER: ${RMTARGET:L:@.ACTION.@${_ORDERED_PASS_ACTIONS_}@}
.ORDER: ${SETUP:L:@.ACTION.@${_ORDERED_PASS_ACTIONS_}@}
# For packaging
.ORDER: ${GATHER:L:@.ACTION.@${_ORDERED_PASS_ACTIONS_}@}
.ORDER: ${INSTALLMVS:L:@.ACTION.@${_ORDERED_PASS_ACTIONS_}@}
.ORDER: ${PARSE:L:@.ACTION.@${_ORDERED_PASS_ACTIONS_}@}
.ORDER: ${RUNPKGTOOL:L:@.ACTION.@${_ORDERED_PASS_ACTIONS_}@}

_TARGET_ACTS_=${_${.ACTION.}_PASSES_:@.PASS.@_TARGET/${.ACTION.}/${.PASS.}@}
_TARGET_DEPS_=${.TARGET:H:T:@.ACTION.@${.TARGET:T:@.PASS.@${_TARGET_PASSES_}@}@}

${_ALL_ACTIONS_:@.ACTION.@${_TARGET_ACTS_}@}: $${_TARGET_DEPS_} .SPECTARG

#
#  subdir recursion rule
#
#  This expands into false targets matching the following pattern
#
#  _SUBDIR/<action>/<pass>/<subdir>
#

SUBDIR_ENTER_STRING ?= [ ${MAKEDIR:S|/$||g}/${.TARGET:T:S|;|/|g} ] (entering)
SUBDIR_LEAVE_STRING ?= [ ${MAKEDIR:S|/$||g}/${.TARGET:T:S|;|/|g} ] (returned from)

_SUBDIR_RECURSION_ = ${_ALL_ACTIONS_:@.ACTION.@${_SUBDIR_ACTIONS_}@}
.if !empty(_SUBDIR_RECURSION_)
.ifdef DIRS_IN_PARALLEL
${_SUBDIR_RECURSION_}: .SPECTARG .PMAKE
.else
${_SUBDIR_RECURSION_}: .SPECTARG .MAKE
.endif
	${SUBDIR_ENTER_STRING:a&STDOUT}
	${.TARGET:T:S|;|/|g:C}
.if !empty(MACHINE:M*_openvms_*)
	${CHDIR} ${.TARGET:T:S|;|/|g:@.PATH.@${VMS_PATH_CONV}@} ${CMDSEP} \
	   ${MAKE:T:R} "MAKEFILE_PASS=${.TARGET:H:T}" \
	  ${_${.TARGET:H:H:T}_ACTION_}_all ${MAKE_LOGGER}
.elif ${BACKSLASH_PATHS}
	${CHDIR} .\${.TARGET:T:S|;|\\|g} ${CMDSEP} \
	  ${MAKE} MAKEFILE_PASS=${.TARGET:H:T} \
	  ${_${.TARGET:H:H:T}_ACTION_}_all ${MAKE_LOGGER}
.else
	${CHDIR} ${.TARGET:T:S|;|/|g} ${CMDSEP} \
	   ${MAKE} MAKEFILE_PASS=${.TARGET:H:T} \
	  ${_${.TARGET:H:H:T}_ACTION_}_all ${MAKE_LOGGER}
.endif # !defined(UNIX)
	${SUBDIR_LEAVE_STRING:a&STDOUT}
.endif # !empty(_SUBDIR_RECURSION_)

#
# End of PASSES
#

.endif # SPECIAL_PASSES

.endif
