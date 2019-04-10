set ylabel "Fraction"
set key left center
set title "Box-Muller with 1,000,000,000 counts"
plot 'test_1B.dat' using 3:4:5 with errorbars title "Cumulant D(x)",\
     'test_1B.dat' using 3:8 with lines title "(1+Erf(x/sqrt(2)))/2",\
     'test_1B.dat' using 3:6:7 with errorbars title "1-D(x)",\
     'test_1B.dat' using 3:9 with lines title "Erfc(x/sqrt(2))/2",\
     'test_1B.dat' using 3:(10000*($4-$8)):(10000*$5) with errorbars title "10,000x Residuals with Erf"
     

