#!/usr/bin/perl
# Assume that file has a name of the form xxxx.crd or xxxx.crd.bz2
if (@ARGV < 2) {
    print STDERR "charmmcrd2dvs.pl fileName crdDir\n";
    return(-1);
}

$fileIn   = $ARGV[0];
$crddir = $ARGV[1];

$bgbindir  = $ENV{"BG_BIN_DIR"};
# $sbbasedir = $ENV{"SANDBOXBASE"};
$dvsconv   = "$bgbindir/setup/dvsconv";
# $crddir = "$sbbasedir/src/mdtest/data/charmm22/";
$datadir = "./";

$fileIn =~ m/^(.*)\.crd/;
$prefix = $1;
if ($fileIn =~ m/\.bz2$/) {
  open(CRDIN, "bunzip2 -k -c $crddir/$fileIn|") || die "Cannot bunzip2 $crddir/$fileIn\n";
}
else {
    open(CRDIN, "<$crddir/$fileIn") || die "Cannot open $crddir/$fileIn\n";
}

$dvsfile  = "$prefix.dvs";
$dvs2file = "$prefix.charmm.aix.dvs";

print "$crddir/$fileIn\n";

$sitecount = 0;

while (<CRDIN>) {
    if (m/^\*/) {
	next;
    }
    chomp;
    split;
    if (@_ < 7) {
	next;
    }
    ++$sitecount;
    $x[$sitecount] = $_[4];
    $y[$sitecount] = $_[5];
    $z[$sitecount] = $_[6];
#	    print "$x[$sitecount] $y[$sitecount] $z[$sitecount] \n";
#      }
}
close(CRDIN);

print "writing $dvsfile .....\n";
    
open(DVSOUT,">$dvsfile") || die "Cannot open dvs file $dvsfile \n ";

print DVSOUT "$sitecount \n";
    
for ($i = 1;$i <= $sitecount; ++$i) {
    printf DVSOUT "%12.5f %12.5f %12.5f \n",$x[$i],$y[$i],$z[$i];
}

print DVSOUT "$sitecount \n";
    
for ($i = 1;$i <= $sitecount; ++$i) {
    print DVSOUT "     0.00000      0.00000      0.00000\n";
} 

close(DVSOUT);

system "($dvsconv $dvsfile $dvs2file)";

exit(0);





