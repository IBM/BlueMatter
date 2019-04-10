reset
#set term mp color psnfss
#set output 'bars_apoa_10_2_dc_spi_respa_20050724.mp'
set key top right
set xrange [256:32768]
set yrange [0:1]
set ytics nomirror
#set y2range [0:0.04]
#set y2range [0:50]
#set y2tics border
#set title "92K ApoA System, 10/2\AA, P3ME Every 4 Steps, Dual Core BG/L ADE SPI"
set xlabel 'Node Count'
set ylabel 'Fraction of Total Time-step'
#set y2label 'Time/Time-step x Node Count (sec.)
set logscale x 2
tot(x)=1
frac=1.1
set style fill solid 1.0
set boxwidth 0.5 relative
plot 'frac.91.dat' using ($1/(frac*frac)):($5+$3)/($2+$3+$5+$6) with boxes title 'Real Space',\
'frac.91.dat' using ($1/frac):($3)/($2+$3+$5+$6) with boxes title 'P3ME',\
'frac.91.dat' using ($1):($6)/($2+$3+$5+$6) with boxes title 'Local Force Reduction',\
'frac.91.dat' using ($1*frac):($2)/($2+$3+$5+$6) with boxes title 'Local Position Broadcast'

#'frac.91.dat' using 1:($4*$1/1e9) with linespoints axes x1y2 title 'Total Time/Time-step x Node Count'
#set output
set term win
