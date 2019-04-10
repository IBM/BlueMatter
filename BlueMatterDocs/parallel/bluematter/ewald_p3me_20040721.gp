# comparison of p3me and ewald scaling data
#
reset
set xlabel 'Node Count'
set ylabel 'Elapsed Time (nanoseconds)'
set logscale xy
set linestyle 1
set linestyle 2
set linestyle 3
set linestyle 4
plot 'ewald/ewald_tree_43k_20040720.dat' index 26 using 1:2:4:5 title 'Ewald 43k' with errorbars linestyle 1,\
'ewald/ewald_tree_43k_20040720.dat' index 26 using 1:2 notitle with lines linestyle 1,\
'p3me/p3me_tree_43k_20040715.dat' index 5 using 1:2:4:5 title 'P3ME 43k' with errorbars linestyle 2,\
'p3me/p3me_tree_43k_20040715.dat' index 5 using 1:2 notitle with lines linestyle 2,\
 './ewald/ewald_tree_5k_20040719_multi.dat' index 11 using 1:2:4:5 title 'Ewald 5k' with errorbars linestyle 3,\
'./ewald/ewald_tree_5k_20040719_multi.dat' index 11 using 1:2 notitle with lines linestyle 3,\
'./p3me/p3me_tree_5k_20040719.dat' index 5 using 1:2:4:5 title 'P3ME 5k' with errorbars linestyle 4,\
'./p3me/p3me_tree_5k_20040719.dat' index 5 using 1:2 notitle with lines linestyle 4

