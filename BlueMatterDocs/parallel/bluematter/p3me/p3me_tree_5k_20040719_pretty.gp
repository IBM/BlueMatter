# Statistics for grouped trace points from trace_group_id 76
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
set xlabel 'Node Count'
set ylabel 'Elapsed Time (nanoseconds)'
plot 'p3me_tree_5k_20040719.dat' index 5 using 1:2:4:5 title 'Total Time' with errorbars linestyle 6,\
'p3me_tree_5k_20040719.dat' index 2 using 1:2:4:5 title 'Non-bond (RS)' with errorbars linestyle 3,\
'p3me_tree_5k_20040719.dat' index 1 using 1:2:4:5 title 'Non-bond (KS)' with errorbars linestyle 2,\
'p3me_tree_5k_20040719.dat' index 11 using 1:2:4:5 title 'ReduceForces' with errorbars linestyle 12,\
'p3me_tree_5k_20040719.dat' index 0 using 1:2:4:5 title 'GlobalizePositions' with errorbars linestyle 1,\
'p3me_tree_5k_20040719.dat' index 3 using 1:2:4:5 title 'Rattle' with errorbars linestyle 4,\
'p3me_tree_5k_20040719.dat' index 4 using 1:2:4:5 title 'Shake' with errorbars linestyle 5,\
'p3me_tree_5k_20040719.dat' index 17 using 1:2:4:5 title 'WaterSPCRattle' with errorbars linestyle 18,\
'p3me_tree_5k_20040719.dat' index 18 using 1:2:4:5 title 'WaterSPCShake' with errorbars linestyle 19,\
'p3me_tree_5k_20040719.dat' index 14 using 1:2:4:5 title 'UpdatePosition' with errorbars linestyle 15,\
'p3me_tree_5k_20040719.dat' index 15 using 1:2:4:5 title 'UpdateVelocity' with errorbars linestyle 16,\
'p3me_tree_5k_20040719.dat' index 13 using 1:2:4:5 title 'Bonded Forces' with errorbars linestyle 14,\
'p3me_tree_5k_20040719.dat' index 0 using 1:2 notitle with lines linestyle 1,\
'p3me_tree_5k_20040719.dat' index 1 using 1:2 notitle with lines linestyle 2,\
'p3me_tree_5k_20040719.dat' index 2 using 1:2 notitle with lines linestyle 3,\
'p3me_tree_5k_20040719.dat' index 4 using 1:2 notitle with lines linestyle 5,\
'p3me_tree_5k_20040719.dat' index 5 using 1:2 notitle with lines linestyle 6,\
'p3me_tree_5k_20040719.dat' index 11 using 1:2 notitle with lines linestyle 12,\
'p3me_tree_5k_20040719.dat' index 13 using 1:2 notitle with lines linestyle 14,\
'p3me_tree_5k_20040719.dat' index 14 using 1:2 notitle with lines linestyle 15,\
'p3me_tree_5k_20040719.dat' index 15 using 1:2 notitle with lines linestyle 16,\
'p3me_tree_5k_20040719.dat' index 17 using 1:2 notitle with lines linestyle 18,\
'p3me_tree_5k_20040719.dat' index 18 using 1:2 notitle with lines linestyle 19
