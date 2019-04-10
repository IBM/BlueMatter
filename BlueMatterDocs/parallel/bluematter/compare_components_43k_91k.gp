reset
set key bottom left
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
set linestyle 8
set linestyle 9
set linestyle 10
set linestyle 11
set linestyle 12
set linestyle 13
plot 'sc_91k_20050406.dat' index 8 using 1:($2/1e9) title 'Total Time (91K)' with linespoints linestyle 1,\
'sc_43k_20050406.dat' index 8 using 1:($2/1e9) title 'Total Time (43K)' with linespoints linestyle 2,\
'sc_91k_20050406.dat' index 6 using 1:($2/1e9) title 'Reciprocal Space (91K)' with linespoints linestyle 11,\
'sc_43k_20050406.dat' index 6 using 1:($2/1e9) title 'Reciprocal Space (43K)' with linespoints linestyle 12,\
'sc_91k_20050406.dat' index 7 using 1:($2/1e9) title 'Real Space (91K)' with linespoints linestyle 3,\
'sc_43k_20050406.dat' index 7 using 1:($2/1e9) title 'Real Space (43K)' with linespoints linestyle 4,\
'sc_91k_20050406.dat' index 15 using 1:($2/1e9) title 'Reduce Forces (91K)' with linespoints linestyle 5,\
'sc_43k_20050406.dat' index 15 using 1:($2/1e9) title 'Reduce Forces (43K)' with linespoints linestyle 6,\
'sc_91k_20050406.dat' index 1 using 1:($2/1e9) title 'Position Broadcast (91K)' with linespoints linestyle 7,\
'sc_43k_20050406.dat' index 1 using 1:($2/1e9) title 'Position Broadcast (43K)' with linespoints linestyle 8,\
'sc_91k_20050406.dat' index 17 using 1:($2/1e9) title 'Bonded Forces (91K)' with linespoints linestyle 9,\
'sc_43k_20050406.dat' index 17 using 1:($2/1e9) title 'Bonded Forces (43K)' with linespoints linestyle 10,\
'sc_91k_20050406.dat' index 8 using 1:($2/1e9):(($2+$6)/1E9):(($2+$7)/1E9)  notitle with yerrorbars linestyle 1,\
'sc_43k_20050406.dat' index 8 using 1:($2/1e9):(($2+$6)/1E9):(($2+$7)/1E9)  notitle with yerrorbars linestyle 2


