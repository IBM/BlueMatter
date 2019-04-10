#! /bin/ksh

rm -rf *.o

rm -rf *.a

# pick out install base
BG_INSTALL_BASE=`installbase.py`
export SHIPDATA_DIR=$BG_INSTALL_BASE/usr/opt/bluegene/
export SHIPDATA_INCLUDE=$SHIPDATA_DIR/include/
export SHIPDATA_LIB=$SHIPDATA_DIR/lib/
export SHIPDATA_SRC=$SHIPDATA_DIR/src/
export SHIPDATA_BIN=$SHIPDATA_DIR/bin/


if [[ -z $BL_INSTALL ]] then
echo "ERROR:: BL_INSTALL is not set."
exit
fi
export BGL_INSTALL=$BL_INSTALL

# -DNO_EXTERNAL_DATAGRAMS \
# -DCOMPILE_VIRTUAL_NODE_MODE
# -DNDEBUG \
#-DPKTRACE \
#-DPKTRACE_ON \
#-qnounwind \

# Second compiler pass ( compile the CODE ONLY )
echo ''
ssh bgfe01 "cd $PWD && /opt/ibmcmp/vacpp/7.0/bin/blrts_xlC  \
-DNO_PK_PLATFORM \
-DMPI \
-O3 \
-qalign=natural \
-qarch=440 \
-qdebug=except \
-qstaticinline \
-qsuppress=1540-1087:1540-1089:1540-1088:1540-1090 \
-qmaxmem=-1 \
-qnoeh \
-qnolm \
-qsource \
-qlist \
-qnostrict \
-qpriority=0 \
-qalign=natural \
-+ \
-I. \
-I$SHIPDATA_INCLUDE \
-I/$BGL_INSTALL/bglsys/include \
$1.cpp \
-L/$BGL_INSTALL/bglsys/lib \
-lmpich.rts \
-lmsglayer.rts \
-lrts.rts \
-ldevices.rts \
-o \
$1.rts"</dev/null;
