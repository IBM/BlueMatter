reset
napoa=92224
nrhod=43222
nsope=13758
nhairpin=5239
set term mp color psnfss
set output 'systems_universal_bluematter_screen.mp'
set size 1.3,1.3
#set key 0.22, 340
set key 16, 1350
#set key 0.22, 675
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
plot 'benchmark_20060221/AlexBld.060220.Float.EOrb.hairpin.spi.dat' index 4 using (nhairpin/$2):(1e9/$3) title 'Hairpin SPI' with linespoints,\
'benchmark_20060221/AlexBld.060220.Float.EOrb.sope.p4.128x128x128.spi.dat' index 4 using (nsope/$2):(1e9/$3) title 'SOPE SPI' with linespoints,\
'benchmark_20060221/AlexBld.060220.Float.EOrb.rhod.p4.128x128x128.spi.dat' index 4 using (nrhod/$2):(1e9/$3) title 'Rhodopsin SPI' with linespoints,\
'benchmark_20060215/apoa1_respa.spi_dcc.dc1.spfft.20060215.dat' index 4 using (napoa/$2):(1e9/$3) title 'ApoA1 Respa SPI' with linespoints,\
'benchmark_20060215/apoa1.spi_dcc.dc1.spfft.20060215.dat' index 4 using (napoa/$2):(1e9/$3) title 'ApoA1 SPI' with linespoints,\
'benchmark_20060215/sope_72.mpi.dc1.spfft.dat' index 4 using (nsope/$2):(1e9/$3) title 'SOPE MPI' with linespoints,\
'benchmark_20060215/rhod.mpi.dc1.spfft.20060215.dat' index 4 using (nrhod/$2):(1e9/$3) title 'Rhodopsin MPI' with linespoints,\
'benchmark_20060215/apoa1_respa.mpi.dc1.spfft.20060215.dat' index 4 using (napoa/$2):(1e9/$3) title 'ApoA1 Respa MPI' with linespoints,\
'benchmark_20060215/apoa1.mpi.dc1.spfft.20060215.dat' index 4 using (napoa/$2):(1e9/$3) title 'ApoA1 MPI' with linespoints
set output
set term win
