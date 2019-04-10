#!/usr/local/bin/perl5
####################################################################
### opls2xml.pl 
###          A perl script file for generating force field independent
###          parameter files for OPLS-AA from IMPACT runs. It will
###          launch an IMPACT job with the pdb file supplied first, and then
###          collect OPLS-AA parameters and structural information 
###          from output files, and finally convert them into a xml file. 
###          The IMPACT excutable and an input command file (or configuration 
###          file) oplsprm.inp is required to run opls2xml.pl.
###
### usage:   opls2xml.pl jobname.pdb
### example: opls2xml.pl enkp.pdb 
###          # this generates "enkp.prm" and "enkp.xml"
###          opls2xml.pl hiv.pdb 
###          # this generates "hiv.prm" and "hiv.xml"
###
### 
### input:   pdb file, impact input file (oplsprm.inp) and impact excutable
### output:  parameter files: jobname.prm (ASCII) and jobname.xml (XML)
###
### R. Zhou, IBM, 09/2000
####################################################################

$IMP_HOME=$ENV{"IMP_ROOT"};

if($IMP_HOME eq ""){ 
  print STDERR "\n";
  print STDERR "You don't have enviornmental variable IMP_ROOT defined\n"; 
  print STDERR "Please consult your IMPACT expert to setup IMP_ROOT\n"; 
  exit(0); 
}

### run impact job to get parameters for the pdb structure

print ("\n");
print (" RUN IMPACT ....\n");
print ("\n");

unlink<oplsprm.out*>if (-r <oplsprm.out*>);
$pdbfile = shift(@ARGV);
($system = $pdbfile) =~ s/\.pdb//;
system("cp $pdbfile myprot.pdb");
system("cp oplsprm.inp fort.1");
system("$IMP_HOME/bin/main1h > $system.log");
system("cp oplsprm.out $system.out");  
system("cp impact.tpl $system.tpl");
$outfile = "$system.out";
$prmfile = "$system.prm";
$tplfile = "$system.tpl";
$xmlfile = "$system.xml";

### get OPLS parameters from output file $system.out

print ("\n");
print (" COLLECT IMPACT DATA ....\n");
print ("\n");

open(FILE,"<$outfile") || die "Cannot open output file $outfile\n";
open(PRMOUT,">$prmfile") || die "Cannot open prm file $prmfile\n";

printf(PRMOUT "##################################################\n");
printf(PRMOUT "# Parameter file for system:  $system \n");
printf(PRMOUT "##################################################\n");

printf(PRMOUT "#-------------------------------------------------\n");
printf(PRMOUT "# Functional Forms \n");
printf(PRMOUT "#-------------------------------------------------\n");
printf(PRMOUT "charge                          1 \n");
printf(PRMOUT "vdw                             1 \n");
printf(PRMOUT "vdw_combining_rule              2 \n");
printf(PRMOUT "1,4_charge                      2 \n");
printf(PRMOUT "1,4_vdw                         2 \n");
printf(PRMOUT "1,4_charge_scaling_factor       0.5 \n");
printf(PRMOUT "1,4_vdw_scaling_factor          0.5 \n");
printf(PRMOUT "improper_torsion                2 \n");
printf(PRMOUT "torsional_input                 2 \n");
printf(PRMOUT "Urey-Bradey                     0 \n");
printf(PRMOUT "grouping_method                 0 \n");
printf(PRMOUT "water_model                     0 \n");
printf(PRMOUT "unit_scheme                     1 \n");

# get into the IMPACT/atomtype output section

printf(PRMOUT "#---------------------------------------------------------\n");
printf(PRMOUT "# site  symbol  epsilon  sigma  epsilon14  sigma14 mass charge \n");
printf(PRMOUT "#---------------------------------------------------------\n");

do {
   $_ = <FILE>;
} until(/OPLSAA FORCE FIELD TYPE/ || eof(FILE));
if(eof(FILE)) {
   print "Run $pdbfile not finished.  Will stop this job.\n";
   exit(0);
}
# skip 3 more lines in the output file
foreach $i (1 .. 3) {
   $_ = <FILE>;
}

$_ = <FILE>;
until(/-------/) {
   @fft = split;
   $mass = &get_mass($fft[0]);
   $sigma = $fft[4];
   $epsilon = $fft[5];
   # Rmin = 0.50 * (2**(1/6)) * sigma
   # $sigma = 0.50 * $sigma * (2.0**(1.0/6.0));
   $sigma14 = $sigma;
   $epsilon14 = $epsilon;
#  generate mass from atom symbol
   printf(PRMOUT "%-6s  %-6s %8.4f  %8.4f  %8.4f  %8.4f  %8.4f %8.4f\n", $fft[0],$fft[2],$epsilon, $sigma, $epsilon14, $sigma14, $mass, $fft[3]);
   $_ = <FILE>;
}

# generate bond information

printf(PRMOUT "#-------------------------------------------------\n");
printf(PRMOUT "# bonds            k           r0 \n");
printf(PRMOUT "#-------------------------------------------------\n");

do {
   $_ = <FILE>;
} until(/Stretch List:/ || eof(FILE));
if(eof(FILE)) {
   print "Run $pdbfile not finished.  Will stop this job.\n";
   exit(0);
}
@fft = split;
$nbonds = $fft[3];

# skip 3 more lines in the output file
foreach $i (1 .. 3) {
   $_ = <FILE>;
}

for($i=0; $i<$nbonds; $i++){
   $_ = <FILE>;
   @fft = split;
   printf(PRMOUT "%-6s  %-6s   %8.4f  %8.4f\n", $fft[0],$fft[1],$fft[2],$fft[3]);
}


# generate angle information

printf(PRMOUT "#-------------------------------------------------\n");
printf(PRMOUT "# angles                  k        theta0       ku      ru\n");
printf(PRMOUT "#-------------------------------------------------\n");

do {
   $_ = <FILE>;
} until(/Bending List:/ || eof(FILE));
if(eof(FILE)) {
   print "Run $pdbfile not finished.  Will stop this job.\n";
   exit(0);
}
@fft = split;
$nangles = $fft[3];

# skip 3 more lines in the output file
foreach $i (1 .. 3) {
   $_ = <FILE>;
}

for($i=0; $i<$nangles; $i++){
   $_ = <FILE>;
   @fft = split;
   # set Urey-Bradey constants to zero
   $ku = 0.0;
   $ru = 0.0;
   printf(PRMOUT "%-6s  %-6s  %-6s  %8.4f  %8.4f  %8.4f  %8.4f\n", $fft[0],$fft[1],$fft[2],$fft[3], $fft[4], $ku, $ru);
}

# generate torsion information

printf(PRMOUT "#-------------------------------------------------\n");
printf(PRMOUT "# Proper Torsions                  V0       V1       V2      V3       \n");
printf(PRMOUT "#-------------------------------------------------\n");

do {
   $_ = <FILE>;
} until(/Torsion List:/ || eof(FILE));
if(eof(FILE)) {
   print "Run $pdbfile not finished.  Will stop this job.\n";
   exit(0);
}
@fft = split;
$nproptors = $fft[5];
$nimptors = $fft[8];

# skip 3 more lines in the output file
foreach $i (1 .. 3) {
   $_ = <FILE>;
}

for($i=0; $i<$nproptors; $i++){
   $_ = <FILE>;
   @fft = split;
   printf(PRMOUT "%-6s  %-6s  %-6s  %-6s  %8.4f %8.4f %8.4f %8.4f\n", $fft[0],$fft[1],$fft[2],$fft[3], $fft[4], $fft[5], $fft[6], $fft[7]);
}

printf(PRMOUT "#-------------------------------------------------\n");
printf(PRMOUT "# Improper Torsions               V0       V1       V2       V3       \n");
printf(PRMOUT "#-------------------------------------------------\n");

for($i=0; $i<$nimptors; $i++){
   $_ = <FILE>;
   @fft = split;
   printf(PRMOUT "%-6s  %-6s  %-6s  %-6s  %8.4f %8.4f %8.4f %8.4f\n", $fft[0],$fft[1],$fft[2],$fft[3], $fft[4], $fft[5], $fft[6], $fft[7]);
}
printf(PRMOUT "#-------------------------------------------------\n");
printf(PRMOUT "# 1,4 vdw symbol1 symbol2   sigma    epsilon   \n");
printf(PRMOUT "#-------------------------------------------------\n");

printf(PRMOUT "#-------------------------------------------------\n");
printf(PRMOUT "# Explicit group definition              \n");
printf(PRMOUT "#-------------------------------------------------\n");

# get into the IMPACT template file to get nbexclusion list



### At the end print out definitions for Fuctional Forms

printf(PRMOUT "##################################################\n");
printf(PRMOUT "# Definitions of Functional Forms                 \n"); 
printf(PRMOUT "##################################################\n");
printf(PRMOUT "## charge      1: fixed charge, 2: fluctuating charge \n");
printf(PRMOUT "## vdw         1: 6-12   \n");
printf(PRMOUT "## vdw combining rule   1: CHARMm, 2: OPLSAA \n");
printf(PRMOUT "## 1,4 charge           1: excluded, 2: scaling, 3: specific parms \n");
printf(PRMOUT "## 1,4 vdw              1: excluded, 2: scaling, 3: specific parms \n");
printf(PRMOUT "## 1,4 charge scaling factor       scaling factor (0.0 - 1.0) \n");
printf(PRMOUT "## 1,4 vdw scaling factor          scaling factor (0.0 - 1.0) \n");
printf(PRMOUT "## improper torsion     1: CHARMm, 2: OPLSAA  \n");
printf(PRMOUT "## torsional input      1: CHARMm, 2: OPLSAA  \n");
printf(PRMOUT "## Urey-Bradey          0: None,   1: Yes     \n");
printf(PRMOUT "## grouping method      0: atoms, 1: residue, 2: molecule, 3: explicit \n");
printf(PRMOUT "## water model          0: none, 1: TIP3P, 2: TIP4P, 3: TIP5P \n");
printf(PRMOUT "## unit scheme          1: kcal/mol, 2: kJ/mol \n");
printf(PRMOUT "##################################################\n");

close(FILE);
close(PRMOUT);



### Writing the XML file from the ASCII prm file
### This part modified from RSG's txt2xml.pl

print ("\n");
print (" WRITING XML FILE ....\n");
print ("\n");

########################################################################
#
# <?xml version="1.0" encoding="UTF-8"?>
# <!DOCTYPE forcefield [
# <!ELEMENT header (charge | vdwcombine | charge14 | charge14scale | vdw14 | vdw14scale | improper | torsioninput | ureybradley | grouping | water | units)*>
# <!ELEMENT sitelist (siterec)+>
# <!ELEMENT bondlist (bondrec)+>
# <!ELEMENT anglelist (anglerec)+>
# <!ELEMENT torsionlist (torsionrec)+>
# <!ELEMENT improperlist (improperrec)+>
# <!ELEMENT siterec (siteid, sitetype, epsilon, sigma, epsilon14, sigma14, mass, charge)>
# <!ELEMENT units (#PCDATA)>
# <!ELEMENT support (#PCDATA)>
# <!ELEMENT siteid (#PCDATA)>
# <!ELEMENT sitetype (#PCDATA)>
# <!ELEMENT charge (#PCDATA)>
# <!ELEMENT sigma (#PCDATA)>
# <!ELEMENT epsilon (#PCDATA)>
# <!ELEMENT bondrec (siteid, siteid, k, r0>
# <!ELEMENT k (#PCDATA)>
# <!ELEMENT r0 (#PCDATA)>
# <!ELEMENT anglerec (siteid, siteid, siteid, k, th0, ku, ru)
# <!ELEMENT th0 (#PCDATA)>
# <!ELEMENT ku (#PCDATA)>
# <!ELEMENT ru (#PCDATA)>
# <!ELEMENT oplstorsionrec (siteid, siteid, siteid, siteid, Vn, n, phi0)>
# <!ELEMENT oplsimproperrec (siteid, siteid, siteid, siteid, Vn, n, phi0)>
# <!ELEMENT torparm (order, v)>
# <!ELEMENT order (#PCDATA)>
# <!ELEMENT v (#PCDATA)>
# <!ELEMENT improperrec (siteid, siteid, siteid, siteid, (torparm)+>
# ]>
#
########################################################################

$unknownMode = 0;
$headerMode = 1;
$siteMode = 2;
$bondMode = 3;
$angleMode = 4;
$torsionMode = 5;
$improperMode = 6;

@mode = ("unknown", "header", "site", "bond", "angle", "normal torsion", "improper torsion");

@listTag = ("invalid", "header", "sitelist", "bondlist", "anglelist",
	    "torsionlist", "improperlist");

%headerTag = ('charge', "charge",
	      'vdw', "vdw",
	      'vdw_combining_rule', "vdwcombine",
	      '1,4_charge', "charge14",
	      '1,4_charge_scaling_factor', "charge14scale",
	      '1,4_vdw', "vdw14",
	      '1,4_vdw_scaling_factor', "vdw14scale",
	      'improper_torsion', "improper",
	      'torsional_input', "torsioninput",
	      'Urey-Bradey', "ureybradley",
	      'grouping_method', "grouping",
	      'water_model', "water",
	      'unit_scheme', "units");

open(PRMIN,"<$prmfile") || die "Cannot open parameter file $prmfile\n";
open(XMLOUT,">$xmlfile") || die "Cannot open XML file $xmlfile\n";

print XMLOUT "<ffparams>\n";

$mode = $unknownMode;
$oldMode = $unknownMode;

while (<PRMIN>) {

    if (m/^\# Functional Forms/) {
	$oldMode = $mode;
	$mode = $headerMode;
	next;
    } elsif (m/^\# site /) {
	$oldMode = $mode;
	$mode = $siteMode;
	next;
    } elsif (m/^\# bonds /) {
	$oldMode = $mode;
	$mode = $bondMode;
	next;
    } elsif (m/^\# angles /) {
	$oldMode = $mode;
	$mode = $angleMode;
	next;
    } elsif (m/^\# Proper Torsions /) {
	$oldMode = $mode;
	$mode = $torsionMode;
	next;
    } elsif (m/^\# Improper Torsions /) {
	$oldMode = $mode;
	$mode = $improperMode;
	next;
    } elsif (m/^\#/) {
	# comment line, do nothing
	next;
    } else {
	# other line
	# print STDERR "mode = $mode[$mode]\n";
	# print STDERR $_;
    }

    if ($mode == $unknownMode) {
	next;
#	print STDERR "ILLEGAL MODE = $mode[$unknown]\n";
	exit;
    }
    if ($oldMode <=> $mode) { # time for a new list record
	if ($oldMode <=> $unknownMode) {
	    print XMLOUT"</$listTag[$oldMode]>\n";
	}
	print XMLOUT"<$listTag[int($mode)]>\n";
	$oldMode = $mode;
    }

    chop;
    split;
    if ($mode == $headerMode) {
	print XMLOUT"<$headerTag{$_[0]}>$_[1]</$headerTag{$_[0]}>\n";
    } elsif ($mode == $siteMode) {
	print XMLOUT"<siterec><siteid>$_[0]</siteid><sitetype>$_[1]</sitetype>";
	print XMLOUT"<epsilon>$_[2]</epsilon><sigma>$_[3]</sigma>";
	print XMLOUT"<epsilon14>$_[4]</epsilon14><sigma14>$_[5]</sigma14>";
	print XMLOUT"<mass>$_[6]</mass>";
	print XMLOUT"<charge>$_[7]</charge>";
        print XMLOUT"</siterec>\n";
    } elsif ($mode == $bondMode) {
	print XMLOUT"<bondrec><siteid>$_[0]</siteid><siteid>$_[1]</siteid>";
	print XMLOUT"<k>$_[2]</k><r0>$_[3]</r0></bondrec>\n";
    } elsif ($mode == $angleMode) {
	print XMLOUT"<anglerec><siteid>$_[0]</siteid><siteid>$_[1]</siteid>";
	print XMLOUT"<siteid>$_[2]</siteid><k>$_[3]</k><th0>$_[4]</th0>";
	print XMLOUT"<ku>$_[5]</ku><ru>$_[6]</ru></anglerec>\n";
    } elsif ($mode == $torsionMode) {
	if (scalar(@_) < 8) {
	    print XMLOUTSTDERR "Illegal torsion record\n";
	    exit;
	}
        for($i=0; $i<4; $i++){
          $Vn = $_[$i+4];      # V0 starts at $_[4]          
          $n = $i;
          $phi0 = 0.0;
          if($n == 2) { $phi0 = 180.0; }
          if (($n == 0) && (abs($Vn) < 1.0e-6)) {next;}
          if (($n == 1) && (abs($Vn) < 1.0e-6)) {next;}
          if (($n == 2) && (abs($Vn) < 1.0e-6)) {next;}
	  print XMLOUT"<oplstorsionrec><siteid>$_[0]</siteid><siteid>$_[1]</siteid><siteid>$_[2]</siteid><siteid>$_[3]</siteid><Vn>$Vn</Vn><n>$n</n><phi0>$phi0</phi0></oplstorsionrec>\n";
        }
#	print XMLOUT"<torsionrec>\n<siteid>$_[0]</siteid><siteid>$_[1]</siteid>";
#	print XMLOUT"<siteid>$_[2]</siteid><siteid>$_[3]</siteid>\n";
#	print XMLOUT"<torparm>";
#	print XMLOUT"<order>0</order><v>$_[4]</v>";
#	print XMLOUT"</torparm>\n";
#	print XMLOUT"<torparm>";
#	print XMLOUT"<order>1</order><v>$_[5]</v>";
#	print XMLOUT"</torparm>\n";
#	print XMLOUT"<torparm>";
#	print XMLOUT"<order>2</order><v>$_[6]</v>";
#	print XMLOUT"</torparm>\n";
#	print XMLOUT"<torparm>";
#	print XMLOUT"<order>3</order><v>$_[7]</v>";
#	print XMLOUT"</torparm>\n";
#	print XMLOUT"</torsionrec>\n";
    } elsif ($mode == $improperMode) {
	if (scalar(@_) < 8) {
	    print STDERR "Illegal improper torsion record\n";
	    exit;
	}

        for($i=0; $i<4; $i++){
          $Vn = $_[$i+4];      # V0 starts at $_[4]          
          $n = $i;
          $phi0 = 0.0;
          if($n == 2) { $phi0 = 180.0; }          
          if($Vn < -1.0e-6 || $Vn >1.0e-6){
	    print XMLOUT"<oplsimproperrec><siteid>$_[0]</siteid><siteid>$_[1]</siteid><siteid>$_[2]</siteid><siteid>$_[3]</siteid><Vn>$Vn</Vn><n>$n</n><phi0>$phi0</phi0></oplsimproperrec>\n";
          }
        }
#	print XMLOUT"<improperrec>\n<siteid>$_[0]</siteid><siteid>$_[1]</siteid>";
#	print XMLOUT"<siteid>$_[2]</siteid><siteid>$_[3]</siteid>\n";
#	print XMLOUT"<torparm>";
#	print XMLOUT"<order>0</order><v>$_[4]</v>\n";
#	print XMLOUT"<order>1</order><v>$_[5]</v>\n";
#	print XMLOUT"<order>2</order><v>$_[6]</v>\n";
#	print XMLOUT"<order>3</order><v>$_[7]</v>\n";
#	print XMLOUT"</torparm></improperrec>\n";
    } else {
	print STDERR "Illegal mode = $mode\n";
	exit;
    }

}
print XMLOUT"</$listTag[$mode]>\n";
print XMLOUT"</ffparams>\n";


close(PRMIN);
close(XMLOUT);


sub get_mass{
  my($name) = @_;
  $_ = $name;

  if(/Cl/){
    return 35.45;
  }
  elsif(/Br/){
    return 79.90;
  }
  elsif(/Na/){
    return 23.00;
  }
  elsif(/Ca/){
    return 40.08;
  }
  elsif(/Zn/){
    return 65.38;
  }
  elsif(/C/){
    return 12.01;
  }
  elsif(/H/){
    return 1.008;
  } 
  elsif(/N/){
    return 14.01;
  }
  elsif(/O/){
    return 16.00;
  } 
  elsif(/S/){
    return 32.06;
  } 
  elsif(/P/){
    return 30.97;
  } 
  elsif(/LP/){
    return 0.0;
  }
  printf(" Can't find mass for your input name %s\n", $name);
  return 0.0;
}




