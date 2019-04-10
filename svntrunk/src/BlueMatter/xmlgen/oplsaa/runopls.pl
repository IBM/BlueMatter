#!/usr/bin/perl

$installbase=`installbase.py`;
chop $installbase;
print "installbase = $installbase\n";
$bgbindir = "$installbase/usr/opt/bluegene/bin";
print "bgbindir = $bgbindir\n";
$bgdriverdir = "$installbase/usr/opt/bluegene/mdtest/drivers/external/oplsaa/brooks";
#$bgbindir  = $ENV{"BG_BIN_DIR"};
#$sbbasedir = $ENV{"SANDBOXBASE"};
#$bmdatadir = "$sbbasedir/src/mdtest/data/bluematter/";
$bmdatadir = "$installbase/usr/opt/bluegene/mdtest/data/bluematter/";
$xml2db2   = "$bgbindir/setup/xml2db2.pl";
$xml2exe   = "$bgbindir/setup/xml2exe.py";
$mdlogger  = "$bgbindir/analysis/MDLogger";

$filelist  = shift(@ARGV);
$buildMode = shift(@ARGV);

# generate OPLSAA XML files from IMPACT raw data first
system("$bgbindir/setup/generate_opls_xml.pl $filelist");

# remove all exsiting mdlog files
unlink (<*.mdlog>);

open(FILIN,"<$filelist");
while (<FILIN>) {
  chomp;
  if ($_ =~ /^\w/) {
    $prefix  = $_;
    $bluematter= "$prefix.opls";
    $excutable= "$prefix.opls.smp.aix.exe";
    $xmlfile = "$prefix.opls.xml";
    $rdgfile = "$prefix.opls.aix.rdg";
    $rtpfile = "./default.rtp";
    $logfile = "$prefix.opls.mdlog";
    $dvsfile = "$prefix.opls.aix.dvs";
    system "$xml2exe $xmlfile $rtpfile ./$bluematter $buildMode";
    #system "$xml2exe $xmlfile $rtpfile ./$bluematter";
    system "$excutable $bmdatadir$dvsfile";
    system "mv RawDatagramOutput $rdgfile";
    system "$mdlogger $rdgfile > $logfile";
    system "rm Snap*";
  }
}

# generate summary data and do comparison here 
system("$bgdriverdir/summary_opls.pl $filelist");

# generate a pass/fail summary table for developers
$overall_pass_fail = "PASS";
$tolerance = 1.0e-6;
open(IN,"<summary.out") || die "can't open summary.out\n";
open(OUT,">pass_fail.out") || die "can't open pass_fail.out\n";

while(<IN>){
    # total energy is currently used for pass/fail
    # but I download all eenrgy terms anyway for later use
    chomp;
    split;
    $len = @_;
    if ($_[0] eq "Test") {
      $bondid     = -999;
      $angleid    = -999;
      $torsionid  = -999;
      $ureybid    = -999;
      $improperid = -999;
      $elecid     = -999;
      $vdwid      = -999;
      $waterid    = -999;
      for ($ii = 1; $ii <= $len; ++$ii) {
        if ($_[$ii] eq "TotalE")       {$totalid    = $ii;}
        if ($_[$ii] eq "StdHarmBond")  {$bondid     = $ii;}
        if ($_[$ii] eq "StdHarmAngle") {$angleid    = $ii;}
        if ($_[$ii] eq "OPLSTorsion")  {$torsionid  = $ii;}
        if ($_[$ii] eq "OPLSImproper") {$improperid = $ii;}
        if ($_[$ii] eq "NSQCoulomb")   {$elecid     = $ii;}
        if ($_[$ii] eq "Coulomb14")    {$elec14id   = $ii;}
        if ($_[$ii] eq "NSQLJ")        {$vdwid      = $ii;}
        if ($_[$ii] eq "LJ14")         {$vdw14id    = $ii;}
        if ($_[$ii] eq "TIP3P")        {$waterid    = $ii;}
      }
    }
    else {
      # get Bluematter energy terms
      $sysid = $_[0];
      @id = split(/\./,$sysid);  
      $sysid = $id[0];
      $total    = $_[$totalid];
      $bond     = $_[$bondid];
      $angle    = $_[$angleid];
      $ureyb    = $_[$ureybid];
      $torsion  = $_[$torsionid];
      $improper = $_[$improperid];
      $elec     = $_[$elecid];
      $elec14   = $_[$elec14id];
      $vdw      = $_[$vdwid];
      $vdw14    = $_[$vdw14id];
      $water    = $_[$waterid];
    }
    open(IN2,"<IMPACT.energy.out") || die "can't open IMPACT.energy.out\n";
    $_=<IN2>;
    while(<IN2>){
      split;
      $impactid = $_[0];
      $impact_totalE = $_[1];
      # imapct id's have subdirectory name in them
      @id = split(/\//,$impactid);  
      $impactid = $id[1];
      #printf ("Impact id= %s sysid = %s \n", $impactid, $sysid);
      if($impactid eq $sysid){
        if($sysid eq "ala") { $tolerance = 1.0e-5; }
        if($sysid eq "arg") { $tolerance = 1.0e-5; }
        if($sysid eq "asn") { $tolerance = 1.0e-5; }
        if($sysid eq "asp") { $tolerance = 1.0e-5; }
        if($sysid eq "cys") { $tolerance = 1.0e-6; }
        if($sysid eq "gln") { $tolerance = 1.0e-6; }
        if($sysid eq "glu") { $tolerance = 1.0e-6; }
        if($sysid eq "gly") { $tolerance = 1.0e-6; }
        if($sysid eq "hsd") { $tolerance = 1.0e-6; }
        if($sysid eq "hse") { $tolerance = 1.0e-6; }
        if($sysid eq "hsp") { $tolerance = 1.0e-6; }
        if($sysid eq "ile") { $tolerance = 1.0e-6; }
        if($sysid eq "leu") { $tolerance = 1.0e-6; }
        if($sysid eq "lys") { $tolerance = 1.0e-6; }
        if($sysid eq "met") { $tolerance = 1.0e-6; }
        if($sysid eq "phe") { $tolerance = 1.0e-6; }
        if($sysid eq "pro") { $tolerance = 1.0e-6; }
        if($sysid eq "ser") { $tolerance = 1.0e-6; }
        if($sysid eq "thr") { $tolerance = 1.0e-6; }
        if($sysid eq "trp") { $tolerance = 1.0e-6; }
        if($sysid eq "tyr") { $tolerance = 1.0e-6; }
        if($sysid eq "val") { $tolerance = 1.0e-6; }
        if($sysid eq "AcFGKVAm.ext")     { $tolerance = 1.0e-6; }
        if($sysid eq "AcFGKVAm.min")     { $tolerance = 1.0e-6; }
        if($sysid eq "AcGLIMAm.ext")     { $tolerance = 1.0e-6; }
        if($sysid eq "AcGLIMAm.min")     { $tolerance = 1.0e-6; }
        if($sysid eq "GLIMP.ext")        { $tolerance = 1.0e-6; }
        if($sysid eq "GLIMP.min")        { $tolerance = 1.0e-6; }
        if($sysid eq "NHADR.ext")        { $tolerance = 1.0e-6; }
        if($sysid eq "NHADR.min")        { $tolerance = 1.0e-6; }
        if($sysid eq "PGPGT.ext")        { $tolerance = 1.0e-6; }
        if($sysid eq "PGPGT.min")        { $tolerance = 1.0e-6; }
        if($sysid eq "PWSCY.ext")        { $tolerance = 1.0e-6; }
        if($sysid eq "PWSCY.min")        { $tolerance = 1.0e-6; }
        if($sysid eq "QPEHdH+.ext")      { $tolerance = 1.0e-6; }
        if($sysid eq "QPEHdH+.min")      { $tolerance = 1.0e-6; }
        if($sysid eq "1hij")             { $tolerance = 1.0e-6; }
        if($sysid eq "1gpr")             { $tolerance = 1.0e-6; }
        if($sysid eq "dipep")            { $tolerance = 1.0e-6; }
        if($sysid eq "enkp")             { $tolerance = 1.0e-6; }
        if($sysid eq "hairpin")          { $tolerance = 1.0e-6; }
        if($sysid eq "methanol")         { $tolerance = 1.0e-5; }
        if($sysid eq "water")            { $tolerance = 1.0e-5; }
        if($sysid eq "weik")             { $tolerance = 1.0e-6; }
        if($sysid eq "2gb1")             { $tolerance = 1.0e-6; }
        if($sysid eq "hiv")              { $tolerance = 1.0e-6; }
        if($sysid eq "1am6_mod")         { $tolerance = 1.0e-6; }
        if($sysid eq "hairpin_water")    { $tolerance = 1.0e-6; }


        if(abs(($total - $impact_totalE)/$impact_totalE) < $tolerance){
           printf(OUT "%20s     PASS\n", $sysid);  
        }
        else{
           printf(OUT "%20s     FAIL\n", $sysid);  
           $overall_pass_fail = "FAIL";
        }
      }
    }
    close(IN2);     
}
close(OUT);

$rc = 0;
printf("\n\n\n=============================================\n");
printf("\n=============================================\n");
printf("  %s  \n", $overall_pass_fail);
if($overall_pass_fail eq "FAIL"){
  printf(" Please check file pass_fail.out for a summary\n");
  $rc = 1;
}
printf("\=============================================\n");
printf("\=============================================\n\n\n");
  

exit($rc);










