reset
#set term mp color psnfss
#set output 'hairpin_p5_breakdown.mp'
set xlabel 'Milliseconds'
set ylabel 'Fraction of Nodes (offset)'
set yrange [0:2]
set xrange [0:100]
toff=9.2e10
set title 'Rhodopsin on 512 nodes' offset 0,-3
vdelta=0.01
nmax=6

plot 'rhod_512_timing_20060726.dat' index 1 using (($2-toff)/0.7e6):($3+nmax*vdelta) title 'TimeStep' with lines,\
'rhod_512_timing_20060726.dat' index 2 using (($2-toff)/0.7e6):($3+(nmax-1)*vdelta) title 'Fwd FFT' with lines,\
'rhod_512_timing_20060726.dat' index 11 using (($2-toff)/0.7e6):($3+(nmax-2)*vdelta) title 'Rev FFT' with lines,\
'rhod_512_timing_20060726.dat' index 13 using (($2-toff)/0.7e6):($3+(nmax-3)*vdelta) title 'Real Space Total' with lines,\
'rhod_512_timing_20060726.dat' index 12 using (($2-toff)/0.7e6):($3+(nmax-4)*vdelta) title 'LocalTupleListDriver' with lines,\
'rhod_512_timing_20060726.dat' index 0 using (($2-toff)/0.7e6):($3+(nmax-5)*vdelta) title 'ThrobberBroadcast' with lines,\
'rhod_512_timing_20060726.dat' index 3 using (($2-toff)/0.7e6):($3+(nmax-6)*vdelta) title 'ThrobberReduce' with lines

