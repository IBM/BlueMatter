################################################################################
# Linux 2.0+ Intel specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=x86_linux_2
#

OBJECT_FORMAT=ELF

INSTALL_TOOL ?= rpm
PKG_TOOL ?= rpm

# Booleans
DEPENDENCIES=
UNIX=
LINUX=
LINUX_X86=

CCFAMILY ?= gnu
CCTYPE   ?= ansi

GENDEPFLAGS = -I/usr/include -E/usr/include

# Additional information needed to make the example work.
CODEPAGE_TO = ISO-8859-1

