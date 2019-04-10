# Statistics for trace points from trace_group_id 12
reset
set size 1.2, 1.2
ns2pclk=0.7
set logscale xy
set xrange [1:1000]
set key 15,1.5e7
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
set xlabel 'Node Count'
set ylabel 'Elapsed Time (pclocks)'
plot 'p3me_mesh_20040406.dat' index 15 using 1:(ns2pclk*$2):(ns2pclk*$4):(ns2pclk*$5) title 'Total Time Step' with errorbars linestyle 1,\
'p3me_mesh_20040406.dat' index 0 using 1:(ns2pclk*$2):(ns2pclk*$4):(ns2pclk*$5) title 'GlobalizePositions\_Cntl' with errorbars linestyle 19,\
'p3me_mesh_20040406.dat' index 1 using 1:(ns2pclk*$2):(ns2pclk*$4):(ns2pclk*$5) title 'P2\_KSpace\_Control' with errorbars linestyle 2,\
'p3me_mesh_20040406.dat' index 2 using 1:(ns2pclk*$2):(ns2pclk*$4):(ns2pclk*$5) title 'P2\_NSQ\_Control' with errorbars linestyle 3,\
'p3me_mesh_20040406.dat' index 3 using 1:(ns2pclk*$2):(ns2pclk*$4):(ns2pclk*$5) title 'P3ME\_AssignCharge\_Control' with errorbars linestyle 4,\
'p3me_mesh_20040406.dat' index 4 using 1:(ns2pclk*$2):(ns2pclk*$4):(ns2pclk*$5) title 'P3ME\_Convolve' with errorbars linestyle 5,\
'p3me_mesh_20040406.dat' index 5 using 1:(ns2pclk*$2):(ns2pclk*$4):(ns2pclk*$5) title 'P3ME\_ForwardFFT\_Control' with errorbars linestyle 6,\
'p3me_mesh_20040406.dat' index 6 using 1:(ns2pclk*$2):(ns2pclk*$4):(ns2pclk*$5) title 'P3ME\_Interpolate\_Control' with errorbars linestyle 7,\
'p3me_mesh_20040406.dat' index 7 using 1:(ns2pclk*$2):(ns2pclk*$4):(ns2pclk*$5) title 'P3ME\_Rev\_FFT' with errorbars linestyle 8,\
'p3me_mesh_20040406.dat' index 8 using 1:(ns2pclk*$2):(ns2pclk*$4):(ns2pclk*$5) title 'ReduceForces\_Cntl' with errorbars linestyle 10,\
'p3me_mesh_20040406.dat' index 10 using 1:(ns2pclk*$2):(ns2pclk*$4):(ns2pclk*$5) title 'UDF\_StdHarmonicBondForce' with errorbars linestyle 11,\
'p3me_mesh_20040406.dat' index 15 using 1:(ns2pclk*$2) notitle with lines linestyle 1,\
'p3me_mesh_20040406.dat' index 0 using 1:(ns2pclk*$2) notitle with lines linestyle 19,\
'p3me_mesh_20040406.dat' index 1 using 1:(ns2pclk*$2) notitle with lines linestyle 2,\
'p3me_mesh_20040406.dat' index 2 using 1:(ns2pclk*$2) notitle with lines linestyle 3,\
'p3me_mesh_20040406.dat' index 3 using 1:(ns2pclk*$2) notitle with lines linestyle 4,\
'p3me_mesh_20040406.dat' index 4 using 1:(ns2pclk*$2) notitle with lines linestyle 5,\
'p3me_mesh_20040406.dat' index 5 using 1:(ns2pclk*$2) notitle with lines linestyle 6,\
'p3me_mesh_20040406.dat' index 6 using 1:(ns2pclk*$2) notitle with lines linestyle 7,\
'p3me_mesh_20040406.dat' index 7 using 1:(ns2pclk*$2) notitle with lines linestyle 8,\
'p3me_mesh_20040406.dat' index 8 using 1:(ns2pclk*$2) notitle with lines linestyle 10,\
'p3me_mesh_20040406.dat' index 10 using 1:(ns2pclk*$2) notitle with lines linestyle 11
