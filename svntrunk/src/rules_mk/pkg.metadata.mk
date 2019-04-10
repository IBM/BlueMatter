################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines variables and targets for generating the packaging metadata
# file and the CMF
#
################################################################################
.if !defined(_PKG_METADATA_MK_)
_PKG_METADATA_MK_=

CMF_PROP_FILE ?= file.properties
FILESTANZA_SCHEMA_URI ?= ${${MAKETOP}rules_mk/metadata.xsd:P}
FILESTANZA_METADATA_URI ?= fileMetadata.xml
FILESTANZA_METADATA_DIR ?= ${MAKEOBJDIR:d}

CMF_VAR_NAMES = CMF_TARGET_DIR CMF_TARGET_FILE CMF_FILE_TYPE CMF_GROUP_ID \
                CMF_USER_ID CMF_PERMISSIONS CMF_PARENT CMF_FLAGS CMF_PART_NUM \
                CMF_MINOR_DEV_NUM CMF_MAJOR_DEV_NUM CMF_FILE_DIRECTIVES \
                CMF_SHIP_TYPE CMF_DIST_LIB \
                CMF_LKED_TO CMF_LKED_PARMS CMF_HFS_COPY_TYPE CMF_LKED_RC CMF_LKED_COND \
                CMF_HFS_ALIAS CMF_PDS_ALIAS CMF_HFS_LKED_NAME CMF_PDS_LKED_NAME \
                CMF_SET_CODE CMF_ENTRY CMF_EXT_ATTR CMF_INCLUDE CMF_ORDER \
                CMF_SYS_LIBS CMF_JCLIN_MODE CMF_JCLIN_LKED_PARMS \
                CMF_LIBRARY_DD CMF_SIDE_DECK_APPEND_DD CMF_SYS_LIBS_LIBRARY_DD CMF_VPL_SECURITY \
                CMF_VPL_PARTQUAL ${CMF_PART_INFO_TAGS:@VAR@CMF_PART_INFO_${VAR}@}

CMF_PART_INFO_TAGS = ALIAS ASSEM BINARY CALLLIBS CSECT DALIAS DISTLIB DISTMOD \
                     DISTNAME DISTSRC HFSSYSLIB LEPARM LMOD LINK MALIAS PARM \
                     PDSSYSLIB PREFIX SHSCRIPT SYMLINK SYMPATH SYSLIB TALIAS \
                     TEXT VERSION

_METADATATOOLFLAGS_ ?= \
        -DCMF_PROP_FILE=${CMF_PROP_FILE} \
        -DFILESTANZA_SCHEMA_URI=${FILESTANZA_SCHEMA_URI} \
        -DFILESTANZA_METADATA_URI=${FILESTANZA_METADATA_URI}

_CMFGENTOOLFLAGS_ ?= \
        -DFILESTANZA_METADATA_DIR=${FILESTANZA_METADATA_DIR} \
        -DFILESTANZA_METADATA_URI=${FILESTANZA_METADATA_URI} \
        -DPKG_CMF_FILE=${PKG_CMF_FILE} \
        -DPKG_CMF_PRODUCT_FILE=${PKG_CMF_PRODUCT_FILE}

################################################################################
# External defaults
################################################################################
CMF_GROUP_ID ?= ${IGROUP:U}
CMF_USER_ID ?= ${IOWNER:U}
CMF_PERMISSIONS ?= ${IMODE:U}

# Filetype defaults
.if (${PKGFAMILY:M*mvs*})
CMF_FILE_TYPE ?= mod
.else
CMF_FILE_TYPE ?= F
.endif

################################################################################
# genmetadata_all target to create properties file and invoke metadata tool
################################################################################

# Add EXTRA_ILIST entries to ILIST
_PKG_ILIST_ = ${ILIST} ${EXTRA_ILIST}

GENMETADATA_TARGETS: .SPECTARG
.if !empty(_PKG_ILIST_)
	@${CMF_PROP_FILE:rm}
	${IDIR:b${CMF_SOURCE_DIR=${IDIR:S/\\/\\\\/g}:L:a${CMF_PROP_FILE}}}
	${_PKG_ILIST_:@VAR@${${VAR}_IDIR:b${${VAR:S/\\/\\\\/g}_CMF_SOURCE_DIR=${${VAR}_IDIR:S/\\/\\\\/g}:L:a${CMF_PROP_FILE}}}@}
	${ILIST:b${ILIST=${ILIST:S/\\/\\\\/g}:L:a${CMF_PROP_FILE}}}
	${EXTRA_ILIST:b${EXTRA_ILIST=${EXTRA_ILIST:S/\\/\\\\/g}:L:a${CMF_PROP_FILE}}}
	${EXCLUDE_ILIST:b${EXCLUDE_ILIST=${EXCLUDE_ILIST:S/\\/\\\\/g}:L:a${CMF_PROP_FILE}}}
	${CMF_VAR_NAMES:@NAME@${${NAME}:b${${NAME}=${${NAME}:S/\\/\\\\/g}:L:a${CMF_PROP_FILE}}}@}
	${_PKG_ILIST_:@TARG@${CMF_VAR_NAMES:@NAME@${${TARG}_${NAME}:b${${TARG:S/\\/\\\\/g}_${NAME}=${${TARG}_${NAME}:S/\\/\\\\/g}:L:a${CMF_PROP_FILE}}}@}@}
	${JVM} ${_METADATATOOLFLAGS_} com.ibm.ode.pkg.cmfgen.FileMetadata
.endif # !empty(_PKG_ILIST_)

################################################################################
# gencmf_all target to invoke CMF generation tool
################################################################################

GENCMF_TARGETS: .SPECTARG
.if defined(GENCMF)
	@${PKG_CMF_FILE:rm}
	${JVM} ${_CMFGENTOOLFLAGS_} com.ibm.ode.pkg.cmfgen.CMFGenerator
.endif

.endif # defined(PKG_METADATA_MK)

