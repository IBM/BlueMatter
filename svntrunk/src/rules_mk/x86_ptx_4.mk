################################################################################
# DYNIX/ptx 4.4+ Intel specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=x86_ptx_4
#

OBJECT_FORMAT  = ELF

# Booleans
NO_RANLIB      = 
DYNIXPTX       =
DYNIXPTX_X86   =
UNIX           =
DEPENDENCIES   = 

CCFAMILY      ?= native
CCTYPE        ?= cc

# Gendep flags - include then exclude the system header files
GENDEPFLAGS = -I/usr/include -E/usr/include
