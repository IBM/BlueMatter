#!/usr/bin/ksh

# backs up specified database to backup directory
# (online backup with required logs)

dbName=$1
dbBaseDir=$2
. $HOME/.profile # need to run this if invoked by cron
if [[ -z $DB2INSTANCE ]]; then
    print "DB2INSTANCE undefined"
    return -1
fi
mkdir -p $dbBaseDir/$DB2INSTANCE/$dbName/online
db2 backup db $dbName online to $dbBaseDir/$DB2INSTANCE/$dbName/online compress include logs
