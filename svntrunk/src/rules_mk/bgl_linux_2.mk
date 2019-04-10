################################################################################
# Linux 2.2+ Blue Gene PowerPC specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=bgl_linux_2
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

_cset_cc_CC_    ?= /opt/ibmcmp/vacpp/7.0/bin/xlc ${DEPENDENCIES:D-M}
_cset_cpp_CC_   ?= /opt/ibmcmp/vacpp/7.0/bin/xlC ${DEPENDENCIES:D-M}
_cset_cppr_CC_  ?= /opt/ibmcmp/vacpp/7.0/bin/xlC ${DEPENDENCIES:D-M}
_cset_cc_CFLAGS_        ?=      -qarch=440d
_cset_cpp_CFLAGS_       ?=      -qarch=440d
_cset_cppr_CFLAGS_      ?=      -qarch=440d

