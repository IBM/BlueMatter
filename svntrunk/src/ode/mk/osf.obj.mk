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
# $Log: osf.obj.mk,v $
# Revision 1.2.2.3  1992/12/03  17:26:18  damon
# 	ODE 2.2 CR 183. Added CMU notice
# 	[1992/12/03  17:11:08  damon]
#
# Revision 1.2.2.2  1992/07/26  17:53:23  gm
# 	Updated copyright marker.  Returned _ALL_OFILES_ and _OFILES_ to
# 	original and OSF/1 form.  Updated to use _YACC_ and _LEX_ macros.
# 	[1992/07/15  14:20:43  gm]
# 
# Revision 1.2  1991/12/05  21:17:56  devrcs
# 	Changed to have foo_OFILES override OFILES
# 	[91/06/25  10:05:49  mckeen]
# 
# 	Combined DCE and ODE Common Makefiles
# 	[91/04/09  08:49:21  mckeen]
# 
# 	Changes for Reno make
# 	[91/03/22  16:36:07  mckeen]
# 
# $EndLog$

.if !defined(_OSF_OBJ_MK_)
_OSF_OBJ_MK_=

#
# definitions for compilation
#
_ALL_OFILES_=${OFILES:U${PROGRAMS:@.PROG.@${${.PROG.}_OFILES:U${.PROG.}.o}@}}
_OFILES_=${OFILES:U${${.TARGET}_OFILES:U${.TARGET}.o}}

#
#  Default double suffix compilation rules
#

.c.o:
	${_CC_} -c ${_CCFLAGS_} ${.IMPSRC}

.y.o:
	${_YACC_} ${_YFLAGS_} ${.IMPSRC}
	${_CC_} -c ${_CCFLAGS_} y.tab.c
	-${RM} -f y.tab.c
	${MV} -f y.tab.o ${.TARGET}

.y.c:
	${_YACC_} ${_YFLAGS_} ${.IMPSRC}
	${MV} -f y.tab.c ${.TARGET}
	${RM} -f y.tab.h

.if	!defined(NO_Y_H_RULE)
.y.h:
	${_YACC_} -d ${_YFLAGS_} ${.IMPSRC}
	${MV} -f y.tab.h ${.TARGET}
	${RM} -f y.tab.c
.endif

.l.o:
	${_LEX_} ${_LFLAGS_} ${.IMPSRC}
	${_CC_} -c ${_CCFLAGS_} lex.yy.c
	-${RM} -f lex.yy.c
	${MV} -f lex.yy.o ${.TARGET}

.l.c:
	${_LEX_} ${_LFLAGS_} ${.IMPSRC}
	${MV} -f lex.yy.c ${.TARGET}

.c.pp:
	${_CC_} -E ${_CCFLAGS_} ${.IMPSRC} > ${.TARGET}

.if defined(OFILES) || defined(PROGRAMS)
${_ALL_OFILES_}: ${HFILES}
.endif

.endif
