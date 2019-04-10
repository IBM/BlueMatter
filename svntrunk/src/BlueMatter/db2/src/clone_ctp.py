#!/usr/bin/env python

import DB2
import sys
import mx.DateTime
import time
import os.path
import pwd

if len(sys.argv) < 3:
    print >>sys.stderr, sys.argv[0], "dbName ctp_id (to be cloned)"
    sys.exit(-1)
dbName = sys.argv[1]
old_ctp = sys.argv[2]
creator = pwd.getpwuid(os.getuid())[0]
created = mx.DateTime.TimestampFromTicks(time.time())
desc = "created by cloning ctp_id " + str(old_ctp)
fileName = "cloned"
conn = DB2.connect(dbName)
cur = conn.cursor()
cmd = "insert into experiment.ctp_table (ctp_id, created, creator, filename, desc) values(DEFAULT, \'" + str(created) + "\', \'" + str(creator) + "\', \'" +\
      str(fileName) + "\', \'" + str(desc) +"\')"
cur.execute(cmd)
cmd = "select ctp_id from experiment.ctp_table where ctp_id = integer(IDENTITY_VAL_LOCAL())"
cur.execute(cmd)
foo = cur.fetchone()
new_ctp = foo[0]
print "New ctp_id: ", str(new_ctp)
cmd = "insert into experiment.ct_params (ctp_id, type, name, value) select value(" +\
      str(new_ctp) +\
      "," + str(new_ctp) + "), type, name, value from experiment.ct_params where ctp_id = " +\
      str(old_ctp)
cur.execute(cmd)


