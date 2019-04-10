reset
# set size 1.25
set xlabel "Task Count"
set ylabel "Time/timestep (seconds)"
set logscale xy

plot 'bluematter_mpi_20031014.dat' using 1:2 title '$\beta$-hairpin (5239 atoms), -O2 -qarch=440, $64^3$ FFT'
