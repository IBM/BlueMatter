################################################################################
# MVS/OE 2.0+ specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=mvs390_oe_2
#
OBJECT_FORMAT  = A.OUT

# Booleans
MVSOE          = 
UNIX           =
NO_RANLIB      =
DEPENDENCIES   =

CCFAMILY      ?= native
CCTYPE        ?= ansi

# listings for vpl
CFLAGS += -Wc,source,list\(${@:R}.lst\)


# Comment out this variable if you don't want to use prelinked object
# modules instead of standalone programs.  The rules don't
# change compiler/linker files this just determines which
# type of files get installed during the install pass for this example.
#
USE_PRELINKED = 

.if defined(USE_PRELINKED)
# get prelinked object module on MVS/OE:
#
_C89_TMPS  %= 0xfffffffd
_C89_STEPS %= 0xffffffff
.endif # defined(USE_PRELINKED)

# Some variables used for packaging tools and rules
#
PKG_MVS_EXEC_DATASET        = g1spa.bps.exec
PKG_MVS_SUBUILD_LOADLIB     = g1spa.subuild.su231.load
PKG_MVS_FUNCCNTL_DATASET    = g1spa.function.cntl
PKG_MVS_TERSE_CLIST_DATASET = g1spa.tersemvs.clist
# PKG_MVS_JOBMONITOR_TIME and PKG_MVS_CRC_LOADLIB are optional
PKG_MVS_JOBMONITOR_TIME     = 1
PKG_MVS_CRC_LOADLIB         = g1spa.crcmvs.load
PKG_MVS_DELETE_OUTPUT       = YES
PKG_MVS_DISPLAY_OUTPUT      = YES
PKG_MVS_SAVE_OUTPUT_FILE    = NO

# Gendep flags - include then exclude the system header files
GENDEPFLAGS = -I/usr/include -E/usr/include

