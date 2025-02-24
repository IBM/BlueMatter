################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines variables and targets for processing of help & message files
#  Some variables used:
#   HELPS, NLV_HELPS
#       Help files to compile using HCRTF command
#   MC_HDRS
#       C inlude header files that are generated by the MC commands
#   MC_SUFF
#       Suffix for sources for the MC command.
#   HLP_SUFF
#       Target suffix for Microsoft Windows help files, output from HCRTF
#   HPJ_SUFF, RTF_SUFF
#       Souce suffixes for input to the HCRTF command
#   HDR_SUFF
#       C include header file suffix that is generated from the MC command
#
################################################################################
.if !defined(__HELP_MK_)
__HELP_MK_=

# Define these suffixes for the following transformations
.SUFFIXES: ${MC_SUFF} ${RC_SUFF} ${HDR_SUFF} ${HPJ_SUFF} ${HLP_SUFF}

# For both MS-VisualC++ and IBM-VisualAgeC++ it is MC
MC   ?= mc
_MC_ ?= ${MC}

MC_DEFAULTS ?= -r
_MCFLAGS_ ?= ${MC_DEFAULTS} \
	     ${${.TARGET}_MCENV:U${MCENV}} \
	     ${${.TARGET}_MCFLAGS:U${MCFLAGS}} \
	     ${${.TARGET}_MCARGS:U${MCARGS}} 

_MC_HDRS_      ?= ${MC_HDRS}
_MC_HDRS_SRCS_ ?= ${.TARGET:T:R}${MC_SUFF}
# .mc -> .h
${MC_SUFF}${HDR_SUFF}:
	${_MC_} ${_MCFLAGS_} ${.TARGET:H:B.S;/;${DIRSEP};g} ${.IMPSRC:S;/;${DIRSEP};g}

${_MC_HDRS_} : $${_MC_HDRS_SRCS_}
	${_MC_} ${_MCFLAGS_} ${.TARGET:H:B.S;/;${DIRSEP};g} ${.ALLSRC:S;/;${DIRSEP};g}

# Both non-NLV and NLV help files makeup the definition for the high-level
# variable for help files.
_HELPS_ ?= ${HELPS} ${_NLV_HELPS_}

HC   ?= hcrtf
_HC_ ?= ${HC}
HC_DEFAULTS ?= -x -xn
_HCFLAGS_ ?= ${HC_DEFAULTS} \
	     ${${.TARGET}_HCENV:U${HCENV}} \
	     ${${.TARGET}_HCFLAGS:U${HCFLAGS}} \
	     ${${.TARGET}_HCARGS:U${HCARGS}}
	     
_HELPS_SRCS_ ?= ${${.TARGET}_HELPS_SRCS:U${HELPS_SRCS:U${.TARGET:R}${RTF_SUFF}}}
#.hpj -> .hlp
${HPJ_SUFF}${HLP_SUFF}: $${_HELPS_SRCS_}
	${.TARGET:H:C}
	${${.TARGET:R:S|/|${DIRSEP}|g}.PH:rm-}
	${_HC_} ${_HCFLAGS_} /o${CURDIR:S|/|${DIRSEP}|g}${DIRSEP}${.TARGET:T} ${.IMPSRC:S|/|${DIRSEP}|g}
	${.TARGET:M*/*:b copy ${.TARGET:T} ${.TARGET:S|/|${DIRSEP}|g}}
	${.TARGET:M*/*:b${.TARGET:T:rm-}}

# In VisualAge ipfc can compile .ipf files to .hlp files or .inf documents.
IPFC   ?= ipfc
_IPFC_ ?= ${IPFC}

# All the ipf help files (both non-NLV and NLV)
_IPFHELPS_ ?= ${IPFHELPS} ${_NLV_IPFHELPS_}
.if !empty(_IPFHELPS_)
IPFHELPS_DEFAULTS ?= /q /hlp
_IPFHELPSFLAGS_ ?= ${IPFHELPS_DEFAULTS} \
	     ${${.TARGET}_IPFHELPSENV:U${IPFHELPSENV}} \
	     ${${.TARGET}_IPFHELPSFLAGS:U${IPFHELPSFLAGS}} \
	     ${${.TARGET}_IPFHELPSARGS:U${IPFHELPSARGS}}
_IPFHELPS_SRCS_ ?= ${${.TARGET}_IPFHELPS_SRCS:U${IPFHELPS_SRCS:U${.TARGET:R}${IPF_SUFF}}}
#_IPFHELPS_SRCS_ = ${.TARGET:R}${IPF_SUFF}
# Copy the files in _IPFHELPS_SRCS_ and the *.ipf file to the current directory
# if we are in a sandbox, to build it here.
${_IPFHELPS_} : $${_IPFHELPS_SRCS_}
	${.TARGET:H:C}
	-${_IPFHELPS_SRCS_:b${IN_SANDBOX:b${${MAKETOP}../../src${MAKEDIR}:L:C-}}} 
	-${_IPFHELPS_SRCS_:b${IN_SANDBOX:b${CHDIR} \
	 ${${MAKETOP}../../src${MAKEDIR}:L:S;/;\\;g} ${CMDSEP} \
	 mklinks -quiet -auto -norefresh ${.ALLSRC:T}}}
	-${_IPFHELPS_SRCS_:b${${_IPFHELPS_SRCS_:@.F.@${.F.:!\
	 ${CHDIR} ${${MAKETOP}../../src${MAKEDIR}:L:S;/;\\;g} ${CMDSEP} \
	 ${CP} ${.F.:S;/;\\;g} ${CURDIR:S;/;\\;g}!}@}:L:a${NULL_DEVICE}}}
	${_IPFC_} ${_IPFHELPSFLAGS_} ${.TARGET:R}${IPF_SUFF}
.endif # _IPFHELPS_

# All the inf files (both non-NLV and NLV)
_INFS_ ?= ${INFS} ${_NLV_INFS_}
.if !empty(_INFS_)
INFS_DEFAULTS ?= /q /inf
_INFSFLAGS_ ?= ${INFS_DEFAULTS} \
	     ${${.TARGET}_INFSENV:U${INFSENV}} \
	     ${${.TARGET}_INFSFLAGS:U${INFSFLAGS}} \
	     ${${.TARGET}_INFSARGS:U${INFSARGS}}
_INFS_SRCS_ ?= ${${.TARGET}_INFS_SRCS:U${INFS_SRCS:U${.TARGET:R}${IPF_SUFF}}}
#_INFS_SRCS_ = ${.TARGET:R}${IPF_SUFF}
# Copy the files in _INFS_SRCS_ and the *.ipf file to the current directory
# if we are in a sandbox, to build it here.
${_INFS_} : $${_INFS_SRCS_}
	${.TARGET:H:C}
	-${_INFS_SRCS_:b${IN_SANDBOX:b${${MAKETOP}../../src${MAKEDIR}:L:C-}}} 
	-${_INFS_SRCS_:b${IN_SANDBOX:b${CHDIR} \
	 ${${MAKETOP}../../src${MAKEDIR}:L:S;/;\\;g} ${CMDSEP} \
	 mklinks -quiet -auto -norefresh ${.ALLSRC:T}}}
	-${_INFS_SRCS_:b${${_INFS_SRCS_:@.F.@${.F.:! \
	 ${CHDIR} ${${MAKETOP}../../src${MAKEDIR}:L:S;/;\\;g} ${CMDSEP} \
	 ${CP} ${.F.:S;/;\\;g} ${CURDIR:S;/;\\;g}!}@}:L:a${NULL_DEVICE}}
	${_IPFC_} ${_INFSFLAGS_} ${.TARGET:R}${IPF_SUFF}
.endif # _INFS_

.endif # defined(__HELP_MK_)
