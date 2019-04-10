################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This is the top-level rules makefile that includes all the other makefiles
# needed.
#  Included makefiles:
#    std.mk
#    bld.rules.mk
#    pkg.rules.mk
#    tso.rules.mk
#    ${PROJECT_NAME}.rules.mk
#
################################################################################
.if !defined(__RULES_MK_)
__RULES_MK_=

help::
	@${ECHO} ODE Rules: Version ODE5.0 Build 20041004.0550, 04-Oct-2004
	@${ECHO} [Available targets for mk]

################################################################################ 
# Include the standard definitions rules
#
.include <std.mk>

################################################################################ 
# Include the base building rules
#
.if !empty(MACHINE:M*_openvms_*)
.include <bld_rules.mk>
.if ${USE_TSO_RULES}
.include <tso_rules.mk>
.endif
.else
.include <bld.rules.mk>
.if ${USE_TSO_RULES}
.include <tso.rules.mk>
.endif
.endif

################################################################################ 
# Include the base packaging rules
#
.ifndef OMIT_PACKAGING_RULES
.if !empty(MACHINE:M*_openvms_*)
.include <pkg_rules.mk>
.else
.include <pkg.rules.mk>
.endif
.endif

################################################################################ 
# Try to include a project specific rules makefile.  This can be used to
# to extend and change the behaviour of the base set of rules.
#
.if defined(PROJECT_NAME)
.if !empty(MACHINE:M*_openvms_*)
.tryinclude <${PROJECT_NAME}_rules.mk>
.else
.tryinclude <${PROJECT_NAME}.rules.mk>
.endif
.endif

.include <clean.mk>

.endif

