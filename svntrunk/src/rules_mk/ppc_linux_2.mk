################################################################################
# Linux 2.2+ PowerPC specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=ppc_linux_2
#

OBJECT_FORMAT=ELF

# Booleans
DEPENDENCIES=
UNIX=
LINUX=
LINUX_PPC=

CCFAMILY ?= gnu
CCTYPE   ?= ansi

GENDEPFLAGS = -I/usr/include -E/usr/include

# Additional information needed to make the example work.
CODEPAGE_TO = ISO-8859-1

