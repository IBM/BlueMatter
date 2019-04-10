# Main options
#-qsource
#-qlist
-O3
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
-zmuldefs
# Opt options

Compiler /opt/ibmcmp/vacpp/7.0/bin/blrts_xlC
CompilerHost bgwfen1

blrts
mpi
double_hummer_3phase_pass

-DBMM_RDG_Split
