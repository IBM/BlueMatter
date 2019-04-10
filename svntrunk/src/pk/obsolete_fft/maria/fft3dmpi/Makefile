

SRCS := fft_one_dim.cpp 
SRCS += comm3DLayer.cpp
SCRS += fft3D.cpp
SCRS += timer.cpp
SCRS += fftmain.cpp

HDRS :=
HDRS += fft_one_dim.hpp
HDRS += comm3DLayer.hpp 
HDRS += fft3D.hpp 
HDRS += timer.hpp

CCFLAGS = -I. -I./../fftw/  -g -lm -bmaxdata:0x80000000
#    -bnoquiet
LIBS = ~mariae/sandboxes/sb48708/src/BlueMatter/p3me/lib/libfftw.a 
EXEC = fft.exe
OBJS := fft_one_dim.o 
OBJS += comm3DLayer.o
OBJS += fft3D.o
OBJS += fftmain.o
OBJS += timer.o

EXEC:$(OBJS) 
	echo "linking $@"
	mpCC $(CCFLAGS) -o $(EXEC)  $(OBJS) $(LIBS)
	echo -n "make finished at "; date

# Define a rule for building .o from .cc files
.cpp.o: 
	echo "compiling $<"
	mpCC -c $(CCFLAGS) $(IFLAGS)  $<

default:fft

clean:
	rm -f  core  $(OBJS) $(EXEC) *~
