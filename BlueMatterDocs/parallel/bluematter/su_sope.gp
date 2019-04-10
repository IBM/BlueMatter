reset
napoa=92224
nrhod=43222
nsope=13758
nhairpin=5239
set term mp color psnfss
set output 'su_sope.mp'
set size 1.3,1.3
#set key 0.22, 340
#set key top left
set key 16, 1350
#set key 0.22, 450
#set key bottom right
#set key top left
#set xrange [1000:0.1]
set xrange [200:1]
set yrange [0:1400]
#set xrange [100:0]
set xlabel 'Atoms/Node'
set ylabel 'Computation Rate (time-steps/second)'
#set xrange [100:]
set logscale x
# set logscale xy
set linestyle 1
set linestyle 2
set linestyle 3
set linestyle 4
plot 'benchmark_20060726/sope_p5_64x64x64_20060728.dat' index 27 using (nsope/$2):(7e8/$3) title '$64^3$ FFT' with linespoints lt 3 pt 3,\
'benchmark_20060726/sope_p5_20060726.dat' index 27 using (nsope/$2):(7e8/$3) title '$128^3$ FFT' with linespoints lt 1 pt 5
#'benchmark_20060423/sope.p5.float.spi.dat' index 19 using (nsope/$2):(1e9/$3) title '$128^3$ FFT' with linespoints lt 1 pt 5
#'benchmark_20060221/AlexBld.060220.Float.EOrb.sope.p4.128x128x128.spi.dat' index 4 using (nsope/$2):(1e9/$3) title 'SOPE SPI (V4)' with linespoints lt 1 pt 8,\
#'benchmark_20060215/sope_72.mpi.dc1.spfft.dat' index 4 using (nsope/$2):(1e9/$3) title 'SOPE MPI (V4)' with linespoints lt 2 pt 8
#set output
#'benchmark_20060215/apoa1_respa.spi_dcc.dc1.spfft.20060215.dat' index 4 using (napoa/$2):(1e9/$3) title 'ApoA1 Respa SPI (BM4)' with linespoints,\
#'benchmark_20060411/sope.p5.float.spi.dat' index 18 using (nsope/$2):(1e9/$3) title 'SOPE SPI (BM5)' with linespoints,\
#'benchmark_20060411/hairpin.p5.float.spi.dat' index 6 using (nhairpin/$2):(1e9/$3) title 'Hairpin SPI (BM5)' with linespoints,\
#'benchmark_20060411/rhod.p5.float.spi.dat' index 18 using (nrhod/$2):(1e9/$3) title 'Rhodopsin SPI (BM5)' with linespoints,\
#'benchmark_20060411/apoa.p5.float.spi.dat' index 18 using (napoa/$2):(1e9/$3) title 'ApoA1 SPI (BM5)' with linespoints
#set output
set term win
