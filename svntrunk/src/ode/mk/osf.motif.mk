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
# $Log: osf.motif.mk,v $
# Revision 1.2.2.2  1992/12/03  17:26:11  damon
# 	ODE 2.2 CR 183. Added CMU notice
# 	[1992/12/03  17:11:05  damon]
#
# Revision 1.2  1991/12/05  21:17:51  devrcs
# 	Changes for Reno make
# 	[91/04/24  14:48:22  mckeen]
# 
# $EndLog$

.if !defined(_OSF_ODE_MK_)
_OSF_ODE_MK_=

#
#  Define MAILSYSTEM (used by rcs, etc. to send mail)
#
MAILSYSTEM?=SENDMAIL

#
#  Shared libraries definitions
#
USE_SHARED_LIBRARIES=0

.endif
