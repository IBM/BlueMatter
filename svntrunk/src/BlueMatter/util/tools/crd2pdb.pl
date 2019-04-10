#!/usr/bin/perl

$crdfile = shift(@ARGV);
($prefix  = $crdfile) =~ s/\.crd//;
$pdbfile = "$prefix.pdb";

print "Converting $crdfile to $pdbfile \n";
open(CRDIN, "<$crdfile" || die "Cannot open crd file $crdfile \n ");
open(PDBOUT,">$pdbfile" || die "Cannot open pdb file $pdbfile \n ");

$check = 0;
$site = 0;

while (<CRDIN>) {
  if ($_ =~ /^\*/) {
  } elsif ($check == 0) {
    $check = 1;
  } else {
    ++$site;
    $resnum[$site]  = substr($_,56,5);
    $resname[$site] = substr($_,11,4);
    $atname[$site]  = substr($_,16,4);
    $x[$site]       = substr($_,21,10);
    $y[$site]       = substr($_,31,10);
    $z[$site]       = substr($_,41,10);
    printf PDBOUT "ATOM  %5d  %4s%4s%5d    %8.3f%8.3f%8.3f \n",$site,$atname[$site],$resname[$site],$resnum[$site],$x[$site],$y[$site],$z[$site];
  }
}

close(CRDIN);
close(PDBOUT);

