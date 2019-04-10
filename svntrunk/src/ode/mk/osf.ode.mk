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
# $Log: osf.ode.mk,v $
# Revision 1.2.5.1  1995/05/12  18:52:32  robert
# 	allow for/add NO_GLUE option
# 	[1995/05/12  18:42:19  robert]
#
# Revision 1.2.2.6  1992/12/03  17:26:19  damon
# 	ODE 2.2 CR 183. Added CMU notice
# 	[1992/12/03  17:11:11  damon]
# 
# Revision 1.2.2.5  1992/11/12  15:16:43  damon
# 	CR 329. Now using USE_DASH_I_DASH
# 	[1992/11/12  15:16:10  damon]
# 
# Revision 1.2.2.4  1992/09/24  19:03:26  gm
# 	CR282: Made more portable to non-BSD systems.
# 	[1992/09/23  18:23:24  gm]
# 
# Revision 1.2.2.3  1992/09/15  18:24:42  damon
# 	CR 280. Fixed SHARED_LIBRARY_FORMAT problem
# 	[1992/09/15  18:24:03  damon]
# 
# Revision 1.2.2.2  1992/07/26  17:53:28  gm
# 	Added MAKEPATHFLAGS to reduce verbosity of make output in "expected"
# 	situations.  Added code to support OSF/1 shared libraries.
# 	[1992/07/16  21:57:12  gm]
# 
# Revision 1.2  1991/12/05  21:17:59  devrcs
# 	Changes for Reno make
# 	[91/03/22  16:36:12  mckeen]
# 
# $EndLog$

.if !defined(_OSF_ODE_MK_)
_OSF_ODE_MK_=

#
#  Define MAILSYSTEM (used by rcs, etc. to send mail)
#
MAILSYSTEM?=SENDMAIL

#
# Flags to keep ${MAKEPATH} program quiet on success
#
MAKEPATHFLAGS=-quiet

#
# Use -I- feature if available
#
USE_DASH_I_DASH ?= 0
.if ${USE_DASH_I_DASH}
_CC_GENINC_:=${x:L:!${GENPATH} -I.!} -I-
.endif

#
#  Shared libraries definitions
#
USE_SHARED_LIBRARIES?=0

.if (defined(SHARED_LIBRARY_FORMAT) && ${SHARED_LIBRARY_FORMAT} == "MACHO")

.if ${USE_SHARED_LIBRARIES}
_SHCCDEFS_=-D_SHARED_LIBRARIES
.else
_SHCCDEFS_=
.endif

.if ${USE_SHARED_LIBRARIES}
_PICLIB_=-pic-lib ${_GENLIB_} ${_CC_LIBDIRS_} ${_LIBS_}
.else
_PICLIB_=-pic-none
.endif
_CC_PICLIB_=${_PICLIB_}

.if ${USE_SHARED_LIBRARIES}
_GLUE_=-%ld," -warn_nopic -glue"
.else
_GLUE_=-noshrlib
.endif
_CC_GLUE_=${_GLUE_}

.if (defined(NO_GLUE))
_GLUE_=
_CC_GLUE_=
.endif

.endif

.endif
