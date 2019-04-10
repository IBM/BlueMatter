################################################################################
# OS/2 Version 4.0 specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=x86_os2_4
#

OBJECT_FORMAT = OMF

# Booleans
OS2            = 
NO_RANLIB      = 
DEPENDENCIES   =

CCFAMILY      ?= vage
CCTYPE        ?= cpp

.if (${CCFAMILY} == "vage")
LDFLAGS       += /PM:VIO /NOI
# where is the compiler installed? 
CC_INST_BASE  ?= C:/IBMCPP
# Remove comment if you want to use response files
# USE_RESPFILE   =
.endif

# Overwrite base definition
#
TOSTAGE        = ${SOURCEBASE}/../tools/${CONTEXT}

# Gendep flags - include then exclude the system header files
GENDEPFLAGS = -I${CC_INST_BASE}/include -E${CC_INST_BASE}/include
GENDEPFLAGS += -I${CC_INST_BASE}/include/os2 -E${CC_INST_BASE}/include/os2
