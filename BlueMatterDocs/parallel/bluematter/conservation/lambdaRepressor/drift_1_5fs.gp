reset
#set term mp color psnfss
#set output 'edrift2.mp'
set key bottom right
set fit errorvariables
ts=1.5e-15
tsconv=ts*1e9
e0=-136738
ke0=52486
f(x)=a*x+b
a=-7.9e-07
b=0.0
set xrange [0:200]
fit f(x) 'lambda_repressor_1_5fs_ts.dat' every 5 using ($1*tsconv):(($2-e0)/ke0) via a,b
if (FIT_CONVERGED==1 && GNUTERM eq 'win') set label sprintf("a = %5.2e    a_err = %4.1e",a,a_err) at graph 0.1, 0.9
#if (FIT_CONVERGED==1) set label gprintf("Drift $= %.2t \\times 10^{%T}$/ns",a) at graph 0.1, 0.9
if (FIT_CONVERGED==1 && GNUTERM eq 'win') set label sprintf("b = %5.2e    b_err = %4.1e",b,b_err) at graph 0.1, 0.8
#if (FIT_CONVERGED==1) set label gprintf("$b = %.2t \\times 10^{%T}$",b) at graph 0.1, 0.8
if (FIT_CONVERGED!=1) set label "Fit not converged!!" graph 0.9,0.1
#set yrange [-0.002:0.002]
#set y2range [-0.002*310:0.002*310]
set ytics border nomirror
set lmargin 11
set rmargin 7
#set y2tics border nomirror
set xlabel "Simulation Time (nanoseconds)"
set ylabel "$(E_{total}(t)-\\overline{E}_{total})/\\overline{E}_{kinetic}$"
set y2label "$(E_{total}(t)-\\overline{E}_{total})/(N_{dof}\\,k_B/2)$ (K)"
plot 'lambda_repressor_1_5fs_ts.dat' every 5 using ($1*tsconv):(($2-e0)/ke0) title 'Lamba Repressor (1.5 fs)' with lines,\
f(x) title gprintf("Linear Fit with slope $= %.1t \\times 10^{%T}$/ns",a)
#set output
set term win
