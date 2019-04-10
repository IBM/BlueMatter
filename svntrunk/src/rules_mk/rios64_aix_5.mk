################################################################################
# PowerPC/AIX5 64-bit specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=rios64_aix_5
#

OBJECT_FORMAT  = XCOFF

# Booleans
AIX            =
AIX_PPC64      =
UNIX           =
DEPENDENCIES   = 

#LDFLAGS       += -lbsd
CCFAMILY      ?= cset
CCTYPE        ?= cpp

# Gendep flags - include then exclude the system header files
GENDEPFLAGS = -I/usr/include -E/usr/include

