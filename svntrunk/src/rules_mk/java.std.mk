################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines the java variables used by java.mk.
#
################################################################################
.if !defined(__JAVA_STD_MK_)
__JAVA_STD_MK_=


################################################################################
# Miscellaneous variables
#

.ifdef QUOTE_JAVA_FILES
JAVA_QUOTE ?= ${FILE_QUOTE}
.else
.undef JAVA_QUOTE
.endif

# Java suffixes
#
JAVA_SUFF    ?= .java
CLASS_SUFF   ?= .class
JAR_SUFF     ?= .jar
PROP_SUFF    ?= .properties
PROPSRC_SUFF ?= .prop
XML_SUFF     ?= .xml
JS_SUFF      ?= .js
EAR_SUFF     ?= .ear
WAR_SUFF     ?= .war

JAVA_CLASSES ?= ${JAVASRC:S/${JAVA_SUFF}$/${CLASS_SUFF}/g}

.if !empty(JAVA_CLASSES_DIRS)
_SBLS_RUN_DIR_ = ${SANDBOXBASE}${DIRSEP}${ODESRCNAME}${MAKEDIR:S|/$||:S|/|${DIRSEP}|g}
JAVA_CLASSES += ${_SBLS_RUN_DIR_:C:!${CHDIR} ${_SBLS_RUN_DIR_} ${CMDSEP} \
                sbls -p${USE_JAVADIR_RECURSION:DR} \
                ${JAVA_CLASSES_DIRS}!:M*${JAVA_SUFF}:S/${JAVA_SUFF}$/${CLASS_SUFF}/g}
.endif

# Java Virtual Machine (mainly for packaging tools)
JVM		    ?= java
# Used by no need to set it here (no default)
#JVMFLAGS	    ?=

# if javadoc_all is the target, imply BUILDJAVADOCS also
.if make(javadoc_all)
BUILDJAVADOCS =
.endif

CLASSGEN_SUBDIR     ?= ${DIRSEP}export${DIRSEP}classes
JAVA_PACKAGE_NAME   ?= ${MAKEDIR:S!^/!!g:S!/!.!g}
_JAVA_PACKAGE_NAME_  = ${${.TARGET}_JAVA_PACKAGE_NAME:U${JAVA_PACKAGE_NAME}}
CLASSGENDIR	    ?= ${SANDBOXBASE}${CLASSGEN_SUBDIR}
CLASSBASES	    ?= ${BACKED_SANDBOXDIR:S!${PATHSEP}!${CLASSGEN_SUBDIR}${PATHSEP}!g}${CLASSGEN_SUBDIR}${PATHSEP}
.if ${BACKSLASH_PATHS}
ALLJAVASRC    = ${.ALLSRC:S;/;\\;g}
IMPJAVASRC    = ${.IMPSRC:S;/;\\;g}
_CLASSGENDIR_ = ${CLASSGENDIR:S;/;\\;g}
.else
ALLJAVASRC    = ${.ALLSRC}
IMPJAVASRC    = ${.IMPSRC}
_CLASSGENDIR_ = ${CLASSGENDIR} 
.endif

JAVAC_FILE_ARGS           ?= ${ALLJAVASRC}
JAVA_SRCDIR_GENPATH_FLAGS ?= -S

################################################################################
# Variables for the compiler tool (javac)
#
JAVAC       ?= javac
_JAVAC_     ?= ${JAVAC}

_JVM_JFLAGS_ = ${${.TARGET}_JVM_JFLAGS:U${JVM_JFLAGS}}

.ifdef JAVAC_SRCDIR

.ifdef USE_GENPATH_FOR_JAVA_SRCDIR
_JAVAC_SRCDIR_ = ${JAVAC_SRCDIR:S/${PATHSEP}/ /g:@WORD@-I${WORD}@:S/$/ -V ${JAVA_SRCDIR_GENPATH_FLAGS}/:G}
.else
_JAVAC_SRCDIR_ = ${JAVAC_SRCDIR}
.endif

_JFLAGS_     = ${${.TARGET}_JENV:U${JENV}} \
	       ${${.TARGET}_JFLAGS:U${JFLAGS}} \
	       ${${.TARGET}_JARGS:U${JARGS}} \
	       -classpath ${_JAVAC_SRCDIR_}

.else # JAVAC_SRCDIR

_JFLAGS_     = ${${.TARGET}_JENV:U${JENV}} \
	       ${${.TARGET}_JFLAGS:U${JFLAGS}} \
	       ${${.TARGET}_JARGS:U${JARGS}}

.endif # JAVAC_SRCDIR

.ifdef DEBUGGING
JAVA_OPT_LEVEL ?= -g
_JFLAGS_ += ${JAVA_OPT_LEVEL}
.endif

.ifndef USE_JAVA_RESPFILE
_JFLAGS_ += ${_JVM_JFLAGS_}
.endif


################################################################################
# Variables for the dependency generation tool
#
JAVADEP		          ?= jikes
_JAVADEP_	          ?= ${JAVADEP}
_JAVADEPFLAGS_       = ${${.TARGET}_JAVADEPFLAGS:U${JAVADEPFLAGS}}
JAVADEP_FILE_ARGS   ?= ${ALLJAVASRC}

_JVM_JAVADEPFLAGS_ = ${${.TARGET}_JVM_JAVADEPFLAGS:U${JVM_JAVADEPFLAGS}}

.ifdef JAVADEP_SRCDIR

.ifdef USE_GENPATH_FOR_JAVA_SRCDIR
_JAVADEP_SRCDIR_ = ${JAVADEP_SRCDIR:S/${PATHSEP}/ /g:@WORD@-I${WORD}@:S/$/ -V ${JAVA_SRCDIR_GENPATH_FLAGS}/:G}
.else
_JAVADEP_SRCDIR_ = ${JAVADEP_SRCDIR}
.endif

JAVADEPFLAGS     ?= +M +F +B -nowrite -nowarn >${NULL_DEVICE} \
		    -classpath ${_JAVADEP_SRCDIR_}

.else # JAVADEP_SRCDIR

JAVADEPFLAGS     ?= +M +F +B -nowrite -nowarn >${NULL_DEVICE}

.endif # JAVADEP_SRCDIR

.ifndef USE_JAVA_RESPFILE
_JAVADEPFLAGS_ += ${_JVM_JAVADEPFLAGS_}
.endif


################################################################################
# Variables for the archive tool (jar)
#
JAR		       ?= jar
_JAR_		     ?= ${JAR}
EAR		       ?= jar
_EAR_		     ?= ${EAR}
WAR		       ?= jar
_WAR_		     ?= ${WAR}

.if defined(USE_JAR_UPDATE)
.if !empty(JAR_MANIFEST)
JARFLAGS	   ?= -uvfm
.else
JARFLAGS	   ?= -uvf
.endif
.else
.if !empty(JAR_MANIFEST)
JARFLAGS	   ?= -cvfm
.else
JARFLAGS	   ?= -cvf
.endif
.endif

.if defined(USE_EAR_UPDATE)
.if !empty(EAR_MANIFEST)
EARFLAGS	   ?= -uvfm
.else
EARFLAGS	   ?= -uvf
.endif
.else
.if !empty(EAR_MANIFEST)
EARFLAGS	   ?= -cvfm
.else
EARFLAGS	   ?= -cvf
.endif
.endif

.if defined(USE_WAR_UPDATE)
.if !empty(WAR_MANIFEST)
WARFLAGS	   ?= -uvfm
.else
WARFLAGS	   ?= -uvf
.endif
.else
.if !empty(WAR_MANIFEST)
WARFLAGS	   ?= -cvfm
.else
WARFLAGS	   ?= -cvf
.endif
.endif

_JVM_JARFLAGS_ = ${${.TARGET}_JVM_JARFLAGS:U${JVM_JARFLAGS}}
_JVM_EARFLAGS_ = ${${.TARGET}_JVM_EARFLAGS:U${JVM_EARFLAGS}}
_JVM_WARFLAGS_ = ${${.TARGET}_JVM_WARFLAGS:U${JVM_WARFLAGS}}

_JARFLAGS_    = ${${.TARGET}_JARFLAGS:U${JARFLAGS}}
_EARFLAGS_    = ${${.TARGET}_EARFLAGS:U${EARFLAGS}}
_WARFLAGS_    = ${${.TARGET}_WARFLAGS:U${WARFLAGS}}

JAR_RUNDIR EAR_RUNDIR WAR_RUNDIR ?= ${_CLASSGENDIR_}

.if !empty(MACHINE:M*_openvms_*)
_JAR_RUNDIR_ = ${${.TARGET}_JAR_RUNDIR:U${JAR_RUNDIR}:@.PATH.@${VMS_PATH_CONV}@}
_EAR_RUNDIR_ = ${${.TARGET}_EAR_RUNDIR:U${EAR_RUNDIR}:@.PATH.@${VMS_PATH_CONV}@}
_WAR_RUNDIR_ = ${${.TARGET}_WAR_RUNDIR:U${WAR_RUNDIR}:@.PATH.@${VMS_PATH_CONV}@}
.elif ${BACKSLASH_PATHS}
_JAR_RUNDIR_ = ${${.TARGET}_JAR_RUNDIR:U${JAR_RUNDIR}:s|/|\\|g}
_EAR_RUNDIR_ = ${${.TARGET}_EAR_RUNDIR:U${EAR_RUNDIR}:s|/|\\|g}
_WAR_RUNDIR_ = ${${.TARGET}_WAR_RUNDIR:U${WAR_RUNDIR}:s|/|\\|g}
.else
_JAR_RUNDIR_ = ${${.TARGET}_JAR_RUNDIR:U${JAR_RUNDIR}}
_EAR_RUNDIR_ = ${${.TARGET}_EAR_RUNDIR:U${EAR_RUNDIR}}
_WAR_RUNDIR_ = ${${.TARGET}_WAR_RUNDIR:U${WAR_RUNDIR}}
.endif

_JAR_OBJECTS_	      = ${${.TARGET}_JAR_OBJECTS:U${JAR_OBJECTS}}
_EAR_OBJECTS_	      = ${${.TARGET}_EAR_OBJECTS:U${EAR_OBJECTS}}
_WAR_OBJECTS_	      = ${${.TARGET}_WAR_OBJECTS:U${WAR_OBJECTS}}

_OTHER_JAR_OBJECTS_ = ${${.TARGET}_OTHER_JAR_OBJECTS:U${OTHER_JAR_OBJECTS}}
_OTHER_EAR_OBJECTS_ = ${${.TARGET}_OTHER_EAR_OBJECTS:U${OTHER_EAR_OBJECTS}}
_OTHER_WAR_OBJECTS_ = ${${.TARGET}_OTHER_WAR_OBJECTS:U${OTHER_WAR_OBJECTS}}

_JAR_MANIFEST_      ?= ${${.TARGET}_JAR_MANIFEST:U${JAR_MANIFEST}}
_EAR_MANIFEST_      ?= ${${.TARGET}_EAR_MANIFEST:U${EAR_MANIFEST}}
_WAR_MANIFEST_      ?= ${${.TARGET}_WAR_MANIFEST:U${WAR_MANIFEST}}

.if !empty(MACHINE:M*_openvms_*)
JAR_RESPFILE_NAME   ?= ${.TARGET:s|.|_|g}.tmp
EAR_RESPFILE_NAME   ?= ${.TARGET:s|.|_|g}.tmp
WAR_RESPFILE_NAME   ?= ${.TARGET:s|.|_|g}.tmp
.else
JAR_RESPFILE_NAME   ?= ${.TARGET}.tmp
EAR_RESPFILE_NAME   ?= ${.TARGET}.tmp
WAR_RESPFILE_NAME   ?= ${.TARGET}.tmp
.endif

_JAR_RESPFILE_NAME_ ?= ${${.TARGET}_JAR_RESPFILE_NAME:U${JAR_RESPFILE_NAME}}
_EAR_RESPFILE_NAME_ ?= ${${.TARGET}_EAR_RESPFILE_NAME:U${EAR_RESPFILE_NAME}}
_WAR_RESPFILE_NAME_ ?= ${${.TARGET}_WAR_RESPFILE_NAME:U${WAR_RESPFILE_NAME}}

JAR_RESPFILE_DIR    ?= ${_JAR_RUNDIR_}
EAR_RESPFILE_DIR    ?= ${_EAR_RUNDIR_}
WAR_RESPFILE_DIR    ?= ${_WAR_RUNDIR_}

_JAR_RESPFILE_DIR_  ?= ${${.TARGET}_JAR_RESPFILE_DIR:U${JAR_RESPFILE_DIR}}
_EAR_RESPFILE_DIR_  ?= ${${.TARGET}_EAR_RESPFILE_DIR:U${EAR_RESPFILE_DIR}}
_WAR_RESPFILE_DIR_  ?= ${${.TARGET}_WAR_RESPFILE_DIR:U${WAR_RESPFILE_DIR}}

.if !empty(MACHINE:M*_openvms_*)
JAR_RESPFILE        ?= ${_JAR_RESPFILE_DIR_}${_JAR_RESPFILE_NAME_}
EAR_RESPFILE        ?= ${_EAR_RESPFILE_DIR_}${_EAR_RESPFILE_NAME_}
WAR_RESPFILE        ?= ${_WAR_RESPFILE_DIR_}${_WAR_RESPFILE_NAME_}
.else
JAR_RESPFILE        ?= ${_JAR_RESPFILE_DIR_}/${_JAR_RESPFILE_NAME_}
EAR_RESPFILE        ?= ${_EAR_RESPFILE_DIR_}/${_EAR_RESPFILE_NAME_}
WAR_RESPFILE        ?= ${_WAR_RESPFILE_DIR_}/${_WAR_RESPFILE_NAME_}
.endif

.if ${BACKSLASH_PATHS}
_JAR_RESPFILE_       = ${${.TARGET}_JAR_RESPFILE:U${JAR_RESPFILE}:S|/|\\|g}
_EAR_RESPFILE_       = ${${.TARGET}_EAR_RESPFILE:U${EAR_RESPFILE}:S|/|\\|g}
_WAR_RESPFILE_       = ${${.TARGET}_WAR_RESPFILE:U${WAR_RESPFILE}:S|/|\\|g}
.else
_JAR_RESPFILE_       = ${${.TARGET}_JAR_RESPFILE:U${JAR_RESPFILE}}
_EAR_RESPFILE_       = ${${.TARGET}_EAR_RESPFILE:U${EAR_RESPFILE}}
_WAR_RESPFILE_       = ${${.TARGET}_WAR_RESPFILE:U${WAR_RESPFILE}}
.endif

.ifdef JAROBJS_NOABS
ALLJAROBJS =  ${_JAR_OBJECTS_}
.else
ALLJAROBJS =  ${ALLJAVASRC}
.endif

.ifdef OTHER_JAROBJS_NOABS
ALLJAROBJS += ${_OTHER_JAR_OBJECTS_}
.else
ALLJAROBJS += ${_OTHER_JAR_OBJECTS_:p}
.endif

.ifdef SJAROBJS_NOABS
ALLSJAROBJS =  ${_SIGNED_JAR_OBJECTS_}
.else
ALLSJAROBJS =  ${ALLJAVASRC}
.endif

.ifdef OTHER_SJAROBJS_NOABS
ALLSJAROBJS += ${_OTHER_SIGNED_JAR_OBJECTS_}
.else
ALLSJAROBJS += ${_OTHER_SIGNED_JAR_OBJECTS_:p}
.endif

.ifdef EAROBJS_NOABS
ALLEAROBJS =  ${_EAR_OBJECTS_}
.else
ALLEAROBJS =  ${ALLJAVASRC}
.endif

.ifdef OTHER_EAROBJS_NOABS
ALLEAROBJS += ${_OTHER_EAR_OBJECTS_}
.else
ALLEAROBJS += ${_OTHER_EAR_OBJECTS_:p}
.endif

.ifdef SEAROBJS_NOABS
ALLSEAROBJS =  ${_SIGNED_EAR_OBJECTS_}
.else
ALLSEAROBJS =  ${ALLJAVASRC}
.endif

.ifdef OTHER_SEAROBJS_NOABS
ALLSEAROBJS += ${_OTHER_SIGNED_EAR_OBJECTS_}
.else
ALLSEAROBJS += ${_OTHER_SIGNED_EAR_OBJECTS_:p}
.endif

.ifdef WAROBJS_NOABS
ALLWAROBJS =  ${_WAR_OBJECTS_}
.else
ALLWAROBJS =  ${ALLJAVASRC}
.endif

.ifdef OTHER_WAROBJS_NOABS
ALLWAROBJS += ${_OTHER_WAR_OBJECTS_}
.else
ALLWAROBJS += ${_OTHER_WAR_OBJECTS_:p}
.endif

.ifdef SWAROBJS_NOABS
ALLSWAROBJS =  ${_SIGNED_WAR_OBJECTS_}
.else
ALLSWAROBJS =  ${ALLJAVASRC}
.endif

.ifdef OTHER_SWAROBJS_NOABS
ALLSWAROBJS += ${_OTHER_SIGNED_WAR_OBJECTS_}
.else
ALLSWAROBJS += ${_OTHER_SIGNED_WAR_OBJECTS_:p}
.endif

JAR_FILE_ARGS          ?= ${ALLJAROBJS:@WORD@${JAVA_QUOTE}${WORD}${JAVA_QUOTE}@}
_JAR_FILE_ARGS_         = ${${.TARGET}_JAR_FILE_ARGS:U${JAR_FILE_ARGS}}
SIGNED_JAR_FILE_ARGS   ?= ${ALLSJAROBJS:@WORD@${JAVA_QUOTE}${WORD}${JAVA_QUOTE}@}
_SIGNED_JAR_FILE_ARGS_  = ${${.TARGET}_SIGNED_JAR_FILE_ARGS:U${SIGNED_JAR_FILE_ARGS}}

EAR_FILE_ARGS          ?= ${ALLEAROBJS:@WORD@${JAVA_QUOTE}${WORD}${JAVA_QUOTE}@}
_EAR_FILE_ARGS_         = ${${.TARGET}_EAR_FILE_ARGS:U${EAR_FILE_ARGS}}
SIGNED_EAR_FILE_ARGS   ?= ${ALLSEAROBJS:@WORD@${JAVA_QUOTE}${WORD}${JAVA_QUOTE}@}
_SIGNED_EAR_FILE_ARGS_  = ${${.TARGET}_SIGNED_EAR_FILE_ARGS:U${SIGNED_EAR_FILE_ARGS}}

WAR_FILE_ARGS          ?= ${ALLWAROBJS:@WORD@${JAVA_QUOTE}${WORD}${JAVA_QUOTE}@}
_WAR_FILE_ARGS_         = ${${.TARGET}_WAR_FILE_ARGS:U${WAR_FILE_ARGS}}
SIGNED_WAR_FILE_ARGS   ?= ${ALLSWAROBJS:@WORD@${JAVA_QUOTE}${WORD}${JAVA_QUOTE}@}
_SIGNED_WAR_FILE_ARGS_  = ${${.TARGET}_SIGNED_WAR_FILE_ARGS:U${SIGNED_WAR_FILE_ARGS}}

.ifndef USE_JAVA_RESPFILE
_JARFLAGS_ += ${_JVM_JARFLAGS_}
_EARFLAGS_ += ${_JVM_EARFLAGS_}
_WARFLAGS_ += ${_JVM_WARFLAGS_}
.endif


################################################################################
# Variables for the archive signing tool (javakey or jarsigner)
#
.ifdef USE_JARSIGNER
JAVAKEY			  ?= jarsigner
JAVAKEYFLAGS		  ?= 
.else
JAVAKEY			  ?= javakey
JAVAKEYFLAGS		  ?= -gs
.endif

_JVM_JAVAKEYFLAGS_ = ${${.TARGET}_JVM_JAVAKEYFLAGS:U${JVM_JAVAKEYFLAGS}}

_JAVAKEY_		  ?= ${JAVAKEY}
_JAVAKEYFLAGS_		   = ${${.TARGET}_JAVAKEYFLAGS:U${JAVAKEYFLAGS}}

_SIGNED_JAR_OBJECTS_       = ${${.TARGET}_SIGNED_JAR_OBJECTS:U${SIGNED_JAR_OBJECTS}}
_OTHER_SIGNED_JAR_OBJECTS_  = ${${.TARGET}_OTHER_SIGNED_JAR_OBJECTS:U${OTHER_SIGNED_JAR_OBJECTS}}
SIGNED_JAR_JARFILE  ?= ${.TARGET}${JAR_SUFF}

_SIGNED_EAR_OBJECTS_       = ${${.TARGET}_SIGNED_EAR_OBJECTS:U${SIGNED_EAR_OBJECTS}}
_OTHER_SIGNED_EAR_OBJECTS_  = ${${.TARGET}_OTHER_SIGNED_EAR_OBJECTS:U${OTHER_SIGNED_EAR_OBJECTS}}
SIGNED_EAR_EARFILE  ?= ${.TARGET}${EAR_SUFF}

_SIGNED_WAR_OBJECTS_       = ${${.TARGET}_SIGNED_WAR_OBJECTS:U${SIGNED_WAR_OBJECTS}}
_OTHER_SIGNED_WAR_OBJECTS_  = ${${.TARGET}_OTHER_SIGNED_WAR_OBJECTS:U${OTHER_SIGNED_WAR_OBJECTS}}
SIGNED_WAR_WARFILE  ?= ${.TARGET}${WAR_SUFF}

JAVAKEY_DIRECT_FILE ?= ${.TARGET}.d
JAVA_SIGN_SIGNER	  ?= duke
JAVA_SIGN_CERT		  ?= 1
JAVA_SIGN_CHAIN		  ?= 0
JAVA_SIGN_SIGFILE	  ?= ${.TARGET:R}

.ifndef USE_JAVA_RESPFILE
_JAVAKEYFLAGS_ += ${_JVM_JAVAKEYFLAGS_}
.endif


################################################################################
# Variables for the JNI/native header tool (javah)
#
JAVAH		    ?= javah
_JAVAH_		    ?= ${JAVAH}
JAVAH_GENDIR	    ?= . 
JAVAH_TARGET_SUFFIX ?= .h
JAVAH_TARGET_PREFIX ?= ${_JAVA_PACKAGE_NAME_:S/./_/g:S/$/_/g}
JAVAH_TARGETS       += ${JAVAH_CLASSES:S/^/${JAVAH_TARGET_PREFIX}/g:S/${CLASS_SUFF}$/${JAVAH_TARGET_SUFFIX}/g}
JAVAH_SOURCES	     = ${.TARGET:S/^${JAVAH_TARGET_PREFIX}//g:S/${JAVAH_TARGET_SUFFIX}$/${CLASS_SUFF}/g}
_JAVAHFLAGS_	     = ${JAVAHFLAGS}
_JVM_JAVAHFLAGS_ = ${${.TARGET}_JVM_JAVAHFLAGS:U${JVM_JAVAHFLAGS}}

.ifdef JAVAH_SRCDIR

.ifdef USE_GENPATH_FOR_JAVA_SRCDIR
_JAVAH_SRCDIR_ = ${JAVAH_SRCDIR:S/${PATHSEP}/ /g:@WORD@-I${WORD}@:S/$/ -V ${JAVA_SRCDIR_GENPATH_FLAGS}/:G}
.else
_JAVAH_SRCDIR_ = ${JAVAH_SRCDIR}
.endif

JAVAHFLAGS	    ?= -d ${JAVAH_GENDIR} -classpath ${_JAVAH_SRCDIR_}

.else # JAVAH_SRCDIR

JAVAHFLAGS	    ?= -d ${JAVAH_GENDIR}

.endif # JAVAH_SRCDIR

.ifndef USE_JAVA_RESPFILE
_JAVAHFLAGS_ += ${_JVM_JAVAHFLAGS_}
.endif


################################################################################
# Variables for the documentation tool (javadoc)
#
JAVADOC	       ?= javadoc 
_JAVADOC_      ?= ${JAVADOC}
JAVADOC_GENDIR ?= .
_JAVADOCFLAGS_  = ${JAVADOCFLAGS}
_JVM_JAVADOCFLAGS_ = ${${.TARGET}_JVM_JAVADOCFLAGS:U${JVM_JAVADOCFLAGS}}

.ifdef JAVADOC_SRCDIR

.ifdef USE_GENPATH_FOR_JAVA_SRCDIR
_JAVADOC_SRCDIR_ = ${JAVADOC_SRCDIR:S/${PATHSEP}/ /g:@WORD@-I${WORD}@:S/$/ -V ${JAVA_SRCDIR_GENPATH_FLAGS}/:G}
.else
_JAVADOC_SRCDIR_ = ${JAVADOC_SRCDIR}
.endif

JAVADOCFLAGS   ?= -d ${JAVADOC_GENDIR} -sourcepath ${_JAVADOC_SRCDIR_}

.else # JAVADOC_SRCDIR

JAVADOCFLAGS   ?= -d ${JAVADOC_GENDIR}

.endif # JAVADOC_SRCDIR

.ifndef USE_JAVA_RESPFILE
_JAVADOCFLAGS_ += ${_JVM_JAVADOCFLAGS_}
.endif


################################################################################
# Variables for the RMI tool (rmic)
#
RMIC		 ?= rmic 
_RMIC_		 ?= ${RMIC}
RMIC_GENDIR      ?= ${_CLASSGENDIR_}
RMI_TARGET_SUFFIX = _Skel${CLASS_SUFF}
RMI_TARGETS       = ${RMI_CLASSES:R:S/$/${RMI_TARGET_SUFFIX}/g}
RMI_SOURCES       = ${.TARGET:S/${RMI_TARGET_SUFFIX}$/${CLASS_SUFF}/g}
_RMICFLAGS_       = ${RMICFLAGS}
_JVM_RMICFLAGS_ = ${${.TARGET}_JVM_RMICFLAGS:U${JVM_RMICFLAGS}}

.ifdef RMIC_SRCDIR

.ifdef USE_GENPATH_FOR_JAVA_SRCDIR
_RMIC_SRCDIR_ = ${RMIC_SRCDIR:S/${PATHSEP}/ /g:@WORD@-I${WORD}@:S/$/ -V ${JAVA_SRCDIR_GENPATH_FLAGS}/:G}
.else
_RMIC_SRCDIR_ = ${RMIC_SRCDIR}
.endif

RMICFLAGS	 ?= -d ${RMIC_GENDIR} -classpath ${_RMIC_SRCDIR_}

.else # RMIC_SRCDIR

RMICFLAGS	 ?= -d ${RMIC_GENDIR}

.endif # RMIC_SRCDIR

.ifndef USE_JAVA_RESPFILE
_RMICFLAGS_ += ${_JVM_RMICFLAGS_}
.endif


################################################################################
# Variables for the properties rules
#
N2A       ?= native2ascii
_N2A_     ?= ${N2A}
N2A_DEFAULTS  ?= -encoding ${${.TARGET}_N2ACODEPAGE:U${_N2ACODEPAGE_}} 

# This will take the target name and filter off the "_ll_cc.prop" to determine
# the base class name.  This is needed inorder to then determine the
# language it is and then use that information to get the codepage to use.
_CLASS_NAME_   = ${JAVA_LANGS:@.LANG.@${IMPJAVASRC:T:R:M*_${.LANG.}:S;_${.LANG.}$;;}@}
_N2ACODEPAGE_ ?= ${${IMPJAVASRC:T:R:S;^${_CLASS_NAME_}_;;}_N2ACODEPAGE:U${default_N2ACODEPAGE}}
_N2AFLAGS_    ?= ${N2A_DEFAULTS} \
		 ${${.TARGET}_N2AENV:U${N2AENV}} \
		 ${${.TARGET}_N2AFLAGS:U${N2AFLAGS}} \
		 ${${.TARGET}_N2AARGS:U${N2AARGS}}

# Java languages defined, needed in order to determine the classname and 
# codepage mappings.
JAVA_LANGS ?= \
	# Danish Denmark 
	da_DK \
	# Danish (basic)
	da \
	# German Austria 
	de_AT_EURO \
	de_AT \
	# German Switzerland 
	de_CH \
	# German Germany 
	de_DE_EURO \
	de_DE \
	# German Luxembourg 
	de_LU_EURO \
	de_LU \
	# German (basic)
	de \
	# Greek Greece 
	el_GR \
	# Greek (basic)
	el \
	# English Canada 
	en_CA \
	# English United Kingdom 
	en_GB \
	# English Ireland 
	en_IE_EURO \
	en_IE \
	# English United States 
	en_US \
	# English (basic)
	en \
	# Spanish Spain 
	es_ES_EURO \
	es_ES \
	# Spanish (basic)
	es \
	# Finnish Finland 
	fi_FI_EURO \
	fi_FI \
	# Finnish (basic)
	fi \
	# French Belgium 
	fr_BE_EURO \
	fr_BE \
	# French Canada 
	fr_CA \
	# French Switzerland 
	fr_CH \
	# French France 
	fr_FR_EURO \
	fr_FR \
	# French Luxembourg 
	fr_LU_EURO \
	fr_LU \
	# French (basic)
	fr \
	# Italian Switzerland 
	it_CH \
	# Italian Italy 
	it_IT_EURO \
	it_IT \
	# Italian (basic)
	it \
	# Japanese Japan 
	ja_JP \
	# Japanese (basic)
	ja \
	# Korean Korea 
	ko_KR \
	# Korean (basic)
	ko \
	# Dutch Belgium 
	nl_BE_EURO \
	nl_BE \
	# Dutch Netherlands 
	nl_NL_EURO \
	nl_NL \
	# Dutch (basic)
	nl \
	# Norwegian (Bokmål) Norway 
	no_NO_B \
	# Norwegian (Nynorsk) Norway 
	no_NO \
	# Norwegian (basic)
	no \
	# Portuguese Portugal 
	pt_PT_EURO \
	pt_PT \
	# Portuguese (basic)
	pt \
	# Swedish Sweden 
	sv_SE \
	# Swedish (basic)
	sv \
	# Turkish Turkey 
	tr_TR \
	# Turkish (basic)
	tr \
	# Chinese (Simplified) China 
	zh_CN \
	# Chinese (Traditional) Taiwan 
	zh_TW \
	# Chinese (basic)
	zh

# Code page mappings
# default code page 
default_N2ACODEPAGE ?= Cp1252
# specialized code pages
zh_CN_N2ACODEPAGE   ?= Cp1381
zh_TW_N2ACODEPAGE   ?= Cp950
ja_JP_N2ACODEPAGE   ?= SJIS
ja_N2ACODEPAGE      ?= ${ja_JP_N2ACODEPAGE}
ko_N2ACODEPAGE      ?= Cp949

# Path definitions for properties
PROPERTIES_SUBDIR     ?= export${DIRSEP}classes${DIRSEP}${JAVA_PACKAGE_NAME:S|.|${DIRSEP}|g}
_PROPERTIESSUBGENDIR_ ?= ${${.TARGET}_PROPERTIESSUBGENDIR:U${PROPERTIES_SUBDIR}}
PROPERTIESGENDIR      ?= ${SANDBOXBASE}${DIRSEP}${_PROPERTIESSUBGENDIR_}
_PROPERTIESGENDIR_    ?= ${${.TARGET}_PROPERTIESGENDIR:U${PROPERTIESGENDIR}}

.ifdef PROPERTIES_SAMESUFF
_RELATIVE_PROPDIR_ = ${MAKETOP}${OBJECTDIRTOP}..${DIRSEP}${_PROPERTIESSUBGENDIR_}
_PROP_TGT_  = ${.TARGET:s/^properties_//:@.TARGET@${_RELATIVE_PROPDIR_}${DIRSEP}${.TARGET}@}
.else
_PROP_TGT_  = ${${_PROPERTIESGENDIR_}/${.TARGET:T}:L:S;/;${DIRSEP};g}
.endif


################################################################################
# Variables for the .PATH target
#
.if empty(JAVA_PACKAGE_NAME)
CLASS_SEARCH_PATHS ?= \
  ${CLASSBASES:S/${PATHSEP}/ /g}
.else
CLASS_SEARCH_PATHS ?= \
  ${CLASSBASES:S/${PATHSEP}/${DIRSEP}${JAVA_PACKAGE_NAME:S!.!${DIRSEP}!g} /g} \
  ${CLASSBASES:S/${PATHSEP}/ /g}
.endif


.endif  # __JAVA_STD_MK_
