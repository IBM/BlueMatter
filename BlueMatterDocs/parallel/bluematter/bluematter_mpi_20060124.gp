reset
#set term mp color psnfss
#set output 'bluematter_mpi_20060124.mp'
set xlabel 'Node/CPU Count'
set ylabel 'Elapsed Time/Time-step (seconds)'
set xrange [100:]
set logscale xy
set linestyle 1
set linestyle 2
set linestyle 3
set linestyle 4
plot 'rhod_RELEASE_CANDIDATE_011906_mpi.dat' index 3 using 2:($3/1e9) title '43K Rhodopsin (MPI, dc1 P3ME 1:1 1/2006)' with linespoints,\
'rhod.10a.norespa.dc.spi.20050724.dat' index 5 using 1:($2/1e9) title '43K Rhodopsin (BG/L ADE SPI, dc2 P3ME 1:1 7/2005)' with linespoints,\
'rhod.packet.sc_comms.dc1.20060131.dat' index 6 using 2:($3/1e9) title '43K Rhodopsin (BG/L ADE SPI sc comms, dc1 P3ME 1:1 1/2006)' with linespoints,\
'RhodPkt1CoreCommsNew_20060202.dat' index 6 using 2:($3/1e9) title '43K Rhodopsin (BG/L ADE SPI new sc comms, dc1 P3ME 1:1 2/2007' with linespoints

#'apoa.12a.respa.dc.spi.20050724.dat' index 5 using 1:($2/1e9) title '92K ApoA1 (SPI, dc2 P3ME 1:4 7/2005)' with linespoints,\
#'apoa1_RELEASE_CANDIDATE_011906_mpi.dat' index 3 using 2:($3/1e9) title '92K ApoA1 (MPI, dc1 P3ME 1:1 1/2006)' with linespoints,\
#'rhod.dc1.norespa.blade.sc_comms.20060117a.dat' index 3 using 2:($3/1e9) title '43K Rhodopsin (BG/L ADE SPI sc comms, dc1 P3ME 1:1 1/2006)' with linespoints
