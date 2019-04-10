#!/usr/bin/perl

$DumpSchema = "java com.ibm.bluematter.utils.DumpSchema";
$DropSchema = "java com.ibm.bluematter.utils.DropSchema";

system "($DumpSchema -all >tmp)";
open(FILIN,"<tmp");
while (<FILIN>) {
  if($_ =~ /SYS/ || $_ =~ /NULL/) {
  } else {
    chomp;
    system "($DropSchema -schemaname $_)";
  }
}
system "(rm tmp)";
