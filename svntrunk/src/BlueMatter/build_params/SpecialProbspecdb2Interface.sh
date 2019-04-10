#! /bin/ksh

# $1 = system ID
# $2 = ctp ID
# $3 = Legacy compile_msd name
# $4 = msd file name

BG_INSTALL_BASE=`installbase.py`
export SHIPDATA_DIR=$BG_INSTALL_BASE/usr/opt/bluegene
export SHIPDATA_SETUP=$SHIPDATA_DIR/bin/setup

echo "Creating msd..."
echo "Using parameters: "
echo $1
echo $2
echo `cat $SHIPDATA_SETUP/$3.ref`
echo $4

java com.ibm.bluematter.db2probspec.ProbspecgenDB2 $1 $2 `cat $SHIPDATA_SETUP/$3.ref` $4 -database mdsetup
