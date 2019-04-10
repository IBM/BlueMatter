reset
napoa=92224
nrhod=43222
nsope=13758
nhairpin=5239
#set term mp color psnfss
#set output "node_counts.mp"
#set size 1.3,1.3
set size 0.7, 1
set logscale xy 2
#set key outside
set key 120000, 9
set yrange [:15]
set xrange [450:20000]
set xlabel "Partition Size (p)"
set ylabel "Communicating Partners/$\sqrt{p}$"
plot 'node_counts.dat' index 0 using ($1*$2*$3):($4/sqrt($1*$2*$3)) title 'Hairpin (V4)' with linespoints lt 3 pt 7,\
'node_counts.dat' index 0 using ($1*$2*$3):($5/sqrt($1*$2*$3)) title 'Hairpin (V5 Full Skin)' with linespoints lt 1 pt  1,\
'node_counts.dat' index 0 using ($1*$2*$3):($6/sqrt($1*$2*$3)) title 'Hairpin (V5 Sparse Skin)' with linespoints lt 8 pt 1,\
'node_counts.dat' index 1 using ($1*$2*$3):($4/sqrt($1*$2*$3)) title 'SOPE (V4)' with linespoints lt 3 pt 8,\
'node_counts.dat' index 1 using ($1*$2*$3):($5/sqrt($1*$2*$3)) title 'SOPE (V5 Full Skin)' with linespoints lt 1 pt 3,\
'node_counts.dat' index 1 using ($1*$2*$3):($6/sqrt($1*$2*$3)) title 'SOPE (V5 Sparse Skin)' with linespoints lt 8 pt 3,\
'node_counts.dat' index 2 using ($1*$2*$3):($4/sqrt($1*$2*$3)) title 'Rhodopsin (V4)' with linespoints lt 3 pt 9,\
'node_counts.dat' index 2 using ($1*$2*$3):($5/sqrt($1*$2*$3)) title 'Rhodopsin (V5 Full Skin)' with linespoints lt 1 pt 5,\
'node_counts.dat' index 2 using ($1*$2*$3):($6/sqrt($1*$2*$3)) title 'Rhodopsin (V5 Sparse Skin)' with linespoints lt 8 pt 5,\
'node_counts.dat' index 3 using ($1*$2*$3):($4/sqrt($1*$2*$3)) title 'ApoA1 (V4)' with linespoints lt 3 pt 10,\
'node_counts.dat' index 3 using ($1*$2*$3):($5/sqrt($1*$2*$3)) title 'ApoA1 (V5 Full Skin)' with linespoints lt 1 pt 6,\
'node_counts.dat' index 3 using ($1*$2*$3):($6/sqrt($1*$2*$3)) title 'ApoA1 (V5 Sparse Skin)' with linespoints lt 8 pt 6
#set output
set term win
