# comparison of p3me and ewald scaling data
#
reset
set xlabel 'Node Count'
set ylabel 'Elapsed Time (pclocks)'
set logscale xy
ns2pclk=0.7
set linestyle 1
set linestyle 2
plot './p3me/p3me_mesh_20040406.dat' index 15 using 1:(ns2pclk*$2):(ns2pclk*$4):(ns2pclk*$5) title 'P3ME-mesh-based collectives' with errorbars linestyle 1,\
'./ewald/ewald_20040423.dat' index 17 using 1:(ns2pclk*$2):(ns2pclk*$4):(ns2pclk*$5) title 'Ewald' with errorbars linestyle 2,\
'./p3me/p3me_mesh_20040406.dat' index 15 using 1:(ns2pclk*$2) notitle with lines linestyle 1,\
'./ewald/ewald_20040423.dat' index 17 using 1:(ns2pclk*$2) notitle with lines linestyle 2
