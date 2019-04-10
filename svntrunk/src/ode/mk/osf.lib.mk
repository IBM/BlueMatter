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
# $Log: osf.lib.mk,v $
# Revision 1.2.2.4  1992/12/03  17:26:03  damon
# 	ODE 2.2 CR 183. Added CMU notice
# 	[1992/12/03  17:11:01  damon]
#
# Revision 1.2.2.3  1992/09/24  19:03:24  gm
# 	CR282: Made more portable to non-BSD systems.
# 	[1992/09/23  18:23:16  gm]
# 
# Revision 1.2.2.2  1992/07/26  17:53:18  gm
# 	Updated library code to support shared libraries.
# 	[1992/07/15  14:18:20  gm]
# 
# Revision 1.2  1991/12/05  21:17:44  devrcs
# 	Added _FREE_ to copyright marker
# 	[91/08/01  08:18:00  mckeen]
# 
# 	Combined DCE and ODE Common Makefiles
# 	[91/04/09  08:48:06  mckeen]
# 
# 	Changes for Reno make
# 	[91/03/22  16:35:56  mckeen]
# 
# $EndLog$

.if !defined(_OSF_LIB_MK_)
_OSF_LIB_MK_=

.if defined(ORDER_LIBRARIES)
COFF_LORDER=`lorder *.o | tsort`
AIXCOFF_LORDER=`lorder *.o | tsort`
A_OUT_LORDER=${_OFILES_}
MACHO_LORDER=`${LORDER} ${_OFILES_} | ${TSORT}`
.endif

#
#  Build rules
#
.if defined(LIBRARIES)

#${LIBRARIES}: $${_LIBRARY_OFILES_}

${LIBRARIES}: $${.TARGET}($${_OFILES_})
	${_AR_} ${DEF_ARFLAGS} ${.TARGET} ${.OODATE}
.if defined(ORDER_LIBRARIES)
	${RM} -rf tmp
	mkdir tmp
	cd tmp && { \
	    ${_AR_} x ../${.TARGET}; \
	    ${RM} -f __.SYMDEF __________ELELX; \
	    ${_AR_} cr ${.TARGET} ${${OBJECT_FORMAT}_LORDER}; \
	}
	${MV} -f tmp/${.TARGET} .
	${RM} -rf tmp
.endif
.ifndef NO_RANLIB
	${_RANLIB_} ${.TARGET}
.endif
	${RM} -f ${.OODATE}

.endif

.if defined(SHARED_LIBRARIES)

${SHARED_LIBRARIES}: $${_OFILES_}
.if defined(ORDER_LIBRARIES)
	${_LD_} ${_SHLDFLAGS_} -o ${.TARGET}.X ${${OBJECT_FORMAT}_LORDER} ${_LIBS_}
.else
	${_LD_} ${_SHLDFLAGS_} -o ${.TARGET}.X ${_OFILES_} ${_LIBS_}
.endif
	${MV} ${.TARGET}.X ${.TARGET}

.endif

.endif
