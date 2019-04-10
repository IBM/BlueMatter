reset
set logscale xy
set xlabel "Node Count"
set ylabel "Time/Time-step (Seconds)"
plot '../benchmark_20060411/apoa.p5.float.spi.dat' index 18 using 2:($3/1e9) title 'Total Time (P5)' with linespoints,\
'../benchmark_20060215/apoa1.spi_dcc.dc1.spfft.20060215.dat' index 4 using 2:($3/1e9) title 'Total Time (P4)' with linespoints,\
'../benchmark_20060411/apoa.p5.float.spi.dat' index 3 using 2:($3/1e9) title 'K-space (P5)' with linespoints,\
'../benchmark_20060215/apoa1.spi_dcc.dc1.spfft.20060215.dat' index 3 using 2:($3/1e9) title 'K-Space (P4)' with linespoints,\
'../benchmark_20060411/apoa.p5.float.spi.dat' index 15 using 2:($3/1e9) title 'Throbber Bcast (P5)' with linespoints,\
'../benchmark_20060215/apoa1.spi_dcc.dc1.spfft.20060215.dat' index 9 using 2:($3/1e9) title 'Throbber Bcast (P4)' with linespoints,\
'../benchmark_20060411/apoa.p5.float.spi.dat' index 16 using 2:($3/1e9) title 'Throbber Reduce (P5)' with linespoints,\
'../benchmark_20060215/apoa1.spi_dcc.dc1.spfft.20060215.dat' index 10 using 2:($3/1e9) title 'Throbber Reduce (P4)' with linespoints