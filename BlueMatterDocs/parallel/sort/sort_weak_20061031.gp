reset
dataPerNode=0.22
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
set key bottom left
plot 'sort_20061031.dat' index 4 using 1:2:3:2 with errorlines title '220MB/node', \
'sort_20061031.dat' index 5 using 1:2 with linespoints title '220MB/node Sweep'
