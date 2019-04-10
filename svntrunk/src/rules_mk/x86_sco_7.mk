################################################################################
# SCO UnixWare 7 specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=x86_sco_7
#

OBJECT_FORMAT=ELF

# Booleans
DEPENDENCIES=
UNIX=
SCO=
NO_RANLIB=

CCFAMILY ?= native
CCTYPE   ?= cc

GENDEPFLAGS = -I/usr/include -E/usr/include
