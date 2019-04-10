#!/usr/bin/perl

# UDFToDel

use strict;

#some globals

my $UDFRegistryName = "UDF_Registry.txt";
my $MaxColumnIndex = 10;
my @Variables = ();
my @OutputVariables = ();
my $InputStructName = "";
my $OutputStructName = "";
my $UDFName;
my $UDFNumber;
my $UDFVariableString;
my $UDFOutputVariableString;
my $UDFCategoryString = "Default";
my $UDFArgCheckSum = 1111;
my $UDFBodyCheckSum = 2222;
my $UDFSiteCount = 2;
my $UDFParamCount = 0;
my $NewRegistryEntry = 0;
my $ExistingRegistryEntry = "";
my %PrimitiveSizes = (double => 8, int => 4, float => 4, XYZ => 24, EMPTY => 0);

sub Main ()
{
    open(REGISTRYFILE, $UDFRegistryName) || die "cannot open $UDFRegistryName: $!";
    open(RECORDDELFILE, ">UDF_Record.del") or die "UDF_Record.del $!";
    select(RECORDDELFILE);
    
    while (<REGISTRYFILE>) {
        my @parms = split();
        if ($#parms == $MaxColumnIndex) {
	    my $DateStr = $parms[2];
	    $DateStr =~ s/\//\-/g;
	    my $TimeStr = $parms[3];
	    $TimeStr =~ s/\:/\./g;
	    print "$parms[0], \"$parms[1]\", \"$DateStr\",\"$TimeStr\",$parms[4],$parms[5],$parms[6],$parms[7]\n";
        }
    }
    close(RECORDDELFILE);
    close(REGISTRYFILE);

    open(REGISTRYFILE, $UDFRegistryName) || die "cannot open $UDFRegistryName: $!";
    open(CATDELFILE, ">UDF_Category.del") or die "UDF_Category.del $!";
    select(CATDELFILE);
    
    while (<REGISTRYFILE>) {
        my @parms = split();
        if ($#parms == $MaxColumnIndex) {
	    my $CatStr = $parms[8];
	    my @Cats = split(':',$CatStr);
	    my $cat;
	    foreach $cat (@Cats) {
	      print "$parms[0],\"$cat\"\n";
	    }
        }
    }
    close(CATDELFILE);
    close(REGISTRYFILE);

    open(REGISTRYFILE, $UDFRegistryName) || die "cannot open $UDFRegistryName: $!";
    open(VARDELFILE, ">UDF_Variable.del") or die "UDF_Variable.del $!";
    select(VARDELFILE);
    while (<REGISTRYFILE>) {
        my @parms = split();
        if ($#parms == $MaxColumnIndex) {
	    my $VarStr = $parms[9];
	    my @Vars = split(':',$VarStr);
	    my $var;
	    my $i;
	    for $i (0 .. $#Vars/2) {
	      print "$parms[0],\"$Vars[2*$i+1]\",\"$Vars[2*$i]\",$PrimitiveSizes{$Vars[2*$i]}\n";
	    }
        }
    }
    close(VARDELFILE);
    close(REGISTRYFILE);
}
  
Main();
exit();
  
