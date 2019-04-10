################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines the global rules to generate static, dynamic, and shared
# libraries.
#  Some variables used:
#    LIBRARIES
#    SHARED_LIBRARIES
#    NLV_SHARED_LIBRARIES
#    ARFLAGS
#    SHLDFLAGS
#    _LIBS_
#    _OFILES_
#    _EXTRA_OFILES_
#
.if !defined(__LIB_MK_)
__LIB_MK_=

################################################################################
# Library ordering variables
#
.if defined(ORDER_LIBRARIES)
DEFAULT_LORDER = `${LORDER} *${OBJ_SUFF} | ${TSORT}`

A.OUT_LORDER ?= *${OBJ_SUFF}
ELF_LORDER   ?= *${OBJ_SUFF}
OMF_LORDER   ?= *${OBJ_SUFF}
COFF_LORDER  ?= ${DEFAULT_LORDER}
XCOFF_LORDER ?= ${DEFAULT_LORDER}
MACHO_LORDER ?= ${DEFAULT_LORDER}
.endif


################################################################################
# Variables for various archiving tools
#
.if !empty(LIBRARIES)

LIBTARGET=${.TARGET}
.if (${CCFAMILY}=="visual")
LIBTARGET=/OUT:${.TARGET}
.elif (${CCFAMILY}=="vage")
.if !empty(MACHINE:Mx86_os2_*) && ${CCVERSION} != "3.6"
LIBTARGET=${.TARGET}
LIBCONCAT=
.else
LIBTARGET=/OUT:${.TARGET}
.endif # OS/2
.endif # vage

################################################################################
# Static library rule
#

# Temporary variable used below
_ALL_LIB_OFILES_TMP_ = ${.ALLSRC:M*${OBJ_SUFF}} ${_EXTRA_OFILES_:p}

.if ${BACKSLASH_PATHS}
_ALL_LIB_OFILES_ = ${_ALL_LIB_OFILES_TMP_:s|/|\\|g}
.else
_ALL_LIB_OFILES_ = ${_ALL_LIB_OFILES_TMP_}
.endif

${LIBRARIES}: $${_OFILES_}
.if !empty(MACHINE:M*_openvms_*)
	${_AR_} /CREATE ${LIBTARGET}
	${_RESPFILE_:rm-}
	${_ALL_LIB_OFILES_:@.PATH.@${$$ ${_AR_} ${_ARFLAGS_} ${LIBTARGET} ${VMS_PATH_CONV}:L:a${_RESPFILE_}}@}
	$$@${_RESPFILE_:R}
.else
.if defined(USE_RESPFILE) && !defined(UNIX) && ${CCFAMILY} != "gnu"
# Generate a response file
	${_RESPFILE_:rm-}
.if !empty(MACHINE:Mx86_os2_*) && ${CCFAMILY} == "vage" && \
    ${CCVERSION} != "3.6"
	${${LIBTARGET} &:L:A${_RESPFILE_}}
	${_ALL_LIB_OFILES_:@WORD@${${WORD} &:L:a${_RESPFILE_}}@}
	${,,:L:a${_RESPFILE_}}
	${.TARGET:rm-}
	${_AR_} ${_ARFLAGS_} @${_RESPFILE_}
.else
	${_ARFLAGS_:b${${_ARFLAGS_:B""}:L:a${_RESPFILE_}}}
	${LIBTARGET:a${_RESPFILE_}}
	${_ALL_LIB_OFILES_:@WORD@${WORD:a${_RESPFILE_}}@}
	${_AR_} @${_RESPFILE_}
.endif
.else  # USE_RESPFILE
.if !empty(MACHINE:Mx86_os2_*) && ${CCFAMILY} == "vage" && \
    ${CCVERSION} != "3.6"
	${.TARGET:rm-}
.endif
	${_AR_} ${_ARFLAGS_} ${LIBTARGET} \
          ${SHORT_OBJ_PATHS:D${_ALL_LIB_OFILES_:S|^${_CURDIR_}${_CURDIRSEP_}||g}:U${_ALL_LIB_OFILES_}} \
	        ${LIBCONCAT:D,,}
.endif # USE_RESPFILE
.if defined(ORDER_LIBRARIES)
	${tmp:L:rr-}
	mkdir tmp
	${CHDIR} tmp ${CMDSEP} { \
	    ${_AR_} x ../${.TARGET}; \
	    ${RM} -f __.SYMDEF __________ELELX; \
	    ${_AR_} cr ${.TARGET} ${${OBJECT_FORMAT}_LORDER}; \
	}
	${MV} ${_MVFLAGS_} tmp/${.TARGET} .
	${tmp:L:rr-}
.endif
.ifndef NO_RANLIB
	${_RANLIB_} ${.TARGET}
.endif
.if defined(STRIP_ARCH)
	${_STRIP_} ${_STRIPFLAGS_} ${.TARGET}
.endif
.endif

.endif # !empty(LIBRARIES)

_SHARED_LIBRARIES_ = ${SHARED_LIBRARIES} ${_NLV_SHARED_LIBRARIES_}

.if !empty(_SHARED_LIBRARIES_)

################################################################################
# Dynamic/shared library rules
#
IMPORT_LIBRARIES ?= ${_SHARED_LIBRARIES_:S/${SHLIB_SUFF}$/${IMPLIB_SUFF}/g}

# Temporary variable used for object of shared libraries
.ifdef NO_LINK_EXPFILES
_ALL_SHLIB_OFILES_TMP_ = ${.ALLSRC:N*${STATLIB_SUFF}:N*${IMPLIB_SUFF}:N*${EXPFILE_SUFF}}
.else
_ALL_SHLIB_OFILES_TMP_ = ${.ALLSRC:N*${STATLIB_SUFF}:N*${IMPLIB_SUFF}}
.endif

_ALL_SHLIB_OFILES_TMP_ += ${_EXTRA_OFILES_:p}

.if ${BACKSLASH_PATHS}
_ALL_SHLIB_OFILES_ = ${_ALL_SHLIB_OFILES_TMP_:s|/|\\|g}
.else
_ALL_SHLIB_OFILES_ = ${_ALL_SHLIB_OFILES_TMP_}
.endif

.if !empty(TARGET_MACHINE:Mmvs390_oe_*)
#
# creation of the shared object generates the side deck simultaneously,
# so just form a dependency with no commands.
#
IMPLIB_SOURCES ?= ${.TARGET:S/${IMPLIB_SUFF}$/${SHLIB_SUFF}/g}

${IMPORT_LIBRARIES}: $${IMPLIB_SOURCES}
.endif

.if (${CCFAMILY} == "visual")

#
# rule to create the .lib file
#

# Visual C++ 4.x doesn't provide a way to generate the .LIB separately
.if (${CCVERSION} == "default")
NO_IMPLIB_COMMANDS =
.endif

.ifdef NO_IMPLIB_COMMANDS
IMPLIB_SOURCES ?= ${.TARGET:S/${IMPLIB_SUFF}$/${SHLIB_SUFF}/g}
.else
IMPLIB_SOURCES ?= ${.TARGET:S/${IMPLIB_SUFF}$/${DEFFILE_SUFF}/g} ${_OFILES_}
.endif

${IMPORT_LIBRARIES}: $${IMPLIB_SOURCES}
.ifndef NO_IMPLIB_COMMANDS
	${_IMPLIB_} ${_IMPLIBFLAGS_} ${_IMPLIBARGS_}
.endif

.endif # visual

.if (${CCFAMILY} == "vage")

#
# rule to create the .lib file
#
.if defined(NO_IMPLIB_COMMANDS) || !empty(TARGET_MACHINE:Mx86_os2_*)
IMPLIB_SOURCES ?= ${.TARGET:S/${IMPLIB_SUFF}$/${SHLIB_SUFF}/g}
.else
IMPLIB_SOURCES ?= ${.TARGET:S/${IMPLIB_SUFF}$/${DEFFILE_SUFF}/g}
.endif

${IMPORT_LIBRARIES}: $${IMPLIB_SOURCES}
.ifndef NO_IMPLIB_COMMANDS
	${_IMPLIB_} ${_IMPLIBFLAGS_} ${_IMPLIBARGS_}
.endif

#
# rule to create the .def file
# Note: if DEFFILE_LINE1 is not defined, lines 2-5 are not checked
#
_SHLIB_NAMES_=${.TARGET:S/${DEFFILE_SUFF}$/${SHLIB_SUFF}/g}
# The _SHLIB_DEF_OFILES_ assignment is copied from the _OFILES_ assignment.
_SHLIB_DEF_OFILES_=${${_SHLIB_NAMES_}_OFILES:U${_SHLIB_NAMES_:T}_OFILES:U${OFILES:U${_SHLIB_NAMES_:M*${PROG_SUFF}:S/${PROG_SUFF}$/${OBJ_SUFF}/g}}}
${IMPORT_LIBRARIES:S/${IMPLIB_SUFF}$/${DEFFILE_SUFF}/g}: $${_SHLIB_DEF_OFILES_}
.if !empty(DEFFILE_LINE1)
	${DEFFILE_LINE1:A${.TARGET}}
.if !empty(DEFFILE_LINE2)
	${DEFFILE_LINE2:a${.TARGET}}
.endif # LINE2
.if !empty(DEFFILE_LINE3)
	${DEFFILE_LINE3:a${.TARGET}}
.endif # LINE3
.if !empty(DEFFILE_LINE4)
	${DEFFILE_LINE4:a${.TARGET}}
.endif # LINE4
.if !empty(DEFFILE_LINE5)
	${DEFFILE_LINE5:a${.TARGET}}
.endif # LINE5
.endif # LINE1
	${${_ALL_SHLIB_OFILES_:@FILE@${FILE:!${_CPPFILT_} ${_CPPFILTFLAGS_} \
	${FILE} >>${.TARGET}!}@}:L:A${NULL_DEVICE}}
.endif # vage

.if (!empty(TARGET_MACHINE:Mx86_nt_*) || !empty(TARGET_MACHINE:Mx86_95_*)) && \
    (${CCFAMILY} == "visual" || ${CCFAMILY} == "vage")
#
# Visual C++ and Visual Age for Windows use an export file for linking,
# created when the .lib file is created.
#
${IMPORT_LIBRARIES:S/${IMPLIB_SUFF}$/${EXPFILE_SUFF}/g}: $${.TARGET:S/$${EXPFILE_SUFF}$$/$${IMPLIB_SUFF}/g}
.endif # Windows NT/95 visual/vage

${_SHARED_LIBRARIES_}: $${_OFILES_} $${_LIBS_}
.if !empty(MACHINE:M*_openvms_*)
	${_RESPFILE_:rm-}
	${_SHLDOPTS_:@.WORD.@${.WORD.:a${_RESPFILE_}}@}
	${SHORT_OBJ_PATHS:D${_ALL_SHLIB_OFILES_:S|^${_CURDIR_}${_CURDIRSEP_}||g}:U${_ALL_SHLIB_OFILES_}:@.PATH.@${VMS_PATH_CONV}@:s|$| ${_LINKING_LIBS_}|:@.PATH.@${.PATH.:a${_RESPFILE_}}@}
	${_SHLD_} ${_SHLDFLAGS_:N-L*} ${_RESPFILE_}/OPT
.else
.ifdef UNIX
.if (${STORE_SHLIB_IN_ARCHIVE} == "TRUE")
	${_SHLD_} ${_SHLDFLAGS_} -o ${.TARGET}.so \
            ${SHORT_OBJ_PATHS:D${_ALL_SHLIB_OFILES_:S|^${_CURDIR_}${_CURDIRSEP_}||g}:U${_ALL_SHLIB_OFILES_}} \
	          ${_LINKING_LIBS_} ${SHLD_TRAILING_FLAGS}
	${_AR_} ${_ARFLAGS_} ${.TARGET} ${.TARGET}.so
.else
	${_SHLD_} ${_SHLDFLAGS_} -o ${.TARGET} \
            ${SHORT_OBJ_PATHS:D${_ALL_SHLIB_OFILES_:S|^${_CURDIR_}${_CURDIRSEP_}||g}:U${_ALL_SHLIB_OFILES_}} \
	          ${_LINKING_LIBS_} ${SHLD_TRAILING_FLAGS}
.endif
.else # not UNIX
.if defined(USE_RESPFILE) && ${CCFAMILY} != "gnu"
	${_RESPFILE_:rm-}
	${_SHLDFLAGS_:N\-L*:A${_RESPFILE_}}
.if !empty(MACHINE:Mx86_os2_*)
	${_ALL_SHLIB_OFILES_:l:N*${RES_SUFF}:@FILE@${FILE:a${_RESPFILE_}}@}
.else  # OS2
	${_ALL_SHLIB_OFILES_:@FILE@${FILE:a${_RESPFILE_}}@}
.endif # OS2
	${_LINKING_LIBS_:@FILE@${FILE:b${FILE:a${_RESPFILE_}}}@}
	${SHLD_TRAILING_FLAGS:a${_RESPFILE_}}
	${_SHLD_} @${_RESPFILE_}
.else  # USE_RESPFILE
.if !empty(MACHINE:Mx86_os2_*)
	${_SHLD_} ${_SHLDFLAGS_:N\-L*} \
	    ${_ALL_SHLIB_OFILES_:l:N*${RES_SUFF}} \
	    ${_LINKING_LIBS_} ${SHLD_TRAILING_FLAGS}
.else  # OS2
	${_SHLD_} ${_SHLDFLAGS_:N\-L*} \
	    ${_ALL_SHLIB_OFILES_} \
	    ${_LINKING_LIBS_} ${SHLD_TRAILING_FLAGS}
.endif # OS2
.endif # USE_RESPFILE
.if !empty(MACHINE:Mx86_os2_*)
	-${_ALL_SHLIB_OFILES_:l:M*${RES_SUFF}:b${_RC_} \
	                      ${_ALL_SHLIB_OFILES_:l:M*${RES_SUFF}} ${.TARGET}}
.endif # OS2
.endif # not UNIX
.if defined(STRIP_LIB)
	${_STRIP_} ${_STRIPFLAGS_} ${.TARGET}
.endif
.endif # VMS

.endif # !empty(_SHARED_LIBRARIES_)

.endif
