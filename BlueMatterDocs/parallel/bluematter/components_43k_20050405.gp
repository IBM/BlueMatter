reset
set size 1.2, 1.2
set key top right
# set title '43K Rhodopsin System'
set xlabel 'Node Count'
set ylabel 'Elapsed Time (seconds)'
set logscale xy
set linestyle 1
set linestyle 2
set linestyle 3
set linestyle 4
set linestyle 5
set linestyle 6
set linestyle 7
plot 'dual_43k_20050405.dat' index 19 using 1:($2/1e9) title 'Total Time Step' with linespoints linestyle 1,\
'dual_43k_20050405.dat' index 3 using 1:($2/1e9) title 'Reciprocal Space' with linespoints linestyle 2,\
'sc_43k_20050410.dat' index 7 using 1:($2/1e9) title 'Real Space (from single core run)' with linespoints linestyle 6,\
'dual_43k_20050405.dat' index 6 using 1:($2/1e9) title 'Local Force Reduction' with linespoints linestyle 3,\
'dual_43k_20050405.dat' index 20 using 1:($2/1e9) title 'Local Position Broadcast' with linespoints linestyle 4,\
'dual_43k_20050405.dat' index 10 using 1:($2/1e9) title 'Bonded Forces' with linespoints linestyle 5
# 'dual_43k_20050405_cubic4k.dat' index 6 using 1:($2/1e9) title 'Total Time Step' with linespoints linestyle 1,\
# 'dual_43k_20050405.dat' index 10 using 1:($2/1e9) title 'Forward FFT' with linespoints linestyle 6,\
# 'dual_43k_20050405.dat' index 12 using 1:($2/1e9) title 'Reverse FFT' with linespoints linestyle 7,\
# 'dual_43k_20050405.dat' index 6 using 1:($2/1e9):(($2+$6)/1E9):(($2+$7)/1E9)  notitle with yerrorbars linestyle 1,\
# 'dual_43k_20050405.dat' index 4 using 1:($2/1e9):(($2+$6)/1E9):(($2+$7)/1E9)  notitle with yerrorbars linestyle 2,\
# 'dual_43k_20050405.dat' index 0 using 1:($2/1e9):(($2+$6)/1E9):(($2+$7)/1E9)  notitle with yerrorbars linestyle 4,\
# 'dual_43k_20050405.dat' index 15 using 1:($2/1e9):(($2+$6)/1E9):(($2+$7)/1E9)  notitle with yerrorbars linestyle 5
#'dual_43k_20050405.dat' index 13 using 1:($2/1e9):(($2+$6)/1E9):(($2+$7)/1E9)  notitle with yerrorbars linestyle 3,\


