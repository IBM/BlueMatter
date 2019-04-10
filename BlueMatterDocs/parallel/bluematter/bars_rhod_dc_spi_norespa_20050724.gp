reset
#set term mp color psnfss
#set term postscript eps colour "Times-Roman" 18
#set output 'bars_rhod_dc_spi_norespa_20050724.ps'
set style line 1 lt -1 lw 3 pt 7 ps 2
#set output 'bars_rhod_dc_spi_norespa_20050724.mp'
set key top right
set xrange [256:32768]
set yrange [0:1]
# set y2range [0:1]
set ytics nomirror
#set yrange [0:0.02]
#set y2tics border
#set title "43K Rhodopsin System, 9/1\AA, Dual Core BG/L ADE SPI"
set xlabel 'Node Count'
set ylabel 'Fraction of Total Time-step'
#set y2label 'Time/Time-step (sec.)
set logscale x 2
tot(x)=1
frac=1.1
set style fill solid 1.0
#set style fill pattern
set boxwidth 0.5 relative
plot 'frac.93.dat' using ($1/(frac*frac)):($5+$3)/($2+$3+$5+$6) with boxes title 'Real Space',\
'frac.93.dat' using ($1/frac):($3)/($2+$3+$5+$6) with boxes title 'P3ME',\
'frac.93.dat' using ($1):($6)/($2+$3+$5+$6) with boxes title 'Local Force Reduction',\
'frac.93.dat' using ($1*frac):($2)/($2+$3+$5+$6) with boxes title 'Local Position Broadcast'

#'frac.93.dat' using 1:($4/1e9) with linespoints ls 1 axes x1y1 title 'Total Time/Time-step'
#set output
set term win
