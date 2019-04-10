#!/usr/bin/env python
import DB2
import sys
import os
import mx.DateTime
import time

if len(sys.argv) < 6:
    print sys.argv[0], "sysID waterModel dvsFileName \"annotation\" dbName"
    sys.exit(1)
sysID=sys.argv[1]
waterModel=sys.argv[2]
dvsFile=sys.argv[3]
annotation=sys.argv[4]
dbName=sys.argv[5]

dvsBaseName=os.path.basename(dvsFile)

created = mx.DateTime.TimestampFromTicks(time.time())

cmdstr = "insert into experiment.coords_table " \
+ "(coord_id, sys_id, solvent_model, file_name, annotation, created, snapshot) " \
+ "values (DEFAULT, " \
+ str(sysID) + ", " \
+ "\'" + str(waterModel) + "\', " \
+ "\'" + str(dvsBaseName) + "\', " \
+ "\'" + str(annotation) + "\', " \
+ "\'" + str(created) + "\', " \
+ "?)"
#print cmdstr

conn = DB2.Connection(dbName)
curs = conn.cursor()

curs.execute(cmdstr, (DB2.BLOB(dvsFile) ) )

querystr = "select coord_id from experiment.coords_table where sys_id=" \
           + str(sysID ) \
           + " and solvent_model=\'" + str(waterModel) + "\' and " \
           + "file_name=\'" + str(dvsBaseName) + "\' and " \
           + "annotation=\'" + str(annotation) + "\' and " \
           + "created=\'" + str(created) + "\'"
	 
curs.execute(querystr)	 
print 'Coord Ids for SysId = ' + str(sysID)
foo = curs.fetchone()
while (foo):
     print '%20d' % foo[0]
     foo = curs.fetchone()


