reset
set term mp color psnfss
set output 'fft_mpi_compare_128_20050511.mp'
set size 1.2, 1.2
set key top right
set xrange [1:17000]
set logscale xy
set xlabel "Task Count"
set ylabel "Time (sec.)"
tick=1.0/700e6

plot './mariae/fftbgmpi_20040704.dat' using 1:2 title '$128^3$ MPI Collective (MPICH2)' with linespoints,\
'./mariae/fftbgwmpi_20050511.dat' index 0 using 1:5 title '$128^3$ MPI Collective (optimized)' with linespoints,\
'fft_pkt_sc_20050418.dat' using ($1*$2*$3):5 title '$128^3$ BG/L ADE Single Core' with linespoints,\
'../fft.700mhz.bw.128' using 1:((($1 < 512 ? 2 : 1)*$5+$6)*tick) title '$128^3$ Model' with lines

#'fft_mpi_20050406.dat' index 1 using ($1*$2*$3):4 title '$128^3$ MPI Collective (optimized)' with linespoints,\
#'./mariae/fftbgmpi_20050206.dat' using 1:2 title '$128^3$ MPI Collective (optimized)' with linespoints,\
#'./arayshu/fft_blade_20040713.dat' index 2 using 1:2 title '$128^3$ Active Packet' with linespoints,\
# './arayshu/scaling_20030723.dat' using 1:($2*700/444) title "$64^3$ packet layer" with linespoints,\
# './arayshu/scaling_20030723.dat' using 1:($3*700/444) title "$128^3$ packet layer" with linespoints,\
# '././fft.700mhz.mem.32' using 1:((2*$5+$6+$9)*tick) title "$32^3$ model" with lines,\
# '././fft.700mhz.mem.64' using 1:((2*$5+$6+$9)*tick) title "$64^3$ model" with lines,\
# '../../fft.700mhz.mem.128' using 1:((2*$5+$6+$9)*tick) title "$128^3$ model" with lines,\
# '../../fft.700mhz.mem.256' using 1:((2*$5+$6+$9)*tick) title "$256^3$ model" with lines

# '../../fft.500mhz.tjcw.64' using 1:($7*tick) title "$64^3$ model" with linespoints,\

set output
set term win
