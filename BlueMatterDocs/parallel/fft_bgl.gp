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
set key top right
#set xrange [*:20000]
#set yrange [0:1]
set xlabel "Node Count"
set ylabel "3D-FFT time (seconds)"
set logscale xy
plot 'fft_bgl.128' using 1:($7/clock) title '128x128x128' with lines,\
'fft_bgl.256' using 1:($7/clock) title '256x256x256' with lines,\
'fft_bgl.512' using 1:($7/clock) title '512x512x512' with lines,\
'fft_bgl.1024' using 1:($7/clock) title '1024x1024x1024' with lines

