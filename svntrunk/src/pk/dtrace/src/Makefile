#
# NAME:        Makefile 
#
# AUTHOR:      <<Author>>
#
# VERSION:     1.1
#
# DATE:        97/01/20
#
# COPYRIGHT:   %copyright%
#
# SYNOPSIS:
# N/A
#
# DESCRIPTION:
# Makefile for the tracer application
#
# CAVEATS/WARNINGS:
# <<None.>>
#
# RETURNED VALUES:
# N/A
#
# GENERATED MESSAGES:
# N/A
#
# PRODUCT TRACE:
# N/A
#
# SIDE EFFECTS:
# N/A
#
# PERFORMANCE:
# N/A
#
# SPECIAL CONSIDERATIONS:
# N/A
#
# RELATED INFORMATION:
# N/A
#
# BUILD OPTIONS:
# N/A
#
# DESIGN:
# N/A
#

APP_NAME  = tracer
# -qcpluscmt
CC        = xlC
#CFLAGS    = -c -+ -g -DTRACE_DEST_PK -qidirfirst -DNDEBUG -qnokeyword=bool -qnokeyword=true -qnokeyword=false -qrtti -DNO_PK_PLATFORM -DPK_AIX -DPK_SMP
#-qhd -qinfo=all:novft:nogen -qnokeyword=bool -qnokeyword=true -qnokeyword=false -qidirfirst -L/usr/ibmcxx/lib -I/usr/ibmcxx/include/
CFLAGS    = -c -+ -g -O2 -qrtti   -DTRACE_DEST_PK  -DNO_PK_PLATFORM -DPK_AIX -DPK_SMP
CPPLFLAGS = -qhsflt -qrtti  -DTRACE_DEST_PK -DNO_PK_PLATFORM -DPK_AIX -DPK_SMP -L. -L../../../../../export/rios_aix_5/usr/lib/  -L/usr/vacpp/lib  -L/usr/lpp/xlC/lib -L/usr/lib
INCLUDE   = -I../inc -I../../../../../export/rios_aix_5/usr/include/pk -I../../../../../export/rios_aix_5/usr/include/ -I/usr/vacpp/include/  -I/usr/include  
#MOTLIBS   = -lC -lXm -lXt -lX11 -libmcls -lm -lpthreads -lpktrace -lcxxbase  -lvaccl5 -liocclns -lioc
MOTLIBS   = -lC -lXm -lXt -lX11 -libmcls -lm -lpthreads -lpktrace -lcxxbase   

#-lcxxui
#-lhd

LINK_FLAGS = -bnoquiet

##
## Debugging Flags
##
##

TFILE_CLASS_D_FLAGS = -DPK_SYSTEM_MAIN -DPKFXLOG \
		-DFILE_FILL_BUCKET=0 \
		-DFILE_BINARY_SEARCH=0 \
		-DFILE_SEARCH_UPPER=0 \
		-DFILE_SEARCH_LOWER=0


TITEM_CLASS_D_FLAGS = -DPKFXLOG \
		-DITEM_DRAW_NO_VERT
##
##

C_FILES = tracer_main.C \
	  tracer_lib.C \
	  pktrace_pk.cpp \
	  Tcre_set_dialog_class.C \
	  Tfile_class.C \
	  Tgroup_class.C \
	  Titem_class.C \
          Ttime_mark_class.C \
	  Tapplication_class.C \
	  Twindow_class.C \
	  Tset_scale_dialog.C \
	  Tproperties_dialog.C \
	  Tgroup_properties_dialog.C \
	  Tgroup_save_dialog.C \
	  Tgroup_file_class.C \
	  Tload_set_dialog.C \
	  Ttimer_class.C \
	  Texport_data_dialog.C \
	  Tmessage_dialog.C \
          Texported_file_dialog.C \
	  Tgnuplot_dialog.C \


OBJS    = tracer_main.o \
	  tracer_lib.o \
	  pktrace_pk.o \
	  Tcre_set_dialog_class.o \
	  Tfile_class.o \
	  Tgroup_class.o \
	  Titem_class.o \
	  Ttime_mark_class.o \
	  Tapplication_class.o \
	  Twindow_class.o \
	  Tset_scale_dialog.o \
	  Tproperties_dialog.o \
	  Tgroup_properties_dialog.o \
	  Tgroup_save_dialog.o \
	  Tgroup_file_class.o \
	  Tload_set_dialog.o \
	  Ttimer_class.o \
	  Texport_data_dialog.o \
	  Tmessage_dialog.o \
	  Texported_file_dialog.o \
	  Tgnuplot_dialog.o \

tracer :  $(OBJS)
	$(CC) $(CPPLFLAGS) -DTRACE_DEST_PK $(INCLUDE) -o $(APP_NAME) $(LINK_FLAGS) $(OBJS) $(MOTLIBS)

tracer_lib.o :	tracer_lib.C
	$(CC) $(CFLAGS) $(INCLUDE) tracer_lib.C

pktrace_pk.o :	pktrace_pk.cpp
	$(CC) $(CFLAGS) $(INCLUDE) pktrace_pk.cpp

tracer_main.o:	tracer_main.C
	$(CC) $(CFLAGS) $(INCLUDE) tracer_main.C

Tcre_set_dialog_class.o: Tcre_set_dialog_class.C
	$(CC) $(CFLAGS) $(INCLUDE) Tcre_set_dialog_class.C

Tfile_class.o: Tfile_class.C
	$(CC) $(INCLUDE) $(CFLAGS) Tfile_class.C

Titem_class.o: Titem_class.C
	$(CC) $(CFLAGS) $(INCLUDE) Titem_class.C

Tgroup_class.o: Tgroup_class.C
	$(CC) $(CFLAGS) $(INCLUDE) Tgroup_class.C

Ttime_mark_class.o: Ttime_mark_class.C
	$(CC) $(CFLAGS) $(INCLUDE) Ttime_mark_class.C

Tapplication_class.o: Tapplication_class.C
	$(CC) $(CFLAGS) $(INCLUDE) Tapplication_class.C

Twindow_class.o: Twindow_class.C
	$(CC) $(CFLAGS) $(INCLUDE) Twindow_class.C

Tset_scale_dialog.o: Tset_scale_dialog.C
	$(CC) $(CFLAGS) $(INCLUDE) Tset_scale_dialog.C

Tproperties_dialog.o: Tproperties_dialog.C
	$(CC) $(CFLAGS) $(INCLUDE) Tproperties_dialog.C

Tgroup_properties_dialog.o: Tgroup_properties_dialog.C
	$(CC) $(CFLAGS) $(INCLUDE) Tgroup_properties_dialog.C

Tgroup_save_dialog.o: Tgroup_save_dialog.C
	$(CC) $(CFLAGS) $(INCLUDE) Tgroup_save_dialog.C

Tgroup_file_class.o: Tgroup_file_class.C
	$(CC) $(CFLAGS) $(INCLUDE) Tgroup_file_class.C

Tload_set_dialog.o: Tload_set_dialog.C
	$(CC) $(CFLAGS) $(INCLUDE) Tload_set_dialog.C

Ttimer_class.o: Ttimer_class.C
	$(CC) $(CFLAGS) $(INCLUDE) Ttimer_class.C

Texport_data_dialog.o: Texport_data_dialog.C
	$(CC) $(CFLAGS) $(INCLUDE) Texport_data_dialog.C

Tmessage_dialog.o: Tmessage_dialog.C
	$(CC) $(CFLAGS) $(INCLUDE) Tmessage_dialog.C

Texported_file_dialog.o: Texported_file_dialog.C
	$(CC) $(CFLAGS) $(INCLUDE) Texported_file_dialog.C

Tgnuplot_dialog.o: Tgnuplot_dialog.C
	$(CC) $(CFLAGS) $(INCLUDE) Tgnuplot_dialog.C

clean:
	rm $(OBJS)
	rm $(APP_NAME)
	rm core
 
clean_objs:
	rm $(OBJS)
	rm core

lint:
	lint $(INCLUDE) -C -wClassPDO -Nn10000 -Nd10000 $(C_FILES)

