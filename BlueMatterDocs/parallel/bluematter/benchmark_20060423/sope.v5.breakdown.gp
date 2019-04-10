reset
set logscale xy
set xlabel "Node Count"
set ylabel "Time/Time-step (Seconds)"
plot 'sope.p5.float.spi.dat' index 18 using 2:($3/1e9) title 'Total Time (V5)' with linespoints,\
'sope.p5.float.spi.dat' index 18 using 2:($3/1e9) title 'Total Time (V5)' with linespoints,\