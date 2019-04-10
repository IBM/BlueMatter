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
export BL_INSTALL="/bgl/BlueLight/ppcfloor/"
fi
export BGL_INSTALL=$BL_INSTALL


# ssh bgwfen1 "cd $PWD && $BGL_INSTALL/blrts-gnu/bin/powerpc-bgl-blrts-gnu-g++ \

# ssh bgwfen1 "cd $PWD && $BGL_INSTALL/blrts-gnu/bin/powerpc-bgl-blrts-gnu-g++ \

ssh bgf@bgwfen1 "cd $PWD && /opt/ibmcmp/vacpp/7.0/bin/blrts_xlC \
-qlist \
-qshowinc \
-qsource \
-O3 \
-I. \
-I$SHIPDATA_INCLUDE \
-I/$BGL_INSTALL/bglsys/include \
./HeapManager.cpp \
-L$SHIPDATA_LIB \
-L/$BGL_INSTALL/bglsys/lib \
-lmpich.rts \
-lmsglayer.rts \
-lrts.rts \
-ldevices.rts \
-o \
heap_test.rts"</dev/null;


ssh bgf@bgwfen1 "cd $PWD && $BGL_INSTALL/blrts-gnu/bin/powerpc-bgl-blrts-gnu-g++ \
-O3 \
-I. \
-I$SHIPDATA_INCLUDE \
-I/$BGL_INSTALL/bglsys/include \
./HeapManager.cpp \
-L$SHIPDATA_LIB \
-L/$BGL_INSTALL/bglsys/lib \
-lmpich.rts \
-lmsglayer.rts \
-lrts.rts \
-ldevices.rts \
-o \
heap_test_g++.rts"</dev/null;
