reset
set title 'Strong Scaling vs. Node Count'
#set size 0.7,1
set bmargin 5
set xlabel 'Node Count'
set ylabel 'Sort Time (sec.)'
set logscale x
set xtics 4000, 2, 23000
set yrange [0:120]
set xrange [4000:23000]
set key top left
plot 'sort_110706.data' index 1 using 1:3 with linespoint title '1 TeraByte', \
'sort_110706.data' index 2 using 1:3 with linespoint title '2 TeraByte',\
'sort_110706.data' index 3 using 1:3 with linespoint title '3 TeraByte',\
'sort_110706.data' index 4 using 1:3 with linespoint title '4 TeraByte', \
'sort_110706.data' index 5 using 1:3 with linespoint title '5 TeraByte', \
'sort_110706.data' index 6 using 1:3 with linespoint title '6 TeraByte', \
'sort_110706.data' index 7 using 1:3 with linespoint title '7 TeraByte' 
