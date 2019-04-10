SRCS := fft_one_dim.cpp 
#SRCS += comm3DLayer.cpp
SCRS += fft3D.cpp
SRCS += BladeMpi.cpp
SRCS += fxlogger.cpp

HDRS :=
HDRS += fft_one_dim.hpp
#HDRS += comm3DLayer.hpp 
HDRS += fft3D.hpp
HDRS += fftmain.cpp
HDRS += BladeMpi.hpp
HDRS += fxlogger.hpp
HDRS += fft4.hpp
HDRS += fft128.hpp

CCFLAGS = -DPKFXLOG -DUSE_FFTW_ONED -I. -g -lm -lpthreads -bmaxdata:0x80000000 
#    -bnoquiet -qsource -qshowinc -DUSE_FFTW_ONED

LIBS = ../../../../../BlueMatter/p3me/lib/libfftw.a
EXEC = fft.exe
OBJS := fft_one_dim.o 
# OBJS += comm3DLayer.o
OBJS += fft3D.o
OBJS += fftmain.o
OBJS += BladeMpi.o
OBJS += fxlogger.o

EXEC:$(OBJS) 
	echo "linking $@"
	mpCC_r  $(CCFLAGS) -o $(EXEC)  $(OBJS) $(LIBS)
	echo -n "make finished at "; date

# Define a rule for building .o from .cc files
.cpp.o: 
	echo "compiling $<"
	mpCC_r -c $(CCFLAGS) $(IFLAGS)  $<

default:fft

clean:
	rm -fr  core*  $(OBJS) $(EXEC) *~ LOG.* 
