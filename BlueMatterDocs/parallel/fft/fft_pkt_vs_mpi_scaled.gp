reset
#set size 1.3,1.3
set key bottom right
set xlabel 'Node Count'
set ylabel 'Elapsed Time x p**(2/3) (seconds)'
#set logscale xy
set logscale x
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
plot 'fft_mpi_20050406.dat' index 1 using ($1*$2*$3):(($1*$2*$3)**0.666666667*$4) title '$128^3$ MPI' with linespoints,\
'fft_pkt_sc_20050418.dat' using ($1*$2*$3):(($1*$2*$3)**0.666666667*$5) title '$128^3$ Blade Single Core' with linespoints,\
'fft_mpi_20050406.dat' index 0 using ($1*$2*$3):(($1*$2*$3)**0.666666667*$4) title '$64^3$ MPI' with linespoints,\
'fft_pkt_sc_20050418.dat' using ($1*$2*$3):($4 > 0 ? (($1*$2*$3)**0.666666667*$4) : 1/0) title '$64^3$ Blade Single Core' with linespoints

# 'fft_pkt_20050411.dat' using ($1*$2*$3):5 title '$128^3$ Blade Dual Core' with linespoints,\
# 'fft_pkt_20050411.dat' using ($1*$2*$3):4 title '$64^3$ Blade Dual Core' with linespoints,\
