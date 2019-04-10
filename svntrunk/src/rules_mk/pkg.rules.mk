################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
#  This file is the top-level packaging rules file
#   Included makefiles:
#     pkg.installmvs.mk
#     pkg.parse.mk
#     pkg.runpkgtool.mk
#     pkg.metadata.mk
#
################################################################################
.if !defined(_PKG_RULES_MK_)
_PKG_RULES_MK_=

.if !empty(MACHINE:Mmvs390_oe_*)
INSTALLMVS=
.include <pkg.installmvs.mk>
.endif

.if !empty(MACHINE:M*_openvms_*)
.include <pkg_parse.mk>
.include <pkg_runpkgtool.mk>
.include <pkg_svc390.mk>
.include <pkg_metadata.mk>
.else
.include <pkg.parse.mk>
.include <pkg.runpkgtool.mk>
.include <pkg.svc390.mk>
.include <pkg.metadata.mk>
.endif

.endif

