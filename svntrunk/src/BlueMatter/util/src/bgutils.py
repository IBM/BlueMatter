from __future__ import print_function
# utilities to define paths and such for the bluegene application environment

import os
import os.path
import sys
import glob
#from DB2 import *
import DB2

def installbase():
    if os.environ.has_key("PACKAGEBASE"):
        return os.environ["PACKAGEBASE"] + '/shipdata/'
    elif os.environ.has_key("SANDBOXBASE"):
        return os.environ["SANDBOXBASE"] + '/inst.images/' + os.environ["CONTEXT"] + '/shipdata/'
    elif os.environ.has_key("BG_INSTALL_BASE"):
        return os.environ["BG_INSTALL_BASE"] + '/'
    return '/'

def ffinfo():
    return os.path.normpath(str(installbase() + 'usr/opt/bluegene/forcefields'))

def exeInfo(dbName, exeId):
    conn = DB2.Connection(dbName)
    cur = conn.cursor()
    cmd = "select sys_id, ctp_id from experiment.system_source where source_id in (select source_id from experiment.executable where executable_id = " + str(exeId) + ")"
    cur.execute(cmd)
    foo = cur.fetchone()
    return (foo[0], foo[1])

class PlatformInfo:
    def __init__(self, dbName):
        self.__conn = DB2.Connection(dbName)
        self.__cursor = self.__conn.cursor()
        pathstr = installbase() + '/usr/opt/bluegene/bin/setup/*.ref'
        self.__refFileList = glob.glob(pathstr)
        self.__keyMap = {}
        for i in self.__refFileList:
            refFile = open(i,"r")
            line = refFile.readline().split();
            implId = line[0]
            platId = line[1]
            foo = os.path.split(i)
            key = os.path.splitext(foo[1])[0]
            self.__keyMap[key] = (implId, platId)
    def choices(self):
        return self.__keyMap

    def implParams(self, implId):
        cmd = "select name, value, literal from experiment.impl_params where impl_id = " + str(implId) + " order by impl_id"
        self.__cursor.execute(cmd)
        print("%30s %30s %15s" % ("NAME", "VALUE", "LITERAL"))
        foo = self.__cursor.fetchone()
        while(foo):
            print("%30s %30s %15s" % (foo[0], foo[1], foo[2]))
            foo = self.__cursor.fetchone()
        return

    def platformParams(self, platId):
        cmd = "select name, value, type from experiment.platform_params where platform_id = " + str(platId) + " order by platform_id"
        self.__cursor.execute(cmd)
        print("%30s %30s %15s" % ("NAME", "VALUE", "TYPE"))
        foo = self.__cursor.fetchone()
        while(foo):
            print("%30s %30s %15s" % (foo[0], foo[1], foo[2]))
            foo = self.__cursor.fetchone()
        return
