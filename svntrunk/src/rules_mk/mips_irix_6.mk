################################################################################
# IRIX 6.5+ specific variables
#
# This is the Makeconf platform specific part for CONTEXT=mips_irix_6
#

OBJECT_FORMAT=ELF

# Booleans
DEPENDENCIES=
UNIX=
IRIX=
IRIX_MIPS=
NO_RANLIB=

CCFAMILY ?= native
CCTYPE   ?= ansi

GENDEPFLAGS = -I/usr/include -E/usr/include

# Additional information needed to make the example work.
CODEPAGE_TO = ISO-8859-1

