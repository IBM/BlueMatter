# Statistics for trace points from trace_group_id 12
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
set xlabel 'Node Count'
set ylabel 'Elapsed Time (pclocks)'
plot 'p3me_mesh_20040406.dat' index 0 using 1:2:4:5 title 'GlobalizePositions_Cntl' with errorbars linestyle 1,\
'p3me_mesh_20040406.dat' index 1 using 1:2:4:5 title 'P2_KSpace_Control' with errorbars linestyle 2,\
'p3me_mesh_20040406.dat' index 2 using 1:2:4:5 title 'P2_NSQ_Control' with errorbars linestyle 3,\
'p3me_mesh_20040406.dat' index 3 using 1:2:4:5 title 'P3ME_AssignCharge_Control' with errorbars linestyle 4,\
'p3me_mesh_20040406.dat' index 4 using 1:2:4:5 title 'P3ME_Convolve' with errorbars linestyle 5,\
'p3me_mesh_20040406.dat' index 5 using 1:2:4:5 title 'P3ME_ForwardFFT_Control' with errorbars linestyle 6,\
'p3me_mesh_20040406.dat' index 6 using 1:2:4:5 title 'P3ME_Interpolate_Control' with errorbars linestyle 7,\
'p3me_mesh_20040406.dat' index 7 using 1:2:4:5 title 'P3ME_Rev_FFT' with errorbars linestyle 8,\
'p3me_mesh_20040406.dat' index 8 using 1:2:4:5 title 'ReduceForces_Cntl' with errorbars linestyle 9,\
'p3me_mesh_20040406.dat' index 9 using 1:2:4:5 title 'UDF_KineticEnergy' with errorbars linestyle 10,\
'p3me_mesh_20040406.dat' index 10 using 1:2:4:5 title 'UDF_StdHarmonicBondForce' with errorbars linestyle 11,\
'p3me_mesh_20040406.dat' index 11 using 1:2:4:5 title 'UDF_UpdatePosition' with errorbars linestyle 12,\
'p3me_mesh_20040406.dat' index 12 using 1:2:4:5 title 'UDF_UpdateVelocity' with errorbars linestyle 13,\
'p3me_mesh_20040406.dat' index 13 using 1:2:4:5 title 'UDF_WaterSPCRattle' with errorbars linestyle 14,\
'p3me_mesh_20040406.dat' index 14 using 1:2:4:5 title 'UDF_WaterSPCShake' with errorbars linestyle 15,\
'p3me_mesh_20040406.dat' index 15 using 1:2:4:5 title 'TS_from_GlobalizePositions_Cntl' with errorbars linestyle 16,\
'p3me_mesh_20040406.dat' index 16 using 1:2:4:5 title 'TS_from_P2_KSpace_Control' with errorbars linestyle 17,\
'p3me_mesh_20040406.dat' index 17 using 1:2:4:5 title 'TS_from_P2_NSQ_Control' with errorbars linestyle 18,\
'p3me_mesh_20040406.dat' index 18 using 1:2:4:5 title 'TS_from_P3ME_AssignCharge_Control' with errorbars linestyle 19,\
'p3me_mesh_20040406.dat' index 19 using 1:2:4:5 title 'TS_from_P3ME_Convolve' with errorbars linestyle 20,\
'p3me_mesh_20040406.dat' index 20 using 1:2:4:5 title 'TS_from_P3ME_ForwardFFT_Control' with errorbars linestyle 21,\
'p3me_mesh_20040406.dat' index 21 using 1:2:4:5 title 'TS_from_P3ME_Interpolate_Control' with errorbars linestyle 22,\
'p3me_mesh_20040406.dat' index 22 using 1:2:4:5 title 'TS_from_P3ME_Rev_FFT' with errorbars linestyle 23,\
'p3me_mesh_20040406.dat' index 23 using 1:2:4:5 title 'TS_from_ReduceForces_Cntl' with errorbars linestyle 24,\
'p3me_mesh_20040406.dat' index 24 using 1:2:4:5 title 'TS_from_UDF_KineticEnergy' with errorbars linestyle 25,\
'p3me_mesh_20040406.dat' index 25 using 1:2:4:5 title 'TS_from_UDF_StdHarmonicBondForce' with errorbars linestyle 26,\
'p3me_mesh_20040406.dat' index 26 using 1:2:4:5 title 'TS_from_UDF_UpdatePosition' with errorbars linestyle 27,\
'p3me_mesh_20040406.dat' index 27 using 1:2:4:5 title 'TS_from_UDF_UpdateVelocity' with errorbars linestyle 28,\
'p3me_mesh_20040406.dat' index 28 using 1:2:4:5 title 'TS_from_UDF_WaterSPCRattle' with errorbars linestyle 29,\
'p3me_mesh_20040406.dat' index 29 using 1:2:4:5 title 'TS_from_UDF_WaterSPCShake' with errorbars linestyle 30,\
'p3me_mesh_20040406.dat' index 0 using 1:2 notitle with lines linestyle 1,\
'p3me_mesh_20040406.dat' index 1 using 1:2 notitle with lines linestyle 2,\
'p3me_mesh_20040406.dat' index 2 using 1:2 notitle with lines linestyle 3,\
'p3me_mesh_20040406.dat' index 3 using 1:2 notitle with lines linestyle 4,\
'p3me_mesh_20040406.dat' index 4 using 1:2 notitle with lines linestyle 5,\
'p3me_mesh_20040406.dat' index 5 using 1:2 notitle with lines linestyle 6,\
'p3me_mesh_20040406.dat' index 6 using 1:2 notitle with lines linestyle 7,\
'p3me_mesh_20040406.dat' index 7 using 1:2 notitle with lines linestyle 8,\
'p3me_mesh_20040406.dat' index 8 using 1:2 notitle with lines linestyle 9,\
'p3me_mesh_20040406.dat' index 9 using 1:2 notitle with lines linestyle 10,\
'p3me_mesh_20040406.dat' index 10 using 1:2 notitle with lines linestyle 11,\
'p3me_mesh_20040406.dat' index 11 using 1:2 notitle with lines linestyle 12,\
'p3me_mesh_20040406.dat' index 12 using 1:2 notitle with lines linestyle 13,\
'p3me_mesh_20040406.dat' index 13 using 1:2 notitle with lines linestyle 14,\
'p3me_mesh_20040406.dat' index 14 using 1:2 notitle with lines linestyle 15,\
'p3me_mesh_20040406.dat' index 15 using 1:2 notitle with lines linestyle 16,\
'p3me_mesh_20040406.dat' index 16 using 1:2 notitle with lines linestyle 17,\
'p3me_mesh_20040406.dat' index 17 using 1:2 notitle with lines linestyle 18,\
'p3me_mesh_20040406.dat' index 18 using 1:2 notitle with lines linestyle 19,\
'p3me_mesh_20040406.dat' index 19 using 1:2 notitle with lines linestyle 20,\
'p3me_mesh_20040406.dat' index 20 using 1:2 notitle with lines linestyle 21,\
'p3me_mesh_20040406.dat' index 21 using 1:2 notitle with lines linestyle 22,\
'p3me_mesh_20040406.dat' index 22 using 1:2 notitle with lines linestyle 23,\
'p3me_mesh_20040406.dat' index 23 using 1:2 notitle with lines linestyle 24,\
'p3me_mesh_20040406.dat' index 24 using 1:2 notitle with lines linestyle 25,\
'p3me_mesh_20040406.dat' index 25 using 1:2 notitle with lines linestyle 26,\
'p3me_mesh_20040406.dat' index 26 using 1:2 notitle with lines linestyle 27,\
'p3me_mesh_20040406.dat' index 27 using 1:2 notitle with lines linestyle 28,\
'p3me_mesh_20040406.dat' index 28 using 1:2 notitle with lines linestyle 29,\
'p3me_mesh_20040406.dat' index 29 using 1:2 notitle with lines linestyle 30
