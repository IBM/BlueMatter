reset
set xlabel "Task Count"
set ylabel "3D-FFT time (seconds)"
set logscale xy
plot 'fftbgmpi_20031009.dat' index 0 using ($2*$3*$4):5 title '64x64x64 FFTBGMPI on BG/L -qarch=440 w/tjcw 1d fft',\
'fftbgmpi_20031009.dat' index 1 using ($2*$3*$4):5 title '128x128x128 FFTBGMPI on BG/L -qarch=440 w/tjcw 1d fft',\
'fftbgmpi_20031116.dat' index 0 using ($2*$3*$4):5 title '64x64x64 FFTBGMPI on BG/L -qarch=440d w/tjcw 1d fft',\
'fftbgmpi_20031116.dat' index 1 using ($2*$3*$4):5 title '128x128x128 FFTBGMPI on BG/L -qarch=440d w/tjcw 1d fft',\
'fftbg_20030509.dat' index 0 using ($4*$5*$6):7 title '128x128x128 FFTBG on Power4',\
'fftbgmpi_20031008.dat' index 0 using ($2*$3*$4):5 title '64x64x64 FFTBGMPI on BG/L w/vienna 1d fft',\
'fftbgmpi_20031008.dat' index 1 using ($2*$3*$4):5 title '128x128x128 FFTBGMPI on BG/L w/vienna 1d fft',\
'../arayshu/scaling_20030723.dat' using 1:2 title "$64^3$ packet layer -qarch=440d, 444MHz",\
'../arayshu/scaling_20030723.dat' using 1:3 title "$128^3$ packet layer -qarch=440d, 444MHz",\
'../../fft.500mhz.64' using 1:8 title '$64^3$ estimate' with lines,\
'../../fft.500mhz.128' using 1:8 title '$128^3$ estimate' with lines,
