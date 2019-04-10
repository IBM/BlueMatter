reset
#set term mp mono psnfss
#set term mp color psnfss
#set output 'fft_pkt_vs_mpi_mono.mp'
#set output 'fft_pkt_vs_mpi.mp'
#set size 1.3,1.3
set xrange [100:100000]
set xlabel 'Node Count'
set ylabel 'Elapsed Time (seconds)'
set logscale xy
# set label '$8\times 8\times 8$' at 512, 1.1e-4 rotate
# set label '$8\times 8\times 16$' at 1024, 1.1e-4 rotate
# set label '$16\times 8\times 16$' at 2048, 1.1e-4 rotate
# set label '$8\times 32\times 16$' at 4096, 1.1e-4 rotate
# set label '$16\times 16\times 16$' at 4096, 1.43e-3
set linestyle 1
set linestyle 2
set linestyle 3
set linestyle 4
set linestyle 5
set linestyle 6
plot './mariae/fftbgwmpi_20050511.dat' index 0 using 1:5 title '128^3 MPI' with linespoints,\
'fft_pkt_sc_20050418.dat' using ($1*$2*$3):5 title '128^3 BG/L ADE Single Core' with linespoints,\
'./mariae/fftbgwmpi_20050511.dat' index 1 using 1:5 title '64^3 MPI' with linespoints,\
'fft_pkt_sc_20050418.dat' using ($1*$2*$3):($4 > 0 ? $4 : 1/0) title '64^3 BG/L ADE Single Core' with linespoints
#'../newfft_128.dat' using 1:($8) title 'estimate for $128^3$' with line,\
#'../newfft_64.dat' using 1:($8) title 'estimate for $64^3$' with line

#'fft_mpi_20050406.dat' index 1 using ($1*$2*$3):4 title '$128^3$ MPI' with linespoints,\
#fft_mpi_20050406.dat' index 0 using ($1*$2*$3):4 title '$64^3$ MPI' with linespoints,\
# 'fft_pkt_20050411.dat' using ($1*$2*$3):5 title '$128^3$ Blade Dual Core' with linespoints,\
# 'fft_pkt_20050411.dat' using ($1*$2*$3):4 title '$64^3$ Blade Dual Core' with linespoints,\

#set output
set term win
