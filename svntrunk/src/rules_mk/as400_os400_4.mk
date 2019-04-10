################################################################################
# OS/400 V4R4+ specific variables

# Booleans
OS400        =
UNIX         =
NO_RANLIB    =
DEPENDENCIES =

# use del so both the symlink and the underlying file are deleted
RM ?= del

CCFAMILY ?= native
CCTYPE   ?= ansi

# no includes are in IFS, so keep gendep quiet about it
GENDEPFLAGS += -quiet
