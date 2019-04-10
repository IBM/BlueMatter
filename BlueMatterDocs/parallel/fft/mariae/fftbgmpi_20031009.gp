reset
set size 1.25
set xlabel "Task Count"
set ylabel "3D-FFT time (seconds)"
set logscale xy

# 'fftbgmpi_20031008.dat' index 0 using ($2*$3*$4):5 title '64x64x64 FFTBGMPI on BG/L w/vienna 1d fft',\
#'fftbgmpi_20031008.dat' index 1 using ($2*$3*$4):5 title '128x128x128 FFTBGMPI on BG/L w/vienna 1d fft'
#'../arayshu/scaling_20031014.dat' using 1:2 title "$128^3$ packet layer -qarch=440d 500MHz",\

plot 'fftbgmpi_20031009.dat' index 0 using ($2*$3*$4):5 title '$64^3$ FFTBGMPI on BG/L -qarch=440 w/tjcw 1d fft',\
'fftbgmpi_20031009.dat' index 1 using ($2*$3*$4):5 title '$128^3$ FFTBGMPI on BG/L -qarch=440 w/tjcw 1d fft',\
'../arayshu/scaling_20030723.dat' using 1:2 title "$64^3$ packet layer -qarch=440d 444MHz",\
'../arayshu/scaling_20030723.dat' using 1:3 title "$128^3$ packet layer -qarch=440d 444MHz",\
'fftbg_20030509.dat' index 0 using ($4*$5*$6):7 title '$128^3$ FFTBG on Power4
