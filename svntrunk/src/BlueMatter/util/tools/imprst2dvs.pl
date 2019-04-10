#!/usr/bin/perl
####################################################################
### imprst2dvs.pl 
###          script to convert IMPACT restart file to dvs file for Bluematter
###
### usage:   imprst2dvs.pl < jobname.rst > jobname.dvs
### example: imprst2dvs.pl < enkp.rst > enkp.dvs 
### 
### input:   IMPACT rst file
### output:  BlueMatter dvs file
###
### R. Zhou, IBM, 07/2002
####################################################################


$_=<STDIN>;

# get number of species in IMPACT
$_=<STDIN>;
@fft = split;
$nspec = $fft[8];

# get number of molecules and atoms in each molecule
$_=<STDIN>;
@mol = split;
$_=<STDIN>;
@atom = split;

#calculate the total atoms/particles
if($nspec == 1){
  $natoms = $mol[1] * $atom[1];
}
elsif($nspec == 2){
  $natoms = $mol[1] * $atom[1] + $mol[2] * $atom[2];
}
$_=<STDIN>;

printf(" %d\n", $natoms);

# coordinates & velocities
$i=0;
while(<STDIN>){
  print $_;
  $i++;
}

if($i != 2*$natoms){
  printf(stderr "The number of atoms are not right, please check!\n");
}
  
