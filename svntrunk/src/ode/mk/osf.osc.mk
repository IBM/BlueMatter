#
# @OSF_FREE_COPYRIGHT@
# COPYRIGHT NOTICE
# Copyright (c) 1992, 1991, 1990  
# Open Software Foundation, Inc. 
#  
# Permission is hereby granted to use, copy, modify and freely distribute 
# the software in this file and its documentation for any purpose without 
# fee, provided that the above copyright notice appears in all copies and 
# that both the copyright notice and this permission notice appear in 
# supporting documentation.  Further, provided that the name of Open 
# Software Foundation, Inc. ("OSF") not be used in advertising or 
# publicity pertaining to distribution of the software without prior 
# written permission from OSF.  OSF makes no representations about the 
# suitability of this software for any purpose.  It is provided "as is" 
# without express or implied warranty. 
#
#
# HISTORY
# $Log: osf.osc.mk,v $
# Revision 1.2.2.2  1992/12/03  17:26:25  damon
# 	ODE 2.2 CR 183. Added CMU notice
# 	[1992/12/03  17:11:14  damon]
#
# Revision 1.2  1991/12/05  21:18:05  devrcs
# 	Incorporated changes from osc
# 	[91/04/25  13:27:47  mckeen]
# 
# 	Initial version for osc
# 	[91/04/21  20:15:24  damon]
# 
# $EndLog$

.if !defined(_OSF_OSC_MK_)
_OSF_OSC_MK_=

#
#  Define MAILSYSTEM (used by rcs, etc. to send mail)
#
MAILSYSTEM?=SENDMAIL

#
#  Security flags
#
_SEC_C2_DEFS_=-DSEC_BASE -DSEC_ACL_SWARE
_SEC_B1_DEFS_=-DSEC_BASE -DSEC_ACL_SWARE -DSEC_PRIV -DSEC_MAC_OB

.if !defined(SEC_DEFS) && defined(SEC_LEVEL)
SEC_DEFS=${_SEC_${SEC_LEVEL}_DEFS_}
.endif

.if defined(SEC_DEFS)
SEC_LIBS=-lsecurity
.else
SEC_LIBS=
.endif
PROGRAMS_C2=${PROGRAMS_BASE} ${PROGRAMS_ACL}
PROGRAMS_B1=${PROGRAMS_C2} ${PROGRAMS_PRIV} ${PROGRAMS_MAC}
#
#  Default locale/language information
#
LOCALE?=C
LOCALEPATH=${MAKETOP}usr/lib/nls/loc
MSGLANG?=en_US.88591
MSGLANGPATH=${MAKETOP}usr/lib/nls/msg/${MSGLANG}

#
#  Internationalization flag
#
I18N_FLAG?=NLS

#
#  Internationalization CC flags
#
_NLS_I18N_DEFS_=-DNLS
_KJI_I18N_DEFS_=-DKJI
_I18N_DEFS_=${_${I18N_FLAG}_I18N_DEFS_}

#
#  Flags to get the loader to strip symbols
#
LDSTRIP=-%ld," -x" -%ld," -S"

#
#  Shared libraries definitions
#
.if defined(NO_SHARED_LIBRARIES) || defined(USE_STATIC_LIBRARIES)
USE_SHARED_LIBRARIES=0
.else
USE_SHARED_LIBRARIES=1
.endif

#
#  C compiler variations
#
_GCC_EXEC_PREFIX_=${${OBJECT_FORMAT}_GCC_EXEC_PREFIX}
GLINE?=-gline

ANSI_CC?=${_GCC_EXEC_PREFIX_}gcc -B${_GCC_EXEC_PREFIX_} ${GLINE}
TRADITIONAL_CC?=${ANSI_CC} -traditional
WRITABLE_STRINGS_CC?=${ANSI_CC} -fwritable-strings

ANSI_LD?=${_GCC_EXEC_PREFIX_}ld
TRADITIONAL_LD?=${ANSI_LD}
WRITABLE_STRINGS_LD?=${ANSI_LD}

ANSI_AR?=${_GCC_EXEC_PREFIX_}ar
TRADITIONAL_AR?=${ANSI_AR}
WRITABLE_STRINGS_AR?=${ANSI_AR}

ANSI_RANLIB?=${_GCC_EXEC_PREFIX_}ranlib
TRADITIONAL_RANLIB?=${ANSI_RANLIB}
WRITABLE_STRINGS_RANLIB?=${ANSI_RANLIB}

_CCDEFS_=-DBSD44 -DMSG ${_I18N_DEFS_} -DMACH -DCMU -D${MACHINE} -D__${MACHINE}__
.if ${USE_SHARED_LIBRARIES}
_SHCCDEFS_=-D_SHARED_LIBRARIES
.else
_SHCCDEFS_=
.endif

_PMAX_MACHO_CFLAGS_=-G 0
_MMAX_MACHO_CFLAGS_=-mnosb
_O_F_CFLAGS_=${_${_T_M_}_${OBJECT_FORMAT}_CFLAGS_}

_host_NOSTDINC_=
_ansi_NOSTDINC_=-nostdinc
_traditional_NOSTDINC_=-nostdinc
_writable_strings_NOSTDINC_=-nostdinc
_CC_NOSTDINC_=${_${_CCTYPE_}_NOSTDINC_}

_host_GENINC_=
_ansi_GENINC_=`${GENPATH} -I.` -I-
_traditional_GENINC_=`${GENPATH} -I.` -I-
_writable_strings_GENINC_=`${GENPATH} -I.` -I-
_CC_GENINC_=${_${_CCTYPE_}_GENINC_}

.if ${USE_SHARED_LIBRARIES}
_PICLIB_=-pic-lib ${_GENLIB_} ${_CC_LIBDIRS_} ${_LIBS_}
.else
_PICLIB_=
.endif
_host_PICLIB_=
_ansi_PICLIB_=${_PICLIB_}
_traditional_PICLIB_=${_PICLIB_}
_writable_strings_PICLIB_=${_PICLIB_}
_CC_PICLIB_=${_${_CCTYPE_}_PICLIB_}

.if ${USE_SHARED_LIBRARIES}
_GLUE_=-%ld," -warn_nopic -glue"
.else
_GLUE_=
.endif
_host_GLUE_=
_ansi_GLUE_=${_GLUE_}
_traditional_GLUE_=${_GLUE_}
_writable_strings_GLUE_=${_GLUE_}
_CC_GLUE_=${_${_CCTYPE_}_GLUE_}

_host_GENLIB_=
_ansi_GENLIB_=
_traditional_GENLIB_=
_writable_strings_GENLIB_=
_CC_GENLIB_=${_${_CCTYPE_}_GENLIB_}

#
#  Project rules
#
MIG?=mig
MIGCPP?=${_GCC_EXEC_PREFIX_}cpp

_MIGFLAGS_=\
	-cpp ${MIGCPP}\
	${${.TARGET}_MIGENV:U${MIGENV}}\
	${${.TARGET}_MIGFLAGS:U${MIGFLAGS}}\
	${${.TARGET}_MIGARGS:U${MIGARGS}}\
	${_CC_NOSTDINC_} ${_GENINC_} ${_CC_INCDIRS_}

.if defined(MIG_DEFS)
_MIG_HDRS_=${MIG_DEFS:.defs=.h}
_MIG_USRS_=${MIG_DEFS:.defs=User.c}
_MIG_SRVS_=${MIG_DEFS:.defs=Server.c}

${_MIG_HDRS_}: $${.TARGET:.h=.defs}
	${MIG} ${_MIGFLAGS_} ${${.TARGET:.h=.defs}:P} \
		-server /dev/null -user /dev/null

${_MIG_USRS_}: $${.TARGET:S/User.c$$/.defs/}
	${MIG} ${_MIGFLAGS_} ${${.TARGET:S/User.c$/.defs/}:P} \
		-header /dev/null -server /dev/null

${_MIG_SRVS_}: $${.TARGET:S/Server.c$$/.defs/}
	${MIG} ${_MIGFLAGS_} ${${.TARGET:S/Server.c$/.defs/}:P} \
		-header /dev/null -user /dev/null
.endif

.if defined(MSGHDRS)
${MSGHDRS}: $${.TARGET:_msg.h=.msg}
	${MKCATDEFS} ${.TARGET:_msg.h=} ${.ALLSRC} > ${.TARGET:_msg.h=.cat.in}
	-${GENCAT} ${.TARGET:_msg.h=.cat} ${.TARGET:_msg.h=.cat.in}
	-${RM} ${_RMFLAGS_} ${.TARGET:_msg.h=.cat.in}
	-${RM} ${_RMFLAGS_} ${MSGLANGPATH}/${.TARGET:_msg.h=.cat}
	${MAKEPATH} ${MSGLANGPATH}/${.TARGET:_msg.h=.cat}
	${CP} ${.TARGET:_msg.h=.cat} ${MSGLANGPATH}/${.TARGET:_msg.h=.cat}
.endif

.if defined(CATFILES)
${CATFILES}: $${.TARGET:.cat=.msg}
	${MKCATDEFS} ${.TARGET:.cat=} ${.ALLSRC} > ${.TARGET}.in
	-${GENCAT} ${.TARGET} ${.TARGET}.in
	-${RM} ${_RMFLAGS_} ${.TARGET}.in
	-${RM} ${_RMFLAGS_} ${MSGLANGPATH}/${.TARGET}
	${MAKEPATH} ${MSGLANGPATH}/${.TARGET}
	${CP} ${.TARGET} ${MSGLANGPATH}/${.TARGET}
.endif

.if defined(CTABFILES)
${CTABFILES}: $${.TARGET:=.ctab}
	${CTAB} -i ${.ALLSRC} -o ${.TARGET}.X
	${MV} ${.TARGET}.X ${.TARGET}
.endif

.endif
