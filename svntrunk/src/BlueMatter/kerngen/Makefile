# Makefile to compile and execute a program under bgas simulator.
#

# ---------------------------------- Begin Standard Cyclops Boilerplate ----------------------------

   CYCLOPS_ROOT=/BlueGene/cyclops
###CYCLOPS_ROOT=/BlueGene/cyclops-test

# Rules for building cyclops executables.
#
##### include $(CYCLOPS_ROOT)/Make.cyclops_rules
include ./Make.cyclops_rules

#SILENT=@  # silent compilation and linking
 SILENT=   # noisy compilation and linking

# Compiler flags.
#
###CC_FLAGS:=$(CC_FLAGS) -I$(CYCLOPS_ROOT)/include -I$(CYCLOPS_ROOT)/include/spi -Wall -O2
CC_FLAGS:=$(CC_FLAGS) -I$(CYCLOPS_ROOT)/include -I$(CYCLOPS_ROOT)/include/spi

# Libraries.
#
LIBS=
LIBS:=$(LIBS) $(CYCLOPS_ROOT)/lib/libbluegene.a

# Simulator.
#
RUN=$(SILENT)$(CYCLOPS_ROOT)/bin/bgas -k $(CYCLOPS_ROOT)/bin/kernel.cyclops
RUN:=$(RUN) -q # non-interactive
RUN:=$(RUN) -1 # single chip, no communications

# ----------------------------------- End Standard Cyclops Boilerplate -----------------------------

# Name of application program.
#
PROG=p1

   GFESOURCEROOT=/home/blake/foxfm
   GFEBUILDROOT=/home/blake/GFEBUILDROOT

# Application-specific object files.
#
OBJS=
OBJS:=$(OBJS) $(PROG).o
OBJS:=$(OBJS)  $(GFESOURCEROOT)/pk/platform/src/platform.o
OBJS:=$(OBJS)  $(GFESOURCEROOT)/pk/reactor/src/smp_reactor.o                                  
OBJS:=$(OBJS)  $(GFESOURCEROOT)/pk/fxlogger/src/fxlogger.o                                
OBJS:=$(OBJS)  $(GFESOURCEROOT)/pk/exception/src/exception.o                             
OBJS:=$(OBJS)  $(GFESOURCEROOT)/pk/random/src/random.o        

# Application-specific compiler flags.
#
# BGF NOTE: -D LINUX and -D A98_TYPENAME should come out and go in platform file.
CC_FLAGS:=$(CC_FLAGS) -D PK_SPI -D SPI_HACK   ## !!TEMP --DL
CC_FLAGS:=$(CC_FLAGS) -D PK_GCC -D PK_SMP
###CC_FLAGS:=$(CC_FLAGS) -D PKFXLOG -D PKFXLOG_ALL
CC_FLAGS:=$(CC_FLAGS) -D SMP
CC_FLAGS:=$(CC_FLAGS) -I $(GFEBUILDROOT)/include
CC_FLAGS:=$(CC_FLAGS) -I .

CPP_LIBS:=$(CPP_LIBS) -lstdc++
CPP_LIBS:=$(CPP_LIBS) -lgcc




default:	$(PROG).cyclops
		$(RUN) -S4096 $(PROG).cyclops ../regtest/hiv/hiv.linux.msd ../regtest/hiv/hiv.dvs ../regtest/hiv/hiv.rtp

$(PROG).cyclops:$(OBJS)
		$(LD) $(CYCLOPS_ROOT)/lib/__cppstart.o $(OBJS) $(LIBS) $(CPP_LIBS) $(LIBS) $(LD_FLAGS) -o$(PROG).cyclops -M >$(PROG).map

clean:		defaultclean
	rm -f $(OBJS)
	rm -f $(GFESOURCEROOT)/pk/platform/src/platform.s \
        $(GFESOURCEROOT)/pk/reactor/src/smp_reactor.s \
        $(GFESOURCEROOT)/pk/fxlogger/src/fxlogger.s   \
        $(GFESOURCEROOT)/pk/exception/src/exception.s \
        $(GFESOURCEROOT)/pk/random/src/random.s        
