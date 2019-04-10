#!/usr/bin/perl

# always invoked by another script that specifies the database

if (@ARGV < 1) {
    print STDERR "clean_system dbName\n";
    exit(-1);
}

$dbName = $ARGV[0];
open(SELECTHANDLE, "db2 connect to $dbName;db2 \"select sys_id from mdsystem.system where created + 30 days < CURRENT TIMESTAMP and sys_class='scratch'\";db2 connect reset |");

$selMode = 0;

while(<SELECTHANDLE>) {
    if ($selMode == 0) {
	if (m/^-/) {
	    $selMode = 1;
	}
    }
    else {
	if (m/^\s+(\d+)/) {
	    push(@sysList, $1);
	}
	else {
	    last;
	}
    }
}

open(DELHANDLE, "| db2 -t");
print DELHANDLE "connect to $dbName;\n";
foreach $sysID (@sysList) {
    print "delete from mdsystem.system where sys_id=$sysID and sys_class='scratch';\n";
    print DELHANDLE "delete from mdsystem.system where sys_id=$sysID and sys_class='scratch';\n";
    print DELHANDLE "commit;\n";
}
print DELHANDLE "connect reset;\n";
close(DELHANDLE);

