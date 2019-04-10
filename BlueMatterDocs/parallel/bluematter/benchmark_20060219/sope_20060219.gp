reset
set key top right
set xlabel 'Node Count'
set ylabel 'Time/Time-step'
set logscale xy
set linestyle 1
set linestyle 2
set linestyle 3
set linestyle 4
plot '../benchmark_20060215/sope_72.spi_dcc.dc1.spfft.dat' index 4 using 2:($3/1e9) title 'SOPE SPI SPFFT 2/15/06' with linespoints,\
'../benchmark_20060215/sope_72.spi_dcc.dc1.dpfft.dat' index 4 using 2:($3/1e9) title 'SOPE SPI DPFFT 2/15/06' with linespoints,\
'AlexTag2.NewPktFeb1506.sope.p4.128x128x128.spi.dat' index 4 using 2:($3/1e9) title 'SOPE SPI DPFFT 2/19/06' with linespoints,\
'AlexTag2.FixedOrb.sope.p4.128x128x128.spi.dat' index 4 using 2:($3/1e9) title 'SOPE SPI Fixed ORB DPFFT 2/19/06' with linespoints

