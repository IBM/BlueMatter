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
# $Log: osf.idl.mk,v $
# Revision 1.1.2.1  1993/08/06  19:21:42  damon
# 	CR 616. Adding IDL rules
# 	[1993/08/06  19:21:29  damon]
#
# $EndLog$

.if defined(IDLFILES)
${IDLFILES:.idl=.h}: \
		$${.TARGET:.h=.idl} \
		$${$${.TARGET:.h=}_ACF:D$${.TARGET:.h=.acf}}
	${IDL} ${_IDLFLAGS_} ${${.TARGET:.h=.idl}:P}\
	-client none -server none

${IDLFILES:.idl=_cstub.c}: \
		$${.TARGET:_cstub.c=.idl} \
		$${.TARGET:_cstub.c=.h} \
		$${$${.TARGET:_cstub.c=}_ACF:D$${.TARGET:_cstub.c=.acf}}
	${IDL} ${_IDLFLAGS_} ${${.TARGET:_cstub.c=.idl}:P} -server none -header /tmp/${.TARGET:_cstub.c=.h}
	-${RM} ${_RMFLAGS_} /tmp/${.TARGET:_cstub.c=.h}

${IDLFILES:.idl=_sstub.c}: \
		$${.TARGET:_sstub.c=.idl} \
		$${.TARGET:_sstub.c=.h} \
		$${$${.TARGET:_sstub.c=}_ACF:D$${.TARGET:_sstub.c=.acf}}
	${IDL} ${_IDLFLAGS_} ${${.TARGET:_sstub.c=.idl}:P} -client none -header /tmp/${.TARGET:_sstub.c=.h}
	-${RM} ${_RMFLAGS_} /tmp/${.TARGET:_sstub.c=.h}

${IDLFILES:.idl=_caux.c}: \
	$${.TARGET:_caux.c=.idl} \
	$${.TARGET:_caux.c=.h} \
	$${$${.TARGET:_caux.c=}_ACF:D$${.TARGET:_caux.c=.acf}}
	${IDL} ${_IDLFLAGS_} ${${.TARGET:_caux.c=.idl}:P} -server none -header /tmp/${.TARGET:_caux.c=.h}
	-${RM} ${_RMFLAGS_} /tmp/${.TARGET:_caux.c=.h}

${IDLFILES:.idl=_saux.c}: \
	$${.TARGET:_saux.c=.idl} \
	$${.TARGET:_saux.c=.h} \
	$${$${.TARGET:_saux.c=}_ACF:D$${.TARGET:_saux.c=.acf}}
	${IDL} ${_IDLFLAGS_} ${${.TARGET:_saux.c=.idl}:P} -client none -header /tmp/${.TARGET:_saux.c=.h}
	-${RM} ${_RMFLAGS_} /tmp/${.TARGET:_saux.c=.h}
_IDLINCFLAGS_=\
	${${.TARGET}_IDLINCFLAGS} \
	${${.ALLSRC:T:M*.idl}_IDLINCARGS:U${IDLINCARGS}}\
	${${.ALLSRC:T:M*.idl}_IDLINCFLAGS:U${IDLINCFLAGS}}\
	${${.ALLSRC:T:M*.idl}_IDLINCENV:U${IDLINCENV}}
_GENIDLINC_=\
	${_IDLINCFLAGS_:!${GENPATH} ${_IDLINCFLAGS_}!}
_IDLFLAGS_=\
	${${TARGET_MACHINE}_IDLFLAGS} \
	${${.TARGET}_IDLFLAGS} \
	${${.ALLSRC:T:M*.idl}_IDLENV:U${IDLENV}}\
	${${.ALLSRC:T:M*.idl}_IDLFLAGS:U${IDLFLAGS}}\
	${${.ALLSRC:T:M*.idl}_IDLARGS:U${IDLARGS}}\
	${_GENIDLINC_} ${_CC_INCDIRS_}
.endif
