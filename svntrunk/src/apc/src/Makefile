
BGLGNU		= /bgl/BlueLight/ppcfloor/blrts-gnu
BGLSYS		= /bgl/BlueLight/ppcfloor/bglsys
BGLSRC	        = ../bglsw

# 
# order here is very important,  we need to get bgl_perfctr headers from our sandbox not from the floor
#

INCLUDES_RTS	 = -I ../include -I$(BGLSRC)/common/include
INCLUDES_RTS	+= -I$(BGLSRC)/devices
INCLUDES_RTS    += -I$(BGLSRC)/counters/bgl_perfctr
INCLUDES_RTS    += -I$(BGLSYS)/include
CC_RTS = $(BGLGNU)/bin/powerpc-bgl-blrts-gnu-gcc
AR_RTS = $(BGLGNU)/bin/powerpc-bgl-blrts-gnu-ar
RANLIB_RTS = $(BGLGNU)/bin/powerpc-bgl-blrts-gnu-ranlib
OBJDUMP_RTS = $(BGLGNU)/bin/powerpc-bgl-blrts-gnu-objdump
CFLAGS_RTS = -O3 -Wall $(INCLUDES_RTS)

INCLUDES_HOST   = -I ../include -I$(BGLSRC)/counters/bgl_perfctr
CC_HOST         = gcc
CFLAGS_HOST     = -Wall -g -O3 $(INCLUDES_HOST)

LIBAPC_OBJ	= apc.rts.o \
		  upc.rts.o \
		  bgl_perfctr.rts.o \
		  bgl_perfctr_counters.rts.o 



#------------------------------------------
# targets
#------------------------------------------

default: install

all:    libapc.rts.a  

install:	all
#	cp apc.h $(BGLUSR)/include/apc.h
#	cp libapc.rts.a $(BGLUSR)/lib
#	cp apc_scan $(BGLUSR)/bin
#	cp README.apc $(BGLUSR)/docs

apc_scan:	apc_scan.c
	$(CC_HOST) $(CFLAGS_HOST) -o apc_scan apc_scan.c

libapc.rts.a:	$(LIBAPC_OBJ)
	$(AR_RTS) crvs $@ $(LIBAPC_OBJ)
	#$(OBJDUMP_RTS) -D $(LIBAPC_OBJ) > $@.objdump

clean:	
	rm -f *~ *.o *.rts.a
	rm -f apc_scan


#------------------------------------------
# rules
#------------------------------------------

apc.rts.o:	apc.c ../include/apc.h 
	$(CC_RTS) $(CFLAGS_RTS) -DAPC_ENABLE -c $< -o $@

upc.rts.o:	upc.c ../include/apc.h
	$(CC_RTS) $(CFLAGS_RTS) -DAPC_ENABLE -c $< -o $@

bgl_perfctr.rts.o: $(BGLSRC)/counters/bgl_perfctr/bgl_perfctr.c
	$(CC_RTS) $(CFLAGS_RTS) -DAPC_ENABLE -c $< -o $@

bgl_perfctr_counters.rts.o: $(BGLSRC)/counters/bgl_perfctr/bgl_perfctr_counters.c
	$(CC_RTS) $(CFLAGS_RTS) -DAPC_ENABLE -c $< -o $@

bgl_perfctr_kek.rts.o: $(BGLSRC)/counters/bgl_perfctr/bgl_perfctr_kek.c
	$(CC_RTS) $(CFLAGS_RTS) -DAPC_ENABLE -c $< -o $@

