#! /bin/ksh
echo

#BG_INSTALL_BASE=`installbase.py`
#BG_INSTALL_BASE=/gsa/watgsa/home/b/g/bgf/sb/blood/inst.images/rios_aix_5/shipdata
#BG_INSTALL_BASE=${SANDBOXBASE}/inst.images/rios_aix_5/shipdata
#export SHIPDATA_DIR=$BG_INSTALL_BASE/usr/opt/bluegene/
#export SHIPDATA_INCLUDE=$SHIPDATA_DIR/include/
#export SHIPDATA_LIB=$SHIPDATA_DIR/lib/
#export SHIPDATA_SRC=$SHIPDATA_DIR/src/
#export SHIPDATA_BIN=$SHIPDATA_DIR/bin/
#export BLADE_INC=$BG_INSTALL_BASE/../../../export/rios_aix_5/usr/include/

export LIB_BLADE=../
export BGL_INSTALL=/bgl/BlueLight/ppcfloor

$BGL_INSTALL/blrts-gnu/bin/powerpc-bgl-blrts-gnu-g++ \
-DNDEBUG \
-O3 \
-DPKT_STAND_ALONE \
-DPKFXLOG \
-g \
-DPK_PARALLEL \
-DPK_BGL \
-DPK_BLADE_SPI \
-I./ \
-I$LIB_BLADE \
-I$BGL_INSTALL/bglsys/include \
-c \
iv00.c \


/opt/ibmcmp/vacpp/bg/8.0/bin/blrts_xlC  \
-qnolm \
iv00.o \
-zmuldefs \
-L/$BGL_INSTALL/bglsys/lib \
-L$LIB_BLADE/src/ \
-lbonb.rts \
-lmpich.rts \
-lmsglayer.rts \
-lrts.rts \
-ldevices.rts \
-o iv00.rts
