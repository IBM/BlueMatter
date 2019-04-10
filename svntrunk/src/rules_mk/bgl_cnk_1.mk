################################################################################
# BG/L Compute Node specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=bgl_cnk_1
#  (cross-compiled from a ppc_linux host)
#

OBJECT_FORMAT=ELF

# Booleans
DEPENDENCIES=
UNIX=
LINUX=
LINUX_PPC=

CCFAMILY ?= cset
CCTYPE   ?= cpp

GENDEPFLAGS = -I/usr/include -E/usr/include

# Additional information needed to make the example work.
CODEPAGE_TO = ISO-8859-1

