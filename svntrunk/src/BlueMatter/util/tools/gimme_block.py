#!/usr/bin/env python
# ####################################################################
#
#
# ####################################################################

import DB2
import sys
import os
import time
import string
    
class MMCS:
    def __init__(self):
        (self.__mmcs_in, self.__mmcs_out)=os.popen4("/usr/bin/ksh", 1)
        self.__mmcs_fdout = self.__mmcs_out.fileno()

    def setusername(self, user_name):
        self.__mmcs_in.write("bgwmmcs setusername " + user_name + "\n")
        self.__mmcs_in.flush()
    
    def do_cmd(self, cmd, timeout=300):
        self.__mmcs_in.write("bgwmmcs " + cmd + "\n")
        self.__mmcs_in.flush()
        tries = 0
        seen = 0
        while seen < 2 and tries < timeout:
            buffer2 = os.read(self.__mmcs_fdout, 128)
            # print "len(buffer2) =", len(buffer2)
            buffer = buffer2.strip()
            # if len(buffer) > 0:
            #    print buffer
            # sys.stdout.flush()
            
            if len(buffer) == 0:
                time.sleep(1)
                tries = tries + 1
                continue
            current = 0
            while current < len(buffer):
                # print "current =", current, " seen =", seen, " tries =", tries
                if seen == 1:
                    current2 = string.find(buffer, "OK", current)
                    if current2 < 0:
                        sys.stderr.write(cmd + "FAILED with " + buffer[current:] + "\n")
                        seen = 3
                    else:
                        seen = 2
                    break
                # print "find(",buffer,",",cmd,",",current,")"
                current = string.find(buffer, cmd, current)
                # print "current =", current
                # sys.stdout.flush()
                if current < 0:
                    break
                else:
                    current = current + len(cmd)
                    # print cmd, "seen"
                    sys.stdout.flush()
                    seen=1
        if seen > 2:
            return "Failed"
        return None
    
    def free_block(self, blockid, owner, status, jobid, username, job_status):
        if jobid > 0: # there is a job running in this block
            self.setusername(username)
            self.do_cmd( "killjob " + blockid + " " + str(jobid) )
            self.do_cmd( "waitjob " + blockid + " " + str(jobid) )
        self.setusername(owner)
        self.do_cmd( "free " + blockid )
        conn=DB2.connect('bgdb0','bglsysdb','db24bgls');
        cmd="select status from bglsysdb.tbglblock " +\
             " where blockid=\'" + blockid + "\'"
        cur=conn.cursor()
        status=''
        waitCount = 0;
        while status != 'F' and waitCount < 10:
            time.sleep(10)
            cur.execute(cmd)
            status=cur.fetchone()[0]
            waitCount = waitCount + 1
        if waitCount >= 10:
            return status
        return None
            


    def allocate_block(self, blockid, userid):
        self.setusername(userid)
        return self.do_cmd("allocate " + blockid)
    
    def close(self):
        self.__mmcs_in.close()
        self.__mmcs_out.close()

def overlapping(target):
    conn=DB2.connect('bgdb0','bglsysdb','db24bgls');
    cmd = "select bglsysdb.tbglbpblockmap.blockid," +\
          " bglsysdb.tbglblock.owner," +\
          " bglsysdb.tbglblock.status," +\
          " coalesce(bglsysdb.tbgljob.jobid, -1)," +\
          " coalesce(bglsysdb.tbgljob.username, 'nobody')," +\
          " coalesce(bglsysdb.tbgljob.status, ' ')" +\
          " from" +\
          "  (bglsysdb.tbgljob right join" +\
          "    (bglsysdb.tbglbpblockmap inner join bglsysdb.tbglblock" +\
          "    on bglsysdb.tbglbpblockmap.blockid=bglsysdb.tbglblock.blockid)" +\
          "   on bglsysdb.tbgljob.blockid=bglsysdb.tbglbpblockmap.blockid)" +\
          " where" +\
          "  bglsysdb.tbglbpblockmap.bpid in" +\
          "   (select bpid from bglsysdb.tbglbpblockmap where blockid='" +\
          target + "')" +\
          "  and bglsysdb.tbglblock.status <> 'F'" +\
          "  group by bglsysdb.tbglbpblockmap.blockid," +\
          "           bglsysdb.tbglblock.owner," +\
          "           bglsysdb.tbglblock.status," +\
          "           coalesce(bglsysdb.tbgljob.jobid, -1)," +\
          "           coalesce(bglsysdb.tbgljob.username, 'nobody')," +\
          "           coalesce(bglsysdb.tbgljob.status, ' ')"
    cur=conn.cursor()
    cur.execute(cmd)
    ret=[]
    foo=cur.fetchone()
    while foo:
        ret.append( (foo[0], foo[1], foo[2], foo[3], foo[4], foo[5]) )
        foo=cur.fetchone()
    return(ret)


if len(sys.argv) < 2:
    print sys.argv[0], "blockID <userid>"
    sys.exit(-1)
cmd_out=os.popen("/usr/bin/whoami")
userid=cmd_out.readline().strip()
if len(sys.argv) > 2:
    userid=sys.argv[2]
target=sys.argv[1]
blocks=overlapping(target)
mmcs=MMCS()
for blockid, owner, status, jobid, username, job_status in blocks:
    print blockid.strip(), owner, status, jobid, username, job_status
    if mmcs.free_block(blockid.strip(), owner.strip(), status, jobid, username.strip(), job_status):
        sys.stderr.write("FAILED TO FREE BLOCK: " + blockid.strip() + "\n")
        sys.exit(-1)
    else:
        sys.stdout.write("Freed " + blockid.strip() + "\n")
        sys.stdout.flush()

if mmcs.allocate_block(target, userid.strip()):
    sys.stderr.write("FAILED TO ALLOCATE " + target + "!\n")
    sys.exit(-1)

print "Allocated", target
mmcs.close()
