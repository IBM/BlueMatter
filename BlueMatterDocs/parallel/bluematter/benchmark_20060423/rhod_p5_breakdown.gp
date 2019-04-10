# Statistics for grouped trace points from trace_group_id 247
# data read from file
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
set xlabel 'Node Count'
set ylabel 'Elapsed Time (nsec.)'
plot 'rhod_p5_breakdown.dat' index 0 using 2:3:5:6 title 'EveryNTimeSteps' with errorbars linestyle 1,\
'rhod_p5_breakdown.dat' index 1 using 2:3:5:6 title 'FatCompVerletListGen' with errorbars linestyle 2,\
'rhod_p5_breakdown.dat' index 2 using 2:3:5:6 title 'GlobalizePositions_Cntl' with errorbars linestyle 3,\
'rhod_p5_breakdown.dat' index 3 using 2:3:5:6 title 'GuardZoneViolationReduction' with errorbars linestyle 4,\
'rhod_p5_breakdown.dat' index 4 using 2:3:5:6 title 'MsgBcast' with errorbars linestyle 5,\
'rhod_p5_breakdown.dat' index 5 using 2:3:5:6 title 'MsgReduce' with errorbars linestyle 6,\
'rhod_p5_breakdown.dat' index 6 using 2:3:5:6 title 'NSQ_RealSpace_Meat_' with errorbars linestyle 7,\
'rhod_p5_breakdown.dat' index 7 using 2:3:5:6 title 'P2_InitialBarrier' with errorbars linestyle 8,\
'rhod_p5_breakdown.dat' index 8 using 2:3:5:6 title 'P2_NSQ_Control' with errorbars linestyle 9,\
'rhod_p5_breakdown.dat' index 9 using 2:3:5:6 title 'P2_Simtick_Loop_Control' with errorbars linestyle 10,\
'rhod_p5_breakdown.dat' index 10 using 2:3:5:6 title 'ReduceForces_Cntl' with errorbars linestyle 11,\
'rhod_p5_breakdown.dat' index 11 using 2:3:5:6 title 'Sync' with errorbars linestyle 12,\
'rhod_p5_breakdown.dat' index 12 using 2:3:5:6 title 'ThrobberBcast' with errorbars linestyle 13,\
'rhod_p5_breakdown.dat' index 13 using 2:3:5:6 title 'ThrobberReduce' with errorbars linestyle 14,\
'rhod_p5_breakdown.dat' index 14 using 2:3:5:6 title 'UDF_KineticEnergy' with errorbars linestyle 15,\
'rhod_p5_breakdown.dat' index 15 using 2:3:5:6 title 'UDF_StdHarmonicBondForce' with errorbars linestyle 16,\
'rhod_p5_breakdown.dat' index 16 using 2:3:5:6 title 'UDF_UpdatePosition' with errorbars linestyle 17,\
'rhod_p5_breakdown.dat' index 17 using 2:3:5:6 title 'UDF_UpdateVelocity' with errorbars linestyle 18,\
'rhod_p5_breakdown.dat' index 18 using 2:3:5:6 title 'UDF_WaterTIP3PRattle' with errorbars linestyle 19,\
'rhod_p5_breakdown.dat' index 19 using 2:3:5:6 title 'UDF_WaterTIP3PShake' with errorbars linestyle 20,\
'rhod_p5_breakdown.dat' index 20 using 2:3:5:6 title 'mFragmentMigration' with errorbars linestyle 21,\
'rhod_p5_breakdown.dat' index 0 using 2:3 notitle with lines linestyle 1,\
'rhod_p5_breakdown.dat' index 1 using 2:3 notitle with lines linestyle 2,\
'rhod_p5_breakdown.dat' index 2 using 2:3 notitle with lines linestyle 3,\
'rhod_p5_breakdown.dat' index 3 using 2:3 notitle with lines linestyle 4,\
'rhod_p5_breakdown.dat' index 4 using 2:3 notitle with lines linestyle 5,\
'rhod_p5_breakdown.dat' index 5 using 2:3 notitle with lines linestyle 6,\
'rhod_p5_breakdown.dat' index 6 using 2:3 notitle with lines linestyle 7,\
'rhod_p5_breakdown.dat' index 7 using 2:3 notitle with lines linestyle 8,\
'rhod_p5_breakdown.dat' index 8 using 2:3 notitle with lines linestyle 9,\
'rhod_p5_breakdown.dat' index 9 using 2:3 notitle with lines linestyle 10,\
'rhod_p5_breakdown.dat' index 10 using 2:3 notitle with lines linestyle 11,\
'rhod_p5_breakdown.dat' index 11 using 2:3 notitle with lines linestyle 12,\
'rhod_p5_breakdown.dat' index 12 using 2:3 notitle with lines linestyle 13,\
'rhod_p5_breakdown.dat' index 13 using 2:3 notitle with lines linestyle 14,\
'rhod_p5_breakdown.dat' index 14 using 2:3 notitle with lines linestyle 15,\
'rhod_p5_breakdown.dat' index 15 using 2:3 notitle with lines linestyle 16,\
'rhod_p5_breakdown.dat' index 16 using 2:3 notitle with lines linestyle 17,\
'rhod_p5_breakdown.dat' index 17 using 2:3 notitle with lines linestyle 18,\
'rhod_p5_breakdown.dat' index 18 using 2:3 notitle with lines linestyle 19,\
'rhod_p5_breakdown.dat' index 19 using 2:3 notitle with lines linestyle 20,\
'rhod_p5_breakdown.dat' index 20 using 2:3 notitle with lines linestyle 21
