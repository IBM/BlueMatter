################################################################################
# OpenBSD 2.5+ specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=x86_openbsd_2
#

OBJECT_FORMAT=A.OUT

# Booleans
DEPENDENCIES=
UNIX=
OPENBSD=

CCFAMILY ?= gnu
CCTYPE   ?= ansi

GENDEPFLAGS = -I/usr/include -E/usr/include

# Additional information needed to make the example work.
CODEPAGE_TO = ISO-8859-1

