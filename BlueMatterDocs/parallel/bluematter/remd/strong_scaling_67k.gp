reset
set term mp color psnfss
set output 'strong_scaling_67k.mp'
set logscale xy
set xtics nomirror
set x2tics nomirror
set logscale x2
#set yrange [0:]
set xrange [10:5000]
set x2range [4096/10.0:4096/5000.0]
set x2label 'Replica Count'
set xlabel 'Nodes/Replica'
set ylabel 'Time/Time-step (sec.)'

plot "strong_scaling_20051031.dat" using ($3/$1):($2) axes x1y1 title "REMD",\
"lambda_1LMB_20051103.dat" index 3 using ($2):($3*1.0e-9) axes x1y1 title "Independent"
set output
set term win
