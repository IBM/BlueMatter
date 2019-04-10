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

Compiler /opt/ibmcmp/vacpp/6.0/bin/blrts_xlC
blrts
mpi

-DPK_RAIDFS
-DCOMPILE_VIRTUAL_NODE_MODE
-DPARTITION_SIZE 4 4 2
