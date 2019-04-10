################################################################################
# Version: %RELEASE_NAME% (Build %LEVEL_NAME%, %BUILD_DATE%)
#
# This file defines the OBJECTS target and suffix transformations.
#  Some variables used and defined:
#   OFILES, _OFILES_
#       The objects to be compiled.
#   EXTRA_OFILES, _EXTRA_OFILES_
#       The objects to be compiled.
#   HFILES
#       Header files that the OFILES depend
#   _PROGRAMS_
#       Used for a default if OFILES is undefined.  The OFILES will be program
#       name with OBJ_SUFF.
#   NO_YACC_RULES
#       When defined the yacc rules are not included.
#   NO_LEX_RULES
#       When defined the lex rules are not included.
#   NO_Y_H_RULE
#       When defined the .y->.h yacc rule is not included.  Must not define
#       NO_YACC_RULES also if you want this included.
#   USE_GENDEP
#       If USE_GENDEP is defined, call gendep to generate dependencies after
#       the object file is compiled.
#
################################################################################
.if !defined(__OBJ_MK_)
__OBJ_MK_=

################################################################################
# All the object files to compile.
#

PROGRAMS_OFILES=${_PROGRAMS_:@.PROG.@${${.PROG.}_OFILES:U${OFILES:U${.PROG.:S/${PROG_SUFF}$/${OBJ_SUFF}/g}}}@}
LIBRARIES_OFILES=${LIBRARIES:@.LIB.@${${.LIB.}_OFILES:U${OFILES}}@}
SHARED_LIBRARIES_OFILES=${_SHARED_LIBRARIES_:@.SHLIB.@${${.SHLIB.}_OFILES:U${OFILES}}@}

_ALL_OFILES_ = ${PROGRAMS_OFILES} ${LIBRARIES_OFILES} ${SHARED_LIBRARIES_OFILES} ${OBJECTS}

_OFILES_=${${.TARGET}_OFILES:U${.TARGET:T}_OFILES:U${OFILES:U${.TARGET:M*${PROG_SUFF}:S/${PROG_SUFF}$/${OBJ_SUFF}/g}}}

# OFILES that programs and libraries don't depend on, but are linked with
_EXTRA_OFILES_=${${.TARGET}_EXTRA_OFILES:U${EXTRA_OFILES}}

################################################################################
#  C/C++ suffix compilation rules
#
${CC_SUFF:S|$|${OBJ_SUFF}|g}:
.if !empty(MACHINE:M*_openvms_*)
	${$$ ${_CC_} -:L:A${_RESPFILE_}}
	${_CCFLAGS_:N-I*:@.WORD.@${${.WORD.} -:L:a${_RESPFILE_}}@}
	${_CCFLAGS_:M-I*:S|^-I||g:@.WORD.@"${.WORD.}",@:S|^|/INC=(|:s|,$|)|:@.WORD.@${${.WORD.} -:L:a${_RESPFILE_}}@}
	${.IMPSRC:@.PATH.@${VMS_PATH_CONV}@:a${_RESPFILE_}}
	$$@${_RESPFILE_:R}
.else
.if ${BACKSLASH_PATHS}
.if defined(USE_RESPFILE)
	${ -c:L:A${_RESPFILE_}}
	${_CCFLAGS_:a${_RESPFILE_}}
	${${.IMPSRC:s|/|\\|g}:L:a${_RESPFILE_}}
	${_CC_} @${_RESPFILE_}
.else
	${_CC_} -c ${_CCFLAGS_} ${.IMPSRC:s|/|\\|g}
.endif
.else  # platforms that use forward slash paths follow
	${_CC_} -c ${_CCFLAGS_} ${.IMPSRC}
.endif
.if defined(STRIP_OBJ)
	${_STRIP_} ${_STRIPFLAGS_} ${.TARGET}
.endif
.endif
.if defined(DEPENDENCIES)
.if defined(USE_GENDEP)
.if defined(USE_RESPFILE)
	${_RESPFILE_:rm-}
	${_GENDEPFLAGS_:a${_RESPFILE_}}
.if ${BACKSLASH_PATHS}
	${.IMPSRC:s|/|\\|g:a${_RESPFILE_}}
.else
	${.IMPSRC:a${_RESPFILE_}}
.endif
	${GENDEP} -respfile ${_RESPFILE_}
.else
.if ${BACKSLASH_PATHS}
	${GENDEP} ${_GENDEPFLAGS_} ${.IMPSRC:s|/|\\|g}
.else
	${GENDEP} ${_GENDEPFLAGS_} ${.IMPSRC}
.endif
.endif
.elif !empty(MACHINE:M*_solaris_*) && ${CCFAMILY} == "native"
	${_CC_} -xM ${_CCFLAGS_} ${.IMPSRC} > ${.TARGET:R}${DEP_SUFF}
.elif !empty(MACHINE:Mmips_irix_*) && ${CCFAMILY} == "native"
	${_CC_} -M ${_CCFLAGS_} ${.IMPSRC} > ${.TARGET:R}${DEP_SUFF}
.elif !empty(MACHINE:M*_hposs_*) && ${CCFAMILY} == "native"
	${_CC_} -WM ${_CCFLAGS_} ${.IMPSRC} > ${.TARGET:R}${DEP_SUFF}
.endif
.endif

################################################################################
# Yacc suffix compilation rules
#
.if !defined(NO_YACC_RULES)

${YACC_SUFF}${OBJ_SUFF}:
	${_YACC_} ${_YFLAGS_} ${.IMPSRC}
	.rif (${Y_TAB_C} != ${YACC_COMPILE_NAME})
	${YACC_COMPILE_NAME:rm-}
	${MV} ${_MVFLAGS_} ${Y_TAB_C} ${YACC_COMPILE_NAME}
	.rendif
	${_CC_} -c ${_CCFLAGS_} ${YACC_COMPILE_NAME}
.ifndef KEEP_ALL_YACC_FILES
	${YACC_COMPILE_NAME:rm-}
	${YACC_HEADER_NAME:rm-}
.endif
.if defined(STRIP_OBJ)
	${_STRIP_} ${_STRIPFLAGS_} ${.TARGET}
.endif

${YACC_SUFF}${YACC_COMPILE_SUFF}:
	${_YACC_} ${_YFLAGS_} ${.IMPSRC}
	.rif (${Y_TAB_C} != ${YACC_COMPILE_NAME})
	${YACC_COMPILE_NAME:rm-}
	${MV} ${_MVFLAGS_} ${Y_TAB_C} ${YACC_COMPILE_NAME}
	.rendif
	.rif (${Y_TAB_H} != ${YACC_HEADER_NAME})
	${YACC_HEADER_NAME:rm-}
	${MV} ${_MVFLAGS_} ${Y_TAB_H} ${YACC_HEADER_NAME}
	.rendif

.if !defined(NO_Y_H_RULE)
${YACC_SUFF}${YACC_HEADER_SUFF}:
	${_YACC_} ${_YFLAGS_} ${.IMPSRC}
	.rif (${Y_TAB_H} != ${YACC_HEADER_NAME})
	${YACC_HEADER_NAME:rm-}
	${MV} ${_MVFLAGS_} ${Y_TAB_H} ${YACC_HEADER_NAME}
	.rendif
.endif # NO_Y_H_RULE

.endif # NO_YACC_RULES

################################################################################
# Lex suffix compilation rules
#
.if !defined(NO_LEX_RULES)

${LEX_SUFF}${OBJ_SUFF}:
	${_LEX_} ${_LFLAGS_} ${.IMPSRC}
	.rif (${LEX_YY_C} != ${LEX_COMPILE_NAME})
	${LEX_COMPILE_NAME:rm-}
	${MV} ${_MVFLAGS_} ${LEX_YY_C} ${LEX_COMPILE_NAME}
	.rendif
	${_CC_} -c ${_CCFLAGS_} ${LEX_COMPILE_NAME}
.ifndef KEEP_ALL_LEX_FILES
	${LEX_COMPILE_NAME:rm-}
.endif
.if defined(STRIP_OBJ)
	${_STRIP_} ${_STRIPFLAGS_} ${.TARGET}
.endif

${LEX_SUFF}${LEX_COMPILE_SUFF}:
	${_LEX_} ${_LFLAGS_} ${.IMPSRC}
	.rif (${LEX_YY_C} != ${LEX_COMPILE_NAME})
	${LEX_COMPILE_NAME:rm-}
	${MV} ${_MVFLAGS_} ${LEX_YY_C} ${LEX_COMPILE_NAME}
	.rendif

.endif # NO_LEX_RULES

################################################################################
# Preprocess suffix compilation rules
#
.if (${CCFAMILY} == "vage")
PPFLAGS += /Pd
.if !empty(MACHINE:Mx86_nt_*) || !empty(MACHINE:Mx86_95_*)
PPFLAGS += /Pe+
.endif
.elif ${CCFAMILY} == "visual"
PPFLAGS += /EP
.elif ${CCFAMILY} == "gnu"
PPFLAGS += -E -P
.elif !empty(MACHINE:M*_openvms_*)
PPFLAGS += /PREPROC=${.TARGET}
.elif ${CCFAMILY} == "cset" || ((!empty(MACHINE:M*_solaris_*) || \
      !empty(MACHINE:Mhp9000_ux_*) || !empty(MACHINE:Mmips_irix_*)) && \
      ${CCFAMILY} == "native")
PPFLAGS += -P
.if !empty(MACHINE:M*_solaris_*) && ${CCFAMILY} == "native" && \
    (${CCTYPE} == "cc" || ${CCTYPE} == "ansi")
# Option to disable #indent in preprocessed files
PPFLAGS += -Qn
.endif # solaris and CCTYPE==cc
.elif !empty(MACHINE:Mmvs390_oe_*) && ${CCFAMILY} == "native"
PPFLAGS += -E -Wc,nolocale
.elif !empty(MACHINE:Mx86_sco_*) && ${CCFAMILY} == "native"
PPFLAGS += -P -Qn -E
.else
# any other family
PPFLAGS += -E
.endif

${CC_SUFF:S|$|${PP_SUFF}|g}:
.if !empty(MACHINE:M*_openvms_*)
	${$$ ${_CC_} -:L:A${_RESPFILE_}}
	${PPFLAGS:@.WORD.@${${.WORD.} -:L:a${_RESPFILE_}}@}
	${_CCFLAGS_:N-I*:@.WORD.@${${.WORD.} -:L:a${_RESPFILE_}}@}
	${_CCFLAGS_:M-I*:S|^-I||g:@.WORD.@"${.WORD.}",@:S|^|/INC=(|:s|,$|)|:@.WORD.@${${.WORD.} -:L:a${_RESPFILE_}}@}
	${.IMPSRC:@.PATH.@${VMS_PATH_CONV}@:a${_RESPFILE_}}
	$$@${_RESPFILE_:R}
.else
.if ${BACKSLASH_PATHS}
.if defined(USE_RESPFILE)
	${_RESPFILE_:rm-}
	${PPFLAGS:a${_RESPFILE_}}
	${_CCFLAGS_:a${_RESPFILE_}}
	${${.IMPSRC:s|/|\\|g}:L:a${_RESPFILE_}}
	${_CC_} @${_RESPFILE_} > ${.TARGET}
.else
	${_CC_} ${PPFLAGS} ${_CCFLAGS_} ${.IMPSRC:s|/|\\|g} > ${.TARGET}
.endif
.else
	${_CC_} ${PPFLAGS} ${_CCFLAGS_} ${.IMPSRC} > ${.TARGET}
.endif
.if ${CCFAMILY} == "cset" || ((!empty(MACHINE:M*_solaris_*) || \
    !empty(MACHINE:Mhp9000_ux_*) || !empty(MACHINE:Mmips_irix_*)) && \
    ${CCFAMILY} == "native")
	${MV} ${_MVFLAGS_} ${.TARGET:R}.i ${.TARGET}
.endif
.endif


################################################################################
# Add HFILES as a dependent to _ALL_OFILES_
#
.if defined(_ALL_OFILES_) && defined(HFILES)
${_ALL_OFILES_}: ${HFILES}
.endif

.endif
