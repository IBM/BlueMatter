################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines the packaging passes and actions.
#  The defined passes are:
#    PARSE
#    RUNPKGTOOL
#    INSTALLMVS
#    GENMETADATA
#    GENCMF
#
################################################################################
.if !defined(_PKG_PASSES_MK_)
_PKG_PASSES_MK_=

.if defined(INSTALL_STANDARD_SUBDIRS)
GATHER_STANDARD_SUBDIRS ?= ${INSTALL_STANDARD_SUBDIRS}
GENMETADATA_STANDARD_SUBDIRS ?= ${INSTALL_STANDARD_SUBDIRS}
.endif

.ifdef USE_SUBDIR_INHERITANCE
CMFPARSE_SUBDIRS    ?= ${SUBDIRS}
RUNPKGTOOL_SUBDIRS  ?= ${SUBDIRS}
INSTALLMVS_SUBDIRS  ?= ${SUBDIRS}
GENMETADATA_SUBDIRS ?= ${SUBDIRS}
GENCMF_SUBDIRS      ?= ${SUBDIRS}
.endif

################################################################################
#  These list the variables used to define subdirectories to recurse into
#
.if defined(SPECIAL_PASSES)
_CMFPARSE_SUBDIRS_   = ${${MAKEACTION}_CMFPARSE_SUBDIRS:U${CMFPARSE_SUBDIRS}}
_RUNPKGTOOL_SUBDIRS_ = ${${MAKEACTION}_RUNPKGTOOL_SUBDIRS:U${RUNPKGTOOL_SUBDIRS}}
_INSTALLMVS_SUBDIRS_ = ${${MAKEACTION}_INSTALLMVS_SUBDIRS:U${INSTALLMVS_SUBDIRS}}
_GENCMF_SUBDIRS_     = ${${MAKEACTION}_GENCMF_SUBDIRS:U${GENCMF_SUBDIRS}}
.else
# Replace the directory slash '/' with a ';' to allow for multiple-level
# subdirectories to be specified and it will still work with modifiers like
# :H and :T which depend on the '/' character.
#
_CMFPARSE_SUBDIRS_   = ${${.ACTION.}_CMFPARSE_SUBDIRS:U${CMFPARSE_SUBDIRS}:S|/|;|g}
_RUNPKGTOOL_SUBDIRS_ = ${${.ACTION.}_RUNPKGTOOL_SUBDIRS:U${RUNPKGTOOL_SUBDIRS}:S|/|;|g}
_INSTALLMVS_SUBDIRS_ = ${${.ACTION.}_INSTALLMVS_SUBDIRS:U${INSTALLMVS_SUBDIRS}:S|/|;|g}
_GENCMF_SUBDIRS_     = ${${.ACTION.}_GENCMF_SUBDIRS:U${GENCMF_SUBDIRS}:S|/|;|g}
.endif

################################################################################
# ALL ACTIONS
#
_ALL_ACTIONS_+=PARSE GATHER RUNPKGTOOL INSTALLMVS GENMETADATA GENCMF

################################################################################
#
#  For each ACTION define the action for recursion, the passes for the
#  action, the targets for the complete action, and the targets for each
#  pass of the action
#

#gather_all is equivalent to running install_all on all machines except mvs
#gather_all is equivalent to running install_all and installmvs_all on mvs
.if defined(MAKEFILE_PASS)
 _GATHER_PASSES_=$(MAKEFILE_PASS)
.else
 .if (!empty(MACHINE:Mmvs390_oe_*) && ${PKG_CLASS:l} == "ipp")
   _GATHER_PASSES_=STANDARD INSTALLMVS
   .ORDER: STANDARD INSTALLMVS
 .else
   _GATHER_PASSES_=STANDARD
 .endif
.endif

_GATHER_ACTION_=gather
_gather_action_=GATHER

_GATHER_STANDARD_TARGETS_=${_INSTALL_STANDARD_TARGETS_}
_GATHER_INSTALLMVS_TARGETS_=INSTALLMVS_TARGETS

.if defined(MAKEFILE_PASS)
_PARSE_PASSES_=${MAKEFILE_PASS}
.else
_PARSE_PASSES_=CMFPARSE
.endif

_PARSE_ACTION_=parse
_parse_action_=PARSE

_PARSE_CMFPARSE_TARGETS_=\
        PARSE_TARGETS

.if defined(MAKEFILE_PASS)
_RUNPKGTOOL_PASSES_=${MAKEFILE_PASS}
.else
_RUNPKGTOOL_PASSES_=RUNPKGTOOL
.endif

_RUNPKGTOOL_ACTION_=runpkgtool
_runpkgtool_action_=RUNPKGTOOL

_RUNPKGTOOL_RUNPKGTOOL_TARGETS_=\
        RUNPKGTOOL_TARGETS

.if defined(MAKEFILE_PASS)
_INSTALLMVS_PASSES_=${MAKEFILE_PASS}
.else
_INSTALLMVS_PASSES_=INSTALLMVS
.endif

_INSTALLMVS_ACTION_=installmvs
_installmvs_action_=INSTALLMVS

_INSTALLMVS_INSTALLMVS_TARGETS_=\
       INSTALLMVS_TARGETS

.if defined(MAKEFILE_PASS)
_GENMETADATA_PASSES_=${MAKEFILE_PASS}
.else
_GENMETADATA_PASSES_=STANDARD
.endif

_GENMETADATA_ACTION_=genmetadata
_genmetadata_action_=GENMETADATA

_GENMETADATA_STANDARD_TARGETS_=\
        GENMETADATA_TARGETS

.if defined(MAKEFILE_PASS)
_GENCMF_PASSES_=${MAKEFILE_PASS}
.else
_GENCMF_PASSES_=GENCMF
.endif

_GENCMF_ACTION_=gencmf
_gencmf_action_=GENCMF

_GENCMF_GENCMF_TARGETS_=\
        GENCMF_TARGETS

.if defined(SPECIAL_PASSES)
PARSE       : .PASSES ${_PARSE_PASSES_}
RUNPKGTOOL  : .PASSES ${_RUNPKGTOOL_PASSES_}
INSTALLMVS  : .PASSES ${_INSTALLMVS_PASSES_}
GENMETADATA : .PASSES ${_GENMETADATA_PASSES_}
GENCMF      : .PASSES ${_GENCMF_PASSES_}
.endif

.endif
