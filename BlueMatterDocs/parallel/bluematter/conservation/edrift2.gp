reset
set term mp color psnfss
set output 'edrift2.mp'
set key bottom right
set fit errorvariables
f(x)=a*x+b
a=2e-06
b=0.0025
fit f(x) 'rhod_energy.dat' every 1000 using ($1*2e-6):(($2+42570.6)/29439.1) via a,b
set yrange [-0.002:0.002]
set y2range [-0.002*310:0.002*310]
set ytics border nomirror
set lmargin 11
set rmargin 7
set y2tics border nomirror
set xlabel "Simulation Time (nanoseconds)"
set ylabel "$(E_{total}(t)-\\overline{E}_{total})/\\overline{E}_{kinetic}$"
set y2label "$(E_{total}(t)-\\overline{E}_{total})/(N_{dof}\\,k_B/2)$ (K)"
plot 'rhod_energy.dat' every 1000 using ($1*2e-6):(($2+42570.6)/29439.1) title 'Rhodopsin (NVE)',\
f(x) title 'Fit'
set output
set term win
