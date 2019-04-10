reset
#set term mp color psnfss
#set output 'sope_72_20060215.mp'
set key 600,0.009
set title '13758 Atom SOPE BG/L ADE SPI'
set xlabel 'Node/CPU Count'
set ylabel 'Elapsed Time/Time-step (seconds)'
set xrange [100:]
set logscale xy
set linestyle 1
set linestyle 2
set linestyle 3
set linestyle 4
plot 'sope_72.spi_dcc.dc1.dpfft.dat' index 4 using 2:($3/1e9) title 'DPFFT Total Time' with linespoints,\
'sope_72.spi_dcc.dc1.spfft.dat' index 4 using 2:($3/1e9) title 'SPFFT Total Time' with linespoints,\
'sope_72.spi_dcc.dc1.dpfft.dat' index 3 using 2:($3/1e9) title 'DPFFT K-space' with linespoints,\
'sope_72.spi_dcc.dc1.spfft.dat' index 3 using 2:($3/1e9) title 'SPFFT K-space' with linespoints,\
'sope_72.spi_dcc.dc1.dpfft.dat' index 1 using 2:($3/1e9) title 'DPFFT Globalize Positions' with linespoints,\
'sope_72.spi_dcc.dc1.spfft.dat' index 1 using 2:($3/1e9) title 'SPFFT Globalize Positions' with linespoints,\
'sope_72.spi_dcc.dc1.dpfft.dat' index 7 using 2:($3/1e9) title 'DPFFT Reduce Forces' with linespoints,\
'sope_72.spi_dcc.dc1.spfft.dat' index 7 using 2:($3/1e9) title 'SPFFT Reduce Forces' with linespoints,\
'sope_72.spi_dcc.dc1.dpfft.dat' index 9 using 2:($3/1e9) title 'DPFFT Throbber Bcast' with linespoints,\
'sope_72.spi_dcc.dc1.spfft.dat' index 9 using 2:($3/1e9) title 'SPFFT Throbber Bcast' with linespoints,\
'sope_72.spi_dcc.dc1.dpfft.dat' index 10 using 2:($3/1e9) title 'DPFFT Throbber Reduce' with linespoints,\
'sope_72.spi_dcc.dc1.spfft.dat' index 10 using 2:($3/1e9) title 'SPFFT Throbber Reduce' with linespoints

