################################################################################
# FreeBSD 3.2+ specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=x86_freebsd_3
#

OBJECT_FORMAT=ELF

# Booleans
DEPENDENCIES=
UNIX=
FREEBSD=

CCFAMILY ?= gnu
CCTYPE   ?= ansi

GENDEPFLAGS = -I/usr/include -E/usr/include

# Additional information needed to make the example work.
CODEPAGE_TO = ISO-8859-1

