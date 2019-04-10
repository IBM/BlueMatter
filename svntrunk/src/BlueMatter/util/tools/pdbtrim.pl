#!/usr/bin/perl
####################################################################
### pdb2dvs.pl 
###          script to convert pdb file to dvs file for Bluematter
###
### usage:   pdb2dvs.pl < jobname.pdb > jobname.dvs
### example: pdb2dvs.pl < enkp.pdb > enkp.dvs 
### 
### input:   pdb file
### output:  dvs file
###
### R. Zhou, IBM, 11/2000
####################################################################

$i = 0;
$replicate = 0;

 #  read in bounding box
$xmin = shift(@ARGV);
$ymin = shift(@ARGV);
$zmin = shift(@ARGV);
$xmax = shift(@ARGV);
$ymax = shift(@ARGV);
$zmax = shift(@ARGV);
$MIN_DIST = shift(@ARGV);


while (<STDIN>) {
  if (/CONECT/) {last;}
  if (/REMARK/) {print "$_";}
  if( (/ATOM  /) || (/HETATM/) ){
    # $coord = substr($_,28, 30);
    # $_ = $coord;
    # @fft = split;
    # $x[$i] =  $fft[0];
    # $y[$i] =  $fft[1];
    # $z[$i] =  $fft[2];
    
    # to handle awful pdb files
    $pdbrecs[$i] = $_;
    $x[$i] =  substr($_,30,8);
    $y[$i] =  substr($_,38,8); 
    $z[$i] =  substr($_,46,8);
    
    $keep[$i] = 1;
    $watO[$i] = 0;
    if(/TIP3/ && /OH2/ )
      {
	$watO[$i] = 1;
	if(IsLow($i))
	  {	    $keep[$i] = 0;	  }
	if(IsHigh($i))
	  {	    $keep[$i] = 0;	  }
      }
    elsif(/TIP3/ && / H1 / )
      {
	$watH1[$i] = 1;
	if($keep[$i-1] == 0){	    $keep[$i] = 0;	  }
      }
    elsif(/TIP3/ && / H2 / )
      {
	$watH2[$i] = 1;
	if($keep[$i-2] == 0)
	  {	    $keep[$i] = 0;	  }
	
      }
    
    $i++;
  }
}

$n = $i;

for($i=0; $i<$n;$i++)
{
    if($watO[$i])
    {
	if($keep[$i] == 0)
	{
	    $keep[$i+1] = 0;
	    $keep[$i+2] = 0;
#	    print "Water $i is fragmented\n";
	}
    }
}

$xmn = $x[0];
$xmx = $x[0];
$ymn = $y[0];
$ymx = $y[0];
$zmn = $z[0];
$zmx = $z[0];

for($i = 1;$i < $n; $i++)
  {
    if($x[$i] < $xmn){$xmn = $x[$i];}
    if($x[$i] > $xmx){$xmx = $x[$i];}
    if($y[$i] < $ymn){$ymn = $y[$i];}
    if($y[$i] > $ymx){$ymx = $y[$i];}
    if($z[$i] < $zmn){$zmn = $z[$i];}
    if($z[$i] > $zmx){$zmx = $z[$i];}
  }

printf "REMARK XRANGE { $xmn -- $xmx }\n";
printf "REMARK YRANGE { $ymn -- $ymx }\n";
printf "REMARK ZRANGE { $zmn -- $zmx }\n";
printf "REMARK XCLIP { $xmin -- $xmax }\n";
printf "REMARK YCLIP { $ymin -- $ymax }\n";
printf "REMARK ZCLIP { $zmin -- $zmax }\n";

$noh2 = 0;
$nh1 = 0;
$nh2 = 0;
#for($i=0;$i<$n;$i++)
#  {
#    if(watO[$i]
#  }

$ncw = ScanDists();
print "REMARK checked $ncw waters\n";

for($i = 0;$i < $n; $i++)
  {
    if ($keep[$i] == 1) { print "$pdbrecs[$i]";}
  }

if($replicate)
{
    $xtran = $xmax - $xmin;
    $ytran = $ymax - $ymin;
    
    trans( $xtran, 0.0 );
    
    for($i = 0;$i < $n; $i++)
    {
	if ($keep[$i] == 1) { print "$pdbrecs[$i]";}
    }
    
    trans( -$xtran, $ytran );
    
    
    for($i = 0;$i < $n; $i++)
    {
	if ($keep[$i] == 1) { print "$pdbrecs[$i]";}
    }
    
    trans( $xtran, 0.0 );
    
    for($i = 0;$i < $n; $i++)
    {
	if ($keep[$i] == 1) { print "$pdbrecs[$i]";}
    }
}

sub IsLow
{

$ii = $_[0];

#print " $ii has coord $x[$ii] $y[$ii] $z[$ii]  ";
  if($x[$ii] < $xmin || $y[$ii] < $ymin || $z[$ii] < $zmin )  { #print "TOO LOW $xmin $ymin $zmin\n"; 
								return 1; }

#print "\n";
return 0;

}

sub IsHigh
{

$ii = $_[0];
#print " $ii has coord $x[$ii] $y[$ii] $z[$ii]  ";

  if($x[$ii] > $xmax || $y[$ii] > $ymax || $z[$ii] > $zmax )  { #print "TOO HIGH $xmax $ymax $zmax\n"; 
								return 1; }
#print "\n";
return 0;

}

sub trans
  {

for($i = 0;$i < $n;$i++)
  {
    $x[$i] +=  $_[0];
    $y[$i] +=  $_[1]; 

    $pref = substr($pdbrecs[$i],0,30);
    $suf = substr($pdbrecs[$i],54,26);
    $pdbrecs[$i] = sprintf "%s%8.3f%8.3f%8.3f%s",$pref,$x[$i],$y[$i],$z[$i],$suf;
#print" changed to $pdbrecs[$i]";
  }
return 1;
  }

sub ScanDists
  {

    $nw = 0;
#    print "Checking $n atoms\n";
    for($i=0;$i<$n-1;$i++)
      {
	if($keep[$i] == 0){next;}
	if($watO[$i] == 0){next;}
	$nw++;
#	next;
	for($j=$i+1;$j<$n;$j++)
	  {
	    if($keep[$j] == 0){next;}
	    if($watO[$j] == 0){next;}
	    $d = sqrt(($x[$i] - $x[$j]) * ($x[$i] - $x[$j]) +
	         ($y[$i] - $y[$j]) * ($y[$i] - $y[$j]) +
		($z[$i] - $z[$j]) * ($z[$i] - $z[$j]));
	    if($d > 1.6 && $d < $MIN_DIST)
	      {
		$ii = $i + 1; 
		$jj = $j + 1;
#		print " atom $ii and $jj are too close : $d\n$pdbrecs[$i]$pdbrecs[$j]\n";
		$keep[$j] = 0;
		$keep[$j+1] = 0;
		$keep[$j+2] = 0;
	      }
	  }
      }
    return $nw;
  }
