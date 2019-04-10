reset
set logscale xy
set xlabel "Node Count"
set ylabel "Time/Time-step (Seconds)"
plot '../benchmark_20060411/rhod.p5.float.spi.dat' index 18 using 2:($3/1e9) title 'Total Time (P5)' with linespoints,\
'../benchmark_20060221/AlexBld.060220.Float.EOrb.rhod.p4.128x128x128.spi.dat' index 4 using 2:($3/1e9) title 'Total Time (P4)' with linespoints,\
'../benchmark_20060411/rhod.p5.float.spi.dat' index 3 using 2:($3/1e9) title 'K-space (P5)' with linespoints,\
'../benchmark_20060221/AlexBld.060220.Float.EOrb.rhod.p4.128x128x128.spi.dat' index 3 using 2:($3/1e9) title 'K-space (P4)' with linespoints,\
'../benchmark_20060411/rhod.p5.float.spi.dat' index 15 using 2:($3/1e9) title 'Throbber Bcast (P5)' with linespoints,\
'../benchmark_20060221/AlexBld.060220.Float.EOrb.rhod.p4.128x128x128.spi.dat' index 9 using 2:($3/1e9) title 'Throbber Bcast (P4)' with linespoints,\
'../benchmark_20060411/rhod.p5.float.spi.dat' index 16 using 2:($3/1e9) title 'Throbber Reduce (P5)' with linespoints,\
'../benchmark_20060221/AlexBld.060220.Float.EOrb.rhod.p4.128x128x128.spi.dat' index 10 using 2:($3/1e9) title 'Throbber Reduce (P4)' with linespoints
