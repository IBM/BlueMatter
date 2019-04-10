################################################################################
# Version: %RELEASE_NAME% (Build %LEVEL_NAME%, %BUILD_DATE%)
#
# This file defines many variables for tools names and some top level rules.
#  Included makefiles:
#    passes.mk
#    bld.std.mk
#    pkg.std.mk
#    tso.std.mk
#
#  Some variables used:
#    UNIX
#	 If defined, then we are on a UNIX system.
#    CC_SUFF, OBJ_SUFF, PROG_SUFF, LIB_SUFF, SHLIB_SUFF, PP_SUFF, DEP_SUFF
#    LIB_PREF
#    CCFAMILY, CCTYPE
#    CFLAGS, CARGS, CENV
#    LDFLAGS, LDARGS, LDENV
#    LIBFLAGS, LIBARGS, LIBENV
#    LIBS, LIBSARGS, LIBSENV
#    TOOLSBASE
#      The base directory of ODE tools to be used.
#    CDEFS
#      Makefile variable to store command-line variables defined via "-D".
#      CDEFS is used as part of _CC_FLAGS_ and _GENDEPFLAGS_
#   CAT_SUFF
#       Suffix  of CATALOGS, default it .cat
#   MSG_SUFF
#       Source suffix of CATALOGS, default it .msg
#   ODEMRINAME
#       The subdirectory name of the MRI source. Defaults to 'src'.  Allows
#       separation of MRI, if setting ODEMRINAME=mri.
#   PKG_IS_SUPPORTED
#       A variable that is conditionally set based on MACHINE for only
#       those platforms that supported packaging.
#   PKG_IS_SP_SUPPORTED
#       A variable that is conditionally set based on MACHINE for only
#       those platforms that support  Service packaging.
#
#  clean and clobber passes normally use the default makefile
#  target information (PROGRAMS, OFILES, etc.), but this can
#  be overriden using the CLEANFILES and CLOBBERFILES variables
#  to explicitly define which files to delete.
#
#
################################################################################
.if !defined(__STD_MK_)
__STD_MK_=


################################################################################
#  Default rule - All other rules appear after variable definitions
#
.MAIN: build_all

################################################################################
#  Debugging entry for checking environment
#
print_env: .SPECTARG
	@printenv

#
# Set the variable IN_SANDBOX so it can be used later, if needed
#
.if !empty(BACKED_SANDBOXDIR:M*${PATHSEP}*)
IN_SANDBOX = yes
.endif

#
# On what platform are the objects/executables going to be used?
#
TARGET_MACHINE ?= ${MACHINE}

# The default packaging tool to pre-process the cmf with.

.if (${TARGET_MACHINE:M*aix*})
PKGFAMILY ?= mkinstall
.elif ( (${TARGET_MACHINE:M*solaris*}) || (${TARGET_MACHINE:M*sco*}) )
PKGFAMILY ?= pkgmk
.elif (${TARGET_MACHINE:M*irix*})
PKGFAMILY ?= pkgmk
.elif (${TARGET_MACHINE:M*hp*})
PKGFAMILY ?= swpackage
.elif (${TARGET_MACHINE:M*mvs*})
PKGFAMILY ?= mvs
.elif (${TARGET_MACHINE:M*linux*})
PKGFAMILY ?= rpm
.elif (${TARGET_MACHINE:M*nt*}) || (${TARGET_MACHINE:M*95*})
PKGFAMILY ?= ispe
.endif

################################################################
#  Rules for defining PKG_CLASS
#
PKG_TYPE         ?= user
PKG_CLASS        ?= ipp

.if (${PKG_TYPE:l}=="apar" || ${PKG_TYPE:l}=="++apar" || \
     ${PKG_TYPE:l}=="usermod" || ${PKG_TYPE:l}=="++usermod" || \
     ${PKG_TYPE:l}=="ptf" || ${PKG_TYPE:l}=="++ptf" || \
     ${PKG_TYPE:l}=="ptfvpl" || ${PKG_TYPE:l}=="retrieveapar" || \
     ${PKG_TYPE:l}=="retrieveptf" )
PKG_CLASS = sp
.endif

################################################################
#   If PKG_TYPE is equal to APAR or USERMOD and if
#   B390_AUTO_DELETE_DRIVER is blank, then set the default
#   B390_AUTO_DELETE_DRIVER to NO.  This will cause the driver
#   report command to run instead of the delete driver command.
#   If B390_AUTO_DELETE_DRIVER was left blank, the process would
#   try to delete a delta driver during the initial APAR or
#   USERMOD process that does not exist, therefore stopping
#   the service process.
#
#
.if (${PKG_TYPE:l}=="apar" || ${PKG_TYPE:l}=="usermod")
.if empty(B390_AUTO_DELETE_DRIVER)
B390_AUTO_DELETE_DRIVER = NO
.endif
.endif

################################################################
# If PATCH_NAME is not empty, and then if PKGFAMILY not equal pkgmk
# set PKG_CLASS equal SP
.if !empty(PATCH_NAME)
.if (${PKGFAMILY:l}!="pkgmk")
PKG_CLASS = sp
.endif
.endif # !empty(PATCH_NAME)

################################################################
# If PKG_CLASS equals "sp" (Service Package),
# set INSTALL_USING_DEPS to prevent the entire product from being installed
#
.if defined(PKG_CLASS) && (${PKG_CLASS:l}=="sp")
INSTALL_USING_DEPS = install_using_deps
.endif
#################################################################
#Defined PKG_IS_SP_SUPPORTED for OSs that have Service Packaging Support

.if !empty(MACHINE:Mhp9000_ux_11) || !empty(MACHINE:M*_solaris_*) || \
    !empty(MACHINE:Mmvs390_oe_*) || !empty(MACHINE:M*_linux_*) || \
	 !empty(MACHINE:Mia64_hpux_*)
PKG_IS_SP_SUPPORTED =
.else
.undef PKG_IS_SP_SUPPORTED
.endif   

# 
# New variables for building (TSO) and packaging
# Set defaults for packaging
# 
.if !empty(MACHINE:Mmvs390_oe_*)
MVS_USERID              ?= ${PKG_MVS_USERID}
MVS_PASSWORD            ?= ${PKG_MVS_PASSWORD}
MVS_EXEC_DATASET        ?= ${PKG_MVS_EXEC_DATASET}
MVS_JOBCARD             ?= ${PKG_MVS_JOBCARD}
MVS_TERSE_LOAD_DATASET  ?= ${PKG_MVS_TERSE_LOAD_DATASET}
MVS_TERSE_CLIST_DATASET ?= ${PKG_MVS_TERSE_CLIST_DATASET}
MVS_RTG_LOADLIB         ?= ${PKG_MVS_RTG_LOADLIB}
MVS_FUNCCNTL_DATASET    ?= ${PKG_MVS_FUNCCNTL_DATASET}
MVS_JOBMONITOR_TIME     ?= ${PKG_MVS_JOBMONITOR_TIME}
MVS_RTG_ALLOC_INFO      ?= "${PKG_MVS_RTG_ALLOC_INFO}"
MVS_VSAM_VOLUME_INFO    ?= "${PKG_MVS_VSAM_VOLUME_INFO}"
MVS_DELETE_OUTPUT       ?= ${PKG_MVS_DELETE_OUTPUT}
MVS_DISPLAY_OUTPUT      ?= ${PKG_MVS_DISPLAY_OUTPUT}
MVS_SAVE_OUTPUT_FILE    ?= ${PKG_MVS_SAVE_OUTPUT_FILE}
.endif

#
# Default object file format
# Currently only affects lib.mk when making archive libraries.
#
OBJECT_FORMAT ?= ELF

#
#  Set defaults for input variables which are not already defined
#
MDFLAGS ?= -rm -top -elxdep

OWNER?=${USER:Uroot}
IOWNER?=${OWNER}
EOWNER?=${OWNER}
GROUP?=staff
IGROUP?=${GROUP}
EGROUP?=${GROUP}
IMODE?=755
EMODE?=644

.if empty(MACHINE:Mx86_os2_*) && empty(MACHINE:Mx86_nt_*) && \
    empty(MACHINE:Mx86_95_*) && empty(MACHINE:M*_openvms_*)
UNIX ?= UNIX
.endif

.ifdef UNIX
NULL_DEVICE ?= /dev/null
.else
NULL_DEVICE ?= NUL
.endif

.if !empty(MACHINE:Malpha_*) || !empty(MACHINE:Mx86_*) || !empty(ia64_*)
ENDIAN ?= little
.else
ENDIAN ?= big
.endif

.if !empty(MACHINE:Mmvs390_oe_*) && defined(KEEP_PRELINKER_FILES)
_C89_TMPS _CC_TMPS _CXX_TMPS    %= 0xfffffffd
_C89_STEPS _CC_STEPS _CXX_STEPS %= 0xffffffff
.endif

.if !empty(MACHINE:M*_openvms_*)
# using response files is effectively required on VMS
USE_RESPFILE      = 1
USE_JAVA_RESPFILE = 1
# variables to convert Unix style paths to VMS style
VMS_PATH_CONV    ?= ${.PATH.:M/*:b${VMS_ABSPATH_CONV}} \
                    ${.PATH.:N/*:N*]*:b${VMS_RELPATH_CONV}} \
                    ${.PATH.:M*]*}
VMS_ABSPATH_CONV ?= ${.PATH.:s|/..|/-|g:s|/.|/|g:xs|/+\$||e:xs|^/+||e:xs|/+|:[|e:xs|[[/][[:alnum:]_\$-]+\$|&]|e:xs|/[[:alnum:]_\$-]+\.[^/]*\$|^&|e:s|\^/|]|:xs|/+|.|ge}
VMS_RELPATH_CONV ?= ${.PATH.:s|^|[/|:s|/..|/-|g:s|/.|/|g:xs|/+\$||e:xs|[[/][[:alnum:]_\$-]+\$|&]|e:xs|/[[:alnum:]_\$-]+\.[^/]*\$|^&|e:s|\^/|]|:xs|/+|.|ge:s|^[$|[]|}
.endif

#
#  Program macros
#
.if !empty(MACHINE:M*_openvms_*)
GENPATH   ?= genpath
MAKEPATH  ?= mkpath
MAKELINKS ?= mklinks
GENDEP    ?= gendep
CRLFCON   ?= crlfcon
.elifndef NO_TOOLSBASE
GENPATH   ?= ${TOOLSBASE}genpath${PROG_SUFF}
MAKEPATH  ?= ${TOOLSBASE}mkpath${PROG_SUFF}
MAKELINKS ?= ${TOOLSBASE}mklinks${PROG_SUFF}
GENDEP    ?= ${TOOLSBASE}gendep${PROG_SUFF}
CRLFCON   ?= ${TOOLSBASE}crlfcon${PROG_SUFF}
.else
GENPATH   ?= genpath${PROG_SUFF}
MAKEPATH  ?= mkpath${PROG_SUFF}
MAKELINKS ?= mklinks${PROG_SUFF}
GENDEP    ?= gendep${PROG_SUFF}
CRLFCON   ?= crlfcon${PROG_SUFF}
.endif
MD       ?= .rmkdep
AS       ?= as
CHMOD    ?= chmod
CHOWN    ?= chown
.if !empty(MACHINE:M*_openvms_*)
CHDIR    ?= SET DEFAULT
ECHO     ?= WRITE SYS$OUTPUT
.else
CHDIR    ?= cd
ECHO     ?= echo
.endif
LN       ?= ln
LORDER   ?= lorder
LNFLAGS  ?= -f -s
.if defined(UNIX)
ODEECHO  ?= ${ECHO}
CP       ?= cp
RM       ?= rm
MV       ?= mv
MVFLAGS  ?= -f
MKDIR    ?= mkdir
.if !empty(MACHINE:Mmvs390_oe_*)
CPFLAGS  ?= -m
.elif !empty(MACHINE:Mas400_os400_*)
CPFLAGS  ?= -hp
.elifdef UNIX
CPFLAGS  ?= -p
.endif
RMFLAGS  ?= -f
.else
.ifndef NO_TOOLSBASE
ODEECHO   ?= ${TOOLSBASE}odeecho${PROG_SUFF}
.else
ODEECHO   ?= odeecho${PROG_SUFF}
.endif
ODEECHOFLAGS ?= -e
CP       ?= copy
.if !empty(MACHINE:M*_openvms_*)
RM       ?= DELETE
MV       ?= RENAME
MKDIR    ?= CREATE /DIR
.else
RM       ?= del
MV       ?= move
MKDIR    ?= md
.endif
.endif
SED      ?= sed
SORT     ?= sort
TAR      ?= tar
TOUCH    ?= touch
TSORT    ?= tsort
MSGBIND  ?= msgbind

# EMXBIND stuff
EMXBIND  ?= emxbind
_EMXBIND_  = ${${.TARGET}_EMXBIND:U${EMXBIND}}
_EMXBINDFLAGS_  = ${${.TARGET}_EMXBINDFLAGS:U${EMXBINDFLAGS}}

# CRLFCON settings
_CRLFCON_ ?= ${CRLFCON}

# Flags to control CRLFCON
CRLFDEFFLAGS ?= -format d
_CRLFFLAGS_  = ${${.TARGET}_CRLFFLAGS:U${CRLFFLAGS:U${CRLFDEFFLAGS}}}

# lex variations
#
.if !defined(NO_LEX_RULES)
LEX  ?= lex
LEX_YY_C ?= lex.yy.c
_LEX_ = ${${.TARGET}_LEX:U${LEX}}
.endif

# yacc variations
#
.if !defined(NO_YACC_RULES)
YACC   ?= yacc
Y_TAB_C ?= y.tab.c
Y_TAB_H ?= y.tab.h
_YACC_  = ${${.TARGET}_YACC:U${YACC}}
.endif

FILE_QUOTE ?= '

.ifdef QUOTE_INSTALL_FILES
INST_QUOTE ?= ${FILE_QUOTE}
.else
.undef INST_QUOTE
.endif

.ifdef QUOTE_EXPORT_FILES
EXP_QUOTE ?= ${FILE_QUOTE}
.else
.undef EXP_QUOTE
.endif

.ifdef QUOTE_SETUP_FILES
SETUP_QUOTE ?= ${FILE_QUOTE}
.else
.undef SETUP_QUOTE
.endif

.if !empty(MACHINE:M*_openvms_*)
INSTALLATION_DIR ?= inst_images
.else
INSTALLATION_DIR ?= inst.images
.endif

#
# Flags to keep ${MAKEPATH} program quiet on success
#
MAKEPATHFLAGS ?= -quiet

STRIP        ?= strip
_STRIP_	     ?= ${STRIP:L:@.TOOL.@${_CC_TOOL_}@}
_STRIPFLAGS_  = ${${.TARGET}_STRIPFLAGS:U${STRIPFLAGS}}

#
# Flags for the TAR command
# X_TARFLAGS are used for extraction operations, while
# C_TARFLAGS are used for creation operations.
#
.if ${USE_TSO_RULES}
X_TARFLAGS ?= -xof
.else
X_TARFLAGS ?= -xf
.endif
C_TARFLAGS ?= -cf

_X_TARFLAGS_ = ${${.TARGET}_X_TARFLAGS:U${X_TARFLAGS}} \
	       ${${.TARGET}_TARFLAGS:U${TARFLAGS}}
_C_TARFLAGS_ = ${${.TARGET}_C_TARFLAGS:U${C_TARFLAGS}} \
	       ${${.TARGET}_TARFLAGS:U${TARFLAGS}}

#
# Use -I- feature if available
#
USE_DASH_I_DASH ?= 0
.if ${USE_DASH_I_DASH}
_CC_GENINC_:=${-I.:L:G} -I-
.endif

################################################################################
#  Compiler variations
#
# CCVERSION, CCTYPE and CCFAMILY together gives the desired compiler.
#
# Certain portions of the build need to lex/compile/run
# programs. These use the CCTYPE=host flag in order to provide the
# "right" lexer/compiler/linker/archiver
#
CCVERSION   ?= default
_CCVERSION_ ?= ${${.TARGET}_CCVERSION:U${CCVERSION}}

CCTYPE    ?= ansi
_CCTYPE_   = ${${.TARGET}_CCTYPE:U${CCTYPE}}

CCFAMILY  ?= cset
_CCFAMILY_ = ${${.TARGET}_CCFAMILY:U${CCFAMILY}}

_CC_TOOL_	 = ${_${_CCFAMILY_}_${_CCTYPE_}_${_CCVERSION_}_${.TOOL.}_:U${_${_CCFAMILY_}_${_CCTYPE_}_${.TOOL.}_:U${_${_CCFAMILY_}_${_CCVERSION_}_${.TOOL.}_:U${_${_CCFAMILY_}_${.TOOL.}_:U${_${_CCTYPE_}_${.TOOL.}_:U${${.TOOL.}}}}}}}

_CC_TOOL_FLAGS_  = ${_${_CCFAMILY_}_${_CCTYPE_}_${_CCVERSION_}_${.TOOL.}_:U${_${_CCFAMILY_}_${_CCTYPE_}_${.TOOL.}_:U${_${_CCFAMILY_}_${_CCVERSION_}_${.TOOL.}_:U${_${_CCFAMILY_}_${.TOOL.}_:U${_${_CCTYPE_}_${.TOOL.}_}}}}}

.if !empty(TARGET_MACHINE:Mrios_aix_*)
STORE_SHLIB_IN_ARCHIVE ?= TRUE
.endif

# Convert paths with forward slashes to use backslashes.
# To disable this for the condition shown below, set BACKSLASH_PATHS
# to an empty string or 0 (zero) instead.
#
.if !defined(UNIX) && (${CCFAMILY} == "vage" || ${CCFAMILY} == "visual")
BACKSLASH_PATHS ?= 1
.endif

.if ${BACKSLASH_PATHS}
_CURDIR_    = ${CURDIR:s|/|\\|g}
_CURDIRSEP_ = \                         # this comment protects the backslash 
.else
_CURDIR_    = ${CURDIR}
_CURDIRSEP_ = /
.endif

#
#  Automatic file-finding rules
#
.if ${USE_TSO_RULES}
USE_FILEFIND =
.endif

.ifdef USE_FILEFIND
DIR_CONTENTS != sbls -p ${SANDBOXBASE}${DIRSEP}${ODESRCNAME}${MAKEDIR}
ALL_FILES ?= ${DIR_CONTENTS:XF}
ALL_DIRS ?= ${DIR_CONTENTS:XD}
ALL_SUBDIRS ?= ${ALL_DIRS:T}
_CC_PAT_ = (${CC_SUFF:s/./[.]/g:xs/[[:blank:]]+/|/eg})
ALL_C_OFILES ?= ${DIR_CONTENTS:xM/${_CC_PAT_}\$/ce:xS/${_CC_PAT_}\$/${OBJ_SUFF}/cef:T}
ALL_JAVA_CLASSES ?= ${DIR_CONTENTS:M*${JAVA_SUFF}:S/${JAVA_SUFF}$/${CLASS_SUFF}/f:T}
.endif

################################################################################
# Suffix definitions for .obj verses .o

# CC_SUFF
#
.ifdef UNIX
CC_SUFF ?= .c .cc .cpp .C .cxx
.else
CC_SUFF ?= .c .cc .cpp .cxx
.endif

# YACC and LEX suffixes
#
YACC_COMPILE_SUFF ?= .c
YACC_HEADER_SUFF  ?= .h
YACC_COMPILE_NAME ?= ${.TARGET:R}${YACC_COMPILE_SUFF}
YACC_HEADER_NAME  ?= ${.TARGET:R}${YACC_HEADER_SUFF}
YACC_SUFF ?= .y
LEX_COMPILE_SUFF ?= .c
LEX_COMPILE_NAME ?= ${.TARGET:R}${LEX_COMPILE_SUFF}
LEX_SUFF ?= .l

# OBJ_SUFF
#
.if (!defined(UNIX)) && (${CCFAMILY} != "gnu")
OBJ_SUFF?=.obj
.else
OBJ_SUFF?=.o
.endif

# PROG_SUFF
#
.if defined(UNIX)
PROG_SUFF?=
.else
PROG_SUFF?=.exe
.endif

# STATLIB_SUFF
#
.if !empty(MACHINE:M*_openvms_*)
STATLIB_SUFF?=.olb
.elif (!defined(UNIX)) && (${CCFAMILY} != "gnu")
STATLIB_SUFF?=.lib
.else
STATLIB_SUFF?=.a
.endif

# For OS's that use libname.so.m.n format for shared library names
MAJOR_VER_LIBSUFF ?= .0
MINOR_VER_LIBSUFF ?= .0

# IMPLIB_SUFF
#
.if !empty(MACHINE:M*_openvms_*)
IMPLIB_SUFF?=.exe
.elif (!defined(UNIX)) && (${CCFAMILY} != "gnu")
IMPLIB_SUFF?=.lib
.elif !empty(TARGET_MACHINE:Mrios_aix_*)
IMPLIB_SUFF?=.a
.elif !empty(TARGET_MACHINE:Mhp9000_ux_*) || !empty(MACHINE:Mia64_hpux_*)
IMPLIB_SUFF?=.sl
.elif !empty(TARGET_MACHINE:Mmvs390_oe_*)
IMPLIB_SUFF?=.x
.elif !empty(TARGET_MACHINE:Mx86_netbsd_*) || \
      !empty(TARGET_MACHINE:Mx86_openbsd_*)
IMPLIB_SUFF?=|.so${MAJOR_VER_LIBSUFF}${MINOR_VER_LIBSUFF}
.else
IMPLIB_SUFF?=.so
.endif

# SHLIB_SUFF
#
.if !empty(MACHINE:M*_openvms_*)
SHLIB_SUFF?=.exe
.elif (!defined(UNIX)) && (${CCFAMILY} != "gnu")
SHLIB_SUFF?=.dll
.elif !empty(TARGET_MACHINE:Mhp9000_ux_*) || !empty(MACHINE:Mia64_hpux_*)
SHLIB_SUFF?=.sl
.elif !empty(TARGET_MACHINE:Mrios_aix_*) || !empty(TARGET_MACHINE:Mmvs390_oe_*)
SHLIB_SUFF?=.a
.elif !empty(TARGET_MACHINE:Mx86_netbsd_*) || \
      !empty(TARGET_MACHINE:Mx86_openbsd_*)
SHLIB_SUFF?=|.so${MAJOR_VER_LIBSUFF}${MINOR_VER_LIBSUFF}
.else
SHLIB_SUFF?=.so
.endif

.if defined(UNIX) || (${CCFAMILY} == "gnu" && empty(MACHINE:Mx86_os2_*))
LIB_PREF ?= lib
.else
LIB_PREF ?=
.endif

# Which library suffix should be used by default?
.ifdef USE_SHARED_LIBRARY
LIB_SUFF ?= ${IMPLIB_SUFF}
.else
LIB_SUFF ?= ${STATLIB_SUFF}
.endif

# Module definition file, response file, and export file suffixes,
# typically used on Windows/OS2 platforms.
#
DEFFILE_SUFF  ?= .def
.if !empty(MACHINE:M*_openvms_*)
RESPFILE_SUFF ?= .com
.else
RESPFILE_SUFF ?= .rsp
.endif
EXPFILE_SUFF  ?= .exp

LST_SUFF ?= .lst
MAP_SUFF ?= .map

# _RESPFILE_ is used to determine the name of response files
.if !empty(MACHINE:M*_openvms_*)
_RESPFILE_ ?= ${.TARGET:T:s|.|_|g}${RESPFILE_SUFF}
.else
_RESPFILE_ ?= ${.TARGET:T}${RESPFILE_SUFF}
.endif

# Module defintion file strings
#
.if (${CCFAMILY} == "vage")
.if !empty(MACHINE:Mx86_os2_*)
DEFFILE_LINE1 ?= LIBRARY ${.TARGET:S/^${LIB_PREF}//g:S/${DEFFILE_SUFF}//g} \
		 INITINSTANCE TERMINSTANCE
DEFFILE_LINE2 ?= DATA MULTIPLE NONSHARED
DEFFILE_LINE3 ?= EXPORTS
.else
DEFFILE_LINE1 ?= LIBRARY ${.TARGET:S/^${LIB_PREF}//g:S/${DEFFILE_SUFF}//g}
DEFFILE_LINE2 ?= EXPORTS
.endif
.endif

# Prelinker suffix
#
PRE_SUFF?=.p

# Preprocessor suffix
#
PP_SUFF?=.pp

# Dependency suffix
#
DEP_SUFF?=.u

# This is where the MRI/PII is located.  By default it will be in 'src'.
ODEMRINAME ?= src

# Message file suffix
#
MSG_SUFF ?= .msg

# Catalog file suffix
#
CAT_SUFF ?= .cat

# Resource target suffix
RES_SUFF ?= .res

# Resource source suffix
RC_SUFF  ?= .rc

# Suffix for header targets from catalog/help sources (see help.mk)
HDR_SUFF ?= .h

# Suffixes for help files
MC_SUFF  ?= .mc
HLP_SUFF ?= .hlp
HPJ_SUFF ?= .hpj
RTF_SUFF ?= .rtf
IPF_SUFF ?= .ipf
INF_SUFF ?= .inf

# Suffix used for iconv source files
ICONV_SUFF ?= .iconv

CPP_CCFAMILY C_CCFAMILY   ?= ${CCFAMILY}
CPP_CCTYPE                ?= cpp
C_CCTYPE                  ?= ansi
CPP_CCVERSION C_CCVERSION ?= ${CCVERSION}

.if !empty(CPP_PREPROCESS)
${CPP_PREPROCESS:S|$|_CCFAMILY|g}  ?= ${CPP_CCFAMILY}
${CPP_PREPROCESS:S|$|_CCTYPE|g}    ?= ${CPP_CCTYPE}
${CPP_PREPROCESS:S|$|_CCVERSION|g} ?= ${CPP_CCVERSION}
.endif

.if !empty(C_PREPROCESS)
${C_PREPROCESS:S|$|_CCFAMILY|g}  ?= ${C_CCFAMILY}
${C_PREPROCESS:S|$|_CCTYPE|g}    ?= ${C_CCTYPE}
${C_PREPROCESS:S|$|_CCVERSION|g} ?= ${C_CCVERSION}
.endif

.if !empty(CPP_OFILES)
${CPP_OFILES:S|$|_CCFAMILY|g}  ?= ${CPP_CCFAMILY}
${CPP_OFILES:S|$|_CCTYPE|g}    ?= ${CPP_CCTYPE}
${CPP_OFILES:S|$|_CCVERSION|g} ?= ${CPP_CCVERSION}
.endif

.if !empty(C_OFILES)
${C_OFILES:S|$|_CCFAMILY|g}  ?= ${C_CCFAMILY}
${C_OFILES:S|$|_CCTYPE|g}    ?= ${C_CCTYPE}
${C_OFILES:S|$|_CCVERSION|g} ?= ${C_CCVERSION}
.endif

.if !empty(CPP_OBJECTS)
${CPP_OBJECTS:S|$|_CCFAMILY|g}  ?= ${CPP_CCFAMILY}
${CPP_OBJECTS:S|$|_CCTYPE|g}    ?= ${CPP_CCTYPE}
${CPP_OBJECTS:S|$|_CCVERSION|g} ?= ${CPP_CCVERSION}
.endif

.if !empty(C_OBJECTS)
${C_OBJECTS:S|$|_CCFAMILY|g}  ?= ${C_CCFAMILY}
${C_OBJECTS:S|$|_CCTYPE|g}    ?= ${C_CCTYPE}
${C_OBJECTS:S|$|_CCVERSION|g} ?= ${C_CCVERSION}
.endif

.if !empty(CPP_PROGRAMS)
${CPP_PROGRAMS:S|$|_CCFAMILY|g}  ?= ${CPP_CCFAMILY}
${CPP_PROGRAMS:S|$|_CCTYPE|g}    ?= ${CPP_CCTYPE}
${CPP_PROGRAMS:S|$|_CCVERSION|g} ?= ${CPP_CCVERSION}
.endif

.if !empty(C_PROGRAMS)
${C_PROGRAMS:S|$|_CCFAMILY|g}  ?= ${C_CCFAMILY}
${C_PROGRAMS:S|$|_CCTYPE|g}    ?= ${C_CCTYPE}
${C_PROGRAMS:S|$|_CCVERSION|g} ?= ${C_CCVERSION}
.endif

.if !empty(CPP_LIBRARIES)
${CPP_LIBRARIES:S|$|_CCFAMILY|g}  ?= ${CPP_CCFAMILY}
${CPP_LIBRARIES:S|$|_CCTYPE|g}    ?= ${CPP_CCTYPE}
${CPP_LIBRARIES:S|$|_CCVERSION|g} ?= ${CPP_CCVERSION}
.endif

.if !empty(C_LIBRARIES)
${C_LIBRARIES:S|$|_CCFAMILY|g}  ?= ${C_CCFAMILY}
${C_LIBRARIES:S|$|_CCTYPE|g}    ?= ${C_CCTYPE}
${C_LIBRARIES:S|$|_CCVERSION|g} ?= ${C_CCVERSION}
.endif

.if !empty(CPP_SHARED_LIBRARIES)
${CPP_SHARED_LIBRARIES:S|$|_CCFAMILY|g}  ?= ${CPP_CCFAMILY}
${CPP_SHARED_LIBRARIES:S|$|_CCTYPE|g}    ?= ${CPP_CCTYPE}
${CPP_SHARED_LIBRARIES:S|$|_CCVERSION|g} ?= ${CPP_CCVERSION}
${CPP_SHARED_LIBRARIES:S|${SHLIB_SUFF}$|${IMPLIB_SUFF}_CCFAMILY|g}  ?= \
    ${CPP_CCFAMILY}
${CPP_SHARED_LIBRARIES:S|${SHLIB_SUFF}$|${IMPLIB_SUFF}_CCTYPE|g}    ?= \
    ${CPP_CCTYPE}
${CPP_SHARED_LIBRARIES:S|${SHLIB_SUFF}$|${IMPLIB_SUFF}_CCVERSION|g} ?= \
    ${CPP_CCVERSION}
.endif

.if !empty(C_SHARED_LIBRARIES)
${C_SHARED_LIBRARIES:S|$|_CCFAMILY|g}  ?= ${C_CCFAMILY}
${C_SHARED_LIBRARIES:S|$|_CCTYPE|g}    ?= ${C_CCTYPE}
${C_SHARED_LIBRARIES:S|$|_CCVERSION|g} ?= ${C_CCVERSION}
${C_SHARED_LIBRARIES:S|${SHLIB_SUFF}$|${IMPLIB_SUFF}_CCFAMILY|g}  ?= \
    ${C_CCFAMILY}
${C_SHARED_LIBRARIES:S|${SHLIB_SUFF}$|${IMPLIB_SUFF}_CCTYPE|g}    ?= \
    ${C_CCTYPE}
${C_SHARED_LIBRARIES:S|${SHLIB_SUFF}$|${IMPLIB_SUFF}_CCVERSION|g} ?= \
    ${C_CCVERSION}
.endif

# The trailing part of header files generated by mkcatsdef.
#
.if !empty(TARGET_MACHINE:M*_aix_*)
MSG_HDRS_TAIL ?= _msg
.endif

# C/C++ Compiler variations
#
CC		?= cc

.if !empty(MACHINE:Mhp9000_ux_*) || !empty(MACHINE:Mia64_hpux_*)
_native_cc_CC_    ?= cc
_native_ansi_CC_  ?= c89
_native_cpp_CC_   ?= aCC
.elif !empty(MACHINE:Mx86_sco_*) || !empty(MACHINE:M*_solaris_*) || \
      !empty(MACHINE:Mmips_irix_*)
_native_cc_CC_    ?= cc
_native_ansi_CC_  ?= c89
_native_cpp_CC_   ?= CC
.elif !empty(MACHINE:Mmvs390_oe_*) || !empty(MACHINE:Mx86_ptx_*)
_native_cc_CC_    ?= cc
_native_ansi_CC_  ?= c89
_native_cpp_CC_   ?= c++
.elif !empty(MACHINE:Malpha_tru64_*)
_native_cc_CC_    ?= cc ${DEPENDENCIES:D-MD}
_native_ansi_CC_  ?= c89 ${DEPENDENCIES:D-MD}
_native_cpp_CC_   ?= cxx ${DEPENDENCIES:D-MD}
.elif !empty(MACHINE:Mas400_os400_*)
_native_CC_       ?= icc
.elif !empty(MACHINE:M*_hposs_*)
_native_CC_       ?= c89
.elif !empty(MACHINE:M*_openvms_*)
_native_cc_CC_    ?= cc
_native_ansi_CC_  ?= cc
_native_cpp_CC_   ?= cxx
.endif

_cset_cc_CC_    ?= cc ${DEPENDENCIES:D-M}
_cset_ansi_CC_  ?= c89 ${DEPENDENCIES:D-M}
_cset_cpp_CC_   ?= xlC ${DEPENDENCIES:D-M}
_cset_cppr_CC_  ?= xlC_r ${DEPENDENCIES:D-M}

# GNU-style compilers on Windows do not create proper dependency files,
# so only set the -MD flag elsewhere.
.if empty(MACHINE:Mx86_nt_*) && empty(MACHINE:Mx86_95_*)
GNU_GENDEP_FLAG ?= -MD
.endif

_gnu_cc_CC_     ?= gcc ${DEPENDENCIES:D${GNU_GENDEP_FLAG}}
_gnu_ansi_CC_   ?= gcc -ansi ${DEPENDENCIES:D${GNU_GENDEP_FLAG}}
_gnu_cpp_CC_    ?= g++ ${DEPENDENCIES:D${GNU_GENDEP_FLAG}}

_kai_CC_ ?= KCC ${DEPENDENCIES:D--output_dependencies ${.TARGET:R}${DEP_SUFF}}

_visual_CC_  ?= cl

.if !empty(MACHINE:Mx86_os2_*)
_vage_CC_      ?= icc /Q+
_vage_3.6_CC_  ?= icc /Q+ ${DEPENDENCIES:D/qmakedep}
.else
_vage_CC_      ?= icc /Q+ ${DEPENDENCIES:D/qmakedep}
.endif

_CC_		?= ${CC:L:@.TOOL.@${_CC_TOOL_}@}


# _CFLAGS_ definitions
#

.if defined(BUILD_SHARED_OBJECTS)

.if !empty(MACHINE:M*_solaris_*) || !empty(MACHINE:Mx86_ptx_*)
_native_CFLAGS_  ?= -Kpic
.elif !empty(MACHINE:Mx86_sco_*)
_native_CFLAGS_  ?= -KPIC
.elif !empty(MACHINE:Mhp9000_ux_*) || !empty(MACHINE:Mia64_hpux_*)
_native_CFLAGS_  ?= +z
.elif !empty(MACHINE:Mmvs390_oe_*)
_native_CFLAGS_  ?= -Wc,EXPORTALL -Wc,DLL
.elif !empty(MACHINE:M*_hposs_*)
_native_CFLAGS_  ?= -Wcall_shared
.endif

# non-AIX platforms' cset requires a PIC flag
.if empty(TARGET_MACHINE:M*_aix_*)
_cset_CFLAGS_  ?= -qpic=small
.endif

_gnu_CFLAGS_   ?= -fpic

_kai_CFLAGS_   ?= -Kpic

_vage_CFLAGS_  ?= /Ge-

.elif defined(USE_SHARED_LIBRARY)

# For the exe modules, MVS must compile with the DLL flag
#
.if !empty(MACHINE:Mmvs390_oe_*)
_native_CFLAGS_  ?= -Wc,DLL
.endif

.endif # BUILD_SHARED_OBJECTS

_visual_CFLAGS_  ?= /nologo

_CC_CFLAGS_  = ${CFLAGS:L:@.TOOL.@${_CC_TOOL_FLAGS_}@}


# Linker/Loader variations
#
LD    ?= cc

.if !empty(MACHINE:Mhp9000_ux_*) || !empty(MACHINE:Mia64_hpux_*)
_native_cc_LD_    ?= cc
_native_ansi_LD_  ?= c89
_native_cpp_LD_   ?= aCC
.elif !empty(MACHINE:Mx86_sco_*) || !empty(MACHINE:M*_solaris_*) || \
      !empty(MACHINE:Mmips_irix_*)
_native_cc_LD_    ?= cc
_native_ansi_LD_  ?= c89
_native_cpp_LD_   ?= CC
.elif !empty(MACHINE:Mmvs390_oe_*) || !empty(MACHINE:Mx86_ptx_*)
_native_cc_LD_    ?= cc
_native_ansi_LD_  ?= c89
_native_cpp_LD_   ?= c++
.elif !empty(MACHINE:Malpha_tru64_*)
_native_cc_LD_    ?= cc
_native_ansi_LD_  ?= c89
_native_cpp_LD_   ?= cxx
.elif !empty(MACHINE:Mas400_os400_*)
_native_LD_       ?= icc
.elif !empty(MACHINE:M*_hposs_*)
_native_LD_       ?= c89
.elif !empty(MACHINE:M*_openvms_*)
_native_cc_LD_    ?= link
_native_ansi_LD_  ?= link
_native_cpp_LD_   ?= cxxlink
.endif

_cset_cc_LD_    ?= cc
_cset_ansi_LD_  ?= c89
_cset_cpp_LD_   ?= xlC
_cset_cppr_LD_  ?= xlC_r

_gnu_cc_LD_     ?= gcc
_gnu_ansi_LD_   ?= gcc
_gnu_cpp_LD_    ?= g++

_kai_LD_    ?= KCC

_vage_LD_   ?= ilink

_visual_LD_ ?= link

_LD_		?= ${LD:L:@.TOOL.@${_CC_TOOL_}@}

# _LDFLAGS_ definitions
#
.if !empty(MACHINE:Mx86_os2_*)
_vage_LDFLAGS_  ?= /OUT:${.TARGET} /NOLOGO /NOI ${GEN_MAPS:D/MAP}
.else
_vage_LDFLAGS_  ?= /OUT:${.TARGET} /NOLOGO ${GEN_MAPS:D/MAP}
.endif

.if !empty(MACHINE:M*_openvms_*)
_native_LDFLAGS_ ?= /EXE=${.TARGET}
.elif !empty(MACHINE:Mmvs390_oe_*) && defined(GEN_MAPS)
_native_LDFLAGS_ ?= -Wl,MAP >${.TARGET:R}${MAP_SUFF}
.elif !empty(MACHINE:M*_linux_*) && defined(GEN_MAPS)
_gnu_LDFLAGS_ ?= -Wl,-M 2>${.TARGET:R}${MAP_SUFF}
.elif !empty(MACHINE:M*_hposs_*) && defined(BUILD_SHARED_OBJECTS)
_native_LDFLAGS_ ?= -Wcall_shared
.endif

_cset_LDFLAGS_  ?= ${GEN_MAPS:D-Wl,-bloadmap\:${.TARGET:R}${MAP_SUFF}}

_visual_LDFLAGS_  ?= /OUT:${.TARGET} /nologo ${GEN_MAPS:D/MAP}

_CC_LDFLAGS_ = ${LDFLAGS:L:@.TOOL.@${_CC_TOOL_FLAGS_}@}

# Shared lib options
#
SHLD    ?= cc

.if !empty(MACHINE:Mhp9000_ux_*) || !empty(MACHINE:Mia64_hpux_*)
_native_cc_SHLD_    ?= ld
_native_ansi_SHLD_  ?= ld
_native_cpp_SHLD_   ?= aCC
.elif !empty(MACHINE:Mx86_sco_*) || !empty(MACHINE:M*_solaris_*) || \
      !empty(MACHINE:Mmips_irix_*)
_native_cc_SHLD_    ?= cc
_native_ansi_SHLD_  ?= c89
_native_cpp_SHLD_   ?= CC
.elif !empty(MACHINE:Mmvs390_oe_*) || !empty(MACHINE:Mx86_ptx_*)
_native_cc_SHLD_    ?= cc
_native_ansi_SHLD_  ?= c89
_native_cpp_SHLD_   ?= c++
.elif !empty(MACHINE:Malpha_tru64_*)
_native_cc_SHLD_    ?= cc
_native_ansi_SHLD_  ?= c89
_native_cpp_SHLD_   ?= cxx
.elif !empty(MACHINE:Mas400_os400_*)
_native_SHLD_       ?= ld
.elif !empty(MACHINE:M*_hposs_*)
_native_SHLD_       ?= c89
.elif !empty(MACHINE:M*_openvms_*)
_native_cc_SHLD_    ?= link
_native_ansi_SHLD_  ?= link
_native_cpp_SHLD_   ?= cxxlink
.endif

.if !empty(TARGET_MACHINE:Mrios_aix_*)
_cset_cc_SHLD_    ?= ld
_cset_ansi_SHLD_  ?= ld
_cset_cpp_SHLD_   ?= makeC++SharedLib
_cset_cppr_SHLD_  ?= makeC++SharedLib_r
.else
_cset_cc_SHLD_    ?= cc
_cset_ansi_SHLD_  ?= c89
_cset_cpp_SHLD_   ?= xlC
_cset_cppr_SHLD_  ?= xlC_r
.endif

_gnu_cc_SHLD_     ?= gcc
_gnu_ansi_SHLD_   ?= gcc
_gnu_cpp_SHLD_    ?= g++

_kai_SHLD_    ?= KCC

_vage_SHLD_   ?= ilink

_visual_SHLD_ ?= link

_SHLD_		  ?= ${SHLD:L:@.TOOL.@${_CC_TOOL_}@}

# _SHLDFLAGS_ definitions
#

.if !empty(MACHINE:M*_solaris_*) || !empty(MACHINE:Mx86_sco_*) || \
    !empty(MACHINE:Mx86_ptx_*)
_native_SHLDFLAGS_  ?= -G
.elif !empty(MACHINE:Mhp9000_ux_*) || !empty(MACHINE:Mia64_hpux_*)
_native_SHLDFLAGS_  ?= -b
.elif !empty(MACHINE:Mmvs390_oe_*)
_native_SHLDFLAGS_  ?= -Wl,DLL ${GEN_MAPS:D-Wl,MAP >${.TARGET:R}${MAP_SUFF}}
.elif !empty(MACHINE:Mmips_irix_*) || !empty(MACHINE:Malpha_tru64_*)
_native_SHLDFLAGS_  ?= -shared
.elif !empty(MACHINE:M*_hposs_*)
_native_SHLDFLAGS_  ?= -Wshared
.elif !empty(MACHINE:M*_openvms_*)
_native_SHLDFLAGS_  ?= /SHARE=${.TARGET}
.endif

.if !empty(TARGET_MACHINE:Mrios_aix_*)
_cset_cc_SHLDFLAGS_    ?= -bnoentry -bM:SRE -bexpall -lc \
                          ${GEN_MAPS:D-bloadmap\:${.TARGET:R}${MAP_SUFF}}
_cset_ansi_SHLDFLAGS_  ?= -bnoentry -bM:SRE -bexpall -lc \
                          ${GEN_MAPS:D-bloadmap\:${.TARGET:R}${MAP_SUFF}}
_cset_cpp_SHLDFLAGS_   ?= -bnoentry -p0 \
                          ${GEN_MAPS:D-bloadmap\:${.TARGET:R}${MAP_SUFF}}
_cset_cppr_SHLDFLAGS_  ?= -bnoentry -p0 \
                          ${GEN_MAPS:D-bloadmap\:${.TARGET:R}${MAP_SUFF}}
.else
_cset_SHLDFLAGS_  ?= -G
.endif

.if !empty(MACHINE:Mx86_beos_*)
_gnu_SHLDFLAGS_   ?= -nostart
.elif !empty(MACHINE:M*_linux_*)
_gnu_SHLDFLAGS_   ?= -shared ${GEN_MAPS:D-Wl,-M 2>${.TARGET:R}${MAP_SUFF}}
.else
_gnu_SHLDFLAGS_   ?= -shared
.endif

.if !empty(MACHINE:Mx86_os2_*)
_vage_SHLDFLAGS_  ?= /NOLOGO /DLL /NOI /OUT:${.TARGET} ${GEN_MAPS:D/MAP}
.else
_vage_SHLDFLAGS_  ?= /NOLOGO /DLL /OUT:${.TARGET} ${GEN_MAPS:D/MAP}
.endif

_visual_SHLDFLAGS_  ?= /DLL /OUT:${.TARGET} \
                       /IMPLIB:${.TARGET:S/${SHLIB_SUFF}$/${IMPLIB_SUFF}/g} \
                       ${GEN_MAPS:D/MAP}

_CC_SHLDFLAGS_ = ${SHLDFLAGS:L:@.TOOL.@${_CC_TOOL_FLAGS_}@}


# Import library tool/flags/args variations
#
_visual_5.0_IMPLIB_      ?= lib
_visual_5.0_IMPLIBFLAGS_ ?= /NOLOGO
_visual_5.0_IMPLIBARGS_  ?= /OUT:${.TARGET} \
			    /DEF:${.ALLSRC:M*${DEFFILE_SUFF}:s|/|\\|g} \
			    ${.ALLSRC:N*${DEFFILE_SUFF}:s|/|\\|g}

_visual_6.0_IMPLIB_      ?= lib
_visual_6.0_IMPLIBFLAGS_ ?= /NOLOGO
_visual_6.0_IMPLIBARGS_  ?= /OUT:${.TARGET} \
			    /DEF${.ALLSRC:M*${DEFFILE_SUFF}:S/^/:/:s|/|\\|g} \
			    ${.ALLSRC:N*${DEFFILE_SUFF}:s|/|\\|g}

.if !empty(MACHINE:Mx86_os2_*)
_vage_IMPLIB_		?= implib
_vage_IMPLIBFLAGS_      ?= /nologo
_vage_IMPLIBARGS_       ?= ${.TARGET} ${.ALLSRC:s|/|\\|g}
_vage_3.6_IMPLIB_       ?= ilib
_vage_3.6_IMPLIBFLAGS_  ?= /gi /nologo
_vage_3.6_IMPLIBARGS_   ?= ${.ALLSRC:s|/|\\|g}
.else
_vage_IMPLIB_       ?= ilib
_vage_IMPLIBFLAGS_  ?= /gi /nologo
_vage_IMPLIBARGS_   ?= ${.ALLSRC:s|/|\\|g}
.endif

_IMPLIB_	 ?= ${IMPLIB:L:@.TOOL.@${_CC_TOOL_}@}
_CC_IMPLIBFLAGS_ ?= ${IMPLIBFLAGS:L:@.TOOL.@${_CC_TOOL_FLAGS_}@}
_CC_IMPLIBARGS_  ?= ${IMPLIBARGS:L:@.TOOL.@${_CC_TOOL_FLAGS_}@}

# cppfilt tool/flags
#
CPPFILT		?= cppfilt
_cset_CPPFILT_  ?= c++filt
_vage_CPPFILT_  ?= cppfilt
_CPPFILT_       ?= ${CPPFILT:L:@.TOOL.@${_CC_TOOL_}@}

_vage_CPPFILTFLAGS_  ?= /b /p /q
_vage_CPPFILTFLAGS_  ?=
_CC_CPPFILTFLAGS_    ?= ${CPPFILTFLAGS:L:@.TOOL.@${_CC_TOOL_FLAGS_}@}

# Archiver/ranlib variations
#
.if !empty(MACHINE:Mas400_os400_*)
AR	    ?= qar
.elif !empty(MACHINE:M*_openvms_*)
AR	    ?= LIBRARY
.else
AR	    ?= ar
.endif
_vage_AR_   ?= ilib
_visual_AR_ ?= lib
_AR_	    ?= ${AR:L:@.TOOL.@${_CC_TOOL_}@}

# _ARFLAGS_ definitions
#
.if !empty(MACHINE:Mas400_os400_*)
_native_ARFLAGS_ ?= -cr
.elif !empty(MACHINE:M*_openvms_*)
_native_ARFLAGS_ ?= /INSERT
.else
_native_ARFLAGS_ ?= -cr
.endif
_gnu_ARFLAGS_    ?= -cr
_cset_ARFLAGS_   ?= -cr
_kai_ARFLAGS_    ?= -cr
_vage_ARFLAGS_   ?= /NOLOGO
_visual_ARFLAGS_ ?= /NOLOGO
_CC_ARFLAGS_     ?= ${ARFLAGS:L:@.TOOL.@${_CC_TOOL_FLAGS_}@}

# Ranlib variations
#
RANLIB		?= ranlib
_RANLIB_	?= ${RANLIB:L:@.TOOL.@${_CC_TOOL_}@}

.ifdef GEN_CC_LISTINGS
.if !empty(MACHINE:Mmvs390_oe_*)
_native_LISTING_FLAGS_ ?= -Wc,LIST\(${.TARGET:R}${LST_SUFF}\)
.endif # various machines
_gnu_LISTING_FLAGS_    ?= -Wa,-a=${.TARGET:R}${LST_SUFF}
_cset_LISTING_FLAGS_   ?= -qlist
_vage_LISTING_FLAGS_   ?= /Fl
_visual_LISTING_FLAGS_ ?= /Fa${.TARGET:R}${LST_SUFF}
.endif # GEN_CC_LISTINGS

_CC_LISTING_FLAGS_ = ${LISTING_FLAGS:L:@.TOOL.@${_CC_TOOL_FLAGS_}@}

#
# Additional compiler flag definitions
#

# _CC_INCDIRS_ definitions
#
_CC_INCDIRS_=${_${_CCFAMILY_}_${_CCTYPE_}_INCDIRS_:U${INCDIRS}}

# _CC_LIBDIRS_ definitions
#
_CC_LIBDIRS_=${_${_CCFAMILY_}_${_CCTYPE_}_LIBDIRS_:U${LIBDIRS}}

#
#  Compilation optimization level.  This should be set to whatever
#  combination of -O and -g flags you desire.  Defaults to -O.
#
#  Allow these flags to be overridden per target
#

_CC_OL_   = ${${.TARGET}_CC_OPT_LEVEL:U${${.TARGET}_OPT_LEVEL:U${OPT_LEVEL:U${CC_OPT_LEVEL:L:@.TOOL.@${_CC_TOOL_}@}}}}
_LD_OL_   = ${${.TARGET}_LD_OPT_LEVEL:U${${.TARGET}_OPT_LEVEL:U${OPT_LEVEL:U${LD_OPT_LEVEL:L:@.TOOL.@${_CC_TOOL_}@}}}}
_SHLD_OL_ = ${${.TARGET}_SHLD_OPT_LEVEL:U${${.TARGET}_OPT_LEVEL:U${OPT_LEVEL:U${SHLD_OPT_LEVEL:L:@.TOOL.@${_CC_TOOL_}@}}}}

# Determine defaults for debug build
.ifdef DEBUGGING

CC_OPT_LEVEL   ?= -g
LD_OPT_LEVEL   ?= -g
SHLD_OPT_LEVEL ?= -g

_cset_CC_OPT_LEVEL_     ?= -Q! -g

.if !empty(MACHINE:Mhp9000_ux_*) || !empty(MACHINE:Mia64_hpux_*)
_native_CC_OPT_LEVEL_   ?= -g${CCTYPE:Mcpp:b0}
_native_LD_OPT_LEVEL_   ?= -g${CCTYPE:Mcpp:b0}
_native_SHLD_OPT_LEVEL_ ?= -g${CCTYPE:Mcpp:b0}
.elif !empty(MACHINE:M*_openvms_*)
_native_CC_OPT_LEVEL_   ?= /DEBUG
_native_LD_OPT_LEVEL_   ?= /DEBUG
_native_SHLD_OPT_LEVEL_ ?= /DEBUG
.endif # !empty(MACHINE:M...*)

_visual_CC_OPT_LEVEL_   ?= /Zi /Yd
_visual_LD_OPT_LEVEL_   ?= /DEBUG
_visual_SHLD_OPT_LEVEL_ ?= /DEBUG

_vage_CC_OPT_LEVEL_     ?= /Oi- /O- /Ti+
_vage_LD_OPT_LEVEL_     ?= /DEBUG
_vage_SHLD_OPT_LEVEL_   ?= /DEBUG

.else # !defined(DEBUGGING)

.if !empty(MACHINE:M*_openvms_*)
_native_CC_OPT_LEVEL_   ?= /OPTIMIZE
.else
CC_OPT_LEVEL   ?= -O
LD_OPT_LEVEL   ?= -O
.endif

.if empty(MACHINE:Mas400_os400_*) && empty(MACHINE:M*_openvms_*)
SHLD_OPT_LEVEL ?= -O
.endif

_vage_CC_OPT_LEVEL_     ?= /O /Ti-
_vage_LD_OPT_LEVEL_     ?= /OPTF
_vage_SHLD_OPT_LEVEL_   ?= /OPTF

_visual_CC_OPT_LEVEL_   ?= /O2
_visual_LD_OPT_LEVEL_   ?= /OPT:REF
_visual_SHLD_OPT_LEVEL_ ?= /OPT:REF


.endif # defined(DEBUGGING)

#
#  Program flags for makefile, environment and command line args
#
# - for INCFLAGS and CDEFS, convert DEP_SUFF targets to OBJ_SUFF targets
_INCFLAGS_=\
	${${.TARGET:S/${DEP_SUFF}$/${OBJ_SUFF}/g}_INCARGS:U${INCARGS}} \
	${${.TARGET:S/${DEP_SUFF}$/${OBJ_SUFF}/g}_INCFLAGS:U${INCFLAGS}} \
	${${.TARGET:S/${DEP_SUFF}$/${OBJ_SUFF}/g}_INCENV:U${INCENV}}
_GENINC_ = ${_CC_GENINC_} ${_INCFLAGS_:G}

_LIBFLAGS_=\
	${${.TARGET}_LIBARGS:U${LIBARGS}}\
	${${.TARGET}_LIBFLAGS:U${LIBFLAGS}}\
	${${.TARGET}_LIBENV:U${LIBENV}}

_GENLIB_ = ${-l ${_LIBFLAGS_}:L:G}

_LIBS_=	${${.TARGET}_LIBSENV:U${LIBSENV}} \
	${${.TARGET}_LIBS:U${LIBS}} \
	${${.TARGET}_LIBSARGS:U${LIBSARGS}}

_EXTRA_LIBS_ = ${${.TARGET}_EXTRA_LIBS:U${EXTRA_LIBS}}

#
# Libs used on the linker line for the SHARED_LIBRARIES and PROGRAMS targets.
#
.if !empty(MACHINE:Mmvs390_oe_*)
_LINKING_LIBS_TMP_ = \
	  ${.ALLSRC:M*${IMPLIB_SUFF}} \
	  ${_EXTRA_LIBS_:M*${IMPLIB_SUFF}:p} \
	  ${_LIBS_:N*${IMPLIB_SUFF}:S/^${LIB_PREF}/\-l/g:S/${STATLIB_SUFF}$//g} \
	  ${_EXTRA_LIBS_:N*${IMPLIB_SUFF}:S/^${LIB_PREF}/\-l/g:S/${STATLIB_SUFF}$//g}
.elif !empty(MACHINE:Mas400_os400_*) || !empty(MACHINE:M*_openvms_*) || \
      ${CCFAMILY} == "vage" || ${CCFAMILY} == "visual"
_LINKING_LIBS_TMP_ = \
	  ${.ALLSRC:M*${IMPLIB_SUFF}} \
	  ${_EXTRA_LIBS_:p}
.if (${STATLIB_SUFF} != ${IMPLIB_SUFF})
_LINKING_LIBS_TMP_ += \
    ${.ALLSRC:M*${STATLIB_SUFF}}
.endif
.else
_LINKING_LIBS_TMP_ = \
	  ${_LIBS_:M*${IMPLIB_SUFF}:S/^${LIB_PREF}/\-l/g:S/${IMPLIB_SUFF}$//g} \
	  ${_EXTRA_LIBS_:M*${IMPLIB_SUFF}:S/^${LIB_PREF}/\-l/g:S/${IMPLIB_SUFF}$//g} \
	  ${_LIBS_:N*${IMPLIB_SUFF}:S/^${LIB_PREF}/\-l/g:S/${STATLIB_SUFF}$//g} \
	  ${_EXTRA_LIBS_:N*${IMPLIB_SUFF}:S/^${LIB_PREF}/\-l/g:S/${STATLIB_SUFF}$//g}
.endif

.if ${BACKSLASH_PATHS}
_LINKING_LIBS_ = ${_LINKING_LIBS_TMP_:s|/|\\|g}
.elif !empty(MACHINE:M*_openvms_*)
_LINKING_LIBS_ = ${_LINKING_LIBS_TMP_:@.PATH.@${VMS_PATH_CONV}@:s|${SHLIB_SUFF}|${SHLIB_SUFF}/SHARE|g:s|${STATLIB_SUFF}|${STATLIB_SUFF}/LIB|g}
.else
_LINKING_LIBS_ = ${_LINKING_LIBS_TMP_}
.endif

_CDEFS_ =\
	${${.TARGET:S/${DEP_SUFF}$/${OBJ_SUFF}/g}_CDEFS:U${CDEFS}}

_CCFLAGS_=\
	${_CC_CFLAGS_} \
	${_CC_OL_} \
	${_CDEFS_} \
	${${.TARGET}_CENV:U${CENV}} \
	${${.TARGET}_CFLAGS:U${CFLAGS}} \
	${${.TARGET}_CARGS:U${CARGS}} \
  ${_CC_LISTING_FLAGS_} \
	${_GENINC_} \
	${_CC_INCDIRS_}
_LDFLAGS_=\
	${_CC_LDFLAGS_} \
	${_LD_OL_} \
	${${.TARGET}_LDENV:U${LDENV}} \
	${${.TARGET}_LDFLAGS:U${LDFLAGS}} \
	${${.TARGET}_LDARGS:U${LDARGS}} \
	${_GENLIB_} \
  ${_CC_LIBDIRS_}
_SHLDFLAGS_=\
	${_CC_SHLDFLAGS_} \
	${_SHLD_OL_} \
	${${.TARGET}_SHLDENV:U${SHLDENV}} \
	${${.TARGET}_SHLDFLAGS:U${SHLDFLAGS}} \
	${${.TARGET}_SHLDARGS:U${SHLDARGS}} \
	${_GENLIB_} \
	${_CC_LIBDIRS_}
.if !empty(MACHINE:M*_openvms_*)
_LDOPTS_   = ${${.TARGET}_LDOPTS:U${LDOPTS}}
_SHLDOPTS_ = ${${.TARGET}_SHLDOPTS:U${SHLDOPTS}}
.endif
.if !defined(NO_LEX_RULES)
_LFLAGS_=\
	${_LEX_LFLAGS_} \
	${${.TARGET}_LENV:U${LENV}} \
	${${.TARGET}_LFLAGS:U${LFLAGS}} \
	${${.TARGET}_LARGS:U${LARGS}}
.endif # NO_LEX_RULES
.if !defined(NO_YACC_RULES)
_YFLAGS_=\
	${${.TARGET}_YENV:U${YENV}} \
  ${${.TARGET}_YFLAGS:U${YFLAGS:U-d}} \
	${${.TARGET}_YARGS:U${YARGS}}
.endif # NO_YACC_RULES
_ODEECHOFLAGS_ = ${${.TARGET}_ODEECHOFLAGS:U${ODEECHOFLAGS}}
_CPFLAGS_ = ${${.TARGET}_CPFLAGS:U${CPFLAGS}}
_LNFLAGS_ = ${${.TARGET}_LNFLAGS:U${LNFLAGS}}
_RMFLAGS_ = ${${.TARGET}_RMFLAGS:U${RMFLAGS}}
_MVFLAGS_ = ${${.TARGET}_MVFLAGS:U${MVFLAGS}}
_ARFLAGS_=\
	${_CC_ARFLAGS_} \
	${${.TARGET}_ARENV:U${ARENV}} \
	${${.TARGET}_ARFLAGS:U${ARFLAGS}} \
	${${.TARGET}_ARARGS:U${ARARGS}}
_IMPLIBFLAGS_=\
	${_CC_IMPLIBFLAGS_} \
	${${.TARGET}_IMPLIBFLAGS:U${IMPLIBFLAGS}}
_IMPLIBARGS_=\
	${_CC_IMPLIBARGS_} \
	${${.TARGET}_IMPLIBARGS:U${IMPLIBARGS}}
_CPPFILTFLAGS_=\
	${_CC_CPPFILTFLAGS_} \
	${${.TARGET}_CPPFILTFLAGS:U${CPPFILTFLAGS}}

#
# Define binary targets
#  Used to determine whether or not to run mkdep
#
.if !empty(PROGRAMS)		 || \
    !empty(LIBRARIES)		 || \
    !empty(SHARED_LIBRARIES)     || \
    !empty(OBJECTS)		 || \
    !empty(NLV_PROGRAMS)	 || \
    !empty(NLV_SHARED_LIBRARIES)
BINARIES =
.endif

#
#  Definitions for clean/clobber
#
_CLEAN_TARGET=${.TARGET:S/^clean_//:S/\//${DIRSEP}/g}

.if !empty(MACHINE:M*_openvms_*)
_CLEAN_RESPFILE=${USE_RESPFILE:D${_CLEAN_TARGET:H}/${_CLEAN_TARGET:T:s|.|_|g}${RESPFILE_SUFF}}
.else
_CLEAN_RESPFILE=${USE_RESPFILE:D${_CLEAN_TARGET}${RESPFILE_SUFF}}
.endif

_CLEAN_DEFAULT_=\
	${_CLEAN_TARGET} \
	${${_CLEAN_TARGET}_OFILES} \
	${_CLEAN_RESPFILE} \
	${${_CLEAN_TARGET}_GARBAGE}
_CLEANFILES_=\
	${CLEANFILES:U${${_CLEAN_TARGET}_CLEANFILES:U${_CLEAN_DEFAULT_}}}

_RMTARGET_TARGET=${.TARGET:S/^rmtarget_//:S/\//${DIRSEP}/g}

_CLOBBER_TARGET=${.TARGET:S/^clobber_//:S/\//${DIRSEP}/g}

.if !empty(MACHINE:M*_openvms_*)
_CLOBBER_RESPFILE=${USE_RESPFILE:D${_CLOBBER_TARGET:H}/${_CLOBBER_TARGET:T:s|.|_|g}${RESPFILE_SUFF}}
.else
_CLOBBER_RESPFILE=${USE_RESPFILE:D${_CLOBBER_TARGET}${RESPFILE_SUFF}}
.endif

_CLOBBER_DEFAULT_=\
	${_CLOBBER_TARGET} \
	${${_CLOBBER_TARGET}_OFILES} \
	${_CLOBBER_RESPFILE} \
	${${_CLOBBER_TARGET}_GARBAGE}
_CLOBBERFILES_=\
	${CLOBBERFILES:U${${_CLOBBER_TARGET}_CLOBBERFILES:U${_CLOBBER_DEFAULT_}}}

#
# OBJECTDIRTOP is usually appended to MAKETOP to get to the obj root
# directory (prior to any CONTEXT-specific directories) from wherever 
# mk is running.
#
OBJECTDIRTOP ?= ${MAKEOBJDIR:xs|[/\\][^/\\]+|/..|ge:S|$|/|:xs|^..[/\\]..[/\\]||}

#
#  Definitions for setup
#
SETUPBASE     ?= ${MAKETOP}${OBJECTDIRTOP}../tools/${CONTEXT}
_SETUP_TARGET  = ${.TARGET:S/^setup_//}
_SETUPDIR_     = ${${_SETUP_TARGET}_SETUPDIR:U${SETUPDIR:U/_MISSING_SETUPDIR_/}}
_SETUPFILES_   = ${SETUPBASE}${_SETUPDIR_}${_SETUP_TARGET:T}

#
#  Definitions for export rules
#

#
# Various defaults for exporting
#
EXPORTDIR    ?= ../export/${CONTEXT}
EXPORTDIRTOP ?= ${EXPORTDIR:xs|[/\\][^/\\]+|/..|ge:S|$|/|:xs|^..[/\\]..[/\\]||}
EXPORTBASE   ?= ${MAKETOP}${OBJECTDIRTOP}${EXPORTDIR}

#
# Definitions for export programs
#
_EXPPGM_TARGET=${.TARGET:S/^exppgm_//:T}
_EXPPGMDIR_=${${_EXPPGM_TARGET}_EXPDIR:U${EXPPGMDIR:U${EXPDIR:U${${_EXPPGM_TARGET}_IDIR:U${IDIR:U/_MISSING_EXPDIR_/}}}}}
_EXPPGMFILES_=${_EXPPGMDIR_:@.DIR.@${EXPORTBASE}${.DIR.}${_EXPPGM_TARGET}@}
_EXPPGMTOP_ = ${EXPPGMTOP:U${EXPTOP:U${_EXPPGMDIR_:xs|[/\\][^/\\]+|/..|ge:xs|^[/\\]||}}}
.ifdef USE_OLD_RELEXP_METHOD
RELATIVE_EXPPGM_TARGET ?= ${_EXPPGMTOP_}${EXPORTDIRTOP}../${ODESRCNAME}${MAKEDIR}/${.TARGET:T}
.else
RELATIVE_EXPPGM_TARGET ?= ${BACKED_SANDBOXDIR:s/${PATHSEP}/ /g:xs|[/\\][/\\]+|${DIRSEP}|ge:xS|[/\\]\$||g:@WORD@${.ALLSRC:M${WORD}/*:s|^${WORD}|${_EXPPGMTOP_}${EXPORTDIRTOP}..|}@:B${.ALLSRC}}
.endif
_RELATIVE_EXPPGM_TARGET_ = ${${.TARGET:T}_RELATIVE_EXPPGM_TARGET:U${RELATIVE_EXPPGM_TARGET}}

#
#  Definitions for export libraries
#
_EXPORT_TARGET = ${.TARGET:S/^export_//:T}
_EXPLIBDIR_ = ${${_EXPORT_TARGET}_EXPDIR:U${EXPLIBDIR:U${EXPDIR:U${${_EXPORT_TARGET}_IDIR:U${IDIR:U/_MISSING_EXPDIR_/}}}}}
_EXPFILES_=${_EXPLIBDIR_:@.DIR.@${EXPORTBASE}${.DIR.}${_EXPORT_TARGET}@}
_EXPLIBTOP_ = ${EXPLIBTOP:U${EXPTOP:U${_EXPLIBDIR_:xs|[/\\][^/\\]+|/..|ge:xs|^[/\\]||}}}
.ifdef USE_OLD_RELEXP_METHOD
RELATIVE_EXPLIB_TARGET ?= ${_EXPLIBTOP_}${EXPORTDIRTOP}${MAKEOBJDIR:B../${ODESRCNAME}}${MAKEDIR}/${.TARGET:T}
.else
RELATIVE_EXPLIB_TARGET ?= ${BACKED_SANDBOXDIR:s/${PATHSEP}/ /g:xs|[/\\][/\\]+|${DIRSEP}|ge:xS|[/\\]\$||g:@WORD@${.ALLSRC:M${WORD}/*:s|^${WORD}|${_EXPLIBTOP_}${EXPORTDIRTOP}..|}@:B${.ALLSRC}}
.endif
_RELATIVE_EXPLIB_TARGET_ = ${${.TARGET:T}_RELATIVE_EXPLIB_TARGET:U${RELATIVE_EXPLIB_TARGET}}

#
# Definitions for export includes
#
_EXPINC_TARGET=${.TARGET:S/^expinc_//:T}
_EXPINCDIR_=${${_EXPINC_TARGET}_EXPDIR:U${EXPINCDIR:U${EXPDIR:U${${_EXPINC_TARGET}_IDIR:U${IDIR:U/_MISSING_EXPDIR_/}}}}}
_EXPINCFILES_=${_EXPINCDIR_:@.DIR.@${EXPORTBASE}${.DIR.}${_EXPINC_TARGET}@}
_EXPINCTOP_ = ${EXPINCTOP:U${EXPTOP:U${_EXPINCDIR_:xs|[/\\][^/\\]+|/..|ge:xs|^[/\\]||}}}
.ifdef USE_OLD_RELEXP_METHOD
RELATIVE_EXPINC_TARGET ?= ${_EXPINCTOP_}${EXPORTDIRTOP}../${ODESRCNAME}${MAKEDIR}/${.TARGET:T}
.else
RELATIVE_EXPINC_TARGET ?= ${BACKED_SANDBOXDIR:s/${PATHSEP}/ /g:xs|[/\\][/\\]+|${DIRSEP}|ge:xS|[/\\]\$||g:@WORD@${.ALLSRC:M${WORD}/*:s|^${WORD}|${_EXPINCTOP_}${EXPORTDIRTOP}..|}@:B${.ALLSRC}}
.endif
_RELATIVE_EXPINC_TARGET_ = ${${.TARGET:T}_RELATIVE_EXPINC_TARGET:U${RELATIVE_EXPINC_TARGET}}

#
#  Definitions for install/release
#
.if make(instdep_all)
_INSTDEP_TARGET=${.TARGET:S/^instdep_//:T}
.elif defined(INSTALL_USING_DEPS)
_INSTALL_TARGET=${.TARGET:S/^instdep_//:T}
.else
_INSTALL_TARGET=${.TARGET:S/^install_//}
.endif # defined(INSTALL_USING_DEPS)

#
#  Include Java variable definitions
#
.if !empty(MACHINE:M*_openvms_*)
.include <java_std.mk>
.else
.include <java.std.mk>
.endif

#
#  Default single suffix compilation rules
#
.SUFFIXES: ${CC_SUFF} ${OBJ_SUFF} \
	   ${STATLIB_SUFF} ${IMPLIB_SUFF} ${SHLIB_SUFF} \
	   ${PP_SUFF} ${NO_YACC_RULES:U${YACC_SUFF} .h} ${NO_LEX_RULES:U${LEX_SUFF}}

_LIB_PATH_ := ${-z -I. ${_LDFLAGS_:M-L*}:L:G}

.PATH${STATLIB_SUFF}: ${_LIB_PATH_}
.PATH${IMPLIB_SUFF}: ${_LIB_PATH_}
.PATH${SHLIB_SUFF}: ${_LIB_PATH_}

_EXPORT_PATHS = ${EXPDIR:D${BACKED_SANDBOXDIR:s|${PATHSEP}| |g:S|$|/export/${CONTEXT}${EXPDIR:S|/$||g}|g}}

_PATH_ := ${-z -I. ${_LIBFLAGS_} ${_EXPORT_PATHS:S/^/-E&/g}:L:G}

.PATH: ${_PATH_}

_path_: .SPECTARG
	${.PATH${STATLIB_SUFF} = ${.PATH${STATLIB_SUFF}}:L:a&STDOUT}
	${.PATH = ${.PATH}:L:a&STDOUT}

# Defined PKG_IS_SUPPORTED for those OSes that have packaging support
.if !empty(MACHINE:M*_aix_*) || !empty(MACHINE:M*_solaris_*) || \
    !empty(MACHINE:Mx86_nt_*) || !empty(MACHINE:Mx86_95_*) || \
    !empty(MACHINE:Mmvs390_oe_*) || !empty(MACHINE:Mx86_sco_*) || \
    !empty(MACHINE:Mhp9000_ux_*) || !empty(MACHINE:M*_linux_*) || \
    !empty(MACHINE:Mmips_irix_*) || !empty(MACHINE:Mia64_hpux_*)
PKG_IS_SUPPORTED =
.else
.undef PKG_IS_SUPPORTED
.endif

#
#  Include pass definitions for standard targets
#
.include <passes.mk>

#
# Include build and packaging standard rules
#
.if !empty(MACHINE:M*_openvms_*)
.include <bld_std.mk>
.if ${USE_TSO_RULES}
.include <tso_std.mk>
.endif
.else
.include <bld.std.mk>
.if ${USE_TSO_RULES}
.include <tso.std.mk>
.endif
.endif

.ifndef OMIT_PACKAGING_RULES
.if !empty(MACHINE:M*_openvms_*)
.include <pkg_std.mk>
.else
.include <pkg.std.mk>
.endif
.endif


.endif  # __STD_MK_
