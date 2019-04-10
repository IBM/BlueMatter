################################################################################
# WindowsNT 4.0 specific variables
#
#
# This is the Makeconf platform specific part for CONTEXT=x86_nt_4
#

OBJECT_FORMAT = OMF

# Booleans
WINNT          = 
WIN32          =
NO_RANLIB      =
DEPENDENCIES   =

CCFAMILY      ?= visual
CCTYPE        ?= cpp

.if (${CCFAMILY} == "vage")
# where is the compiler installed? 
CC_INST_BASE  ?= C:/IBMCPPW
LDFLAGS       += /PM:VIO
# Remove comment if you want to use response files
# USE_RESPFILE   =
.elif (${CCFAMILY} == "visual")
# where is the compiler installed? 
CC_INST_BASE  ?= C:/MSDEV
.endif

# Overwrite base definition
#
PKG_CONTROL_DIR=${SOURCEBASE}/../inst.images/${CONTEXT}/mdata/OdeHello
PKG_CONTROL_DIR%=${PKG_CONTROL_DIR}
TOSTAGE=${SOURCEBASE}/../inst.images/${CONTEXT}/mdata/OdeHello
# Assign TOSTAGE as an environment variable for InstallShield
TOSTAGE%=${TOSTAGE}

PKG_ISPE_MEDIA=OdeHelloMedia

# Gendep flags - include then exclude the system header files
GENDEPFLAGS = -I${CC_INST_BASE}/include -E${CC_INST_BASE}/include
.if (${CCFAMILY} == "vage")
GENDEPFLAGS += -I${CC_INST_BASE}/sdk/winh -E${CC_INST_BASE}/sdk/winh
.endif
