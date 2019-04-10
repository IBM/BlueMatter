reset
set term mp color psnfss
set output 'edrift.mp'
#set key bottom right
#set key left top
set key at graph 0.82,0.99

set xrange [0:200]
set fit errorvariables
f(x)=a*x+b
# 2 fs time-step fit
a=-7.9e-07
b=0.0
e02=-136729
ke02=52403
tsconv2=2e-15*1e9
fit f(x) 'lambda_repressor_2fs_ts.dat' every 5 using ($1*tsconv2):(($2-e02)/ke02) via a,b
a2fs=a
b2fs=b
f2(x)=a2fs*x+b2fs
# 1.5 fs time-step fit
a=-7.9e-07
b=0.0
#set xrange [0:200]
e01_5=-136738
ke01_5=52486
tsconv1_5=1.5e-15*1e9
fit f(x) 'lambda_repressor_1_5fs_ts.dat' every 5 using ($1*tsconv1_5):(($2-e01_5)/ke01_5) via a,b
a1_5fs=a
b1_5fs=b
f1_5(x)=a1_5fs*x+b1_5fs
# 1 fs time-step fit
a=-7.9e-07
b=0.0
e01=-136740
ke01=52539
fit f(x) 'lambda_repressor_1fs_ts.dat' every 5 using ($1*1e-6):(($2-e01)/ke01) via a,b
a1fs=a
b1fs=b
f1(x)=a1fs*x+b1fs
set yrange [-0.0005:0.0005]
set y2range [-0.0005*394:0.0005*394]
set ytics border nomirror
#set lmargin 11
#set rmargin 7
set y2tics border nomirror
set xlabel "Simulation Time (nanoseconds)"
set ylabel "$(E_{total}(t)-\\overline{E}_{total})/\\overline{E}_{kinetic}$"
set y2label "$(E_{total}(t)-\\overline{E}_{total})/(N_{dof}\\,k_B/2)$ (K)"
plot 'lambda_repressor_2fs_ts.dat' every 5 using ($1*tsconv2):(($2-e02)/ke02) title '2 fs' with lines,\
f2(x) title gprintf("Fit to 2 fs data, slope $= %.1t \\times 10^{%T}$/ns",a2fs),\
 'lambda_repressor_1_5fs_ts.dat' every 5 using ($1*tsconv1_5):(($2-e01_5)/ke01_5) title '1.5 fs' with lines,\
f1_5(x) title gprintf("Fit to 1.5 fs data, slope $= %.1t \\times 10^{%T}$/ns",a1_5fs),\
'lambda_repressor_1fs_ts.dat' every 5 using ($1*1e-6):(($2-e01)/ke01) title '1 fs' with lines,\
f1(x) title gprintf("Fit to 1 fs data, slope $= %.1t \\times 10^{%T}$/ns",a1fs)

set output
set term win
