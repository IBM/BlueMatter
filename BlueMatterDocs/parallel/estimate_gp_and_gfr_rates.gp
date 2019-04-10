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
#set key graph .28, .95
set key bottom right
set xrange [*:5000]
#set yrange [0:1.3e-6]
set xlabel "Node Count"
set ylabel "Computation Rate (ts/cycle)"
# set logscale xy
plot 'est_gpv.5000' using 1:(1/$10) title '5000 atoms (gp)' with lines,\
  'est_gfrv.5000' using 1:(1/$11) title '5000 atoms (gfr)' with lines,\
  'est_gpv.10000' using 1:(1/$10) title '10,000 atoms (gp)' with lines,\
  'est_gfrv.10000' using 1:(1/$11) title '10,000 atoms (gfr)' with lines,\
  'est_gpv.20000' using 1:(1/$10) title '20,000 atoms (gp)' with lines,\
  'est_gfrv.20000' using 1:(1/$11) title '20,000 atoms (gfr)' with lines,\
  'est_gpv.30000' using 1:(1/$10) title '30,000 atoms (gp)' with lines,\
  'est_gfrv.30000' using 1:(1/$11) title '30,000 atoms (gfr)' with lines
