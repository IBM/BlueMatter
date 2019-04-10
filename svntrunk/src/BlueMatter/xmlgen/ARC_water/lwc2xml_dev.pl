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
	print STDERR "usage:  lwc2xml <numwat> <xmlout>\n";
        exit(1);
}

$numwat = $ARGV[0];
$xmlout = $ARGV[1];


if (! open(OUTPUT,">$xmlout") ) {
	print STDERR "Can't open output xmlout file $xmlout\n";
	exit(1);
}

print "writing XML for $numwat waters\n";

# water is O H1 H2 -- HOH
#

$c6 =    595.05;
$c12 = 582000.0;
$sigma = ($c12/$c6)**(1.0/6.0);
$eps = $c6/(4.0*($c12/$c6));
$rmin = $sigma * 2.0**(1.0/6.0);
print "$c6 $c12 $sigma $eps $rmin\n";

$ochg = -15.19784022;
$hchg =   7.59892011;

# WARNING: note all the WARNINGS! 
#
# header information is grabbed directly from Yuk's psf2xml.pl
#

print_header_spiel();

#
print OUTPUT "<ffparams>\n";
print OUTPUT "<header_parameter_list>\n";
print_head ("ff_family","char","AMBER");
print_head ("ff_spec","char","ARC_water");
# note assuming parm94, unable to tell from input data
print_head ("charge14","int",2);
print_headc ("charge14scale","double","true",1.0/1.2);
print_head ("lj14","int",2);
print_head ("lj14scale","double",0.5);
print_head ("torsioninput","int",2);
print_head ("improper","int",2);
print_head ("ureybradley","int",0);
print_head ("grouping","int",0);
print_head ("water","char","NULL");
print_head ("units","char","CHEM");
print_headc ("ChargeConversion","double","true","SciConst::AmberChargeConversion");
# time conversion is 20.455
print_headc ("TimeConversionFactor","double","true","SciConst::AmberTimeConversion");
print OUTPUT "</header_parameter_list>\n";

# now the molecule name datalist

print_data("mn_","molecule_name","mnp_","char","name","char","type");
@found_moltypes = ("","solvent");

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
# list the chemical component instances that are in this file
#

print_data ("cci_","chemical_component_instance","ccip_","char","cc_id");
foreach $cci (1 .. $numwat) {
	print OUTPUT " <data_record data_id=\"cci_$cci\">\n";
	print OUTPUT "  <data_parameter param_ref=\"ccip_1\">HOH";
	print OUTPUT "</data_parameter>\n </data_record>\n";
}
print OUTPUT "</data_list>\n";

# now "monomer instances" -- i.e. residues
# note we only print this if there are chains in the file. . .

# now "molecule instances" -- note that we have no way to check
# that any two molecules are the same/different so this is content-less
# for AMBER

print_data ("mi_","molecule_instance",
		"mip_","idref","mn_","ref","molecule_name");
foreach $index (1 .. $numwat) {
	print OUTPUT " <data_record data_id=\"mi_$index\">\n";
 	print OUTPUT "  <data_parameter_ref param_ref=\"mip_1\" ref_ptr=\"mn_1\"/>\n";
	print OUTPUT " </data_record>\n";
}
print OUTPUT "</data_list>\n";

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

$atom=1;
foreach $index (1 .. $numwat) {
	print_site($atom,16.0,$ochg,
	$index, $index, "O",
	$eps, $rmin, $eps, $rmin,
	"O");
	$atom++;
	foreach $idx (1 .. 2) {
		print_site($atom,1.0,$hchg,
		$index, $index, "H$idx",
		0.0, 0.0, 0.0, 0.0,
		"H");
		$atom++;
	}	
}
print OUTPUT "</data_list>\n";

# why the fuck are there two bond lists? here is the first one 

if ($numwat > 0) {
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
        foreach $index (1 .. $numwat) {
		$oxy = $index*3 - 2;
		foreach $idx (1 .. 2) {
			$hyd = $oxy + $idx;
		print OUTPUT "<data_record data_id=\"bond_$bondcount\">\n";
		print OUTPUT "<data_parameter_ref param_ref=\"bondp_1\"";
		print OUTPUT " ref_ptr=\"site_$oxy\"/>\n";
		print OUTPUT "<data_parameter_ref param_ref=\"bondp_2\"";
		print OUTPUT " ref_ptr=\"site_$hyd\"/>\n";
		print OUTPUT "<data_parameter param_ref=\"bondp_3\">";
		print OUTPUT "single</data_parameter>\n";
		print OUTPUT "</data_record>\n";
		$bondcount++;
		}
        }
        print OUTPUT "</data_list>\n";
}

# print UDF invocation table

print OUTPUT "<udf_invocation_table>\n";

# add rigid water UDF here, assume TIP3P for now

#if ($waters > 0) {
#  if ($watertype eq "TIP3P_rigid") {
#    print OUTPUT "<udf_invocation_rec udf_id=\"WaterRigid3SiteRattle\" ";
#    print OUTPUT "site_tuple_ref=\"Water3sites\"/>\n";
#    print OUTPUT "<udf_invocation_rec udf_id=\"WaterRigid3SiteShake\" ";
#    print OUTPUT "site_tuple_ref=\"Water3sites\"/>\n";
#  }
#  if ($watertype eq "TIP3P_flex") {
#    print OUTPUT "<udf_invocation_rec udf_id=\"TIP3PForce\" ";
#    print OUTPUT "site_tuple_ref=\"Water3sites\"/>\n";
#  } 
#}

print OUTPUT "</udf_invocation_table>\n";

# print bond UDF entries

#
# here are the water tuple lists, since they replace angles/bonds
# note we supply this regardless of whether we know the appropriate
# UDF's or not. . .
#

if ($numwat > 0) {
  print OUTPUT "<site_tuple_list id=\"Water3sites\">\n";
  for ($i=1; $i < $numwat*3; $i+=3) {
     print_tuple(($i+1), $i, ($i+2));
  }
  print OUTPUT "</site_tuple_list>\n";
}

# done writing out LJ pair lists
#
print OUTPUT "</ffparams>\n";

# all done, close input & output files
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
H        HE       LI       BE       B        C        N        O        F        NE
NA       MG       AL       SI       P        S        CL       AR       K        CA       
SC       TI       V        CR       MN       FE       CO       NI       CU       ZN       
GA       GE       AS       SE       BR       KR       RB       SR       YB       ZR       
NB       MO       TC       RU       RH       PD       AG       CD       IN       SN       
SB       TE       I        XI       CS       BA       LA       CE       PR       ND       
PM       SM       EU       GD       TB       DY       HO       ER       TM       YB       
LU       HF       TA       W        RE       OS       IR       PT       AU       HG       
TL       PB       BI       PO       AT       RN       FR       RA       AC       TH       
PA       U        NP       PU       AM       CM       BK       CF       ES       FM       
MD       NO       LR       RF       HA       SG       NS       HS       MT";

$rawstring = $_[0];
$namestring = substr($rawstring, 0, 1);

# add special cases to this list 

@special_cases = ("CL", "NA", "Na");

foreach $case (@special_cases) {
      if (substr($rawstring, 0, length($case)) eq $case) {
         $namestring = $case;
      }
}
print " $namestring\n";

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
