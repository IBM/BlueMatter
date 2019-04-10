#!/usr/bin/perl

# Jed W. Pitera
# IBM Almaden 
# 26 February 2001

# revised JWP 
# 2 June 2001 
# use new XML format

# revised JWP
# 5 September 2001
# 8 November 2001
# polish new XML format

# revised JWP
# 21 March 2002
# passes single-point amino acid regression

# revised JWP
# 20 April 2002
# added modifications for new XML format (AGAIN!)

# revised JWP
# 20 May 2004
# modified to read AMBER8 topology files rather than AMBER6
 
# top2xml
#
# PERL script for coverting an AMBER .top file to a .xml file for
# xml2hpp that can then be used in Blue Matter
#
# usage: top2xml <prmtop> <xmlout> (TIP3P,etc.)
#
# WARNING: waters don't work yet, no energy tests done. . .

# declare subroutine(s)
 
sub get_atom;
sub get_atomic_number;
sub read_block;
sub print_header_spiel;
sub print_head;
sub print_headc;
sub print_data;
sub print_site;
sub print_fps;
sub print_tuple;
sub print_param;
sub print_bond;
sub print_angle;
sub print_torsion;
sub print_improper;
sub print_fsite;
sub print_fparm;
# check inputs

if (($#ARGV != 1)&&($#ARGV != 2)) {
	print STDERR "usage:  top2xml <prmtop> <xmlout>\n";
	print STDERR " or     top2xml <prmtop> <xmlout> [TIP3P,SPC,etc.]\n";
        exit(1);
}

$prmtop = $ARGV[0];
$xmlout = $ARGV[1];
if ($#ARGV > 1) {
	$watertype = $ARGV[2];
} else {
	$watertype = "NULL";
}

if (! open(INPUT,"<$prmtop") ) {
	print STDERR "Can't open input prmtop file $prmtop\n";
	exit(1);
}

if (! open(OUTPUT,">$xmlout") ) {
	print STDERR "Can't open output xmlout file $xmlout\n";
	exit(1);
}

# check if watertype is on the supported list
#@waterlist = ("NONE");
@waterlist = ("NONE", "NULL", "NULL_TEST", "TIP3P_flex", "TIP3P_rigid");

$index=0;
while (($watertype ne $waterlist[$index])&&($index<=$#waterlist)) { 
	$index++ }
if ($index == ($#waterlist+1)) {
	print STDERR "Can't find water type $watertype! Exiting.\n";
	print STDERR "Supported types are: \n";
	print STDERR "@waterlist\n";
	exit(1);
} else {
	print STDERR "Watertype $watertype is supported, proceeding.\n";
}
if ($watertype eq "NULL_TEST") {
	$fullchgprec = 1;
} else {
	$fullchgprec = 0;
}

# read first line of file

$title=<INPUT>;
print STDOUT "prmtop TITLE: $title\n";

#
# new i/o approach; read things based on the supplied formats
# note most of the rest of the code is based on 1-indexed rather
# than 0-indexed arrays; we add on to these lists rather than
# writing over them
#
@POINTERS=();
@IGRAPH=(0);
@CHRG=(0);
@AMASS=(0);
@IAC=(0);
@NUMEX=(0);
@ICO=(0);
@LABRES=(0);
@IPRES=(0);
@RK=(0);
@REQ=(0);
@TK=(0);
@TEQ=(0);
@PK=(0);
@PN=(0);
@PHASE=(0);
@SOLTY=(0);
@CN1=(0);
@CN2=(0);
@BONDH=(0);
@BOND=(0);
@ANGLEH=(0);
@ANGLE=(0);
@THETAH=(0);
@THETA=(0);
@NATEX=(0);
@ASOL=(0);
@BSOL=(0);
@HBCUT=(0);
@ISYMBL=(0);
@ITREE=(0);
@JOIN=(0);
@IROTAT=(0);
@SOLVENT_POINTERS=();
@NSP=(0);
@BOX=(0);
@GB_RADII=(0);
@GB_SCREEN=(0);
#
# need to establish a mapping between arrays and flags
#
%array_by_flag=();
$array_by_flag{POINTERS}=\@POINTERS;
$array_by_flag{ATOM_NAME}=\@IGRAPH;
$array_by_flag{CHARGE}=\@CHRG;
$array_by_flag{MASS}=\@AMASS;
$array_by_flag{ATOM_TYPE_INDEX}=\@IAC;
$array_by_flag{NUMBER_EXCLUDED_ATOMS}=\@NUMEX;
$array_by_flag{NONBONDED_PARM_INDEX}=\@ICO;
$array_by_flag{RESIDUE_LABEL}=\@LABRES;
$array_by_flag{RESIDUE_POINTER}=\@IPRES;
$array_by_flag{BOND_FORCE_CONSTANT}=\@RK;
$array_by_flag{BOND_EQUIL_VALUE}=\@REQ;
$array_by_flag{ANGLE_FORCE_CONSTANT}=\@TK;
$array_by_flag{ANGLE_EQUIL_VALUE}=\@TEQ;
$array_by_flag{DIHEDRAL_FORCE_CONSTANT}=\@PK;
$array_by_flag{DIHEDRAL_PERIODICITY}=\@PN;
$array_by_flag{DIHEDRAL_PHASE}=\@PHASE;
$array_by_flag{SOLTY}=\@SOLTY;
$array_by_flag{LENNARD_JONES_ACOEF}=\@CN1;
$array_by_flag{LENNARD_JONES_BCOEF}=\@CN2;
$array_by_flag{BONDS_INC_HYDROGEN}=\@BONDH;
$array_by_flag{BONDS_WITHOUT_HYDROGEN}=\@BOND;
$array_by_flag{ANGLES_INC_HYDROGEN}=\@ANGLEH;
$array_by_flag{ANGLES_WITHOUT_HYDROGEN}=\@ANGLE;
$array_by_flag{DIHEDRALS_INC_HYDROGEN}=\@THETAH;
$array_by_flag{DIHEDRALS_WITHOUT_HYDROGEN}=\@THETA;
$array_by_flag{EXCLUDED_ATOMS_LIST}=\@NATEX;
$array_by_flag{HBOND_ACOEF}=\@ASOL;
$array_by_flag{HBOND_BCOEF}=\@BSOL;
$array_by_flag{HBCUT}=\@HBCUT;
$array_by_flag{AMBER_ATOM_TYPE}=\@ISYMBL;
$array_by_flag{TREE_CHAIN_CLASSIFICATION}=\@ITREE;
$array_by_flag{JOIN_ARRAY}=\@JOIN;
$array_by_flag{IROTAT}=\@IROTAT;
$array_by_flag{SOLVENT_POINTERS}=\@SOLVENT_POINTERS;
$array_by_flag{ATOMS_PER_MOLECULE}=\@NSP;
$array_by_flag{BOX_DIMENSIONS}=\@BOX;
$array_by_flag{RADII}=\@GB_RADII;
$array_by_flag{SCREEN}=\@GB_SCREEN;
#
#
while (<INPUT>) {
	@input_line = split;
	#
	# %FLAG denotes the start of each new block
	#
	if ($input_line[0] eq "%FLAG") {
		$flag = $input_line[1];
		$_ = <INPUT>;
		s/\(/ /g;
		s/\)/ /g;
		($dum,$format,$dum) = split;
		print STDERR "reading block $flag with format $format\n";
		#
		# read this block
		# note read_block reads to the next %FLAG and then
		# rewinds the file by 1 line
		#
		push @{$array_by_flag{$flag}},read_block($format);
		$first = ${$array_by_flag{$flag}}[1];
		$last = ${$array_by_flag{$flag}}[$#{$array_by_flag{$flag}}];
                print STDERR "first: $first last: $last\n";
	}
}

#
# we are actually done reading the file, just parsing from here
# on out. . .
#

# prepare the $param_key list for indexing the system
# dimension variables from the header of the topology file

@param_key=(
	"NATOM","NTYPES","NBONH","MBONA","NTHETH","MTHETA",
	"NPHIH","MPHIA","NHPARM","NPARM","NEXT","NRES",
	"NBONA","NTHETA","NPHIA","NUMBND","NUMANG","NPTRA",
	"NATYP","NPHB","IFPERT","NBPER","NGPER","NDPER",
	"MBPER","MGPER","MDPER","IFBOX","NMXRS","IFCAP",
        "NEXTRA"
);

# first five lines of prmtop have system dimensions
# all names are exact uppercase AMBER equivalents
# these go in the hash %param_hash

%param_hash=();
foreach $key (@param_key) {
	$param_hash{$key} = shift @POINTERS;
}

# prepare the @param_vars list and corresponding 
# @param_sz_list
# these are used to make the %param_sz_hash which
# contains the scalar size of each array keyed by the
# array name

@param_vars=("IGRAPH","CHRG","AMASS","IAC","NUMEX",
		"ICO",
		"LABRES","IPRES",
		"RK","REQ",
		"TK","TEQ",
		"PK","PN","PHASE",
		"SOLTY",
		"CN1","CN2",
		"BONDH",
		"BOND",
		"ANGLEH",
		"ANGLE",
		"THETAH",
		"THETA",
		"NATEX",
		"ASOL","BSOL","HBCUT",
		"ISYMBL","ITREE","JOIN","IROTAT");

@param_sz_list=("NATOM","NATOM","NATOM","NATOM","NATOM",
		"NTYPES1",
		"NRES","NRES",
		"NUMBND","NUMBND",
		"NUMANG","NUMANG",
		"NPTRA","NPTRA","NPTRA",
		"NATYP",
		"NTYPES2","NTYPES2",
		"NBONH3",
		"NBONA3",
		"NTHETH4",
		"NTHETA4",
		"NPHIH5",
		"NPHIA5",
		"NEXT",
		"NPHB","NPHB","NPHB",
		"NATOM","NATOM","NATOM","NATOM");

# echo back the parameter/value pairs

foreach $cur_key (keys %param_hash) {
	print STDOUT "$cur_key : $param_hash{$cur_key}\n";
}

# add NTYPES1 = NTYPES**2 and NTYPES2 = NTYPES*(NTYPES+1)/2
# these are used for the LJ parameters and LJ type lists

$natom=$param_hash{"NATOM"};
$nres=$param_hash{"NRES"};
$ntypes=$param_hash{"NTYPES"};

%param_hash = (%param_hash, "NTYPES1"=>($ntypes**2),
		"NTYPES2"=>($ntypes*($ntypes+1)/2));

# add additional size parameters for the big internal arrays
# to the %param_hash
# these arrays need to be broken out into individual terms later. . .

%param_hash = (%param_hash,
		"NBONH3"=>(3*$param_hash{"NBONH"}),
		"NBONA3"=>(3*$param_hash{"MBONA"}),
		"NTHETH4"=>(4*$param_hash{"NTHETH"}),
		"NTHETA4"=>(4*$param_hash{"MTHETA"}),
		"NPHIH5"=>(5*$param_hash{"NPHIH"}),
		"NPHIA5"=>(5*$param_hash{"MPHIA"}));

# make %param_sz_hash, contains the scalar size of each value
# as well as $data_needed, the number of elements we still need to
# read from input

%param_sz_hash=();
if ($#param_vars == $#param_sz_list) {

        foreach $param (@param_vars) {
		$size = shift @param_sz_list;
        	print STDOUT "hash $param $size $param_hash{$size}\n";
                $param_sz_hash{$param}=$param_hash{$size};
        }

} else {

	print STDERR "Parameter size array problems!\n";
	exit(1);

}
# a few sanity checks

if ($IFPERT!=0) {
	print STDERR "ERROR: top2xml cannot handle perturbation topologies!\n";
	exit(1);
}

if ($IFCAP!=0) {
	print STDERR "ERROR: Blue Matter will not handle cap boundary cond.\n";
	exit(1);
}

#
# we no longer need to read in parameter arrays using the size data
# instead do a consistency check
#
foreach $parray (keys %param_sz_hash) {
	$len = $#{ $parray };
	if ($param_sz_hash{$parray} != $len) {
		print STDERR "size mismatch for $parray\n";
		print STDERR "read $len need $param_sz_hash{$parray}\n";
		exit(1);
	} else {
		print STDERR "read $len of $param_sz_hash{$parray} $parray\n";
	}
}
print STDERR "*****\n";

# get bonds

$num_bonds = $param_hash{"NBONH"} + $param_hash{"NBONA"};
#
# we assume all bonds are non-water unless we discover otherwise later
#
$num_nonwat_bonds = $num_bonds;
shift @BONDH;
shift @BOND;
print STDERR "shifted BONDS, $num_bonds \n";
#
# put BOND before BONDH since water-water bonds are all in BONDH
#
@bond = (@BOND, @BONDH);

if ($num_bonds > 0) {
	$b3 = ($#bond+1)/3;
	print "Reading $num_bonds bonds from a list of $b3\n";

	foreach $bnd (1 .. $num_bonds) {
	if ($bnd%1000 == 0) {
		print STDERR "reading bond $bnd\n";
	}
	$iba = get_atom(shift @bond);
	$IB[$bnd] = $iba;
	$jba = get_atom(shift @bond);
	$JB[$bnd] = $jba;
	$ICB[$bnd] = shift @bond;
	}
} else {
	print STDERR "no bonds; $num_bonds = 0\n";
}
print STDERR "done reading bonds\n";

# get angles

$num_angles = $param_hash{"NTHETH"} + $param_hash{"NTHETA"};
shift @ANGLEH;
shift @ANGLE;
@angles = (@ANGLE, @ANGLEH);

if ($num_angles > 0) {
$a4 = ($#angles+1)/4;
print "Reading $num_angles angles from a list of $a4\n";

foreach $ang (1 .. $num_angles) {
	if ($ang%100 == 0) {
		print STDERR "reading angle $ang\n";
	}
	$IT[$ang] = get_atom (shift @angles);
	$JT[$ang] = get_atom (shift @angles);
	$KT[$ang] = get_atom (shift @angles);
	$ICT[$ang] = shift @angles;
}
}
print STDERR "done reading angles\n";

# get torsions

$num_torsions = $param_hash{"NPHIH"} + $param_hash{"NPHIA"};
shift @THETAH;
shift @THETA;
@torsion = (@THETA, @THETAH);
$total_torsions=0;
$total_impropers=0;

$t5 = ($#torsion+1)/5;
print "Reading $num_torsions torsions from a list of $t5\n";
foreach $tor (1 .. $num_torsions) {
	if ($tor%1000 == 0) {
		print STDERR "reading torsion $tor\n";
	}
	$ip = get_atom (shift @torsion);
	$jp = get_atom (shift @torsion);
	$sign3 = shift @torsion;
	$kp = get_atom ($sign3);
	$sign4 = shift @torsion;
	$lp = get_atom ($sign4);
	$icp= shift @torsion;
	if ($sign3 < 0) {
# add code to block 1-4 interactions here
# not necessary right now 
	}
	if ($sign4 < 0) {
		$total_impropers++;
		$IPI[$total_impropers]=$ip;
		$JPI[$total_impropers]=$jp;
		$KPI[$total_impropers]=$kp;
		$LPI[$total_impropers]=$lp;
		$ICPI[$total_impropers]=$icp;
	} else {
		$total_torsions++;
		$IP[$total_torsions]=$ip;
		$JP[$total_torsions]=$jp;
		$KP[$total_torsions]=$kp;
		$LP[$total_torsions]=$lp;
		$ICP[$total_torsions]=$icp;
		$icp++;
		while ($PN[$icp-1]<0) {
			$total_torsions++;
			$IP[$total_torsions]=$ip;
			$JP[$total_torsions]=$jp;
			$KP[$total_torsions]=$kp;
			$LP[$total_torsions]=$lp;
			$ICP[$total_torsions]=$icp;
			$icp++;	
		}
	}
}
print STDERR "done reading torsions\n";

print "First atom: $IGRAPH[1] $CHRG[1] $AMASS[1] $IAC[1]\n";
print "Last atom: $IGRAPH[$natom] $CHRG[$natom] $AMASS[$natom] $IAC[$natom]\n";
print "First residue: 1 $LABRES[1] $IPRES[1]\n";
print "Last residue: $nres $LABRES[$nres] $IPRES[$nres]\n";

print "ASOL: $ASOL[1] BSOL: $BSOL[1] HBCUT: $HBCUT[1]\n";

# we need the RESIDUE() array for the following
#
# first rearrange the residue names
# hash of AMBER residue name keys that need to be changed
# to Blue Matter values

%res_name_hash = (
	"CYM" => "CYD",
	"HIP" => "HIH",
	"HID" => "HIS",
	"LYN" => "LYD",
        "NME" => "NMA",
        "WAT" => "HOH",
        "Cl-" => "CLM",
        "Na+" => "NAP",
);

@resname=("blank");

foreach $res (1 .. $nres) {
#fix the name
	foreach $reskey (keys %res_name_hash) {
		if ($LABRES[$res] eq $reskey) {
			$LABRES[$res] = $res_name_hash{$reskey};
		}
	}
#make the RESIDUE() array
	if ($res < $nres) {
		$atmax = $IPRES[$res+1]-1;
	} else {
		$atmax = $natom;
	}
	foreach $atom ($IPRES[$res] .. $atmax) {
		$RESIDUE[$atom] = $res;
	}
#
#make the RESINDEX and resname arrays
# check if this residue name is unique
# resname is the list of unique residue names
# RESINDEX is the pointer into resname for each residue
#
	$idx=0;
	while (($resname[$idx] ne $LABRES[$res])&&($idx <= $#resname)) {$idx++};
	if ($idx > $#resname) {
		push @resname, $LABRES[$res];
	}
	$RESINDEX[$res] = $idx;
}

# check to see if residues need to be renamed

foreach $res (0 .. $#resname) {
	foreach $reskey (keys %res_name_hash) {
		if ($resname[$res] eq $reskey) {
			$resname[$res]=$res_name_hash{$reskey};
		}
	}
}

#
# atom_name_hash: common atom names that need to get fixed.  3-letter
# hydrogen names with one number are problematic since they are not 
# consistent -- i.e. it is always 1HB, but sometimes 1HD and sometimes HD1
# depending on the residue
# 
# atom_res_hash: this is a hash of hashes of residue/atom name keys
# that we use to find atom names which must be changed
# note that this is used after the residue names have already been 
# cleaned up, so it has to refer to them.
#

%atom_name_hash = (
	"H1"  => "1H",
	"H2"  => "2H",
	"H3"  => "3H",
	"HB1" => "1XX",
	"HB2" => "1HB",
	"HB3" => "2HB",
	"HG1" => "1HG",
	"HG2" => "1HG",
	"HG3" => "2HG",
);

%atom_res_hash = (
	"NMA" => {
		"CH3" => "CA",
		"1HH3" => "1HA",
		"2HH3" => "2HA",
		"3HH3" => "3HA"
		},
	"ACE" => {
		"CT3" => "CH3",
		"1HH3" => "1H",
		"2HH3" => "2H",
		"3HH3" => "3H"
		},
	"ALA" => {
		"1XX" => "1HB",
		"1HB" => "2HB",
		"2HB" => "3HB",
		},
	"ARG" => {
		"HD2" => "1HD",
		"HD3" => "2HD",
		},
	"GLY" => {
		"HA2" => "1HA",
		"HA3" => "2HA",
		},
	"ILE" => {
		"2HG1" => "1HG1",
		"3HG1" => "2HG1",
		},
	"LYS" => {
		"HD2" => "1HD",
		"HD3" => "2HD",
		"HE2" => "1HE",
		"HE3" => "2HE",
		"HZ1" => "1HZ",
		"HZ2" => "2HZ",
		"HZ3" => "3HZ",
		},
	"MET" => {
		"HE1" => "1HE",
		"HE2" => "2HE",
		"HE3" => "3HE",
		},
	"PRO" => {
		"HD2" => "1HD",
		"HD3" => "2HD",
		},
	"THR" => {
		"1HG" => "HG1"
		},
	"HOH" => {
		"1H" => "H1",
		"2H" => "H2"
		},
        "CLM" => {
		"Cl-" => "Cl",
		},
        "NAP" => {
		"Na+" => "Na",
		}
);

@atomname=("blank");

foreach $name (0 .. $#IGRAPH) {
#
# if a name is four characters long (1234) wrap the 4th character (4123)
# this is done to get everything back into PDB format
#
	$IGRAPH[$name] =~ s/(.)(.)(.)(.)/$4$1$2$3/g;
#
# this misses some hydrogens like HB1/2, catch those here
#
	if (exists $atom_name_hash{$IGRAPH[$name]}) {
		$IGRAPH[$name] = $atom_name_hash{$IGRAPH[$name]};
	}
#
# check the hash of hashes of atom names we have to convert
#
	if (exists $atom_res_hash{$LABRES[$RESIDUE[$name]]}{$IGRAPH[$name]}) {
		$IGRAPH[$name] = $atom_res_hash{$LABRES[$RESIDUE[$name]]}{$IGRAPH[$name]};
	}
}

foreach $atom (1 .. $natom) {
	$idx=0;
	while (($atomname[$idx] ne $IGRAPH[$atom])&&($idx <= $#atomname)) 
		{$idx++};
	if ($idx > $#atomname) {
		push @atomname, $IGRAPH[$atom];
	}
	$ATOMINDEX[$atom] = $idx;
}
print STDERR "done name manimulations\n";

# if necessary, read the box information

$ibx = $param_hash{"IFBOX"};
print STDOUT "IFBOX: $ibx\n";

if ($param_hash{"IFBOX"}==2) {
	print STDOUT "Octahedral boxes not supported yet!\n"; exit(1);
}

if ($param_hash{"IFBOX"}==1) {
	print STDOUT "Reading box information\n";
	print STDERR "Reading box information\n";
	$iptres = shift @SOLVENT_POINTERS;
	$nspm = shift @SOLVENT_POINTERS;
	$nspsol = shift @SOLVENT_POINTERS;
	print "IPTRES: $iptres NSPM: $nspm NSPSOL: $nspsol\n";
	$waters = $nspm - $nspsol + 1;
        $lastatom=0;
	foreach $index (1 .. $nspm) {
		foreach $idx ($lastatom+1 .. $lastatom+$NSP[$index]) {
			$MOLECULE[$idx]=$index;
		}
# check molecule type
		$MTYPE[$index]="other";
		if ($index < $nspsol) {
			if ($NSP[$index] > 1) {
				if ($RESIDUE[$lastatom+1]!=$RESIDUE[$lastatom+$NSP[$index]]) {
					$MTYPE[$index]="other_polymer";
# add protein recognition here later
					$MTYPE[$index]="PROTEIN";
# 
				} else {
					$MTYPE[$index]="OTHER";
				}
			} else {
				$MTYPE[$index]="SALT";
			}
		} else {
			$MTYPE[$index]="SOLVENT";
		}
		$lastatom += $NSP[$index];
	}
        if ($waters > 0) {
          $firstwateratom = 0;
          while ($MOLECULE[$firstwateratom] != $nspsol) {
            $firstwateratom++;
	  }
        print STDOUT "$waters solvent molecules, first atom $firstwateratom\n";
#
# if this is a known water-type, 
# we have to reassign the bond and angle sizes to skip the water
# bonds and angles. . .
#
# the bond and angle list start from index #1.  we want num_nonwat_bonds
# and num_angls to point to the last non_water bond or angle or zero if we
# have an all-water topology
#
          if (($watertype ne "NONE")&&($watertype ne "TIP3P_flex")) {
            $nbond=1;
            while ($MOLECULE[$IB[$nbond]] < $nspsol) { $nbond++; }
            if ($nbond < $num_bonds) { $num_nonwat_bonds = $nbond-1; }
	    print "water bondex: $MOLECULE[$IB[$num_nonwat_bonds]] $num_nonwat_bonds\n";
            $nangl=1;
            while ($MOLECULE[$IB[$nangl]] < $nspsol) { $nangl++; }
            if ($nangl < $num_angles) { $num_angls = $nangl-1; }
	    print "water anglex: $MOLECULE[$IB[$num_angls]] $num_angls\n";
	  }
	} else {
	  # no water molecules but we have box information
	  $waters = 0;
	  #$nspm=1; $iptres=$nres;
	  $waters=0; $firstwateratom=$natom+1;
          $watertype="NONE";
	}
	
	$beta = $BOX[0];
	#
	# we have already read BOX 1-3
	#
} else {
# create dummy molecule array
	print STDOUT "No box information found\n";
	foreach $index (1 .. $natom) {
		$MOLECULE[$index]=1;
	}
        $nspm=1; $iptres=$nres;
	$nspsol=2; $waters=0; $firstwateratom=$natom+1;
	$MTYPE[0]="protein"; $MTYPE[1]="protein";
#
# use this to assume watertype=NONE, i.e. no waters
#
        $watertype="NONE";
}
print STDERR "done reading box & molecule info\n";

# convert the charges back to units of electron charge
# using the AMBER conversion factor
# 
# also assign atomic number here

print STDERR "converting units\n";
if (! $fullchgprec) {
	$charge_convert=18.2223;
} else {
	$charge_convert = 1.0;
}

@atomic_number=();
@element=();

foreach $index (1 .. $natom) {
	$CHRG[$index] = $CHRG[$index]/$charge_convert;
	$atname_nonum = $IGRAPH[$index];
	$atname_nonum =~ s/[0-9]//g;
        ($atomic_number[$index],$element[$index])
                = get_atomic_number( $atname_nonum );
#print "$index $IGRAPH[$index] $atomic_number[$index] $element[$index] $atname_nonum\n";
}

# convert the force constants and equilibrium angles for
# the angle and torsion parameter arrays to degrees (from radians)
# these values are straight from LEaP

$pi=3.1415926535;
#$radian_convert = 180.0/$pi;
$radian_convert = 1.0/0.0174533;
$radian_conv2 = $radian_convert ** 2;

#
# we keep angles in radians since there is a problem with loss/increase
# of precision if we let BlueMatter do the degrees -> radians conversion
# itself. . .
#

#foreach $index (1 .. $param_hash{"NUMANG"}) {
#	$TEQ[$index] *= $radian_convert;
#}
foreach $index (1 .. $param_hash{"NPTRA"}) {
	$PHASE[$index] *= $radian_convert;
}	

# calculate the SIGMA and EPSILON arrays per atom,
#

@SIGMA=();
@EPSILON=();
foreach $index (1 .. $ntypes) {
	foreach $jndex ($index .. $ntypes) {
		$nbtype=$ICO[$ntypes*($index-1)+$jndex];
		$c6 = $CN2[$nbtype];
		$c12 = $CN1[$nbtype];
	    if ($c12!=0&&$c6!=0) {
#		$SIGMA[$index][$jndex] = ($c12/$c6)**(1.0/6.0);
		$eps = $c6/(4.0*($c12/$c6));
#		$eps = nint($eps*10000.0)/10000.0;
		$EPSILON[$index][$jndex] = $eps;
		$SIGMA[$index][$jndex] = ($c6/(4.0*$eps))**(1.0/6.0);
	    } else {
		$SIGMA[$index][$jndex] = 0.0;
		$EPSILON[$index][$jndex] = 0.0;
	    }
#
# stupid -- the UDF wants r* and not sigma, just convert it here
#
#            $SIGMA[$index][$jndex] *= 2**(1.0/6.0);
	}
}
print STDERR "done converting units\n";

#
# done with the directly atom-indexed properties
# print them out. . . 

#foreach $index (1 .. $natom) {
#	print STDOUT "$IGRAPH[$index] $CHRG[$index] $AMASS[$index] ";
#        print STDOUT "$IAC[$index] $NUMEX[$index]\n";
#} 

# build a per-residue data structure
# array of lists, each list is LABRES, first atom, last atom
# n.b. not totally necessary, just to check stuff

foreach $index (1 .. $nres) {
	@residue = ($LABRES[$index], $IPRES[$index], $IPRES[$index+1]-1);
	$residue_list[$index] = [@residue];
}

# NOTE: add a per-residue firstatom lastatom output here

# now write out data in Blue Matter xml format
#

# JWP hand-coded right now
#
# WARNING: note all the WARNINGS! 
#
# header information is grabbed directly from Yuk's psf2xml.pl
#

print_header_spiel();

#
print OUTPUT "<ffparams>\n";
print OUTPUT "<header_parameter_list>\n";
print_head ("ff_family","char","AMBER");
print_head ("ff_spec","char","parm94");
# note assuming parm94, unable to tell from input data
print_head ("charge14","int",2);
print_headc ("charge14scale","double","true",1.0/1.2);
print_head ("lj14","int",2);
print_head ("lj14scale","double",0.5);
print_head ("torsioninput","int",2);
print_head ("improper","int",2);
print_head ("ureybradley","int",0);
print_head ("grouping","int",0);
@wtype = split /_/,$watertype;
print_head ("water","char",$wtype[0]);
print_head ("units","char","CHEM");
# charge conversion is 18.2223
print_headc ("ChargeConversion","double","true","SciConst::AmberChargeConversion");
# time conversion is 20.455
print_headc ("TimeConversionFactor","double","true","SciConst::AmberTimeConversion");
print OUTPUT "</header_parameter_list>\n";

# done with header, now molecule_type datalist

@MOLTYPES=("blank","protein","solvent","ion","other_polymer","other");

#print_data ("mt_","molecule_type","mtp_","char","class");
#for $index (1 .. $#MOLTYPES) {
#	print OUTPUT " <data_record data_id=\"mt_$index\">\n";
#	print OUTPUT "   <data_parameter param_ref=\"mtp_1\">$MOLTYPES[$index]";
#	print OUTPUT "</data_parameter>\n </data_record>\n";
#}
#print OUTPUT "</data_list>\n";

# now the molecule name datalist

print_data("mn_","molecule_name","mnp_","char","name","char","type");
#print OUTPUT "<data_list data_class_prepend=\"mn_\">\n";
#print OUTPUT "  <data_title>molecule_name</data_title>\n";
#print OUTPUT "  <data_parameter_list param_prepend=\"mnp_\">\n";
#print OUTPUT "     <data_parameter_spec param_id=\"mnp_1\" ";
#print OUTPUT "type=\"char\">name</data_parameter_spec>\n";
#print OUTPUT "     <data_parameter_spec param_id=\"mnp_2\" type=\"char\"";
#print OUTPUT ">type</data_parameter_spec>\n";
#print OUTPUT "  </data_parameter_list>\n";

# find novel molecule types (stored in @found_moltypes list)
# for each residue, check to see if it is on the list already
# if it isn't, push it on the list

@found_moltypes = ( 0 );
for $index (1 .. $nspm) {
	$found=0;
        $idx=0;
        while (($found == 0) && ($idx <= $#found_moltypes)) {
          if ($MTYPE[$index] eq $found_moltypes[$idx]) {
		$found = 1;
          }
          $idx++;
        }
        if ($found == 0) { push @found_moltypes, $MTYPE[$index]; }
}

# print out the molecule types in this topology
for $index (1 .. $#found_moltypes) {
	print OUTPUT " <data_record data_id=\"mn_$index\">\n";
	print OUTPUT "  <data_parameter param_ref=\"mnp_1\">molecule_$index";
	print OUTPUT "</data_parameter>\n";
	print OUTPUT "  <data_parameter param_ref=\"mnp_2\">";
	print OUTPUT "$found_moltypes[$index]";
	print OUTPUT "</data_parameter>\n";
	print OUTPUT " </data_record>\n";
}
print OUTPUT "</data_list>\n";

#
# now the chain list
# to get the chain list we look at all molecules which are not solvent;
# each one is called a chain
#

@chainletter=("A","B","C","D","E","F","G","H","I","J");

if (($nspsol-1) > 0) {
print OUTPUT "<data_list data_class_prepend=\"ch_\">\n";
print OUTPUT " <data_title>chain</data_title>\n";
print OUTPUT " <data_parameter_list param_prepend=\"chp_\">\n";
print OUTPUT "   <data_parameter_spec param_id=\"chp_1\" type=\"char\"";
print OUTPUT " >name</data_parameter_spec>\n";
print OUTPUT " </data_parameter_list>\n";
for $index (1 .. ($nspsol-1)) {
	
	print OUTPUT " <data_record data_id=\"ch_$index\">\n";
	print OUTPUT "  <data_parameter param_ref=\"chp_1\">$chainletter[$index-1]";
	print OUTPUT "</data_parameter>\n";
	print OUTPUT " </data_record>\n";
}
print OUTPUT "</data_list>\n";
}

# finally the molecule datalist

#print OUTPUT "<data_list data_class_prepend=\"m_\">\n";
#print OUTPUT " <data_title>molecule</data_title>\n";
#print OUTPUT " <data_parameter_list param_prepend=\"mp_\">\n";
#print OUTPUT "   <data_parameter_spec param_id=\"mp_1\" type=\"idref\"";
#print OUTPUT " ref_prepend=\"mn_\">molecule_type</data_parameter_spec>\n";
#print OUTPUT " </data_parameter_list>\n";
#for $index (1 .. $nspm) {
# find molecule type
#	$idx=0;
#	while ($MTYPE[$index] ne $MOLTYPES[$idx]) {$idx++;}
#	print OUTPUT " <data_record data_id=\"m_$index\">\n";
#	print OUTPUT "  <data_parameter_ref param_ref=\"mp_1\" ";
#	print OUTPUT "ref_ptr=\"mn_$index\"/>\n";
#	print OUTPUT " </data_record>\n";
#}
#print OUTPUT "</data_list>\n";

# 
# list the chemical component instances that are in this file
#

print_data ("cci_","chemical_component_instance","ccip_","char","cc_id");
foreach $cci (1 .. $nres) {
	print OUTPUT " <data_record data_id=\"cci_$cci\">\n";
	print OUTPUT "  <data_parameter param_ref=\"ccip_1\">$resname[$RESINDEX[$cci]]";
	print OUTPUT "</data_parameter>\n </data_record>\n";
}
print OUTPUT "</data_list>\n";

# now "monomer instances" -- i.e. residues
# note we only print this if there are chains in the file. . .

if (($nspsol-1) > 0) {
print_data ("moni_","monomer_instance","monip_",
            "idref","ch_","ref","chain_id",
            "idref","cci_","ref","cc_instance_id");
foreach $index (1 .. $iptres) {
	print OUTPUT " <data_record data_id=\"moni_$index\">\n";
	print OUTPUT "  <data_parameter_ref param_ref=\"monip_1\" ref_ptr=\"ch_$MOLECULE[$IPRES[$index]]\"/>\n";
	print OUTPUT "  <data_parameter_ref param_ref=\"monip_2\" ref_ptr=\"cci_$RESINDEX[$index]\"/>\n";
	print OUTPUT " </data_record>\n";
}
print OUTPUT "</data_list>\n";
}

# now "molecule instances" -- note that we have no way to check
# that any two molecules are the same/different so this is content-less
# for AMBER

print_data ("mi_","molecule_instance",
		"mip_","idref","mn_","ref","molecule_name");
foreach $index (1 .. $nspm) {
	print OUTPUT " <data_record data_id=\"mi_$index\">\n";
	$idx = 0;
	while ($found_moltypes[$idx] ne $MTYPE[$index]) {$idx++;}
 	print OUTPUT "  <data_parameter_ref param_ref=\"mip_1\" ref_ptr=\"mn_$idx\"/>\n";
	print OUTPUT " </data_record>\n";
}
print OUTPUT "</data_list>\n";

#print_data ("at_","atom_name","atp_","char","atom_name");
#foreach $index (1 .. $#atomname) {
#	print OUTPUT " <data_record data_id=\"at_$index\">\n";
#	print OUTPUT "  <data_parameter param_ref=\"atp_1\">$atomname[$index]";
#	print OUTPUT "</data_parameter>\n </data_record>\n";
#}
#print OUTPUT "</data_list>\n";

# now print data lists that contain the number of lj and lj14 params
# since AMBER uses the same parameters for both, these lists are the same 
# length. . .
#print OUTPUT "<data_list data_class_prepend=\"lj_\">\n";
#print OUTPUT " <data_title>Lennard_Jones_Types</data_title>\n";
#print OUTPUT " <data_parameter_list param_prepend=\"ljt_\">\n";
#print OUTPUT " </data_parameter_list>\n";
#foreach $index (1 .. $ntypes) {
#	print OUTPUT " <data_record data_id=\"lj_$index\"></data_record>\n";
#}
#print OUTPUT "</data_list>\n";
#print OUTPUT "<data_list data_class_prepend=\"lj14_\">\n";
#print OUTPUT " <data_title>Lennard_Jones_14_Types</data_title>\n";
#print OUTPUT " <data_parameter_list param_prepend=\"lj14t_\">\n";
#print OUTPUT " </data_parameter_list>\n";
#foreach $index (1 .. $ntypes) {
#	print OUTPUT " <data_record data_id=\"lj14_$index\"></data_record>\n";
#}
#print OUTPUT "</data_list>\n";

# WARNING: no exclusion facility
# AMBER topologies use explicitly-specified (rather than rules-based)
# nonbonded exclusion lists.
# There doesn't appear to be a corresponding option in the XML file.
#
# OK: Talked to Mike P.  1-3 exclusions are implicit.  Further exclusions
# will probably have to be hacked into xml2hpp or the hpp file itself.

@siteparm=([3,"m_","molecule"],
		[4,"cc_","chemical_component"],
		[5,"at_","atom_name"],
		[6,"lj_","lennard_jones_type"],
                [7,"lj14_","lennard_jones_14_type"]);

print_data ("site_","site_list","sp_",
		"double","mass",
		"double","charge",
		"int","mol_instance_id",
		"int","cc_instance_id",
		"char","atom_tag",
		"double","epsilon",
		"double","rmin",
		"double","epsilon14",
		"double","rmin14",
		"char","element");

#print OUTPUT "<data_list data_class_prepend=\"site_\">\n";
#print OUTPUT "<data_title>site_list</data_title>\n";
#print OUTPUT "<data_parameter_list param_prepend=\"sp_\">\n";
#
#$num=1;
#foreach $name ("mass","charge") {
#	print OUTPUT "<data_parameter_spec param_id=\"sp_$num\" ";
#	print OUTPUT "type=\"double\">$name</data_parameter_spec>\n";
#	$num++
#}
#for $ref (0 .. $#siteparm) {
#	$num = $siteparm[$ref][0];
#	$type = $siteparm[$ref][1];
#	$name = $siteparm[$ref][2];
#	print OUTPUT "<data_parameter_spec param_id=\"sp_$num\" ";
#	print OUTPUT " type=\"idref\" ref_prepend=\"$type\">$name";
#	print OUTPUT "</data_parameter_spec>\n";
#}
#print OUTPUT "<data_parameter_spec param_id=\"sp_8\" type=\"int\">";
#print OUTPUT "atomic_number</data_parameter_spec>\n";
#print OUTPUT "</data_parameter_list>\n";

foreach $index (1 .. $natom) {
	$res=$RESIDUE[$index];
	$rmin=nint($SIGMA[$IAC[$index]][$IAC[$index]]*10000.0)/10000.0;
	$rmin=$SIGMA[$IAC[$index]][$IAC[$index]];
	$mas= nint($AMASS[$index]*10000.0)/10000.0;
	$eps= nint($EPSILON[$IAC[$index]][$IAC[$index]]*100000000.0)/100000000.0;
	$eps= $EPSILON[$IAC[$index]][$IAC[$index]];
	if (($index < $firstwateratom)||
	    (($index >= $firstwateratom)&&($watertype ne "NULL"))) {
		$rmin="("."$rmin"."*SciConst::SixthRootOfTwo)";
		if (! $fullchgprec) {
			$chg= nint($CHRG[$index]*10000.0)/10000.0;
			$chg="("."$chg"."*ChargeConversion)";
		} else {
			$chg = "$CHRG[$index]";
		}
		print_site($index,$mas,$chg,
		$MOLECULE[$index],
                $res,$IGRAPH[$index],
		$eps,
		$rmin,
		$eps,
		$rmin,
		$element[$index]);
	} else {
		print_site($index,$mas,"NULL",
		$MOLECULE[$index],
                $res,$IGRAPH[$index],
		"NULL",
		"NULL",
		"NULL",
		"NULL",
		$element[$index]);
	}
}
print OUTPUT "</data_list>\n";

# why the fuck are there two bond lists? here is the first one 

if ($num_bonds > 0) {
	$bondcount=1;
	print OUTPUT "<data_list data_class_prepend=\"bond_\">\n";
	print OUTPUT "<data_title>bond_list</data_title>\n";
	print OUTPUT "<data_parameter_list param_prepend=\"bondp_\">";
	print OUTPUT "<data_parameter_spec param_id=\"bondp_1\" type=\"idref\"";
	print OUTPUT " ref_prepend=\"site_\">site_ref1</data_parameter_spec>\n";
	print OUTPUT "<data_parameter_spec param_id=\"bondp_2\" type=\"idref\"";
	print OUTPUT " ref_prepend=\"site_\">site_ref2</data_parameter_spec>\n";
	print OUTPUT "<data_parameter_spec param_id=\"bondp_3\" type=\"char\">";
	print OUTPUT "type</data_parameter_spec>\n";
	print OUTPUT "</data_parameter_list>\n";
        foreach $index (1 .. $num_bonds) {
            if (($AMASS[$IB[$index]]==1.008)&&($AMASS[$JB[$index]]==1.008)) {
#
# skip H-H bonds in this list for connectivity purposes
#
            } else { 
		print OUTPUT "<data_record data_id=\"bond_$bondcount\">\n";
		print OUTPUT "<data_parameter_ref param_ref=\"bondp_1\"";
		print OUTPUT " ref_ptr=\"site_$IB[$index]\"/>\n";
		print OUTPUT "<data_parameter_ref param_ref=\"bondp_2\"";
		print OUTPUT " ref_ptr=\"site_$JB[$index]\"/>\n";
		print OUTPUT "<data_parameter param_ref=\"bondp_3\">";
		print OUTPUT "single</data_parameter>\n";
		print OUTPUT "</data_record>\n";
            }
	    $bondcount++;
        }
        print OUTPUT "</data_list>\n";
}

# print UDF invocation table

print OUTPUT "<udf_invocation_table>\n";

# bonds
if ($num_nonwat_bonds > 0) {
  print OUTPUT "<udf_invocation_rec udf_id=\"StdHarmonicBondForce\" ";
  print OUTPUT "site_tuple_ref=\"StdHarmonicBondForce_sites\" ";
  print OUTPUT "param_tuple_ref=\"StdHarmonicBondForce_params\"/>\n";
}
 
# angles
if ($num_angles > 0) {
  print OUTPUT "<udf_invocation_rec udf_id=\"StdHarmonicAngleForce\" ";
  print OUTPUT "site_tuple_ref=\"StdHarmonicAngleForce_sites\" ";
  print OUTPUT "param_tuple_ref=\"StdHarmonicAngleForce_params\"/>\n";
}

# dihedrals
if (($total_torsions + $total_impropers) > 0 ) {
  print OUTPUT "<udf_invocation_rec udf_id=\"OPLSTorsionForce\" ";
  print OUTPUT "site_tuple_ref=\"OPLSTorsionForce_sites\" ";
  print OUTPUT "param_tuple_ref=\"OPLSTorsionForce_params\"/>\n";
}

# add rigid water UDF here, assume TIP3P for now

if ($waters > 0) {
  if ($watertype eq "TIP3P_rigid") {
    print OUTPUT "<udf_invocation_rec udf_id=\"WaterRigid3SiteRattle\" ";
    print OUTPUT "site_tuple_ref=\"Water3sites\"/>\n";
    print OUTPUT "<udf_invocation_rec udf_id=\"WaterRigid3SiteShake\" ";
    print OUTPUT "site_tuple_ref=\"Water3sites\"/>\n";
  }
  if ($watertype eq "TIP3P_flex") {
    print OUTPUT "<udf_invocation_rec udf_id=\"TIP3PForce\" ";
    print OUTPUT "site_tuple_ref=\"Water3sites\"/>\n";
  } 
}

print OUTPUT "</udf_invocation_table>\n";

# print bond UDF entries

if ($num_nonwat_bonds > 0) {
	print OUTPUT "<site_tuple_list id=\"StdHarmonicBondForce_sites\">\n";
        foreach $index (1 .. $num_nonwat_bonds) {
	   $bondcount++;
           print_tuple($IB[$index], $JB[$index]);
	}
        print STDERR "output $bondcount bonds\n";
	print OUTPUT "</site_tuple_list>\n";
# print bond UDF params
        print OUTPUT "<param_tuple_list id=\"StdHarmonicBondForce_params\">\n";
	print OUTPUT "<param_spec_list param_prepend=\"bh_\">\n";
 	print_fps("bh_1","double","k");
 	print_fps("bh_2","double","r0");
        print OUTPUT "</param_spec_list>\n";
        foreach $index (1 .. $num_nonwat_bonds) {
		print_param("bh_",$RK[$ICB[$index]],$REQ[$ICB[$index]]);
	}
        print OUTPUT "</param_tuple_list>\n";
#
# here is the real one
#        print OUTPUT "<force_list site_count=\"2\"";
#        print OUTPUT " force_term_id=\"StdHarmonicBondForce\">\n";
#	print OUTPUT "<force_parameter_list param_prepend=\"bh_\">\n";
#	print_fps ("bh_1","double","k");
#	print_fps ("bh_2","double","r0");
#	print OUTPUT "</force_parameter_list>\n";
#	foreach $index (1 .. $num_bonds) {
#		print_bond($IB[$index],$JB[$index],$RK[$ICB[$index]],
#                            $REQ[$ICB[$index]]);
#	}
#	print OUTPUT "</force_list>\n";
}

if ($num_angles > 0) {
        print OUTPUT "<site_tuple_list id=\"StdHarmonicAngleForce_sites\">\n";
	foreach $index (1 .. $num_angles) {
		print_tuple($IT[$index],$JT[$index],$KT[$index]);
	}
	print OUTPUT "</site_tuple_list>";
	print OUTPUT "<param_tuple_list id=\"StdHarmonicAngleForce_params\">\n";
	print OUTPUT "<param_spec_list param_prepend=\"ah_\">\n";
	print_fps ("ah_1","double","k");
	print_fps ("ah_2","double","th0");
	print OUTPUT "</param_spec_list>\n";
	foreach $index (1 .. $num_angles) {
#
# want 2 digits after the decimal
#
#                $angleval = (nint($TEQ[$ICT[$index]]*100.0)/100.0);
		if ($TK[$ICT[$index]] != 0) {
		  print_param("ah_",
		  $TK[$ICT[$index]],$TEQ[$ICT[$index]]);
		} else {
		  print_param("ah_",
		  0.0,$TEQ[$ICT[$index]]);
		}
	}
	print OUTPUT "</param_tuple_list>\n";
}

#
# here are the water tuple lists, since they replace angles/bonds
# note we supply this regardless of whether we know the appropriate
# UDF's or not. . .
#

if ($waters > 0) {
  print OUTPUT "<site_tuple_list id=\"Water3sites\">\n";
  for ($i=$firstwateratom; $i < $natom; $i+=3) {
     print_tuple(($i+1), $i, ($i+2));
  }
  print OUTPUT "</site_tuple_list>\n";
}

# note torsion list is special; may contain multi-term torsions
# the torsion list is expanded when it is read in, above
# so we can just dump it out here. . .

if (($total_torsions > 0)||($total_impropers > 0)) {
	print OUTPUT "<site_tuple_list id=\"OPLSTorsionForce_sites\">\n";
	foreach $index (1 .. $total_torsions) {
		if ($PK[$ICP[$index]] != 0) {
		print_tuple($IP[$index], $JP[$index], $KP[$index],
				$LP[$index]);
		}
	}
	foreach $index (1 .. $total_impropers) {
		if ($PK[$ICPI[$index]] != 0) {
		print_tuple($IPI[$index], $JPI[$index], $KPI[$index],
				$LPI[$index]);
		}
	}
	print OUTPUT "</site_tuple_list>\n";
        print OUTPUT "<param_tuple_list id=\"OPLSTorsionForce_params\">\n";
	print OUTPUT "<param_spec_list param_prepend=\"otp_\">\n";
	print_fps ("otp_1","double","k");
	print_fps ("otp_2","int","n");
	print_fps ("otp_3","double","sinDelta");
	print_fps ("otp_4","double","cosDelta");
	print OUTPUT "</param_spec_list>\n";
	foreach $index (1 .. $total_torsions) {
		if ($PK[$ICP[$index]] != 0) {
	 	print_param("otp_",
			2.0*$PK[$ICP[$index]],abs($PN[$ICP[$index]]),
			"sin(".$PHASE[$ICP[$index]]."*Math::Deg2Rad)",
			"cos(".$PHASE[$ICP[$index]]."*Math::Deg2Rad)",
                );
		}
	}	
	foreach $index (1 .. $total_impropers) {
		if ($PK[$ICPI[$index]] != 0) {
	 	print_param("otp_",
			2.0*$PK[$ICPI[$index]],$PN[$ICPI[$index]],
			"sin(".$PHASE[$ICPI[$index]]."*Math::Deg2Rad)",
			"cos(".$PHASE[$ICPI[$index]]."*Math::Deg2Rad)",
                );
		}
	}	
	print OUTPUT "</param_tuple_list>\n";
}

# done with all the bonded terms, on to LJ array -- no longer supported

#print OUTPUT "<pair_list type_class_prepend=\"lj_\">\n";
#print OUTPUT "<pair_parameter_list param_prepend=\"ljp_\">\n";
#print OUTPUT "<pair_parameter_spec param_id=\"ljp_1\"\n";
#print OUTPUT " type=\"double\">epsilon</pair_parameter_spec>\n";
#print OUTPUT "<pair_parameter_spec param_id=\"ljp_2\"\n";
#print OUTPUT " type=\"double\">sigma</pair_parameter_spec>\n";
#print OUTPUT "</pair_parameter_list>\n";
#for $index (1 .. $ntypes) {
#	for $jndex ($index .. $ntypes) {
#		print_pair("lj_".$index,"lj_".$jndex,$EPSILON[$index][$jndex],
#			$SIGMA[$index][$jndex],"ljp_");
#	}
#}
#
# output 1-4 pair list; not yet packed
# note epsilon has been scaled by 0.5 as per parm94,96,98
#
#print OUTPUT "</pair_list>\n";
#print OUTPUT "<pair_list type_class_prepend=\"lj14_\">\n";
#print OUTPUT "<pair_parameter_list param_prepend=\"lj14p_\">\n";
#print OUTPUT "<pair_parameter_spec param_id=\"lj14p_1\"\n";
#print OUTPUT " type=\"double\">epsilon</pair_parameter_spec>\n";
#print OUTPUT "<pair_parameter_spec param_id=\"lj14p_2\"\n";
#print OUTPUT " type=\"double\">sigma</pair_parameter_spec>\n";
#print OUTPUT "</pair_parameter_list>\n";
#for $index (1 .. $ntypes) {
#	for $jndex ($index .. $ntypes) {
#		print_pair("lj14_".$index,"lj14_".$jndex,0.5*$EPSILON[$index][$jndex],
#			$SIGMA[$index][$jndex],"lj14p_");
#	}
#}
#print OUTPUT "</pair_list>\n";
#
# done writing out LJ pair lists
#
print OUTPUT "</ffparams>\n";

# all done, close input & output files
close INPUT;
close OUTPUT;
exit(0);

### subroutine get_atom
#
#	de-obfuscates the atom references in the AMBER prmtop file
#
#	PRMTOP holds array indices rather than atom numbers
#
#	to get atom numbers, take the absolute value of the index,
#	divided by three, plus one
#
#	sign only matters for torsions, where it indicates either
#	proper/improper (sign of LP) or turns 1-4 interactions on/off
#	(sign of KP)
#
sub get_atom { return (abs($_[0])/3 + 1); };

sub nint { $ival = int($_[0]);
	if (($_[0]-$ival)>0.5) { $ival++ }
	return $ival;
}

#
# output subroutines
#

sub print_head {
	print OUTPUT "<header_parameter param_id=\"$_[0]\"";
	print OUTPUT " type=\"$_[1]\">$_[2]</header_parameter>\n";
}

sub print_headc {
	print OUTPUT "<header_parameter param_id=\"$_[0]\"";
	print OUTPUT " type=\"$_[1]\" symbolic_constant=\"$_[2]\">";
	print OUTPUT "$_[3]</header_parameter>\n";
}

sub print_data {
	print OUTPUT "<data_list data_class_prepend=\"$_[0]\">\n";
	print OUTPUT " <data_title>$_[1]</data_title>\n";
	print OUTPUT " <data_parameter_list param_prepend=\"$_[2]\">\n";
	@input = @_; shift @input; shift @input;
	my $pscount=1;
	my $tag = shift @input;
	while (@input) {
		my $type = shift @input;
		if ($type ne "idref") {
			my $name = shift @input;	
	print OUTPUT "  <data_parameter_spec param_id=\"$tag$pscount\"";
	print OUTPUT " type=\"$type\">$name</data_parameter_spec>\n";
		} else {
			my $pref = shift @input;
			my $rp = shift @input; $rp = $rp."_prepend";	
			my $name = shift @input;	
	print OUTPUT "   <data_parameter_spec param_id=\"$tag$pscount\"";
	print OUTPUT " type=\"$type\" $rp=\"$pref\"";
	print OUTPUT ">$name</data_parameter_spec>\n";
		}
		$pscount++;
	}
	print OUTPUT " </data_parameter_list>\n";
}

sub print_site {
	my $idx=0;
	print OUTPUT " <data_record data_id=\"site_$_[0]\">\n"; shift @_;
#        $val = shift @_; $idx = 1;
#	print OUTPUT "   <data_parameter param_ref=\"sp_$idx\">";
#	printf OUTPUT "%10.4f",$val;
#	print OUTPUT "</data_parameter>\n";
#	$val = shift @_; $idx = 2;
#	print OUTPUT "   <data_parameter param_ref=\"sp_$idx\">";
#	printf OUTPUT "(%10.4f*ChargeConversion)",$val;
#	print OUTPUT "</data_parameter>\n";
        while (@_) {
		$idx++; $val = shift @_;
		print OUTPUT "   <data_parameter param_ref=\"sp_$idx\">";
#		if (($idx==6)||($idx==8)) {
#			printf OUTPUT "%16.8f",$val;
#		} else {
			print OUTPUT "$val";
#		}
		print OUTPUT "</data_parameter>\n";
	}
	print OUTPUT " </data_record>\n";
}

sub print_fps {
	print OUTPUT "<param_spec param_id=\"$_[0]\"";
	print OUTPUT " type=\"$_[1]\">$_[2]</param_spec>\n";
}

sub print_tuple {
	print OUTPUT " <site_tuple>\n";
	foreach $index (0 .. $#_) {
                $iplus = $index+1;
		print OUTPUT "   <site site_ref=\"site_$_[$index]\"";
		print OUTPUT " site_ordinal=\"$iplus\"/>\n";
	}
        print OUTPUT " </site_tuple>\n";
}

sub print_param {
	print OUTPUT " <param_tuple>\n";
	foreach $index (1 .. $#_) {
		print OUTPUT "   <param param_ref=\"$_[0]$index\">";
		print OUTPUT "$_[$index]</param>\n";
	}
	print OUTPUT " </param_tuple>\n";
}

sub print_bond {
	print OUTPUT "<force_parameter_record>\n";
	print_fsite($_[0],1);
	print_fsite($_[1],2);
	print_fparm("bh_1",$_[2]);
	print_fparm("bh_2",$_[3]);
	print OUTPUT "</force_parameter_record>\n";
}

sub print_angle {
	print OUTPUT "<force_parameter_record>\n";
	print_fsite($_[0],1);
	print_fsite($_[1],2);
	print_fsite($_[2],3);
	print_fparm("ah_1",$_[3]);
	print_fparm("ah_2",$_[4]);
	print OUTPUT "</force_parameter_record>\n";
}

sub print_torsion {
	print OUTPUT "<force_parameter_record>\n";
	print_fsite($_[0],1);
	print_fsite($_[1],2);
	print_fsite($_[2],3);
	print_fsite($_[3],4);
	print_fparm("$_[7]1",$_[4]);
	print_fparm("$_[7]2",$_[5]);
	print_fparm("$_[7]3",$_[6]);
	print OUTPUT "</force_parameter_record>\n";
}

sub print_pair {
	print OUTPUT "<pair_record type_ref_1=\"$_[0]\" ";
	print OUTPUT "type_ref_2=\"$_[1]\">\n";
        print OUTPUT "<pair_parameter param_ref=\"$_[4]1\">$_[2]";
	print OUTPUT "</pair_parameter>\n";
	print OUTPUT "<pair_parameter param_ref=\"$_[4]2\">$_[3]";
	print OUTPUT "</pair_parameter>\n";
	print OUTPUT "</pair_record>\n";
}

sub print_fsite {
	print OUTPUT "<force_site site_ref=\"site_$_[0]\" ";
	print OUTPUT "site_ordinal=\"$_[1]\"/>\n";
}

sub print_fparm {
	print OUTPUT "<force_parameter param_ref=\"$_[0]\">";
        printf OUTPUT "%10.4f",$_[1];
	print OUTPUT "</force_parameter>\n";
}
 
sub get_atomic_number {
#
# JWP built from data lists I grabbed from Yuk's psf2xml.pl, 5 September 2001
#
# subroutine takes a string input -- corresponding to the atomic name
# and figures out the appropriate atomic number, which is returned
#
@elemid = qw"
    1    2    3    4    5    6    7    8    9   10
   11   12   13   14   15   16   17   18   19   20
   21   22   23   24   25   26   27   28   29   30
   31   32   33   34   35   36   37   38   39   40
   41   42   43   44   45   46   47   48   49   50
   51   52   53   54   55   56   57   58   59   60
   61   62   63   64   65   66   67   68   69   70
   71   72   73   74   75   76   77   78   79   80
   81   82   83   84   85   86   87   88   89   90
   91   92   93   94   95   96   97   98   99  100
  101  102  103  104  105  106  107  108  109";

@elemtype = qw"
H        He       Li       Be       B        C        N        O        F        Ne
Na       Mg       Al       Si       P        S        Cl       Ar       K        Ca       
Sc       Ti       V        Cr       Mn       Fe       Co       Ni       Cu       Zn       
GA       GE       AS       SE       BR       KR       RB       SR       YB       Zr       
NB       MO       TC       RU       RH       PD       AG       CD       IN       Sn       
SB       TE       I        XI       Cs       BA       LA       CE       PR       Nd       
PM       SM       EU       GD       TB       DY       HO       ER       TM       Yb       
LU       HF       TA       W        RE       OS       IR       PT       AU       Hg       
TL       PB       BI       PO       AT       RN       FR       RA       AC       TH       
PA       U        NP       PU       AM       CM       BK       CF       ES       FM       
MD       NO       LR       RF       HA       SG       NS       HS       MT";

$rawstring = $_[0];
$namestring = substr($rawstring, 0, 1);

# add special cases to this list 

@special_cases = ("CL", "Cl", "NA", "Na");
@special_index = (16,16,10,10);

my $idx = 0;
foreach $case (@special_cases) {
      if (substr($rawstring, 0, length($case)) eq $case) {
         $namestring = $case;
	 my $index = $special_index[$idx];
         return ($elemid[$index],$elemtype[$index]);
      }
      $idx++;
}
#print " $namestring\n";

NAME_CHECK: foreach $index (0 .. $#elemtype) {
	if ($elemtype[$index] eq $namestring) {
		return ($elemid[$index],$elemtype[$index]);
	}
}

return 999;

}

#
sub print_header_spiel {
#
# grabbed from Yuk's psf2xml.pl, 5 September 2001
#
# JWP
#
print OUTPUT "<?xml version='1.0' encoding='US-ASCII' standalone='no' ?>\n";
print OUTPUT "<!DOCTYPE ffparams [\n";
print OUTPUT "<!ELEMENT ffparams (header_parameter_list, (data_list)+,\n";
print OUTPUT "(udf_invocation_table)?, (site_tuple_list | param_tuple_list)*,\n";
print OUTPUT "(pair_list)*, (group_list)*)>\n\n";

print OUTPUT "<!-- The header parameter list contains per system information.  Each -->\n";
print OUTPUT "<!-- parameter has associated name, type, and symbolic_constant -->\n";
print OUTPUT "<!-- attributes.  If symbolic_constant=\"true\" then it is intended that -->\n";
print OUTPUT "<!-- the parameter so labeled is to be treated as a symbolic constant -->\n";
print OUTPUT "<!-- for use by downstream applications-->\n";
print OUTPUT "<!ELEMENT header_parameter_list (header_parameter)+>\n";
print OUTPUT "<!ELEMENT header_parameter (#PCDATA)>\n";
print OUTPUT "<!ATTLIST header_parameter param_id ID #REQUIRED>\n";
print OUTPUT "<!ATTLIST header_parameter type (char | double | int) \"char\">\n";
print OUTPUT "<!ATTLIST header_parameter symbolic_constant (true | false) \"false\">\n\n";

print OUTPUT "<!-- The data_list contains a list of type_ids that are used -->\n";
print OUTPUT "<!-- for a variety of purposes, including molecule types and molecule -->\n";
print OUTPUT "<!-- instances as well as LJ types.   These types may have -->\n";
print OUTPUT "<!-- parameter values associated with them which are defined in the -->\n";
print OUTPUT "<!-- type_parameter_list. Information about what type_id is -->\n";
print OUTPUT "<!-- bound to a particular site_id must be conveyed via a -->\n";
print OUTPUT "<!-- site_parameter_ref. It is required that type_ids consist of the -->\n";
print OUTPUT "<!-- type_class string with an underscore and number appended -->\n";
print OUTPUT "<!ELEMENT data_list (data_title, data_parameter_list, (data_record)+)>\n";
print OUTPUT "<!ATTLIST data_list data_class_prepend ID #REQUIRED>\n";
print OUTPUT "<!ELEMENT data_title (#PCDATA)>\n";
print OUTPUT "<!ELEMENT data_parameter_list (data_parameter_spec)*>\n";
print OUTPUT "<!ATTLIST data_parameter_list param_prepend ID #REQUIRED>\n";
print OUTPUT "<!ELEMENT data_parameter_spec (#PCDATA)>\n";
print OUTPUT "<!ATTLIST data_parameter_spec param_id ID #REQUIRED>\n";
print OUTPUT "<!ATTLIST data_parameter_spec type (char | double | int | idref) \"char\">\n";
print OUTPUT "<!ATTLIST data_parameter_spec ref_prepend IDREF #IMPLIED>\n";
print OUTPUT "<!ELEMENT data_record ((data_parameter | data_parameter_ref))*>\n";
print OUTPUT "<!ATTLIST data_record data_id ID #REQUIRED>\n";
print OUTPUT "<!ELEMENT data_parameter (#PCDATA)>\n";
print OUTPUT "<!ATTLIST data_parameter param_ref IDREF #REQUIRED>\n";
print OUTPUT "<!ELEMENT data_parameter_ref EMPTY>\n";
print OUTPUT "<!ATTLIST data_parameter_ref param_ref IDREF #REQUIRED>\n";
print OUTPUT "<!ATTLIST data_parameter_ref ref_ptr IDREF #REQUIRED>\n";

print OUTPUT "<!-- The invocation table contains a list of all the user defined -->\n";
print OUTPUT "<!-- functions required for this system and pointers to tuples of -->\n";
print OUTPUT "<!-- sites and parameters required for each UDF   -->\n";
print OUTPUT "<!ELEMENT udf_invocation_table (udf_invocation_rec)*>\n";
print OUTPUT "<!ELEMENT udf_invocation_rec EMPTY>\n";
print OUTPUT "<!ATTLIST udf_invocation_rec udf_id ID #REQUIRED>\n";
print OUTPUT "<!ATTLIST udf_invocation_rec site_tuple_ref IDREF #REQUIRED>\n";
print OUTPUT "<!ATTLIST udf_invocation_rec param_tuple_ref IDREF #IMPLIED>\n\n";

print OUTPUT "<!-- The site tuple list contains a collection of site_id tuples that are -->\n";
print OUTPUT "<!-- used by one or more udfs.  The order of the tuples in this list -->\n";
print OUTPUT "<!-- must correspond to the order of the tuples in the corresponding -->\n";
print OUTPUT "<!-- param_tuple_list -->\n";
print OUTPUT "<!ELEMENT site_tuple_list (site_tuple)+>\n";
print OUTPUT "<!ATTLIST site_tuple_list id ID #REQUIRED>\n";
print OUTPUT "<!ELEMENT site_tuple (site)+>\n";
print OUTPUT "<!ELEMENT site EMPTY>\n";
print OUTPUT "<!ATTLIST site site_ref IDREF #REQUIRED>\n";
print OUTPUT "<!ATTLIST site site_ordinal CDATA #REQUIRED>\n\n";

print OUTPUT "<!-- The param tuple list contains a collection of parameter tuples -->\n";
print OUTPUT "<!-- that are used by one or more udfs.  The order of the tuples in -->\n";
print OUTPUT "<!-- this list must correspond to the order of the tuples in the -->\n";
print OUTPUT "<!-- corresponding site_tuple_list.  This list also has the usual -->\n";
print OUTPUT "<!-- parameter specification block at the beginning.  The element -->\n";
print OUTPUT "<!-- value of the param_spec element is the name of the parameter -->\n";
print OUTPUT "<!ELEMENT param_tuple_list (param_spec_list, (param_tuple)+)>\n";
print OUTPUT "<!ATTLIST param_tuple_list id ID #REQUIRED>\n";
print OUTPUT "<!ELEMENT param_spec_list (param_spec)+>\n";
print OUTPUT "<!ATTLIST param_spec_list param_prepend NMTOKEN #REQUIRED>\n";
print OUTPUT "<!ELEMENT param_spec (#PCDATA)>\n";
print OUTPUT "<!ATTLIST param_spec param_id ID #REQUIRED>\n";
print OUTPUT "<!ATTLIST param_spec type (double | int) \"double\">\n";
print OUTPUT "<!ELEMENT param_tuple (param)+>\n";
print OUTPUT "<!ELEMENT param (#PCDATA)>\n";
print OUTPUT "<!ATTLIST param param_ref IDREF #REQUIRED>\n\n";

print OUTPUT "<!-- The pair list contains a list of pair interaction parameters for -->\n";
print OUTPUT "<!-- each pair of the associated types-->\n";
print OUTPUT "<!ELEMENT pair_list (pair_parameter_list, (pair_record)+)>\n";
print OUTPUT "<!ATTLIST pair_list type_class_prepend IDREF #REQUIRED>\n";
print OUTPUT "<!ELEMENT pair_parameter_list (pair_parameter_spec)+>\n";
print OUTPUT "<!ATTLIST pair_parameter_list param_prepend ID #REQUIRED>\n";
print OUTPUT "<!ELEMENT pair_parameter_spec (#PCDATA)>\n";
print OUTPUT "<!ATTLIST pair_parameter_spec param_id ID #REQUIRED>\n";
print OUTPUT "<!ATTLIST pair_parameter_spec type (char | double | int) \"char\">\n";
print OUTPUT "<!ELEMENT pair_record (pair_parameter)+>\n";
print OUTPUT "<!ATTLIST pair_record type_ref_1 IDREF #REQUIRED>\n";
print OUTPUT "<!ATTLIST pair_record type_ref_2 IDREF #REQUIRED>\n";
print OUTPUT "<!ELEMENT pair_parameter (#PCDATA)>\n";
print OUTPUT "<!ATTLIST pair_parameter param_ref IDREF #REQUIRED>\n\n";

print OUTPUT "<!-- User defined groups contain group information and a list of group -->\n";
print OUTPUT "<!-- members specified by site_id-->\n";
print OUTPUT "<!ELEMENT group_list (group_info, (group_member)+)>\n";
print OUTPUT "<!ATTLIST group_list group_id ID #REQUIRED>\n";
print OUTPUT "<!ATTLIST group_list group_class CDATA #REQUIRED>\n";
print OUTPUT "<!ELEMENT group_info (#PCDATA)>\n";
print OUTPUT "<!ELEMENT group_member EMPTY>\n";
print OUTPUT "<!ATTLIST group_member site_ref IDREF #REQUIRED>\n";
print OUTPUT "]>\n";

# done spewing header info
}

sub nint {
	my $integer = int ($_[0]);
	my $residual = $_[0] - $integer;
	if (abs($residual) < 0.5) {
		return $integer; 
	} else {
		if ($integer > 0) {
			return $integer+1;
		} else {
			return $integer-1;
		}
	}
}

#
sub read_block {
	#
	my $format_string = shift @_;
	$format_string =~ s/E/e/g;
	$format_string =~ s/I/i/g;
	#
	# we have to rearrange the format string
	#
	my @tmplist = split /[a-z]/,$format_string;
	my $num = @tmplist[0];
	my $for = "";
	my @tmplist = split / */,$format_string;
	while (!($tmplist[0] =~ /[a-z]/)) {
		shift @tmplist;
	}
	my $ftype = shift @tmplist;
	my $fstring = "";
	foreach $ele (@tmplist) {
		$fstring .= $ele;
	}
	print STDERR "$num $ftype $fstring\n";
	my @output_list = ();
	$oldpos=tell(INPUT);
	while (($_ = <INPUT>)&&(!($_ =~ /FLAG/))) {
                #print STDERR "$_";
                my @read = my_scanf($ftype,$fstring,$_);
                #print STDERR "@read\n";
                push @output_list, @read;
		$oldpos=tell(INPUT);
	}
	my $len = $#output_list + 1;
	seek(INPUT,$oldpos,0);
	print STDERR "read_block $format_string read $len elements\n";
	return @output_list;
}

sub my_scanf {
	#
	# isn't a true scanf -- we just make sure to put a blank
	# space between all elements as necessary
	#
	my $ftype = shift @_;
	my $fstring = shift @_;
	my $input = shift @_;
	#
	#
	my @tmp = split /\./,$fstring;
	my $fsize = $tmp[0];
	my $instr = "";
	my $pos = 0;
	while ($pos < length($input)) {
		my $tmp = substr $input,$pos,$fsize;
		#
		# delete the whitespace in each element
		#
		$tmp =~ s/\s+//g;
		$instr .= $tmp;
		$instr .= " ";
		$pos += $fsize;
	}
	#
	my @output_list = split /\s+/,$instr;
	#
	return @output_list;
}		
