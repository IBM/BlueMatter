# Statistics for grouped trace points from trace_group_id 245
# Statistics for grouped trace points from trace_group_id 245
# All data will be placed inline
# column 1: node count
# column 2: average
# column 3: std. dev.
# column 4: min()
# column 5: max()
# column 6: count()
reset
set logscale xy
set key below
set linestyle 1
set linestyle 2
set linestyle 3
set linestyle 4
set linestyle 5
set linestyle 6
set linestyle 7
set linestyle 8
set linestyle 9
set linestyle 10
set linestyle 11
set linestyle 12
set linestyle 13
set linestyle 14
set linestyle 15
set linestyle 16
set linestyle 17
set linestyle 18
set linestyle 19
set linestyle 20
set linestyle 21
set linestyle 22
set xlabel 'Node Count'
set ylabel 'Elapsed Time (nsec.)'
plot 'hairpin.p5.float.spi.dat' index 0 using 2:3:4:5 title 'FatCompVerletListGen' with errorbars linestyle 1,\
'hairpin.p5.float.spi.dat' index 1 using 2:3:4:5 title 'MsgBcast' with errorbars linestyle 2,\
'hairpin.p5.float.spi.dat' index 2 using 2:3:4:5 title 'EveryNTimeSteps' with errorbars linestyle 3,\
'hairpin.p5.float.spi.dat' index 4 using 2:3:4:5 title 'ThrobberBcast' with errorbars linestyle 4,\
'hairpin.p5.float.spi.dat' index 5 using 2:3:4:5 title 'UDF_WaterSPCRattle' with errorbars linestyle 5,\
'hairpin.p5.float.spi.dat' index 6 using 2:3:4:5 title 'UDF_UpdateVelocity' with errorbars linestyle 6,\
'hairpin.p5.float.spi.dat' index 7 using 2:3:4:5 title 'GuardZoneViolationReduction' with errorbars linestyle 7,\
'hairpin.p5.float.spi.dat' index 9 using 2:3:4:5 title 'MsgReduce' with errorbars linestyle 9,\
'hairpin.p5.float.spi.dat' index 10 using 2:3:4:5 title 'P2_Simtick_Loop_Control' with errorbars linestyle 10,\
'hairpin.p5.float.spi.dat' index 11 using 2:3:4:5 title 'GlobalizePositions_Cntl' with errorbars linestyle 11,\
'hairpin.p5.float.spi.dat' index 12 using 2:3:4:5 title 'P2_InitialBarrier' with errorbars linestyle 12,\
'hairpin.p5.float.spi.dat' index 3 using 2:3:4:5 title 'UDF_StdHarmonicBondForce' with errorbars linestyle 13,\
'hairpin.p5.float.spi.dat' index 14 using 2:3:4:5 title 'P2_NSQ_Control' with errorbars linestyle 14,\
'hairpin.p5.float.spi.dat' index 15 using 2:3:4:5 title 'UDF_KineticEnergy' with errorbars linestyle 15,\
'hairpin.p5.float.spi.dat' index 13 using 2:3:4:5 title 'mFragmentMigration' with errorbars linestyle 16,\
'hairpin.p5.float.spi.dat' index 16 using 2:3:4:5 title 'ReduceForces_Cntl' with errorbars linestyle 17,\
'hairpin.p5.float.spi.dat' index 17 using 2:3:4:5 title 'UDF_WaterSPCShake' with errorbars linestyle 18,\
'hairpin.p5.float.spi.dat' index 19 using 2:3:4:5 title 'UDF_UpdatePosition' with errorbars linestyle 20,\
'hairpin.p5.float.spi.dat' index 20 using 2:3:4:5 title 'ThrobberReduce' with errorbars linestyle 21,\
'hairpin.p5.float.spi.dat' index 21 using 2:3:4:5 title 'NSQ_RealSpace_Meat_' with errorbars linestyle 22,\
'hairpin.p5.float.spi.dat' index 0 using 2:3 notitle with lines linestyle 1,\
'hairpin.p5.float.spi.dat' index 1 using 2:3 notitle with lines linestyle 2,\
'hairpin.p5.float.spi.dat' index 2 using 2:3 notitle with lines linestyle 3,\
'hairpin.p5.float.spi.dat' index 4 using 2:3 notitle with lines linestyle 4,\
'hairpin.p5.float.spi.dat' index 5 using 2:3 notitle with lines linestyle 5,\
'hairpin.p5.float.spi.dat' index 6 using 2:3 notitle with lines linestyle 6,\
'hairpin.p5.float.spi.dat' index 7 using 2:3 notitle with lines linestyle 7,\
'hairpin.p5.float.spi.dat' index 9 using 2:3 notitle with lines linestyle 9,\
'hairpin.p5.float.spi.dat' index 10 using 2:3 notitle with lines linestyle 10,\
'hairpin.p5.float.spi.dat' index 11 using 2:3 notitle with lines linestyle 11,\
'hairpin.p5.float.spi.dat' index 12 using 2:3 notitle with lines linestyle 12,\
'hairpin.p5.float.spi.dat' index 3 using 2:3 notitle with lines linestyle 13,\
'hairpin.p5.float.spi.dat' index 14 using 2:3 notitle with lines linestyle 14,\
'hairpin.p5.float.spi.dat' index 15 using 2:3 notitle with lines linestyle 15,\
'hairpin.p5.float.spi.dat' index 13 using 2:3 notitle with lines linestyle 16,\
'hairpin.p5.float.spi.dat' index 16 using 2:3 notitle with lines linestyle 17,\
'hairpin.p5.float.spi.dat' index 17 using 2:3 notitle with lines linestyle 18,\
'hairpin.p5.float.spi.dat' index 19 using 2:3 notitle with lines linestyle 20,\
'hairpin.p5.float.spi.dat' index 20 using 2:3 notitle with lines linestyle 21,\
'hairpin.p5.float.spi.dat' index 21 using 2:3 notitle with lines linestyle 22
