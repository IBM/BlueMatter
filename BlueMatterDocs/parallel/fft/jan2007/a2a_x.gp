# benchmarking data for 3D-FFT 
reset
set title '64 x 64 x 64'
#set key 300, 9e-4
set key top left
set logscale xy
set xlabel "Task Count"
set ylabel "Time (sec.)"

plot 'MPI/double/fft_mpi_double_64_64_64.dat' index 1 using 2:($3/1e9) title 'MPI double A2A X' with linespoints,\
'MPI/float/fft_mpi_float_64_64_64.dat' index 1 using 2:($3/1e9) title 'MPI float A2A X' with linespoints,\
'SPI/regular-double/fft_spi_regular_double.dat' index 1 using 2:($3/1e9) title 'SPI reg. double A2A X' with linespoints,\
'SPI/regular-float/fft_spi_regular_float_64_64_64.dat' index 2 using 2:($3/1e9) title 'SPI reg. float A2A X' with linespoints,\
'SPI/fast-double/fft_spi_fast_double_64_64_64.dat' index 3 using 2:($3/1e9) title 'SPI fast double A2A X' with linespoints

#'SPI/regular-double/fft_spi_regular_double.dat' index 2 using 2:($3/1e9) title 'SPI reg. double A2A Y' with linespoints,\
#'SPI/regular-double/fft_spi_regular_double.dat' index 3 using 2:($3/1e9) title 'SPI reg. double A2A Z' with linespoints,\
#'SPI/regular-float/fft_spi_regular_float_64_64_64.dat' index 3 using 2:($3/1e9) title 'SPI reg. float A2A Y' with linespoints,\
#'SPI/regular-float/fft_spi_regular_float_64_64_64.dat' index 4 using 2:($3/1e9) title 'SPI reg. float A2A Z' with linespoints,\
#'SPI/fast-double/fft_spi_fast_double_64_64_64.dat' index 4 using 2:($3/1e9) title 'SPI fast float A2A Y' with linespoints,\
#'SPI/fast-double/fft_spi_fast_double_64_64_64.dat' index 5 using 2:($3/1e9) title 'SPI fast float A2A Z' with linespoints,\
#'MPI/double/fft_mpi_double_64_64_64.dat' index 2 using 2:($3/1e9) title 'MPI double A2A Y' with linespoints,\
#'MPI/double/fft_mpi_double_64_64_64.dat' index 3 using 2:($3/1e9) title 'MPI double A2A Z' with linespoints,\
#'MPI/float/fft_mpi_float_64_64_64.dat' index 2 using 2:($3/1e9) title 'MPI float A2A Y' with linespoints,\
#'MPI/float/fft_mpi_float_64_64_64.dat' index 3 using 2:($3/1e9) title 'MPI float A2A Z' with linespoints
