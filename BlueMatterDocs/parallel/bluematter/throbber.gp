reset
set term mp color psnfss
set output 'throbber_color.mp'
#set key top left
#set key 500, .009
set key top right
set size 1.5,1.5
set xlabel 'Node/CPU Count'
set ylabel 'Elapsed Time/Time-step (seconds)'
set xrange [200:200000]
set logscale xy
plot 'benchmark_20060215/apoa1_respa.mpi.dc1.spfft.20060215.dat' index 9 using 2:($3/1e9) title 'Local Bcast ApoA1 MPI' with linespoints lt 2 pt 10,\
'benchmark_20060215/rhod.mpi.dc1.spfft.20060215.dat' index 9 using 2:($3/1e9) title 'Local Bcast Rhod MPI' with linespoints lt 2 pt 9,\
'benchmark_20060215/sope_72.mpi.dc1.spfft.dat' index 9 using 2:($3/1e9) title 'Local Bcast SOPE MPI' with linespoints lt 2 pt 8,\
'benchmark_20060215/apoa1_respa.mpi.dc1.spfft.20060215.dat' index 10 using 2:($3/1e9) title 'Local Reduce ApoA1 MPI' with linespoints lt 2 pt 6,\
'benchmark_20060215/rhod.mpi.dc1.spfft.20060215.dat' index 10 using 2:($3/1e9) title 'Local Reduce Rhod MPI' with linespoints lt 2 pt 5,\
'benchmark_20060215/sope_72.mpi.dc1.spfft.dat' index 10 using 2:($3/1e9) title 'Local Reduce SOPE MPI' with linespoints lt 2 pt 3,\
'benchmark_20060215/apoa1_respa.spi_dcc.dc1.spfft.20060215.dat' index 9 using 2:($3/1e9) title 'Local Bcast ApoA1 SPI' with linespoints lt 1 pt 10,\
'benchmark_20060221/AlexBld.060220.Float.EOrb.rhod.p4.128x128x128.spi.dat' index 9 using 2:($3/1e9) title 'Local Bcast Rhod SPI' with linespoints lt 1 pt 9,\
'benchmark_20060221/AlexBld.060220.Float.EOrb.sope.p4.128x128x128.spi.dat' index 9 using 2:($3/1e9) title 'Local Bcast SOPE SPI' with linespoints lt 1 pt 8,\
'benchmark_20060215/apoa1_respa.spi_dcc.dc1.spfft.20060215.dat' index 10 using 2:($3/1e9) title 'Local Reduce ApoA1 SPI' with linespoints lt 1 pt 6,\
'benchmark_20060221/AlexBld.060220.Float.EOrb.rhod.p4.128x128x128.spi.dat' index 10 using 2:($3/1e9) title 'Local Reduce Rhod SPI' with linespoints lt 1 pt 5,\
'benchmark_20060221/AlexBld.060220.Float.EOrb.sope.p4.128x128x128.spi.dat' index 10 using 2:($3/1e9) title 'Local Reduce SOPE SPI' with linespoints lt 1 pt 3
set output
set term win
