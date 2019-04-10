reset
#set size 0.7, 0.9
set bmargin 5
set xlabel 'Sort Size ( TB )'
set ylabel 'Sort Time (sec.)'
#set logscale x
#set xtics 2048, 2, 32768
set yrange [0:120]
set xrange [:]
set key top right
#'sort_110706.data' index 8 using 1:3 with linespoint title '16K Nodes'
set title 'Strong Scaling vs. Sort Size'
plot 'sort_110706.data' index 8 using 1:2 with linespoint title '20K Nodes'
