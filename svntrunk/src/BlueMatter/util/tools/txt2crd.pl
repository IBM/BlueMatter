#!/usr/bin/perl

$pdbfile = shift(@ARGV);
$txtfile = shift(@ARGV);
($prefix = $txtfile) =~ s/\.xyz//;
$crdfile = "$prefix.crd";

open(PDBIN, "<$pdbfile" || die "Cannot open crd file $crdfile \n ");

while (<PDBIN>) {
  if ($_ =~ /^ATOM/) {
    ++$site;
    $atomtype[$site] = substr($_,13,4);
    chomp;
    split;
    $atomnum[$site]  = $_[1];
    $restype[$site]  = $_[3];
    $resnum[$site]   = $_[4];
  }
}

close(PDBIN);

open(TXTIN, "<$txtfile" || die "Cannot open crd file $crdfile \n ");
open(CRDOUT,">$crdfile" || die "Cannot open pdb file $pdbfile \n ");

$natom = <TXTIN>;
if($natom != $site) {
  print "Total atoms in $txtfile is not the same as in $pdbfile \n";
  die;
} else {
  print " Converting $txtfile to $crdfile \n";
}

print CRDOUT "* REMARK txt2crd \n";
print CRDOUT "* \n";
print CRDOUT " $natom";

for ($i = 1; $i <= $natom; ++$i) {
  $_ = <TXTIN>;
  $a = substr($_,2,10);
  $b = substr($_,14,10);
  $c = substr($_,26,10);
  printf CRDOUT "%5d%5d %3s %4s%10.5f%10.5f%10.5f PRO  %-5d  .00000\n",$atomnum[$i],$resnum[$i],$restype[$i],$atomtype[$i],$a,$b,$c,$resnum[$i];
}

close(TXTIN);
close(CRDOUT);

