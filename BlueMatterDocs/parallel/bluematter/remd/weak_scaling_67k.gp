reset
set term mp color psnfss
set output 'weak_scaling_67k.mp'

set logscale x
set logscale x2
set xtics nomirror
set x2tics nomirror
set x2range [10/32.0:10000/32.0]
#set xrange [32:32*300]
set xrange [10:10000]
set yrange [0:0.5]
set xlabel 'Total Node Count'
set x2label 'Replica Count'
set ylabel 'Time/Time-step (sec.)'

plot 'weak_scaling_20051031.dat' using 2:3 notitle
set output
