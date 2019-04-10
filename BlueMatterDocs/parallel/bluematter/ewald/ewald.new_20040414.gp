# Statistics for trace points from trace_group_id 2
reset
set size 1.6, 1.2
set logscale xy
# set yrange [1000:1e10]
set key 5, 1e9
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
plot 'ewald.new.dat' index 0 using 1:2:4:5 title 'Ewald\_AllReduce' with errorbars linestyle 1,\
'ewald.new.dat' index 1 using 1:2:4:5 title 'Ewald\_Eiks' with errorbars linestyle 2,\
'ewald.new.dat' index 2 using 1:2:4:5 title 'Ewald\_Forces' with errorbars linestyle 3,\
'ewald.new.dat' index 3 using 1:2:4:5 title 'Ewald\_KSpace' with errorbars linestyle 4,\
'ewald.new.dat' index 4 using 1:2:4:5 title 'Ewald\_LocalReduce' with errorbars linestyle 5,\
'ewald.new.dat' index 5 using 1:2:4:5 title 'Ewald\_Loop' with errorbars linestyle 6,\
'ewald.new.dat' index 6 using 1:2:4:5 title 'GlobalizePositions\_Cntl' with errorbars linestyle 7,\
'ewald.new.dat' index 7 using 1:2:4:5 title 'P2\_KSpace\_Control' with errorbars linestyle 19,\
'ewald.new.dat' index 8 using 1:2:4:5 title 'P2\_NSQ\_Control' with errorbars linestyle 20,\
'ewald.new.dat' index 9 using 1:2:4:5 title 'ReduceForces\_Cntl' with errorbars linestyle 10,\
'ewald.new.dat' index 11 using 1:2:4:5 title 'StdHarmonicBondForce' with errorbars linestyle 12,\
'ewald.new.dat' index 14 using 1:2:4:5 title 'WaterSPCRattle' with errorbars linestyle 11,\
'ewald.new.dat' index 15 using 1:2:4:5 title 'WaterSPCShake' with errorbars linestyle 16,\
'ewald.new.dat' index 17 using 1:2:4:5 title 'TS\_from\_Ewald\_Eiks' with errorbars linestyle 18,\
'ewald.new.dat' index 0 using 1:2 notitle with lines linestyle 1,\
'ewald.new.dat' index 1 using 1:2 notitle with lines linestyle 2,\
'ewald.new.dat' index 2 using 1:2 notitle with lines linestyle 3,\
'ewald.new.dat' index 3 using 1:2 notitle with lines linestyle 4,\
'ewald.new.dat' index 4 using 1:2 notitle with lines linestyle 5,\
'ewald.new.dat' index 5 using 1:2 notitle with lines linestyle 6,\
'ewald.new.dat' index 6 using 1:2 notitle with lines linestyle 7,\
'ewald.new.dat' index 7 using 1:2 notitle with lines linestyle 19,\
'ewald.new.dat' index 8 using 1:2 notitle with lines linestyle 20,\
'ewald.new.dat' index 9 using 1:2 notitle with lines linestyle 10,\
'ewald.new.dat' index 11 using 1:2 notitle with lines linestyle 12,\
'ewald.new.dat' index 14 using 1:2 notitle with lines linestyle 11,\
'ewald.new.dat' index 15 using 1:2 notitle with lines linestyle 16,\
'ewald.new.dat' index 17 using 1:2 notitle with lines linestyle 18
