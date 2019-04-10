################################################################################
# Version: %RELEASE_NAME% (Build %LEVEL_NAME%, %BUILD_DATE%)
#
# This file defines targets to run packaging tools.
#  Targets defined:
#    RUNPKGTOOL_TARGETS
#
################################################################################
.if !defined(_PKG_RUNPKGTOOL_MK_)
_PKG_RUNPKGTOOL_MK_=

.if defined(RUNPKGTOOL)

ZIP       ?= jar
ZIPFLAGS  ?= -cvMf
ZIP_SUFF  ?= .zip

.if (defined(APAR))
  PKG_TYPE ?= APAR
  PKG_CLASS ?= SP
.endif
.if (defined(USERMOD))
  PKG_TYPE = USERMOD
  PKG_CLASS = SP
.endif

.if defined(APAR)&&defined(USERMOD)
  @${ECHO} ERROR: Can not define both APAR and USERMOD.
.elif (!empty(MACHINE:Mmvs390_oe_*)&& defined(PKG_CLASS))
.if (${PKG_CLASS:l}=="ipp")
_PKGMVSFLAGS_ ?= \
	-DPKG_CONTROL_DIR=${PKG_CONTROL_DIR} \
	-DPKG_OUTPUT_DIR=${PKG_OUTPUT_DIR} \
	-DPKG_TEMP_DIR=${PKG_TEMP_DIR} \
	-DTOSTAGE=${TOSTAGE} \
	-DMVS_JOBCARD=${MVS_JOBCARD} \
	-DMVS_USERID=${MVS_USERID} \
	-DMVS_PASSWORD=${MVS_PASSWORD} \
	-DMVS_EXEC_DATASET=${MVS_EXEC_DATASET} \
	-DMVS_TERSE_LOAD_DATASET=${MVS_TERSE_LOAD_DATASET} \
	-DMVS_TERSE_CLIST_DATASET=${MVS_TERSE_CLIST_DATASET} \
	-DMVS_RTG_LOADLIB=${MVS_RTG_LOADLIB} \
	-DMVS_FUNCCNTL_DATASET=${MVS_FUNCCNTL_DATASET} \
	-DMVS_JOBMONITOR_TIME=${MVS_JOBMONITOR_TIME} \
	-DMVS_RTG_ALLOC_INFO="${MVS_RTG_ALLOC_INFO}" \
	-DMVS_VSAM_VOLUME_INFO="${MVS_VSAM_VOLUME_INFO}" \
	-DPKG_CLASS=${PKG_CLASS} \
	-DMVS_DELETE_OUTPUT=${MVS_DELETE_OUTPUT} \
	-DMVS_DISPLAY_OUTPUT=${MVS_DISPLAY_OUTPUT} \
	-DMVS_SAVE_OUTPUT_FILE=${MVS_SAVE_OUTPUT_FILE} \
	-DPKG_EVENT="package"
.elif ${PKG_CLASS:l}=="sp"
     _SVCMVSFLAGS_ ?= \
      -DPKG_EVENT="package" \
      -DPKG_TYPE=${PKG_TYPE} \
      -DPKG_CLASS=${PKG_CLASS} \
      -DPKG_CONTROL_DIR=${PKG_CONTROL_DIR} \
      -DMVS_DISPLAY_OUTPUT=${MVS_DISPLAY_OUTPUT} \
      -DB390_WORKPATH1=${B390_WORKPATH1}
     .if ${PKG_TYPE:l}!="ptfvpl"
       _SVCMVSFLAGS_ += \
        -DB390_PATH=${B390_PATH} \
        -DB390_PARM_FILE=${B390_PARM_FILE} \
        -DB390_TRACE_ONLY=${B390_TRACE_ONLY} \
        -DB390_DEBUG_ON=${B390_DEBUG_ON}
     .endif
     .if ${PKG_TYPE:l}=="apar" || ${PKG_TYPE:l}=="++apar" || \
         ${PKG_TYPE:l}=="retrieveapar"
       _SVCMVSFLAGS_ += \
        -DAPAR=${APAR}
     .endif
     .if ${PKG_TYPE:l}=="usermod" || ${PKG_TYPE:l}=="++usermod"
       _SVCMVSFLAGS_ += \
        -DUSERMOD=${USERMOD}
     .endif
     .if ${PKG_TYPE:l}=="apar" || ${PKG_TYPE:l}=="usermod"
       _SVCMVSFLAGS_ += \
        -DB390_DRV_BLKP=${B390_DRV_BLKP} \
        -DB390_DRV_BLKS=${B390_DRV_BLKS} \
        -DB390_DRV_STGCLS=${B390_DRV_STGCLS} \
        -DB390_DRV_MGTCLS=${B390_DRV_MGTCLS} \
        -DB390_DRV_UBKP=${B390_DRV_UBKP} \
        -DB390_DRV_MAXCYL=${B390_DRV_MAXCYL} \
        -DB390_DRV_MAXEXT=${B390_DRV_MAXEXT} \
        -DB390_DRV_VOLID=${B390_DRV_VOLID} \
        -DB390_AUTO_DELETE_DRIVER=${B390_AUTO_DELETE_DRIVER} \
        -DB390_RUNSCAN=${B390_RUNSCAN} \
        -DB390_SHIPTYPE=${B390_SHIPTYPE}
     .endif
     .if ${PKG_TYPE:l}=="apar" || ${PKG_TYPE:l}=="++apar" || \
         ${PKG_TYPE:l}=="usermod" || ${PKG_TYPE:l}=="++usermod" || \
         ${PKG_TYPE:l}=="retrieveapar" || ${PKG_TYPE:l}=="retrieveptf"
       _SVCMVSFLAGS_ += \
        -DB390_SHIPTO=${B390_SHIPTO}
     .endif
     .if ${PKG_TYPE:l}=="++apar" || ${PKG_TYPE:l}=="++usermod"
       _SVCMVSFLAGS_ += \
        -DB390_RETRIEVE_APAR=${B390_RETRIEVE_APAR} 
     .endif
     .if ${PKG_TYPE:l}=="++apar" 
       _SVCMVSFLAGS_ += \
        -DB390_APAR_REBUILD=${B390_APAR_REBUILD}
     .endif
     .if ${PKG_TYPE:l}=="++ptf"
       _SVCMVSFLAGS_ += \
        -DPTF_SHIPCODE=${PTF_SHIPCODE} \
        -DB390_RETRIEVE_PTF=${B390_RETRIEVE_PTF}
     .endif
     .if ${PKG_TYPE:l}=="retrieveapar" || ${PKG_TYPE:l}=="retrieveptf" 
       _SVCMVSFLAGS_ += \
        -DB390_FUNCTION=${B390_FUNCTION}
     .endif
     .if ${PKG_TYPE:l}=="retrieveapar" || ${PKG_TYPE:l}=="retrieveptf" || \
         ${PKG_TYPE:l}=="++apar" || ${PKG_TYPE:l}=="++usermod" || \
         ${PKG_TYPE:l}=="++ptf"
       _SVCMVSFLAGS_ += \
        -DB390_COPYTO=${B390_COPYTO}
     .endif
     .if ${PKG_TYPE:l}=="retrieveapar"
       _SVCMVSFLAGS_ += \
        -DB390_++APAR_NAME=${B390_++APAR_NAME} 
     .endif
     .if ${PKG_TYPE:l}=="ptfvpl" || ${PKG_TYPE:l}=="ptf" || \
         ${PKG_TYPE:l}=="++ptf" || ${PKG_TYPE:l}=="retrieveptf"
       _SVCMVSFLAGS_ += \
        -Djava.security.policy=${JAVA_SECURITY_POLICY_FILE} \
        -DPKG_API_URL=${PKG_API_URL} \
        -DCMVC_LEVEL=${CMVC_LEVEL} \
        -DCMVC_RELEASE=${CMVC_RELEASE} \
        -DCMVC_FAMILY=${CMVC_FAMILY}
     .endif 
     .if ${PKG_TYPE:l}=="ptfvpl" || ${PKG_TYPE:l}=="retrieveapar" || \
         ${PKG_TYPE:l}=="retrieveptf" || ${PKG_TYPE:l}=="++apar" || \
         ${PKG_TYPE:l}=="++usermod" || ${PKG_TYPE:l}=="++ptf"
       _SVCMVSFLAGS_ += \
        -DMVS_JOBCARD=${MVS_JOBCARD} \
        -DMVS_USERID=${MVS_USERID} \
        -DMVS_PASSWORD=${MVS_PASSWORD} \
        -DMVS_EXEC_DATASET=${MVS_EXEC_DATASET}
     .endif
     .if ${PKG_TYPE:l}=="ptfvpl"
       _SVCMVSFLAGS_ += \
        -DTOSTAGE=${TOSTAGE} \
        -DMVS_TERSE_LOAD_DATASET=${MVS_TERSE_LOAD_DATASET} \
        -DMVS_TERSE_CLIST_DATASET=${MVS_TERSE_CLIST_DATASET} \
        -DMVS_JOBMONITOR_TIME=${MVS_JOBMONITOR_TIME} \
        -DMVS_DELETE_OUTPUT=${MVS_DELETE_OUTPUT} \
        -DMVS_SAVE_OUTPUT_FILE=${MVS_SAVE_OUTPUT_FILE}
     .endif
.elif (${PKG_CLASS:l}=="st")
    _INITSVCMVSFLAGS_ ?= \
      -DPKG_CONTROL_DIR=${PKG_CONTROL_DIR} \
      -DPKG_CLASS=${PKG_CLASS} \
      -DB390_PARM_FILE=${B390_PARM_FILE} \
      -DB390_PATH=${B390_PATH} \
      -DB390_TRACE_ONLY=${B390_TRACE_ONLY} \
      -DB390_DEBUG_ON=${B390_DEBUG_ON} \
      -DB390_INIT_PDT=${B390_INIT_PDT} \
      -DB390_PDT=${B390_PDT} \
      -DB390_PDT_STGCLS=${B390_PDT_STGCLS} \
      -DB390_PDT_MGTCLS=${B390_PDT_MGTCLS} \
      -DB390_PDT_UBKP=${B390_PDT_UBKP} \
      -DB390_REL_BLKP=${B390_REL_BLKP} \
      -DB390_REL_BLKS=${B390_REL_BLKS} \
      -DB390_REL_STGCLS=${B390_REL_STGCLS} \
      -DB390_REL_MGTCLS=${B390_REL_MGTCLS} \
      -DB390_REL_UBKP=${B390_REL_UBKP} \
      -DB390_REL_MAXCYL=${B390_REL_MAXCYL} \
      -DB390_REL_MAXEXT=${B390_REL_MAXEXT} \
      -DB390_REL_STEPS=${B390_REL_STEPS} \
      -DB390_REL_COLLECTORS=${B390_REL_COLLECTORS} \
      -DB390_REL_VOLID=${B390_REL_VOLID} \
      -DB390_DRV_BLKP=${B390_DRV_BLKP} \
      -DB390_DRV_BLKS=${B390_DRV_BLKS} \
      -DB390_DRV_STGCLS=${B390_DRV_STGCLS} \
      -DB390_DRV_MGTCLS=${B390_DRV_MGTCLS} \
      -DB390_DRV_UBKP=${B390_DRV_UBKP} \
      -DB390_DRV_MAXCYL=${B390_DRV_MAXCYL} \
      -DB390_DRV_MAXEXT=${B390_DRV_MAXEXT} \
      -DB390_DRV_VOLID=${B390_DRV_VOLID} \
      -DCMVC_FAMILY=${CMVC_FAMILY} \
      -DCMVC_FAMILY_ADDR=${CMVC_FAMILY_ADDR} \
      -DMVS_DISPLAY_OUTPUT=${MVS_DISPLAY_OUTPUT} \
      -DB390_WORKPATH1=${B390_WORKPATH1} \
      -DPKG_EVENT="package"
.endif

.if defined(RACFGROUP)
GROUP %= ${RACFGROUP}
.endif

.endif # !empty(MACHINE:Mmvs390_oe_*)

_RUNPKGFLAGS_ ?= ${${.TARGET}_RUNPKGTOOLENV:U${RUNPKGTOOLENV}} \
                 ${${.TARGET}_RUNPKGTOOLFLAGS:U${RUNPKGTOOLFLAGS}} \
                 ${${.TARGET}_RUNPKGTOOLARGS:U${RUNPKGTOOLARGS}}

.if (${PKGFAMILY:M*pkgmk*})
 .if (${PKG_CLASS:l}=="ipp")
  _RUNPKGTOOL_     ?= Spti_pkgmk
  RUNPKGTOOLFLAGS  ?= ${PKG_CONTROL_DIR} ${PKG_OUTPUT_DIR} ${TOSTAGE} ${PKGMK}
  _GENDISTFILETOOL_CMD_ ?= ${CHDIR} ${PKG_OUTPUT_DIR} && \
                      ${ZIP} ${ZIPFLAGS} ${INSTALL_IMAGE_FILE}${ZIP_SUFF} ${INSTALL_IMAGE_NAME}
 .elif ${PKG_CLASS:l}=="sp"
  _RUNPKGTOOL_     ?= Spti_buildpatch

  .if empty(PATCH_FORMAT)
   PATCH_FORMAT = directory
  .endif

  .if empty(BUILD_ONE_PATCH)
   BUILD_ONE_PATCH = no
  .endif

  RUNPKGTOOLFLAGS  ?= ${PKG_CONTROL_DIR} ${TOSTAGE} ${PKG_OUTPUT_DIR} \
                      ${PATCH_NAME} ${PATCH_FORMAT} ${BUILD_ONE_PATCH:l} ${BUILDPATCH} 

  .if !empty(PKGTOOL_CONFIG_FILE)
   ADDITIONAL_FLAGS += -c ${PKGTOOL_CONFIG_FILE}
  .endif
  .if !empty(OBSOLETES_LIST)
   ADDITIONAL_FLAGS += -o ${PATCH_OBSOLETES_LIST}
  .endif
  .if !empty(REQUIRES_LIST)
   ADDITIONAL_FLAGS += -r ${PATCH_REQUIRES_LIST}
  .endif  
  .if !empty(INCOMPATIBLES_LIST)
   ADDITIONAL_FLAGS += -i ${PATCH_INCOMPATIBLES_LIST}
  .endif
   RUNPKGTOOLFLAGS += "${ADDITIONAL_FLAGS}"

  _GENDISTFILETOOL_CMD_ ?= ${CHDIR} ${PKG_OUTPUT_DIR} && \
                      ${ZIP} ${ZIPFLAGS} ${INSTALL_IMAGE_FILE}${ZIP_SUFF} ${INSTALL_IMAGE_NAME}
 .endif
.endif

.if (${PKGFAMILY:M*rpm*}) 
_RUNPKGTOOL_     ?= Spti_rpm
RUNPKGTOOLFLAGS  ?= ${PKG_CONTROL_DIR} ${PKG_OUTPUT_DIR} ${TOSTAGE} ${RPM} 
_GENDISTFILETOOL_CMD_ ?= ${CP} ${PKG_OUTPUT_DIR}${DIRSEP}${PKG_ARCH}${DIRSEP}${INSTALL_IMAGE_NAME} \
                         ${INSTALL_IMAGE_FILE}


.elif (${PKGFAMILY:M*mkinstall*})
_RUNPKGTOOL_     ?= Spti_mkinstall
RUNPKGTOOLFLAGS  ?= ${PKG_CONTROL_DIR} ${PKG_OUTPUT_DIR}  ${PKG_CLASS:l} ${MKINSTALL}
_GENDISTFILETOOL_CMD_ ?= ${CP} ${PKG_OUTPUT_DIR}${DIRSEP}${INSTALL_IMAGE_NAME} \
                         ${INSTALL_IMAGE_FILE}

.elif (${PKGFAMILY:M*swpackage*}) 
_RUNPKGTOOL_     ?= Spti_swpackage
.ifdef PKG_OUTPUT_FILE
RUNPKGTOOLFLAGS  ?= ${PKG_CONTROL_DIR} ${PKG_OUTPUT_DIR}/${PKG_OUTPUT_FILE} ${SWPACKAGE}
.if (${MACHINE}==hp9000_ux_11) || !empty(MACHINE:Mia64_hpux_*)
PKG_TOOL_FLAGS += "-x media_type=tape"
.else
PKG_TOOL_FLAGS += "-x target_type=tape"
.endif
.else
RUNPKGTOOLFLAGS  ?= ${PKG_CONTROL_DIR} ${PKG_OUTPUT_DIR} ${SWPACKAGE}
.endif
_GENDISTFILETOOL_CMD_ ?= ${CHDIR} ${PKG_OUTPUT_DIR} && \
                      ${ZIP} ${ZIPFLAGS} ${INSTALL_IMAGE_FILE}${ZIP_SUFF} ${INSTALL_IMAGE_NAME}

.elif (${PKGFAMILY:M*ispe*}) 
.if make(runpkgtool_all) || make(package_all)
TOSTAGE %= ${TOSTAGE}
.endif
_RUNPKGTOOL_     ?= Spti_isbuild
RUNPKGTOOLFLAGS  ?= ${PKG_CONTROL_DIR:S/\//\\/g} \
                    ${PKG_OUTPUT_DIR:S/\//\\/g} \
                    ${PKG_ISPE_MEDIA} ${ISBUILD}
_GENDISTFILETOOL_CMD_ ?= Spti_pftwwiz ${PFTWWIZ} ${PKG_ISPE_PFW_FILE} && \
             ${CP} ${PKG_OUTPUT_DIR:S/\//\\/g}${DIRSEP}${INSTALL_IMAGE_NAME} \
                         ${INSTALL_IMAGE_FILE}

.elif (${PKGFAMILY:M*ismp*}) 
_RUNPKGTOOL_     ?= ${JVM}
RUNPKGTOOLFLAGS  ?= -classpath ${_PKG_ISMP_CLASSPATH_} \
                    com.installshield.isje.ISJE \
                    ${PKG_ISMP_PROJFILE} -build
_GENDISTFILETOOL_CMD_ ?= @${ECHO} PKG_GEN_DIST_FILE: No distribution file needed for ISMP
.endif

_RUNPKGTOOL_CMD_ ?= ${_RUNPKGTOOL_} ${_RUNPKGFLAGS_} ${PKG_TOOL_FLAGS}
RUNPKGTOOL_TARGETS: .SPECTARG
.if (${PKG_CLASS:l}=="ipp" && defined(PKG_IS_SUPPORTED)) || \
    (${PKG_CLASS:l}=="sp" && defined(PKG_IS_SP_SUPPORTED)) || \
    ${PKG_CLASS:l}=="st"
	${PKG_TEMP_DIR:C}
	${PKG_OUTPUT_DIR:C}

 .if !empty(MACHINE:Mmvs390_oe_*)
  .if (${PKG_CLASS:l}=="ipp")
	${JVM} ${_PKG_JVM_FLAGS_} ${_PKGMVSFLAGS_} \
	  com.ibm.ode.pkg.pkgMvs.RunMvsPkg
  .elif (${PKG_CLASS:l}=="sp")
	${JVM} ${_PKG_JVM_FLAGS_} ${_SVCMVSFLAGS_} \
          com.ibm.ode.pkg.pkgMvs.service.RunMvsSvc
  .elif (${PKG_CLASS:l}=="st")
	${JVM} ${_PKG_JVM_FLAGS_} ${_INITSVCMVSFLAGS_} \
          com.ibm.ode.pkg.pkgMvs.service.RunMvsInitSvc
  .endif
 .else
  .if (${PKGFAMILY:l}=="ismp")
	${PKG_OUTPUT_DIR:rr}
	.if defined(USE_PKG_ISMP_REL_CLASSPATH)
	 .if defined(UNIX)
	${CHDIR} ${PKG_ISMP_INSTALLDIR} && ${_RUNPKGTOOL_CMD_}
	 .else
	${CHDIR} /D ${PKG_ISMP_INSTALLDIR} && ${_RUNPKGTOOL_CMD_}
	 .endif
	.else
	${_RUNPKGTOOL_CMD_}
	.endif
  .elif (${PKGFAMILY:l}=="pkgmk")
    .if (${PKG_CLASS:l}=="sp" && empty(PATCH_NAME))
	@${ECHO} ERROR: The makefile variable PATCH_NAME needs to be defined for Service Packaging.
    .else
	${_RUNPKGTOOL_CMD_}
    .endif
  .else  # not ismp
	${_RUNPKGTOOL_CMD_}
  .endif

  # creating install images
  .ifdef PKG_GEN_DIST_FILE
   .if ( defined(PKG_CLASS) && (${PKG_CLASS:l}=="sp"))
	@${ECHO} WARNING: Creation of a distribution file is not supported in Service Packaging
   .else
	${_GENDISTFILETOOL_CMD_}
   .endif
  .endif

 .endif # !empty(MACHINE:Mmvs390_oe_*)

.elif ${PKG_CLASS:l}=="ipp" && !defined(PKG_IS_SUPPORTED)
	@${ECHO} ERROR: ODE Packaging are tasks not supported on platform ${MACHINE}
.elif ${PKG_CLASS:l}=="sp" && !defined(PKG_IS_SP_SUPPORTED)
	@${ECHO} ERROR:  ODE Service Packaging tasks are not supported on platform ${MACHINE}
.endif  #if pkg_is_supported

.else   # nothing to package
  RUNPKGTOOL_TARGETS: .SPECTARG
.endif

.endif
