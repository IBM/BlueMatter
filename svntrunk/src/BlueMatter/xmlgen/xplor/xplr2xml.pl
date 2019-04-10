#!/usr/local/bin/perl

### xplr2xml.pl 
### 
### usage:   xplr2xml.pl <parameter file> <psf file> <option> <Number of additional waters> <replication number> <additional psfs ...> ...
###
### example: xplr2xml.pl par_all22_prot.inp enkp.psf 
###          # this generates "enkp.charmm.xml"
###
### example: xplr2xml.pl par_all22_prot.inp enkp.psf 0 4
###          # generates enkp.charmm.xml with 4 additional waters
###
###
### example: xplr2xml.pl par_all22_prot.inp enkp.psf 0 4 3
###          # generates 3 instances of (enkp with 4 additional waters)
###
### example: xplr2xml.pl par_all22_prot.inp enkp.psf 0 4 0 ala.psf
###          # generates enkp and ala with 4 additional waters)
###
### example: xplr2xml.pl enkp.psf 0 4 0 ala.psf pro.ala
###          # generates enkp, ala, pro with 4 additional waters)
###
### example: xplr2xml.pl par_all22_prot.inp enkp.psf 0 0 5 ala.psf pro.psf
###          # generates 5 instances of enkp, ala, pro
###
### option
### nonnull - skip nullifying the water params
### 
### by Alexander Balaeff, IBM, Feb. 2003
### by Mike Pitman, IBM, Mar. 2002,
### by Y.Y. Sham, IBM, Jul. 2001,
### last update Feb. 2003
###############################################################################

## $topfile = shift(@ARGV);
$prmfile = shift(@ARGV);
$psffile = shift(@ARGV);
$option  = shift(@ARGV);
$naddwat = shift(@ARGV);
$NReplicas = shift(@ARGV);


if($NReplicas == 0) {$NReplicas = 1;}

print ("Adding $naddwat waters");


$verbose_output = 0;

if ($option eq "nonull") {$watparam = 1;}

($system = $psffile) =~ s/\.psf//;

# if debug_flow is set, various stages are printed to indicate locality for large systems (poor-man's profiling)
$debug_flow = 1;

$nsite = 0;
$nbond = 0;
$nangle = 0;
$ntorsion = 0;
$nimproper = 0;

##################### read charmm psf file #####################
do
  {
$nsiteoffset = $nsite;

$appendpsffile = shift(@ARGV);
$nfiles = open(PSFIN2,"<$appendpsffile");

if($nfiles == 1)
  {
#    print ("next append file is $appendpsffile\n\n");
    close(PSFIN2);
  }
	       
print ("\n READING CHARMM PSF FILE ...$psffile. \n \n");
open(PSFIN,"<$psffile") || die "Cannot open psf file $psffile \n ";


#skip lines
do {$_=<PSFIN>;} until(/!NATOM/);

#read sitelist
do {
  $_=<PSFIN>;
  chomp;
  if (! $_ =~ /!NBOND/ || $_ =~ /\b\s+\b/) {
    split;
##/** ????? AB ???? **/
    ++$nsite; 
    $mol[$nsite]      = $_[1];
    $resnum[$nsite]   = $_[2];
    $resname[$nsite]  = $_[3];
    $atname[$nsite]   = $_[4];
##    $atid[$nsite]     = $_[5];
    $attype[$nsite]   = $_[5];
    $element[$nsite]  = substr($attype[$nsite],0,1); # A quick fix: assuming that the atom types always start with the character corresponding to their chemical element (N, C, O, H, etc). May need to be more careful in case new atom types not following this rule appear in the force field. -- AB., 4/1/2003.
    $charge[$nsite]   = $_[6];
    $mass[$nsite]     = $_[7];

    if($verbose_output)
    { print "site $nsite resnum $resnum[$nsite] resname $resname[$nsite] atname $atname[$nsite]\n";}
##         All waters get renamed as HOH       ##
    if (($_[3] =~ /TIP/) || ($_[3] =~ /SPC/))
    {      $resname[$nsite] = "HOH"; }

##      Differently charged histidines' names are changed from    ##
##      CHARMm/XPLOR convention to AMBER (?) convention           ##
    if ($_[3] =~ /HSD/)
    {      $resname[$nsite] = "HIS"; }
    if ($_[3] =~ /HSE/)
    {      $resname[$nsite] = "HIE"; }
    if ($_[3] =~ /HSP/)
    {      $resname[$nsite] = "HIH"; }

  }
} until(/!NBOND/);


#read bondlist
do {
  $_=<PSFIN>;
  chomp;
  if (! $_ =~ /!NTHETA/ || $_ =~ /\b\s+\b/) {
    split;
    for ($i = 0; $i <= 3; $i++) {
      if ($_[2*$i] ne "" && $_[2*$i+1] ne "" ) {
	++$nbond;
	$bondatom1[$nbond] = $_[2*$i] + $nsiteoffset;
	$bondatom2[$nbond] = $_[2*$i+1] + $nsiteoffset ;
      }
    }
  }
} until(/!NTHETA/);



#read anglelist
do {
  $_=<PSFIN>;
  chomp;
  if (! $_ =~ /!NPHI/ || $_ =~ /\b\s+\b/) {
    split;
    for ($i = 0; $i <= 2; $i++) {
      if ($_[3*$i] ne "" && $_[3*$i+1] ne "" && $_[3*$i+2] ne "") {
	++$nangle;
	$angleatom1[$nangle] = $_[3*$i] + $nsiteoffset;
	$angleatom2[$nangle] = $_[3*$i+1] + $nsiteoffset;
	$angleatom3[$nangle] = $_[3*$i+2] + $nsiteoffset;
      }
    }
  }
} until(/!NPHI/);

#read torsionlist
do {
  $_=<PSFIN>;
  chomp;
  if (! $_ =~ /!NPHI/ || $_ =~ /\b\s+\b/) {
    split;
    for ($i = 0; $i <= 1; $i++) {
      if ($_[4*$i] ne "" && $_[4*$i+1] ne "" && $_[4*$i+2] ne "" && $_[4*$i+2] ne "") {
	++$ntorsion;
	$torsionatom1[$ntorsion] = $_[4*$i] + $nsiteoffset;
	$torsionatom2[$ntorsion] = $_[4*$i+1] + $nsiteoffset;
	$torsionatom3[$ntorsion] = $_[4*$i+2] + $nsiteoffset;
	$torsionatom4[$ntorsion] = $_[4*$i+3] + $nsiteoffset;
      }
    }
  }
} until(/!NIMPHI/);

#read improperlist
do {
  $_=<PSFIN>;
  chomp;
  if (! $_ =~ /!NPHI/ || $_ =~ /\b\s+\b/) {
    split;
    for ($i = 0; $i <= 1; $i++) {
      if ($_[4*$i] ne "" && $_[4*$i+1] ne "" && $_[4*$i+2] ne "" && $_[4*$i+2] ne "") {
	++$nimproper;
	$improperatom1[$nimproper] = $_[4*$i] + $nsiteoffset;
	$improperatom2[$nimproper] = $_[4*$i+1] + $nsiteoffset;
	$improperatom3[$nimproper] = $_[4*$i+2] + $nsiteoffset;
	$improperatom4[$nimproper] = $_[4*$i+3] + $nsiteoffset;
      }
    }
  }
} until(/!NDON/);

close(PSFIN);

$psffile = $appendpsffile;

} until($nfiles == 0);


############################  add extra waters  #########################
### Presuming that the last 3 atoms are a water molecule ????  AB #######

$basewatsite = $nsite-2;
$basewatsiteshift = 0;     
$nextrawats = $naddwat;

while( $nextrawats > 0 )
{
    ++$nsite;
    $basewatsiteoffset = $basewatsite + $basewatsiteshift;
    $atshift = $nsite - $basewatsiteoffset;
    $mol[$nsite]      = $mol[$basewatsiteoffset];
    $resnum[$nsite]   = $resnum[$basewatsiteoffset] + $naddwat - $nextrawats + 1;
    $resname[$nsite]  = $resname[$basewatsiteoffset];
    $atname[$nsite]   = $atname[$basewatsiteoffset];
    $attype[$nsite]   = $attype[$basewatsiteoffset]; # + $atshift;
    $element[$nsite]  = $element[$basewatsiteoffset];
#    $atid[$nsite]     = $atid[$basewatsiteoffset]; # + $atshift;
    $charge[$nsite]   = $charge[$basewatsiteoffset];
    $mass[$nsite]     = $mass[$basewatsiteoffset];

    ++$basewatsiteshift;
    if($basewatsiteshift >= 3)
    { 
	$basewatsiteshift = 0;  
	--$nextrawats;
    }
    if($verbose_output)
    { print "site $nsite resnum $resnum[$nsite] resname $resname[$nsite] atname $atname[$nsite]\n";}

}

$basewatbond = $nbond-2;
$basewatbondshift = 0;
$nextrawats = $naddwat;
while( $nextrawats > 0 )
{
    ++$nbond;
    $basewatbondoffset = $basewatbond + $basewatbondshift;
    $atshift = $nbond - $basewatbondoffset;
    $bondatom1[$nbond] = $bondatom1[$basewatbondoffset] + $atshift;
    $bondatom2[$nbond] = $bondatom2[$basewatbondoffset] + $atshift;

    ++$basewatbondshift;
    if($basewatbondshift >= 3)
    { 
	$basewatbondshift = 0;     
	--$nextrawats;
    }
}


$basewatang = $nangle;
$nextrawats = $naddwat;
while( $nextrawats > 0 )
{
    ++$nangle;
    $atshift = $nangle - $baswatang;
    $angleatom1[$nangle] = $angleatom1[$basewatang] + $atshift;
    $angleatom2[$nangle] = $angleatom2[$basewatang] + $atshift;
    $angleatom3[$nangle] = $angleatom3[$basewatang] + $atshift;
    --$nextrawats;
}


################################################################

$debug = 0;
if ($debug) {
for ($i =1; $i <= $nsite; ++$i) {
### print "$i $mol[$i] $resnum[$i] $resname[$i] $atname[$i] $atid[$i] $charge[$i] $mass[$i]\n";
  print "$i $mol[$i] $resnum[$i] $resname[$i] $atname[$i] $attype[$i] $charge[$i] $mass[$i]\n";}
 for ($i =1; $i <= $nbond; ++$i) {print "$i $bondatom1[$i] $bondatom2[$i]\n";}
  for ($i =1; $i <= $nangle; ++$i) {print "$i $angleatom1[$i] $angleatom2[$i] $angleatom3[$i]\n";}
  for ($i =1; $i <= $ntorsion; ++$i) {print "$i $torsionatom1[$i] $torsionatom2[$i] $torsionatom3[$i] $torsionatom4[$i]\n";}
  for ($i =1; $i <= $nimproper; ++$i) {print "$i $improperatom1[$i] $improperatom2[$i] $improperatom3[$i] $improperatom4[$i]\n";}
}

########################### read charmm parameter file ##########################

print ("\n READING CHARMM PARAMETER FILE .... \n \n");
open(PRMIN,"<$prmfile") || die "Cannot open parameter file $prmfile \n ";

#skip header
do {$_=<PRMIN>;} until(/BONDS/);

#read bond parameters
$nbondparm = 0;

do {
  $_ = <PRMIN>;
  chomp;
  if ($_ =~ /^!/ || $_ =~ /^\s+/ || $_ =~ /ANGLE/ || $_ eq "") {
  } else {
    ++$nbondparm;
    split;
    $bondparm1[$nbondparm] = $_[0];
    $bondparm2[$nbondparm] = $_[1];
    $bondparmk[$nbondparm] = $_[2];
    $bondparmr[$nbondparm] = $_[3];
  }
} until(/ANGLES/);

#read angle parameters
$nangleparm = 0;
$nureybparm = 0;

do {
  $_ = <PRMIN>;
  chomp;
  if ($_ =~ /^!/ || $_ =~ /^\s+/ || $_ =~ /DIHEDRAL/ || $_ eq "") {
  } else {
    ++$nangleparm;
    split;
    $angleparm1[$nangleparm]  = $_[0];
    $angleparm2[$nangleparm]  = $_[1];
    $angleparm3[$nangleparm]  = $_[2];
    $angleparmk[$nangleparm]  = $_[3];
    $angleparm0[$nangleparm]  = $_[4];
    if ($_[5] =~ /^\d/) {
      ++$nureybparm;
      $ureybparm1[$nureybparm] = $_[0];
      $ureybparm2[$nureybparm] = $_[1];
      $ureybparm3[$nureybparm] = $_[2];
      $ureybparmk[$nureybparm] = $_[5];
      $ureybparm0[$nureybparm] = $_[6];
    }
  }
} until(/DIHEDRALS/);

#read torsion parameters
$ntorsionparm = 0;

do {
  $_ = <PRMIN>;
  chomp;
  if ($_ =~ /^!/ || $_ =~ /^\s+/ || $_ =~ /IMPROPER/ || $_ eq "") {
  } else {
    ++$ntorsionparm;
    split;
    $torsionparm1[$ntorsionparm] = $_[0];
    $torsionparm2[$ntorsionparm] = $_[1];
    $torsionparm3[$ntorsionparm] = $_[2];
    $torsionparm4[$ntorsionparm] = $_[3];
    $torsionparmk[$ntorsionparm] = $_[4];
    $torsionparmn[$ntorsionparm] = $_[5];
    $torsionparmd[$ntorsionparm] = $_[6];
  }
} until(/IMPROPER/);

#read improper parameters
$nimproperparm = 0;
do {
  $_ = <PRMIN>;
  chomp;
  if ($_ =~ /^!/ || $_ =~ /^\s+/ || $_ =~ /NONBONDED/ || $_ eq "") {
  } else {
    ++$nimproperparm;
    split;
    $improperparm1[$nimproperparm] = $_[0];
    $improperparm2[$nimproperparm] = $_[1];
    $improperparm3[$nimproperparm] = $_[2];
    $improperparm4[$nimproperparm] = $_[3];
    $improperparmk[$nimproperparm] = $_[4];
    $improperparmd[$nimproperparm] = $_[6];
  }
} until(/NONBONDED/);

#skip nonbond header lines
for ($i = 1; $i <= 3; ++$i) {$_ = <PRMIN>;}

#read nonbond parameters

do {
  $_ = <PRMIN>;
  chomp;
  if ($_ =~ /^!/ || $_ =~ /^\s+/ || $_ =~ /HBOND/ || $_ eq "" || $_ =~ /^NBFIX/) {
  } else {
    split;
    $nbparmtype  = $_[0];
    $nbparme{$nbparmtype} = $_[2];
    $nbparmr{$nbparmtype} = $_[3];
    if ($_[4] =~ /^\d/) {
      $nb14parme{$nbparmtype} = $_[5];
      $nb14parmr{$nbparmtype} = $_[6];
    } else {
      $nb14parme{$nbparmtype} = $_[2];
      $nb14parmr{$nbparmtype} = $_[3];
    }
  }
##/** Substituting the water model??? AB. **/##
  if ($nbparmtype eq "OT") {
    $nbparme{$nbparmtype}   = -0.15207259450172;
    $nbparmr{$nbparmtype}   = 1.76824595565275;
    $nb14parme{$nbparmtype} = 0.0;
    $nb14parmr{$nbparmtype} = 0.0;
  }
  if ($nbparm{$nbparmtype} eq "HT") {
    $nbparme{$nbparmtype}   = 0.0;
    $nbparmr{$nbparmtype}   = 0.0;
    $nb14parme{$nbparmtype} = 0.0;
    $nb14parmr{$nbparmtype} = 0.0;
  }
} until(/HBOND/ || /^NBFIX/);

close(PRMIN);

#$debug = 1;
if ($debug) {
  for ($i = 1; $i <= $nbondparm; ++$i) {print "$i $bondparm1[$i] $bondparm2[$i] $bondparmk[$i] $bondparmr[$i] \n"}
  for ($i = 1; $i <= $nangleparm; ++$i) {print "$i $angleparm1[$i] $angleparm2[$i] $angleparm3[$i] $angleparmk $angleparm0[$i] \n"}
  for ($i = 1; $i <= $nureybparm; ++$i) {print "$i $ureybparm1[$i] $ureybparm2[$i] $ureybparm3[$i] $ureybparmk[$i] $ureybparm0[$i] \n"}
  for ($i = 1; $i <= $ntorsionparm; ++$i) {print "$i $torsionparm1[$i] $torsionparm2[$i] $torsionparm3[$i] $torsionparm4[$i] $torsionparmk[$i] $torsionparmn[$i] $torsionparmd[$i] \n"}
  for ($i = 1; $i <= $nimproperparm; ++$i) {print "$i $improperparm1[$i] $improperparm2[$i] $improperparm3[$i] $improperparm4[$i] $improperparmk[$i] $improperparmd[$i] \n"}
##  for ($i = 1; $i <= $nnbparm; ++$i) {print "$i $nbparmatom[$i] $nbparme[$i] $nbparmr[$i] $nb14parme[$i] $nb14parmr[$i] \n";}
  foreach $ikey (keys (%nbparme)) {print "$ikey $nbparme{$ikey} $nbparmr{$ikey} $nb14parme{$ikey} $nb14parmr{$ikey} \n";}
}

############################# read charmm topology file ###########################
########   Unnesessary: all masses are in the PSF file already    #################
########                 - AB.                                    #################
#print ("\n READING CHARMM TOPOLOGY FILE .... \n \n");
#open(TOPIN,"<$topfile") || die "Cannot open topology file $topfile \n ";
#
##read atom ids
#$ntopatom = 0;
#
#do {
# $_ = <TOPIN>;
# chomp;
#  if ($_ =~ /^MASS/) {
#    ++$ntopatom;
#    split;
#
#
#    $topatid[$ntopatom]             = $_[1];
#    $topatname[$topatid[$ntopatom]] = $_[2]; 
#    $topatmass[$topatid[$ntopatom]] = $_[3];
#    $topattype[$topatid[$ntopatom]] = $_[4];
#  }
#
#} until(/END/);
#
#close(TOPIN);
#
#$debug = 0;
#if ($debug) {
#  for ($i = 1; $i <= $ntopatom; ++$i) {print "$ntopatom $topatid[$i] $topatname[$topatid[$i]] #$topatmass[$topatid[$i]] $topattype[$topatid[$i]] \n";}
#}
#if ($debug_flow) 
#{    print "Now processing psf file\n";}
#

###################### Figuring things out ######################

# figure out all forcefield parameters

#number of water and water site

$nwatsite = 0;
$nwat = 0;
$foo = 0;

for ($i = 1; $i <= $nsite; ++$i) {
    $isWaterAtom[$i] = 0;
    if ($resname[$i] =~ /HOH/) {
	++$nwatsite;
	$isWaterAtom[$i] = 1;
	$watsiteatom[$nwatsite] = $i;
	if ($resnum[$i] != $foo) { 
	    ++$nwat;
	    $foo = $resnum[$i];
	}
	if ($atname[$i] =~ /OH2/){
	    $watatom1[$nwat] = $i;
	} elsif ($atname[$i] =~ /H1/){
	    $watatom2[$nwat] = $i;
	} elsif ($atname[$i] =~ /H2/){
	    $watatom3[$nwat] = $i;
	}
    }
}
if ($debug_flow){ print "Found $nwat waters\n";}

#for ($i =1; $i <= $nwat; ++$i) {print "$i   $watatom1[$i]    $watatom2[$i]   $watatom3[$i] \n";}
#for ($i =1; $i <= $nwatsite; ++$i) {print "$i   $watsiteatom[$i] \n";}

# non-zero parameter bond list

for ($j =1; $j <= $nbondparm; ++$j) {
    $bondparmtag = "$bondparm1[$j]_$bondparm2[$j]";
    $bondparmIndex{$bondparmtag} = $j;
    $bondparmtag = "$bondparm2[$j]_$bondparm1[$j]";
    $bondparmIndex{$bondparmtag} = $j;
}
if($debug_param)
{
   foreach $tmpind (keys %bondparmIndex) {
	print "bondparmIndex: $tmpind = $bondparmIndex{$tmpind}\n";
    }
}

$bond = 0;
for ($i = 1; $i <= $nbond; ++$i) {
#      if ($debug_flow){ print "\t\tworking on Bond $bond of $nbond bonds\n";}
     
###     $bondparmtag = "$topatname[$atid[$bondatom1[$i]]]_$topatname[$atid[$bondatom2[$i]]]";
     $bondparmtag = "$attype[$bondatom1[$i]]_$attype[$bondatom2[$i]]";
     $j = $bondparmIndex{$bondparmtag};

####  HOW CAN $j be 0 ????  AB.  ####
#### SAME LATER FOR THE ANGLES, DIHEDRALS, ETC ####
     if($j ne "" &&  $bondparmk[$j] != 0.0) {
	 ++$bond;
	 $bondk[$bond]   = $bondparmk[$j];
	 $bondr[$bond]   = $bondparmr[$j];
	 $bondat1[$bond] = $bondatom1[$i];
	 $bondat2[$bond] = $bondatom2[$i];
     } else {
       die "UNKNOWN BOND $i between atoms $bondatom1[$i] and $bondatom2[$i] : $bondparmtag\n";
     }

 }

if ($debug_flow){ print "\t\tFinished bonds. Now tagging water bonds\n";}
# tag water bonds

$nonwatbond = 0;
for($i = 1; $i <= $bond; $i++){
#    if ($debug_flow){ print "\tconsidering bond $i of $bond\n";}
    $foo =  $isWaterAtom[$bondat1[$i]] ;
    if ($foo) {
	$watbond[$i] = 1;
#	if ($debug_flow){ print "\t\t Tagging water bond $i (nonwatbonds $nonwatbond)\n";}
    } else {
	$watbond[$i] = 0;
	++$nonwatbond;
    }
}
$watbond = $bond - $nonwatbond;
if ($debug_flow) { print "Processed bondlist: nonwaterbonds: $nonwatbond   water bonds $watbond \n";}

#for($i=1; $i <= $bond; $i++){print "$i $bondat1[$i] $bondat2[$i] $bondk[$i] $watbond[$i] \n";}

# non zero parameter angle list
for ($j =1; $j <= $nangleparm; ++$j) {
    $angleparmtag = "$angleparm1[$j]_$angleparm2[$j]_$angleparm3[$j]";
    $angleparmIndex{$angleparmtag} = $j;
    $angleparmtag = "$angleparm3[$j]_$angleparm2[$j]_$angleparm1[$j]";
    $angleparmIndex{$angleparmtag} = $j;
}
if($debug_param)
{
    foreach $tmpind (keys %angleparmIndex) {
	print "angleparmIndex: $tmpind = $angleparmIndex{$tmpind}\n";
    }
}
$angle = 0;
for ($i = 1; $i <= $nangle; ++$i) {
#    $angleparmtag = "$topatname[$atid[$angleatom1[$i]]]_$topatname[$atid[$angleatom2[$i]]]_$topatname[$atid[$angleatom3[$i]]]";
    $angleparmtag = "$attype[$angleatom1[$i]]_$attype[$angleatom2[$i]]_$attype[$angleatom3[$i]]";
    $j = $angleparmIndex{$angleparmtag};
    if ($j ne "" && $angleparmk[$j] != 0.0) {
	++$angle;
	$anglek[$angle]   = $angleparmk[$j];
	$angle0[$angle]   = $angleparm0[$j];
	$angleat1[$angle] = $angleatom1[$i];
	$angleat2[$angle] = $angleatom2[$i];
	$angleat3[$angle] = $angleatom3[$i];
    } else {
       die "UNKNOWN ANGLE $i between atoms $angleatom1[$i] $angleatom2[$i] $angleatom3[$i] : $angleparmtag\n";
     }

}

$nonwatangle = 0;
for($i = 1; $i <= $angle; $i++){
    $foo =  $isWaterAtom[$angleat1[$i]] ;
    if ($foo) {
	$watangle[$i]   = 1;
    } else {
	$watangle[$i] = 0;
	++$nonwatangle;
    }
}
if ($debug_flow){ print "Processed anglelist\n";}

#for($i=1; $i <= $angle; $i++){print "$i $angleat1[$i] $angleat2[$i] $angleat3[$i] $anglek[$i] $watangle[$i]\n";}

# urey bradley 

$ureyb = 0;
for ($j =1; $j <= $nureybparm; ++$j) {
    $ureybparmtag = "$ureybparm1[$j]_$ureybparm2[$j]_$ureybparm3[$j]";
    $ureybparmIndex{$ureybparmtag} = $j;
    $ureybparmtag = "$ureybparm3[$j]_$ureybparm2[$j]_$ureybparm1[$j]";
    $ureybparmIndex{$ureybparmtag} = $j;
}
if($debug_param)
{
    foreach $tmpind (keys %ureybparmIndex) {
	print "ureybparmIndex: $tmpind = $ureybparmIndex{$tmpind}\n";
    }
}

for ($i = 1; $i <= $nangle; ++$i) {
#    $ureybparmtag = "$topatname[$atid[$angleatom1[$i]]]_$topatname[$atid[$angleatom2[$i]]]_$topatname[$atid[$angleatom3[$i]]]";
    $ureybparmtag = "$attype[$angleatom1[$i]]_$attype[$angleatom2[$i]]_$attype[$angleatom3[$i]]";
    $j = $ureybparmIndex{$ureybparmtag};
    if ($j ne "" ) {
	++$ureyb;
	$ureybk[$ureyb]   = $ureybparmk[$j];
	$ureyb0[$ureyb]   = $ureybparm0[$j];
	$ureybat1[$ureyb] = $angleatom1[$i];
	$ureybat2[$ureyb] = $angleatom2[$i];
	$ureybat3[$ureyb] = $angleatom3[$i];
    }
    else
    {
#	print "Falied to find UB $ureybparmtag\n";
    }
} 
if ($debug_flow){ print "Processed UB list\n";}

#for ($i = 1; $i <= $ureyb; ++$i) {print "$i $ureybat1[$i] $ureybat2[$i] $ureybat3[$i] $ureybk[$i] $ureyb0[$i]\n";}

# torsion

torsionlbl: for ($i = 1; $i <= $ntorsion; ++$i) {
  $score = 0;
  for ($j =1; $j <= $ntorsionparm; ++$j) {
    $check = 0;
    if ($attype[$torsionatom1[$i]] eq $torsionparm1[$j] && 
	$attype[$torsionatom2[$i]] eq $torsionparm2[$j] && 
	$attype[$torsionatom3[$i]] eq $torsionparm3[$j] &&
	$attype[$torsionatom4[$i]] eq $torsionparm4[$j]) {
      $check = 1;
      $score = 1;
    } elsif ($attype[$torsionatom1[$i]] eq $torsionparm4[$j] &&
	     $attype[$torsionatom2[$i]] eq $torsionparm3[$j] && 
	     $attype[$torsionatom3[$i]] eq $torsionparm2[$j] &&
	     $attype[$torsionatom4[$i]] eq $torsionparm1[$j]) {
      $check = 1;
      $score = 1;
    } elsif ($score != 1 && 
	     ($torsionparm1[$j] eq "X" && 
	     $torsionparm2[$j] eq $attype[$torsionatom2[$i]] && 
	     $torsionparm3[$j] eq $attype[$torsionatom3[$i]] &&
	     $torsionparm4[$j] eq "X")) {
      $check = 1;
    } elsif ($score != 1 &&
	     ($torsionparm1[$j] eq "X" && 
	     $torsionparm2[$j] eq $attype[$torsionatom3[$i]] && 
	     $torsionparm3[$j] eq $attype[$torsionatom2[$i]] &&
	     $torsionparm4[$j] eq "X")) {
      $check = 1;
    }
    if ($check == 1 && $torsionparmk[$j] != 0.0) {
      ++$torsion;
      $torsionk[$torsion]    = $torsionparmk[$j];
      $torsionn[$torsion]    = $torsionparmn[$j];
      $torsiond[$torsion]    = $torsionparmd[$j];
      $torsionat1[$torsion]  = $torsionatom1[$i];
      $torsionat2[$torsion]  = $torsionatom2[$i];
      $torsionat3[$torsion]  = $torsionatom3[$i];
      $torsionat4[$torsion]  = $torsionatom4[$i];
      $check = 0;
    }
  }
}

if ($debug_flow){ print "Processed torsion list\n";}
#for ($i = 1; $i <= $torsion; ++$i) {print "$i $torsionat1[$i] $torsionat2[$i] $torsionat3[$i] $torsionat4[$i] $atname[$torsionat1[$i]] $atname[$torsionat2[$i]] $atname[$torsionat3[$i]] $atname[$torsionat4[$i]] $torsionk[$i] $torsionn[$i] $torsiond[$i] \n";}

# improper torsion

$improper = 0;

improperlbl: for ($i = 1; $i <= $nimproper; ++$i) {
  for ($j =1; $j <= $nimproperparm; ++$j) {
    if (($attype[$improperatom1[$i]] eq $improperparm1[$j] && 
	 $attype[$improperatom2[$i]] eq $improperparm2[$j] && 
	 $attype[$improperatom3[$i]] eq $improperparm3[$j] &&
	 $attype[$improperatom4[$i]] eq $improperparm4[$j]) ||
	($attype[$improperatom1[$i]] eq $improperparm4[$j] && 
	 $attype[$improperatom2[$i]] eq $improperparm3[$j] && 
	 $attype[$improperatom3[$i]] eq $improperparm2[$j] &&
	 $attype[$improperatom4[$i]] eq $improperparm1[$j])) {
      ++$improper;
      $improperk[$i]   = $improperparmk[$j];
      $improperd[$i]   = $improperparmd[$j];
      $improperat1[$i] = $improperatom1[$i];
      $improperat2[$i] = $improperatom2[$i];
      $improperat3[$i] = $improperatom3[$i];
      $improperat4[$i] = $improperatom4[$i];
      next improperlbl;
    } elsif (($improperparm1[$j] eq $attype[$improperatom1[$i]] && 
	      $improperparm2[$j] eq "X" && 
	      $improperparm3[$j] eq "X" &&
	      $improperparm4[$j] eq $attype[$improperatom4[$i]]) ||
	     ($improperparm1[$j] eq $attype[$improperatom4[$i]] && 
	      $improperparm2[$j] eq "X" && 
	      $improperparm3[$j] eq "X" &&
	      $improperparm4[$j] eq $attype[$improperatom1[$i]])) {
      ++$improper;
      $improperk[$i]   = $improperparmk[$j];
      $improperd[$i]   = $improperparmd[$j];
      $improperat1[$i] = $improperatom1[$i];
      $improperat2[$i] = $improperatom2[$i];
      $improperat3[$i] = $improperatom3[$i];
      $improperat4[$i] = $improperatom4[$i];
      next improperlbl;
    }
  }
}

if ($debug_flow){ print "Processed improper torsion list\n";}

#for ($i = 1; $i <= $improper; ++$i) {print "$i $improperat1[$i] $improperat2[$i] $improperat3[$i] $improperat4[$i] $atname[$improperat1[$i]] $atname[$improperat2[$i]] $atname[$improperat3[$i]] $atname[$improperat4[$i]] $improperk[$i] $impropern[$i] $improperd[$i] \n";}


###
# get lj and lj14 parameters for site list
###

# for ($j =1; $j <= $ntopatom; ++$j) {
#    $topatidIndex{$topatid[$j]} = $j;
# }

#if($debug_param)
#{
#    foreach $tmpind (keys %topatidIndex) {
#	print "topatidIndex: $tmpind = $topatidIndex{$tmpind}\n";
#    }
#
# for ($j =1; $j <= $nnbparm; ++$j) {
#    $nbparamIndex{$nbparmatom[$j]} = $j;
# }
#    foreach $tmpind (keys %nbparamIndex) {
#	print "nbparmIndex: $tmpind = $nbparmIndex{$tmpind}\n";
#    }
#    foreach $tmpind (keys %topatname) {
#	print "nbparmIndex: $tmpind = $nbparmIndex{$tmpind}\n";
#    }
#}

for($i = 1; $i <= $nsite; $i++){

# UNNESESSARY.  AB.
#    $ii = $topatidIndex{$atid[$i]};
#    if($ii == 0){
#	print " site $i \natid $atid[$i] \ntopatidindex $ii \ntopatid $topatid[$ii]\n";
#	print "dieing at atid[$i] = $atid[$i]\n"; 
#	die;}

#    $element[$i] = $topattype[$topatid[$ii]];
#    $element[$i] = $attype[$i];
#    
#    $iii = $nbparmIndex{$topatname[$topatid[$ii]]};
#    if($iii == 0){
#	print " site $i \nelement $element[$i] \natid $atid[$i] \ntopatidindex $ii \ntopatid $topatid[$ii]\n";
#	print "dieing at topatname = $topatname[$topatid[$ii]];\n"; 
#	die;}

    if ($resname[$i] eq "HOH" && !$watparam) {
	$charge[$i] = "NULL";
	$lje[$i]   = "NULL";
	$ljr[$i]   = "NULL";
	$lj14e[$i] = "NULL";
	$lj14r[$i] = "NULL";
    } elsif ($resname[$i] eq "HOH" && $watparam) {
	# store the negative epsilon to reconcile with the rest of the world
	$lje[$i]   = - $nbparme{$attype[$i]};
	$ljr[$i]   = $nbparmr{$attype[$i]};
	$lj14e[$i] = "0.0";
	$lj14r[$i] = "0.0";
    } else {
	# store the negative epsilon to reconcile with the rest of the world
	$lje[$i]   = - $nbparme{$attype[$i]};
	$ljr[$i]   = $nbparmr{$attype[$i]};
	$lj14e[$i] = - $nb14parme{$attype[$i]};
	$lj14r[$i] = $nb14parmr{$attype[$i]};
    }
}


if ($debug_flow){ print "Processed LJ and LJ14 list\n";}
#for($i = 1; $i <= $nsite; $i++){print "$i $lje[$i] $ljr[$i] $lj14e[$i] $lj14r[$i] $atname[$i] $element[$i]\n";}

# figure out sites relationship in the data model

#renaming atom tag

for($i = 1; $i <= $nsite; ++$i) {
  $foo = convertatomtag($atname[$i],$resname[$i],$i);
  $atname[$i] = $foo;

}

#for ($i = 1; $i <= $nsite; ++$i) {print "$i $atname[$i] \n";}

#renaming cap atom tag

for($i = 1; $i <= $nsite; ++$i) {
  $moleculetype[$nmoleculename] = checkmoleculetype($resname[$i],$atname[$i]);
  convertcapatomtag($atname[$i],$resname[$i],$resnum[$i],$cap);
  if($resname[$i] =~ /CT/) {$cap = "1";}
}
if ($debug_flow){ print "Standardized atom tags \n";}
#for ($i = 1; $i <= $nsite; ++$i) {print "$i $resname[$i] \n";}

# molecule name

@junk = split(/_/,$system);
$pref = $junk[0];

for($i = 1; $i <= $nsite; ++$i) {
  if ($mol[$i] =~ /MAIN/ || $mol[$i] =~ /PRO/) {
    $mol[$i] = "$pref";
  }
  if ($mol[$i] =~ /ION/ ) {
    $mol[$i] = "$resname[$i]";
  }
  if ($mol[$i] =~ /WAT/ ) {
    $mol[$i] = "HOH";
  }
#  print " $i $mol[$i]" ;
}

$nmoleculename = 0;

for($i = 1; $i <= $nsite; ++$i) {
  $check = 0;
  for ($j = 1; $j <= $nmoleculename; ++$j) {
    if ($moleculename[$j] eq $mol[$i]) {
      $check = 1;
    }
  }
  if (!$check) {
    ++$nmoleculename;
    $moleculename[$nmoleculename] = $mol[$i];
    $moleculetype[$nmoleculename] = checkmoleculetype($resname[$i],$atname[$i]);
  }
}
if ($debug_flow){ print "Standardized molecule names\n";}

#for($i = 1; $i <= $nmoleculename; ++$i) {print "$i $moleculename[$i] $moleculetype[$i] \n";}

# number of non-water molecule based on bond list
for ($i = 1; $i <= $nsite; ++$i) {
  $natch[$i]=0;
}
$maxsite = 0;
for ($i = 1; $i <= $bond; ++$i) {
    $a1 = $bondat1[$i];
    $a2 = $bondat2[$i];
    $natch[$a1]++;
    $natch[$a2]++;
    $adj[$a1][$natch[$a1]]=$a2;
    $adj[$a2][$natch[$a2]]=$a1;
    if ($maxsite <= $a1) {
      $maxsite = $a1;
    }
    if ($maxsite <= $a2) {
      $maxsite = $a2;
    }
}

if ($debug_flow){ print "Cooked Adj matrix\n";}
$molecount = 0;
$nion = 0;

for ($i = 1; $i <= $nsite; ++$i) {
  $val[$i] = 0;
}

for ($i = 1; $i <= $nsite; ++$i) 
{
    if (!$val[$i]) 
    {
	++$molecount;
#	if ($debug_flow){ print "found edge of $molecount molecules\n";}    
	@molesite = "";
	@stack = "";
	visit($i);
	@molesite = sort by_number @molesite;
    }
    $mi[$i] = $molecount;

  if ($natch[$i] == 0) {
      ++$nion;
      $ion[$nion] = $i;
      print "found ion $i $mol[$i]\n";
  }
}

if ($debug_flow){ print "Adding waters as molinstances\n";}
# get molecule instance for the rest of water and ion sites


if ($debug_flow){ print "Assigning molnames to sites\n";}
for ($i = 1; $i <= $nsite; ++$i) {
  for ($ii = 1; $ii <= $nmoleculename; ++$ii) {
    if ($moleculename[$ii] eq $mol[$i]) {
      $moleculenameid[$i] = $ii;
    }
  }
}
if ($debug_flow){ print "Finished total of $molecount molecules including $nwat waters and $nion ions\n";}

$lastmolinst = 0;

for ($i = 1; $i <= $nsite; ++$i) {
    if( $lastmolinst ne $mi[$i] ) {
#	print " molinsttab[$i] = $mi[$i] \n";
	$lastmolinst = $mi[$i];
	$molInst2Name{$lastmolinst} = $mol[$i];
    }
}

for ($i = 1; $i <= $nmoleculename; ++$i) {
    $molName2Index{$moleculename[$i]} = $i;
#    print " molName2Index:    $i  $moleculename[$i] $molName2Index{$moleculename[$i]} \n";
}



#foreach $molInst (keys %molInst2Name) {
#    print "molInst2Name: $molInst $molInst2Name{$molInst}\n";
#}


# number chemical component and monomer instances

$ncc = 0;
$foo = 0;
$foo2 = "";
$nmoni = 0;

for($i = 1; $i <= $nsite; ++$i) {
#  print "$ncc $i $resname[$i] $resnum[$i]\n";
    if ($foo != $resnum[$i] || $foo2 ne $resname[$i]) {
	++$ncc;
	$foo = $resnum[$i];
	$foo2= $resname[$i];
	if (checkpolymer($moleculetype[$moleculenameid[$i]])) {
	    ++$nmoni;
	    $moni[$nmoni] = $ncc;
	    $monichain[$nmoni] = $mi[$i];
	}
    }
    $cc[$ncc] = $resname[$i];
    $cci[$i] = $ncc;
}    

if ($debug_flow){ print "Processed $ncc chemical component instances\n";}
#for ($i = 1; $i <= $ncc; ++$i) {print "$i $cc[$i] \n";}

# number of chain

$nchain = 0;
$foo = 0;
$foo2 = "A";

for($i = 1; $i <= $nsite; ++$i) {
  if(checkpolymer($moleculetype[$moleculenameid[$i]]) && $foo != $mi[$i]) {
    ++$nchain;
    $chain[$nchain] = "$foo2";
    ++$foo2;
    $foo = $mi[$i];
  }
}
if ($debug_flow){ print "Processed $nchain chains\n";}
#for ($i = 1; $i <= $nchain; ++$i) {print "$i $chain[$i] \n";}

# monomer instances

if ($debug_flow){ print "Processed $nmoni monomer instances\n";}
$debug = 0;
if ($debug) {
  for ($i = 1; $i <= $nmoni; ++$i) {print "$i $moni[$i] $monichain[$i] \n";}
  for ($i = 1; $i <= $molecount; ++$i) {print "$i \n";}
  for ($i = 1; $i <= $nmoleculename; ++$i) {print "$i $moleculename[$i] $moleculetype[$i]\n";}
  for ($i = 1; $i <= $nsite; ++$i) {print "$i $mi[$i] $cci[$i] $moleculenameid[$i] $moleculename[$moleculenameid[$i]] $moleculetype[$moleculenameid[$i]]\n";}
}

################### Write out xml file ###################

$xmlfile = "$system.charmm.xml";

print ("WRITING XML FILE CALLED $xmlfile .... \n");

open(XMLOUT,">$xmlfile") || die "Cannot open psf file $xmlfile \n ";

# dtd header info

print XMLOUT "<?xml version='1.0' encoding='US-ASCII' standalone='no' ?>\n";
print XMLOUT "<!DOCTYPE ffparams [\n";
print XMLOUT "<!ELEMENT ffparams (header_parameter_list, (data_list)+,\n";
print XMLOUT "(udf_invocation_table)?, (site_tuple_list | param_tuple_list)*,\n";
print XMLOUT "(pair_list)*, (group_list)*)>\n\n";

print XMLOUT "<!-- The header parameter list contains per system information.  Each -->\n";
print XMLOUT "<!-- parameter has associated name, type, and symbolic_constant -->\n";
print XMLOUT "<!-- attributes.  If symbolic_constant=\"true\" then it is intended that -->\n";
print XMLOUT "<!-- the parameter so labeled is to be treated as a symbolic constant -->\n";
print XMLOUT "<!-- for use by downstream applications-->\n";
print XMLOUT "<!ELEMENT header_parameter_list (header_parameter)+>\n";
print XMLOUT "<!ELEMENT header_parameter (#PCDATA)>\n";
print XMLOUT "<!ATTLIST header_parameter param_id ID #REQUIRED>\n";
print XMLOUT "<!ATTLIST header_parameter type (char | double | int) \"char\">\n";
print XMLOUT "<!ATTLIST header_parameter symbolic_constant (true | false) \"false\">\n\n";

print XMLOUT "<!-- The data_list contains a list of type_ids that are used -->\n";
print XMLOUT "<!-- for a variety of purposes, including molecule types and molecule -->\n";
print XMLOUT "<!-- instances as well as LJ types.   These types may have -->\n";
print XMLOUT "<!-- parameter values associated with them which are defined in the -->\n";
print XMLOUT "<!-- type_parameter_list. Information about what type_id is -->\n";
print XMLOUT "<!-- bound to a particular site_id must be conveyed via a -->\n";
print XMLOUT "<!-- site_parameter_ref. It is required that type_ids consist of the -->\n";
print XMLOUT "<!-- type_class string with an underscore and number appended -->\n";
print XMLOUT "<!ELEMENT data_list (data_title, data_parameter_list, (data_record)*)>\n";
print XMLOUT "<!ATTLIST data_list data_class_prepend ID #REQUIRED>\n";
print XMLOUT "<!ELEMENT data_title (#PCDATA)>\n";
print XMLOUT "<!ELEMENT data_parameter_list (data_parameter_spec)*>\n";
print XMLOUT "<!ATTLIST data_parameter_list param_prepend ID #REQUIRED>\n";
print XMLOUT "<!ELEMENT data_parameter_spec (#PCDATA)>\n";
print XMLOUT "<!ATTLIST data_parameter_spec param_id ID #REQUIRED>\n";
print XMLOUT "<!ATTLIST data_parameter_spec type (char | double | int | idref) \"char\">\n";
print XMLOUT "<!ATTLIST data_parameter_spec ref_prepend IDREF #IMPLIED>\n";
print XMLOUT "<!ELEMENT data_record ((data_parameter | data_parameter_ref))*>\n";
print XMLOUT "<!ATTLIST data_record data_id ID #REQUIRED>\n";
print XMLOUT "<!ELEMENT data_parameter (#PCDATA)>\n";
print XMLOUT "<!ATTLIST data_parameter param_ref IDREF #REQUIRED>\n";
print XMLOUT "<!ELEMENT data_parameter_ref EMPTY>\n";
print XMLOUT "<!ATTLIST data_parameter_ref param_ref IDREF #REQUIRED>\n";
print XMLOUT "<!ATTLIST data_parameter_ref ref_ptr IDREF #REQUIRED>\n";

print XMLOUT "<!-- The invocation table contains a list of all the user defined -->\n";
print XMLOUT "<!-- functions required for this system and pointers to tuples of -->\n";
print XMLOUT "<!-- sites and parameters required for each UDF   -->\n";
print XMLOUT "<!ELEMENT udf_invocation_table (udf_invocation_rec)*>\n";
print XMLOUT "<!ELEMENT udf_invocation_rec EMPTY>\n";
print XMLOUT "<!ATTLIST udf_invocation_rec udf_id ID #REQUIRED>\n";
print XMLOUT "<!ATTLIST udf_invocation_rec site_tuple_ref IDREF #REQUIRED>\n";
print XMLOUT "<!ATTLIST udf_invocation_rec param_tuple_ref IDREF #IMPLIED>\n\n";

print XMLOUT "<!-- The site tuple list contains a collection of site_id tuples that are -->\n";
print XMLOUT "<!-- used by one or more udfs.  The order of the tuples in this list -->\n";
print XMLOUT "<!-- must correspond to the order of the tuples in the corresponding -->\n";
print XMLOUT "<!-- param_tuple_list -->\n";
print XMLOUT "<!ELEMENT site_tuple_list (site_tuple)+>\n";
print XMLOUT "<!ATTLIST site_tuple_list id ID #REQUIRED>\n";
print XMLOUT "<!ELEMENT site_tuple (site)+>\n";
print XMLOUT "<!ELEMENT site EMPTY>\n";
print XMLOUT "<!ATTLIST site site_ref IDREF #REQUIRED>\n";
print XMLOUT "<!ATTLIST site site_ordinal CDATA #REQUIRED>\n\n";

print XMLOUT "<!-- The param tuple list contains a collection of parameter tuples -->\n";
print XMLOUT "<!-- that are used by one or more udfs.  The order of the tuples in -->\n";
print XMLOUT "<!-- this list must correspond to the order of the tuples in the -->\n";
print XMLOUT "<!-- corresponding site_tuple_list.  This list also has the usual -->\n";
print XMLOUT "<!-- parameter specification block at the beginning.  The element -->\n";
print XMLOUT "<!-- value of the param_spec element is the name of the parameter -->\n";
print XMLOUT "<!ELEMENT param_tuple_list (param_spec_list, (param_tuple)+)>\n";
print XMLOUT "<!ATTLIST param_tuple_list id ID #REQUIRED>\n";
print XMLOUT "<!ELEMENT param_spec_list (param_spec)+>\n";
print XMLOUT "<!ATTLIST param_spec_list param_prepend NMTOKEN #REQUIRED>\n";
print XMLOUT "<!ELEMENT param_spec (#PCDATA)>\n";
print XMLOUT "<!ATTLIST param_spec param_id ID #REQUIRED>\n";
print XMLOUT "<!ATTLIST param_spec type (double | int) \"double\">\n";
print XMLOUT "<!ELEMENT param_tuple (param)+>\n";
print XMLOUT "<!ELEMENT param (#PCDATA)>\n";
print XMLOUT "<!ATTLIST param param_ref IDREF #REQUIRED>\n\n";

print XMLOUT "<!-- The pair list contains a list of pair interaction parameters for -->\n";
print XMLOUT "<!-- each pair of the associated types-->\n";
print XMLOUT "<!ELEMENT pair_list (pair_parameter_list, (pair_record)+)>\n";
print XMLOUT "<!ATTLIST pair_list type_class_prepend IDREF #REQUIRED>\n";
print XMLOUT "<!ELEMENT pair_parameter_list (pair_parameter_spec)+>\n";
print XMLOUT "<!ATTLIST pair_parameter_list param_prepend ID #REQUIRED>\n";
print XMLOUT "<!ELEMENT pair_parameter_spec (#PCDATA)>\n";
print XMLOUT "<!ATTLIST pair_parameter_spec param_id ID #REQUIRED>\n";
print XMLOUT "<!ATTLIST pair_parameter_spec type (char | double | int) \"char\">\n";
print XMLOUT "<!ELEMENT pair_record (pair_parameter)+>\n";
print XMLOUT "<!ATTLIST pair_record type_ref_1 IDREF #REQUIRED>\n";
print XMLOUT "<!ATTLIST pair_record type_ref_2 IDREF #REQUIRED>\n";
print XMLOUT "<!ELEMENT pair_parameter (#PCDATA)>\n";
print XMLOUT "<!ATTLIST pair_parameter param_ref IDREF #REQUIRED>\n\n";

print XMLOUT "<!-- User defined groups contain group information and a list of group -->\n";
print XMLOUT "<!-- members specified by site_id-->\n";
print XMLOUT "<!ELEMENT group_list (group_info, (group_member)+)>\n";
print XMLOUT "<!ATTLIST group_list group_id ID #REQUIRED>\n";
print XMLOUT "<!ATTLIST group_list group_class CDATA #REQUIRED>\n";
print XMLOUT "<!ELEMENT group_info (#PCDATA)>\n";
print XMLOUT "<!ELEMENT group_member EMPTY>\n";
print XMLOUT "<!ATTLIST group_member site_ref IDREF #REQUIRED>\n";
print XMLOUT "]>\n";

# header info

print XMLOUT "<ffparams>\n";
print XMLOUT  "<header_parameter_list>\n";
print XMLOUT  "<header_parameter param_id=\"ff_family\" type=\"char\" symbolic_constant=\"false\">CHARMM</header_parameter>\n";
print XMLOUT  "<header_parameter param_id=\"ff_spec\" type=\"char\" symbolic_constant=\"false\">CHARMM22</header_parameter>\n";
print XMLOUT  "<header_parameter param_id=\"charge14\" type=\"int\" symbolic_constant=\"true\">1</header_parameter>\n";
print XMLOUT  "<header_parameter param_id=\"charge14scale\" type=\"double\" symbolic_constant=\"true\">1.0</header_parameter>\n";
print XMLOUT  "<header_parameter param_id=\"lj14\" type=\"int\" symbolic_constant=\"true\">1</header_parameter>\n";
print XMLOUT  "<header_parameter param_id=\"lj14scale\" type=\"double\" symbolic_constant=\"true\">1.0</header_parameter>\n";
print XMLOUT  "<header_parameter param_id=\"torsioninput\" type=\"int\" symbolic_constant=\"false\">1</header_parameter>\n";
print XMLOUT  "<header_parameter param_id=\"improper\" type=\"int\" symbolic_constant=\"false\">1</header_parameter>\n";
print XMLOUT  "<header_parameter param_id=\"ureybradley\" type=\"int\" symbolic_constant=\"false\">1</header_parameter>\n";
print XMLOUT  "<header_parameter param_id=\"grouping\" type=\"int\" symbolic_constant=\"false\">1</header_parameter>\n";
print XMLOUT  "<header_parameter param_id=\"water\" type=\"char\" symbolic_constant=\"false\">WATP</header_parameter>\n";
print XMLOUT  "<header_parameter param_id=\"units\" type=\"char\" symbolic_constant=\"false\">CHEM</header_parameter>\n";
print XMLOUT  "<header_parameter param_id=\"ChargeConversion\" type=\"double\" symbolic_constant=\"true\">SciConst::StdChargeConversion</header_parameter>\n";
print XMLOUT  "<header_parameter param_id=\"TimeConversionFactor\" type=\"double\" symbolic_constant=\"true\">SciConst::StdTimeConversion</header_parameter>\n";
print XMLOUT  "</header_parameter_list>\n";

# molecule name info

print XMLOUT "\n";
print XMLOUT "<data_list data_class_prepend=\"mn_\">\n";
print XMLOUT "<data_title>molecule_name</data_title>\n";
print XMLOUT " <data_parameter_list param_prepend=\"mnp_\">\n";
print XMLOUT "   <data_parameter_spec param_id=\"mnp_1\" type=\"char\">name</data_parameter_spec>\n";
print XMLOUT "   <data_parameter_spec param_id=\"mnp_2\" type=\"char\">type</data_parameter_spec>\n";
print XMLOUT " </data_parameter_list>\n";

for($i = 1; $i <= $nmoleculename; $i++){
  print XMLOUT " <data_record data_id=\"mn_$i\">\n";
  print XMLOUT "    <data_parameter param_ref=\"mnp_1\">$moleculename[$i]</data_parameter>\n";
  print XMLOUT "    <data_parameter param_ref=\"mnp_2\">$moleculetype[$i]</data_parameter>\n";
  print XMLOUT " </data_record>\n";
}
print XMLOUT " </data_list>\n";

# molecule chain info

if ($nchain) {
  print XMLOUT "\n";
  print XMLOUT "<data_list data_class_prepend=\"ch_\">\n";
  print XMLOUT "<data_title>chain</data_title>\n";
  print XMLOUT "<data_parameter_list param_prepend=\"chp_\">\n";
  print XMLOUT "<data_parameter_spec param_id=\"chp_1\" type=\"char\">name</data_parameter_spec>\n";
  print XMLOUT "</data_parameter_list>\n";
  
  for($ii = 0 ; $ii < $NReplicas ; $ii++)
    {
      $offset = $ii * $nchain;
      for($i=1; $i <= $nchain; $i++){
	$j = $i + $offset;
	print XMLOUT "    <data_record data_id=\"ch_$j\">\n";
	print XMLOUT "      <data_parameter param_ref=\"chp_1\">$chain[$i]</data_parameter>\n";
	print XMLOUT "    </data_record>\n";
	++$molechain;
      }
    }
  print XMLOUT "</data_list>\n";
}

# chemical component info

print XMLOUT "\n";
print XMLOUT "<data_list data_class_prepend=\"cci_\">\n";
print XMLOUT "<data_title>chemical_component_instance</data_title>\n";
print XMLOUT "<data_parameter_list param_prepend=\"ccip_\">\n";
print XMLOUT "<data_parameter_spec param_id=\"ccip_1\" type=\"char\">cc_id</data_parameter_spec>\n";
print XMLOUT "</data_parameter_list>\n";

  for($ii = 0 ; $ii < $NReplicas ; $ii++)
  {
    $offset = $ii * $ncc;
    for($i=1; $i <= $ncc; $i++){
      $j = $i + $offset;
      print XMLOUT "    <data_record data_id=\"cci_$j\">\n";
      print XMLOUT "      <data_parameter param_ref=\"ccip_1\">$cc[$i]</data_parameter>\n";
      print XMLOUT "    </data_record>\n";
    }
  }
print XMLOUT "</data_list>\n";

# monomer info

if ($nmoni) {
  print XMLOUT "\n";
  print XMLOUT "<data_list data_class_prepend=\"moni_\">\n";
  print XMLOUT "<data_title>monomer_instance</data_title>\n";
  print XMLOUT "<data_parameter_list param_prepend=\"monip_\">\n";
  print XMLOUT "<data_parameter_spec param_id=\"monip_1\" type=\"idref\" ref_prepend=\"ch_\">chain_id</data_parameter_spec>\n";
  print XMLOUT "<data_parameter_spec param_id=\"monip_2\" type=\"idref\" ref_prepend=\"cci_\">cc_instance_id</data_parameter_spec>\n";
  print XMLOUT "</data_parameter_list>\n";
  
  for($ii = 0 ; $ii < $NReplicas ; $ii++)
    {
      $offset = $ii * $nmoni;
      for($i=1; $i <= $nmoni; $i++){
	$j = $i + $offset;
	print XMLOUT "     <data_record data_id=\"moni_$j\">\n";
	print XMLOUT "       <data_parameter_ref param_ref=\"monip_1\" ref_ptr=\"ch_$monichain[$i]\"/>\n";
	print XMLOUT "       <data_parameter_ref param_ref=\"monip_2\" ref_ptr=\"cci_$moni[$i]\"/>\n";
	print XMLOUT "     </data_record>\n";
      }
    }
  print XMLOUT "</data_list>\n";
}

# molecule instance info

print XMLOUT "\n";
print XMLOUT "<data_list data_class_prepend=\"mi_\">\n";
print XMLOUT "<data_title>molecule_instance</data_title>\n";
print XMLOUT " <data_parameter_list param_prepend=\"mip_\">\n";
print XMLOUT "    <data_parameter_spec param_id=\"mip_1\" type=\"idref\" ref_prepend=\"mn_\">molecule_name</data_parameter_spec>\n";
print XMLOUT " </data_parameter_list>\n";

  for($ii = 0 ; $ii < $NReplicas ; $ii++)
    {
      $offset = $ii * $molecount;
      for($i=1; $i <= $molecount; $i++){
	$j = $i + $offset;
	print XMLOUT " <data_record data_id=\"mi_$j\">\n";
	print XMLOUT " <data_parameter_ref param_ref=\"mip_1\" ref_ptr=\"mn_$molName2Index{$molInst2Name{$i}}\"/>\n"; 
	print XMLOUT " </data_record>\n";
      }
    }
print XMLOUT " </data_list>\n";

# site instance info

print XMLOUT "\n";
print XMLOUT "<data_list data_class_prepend=\"site_\">\n";
print XMLOUT "<data_title>site_list</data_title>\n";
print XMLOUT "  <data_parameter_list param_prepend=\"sp_\">\n";
print XMLOUT "     <data_parameter_spec param_id=\"sp_1\" type=\"double\">mass</data_parameter_spec>\n";
print XMLOUT "     <data_parameter_spec param_id=\"sp_2\" type=\"double\">charge</data_parameter_spec>\n";
print XMLOUT "     <data_parameter_spec param_id=\"sp_3\" type=\"int\">mol_instance_id</data_parameter_spec>\n";
print XMLOUT "     <data_parameter_spec param_id=\"sp_4\" type=\"int\">cc_instance_id</data_parameter_spec>\n";
print XMLOUT "     <data_parameter_spec param_id=\"sp_5\" type=\"char\">atom_tag</data_parameter_spec>\n";
print XMLOUT "     <data_parameter_spec param_id=\"sp_6\" type=\"double\">epsilon</data_parameter_spec>\n";
print XMLOUT "     <data_parameter_spec param_id=\"sp_7\" type=\"double\">rmin</data_parameter_spec>\n";
print XMLOUT "     <data_parameter_spec param_id=\"sp_8\" type=\"double\">epsilon14</data_parameter_spec>\n";
print XMLOUT "     <data_parameter_spec param_id=\"sp_9\" type=\"double\">rmin14</data_parameter_spec>\n";
print XMLOUT "     <data_parameter_spec param_id=\"sp_10\" type=\"char\">element</data_parameter_spec>\n";
print XMLOUT "  </data_parameter_list>\n";

  for($ii = 0 ; $ii < $NReplicas ; $ii++)
    {
      $offset = $ii * $nsite;
      for($i=1; $i <= $nsite; $i++){
	$j = $offset + $i;
	print XMLOUT " <data_record data_id=\"site_$j\">\n";
	print XMLOUT "   <data_parameter param_ref=\"sp_1\">$mass[$i]</data_parameter>\n";
	print XMLOUT "   <data_parameter param_ref=\"sp_2\">($charge[$i]*ChargeConversion)</data_parameter>\n";
	print XMLOUT "   <data_parameter param_ref=\"sp_3\">$mi[$i]</data_parameter>\n";
	print XMLOUT "   <data_parameter param_ref=\"sp_4\">$cci[$i]</data_parameter>\n";
	print XMLOUT "   <data_parameter param_ref=\"sp_5\">$atname[$i]</data_parameter>\n";
	print XMLOUT "   <data_parameter param_ref=\"sp_6\">$lje[$i]</data_parameter>\n";
	#   print XMLOUT "   <data_parameter param_ref=\"sp_7\">$ljr[$i]</data_parameter>\n";
	print XMLOUT "   <data_parameter param_ref=\"sp_7\">2*$ljr[$i]</data_parameter>\n";
	print XMLOUT "   <data_parameter param_ref=\"sp_8\">$lj14e[$i]</data_parameter>\n";
	#   print XMLOUT "   <data_parameter param_ref=\"sp_9\">$lj14r[$i]</data_parameter>\n";
	print XMLOUT "   <data_parameter param_ref=\"sp_9\">2*$lj14r[$i]</data_parameter>\n";
	print XMLOUT "   <data_parameter param_ref=\"sp_10\">$element[$i]</data_parameter>\n";
###	print XMLOUT "   <data_parameter param_ref=\"sp_10\">$attype[$i]</data_parameter>\n";
	print XMLOUT " </data_record>\n";
      }
    }
print XMLOUT "</data_list>\n";

# bond list info

if ($bond > 0) {
  print XMLOUT "\n";
  print XMLOUT "<data_list data_class_prepend=\"bond_\">\n";
  print XMLOUT " <data_title>bond_list</data_title>\n";
  print XMLOUT " <data_parameter_list param_prepend=\"bondp_\">\n";
  print XMLOUT "   <data_parameter_spec param_id=\"bondp_1\" type=\"idref\" ref_prepend=\"site_\">site_ref1</data_parameter_spec>\n";
  print XMLOUT "   <data_parameter_spec param_id=\"bondp_2\" type=\"idref\" ref_prepend=\"site_\">site_ref2</data_parameter_spec>\n";
  print XMLOUT "   <data_parameter_spec param_id=\"bondp_3\" type=\"char\">type</data_parameter_spec>\n";
  print XMLOUT " </data_parameter_list>\n";
  
  for($ii = 0 ; $ii < $NReplicas ; $ii++)
    {
      $offset = $ii * $bond;
      $atoffset = $ii * $nsite;
      for($i = 1; $i <= $bond; $i++)
	{
	  $j = $offset + $i;
	  #      print "$i $atname[$bondat1[$i]] $atname[$bondat2[$i]] $bondat1[$i] $bondat2[$i]\n";
	  print XMLOUT " <data_record data_id=\"bond_$j\">\n";
	  $a1 = $bondat1[$i] + $atoffset;
	  $a2 = $bondat2[$i] + $atoffset;
	  print XMLOUT "   <data_parameter_ref param_ref=\"bondp_1\" ref_ptr=\"site_$a1\"/>\n";
	  print XMLOUT "   <data_parameter_ref param_ref=\"bondp_2\" ref_ptr=\"site_$a2\"/>\n";
	  print XMLOUT "   <data_parameter param_ref=\"bondp_3\">single</data_parameter>\n";
	  print XMLOUT " </data_record>\n";
	}
    }
  print XMLOUT "</data_list>\n";
}

# udf invocation table

print XMLOUT "<udf_invocation_table>\n";
if ($nonwatbond) {
  print XMLOUT "<udf_invocation_rec udf_id=\"StdHarmonicBondForce\" site_tuple_ref=\"StdHarmonicBondForce_sites\" param_tuple_ref=\"StdHarmonicBondForce_params\"/>\n";
}
if ($nonwatangle) {
  print XMLOUT "<udf_invocation_rec udf_id=\"StdHarmonicAngleForce\" site_tuple_ref=\"StdHarmonicAngleForce_sites\" param_tuple_ref=\"StdHarmonicAngleForce_params\"/>\n";
}
if ($ureyb) {
  print XMLOUT "<udf_invocation_rec udf_id=\"UreyBradleyForce\" site_tuple_ref=\"UreyBradleyForce_sites\" param_tuple_ref=\"UreyBradleyForce_params\"/>\n";
}
if ($torsion) {
  print XMLOUT "<udf_invocation_rec udf_id=\"SwopeTorsionForce\" site_tuple_ref=\"SwopeTorsionForce_sites\" param_tuple_ref=\"SwopeTorsionForce_params\"/>\n";
}
if ($nimproper) {
  print XMLOUT "<udf_invocation_rec udf_id=\"ImproperDihedralForce\" site_tuple_ref=\"ImproperDihedralForce_sites\" param_tuple_ref=\"ImproperDihedralForce_params\"/>\n";
}
print XMLOUT "</udf_invocation_table>\n";

# wat3sites tuple

if ($nwat > 0 ) {
  print XMLOUT "<site_tuple_list id=\"Water3sites\">\n";

  for($ii = 0 ; $ii < $NReplicas ; $ii++)
    {
      $atoffset = $ii * $nsite;
      for($i=1; $i <= $nwat; $i++){
	print XMLOUT "<site_tuple>\n";
	$a1 = $watatom2[$i] + $atoffset;
	$a2 = $watatom1[$i] + $atoffset;
	$a3 = $watatom3[$i] + $atoffset;
	print XMLOUT "<site site_ref=\"site_$a1\" site_ordinal=\"1\"/>\n";
	print XMLOUT "<site site_ref=\"site_$a2\" site_ordinal=\"2\"/>\n";
	print XMLOUT "<site site_ref=\"site_$a3\" site_ordinal=\"3\"/>\n";
	print XMLOUT "</site_tuple>\n";
      }
    }
  print XMLOUT "</site_tuple_list>\n";
  print "Wrote $nwat water site tuples\n"
}

#nonwat bond sites and parameters
if ($nonwatbond) 
{
    print XMLOUT "<site_tuple_list id=\"StdHarmonicBondForce_sites\">\n";
    $nn = 0;

 for($ii = 0 ; $ii < $NReplicas ; $ii++)
    {
      $offset = $ii * $bond;
      $atoffset = $ii * $nsite;
      for($i=1; $i <= $bond; $i++)
	{
	  if (!$watbond[$i]) 
	    {
	      $nn++;
	      $a1 = $bondat1[$i] + $atoffset;
	      $a2 = $bondat2[$i] + $atoffset;
	      print XMLOUT "<site_tuple>\n";
	      print XMLOUT "<site site_ref=\"site_$a1\" site_ordinal=\"1\"/>\n";
	      print XMLOUT "<site site_ref=\"site_$a2\" site_ordinal=\"2\"/>\n";
	      print XMLOUT "</site_tuple>\n";
	    }
	}
    }
#    print "Wrote $nn non-water bonds\n";
    print XMLOUT "</site_tuple_list>\n";
    
    print XMLOUT "<param_tuple_list id=\"StdHarmonicBondForce_params\">\n";
    print XMLOUT "<param_spec_list param_prepend=\"bh_\">\n";
    print XMLOUT "<param_spec param_id=\"bh_1\" type=\"double\">k</param_spec>\n";
    print XMLOUT "<param_spec param_id=\"bh_2\" type=\"double\">r0</param_spec>\n";
    print XMLOUT "</param_spec_list>\n";
    $nnn = 0;

  for($ii = 0 ; $ii < $NReplicas ; $ii++)
    {
      for($i=1; $i <= $bond; $i++)
	{
	  if (!$watbond[$i]) 
	    {
	      $nnn++;
	      print XMLOUT "<param_tuple>\n";
	      print XMLOUT "<param param_ref=\"bh_1\">$bondk[$i]</param>\n";
	      print XMLOUT "<param param_ref=\"bh_2\">$bondr[$i]</param>\n";
	      print XMLOUT "</param_tuple>\n";
	    }
	}
    }
#    print "Wrote $nnn non-water params\n";
    print XMLOUT "</param_tuple_list>\n";
}

# angle sites and parameters
if ($nonwatangle) {
  print XMLOUT "<site_tuple_list id=\"StdHarmonicAngleForce_sites\">\n";
  for($ii = 0 ; $ii < $NReplicas ; $ii++)
    {
      $offset = $ii * $angle;
      $atoffset = $ii * $nsite;

      for($i=1; $i <= $angle; $i++){
	if (!$watangle[$i]) {
	  print XMLOUT "<site_tuple>\n";
	  $a1 = $angleat1[$i] + $atoffset;
	  $a2 = $angleat2[$i] + $atoffset;
	  $a3 = $angleat3[$i] + $atoffset;
	  print XMLOUT "<site site_ref=\"site_$a1\" site_ordinal=\"1\"/>\n";
	  print XMLOUT "<site site_ref=\"site_$a2\" site_ordinal=\"2\"/>\n";
	  print XMLOUT "<site site_ref=\"site_$a3\" site_ordinal=\"3\"/>\n";
	  print XMLOUT "</site_tuple>\n";
	}
      }
    }
  print XMLOUT "</site_tuple_list>\n";

  print XMLOUT "<param_tuple_list id=\"StdHarmonicAngleForce_params\">\n";
  print XMLOUT "<param_spec_list param_prepend=\"ah_\">\n";
  print XMLOUT "<param_spec param_id=\"ah_1\" type=\"double\">k</param_spec>\n";
  print XMLOUT "<param_spec param_id=\"ah_2\" type=\"double\">th0</param_spec>\n";
  print XMLOUT "</param_spec_list>\n";
  for($ii = 0 ; $ii < $NReplicas ; $ii++)
    {
      $offset = $ii * $angle;
      $atoffset = $ii * $nsite;

      for($i=1; $i <= $angle; $i++){
	if (!$watangle[$i]) {
	  print XMLOUT "<param_tuple>\n";
	  print XMLOUT "<param param_ref=\"ah_1\">$anglek[$i]</param>\n";
	  print XMLOUT "<param param_ref=\"ah_2\">($angle0[$i]*Math::Deg2Rad)</param>\n";
	  print XMLOUT "</param_tuple>\n";
	}
      }
    }
  print XMLOUT "</param_tuple_list>\n";
}

#urey-bradley sites and parameters
if ($ureyb > 0) {
  print XMLOUT "<site_tuple_list id=\"UreyBradleyForce_sites\">\n";

  for($ii = 0 ; $ii < $NReplicas ; $ii++)
    {
      $offset = $ii * $angle;
      $atoffset = $ii * $nsite;
      for($i=1; $i <= $ureyb; $i++){
	print XMLOUT "<site_tuple>\n";
	print XMLOUT "<site site_ref=\"site_$ureybat1[$i]\" site_ordinal=\"1\"/>\n";
	print XMLOUT "<site site_ref=\"site_$ureybat3[$i]\" site_ordinal=\"2\"/>\n";
	print XMLOUT "</site_tuple>\n";
      }
    }
  print XMLOUT "</site_tuple_list>\n";
  print XMLOUT "<param_tuple_list id=\"UreyBradleyForce_params\">\n";
  print XMLOUT "<param_spec_list param_prepend=\"ub_\">\n";
  print XMLOUT "<param_spec param_id=\"ub_1\" type=\"double\">k</param_spec>\n";
  print XMLOUT "<param_spec param_id=\"ub_2\" type=\"double\">s0</param_spec>\n";
  print XMLOUT "</param_spec_list>\n";
  for($ii = 0 ; $ii < $NReplicas ; $ii++)
    {
      $offset = $ii * $angle;
      $atoffset = $ii * $nsite;
      for($i=1; $i <= $ureyb; $i++){
	print XMLOUT "<param_tuple>\n";
	print XMLOUT "<param param_ref=\"ub_1\">$ureybk[$i]</param>\n";
	print XMLOUT "<param param_ref=\"ub_2\">$ureyb0[$i]</param>\n";
	print XMLOUT "</param_tuple>\n";
      }
    }
  print XMLOUT "</param_tuple_list>\n";
}

#swopetorsion sites and parameters

if ($torsion > 0) {
  print XMLOUT "<site_tuple_list id=\"SwopeTorsionForce_sites\">\n";
  for($ii = 0 ; $ii < $NReplicas ; $ii++)
    {
      $offset = $ii * $torsion;
      $atoffset = $ii * $nsite;

      for($i=1; $i <= $torsion; $i++){
	print XMLOUT "<site_tuple>\n";
	  $a1 = $torsionat1[$i] + $atoffset;
	  $a2 = $torsionat2[$i] + $atoffset;
	  $a3 = $torsionat3[$i] + $atoffset;
	  $a4 = $torsionat4[$i] + $atoffset;
	print XMLOUT "<site site_ref=\"site_$a1\" site_ordinal=\"1\"/>\n";
	print XMLOUT "<site site_ref=\"site_$a2\" site_ordinal=\"2\"/>\n";
	print XMLOUT "<site site_ref=\"site_$a3\" site_ordinal=\"3\"/>\n";
	print XMLOUT "<site site_ref=\"site_$a4\" site_ordinal=\"4\"/>\n";
	print XMLOUT "</site_tuple>\n";
      }
    }
  print XMLOUT "</site_tuple_list>\n";
  print XMLOUT "<param_tuple_list id=\"SwopeTorsionForce_params\">\n";
  print XMLOUT "<param_spec_list param_prepend=\"ctp_\">\n";
  print XMLOUT "<param_spec param_id=\"ctp_1\" type=\"double\">k</param_spec>\n";
  print XMLOUT "<param_spec param_id=\"ctp_2\" type=\"int\">n</param_spec>\n";
  print XMLOUT "<param_spec param_id=\"ctp_3\" type=\"double\">sinDelta</param_spec>\n";
  print XMLOUT "<param_spec param_id=\"ctp_4\" type=\"double\">cosDelta</param_spec>\n";
  print XMLOUT "</param_spec_list>\n";
  for($ii = 0 ; $ii < $NReplicas ; $ii++)
    {
      $offset = $ii * $torsion;
      $atoffset = $ii * $nsite;
      for($i=1; $i <= $torsion; $i++){
	print XMLOUT "<param_tuple>\n";
	print XMLOUT "<param param_ref=\"ctp_1\">$torsionk[$i]</param>\n";
	print XMLOUT "<param param_ref=\"ctp_2\">$torsionn[$i]</param>\n";
	print XMLOUT "<param param_ref=\"ctp_3\">sin($torsiond[$i]*Math::Deg2Rad)</param>\n";
	print XMLOUT "<param param_ref=\"ctp_4\">cos($torsiond[$i]*Math::Deg2Rad)</param>\n";
	print XMLOUT "</param_tuple>\n";
      }
    }
  print XMLOUT "</param_tuple_list>\n";
}

#improper sites and parameters
if ($improper > 0) {
  print XMLOUT "<site_tuple_list id=\"ImproperDihedralForce_sites\">\n";
  for($ii = 0 ; $ii < $NReplicas ; $ii++)
    {
      $offset = $ii * $angle;
      $atoffset = $ii * $nsite;

      for($i=1; $i <= $improper; $i++){
	print XMLOUT "<site_tuple>\n";
	$a1 = $improperat1[$i] + $atoffset;
	$a2 = $improperat2[$i] + $atoffset;
	$a3 = $improperat3[$i] + $atoffset;
	$a4 = $improperat4[$i] + $atoffset;
	print XMLOUT "<site site_ref=\"site_$a1\" site_ordinal=\"1\"/>\n";
	print XMLOUT "<site site_ref=\"site_$a2\" site_ordinal=\"2\"/>\n";
	print XMLOUT "<site site_ref=\"site_$a3\" site_ordinal=\"3\"/>\n";
	print XMLOUT "<site site_ref=\"site_$a4\" site_ordinal=\"4\"/>\n";
	print XMLOUT "</site_tuple>\n";
      }
    }
  print XMLOUT "</site_tuple_list>\n";
  print XMLOUT "<param_tuple_list id=\"ImproperDihedralForce_params\">\n";
  print XMLOUT "<param_spec_list param_prepend=\"citp_\">\n";
  print XMLOUT "<param_spec param_id=\"citp_1\" type=\"double\">k</param_spec>\n";
  print XMLOUT "<param_spec param_id=\"citp_2\" type=\"double\">psi0</param_spec>\n";
  print XMLOUT "</param_spec_list>\n";
  for($ii = 0 ; $ii < $NReplicas ; $ii++)
    {
      for($i=1; $i <= $improper; $i++){
	print XMLOUT "<param_tuple>\n";
	print XMLOUT "<param param_ref=\"citp_1\">$improperk[$i]</param>\n";
	print XMLOUT "<param param_ref=\"citp_2\">($improperd[$i]*Math::Deg2Rad)</param>\n";
	print XMLOUT "</param_tuple>\n";
      }
    }
  print XMLOUT "</param_tuple_list>\n";
}

print XMLOUT"</ffparams>\n";

close(XMLOUT);

###############################################################

sub visit {
  push(@molesite,$_[0]);
  $sitemole[$_[0]] = $molecount;
  $val[$_[0]] = 1;
  for ($j = 1; $j <= $natch[$_[0]]; ++$j) {
    if ($val[$adj[$_[0]][$j]] == 0) {
      push(@stack,$adj[$_[0]][$j]);
      ++$val[$adj[$_[0]][$j]];
    }
  }
#  print " stack: @stack \n site:@molesite \n";
  if (@stack != "") {
    $lastsite = pop(@stack);
    visit($lastsite);
  }
}

sub by_number {
 $a <=> $b;
}

sub checkmoleculetype {
  $foo = "null";
  if ($_[0] eq 'ALA' || $_[0] eq 'ARG' || $_[0] eq 'ACE' ||
      $_[0] eq 'ARD' || $_[0] eq 'ASH' || $_[0] eq 'ASP' || $_[0] eq 'ASN' ||
      $_[0] eq 'CYS' || $_[0] eq 'CYD' || $_[0] eq 'CYX' || $_[0] eq 'GLH' ||
      $_[0] eq 'GLU' || $_[0] eq 'GLN' || $_[0] eq 'GLY' || $_[0] eq 'HIH' ||
      $_[0] eq 'HIS' || $_[0] eq 'HIE' || $_[0] eq 'ILE' || $_[0] eq 'LEU' ||
      $_[0] eq 'LYS' || $_[0] eq 'LYD' || $_[0] eq 'MET' || $_[0] eq 'PHE' ||
      $_[0] eq 'PRO' || $_[0] eq 'SER' || $_[0] eq 'SED' || $_[0] eq 'THR' ||
      $_[0] eq 'TRP' || $_[0] eq 'TYR' || $_[0] eq 'TYD' || $_[0] eq 'VAL' ||
      $_[0] eq 'NT1' || $_[0] eq 'NT2' || $_[0] eq 'NT3' || $_[0] eq 'CT5' ||
      $_[0] eq 'CT1' || $_[0] eq 'CT2' || $_[0] eq 'CT3' || $_[0] eq 'CT4' ){
    $foo = "protein"
  } elsif ($_[0] eq 'TIP3' || $_[0] eq 'TIP4' || $_[0] eq 'TIP5' ||
	   $_[0] eq 'SPC'  || $_[0] eq 'SPCE' || $_[0] eq 'DMSO' ||
	   $_[0] eq 'MEOH' || $_[0] eq 'UREA' || $_[0] eq 'HOH' ){
    $foo = "solvent"
  } elsif ($_[0] eq 'NUSA' || $_[0] eq 'NUSC' || $_[0] eq 'NUSG' ||
	   $_[0] eq 'NUST' || $_[0] eq 'THY' || $_[0] eq 'T' ){
    $foo = "dna"
  } elsif ($_[0] eq 'RNUA' || $_[0] eq 'RNUC' || $_[0] eq 'RNUG' ||
	   $_[0] eq 'RNUU' || $_[0] eq 'URA' || $_[0] eq 'U' ){
    $foo = "rna"
  } elsif ($_[0] eq 'ADE' || $_[0] eq 'CYT' || $_[0] eq 'GUA' ||
	   $_[0] eq 'A' || $_[0] eq 'C' || $_[0] eq 'G' ){
    $foo = "nucleic"
  } elsif ($_[0] eq 'OLEO' || $_[0] eq 'PCGL' ){
    $foo = "lipid"
  } elsif ($_[0] eq 'UNK')
  {$foo = "ligand"}
  elsif($_[0] eq 'F'  || $_[0] eq 'NA' || $_[0] eq 'CA' || $_[0] eq 'MG' ||
	$_[0] eq 'AR' || $_[0] eq 'FE' || $_[0] eq 'K'  || $_[0] eq 'CL') 
  {	  $foo = "salt" }
  else
  { print "Unknown molecule type $_[0]\n"; die;}
  return($foo);
}

sub checkpolymer {
  if ($_[0] eq 'protein' || $_[0] eq 'dna' || $_[0] eq 'rna') {
    return(1);
  } else {
    return(0);
  }
}

sub convertatomtag {

    $tmpname = ConvertIonAtname ($_[0]);
    if($tmpname ne "")
    {
	$resname[$_[2]] = ConvertIonResname($_[1]);
	return ($tmpname);
    }
  if ($_[0] eq "HN")  {return ("H");}
  if ($_[1] eq "HOH" ) 
  {
      if ($_[0] eq "OH2")  {return ("O");}
#      if ($_[0] eq "H1")  {return ("H");}
#      if ($_[0] eq "H2")  {return ("H");}
      return($_[0]);
  }
    if ($_[1] eq "ARG") {
	if ($_[0] eq "HB1")  {return ("1HB");}
	if ($_[0] eq "HB2")  {return ("2HB");}
	if ($_[0] eq "HG1")  {return ("1HG");}
	if ($_[0] eq "HG2")  {return ("2HG");}
	if ($_[0] eq "HD1")  {return ("1HD");}
	if ($_[0] eq "HD2")  {return ("2HD");}
	if ($_[0] eq "HH11") {return ("1HH1");}
	if ($_[0] eq "HH12") {return ("2HH1");}
	if ($_[0] eq "HH21") {return ("1HH2");}
	if ($_[0] eq "HH22") {return ("2HH2");}
	return($_[0]);
    }
    if ($_[1] eq "ASN") {
	if ($_[0] eq "HB1")  {return ("1HB");}
	if ($_[0] eq "HB2")  {return ("2HB");}
	if ($_[0] eq "HD21") {return ("1HD2");}
	if ($_[0] eq "HD22") {return ("2HD2");}
	return($_[0]);
    }
    if ($_[1] eq "LEU") {
	if ($_[0] eq "HB1")  {return ("1HB");}
	if ($_[0] eq "HB2")  {return ("2HB");}
	if ($_[0] eq "HG1")  {return ("1HG");}
	if ($_[0] eq "HG2")  {return ("2HG");}
	if ($_[0] eq "HD11") {return ("1HD1");}
	if ($_[0] eq "HD12") {return ("2HD1");}
	if ($_[0] eq "HD13") {return ("3HD1");}
	if ($_[0] eq "HD21") {return ("1HD2");}
	if ($_[0] eq "HD22") {return ("2HD2");}
	if ($_[0] eq "HD23") {return ("3HD2");}
	return($_[0]);
    }
    if ($_[1] eq "THR") {
	if ($_[0] eq "HG21")  {return ("1HG2");}
	if ($_[0] eq "HG22")  {return ("2HG2");}
	if ($_[0] eq "HG23")  {return ("3HG2");}
	return($_[0]);
    }
    if ($_[1] eq "VAL") {
	if ($_[0] eq "HG11")  {return ("1HG1");}
	if ($_[0] eq "HG12")  {return ("2HG1");}
	if ($_[0] eq "HG13")  {return ("3HG1");}
	if ($_[0] eq "HG21")  {return ("1HG2");}
	if ($_[0] eq "HG22")  {return ("2HG2");}
	if ($_[0] eq "HG23")  {return ("3HG2");}
	return($_[0]);
    }
    if ($_[1] eq "ILE") {
	if ($_[0] eq "CD")    {return ("CD1");}
	if ($_[0] eq "HG11")  {return ("1HG1");}
	if ($_[0] eq "HG12")  {return ("2HG1");}
	if ($_[0] eq "HG21")  {return ("1HG2");}
	if ($_[0] eq "HG22")  {return ("2HG2");}
	if ($_[0] eq "HG23")  {return ("3HG2");}
	if ($_[0] eq "HD11")  {return ("1HD1");}
	if ($_[0] eq "HD12")  {return ("2HD1");}
	if ($_[0] eq "HD13")  {return ("3HD1");}
	if ($_[0] eq "HD1")  {return ("1HD1");}
	if ($_[0] eq "HD2")  {return ("2HD1");}
	if ($_[0] eq "HD3")  {return ("3HD1");}
	return($_[0]);
    }
    if ($_[1] eq "PRO") {
	if ($_[0] eq "HN1")  {return ("1H");}
	if ($_[0] eq "HN2")  {return ("2H");}
	if ($_[0] eq "HB1")  {return ("1HB");}
	if ($_[0] eq "HB2")  {return ("2HB");}
	if ($_[0] eq "HG1")  {return ("1HG");}
	if ($_[0] eq "HG2")  {return ("2HG");}
	if ($_[0] eq "HD1")  {return ("1HD");}
	if ($_[0] eq "HD2")  {return ("2HD");}
	return($_[0]);
    }
    if ($_[1] eq "GLY") {
	if ($_[0] eq "HA1")  {return ("1HA");}
	if ($_[0] eq "HA2")  {return ("2HA");}
	return($_[0]);
    }
    if ($_[1] eq "ALA") {
	if ($_[0] eq "HB1")  {return ("1HB");}
	if ($_[0] eq "HB2")  {return ("2HB");}
	if ($_[0] eq "HB3")  {return ("3HB");}
	return($_[0]);
    }
    if ($_[1] eq "LYS" || $_[1] eq "LYD") {
	if ($_[0] eq "HB1")  {return ("1HB");}
	if ($_[0] eq "HB2")  {return ("2HB");}
	if ($_[0] eq "HG1")  {return ("1HG");}
	if ($_[0] eq "HG2")  {return ("2HG");}
	if ($_[0] eq "HD1")  {return ("1HD");}
	if ($_[0] eq "HD2")  {return ("2HD");}
	if ($_[0] eq "HE1")  {return ("1HE");}
	if ($_[0] eq "HE2")  {return ("2HE");}
	if ($_[0] eq "HZ1")  {return ("1HZ");}
	if ($_[0] eq "HZ2")  {return ("2HZ");}
	if ($_[0] eq "HZ3")  {return ("3HZ");}
	return($_[0]);
    }
    if ($_[1] eq "MET") {
	if ($_[0] eq "HB1")  {return ("1HB");}
	if ($_[0] eq "HB2")  {return ("2HB");}
	if ($_[0] eq "HG1")  {return ("1HG");}
	if ($_[0] eq "HG2")  {return ("2HG");}
	if ($_[0] eq "HE1")  {return ("1HE");}
	if ($_[0] eq "HE2")  {return ("2HE");}
	if ($_[0] eq "HE3")  {return ("3HE");}
	return($_[0]);
    }
    if ($_[1] eq "GLN" || $_[1] eq "GLH" || $_[1] eq "GLU") {
	if ($_[0] eq "HB1")  {return ("1HB");}
	if ($_[0] eq "HB2")  {return ("2HB");}
	if ($_[0] eq "HG1")  {return ("1HG");}
	if ($_[0] eq "HG2")  {return ("2HG");}
	if ($_[0] eq "HE21") {return ("1HE2");}
	if ($_[0] eq "HE22") {return ("2HE2");}
	return($_[0]);
    }
    if ($_[1] eq "CYS") {
	if ($_[0] eq "HB1")  {return ("1HB");}
	if ($_[0] eq "HB2")  {return ("2HB");}
	if ($_[0] eq "HG1")  {return ("HG");}
	return($_[0]);
    }
    if ($_[1] eq "SER") {
	if ($_[0] eq "HB1")  {return ("1HB");}
	if ($_[0] eq "HB2")  {return ("2HB");}
	if ($_[0] eq "HG1")  {return ("HG");}
	return($_[0]);
    }
    if ($_[1] eq "ASH" || $_[1] eq "ASP" || $_[1] eq "CYD" ||
	$_[1] eq "CYX" || $_[1] eq "HIH" || $_[1] eq "HIS" || $_[1] eq "HIE" ||
	$_[1] eq "SER" || $_[1] eq "SED" || $_[1] eq "PHE" || $_[1] eq "TYR" ||
	$_[1] eq "TYD" || $_[1] eq "TRP" ) {
	if ($_[0] eq "HB1")  {return ("1HB");}
	if ($_[0] eq "HB2")  {return ("2HB");}
	return($_[0]);
    }
    return($_[0]);
}

sub convertcapatomtag {
  if (!$cap) { 
    if ($atname[$i] eq "HT1") {$atname[$i] = "1H";}
    if ($atname[$i] eq "HT2") {$atname[$i] = "2H";}
    if ($atname[$i] eq "HT3") {$atname[$i] = "3H";}
    if ($atname[$i] eq "HN1") {$atname[$i] = "1H";}
    if ($atname[$i] eq "HN2") {$atname[$i] = "2H";}
    if ($atname[$i] eq "OT1") {$atname[$i] = "O";}
    if ($atname[$i] eq "OT2") {$atname[$i] = "OXT";}
    if ($atname[$i] eq "HY1") {$atname[$i] = "1H" ;$resname[$i] = "ACE";$resnum[$i] = $resnum[$i] - 1;}
    if ($atname[$i] eq "HY2") {$atname[$i] = "2H" ;$resname[$i] = "ACE";$resnum[$i] = $resnum[$i] - 1;}
    if ($atname[$i] eq "HY3") {$atname[$i] = "3H" ;$resname[$i] = "ACE";$resnum[$i] = $resnum[$i] - 1;}
    if ($atname[$i] eq "CAY") {$atname[$i] = "CH3";$resname[$i] = "ACE";$resnum[$i] = $resnum[$i] - 1;}
    if ($atname[$i] eq "CY")  {$atname[$i] = "C"  ;$resname[$i] = "ACE";$resnum[$i] = $resnum[$i] - 1;}
    if ($atname[$i] eq "OY")  {$atname[$i] = "O"  ;$resname[$i] = "ACE";$resnum[$i] = $resnum[$i] - 1;}
    if ($atname[$i] eq "NT")  {$atname[$i] = "N"  ;$resname[$i] = "CT5";$resnum[$i] = $resnum[$i] + 1;}
  } else {
    if ($atname[$i] eq "HNT") {$atname[$i] = "H"  ;$resname[$i] = "CT5";$resnum[$i] = $resnum[$i] + 1;}
    if ($atname[$i] eq "CAT") {$atname[$i] = "CA" ;$resname[$i] = "CT5";$resnum[$i] = $resnum[$i] + 1;}
    if ($atname[$i] eq "HT1") {$atname[$i] = "HA1";$resname[$i] = "CT5";$resnum[$i] = $resnum[$i] + 1;}
    if ($atname[$i] eq "HT2") {$atname[$i] = "HA2";$resname[$i] = "CT5";$resnum[$i] = $resnum[$i] + 1;}
    if ($atname[$i] eq "HT3") {$atname[$i] = "HA3";$resname[$i] = "CT5";$resnum[$i] = $resnum[$i] + 1;}
  }
  return;
  if ($atname[$i] eq "OT1") {$atname[$i] = "O1";$resname[$i] = "CT3";}
  if ($atname[$i] eq "OT2") {$atname[$i] = "O2";$resname[$i] = "CT3";}
  if ($atname[$i] eq "CT")  {$atname[$i] = "CA";$resname[$i] = "CT3";}
  if ($atname[$i] eq "HT1") {$atname[$i] = "1H";$resname[$i] = "CT3";}
  if ($atname[$i] eq "HT2") {$atname[$i] = "2H";$resname[$i] = "CT3";}
  if ($atname[$i] eq "HT3") {$atname[$i] = "3H";$resname[$i] = "CT3";}
  if ($atname[$i] eq "NT")  {$atname[$i] = "N"; $resname[$i] = "CT4";}
  if ($atname[$i] eq "HT1") {$atname[$i] = "1H";$resname[$i] = "CT4";}
  if ($atname[$i] eq "HT2") {$atname[$i] = "2H";$resname[$i] = "CT4";}
}


sub ConvertIonResname{
    if ($_[0] =~ /SOD/)
    {  return ("NA"); }
    if ($_[0] =~ /CAL/)
    {  return ("CA"); }
    if ($_[0] =~ /FE/)
    {  return ("FE"); }
    if ($_[0] =~ /CL/)
    {  return ("CL"); }
    if ($_[0] =~ /MG/)
    {  return ("MG"); }
    if ($_[0] =~ /AR/)
    {  return ("AR"); }
    return (""); 
}

sub ConvertIonAtname {

    if ($_[0] =~ /SOD/)
    {  return ("NA_"); }
    if ($_[0] =~ /CAL/)
    {  return ("CA_"); }
    if ($_[0] =~ /FE/)
    {  return ("FE_"); }
    if ($_[0] =~ /CL/)
    {  return ("CL_"); }
    if ($_[0] =~ /MG/)
    {  return ("MG_"); }
    if ($_[0] =~ /AR/)
    {  return ("AR_"); }
    return (""); 

}



