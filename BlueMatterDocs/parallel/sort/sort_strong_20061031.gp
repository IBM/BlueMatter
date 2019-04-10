reset
set bmargin 4
set xlabel 'Node Count'
set ylabel 'Sort Time (sec.)'
set logscale x
set xtics 2048, 2, 32768
set yrange [0:]
set xrange [2048:32768]
set key bottom left
plot 'sort_20061031.dat' index 0 using 1:2 with linespoints title '1 TeraByte', \
'sort_20061031.dat' index 1 using 1:2 with linespoints title '2 TeraByte',\
'sort_20061031.dat' index 2 using 1:2 with linespoints title '3 TeraByte',\
'sort_20061031.dat' index 3 using 1:2 with linespoints title '4 TeraByte'
