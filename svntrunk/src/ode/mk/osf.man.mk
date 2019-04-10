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
# $Log: osf.man.mk,v $
# Revision 1.2.2.2  1992/12/03  17:26:07  damon
# 	ODE 2.2 CR 183. Added CMU notice
# 	[1992/12/03  17:11:03  damon]
#
# Revision 1.2  1991/12/05  21:17:47  devrcs
# 	Changes for Reno make
# 	[91/03/22  16:36:02  mckeen]
# 
# $EndLog$

.if !defined(_OSF_MAN_MK_)
_OSF_MAN_MK_=

#
# default nroff program to run
#
NROFF?=nroff

#
# default flags to nroff
#
DEF_NROFFFLAGS?=-man -h

#
# all flags for nroff
#
_NROFFFLAGS_=${${.TARGET}_DEF_NROFFFLAGS:U${DEF_NROFFFLAGS}} ${${.TARGET}_NROFFENV:U${NROFFENV}} ${${.TARGET}_NROFFFLAGS:U${NROFFFLAGS}} ${${.TARGET}_NROFFARGS:U${NROFFARGS}}

#
#  Default single suffix compilation rules
#
.SUFFIXES: .0 .${MANSECTION}

#
#  Default double suffix compilation rules
#
.${MANSECTION}.0:
	${NROFF} ${_NROFFFLAGS_} ${.IMPSRC} > ${.TARGET}.X
	${MV} -f ${.TARGET}.X ${.TARGET}

.endif
