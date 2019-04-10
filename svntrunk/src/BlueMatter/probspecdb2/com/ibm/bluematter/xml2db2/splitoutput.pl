#!/usr/agora/bin/perl

$openFile = 0;

#path from the current directory
open FILE, $ARGV[0] or die "Cannot open $ARGV[0] for input";
$system_id       = $ARGV[1];
$delDirectory    = $ARGV[2];
# print "dir: $delDirectory\n";

print join(' ', @ARGV). "\n";

while(<FILE>) {
    if ($openFile == 1) {
	if (m/^\*\*\*END/) {
	    close(DELFILE);
	    $openFile = 0;
	    next;
	}
	if (m/^\s+$/) {  # skip blank lines
	    next;
	}
	$tempString = $_;
	
	if ($_ ne "\n") {
	  $tempString = "$system_id, $_";  
	}
	
	print DELFILE $tempString;
	next;
    }
    if (m/^\*\*\*BEGIN (\w+\.(DEL))/) {
	unless (open DELFILE, ">$delDirectory/$1") {
	    print STDERR "Cannot open $delDirectory/$1 for output: $!\n";
	    return;
	}
	system("chmod a+r $delDirectory/$1");
	$openFile = 1;
    }
}
 
