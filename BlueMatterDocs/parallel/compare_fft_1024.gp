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
set key bottom left
#set xrange [*:20000]
#set yrange [0:1]
set xlabel "Node Count"
set ylabel "3D-FFT time (seconds)"
set logscale xy
plot 'fft_bgl.1024' using 1:($7/clock) title 'BG/L' with lines,\
     'fft_power5blade.1024' using 1:4 title 'Power5 blade (GS)' with lines,\
     'fft_gpul2blade.1024' using 1:4 title 'GPUL-2 blade' with lines,\
     'fft_ia32blade.1024' using 1:4 title 'IA32 blade' with lines,\
     'fft_power5blade_halfbw.1024' using 1:4 title 'Power5 blade (GS) 1GB/s' with lines,\
     'fft_gpul2blade_halfbw.1024' using 1:4 title 'GPUL-2 blade 1GB/s' with lines,\
     'fft_ia32blade_halfbw.1024' using 1:4 title 'IA32 blade 1GB/s' with lines
