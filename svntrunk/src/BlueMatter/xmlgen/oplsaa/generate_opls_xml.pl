#!/usr/bin/perl
####################################################################
### generate_all_xml.pl 
###          A perl script file for generating all force field independent
###          parameter files for OPLS-AA from IMPACT runs. It will
###          call opls2xml.pl and generate all XML files in a file list 
###
### usage:   generate_all_xml.pl joblist
### example: generate_all_xml.pl oplsaajobs.lst 
###          
###
### input:   a file containing all job lists 
### output:  XML files  
###
### RZ, IBM, 04/2002
### modified from generate_all_xml.pl (this script assumes data files are
### already stored under mdtest/data/oplsaa/) RZ 05/02
####################################################################

$installbase = `installbase.py`;
chop $installbase;
$bgbindir  = $installbase . "usr/opt/bluegene/bin";
#$bgbindir  = $ENV{"BG_BIN_DIR"};
#$sbbasedir = $ENV{"SANDBOXBASE"};
#$bmdatadir = "$sbbasedir/src/mdtest/data/bluematter/";
$bmdatadir = $installbase . "usr/opt/bluegene/mdtest/data/bluematter/";
#$oplsdatadir = "$sbbasedir/src/mdtest/data/oplsaa/";
$oplsdatadir = $installbase . "usr/opt/bluegene/mdtest/data/oplsaa/";
$bgdriverdir = $installbase . "usr/opt/bluegene/mdtest/drivers/external/oplsaa/brooks";
$xml2db2   = "$bgbindir/setup/xml2db2.pl";
$xml2exe   = "$bgbindir/setup/xml2exe.py";
$mdlogger  = "$bgbindir/analysis/MDLogger";


$filelist  = shift(@ARGV);

open(FILIN,"<$filelist");
$nfile = 0;

### collect file (direcory) names for all the job list and assign right names

while (<FILIN>) {
  chomp;
  if ($_ =~ /^\w/) {
    $prefix = $_;
    $nfile++;
    $fileout[$nfile] = "$prefix.out";
    $filepdb[$nfile] = "$prefix.pdb";
    $fileprm[$nfile] = "$prefix.prm";
    $filexml[$nfile] = "$prefix.opls.xml";
  }
}

### generating XML one by one and copy them to the right place

for ($i = 1; $i <= $nfile; ++$i) {
  printf ("Generating XML $filexml[$i]: \n");
  system("cp $oplsdatadir/$fileout[$i].bz2 .");
  system("cp $oplsdatadir/$filepdb[$i].bz2 .");
  system("bunzip2 $fileout[$i].bz2");
  system("bunzip2 $filepdb[$i].bz2");

  system("$bgbindir/setup/opls2xml.pl $fileout[$i] $filepdb[$i]") && die "can't run opls2xml.pl, please check";
#  system("mv $filexml[$i] $bmdatadir") && warn "can't copy XML file";
  system("rm -f $fileout[$i]");
  system("rm -f $filepdb[$i]");
  system("rm -f $fileprm[$i]");
}

