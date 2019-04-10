# Statistics for trace points from trace_group_id 7
reset
set size 1.4, 1.2
set logscale xy
#set key outside
set key 15, 7e8
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
set xlabel 'Node Count'
set ylabel 'Elapsed Time (pclocks)'
plot 'ewald.dat' index 0 using 1:2:4:5 title 'Ewald\_AllReduce' with errorbars,\
'ewald.dat' index 1 using 1:2:4:5 title 'Ewald\_Eiks' with errorbars,\
'ewald.dat' index 2 using 1:2:4:5 title 'Ewald\_KSpace' with errorbars,\
'ewald.dat' index 3 using 1:2:4:5 title 'Ewald\_Loop' with errorbars,\
'ewald.dat' index 4 using 1:2:4:5 title 'GlobalizePositions\_Cntl' with errorbars,\
'ewald.dat' index 5 using 1:2:4:5 title 'P2\_KSpace\_Control' with errorbars,\
'ewald.dat' index 6 using 1:2:4:5 title 'P2\_NSQ\_Control' with errorbars,\
'ewald.dat' index 7 using 1:2:4:5 title 'ReduceForces\_Cntl' with errorbars,\
'ewald.dat' index 8 using 1:2:4:5 title 'UDF\_KineticEnergy' with errorbars linestyle 19,\
'ewald.dat' index 9 using 1:2:4:5 title 'UDF\_StdHarmonicBondForce' with errorbars,\
'ewald.dat' index 10 using 1:2:4:5 title 'UDF\_UpdatePosition' with errorbars,\
'ewald.dat' index 11 using 1:2:4:5 title 'UDF\_UpdateVelocity' with errorbars,\
'ewald.dat' index 12 using 1:2:4:5 title 'UDF\_WaterSPCRattle' with errorbars,\
'ewald.dat' index 13 using 1:2:4:5 title 'UDF\_WaterSPCShake' with errorbars,\
'ewald.dat' index 18 using 1:2:4:5 title 'TS\_from\_GlobalizePositions\_Cntl' with errorbars,\
'ewald.dat' index 0 using 1:2 notitle with lines linestyle 1,\
'ewald.dat' index 1 using 1:2 notitle with lines linestyle 2,\
'ewald.dat' index 2 using 1:2 notitle with lines linestyle 3,\
'ewald.dat' index 3 using 1:2 notitle with lines linestyle 4,\
'ewald.dat' index 4 using 1:2 notitle with lines linestyle 5,\
'ewald.dat' index 5 using 1:2 notitle with lines linestyle 6,\
'ewald.dat' index 6 using 1:2 notitle with lines linestyle 7,\
'ewald.dat' index 7 using 1:2 notitle with lines linestyle 8,\
'ewald.dat' index 8 using 1:2 notitle with lines linestyle 19,\
'ewald.dat' index 9 using 1:2 notitle with lines linestyle 10,\
'ewald.dat' index 10 using 1:2 notitle with lines linestyle 11,\
'ewald.dat' index 11 using 1:2 notitle with lines linestyle 12,\
'ewald.dat' index 12 using 1:2 notitle with lines linestyle 13,\
'ewald.dat' index 13 using 1:2 notitle with lines linestyle 14,\
'ewald.dat' index 18 using 1:2 notitle with lines linestyle 16
