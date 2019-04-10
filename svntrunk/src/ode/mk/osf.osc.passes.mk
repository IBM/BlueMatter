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
# $Log: osf.osc.passes.mk,v $
# Revision 1.2.4.3  1992/12/03  17:26:27  damon
# 	ODE 2.2 CR 183. Added CMU notice
# 	[1992/12/03  17:11:16  damon]
#
# Revision 1.2.4.2  1992/06/16  13:33:56  damon
# 	Synched with OSF/1 1.1 latest
# 	[1992/06/16  13:26:22  damon]
# 
# Revision 1.2.2.2  1992/03/18  17:03:59  damon
# 	Synched with OSF/1 1.1 latest
# 	[1992/03/18  17:03:38  damon]
# 
# Revision 1.2  1991/12/05  21:18:08  devrcs
# 	Incorporated changes from osc
# 	[91/04/25  13:27:53  mckeen]
# 
# 	Initial version for osc
# 	[91/04/21  20:15:29  damon]
# 
# $EndLog$

.if !defined(_OSF_OSC_PASSES_MK_)
_OSF_OSC_PASSES_MK_=

#
# Start of PASSES
#

#
#  These list the "tags" associated with each pass
#
_PASS_SETUP_TAGS_=SETUP
_PASS_FIRST_TAGS_=EXPINC
_PASS_SECOND_TAGS_=EXPLIBC EXPCRT0 EXPLIBLD
.if defined(NO_SHARED_LIBRARIES)
_PASS_THIRD_TAGS_=
_PASS_FOURTH_TAGS_=
.else
_PASS_THIRD_TAGS_=EXPLOADER
_PASS_FOURTH_TAGS_=EXPSHLIB
.endif
_PASS_FIFTH_TAGS_=EXPLIB
_PASS_COMPILE_TAGS_=COMP
.if defined(SECURITY_ONLY)
_PASS_BASIC_TAGS_=SECURITY
.else
_PASS_BASIC_TAGS_=STANDARD
.endif

#
#  These list the variables used to define subdirectories to recurse into
#
_EXPINC_SUBDIRS_=${EXPINC_SUBDIRS}
_EXPLIBC_SUBDIRS_=${EXPLIBC_SUBDIRS}
_EXPCRT0_SUBDIRS_=${EXPCRT0_SUBDIRS}
_EXPLIBLD_SUBDIRS_=${EXPLIBLD_SUBDIRS}
_EXPLOADER_SUBDIRS_=${EXPLOADER_SUBDIRS}
_EXPSHLIB_SUBDIRS_=${EXPSHLIB_SUBDIRS}
_EXPLIB_SUBDIRS_=${EXPLIB_SUBDIRS}
_SECURITY_SUBDIRS_=${SEC_SUBDIRS}
_SETUP_SUBDIRS_=${SETUP_SUBDIRS}
_COMP_SUBDIRS_=${COMP_SUBDIRS:U${SUBDIRS}}
.if (${.TARGETS} == "tdinst_all")
    _STANDARD_SUBDIRS_=${TDINST_SUBDIRS}
.elif (${.TARGETS} == "install_all") && defined(INSTALL_SUBDIRS)
    _STANDARD_SUBDIRS_=${INSTALL_SUBDIRS}
.else
    _STANDARD_SUBDIRS_=${SUBDIRS} 
.endif

#
#  For each ACTION define the action for recursion, the passes for the
#  action, the targets for the complete action, and the targets for each
#  pass of the action
#
.if defined(MAKEFILE_PASS)
_BUILD_PASSES_=${MAKEFILE_PASS}
.else
_BUILD_PASSES_=FIRST
_BUILD_PASSES_+=SECOND
.if !defined(NO_SHARED_LIBRARIES)
_BUILD_PASSES_+=THIRD FOURTH
.endif
_BUILD_PASSES_+=FIFTH COMPILE
.endif

_BUILD_ACTION_=build
_build_action_=BUILD

_BUILD_EXPINC_TARGETS_=\
	${_EXPORT_EXPINC_TARGETS_}
_BUILD_EXPLIBC_TARGETS_=\
	${_EXPORT_EXPLIBC_TARGETS_}
_BUILD_EXPCRT0_TARGETS_=\
	${_EXPORT_EXPCRT0_TARGETS_}
_BUILD_EXPLIBLD_TARGETS_=\
	${_EXPORT_EXPLIBLD_TARGETS_}
_BUILD_EXPLOADER_TARGETS_=\
	${_EXPORT_EXPLOADER_TARGETS_}
_BUILD_EXPSHLIB_TARGETS_=\
	${_EXPORT_EXPSHLIB_TARGETS_}
_BUILD_EXPLIB_TARGETS_=\
	${_EXPORT_EXPLIB_TARGETS_}
_BUILD_SECURITY_TARGETS_=\
	${_COMP_SECURITY_TARGETS_}
_BUILD_COMP_TARGETS_=\
	${_COMP_STANDARD_TARGETS_}
_BUILD_STANDARD_TARGETS_=\
	${_COMP_STANDARD_TARGETS_}

.if defined(MAKEFILE_PASS)
_COMP_PASSES_=${MAKEFILE_PASS}
.else
_COMP_PASSES_=COMPILE
.endif

_COMP_ACTION_=comp
_comp_action_=COMP

_COMP_SECURITY_TARGETS_=\
	${SEC_PROGRAMS} \
	${SEC_LIBRARIES} \
	${SEC_OBJECTS} \
	${SEC_SCRIPTS} \
	${SEC_DATAFILES} \
	${SEC_OTHERS} \
	${SEC_MANPAGES:=.0} \
	${SEC_DOCUMENTS:S/$/.ps/g} \
	${SEC_DOCUMENTS:S/$/.out/g}
_COMP_STANDARD_TARGETS_=\
	${PROGRAMS} \
	${LIBRARIES} \
	${OBJECTS} \
	${SCRIPTS} \
	${DATAFILES} \
	${OTHERS} \
	${MANPAGES:=.0} \
	${DOCUMENTS:S/$/.ps/g} \
	${DOCUMENTS:S/$/.out/g} \
	${TDINSTFILES} \
	${TDOUTFILES}
_COMP_COMP_TARGETS_=${_COMP_STANDARD_TARGETS_}

.if defined(MAKEFILE_PASS)
_CLEAN_PASSES_=${MAKEFILE_PASS}
.else
_CLEAN_PASSES_=BASIC
.endif

_CLEAN_TARGETS_=${_clean_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
_CLEAN_ACTION_=clean
_clean_action_=CLEAN

_CLEAN_SECURITY_TARGETS_=\
	${SEC_PROGRAMS:S/^/clean_/g} \
	${SEC_LIBRARIES:S/^/clean_/g} \
	${SEC_OBJECTS:S/^/clean_/g} \
	${SEC_SCRIPTS:S/^/clean_/g} \
	${SEC_OTHERS:S/^/clean_/g} \
	${SEC_MANPAGES:S/^/clean_/g:S/$/.0/g} \
	${SEC_DOCUMENTS:S/^/clean_/g:S/$/.ps/g} \
	${SEC_DOCUMENTS:S/^/clean_/g:S/$/.out/g}
_CLEAN_STANDARD_TARGETS_=\
	${PROGRAMS:S/^/clean_/g} \
	${LIBRARIES:S/^/clean_/g} \
	${OBJECTS:S/^/clean_/g} \
	${SCRIPTS:S/^/clean_/g} \
	${OTHERS:S/^/clean_/g} \
	${MANPAGES:S/^/clean_/g:S/$/.0/g} \
	${DOCUMENTS:S/^/clean_/g:S/$/.ps/g} \
	${DOCUMENTS:S/^/clean_/g:S/$/.out/g} \
	${TDINSTFILES:S/^/clean_/g} \
	${TDOUTFILES:S/^/clean_/g}

.if defined(MAKEFILE_PASS)
_RMTARGET_PASSES_=${MAKEFILE_PASS}
.else
_RMTARGET_PASSES_=BASIC
.endif

_RMTARGET_TARGETS_=${_rmtarget_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
_RMTARGET_ACTION_=rmtarget
_rmtarget_action_=RMTARGET

_RMTARGET_SECURITY_TARGETS_=\
	${SEC_PROGRAMS:S/^/rmtarget_/g} \
	${SEC_LIBRARIES:S/^/rmtarget_/g} \
	${SEC_OBJECTS:S/^/rmtarget_/g} \
	${SEC_SCRIPTS:S/^/rmtarget_/g} \
	${SEC_OTHERS:S/^/rmtarget_/g} \
	${SEC_MANPAGES:S/^/rmtarget_/g:S/$/.0/g} \
	${SEC_DOCUMENTS:S/^/rmtarget_/g:S/$/.ps/g} \
	${SEC_DOCUMENTS:S/^/rmtarget_/g:S/$/.out/g}
_RMTARGET_STANDARD_TARGETS_=\
	${PROGRAMS:S/^/rmtarget_/g} \
	${LIBRARIES:S/^/rmtarget_/g} \
	${OBJECTS:S/^/rmtarget_/g} \
	${SCRIPTS:S/^/rmtarget_/g} \
	${OTHERS:S/^/rmtarget_/g} \
	${MANPAGES:S/^/rmtarget_/g:S/$/.0/g} \
	${DOCUMENTS:S/^/rmtarget_/g:S/$/.ps/g} \
	${DOCUMENTS:S/^/rmtarget_/g:S/$/.out/g} \
	${TDINSTFILES:S/^/rmtarget_/g} \
	${TDOUTFILES:S/^/rmtarget_/g}

.if defined(MAKEFILE_PASS)
_CLOBBER_PASSES_=${MAKEFILE_PASS}
.else
_CLOBBER_PASSES_=BASIC
.endif

_CLOBBER_TARGETS_=${_clobber_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
_CLOBBER_ACTION_=clobber
_clobber_action_=CLOBBER

_CLOBBER_SECURITY_TARGETS_=\
	${SEC_PROGRAMS:S/^/clobber_/g} \
	${SEC_LIBRARIES:S/^/clobber_/g} \
	${SEC_OBJECTS:S/^/clobber_/g} \
	${SEC_SCRIPTS:S/^/clobber_/g} \
	${SEC_OTHERS:S/^/clobber_/g} \
	${SEC_MANPAGES:S/^/clobber_/g:S/$/.0/g} \
	${SEC_DOCUMENTS:S/^/clobber_/g:S/$/.ps/g} \
	${SEC_DOCUMENTS:S/^/clobber_/g:S/$/.out/g}
_CLOBBER_STANDARD_TARGETS_=\
	${PROGRAMS:S/^/clobber_/g} \
	${LIBRARIES:S/^/clobber_/g} \
	${OBJECTS:S/^/clobber_/g} \
	${SCRIPTS:S/^/clobber_/g} \
	${OTHERS:S/^/clobber_/g} \
	${MANPAGES:S/^/clobber_/g:S/$/.0/g} \
	${DOCUMENTS:S/^/clobber_/g:S/$/.ps/g} \
	${DOCUMENTS:S/^/clobber_/g:S/$/.out/g} \
	${TDINSTFILES:S/^/clobber_/g} \
	${TDOUTFILES:S/^/clobber_/g}

.if defined (MAKEFILE_PASS)
_SETUP_PASSES_=${MAKEFILE_PASS}
.else
_SETUP_PASSES_=SETUP
.endif

_SETUP_TARGETS_=${_setup_action_:@.ACTION.@${_TARGET_ACTIONS_}@} 
_SETUP_ACTION_=setup
_setup_action_=SETUP

_SETUP_SETUP_TARGETS_=\
	${SETUP_PROGRAMS:S/^/setup_/g} \
	${SETUP_SCRIPTS:S/^/setup_/g} \
	${_EXPORT_SETUP_TARGETS_}

.if defined(MAKEFILE_PASS)
_LINT_PASSES_=${MAKEFILE_PASS}
.else
_LINT_PASSES_=BASIC
.endif

_LINT_TARGETS_=${_lint_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
_LINT_ACTION_=lint
_lint_action_=LINT

_LINT_SECURITY_TARGETS_=\
	${SEC_PROGRAMS:S/^/lint_/g} \
	${SEC_LIBRARIES:S/^/lint_/g} \
	${SEC_OBJECTS:S/^/lint_/g}
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

_TAGS_SECURITY_TARGETS_=\
	${SEC_PROGRAMS:S/^/tags_/g} \
	${SEC_LIBRARIES:S/^/tags_/g} \
	${SEC_OBJECTS:S/^/tags_/g}
_TAGS_STANDARD_TARGETS_=\
	${PROGRAMS:S/^/tags_/g} \
	${LIBRARIES:S/^/tags_/g} \
	${OBJECTS:S/^/tags_/g}

.if defined(MAKEFILE_PASS)
_EXPORT_PASSES_=${MAKEFILE_PASS}
.else
_EXPORT_PASSES_=FIRST
_EXPORT_PASSES_+=SECOND
.if !defined(NO_SHARED_LIBRARIES)
_EXPORT_PASSES_+=THIRD FOURTH
.endif
_EXPORT_PASSES_+=FIFTH
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
_EXPORT_SETUP_TARGETS_=\
	${SETUP_INCLUDES:S/^/export_/g}
_EXPORT_EXPLIBC_TARGETS_=\
	${EXPLIBC_TARGETS}
_EXPORT_EXPCRT0_TARGETS_=\
	${EXPCRT0_TARGETS}
_EXPORT_EXPLIBLD_TARGETS_=\
	${EXPLIBLD_TARGETS}
_EXPORT_EXPLOADER_TARGETS_=\
	${EXPLOADER_TARGETS}
_EXPORT_EXPSHLIB_TARGETS_=\
	${EXPSHLIB_TARGETS}
_EXPORT_EXPLIB_TARGETS_=\
	${EXPLIB_TARGETS}

.if defined(MAKEFILE_PASS)
_INSTALL_PASSES_=${MAKEFILE_PASS}
.else
_INSTALL_PASSES_=BASIC
.endif

_INSTALL_TARGETS_=${_install_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
_INSTALL_ACTION_=install
_install_action_=INSTALL

_INSTALL_SECURITY_TARGETS_=\
	${SEC_ILIST:S/^/install_/g}
_INSTALL_STANDARD_TARGETS_=\
	${ILIST:S/^/install_/g}

.if defined(MAKEFILE_PASS)
_TDINST_PASSES_=${MAKEFILE_PASS}
.else
_TDINST_PASSES_=BASIC
.endif

_TDINST_TARGETS_=${_tdinst_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
_TDINST_ACTION_=tdinst
_tdinst_action_=TDINST

_TDINST_STANDARD_TARGETS_=\
	${_COMP_STANDARD_TARGETS_}

#
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

_ALL_ACTIONS_=BUILD COMP CLEAN RMTARGET CLOBBER LINT TAGS EXPORT INSTALL TDINST SETUP

_SUBDIR_TAGS_=${_${.TAG.}_SUBDIRS_:S;^;_SUBDIR/${.ACTION.}/${.PASS.}/${.TAG.}/;g}
_SUBDIR_PASSES_=${_PASS_${.PASS.}_TAGS_:@.TAG.@${_SUBDIR_TAGS_}@}
_TARGET_TAGS_=${_PASS_${.PASS.}_TAGS_:@.TAG.@${_${.ACTION.}_${.TAG.}_TARGETS_}@}
#_PASS_ACTIONS_=${_${.ACTION.}_PASSES_:@.PASS.@${_SUBDIR_PASSES_} ${_TARGET_TAGS_}@}
_PASS_ACTIONS_=${_${.ACTION.}_PASSES_:@.PASS.@${_SUBDIR_PASSES_} _TARGET/${.ACTION.}/${.PASS.}@}
_TARGET_ACTIONS_=${_${.ACTION.}_PASSES_:@.PASS.@${_TARGET_TAGS_}@}
_SUBDIR_ACTIONS_=${_${.ACTION.}_PASSES_:@.PASS.@${_SUBDIR_PASSES_}@}

_all_targets_=${_${.TARGET:S;_all$;;}_action_:@.ACTION.@${_PASS_ACTIONS_}@}

#
#  Order the high-level pass information so that we do not make
#  things on the next pass until this pass is finished.
#
.ORDER: ${BUILD:L:@.ACTION.@${_PASS_ACTIONS_}@}

_TARGET_ACTS_=${_${.ACTION.}_PASSES_:@.PASS.@_TARGET/${.ACTION.}/${.PASS.}@}
_TARGET_DEPS_=${.TARGET:H:T:@.ACTION.@${.TARGET:T:@.PASS.@${_TARGET_TAGS_}@}@}

${_ALL_ACTIONS_:@.ACTION.@${_TARGET_ACTS_}@}: $${_TARGET_DEPS_}

#
#  subdir recursion rule
#
#  This expands into targets matching the following pattern
#
#  _SUBDIR/<action>/<pass>/<tag>/<subdir>
#
.MAKE: ${_ALL_ACTIONS_:@.ACTION.@${_SUBDIR_ACTIONS_}@}

${_ALL_ACTIONS_:@.ACTION.@${_SUBDIR_ACTIONS_}@}:
	@echo "[ ${MAKEDIR:/=}/${.TARGET:T} ]"
	${MAKEPATH} ${MAKEPATHFLAGS} ${.TARGET:T}/. && cd ${.TARGET:T} && \
	exec ${MAKE} MAKEFILE_PASS=${.TARGET:H:H:T} \
	      ${_${.TARGET:H:H:H:T}_ACTION_}_all \

#
# End of PASSES
#

_Z_=${_${.TAG.}_SUBDIRS_}
_Y_=${_PASS_${.PASS.}_TAGS_:@.TAG.@${_Z_}@}
_X_=${_${.ACTION.}_PASSES_:@.PASS.@${_Y_}@}
${_ALL_ACTIONS_:@.ACTION.@${_X_}@}: _SUBDIR/BUILD/BASIC/STANDARD/$${.TARGET};@

.endif
