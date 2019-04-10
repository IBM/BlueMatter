# Main options
#-qsource
#-qlist
-O3
-qarch=440
-qdebug=except
-qmaxmem=-1
-qnoeh
-qnolm
-qnostrict
-qnounwind
-qpriority=0
-qstaticinline
-qsuppress=1540-1087:1540-1089:1540-1088:1540-1090
-qalign=natural
# Opt options

Compiler /opt/ibmcmp/vacpp/7.0/bin/blrts_xlC
CompilerHost bgfe01
blrts
mpi

-DBMM_RDG_Split
-DBMM_RDG_FlushPeriod=50
