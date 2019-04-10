# Statistics for trace points from trace_group_id 2
reset
set logscale xy
set key outside
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
set linestyle 23
set linestyle 24
set linestyle 25
set linestyle 26
set linestyle 27
set linestyle 28
set linestyle 29
set linestyle 30
set linestyle 31
set linestyle 32
set xlabel 'Node Count'
set ylabel 'Elapsed Time (pclocks)'
plot 'ewald.new.dat' index 0 using 1:2:4:5 title 'Ewald_AllReduce' with errorbars,\
'ewald.new.dat' index 1 using 1:2:4:5 title 'Ewald_Eiks' with errorbars,\
'ewald.new.dat' index 2 using 1:2:4:5 title 'Ewald_Forces' with errorbars,\
'ewald.new.dat' index 3 using 1:2:4:5 title 'Ewald_KSpace' with errorbars,\
'ewald.new.dat' index 4 using 1:2:4:5 title 'Ewald_LocalReduce' with errorbars,\
'ewald.new.dat' index 5 using 1:2:4:5 title 'Ewald_Loop' with errorbars,\
'ewald.new.dat' index 6 using 1:2:4:5 title 'GlobalizePositions_Cntl' with errorbars,\
'ewald.new.dat' index 7 using 1:2:4:5 title 'P2_KSpace_Control' with errorbars,\
'ewald.new.dat' index 8 using 1:2:4:5 title 'P2_NSQ_Control' with errorbars,\
'ewald.new.dat' index 9 using 1:2:4:5 title 'ReduceForces_Cntl' with errorbars,\
'ewald.new.dat' index 10 using 1:2:4:5 title 'UDF_KineticEnergy' with errorbars,\
'ewald.new.dat' index 11 using 1:2:4:5 title 'UDF_StdHarmonicBondForce' with errorbars,\
'ewald.new.dat' index 12 using 1:2:4:5 title 'UDF_UpdatePosition' with errorbars,\
'ewald.new.dat' index 13 using 1:2:4:5 title 'UDF_UpdateVelocity' with errorbars,\
'ewald.new.dat' index 14 using 1:2:4:5 title 'UDF_WaterSPCRattle' with errorbars,\
'ewald.new.dat' index 15 using 1:2:4:5 title 'UDF_WaterSPCShake' with errorbars,\
'ewald.new.dat' index 16 using 1:2:4:5 title 'TS_from_Ewald_AllReduce' with errorbars,\
'ewald.new.dat' index 17 using 1:2:4:5 title 'TS_from_Ewald_Eiks' with errorbars,\
'ewald.new.dat' index 18 using 1:2:4:5 title 'TS_from_Ewald_Forces' with errorbars,\
'ewald.new.dat' index 19 using 1:2:4:5 title 'TS_from_Ewald_KSpace' with errorbars,\
'ewald.new.dat' index 20 using 1:2:4:5 title 'TS_from_Ewald_LocalReduce' with errorbars,\
'ewald.new.dat' index 21 using 1:2:4:5 title 'TS_from_Ewald_Loop' with errorbars,\
'ewald.new.dat' index 22 using 1:2:4:5 title 'TS_from_GlobalizePositions_Cntl' with errorbars,\
'ewald.new.dat' index 23 using 1:2:4:5 title 'TS_from_P2_KSpace_Control' with errorbars,\
'ewald.new.dat' index 24 using 1:2:4:5 title 'TS_from_P2_NSQ_Control' with errorbars,\
'ewald.new.dat' index 25 using 1:2:4:5 title 'TS_from_ReduceForces_Cntl' with errorbars,\
'ewald.new.dat' index 26 using 1:2:4:5 title 'TS_from_UDF_KineticEnergy' with errorbars,\
'ewald.new.dat' index 27 using 1:2:4:5 title 'TS_from_UDF_StdHarmonicBondForce' with errorbars,\
'ewald.new.dat' index 28 using 1:2:4:5 title 'TS_from_UDF_UpdatePosition' with errorbars,\
'ewald.new.dat' index 29 using 1:2:4:5 title 'TS_from_UDF_UpdateVelocity' with errorbars,\
'ewald.new.dat' index 30 using 1:2:4:5 title 'TS_from_UDF_WaterSPCRattle' with errorbars,\
'ewald.new.dat' index 31 using 1:2:4:5 title 'TS_from_UDF_WaterSPCShake' with errorbars,\
'ewald.new.dat' index 0 using 1:2 notitle with lines linestyle 1,\
'ewald.new.dat' index 1 using 1:2 notitle with lines linestyle 2,\
'ewald.new.dat' index 2 using 1:2 notitle with lines linestyle 3,\
'ewald.new.dat' index 3 using 1:2 notitle with lines linestyle 4,\
'ewald.new.dat' index 4 using 1:2 notitle with lines linestyle 5,\
'ewald.new.dat' index 5 using 1:2 notitle with lines linestyle 6,\
'ewald.new.dat' index 6 using 1:2 notitle with lines linestyle 7,\
'ewald.new.dat' index 7 using 1:2 notitle with lines linestyle 8,\
'ewald.new.dat' index 8 using 1:2 notitle with lines linestyle 9,\
'ewald.new.dat' index 9 using 1:2 notitle with lines linestyle 10,\
'ewald.new.dat' index 10 using 1:2 notitle with lines linestyle 11,\
'ewald.new.dat' index 11 using 1:2 notitle with lines linestyle 12,\
'ewald.new.dat' index 12 using 1:2 notitle with lines linestyle 13,\
'ewald.new.dat' index 13 using 1:2 notitle with lines linestyle 14,\
'ewald.new.dat' index 14 using 1:2 notitle with lines linestyle 15,\
'ewald.new.dat' index 15 using 1:2 notitle with lines linestyle 16,\
'ewald.new.dat' index 16 using 1:2 notitle with lines linestyle 17,\
'ewald.new.dat' index 17 using 1:2 notitle with lines linestyle 18,\
'ewald.new.dat' index 18 using 1:2 notitle with lines linestyle 19,\
'ewald.new.dat' index 19 using 1:2 notitle with lines linestyle 20,\
'ewald.new.dat' index 20 using 1:2 notitle with lines linestyle 21,\
'ewald.new.dat' index 21 using 1:2 notitle with lines linestyle 22,\
'ewald.new.dat' index 22 using 1:2 notitle with lines linestyle 23,\
'ewald.new.dat' index 23 using 1:2 notitle with lines linestyle 24,\
'ewald.new.dat' index 24 using 1:2 notitle with lines linestyle 25,\
'ewald.new.dat' index 25 using 1:2 notitle with lines linestyle 26,\
'ewald.new.dat' index 26 using 1:2 notitle with lines linestyle 27,\
'ewald.new.dat' index 27 using 1:2 notitle with lines linestyle 28,\
'ewald.new.dat' index 28 using 1:2 notitle with lines linestyle 29,\
'ewald.new.dat' index 29 using 1:2 notitle with lines linestyle 30,\
'ewald.new.dat' index 30 using 1:2 notitle with lines linestyle 31,\
'ewald.new.dat' index 31 using 1:2 notitle with lines linestyle 32
