#!/usr/bin/ksh

# simple script to tag (rtag) mdtest module as of the most recent hour
# takes a single argument, the symbolic tag

if (( $# <= 0 )) then
    echo $0 "symbolicTagName"
    exit
fi
now=`date "+%Y-%m-%d %H:00"`
echo $now
#cmd="cvs -d /.../watson.ibm.com/fs/proj/G/gfe/code/CVS rtag -D $now $1 mdtest"
#echo $cmd
cvs -d /.../watson.ibm.com/fs/proj/G/gfe/code/CVS rtag -D "$now" $1 mdtest
