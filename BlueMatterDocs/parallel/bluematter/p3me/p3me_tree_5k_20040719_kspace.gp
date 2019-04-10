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
plot 'p3me_tree_5k_20040719.dat' index 1 using 1:2:4:5 title 'Non-bond (KS)' with errorbars linestyle 2,\
'p3me_tree_5k_20040719.dat' index 8 using 1:2:4:5 title 'Forward FFT' with errorbars linestyle 9,\
'p3me_tree_5k_20040719.dat' index 10 using 1:2:4:5 title 'Inverse FFT' with errorbars linestyle 11,\
'p3me_tree_5k_20040719.dat' index 6 using 1:2:4:5 title 'P3ME Assign Charge' with errorbars linestyle 7,\
'p3me_tree_5k_20040719.dat' index 7 using 1:2:4:5 title 'P3ME Convolve' with errorbars linestyle 8,\
'p3me_tree_5k_20040719.dat' index 9 using 1:2:4:5 title 'P3ME Interpolate' with errorbars linestyle 10,\
'p3me_tree_5k_20040719.dat' index 1 using 1:2 notitle with lines linestyle 2,\
'p3me_tree_5k_20040719.dat' index 6 using 1:2 notitle with lines linestyle 7,\
'p3me_tree_5k_20040719.dat' index 7 using 1:2 notitle with lines linestyle 8,\
'p3me_tree_5k_20040719.dat' index 8 using 1:2 notitle with lines linestyle 9,\
'p3me_tree_5k_20040719.dat' index 9 using 1:2 notitle with lines linestyle 10,\
'p3me_tree_5k_20040719.dat' index 10 using 1:2 notitle with lines linestyle 11
