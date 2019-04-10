# Statistics for trace points from trace_group_id 4
reset
set size 1.4, 1.2
set logscale xy
#set key bottom left
#set key bottom left left reverse
set key 15, 8e7
set xrange [1:1000]
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
plot 'p3me.dat' index 0 using 1:2:4:5 title 'GlobalizePositions\_Cntl' with errorbars linestyle 1,\
'p3me.dat' index 1 using 1:2:4:5 title 'P2\_KSpace\_Control' with errorbars linestyle 2,\
'p3me.dat' index 2 using 1:2:4:5 title 'P2\_NSQ\_Control' with errorbars linestyle 3,\
'p3me.dat' index 3 using 1:2:4:5 title 'P3ME\_AssignCharge\_Control' with errorbars linestyle 4,\
'p3me.dat' index 4 using 1:2:4:5 title 'P3ME\_Convolve' with errorbars linestyle 5,\
'p3me.dat' index 5 using 1:2:4:5 title 'P3ME\_ForwardFFT\_Control' with errorbars linestyle 6,\
'p3me.dat' index 6 using 1:2:4:5 title 'P3ME\_Interpolate\_Control' with errorbars linestyle 7,\
'p3me.dat' index 7 using 1:2:4:5 title 'P3ME\_Rev\_FFT' with errorbars linestyle 8,\
'p3me.dat' index 8 using 1:2:4:5 title 'ReduceForces\_Cntl' with errorbars linestyle 18,\
'p3me.dat' index 10 using 1:2:4:5 title 'StdHarmonicBondForce' with errorbars linestyle 11,\
'p3me.dat' index 11 using 1:2:4:5 title 'UDF\_UpdatePosition' with errorbars linestyle 12,\
'p3me.dat' index 12 using 1:2:4:5 title 'UDF\_UpdateVelocity' with errorbars linestyle 13,\
'p3me.dat' index 13 using 1:2:4:5 title 'UDF\_WaterSPCRattle' with errorbars linestyle 14,\
'p3me.dat' index 14 using 1:2:4:5 title 'UDF\_WaterSPCShake' with errorbars linestyle 15,\
'p3me.dat' index 15 using 1:2:4:5 title 'Total Time Step' with errorbars linestyle 16,\
'p3me.dat' index 0 using 1:2 notitle with lines linestyle 1,\
'p3me.dat' index 1 using 1:2 notitle with lines linestyle 2,\
'p3me.dat' index 2 using 1:2 notitle with lines linestyle 3,\
'p3me.dat' index 3 using 1:2 notitle with lines linestyle 4,\
'p3me.dat' index 4 using 1:2 notitle with lines linestyle 5,\
'p3me.dat' index 5 using 1:2 notitle with lines linestyle 6,\
'p3me.dat' index 6 using 1:2 notitle with lines linestyle 7,\
'p3me.dat' index 7 using 1:2 notitle with lines linestyle 8,\
'p3me.dat' index 8 using 1:2 notitle with lines linestyle 18,\
'p3me.dat' index 10 using 1:2 notitle with lines linestyle 11,\
'p3me.dat' index 11 using 1:2 notitle with lines linestyle 12,\
'p3me.dat' index 12 using 1:2 notitle with lines linestyle 13,\
'p3me.dat' index 13 using 1:2 notitle with lines linestyle 14,\
'p3me.dat' index 14 using 1:2 notitle with lines linestyle 15,\
'p3me.dat' index 15 using 1:2 notitle with lines linestyle 16
