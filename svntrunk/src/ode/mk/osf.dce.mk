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
# $Log: osf.dce.mk,v $
# Revision 1.2.2.2  1992/12/03  17:25:49  damon
# 	ODE 2.2 CR 183. Added CMU notice
# 	[1992/12/03  17:10:54  damon]
#
# Revision 1.2  1991/12/05  21:17:33  devrcs
# 	Added changes to support individual component compile flags....
# 	[91/04/12  13:29:03  mckeen]
# 
# 	Combined DCE and ODE Common Makefiles
# 	[91/04/09  08:48:00  mckeen]
# 
# 	Changes for Reno make
# 	[91/03/22  16:35:38  mckeen]
# 
# $EndLog$

.if !defined(_OSF_DCE_MK_)
_OSF_DCE_MK_=

USE_SHARED_LIBRARIES=0

_host_GENINC_=
_ansi_GENINC_=${x:L:!${GENPATH} -I.!}
_traditional_GENINC_=${x:L:!${GENPATH} -I.!}
_writable_strings_GENINC_=${x:L:!${GENPATH} -I.!}
_CC_GENINC_=${_${_CCTYPE_}_GENINC_}
OPT_LEVEL=

.if ${MAKESUB}=="time/service/"
RIOS_CFLAGS = -Dunix -D_BSD
VAX_CFLAGS = -Mg
MIPS_LIBS = -lutc -li
RIOS_LIBS = -lm -lbsd -lutc-user
VAX_LIBS = -lutc -li
.endif

.if ${MAKESUB}=="time/control/"
RIOS_CFLAGS = -Dunix -D_BSD 
VAX_CFLAGS = -Mg
MIPS_LIBS = -lutc -li
RIOS_LIBS = -lm -lcurses -lc -lbsd -lutc-user
VAX_LIBS = -lutc -li 
.endif

.if ${MAKESUB}=="time/common/"
MIPS_LIBRARIES = libutc.a
VAX_LIBRARIES = libutc.a
MIPS_ILIST = libutc.a
VAX_ILIST = libutc.a
RIOS_CFLAGS = -Dunix -D_BSD
VAX_CFLAGS = -Mg
.endif

.if ${MAKESUB}=="time/common-user/"
RIOS_INCLUDES = libutc.a
RIOS_ILIST = libutc.a
VAX_CFLAGS = -Mg
RIOS_CFLAGS = -Dunix -D_BSD 
RIOS_LIBS = -lm -lcurses -lbsd 
.endif

.if ${MAKESUB}=="threads/"
MIPS_DATAFILES	= cma_host.h
RIOS_DATAFILES	= cma_host.h
CC_OPT_LEVEL	= -g
VAX_OFILES	= cma_vmprot.o
MIPS_OFILES	= cma_vmprot.o
RIOS_OFILES	= getasyncinfo.o
.endif

.if ${MAKESUB}=="test/time/service/"
MIPS_CFLAGS = -g3
RIOS_CFLAGS = -Dunix -D_BSD
VAX_CFLAGS = -g
MIPS_LIBS = -li
VAX_LIBS = -li
RIOS_LIBS = -lbsd
.endif

.if ${MAKESUB}=="test/time/common/"
MIPS_CFLAGS = -g3
VAX_CFLAGS = -g
RIOS_CFLAGS = -Dunix
RIOS_LIBS= -lutc-user -lm -lcma -lbsd
.endif


.if ${MAKESUB}=="test/rpc/runtime/v2test_lib/"
RIOS_LIBS 	= -lbsd
RIOS_CFLAGS     = -D_NONSTD_TYPES -D_BSD -Dunix -U__STR__ -DUSE_PROTOTYPES
.endif

.if ${MAKESUB}=="test/rpc/runtime/perf/"
RIOS_CFLAGS     = -D_NONSTD_TYPES -D_BSD -Dunix -U__STR__ 
RIOS_LIBS	= -lbsd
.endif

.if ${MAKESUB}=="test/rpc/rtandidl/testsh/"
RIOS_CFLAGS    = -Dunix -D_BSD 
RIOS_YFLAGS    = -l
APOLLO_YFLAGS  = -l
.endif

.if ${MAKESUB}=="test/rpc/rtandidl/moretests/"
RIOS_LDFLAGS            = -bnodelcsect
RIOS_CFLAGS             = -D_BSD    -Dunix  
RIOS_LIBS               = -lbsd
.endif

.if ${MAKESUB}=="test/rpc/idl/"
RIOS_CFLAGS = -Dunix
RIOS_LIBS= -lbsd
.endif

.if ${MAKESUB}=="test/rpc/idl/"
RIOS_CFLAGS = -Dunix
.endif

.if ${MAKESUB}=="security/utils/"
RIOS_CFLAGS = -Dunix
RIOS_OFILES = environment.o
.endif

.if ${MAKESUB}=="security/server/rsdb/"
RIOS_CFLAGS = -Dunix
.endif

.if ${MAKESUB}=="security/server/rs/"
RIOS_CFLAGS = -Dunix 
.endif

.if ${MAKESUB}=="security/server/bin/"
RIOS_CFLAGS = -Dunix 
RIOS_LIBS = -lbsd
.endif

.if ${MAKESUB}=="security/krb5/lib/krb5/"
RIOS_CFLAGS	= -Dunix -D_BSD -DBERKELEY
.endif

.if ${MAKESUB}=="security/krb5/lib/kdb/"
RIOS_CFLAGS	= -Dunix -D_BSD
.endif

.if ${MAKESUB}=="security/krb5/kdc/"
RIOS_CFLAGS	= -Dunix -D_BSD
.endif

.if ${MAKESUB}=="security/krb5/clients/klist/"
RIOS_CFLAGS	= -Dunix -D_BSD
RIOS_LIBS = -lbsd
.endif

.if ${MAKESUB}=="security/krb5/clients/kinit/"
RIOS_CFLAGS	= -Dunix -D_BSD
RIOS_LIBS = -lbsd
.endif

.if ${MAKESUB}=="security/krb5/clients/kdestroy/"
RIOS_CFLAGS	= -Dunix -D_BSD
RIOS_LIBS = -lbsd
.endif

.if ${MAKESUB}=="security/client/rca/"
RIOS_CFLAGS = -Dunix
.endif

.if ${MAKESUB}=="security/client/admin/rgy_edit/"
RIOS_CFLAGS = -Dunix -D_NO_PROTO
RIOS_LIBS = -lbsd
.endif

.if ${MAKESUB}=="security/client/admin/acl_edit/test/"
RIOS_CFLAGS = -Dunix 
RIOS_LIBS = -lbsd
.endif

.if ${MAKESUB}=="security/client/admin/acl_edit/"
RIOS_CFLAGS = -Dunix 
RIOS_LIBS = -lbsd
.endif

.if ${MAKESUB}=="security/client/acl/"
RIOS_CFLAGS = -Dunix 
.endif

.if ${MAKESUB}=="rpc/uuidgen/"
MIPS_LIBS = -li
RIOS_LIBS = -lbsd
APOLLO68K_LIBS = -lgencat
RIOS_CFLAGS      = -D_NONSTD_TYPES -D_BSD -Dunix -U__STR__
.endif

.if ${MAKESUB}=="rpc/runtime/"
#RIOS_CFLAGS= -D_NONSTD_TYPES -D_BSD -Dunix -U__STR__ -DDNSPI_V3 -DDNS_V3API 
RIOS_CFLAGS= -D_NONSTD_TYPES -D_BSD -Dunix -U__STR__ -DAUTH_KRB  -DDNSPI_V3 -DDNS_V3API 
MIPS_CFLAGS=       -DBSD -DAUTH_KRB -YPOSIX -DDNSPI_V3 -DDNS_V3API
RIOS_ipnaf_sys = -DBROADCAST_NEEDS_LOOPBACK 
RIOS_socki_15 = -DBROADCAST_NEEDS_LOOPBACK -D_NO_PROTO -DBSD
RIOS_sock_15 = -DBSD
RIOS_krbreq = -D_NO_PROTO 
.endif

.if ${MAKESUB}=="rpc/rpcd/"
RIOS_LIBS = -lbsd
RIOS_CFLAGS      = -D_NONSTD_TYPES -D_BSD -Dunix -U__STR__
.endif

.if ${MAKESUB}=="rpc/rpccp/"
RIOS_LIBS = -lbsd
RIOS_CFLAGS      = -D_NONSTD_TYPES -D_BSD -Dunix -U__STR__ -D_MBI=void
.endif

.if ${MAKESUB}=="rpc/idl/lib/"
RIOS_CFLAGS = -Dunix
.endif

.if ${MAKESUB}=="rpc/idl/idl_compiler/"
APOLLO68K_PROGRAMS = gencat
gencat_OFILES = APOLLO68K/gencat.o
APOLLO68K_idl_OFILES = APOLLO68K/catalog.o APOLLO68K/nlprintf.o
RIOS_CFLAGS = -Dunix
RIOS_YFLAGS = -l
APOLLO68K_gencat = gencat
_GENCAT_ = ${${TARGET_MACHINE}_gencat:U${GENCAT}}
.endif

.if ${MAKESUB}=="directory/gds/ASN1/lib_cma/"
RIOS_CFLAGS	= -Dunix
.endif

.if ${MAKESUB}=="directory/cds/server/"
RIOS_LIBS	= -lm -lutc-user -lbsd
.endif

.if ${MAKESUB}=="directory/cds/library/dnsmisc_cma/"
RIOS_OFILES = unix_getaddr.o
.endif

.if ${MAKESUB}=="directory/cds/library/dnsrpc/"
RIOS_CFLAGS	= -D_BSD -Dunix
.endif

.if ${MAKESUB}=="directory/cds/control/"
RIOS_LIBS	= -lcurses -lm -lc -lbsd -lutc-user  
.endif

.if ${MAKESUB}=="directory/cds/control/"
RIOS_LIBS	= -lbsd -lm -lutc-user -lcfg -lodm
.endif

.if ${MAKESUB}=="directory/cds/cdsbrowser/"
RIOS_LIBS	= -lMrm -lXm -lXt -lX11  -lm -lutc-user -lbsd
.endif

.if ${MAKESUB}=="directory/cds/cache/cma/"
RIOS_LIBS	= -lc -lbsd -lm -lutc-user
.endif

.if ${MAKESUB}=="directory/cds/cache/"
RIOS_LIBS	= -lbsd -lm -lutc-user -lcfg -lodm
.endif

.if ${MAKESUB}=="directory/cds.NEW/server/"
RIOS_LIBS	= -lm -lutc-user -lbsd
.endif

.if ${MAKESUB}=="directory/cds.NEW/child/"
RIOS_LIBS	= -lbsd -lm -lutc-user -lcfg -lodm
.endif

.endif
