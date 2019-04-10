reset
napoa=92224
nrhod=43222
nsope=13758
nhairpin=5239
#set term mp color psnfss
#set output 'systems_universal_p5_p5_screen.mp'
#set size 1.3,1.3
#set key 0.22, 340
#set key top left
set key 16, 1350
#set key 0.22, 450
#set key bottom right
#set key top left
#set xrange [1000:0.1]
set xrange [200:0.1]
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
plot 'benchmark_20060726/hairpin_p5_20060726.dat' index 27 using (nhairpin/$2):(0.7e9/$3) title 'Hairpin V5 (7/26/06)' with linespoints,\
'benchmark_20060423/hairpin.p5.float.spi.dat' index 10 using (nhairpin/$2):(1e9/$3) title 'Hairpin V5 (4/23/06)' with linespoints,\
'benchmark_20060726/sope_p5_20060726.dat' index 27 using (nsope/$2):(0.7e9/$3) title 'SOPE V5 (7/26/06)' with linespoints,\
'benchmark_20060423/sope.p5.float.spi.dat' index 19 using (nsope/$2):(1e9/$3) title 'SOPE V5 (4/23/2006)' with linespoints,\
'benchmark_20060726/rhod_p5_20060726.dat' index 27 using (nrhod/$2):(0.7e9/$3) title 'Rhodopsin V5 (7/26/06)' with linespoints,\
'benchmark_20060423/rhod.p5.float.spi.dat' index 19 using (nrhod/$2):(1e9/$3) title 'Rhodopsin V5 (4/23/06)' with linespoints,\
'benchmark_20060726/apoa_p5_20060726.dat' index 27 using (napoa/$2):(0.7e9/$3) title 'ApoA1 V5 (7/26/06)' with linespoints,\
'benchmark_20060423/apoa.p5.float.spi.dat' index 19 using (napoa/$2):(1e9/$3) title 'ApoA1 V5 (4/23/06)' with linespoints,\
'namd_bgl_ipdps_2006.dat' using (napoa/$1):(1/$2) title 'ApoA1 NAMD Msging Layer' with linespoints,\
'namd_bgl_ipdps_2006.dat' using (napoa/$1):(1/$3) title 'ApoA1 NAMD MPI' with linespoints,\
'namd_lemieux_2002.dat' using (napoa/$1):(1/$5) title 'ApoA1 NAMD on Lemieux' with linespoints
#set output
#'benchmark_20060215/apoa1_respa.spi_dcc.dc1.spfft.20060215.dat' index 4 using (napoa/$2):(1e9/$3) title 'ApoA1 Respa SPI (BM4)' with linespoints,\
#'benchmark_20060411/sope.p5.float.spi.dat' index 18 using (nsope/$2):(1e9/$3) title 'SOPE SPI (V5)' with linespoints,\
#'benchmark_20060411/hairpin.p5.float.spi.dat' index 6 using (nhairpin/$2):(1e9/$3) title 'Hairpin SPI (V5)' with linespoints,\
#'benchmark_20060411/rhod.p5.float.spi.dat' index 18 using (nrhod/$2):(1e9/$3) title 'Rhodopsin SPI (V5)' with linespoints,\
#'benchmark_20060411/apoa.p5.float.spi.dat' index 18 using (napoa/$2):(1e9/$3) title 'ApoA1 SPI (V5)' with linespoints
#set output
set term win
