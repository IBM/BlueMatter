################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines targets for TSO Build tasks
#  Defined targets:
#    TSO_INFO_FILE
#
################################################################################
#
################################################################################
# ASM COBOL PLS and PL/X  rules
# This file defines the rule for ASM,COBOL,PLS and PLX targets
#
# The rule calls a java program which reads a file (mvssrclist.rsp) and 
# creates dynamic JCLs that are submitted via ftp to the OS/390 TSO
# environment.  The java program uses different values from ODE  
# variables to do the processing.
# 
################################################################################
#
# Call OS/390 Build process for traditional languages
.if !defined(_TSO_MVSBLD_MK_)
_TSO_MVSBLD_MK_=
 
################################################################################
# Define the ASM,COBOL,PLS,PLX-related suffixes
#
.SUFFIXES : ${ASM_SUFF} ${CBL_SUFF} ${PLS_SUFF} ${PLX_SUFF}

################################################################################
# ASM COBOL PLS PLX rule
# Suffix transformation rule for .asm .cbl .cob .cobol .pls .plx --> .o
#
${ASM_SUFF}${OBJ_SUFF} ${CBL_SUFF:S|$|${OBJ_SUFF}|g} ${PLS_SUFF}${OBJ_SUFF} ${PLX_SUFF}${OBJ_SUFF} : ${TSO_SRC_FILE:rm-} 
.if !empty(MACHINE:Mx86_os2_*) || !empty(MACHINE:Mx86_nt_*) || \
    !empty(MACHINE:Mx86_95_*)
	${.IMPSRC:S;/;\\;g:a${TSO_SRC_FILE}}
.else
	${.IMPSRC:a${TSO_SRC_FILE}}
.endif

# The rule to create the file
${TSO_INFO_FILE} : ${OBJECTS} ${TSO_LOCAL_INCLUDES} ${TSO_EXPORT_INCLUDES} 
	.rif !empty(.OODATE:M*${OBJ_SUFF})
	${TSO_ALLOCJCL_FILE:rm-} 
	${TSO_HFSJCL_FILE:rm-} 
	.if empty(MACHINE:Mx86_os2_*) && empty(MACHINE:Mx86_nt_*) && \
	    empty(MACHINE:Mx86_95_*)
	 ${RM} ${_RMFLAGS_} ${TSO_COMPILEJCL_FILES} 
	.endif
	${.TARGET:rm-} 
	${CP} ${_CPFLAGS_} ${TSO_SRC_FILE} ${.TARGET} 
	.if !empty(MACHINE:Mx86_os2_*) || !empty(MACHINE:Mx86_nt_*) || \
	    !empty(MACHINE:Mx86_95_*)
	 ${.ALLSRC:S;/;\\;g:N*${OBJ_SUFF}:@.FILE.@${.FILE.:a${.TARGET}}@}
	.else
	 ${.ALLSRC:N*${OBJ_SUFF}:@.FILE.@${.FILE.:a${.TARGET}}@}
	.endif
	${JVM} ${_BUILDTSOFLAGS_} com.ibm.ode.bin.bldMvs.RunMvsBld
	${dummy:a${.TARGET}}
	.rendif
.endif # defined(TSO_MVSBLD_MK)
