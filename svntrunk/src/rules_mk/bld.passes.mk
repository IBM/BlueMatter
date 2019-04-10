################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines the build passes and actions.
#  The defined passes are:
#    SETUP
#    JAVADOC
#    EXPINC
#    OBJECTS
#    EXPLIB
#    STANDARD
#  The defined actions are:
#    BUILD
#    COMP
#    CLEAN
#    RMTARGET
#    CLOBBER
#    EXPORT
#    INSTALL
#    INSTDEP
#    SETUP
#    JAVADOC
#  The variables used are:
#    SETUP_SUBDIRS
#    JAVADOC_SUBDIRS
#    EXPINC_SUBDIRS
#    OBJECTS_SUBDIRS
#    EXPLIB_SUBDIRS
#    STANDARD_SUBDIRS
#    MAKEFILE_PASS
#    _<action>_<pass>_TARGETS_
#    _<action>_PASSES_
#
################################################################################
.if !defined(__BLD_PASSES_MK_)
__BLD_PASSES_MK_=

#This is done to accomadate people who have INSTALL_STANDARD_SUBDIRS
#defined but not STANDARD_SUBDIRS or SUBDIRS. The subdirs for instdep_all
#needs to be same as install_all hence the following assignment
.if defined(INSTALL_STANDARD_SUBDIRS)
INSTDEP_STANDARD_SUBDIRS ?= ${INSTALL_STANDARD_SUBDIRS}
.endif

.ifdef CONVERT_JAVA_SUBDIRS
JAVAH_SUBDIRS  ?= ${EXPINC_SUBDIRS}
JAVAC_SUBDIRS  ?= ${OBJECTS_SUBDIRS}
JAR_SUBDIRS    ?= ${EXPLIB_SUBDIRS}
EARWAR_SUBDIRS ?= ${STANDARD_SUBDIRS}
.endif

.ifdef USE_SUBDIR_INHERITANCE
SETUP_SUBDIRS    ?= ${SUBDIRS}
JAVADOC_SUBDIRS  ?= ${SUBDIRS}
EXPINC_SUBDIRS   ?= ${SUBDIRS}
JAVAH_SUBDIRS    ?= ${SUBDIRS}
OBJECTS_SUBDIRS  ?= ${SUBDIRS}
JAVAC_SUBDIRS    ?= ${SUBDIRS}
EXPLIB_SUBDIRS   ?= ${SUBDIRS}
JAR_SUBDIRS      ?= ${SUBDIRS}
EARWAR_SUBDIRS   ?= ${SUBDIRS}
EXPSHLIB_SUBDIRS ?= ${SUBDIRS}
.endif

################################################################################
# These list the variables used to define subdirectories to recurse into.
# The format is: _<pass>_SUBDIRS_
#
.if defined(SPECIAL_PASSES)
_SETUP_SUBDIRS_    = ${${MAKEACTION}_SETUP_SUBDIRS:U${SETUP_SUBDIRS}}
_JAVADOC_SUBDIRS_  = ${${MAKEACTION}_JAVADOC_SUBDIRS:U${JAVADOC_SUBDIRS}}
_EXPINC_SUBDIRS_   = ${${MAKEACTION}_EXPINC_SUBDIRS:U${EXPINC_SUBDIRS}}
_JAVAH_SUBDIRS_    = ${${MAKEACTION}_JAVAH_SUBDIRS:U${JAVAH_SUBDIRS}}
_OBJECTS_SUBDIRS_  = ${${MAKEACTION}_OBJECTS_SUBDIRS:U${OBJECTS_SUBDIRS}}
_JAVAC_SUBDIRS_    = ${${MAKEACTION}_JAVAC_SUBDIRS:U${JAVAC_SUBDIRS}}
_EXPLIB_SUBDIRS_   = ${${MAKEACTION}_EXPLIB_SUBDIRS:U${EXPLIB_SUBDIRS}}
_JAR_SUBDIRS_      = ${${MAKEACTION}_JAR_SUBDIRS:U${JAR_SUBDIRS}}
_EXPSHLIB_SUBDIRS_ = ${SHLIB_DURING_EXPSHLIB:D${${MAKEACTION}_EXPSHLIB_SUBDIRS:U${EXPSHLIB_SUBDIRS}}}
_STANDARD_SUBDIRS_ = ${${MAKEACTION}_STANDARD_SUBDIRS:U${STANDARD_SUBDIRS:U${SUBDIRS}}}
_EARWAR_SUBDIRS_   = ${${MAKEACTION}_EARWAR_SUBDIRS:U${EARWAR_SUBDIRS}}
.else
# Replace the directory slash '/' with a ';' to allow for multiple-level
# subdirectories to be specified and it will still work with modifiers like
# :H and :T which depend on the '/' character.
#
_SETUP_SUBDIRS_    = ${${.ACTION.}_SETUP_SUBDIRS:U${SETUP_SUBDIRS}:S|/|;|g}
_JAVADOC_SUBDIRS_  = ${${.ACTION.}_JAVADOC_SUBDIRS:U${JAVADOC_SUBDIRS}:S|/|;|g}
_EXPINC_SUBDIRS_   = ${${.ACTION.}_EXPINC_SUBDIRS:U${EXPINC_SUBDIRS}:S|/|;|g}
_JAVAH_SUBDIRS_    = ${${.ACTION.}_JAVAH_SUBDIRS:U${JAVAH_SUBDIRS}:S|/|;|g}
_OBJECTS_SUBDIRS_  = ${${.ACTION.}_OBJECTS_SUBDIRS:U${OBJECTS_SUBDIRS}:S|/|;|g}
_JAVAC_SUBDIRS_    = ${${.ACTION.}_JAVAC_SUBDIRS:U${JAVAC_SUBDIRS}:S|/|;|g}
_EXPLIB_SUBDIRS_   = ${${.ACTION.}_EXPLIB_SUBDIRS:U${EXPLIB_SUBDIRS}:S|/|;|g}
_JAR_SUBDIRS_      = ${${.ACTION.}_JAR_SUBDIRS:U${JAR_SUBDIRS}:S|/|;|g}
_EXPSHLIB_SUBDIRS_ = ${SHLIB_DURING_EXPSHLIB:D${${.ACTION.}_EXPSHLIB_SUBDIRS:U${EXPSHLIB_SUBDIRS}:S|/|;|g}}
_STANDARD_SUBDIRS_ = ${${.ACTION.}_STANDARD_SUBDIRS:U${STANDARD_SUBDIRS:U${SUBDIRS}}:S|/|;|g}
_EARWAR_SUBDIRS_   = ${${.ACTION.}_EARWAR_SUBDIRS:U${EARWAR_SUBDIRS}:S|/|;|g}
.endif
################################################################################
# ALL ACTIONS
#
_ALL_ACTIONS_+=BUILD COMP CLEAN RMTARGET CLOBBER DEPEND \
               EXPORT INSTALL INSTDEP SETUP JAVADOC

################################################################################
#  For each ACTION define the action for recursion, the passes for the
#  action, the targets for the complete action, and the targets for each
#  pass of the action
#

################################################################################
# BUILD ACTION
#
.if defined(MAKEFILE_PASS)
_BUILD_PASSES_ = ${MAKEFILE_PASS}
.else
.if defined(USE_JAVA_PASSES)
_BUILD_PASSES_ = ${BUILDJAVADOCS:D${JAVADOC_AFTER_JAVAC:UJAVADOC}} \
                 JAVAH JAVAC \
                 ${BUILDJAVADOCS:D${JAVADOC_AFTER_JAVAC:DJAVADOC}} \
                 JAR EARWAR
.else
_BUILD_PASSES_ = ${BUILDJAVADOCS:D${JAVADOC_AFTER_JAVAC:UJAVADOC}} \
                 EXPINC OBJECTS \
                 ${BUILDJAVADOCS:D${JAVADOC_AFTER_JAVAC:DJAVADOC}} \
                 EXPLIB ${SHLIB_DURING_EXPSHLIB:DEXPSHLIB} STANDARD
.endif
.endif

_BUILD_ACTION_=build
_build_action_=BUILD

_BUILD_JAVADOC_TARGETS_=\
	${_JAVADOC_JAVADOC_TARGETS_}

_BUILD_EXPINC_TARGETS_=\
	${_EXPORT_EXPINC_TARGETS_}

_BUILD_JAVAH_TARGETS_ = \
	${_EXPORT_JAVAH_TARGETS_}

_BUILD_OBJECTS_TARGETS_=\
	${_COMP_OBJECTS_TARGETS_}

_BUILD_JAVAC_TARGETS_ = \
  ${_COMP_JAVAC_TARGETS_}

_BUILD_EXPLIB_TARGETS_=\
	${_COMP_EXPLIB_TARGETS_}

_BUILD_JAR_TARGETS_ = \
  ${_COMP_JAR_TARGETS_}

_BUILD_EARWAR_TARGETS_ = \
  ${_COMP_EARWAR_TARGETS_}

_BUILD_EXPSHLIB_TARGETS_=\
	${_COMP_EXPSHLIB_TARGETS_}

_BUILD_STANDARD_TARGETS_=\
	${_COMP_STANDARD_TARGETS_}

################################################################################
# JAVADOC ACTION
#
.if defined (MAKEFILE_PASS)
_JAVADOC_PASSES_=${MAKEFILE_PASS}
.else
_JAVADOC_PASSES_=JAVADOC
.endif

.if defined(SPECIAL_PASSES)
_JAVADOC_TARGETS_=${_EXPORT_${MAKEPASS}_TARGETS_}
.else
_JAVADOC_TARGETS_=${_javadoc_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
.endif
_JAVADOC_ACTION_=javadoc
_javadoc_action_=JAVADOC

# JAVADOC_TARGETS is a .SPECTARG in java.mk
_JAVADOC_JAVADOC_TARGETS_ = ${BUILDJAVADOCS:D${JAVADOCS:bJAVADOC_TARGETS}}

################################################################################
# SETUP ACTION
#
.if defined (MAKEFILE_PASS)
_SETUP_PASSES_=${MAKEFILE_PASS}
.else
_SETUP_PASSES_=SETUP
.endif

.if defined(SPECIAL_PASSES)
_SETUP_TARGETS_=${_EXPORT_${MAKEPASS}_TARGETS_}
.else
_SETUP_TARGETS_=${_setup_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
.endif
_SETUP_ACTION_=setup
_setup_action_=SETUP

_SETUP_SETUP_TARGETS_=\
        ${SETUP_PROGRAMS:S/^/setup_/g} \
        ${SETUP_FILES:S/^/setup_/g} \
        ${SETUP_SCRIPTS:S/^/setup_/g}

################################################################################
# COMP ACTION
#
.if defined(MAKEFILE_PASS)
_COMP_PASSES_=${MAKEFILE_PASS}
.else
.if defined(USE_JAVA_PASSES)
_COMP_PASSES_ = ${BUILDJAVADOCS:D${JAVADOC_AFTER_JAVAC:UJAVADOC}} JAVAC \
                ${BUILDJAVADOCS:D${JAVADOC_AFTER_JAVAC:DJAVADOC}} JAR EARWAR
.else
_COMP_PASSES_ = ${BUILDJAVADOCS:D${JAVADOC_AFTER_JAVAC:UJAVADOC}} OBJECTS \
                ${BUILDJAVADOCS:D${JAVADOC_AFTER_JAVAC:DJAVADOC}} EXPLIB \
                ${SHLIB_DURING_EXPSHLIB:DEXPSHLIB} STANDARD
.endif
.endif

_COMP_ACTION_=comp
_comp_action_=COMP

# Logic to determine when to use JAVA_CLASSES versus JAVAC_TARGETS
# if defined(USE_JAVA_CLASSES)
#   ${JAVA_CLASSES}
# else
#   if empty(JAVA_CLASSES)
#     JAVAC_TARGETS
#   else
#     ${JAVA_CLASSES}
_JAVA_CLASSES_TGTS_ =\
	${JAVA_CLASSES:b${USE_JAVA_CLASSES:S/${USE_JAVA_CLASSES}/${JAVA_CLASSES}/:BJAVAC_TARGETS}}

.ifdef PROPERTIES_SAMESUFF
_PROPERTIES_ = ${PROPERTIES:S/^/properties_/g}
.else
_PROPERTIES_ = ${PROPERTIES}
.endif

_COMP_JAVAC_TARGETS_ = \
	${BUILDJAVA:D${_JAVA_CLASSES_TGTS_}} \
	${BUILDJAVA:D${_PROPERTIES_}} \
	${BUILDJAVA:D${RMI_DURING_JAVAC:D${RMI_TARGETS}}} \
	${OBJECTS}

_COMP_OBJECTS_TARGETS_ = \
	${BUILDJAVA:D${_JAVA_CLASSES_TGTS_}} \
	${BUILDJAVA:D${_PROPERTIES_}} \
	${BUILDJAVA:D${RMI_DURING_JAVAC:D${RMI_TARGETS}}} \
	${OBJECTS} \
	${_RESOURCES_}

_COMP_EXPLIB_TARGETS_ = \
	${_EXPORT_EXPLIB_TARGETS_}

_COMP_JAR_TARGETS_ = \
	${_EXPORT_JAR_TARGETS_}

_COMP_EXPSHLIB_TARGETS_=\
	${_EXPORT_EXPSHLIB_TARGETS_}

_COMP_STANDARD_TARGETS_=\
	${_PROGRAMS_} \
	${_EXPORT_STANDARD_TARGETS_} \
	${PREPROCESS} \
	${_INFS_} \
	${BUILDJAVA:D${EAR_LIBRARIES}} \
	${BUILDJAVA:D${SIGNED_EAR_LIBRARIES}} \
	${BUILDJAVA:D${WAR_LIBRARIES}} \
	${BUILDJAVA:D${SIGNED_WAR_LIBRARIES}} \
	${OTHERS}

_COMP_EARWAR_TARGETS_ = \
	${BUILDJAVA:D${EAR_LIBRARIES}} \
	${BUILDJAVA:D${SIGNED_EAR_LIBRARIES}} \
	${BUILDJAVA:D${WAR_LIBRARIES}} \
	${BUILDJAVA:D${SIGNED_WAR_LIBRARIES}}

.if ${USE_TSO_RULES}
_COMP_OBJECTS_TARGETS_ += ${TSO_INFO_FILE}
.endif

################################################################################
# CLEAN ACTION
#
.if defined(MAKEFILE_PASS)
_CLEAN_PASSES_=${MAKEFILE_PASS}
.else
_CLEAN_PASSES_=STANDARD
.endif

.if defined(SPECIAL_PASSES)
_CLEAN_TARGETS_=${_CLEAN_${MAKEPASS}_TARGETS_}
.else
_CLEAN_TARGETS_=${_clean_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
.endif
_CLEAN_ACTION_=clean
_clean_action_=CLEAN

_CLEAN_STANDARD_TARGETS_= \
	${_RMTARGET_STANDARD_TARGETS_:S/^rmtarget_/clean_/f} \
	${_PROGRAMS_:S/$/.X/g:S/^/clean_/g} \
	${_SHARED_LIBRARIES_:M*${SHLIB_SUFF}:S|${SHLIB_SUFF}$|${IMPLIB_SUFF}|g:S/^/clean_/g} \
	${_SHARED_LIBRARIES_:M*${SHLIB_SUFF}:S|${SHLIB_SUFF}$|${DEFFILE_SUFF}|g:S/^/clean_/g} \
	${_SHARED_LIBRARIES_:M*${SHLIB_SUFF}:S|${SHLIB_SUFF}$|${EXPFILE_SUFF}|g:S/^/clean_/g} \
	${OFILES:U${PROGRAMS:S/${PROG_SUFF}$/${OBJ_SUFF}/g}:S/^/clean_/g} \
	${BUILDJAVA:D${_CLASS_TARGS_:S/^/clean_/g}} \
	${BUILDJAVA:D${_PROP_TARGS_:S/^/clean_/g}} \
	${GARBAGE:S/^/clean_/g}

################################################################################
# RMTARGET ACTION
#
.if defined(MAKEFILE_PASS)
_RMTARGET_PASSES_=${MAKEFILE_PASS}
.else
_RMTARGET_PASSES_=STANDARD
.endif

.if defined(SPECIAL_PASSES)
_RMTARGET_TARGETS_=${_RMTARGET_${MAKEPASS}_TARGETS_}
.else
_RMTARGET_TARGETS_=${_rmtarget_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
.endif
_RMTARGET_ACTION_=rmtarget
_rmtarget_action_=RMTARGET

_RMTARGET_STANDARD_TARGETS_=\
	${_PROGRAMS_:S/^/rmtarget_/g} \
	${LIBRARIES:S/^/rmtarget_/g} \
	${_SHARED_LIBRARIES_:S/^/rmtarget_/g} \
	${OBJECTS:S/^/rmtarget_/g} \
	${_RESOURCES_:S/^/rmtarget_/g} \
	${OTHERS:S/^/rmtarget_/g} \
	${BUILDJAVA:D${JAR_LIBRARIES:S/^/rmtarget_/g}} \
	${BUILDJAVA:D${EAR_LIBRARIES:S/^/rmtarget_/g}} \
	${BUILDJAVA:D${WAR_LIBRARIES:S/^/rmtarget_/g}} \
	${BUILDJAVA:D${SIGNED_JAR_LIBRARIES:S/^/rmtarget_/g}} \
	${BUILDJAVA:D${SIGNED_EAR_LIBRARIES:S/^/rmtarget_/g}} \
	${BUILDJAVA:D${SIGNED_WAR_LIBRARIES:S/^/rmtarget_/g}} \
	${BUILDJAVA:D${_JAR_TARGS_:S/^/rmtarget_/g}} \
	${BUILDJAVA:D${_EAR_TARGS_:S/^/rmtarget_/g}} \
	${BUILDJAVA:D${_WAR_TARGS_:S/^/rmtarget_/g}} \
	${_EXPORT_EXPLIB_TARGS_:S/^/rmtarget_/g} \
	${_EXPORT_EXPSHLIB_TARGS_:S/^/rmtarget_/g} \
	${_EXPORT_OBJECTS_TARGS_:S/^/rmtarget_/g} \
	${_EXPORT_STANDARD_TARGS_:S/^/rmtarget_/g}

################################################################################
# CLOBBER ACTION
#
.if defined(MAKEFILE_PASS)
_CLOBBER_PASSES_=${MAKEFILE_PASS}
.else
_CLOBBER_PASSES_=STANDARD
.endif

.if defined(SPECIAL_PASSES)
_CLOBBER_TARGETS_=${_CLOBBER_${MAKEPASS}_TARGETS_}
.else
_CLOBBER_TARGETS_=${_clobber_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
.endif
_CLOBBER_ACTION_=clobber
_clobber_action_=CLOBBER

_CLOBBER_STANDARD_TARGETS_=\
	${_CLEAN_STANDARD_TARGETS_:S/^clean_/clobber_/f} \
	${INCLUDES:S/^/clobber_/g} \
	${BUILDJAVA:D${JAVAH_TARGETS:S/^/clobber_/g}} \
	${_EXPORT_EXPINC_TARGS_:S/^/clobber_/g} \
	${NLV:D${_CATALOGS_:S/^/clobber_/g}} \
	${NLV:D${_MSG_HDRS_:S/^/clobber_/g}} \
	${NLV:D${_CODEPAGE_TGTS_:S/^/clobber_/g}} \
	${_HELPS_:S/^/clobber_/g} \
	${_IPFHELPS_:S/^/clobber_/g} \
	${_INFS_:S/^/clobber_/g}

#These 'TARGS' are the fully-qualified path/targets for each of the non-qualified 'TARGETS'
_JARFILES_ = ${${_JAR_TARGET}_JAR_RUNDIR:U${JAR_RUNDIR}}${DIRSEP}${_JAR_TARGET}
_JAR_TARGS_ = ${JAR_LIBRARIES:@_JAR_TARGET@${_JARFILES_}@} \
              ${SIGNED_JAR_LIBRARIES:@_JAR_TARGET@${_JARFILES_}@}
_EARFILES_ = ${${_EAR_TARGET}_EAR_RUNDIR:U${EAR_RUNDIR}}${DIRSEP}${_EAR_TARGET}
_EAR_TARGS_ = ${EAR_LIBRARIES:@_EAR_TARGET@${_EARFILES_}@} \
              ${SIGNED_EAR_LIBRARIES:@_EAR_TARGET@${_EARFILES_}@}
_WARFILES_ = ${${_WAR_TARGET}_WAR_RUNDIR:U${WAR_RUNDIR}}${DIRSEP}${_WAR_TARGET}
_WAR_TARGS_ = ${WAR_LIBRARIES:@_WAR_TARGET@${_WARFILES_}@} \
              ${SIGNED_WAR_LIBRARIES:@_WAR_TARGET@${_WARFILES_}@}

#RMI_TARGETS represents the RMI xxx_Skel.class files
_CLASS_TARGETS_  = ${JAVA_CLASSES}
_CLASS_TARGETS_ += ${RMI_TARGETS}
_CLASS_TARGETS_ += ${RMI_CLASSES:R:S/$/_Stub${CLASS_SUFF}/g}
.if empty(JAVA_PACKAGE_NAME)
_CLASSFILES_ = ${_CLASSGENDIR_}${DIRSEP}${_CLASS_TARGET:S/^${SANDBOXBASE}${DIRSEP}${ODESRCNAME}${DIRSEP}//g}
.else
_JAVA_PACKAGE_DIR_ = ${JAVA_PACKAGE_NAME:S!.!${DIRSEP}!g}
_CLASSFILES_ = ${_CLASSGENDIR_}${DIRSEP}${_CLASS_TARGET:S!^!${_JAVA_PACKAGE_DIR_}${DIRSEP}!:S!^${_JAVA_PACKAGE_DIR_}${DIRSEP}${SANDBOXBASE}${DIRSEP}${ODESRCNAME}${DIRSEP}!!}
.endif
_CLASS_TARGS_ = ${_CLASS_TARGETS_:@_CLASS_TARGET@${_CLASSFILES_}@}

################################################################################
# DEPEND ACTION
#
.if defined(MAKEFILE_PASS)
_DEPEND_PASSES_=${MAKEFILE_PASS}
.else
_DEPEND_PASSES_=STANDARD
.endif

.if defined(SPECIAL_PASSES)
_DEPEND_TARGETS_=${_DEPEND_${MAKEPASS}_TARGETS_}
.else
_DEPEND_TARGETS_=${_depend_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
.endif
_DEPEND_ACTION_=depend
_depend_action_=DEPEND

_DEPEND_STANDARD_TARGETS_= \
	${OBJECTS:S/${OBJ_SUFF}$/${DEP_SUFF}/g} \
	${BUILDJAVA:DJAVADEP_TARGETS} \
	${LIBRARIES_OFILES:S/${OBJ_SUFF}$/${DEP_SUFF}/g} \
	${SHARED_LIBRARIES_OFILES:S/${OBJ_SUFF}$/${DEP_SUFF}/g} \
	${PROGRAMS_OFILES:S/${OBJ_SUFF}$/${DEP_SUFF}/g}


################################################################################
# EXPORT ACTION
#
.if defined(MAKEFILE_PASS)
_EXPORT_PASSES_=${MAKEFILE_PASS}
.else
.if defined(USE_JAVA_PASSES)
_EXPORT_PASSES_ = JAVAH JAVAC JAR
.else
_EXPORT_PASSES_ = EXPINC OBJECTS EXPLIB \
                  ${SHLIB_DURING_EXPSHLIB:DEXPSHLIB}
.ifdef STDPASS_DURING_EXPORT
_EXPORT_PASSES_ += STANDARD
.endif
.endif
.endif

.if defined(SPECIAL_PASSES)
_EXPORT_TARGETS_=${_EXPORT_${MAKEPASS}_TARGETS_}
.else
_EXPORT_TARGETS_=${_export_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
.endif
_EXPORT_ACTION_=export
_export_action_=EXPORT

#These three 'TARGS' are the fully-qualified path/targets for each of the non-qualified export 'TARGETS'
_EXPORT_EXPINC_TARGS_ = ${_EXPORT_EXPINC_TARGETS_:Mexpinc_*:S/^expinc_//g:@_EXPINC_TARGET@${_EXPINCFILES_}@}
_EXPORT_OBJECTS_TARGS_ = ${_EXPORT_OBJECTS_TARGETS_:Mexport_*:S/^export_//g:@_EXPORT_TARGET@${_EXPFILES_}@}
_EXPORT_EXPLIB_TARGS_ = ${_EXPORT_EXPLIB_TARGETS_:Mexport_*:S/^export_//g:@_EXPORT_TARGET@${_EXPFILES_}@}
_EXPORT_EXPSHLIB_TARGS_ = ${_EXPORT_EXPSHLIB_TARGETS_:Mexport_*:S/^export_//g:@_EXPORT_TARGET@${_EXPFILES_}@}
_EXPORT_STANDARD_TARGS_ = ${_EXPORT_STANDARD_TARGETS_:Mexppgm_*:S/^exppgm_//g:@_EXPPGM_TARGET@${_EXPPGMFILES_}@}

_EXPORT_JAVAH_TARGETS_ = \
	${EXPINC_TARGETS} \
	${INCLUDES:S/^/expinc_/g} \
	${EXPINCS:S/^/expinc_/g} \
	${BUILDJAVA:D${RMI_DURING_JAVAC:U${RMI_TARGETS}:D}} \
	${BUILDJAVA:D${JAVAH_TARGETS}}

_EXPORT_EXPINC_TARGETS_ = \
	${EXPINC_TARGETS} \
	${INCLUDES:S/^/expinc_/g} \
	${EXPINCS:S/^/expinc_/g} \
	${BUILDJAVA:D${RMI_DURING_JAVAC:U${RMI_TARGETS}:D}} \
	${BUILDJAVA:D${JAVAH_TARGETS}} \
	${NLV:D${_CATALOGS_}} \
	${NLV:D${_MSG_HDRS_}} \
	${NLV:D${_CODEPAGE_TGTS_}} \
	${_HELPS_} \
	${_IPFHELPS_} \
	${_MC_HDRS_}

_EXPORT_JAVAC_TARGETS_ = \
	${_COMP_JAVAC_TARGETS_}

_EXPORT_OBJECTS_TARGETS_ = \
	${_COMP_OBJECTS_TARGETS_}

_EXPORT_JAR_TARGETS_ = \
	${BUILDJAVA:D${JAR_LIBRARIES}} \
	${BUILDJAVA:D${SIGNED_JAR_LIBRARIES}} \
	${EXPLIB_TARGETS} \
	${EXPLIBS:S/^/export_/g}

_EXPORT_EXPLIB_TARGETS_ = \
	${LIBRARIES} \
	${BUILDJAVA:D${JAR_LIBRARIES}} \
	${BUILDJAVA:D${SIGNED_JAR_LIBRARIES}} \
	${EXPLIB_TARGETS} \
	${EXPLIBS:S/^/export_/g}

# by default, nothing happens in EXPSHLIB...see SHLIB_DURING_EXPSHLIB
_EXPORT_EXPSHLIB_TARGETS_=

.if ${USE_TSO_RULES}
_EXPORT_EXPINC_TARGETS_ += ${TSO_INCLUDES:S/^/expinc_/g}
.endif

_EXPORT_STANDARD_TARGETS_ = \
	${EXPPGMS:S/^/exppgm_/g}

################################################################################
# INSTALL ACTION
#
.if make(instdep_all)
.if defined(MAKEFILE_PASS)
_INSTDEP_PASSES_=${MAKEFILE_PASS}
.else
_INSTDEP_PASSES_=STANDARD
.endif

.else

.if defined(MAKEFILE_PASS)
_INSTALL_PASSES_=${MAKEFILE_PASS}
.else
_INSTALL_PASSES_=STANDARD
.endif
.endif # make(instdep_all)

.if make(instdep_all)
.if defined(SPECIAL_PASSES)
_INSTDEP_TARGETS_=${_INSTDEP_${MAKEPASS}_TARGETS_}
.else
_INSTDEP_TARGETS_=${_instdep_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
.endif
_INSTDEP_ACTION_=instdep
_instdep_action_=INSTDEP

.else

.if defined(SPECIAL_PASSES)
_INSTALL_TARGETS_=${_INSTALL_${MAKEPASS}_TARGETS_}
.else
_INSTALL_TARGETS_=${_install_action_:@.ACTION.@${_TARGET_ACTIONS_}@}
.endif
_INSTALL_ACTION_=install
_install_action_=INSTALL
.endif # make(instdep_all)

.if make(instdep_all)
_INSTDEP_STANDARD_TARGETS_ = ${ILIST:S/^/instdep_/g}
.if !empty(MACHINE:Mmvs390_oe_*) && defined(INSTALL_PRELINKER_FILES)
_INSTDEP_STANDARD_TARGETS_ += ${ILIST:S/^/instdep_/g:S/$/${PRE_SUFF}/g}
.endif # mvs390
.elif defined(INSTALL_USING_DEPS)
_INSTALL_STANDARD_TARGETS_ = ${ILIST:S/^/instdep_/g}
.if !empty(MACHINE:Mmvs390_oe_*) && defined(INSTALL_PRELINKER_FILES)
_INSTALL_STANDARD_TARGETS_ += ${ILIST:S/^/instdep_/g:S/$/${PRE_SUFF}/g}
.endif # mvs390
.else
_INSTALL_STANDARD_TARGETS_ = ${ILIST:S/^/install_/g}
.if !empty(MACHINE:Mmvs390_oe_*) && defined(INSTALL_PRELINKER_FILES)
_INSTALL_STANDARD_TARGETS_ += ${ILIST:S/^/install_/g:S/$/${PRE_SUFF}/g}
.endif # mvs390
.endif # make(instdep_all)

#
# User-configurable targets
#
.if defined(SHLIB_DURING_STANDARD)
_COMP_STANDARD_TARGETS_ += ${_SHARED_LIBRARIES_}
.elif defined(SHLIB_DURING_EXPSHLIB)
_EXPORT_EXPSHLIB_TARGETS_ += ${_SHARED_LIBRARIES_} ${EXPSHLIBS:S/^/export_/g}
.else
_EXPORT_EXPLIB_TARGETS_ += ${_SHARED_LIBRARIES_}
.endif

################################################################################
# These are the targets that are really actions.
# The targets have the special source .PASSES which makes them actions
# with passes to the right of the .PASSES
#
.if defined(SPECIAL_PASSES)
BUILD    : .PASSES ${_BUILD_PASSES_}
COMP     : .PASSES ${_COMP_PASSES_}
CLEAN    : .PASSES ${_CLEAN_PASSES_}
RMTARGET : .PASSES ${_RMTARGET_PASSES_}
DEPEND   : .PASSES ${_DEPEND_PASSES_}
INSTALL  : .PASSES ${_INSTALL_PASSES_}
INSTDEP  : .PASSES ${_INSTDEP_PASSES_}
CLOBBER  : .PASSES ${_CLOBBER_PASSES_}
EXPORT   : .PASSES ${_EXPORT_PASSES_}
SETUP    : .PASSES ${_SETUP_PASSES_}
JAVADOC  : .PASSES ${_JAVADOC_PASSES_}
.endif

.endif  # __BLD_PASSES_MK_
