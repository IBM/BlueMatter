reset
set xlabel "Task Count"
set ylabel "3D-FFT time (seconds)"
#set title "FFTW"
set xrange [1:1000]
set yrange [.01:100]
set logscale xy
plot 'fftw_20030509.dat' index 0 using 4:5 title '128x128x128',\
'fftw_20030509.dat' index 1 using 4:5 title '256x256x256'

