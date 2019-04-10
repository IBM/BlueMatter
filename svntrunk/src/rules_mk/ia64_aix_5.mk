################################################################################
# IA64/AIX 5.0+ (Monterey) specific variables
#
# This is the Makeconf platform-specific part for CONTEXT=ia64_aix_5
#

OBJECT_FORMAT  = ELF
#LDFLAGS       += -lbsd
CCFAMILY      ?= cset
CCTYPE        ?= cpp

# Booleans (define to turn on, or use .undef to turn off)
UNIX           =
AIX            =
AIX_IA64       =
DEPENDENCIES   =
NO_RANLIB      =

# Gendep flags - include then exclude the system header files
GENDEPFLAGS = -I/usr/include -E/usr/include

