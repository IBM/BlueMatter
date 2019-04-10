reset
#set term mp color psnfss
#set output 'apoa1_breakdown.mp'
#set title '92K Atom ApoA1 Components'
set xlabel 'Node Count'
set ylabel 'Elapsed Time/Time-step (seconds)'
set xrange [100:]
#set size 1.2,1.2
set logscale xy
set linestyle 1
set linestyle 2
set linestyle 3
set linestyle 4
plot 'apoa1.spi_dcc.dc1.spfft.20060215.dat' index 4 using 2:($3/1e9) title 'Total Time P3ME 1:1' with linespoints,\
'apoa1_respa.spi_dcc.dc1.spfft.20060215.dat' index 4 using 2:($3/1e9) title 'Total Time P3ME 1:4' with linespoints,\
'apoa1.spi_dcc.dc1.spfft.20060215.dat' index 3 using 2:($3/1e9) title 'K-space P3ME 1:1' with linespoints,\
'apoa1_respa.spi_dcc.dc1.spfft.20060215.dat' index 3 using 2:($3/1e9) title 'K-space P3ME 1:4' with linespoints,\
'../apoa.RELEASE_CANDIDATE_012506.mpi.sc.dat' index 1 using 2:($3/1e9) title 'Real Space (MPI SC)' with linespoints,\
'apoa1.spi_dcc.dc1.spfft.20060215.dat' index 9 using 2:($3/1e9) title 'Neighborhood Bcast' with linespoints,\
'apoa1.spi_dcc.dc1.spfft.20060215.dat' index 10 using 2:($3/1e9) title 'Neighborhood Reduce' with linespoints
#set output
set term win
