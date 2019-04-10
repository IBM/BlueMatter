reset
#set term mp color psnfss
#set output 'sope_p5_breakdown.mp'
set xlabel 'Nodes'
set ylabel 'Time/Time-step (ns)'

set logscale xy

plot 'sope_p5_20060726.dat' index 27 using 2:($3/0.7) title 'Total Time-step' with linespoints,\
'sope_p5_20060726.dat' index 14 using 2:($3/0.7) title 'Total K-space' with linespoints,\
'sope_p5_20060726.dat' index 23 using 2:($3/0.7) title 'Real Space Meat' with linespoints,\
'sope_p5_20060726.dat' index 24 using 2:($3/0.7) title 'Real Space Total' with linespoints,\
'sope_p5_20060726.dat' index 4 using 2:($3/0.7) title 'Globalize Positions' with linespoints,\
'sope_p5_20060726.dat' index 25 using 2:($3/0.7) title 'Reduce Forces' with linespoints

#set output

#'sope_p5_20060726.dat' index 13 using 2:(2*$3) title 'Reverse FFT' with linespoints
#'sope_p5_20060726.dat' index 10 using 2:(2*$3) title 'Forward FFT' with linespoints,\
