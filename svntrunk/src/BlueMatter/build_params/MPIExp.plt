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

# Expl options
mpi
Compiler mpCC_r
-qstaticinline
-qnounwind
-qdebug=inl:inl24
-qdebug=maxgridiculous:sqrtextra:regpres
-qdebug=newsched1:newsched2
-qdebug=nhwspilmv
-qdebug=ebbpass1:ebbpass2
-qdebug=modcst5
-qdebug=except
-qdebug=plst3:cycles
-qdebug=inldiff
-O3
#-B/var/quad/test/compiler_backends/20030404/afs/torolab/u/mendell/public/blueLight/
#-tbcd 
