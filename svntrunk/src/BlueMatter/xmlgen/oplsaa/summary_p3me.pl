#!/usr/bin/perl

$filelist  = shift(@ARGV);
$summary   = "summary.out";

open(FILIN,"<$filelist");
$nfile = 0;
while (<FILIN>) {
  chomp;
  if ($_ =~ /^\w/) {
    $prefix = $_;
    $nfile++;
    $fileout = "$prefix.opls.p3me.mdlog";
    $file[$nfile] = "$fileout";
  }
}

open(FILOUT,">$summary");
print FILOUT "               Test        TotalE      BondE      AngleE       TorsionE     ImpTorE        vdwE         EwaldK      EwaldSum      vdW14E       Elece14E        WaterE\n";
for ($i = 1; $i <= $nfile; ++$i) {
  open(FILIN,"<$file[$i]");
  while(<FILIN>) {
    #    if ($_ =~ /\-/) {$_ =~ s/\-/ \-/g;}
    chomp;
    split;
    $len = @_;
    if ($_[0] eq "Step") {
      $bondid     = -999;
      $angleid    = -999;
      $torsionid  = -999;
      $ureybid    = -999;
      $improperid = -999;
      $elecid     = -999;
      $vdwid      = -999;
      $waterid    = -999;
      for ($ii = 1; $ii <= $len; ++$ii) {
	if ($_[$ii] eq "TotalE")       {$totalid     = $ii;}
	if ($_[$ii] eq "StdHarmBond")  {$bondid     = $ii;}
	if ($_[$ii] eq "StdHarmAngle") {$angleid    = $ii;}
	if ($_[$ii] eq "OPLSTorsion")  {$torsionid  = $ii;}
	if ($_[$ii] eq "OPLSImproper") {$improperid = $ii;}
	if ($_[$ii] eq "EwaldSum")     {$elecid     = $ii;}
	if ($_[$ii] eq "EwaldK")       {$ewaldkid     = $ii;}
	if ($_[$ii] eq "Coulomb14")    {$elec14id   = $ii;}
	if ($_[$ii] eq "NSQLJ")        {$vdwid      = $ii;}
	if ($_[$ii] eq "LJ14")         {$vdw14id      = $ii;}
	if ($_[$ii] eq "TIP3P")        {$waterid    = $ii;}
      }
    }
    if ($_[0] == "0") {
      $total    = $_[$totalid];
      $bond     = $_[$bondid];
      $angle    = $_[$angleid];
      $ureyb    = $_[$ureybid];
      $torsion  = $_[$torsionid];
      $improper = $_[$improperid];
      $elec     = $_[$elecid];
      $ewaldk   = $_[$ewaldkid];
      $elec14   = $_[$elec14id];
      $vdw      = $_[$vdwid];
      $vdw14    = $_[$vdw14id];
      $water    = $_[$waterid];
    }
  }
  close(FILIN);
  printf FILOUT "%20s %12.6f %12.6f %12.6f %12.6f %12.6f %12.6f %14.6f %14.6f %12.6f %12.6f %12.6f\n",$file[$i],$total,$bond,$angle,$torsion,$improper,$vdw,$ewaldk,$elec,$vdw14,$elec14,$water;
}
close(FILOUT);

