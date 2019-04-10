################################################################################
# Windows95 specific variables
#
# This is the Makeconf platform specific part for CONTEXT=x86_95_4
#
OBJECT_FORMAT  = OMF

# Booleans
WIN95          = 
WIN32          =
NO_RANLIB      =
DEPENDENCIES   =

CCFAMILY      ?= visual
CCTYPE        ?= cpp

.if (${CCFAMILY} == "vage")
# where is the compiler installed? 
CC_INST_BASE  ?= C:/IBMCPPW
LDFLAGS       += /PM:VIO
USE_RESPFILE   =
.elif (${CCFAMILY} == "visual")
# where is the compiler installed? 
CC_INST_BASE  ?= C:/MSDEV
.endif

# Overwrite base definition
#
TOSTAGE=${SOURCEBASE}/../inst.images/${CONTEXT}/mdata/OdeHello
# Assign TOSTAGE as an environment variable for InstallShield
TOSTAGE%=${TOSTAGE}
PKG_WIN32_MEDIA=OdeHelloMedia

# Gendep flags - include then exclude the system header files
GENDEPFLAGS = -I${CC_INST_BASE}/include -E${CC_INST_BASE}/include
.if (${CCFAMILY} == "vage")
GENDEPFLAGS += -I${CC_INST_BASE}/sdk/winh -E${CC_INST_BASE}/sdk/winh
.endif
