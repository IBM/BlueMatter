################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file includes the depend.mk file if it exists and runs mkdep.
#  Makefiles included:
#    depend.mk - The file located in the obj directory.
#
#  Some variables used:
#    DEPEND_MK
#      The name of the dependency file.
#    BINARIES and BUILDJAVA
#      The .EXIT target which runs mkdep will be executed only if one or
#      both of these variables are defined.
#    USE_GENDEP
#      Flag specifying whether or not to use gendep.  Only OS/2, MVS, and 
#      HPUX will use gendep by default.  Other platforms can use gendep
#      by simply defining the USE_GENDEP flag, along the the DEPENDENCIES flag.
#    DEPEND_FILE_DIRS
#      Optional.  This can contain whitespace-delimited directories which
#      will be passed to mkdep (assumed to be directories that contain .u
#      files).  Typically used during Java builds since Jikes stores its
#      .u files in the directory where the .java file is.
#    MD
#      The mkdep command.
#    MDFLAGS
#      The flags used by MD.
#    GENDEPFLAGS
#      The flags used by gendep
#    _CDEFS_
#      Makefile variable to store command-line variables defined via "-D".
#      _CDEFS_ is used as part of _CC_FLAGS_ and _GENDEPFLAGS_
#
################################################################################
.if !defined(__DEPEND_MK_)
__DEPEND_MK_=

################################################################################
# Defined DEPEND_MK if it is not defined.
#
DEPEND_MK ?= depend.mk

################################################################################
# Include depend.mk if it exists
#
.if exists(${DEPEND_MK})
.include "${DEPEND_MK}"
.endif

################################################################################
# Define the gendep suffixes necessary for suffix transformation
#
.SUFFIXES: ${DEP_SUFF}

################################################################################
# Default conditionals for USE_GENDEP
# - only default gendep usage if USE_GENDEP is not previously defined, 
#   NO_DEFAULT_GENDEP is not defined and DEPENDENCIES is defined.
#
.if (defined(DEPENDENCIES))
.if (!defined(NO_DEFAULT_GENDEP) && !defined(USE_GENDEP))

.if !empty(MACHINE:Mx86_nt_*) || !empty(MACHINE:Mx86_95_*)
# Visual Age is the only supported Windows compiler that can generate
# proper dependency files on its own.
.if (${CCFAMILY} != "vage")
USE_GENDEP=1
.endif

.elif !empty(MACHINE:Mx86_os2_*)
.if (${CCFAMILY} == "vage" && ${CCVERSION} != "3.6")
USE_GENDEP=1
.endif

.elif !empty(MACHINE:Mhp9000_ux_*) || !empty(MACHINE:Mia64_hpux_*)
.if (${CCFAMILY} == "native")
USE_GENDEP=1
.endif

.elif !empty(MACHINE:Mx86_ptx_*)
.if (${CCFAMILY} == "native")
USE_GENDEP=1
.endif

.elif !empty(MACHINE:Mmvs390_oe_*)
.if (${CCFAMILY} == "native")
USE_GENDEP=1
.endif

.elif !empty(MACHINE:Mx86_sco_*)
.if (${CCFAMILY} == "native")
USE_GENDEP=1
.endif

.elif !empty(MACHINE:Mas400_os400_*)
.if (${CCFAMILY} == "native")
USE_GENDEP=1
.endif

.elif !empty(MACHINE:M*_openvms_*)
.if (${CCFAMILY} == "native")
USE_GENDEP=1
.endif

.endif
.endif
.endif

################################################################################
# Flags for and mkdep and gendep
#
_MDFLAGS_=\
	${${.TARGET}_MDFLAGS:U${MDFLAGS}} \
	${${.TARGET}_MDENV:U${MDENV}} \
	${${.TARGET}_MDARGS:U${MDARGS}}

_GENDEPFLAGS_=\
	${_CDEFS_} \
	${${.TARGET:S/${DEP_SUFF}$/${OBJ_SUFF}/g}_GENDEPFLAGS:U${GENDEPFLAGS}} \
	${${.TARGET:S/${DEP_SUFF}$/${OBJ_SUFF}/g}_GENDEPARGS:U${GENDEPARGS}} \
	${${.TARGET:S/${DEP_SUFF}$/${OBJ_SUFF}/g}_GENDEPENV:U${GENDEPENV}} \
	${_GENINC_} \
	${_CC_INCDIRS_}

################################################################################
#  Dependency suffix transformation rule
#
${CC_SUFF:S|$|${DEP_SUFF}|g}:
.if defined(USE_RESPFILE)
	${_RESPFILE_:rm-}
	${_GENDEPFLAGS_:a${_RESPFILE_}}
	${.IMPSRC:a${_RESPFILE_}}
	${GENDEP} -respfile ${_RESPFILE_}
.else
	${GENDEP} ${_GENDEPFLAGS_} ${.IMPSRC}
.endif

################################################################################
# DEPEND target
#
#
.if defined(SPECIAL_PASSES)
depend_all: DEPEND .SPECTARG
.else
depend_all: $${_all_targets_} .SPECTARG;@
.endif

help::
	@${ECHO} Target: depend_all - Generates all target dependencies without compiling.

################################################################################
# Only run MD in directories that have binaries.
#
.if defined(BINARIES) || (defined(BUILDJAVA) && defined(JAVA_DEPENDENCIES))
.ifnmake clobber_all && clean_all && rmtarget_all
.ifdef RUN_MD_WITH_END
.END:
.else
.EXIT:
.endif
	-${MD} ${_MDFLAGS_} . ${DEPEND_FILE_DIRS}
.endif
.endif

.endif
