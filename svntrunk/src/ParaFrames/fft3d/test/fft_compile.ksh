#! /bin/ksh

rm -rf *.o

# pick out install base
BG_INSTALL_BASE=`installbase.py`
export SHIPDATA_DIR=$BG_INSTALL_BASE/usr/opt/bluegene/
export SHIPDATA_INCLUDE=$SHIPDATA_DIR/include/
export SHIPDATA_LIB=$SHIPDATA_DIR/lib/
export SHIPDATA_SRC=$SHIPDATA_DIR/src/
export SHIPDATA_BIN=$SHIPDATA_DIR/bin/


export HW_INC_TOP=$BG_INSTALL_BASE/../../../../
export BLADE_INC=$HW_INC_TOP/blade/include


# -DTEST_FFT_DATA_MOVEMENT \
# -DPKTRACE_TUNE=1 \
# -DPKTRACE_ON  \
# -DPK_BLADE_PROFILING \
# -DUSE_BGL_TREE \
# -DTEST_FFT_DATA_MOVEMENT \
# -DPKTRACE_TUNE=1 \
# -DPKTRACE_ON  \

rsh lipid "cd $PWD && /opt/ibmcmp/vacpp/6.0/bin/blrts_xlC \
-I./ \
-DRANDOM_FOR_FFT \
-I$SHIPDATA_INCLUDE \
-I$BLADE_INC \
-qsuppress=1540-1090:1540-1087:1540-1089:1540-1088 \
-qnoeh -qnounwind \
-qarch=440d \
-qpriority=1 \
-DPK_PARALLEL \
-DSW_HAS_A_PROBLEM_WITH_CTIME \
-DPK_BLADE \
-DPK_BGL \
-DPK_XLC \
-DRDG_TO_FXLOGGER \
-DPKFXLOG \
-DUSE_2_CORES_FOR_FFT \
-DUSE_DOUBLE_HUMMER_1D_FFT \
-qsource \
-qlist \
-O3 \
-c \
./fftmain_new.cpp \
$SHIPDATA_SRC/pk/fxlogger.cpp \
$SHIPDATA_SRC/pk/platform.cpp \
$SHIPDATA_SRC/pk/pktrace.cpp"</dev/null

rsh lipid "cd $PWD && rm -f $1.a && ar q $1.a *.o" </dev/null
