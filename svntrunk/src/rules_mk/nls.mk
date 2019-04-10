################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines variables and targets for National Language Support (NLS)
#  Some variables used:
#   NLV
#       The current language version to be built.  For example, NLV=en_US
#   NLV_PROGRAMS, _NLV_PROGRAMS_
#       The program names with the ${PROG_SUFF} suffix. _ version of the 
#       variable has the relative path set to the non-_ version.
#   NLV_SHARED_LIBRARIES, _NLV_SHARED_LIBRARIES_
#       The shared library names with the ${SHLIB_SUFF} suffix. _ version
#       of the variable has the relative path set to the non-_ version.
#   NLV_HELPS, _NLV_HELPS_
#   NLV_IPFHELPS, _NLV_IPFHELPS_
#       The help file names with the ${HLP_SUFF} suffix. _ version of the 
#       variable has the relative path set to the non-_ version.
#   NLV_INFS, _NLV_INFS_
#       The document file names with the ${INF_SUFF} suffix. _ version of the 
#       variable has the relative path set to the non-_ version.
#   CATALOGS
#       Names of .CAT files to generate using 'gencat'
#   MSG_HDRS
#       C include header files that are generated from .msg files by using
#       mkcatdefs piped through gencat
#   CODEPAGE_TGTS
#       Files that are to be run through the iconv program
#   CODEPAGE_TO, CODEPAGE_FROM
#       Target and source codepages to be used with the iconv program
#   _NLV_SEARCHPATH_
#       Search path that contains the primary and alternative search paths
#   _NLV_GENINC_
#       C include search path that contains NLV only directories (like _GENINC_)
#   <NLV>_ALT
#       Alternative NLV to use for current NLV.
#   GENCAT
#       Tool name of 'gencat' to generate catalog files
#   MKCATDEFS
#       Tool name of 'mkcatdef[s]' to generate catalog files and header files
#
################################################################################
.if !defined(__NLS_MK_)
__NLS_MK_=

# Equal to relative depth

# OBJECTDIR
OBJTOP ?= ..

# ODESRCNAME
SRCTOP ?= ..

# ODEMRINAME
MRITOP ?= ..

################################################################################
# Search order for languages.  Need to find alternate language files.
# For example, for a fr_FA (France French) build, you want a backup of 
# fr_CA (Canadien French), if fr_CA doesn't then use en_US (US English).
#   en_UK_ALT ?= en_US
#   fr_CA_ALT ?= en_US
#   fr_FA_ALT ?= fr_CA
NLV_ALT   ?= ${NLV} ${${NLV}_ALT} ${${${NLV}_ALT}_ALT} ${${${${NLV}_ALT}_ALT}_ALT}

# NLV search paths
_NLV_SEARCHPATH2_ = ${BACKED_SANDBOXDIR:s!${PATHSEP}! !g:@.PATH.@${.PATH.}/${ODEMRINAME}/${MAKEOBJDIR}/${.NLV.}${MAKEDIR} ${.PATH.}/${ODEMRINAME}/${.NLV.}${MAKEDIR}@}
_NLV_SEARCHPATH_      = ${NLV_ALT:@.NLV.@${_NLV_SEARCHPATH2_}@}

_NLV_GENINC_	      = ${_NLV_SEARCHPATH_:S!/!${DIRSEP}!g:S!^!-I!g:G}


################################################################################
# NLS Directories
OBJ_NLVDIR ?= ${MAKETOP}${.NLV.}${MAKEDIR}
EXP_NLVDIR ?= ${MAKETOP}${.NLV.}${MAKEDIR}
SRC_NLVDIR ?= ${MAKETOP}${.NLV.}${MAKEDIR}

################################################################################
# Take 'NLVPROGRAMS=foo.exe' and for 'NLV=en_US en_UK fr_FA' then
# _NLV_PROGRAMS_ = c:\bb\obj\x86_nt_4\en_US\foo.exe \
#		   c:\bb\obj\x86_nt_4\en_UK\foo.exe \
#		   c:\bb\obj\x86_nt_4\fr_FA\foo.exe
_NLV_PROGRAMS_	       ?= \
  ${NLV_PROGRAMS:b${NLV:@.NLV.@${OBJ_NLVDIR}/${NLV_PROGRAMS}@}}

_NLV_SHARED_LIBRARIES_ ?= \
  ${NLV_SHARED_LIBRARIES:b${NLV:@.NLV.@${OBJ_NLVDIR}/${NLV_SHARED_LIBRARIES}@}}

_NLV_HELPS_	       ?= \
  ${NLV_HELPS:b${NLV:@.NLV.@${OBJ_NLVDIR}/${NLV_HELPS}@}}

_NLV_IPFHELPS_		  ?= \
  ${NLV_IPFHELPS:b${NLV:@.NLV.@${OBJ_NLVDIR}/${NLV_IPFHELPS}@}}

_NLV_INFS_	      ?= \
  ${NLV_INFS:b${NLV:@.NLV.@${OBJ_NLVDIR}/${NLV_INFS}@}}

_NLV_RESOURCES_	       ?= \
  ${NLV_RESOURCES:b${NLV:@.NLV.@${OBJ_NLVDIR}/${NLV_RESOURCES}@}}

################################################################################
# Rules to generate catalog files
.SUFFIXES: ${MSG_SUFF} ${CAT_SUFF}

#.PATH${MSG_SUFF}: ${_NLV_SEARCHPATH_}
.ifdef NLV
_NON_NLV_PATH_ := ${.PATH}
# Out with the old
.PATH:
# In with the new, we need to make sure we pick up NLS versions first
.PATH: ${_NLV_SEARCHPATH_} ${_NON_NLV_PATH_}
.endif # def NLV

# Catalogs will be generated in the object dir
_CATALOGS_   = ${CATALOGS:@.CAT.@${NLV:@.NLV.@${OBJ_NLVDIR}/${.CAT.}@}@}

# Sources are either target specific, non-target specific, or based off
#   the base of the target name.
_CATALOGS_SRCS_ = ${${.TARGET}_CATALOGS_SRCS:U${CATALOGS_SRCS:U${.TARGET:T:R}${MSG_SUFF}}}

# @@@ need to set LANG environ var before running gencat?
# @@@ _GENCAT_  ?= LC_CTYPE=${NLV} LOCPATH=${LOCPATH} LIBPATH=${LOCPATH} gencat
GENCAT    ?= gencat
_GENCAT_  ?= ${GENCAT}
_GENCATFLAGS_ ?= ${${.TARGET}_GENCATENV:U${GENCATENV}} \
	     ${${.TARGET}_GENCATFLAGS:U${GENCATFLAGS}} \
	     ${${.TARGET}_GENCATARGS:U${GENCATARGS}}

.ifdef UNIX
MKCATDEFS   ?= mkcatdefs
.else
MKCATDEFS   ?= mkcatdef
.endif # def UNIX
_MKCATDEFS_ ?= ${MKCATDEFS}

_MKCATFLAGS_ ?= ${${.TARGET}_MKCATENV:U${MKCATENV}} \
	     ${${.TARGET}_MKCATFLAGS:U${MKCATFLAGS}} \
	     ${${.TARGET}_MKCATARGS:U${MKCATARGS}}

# Like a suffix transformation rule for .msg->.cat
#   The reason we don't use a suffix transformation is that the target
# name could have a relative path in it, and the source is not in that
# relative path.  This will happen when the <nlv>_ALT search paths are
# used.
${_CATALOGS_}: $${_CATALOGS_SRCS_}
	${.TARGET:H:C}
	${.TARGET:S;/;${DIRSEP};g:rm-}
.ifdef USE_MKCATDEFS
	-${${.TARGET:R:S;/;${DIRSEP};g}${MSG_HDRS_TAIL}.h:L:rm-}
.if !empty(MACHINE:Mmvs390_oe_*)
	${_MKCATDEFS_} ${_MKCATFLAGS_} ${.ALLSRC:T:R} ${.ALLSRC} | ${_GENCAT_} ${_GENCATFLAGS_} ${.TARGET}
.else
	${_MKCATDEFS_} ${.ALLSRC:T:R} ${.ALLSRC} ${_MKCATFLAGS_} | ${_GENCAT_} ${_GENCATFLAGS_} ${.TARGET}
.endif # !empty(MACHINE:Mmvs390_oe_*)
.else
	${_GENCAT_} ${_GENCATFLAGS_} ${.TARGET} ${.ALLSRC}
.endif # USE_MKCATDEF

_MSG_HDRS_      ?= ${MSG_HDRS}
_MSG_HDRS_SRCS_ ?= ${${.TARGET}_MSG_HDRS_SRCS:U${MSG_HDR_SRCS:U${.TARGET:R:S;${MSG_HDRS_TAIL}$;;}${MSG_SUFF}}}

# Like a suffix transformation rule for .msg->.h
#   The reason we don't use a suffix transformation is that the target
# name could have a relative path in it, and the source is not in that
# relative path.  This will happen when the <nlv>_ALT search paths are
# used.
${_MSG_HDRS_}: $${_MSG_HDRS_SRCS_}
	${.TARGET:H:C}
	${.TARGET:S;/;${DIRSEP};g:rm-}
	-${${.TARGET:R:S;/;${DIRSEP};g:S;${MSG_HDRS_TAIL}$;;}${CAT_SUFF}:L:rm-}
	${_MKCATDEFS_} ${.ALLSRC:T:R:S;${MSG_HDRS_TAIL}$;;} \
	   ${.ALLSRC:S;/;${DIRSEP};g} \
	   ${_MKCATFLAGS_} | ${_GENCAT_} ${_GENCATFLAGS_} \
	   ${.TARGET:R:S;/;${DIRSEP};g:S;${MSG_HDRS_TAIL}$;;}${CAT_SUFF}

################################################################################
# Rules to convert code pages
ICONV	     ?= iconv
_ICONV_      ?= ${ICONV}
_ICONVFLAGS_ ?= ${${.TARGET}_ICONVENV:U${ICONVENV}} \
		${${.TARGET}_ICONVFLAGS:U${ICONVFLAGS}} \
		${${.TARGET}_ICONVARGS:U${ICONVARGS}}

_CODEPAGE_TGT_   = ${NLV:@.NLV.@${MAKETOP}${OBJTOP}/${MAKEOBJDIR}/${.NLV.}${MAKEDIR}/${.FILE.}@}
_CODEPAGE_TGTS_  = ${CODEPAGE_TGTS:@.FILE.@${_CODEPAGE_TGT_}@}
_CODEPAGE_SRCS_  = ${${.TARGET:T}_CODEPAGE_SRCS:U${CODEPAGE_SRCS:U${.TARGET:T}${ICONV_SUFF}}}
_CODEPAGE_TO_   ?= ${${.TARGET}_CODEPAGE_TO:U${CODEPAGE_TO}}
_CODEPAGE_FROM_ ?= ${${.TARGET}_CODEPAGE_FROM:U${CODEPAGE_FROM}}

# Some default code page conversions
CODEPAGE_FROM ?= IBM-850
.if !empty(MACHINE:Mx86_nt_*) || !empty(MACHINE:Mx86_os2_*)
CODEPAGE_TO ?= IBM-437
.else
CODEPAGE_TO ?= ISO8859-1
.endif

# Code page conversion rule
#   The reason we don't use a suffix transformation is that the target
# name could have a relative path in it, and the source is not in that
# relative path.  This will happen when the <nlv>_ALT search paths are
# used.  We also don't use a suffix transformation since there are no
# well-defined suffixes to transform.
${_CODEPAGE_TGTS_}: $${_CODEPAGE_SRCS_}
	${.TARGET:H:C}
	${.TARGET:S;/;${DIRSEP};g:rm-}
.ifdef USE_CRLFCON
	${_CRLFCON_} ${.ALLSRC} ${.TARGET:H}${DIRSEP}${.ALLSRC:T}.tmp
.else
	${CP} ${.ALLSRC:S;/;${DIRSEP};g} ${.TARGET:S;/;${DIRSEP};g}.tmp
.endif
	${_ICONV_} ${_ICONVFLAGS_} -f ${_CODEPAGE_FROM_} -t ${_CODEPAGE_TO_} \
	  ${.TARGET:S;/;${DIRSEP};g}.tmp > ${.TARGET:S;/;${DIRSEP};g}
	-${${.TARGET:S;/;${DIRSEP};g}.tmp:L:rm-}

.endif # __NLS_MK_
