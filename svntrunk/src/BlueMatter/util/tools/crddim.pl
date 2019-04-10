#!/usr/bin/perl

$file = shift(@ARGV);

open(CRDIN, "<$file" || die "Cannot open crd file $crdfile \n ");

$check = 0;
$xmin =  9999;
$xmax = -9999;
$ymin =  9999;
$ymax = -9999;
$zmin =  9999;
$zmax = -9999;

while (<CRDIN>) {
  chomp;
  split;
  $x = $_[0];
  $y = $_[1];
  $z = $_[2];
  if ($xmin >= $x) {$xmin = $x;}
  if ($xmax <= $x) {$xmax = $x;}
  if ($ymin >= $y) {$ymin = $y;}
  if ($ymax <= $y) {$ymax = $y;}
  if ($zmin >= $z) {$zmin = $z;}
  if ($zmax <= $z) {$zmax = $z;}
}

print " xmin = $xmin \n";
print " xmax = $xmax \n";
print " ymin = $ymin \n";
print " ymax = $ymax \n";
print " zmin = $zmin \n";
print " zmax = $zmax \n";

close(CRDIN);

