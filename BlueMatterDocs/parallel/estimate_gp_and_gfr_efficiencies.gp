reset
clock=700e6
ideal(x)=x

set size 1, .8
#set size 1.2, 1.2
set origin .1, .1
set lmargin 8
set rmargin 8
set bmargin 2
set tmargin 1
#set key 17000,1300
#set key graph .95, .95
set key bottom left
set xrange [*:10000]
set yrange [.1:1]
#set ytics ax ("0.1" .1, "0.2" .2, "0.3" .3, "0.4" .4, "0.5" .5, "0.6" .6, "0.7" .7, "0.8" .8, "0.9" .9, "1.0" 1)
set ytics ax ("0.1" .1, "0.2" .2, " " .3, " " .4, "0.5" .5, " " .6, "0.7" .7, " " .8, " " .9, "1.0" 1)
#set ytics ax ("0.1" .1, "0.2" .2, "0.4" .4, "0.6" .6, "0.8" .8, "1.0" 1)
#set ytics ax ("0.1" .1, "0.2" .2, "0.5" .5, "0.7" .7, "1.0" 1)
set xlabel "Node Count"
set ylabel "Parallel Efficiency" -3
set logscale xy
plot 'est_gpv.5000' using 1:12 title '5000 atoms (gp)' with lines,\
  'est_gfrv.5000' using 1:13 title '5000 atoms (fr)' with lines,\
  'est_gpv.10000' using 1:12 title '10,000 atoms (gp)' with lines,\
  'est_gfrv.10000' using 1:13 title '10,000 atoms (fr)' with lines,\
  'est_gpv.20000' using 1:12 title '20,000 atoms (gp)' with lines,\
  'est_gfrv.20000' using 1:13 title '20,000 atoms (fr)' with lines,\
  'est_gpv.30000' using 1:12 title '30,000 atoms (gp)' with lines,\
  'est_gfrv.30000' using 1:13 title '30,000 atoms (fr)' with lines
