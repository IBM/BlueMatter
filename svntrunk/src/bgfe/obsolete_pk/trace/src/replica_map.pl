#!/usr/bin/perl

# routine to parse log-lines from output file, pick out mapping lines, and
# create mapping file on stdout

if (@ARGV < 1) {
    print "replica_map.pl fileName\n";
    exit(-1);
}

$fileName=$ARGV[0];

open(GREPPROC, "grep \"myReplicaID\" $fileName | sort -k9,9n -k7,7n |");

$maxRep=0;
while(<GREPPROC>) {
    split();
    $rank = $_[6];
    $replica = $_[8];
    chop($replica);
    $repMap[int($rank)]=int($replica);
    $maxRep=$replica > $maxRep ? $replica : $maxRep;
    print "$rank  $replica\n";
}
$maxRep++;
close(GREPPROC);

for ($i=0; $i < $maxRep; $i++) {
    $fName[$i]=$fileName . "_" . $i;
    $ftemp=$fName[$i];
    unless (open($ftemp, ">$fName[$i]")) {
        print STDERR "Unable to open $fName for output: $!\n";
        exit(-1);
    }
}

open(LOGHANDLE, $fileName);
while(<LOGHANDLE>) {
    if (m/^stdout\[(\d+)\]/) {
        $fHandle=$fName[$repMap[int($1)]];
        print($fHandle ($_));
    }
}

for ($i=0; $i < $maxRep; $i++) {
    $fHandle=$fName[int($i)];
    close($fHandle);
}

