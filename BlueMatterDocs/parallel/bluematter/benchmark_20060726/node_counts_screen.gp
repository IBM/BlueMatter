reset
napoa=92224
nrhod=43222
nsope=13758
nhairpin=5239
set size 0.7, 1
set logscale xy 2
#set term mp color psnfss
#set output "node_counts.mp"
#set size 1.3,1.3
set key 120000, 9
set xrange [450:20000]
set yrange [:15]
set xlabel "Partition Size (p)"
set ylabel "Communicating Partners / p^0.5"
plot 'node_counts.dat' index 0 using ($1*$2*$3):($4/sqrt($1*$2*$3)) title 'Hairpin (V4)' with linespoints lt 1 pt 8,\
'node_counts.dat' index 0 using ($1*$2*$3):($5/sqrt($1*$2*$3)) title 'Hairpin (V5 Full Skin)' with linespoints lt 3 pt 8,\
'node_counts.dat' index 0 using ($1*$2*$3):($6/sqrt($1*$2*$3)) title 'Hairpin (V5 Sparse Skin)' with linespoints lt 2 pt 8,\
'node_counts.dat' index 1 using ($1*$2*$3):($4/sqrt($1*$2*$3)) title 'SOPE (V4)' with linespoints lt 1 pt 9,\
'node_counts.dat' index 1 using ($1*$2*$3):($5/sqrt($1*$2*$3)) title 'SOPE (V5 Full Skin)' with linespoints lt 3 pt 9,\
'node_counts.dat' index 1 using ($1*$2*$3):($6/sqrt($1*$2*$3)) title 'SOPE (V5 Sparse Skin)' with linespoints lt 2 pt 9,\
'node_counts.dat' index 2 using ($1*$2*$3):($4/sqrt($1*$2*$3)) title 'Rhodopsin (V4)' with linespoints lt 1 pt 10,\
'node_counts.dat' index 2 using ($1*$2*$3):($5/sqrt($1*$2*$3)) title 'Rhodopsin (V5 Full Skin)' with linespoints lt 3 pt 10,\
'node_counts.dat' index 2 using ($1*$2*$3):($6/sqrt($1*$2*$3)) title 'Rhodopsin (V5 Sparse Skin)' with linespoints lt 2 pt 10,\
'node_counts.dat' index 3 using ($1*$2*$3):($4/sqrt($1*$2*$3)) title 'ApoA1 (V4)' with linespoints lt 1 pt 11,\
'node_counts.dat' index 3 using ($1*$2*$3):($5/sqrt($1*$2*$3)) title 'ApoA1 (V5 Full Skin)' with linespoints lt 3 pt 11,\
'node_counts.dat' index 3 using ($1*$2*$3):($6/sqrt($1*$2*$3)) title 'ApoA1 (V5 Sparse Skin)' with linespoints lt 2 pt 11

#set output
#set term win
