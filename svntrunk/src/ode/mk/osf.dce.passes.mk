#
# @OSF_FREE_COPYRIGHT@
# COPYRIGHT NOTICE
# Copyright (c) 1992, 1991, 1990  
# Open Software Foundation, Inc. 
#  
# Permission is hereby granted to use, copy, modify and freely distribute 
# the software in this file and its documentation for any purpose without 
# fee, provided that the above copyright notice appears in all copies and 
# that both the copyright notice and this permission notice appear in 
# supporting documentation.  Further, provided that the name of Open 
# Software Foundation, Inc. ("OSF") not be used in advertising or 
# publicity pertaining to distribution of the software without prior 
# written permission from OSF.  OSF makes no representations about the 
# suitability of this software for any purpose.  It is provided "as is" 
# without express or implied warranty. 
#
#
# HISTORY
# $Log: osf.dce.passes.mk,v $
# Revision 1.2.2.2  1992/12/03  17:25:53  damon
# 	ODE 2.2 CR 183. Added CMU notice
# 	[1992/12/03  17:10:56  damon]
#
# Revision 1.2  1991/12/05  21:17:36  devrcs
# 	Changes for Reno make
# 	[91/03/22  16:35:44  mckeen]
# 
# $EndLog$

.if !defined(_OSF_DCE_PASSES_MK_)
_OSF_DCE_PASSES_MK_=

#
# Start of PASSES
#

#
#  These list the "tags" associated with each pass
#
_PASS_FIRST_TAGS_=EXPINC
_PASS_SECOND_TAGS_=EXPBIN
_PASS_THIRD_TAGS_=EXPLIB
_PASS_BASIC_TAGS_=STANDARD

#
#  These list the variables used to define subdirectories to recurse into
#
_EXPINC_SUBDIRS_=${EXPINC_SUBDIRS}
_EXPBIN_SUBDIRS_=${EXPBIN_SUBDIRS}
_EXPLIB_SUBDIRS_=${EXPLIB_SUBDIRS}
_STANDARD_SUBDIRS_=${SUBDIRS}

#
#  For each ACTION define the action for recursion, the passes for the
#  action, the targets for the complete action, and the targets for each
#  pass of the action
#
.if defined(MAKEFILE_PASS)
_BUILD_PASSES_=${MAKEFILE_PASS}
.else
_BUILD_PASSES_=FIRST SECOND THIRD BASIC
.endif

_BUILD_ACTION_=build
_build_action_=BUILD

_BUILD_EXPINC_TARGETS_=\
	${_EXPORT_EXPINC_TARGETS_}
_BUILD_EXPBIN_TARGETS_=\
	${_EXPORT_EXPBIN_TARGETS_}
_BUILD_EXPLIB_TARGETS_=\
	${_EXPORT_EXPLIB_TARGETS_}
_BUILD_STANDARD_TARGETS_=\
	${_COMP_STANDARD_TARGETS_}

.if defined(MAKEFILE_PASS)
_COMP_PASSES_=${MAKEFILE_PASS}
.else
_COMP_PASSES_=BASIC
.endif

_COMP_ACTION_=comp
_comp_action_=COMP

_COMP_STANDARD_TARGETS_=\
	${PROGRAMS} \
	${LIBRARIES} \
	${OBJECTS} \
	${SCRIPTS} \
	${DATAFILES} \
	${OTHERS} \
	${MANPAGES:=.0} \
	${DOCUMENTS:S/$/.ps/g} \
	${DOCUMENTS:S/$/.out/g}

.if defined(MAKEFILE_PASS)
_CLEAN_PASSES_=${MAKEFILE_PASS}
.else
_CLEAN_PASSES_=BASIC
.endif

_CLEAN_TARGETS_=${_clean_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
_CLEAN_ACTION_=clean
_clean_action_=CLEAN

_CLEAN_STANDARD_TARGETS_=\
	${PROGRAMS:S/^/clean_/g} \
	${LIBRARIES:S/^/clean_/g} \
	${OBJECTS:S/^/clean_/g} \
	${SCRIPTS:S/^/clean_/g} \
	${DATAFILES:S/^/clean_/g} \
	${OTHERS:S/^/clean_/g} \
	${MANPAGES:S/^/clean_/g:S/$/.0/g} \
	${DOCUMENTS:S/^/clean_/g:S/$/.ps/g} \
	${DOCUMENTS:S/^/clean_/g:S/$/.out/g}

.if defined(MAKEFILE_PASS)
_RMTARGET_PASSES_=${MAKEFILE_PASS}
.else
_RMTARGET_PASSES_=BASIC
.endif

_RMTARGET_TARGETS_=${_rmtarget_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
_RMTARGET_ACTION_=rmtarget
_rmtarget_action_=RMTARGET

_RMTARGET_STANDARD_TARGETS_=\
	${PROGRAMS:S/^/rmtarget_/g} \
	${LIBRARIES:S/^/rmtarget_/g} \
	${OBJECTS:S/^/rmtarget_/g} \
	${SCRIPTS:S/^/rmtarget_/g} \
	${DATAFILES:S/^/rmtarget_/g} \
	${OTHERS:S/^/rmtarget_/g} \
	${MANPAGES:S/^/rmtarget_/g:S/$/.0/g} \
	${DOCUMENTS:S/^/rmtarget_/g:S/$/.ps/g} \
	${DOCUMENTS:S/^/rmtarget_/g:S/$/.out/g}

.if defined(MAKEFILE_PASS)
_CLOBBER_PASSES_=${MAKEFILE_PASS}
.else
_CLOBBER_PASSES_=BASIC
.endif

_CLOBBER_TARGETS_=${_clobber_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
_CLOBBER_ACTION_=clobber
_clobber_action_=CLOBBER

_CLOBBER_STANDARD_TARGETS_=\
	${PROGRAMS:S/^/clobber_/g} \
	${LIBRARIES:S/^/clobber_/g} \
	${OBJECTS:S/^/clobber_/g} \
	${SCRIPTS:S/^/clobber_/g} \
	${DATAFILES:S/^/clobber_/g} \
	${OTHERS:S/^/clobber_/g} \
	${MANPAGES:S/^/clobber_/g:S/$/.0/g} \
	${DOCUMENTS:S/^/clobber_/g:S/$/.ps/g} \
	${DOCUMENTS:S/^/clobber_/g:S/$/.out/g}

.if defined(MAKEFILE_PASS)
_LINT_PASSES_=${MAKEFILE_PASS}
.else
_LINT_PASSES_=BASIC
.endif

_LINT_TARGETS_=${_lint_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
_LINT_ACTION_=lint
_lint_action_=LINT

_LINT_STANDARD_TARGETS_=\
	${PROGRAMS:S/^/lint_/g} \
	${LIBRARIES:S/^/lint_/g} \
	${OBJECTS:S/^/lint_/g}

.if defined(MAKEFILE_PASS)
_TAGS_PASSES_=${MAKEFILE_PASS}
.else
_TAGS_PASSES_=BASIC
.endif

_TAGS_TARGETS_=${_tags_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
_TAGS_ACTION_=tags
_tags_action_=TAGS

_TAGS_STANDARD_TARGETS_=\
	${PROGRAMS:S/^/tags_/g} \
	${LIBRARIES:S/^/tags_/g} \
	${OBJECTS:S/^/tags_/g}

.if defined(MAKEFILE_PASS)
_EXPORT_PASSES_=${MAKEFILE_PASS}
.else
_EXPORT_PASSES_=FIRST SECOND THIRD
.endif

_EXPORT_TARGETS_=${_export_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
_EXPORT_ACTION_=export
_export_action_=EXPORT

.if defined(EXPINC_TARGETS)
_EXPORT_EXPINC_TARGETS_=\
	${EXPINC_TARGETS}
.else
_EXPORT_EXPINC_TARGETS_=\
	${INCLUDES:S/^/export_/g}
.endif
_EXPORT_EXPBIN_TARGETS_=\
	${EXPBIN_TARGETS:S/^/export_/g}
.if defined(EXPLIB_TARGETS)
_EXPORT_EXPLIB_TARGETS_=\
	${EXPLIB_TARGETS}
.else
_EXPORT_EXPLIB_TARGETS_=\
	${LIBRARIES:S/^/export_/g}
.endif

.if defined(MAKEFILE_PASS)
_INSTALL_PASSES_=${MAKEFILE_PASS}
.else
_INSTALL_PASSES_=BASIC
.endif

_INSTALL_TARGETS_=${_install_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
_INSTALL_ACTION_=install
_install_action_=INSTALL

_INSTALL_STANDARD_TARGETS_=\
	${ILIST:S/^/install_/g}

#
#  Magic begins here...
#
#  This sequence of indirect macros basically performs the following
#  expansion inline to generate the correct dependents for each action
#
#  foreach pass (${_${action}_PASSES_})
#     foreach tag (${_PASS_${pass}_TAGS_})
#        foreach subdir (${_${tag}_SUBDIRS_})
#           _SUBDIR/${action}/${pass}/${tag}/${subdir}
#        end
#     foreach tag (${_PASS_${pass}_TAGS_})
#        ${_${action}_${tag}_TARGETS_}
#     end
#  end
#

_ALL_ACTIONS_=BUILD COMP CLEAN RMTARGET CLOBBER LINT TAGS EXPORT INSTALL

_SUBDIR_TAGS_=${_${.TAG.}_SUBDIRS_:S;^;_SUBDIR/${.ACTION.}/${.PASS.}/${.TAG.}/;g}
_SUBDIR_PASSES_=${_PASS_${.PASS.}_TAGS_:@.TAG.@${_SUBDIR_TAGS_}@}
_TARGET_TAGS_=${_PASS_${.PASS.}_TAGS_:@.TAG.@${_${.ACTION.}_${.TAG.}_TARGETS_}@}
_PASS_ACTIONS_=${_${.ACTION.}_PASSES_:@.PASS.@${_SUBDIR_PASSES_} ${_TARGET_TAGS_}@}
_TARGET_ACTIONS_=${_${.ACTION.}_PASSES_:@.PASS.@${_TARGET_TAGS_}@}
_SUBDIR_ACTIONS_=${_${.ACTION.}_PASSES_:@.PASS.@${_SUBDIR_PASSES_}@}

_all_targets_=${_${.TARGET:S;_all$;;}_action_:@.ACTION.@${_PASS_ACTIONS_}@}

#
#  subdir recursion rule
#
#  This expands into targets matching the following pattern
#
#  _SUBDIR/<action>/<pass>/<tag>/<subdir>
#
${_ALL_ACTIONS_:@.ACTION.@${_SUBDIR_ACTIONS_}@}:
	@echo "[ ${MAKEDIR:/=}/${.TARGET:T} ]"
	makepath ${.TARGET:T}/. && cd ${.TARGET:T} && \
	exec ${MAKE} MAKEFILE_PASS=${.TARGET:H:H:T} \
	      ${_${.TARGET:H:H:H:T}_ACTION_}_all \

#
#  Special tag passes
#
.if !defined(EXPORTBASE)

export_all_EXPINC: ${ALWAYS}
	@echo "You must define EXPORTBASE to do an ${.TARGET}"

export_all_EXPLIB: ${ALWAYS}
	@echo "You must define EXPORTBASE to do an ${.TARGET}"

.else

_X=${_${.ACTION.}_PASSES_:MFIRST:@.PASS.@${_SUBDIR_PASSES_} ${_TARGET_TAGS_}@}
_Y=${_${.ACTION.}_PASSES_:MTHIRD:@.PASS.@${_SUBDIR_PASSES_} ${_TARGET_TAGS_}@}

export_all_EXPINC: ${_ALL_ACTIONS_:MEXPORT:@.ACTION.@${_X}@}

export_all_EXPLIB: ${_ALL_ACTIONS_:MEXPORT:@.ACTION.@${_Y}@}

.endif

#
# End of PASSES
#

.endif
