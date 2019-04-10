################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This makefile defines the rules to interface with CMVC
#
#  Some variables used:
#   DEFECT
#       The defect name to work with.
#   _DEFECT_
#       The actual defect name used.  If DEFECT is defined then it is used,
#       else SANDBOX is used
#   ANS_ACCEPT
#       The answer (reason) for this defect
#   _ANS_ACCEPT_
#       The actual answer (reason) used for this defect.  If ANS_ACCEPT is
#       defined then it is used, else 'program_defect' is used.
#   CIFLAGS
#       Additional flags to the "File -checkin" command.
#   COFLAGS
#       Additional flags to the "File -checkout" command.
#
#  Targets defined:
#   accept
#       This target will accept a defect and create 1 track for the given
#       release
#   co_<filename>, ci_<filename>
#       Checkout/checkin <filename>
#   complete
#       Complete all fix records associated with this defect/release
#
################################################################################

################################################################################
# Some basic setttings
_DEFECT_       ?= ${DEFECT:U${SANDBOX}}

################################################################################
# To accept a defect:
#  - first accept the defect
#  - second create a track for this release

_ANS_ACCEPT_ ?= ${ANS_ACCEPT:Uprogram_defect}

accept: .SPECTARG
	Defect -accept ${_DEFECT_} -answer ${_ANS_ACCEPT_}
	Track -create -defect ${_DEFECT_} -target ${CMVC_RELEASE}

################################################################################
# Rules to checkout files

_CHECKOUT_TARGETS_ := ${.TARGETS:Mco_*}

.if !empty(_CHECKOUT_TARGETS_)
${_CHECKOUT_TARGETS_}:  .SPECTARG
	File -checkout \
	  ${.TARGET:S!^co_!!:M*/*:B${MAKESUB:S!^/!!}${.TARGET:S!^co_!!}} \
	  -defect ${_DEFECT_} -relative ${SANDBOXBASE}${DIRSEP}${ODESRCNAME} \
    ${COFLAGS}
.endif # !empty(_CHECKOUT_TARGETS_)

################################################################################
# Rules to checkin files

_CHECKIN_TARGETS_  := ${.TARGETS:Mci_*}

.if !empty(_CHECKIN_TARGETS_)
${_CHECKIN_TARGETS_}:  .SPECTARG
	File -checkin \
	  ${.TARGET:S!^ci_!!:M*/*:B${MAKESUB:S!^/!!}${.TARGET:S!^ci_!!}} \
	  -defect ${_DEFECT_} -relative ${SANDBOXBASE}${DIRSEP}${ODESRCNAME} \
    ${CIFLAGS}
.endif # !empty(_CHECKIN_TARGETS_)

################################################################################
# Rules to complete a defect
.if make(complete)
_FIXREC_   != Report -view fixView -where "defectName in ('${_DEFECT_}')" -raw
_FIXRECPAT_ := ${_DEFECT_}|${CMVC_RELEASE}|

_FIXCOMPS_ := \
${_FIXREC_:M${_FIXRECPAT_}*:S/;//g:S/^${_FIXRECPAT_}/;/f:S/|/ /g:M;*:S/;//g}

.if !empty(_FIXCOMPS_)
${_FIXCOMPS_:@.WORD.@complete_${.WORD.}@}:  .SPECTARG
	Fix -complete -defect ${_DEFECT_} -component ${.TARGET:S!^complete_!!}
.endif # !empty(_FIXCOMPS_)

complete: ${_FIXCOMPS_:@.WORD.@complete_${.WORD.}@} .SPECTARG
.if !empty(_FIXCOMPS_)
	${Defect ${_DEFECT_} completed.:L:a&STDOUT}
.else
	${No fix records for defect ${_DEFECT_}:L:a&STDOUT}
.endif # !empty(_FIXCOMPS_)

.endif # make(complete)

