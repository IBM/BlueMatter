reset
#set term mp color psnfss
#set output 'rhod.mp'
set xlabel 'Node Count'
set ylabel 'Elapsed Time/Time-step (seconds)'
set xrange [100:]
set logscale xy
set linestyle 1
set linestyle 2
set linestyle 3
set linestyle 4
plot 'AlexBld.060220.Float.rhod.p4.128x128x128.spi.dat' index 4 using 2:($3/1e9) title 'Active ORB' with linespoints,\
'AlexBld.060220.Float.EOrb.rhod.p4.128x128x128.spi.dat' index 4 using 2:($3/1e9) title 'Even ORB' with linespoints
