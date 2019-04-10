#!/usr/bin/perl

# takes command line args consisting of:
# arg 1: number of lines to skip at start of file
# additional args:  a list of column ordinals (1,...)
# to be extracted from the file

if (@ARGV < 2) {
    print "extract.pl skipLines firstCol secondCol ... nthCol\n";
    exit(-1);
}

$skipCount = $ARGV[0];
print "# skip count = $skipCount\n";
for ($i = 1; $i < @ARGV; $i=$i+1) {
    print "# Column $i = column $ARGV[$i] from input file\n";
}

while ($skipCount > 0) {
    $foo = <STDIN>;
    $skipCount = $skipCount - 1;
}

while (<STDIN>) {
    s/^\s+//;
    split;
    for ($i = 1; $i < @ARGV; $i=$i+1) {
        print "$_[$ARGV[$i]]      \t";
    }
    print "\n";
}

