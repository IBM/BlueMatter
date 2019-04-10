reset
napoa=92224
nrhod=43222
nsope=13758
nhairpin=5239
#set term mp color psnfss
#set output 'systems_universal_summary_screen.mp'
set key 0.22, 340
#set key 0.22, 450
#set key bottom right
#set key top left
#set xrange [1000:0.1]
set xrange [200:0.1]
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
plot 'benchmark_20060221/AlexBld.060220.Float.EOrb.sope.p4.128x128x128.spi.dat' index 4 using (nsope/$2):(1e9/$3) title 'SOPE SPI fixed ORB 2/21/06' with linespoints,\
'benchmark_20060221/AlexBld.060220.Float.sope.p4.128x128x128.spi.dat' index 4 using (nsope/$2):(1e9/$3) title 'SOPE SPI active ORB 2/21/06' with linespoints,\
'benchmark_20060215/sope_72.spi_dcc.dc1.spfft.dat' index 4 using (nsope/$2):(1e9/$3) title 'SOPE SPI 2/15/06' with linespoints,\
'benchmark_20060221/AlexBld.060220.Float.EOrb.hairpin.spi.dat' index 4 using (nhairpin/$2):(1e9/$3) title 'Hairpin SPI fixed ORB 2/21/06' with linespoints,\
'benchmark_20060221/AlexBld.060220.Float.hairpin.spi.dat' index 4 using (nhairpin/$2):(1e9/$3) title 'Hairpin SPI active ORB 2/21/06' with linespoints,\
'benchmark_20060221/AlexBld.060220.Float.EOrb.rhod.p4.128x128x128.spi.dat' index 4 using (nrhod/$2):(1e9/$3) title 'Rhodopsin SPI fixed ORB 2/21/06' with linespoints,\
'benchmark_20060215/rhod.spi_dcc.dc1.spfft.20060215.dat' index 4 using (nrhod/$2):(1e9/$3) title 'Rhodopsin SPI' with linespoints,\
'benchmark_20060215/apoa1_respa.spi_dcc.dc1.spfft.20060215.dat' index 4 using (napoa/$2):(1e9/$3) title 'ApoA1 Respa SPI' with linespoints,\
'benchmark_20060215/apoa1.spi_dcc.dc1.spfft.20060215.dat' index 4 using (napoa/$2):(1e9/$3) title 'ApoA1 SPI' with linespoints,\
'benchmark_20060215/sope_72.mpi.dc1.spfft.dat' index 4 using (nsope/$2):(1e9/$3) title 'SOPE MPI' with linespoints,\
'benchmark_20060215/rhod.mpi.dc1.spfft.20060215.dat' index 4 using (nrhod/$2):(1e9/$3) title 'Rhodopsin MPI' with linespoints,\
'benchmark_20060215/apoa1_respa.mpi.dc1.spfft.20060215.dat' index 4 using (napoa/$2):(1e9/$3) title 'ApoA1 Respa MPI' with linespoints,\
'benchmark_20060215/apoa1.mpi.dc1.spfft.20060215.dat' index 4 using (napoa/$2):(1e9/$3) title 'ApoA1 MPI' with linespoints

# 'namd_bgl_ipdps_2006.dat' using (napoa/$1):(1/$2) title 'ApoA1 NAMD Non-blocking Msging Layer' with linespoints,\
# 'namd_bgl_ipdps_2006.dat' using (napoa/$1):(1/$3) title 'ApoA1 NAMD MPI' with linespoints,\
#'namd_lemieux_2002.dat' using (napoa/$1):(1/$5) title 'ApoA1 NAMD on Lemieux (Elan/Quadrics)' with linespoints
#'namd_bgl_mpi_20060216.dat' using (napoa/$1):(1/$2) title 'ApoA1 NAMD MPI' with linespoints,\
#set output
set term win
