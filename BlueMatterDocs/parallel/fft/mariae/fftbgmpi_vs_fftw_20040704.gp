reset
set xlabel "Node Count"
set ylabel "3D-FFT time (seconds)"
set logscale xy
set title '128 x 128 x 128 FFT Results on BG/L'
plot 'fftbgmpi_20040704.dat' using 1:2 title 'FFTBG on MPI',\
'../arayshu/fft_blade_20040422.dat' index 2 using 1:2 title 'FFTBG on Active Packet',\
'fftbgmpi_20040704.dat' using 1:3 title 'FFTW Port'
