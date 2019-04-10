reset
set term mp color psnfss
set output 'sope_p5_breakdown_cmp.mp'
set xlabel 'Nodes'
set ylabel 'Time/Time-step (ns)'
set key bottom left
set logscale xy

plot 'sope_p5_20060726.dat' index 27 using 2:($3/0.7) title 'Total Time-step $128^3$' with linespoints,\
'sope_p5_20060726.dat' index 14 using 2:($3/0.7) title 'K-space $128^3$' with linespoints,\
'sope_p5_20060726.dat' index 24 using 2:($3/0.7) title 'Real Space $128^3$' with linespoints,\
'sope_p5_64x64x64_20060728.dat' index 27 using 2:($3/0.7) title 'Total Time-step $64^3$' with linespoints,\
'sope_p5_64x64x64_20060728.dat' index 14 using 2:($3/0.7) title 'K-space $64^3$' with linespoints,\
'sope_p5_64x64x64_20060728.dat' index 24 using 2:($3/0.7) title 'Real Space $64^3$' with linespoints

set output
set term win
#'sope_p5_20060726.dat' index 23 using 2:($3/0.7) title 'Real Space Meat' with linespoints,\
#'sope_p5_20060726.dat' index 4 using 2:($3/0.7) title 'Globalize Positions' with linespoints,\
#'sope_p5_20060726.dat' index 25 using 2:($3/0.7) title 'Reduce Forces' with linespoints

#'sope_p5_20060726.dat' index 13 using 2:(2*$3) title 'Reverse FFT' with linespoints
#'sope_p5_20060726.dat' index 10 using 2:(2*$3) title 'Forward FFT' with linespoints,\
