reset
clock=700e6
ideal(x)=x

set size .9, .9
# set size 1.2, 1.2
set origin .1, .1
set lmargin 8
set rmargin 8
set bmargin 2
set tmargin 1
#set key 17000,1300
#set key graph .25, .95
set key top left
#set xrange [*:20000]
#set yrange [0:1]
set xlabel "Node Count"
set ylabel "Speedup relative to Single Node"
set logscale xy
plot 'fft_bgl.1024' using 1:1 title 'ideal speedup' with lines,\
'fft_bgl.1024' using 1:($6*$1/$7) title 'est. speedup for $1024^3$' with lines,\
'fft_bgl.512' using 1:($6*$1/$7) title 'est. speedup for $512^3$' with lines,\
'fft_bgl.256' using 1:($6*$1/$7) title 'est. speedup for $256^3$' with lines,\
'fft_bgl.128' using 1:($6*$1/$7) title 'est. speedup for $128^3$' with lines


