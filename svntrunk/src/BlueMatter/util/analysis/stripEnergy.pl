#!/usr/agora/bin/perl

##########################################################################
# File: stripEnergy.pl
# Author: RSG
# Date: February 16, 2001
# Description: This program takes input from stdin and puts it to
#              stdout. Input should come from a LOG.* file produced by
#              the pkfxlog facility of BlueMatter.  This program will
#              extract various energies reported at each time step and
#              compute their average values and rmsd values over a
#              window specified by the first  (and only) command line
#              argument whose default value is 100.  The output file
#              contains two comment lines beginning with a # symbol.
#              The first line specifies the window size and the second
#              identifies the columns put out by the program.
# 
##########################################################################

$windowSize = 100;

if (@ARGV > 0)
{
    $windowSize = int($ARGV[0]);
}

$invStep = 1.0/$windowSize;

print STDOUT "# windowSize = $windowSize\n";
print STDOUT "# Step Avg(intraE)  RMSD(intraE) Avg(interE)  RMSD(interE) ";
print STDOUT "Avg(kineticE)  RMSD(kineticE)  Avg(totalE)  RMSD(totalE)\n";
$step = 0;
$sumIntraE = 0;
$sumIntraE2 = 0;
$sumInterE = 0;
$sumInterE2 = 0;
$sumKineticE = 0;
$sumKineticE2 = 0;
$sumTotalE = 0;
$sumTotalE2 = 0;

while(<STDIN>) {
    s/^\s+//; # strip off leading whitespace
    ($field1, $field2, $field3, $field4, $field5, $intraE, $field6, $interE, $field7, $kineticE, $field8, $field9, $field10, $totalE, $rest) = split(/\s+/, $_, 15);
    if (($field5 eq "Intra") && ($field6 eq "Inter") && ($field7 eq "Kinetic")) {
	++$step;

	$sumIntraE += $intraE;
	$sumIntraE2 += $intraE*$intraE;

	$sumInterE += $interE;
	$sumInterE2 += $interE*$interE;

	$sumKineticE += $kineticE;
	$sumKineticE2 += $kineticE*$kineticE;

	$sumTotalE += $totalE;
	$sumTotalE2 += $totalE*$totalE;

	if (($step % $windowSize) == 0) {
	    
	    print STDOUT $step, " ";

	    print STDOUT $invStep*$sumIntraE, " ";
	    print STDOUT sqrt($invStep*($sumIntraE2 - $sumIntraE*$sumIntraE*$invStep)), " ";
	    
	    print STDOUT $invStep*$sumInterE, " ";
	    print STDOUT sqrt($invStep*($sumInterE2 - $sumInterE*$sumInterE*$invStep)), " ";

	    print STDOUT $invStep*$sumKineticE, " ";
	    print STDOUT sqrt($invStep*($sumKineticE2 - $sumKineticE*$sumKineticE*$invStep)), " ";

	    print STDOUT $invStep*$sumTotalE, " ";
	    print STDOUT sqrt($invStep*($sumTotalE2 - $sumTotalE*$sumTotalE*$invStep)), "\n";

	    $sumIntraE = 0;
	    $sumIntraE2 = 0;
	    $sumInterE = 0;
	    $sumInterE2 = 0;
	    $sumKineticE = 0;
	    $sumKineticE2 = 0;
	    $sumTotalE = 0;
	    $sumTotalE2 = 0;
	}
    }
}
