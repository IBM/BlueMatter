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
# $Log: osf.depend.mk,v $
# Revision 1.2.2.2  1992/12/03  17:25:57  damon
# 	ODE 2.2 CR 183. Added CMU notice
# 	[1992/12/03  17:10:58  damon]
#
# Revision 1.2  1991/12/05  21:17:39  devrcs
# 	Incorporated changes from osc
# 	[91/04/25  13:28:44  mckeen]
# 
# 	Initial version for osc
# 	[91/04/21  20:15:06  damon]
# 
# $EndLog$

.if !defined(_OSF_DEPEND_MK_)
_OSF_DEPEND_MK_=

nodep_all:
	${RM} ${_RMFLAGS_} depend.mk

.if !empty(.TARGETS:Mnodep_*.o)
${TARGETS:Mnodep_*.o}:
	${RM} -f ${.TARGET:S/^nodep_//} ${.TARGET:S/^nodep_//:.o=.d}
	echo "${.TARGET:S/^nodep_//}: ${.TARGET:S/^nodep_//}" \
		> ${.TARGET:S/^nodep_//:.o=.d}
.endif

.EXIT:
	${MD} ${_MDFLAGS_} .

.endif
