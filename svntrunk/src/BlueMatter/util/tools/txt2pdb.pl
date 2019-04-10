#!/usr/bin/perl

$pdbfile = shift(@ARGV);
$txtfile = shift(@ARGV);
($prefix = $txtfile) =~ s/\.xyz//;
$newpdbfile = "$prefix.new.pdb";

open(PDBIN, "<$pdbfile" || die "Cannot open crd file $crdfile \n ");

while (<PDBIN>) {
  if ($_ =~ /^ATOM/) {
    ++$site;
    $header[$site] = substr($_,0,26);
  }
}

close(PDBIN);

open(TXTIN, "<$txtfile" || die "Cannot open crd file $crdfile \n ");
open(PDBOUT,">$newpdbfile" || die "Cannot open pdb file $pdbfile \n ");

#$natom = <TXTIN>;
#if($natom != $site) {
#  print "Total atoms in $txtfile is not the same as in $pdbfile \n";
#  die;
#} else {
  print " Converting $txtfile to $newpdbfile \n";
#}

for ($i = 1; $i <= $site; ++$i) {
  $_ = <TXTIN>;
  chomp;
  split;
  printf PDBOUT "%26s    %8.3f%8.3f%8.3f\n",$header[$i],$_[2],$_[3],$_[4];
}

close(TXTIN);
close(PDBOUT);

