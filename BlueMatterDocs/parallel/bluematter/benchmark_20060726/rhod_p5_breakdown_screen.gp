reset
#set size 1.5, 2
#set size 0.75,1
set size 0.7,1
set xlabel 'Nodes'
set ylabel 'Time/Time-step (sec.)'
set yrange [1e-5:0.1]
set y2range [1e-5*0.7e9:0.1*0.7e9]
set y2label '(cycles)'
set ytics nomirror
set y2tics nomirror
#set key below Left reverse
set key 1e8, 0.01

set logscale xy

plot 'rhod_p5_20060726.dat' index 27 using 2:($3/0.7e9) title 'Total Time-step' with linespoints,\
'rhod_p5_20060726.dat' index 14 using 2:($3/0.7e9) title 'Total K-space' with linespoints,\
'rhod_p5_20060726.dat' index 8 using 2:($3/0.7e9) title 'K-space Pos. Dist.' with linespoints,\
'rhod_p5_20060726.dat' index 12 using 2:($3/0.7e9) title 'K-space Force Reduction' with linespoints,\
'rhod_p5_20060726.dat' index 10 using 2:($3/0.7e9) title 'K-space Forward FFT' with linespoints,\
'rhod_p5_20060726.dat' index 13 using 2:($3/0.7e9) title 'K-space Reverse FFT' with linespoints,\
'rhod_p5_20060726.dat' index 7 using 2:($3/0.7e9) title 'K-space Mesh Charges' with linespoints,\
'rhod_p5_20060726.dat' index 11 using 2:($3/0.7e9) title 'K-space Unmesh Forces' with linespoints,\
'rhod_p5_20060726.dat' index 24 using 2:($3/0.7e9) title 'Real Space Non-bond Comp.' with linespoints,\
'rhod_p5_20060726.dat' index 4 using 2:($3/0.7e9) title 'Real Space Non-bond Pos. Dist.' with linespoints,\
'rhod_p5_20060726.dat' index 25 using 2:($3/0.7e9) title 'Real Space Non-bond Force Reduction' with linespoints,\
'rhod_p5_20060726.dat' index 0 using 2:($3/0.7e9) title 'Bonded Force Pos. Dist' with linespoints,\
'rhod_p5_20060726.dat' index 1 using 2:($3/0.7e9) title 'Bonded Force Reduction' with linespoints,\
'rhod_p5_20060726.dat' index 15 using 2:($3/0.7e9) title 'Bonded Force Comp.' with linespoints

#'rhod_p5_20060726.dat' index 23 using 2:($3/0.7e9) title 'Real Space Non-bond Meat' with linespoints,\
#'rhod_p5_20060726.dat' index 13 using 2:(2*$3) title 'Reverse FFT' with linespoints
#'rhod_p5_20060726.dat' index 10 using 2:(2*$3) title 'Forward FFT' with linespoints,\
