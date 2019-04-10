################################################################################
# Linux 2.2+ PowerPC 64-bit specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=ppc64_linux_2
#

OBJECT_FORMAT=ELF

# Booleans
DEPENDENCIES=
UNIX=
LINUX=
LINUX_PPC64=

CCFAMILY ?= gnu
CCTYPE   ?= ansi

GENDEPFLAGS = -I/usr/include -E/usr/include

# Additional information needed to make the example work.
CODEPAGE_TO = ISO-8859-1

