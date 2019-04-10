#!/usr/agora/bin/perl

# This script will create (if necessary) a series of subdirectories of
# the working directory, create rtp files with appropriate values for time
# step size and total number of time steps from a prototype, and run
# simulations with each rtp file in the appropriate directory.
#
# command line arguments:
# working directory
# executable (full path)
# msd file (full path)
# dvs file (full path)
# prototype rtp file
# analysis script 
# length of simulations in picoseconds
# n arguments, representing the time-step sizes to use for the tests.

if (@ARGV < 8) {
    print STDERR "rmsde_vs_ts.pl workingDirectory executableFile msdFile dvsFile rtpFileProto analysisFile simTimeInPicoSeconds timeStep1InPS ... timeStepNInPS\n";
    exit(-1);
}
$workingDir = $ARGV[0];
$exeFile = $ARGV[1];
$msdFile = $ARGV[2];
$dvsFile = $ARGV[3];
$rtpProto = $ARGV[4];
$analysisFile = $ARGV[5];
$simTimePS = $ARGV[6];
@timeStep = @ARGV[7 .. $#ARGV];

foreach (@ARGV) {
    print STDOUT $_, " ";
}
print STDOUT "\n";
print STDOUT "arg count: ", scalar(@ARGV), "\n";

foreach (@timeStep) {
    print STDOUT $_, " ";
}

print STDOUT "\n";

unless(open(DATAFILE, ">$workingDir/test.dat")) {
    print STDERR "ERROR OPENING ", "$workingDir/test.dat FOR OUTPUT\n";
    exit(-1);
}

print DATAFILE "# TimeStepSize (fs) StepCount EndOfWindow Avg(intraE)  RMSD(intraE) Avg(interE)  RMSD(interE) Avg(kineticE)  RMSD(kineticE)  Avg(totalE)  RMSD(totalE)\n";

foreach $ts (@timeStep) {
    print STDOUT "tstep = $ts\n";
    $localdir = $workingDir . "/tstep_" . $ts;
    push(@dirList, $localdir);
    if (!-d $localdir) {
	unless ( mkdir($localdir, 0777)) {
	    print STDERR "ERROR CREATING DIRECTORY: ", $localdir, "\n";
	    exit(-1);
	}
    }

    unless (open(PROTOHANDLE, $rtpProto)) { # open prototype rtp file for reading
	print STDERR "ERROR OPENING FILE ", $rtpProto, " FOR READING\n";
	exit(-1);
    }
    
    $rtpFile = ">" . $localdir . "/test.rtp";
    unless (open(RTPHANDLE, $rtpFile)) {
	print STDERR "ERROR OPENING FILE ", $rtpFile, " FOR WRITING\n";
    }

    $stepCount = int($simTimePS/$ts);
    push(@stepList, $stepCount);

    while(<PROTOHANDLE>) {
	s/^(NumberOf\w+TimeSteps).*/NumberOfOuterTimeSteps $stepCount/;
	s/^\w+TimeStepInPicoSeconds.*/OuterTimeStepInPicoSeconds $ts/;
	print RTPHANDLE $_;
    }

    close(RTPHANDLE);
    close(PROTHANDLE);
}

print STDOUT "Directory List: ";
foreach(@dirList) {
    print STDOUT $_, " ";
}
print STDOUT "\n";

for ($i = 0; $i < @dirList; $i++) {
    $currentDir = $dirList[$i];
    unless (chdir($currentDir)) {
	print STDERR "ERROR CHANGING DIRECTORY TO ", $currentDir,
	"\n";
    }
    print STDOUT "running in $currentDir\n";
    system("$exeFile $msdFile $dvsFile $currentDir/test.rtp > $currentDir/test.log 2>&1");
    unless(open(ANALYSISHANDLE, "$analysisFile $stepList[$i] < $currentDir/LOG.0 |")) {
	print STDERR "ERROR RUNNING ANALYSIS SCRIPT ",
	$analysisFile, " ON ", "$currentDir/LOG.0\n";
	exit(-1);
    }

    print DATAFILE $timeStep[$i], " ";

    while(<ANALYSISHANDLE>) {
	if (m/^# windowSize = (\d+)/) {
	    $stepCount = $1;
	    print DATAFILE $stepCount, " ";
	    next;
	}
	if (!m/^#/) {
	    print DATAFILE $_;
	    last;
	}
    }
    close(ANALYSISHANDLE);
}
close(DATAFILE);
