#!/usr/bin/ksh
WORKINGDIR=$1
GFEROOT=/u/germain/bluegene/foxfm
STARTDIR=$GFEROOT/BlueMatter/kerngen
ANALYSISDIR=$GFEROOT/BlueMatter/util/analysis

/usr/bin/cp $STARTDIR/enkp.respa.rtp $WORKINGDIR
/usr/bin/cp $STARTDIR/p1.smp.exe $WORKINGDIR
/usr/bin/cp $STARTDIR/enkp.aix.msd $WORKINGDIR
/usr/bin/cp $STARTDIR/enkp.dvs $WORKINGDIR
/usr/bin/cp $STARTDIR/energy_var.ksh $WORKINGDIR
/usr/bin/cp $ANALYSISDIR/rmsde_vs_ts.pl $WORKINGDIR
/usr/bin/cp $ANALYSISDIR/stripEnergy.pl $WORKINGDIR

nohup $WORKINGDIR/rmsde_vs_ts.pl $WORKINGDIR $WORKINGDIR/p1.smp.exe $WORKINGDIR/enkp.aix.msd $WORKINGDIR/enkp.dvs $WORKINGDIR/enkp.respa.rtp $WORKINGDIR/stripEnergy.pl 2 0.0000625 0.000125 0.00025 0.0005 0.001 0.002 > $WORKINGDIR/wrapper.log 2>&1 &
