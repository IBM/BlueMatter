reset
#set term mp color psnfss
#set output 'rhod_breakdown.mp'
#set title '43K Atom Rhodopsin'
set xlabel 'Node Count'
set ylabel 'Elapsed Time/Time-step (seconds)'
set xrange [100:]
set logscale xy
set linestyle 1
set linestyle 2
set linestyle 3
set linestyle 4
plot 'AlexBld.060220.Float.EOrb.rhod.p4.128x128x128.spi.dat' index 4 using 2:($3/1e9) title 'Total Time' with linespoints,\
'AlexBld.060220.Float.EOrb.rhod.p4.128x128x128.spi.dat' index 3 using 2:($3/1e9) title 'K-space' with linespoints,\
'../rhod.RELEASE_CANDIDATE_012506.mpi.sc.dat' index 1 using 2:($3/1e9) title 'Real Space' with linespoints,\
'AlexBld.060220.Float.EOrb.rhod.p4.128x128x128.spi.dat' index 9 using 2:($3/1e9) title 'Neighborhood Broadcast' with linespoints,\
'AlexBld.060220.Float.EOrb.rhod.p4.128x128x128.spi.dat' index 10 using 2:($3/1e9) title 'Neighborhood Reduce' with linespoints
#set output
set term win
