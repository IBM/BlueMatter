reset
# set size 1.25
set key bottom right
set xlabel "Task Count"
set ylabel "Speedup"
set logscale xy

# 'fftbgmpi_20031008.dat' index 0 using ($2*$3*$4):(0.458856/$5) title '$64^3$ FFTBGMPI on BG/L w/vienna 1d fft',\
# 'fftbgmpi_20031009.dat' index 0 using ($2*$3*$4):(5.572062e-01/$5) title '$64^3$ FFTBGMPI on BG/L -qarch=440, 500MHz',\
# 'fftbg_20030509.dat' index 0 using ($4*$5*$6):(1.71777777777778/$7) title '$128^3$ FFTBG on Power4',\
# 'fftbgmpi_20040202.dat' index 0 using 2:(3.31570e-02/$3) title "$32^3$ FFTBGMPI -qarch=440d, 500MHz",\

plot 'fftbgmpi_20031009.dat' index 0 using ($2*$3*$4):($2*$3*$4) title "Ideal" with lines,\
'fftbgmpi_20040202.dat' index 1 using 2:(4.99185e-01/$3) title "$64^3$ FFTBGMPI -qarch=440d, 500MHz",\
'../arayshu/scaling_20030723.dat' using 1:(.277456/$2) title "$64^3$ packet layer -qarch=440d, 444MHz",\
'../arayshu/scaling_20030723.dat' using 1:(2.87/$3) title "$128^3$ packet layer -qarch=440d, 444MHz"

