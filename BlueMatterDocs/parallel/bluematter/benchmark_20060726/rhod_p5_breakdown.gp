reset
set term mp color magnification 0.8 psnfss 
set output 'rhod_p5_breakdown.mp'
#set size 1.5, 2
set xlabel 'Nodes'
set ylabel 'Time/Time-step (ns)'
set key below Left reverse horizontal

set logscale xy

plot 'rhod_p5_20060726.dat' index 27 using 2:($3/0.7) title 'Total Time-step' with linespoints lw 2,\
'rhod_p5_20060726.dat' index 14 using 2:($3/0.7) title 'Total K-space' with linespoints,\
'rhod_p5_20060726.dat' index 8 using 2:($3/0.7) title 'K-space Broadcast' with linespoints,\
'rhod_p5_20060726.dat' index 12 using 2:($3/0.7) title 'K-space Reduction' with linespoints,\
'rhod_p5_20060726.dat' index 10 using 2:($3/0.7) title 'K-space Forward FFT' with linespoints,\
'rhod_p5_20060726.dat' index 13 using 2:($3/0.7) title 'K-space Reverse FFT' with linespoints,\
'rhod_p5_20060726.dat' index 7 using 2:($3/0.7) title 'K-space Mesh Charges' with linespoints,\
'rhod_p5_20060726.dat' index 11 using 2:($3/0.7) title 'K-space Unmesh Forces' with linespoints,\
'rhod_p5_20060726.dat' index 24 using 2:($3/0.7) title 'Real Space Non-bond Computation' with linespoints,\
'rhod_p5_20060726.dat' index 4 using 2:($3/0.7) title 'Real Space Non-bond Broadcast' with linespoints,\
'rhod_p5_20060726.dat' index 25 using 2:($3/0.7) title 'Real Space Non-bond Reduction' with linespoints
#'rhod_p5_20060726.dat' index 0 using 2:($3/0.7) title 'Bonded Force Broadcast' with linespoints,\
#'rhod_p5_20060726.dat' index 1 using 2:($3/0.7) title 'Bonded Force Reduction' with linespoints,\
#'rhod_p5_20060726.dat' index 15 using 2:($3/0.7) title 'Bonded Force Computation' with linespoints

set output
set term win

#'rhod_p5_20060726.dat' index 23 using 2:($3/0.7) title 'Real Space Non-bond Meat' with linespoints,\
#'rhod_p5_20060726.dat' index 13 using 2:(2*$3) title 'Reverse FFT' with linespoints
#'rhod_p5_20060726.dat' index 10 using 2:(2*$3) title 'Forward FFT' with linespoints,\
