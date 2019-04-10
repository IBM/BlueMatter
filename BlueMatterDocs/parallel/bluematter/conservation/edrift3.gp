reset
set term mp color psnfss
set output 'edrift3.mp'
set key bottom right
set fit errorvariables
f(x)=a*x+b
a=2e-06
b=0.0025
g(x)=c*(x-800)
c=1e-6
#fit f(x) 'rhod_energy.dat' every 1000 using ($1*2e-6):(($2+42570.6)/29439.1) via a,b
#fit g(x) 'rhod_energy.dat' every 1000 using ($1*2e-6):(($3-29439.1)/29439.1) via c
set yrange [-0.02:0.02]
set y2range [-0.02*310:0.02*310]
set ytics border nomirror
set lmargin 11
set rmargin 7
set y2tics border nomirror
set xlabel "Simulation Time (nanoseconds)"
set ylabel "$(E(t)-\\overline{E})/\\overline{E}_{kinetic}$"
set y2label "$(E(t)-\\overline{E})/(N_{dof}\\,k_B/2)$ (K)"
plot 'rhod_energy.dat' every 1000 using ($1*2e-6):(($2+42570.6)/29439.1) title 'Rhodopsin (NVE) $E_{total}$',\
'rhod_energy.dat' every 1000 using ($1*2e-6):(($3-29439.1)/29439.1) title 'Rhodopsin (NVE) $E_{kinetic}$'
#f(x) title 'Fit to $E_{total}$',\
#g(x) title 'Fit to $E_{kinetic}$'
set output
set term win
