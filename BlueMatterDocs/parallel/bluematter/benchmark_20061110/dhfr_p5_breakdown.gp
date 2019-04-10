reset
#set term mp color psnfss
#set output 'dhfr_p5_breakdown.mp'
#set size 1.5, 2
set xlabel 'Nodes'
set ylabel 'Time/Time-step (sec.)'
#set key below Left reverse
set key 300, 1e-3

set logscale xy

plot 'dhfr_p5_20061110.dat' index 26 using 2:($3/0.7e9) title 'Total Time-step' with linespoints,\
'dhfr_p5_20061110.dat' index 15 using 2:($3/0.7e9) title 'Total K-space' with linespoints,\
'dhfr_p5_20061110.dat' index 9 using 2:($3/0.7e9) title 'K-space Broadcast' with linespoints,\
'dhfr_p5_20061110.dat' index 13 using 2:($3/0.7e9) title 'K-space Reduction' with linespoints,\
'dhfr_p5_20061110.dat' index 11 using 2:($3/0.7e9) title 'K-space Forward FFT' with linespoints,\
'dhfr_p5_20061110.dat' index 14 using 2:($3/0.7e9) title 'K-space Reverse FFT' with linespoints,\
'dhfr_p5_20061110.dat' index 8 using 2:($3/0.7e9) title 'K-space Mesh Charges' with linespoints,\
'dhfr_p5_20061110.dat' index 12 using 2:($3/0.7e9) title 'K-space Unmesh Forces' with linespoints,\
'dhfr_p5_20061110.dat' index 22 using 2:($3/0.7e9) title 'Real Space Non-bond Computation' with linespoints,\
'dhfr_p5_20061110.dat' index 5 using 2:($3/0.7e9) title 'Real Space Non-bond Broadcast' with linespoints,\
'dhfr_p5_20061110.dat' index 24 using 2:($3/0.7e9) title 'Real Space Non-bond Reduction' with linespoints,\
'dhfr_p5_20061110.dat' index 0 using 2:($3/0.7e9) title 'Bonded Force Broadcast' with linespoints,\
'dhfr_p5_20061110.dat' index 1 using 2:($3/0.7e9) title 'Bonded Force Reduction' with linespoints,\
'dhfr_p5_20061110.dat' index 16 using 2:($3/0.7e9) title 'Local Tuple List Driver' with linespoints

#set output
#set term win

#'dhfr_p5_20061110.dat' index 23 using 2:($3/0.7e9) title 'Real Space Non-bond Meat' with linespoints,\
#'dhfr_p5_20061110.dat' index 13 using 2:(2*$3) title 'Reverse FFT' with linespoints
#'dhfr_p5_20061110.dat' index 10 using 2:(2*$3) title 'Forward FFT' with linespoints,\
