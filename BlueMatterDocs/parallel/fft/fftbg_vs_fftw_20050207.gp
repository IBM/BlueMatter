reset
set xlabel "Node Count"
set ylabel "3D-FFT Execution Time (seconds)"
set logscale xy
plot 'mariae/fftbgmpi_20040704.dat' using 1:2 title 'Volumetric FFT on MPI (MPICH2)',\
'mariae/fftbgmpi_20050206.dat' using 1:2 title 'Volumetric FFT on MPI (optimized)',\
'arayshu/fft_blade_20040713.dat' index 2 using 1:2 title 'Volumetric FFT on Active Packet/Blade',\
'mariae/fftbgmpi_20040704.dat' using 1:3 title 'FFTW Port'
