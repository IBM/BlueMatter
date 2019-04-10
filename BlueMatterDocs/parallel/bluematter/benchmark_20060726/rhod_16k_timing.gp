reset
#set term mp color psnfss
#set output 'rhod_16k_timing.mp'
#set term epslatex color solid blacktext
set term postscript eps color blacktext solid font "TimesRoman" 18
#set output 'rhod_16k_timing.tex'
set output 'rhod_16k_timing.eps'
set key horizontal
set xlabel 'Milliseconds'
set ylabel 'Fraction of Nodes (offset)'
set yrange [0:1.5]
set xrange [0:5]
#set title 'Rhodopsin on 16,384 nodes' offset 0,-3
toff=9.83409e11
vdelta=0.01
nmax=10

plot 'rhod_16k_timing_20060726.dat' index 1 using (($4-toff)/0.7e6):($5+nmax*vdelta) title 'TimeStep' with lines,\
'rhod_16k_timing_20060726.dat' index 2 using (($4-toff)/0.7e6):($5+(nmax-1)*vdelta) title 'Fwd FFT' with filledcurves above y1=(nmax-1)*vdelta,\
'rhod_16k_timing_20060726.dat' index 11 using (($4-toff)/0.7e6):($5+(nmax-2)*vdelta) title 'Rev FFT' with filledcurves above y1=(nmax-2)*vdelta,\
'rhod_16k_timing_20060726.dat' index 13 using (($4-toff)/0.7e6):($5+(nmax-3)*vdelta) title 'Real Space Computation' with filledcurves above y1=(nmax-3)*vdelta,\
'rhod_16k_timing_20060726.dat' index 0 using (($4-toff)/0.7e6):($5+(nmax-5)*vdelta) title 'Real Space Broadcast' with lines,\
'rhod_16k_timing_20060726.dat' index 3 using (($4-toff)/0.7e6):($5+(nmax-6)*vdelta) title 'Real Space Reduce' with filledcurves above y1=(nmax-6)*vdelta,\
'rhod_16k_timing_20060726.dat' index 27 using (($4-toff)/0.7e6):($5+(nmax-7)*vdelta) title 'K Space Broadcast' with lines,\
'rhod_16k_timing_20060726.dat' index 6 using (($4-toff)/0.7e6):($5+(nmax-8)*vdelta) title 'K Space Reduce' with filledcurves above y1=(nmax-8)*vdelta

#'rhod_16k_timing_20060726.dat' index 8 using (($4-toff)/0.7e6):($5+(nmax-9)*vdelta) title 'Bonded Broadcast' with lines,\
#'rhod_16k_timing_20060726.dat' index 10 using (($4-toff)/0.7e6):($5+(nmax-10)*vdelta) title 'Bonded Reduction' with lines

#'rhod_16k_timing_20060726.dat' index 12 using (($4-toff)/0.7e6):($5+(nmax-4)*vdelta) title 'LocalTupleListDriver' with lines,\

set output
set term win
