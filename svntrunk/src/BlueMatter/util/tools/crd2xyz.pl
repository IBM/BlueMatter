#!/usr/bin/perl

$crdfile = shift(@ARGV);
($prefix = $crdfile) =~ s/\.crd//;
$dvstxtfile = "$prefix.xyz";

open(CRDIN,"<$crdfile" || die "Cannot open crd file $crdfile \n ");

$sitecount = 0;

while (<CRDIN>) {
  chomp;
  split;
  if ($_[0] ne "*" && $_[6] ne "") {
    ++$sitecount;
    $x[$sitecount] = $_[4];
    $y[$sitecount] = $_[5];
    $z[$sitecount] = $_[6];
  }
}
close(CRDIN);

print "writing $dvstxtfile .....\n";
    
open(DVSOUT,">$dvstxtfile" || die "Cannot open dvs file $crdfile \n ");

print DVSOUT "$sitecount \n";
    
for ($i = 1;$i <= $sitecount; ++$i) {
  printf DVSOUT "%12.5f %12.5f %12.5f \n",$x[$i],$y[$i],$z[$i]
} 

print DVSOUT "$sitecount \n";
    
for ($i = 1;$i <= $sitecount; ++$i) {
  print DVSOUT "     0.00000      0.00000      0.00000\n";
} 

close(DVSOUT);

