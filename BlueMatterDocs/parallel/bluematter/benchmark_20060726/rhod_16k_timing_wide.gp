reset
#set term mp color psnfss
#set output 'rhod_16k_timing_wide.mp'
set size 1,0.5
set term postscript eps color blacktext solid font "TimesRoman" 18
set output 'rhod_16k_timing_wide.eps'
set key inside horizontal
set xlabel 'Milliseconds'
set ylabel 'Fraction of Nodes'
set yrange [0:1.2]
set xrange [0:100]
#set title 'Rhodopsin on 16,384 nodes' offset 0,-3
toff=9.83409e11
vdelta=0.00
nmax=6

plot 'rhod_16k_timing_20060726.dat' index 1 using (($2-toff)/0.7e6):($3+nmax*vdelta) title 'TimeStep' with lines
set output
set term win
