reset
set xlabel 'Node Count'
set ylabel 'Elapsed Time (seconds)'
set logscale xy
set linestyle 1
set linestyle 2
set linestyle 3
set linestyle 4
plot 'apoa_92k_dc_20050721.dat' index 6 using 1:($2/1e9) title '92K atom ApoA1' with linespoints,\
'dc_51k_1000_20050424.dat' index 6 using 1:($2/1e9) title '51K atom Mini FBP' with linespoints,\
'p4_43k_dual_20050405.dat' index 19 using 1:($2/1e9) title '43K atom Rhodopsin' with linespoints,\
'dhfr_dc_20050418.dat' index 6 using 1:($2/1e9) title '23K atom DHFR' with linespoints

# 'dual_23K_20050414.dat' index 2 using 1:($2/1e9) title '23K DHFR (MPI)' with linespoints,\
#'dual_43k_20050405.dat' index 6 using 1:($2/1e9) title '43K Rhodopsin (MPI)' with linespoints,\
