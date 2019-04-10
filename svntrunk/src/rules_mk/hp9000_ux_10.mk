################################################################################
# HP-UX 10.0+ specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=hp9000_ux_10
#

OBJECT_FORMAT  = A.OUT

# Booleans
UNIX           = 
HPUX           = 
DEPENDENCIES   =

LDFLAGS       += -lBSD
CCFAMILY      ?= native
CCTYPE        ?= ansi

# Gendep flags - include then exclude the system header files
GENDEPFLAGS = -I/usr/include -E/usr/include

