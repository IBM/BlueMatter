#!/usr/bin/perl

#
# Jed Pitera
# IBM Almaden
# 24 April 2002
#
# script to parse an AMBER mdinfo file and produce a hash of 
# energy term/value pairs
#
# usage: ./diffambbm.pl <mdinfo> <MDLogger.out> <kvp>
#
# sample routine to parse the output of MDLogger to allow numerical
# comparison of BlueMatter and external program energies, etc.
#


# coulomb scaling factors
# I think this is correct but am not sure. . .

$amb_cou = 18.2223**2.0;

#

if ($#ARGV!=2) { 
	print "usage: ./diffambbm.pl <mdinfo> <MDLogger.out> <kvp>\n"; exit(1);
}

if (!open (INAMB,"<$ARGV[0]")) {
	print "unable to open $ARGV[0]\n"; exit(1);
}
if (! open(INFILE,"<$ARGV[1]")) {
	print " unable to open $ARGV[1]\n"; exit(1);
}
if (!open (KVP,"<$ARGV[2]")) {
	print "unable to open $ARGV[2]\n"; exit(1);
}

#
# get amber_hash first
#
$idx=0;
%amber_hash=();

while (<INAMB>) {
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
				%amber_hash=(%amber_hash, 
					$key."_".$step => $value);
			}
		}
	}
}

close (INAMB);

#
# now read the Blue Matter file
#

$linecount = 0;
%bm_hash=();

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
			%bm_hash = (%bm_hash, 
			$column_names[$idx]."_".$step => $input_line[$idx]);
		}
#
# add the "PE" key to the hash, for convenience
#
		$potential = $bm_hash{"IntraE_$step"} + $bm_hash{"InterE_$step"};
                %bm_hash = (%bm_hash,
		"PE_".$step => $potential);
	}
	$linecount++;
}

close (INFILE);

while (<KVP>) {
	@tmpline = split; $k1 = $tmpline[0]; $k2 = $tmpline[1]; 
	if ($#tmpline >= 2) {
		$thresh = $tmpline[2];
	} else {
		$thresh = 10;
	}
	$key1 = $k1."_0"; $key2 = $k2."_0";
	$diff = $amber_hash{$key1} - $bm_hash{$key2};
	if ($thresh > 0) {
		$thresh = 10**(-$thresh);
	} else {
		$thresh = (10**($thresh)) * $amber_hash{$key1};
	}
	if (abs($diff) > abs($thresh)) {
		printf "%12s %12s %16.8f %16.8f %20.10f\n",
            		$k1,$k2,$amber_hash{$key1},$bm_hash{$key2},$diff;
	}
}	

exit(0);
