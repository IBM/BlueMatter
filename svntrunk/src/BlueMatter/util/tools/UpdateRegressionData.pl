#!/usr/bin/perl

# ####################################################################
#
# This routine updates the regression data in mdtest from the results
# of an automated (MultiRun.rtp) test run.
# This script must be executed in an ODE sandbox (after a workon) and
# this sandbox should be the one from which the tests were run.
# It takes a list of (absolute paths to) test directories from STDIN
# that will serve as the source of the updated regression data.
#
# ####################################################################

if ( !defined( $ENV{'SANDBOXBASE'} ) || !defined( $ENV{'PACKAGEBASE'} ) )
{
    print "Not in ode sandbox";
    exit(-1);
}

$mdtestsrc=$ENV{'SANDBOXBASE'} . '/src';
$mdtestinst=$ENV{'PACKAGEBASE'} . "/shipdata/usr/opt/bluegene";

while( <> ) {
    chop;
    $testDir=$_;
    $rFile=$testDir . "/Run.ksh";
    open RFILE, $rFile or die "Can't open $rFile\n";
    while( <RFILE> ) {
	$source = $testDir . "/Run.rdg";
	if ( m/^\s*LaunchRun\.bgl.*\s+\-rdgdiff\s+\/.*\/usr\/opt\/bluegene\/([\w-\.\/]+.rdg)\s+/ ) {
	    $target = $1;
	    # print "target: $target\n";
	    if ( -e $source ) {
		$copyCmd = "/usr/bin/cp $source $mdtestsrc/$target";
		print "$copyCmd\n";
		system( $copyCmd );
	    } else {
		print "$source does not exist, leaving $target in place\n";
	    }
		
	    break;
	}
    }
}
