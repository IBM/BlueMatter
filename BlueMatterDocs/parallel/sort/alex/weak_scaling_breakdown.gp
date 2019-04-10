# Statistics for trace points from trace_group_id 327
reset
set title "Sort Breakdown"
set size 0.7,1
dataPerNode=0.386
set logscale xy 4
set xrange [16:8192]
set xtics nomirror
set x2tics nomirror
set x2range [16*dataPerNode:8192*dataPerNode]
set key 3600, 15
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
set xlabel 'Node Count'
set x2label 'SortSize (GB)'
set ylabel 'Elapsed Time (Seconds)'
plot 'sort_stats.dat' index 0 using 1:($2/700000000):($4/700000000):($5/700000000) title 'Histogram' with errorbars linestyle 1,\
'sort_stats.dat' index 2 using 1:($2/700000000):($4/700000000):($5/700000000) title 'Redistribution' with errorbars linestyle 3,\
'sort_stats.dat' index 3 using 1:($2/700000000):($4/700000000):($5/700000000) title 'Redistribution Comm.' with errorbars linestyle 4,\
'sort_stats.dat' index 4 using 1:($2/700000000):($4/700000000):($5/700000000) title 'Sort' with errorbars linestyle 5,\
'sort_stats.dat' index 2 using 1:($2/700000000) notitle with lines linestyle 3,\
'sort_stats.dat' index 3 using 1:($2/700000000) notitle with lines linestyle 4,\
'sort_stats.dat' index 4 using 1:($2/700000000) notitle with lines linestyle 5, \
'sort_stats.dat' index 0 using 1:($2/700000000) notitle with lines linestyle 1
