# Statistics for grouped trace points from trace_group_id 77
reset
# set size when using metapost output
# set size 1.3, 1.3
set logscale xy
set key top right
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
set xlabel 'Node Count'
set ylabel 'Elapsed Time (nanoseconds)'
plot 'ewald_tree_5k_20040719_multi.dat' index 7 using 1:2:4:5 title 'Non-bond (KS)' with errorbars linestyle 8,\
'ewald_tree_5k_20040719_multi.dat' index 0 using 1:2:4:5 title 'AllReduce S(k)' with errorbars linestyle 1,\
'ewald_tree_5k_20040719_multi.dat' index 2 using 1:2:4:5 title 'Compute Forces' with errorbars linestyle 3,\
'ewald_tree_5k_20040719_multi.dat' index 4 using 1:2:4:5 title 'LocalReduce S(k)' with errorbars linestyle 5,\
'ewald_tree_5k_20040719_multi.dat' index 1 using 1:2:4:5 title 'Exp. Factors' with errorbars linestyle 2,\
'ewald_tree_5k_20040719_multi.dat' index 0 using 1:2 notitle with lines linestyle 1,\
'ewald_tree_5k_20040719_multi.dat' index 1 using 1:2 notitle with lines linestyle 2,\
'ewald_tree_5k_20040719_multi.dat' index 2 using 1:2 notitle with lines linestyle 3,\
'ewald_tree_5k_20040719_multi.dat' index 4 using 1:2 notitle with lines linestyle 5,\
'ewald_tree_5k_20040719_multi.dat' index 7 using 1:2 notitle with lines linestyle 8

