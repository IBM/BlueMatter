#!/usr/bin/perl

# Jed W. Pitera
# IBM Almaden 
# 26 February 2001
# revised 8 November 2001

# crd2dvs
#
# PERL script for coverting an AMBER .crd file to a .dvs file for
# Blue Matter
#
# WARNING: dvs format is not correct, units have not been verified
# WARNING: does not verify that coord/vel reads succeed
#
# usage: crd2dvs <prmcrd> <dvsout>
#
# units:
# 
# the AMBER prmcrd file contains positions in Angstroms and velocities
# in sqrt(kcal/mol/amu), as well as (optionally) box dimensions in
# Angstroms and box angles in degrees.  
#
# the following factor is used to convert AMBER velocities to the
# Angstrom/ps units used in the *dvs file.

$vel_conversion=20.455;
$vel_conversion=1.00;

# check input, output files

if ($#ARGV != 1) {
	print STDERR "usage:  crd2dvs <prmcrd> <dvsout>\n";
        exit(1);
}

$prmcrd = $ARGV[0];
$dvsout = $ARGV[1];

if (! open(INPUT,"<$prmcrd") ) {
	print STDERR "Can't open input prmcrd file $prmcrd\n";
	exit(1);
}

if (! open(OUTPUT,">$dvsout") ) {
	print STDERR "Can't open output dvsout file $dvsout\n";
	exit(1);
}

# read first line of file

$title=<INPUT>;
print STDERR "prmcrd TITLE: $title\n";

# grab everything else as one big list

undef $/;
$_=<INPUT>;
s/-/ -/g;
@input_list=split;

$number_of_atoms=shift @input_list;
$atoms3=$number_of_atoms*3.0;
if ((($#input_list+1) == $atoms3)||
    (($#input_list+1) == ($atoms3+6))) {
	print STDERR "minimzation coordinate file\n";
# this is a minimization coordinate file; no box information
	$ifmin = 1; $time="minimization";
} else {
	$ifmin = 0;
	$time=shift @input_list;
}

print STDERR "$number_of_atoms, $time\n";
$input_len = $#input_list+1;
print STDERR " file has $input_len elements\n";

$number_of_xyz=0;

print OUTPUT "$number_of_atoms\n";
while ($number_of_xyz<$atoms3) {
	foreach $index (1 .. 3) {
		$xyz_line[$index] = shift @input_list;
		$number_of_xyz++;
	}
	printf OUTPUT "%20.10f%20.10f%20.10f\n",
		($xyz_line[1],$xyz_line[2],$xyz_line[3]);
}

# should be done with positions, on to velocities
# note velocities are multiplied by $vel_conversion to be
# in units of A/ps

$number_of_vel=0;

print OUTPUT "$number_of_atoms\n";
if ($ifmin==0) {
 while ($number_of_vel<$atoms3) {
	foreach $index (1 .. 3) {
		$vel_line[$index] = shift @input_list;
		$vel_line[$index] *= $vel_conversion;
		$number_of_vel++;
	}
	printf OUTPUT "%20.10f%20.10f%20.10f\n",
		($vel_line[1],$vel_line[2],$vel_line[3]);
 }
} else {
 while ($number_of_vel<$atoms3) {
	printf OUTPUT "%20.10f%20.10f%20.10f\n",
		0,0,0;
	$number_of_vel+=3;
 }
}
# read box information, if it's there
# note this isn't printed out yet

if ($ifmin==0) {
  print STDERR "reading box information\n";
  print STDERR "@input_list\n";
  while (@input_list) {
	push @box,(shift @input_list);
  }
}
print STDERR "box edges:  $box[0] $box[1] $box[2]\n";
print STDERR "box angles: $box[3] $box[4] $box[5]\n";

# all done, close input & output files
close INPUT;
close OUTPUT;
exit(0);

