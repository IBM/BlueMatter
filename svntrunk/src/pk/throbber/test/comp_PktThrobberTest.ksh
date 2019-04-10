#! /bin/ksh

rm -f *.o *.rts

#BG_INSTALL_BASE=`installbase.py`
#BG_INSTALL_BASE=/gsa/yktgsa/home/b/g/bgf/sb/devsb/inst.images/rios_aix_5/shipdata
BG_INSTALL_BASE=../../../../../../..
#BG_INSTALL_BASE=${SANDBOXBASE}/inst.images/rios_aix_5/shipdata
#BG_INSTALL_BASE=../../../../inst.images/rios_aix_5/shipdata
export SHIPDATA_DIR=$BG_INSTALL_BASE/usr/opt/bluegene/
export SHIPDATA_INCLUDE=$SHIPDATA_DIR/include/
export SHIPDATA_LIB=$SHIPDATA_DIR/lib/
export SHIPDATA_SRC=$SHIPDATA_DIR/src/
export SHIPDATA_BIN=$SHIPDATA_DIR/bin/
#export BLADE_INC=$BG_INSTALL_BASE/../../../export/rios_aix_5/usr/include/
export LIB_BLADE=./
export BGL_INSTALL=/bgl/BlueLight/ppcfloor

rm -rf *.o
#-g \
#-O2 \
#-DNDEBUG \

echo $SHIPDATA_INCLUDE

#-DTHROBBER_MINSPAN_TREE \

#ssh bgf@bgfe01 "cd $PWD && $BGL_INSTALL/blrts-gnu/bin/powerpc-bgl-blrts-gnu-g++ \
$BGL_INSTALL/blrts-gnu/bin/powerpc-bgl-blrts-gnu-g++ \
-O3 \
-DNDEBUG \
-DPKT_STAND_ALONE \
-DPKFXLOG \
-g \
-DPK_PARALLEL \
-DPK_BGL \
-DPK_BLADE_SPI \
-I./ \
-I$SHIPDATA_INCLUDE \
-I$LIB_BLADE/ \
-I$LIB_BLADE/include \
-I$BGL_INSTALL/bglsys/include \
-I/$BGL_INSTALL/bglsys/pk \
$SHIPDATA_SRC/pk/a2a/packet_alltoallv.cpp \
../PktThrobber.cpp \
-c

#-DTHROBBER_MINSPAN_TREE \

### $LIB_BLADE/PktThrobberIF.cpp \
echo "*************************************************************"
#ssh bgf@bgfe01 "cd $PWD && /opt/ibmcmp/vacpp/bg/8.0/bin/blrts_xlC \
/opt/ibmcmp/vacpp/bg/8.0/bin/blrts_xlC  \
-O3 \
-DNDEBUG \
-DPKFXLOG \
-DPK_BLADE_SPI \
-DPK_PARALLEL \
-DPK_BGL \
-DPK_XLC \
-qarch=440 -I./ \
-I$LIB_BLADE/ \
-I$LIB_BLADE/include \
-I$SHIPDATA_INCLUDE \
-I/$BGL_INSTALL/bglsys/include \
-I/$BGL_INSTALL/bglsys/pk \
-I/bgl/local/fftw-2.1.5/include/fftw.h \
-qsuppress=1540-1090:1540-1087:1540-1089:1540-1088 \
-qnolm \
-qnoeh \
-qnounwind \
-+ \
-g \
-c \
$SHIPDATA_SRC/pk/fxlogger.cpp \
$SHIPDATA_SRC/pk/platform.cpp \
$SHIPDATA_SRC/pk/pktrace.cpp \
$SHIPDATA_SRC/pk/coprocessor_on_coprocessor.cpp \
PktThrobberTest.cpp \

#-pg goes here.
/opt/ibmcmp/vacpp/bg/8.0/bin/blrts_xlC  \
-qnolm \
fxlogger.o \
platform.o \
pktrace.o \
coprocessor_on_coprocessor.o \
PktThrobberTest.o \
packet_alltoallv.o \
PktThrobber.o \
-Wl,-zmuldefs \
-L/$BGL_INSTALL/bglsys/lib \
-L$SHIPDATA_SRC/pk/bonb/ \
-lbonb.rts \
-lmpich.rts \
-lmsglayer.rts \
-lrts.rts \
-ldevices.rts \
-o PktThrobberTest.rts
