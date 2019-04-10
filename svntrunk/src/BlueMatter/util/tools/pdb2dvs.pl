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
while (<STDIN>) {
#  if (/CONECT/) {last;}
  if( (/ATOM  /) || (/HETATM/) ){
    # $coord = substr($_,28, 30);
    # $_ = $coord;
    # @fft = split;
    # $x[$i] =  $fft[0];
    # $y[$i] =  $fft[1];
    # $z[$i] =  $fft[2];

    # to handle awful pdb files
    $x[$i] =  substr($_,30,8);
    $y[$i] =  substr($_,38,8); 
    $z[$i] =  substr($_,46,8);
    $i++;
  }
}

$total_atoms = $i;
printf ("%d\n", $total_atoms);
for($i=0; $i<$total_atoms; $i++){
  printf("%12.5f %12.5f %12.5f\n", $x[$i], $y[$i],$z[$i]);
}

$vx = $vy = $vz = 0.0;
printf ("%d\n", $total_atoms);
for($i=0; $i<$total_atoms; $i++){
  printf("%12.5f %12.5f %12.5f\n", $vx, $vy,$vz);
}
