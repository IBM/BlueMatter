################################################################################
# PowerPC/AIX 4.3+ specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=bluelight_sk_1
#

TARGET_MACHINE = bluelight_sk_1
OBJECT_FORMAT  = ELF

# Booleans
AIX            =
AIX_PPC        =
UNIX           =
DEPENDENCIES   =

#LDFLAGS       += -lbsd
CCFAMILY      ?= cset
CCTYPE        ?= cpp

# Gendep flags - include then exclude the system header files
GENDEPFLAGS = -I/usr/include -E/usr/include

# /afs/torolab/u/mendell/public/blueLight

# BIN_PREFIX = /.../torolab.ibm.com/fs/projects/vabld/run/vacpp/dev/aix/daily/020224/bin
.if $(MACHINE) == "ppc_linux_2"
#BIN_PREFIX = /gsa/watgsa/home/t/j/tjcw/linux_ppc/bin
#BIN_PREFIX = /opt/ibmcmp/vac/6.0/bin
BIN_PREFIX = /opt/ibmcmp/vacpp/6.0/bin
.else
BIN_PREFIX = /.../torolab.ibm.com/fs/projects/vabld/run/vacpp/dev/aix/latest/bin
.endif

HW_INC_TOP=/gsa/watgsa/home/t/j/tjcw/bgl/
BLADE_INC=-I $(HW_INC_TOP)/blade/include
SYSBR_INC_TOP=$(HW_INC_TOP)/sysbringup
SYSBRINGUP_INC=-I $(SYSBR_INC_TOP)/generic/include -I $(SYSBR_INC_TOP)/tree/include

#BL_PREFIX=/gsa/watgsa/home/t/j/tjcw/BlueLight-install/blrts-gnu
#HDR_DIR_1 = $(BL_PREFIX)/include/c++/3.2
#HDR_DIR_2 = $(BL_PREFIX)/include/c++/3.2/powerpc-bgl-blrts-gnu
#HDR_DIR_3 = $(BL_PREFIX)/include/c++/3.2/backward
#HDR_DIR_4 = $(BL_PREFIX)/lib/gcc-lib/powerpc-bgl-blrts-gnu/3.2/include
#HDR_DIR_5 = $(BL_PREFIX)/powerpc-bgl-blrts-gnu/sys-include
#
##HDR_DIR_1 = /usr/include/g++
##HDR_DIR_2 = /usr/include/g++/powerpc-suse-linux
##HDR_DIR_3 = /usr/include/g++/backward
##HDR_DIR_4 = /usr/lib/gcc-lib/powerpc-suse-linux/3.2/include
##HDR_DIR_5 = /usr/include
#
##HDR_DIR_1 = $(BL_PREFIX)/include/c++/3.2
##HDR_DIR_2 = $(BL_PREFIX)/include/c++/3.2/powerpc-bgl-blrts-gnu
##HDR_DIR_3 = $(BL_PREFIX)/include/c++/3.2/backward
##HDR_DIR_4 = $(BL_PREFIX)/lib/gcc-lib/powerpc-bgl-blrts-gnu/3.2/include
##HDR_DIR_5 = $(BL_PREFIX)/powerpc-bgl-blrts-gnu/sys-include
#
#TOR_HDR_PREFIX = /gsa/watgsa/home/t/j/tjcw/vabld/run/vacpp/dev/linux_ppc/latest/vacpp.include

#_cset_cc_CC_      = $(BIN_PREFIX)/xlc
#_cset_ccr_CC_      = $(BIN_PREFIX)/xlc
#_cset_cpp_CC_      = $(BIN_PREFIX)/xlc
#_cset_cppr_CC_      = $(BIN_PREFIX)/xlc
#_cset_cc_CC_      = $(BIN_PREFIX)/blrts_dev_xlC
#_cset_ccr_CC_      = $(BIN_PREFIX)/blrts_dev_xlC
#_cset_cpp_CC_      = $(BIN_PREFIX)/blrts_dev_xlC
#_cset_cppr_CC_      = $(BIN_PREFIX)/blrts_dev_xlC
_cset_cc_CC_      = $(BIN_PREFIX)/xlC
_cset_ccr_CC_      = $(BIN_PREFIX)/xlC
_cset_cpp_CC_      = $(BIN_PREFIX)/xlC
_cset_cppr_CC_      = $(BIN_PREFIX)/xlC

#GCC_LOC = /gsa/watgsa/.home/h1/tjcw/BlueLight-install/blrts-gnu
#GCC_PFX = powerpc-bgl-blrts-gnu-

# Use the 'native' archiver when cross-building from ppc-linux
GCC_LOC = /usr
GCC_BIN = $(GCC_LOC)/bin
GCC_PFX =

AR = $(GCC_BIN)/$(GCC_PFX)ar
RANLIB = $(GCC_BIN)/$(GCC_PFX)ranlib

# Cross-link-edit not working from ODE at present
_cset_cppr_LD_ = $(GCC_BIN)/$(GCC_PFX)gcc
_cset_cpp_LD_ = $(GCC_BIN)/$(GCC_PFX)gcc
LD_OPT_LEVEL =
LDFLAGS = -v -L$(GCC_LOC)/lib/gcc-lib/powerpc-gnu-elf/3.0.2 -L$(GCC_LOC)/powerpc-gnu-elf/lib
# For the moment, no extra libraries; want to know specifically what is needed
EXTRA_LIBS = -lm -lc

# hummer - enable hummer generation
# hummls - always generate load store parallel
# plsthumm - display XIL before/after hummer generation
# nglobshd - turn off global scheduling to make the code more readable
# linux - generate linux/PPC code
# hummchk - check hummer opcodes for correctness
# newsched1:newsched2:maxgridiculous - use new local scheduler
CFLAGS  +=      \
        -B/tmp/tjcw/ -tb \
#       -B/gsa/watgsa/home/t/j/tjcw/mendell/linux_compiler_backends/20030724/ -tb \
        -qarch=440d \
#       -qdebug=shutup:plsthumm:linux \
#       -qdebug=shutup:hummer:plsthumm:linux \
        -qdebug=shutup:hummer:linux \
        -qdebug=newsched1:newsched2:regpres:adra:antidep \
        -qdebug=maxgridiculous \
#       -qdebug=plst3:cycles \
        -qdebug=humsqrt \
# removed per Mark Mendell's advice 2003-07-31       -qdebug=trustalign \
#       -qdebug=hummdbg \
        -qdebug=nopic \
        -DPK_BGL \
        -DPK_BGL_UNI \
        -DPK_BLADE \
        -DRDG_TO_FXLOGGER \
        -DEXCEPTIONS_UNAVAILABLE \
        -qnoeh \
        -qnolm \
        $(BLADE_INC) \
        $(SYSBRINGUP_INC) \
#        -qnounwind \
        -qsuppress=1540-1088:1540-1089:1540-1087:1540-1090 \
#        -qnostdinc \
#        -I $(TOR_HDR_PREFIX) \
#        -I $(HDR_DIR_1) \
#        -I $(HDR_DIR_2) \
#        -I $(HDR_DIR_3) \
#        -I $(HDR_DIR_4) \
#        -I $(HDR_DIR_5) \
##        -I $(HDR_DIR_6) \
##       -I $(HDR_PREFIX)/powerpc-bgl-blrts-gnu \
##       -I $(HDR_PREFIX) \
##       -I $(HDR_PREFIX_2) \
##       -I $(HDR_PREFIX_3) \
##       -I $(HDR_PREFIX)/ext \
##       -I $(HDR_PREFIX)/backward \

#        -D ARCH_HAS_FSEL \

.if $(MACHINE) != "ppc_linux_2"
CFLAGS  +=      \
        -B/afs/tor/u/mendell/public/blueLight/ -tb \

.else
# CFLAGS += -B/gsa/watgsa/.home/h1/tjcw/mendell/public/linux/ -tb \

.endif

CAN_BUILD_MPI = 0
#        -qdebug=ebbpass1:ebbpass2 \
