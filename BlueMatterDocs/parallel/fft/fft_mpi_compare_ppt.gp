reset
set size 1, .9
set key top right
set xrange [1:1000]
set logscale xy
set xlabel "Task Count"
set ylabel "Time (sec.)"
tick=1.0/700e6

plot './mariae/fftbgmpi_20040424_coll.dat' index 2 using 1:2 title '128^3 MPI Collective' with linespoints,\
'./arayshu/fft_blade_20040422.dat' index 2 using 1:2 title '128^3 Active Packet' with linespoints,\
'../fft.700mhz.bw.128' using 1:(($5*2+$6)*tick) title '128^3 Model' with lines,\
'./mariae/fftbgmpi_20040424_coll.dat' index 0 using 1:2 title '32^3 MPI Collective' with linespoints,\
'./arayshu/fft_blade_20040422.dat' index 0 using 1:2 title '32^3 Active Packet' with linespoints,\
'../fft.700mhz.bw.32' using 1:(($5*2+$6)*tick) title '32^3 Model' with lines

#'./mariae/fftbgmpi_20040424_coll.dat' index 1 using 1:2 title '64^3 MPI Collective' with linespoints,\
#'./arayshu/fft_blade_20040422.dat' index 1 using 1:2 title '64^3 Active Packet' with linespoints,\
#'../fft.700mhz.bw.64' using 1:(($5*2+$6)*tick) title '64^3 Model' with lines,\


# './arayshu/scaling_20030723.dat' using 1:($2*700/444) title "64^3 packet layer" with linespoints,\
# './arayshu/scaling_20030723.dat' using 1:($3*700/444) title "128^3 packet layer" with linespoints,\
# '././fft.700mhz.mem.32' using 1:((2*$5+$6+$9)*tick) title "32^3 model" with lines,\
# '././fft.700mhz.mem.64' using 1:((2*$5+$6+$9)*tick) title "64^3 model" with lines,\
# '../../fft.700mhz.mem.128' using 1:((2*$5+$6+$9)*tick) title "128^3 model" with lines,\
# '../../fft.700mhz.mem.256' using 1:((2*$5+$6+$9)*tick) title "256^3 model" with lines

# '../../fft.500mhz.tjcw.64' using 1:($7*tick) title "64^3 model" with linespoints,\

