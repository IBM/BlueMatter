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
plot 'ewald_tree_5k_20040719_multi.dat' index 11 using 1:2:4:5 title 'Total Time' with errorbars linestyle 12,\
'ewald_tree_5k_20040719_multi.dat' index 8 using 1:2:4:5 title 'Non-bond (RS)' with errorbars linestyle 9,\
'ewald_tree_5k_20040719_multi.dat' index 7 using 1:2:4:5 title 'Non-bond (KS)' with errorbars linestyle 8,\
'ewald_tree_5k_20040719_multi.dat' index 12 using 1:2:4:5 title 'ReduceForces' with errorbars linestyle 13,\
'ewald_tree_5k_20040719_multi.dat' index 6 using 1:2:4:5 title 'GlobalizePositions' with errorbars linestyle 7,\
'ewald_tree_5k_20040719_multi.dat' index 9 using 1:2:4:5 title 'Rattle' with errorbars linestyle 10,\
'ewald_tree_5k_20040719_multi.dat' index 10 using 1:2:4:5 title 'Shake' with errorbars linestyle 11,\
'ewald_tree_5k_20040719_multi.dat' index 18 using 1:2:4:5 title 'WaterSPCRattle' with errorbars linestyle 19,\
'ewald_tree_5k_20040719_multi.dat' index 19 using 1:2:4:5 title 'WaterSPCShake' with errorbars linestyle 20,\
'ewald_tree_5k_20040719_multi.dat' index 15 using 1:2:4:5 title 'UpdatePosition' with errorbars linestyle 16,\
'ewald_tree_5k_20040719_multi.dat' index 16 using 1:2:4:5 title 'UpdateVelocity' with errorbars linestyle 17,\
'ewald_tree_5k_20040719_multi.dat' index 14 using 1:2:4:5 title 'Bonded Forces' with errorbars linestyle 15,\
'ewald_tree_5k_20040719_multi.dat' index 6 using 1:2 notitle with lines linestyle 7,\
'ewald_tree_5k_20040719_multi.dat' index 7 using 1:2 notitle with lines linestyle 8,\
'ewald_tree_5k_20040719_multi.dat' index 8 using 1:2 notitle with lines linestyle 9,\
'ewald_tree_5k_20040719_multi.dat' index 9 using 1:2 notitle with lines linestyle 10,\
'ewald_tree_5k_20040719_multi.dat' index 10 using 1:2 notitle with lines linestyle 11,\
'ewald_tree_5k_20040719_multi.dat' index 11 using 1:2 notitle with lines linestyle 12,\
'ewald_tree_5k_20040719_multi.dat' index 12 using 1:2 notitle with lines linestyle 13,\
'ewald_tree_5k_20040719_multi.dat' index 14 using 1:2 notitle with lines linestyle 15,\
'ewald_tree_5k_20040719_multi.dat' index 15 using 1:2 notitle with lines linestyle 16,\
'ewald_tree_5k_20040719_multi.dat' index 16 using 1:2 notitle with lines linestyle 17,\
'ewald_tree_5k_20040719_multi.dat' index 19 using 1:2 notitle with lines linestyle 20
