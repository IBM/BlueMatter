################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines defaults for Variables used in packaging passes and target
#   definitions for all targets defined in packaging passes.
#
#  Targets defined:
#    gather_all
#    installmvs_all
#    parse_all
#    runpkgtool_all
#    package_all
#    buildandpackage_all
#    genmetadata_all
#    gencmf_all
#
################################################################
.if !defined(_PKG_STD_MK_)
_PKG_STD_MK_=

################################################################
# Set defaults for packaging passes
#
PACKAGEBASE ?= ${SANDBOXBASE}${DIRSEP}${INSTALLATION_DIR}${DIRSEP}${CONTEXT}

PKG_FIX_STRATEGY ?= refresh
PKG_TEMP_DIR     ?= ${PACKAGEBASE}${DIRSEP}tmp
PKG_CONTROL_DIR  ?= ${PACKAGEBASE}${DIRSEP}mdata
PKG_OUTPUT_DIR   ?= ${PACKAGEBASE}${DIRSEP}images
PKG_JAVA_DEFS    ?= -DPKGFAMILY=${PKGFAMILY} -DPKGVERSION=${PKGVERSION}

TOSTAGE          ?= ${PACKAGEBASE}${DIRSEP}shipdata
RPM              ?= rpm
MKINSTALL        ?= mkinstall
PKGMK            ?= pkgmk
SWPACKAGE        ?= swpackage
ISBUILD          ?= isbuild
PFTWWIZ          ?= pftwwiz
BUILDPATCH       ?= buildpatch
PKGVERSION       ?= default

# For compatibilities sake, we'll set PKG_JAVA_FLAGS.
PKG_JAVA_FLAGS   ?= ${JVMFLAGS}
_PKG_JVM_FLAGS_  ?= ${PKG_JAVA_FLAGS} ${PKG_JAVA_DEFS}

# See std.mk for definition of PKG_IS_SUPPORTED

################################################################
#  Gather rules
#
.if defined(SPECIAL_PASSES)
gather_all: GATHER .SPECTARG
.else
gather_all: $${_all_targets_} .SPECTARG;@
.endif

################################################################
#  Parse rules
#
.if defined(SPECIAL_PASSES)
parse_all: PARSE .SPECTARG
.else
parse_all: $${_all_targets_} .SPECTARG;@
.endif

################################################################
#  Package rules
#
.if defined(SPECIAL_PASSES)
runpkgtool_all: RUNPKGTOOL .SPECTARG
.else
runpkgtool_all: $${_all_targets_} .SPECTARG;@
.endif

################################################################
#  InstallMvs rules
#
.if defined(SPECIAL_PASSES)
installmvs_all: INSTALLMVS .SPECTARG
.else
installmvs_all: $${_all_targets_} .SPECTARG;@
.endif

################################################################
#  genmetadata rules
#
.if defined(SPECIAL_PASSES)
genmetadata_all: GENMETADATA .SPECTARG
.else
genmetadata_all: $${_all_targets_} .SPECTARG;@
.endif

################################################################
#  gencmf rules
#
.if defined(SPECIAL_PASSES)
gencmf_all: GENCMF .SPECTARG
.else
gencmf_all: $${_all_targets_} .SPECTARG;@
.endif

################################################################
# PACKAGE rules
#
.if (!empty(MACHINE:Mmvs390_oe_*) && (${PKG_TYPE:l} == "++ptf" || ${PKG_TYPE:l} == "++apar" || ${PKG_TYPE:l} == "++usermod"))
package_all: runpkgtool_all .SPECTARG
.else
package_all: gather_all parse_all runpkgtool_all .SPECTARG
.if (!empty(MACHINE:Mmvs390_oe_*) && ${PKG_CLASS:l} == "ipp")
.ORDER: parse_all gather_all runpkgtool_all
.else
.ORDER: gather_all parse_all runpkgtool_all
.endif
.endif

################################################################
#  BUILDANDPACKAGE rules
#
buildandpackage_all: build_all package_all .SPECTARG
.ORDER: build_all package_all

################################################################
#  Target for printing the ODE Version
#
odeversion: .SPECTARG
	@${JVM} ${JVMFLAGS} com.ibm.ode.lib.io.Version

################################################################################
#  Directory where files are installed
#
.if (!empty(MACHINE:Mhp9000_ux_*) || !empty(MACHINE:M*_solaris_*) || \
     !empty(MACHINE:Mx86_sco_*) || !empty(MACHINE:Mmips_irix_*) || \
     !empty(MACHINE:Malpha_tru64_*) || !empty(MACHINE:Mia64_hpux_*))
IDIR_PREF ?= /opt
.elif ( !empty(MACHINE:M*_linux_*) || !empty(MACHINE:M*_aix*) )
IDIR_PREF ?= /usr
.elif ( !empty(MACHINE:M*_nt_*) )
IDIR_PREF ?= \usr\lpp
.else
IDIR_PREF ?= /usr/lpp
.endif

.endif # defined(_PKG_STD_MK_)

################################################################################
#  CLASSPATH settings for ISMP
#
.if (${PKGFAMILY:l}=="ismp")
# Default list of ISMP jars
PKG_ISMP_CLASSES ?= . \
                    ppk${DIRSEP}win32ppk.jar \
                    ppk${DIRSEP}linuxppk.jar \
                    ppk${DIRSEP}solarisppk.jar \
                    ppk${DIRSEP}hpuxppk.jar \
                    ppk${DIRSEP}aixppk.jar \
                    ppk${DIRSEP}os2ppk.jar \
                    ppk${DIRSEP}cimppk.jar \
                    ppk${DIRSEP}as400ppk.jar \
                    ppk${DIRSEP}webppk.jar \
                    ppk${DIRSEP}linux390ppk.jar \
                    ppk${DIRSEP}linuxppcppk.jar \
                    classes \
                    lib${DIRSEP}ide.jar \
                    lib${DIRSEP}idewizards.jar \
                    lib${DIRSEP}wizard.jar \
                    lib${DIRSEP}product.jar \
                    lib${DIRSEP}help.jar \
                    lib${DIRSEP}ProjectWizard.jar \
                    lib${DIRSEP}platform.jar \
                    lib${DIRSEP}swing.jar \
                    lib${DIRSEP}jhall.jar \
                    lib${DIRSEP}parser.jar \
                    lib${DIRSEP}xt.jar \
                    lib${DIRSEP}icebrowserbean.jar \
                    lib${DIRSEP}icebrowserlitebean.jar

.if defined(USE_PKG_ISMP_REL_CLASSPATH)
_PKG_ISMP_CLASSPATH_ = ${PKG_ISMP_OTHERCLASSES}${PKG_ISMP_CLASSES:@VAR@${PATHSEP}${VAR}@:s/ //g}
.else
_PKG_ISMP_CLASSPATH_ = ${PKG_ISMP_OTHERCLASSES}${PKG_ISMP_CLASSES:@VAR@${PATHSEP}${PKG_ISMP_INSTALLDIR}${DIRSEP}${VAR}@:s/ //g}
.endif
.endif
