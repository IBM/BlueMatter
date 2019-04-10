#!/usr/bin/perl

#
# Jed Pitera
# IBM Almaden
# 24 April 2002
#
# script to parse an AMBER or BM mdinfo file and produce a summary list of 
# energy term/value pairs
#
# usage: ./sumambbm.pl <mdinfo> <MDLogger.out> <kvp>
#
# sample routine to parse the output of MDLogger to allow numerical
# comparison of BlueMatter and external program energies, etc.
#
#

if (($#ARGV!=1)&&($#ARGV<2)) { 
	print "usage: ./sumambbm.pl <mdinfo> <kvp> <AMBER|BM> <sysname>\n"; 
	print "usage: ./sumambbm.pl HEADER <kvp>\n"; 
	exit(1);
}
#
#
if ($#ARGV==1) {
#
# we just need to print out a header
#
	$type = $ARGV[0];
} else {
#
# we have an AMBER or BM output file
#
	$type=$ARGV[2];
	if (! open(INFILE,"<$ARGV[0]")) {
		print " unable to open $ARGV[0]\n"; exit(1);
	}
	if ($#ARGV==3) {
		$sysname = $ARGV[3];
	} else {
		$sysname = "";
	}
}
#
#
if (!open (KVP,"<$ARGV[1]")) {
	print "unable to open $ARGV[1]\n"; exit(1);
}
#
# read key-value-pair file to get key correspondence between AMBER/BM udfs
#
@column_order=();
while (<KVP>) {
	@tmpline = split; $k1 = $tmpline[0]; $k2 = $tmpline[1]; 
	$key1 = $k1."_0"; $key2 = $k2."_0";
	$udf_name{$key1} = $key2;
	if ($#tmpline >= 2) {
		$thresh{$key1} = $tmpline[2];
	} else {
		$thresh{$key1} = 10;
	}
	push @column_order,$k2; 
}
close KVP;
#
# now read data files to get the energy_hash.  Keys are the BM udf names
#	
%energy_hash=();

if ($type eq "AMBER") {
#
# get amber_hash first
#
$idx=0;

while (<INFILE>) {
#
# rearrange the line to get rid of some excess whitespace in the 1-4 keys,
# remove units in parentheses from time, temp
#
	s/1-4 /1-4/g;
	s/\(K\)//g; s/\(PS\)//g;
	@tmplist = split;
	if (($#tmplist > 0) && ($tmplist[0] ne "SUM")) {
		while (@tmplist) {
			$key = shift @tmplist; shift @tmplist;
			$value = shift @tmplist;
			if ($key eq "NSTEP") {
				$step = $value;
			} else {
				%energy_hash=(%energy_hash, 
					$udf_name{$key."_".$step} => $value);
			}
		}
	}
}

close (INFILE);

} elsif ($type eq "BM") {
#
# read the Blue Matter file
#

$linecount = 0;

while (<INFILE>) {
	chomp $_;
	@input_line = split; 
	if ($linecount == 0) {
#
# first line lists the energy terms, these are used as root tags for the hashes
#
		@column_names = @input_line;
	} else {
#
# store the value in each column in a hash, using the key of 
# columnname_step; this lets us retrieve values for any step
#
# NOTE: this will get horrendous for output files w/ a large # of 
# timesteps, so only use it for regression OK?
#
		$step = $input_line[0];
		foreach $idx (1 .. $#input_line) {
#
# we skip column zero since that is the step # anyway
#
			%energy_hash = (%energy_hash, 
			$column_names[$idx]."_".$step => $input_line[$idx]);
		}
#
# add the "PE" key to the hash, for convenience
#
		$potential = $energy_hash{"IntraE_$step"} + $energy_hash{"InterE_$step"};
                %energy_hash = (%energy_hash,
		"PE_".$step => $potential);
	}
	$linecount++;
}

close (INFILE);
}

#
# done reading the appropriate data file, output stuff
#
# for HEADER, just output the BM term types
#
if ($type eq "HEADER") {
	unshift @column_order,"SYSTEM";
	foreach $key (@column_order) {
		printf "%13s ", $key;
	}
 	print "\n";
} else {
	printf "%13s ", $sysname;
foreach $key (@column_order) { 
	$k1 = $key."_0";
	if ($energy_hash{$k1} >= 0) {
	printf " %12.6e ", $energy_hash{$k1};
	} else {
	printf "%12.6e ", $energy_hash{$k1};
	}
}
print "\n";
}

exit(0);
