reset
set key bottom left
set size 1.25
set xlabel "Task Count"
set ylabel "Time/timestep (seconds)"
set logscale xy

plot 'fft/mariae/bluematter_mpi_20031014.dat' using 1:2 title 'MPI, $\beta$-hairpin (5239 atoms), -O2 -qarch=440, $64^3$ FFT',\
'fft/arayshu/bluematter_blade_20031014.dat' using 1:2 title 'Blade, $\beta$-hairpin (5239 atoms), -O? -qarch=440, $64^3$ FFT',\
'bglmd.5239' using 1:5  title 'Estimate for position globalization, $64^3$ FFT, 5239 atoms' with lines

