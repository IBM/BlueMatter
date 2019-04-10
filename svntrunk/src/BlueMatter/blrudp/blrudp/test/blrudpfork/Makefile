#####################################################
#
# Make file for simple blrudp test with two forks
# talking to each other.
#
#
# $Id: Makefile 4625 2003-01-31 18:22:00Z blake $
#
# $Log$
# Revision 1.1  2003/01/31 18:21:59  blake
# add interesting parts of blrudp
#
# Revision 1.1  2003/01/23 17:59:32  ralphbel
# empty
#
#
#
#####################################################

VPATH = .:$(COMMON_BOOTSTRAP_DIR):$(COMMON_DIR)/crc

CFILES 	      = main.cpp \


ifndef DEBUG
    DEBUG := 1;
endif

OS := $(shell uname)

ifneq ($(DEBUG),0)
DBG_CXXFLAGS = -D_DEBUG=1 -g 
DBG_CFLAGS = -D_DEBUG=1 -g 
else
DBG_CXXFLAGS = -O2
DBG_CFLAGS = -O2
endif

OBJDIR = linux

ifeq ($(OS),Linux)
OBJDIR = linux
OS_CFLAGS= -DLINUX=1
endif

ifeq ($(OS),AIX)
OBJDIR = aix
OS_CFLAGS= -DAIX=1
endif

ifeq ($(OS),CYGWIN_NT-5.0)
OBJDIR = cygwin
OS_CFLAGS= -DCYGWIN=1 
SUBLIBS=-lwsock32
endif

#
# intermixed asm listing.
#
ASMLIST = -Wa,-ahls=$*.listing 

INCPATH	=   -I../../ -I../../../common -I../../../include
    CC  =       xlC
    CXX =       xlC

    CFLAGS      = -qcpluscmt $(DBG_CFLAGS) $(OS_CFLAGS) $(PROFILE) $(INCPATH)
    CXXFLAGS    = -qcpluscmt $(DBG_CXXFLAGS) $(OS_CFLAGS) $(PROFILE) $(INCPATH)

    LIBS        =       $(SUBLIBS)
    LINK        =       xlC


LFLAGS	=	
LIBS	=	$(SUBLIBS) 
BLRUDPLIB    = ../../$(OBJDIR)/blrudp.a


TAR	=	tar -cf
GZIP	=	gzip -9f

TARGET	=	$(OBJDIR)/blrudpfork

SUBDIRS = ../../

CPPOBJS   = $(notdir  $(CFILES:%.cpp=%.o))
OBJS      = $(addprefix $(OBJDIR)/, $(notdir $(ASFILES:%.S=%.o) $(CPPOBJS:%.c=%.o)) )

VPATH = $(OBJDIR)

# commands to use to compile and assemble modules.
#
mkdep = $(CC) $(CFLAGS) -MG -M 

compile-command.c = $(CC) $(CFLAGS) -c $< -o $@

compile-command.cpp = $(CXX) $(CFLAGS) -c $< -o $@

asm-command = $(CC) $(CFLAGS) $(AFLAGS) -D__ASSEMBLY__ -c $< -o $@


#
# default rules to compile by.
#
$(OBJDIR)/%.o: %.S
	$(asm-command)

$(OBJDIR)/%.o: %.c
	$(compile-command.c)

$(OBJDIR)/%.o: %.cpp
	$(compile-command.cpp)


$(OBJDIR)/%.d: %.S
	$(mkdep) $< >$(@)

$(OBJDIR)/%.d: %.c
	$(mkdep) $< >$(@)

$(OBJDIR)/%.d: %.cpp
	$(mkdep) $< >$(@)


all: subdirs $(OBJDIR)/. $(TARGET)

$(OBJDIR)/.:
	mkdir $(OBJDIR)

$(OBJDDIR)/main.o:      	main.cpp

subdirs:
	@for i in $(SUBDIRS); do $(MAKE) -C $$i $(MDEFINES) ; done

$(TARGET): Makefile $(OBJS) $(BLRUDPLIB)
	$(LINK)  $(LFLAGS)  -o $(TARGET) $(OBJS) $(LIBS) $(BLRUDPLIB) 


clean:
	-rm -f $(OBJECTS) $(OBJMOC) $(SRCMOC) $(UICIMPLS) $(UICDECLS) $(TARGET)
	-rm -f *~ core
	@for i in $(SUBDIRS); do (cd $$i && $(MAKE) clean) ; done




#
# compute the dependency files we need for this.
#
ifeq ($(filter distclean clean,$(MAKECMDGOALS)),)
depfiles := $(addsuffix .d, $(basename $(OBJS))) 
-include $(depfiles) 
endif


