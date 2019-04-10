
################################################################################
# Cyclops cross-compiled from Linux specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=cyclops_cyclops_1
#

OBJECT_FORMAT=ELF

INSTALL_TOOL ?= rpm
PKG_TOOL ?= rpm

# Booleans
DEPENDENCIES=

CCFAMILY ?= gnu
CCTYPE   ?= cpp

GENDEPFLAGS = -I/usr/include -E/usr/include


 PROG_SUFF=.cyclops

  # Run-time addresses  for application code/data (must begin on 512K boundaries)
  LDFLAGS +=  -Ttext 0x20008000
  # Libraries
  GNU_ROOT ?= /BlueGene/gnu2
  CYCLOPS_ROOT ?= /BlueGene/cyclops2
  CYCLOPS_LIB=$(CYCLOPS_ROOT)/lib
  BIN_PREFIX=$(GNU_ROOT)/bin/cyclops-ibm-elf-
  LIB_PREFIX=$(GNU_ROOT)/lib/gcc-lib/cyclops-ibm-elf/2.95.2
  LDFLAGS += -L$(CYCLOPS_LIB) -L$(LIB_PREFIX)
.if $(CCTYPE)=="cc"
  LDFLAGS += $(CYCLOPS_LIB)/__cstart.o
.else
.if $(CCTYPE)=="cpp"
  LDFLAGS += $(CYCLOPS_LIB)/__cppstart.o
.endif
.endif


  _gnu_cc_CC_     ?= $(BIN_PREFIX)gcc ${DEPENDENCIES:D${GNU_GENDEP_FLAG}}
  _gnu_ansi_CC_   ?= $(BIN_PREFIX)gcc -ansi ${DEPENDENCIES:D${GNU_GENDEP_FLAG}}
  _gnu_cpp_CC_    ?= $(BIN_PREFIX)g++ ${DEPENDENCIES:D${GNU_GENDEP_FLAG}}
  _gnu_cc_LD_     ?= $(BIN_PREFIX)gcc
  _gnu_ansi_LD_   ?= $(BIN_PREFIX)gcc
  _gnu_cpp_LD_    ?= $(BIN_PREFIX)ld
  _gnu_cc_SHLD_     ?= $(BIN_PREFIX)gcc
  _gnu_ansi_SHLD_   ?= $(BIN_PREFIX)gcc
  _gnu_cpp_SHLD_    ?= $(BIN_PREFIX)g++
  _RANLIB_  ?= $(BIN_PREFIX)ranlib
  _AR_  ?= $(BIN_PREFIX)ar


################################################################################
#  C/C++ suffix compilation rules
#
# (Blue Gene special --- edit the assembler source to put in SVC instructions)
#
${CC_SUFF:S|$|${OBJ_SUFF}|g}: .REPLCMDS
        ${_CC_} -S ${_CCFLAGS_} ${.IMPSRC}
        mv -f ${.PREFIX}.s ${.PREFIX}.sin
        /bin/sed 's@bal.r63,__svc@svc \# invoke kernel svc@' <${.PREFIX}.sin >${.PREFIX}.s
        ${_CC_} -c ${_CCFLAGS_} ${.PREFIX}.s
        rm ${.PREFIX}.sin ${.PREFIX}.s
