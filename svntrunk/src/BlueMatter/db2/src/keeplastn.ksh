#!/usr/bin/ksh

# deletes all but most recent N backups
# command line args:
# backupDir -- base directory for backups
# dbName -- database name (full path to backups is: $backupDir/$dbName/$DB2INSTANCE/online
# N -- count of most recent backups to keep

let arg_count=$#
if ((arg_count < 3)); then
	    print "$0 backupBaseDir dbName N"
	    exit;
fi

backupDir=$1
dbName=$2
N=$3
print "backupDir = $backupDir\ndbName = $dbName\nN = $N"
if [[ -z $DB2INSTANCE ]]; then
    print "DB2INSTANCE undefined"
    return -1
fi
dbNameUp=`print $dbName | tr [a-z] [A-Z]`
print $dbNameUp
set -A fList `ls -t $backupDir/$DB2INSTANCE/$dbName/online/$dbNameUp.*`
print "${#fList[*]} backups in $backupDir/$DB2INSTANCE/$dbName/online before cleanup"
for fn in ${fList[*]}
do
  print $fn
done
let count=$N
while (( $count < ${#fList[*]} )); do
	    fn=${fList[count]}
	    print $fn
	    /usr/bin/rm $fn
	    let count="count + 1"
done
set -A after `ls -t $backupDir/$DB2INSTANCE/$dbName/online/$dbNameUp.*`
print "${#after[*]} backups in $backupDir/$DB2INSTANCE/$dbName/online after cleanup"
