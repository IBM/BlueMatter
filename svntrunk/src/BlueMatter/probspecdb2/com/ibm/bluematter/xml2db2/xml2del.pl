#!/usr/bin/env perl
$xmlFileName = $ARGV[0];
$systemName = $xmlFileName;
$systemName =~ s/\.(.)*//g;
print "Processing: $systemName\n";

$usrName = `whoami`;
chop($usrName);
$time = `date +"%d%T"`;
chop($time);
$directoryName = "/tmp/${systemName}_${usrName}_$time";

print "directoryName = $directoryName \n";
#print $directoryName ."--\n"; exit;
system("mkdir $directoryName");
system("chmod a+rx $directoryName");


$temp = $ENV{BG_BIN_DIR};

if ($temp) {
  $bg_bin_dir = "$temp\/setup";
}

print "$bg_bin_dir\n";
system("testXSLT -in $xmlFileName -xsl $bg_bin_dir/xml2sdel.xsl -out $directoryName/$systemName.out -validate");

system("splitoutput.pl $directoryName/$systemName.out -1 $directoryName");

print "DEL files are in $directoryName \n\n";
 

