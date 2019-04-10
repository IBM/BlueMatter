#!/bin/perl -w
####################################################################
### replica --Main script file for running Replica Exchange Method 
###
### usage:   replica -i input.inp -p system.pdb -r initial.rst 
###                  -n numberOfGeneration -s MDSteps 
###                  -t temperatureList -o logfile.out
###                  -c continue_replica_num
### example: replica -i hairpin.inp -p hairpin.pdb -r hairpin.rst
###                  -n 50 -s 100 -t temperature.lst -c 120
###                  
###          
### options:    -i    input template file (template.inp)
###             -p    pdb file  (system.pdb)
###             -r    restart file, initial point (initial.rst)
###             -c    continue from replica num
###             -n    number of generations to be done (100)
###             -s    MD steps in each generation (random, 100)
###             -t    temperature list (300, 310, ...,350)
###             
###
### input:   input file (*.inp), pdb file (*.pdb), restart file (*.rst)
###          temperaturel list file, 
### output:  standard impact output files, log file
###
### RZ, IBM 07/2001
####################################################################

use Getopt::Std;
$opt_i = $opt_p = $opt_r = $opt_n = $opt_t = $opt_c = $opt_z = 0;
getopts('i:p:r:n:s:t:c:oz') || die "Options wrong for replica_impact!\n";

### handling command line options
if($opt_i) { 
   # take in an input file 
   if( $opt_i ne "template.inp" && -l "template.inp" ){
      unlink "template.inp";
   }
   system("cp $opt_i template.inp"); 
}

if ($opt_p) {
   system("cp $opt_p system.pdb");
}

if ($opt_r) {
   system("cp $opt_r initial.rst");
}

if ($opt_t) {
   open(IN1,"<$opt_t") || die "cannot open temperature file $opt_t\n";
   $_ = <IN1>;
   @temps = split;
   close(IN1);
}
$nchild = $#temps+1;

# set default temperatures if no temperature list specified
if ($nchild <= 1) {
   @temps = (300, 310, 320, 330, 340, 350);
}
$nchild = $#temps+1;

$ngen = 100;
if ($opt_n) {
   $ngen = $opt_n;
}

$nsteps = 100;
if ($opt_s) {
   $nsteps = $opt_s;
}

$continue = 1;
if ($opt_c) {
   $continue = $opt_c;
}


$kb = 0.001972;
$nrepeat = 0;

if ($continue == 1) {
   for($child=1; $child <= $nchild; $child++){
      $Temp2ReplicaRecord[$child][$continue-1] = $child;
      $Replica2TempRecord[$child][$continue-1] = $temps[$child-1];
   }
}
elsif($continue >= 2) {
   open(IN1,"<replica.lst") || die "cannot open replica record replica.lst\n";
   $_ = <IN1>;
   @lastreplicas = split;
   for($child=1; $child <= $nchild; $child++){
      $Temp2ReplicaRecord[$child][$continue-1] = $lastreplicas[$child-1];
      $Replica2TempRecord[$child][$continue-1] = $temps[$lastreplicas[$child-1]];
   }
}
 

### initial setup for each temperature subdirectory
if ($continue == 1){
   foreach $temperature (@temps) {
      if (-e $temperature) {
         $tmp = `rm -rf $temperature/`;
         $tmp = `mkdir $temperature`;
      } else {
	 $tmp = `mkdir $temperature`;
      }
      $tmp = `cp initial.rst  $temperature/initial.rst`;
      $tmp = `cp wholerun.out  $temperature/wholerun.out`;
      $tmp = `cp wholerun.bmt  $temperature/wholerun.bmt`;
      $beta[$temperature] = 1.0/($temperature*$kb);
   }
   foreach $child (1 .. $nchild) {
      chdir ("./$temps[$child-1]");
      # specific to BlueMatter
      system("/farm/bluegene/BlueMatter/bin/dvs2restart initial.rst -temp $temps[$child-1] -nsteps $nsteps >last_old.rst");
      chdir("../");
   }

}
else{
   foreach $temperature (@temps) {
      # no initialization needed
      $beta[$temperature] = 1.0/($temperature*$kb);
   }
}

for($i=0; $i<=$nchild; $i++) {$accept_ratio[$i] = 0;}	

$average_waiting_cycle = 1000;
$total_waiting_cycle  = 0;


### Launch REM jobs here 
if (!$opt_z) {
   foreach $child (1 .. $nchild) {
      chdir ("./$temps[$child-1]");
      system("/usr/lpp/LoadL/full/bin/llsubmit ../REMbm.cmd > lljunk");
      chdir("../");
   }
}

LOOP: 
for ($gen=$continue; $gen <= $ngen; $gen++) {
   $nextgen = $gen +1;
   #$time = int (rand $nsteps) + 20;
   $time = $nsteps;
   $total_time += $time;
   print "\n===> Generation: $gen  timesteps: $time  Total timesteps: $total_time\n";

   $notdone = 1; $cycle = 0;
   while($notdone == 1){
      sleep 10;
      $count = 0;
      foreach $child(1 .. $nchild){
	 if( -r "./$temps[$child-1]/lock") {$count++;}
      }
      printf("Waiting cycle %d:   %d jobs finished\n", $cycle,$count);
      if($count == $nchild) {
         $notdone=0;
      }
      # error handling, in case one or two jobs are dead
      # and we don't want to wait for that job forever
      # after repeat one more times, REM moves on
      elsif (($cycle > 10*$average_waiting_cycle) && ($nrepeat >= 1)){
         # move on
         $notdone = 0;
         # should not count these repeated cycles iin new averages
         $total_waiting_cycle = $total_waiting_cycle -$cycle;
         foreach $child ($nchild .. 1){
	    if( -r "./$temps[$child-1]/lock") {
            }
            else{
               if ($child == $nchild) {
                  # if last one missing copy the very first one
                  system("cp ./$temps[0]/last_new.rst ./$temps[$child -1]/"); 
                  system("cp ./$temps[0]/last_new.out ./$temps[$child -1]/");
                  system("cp ./$temps[0]/last_new.bmt ./$temps[$child -1]/");
               }
               else{
                  system("cp ./$temps[$child]/last_new.rst ./$temps[$child -1]/"); 
                  system("cp ./$temps[$child]/last_new.out ./$temps[$child -1]/");
                  system("cp ./$temps[$child]/last_new.bmt ./$temps[$child -1]/");               
               }
            }
         }
      }
      # one annoying thing found is that some llsubmit jobs
      # are not run at all, they somehow are skiped by the loadleveler
      # so I repeat the jobs if this happens
      elsif (($cycle > 10*$average_waiting_cycle) && ($nrepeat < 1)){
         $gen--;
         $nrepeat++;
         next LOOP;
      }
      elsif (($cycle > 5*$average_waiting_cycle) && ($count > 0.9*$nchild -2)){
         # temporary skip this since rst file not generated by REMbm.cmd 06/25/02
         next;

         # jobs might have all been run, but a few of them failed, need "cure" here 
         $notdone = 0;
         # make sure all jobs are indeed "finished", but a few of them might have failed
         for ($child = $nchild; $child >= 1;  $child--){
            if (-r "./$temps[$child-1]/lock") {
            }  
            else{
               $notdone = 1;
               last;
            }
         }

         if ($notdone == 0){     
            for ($child = $nchild; $child >= 1;  $child--){
	       if( -r "./$temps[$child-1]/last_new.rst") { 
               }
               else{ 
                  printf (" Replica %d faild, and recovered from %d\n", $child, $child+1);
                  if ($child == $nchild) {
                     # if last one missing copy the very first one
                     system("cp ./$temps[0]/last_new.rst ./$temps[$child -1]/"); 
                     system("cp ./$temps[0]/last_new.out ./$temps[$child -1]/"); 
                     system("cp ./$temps[0]/last_new.bmt ./$temps[$child -1]/"); 
                  }
                  else{
                     system("cp ./$temps[$child]/last_new.rst ./$temps[$child -1]/"); 
                     system("cp ./$temps[$child]/last_new.out ./$temps[$child -1]/"); 
                     system("cp ./$temps[$child]/last_new.bmt ./$temps[$child -1]/"); 
                  }
               }
            }
         }
      }

      # no special case, keep waiting and checking
      $cycle++;
      $total_waiting_cycle++;
   }
   $average_waiting_cycle = $total_waiting_cycle/($nextgen-$continue);
   $nrepeat = 0;

   ### Job finished, collect energy
   ### This is specific to BlueMatter
   foreach $child (1 .. $nchild) {
      chdir("./$temps[$child-1]") ;
      ($e_tot[$child], $e_kin[$child], $e_pot[$child]) = get_BM_energy_terms();
      print "$child $temps[$child-1] $e_tot[$child] $e_pot[$child] $e_kin[$child]\n";
      chdir("../");
   }

   ### done MD, check for transitions

   for($child=1; $child <= $nchild; $child++){
      $Temp2ReplicaRecord[$child][$gen] = $Temp2ReplicaRecord[$child][$gen-1];
   }

   # temporary for BlueMatter since there should be a better place to handle this
   foreach $child (1 .. $nchild) {
      chdir ("./$temps[$child-1]");
      # specific to BlueMatter
      system("/farm/bluegene/BlueMatter/bin/dvs2restart SnapshotFinal.dvs -temp $temps[$child-1] -nsteps $nsteps >last_new.rst");
      chdir("../");
   }

   print "attempting $nchild-1 trial moves\n";
   for($trial = $nchild; $trial > 1; $trial--) {
      #$low_temp = $trial;
      #$high_temp = $low_temp + 1;
      $high_temp = $trial;
      $low_temp = $high_temp -1;
      $deltae = $e_pot[$high_temp] - $e_pot[$low_temp];
      $deltab = $beta[$temps[$low_temp-1]] - $beta[$temps[$high_temp-1]];
      $delta  = $deltae * $deltab;

      print "move $low_temp $high_temp DeltaE: $deltae Deltab: $deltab\n";

      ### simulations selected, check transition

      $prob = 0.0;
      $seed = rand 1.0;

      if ($delta < 0.00) {
         #print "accept automatically\n";
         $prob=1.0;
      } else {
	 # don't need to calculate exp() when too small
	 if ($delta < 50.0) {
       	    $prob = exp(-$delta);
	    #print "$prob $delta\n";
	 } else {
	    $prob = 0.0;
	 }

      }

      if ($seed <= $prob) {
	 print "Random num: $seed <= Probability: $prob ==> accepted\n";
	 $accept_ratio[$trial-1]++;
         # swap the restart files here . . .

	 system("mv ./$temps[$low_temp-1]/last_new.rst tmprst") && warn "cannot move restart file";
	 system("mv ./$temps[$high_temp-1]/last_new.rst ./$temps[$low_temp-1]/last_new.rst") && warn "cannot move restart file";
	 system("mv tmprst ./$temps[$high_temp-1]/last_new.rst") && warn "cannot move restart file";

         # swap temperatures here . . . (specific to BlueMatter)

         system("/farm/bluegene/BlueMatter/bin/dvs2restart ./$temps[$high_temp-1]/last_new.rst -temp $temps[$high_temp-1] >./$temps[$high_temp-1]/last_new.rst2");
         system("mv ./$temps[$high_temp-1]/last_new.rst2 ./$temps[$high_temp-1]/last_new.rst");

         system("/farm/bluegene/BlueMatter/bin/dvs2restart ./$temps[$low_temp-1]/last_new.rst -temp $temps[$low_temp-1] >./$temps[$low_temp-1]/last_new.rst2");
         system("mv ./$temps[$low_temp-1]/last_new.rst2 ./$temps[$low_temp-1]/last_new.rst");

         ### swap energies here . . .

	 $e_kin_tmp = $e_kin[$high_temp];
	 $e_pot_tmp = $e_pot[$high_temp];
	 $e_tot_tmp = $e_tot[$high_temp];
	 $e_kin[$high_temp] = $e_kin[$low_temp];
	 $e_pot[$high_temp] = $e_pot[$low_temp];
	 $e_tot[$high_temp] = $e_tot[$low_temp];
	 $e_kin[$low_temp] = $e_kin_tmp;
	 $e_pot[$low_temp] = $e_pot_tmp;
	 $e_tot[$low_temp] = $e_tot_tmp;

                        
         $replica_record_tmp = $Temp2ReplicaRecord[$high_temp][$gen];
         $Temp2ReplicaRecord[$high_temp][$gen] = $Temp2ReplicaRecord[$low_temp][$gen];
         $Temp2ReplicaRecord[$low_temp][$gen] = $replica_record_tmp;
      } else {
	 print "Random num: $seed >  Probability: $prob ==> rejected\n";
      }
   }

   ### this is done in the REMimpact.rem for each replica

   # unlock all the jobs
   foreach $child (1 .. $nchild) {
      #system("more ./$temps[$child-1]/lock");
      system("rm -f ./$temps[$child-1]/lock");
   }

   # fill up Replica2TempRecord[] 
   for($i=1; $i<= $nchild; $i++){
      for($j=1; $j<= $nchild; $j++){ 
         if ($i == $Temp2ReplicaRecord[$j][$gen]){
            $Replica2TempRecord[$i][$gen] = $j;
            last;
         }
      }
   } 

   if($gen % 100 == 0) {
       ### report the acceptance ratio statistics
       printf("\Acceptance Ratio Report:\n");
       for($i=1; $i< $nchild; $i++){
	  printf("\Acceptance ratio for %d : %7.3f\n",$i, $accept_ratio[$i]/($gen-$continue));
       }

       ### report the temperature/replica record
       printf("\nEach Temperature's Replica Record: \n");
       for ($i=1; $i<=$nchild; $i++){
	  printf("For Temperature %d: %f \n", $i, $temps[$i-1]);
	  for($j=$continue;$j<$gen; $j++){
	     printf("%d  %d \n",$j, $Temp2ReplicaRecord[$i][$j]);
	  }
       }
       printf("\nEach Replica's Temperature Record: \n");
       for ($i=1; $i<= $nchild; $i++){
	  printf("For Replica %d: \n", $i);
	  for($j=$continue;$j<$gen; $j++){
	     $tmp = $Replica2TempRecord[$i][$j];
	     printf("%d  %d  %f \n",$j, $Replica2TempRecord[$i][$j], $temps[$tmp-1]);
	  }
       }
    }
    ### save the last replica record
    open(OUT1,">replica.lst") || die "Can't open replica record file replica.lst";
    for ($i=1; $i<=$nchild; $i++){
       printf( OUT1 "%d ",$Temp2ReplicaRecord[$i][$gen-1]);
    }
    printf( OUT1 "\n");
    close(OUT1);
}


sub find_best_class{
  my ($sp2class);
  $sp2class = 1;
  system("llclass > classfile");

  $_ = `grep 'DCI_PROTEIN_1' classfile`;
  @f=split; $dci1 = $f[3];

  $_ = `grep 'DCI_PROTEIN ' classfile`;
  @f=split; $dci = $f[3];

  $_ = `grep 'GENERAL' classfile`;
  @f=split; $general = $f[3];
  
  # print "dci1, dci, general: $dci1,$dci,$general \n";
  if ($dci1 > 1) {
    # DCI_PROTEIN_1 first
    $sp2class = 1;
  }
  elsif ($dci > 1) {
    # DCI_PROTEIN second
    $sp2class = 2;
  }
  elsif ($general > 1) {
    # GENERAL last
    $sp2class = 3;
  }
  else{
    # wait on DCI_PROTEIN_1
    $sp2class = 1;
  }
  # print "sp2class: $sp2class \n";
  return $sp2class;
}



sub find_available_nodes{

  my ($dci1, $dci, $general);
  system("llclass > classfile");

  $_ = `grep 'DCI_PROTEIN_1' classfile`;
  @f=split; $dci1 = $f[3];

  $_ = `grep 'DCI_PROTEIN ' classfile`;
  @f=split; $dci = $f[3];

  $_ = `grep 'GENERAL' classfile`;
  @f=split; $general = $f[3];
  
  return ($dci1,$dci,$general);
}

### This is specific to BlueMatter
sub get_BM_energy_terms{

   $etot = $epot = $ekin = 0.0;

   open(IN,"<last_new.out") || warn "can't open file last_new.out";    
   while(<IN>){
    chomp;
    @fft = split;
    $len = $#fft+1;
    if ($fft[0] eq "Step") {
      $totalid     = -999;
      $ekinid      = -999;
      $intraid     = -999;
      $interid     = -999;
      for ($ii = 1; $ii < $len; ++$ii) {
        if ($fft[$ii] eq "TotalE")       {$totalid     = $ii;}
        if ($fft[$ii] eq "IntraE")       {$intraid     = $ii;}
        if ($fft[$ii] eq "InterE")       {$interid     = $ii;}
        if ($fft[$ii] eq "KE")           {$ekinid      = $ii;}
      }
      # print $totalid, $intraid, $interid, $ekinid;
    }
    elsif ($fft[0] >= 1) {
      # keep updating the energy terms
      $etot     = $fft[$totalid];
      $eintra   = $fft[$intraid];
      $einter   = $fft[$interid];
      $ekin     = $fft[$ekinid];
      # print $epot, $eintra, $einter, $ekin;
    }
  }
  $epot = $eintra + $einter;
  
  return ($etot,$ekin,$epot);
}

sub get_AMBER_energy_terms{

   $etot = $epot = $ekin = 0.0;

   open(IN,"<last_new.out") || warn "can't open file last_new.out";    
   while(<IN>){
      # amber's energy needs to be taken right after NSTEP during MD
      # it also reports average and fluctuations with the same var name
      if (/A V E R A G E S/) {last;}

      # keep updating till last step
      if(/NSTEP = /){
         $_ = <IN>;
         @fft = split;
         $etot = $fft[2];
         $ekin = $fft[5];
         $epot = $fft[8];
     }
  }
  return ($etot,$ekin,$epot);
}

sub get_IMPACT_energy_terms{

   $etot = $epot = $ekin = 0.0;

   # output file last_new.out
   if(!(-r "last_new.out")) { warn "can't open file last_new.out"; }   
   $_ = `grep ' Total Energy of the system......' last_new.out`;
   @fft = split; $etot = $fft[5];
   $_ = `grep ' Total Potential Energy..........' last_new.out`;
   @fft = split; $epot = $fft[3];
   $_ = `grep ' Total Kinetic Energy............' last_new.out`;
   @fft = split; $ekin = $fft[3];

  return ($etot,$ekin,$epot);
}















