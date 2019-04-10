set ylabel "Fraction"
set key right center
set title "Box-Muller with 100,000,000 counts"
plot 'test_100M.dat' using 3:4:5 with errorbars title "Cumulant D(x)",\
     'test_100M.dat' using 3:8 with lines title "(1+Erf(x/sqrt(2)))/2",\
     'test_100M.dat' using 3:6:7 with errorbars title "1-D(x)",\
     'test_100M.dat' using 3:9 with lines title "Erfc(x/sqrt(2))/2",\
     'test_100M.dat' using 3:(1000*($4-$8)):(1000*$5) with errorbars title "1000x Residuals with Erf"
     

