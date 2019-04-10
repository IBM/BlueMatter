reset
set xlabel "Node Count"
set ylabel "3D FFT Time (sec.)"
set logscale xy
plot 'scaling_20030723.dat' using 1:2 title "64x64x64",\
'scaling_20030723.dat' using 1:3 title "128x128x128"
