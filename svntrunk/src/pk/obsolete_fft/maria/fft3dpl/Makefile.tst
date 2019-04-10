SRCS := tst.cpp 
SRCS += comm3DLayer.cpp
SCRS += fft3D.cpp
SRCS += BladeMpi.cpp
SRCS += fxlogger.cpp


HDRS :=
HDRS += BladeMpi.hpp

CCFLAGS = -DPKFXLOG -I. -I./../fftw/  -g -lm  -lpthreads 
#    -bnoquiet
LIBS = ~mariae/sandboxes/sb48708/src/BlueMatter/p3me/lib/libfftw.a 
EXEC = tst.exe
OBJS := tst.o 
OBJS += fxlogger.o
OBJS += BladeMpi.o

EXEC:$(OBJS) 
	echo "linking $@"
	mpCC_r  $(CCFLAGS) -o $(EXEC)  $(OBJS) $(LIBS)
	echo -n "make finished at "; date

# Define a rule for building .o from .cc files
.cpp.o: 
	echo "compiling $<"
	mpCC_r -c $(CCFLAGS) $(IFLAGS)  $<

default:tst

clean:
	rm -f  core  $(OBJS) $(EXEC) *~

run:
	tst.exe -procs 2 
