reset

set size .9, .9
# set size 1.2, 1.2
set origin .1, .1
set lmargin 8
set rmargin 8
set bmargin 2
set tmargin 1
#set key 17000,1300
#set key graph .25, .95
set key top right
#set xrange [*:20000]
#set yrange [0:1]
set xlabel "Node Count"
set ylabel "MD time-step (seconds)"
set logscale xy
plot 'bglmd.30000' using 1:5 title 'Position Globalization' with lines,\
'bglmd.30000' using 1:6 title 'Global Force Reduction' with lines


