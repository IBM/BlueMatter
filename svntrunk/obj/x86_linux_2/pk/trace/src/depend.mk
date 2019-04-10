#
# dependents of pktrace.o:
pktrace.o:  ${MAKETOP}pk/trace/src/pktrace.cpp
pktrace.o:  ${MAKETOP}../export/x86_linux_2/usr/include/pk/pktrace.hpp
pktrace.o:  ${MAKETOP}../export/x86_linux_2/usr/include/pk/platform.hpp
pktrace.o:  ${MAKETOP}../export/x86_linux_2/usr/include/pk/XYZ.hpp
pktrace.o:  ${MAKETOP}../export/x86_linux_2/usr/include/pk/fxlogger.hpp
#
# dependents of pktrace_pk.o:
pktrace_pk.o:  ${MAKETOP}pk/trace/src/pktrace_pk.cpp
pktrace_pk.o:  ${MAKETOP}../export/x86_linux_2/usr/include/pk/fxlogger.hpp
pktrace_pk.o:  ${MAKETOP}../export/x86_linux_2/usr/include/pk/pktrace_pk.hpp
#
# dependents of fxlog.o:
fxlog.o:  ${MAKETOP}pk/trace/src/fxlog.cpp
fxlog.o:  ${MAKETOP}../export/x86_linux_2/usr/include/pk/pktrace_pk.hpp
fxlog.o:  ${MAKETOP}../export/x86_linux_2/usr/include/pk/fxlogger.hpp
