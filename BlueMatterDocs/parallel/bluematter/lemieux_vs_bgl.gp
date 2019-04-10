reset
#set term mp color psnfss
#set output 'lemieux_vs_bgl.mp'
set xlabel 'Node/CPU Count'
set ylabel 'Elapsed Time (seconds)'
set xrange [100:]
set logscale xy
set linestyle 1
set linestyle 2
set linestyle 3
set linestyle 4
plot 'apoa.12a.respa.dc.spi.20050724.dat' index 5 using 1:($2/1e9) title 'Blue Matter on BG/L (BG/L ADE SPI)' with linespoints,\
'namd_lemieux_2002.dat' using 1:5 title 'NAMD on Lemieux (Elan/Quadrics)' with linespoints

#set output
set term win
#'dc_91k_20050405.dat' index 6 using 1:($2/1e9) title 'Blue Matter Factor IX on BG/L (MPI)' with linespoints
#'namd_bgl_20050420.dat' using 1:2 title 'NAMD ApoA1 on BG/L (MPI) PME every step' with linespoints,\
#'p4_43k_dual_20050405.dat' index 19 using 1:($2/1e9) title 'Blue Matter on BG/L (MPI) 43k system' with linespoints
