reset
clock=700e6
ideal(x)=x

#set size 1, .8
#set size 1.2, 1.2
#set size 0.9, 0.9
#set size 0.8, 0.6
#set size 1, .5
set origin .1, .1
set lmargin 8
set rmargin 8
set bmargin 2
set tmargin 1
#set key 17000,1300
#set key graph .25, .95
set key bottom left
set xrange [1:100000]
set yrange [1:1e9]
set xlabel "Node Count"
set ylabel "Contributions to Time Step Duration (cycles)"
set logscale xy
set origin .1, .1
#set size 1.2, 1.2
set size .9, .9
set title "Position Globalization, $64^3$ mesh, Gradient" 10, -3
plot 'est_gpv.5200.gradient' using 1:($10) title 'Total' with lines,\
  'est_gpv.5200.gradient' using 1:($9) title 'Verlet' with lines,\
  'est_gpv.5200.gradient' every ::1 using 1:($8) title 'Globalization' with lines,\
  'est_gpv.5200.gradient' using 1:(($5+$6)) title 'FFT' with lines,\
  'est_gpv.5200.gradient' using 1:($7) title 'Real space non-bond' with lines
