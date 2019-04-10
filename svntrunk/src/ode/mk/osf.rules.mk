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
# $Log: osf.rules.mk,v $
# Revision 1.2.6.1  1993/08/06  19:21:47  damon
# 	CR 616. Adding IDL rules
# 	[1993/08/06  19:21:33  damon]
#
# Revision 1.2.4.2  1992/12/03  17:26:44  damon
# 	ODE 2.2 CR 183. Added CMU notice
# 	[1992/12/03  17:11:22  damon]
# 
# Revision 1.2  1991/12/05  21:18:20  devrcs
# 	Added id !defined(TOTAGE) to optomize install pass
# 	[91/09/19  15:21:27  mckeen]
# 
# 	New Common Makefile for Reno Make
# 	[91/04/09  08:49:26  mckeen]
# 
# $EndLog$

.if !defined(_OSF_RULES_MK_)
_OSF_RULES_MK_=

.include <osf.std.mk>
.if !defined(TOSTAGE)
.if defined(PROGRAMS)
.include <osf.prog.mk>
.endif
.if defined(LIBRARIES) || defined(SHARED_LIBRARIES)
.include <osf.lib.mk>
.endif
.if defined(BINARIES)
.include <osf.obj.mk>
.endif
.if defined(SCRIPTS)
.include <osf.script.mk>
.endif
.if defined(IDLFILES)
.include <osf.idl.mk>
.endif
.if defined(MANPAGES)
.include <osf.man.mk>
.endif
.if defined(DOCUMENTS)
.include <osf.doc.mk>
.endif
.if defined(DEPENDENCIES)
.include <osf.depend.mk>
.endif
.endif

.endif
