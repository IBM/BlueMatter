#!/usr/agora/bin/perl

use File::Basename;

$xmlFileName = $ARGV[0];
$databaseName = $ARGV[1];
if($databaseName eq "") {
  $databaseName = "mdsetup";
}


$systemName = basename($xmlFileName, ".xml");
print "Processing: $systemName\n";

$usrName = `whoami`;
chop($usrName);
$time = `date +"%d%T"`;
chop($time);
$directoryName = "/tmp/${systemName}_${usrName}_${$}_$time";


print $directoryName ."--\n"; #exit;
system("mkdir $directoryName");
system("chmod a+rx $directoryName");

$bg_bin_dir = "/usr/opt/bluegene/bin/";


$temp = $ENV{BG_BIN_DIR};

if ($temp) {
  $bg_bin_dir = "$temp\/setup";
}

print "$bg_bin_dir\n";
system("Xalan -v -o $directoryName/$systemName.out $xmlFileName $bg_bin_dir/xml2sdel.xsl");

# @args is everything but the first argument
($dummy, @args) = @ARGV;
$args = join(' ', @args);
#system("java com.ibm.bluematter.xml2db2.ProbspecDB2Setup $xmlFileName -dir $directoryName $args");
print "dirname: $directoryName\n";
print "$args\n";
$_ = `java com.ibm.bluematter.xml2db2.ProbspecDB2Setup  $xmlFileName -dir $directoryName -creator $usrName -database $databaseName $args`;

print "output: $_\n";
s/\n/ /g;

/ SYSTEM_ID = ([^ ]*)/;
$system_id = $1;

/ SYSTEM_DESC = ([^ ]*)/;
$sysDesc = $1;

print "system_id: $system_id\n";

system("perl $bg_bin_dir/splitoutput.pl $directoryName/$systemName.out $system_id $directoryName");

system("mv import.db2 $directoryName");

# -s option causes processing to stop on errors
$rcImport = int(system("db2 -rdb2_load_report.txt -tsvf $directoryName/import.db2"));

print STDERR "Return code from import operation = $rcImport\n";

print "SystemId: $system_id\n\n";

print "Checking import success... \n";

$_ = `java com.ibm.bluematter.xml2db2.ImportSuccessCheck -dir $directoryName -sysid $system_id -database $databaseName`;

print $_;

if ($rcImport == 0) {
    system("rm -rf $directoryName");
}

exit(int($rcImport));

 
