################################################################################
# Tru64 5.0+ specific variables
#
# This is the Makeconf platform specific part for CONTEXT=alpha_tru64_5
#

OBJECT_FORMAT=COFF

# Booleans
DEPENDENCIES=
UNIX=
TRU64=
TRU64_ALPHA=

CCFAMILY ?= native
CCTYPE   ?= ansi

GENDEPFLAGS = -I/usr/include -E/usr/include

# Additional information needed to make the example work.
CODEPAGE_TO = ISO-8859-1

