#!/usr/bin/perl

use strict;

#some globals

my $UDFRegistryName = "UDFRegistry.txt";
my @Variables = ();
my $UDFName;
my $UDFNumber;
my $UDFHPPName;
my $UDFVariableString;
my $UDFCategoryString = "Default";
my $UDFArgCheckSum = 0;
my $UDFBodyCheckSum = 0;
my @SortedIndices;

my %PrimitiveSizes = (double => 8, int => 4, float => 4, XYZ => 24);

Main();
exit;



sub Main {

	GetUDFNumber();

	ParseArguments();

	PrintHead();

	DumpBody();

	AddToRegistry();

	# PrintTail();
}

sub GetUDFNumber {
	$UDFName = <>;
	$UDFName =~ s/\(//;
	$UDFName =~ s/\s*//g;
	if (!(-e $UDFRegistryName)) {
		die "Registry file $UDFRegistryName doesn't exist";
	}
	my $s = `grep \" $UDFName \" $UDFRegistryName`;
	$s =~ s/^\s*//;
	my @vals = split(/\s+/, $s);
	if ($#vals > 0) {
		$UDFNumber = $vals[0];
	} else {
		$s = `sort -k 1 $UDFRegistryName | tail -1`;
		$s =~ s/^\s*//;
		@vals = split(/\s+/, $s);
		if ($#vals > 0) {
			$UDFNumber = $vals[0] + 1;
		} else {
			die "Registry looks empty: $!";
		}
	}
}


sub ParseArguments {
	my $i;
	# Assume name starts immediately at top
	# skip MDVM line (must be MDVM &aMDVM,)
	<>;

	# find all variables and build hash
	LINE: while (<>) {
		last LINE if /\)/;
		s/,//;
		s/^\s*//;
		s/\s*$//;
		s/\s+/ /g;
		my $name;
		my $type;
		($type, $name) = split(/\s/);
		push(@Variables, {Name => $name, Type => $type, Size => $PrimitiveSizes{$type}});
	}

	# created index list sorted by variable size, descending order
	my @indices = (0 .. $#Variables);
	@SortedIndices = sort { $Variables[$b]{Size} <=> $Variables[$a]{Size} } @indices;
}

sub PrintHead() {
	print "\n";
	print "enum { $UDFName\_Code = $UDFNumber };\n";
	print "\n";
	print "\n";
}

sub DumpBody {
	my $i;

	print "class $UDFName\_Parameters\n";
	print "{\n";
	print "public:\n";
	for $i (0 .. $#Variables) {
		my $i_sorted = $SortedIndices[$i];
		print "    $Variables[$i_sorted]{Type} $Variables[$i_sorted]{Name};\n";
	}
	print "};\n";
	print "\n";


	print "static\n";
	print "inline\n";
	print "void\n";
	print "$UDFName\_Execute( MDVM &aMDVM, const $UDFName\_Parameters &p )\n";
	print "{\n";

	# skip {
	<>;

	# look for char mUDFCategory[] = "Torsion:Mojo";
	# todo

	# grab and dump, swapping in p., until };
	LINE: while (<>) {
		last LINE if /};/;
		for $i (0 .. $#Variables) {
			my $var = $Variables[$i]{Name};
			s/$var/p.$var/g;
		}
		print;
	}
	print "}\n";
}

sub AddToRegistry {
	my $date = `date -u +"%Y/%m/%d"`;
	chomp $date;
	my $time = `date -u +"%T"`;
	chomp $time;
	my $i;
	$UDFVariableString = "";
	for $i (0 .. $#Variables) {
		if ($i > 0) {
			$UDFVariableString = $UDFVariableString.":";
		}
		my $i_sorted = $SortedIndices[$i];
		$UDFVariableString = $UDFVariableString.$Variables[$i_sorted]{Type}.":".$Variables[$i_sorted]{Name};
	}
	printf "%6d %-30s %10s %8s %7d %7d %-20s %s\n", $UDFNumber, $UDFName, $date, $time, $UDFArgCheckSum, $UDFBodyCheckSum, $UDFCategoryString, $UDFVariableString;
						
}
