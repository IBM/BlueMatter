CC = xlc

CXX = xlC

CPPFLAGS = -I../include  -DNO_PK_PLATFORM

VPATH = ../include ../src

testMRG32k3aEngine_OFILES = \
	testMRG32k3aEngine.o \
	MRG32k3aEngine.o \

testNormalDeviate_OFILES = \
	testNormalDeviate.o \
	MRG32k3aEngine.o \

OBJECTS = \
	testMRG32k3aEngine.o \
	testNormalDeviate.o \
	MRG32k3aEngine.o \

.PHONY: all
all: testMRG32k3aEngine testNormalDeviate


testMRG32k3aEngine: $(testMRG32k3aEngine_OFILES)
	$(CXX) -o testMRG32k3aEngine $(testMRG32k3aEngine_OFILES)

testNormalDeviate: $(testNormalDeviate_OFILES)
	$(CXX) -o testNormalDeviate $(testNormalDeviate_OFILES)

testMRG32k3aEngine.o: MRG32k3aEngine.hpp

MRG32k3aEngine.o: MRG32k3aEngine.hpp

testNormalDeviate.o: \
	NormalDeviate.hpp \
	MRG32k3aEngine.hpp \
	RandomNumberGeneratorIf.hpp

.PHONY: clean
clean:
	/usr/bin/rm testMRG32k3aEngine testNormalDeviate $(OBJECTS)
