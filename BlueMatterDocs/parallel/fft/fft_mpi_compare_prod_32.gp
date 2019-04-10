reset
set size 1, 1.2
# set key top right
set key below
set xrange [1:2050]
set yrange [1e-5:10]
set logscale xy
set xlabel "Task Count"
set ylabel '(Task Count)$^{2/3} \times$ Time (sec.)'
tick=1.0/700e6
pwr=2.0/3.0

plot './mariae/fftbgmpi_20040704.dat' using 1:($1**pwr*$2) title '$128^3$ MPI Collective' with linespoints,\
'./arayshu/fft_blade_20040713.dat' index 2 using 1:($1**pwr*$2) title '$128^3$ Active Packet' with linespoints,\
'./mariae/fftbgmpi_20040714.dat' index 1 using 4:($4**pwr*$5) title '$64^3$ MPI Collective' with linespoints,\
'./arayshu/fft_blade_20040713.dat' index 1 using 1:($1**pwr*$2) title '$64^3$ Active Packet' with linespoints,\
'./mariae/fftbgmpi_20040714.dat' index 0 using 4:($4**pwr*$5) title '$32^3$ MPI Collective' with linespoints,\
'./arayshu/fft_blade_20040713.dat' index 0 using 1:($1**pwr*$2) title '$32^3$ Active Packet' with linespoints

#'../fft.700mhz.bw.128' using 1:($1**pwr*($5*2+$6)*tick) title '$128^3$ Model' with lines,\
#'../fft.700mhz.bw.64' using 1:($1**pwr*($5*2+$6)*tick) title '$64^3$ Model' with lines,\
#'../fft.700mhz.bw.32' using 1:($1**pwr*($5*2+$6)*tick) title '$32^3$ Model' with lines

# './arayshu/scaling_20030723.dat' using 1:($2*700/444) title "$64^3$ packet layer" with linespoints,\
# './arayshu/scaling_20030723.dat' using 1:($3*700/444) title "$128^3$ packet layer" with linespoints,\
# '././fft.700mhz.mem.32' using 1:((2*$5+$6+$9)*tick) title "$32^3$ model" with lines,\
# '././fft.700mhz.mem.64' using 1:((2*$5+$6+$9)*tick) title "$64^3$ model" with lines,\
# '../../fft.700mhz.mem.128' using 1:((2*$5+$6+$9)*tick) title "$128^3$ model" with lines,\
# '../../fft.700mhz.mem.256' using 1:((2*$5+$6+$9)*tick) title "$256^3$ model" with lines

# '../../fft.500mhz.tjcw.64' using 1:($7*tick) title "$64^3$ model" with linespoints,\

