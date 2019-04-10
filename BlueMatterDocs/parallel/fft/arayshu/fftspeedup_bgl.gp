reset
set xlabel "Node Count"
set ylabel "Speedup"
set key top left
# set logscale xy
plot 'scaling_20030723.dat' using 1:1 title "Ideal" with lines,\
'scaling_20030723.dat' using 1:(.277456/$2) title "64x64x64",\
'scaling_20030723.dat' using 1:(2.87/$3) title "128x128x128"
