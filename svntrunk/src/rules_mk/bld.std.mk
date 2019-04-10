################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
#  This file defines high level build targets.
#   Defined targets:
#     all
#         Simply depends on build_all.
#     build_all
#         Main special target that causes the BUILD action to occur.
#     comp_all
#         Special target that causes the COMP action to occur.
#   Included makefiles:
#     export.mk
#
################################################################################
.if !defined(_BLD_STD_MK_)
_BLD_STD_MK_=

all: build_all .SPECTARG;@

.if defined(SPECIAL_PASSES)
build_all: BUILD .SPECTARG
comp_all: COMP .SPECTARG
.else
build_all: $${_all_targets_} .SPECTARG;@
comp_all: $${_all_targets_} .SPECTARG;@
.endif

help::
	@${ECHO} Target: all - Executes build_all target.
	@${ECHO} Target: build_all - Executes comp_all and export_all targets.
	@${ECHO} Target: comp_all - Compiles all targets without exporting.

.include <export.mk>

.endif
