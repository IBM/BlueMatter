#!/usr/bin/perl

$bgbindir  = $ENV{"BG_BIN_DIR"};
$sbbasedir = $ENV{"SANDBOXBASE"};
$bmdatadir = "$sbbasedir/src/mdtest/data/bluematter/";
$xml2db2   = "$bgbindir/setup/xml2db2.pl";
$xml2exe   = "$bgbindir/setup/xml2exe.py";
$mdlogger  = "$bgbindir/analysis/MDLogger";

$filelist  = shift(@ARGV);

# generate OPLSAA XML files from IMPACT raw data first
system("generate_opls_xml.pl $filelist");

# remove all exsiting mdlog files
#unlink (<*.mdlog>);

open(FILIN,"<$filelist");
while (<FILIN>) {
  chomp;
  if ($_ =~ /^\w/) {
    $prefix  = $_;
    $bluematter= "$prefix.opls.p3me";
    $excutable= "$prefix.opls.p3me.smp.aix.exe";
    $rdgfile = "$prefix.opls.p3me.aix.rdg";
    $rtpfile = "$prefix.opls.p3me.rtp";
    $logfile = "$prefix.opls.p3me.mdlog";
    $xmlfile = "$prefix.opls.xml";
    $dvsfile = "$prefix.opls.aix.dvs";
    system "$xml2exe $xmlfile $rtpfile ./$bluematter TestProduction";
    #system "$xml2exe $xmlfile $rtpfile ./$bluematter";
    system "$excutable $bmdatadir$dvsfile";
    system "mv RawDatagramOutput $rdgfile";
    system "$mdlogger $rdgfile > $logfile";
    system "rm Snap*";
  }
}

# generate summary data and do comparison here 
system("summary_p3me.pl $filelist");

# generate a pass/fail summary table for developers
$overall_pass_fail = "PASS";
$tolerance = 1.0e-3;
open(IN,"<summary.out") || die "can't open summary.out\n";
open(OUT,">pass_fail.out") || die "can't open pass_fail.out\n";

while(<IN>){
    # k-sapce energy is currently used for pass/fail
    # but I download all eenrgy terms anyway for later use
    chomp;
    split;
    $len = @_;
    if ($_[0] eq "Test") {
      $totalid    = -999;
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
        if ($_[$ii] eq "EwaldSum")     {$elecid     = $ii;}
        if ($_[$ii] eq "EwaldK")       {$ewaldkid   = $ii;}
        if ($_[$ii] eq "Coulomb14")    {$elec14id   = $ii;}
        if ($_[$ii] eq "NSQLJ")        {$vdwid      = $ii;}
        if ($_[$ii] eq "LJ14")         {$vdw14id    = $ii;}
        if ($_[$ii] eq "TIP3P")        {$waterid    = $ii;}
      }
    }
    else {
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
      $ewaldk   = $_[$ewaldkid];
      $elec14   = $_[$elec14id];
      $vdw      = $_[$vdwid];
      $vdw14    = $_[$vdw14id];
      $water    = $_[$waterid];
    }
   
    open(IN2,"<IMPACT.p3me.out") || die "can't open IMPACT.p3me.out\n";
    $_=<IN2>;
    while(<IN2>){
      split;
      $impactid = $_[0];
      $impact_ewaldk = $_[6];
      # imapct id's have subdirectory name in them
      @id = split(/\//,$impactid);  
      $impactid = $id[1];
      #printf ("Impact id= %s\n", $impactid);
      if($impactid eq $sysid){
        if($sysid eq "dipep")            { $tolerance = 1.0e-3; }
        if($sysid eq "water")            { $tolerance = 8.0e-3; }
        if($sysid eq "hairpin_water")    { $tolerance = 3.0e-3; }


        if(abs(($ewaldk - $impact_ewaldk)/$impact_ewaldk) < $tolerance){
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






