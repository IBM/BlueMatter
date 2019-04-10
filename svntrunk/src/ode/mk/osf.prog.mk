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
# $Log: osf.prog.mk,v $
# Revision 1.2.2.2  1992/12/03  17:26:39  damon
# 	ODE 2.2 CR 183. Added CMU notice
# 	[1992/12/03  17:11:21  damon]
#
# Revision 1.2  1991/12/05  21:18:16  devrcs
# 	Added _FREE_ to copyright marker
# 	[91/08/01  08:18:05  mckeen]
# 
# 	Changes for Reno make
# 	[91/03/22  16:36:36  mckeen]
# 
# $EndLog$

.if !defined(_OSF_PROG_MK_)
_OSF_PROG_MK_=

#
#  Build rules
#
.if defined(PROGRAMS)

${PROGRAMS}: $${_OFILES_}
	${_CC_} ${_LDFLAGS_} -o ${.TARGET}.X ${_OFILES_} ${_LIBS_}
	${MV} ${.TARGET}.X ${.TARGET}

.endif

.endif
