reset
set key bottom right
set fit errorvariables
f(x)=a*x+b
a=2e-06
b=0.0025
fit f(x) 'rhod_energy.dat' every 1000 using ($1*2e-6):(($2+42689.985)/29400) via a,b
set xlabel "Simulation Time (nanoseconds)"
set ylabel "(E-E0)/KE0"
plot 'rhod_energy.dat' every 1000 using ($1*2e-6):(($2+42689.985)/29400) title 'Rhodopsin (NVE)',\
f(x)
