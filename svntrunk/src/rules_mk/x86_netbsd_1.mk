################################################################################
# NetBSD 1.0+ specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=x86_netbsd_1
#

OBJECT_FORMAT=A.OUT

# Booleans
DEPENDENCIES=
UNIX=
NETBSD=

CCFAMILY ?= gnu
CCTYPE   ?= ansi

GENDEPFLAGS = -I/usr/include -E/usr/include

# Additional information needed to make the example work.
CODEPAGE_TO = ISO-8859-1

