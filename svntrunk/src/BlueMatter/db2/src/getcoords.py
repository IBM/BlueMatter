#!/usr/bin/env python
import DB2
import sys

# This utility retrieves a single coordinate/dvs file from the
# experiment.coords_table table by coord_id and puts it to stdout
# usage: getcoords.py coord_id dbName
# where coord_id is an integer and dbName is usually mdsetup

if len(sys.argv) < 3:
    sys.stderr.write(sys.argv[0] + " coord_id dbName\n")
    sys.exit(1)
dbName=sys.argv[2]
coordID=sys.argv[1]

cmdstr="select snapshot from experiment.coords_table where coord_id=" \
        + str(coordID)
# sys.stderr.write(cmdstr)
conn = DB2.Connection(dbName)
curs = conn.cursor()

curs.execute(cmdstr)
dvs=curs.fetchone()
sys.stdout.write(str(dvs[0]))


