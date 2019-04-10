################################################################################
# PowerPC/AIX 4.3+ specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=rios_aix_4
#

OBJECT_FORMAT  = XCOFF

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

