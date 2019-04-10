########################################################################
### Makeifile for P3ME on various platforms
###
### usage:
###        gmake                      - default size HUGE and optimize mode
###        gmake DEBUG=on             - compile with DEBUG mode 
###
### Ruhong Zhou, IBM 2001
########################################################################

P3ME=$(HOME)/p3me
OS=aix
FFTW=$(HOME)/usr/$(OS)
#FFTW=/usr/local/lib

incdir = $(P3ME)/include/
srcdir =  $(P3ME)/src/
bindir =  $(P3ME)/bin/$(OS)/
utildir = $(P3ME)/utils/
libdir = $(P3ME)/lib/
builddir = $(P3ME)/bin/

.SUFFIXES : .dvi .tex .f .for .incf .cmn .i.c .cpp .C

AR = ar
ARFLAGS = cr

MACHNAME = $(shell uname)
MACHFOUND = false

########################################################################
# which machines
# currently supported: SGI IRIX, IBM AIX, Sun Solaris, Linux, HP-UX
########################################################################

ifeq ($(MACHNAME),IRIX64)  
   CPU_TYPE = mips-sgi-irix6.5
   FPP = /lib/cpp -P
   CPP = $(CC) -E
   CC  = cc -xansi -D__INLINE_INTRINSICS -G 0 -multigot
   FC  = f77 -static -r10000 -G 0 -multigot
   C++ = /bin/CC -G 0 -multigot
   
   ifdef DEBUG
     COPT = -g
     FOPT = -g
   else
     COPT =  -O3 -n32 -mips4
     FOPT =  -O3 -n32 -mips4
   endif

   LINKER = $(C++) 
   LOPT = $(COPT)

   # machine dependent compile flags
   P3ME_MACH_CFLAGS = -DIRIS4D -DSGI -DSGI32
   P3ME_MACH_FFLAGS = -DIRIS4D -DSGI -DSGI32 -D___machine___="'SGI'" \
                        -D___osversion___="'mips-sgi-irix6.2'"
   # Libraries for use with the PVM message-passing environment
   #PVM_LIBS = -L/lib/ -lgpvm3 -lpvm3 
   #PVM_LIBS = -L/home3/cao/pvm3/lib/SGI32 
   #HOST_XTRA_LIBS =  -lftn -lm -lmalloc -lfpe

   HOST_XTRA_LIBS =  -L/usr/lib32/mips4 -L/usr/lib32 -lfastm -lftn -lm -lfpe -lC -lCsup 
   LIC_LIB=$(LIC_LIB_DIR)/IRIX64
   MACHFOUND=TRUE
endif

ifeq ($(MACHNAME),AIX)  
   CPU_TYPE = rs6000-ibm-aix4.1.4.0   
   FPP = /usr/lib/cpp -P
   CPP = $(CC) -E
   CC  = xlc
   FC  = xlf -qextname -qdpc=e -bmaxdata:0x80000000
   C++ = xlC

   ifdef DEBUG
     COPT = -g
     FOPT = -g
   else
     COPT = -O2 -qmaxmem=10000
     FOPT = -O2 -qmaxmem=10000
   endif

   LINKER = $(CC)
   LOPT = $(COPT)

   P3ME_MACH_CFLAGS = -DRS6K -DIBM                                         
   P3ME_MACH_FFLAGS = -DRS6K -DIBM -D___machine___="'RS6K'" \
                        -D___osversion___="'rs6000-ibm-aix4.1.4.0'"
   # Libraries for use with the PVM message-passing environment
#   PVM_LIBS = -L/lib/ -lgpvm3 -lpvm3

#   PVM_LIBS = -L/home/zhou/pvm3/lib/RS6K -lgpvm3 -lpvm3 -lm
   HOST_XTRA_LIBS =  -lm -lbsd  -lC -L$(FFTW)/lib -lrfftw -lfftw
   LIC_LIB=$(LIC_LIB_DIR)/AIX
   MACHFOUND = true
endif                                                                      

ifeq ($(MACHNAME),SunOS) 
   CPU_TYPE = i86pc-Solaris-2.5.1
   FPP = /usr/ccs/lib/cpp -P
   CC  = /usr/local/pgi/solaris86/bin/pgcc -D__EXTENSIONS__
   CC  = /usr/local/bin/gcc -mpentiumpro
   FC  = /usr/local/bin/epcf90 -fast -r8 -w90
   C++ = /usr/local/bin/g++ -fno-inline-functions
   CPP = $(CC) -E

   ifdef DEBUG
     COPT = -g
     FOPT = -g
   else
     COPT = -O3 -mpentiumpro
     FOPT =  -fast   -w90
   endif

   LINKER = $(CC)
   LOPT = $(COPT)

   # machine dependent compile flags                                         
   P3ME_MACH_CFLAGS = -DSOLARIS -DFLASH -DPENTIUM
   P3ME_MACH_FFLAGS = -DSOLARIS -DFLASH -DPENTIUM -DEPCFORTRAN -D___machine___="'SOLARIS'" \
			-D___osversion___="'i86pc-Solaris-2.5.1'"           
   # Libraries for use with the PVM message-passing environment
   #PVM_LIBS = -L/lib/ -lgpvm3 -lpvm3
   #PVM_LIBS = -L/home3/cao/pvm3/lib/SOLARIS -lgpvm3 -lpvm3 -lm
   HOST_XTRA_LIBS = -L/usr/local/lib -lm -lstdc++  -lrpcsvc -lnsl \
	-L/usr/local/lib/gcc-lib/i386-pc-solaris2.8/2.95.2 -lgcc \
	-L$(FFTW)/lib -lrfftw -lfftw
   LIC_LIB=$(LIC_LIB_DIR)/SunOS-i86pc
   MACHFOUND = true
endif


ifeq ($(MACHNAME),Linux)

  LINUX_COMPILERS = gnu
#  LINUX_COMPILERS = pgi

  ifeq ($(LINUX_COMPILERS),gnu)
    # egcs + g77 on Red Hat 6.1 and 5.2
    CPU_TYPE = i686
    FPP = /lib/cpp -traditional
    CC  = egcs -m486 
    FC  = g77 -m486 -fno-globals -fno-automatic -finit-local-zero -fno-second-underscore
    C++ = g++ -m486
    CPP = /lib/cpp -traditional

    ifdef DEBUG
      COPT = -g
      FOPT = -g
    else
      COPT =  -O3 -fomit-frame-pointer -ffast-math -funroll-loops 
      FOPT =  -O3 -ffast-math -funroll-loops
    endif

    LINKER = egcs -m486
    LOPT = 
    SGBOPT = $(COPT)

    # machine dependent compile flags
    P3ME_MACH_CFLAGS = -DG77 -DLINUX
    P3ME_MACH_FFLAGS = -DG77 -DLINUX -D___machine___="'pc-linux-gnu'" \
                        -D___osversion___="'pc-linux'"
# on RH5.2 you may need to replace -lg2c with -lf2c
    HOST_XTRA_LIBS =  -lm -lg2c
  endif

  ifeq ($(LINUX_COMPILERS),pgi)
    # Portrland Group compilers on linux
    CPU_TYPE = i686
    FPP = /lib/cpp -traditional
    CC  = pgcc
    FC  = pgf77 -r8
    C++ = pgCC
    CPP = /lib/cpp -traditional

    ifdef DEBUG
      COPT = -g
      FOPT = -g
    else
      COPT =  -O3 -Mvect 
      FOPT =  -O3 -Mnorecursive -Mvect
    endif

    LINKER = pgCC
    LOPT = 
    SGBOPT = $(COPT)

    # machine dependent compile flags
    P3ME_MACH_CFLAGS = -DG77 -DLINUX
    P3ME_MACH_FFLAGS = -DG77 -DLINUX -D___machine___="'pc-linux-pgi'"\
                        -D___osversion___="'pc-linux'"
    HOST_XTRA_LIBS =  -lm   -lpgftnrtl -lpgc
  endif
 
  LIC_LIB=$(LIC_LIB_DIR)/Linux-486
  MACHFOUND = true
endif
                                                                        

ifeq ($(MACHNAME),HP-UX)
   CPU_TYPE =  hppa1.0-hp-hpux11.00

   FPP = /usr/ccs/lbin/cpp -P
   CPP = $(CC) -E
   CC  = cc -Aa -D_HPUX_SOURCE
   FC  = fort77 +E1 +E7 +E4 +E6 +U77 +ppu +FPu -K
   C++ = aCC

   ifdef DEBUG
     COPT = -g
     FOPT = -g
   else
     COPT = +O2
     FOPT = +O2
   endif

   LINKER = $(C++)
   LOPT = $(COPT)
   SGBOPT = $(COPT)

   # machine dependent compile flags

   P3ME_MACH_CFLAGS = -DHPUX -DHPUX9 -DHPUX10 -DHPUX11 -DHPPA
   P3ME_MACH_FFLAGS = -DHPUX -DHPUX9 -DHPUX10 -DHPUX11 -DHPPA \
			-D___machine___="'HPPA'" \
			-D___osversion___="'hppa1.0-hp-hpux11.00'" \
			-D___compdate___="'`date`'"

   # Libraries for use with the PVM message-passing environment

   HOST_XTRA_LIBS =  -L/lib/pa1.1 -L/usr/lib/pa1.1 -L/opt/fortran/lib \
        /usr/lib/libdld.sl -lU77 -lcl -lm /usr/lib/libnsl.sl  \
        -lisamstub -lblas
   LIC_LIB=$(LIC_LIB_DIR)/HP-UX
   MACHFOUND = true
endif

##############################################################################
#                          Lists of source files
##############################################################################

INCLUDE = -I$(P3ME)/include/ -I$(HOME)/usr/$(OS)/include

CSRCS = P3MEmain.cpp \
	P3MEassign.cpp \
	P3MEfield.cpp \
	P3MEgreenFunc.cpp \
	P3MEutils.cpp 

OBJS =  $(bindir)/P3MEmain.o \
	$(bindir)/P3MEassign.o \
	$(bindir)/P3MEfield.o \
	$(bindir)/P3MEgreenFunc.o \
	$(bindir)/P3MEutils.o

all: lib

test: $(bindir)/mytest

lib: $(OBJS)
	mv $(libdir)/libp3me.a $(libdir)/libp3me.a.old
	$(AR) $(ARFLAGS) $(libdir)/libp3me.a $^

$(bindir)/%.o: $(srcdir)/%.cpp
	$(CC) $(COPT) $(CFLAGS) $(INCLUDE) $(P3ME_CFLAGS) $(hostflag) \
	-c -o $@ $<

clean:
	rm -f $(bindir)/*.o work.pc work.pcl core *~

###################################################
# dependency
###################################################

$(bindir)/mytest: $(bindir)/mytest.o $(OBJS)
     ifeq ($(MACHFOUND),false)
	echo "Your machine type $(MACHNAME) not supported"
     else
	$(LINKER) $(LOPT) $(bindir)/mytest.o $(OBJS) \
		$(XTRA_LIBS) $(PVM_LIBS) $(HOST_XTRA_LIBS) \
		$(LDFLAGS) -o $(bindir)/mytest

	rm -f *.f *.c *.for
     endif

HEADER = $(incdir)/P3MElib.hpp $(srcdir)/P3MEimpl.hpp $(srcdir)/P3MEmultiDimArray.hpp \
	$(srcdir)/P3MEutils.hpp

$(bindir)/P3MEmain.o: $(HEADER) $(srcdir)/P3MEmain.cpp

$(bindir)/P3MEassign.o: $(HEADER) $(srcdir)/P3MEassign.cpp

$(bindir)/P3MEfield.o: $(HEADER) $(srcdir)/P3MEfield.cpp

$(bindir)/P3MEgreenFunc.o: $(HEADER) $(srcdir)/P3MEgreenFunc.cpp

$(bindir)/P3MEutils.o: $(srcdir)/P3MEutils.hpp $(srcdir)/P3MEutils.cpp

$(bindir)/mytest.o: $(HEADER) $(srcdir)/mytest.cpp

