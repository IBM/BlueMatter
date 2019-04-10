#!/usr/bin/perl

# script to cat together trace output files from same replica

# command line args:
# baseFileName
# replicaCount
# total node count

if (@ARGV < 3) {
    print STDERR "mergeRemdTrace.pl baseFileName replicaCount nodeCount\n";
    exit(-1);
}

$baseFileName = $ARGV[0];
$shortName = $baseFileName;
$shortName =~ s/.*\/([\w:\.]+$)/\1/;
#print $shortName, "\n";
$replicaCount = int($ARGV[1]);
$totalNodes = int($ARGV[2]);
$nodesPerReplica = int($totalNodes/$replicaCount);

for ($i=0; $i< $replicaCount; $i++) {
    for ($j=0; $j < $nodesPerReplica; $j++) {
        if ($j == 0) {
            $cmd = sprintf("cat %s.%05d:%05d.N.%05d:00000.LOG > %s.%05d.Log", $baseFileName, $i, $replicaCount, $nodesPerReplica, $shortName, $i);
        } else {
            $cmd = sprintf("cat %s.%05d:%05d.N.%05d:%05d.LOG >> %s.%05d.Log", $baseFileName, $i, $replicaCount, $nodesPerReplica, $j, $shortName, $i);
        }
        system($cmd);
    }
}

