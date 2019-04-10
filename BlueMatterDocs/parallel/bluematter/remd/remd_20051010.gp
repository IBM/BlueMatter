reset
set key top right
set xlabel "Replica ID"
set ylabel "Time/Time-step (sec.)"

plot 'remd_20051006.100.100.dat' index 3 using ($3/1e9) with points title '256 replica, same T',\
'remd_20051010_4K_128ST.dat' index 3 using ($3/1e9) with points title '128 replicas, same T',\
'remd_20051010_4K_128DT.dat' index 3 using ($3/1e9) with points title '128 replicas, different T',\
'remd_20051010_2K_64ST.dat' index 3 using ($3/1e9) with points title '64 replicas, same T',\
'remd_20051010_2K_64DT.dat' index 3 using ($3/1e9) with points title '64 replicas, different T',\
'remd_20051010_1K_32ST.dat' index 3 using ($3/1e9) with points title '32 replicas, same T',\
'remd_20051010_1K_32DT.dat' index 3 using ($3/1e9) with points title '32 replicas, different T',\
'remd_20051010_512_16ST.dat' index 3 using ($3/1e9) with points title '16 replicas, same T',\
'remd_20051010_512_16DT.dat' index 3 using ($3/1e9) with points title '16 replicas, different T',\
'remd_20051010_128_4ST.dat' index 3 using ($3/1e9) with points title '4 replicas, same T',\
'remd_20051010_128_4DT.dat' index 3 using ($3/1e9) with points title '4 replicas, different T',\
'remd_20051010_32_1ST.dat' index 3 using ($3/1e9) with points title '1 replica, same T',\
'remd_20051010_32_1DT.dat' index 3 using ($3/1e9) with points title '1 replica, same T'
