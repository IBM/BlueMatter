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
plot 'md.30000' using 1:5 title 'BG/L' with lines,\
     'md.30000' using 1:6 title 'Power5 blade (GS)' with lines,\
     'md.30000' using 1:7 title 'GPUL-2 blade' with lines,\
     'md.30000' using 1:8 title 'IA32 blade' with lines
