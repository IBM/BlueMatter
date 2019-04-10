reset

plot 'remd_20051006.100.100.dat' index 3 using ($3/1e9) with points title 'total time',\
'remd_20051006.100.100.dat' index 2 using ($3/1e9) with points title 'k-space',\
'remd_20051006.100.100.dat' index 6 using ($3/1e9) with points title 'RealSpaceAddForces',\
'remd_20051006.100.100.dat' index 7 using ($3/1e9) with points title 'RealSpaceDualCore',\
'remd_20051006.100.100.dat' index 8 using ($3/1e9) with points title 'ReduceForces',\
'remd_20051006.100.100.dat' index 9 using ($3/1e9) with points title 'remd_allreduce_energies',\
'remd_20051006.100.100.dat' index 0 using ($3/1e9) with points title 'globalize positions'

