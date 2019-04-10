reset
# set key bottom left
set key 35, 7e5
set xlabel "Node Count"
set ylabel "Time (pclocks)"
set logscale xy
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

plot 'scalability2_20040406.dat' index 0 using 1:2:4:5 title 'AssignCharge' with yerrorbars,\
'scalability2_20040406.dat' index 1 using 1:2:4:5 title 'ForwardFFT' with yerrorbars,\
'scalability2_20040406.dat' index 2 using 1:2:4:5 title 'Interpolate' with yerrorbars,\
'scalability2_20040406.dat' index 3 using 1:2:4:5 title 'KSpaceEvaluation' with yerrorbars,\
'scalability2_20040406.dat' index 4 using 1:2:4:5 title 'mGlobalizePositions' with yerrorbars,\
'scalability2_20040406.dat' index 5 using 1:2:4:5 title 'mReduceForces' with yerrorbars,\
'scalability2_20040406.dat' index 6 using 1:2:4:5 title 'NSQ_RealSpace_Meat_' with yerrorbars,\
'scalability2_20040406.dat' index 7 using 1:2:4:5 title 'P3ME_Convolve_' with yerrorbars,\
'scalability2_20040406.dat' index 8 using 1:2:4:5 title 'P3ME_ReverseFFT_' with yerrorbars,\
'scalability2_20040406.dat' index 9 using 1:2:4:5 title 'RealSpace' with yerrorbars,\
'scalability2_20040406.dat' index 0 using 1:2 notitle with lines linestyle 1,\
'scalability2_20040406.dat' index 1 using 1:2 notitle with lines linestyle 2,\
'scalability2_20040406.dat' index 2 using 1:2 notitle with lines linestyle 3,\
'scalability2_20040406.dat' index 3 using 1:2 notitle with lines linestyle 4,\
'scalability2_20040406.dat' index 4 using 1:2 notitle with lines linestyle 5,\
'scalability2_20040406.dat' index 5 using 1:2 notitle with lines linestyle 6,\
'scalability2_20040406.dat' index 6 using 1:2 notitle with lines linestyle 7,\
'scalability2_20040406.dat' index 7 using 1:2 notitle with lines linestyle 8,\
'scalability2_20040406.dat' index 8 using 1:2 notitle with lines linestyle 9,\
'scalability2_20040406.dat' index 9 using 1:2 notitle with lines linestyle 10

