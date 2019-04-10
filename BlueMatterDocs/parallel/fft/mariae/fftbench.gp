reset
set xlabel "Task Count"
set ylabel "3D-FFT time (seconds)"
set xrange [1:1000]
set yrange [.01:100]
set logscale xy
plot 'fftbg_20030509.dat' index 0 using ($4*$5*$6):7 title '128x128x128',\
'fftbg_20030509.dat' index 1 using ($4*$5*$6):7 title '256x256x256',\
'fftbg_20030509.dat' index 2 using ($4*$5*$6):7 title '512x512x512'

