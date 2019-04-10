################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This is the top-level build rules makefile that includes all the other
# makefiles needed.
#  Included makefiles:
#    nls.mk
#    res.mk
#    prog.mk
#    lib.mk
#    obj.mk
#    java.mk
#    dep.mk
#    bld.install.mk
#
################################################################################
.if !defined(__BLD_RULES_MK_)
__BLD_RULES_MK_=

################################################################################
# Include the rules to handle NLS
#
.include <nls.mk>

################################################################################
# Include the rules to handle resources
#
.include <res.mk>

################################################################################
# Include the rules to handle help targets
#
.include <help.mk>

################################################################################
# Include the rules to generate programs.
#
.if !empty(PROGRAMS) || !empty(NLV_PROGRAMS)
.include <prog.mk>
.endif

################################################################################
# Include the rules to generate libraries
#
.include <lib.mk>

################################################################################
# Include the rules to add in dependencies
#
.if defined(DEPENDENCIES) || defined(JAVA_DEPENDENCIES)
.include <dep.mk>
.endif

################################################################################
# Include the rules to generate objects
#
.include <obj.mk>

################################################################################
# Include the rules for using cmvc
#
.if defined(USE_CMVC_RULES)
.include <cmvc.mk>
.endif

################################################################################
# Include the rules to compile java.
# Java rules are included even during the install action because
# java.mk includes things like .SUFFIXES and _SUFF variables that
# may be needed during installation.
#
.if defined(BUILDJAVA) || defined(BUILDJAVADOCS)
.include <java.mk>
.endif

.if !empty(MACHINE:M*_openvms_*)
.include <bld_install.mk>
.else
.include <bld.install.mk>
.endif

.endif

