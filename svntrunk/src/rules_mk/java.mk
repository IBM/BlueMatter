################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines the java targets and suffix transformations, and
# is organized in the following order:
#    1. Special targets
#    2. Normal targets & suffix transforms
#    3. All variables and targets for native2ascii
# See also java.std.mk for variable definitions.
#
################################################################################
.if !defined(__JAVA_MK_)
__JAVA_MK_=


################################################################################
# SPECIAL TARGETS
################################################################################

################################################################################
# Define the Java-related suffixes
#
.SUFFIXES: ${JAVA_SUFF} ${CLASS_SUFF} \
           ${JAR_SUFF} ${EAR_SUFF} ${WAR_SUFF} \
           ${XML_SUFF} ${JS_SUFF}


################################################################################
# Create specialized search paths
#
.PATH${CLASS_SUFF} : ${CLASS_SEARCH_PATHS}

# Only allow make to find the jar file if one of the actions
# that cause installation occurs OR if the user has included
# dependents in the JAR_OBJECTS variable.  If only
# OTHER_JAR_OBJECTS is being used, make will not find the
# jar file and thus always recreate it (desirable over never
# recreating it).
#
.if make(install_all) || make(instdep_all) || make(gather_all) || \
    make(package_all) || !empty(JAR_OBJECTS)
.PATH${JAR_SUFF} : ${JAR_RUNDIR} ${CLASSBASES:S/${PATHSEP}/ /g}
.endif

.if make(install_all) || make(instdep_all) || make(gather_all) || \
    make(package_all) || !empty(EAR_OBJECTS)
.PATH${EAR_SUFF} : ${EAR_RUNDIR} ${CLASSBASES:S/${PATHSEP}/ /g}
.endif

.if make(install_all) || make(instdep_all) || make(gather_all) || \
    make(package_all) || !empty(WAR_OBJECTS)
.PATH${WAR_SUFF} : ${WAR_RUNDIR} ${CLASSBASES:S/${PATHSEP}/ /g}
.endif


################################################################################
# Rules for creating javadocs
#
.if defined(SPECIAL_PASSES)
javadoc_all: JAVADOC .SPECTARG
.else
javadoc_all: $${_all_targets_} .SPECTARG;@
.endif

JAVADOC_TARGETS: .SPECTARG
	${JAVADOC_GENDIR:C}
.if !empty(JAVADOCS)
.ifdef USE_JAVA_RESPFILE
	${_JAVADOCFLAGS_:A${.TARGET}.tmp}
	${JAVADOCS:M*${JAVA_SUFF}:@WORD@${WORD:p:a${.TARGET}.tmp}@}
	${JAVADOCS:N*${JAVA_SUFF}:@WORD@${WORD:a${.TARGET}.tmp}@}
	${_JAVADOC_} ${_JVM_JAVADOCFLAGS_} @${.TARGET}.tmp
.else
	${_JAVADOC_} ${_JAVADOCFLAGS_} \
	       ${JAVADOCS:M*${JAVA_SUFF}:@WORD@${JAVA_QUOTE}${WORD:p}${JAVA_QUOTE}@} \
	       ${JAVADOCS:N*${JAVA_SUFF}:@WORD@${JAVA_QUOTE}${WORD}${JAVA_QUOTE}@}
.endif
.endif


################################################################################
# NORMAL TARGETS & SUFFIX TRANSFORMS
################################################################################

################################################################################
# Suffix transformation rule for .java --> .class
#
${JAVA_SUFF}${CLASS_SUFF}:
	${_CLASSGENDIR_:C}
.ifdef JAVA_DEPENDENCIES
	-${_JAVADEP_} ${_JAVADEPFLAGS_} ${JAVA_QUOTE}${IMPJAVASRC}${JAVA_QUOTE}
.endif
.if !empty(MACHINE:M*_openvms_*)
	${_JAVAC_} -d ${_CLASSGENDIR_} ${_JFLAGS_} \
             "${IMPJAVASRC:H}/${JAVA_CLASSES:S|${CLASS_SUFF}$|${JAVA_SUFF}|g:xM|${IMPJAVASRC:T}|i}"
.else
	${_JAVAC_} -d ${_CLASSGENDIR_} ${_JFLAGS_} \
             ${JAVA_QUOTE}${IMPJAVASRC}${JAVA_QUOTE}
.endif


################################################################################
# Rules for creating class files
#
.if empty(JAVA_CLASSES) && !defined(USE_JAVA_CLASSES)
# do nothing
JAVAC_TARGETS: .SPECTARG ;@
.elif defined(JAVA_DEPENDENCIES) || defined(JAVAC_MULTICALL)
JAVAC_TARGETS: $${JAVA_CLASSES} .SPECTARG
# no commands here...the suffix transform is used
.else
.ifdef GROUP_JAVA_DEPS
JAVAC_TARGETS: ${JAVA_CLASSES} .SPECTARG
.LINKTARGS : ${JAVA_CLASSES}
${JAVA_CLASSES} : ${JAVA_CLASSES:S/${CLASS_SUFF}$/${JAVA_SUFF}/g}
.else
JAVAC_TARGETS: $${JAVA_CLASSES:S/${CLASS_SUFF}$$/${JAVA_SUFF}/g} .SPECTARG
.endif
	${_CLASSGENDIR_:C}
.ifdef USE_JAVA_RESPFILE
	${-d ${_CLASSGENDIR_} ${_JFLAGS_}:L:A${.TARGET:T}.tmp}
.if !empty(MACHINE:M*_openvms_*)
	${JAVAC_FILE_ARGS:@WORD@${WORD:H:S|$|/|:s|$|${JAVA_CLASSES:S|${CLASS_SUFF}$|${JAVA_SUFF}|g:xM|${WORD:T}|i}|:a${.TARGET:T}.tmp}@}
.else
	${JAVAC_FILE_ARGS:@WORD@${WORD:a${.TARGET:T}.tmp}@}
.endif
	${_JAVAC_} ${_JVM_JFLAGS_} @${.TARGET:T}.tmp
.else
.if !empty(MACHINE:M*_openvms_*)
	${_JAVAC_} -d ${_CLASSGENDIR_} ${_JFLAGS_} \
	           ${JAVAC_FILE_ARGS:@WORD@${WORD:H:S|$|/|:s|$|${JAVA_CLASSES:S|${CLASS_SUFF}$|${JAVA_SUFF}|g:xM|${WORD:T}|i}|}@:@WORD@"${WORD}"@}
.else
	${_JAVAC_} -d ${_CLASSGENDIR_} ${_JFLAGS_} \
             ${JAVAC_FILE_ARGS:@WORD@${JAVA_QUOTE}${WORD}${JAVA_QUOTE}@}
.endif # openvms
.endif # USE_JAVA_RESPFILE
.endif # JAVA_CLASSES / JAVA_DEPENDENCIES


################################################################################
# Rules for creating dependency files.
#
.if defined(JAVA_DEPENDENCIES) && !empty(JAVA_CLASSES)
.ifdef JAVADEP_MULTICALL
JAVADEP_TARGETS: ${JAVA_CLASSES:S/${CLASS_SUFF}$/${DEP_SUFF}/g} .SPECTARG
${JAVA_SUFF}${DEP_SUFF}:
	${_JAVADEP_} ${_JAVADEPFLAGS_} ${JAVA_QUOTE}${IMPJAVASRC}${JAVA_QUOTE}
.else
JAVADEP_TARGETS: ${JAVA_CLASSES:S/${CLASS_SUFF}$/${JAVA_SUFF}/g} .SPECTARG
.ifdef USE_JAVA_RESPFILE
	${${.TARGET:T}.tmp:L:rm-}
	${JAVADEP_FILE_ARGS:@WORD@${WORD:a${.TARGET:T}.tmp}@}
	-${_JAVADEP_} ${_JAVADEPFLAGS_} @${.TARGET:T}.tmp
.else
	-${_JAVADEP_} ${_JAVADEPFLAGS_} \
                ${JAVADEP_FILE_ARGS:@WORD@${JAVA_QUOTE}${WORD}${JAVA_QUOTE}@}
.endif
.endif
.else
JAVADEP_TARGETS: .SPECTARG
# no commands...don't do anything if JAVA_DEPENDENCIES isn't defined
.endif


################################################################################
# Rules for creating jar files
#
.if !empty(JAR_LIBRARIES)
${JAR_LIBRARIES} : $${_JAR_OBJECTS_}
	${_JAR_RUNDIR_:C}
.if defined(USE_JAR_UPDATE)
.if !empty(BACKED_SANDBOXDIR:S|^${SANDBOXBASE}||)
	${CHDIR} ${_JAR_RUNDIR_} ${CMDSEP} ${MAKELINKS} -auto -copy \
     ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE}
.else
	.rif !exists(${_JAR_RUNDIR_}${DIRSEP}${.TARGET})
	${CP} ${_CPFLAGS_} ${NULL_DEVICE} \
        ${JAVA_QUOTE}${_JAR_RUNDIR_}${DIRSEP}${.TARGET}${JAVA_QUOTE}
	.rendif
.endif
.endif
.ifdef USE_JAVA_RESPFILE
	${_JARFLAGS_:A${_JAR_RESPFILE_}}
	${.TARGET:a${_JAR_RESPFILE_}}
	${_JAR_MANIFEST_:b${_JAR_MANIFEST_:p:a${_JAR_RESPFILE_}}}
	${_JAR_FILE_ARGS_:@WORD@${WORD:a${_JAR_RESPFILE_}}@}
	${CHDIR} ${_JAR_RUNDIR_} ${CMDSEP} ${_JAR_} \
     ${_JVM_JARFLAGS_} @${JAVA_QUOTE}${_JAR_RESPFILE_}${JAVA_QUOTE}
.else
	${CHDIR} ${_JAR_RUNDIR_} ${CMDSEP} ${_JAR_} ${_JARFLAGS_} \
     ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE} \
     ${_JAR_MANIFEST_:b${JAVA_QUOTE}${_JAR_MANIFEST_:p}${JAVA_QUOTE}} \
     ${_JAR_FILE_ARGS_}
.endif 
.endif 


################################################################################
# Rules for creating signed jar files
#
.if !empty(SIGNED_JAR_LIBRARIES)
${SIGNED_JAR_LIBRARIES} : $${_SIGNED_JAR_OBJECTS_}
	${_JAR_RUNDIR_:C}
.if defined(USE_JAR_UPDATE)
.if !empty(BACKED_SANDBOXDIR:S|^${SANDBOXBASE}||)
	${CHDIR} ${_JAR_RUNDIR_} ${CMDSEP} ${MAKELINKS} -auto -copy \
     ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE}
.else
	.rif !exists(${_JAR_RUNDIR_}${DIRSEP}${.TARGET})
	${CP} ${_CPFLAGS_} ${NULL_DEVICE} \
        ${JAVA_QUOTE}${_JAR_RUNDIR_}${DIRSEP}${.TARGET}${JAVA_QUOTE}
	.rendif
.endif
.endif
.ifdef USE_JARSIGNER
.ifdef USE_JAVA_RESPFILE
	${_JARFLAGS_:A${_JAR_RESPFILE_}}
	${.TARGET:a${_JAR_RESPFILE_}}
	${_SIGNED_JAR_MANIFEST_:b${_SIGNED_JAR_MANIFEST_:p:a${_JAR_RESPFILE_}}}
	${_SIGNED_JAR_FILE_ARGS_:@WORD@${WORD:a${_JAR_RESPFILE_}}@}
	${CHDIR} ${_JAR_RUNDIR_} ${CMDSEP} ${_JAR_} ${_JVM_JARFLAGS_} @${_JAR_RESPFILE_}
.else
	${CHDIR} ${_JAR_RUNDIR_} ${CMDSEP} ${_JAR_} ${_JARFLAGS_} \
     ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE} \
     ${_SIGNED_JAR_MANIFEST_:b${JAVA_QUOTE}${_SIGNED_JAR_MANIFEST_:p}${JAVA_QUOTE}} \
     ${_SIGNED_JAR_FILE_ARGS_}
.endif
	${CHDIR} ${_JAR_RUNDIR_} ${CMDSEP} \
     ${_JAVAKEY_} ${_JAVAKEYFLAGS_} ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE} \
     ${JAVA_SIGN_SIGNER}
.else
.if defined(USE_JAR_UPDATE)
	${CP} ${_CPFLAGS_} \
        ${JAVA_QUOTE}${_JAR_RUNDIR_}${DIRSEP}${.TARGET}${JAVA_QUOTE} \
        ${JAVA_QUOTE}${_JAR_RUNDIR_}${DIRSEP}${SIGNED_JAR_JARFILE}${JAVA_QUOTE}
.endif
	${CHDIR} ${_JAR_RUNDIR_} ${CMDSEP} \
     ${_JAR_} ${_JARFLAGS_} ${JAVA_QUOTE}${SIGNED_JAR_JARFILE}${JAVA_QUOTE} \
     ${_SIGNED_JAR_MANIFEST_:b${JAVA_QUOTE}${_SIGNED_JAR_MANIFEST_:p}${JAVA_QUOTE}} \
     ${_SIGNED_JAR_FILE_ARGS_}
	${signer=${JAVA_SIGN_SIGNER}:L:A${_JAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}}
	${cert=${JAVA_SIGN_CERT}:L:a${_JAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}}
	${chain=${JAVA_SIGN_CHAIN}:L:a${_JAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}}
	${signature.file=${JAVA_SIGN_SIGFILE}:L:a${_JAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}}
	${out.file=${.TARGET}:L:a${_JAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}}
	${CHDIR} ${_JAR_RUNDIR_} ${CMDSEP} \
     ${_JAVAKEY_} ${_JAVAKEYFLAGS_} \
     ${JAVA_QUOTE}${JAVAKEY_DIRECT_FILE}${JAVA_QUOTE} \
     ${JAVA_QUOTE}${SIGNED_JAR_JARFILE}${JAVA_QUOTE}
	${${_JAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}:L:rm-}
	${${_JAR_RUNDIR_}/${SIGNED_JAR_JARFILE}:L:rm-}
.endif
.endif


################################################################################
# Rules for creating ear files
#
.if !empty(EAR_LIBRARIES)
${EAR_LIBRARIES} : $${_EAR_OBJECTS_}
	${_EAR_RUNDIR_:C}
.if defined(USE_EAR_UPDATE)
.if !empty(BACKED_SANDBOXDIR:S|^${SANDBOXBASE}||)
	${CHDIR} ${_EAR_RUNDIR_} ${CMDSEP} ${MAKELINKS} -auto -copy \
     ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE}
.else
	.rif !exists(${_EAR_RUNDIR_}${DIRSEP}${.TARGET})
	${CP} ${_CPFLAGS_} ${NULL_DEVICE} \
        ${JAVA_QUOTE}${_EAR_RUNDIR_}${DIRSEP}${.TARGET}${JAVA_QUOTE}
	.rendif
.endif
.endif
.ifdef USE_JAVA_RESPFILE
	${_EARFLAGS_:A${_EAR_RESPFILE_}}
	${.TARGET:a${_EAR_RESPFILE_}}
	${_EAR_MANIFEST_:b${_EAR_MANIFEST_:p:a${_EAR_RESPFILE_}}}
	${_EAR_FILE_ARGS_:@WORD@${WORD:a${_EAR_RESPFILE_}}@}
	${CHDIR} ${_EAR_RUNDIR_} ${CMDSEP} ${_EAR_} \
     ${_JVM_EARFLAGS_} @${JAVA_QUOTE}${_EAR_RESPFILE_}${JAVA_QUOTE}
.else
	${CHDIR} ${_EAR_RUNDIR_} ${CMDSEP} ${_EAR_} ${_EARFLAGS_} \
     ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE} \
     ${_EAR_MANIFEST_:b${JAVA_QUOTE}${_EAR_MANIFEST_:p}${JAVA_QUOTE}} \
     ${_EAR_FILE_ARGS_}
.endif 
.endif 


################################################################################
# Rules for creating signed ear files
#
.if !empty(SIGNED_EAR_LIBRARIES)
${SIGNED_EAR_LIBRARIES} : $${_SIGNED_EAR_OBJECTS_}
	${_EAR_RUNDIR_:C}
.if defined(USE_EAR_UPDATE)
.if !empty(BACKED_SANDBOXDIR:S|^${SANDBOXBASE}||)
	${CHDIR} ${_EAR_RUNDIR_} ${CMDSEP} ${MAKELINKS} -auto -copy \
     ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE}
.else
	.rif !exists(${_EAR_RUNDIR_}${DIRSEP}${.TARGET})
	${CP} ${_CPFLAGS_} ${NULL_DEVICE} \
        ${JAVA_QUOTE}${_EAR_RUNDIR_}${DIRSEP}${.TARGET}${JAVA_QUOTE}
	.rendif
.endif
.endif
.ifdef USE_EARSIGNER
.ifdef USE_JAVA_RESPFILE
	${_EARFLAGS_:A${_EAR_RESPFILE_}}
	${.TARGET:a${_EAR_RESPFILE_}}
	${_SIGNED_EAR_MANIFEST_:b${_SIGNED_EAR_MANIFEST_:p:a${_EAR_RESPFILE_}}}
	${_SIGNED_EAR_FILE_ARGS_:@WORD@${WORD:a${_EAR_RESPFILE_}}@}
	${CHDIR} ${_EAR_RUNDIR_} ${CMDSEP} ${_EAR_} ${_JVM_EARFLAGS_} @${_EAR_RESPFILE_}
.else
	${CHDIR} ${_EAR_RUNDIR_} ${CMDSEP} ${_EAR_} ${_EARFLAGS_} \
     ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE} \
     ${_SIGNED_EAR_MANIFEST_:b${JAVA_QUOTE}${_SIGNED_EAR_MANIFEST_:p}${JAVA_QUOTE}} \
     ${_SIGNED_EAR_FILE_ARGS_}
.endif
	${CHDIR} ${_EAR_RUNDIR_} ${CMDSEP} \
     ${_JAVAKEY_} ${_JAVAKEYFLAGS_} ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE} \
     ${JAVA_SIGN_SIGNER}
.else
.if defined(USE_EAR_UPDATE)
	${CP} ${_CPFLAGS_} \
        ${JAVA_QUOTE}${_EAR_RUNDIR_}${DIRSEP}${.TARGET}${JAVA_QUOTE} \
        ${JAVA_QUOTE}${_EAR_RUNDIR_}${DIRSEP}${SIGNED_EAR_EARFILE}${JAVA_QUOTE}
.endif
	${CHDIR} ${_EAR_RUNDIR_} ${CMDSEP} \
     ${_EAR_} ${_EARFLAGS_} ${JAVA_QUOTE}${SIGNED_EAR_EARFILE}${JAVA_QUOTE} \
     ${_SIGNED_EAR_MANIFEST_:b${JAVA_QUOTE}${_SIGNED_EAR_MANIFEST_:p}${JAVA_QUOTE}} \
     ${_SIGNED_EAR_FILE_ARGS_}
	${signer=${JAVA_SIGN_SIGNER}:L:A${_EAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}}
	${cert=${JAVA_SIGN_CERT}:L:a${_EAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}}
	${chain=${JAVA_SIGN_CHAIN}:L:a${_EAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}}
	${signature.file=${JAVA_SIGN_SIGFILE}:L:a${_EAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}}
	${out.file=${.TARGET}:L:a${_EAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}}
	${CHDIR} ${_EAR_RUNDIR_} ${CMDSEP} \
     ${_JAVAKEY_} ${_JAVAKEYFLAGS_} \
     ${JAVA_QUOTE}${JAVAKEY_DIRECT_FILE}${JAVA_QUOTE} \
     ${JAVA_QUOTE}${SIGNED_EAR_EARFILE}${JAVA_QUOTE}
	${${_EAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}:L:rm-}
	${${_EAR_RUNDIR_}/${SIGNED_EAR_EARFILE}:L:rm-}
.endif
.endif


################################################################################
# Rules for creating war files
#
.if !empty(WAR_LIBRARIES)
${WAR_LIBRARIES} : $${_WAR_OBJECTS_}
	${_WAR_RUNDIR_:C}
.if defined(USE_WAR_UPDATE)
.if !empty(BACKED_SANDBOXDIR:S|^${SANDBOXBASE}||)
	${CHDIR} ${_WAR_RUNDIR_} ${CMDSEP} ${MAKELINKS} -auto -copy \
     ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE}
.else
	.rif !exists(${_WAR_RUNDIR_}${DIRSEP}${.TARGET})
	${CP} ${_CPFLAGS_} ${NULL_DEVICE} \
        ${JAVA_QUOTE}${_WAR_RUNDIR_}${DIRSEP}${.TARGET}${JAVA_QUOTE}
	.rendif
.endif
.endif
.ifdef USE_JAVA_RESPFILE
	${_WARFLAGS_:A${_WAR_RESPFILE_}}
	${.TARGET:a${_WAR_RESPFILE_}}
	${_WAR_MANIFEST_:b${_WAR_MANIFEST_:p:a${_WAR_RESPFILE_}}}
	${_WAR_FILE_ARGS_:@WORD@${WORD:a${_WAR_RESPFILE_}}@}
	${CHDIR} ${_WAR_RUNDIR_} ${CMDSEP} ${_WAR_} \
     ${_JVM_WARFLAGS_} @${JAVA_QUOTE}${_WAR_RESPFILE_}${JAVA_QUOTE}
.else
	${CHDIR} ${_WAR_RUNDIR_} ${CMDSEP} ${_WAR_} ${_WARFLAGS_} \
     ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE} \
     ${_WAR_MANIFEST_:b${JAVA_QUOTE}${_WAR_MANIFEST_:p}${JAVA_QUOTE}} \
     ${_WAR_FILE_ARGS_}
.endif 
.endif 


################################################################################
# Rules for creating signed war files
#
.if !empty(SIGNED_WAR_LIBRARIES)
${SIGNED_WAR_LIBRARIES} : $${_SIGNED_WAR_OBJECTS_}
	${_WAR_RUNDIR_:C}
.if defined(USE_WAR_UPDATE)
.if !empty(BACKED_SANDBOXDIR:S|^${SANDBOXBASE}||)
	${CHDIR} ${_WAR_RUNDIR_} ${CMDSEP} ${MAKELINKS} -auto -copy \
     ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE}
.else
	.rif !exists(${_WAR_RUNDIR_}${DIRSEP}${.TARGET})
	${CP} ${_CPFLAGS_} ${NULL_DEVICE} \
        ${JAVA_QUOTE}${_WAR_RUNDIR_}${DIRSEP}${.TARGET}${JAVA_QUOTE}
	.rendif
.endif
.endif
.ifdef USE_WARSIGNER
.ifdef USE_JAVA_RESPFILE
	${_WARFLAGS_:A${_WAR_RESPFILE_}}
	${.TARGET:a${_WAR_RESPFILE_}}
	${_SIGNED_WAR_MANIFEST_:b${_SIGNED_WAR_MANIFEST_:p:a${_WAR_RESPFILE_}}}
	${_SIGNED_WAR_FILE_ARGS_:@WORD@${WORD:a${_WAR_RESPFILE_}}@}
	${CHDIR} ${_WAR_RUNDIR_} ${CMDSEP} ${_WAR_} ${_JVM_WARFLAGS_} @${_WAR_RESPFILE_}
.else
	${CHDIR} ${_WAR_RUNDIR_} ${CMDSEP} ${_WAR_} ${_WARFLAGS_} \
     ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE} \
     ${_SIGNED_WAR_MANIFEST_:b${JAVA_QUOTE}${_SIGNED_WAR_MANIFEST_:p}${JAVA_QUOTE}} \
     ${_SIGNED_WAR_FILE_ARGS_}
.endif
	${CHDIR} ${_WAR_RUNDIR_} ${CMDSEP} \
     ${_JAVAKEY_} ${_JAVAKEYFLAGS_} ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE} \
     ${JAVA_SIGN_SIGNER}
.else
.if defined(USE_WAR_UPDATE)
	${CP} ${_CPFLAGS_} \
        ${JAVA_QUOTE}${_WAR_RUNDIR_}${DIRSEP}${.TARGET}${JAVA_QUOTE} \
        ${JAVA_QUOTE}${_WAR_RUNDIR_}${DIRSEP}${SIGNED_WAR_WARFILE}${JAVA_QUOTE}
.endif
	${CHDIR} ${_WAR_RUNDIR_} ${CMDSEP} \
     ${_WAR_} ${_WARFLAGS_} ${JAVA_QUOTE}${SIGNED_WAR_WARFILE}${JAVA_QUOTE} \
     ${_SIGNED_WAR_MANIFEST_:b${JAVA_QUOTE}${_SIGNED_WAR_MANIFEST_:p}${JAVA_QUOTE}} \
     ${_SIGNED_WAR_FILE_ARGS_}
	${signer=${JAVA_SIGN_SIGNER}:L:A${_WAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}}
	${cert=${JAVA_SIGN_CERT}:L:a${_WAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}}
	${chain=${JAVA_SIGN_CHAIN}:L:a${_WAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}}
	${signature.file=${JAVA_SIGN_SIGFILE}:L:a${_WAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}}
	${out.file=${.TARGET}:L:a${_WAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}}
	${CHDIR} ${_WAR_RUNDIR_} ${CMDSEP} \
     ${_JAVAKEY_} ${_JAVAKEYFLAGS_} \
     ${JAVA_QUOTE}${JAVAKEY_DIRECT_FILE}${JAVA_QUOTE} \
     ${JAVA_QUOTE}${SIGNED_WAR_WARFILE}${JAVA_QUOTE}
	${${_WAR_RUNDIR_}/${JAVAKEY_DIRECT_FILE}:L:rm-}
	${${_WAR_RUNDIR_}/${SIGNED_WAR_WARFILE}:L:rm-}
.endif
.endif


################################################################################
# Rules for creating JNI headers/stubs
#
.ifndef NO_JAVAH_SUFFRULES
${CLASS_SUFF}.h : 
	${_JAVAH_} ${_JAVAHFLAGS_} -o ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE} \
             ${JAVA_QUOTE}${IMPJAVASRC:T:R}${JAVA_QUOTE}

${CLASS_SUFF}.c : 
	${_JAVAH_} -stubs ${_JAVAHFLAGS_} -o ${JAVA_QUOTE}${.TARGET}${JAVA_QUOTE} \
             ${JAVA_QUOTE}${IMPJAVASRC:T:R}${JAVA_QUOTE}
.endif

.if !empty(JAVAH_TARGETS)
${JAVAH_TARGETS}: $${JAVAH_SOURCES}
	${.TARGET:rm-}
	${_JAVAH_} ${_JAVAHFLAGS_} \
	           ${ALLJAVASRC:T:S/^/${JAVA_QUOTE}${_JAVA_PACKAGE_NAME_}./g:S/${CLASS_SUFF}$/${JAVA_QUOTE}/g}
.endif


################################################################################
# Rules for creating rmi skeleton/stub classes
#
.if !empty(RMI_TARGETS)
${RMI_TARGETS} : $${RMI_SOURCES}
	${RMIC_GENDIR:C}
	${_RMIC_} ${_RMICFLAGS_} \
            ${ALLJAVASRC:T:S/^/${JAVA_QUOTE}${_JAVA_PACKAGE_NAME_}./g:S/${CLASS_SUFF}$/${JAVA_QUOTE}/g}
.endif


################################################################################
# Rules to handle Java property files
#

# Suffix definitions for .prop and .properties
.SUFFIXES: ${PROP_SUFF} ${PROPSRC_SUFF}

# Search path for .properties files
.if !empty(_PROPERTIES_) && !defined(PROPERTIES_SAMESUFF)
.PATH${PROP_SUFF}: ${CLASS_SEARCH_PATHS}
.endif

.ifdef PROPERTIES_SAMESUFF
${_PROPERTIES_} : $${_PROP_TGT_}

${_PROPERTIES_:@.TARGET@${_PROP_TGT_}@} : $${.TARGET:T}
	${.TARGET:H:C}
	${.TARGET:rm-}
	${_N2A_} ${_N2AFLAGS_} ${JAVA_QUOTE}${ALLJAVASRC}${JAVA_QUOTE} \
           ${JAVA_QUOTE}${.TARGET:S|/|${DIRSEP}|g}${JAVA_QUOTE}
.else
# Suffix transformation from .prop->.properties
# Get source from src directory and put target in export/classes
${PROPSRC_SUFF:S|$|${PROP_SUFF}|g} :
	${_PROPERTIESGENDIR_:C}
	${_PROP_TGT_:rm-}
	${_N2A_} ${_N2AFLAGS_} ${JAVA_QUOTE}${IMPJAVASRC}${JAVA_QUOTE} \
           ${JAVA_QUOTE}${_PROP_TGT_}${JAVA_QUOTE}
.endif


.endif  # __JAVA_MK_
