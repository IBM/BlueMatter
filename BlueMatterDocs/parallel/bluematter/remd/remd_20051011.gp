reset
set key bottom right
set xlabel "Replica ID"
set ylabel "Time/Time-step (sec.)"

plot 'remd_20051011_4kcubic_128ST.dat' index 3 using ($3/1e9) with points title '128 replicas, same T',\
'remd_20051011_4kcubic_128DT.dat' index 3 using ($3/1e9) with points title '128 replicas, different T',\
'remd_20051011_4kcubic_64ST.dat' index 3 using ($3/1e9) with points title '64 replicas, same T',\
'remd_20051011_4kcubic_64DT.dat' index 3 using ($3/1e9) with points title '64 replicas, different T',\
'remd_20051011_4kcubic_32ST.dat' index 3 using ($3/1e9) with points title '32 replicas, same T',\
'remd_20051011_4kcubic_32DT.dat' index 3 using ($3/1e9) with points title '32 replicas, different T',\
'remd_20051011_4kcubic_16ST.dat' index 3 using ($3/1e9) with points title '16 replicas, same T',\
'remd_20051011_4kcubic_16DT.dat' index 3 using ($3/1e9) with points title '16 replicas, different T',\
'remd_20051011_4kcubic_8ST.dat' index 3 using ($3/1e9) with points title '8 replicas, same T',\
'remd_20051011_4kcubic_8DT.dat' index 3 using ($3/1e9) with points title '8 replicas, different T',\
'remd_20051011_4kcubic_4ST.dat' index 3 using ($3/1e9) with points title '4 replica, same T',\
'remd_20051011_4kcubic_4DT.dat' index 3 using ($3/1e9) with points title '4 replica, different T',\
'remd_20051011_4kcubic_2ST.dat' index 3 using ($3/1e9) with points title '2 replica, same T'
