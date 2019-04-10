#!/usr/agora/bin/perl5
####################################################################
### pdb2frg.pl 
###          script to generate a fragment scheme from pdb file for Bluematter
###          each residue will be a fragment
###
### usage:   pdb2dvs.pl < jobname.pdb > jobname.frg
### example: pdb2dvs.pl < enkp.pdb > enkp.frg 
### 
### input:   pdb file
### output:  frg file
###
### R. Zhou, IBM, 11/2000
####################################################################

$natom = 0;
$nres = 0;
$preres=" ";

while (<STDIN>) {
  if (/CONECT/) {last;}
  if( (/ATOM  /) || (/HETATM/) ){
    # @fft = split;
    # $resname = $fft[4];
    $resnum = substr($_,23,4); 
    if ($resnum ne $preres){
      $firstatom[$nres] = $natom + 1;
      $preres = $resnum;
      $nres++;
    } 
    $natom++;
  }
}
$firstatom[$nres] = $natom+1;

# print out fragment information

printf ("%d\n", $nres);
for($i=0; $i<$nres; $i++){
  printf("%d\n",$firstatom[$i+1] - $firstatom[$i]);
  for($j=$firstatom[$i]; $j <$firstatom[$i+1]; $j++){
    printf("%d\n", $j);
  }
}





