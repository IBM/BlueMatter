ENV_TOP=../../../../

include $(ENV_TOP)/sysbringup/Make.sys

TEST     = ring
C_SRC    = aLockBoxTryLock.c
#TEST_BLNIE     = enkp_10a.verlet.blnie.fxlog
#C_SRC          = tr_fpreduce.c treereduce.cpp

LOCAL_INCLUDES = -I. -I$(SYSBRINGUP_TOP)/tree/include -I$(SYSBRINGUP_TOP)/generic/include
LOCAL_LIBS     = $(TEST).a -L$(SYSBRINGUP_TOP)/tree/lib/$(SB_TARGET) -L$(SYSBRINGUP_TOP)/generic/lib/$(SB_TARGET) -lbgl_tr -ldiag_tr -lsysbringup

SB_OBJ_DIR     = ../$(SB_TARGET)/$(TEST)

include $(SYSBRINGUP_TOP)/Make.test
