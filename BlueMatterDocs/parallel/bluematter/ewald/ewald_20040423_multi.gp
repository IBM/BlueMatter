# Statistics for grouped trace points from trace_group_id 14
reset
set logscale xy
set key outside
set linestyle 1
set linestyle 2
set linestyle 3
set xlabel 'Node Count'
set ylabel 'Elapsed Time (pclocks)'
plot 'ewald_20040423_multi.dat' index 0 using 1:2:4:5 title 'Ewald_AllReduce' with errorbars linestyle 1,\
'ewald_20040423_multi.dat' index 1 using 1:2:4:5 title 'Ewald_Forces' with errorbars linestyle 2,\
'ewald_20040423_multi.dat' index 2 using 1:2:4:5 title 'Ewald_LocalReduce' with errorbars linestyle 3,\
'ewald_20040423_multi.dat' index 0 using 1:2 notitle with lines linestyle 1,\
'ewald_20040423_multi.dat' index 1 using 1:2 notitle with lines linestyle 2,\
'ewald_20040423_multi.dat' index 2 using 1:2 notitle with lines linestyle 3
