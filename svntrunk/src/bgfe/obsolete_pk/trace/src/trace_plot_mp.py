#! /usr/bin/env python

from __future__ import print_function
from DB2 import *
import sys
import os
import os.path
import pwd

# processes a trace file and generates metapost to plot it out
# command line args: dbName trace_id minTimeStep nSteps <bracketName = TimeStep__0>

bracketName = "TimeStep__0"
if len(sys.argv) < 5:
    print(sys.argv[0], "dbName trace_id minTimeStep nSteps <bracketName = TimeStep__0>")
    sys.exit(-1)
dbName = sys.argv[1]
traceSetId = sys.argv[2]
minTimeStep = sys.argv[3]
nSteps = sys.argv[4]
if len(sys.argv) > 5:
    bracketName = sys.argv[5]
label = []
conn = Connection(dbName)
cursor = conn.cursor()
cursor2 = conn.cursor()
cmd = "select node_count, bin_file from trace_set where trace_set_id =" + traceSetId
cursor.execute(cmd)

print("% BEGINPRE")
print("verbatimtex")
print("\documentclass{article}")
print("\\usepackage[latin1]{inputenc}")
print("\\usepackage[T1]{fontenc}")
print("\\usepackage{textcomp}")
print("\\usepackage{mathptmx}")
print("\\usepackage[scaled=.92]{helvet}")
print("\\usepackage{courier}")
print("\\usepackage{latexsym}")
print("\begin{document}")
print("etex")
print("% ENDPRE")
print("")
print("warningcheck:=0;")
print("defaultmpt:=mpt:=4;")
print("th:=.6;")
print("%% Have nice sharp joins on our lines")
print("linecap:=butt;")
print("linejoin:=mitered;")
print("")
print("def scalepen expr n = pickup pencircle scaled (n*th) enddef;")
print("def ptsize expr n = mpt:=n*defaultmpt enddef;")
print("")
print("")
print("textmag:= 1.000;")
print("vardef makepic(expr str) =")
print("  if picture str : str scaled textmag")
print("  % otherwise a string")
print("  else: str infont defaultfont scaled (defaultscale*textmag)")
print("  fi")
print("enddef;")
print("")
print("def infontsize(expr str, size) =")
print("  infont str scaled (size / fontsize str)")
print("enddef;")
print("")
print("color col[];")
print("col[-2]:=col[-1]:=col0:=black;")
print("% RED");
print("col13 := (0.996094, 0.796875, 0.796875);");
print("col37 := (0.996094, 0.597656, 0.597656);");
print("col61 := (0.996094, 0.398438, 0.398438);");
print("col1 := (0.996094, 0.199219, 0.199219);");
print("col73 := (0.796875, 0, 0);");
print("col49 := (0.597656, 0, 0);");
print("col25 := (0.398438, 0, 0);");
print("% ORANGE");
print("col14 := (0.996094, 0.898438, 0.796875);");
print("col38 := (0.996094, 0.796875, 0.597656);");
print("col62 := (0.996094, 0.695312, 0.398438);");
print("col2 := (0.996094, 0.597656, 0.199219);");
print("col74 := (0.796875, 0.398438, 0);");
print("col50 := (0.597656, 0.296875, 0);");
print("col26 := (0.398438, 0.199219, 0);");
print("% YELLOW");
print("col15 := (0.996094, 0.996094, 0.796875);");
print("col39 := (0.996094, 0.996094, 0.597656);");
print("col63 := (0.996094, 0.996094, 0.398438);");
print("col3 := (0.996094, 0.996094, 0.199219);");
print("col75 := (0.796875, 0.796875, 0);");
print("col51 := (0.597656, 0.597656, 0);");
print("col27 := (0.398438, 0.398438, 0);");
print("% GREEN");
print("col16 := (0.898438, 0.996094, 0.796875);");
print("col40 := (0.796875, 0.996094, 0.597656);");
print("col64 := (0.699219, 0.996094, 0.398438);");
print("col4 := (0.597656, 0.996094, 0.199219);");
print("col76 := (0.398438, 0.796875, 0);");
print("col52 := (0.300781, 0.597656, 0);");
print("col28 := (0.199219, 0.398438, 0);");
print("% GREEN2");
print("col17 := (0.796875, 0.996094, 0.796875);");
print("col41 := (0.597656, 0.996094, 0.597656);");
print("col65 := (0.398438, 0.996094, 0.398438);");
print("col5 := (0.199219, 0.996094, 0.199219);");
print("col77 := (0, 0.996094, 0);");
print("col53 := (0, 0.796875, 0);");
print("col29 := (0, 0.597656, 0);");
print("% TURQUOISE");
print("col18 := (0.796875, 0.996094, 0.898438);");
print("col42 := (0.597656, 0.996094, 0.796875);");
print("col66 := (0.398438, 0.996094, 0.699219);");
print("col6 := (0.199219, 0.996094, 0.597656);");
print("col78 := (0, 0.796875, 0.398438);");
print("col54 := (0, 0.597656, 0.300781);");
print("col30 := (0, 0.398438, 0.199219);");
print("% TURQUOISE2");
print("col19 := (0.796875, 0.996094, 0.996094);");
print("col43 := (0.597656, 0.996094, 0.996094);");
print("col67 := (0.398438, 0.996094, 0.996094);");
print("col7 := (0.199219, 0.996094, 0.996094);");
print("col79 := (0, 0.796875, 0.796875);");
print("col55 := (0, 0.597656, 0.597656);");
print("col31 := (0, 0.398438, 0.398438);");
print("% BLUE");
print("col20 := (0.796875, 0.894531, 0.996094);");
print("col44 := (0.597656, 0.796875, 0.996094);");
print("col68 := (0.398438, 0.695312, 0.996094);");
print("col8 := (0.199219, 0.597656, 0.996094);");
print("col80 := (0, 0.398438, 0.796875);");
print("col56 := (0, 0.296875, 0.597656);");
print("col32 := (0, 0.199219, 0.398438);");
print("% INDIGO");
print("col21 := (0.796875, 0.796875, 0.996094);");
print("col45 := (0.597656, 0.597656, 0.996094);");
print("col69 := (0.398438, 0.398438, 0.996094);");
print("col9 := (0.199219, 0.199219, 0.996094);");
print("col81 := (0, 0, 0.796875);");
print("col57 := (0, 0, 0.597656);");
print("col33 := (0, 0, 0.398438);");
print("% VIOLET");
print("col22 := (0.894531, 0.796875, 0.996094);");
print("col46 := (0.796875, 0.597656, 0.996094);");
print("col70 := (0.695312, 0.398438, 0.996094);");
print("col10 := (0.597656, 0.199219, 0.996094);");
print("col82 := (0.398438, 0, 0.796875);");
print("col58 := (0.296875, 0, 0.597656);");
print("col34 := (0.199219, 0, 0.398438);");
print("% VIOLET2");
print("col23 := (0.996094, 0.796875, 0.996094);");
print("col47 := (0.996094, 0.597656, 0.996094);");
print("col71 := (0.996094, 0.398438, 0.996094);");
print("col11 := (0.996094, 0.199219, 0.996094);");
print("col83 := (0.796875, 0, 0.796875);");
print("col59 := (0.597656, 0, 0.597656);");
print("col35 := (0.398438, 0, 0.398438);");
print("% PINK");
print("col24 := (0.996094, 0.796875, 0.898438);");
print("col48 := (0.996094, 0.597656, 0.796875);");
print("col72 := (0.996094, 0.398438, 0.699219);");
print("col12 := (0.996094, 0.199219, 0.597656);");
print("col84 := (0.796875, 0, 0.398438);");
print("col60 := (0.597656, 0, 0.300781);");
print("col36 := (0.398438, 0, 0.199219);");

print("beginfig(0);")

foo = cursor.fetchone()
if (foo):
    cmd2 = "select trace_id from trace_id where trace_set_id = " +\
           traceSetId + " and trace_type=0 and trace_label =\'" + bracketName +\
           "\' group by trace_id"
    cursor2.execute(cmd2)
    bar = cursor2.fetchone()
    if bar:
        bracketId = bar[0]
    else:
        print("No bracketId found for", bracketName)
        sys.exit(-1)
    nodeCount = foo[0]
    ts = os.popen2("timestamp2mp " + str(foo[1]) + " " +\
                  str(nodeCount) + " " + str(minTimeStep) + " " +\
                   str(nSteps) + " " + str(bar[0]), 't', 1)
    tsIn = ts[0]
    tsOut = ts[1]
    cmd2 = "with st (start_id, trace_label) as " +\
           "(select distinct trace_id, trace_label from trace_id " +\
           "where trace_type = 0 and trace_set_id = " + str(traceSetId) + "), " +\
           "fi (finish_id, trace_label) as " +\
           "(select distinct trace_id, trace_label from trace_id " +\
           "where trace_type = 1 and trace_set_id = " + str(traceSetId) + ") " +\
           "select start_id, finish_id, st.trace_label " +\
           "from st, fi " +\
           "where st.trace_label = fi.trace_label " +\
           "order by st.trace_label"
    cursor2.execute(cmd2)
    bar = cursor2.fetchone()
    while bar:
        if label.count(bar[2]) == 0:
            label.append(bar[2])
        print("% plotting " + str(bar[2]))
        tsIn.write(str(bar[0]) + " " + str(bar[1]) + " " + str(bar[2]) + "\n")
        tsIn.flush()
        bar = cursor2.fetchone()
    tsIn.close()
    for line in tsOut.xreadlines():
        print(line);
    tsOut.close()
print("endfig;")
print("end;")
