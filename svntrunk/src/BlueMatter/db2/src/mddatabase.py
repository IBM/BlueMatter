#! /usr/bin/env python

from __future__ import print_function
from DB2 import *
import sys
import os
import pwd
import mx.DateTime
import time

class MDDatabase:
  "Class encapsulating connectivity and operations on the MD database"
  DefaultDBName = 'mdsetup'
  DefaultUserName = 'suits'
  def __init__(self):
    self.dbName = os.getenv('BG_MD_DATABASE')
    if self.dbName == None or self.dbName == '':
      self.dbName = self.DefaultDBName
    try:
      self.conn = Connection(self.dbName);
      self.cursor = self.conn.cursor();
    except:
      print('Failed to connect to database ', self.dbName)
      sys.exit(-1)
    try:
      self.USERID = pwd.getpwuid(os.getuid())[0].upper()
    except:
      print('Warning - Failed to get USERID')
      self.USERID = 'Unknown'

  def Close(self):
    self.cursor.close()
    self.conn.close()  

  def ParseCompoundName(self, CompoundName):
    "Decompose colon separated schema:name compound name.  If no colon, use default user name in class."
    if CompoundName.find(':') > -1:
      (TableUser, CommonName) = CompoundName.split(':')
    else:
      TableUser = self.DefaultUserName
      CommonName = CompoundName
    return (TableUser, CommonName)

  def ChangeSysClass(self, SysID, NewClass):
    "Convert sysclass of existing system, but prevent downcast from production to testing"
    cmd = "select sys_id, sys_desc, sys_class, creator, created from mdsystem.system where sys_id = " + SysID 

    self.cursor.execute(cmd)

    foo = self.cursor.fetchone()
    if not foo:
      print("row with sys_id = ", SysID, " does not exist")
      return

    if foo[2].upper() == NewClass.upper():
      print("System %s already has class %s - Class left unchanged" % (SysID, NewClass))
      return

    if foo[2].upper() == 'PRODUCTION' and NewClass.upper() == 'TESTING':
      print("System %s has class PRODUCTION - Conversion to TESTING not allowed here.  Class left unchanged." % SysID)
      return


    print("Before update:")
    print('%8s %16s %10s %16s %16s' % ('sys_id','sys_desc','sys_class','creator','created'))
    print('%8s %16s %10s %16s %16s' % foo)

    if (self.USERID != foo[3].upper()):
      print("System created by", foo[3].upper(), "not you")
      return

    update = "update mdsystem.system set sys_class = \'" + NewClass + "\' where sys_id = " + SysID
    try:
      self.cursor.execute(update)
    except:
      print("Failed sysclass update using:")
      print(update)
      return

    print("After update:")

    self.cursor.execute(cmd)
    foo = self.cursor.fetchone()
    if not foo:
      print("Error in select after update")
    else:
      print('%8s %16s %10s %16s %16s' % ('sys_id','sys_desc','sys_class','creator','created'))
      print('%8s %16s %10s %16s %16s' % foo)

  def InsertCommonName(self, CompoundName, SysID, NewClass='testing'):
    "Insert common name into table and change class of the referenced system.  Allows changing an existing name."
    (TableUser, CommonName) = self.ParseCompoundName(CompoundName)

    TimeStamp = mx.DateTime.TimestampFromTicks(time.time())

    check = "select * from %s.common_name where name='%s'" % (TableUser, CommonName)
    self.cursor.execute(check)
    foo = self.cursor.fetchone()

    verb = 'Inserted'
    if foo:
      if int(foo[1]) == int(SysID):
	print("Common name %s is already assigned to system %s - No action taken" % (CommonName, SysID))
	return
      cmd = "update %s.common_name set sys_id=%s, created='%s' where name='%s'" % (TableUser, SysID, TimeStamp, CommonName)
      verb = 'Updated'
    else:
      self.ChangeSysClass(SysID, NewClass)
      cmd = "insert into %s.common_name ( name, sys_id, created ) values ( '%s', %s, '%s' ) " % \
            (TableUser, CommonName, SysID, TimeStamp)
  
    try:
      self.cursor.execute(cmd)
      print("%s %s %s %s" % (verb, CommonName, SysID, TimeStamp))
    except:
      print("Failure: %s using:" % verb)
      print(cmd) 

  def SysIDFromName(self, CompoundName):
    "Get the sys_id of a system referenced by the name in the common_name table"
    (TableUser, CommonName) = self.ParseCompoundName(CompoundName)
    check = "select * from %s.common_name where name='%s'" % (TableUser, CommonName)
    try:
      self.cursor.execute(check)
    except:
      print("Failure to select name from db using query: ", check)
      return -1
    foo = self.cursor.fetchone()
    if foo:
      return int(foo[1])
    else:
      return -1


def PrintHelp():
  print(sys.argv[0], "-file names.txt")
  print(sys.argv[0], "-idfromname commonname")
  print(sys.argv[0], "suits:common_name sysId new_sys_class=(scratch|testing|production)")
  sys.exit(-1)

if __name__ == '__main__':
  loadfromfile = 1
  idfromname = 0
  nargs = len(sys.argv)
  if nargs < 2:
    PrintHelp()
  elif sys.argv[1] == '-idfromname' and nargs == 3:
    idfromname = 1
    CommonName = sys.argv[2]
  elif sys.argv[1] == '-file' and nargs == 3:
    NameFile = sys.argv[2]
  elif nargs < 4:
    PrintHelp()
  else:
    loadfromfile = 0

  time.sleep(1)
  db = MDDatabase()
  time.sleep(1)

  if idfromname:
    print("System %s has SysID %d" % (CommonName, db.SysIDFromName(CommonName)))
  elif loadfromfile:
    try:
      f = open(NameFile)
    except:
      print("Cannot open file ", NameFile)
      db.Close()
      sys.exit(-1)
    for l in f.readlines():
      s = l.split()
      db.InsertCommonName(s[0], s[1])
    f.close()
  else:
    db.InsertCommonName(sys.argv[1], sys.argv[2], sys.argv[3])

  db.Close()
