reset
set size 0.7,1
dataPerNode=0.386
set xlabel 'Node Count'
set x2label 'Sort Size (GB)'
set ylabel 'Sort Time (sec.)'
set logscale xx2 2
set xrange [16:32768]
set yrange [0:]
#set xtics nomirror 16, 2, 32768
set x2range [16*dataPerNode:32768*dataPerNode]
set xtics nomirror
set x2tics nomirror
#set x2tics nomirror 2
set key bottom right
#plot 'sort_110706.data' index 0 using 1:3:2:3 with errorlines title '386MB/node'
#plot 'sort_110706.data' index 0 using 1:(($3+$2)/2) with linespoint title '386MB/node'
plot 'sort_110706.data' index 0 using 1:3 with linespoint title '386MB/node'
