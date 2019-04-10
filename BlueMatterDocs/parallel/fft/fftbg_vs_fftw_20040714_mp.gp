set term mp psnfss
set output "fftbg_vs_fftw_20040714.mp"
reset
set xlabel "Node Count"
set ylabel "3D-FFT Execution Time (seconds)"
set logscale xy
set title '128 x 128 x 128 FFT Results on BG/L' -15, -20
plot 'mariae/fftbgmpi_20040704.dat' using 1:2 title 'Volumetric FFT on MPI',\
'arayshu/fft_blade_20040713.dat' index 2 using 1:2 title 'Volumetric FFT on Active Packet/Blade',\
'mariae/fftbgmpi_20040704.dat' using 1:3 title 'FFTW Port'

set output
