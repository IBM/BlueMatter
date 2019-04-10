ENV_TOP=../../../../
include $(ENV_TOP)/sysbringup/Make.sys

#-DUSE_2_CORES_FOR_FFT - when not set, packet send must also poll
#-DPKFXLOG
#-DUSE_FFTW_ONE_DIM
#-DREGRESS_FFTW

#-DFFT3D_SPINWAIT - don't use sleep to desched - for HW or if you OWN enough procs

#
#-DUSE_2_CORES_FOR_FFT - when not set, packet send must also poll
#-DFFT3D_SEND_LOCAL_ACTORS  # This is fored in code by USE_2_CORES_FOR_FFT flag
#-DPKFXLOG_USE_WALL_CLOCK_TIME -DPKFXLOG_GATHER_Z=0 -DPKFXLOG_PACKET_ACTOR_FX_Z=0 -DPKFXLOG_DEBUG_DATA=0

CFLAGS +=  -DUSE_2_CORES_FOR_FFT -DFFT3D_SENDER_COMPUTES -DNO_PK_PLATFORM -DPK_AIX -DPK_BLADE -DPKFXLOG  -Wno-deprecated -O3 

TEST_BLNIE     = fft3D_blnie
C_SRC          = fftmain.cpp

LOCAL_INCLUDES = -I. -I$(GFEBUILDROOT)/include -I$(SYSBRINGUP_TOP)/tree/include -I$(SYSBRINGUP_TOP)/generic/include
LOCAL_LIBS     = -L$(SYSBRINGUP_TOP)/tree/lib/$(SB_TARGET) -L$(SYSBRINGUP_TOP)/generic/lib/$(SB_TARGET) -lm -lbgl_tr -ldiag_tr -lsysbringup  -L/usr/gnu/lib -lstdc++ ./libfftw.a
SB_OBJ_DIR     = ../$(SB_TARGET)/$(TEST)

include $(SYSBRINGUP_TOP)/Make.test
