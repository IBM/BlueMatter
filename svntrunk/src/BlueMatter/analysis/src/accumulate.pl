#!/usr/bin/perl

if (@ARGV < 1) {
    print STDERR "accumulate.pl HistogramKeyWord\n";
    exit(-1);
}

$keyWord = $ARGV[0];

$overFlow = 0;
$count = 0;
while(<STDIN>) {
    chop;
    split;
    $overFlow = $overFlow + int(pop(@_));
    shift(@_); # take out stdout[0]:
    shift(@_); # take out keyword
    $timeStep = shift(@_);
        for ($i = 0; $i < scalar(@_); $i++) {
            if ($count == 0) {
                $hist[$i] = $_[$i];
            } else {
                $hist[$i] = $hist[$i] + $_[$i];
            }
        }
    $count++;
}
for ($i = 0; $i < scalar(@hist); $i++) {
    print $i, " \t", $hist[$i], "\n";
}

