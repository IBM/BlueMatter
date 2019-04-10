reset
set term mp color psnfss
set output 'rhod_breakdown.mp'
#set title '43K Atom Rhodopsin'
set xlabel 'Node Count'
set ylabel 'Elapsed Time/Time-step (seconds)'
set xrange [100:]
set logscale xy
set linestyle 1
set linestyle 2
set linestyle 3
set linestyle 4
plot 'rhod.spi_dcc.dc1.dpfft.20060215.dat' index 4 using 2:($3/1e9) title 'BG/L ADE SPI DPFFT Total Time' with linespoints,\
'rhod.spi_dcc.dc1.spfft.20060215.dat' index 4 using 2:($3/1e9) title 'BG/L ADE SPI SPFFT Total Time' with linespoints,\
'rhod.spi_dcc.dc1.dpfft.20060215.dat' index 3 using 2:($3/1e9) title 'BG/L ADE SPI DPFFT K-space' with linespoints,\
'rhod.spi_dcc.dc1.spfft.20060215.dat' index 3 using 2:($3/1e9) title 'BG/L ADE SPI SPFFT K-space' with linespoints,\
'rhod.spi_dcc.dc1.dpfft.20060215.dat' index 1 using 2:($3/1e9) title 'BG/L ADE SPI DPFFT Globalize Positions' with linespoints,\
'rhod.spi_dcc.dc1.dpfft.20060215.dat' index 7 using 2:($3/1e9) title 'BG/L ADE SPI DPFFT Reduce Forces' with linespoints,\
'rhod.spi_dcc.dc1.dpfft.20060215.dat' index 9 using 2:($3/1e9) title 'BG/L ADE SPI DPFFT Throbber Bcast' with linespoints,\
'rhod.spi_dcc.dc1.dpfft.20060215.dat' index 10 using 2:($3/1e9) title 'BG/L ADE SPI DPFFT Throbber Reduce' with linespoints
set output
set term win
