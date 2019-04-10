reset
set xlabel "Node Count"
set ylabel "3D-FFT time (seconds)"
set logscale xy
plot 'fftbg_20030509.dat' index 0 using ($4*$5*$6):7 title '128x128x128 FFTBG',\
'fftw_20030509.dat' index 0 using 4:5 title '128x128x128 FFTW',\
'fftbg_20030509.dat' index 1 using ($4*$5*$6):7 title '256x256x256 FFTBG',\
'fftw_20030509.dat' index 1 using 4:5 title '256x256x256 FFTW',\
'fftbg_20030509.dat' index 2 using ($4*$5*$6):7 title '512x512x512 FFTBG'

