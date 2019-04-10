# Main Opts
-qnolm
-qarch=ppcgr
-qtune=pwr3
-qmaxmem=-1
-lpthread
-bmaxstack:0x10000000
-bmaxdata:0x40000000
-qpriority=0
-qalign=natural
-qsuppress=1540-1087:1540-1089:1540-1088:1540-1090

# Expl options
-qstaticinline
-qnounwind
-qdebug=inl:inl24
-qdebug=maxgridiculous:sqrtextra:regpres
-qdebug=newsched1:newsched2
-qdebug=nhwspilmv
-qdebug=ebbpass1:ebbpass2
-qdebug=modcst5
-qdebug=except
-qdebug=inldiff
#-O3
#-qdebug=plst3:cycles
#-qsource 
#-qlist 
#-qshowinc

# Profiling
#profiled
