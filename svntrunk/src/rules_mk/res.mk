################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines variables and targets for resource compilation.
#  Some variables used:
#   _RC_
#       Tool to compile rc files to res files.
#       Note that in OS/2, _RC_ is also used to link the .res file to the
#       exe or dll file.
#   RESOURCES, NLV_RESOURCES
#       The RES files to be produced
#   RC_SUFF
#       Extension of resources source file.
#   RES_SUFF
#       Extension of binary output of resource compilation.
#   BMP_SUFF, CUR_SUFF, FNT_SUFF, ICO_SUFF
#       bitmap, cursor, font, and icon file suffixes that can be included
#       in a resource compilation.
#   RESOURCES_SRCS
#       Variable with all files that the RES files depend upon, such as
#       rc, header, bitmap, cursor, font, or icon files.
#       This variable can be target-specific.
#   RCFLAGS
#       Makefile flags for _RC_
#   RCENV
#       Environment variables for _RC_
#   RCARGS
#       build/mk command line arguments for _RC_
#   INCARGS, INCENV, and INCFLAGS are used with _RC_ and are defined elsewhere
#   for the C/C++ compilers.
#
################################################################################
.if !defined(__RES_MK_)
__RES_MK_=

# bitmap file suffixes, for WinNT, Win95, and OS/2
BMP_SUFF?=.bmp
.if !empty(MACHINE:Mx86_os2_*)
# cursor file suffixes, for OS/2
CUR_SUFF?=.ptr
.else
# cursor file suffixes, for WinNT, Win95
CUR_SUFF?=.cur
.endif
# font file suffixes, for WinNT, Win95, and OS/2
FNT_SUFF?=.fnt
# icon file suffixes, for WinNT, Win95, and OS/2
ICO_SUFF?=.ico

# Resource compiler variations
#
RC	    ?= rc
.if !empty(MACHINE:Mx86_os2_*)
_vage_RC_   ?= rc
.else
_vage_RC_   ?= irc
.endif
_visual_RC_ ?= rc
_RC_	    ?= ${RC:L:@.TOOL.@${_CC_TOOL_}@}

# new suffixes for various kinds of resource files
.SUFFIXES: ${RC_SUFF} ${RES_SUFF}

# resource compiler flag definitions
_RCFLAGS_=\
	${${.TARGET}_RCENV:U${RCENV}} \
	${${.TARGET}_RCFLAGS:U${RCFLAGS}} \
	${${.TARGET}_RCARGS:U${RCARGS}}

# bitmap, cursor, font, and icon files that are mentioned in the RC_SUFF 
# file are handled by this statement

# All the resource files (both non-NLV and NLV)
_RESOURCES_ ?= ${RESOURCES} ${_NLV_RESOURCES_}
_RESOURCES_SRCS_ ?= ${${.TARGET}_RESOURCES_SRCS:U${RESOURCES_SRCS:U\
${.TARGET:T:S;${RES_SUFF}$;${RC_SUFF};}}}

.ifdef NLV
_RES_GENINC_ ?= ${_NLV_GENINC_:S;/;\\;g} ${_GENINC_:S;/;\\;g}
.else
_RES_GENINC_ ?= ${_GENINC_:S;/;\\;g}
.endif

# Copy the files in _RESOURCES_SRCS_ and the *.rc file to the current directory
# if we are in a sandbox, to build it here.
.if !empty(_RESOURCES_)
${_RESOURCES_} : $${_RESOURCES_SRCS_}
	${.TARGET:H:C}
	-${_RESOURCES_SRCS_:b${IN_SANDBOX:b${${MAKETOP}../../src${MAKEDIR}:L:C-}}} 
	-${_RESOURCES_SRCS_:b${IN_SANDBOX:b${CHDIR} \
	 ${${MAKETOP}../../src${MAKEDIR}:L:S;/;\\;g} ${CMDSEP} \
	 mklinks -quiet -auto -norefresh ${.ALLSRC:T:N*.h}}}
.if !empty(MACHINE:Mx86_os2_*)
	-${_RESOURCES_SRCS_:b${${_RESOURCES_SRCS_:N*.h:@.F.@${.F.:! \
	 ${CHDIR} ${${MAKETOP}../../src${MAKEDIR}:L:S;/;\\;g} ${CMDSEP} \
	 ${CP} ${.F.:S;/;\\;g} ${CURDIR:S;/;\\;g}!}@}:L:a${NULL_DEVICE}}}
	${_RC_} ${_RCFLAGS_:S/^-D/-D /g} ${_RES_GENINC_:S/^-I/-I /g} -r \
	 ${${MAKETOP}../../src${MAKEDIR}/${.IMPSRC:T}:L:S;/;\\;g} ${.TARGET}
.else
	${_RC_} ${_RCFLAGS_} ${_RES_GENINC_} -Fo${.TARGET:S;/;\\;g} \
	 ${.ALLSRC:S;/;\\;g:M*.rc}
.endif # !empty(MACHINE:Mx86_os2_*)

.endif # _RESOURCES_

# The implied transformation from .rc to .res
${RC_SUFF}${RES_SUFF}:
	${.TARGET:H:C}
	-${_RESOURCES_SRCS_:b${IN_SANDBOX:b${${MAKETOP}../../src${MAKEDIR}:L:C-}}} 
	-${_RESOURCES_SRCS_:b${IN_SANDBOX:b${CHDIR} \
	 ${${MAKETOP}../../src${MAKEDIR}:L:S;/;\\;g} ${CMDSEP} \
	 mklinks -quiet -auto -norefresh ${_RESOURCES_SRCS_:N*.h} ${.IMPSRC:T}}}
.if !empty(MACHINE:Mx86_os2_*)
	-${_RESOURCES_SRCS_:b${${_RESOURCES_SRCS_:N*.h:@.F.@${.F.:! \
	 ${CHDIR} ${${MAKETOP}../../src${MAKEDIR}:L:S;/;\\;g} ${CMDSEP} \
	 ${CP} ${.F.:S;/;\\;g} ${CURDIR:S;/;\\;g}!}@}:L:a${NULL_DEVICE}}}
	${_RC_} ${_RCFLAGS_:S/^-D/-D /g} ${_RES_GENINC_:S/^-I/-I /g} -r \
	 ${${MAKETOP}../../src${MAKEDIR}/${.IMPSRC:T}:L:S;/;\\;g} ${.TARGET}
.else
	${_RC_} ${_RCFLAGS_} ${_RES_GENINC_} -Fo${.TARGET:S;/;\\;g} \
	 ${.IMPSRC:S;/;\\;g:M*.rc}
.endif # !empty(MACHINE:Mx86_os2_*)

.endif # __RES_MK_
