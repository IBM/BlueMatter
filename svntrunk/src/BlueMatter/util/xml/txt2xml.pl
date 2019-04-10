#!/usr/agora/bin/perl
########################################################################
#
# File: txt2xml.pl
# Author: RSG
# Date: September 28, 2000
# Description: Filter to take force field independent text file and
# transform it into xml obeying the following dtd:
#
# <?xml version="1.0" encoding="UTF-8"?>
# <!DOCTYPE ffparams [
# <!ELEMENT ffparams (header, sitelist, bondlist, anglelist, torsionlist, improperlist)>
# <!ELEMENT header (charge | vdw | vdwcombine | charge14 | charge14scale | vdw14 | vdw14scale | improper | torsioninput | ureybradley | grouping | water | units)*>
# <!ELEMENT vdw (#PCDATA)>
# <!ELEMENT vdwcombine (#PCDATA)>
# <!ELEMENT charge14 (#PCDATA)>
# <!ELEMENT charge14scale (#PCDATA)>
# <!ELEMENT vdw14 (#PCDATA)>
# <!ELEMENT vdw14scale (#PCDATA)>
# <!ELEMENT improper (#PCDATA)>
# <!ELEMENT torsioninput (#PCDATA)>
# <!ELEMENT ureybradley (#PCDATA)>
# <!ELEMENT grouping (#PCDATA)>
# <!ELEMENT water (#PCDATA)>
# <!ELEMENT units (#PCDATA)>
# <!ELEMENT sitelist (siterec)+>
# <!ELEMENT bondlist (bondrec)+>
# <!ELEMENT anglelist (anglerec)+>
# <!ELEMENT torsionlist (torsionrec)+>
# <!ELEMENT improperlist (improperrec)+>
# <!ELEMENT siterec (siteid, sitetype, charge, sigma, epsilon)>
# <!ELEMENT support (#PCDATA)>
# <!ELEMENT siteid (#PCDATA)>
# <!ELEMENT sitetype (#PCDATA)>
# <!ELEMENT charge (#PCDATA)>
# <!ELEMENT sigma (#PCDATA)>
# <!ELEMENT epsilon (#PCDATA)>
# <!ELEMENT bondrec (siteid, siteid, k, r0)>
# <!ELEMENT k (#PCDATA)>
# <!ELEMENT r0 (#PCDATA)>
# <!ELEMENT anglerec (siteid, siteid, siteid, k, th0, ku, ru)>
# <!ELEMENT th0 (#PCDATA)>
# <!ELEMENT ku (#PCDATA)>
# <!ELEMENT ru (#PCDATA)>
# <!ELEMENT torsionrec (siteid, siteid, siteid, siteid, (torparm)+)>
# <!ELEMENT improperrec (siteid, siteid, siteid, siteid, (torparm)+)>
# <!ELEMENT torparm (order, v)>
# <!ELEMENT order (#PCDATA)>
# <!ELEMENT v (#PCDATA)>
# ]>
#
#
# Usage: txt2xml.pl < file.in > file.xml
#
########################################################################

$unknownMode = 0;
$headerMode = 1;
$siteMode = 2;
$bondMode = 3;
$angleMode = 4;
$torsionMode = 5;
$improperMode = 6;

@mode = ("unknown", "header", "site", "bond", "angle", "normal
torsion", "improper torsion");

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


print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
print "<!DOCTYPE ffparams [\n";
print "<!ELEMENT ffparams (header, sitelist, bondlist, anglelist, torsionlist, improperlist)>\n";
print "<!ELEMENT header (charge | vdw | vdwcombine | charge14 | charge14scale | vdw14 | vdw14scale | improper | torsioninput | ureybradley | grouping | water | units)*>\n";
print "<!ELEMENT vdw (#PCDATA)>\n";
print "<!ELEMENT vdwcombine (#PCDATA)>\n";
print "<!ELEMENT charge14 (#PCDATA)>\n";
print "<!ELEMENT charge14scale (#PCDATA)>\n";
print "<!ELEMENT vdw14 (#PCDATA)>\n";
print "<!ELEMENT vdw14scale (#PCDATA)>\n";
print "<!ELEMENT improper (#PCDATA)>\n";
print "<!ELEMENT torsioninput (#PCDATA)>\n";
print "<!ELEMENT ureybradley (#PCDATA)>\n";
print "<!ELEMENT grouping (#PCDATA)>\n";
print "<!ELEMENT water (#PCDATA)>\n";
print "<!ELEMENT units (#PCDATA)>\n";
print "<!ELEMENT sitelist (siterec)+>\n";
print "<!ELEMENT bondlist (bondrec)+>\n";
print "<!ELEMENT anglelist (anglerec)+>\n";
print "<!ELEMENT torsionlist (torsionrec)+>\n";
print "<!ELEMENT improperlist (improperrec)+>\n";
print "<!ELEMENT siterec (siteid, sitetype, charge, sigma, epsilon)>\n";
print "<!ELEMENT support (#PCDATA)>\n";
print "<!ELEMENT siteid (#PCDATA)>\n";
print "<!ELEMENT sitetype (#PCDATA)>\n";
print "<!ELEMENT charge (#PCDATA)>\n";
print "<!ELEMENT sigma (#PCDATA)>\n";
print "<!ELEMENT epsilon (#PCDATA)>\n";
print "<!ELEMENT bondrec (siteid, siteid, k, r0)>\n";
print "<!ELEMENT k (#PCDATA)>\n";
print "<!ELEMENT r0 (#PCDATA)>\n";
print "<!ELEMENT anglerec (siteid, siteid, siteid, k, th0, ku, ru)>\n";
print "<!ELEMENT th0 (#PCDATA)>\n";
print "<!ELEMENT ku (#PCDATA)>\n";
print "<!ELEMENT ru (#PCDATA)>\n";
print "<!ELEMENT torsionrec (siteid, siteid, siteid, siteid, (torparm)+)>\n";
print "<!ELEMENT improperrec (siteid, siteid, siteid, siteid, (torparm)+)>\n";
print "<!ELEMENT torparm (order, v)>\n";
print "<!ELEMENT order (#PCDATA)>\n";
print "<!ELEMENT v (#PCDATA)>\n";
print "]>\n";
print "<ffparams>\n";

$mode = $unknownMode;
$oldMode = $unknownMode;

while (<>) {

    if (m/^\# Functional Forms/) {
	$oldMode = $mode;
	$mode = $headerMode;
	next;
    } elsif (m/^\# atom /) {
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
	    print "</$listTag[$oldMode]>\n";
	}
	print "<$listTag[int($mode)]>\n";
	$oldMode = $mode;
    }

    chop;
    split;
    if ($mode == $headerMode) {
	print "<$headerTag{$_[0]}>$_[1]</$headerTag{$_[0]}>\n";
    } elsif ($mode == $siteMode) {
	print "<siterec><siteid>$_[0]</siteid><sitetype>$_[1]</sitetype>";
	print "<charge>$_[2]</charge><sigma>$_[3]</sigma>";
	print "<epsilon>$_[4]</epsilon></siterec>\n";
    } elsif ($mode == $bondMode) {
	print "<bondrec><siteid>$_[0]</siteid><siteid>$_[1]</siteid>";
	print "<k>$_[2]</k><r0>$_[3]</r0></bondrec>\n";
    } elsif ($mode == $angleMode) {
	print
    "<anglerec><siteid>$_[0]</siteid><siteid>$_[1]</siteid>";
	print "<siteid>$_[2]</siteid><k>$_[3]</k><th0>$_[4]</th0>";
	print "<ku>$_[5]</ku><ru>$_[6]</ru></anglerec>\n";
    } elsif ($mode == $torsionMode) {
	if (scalar(@_) < 8) {
	    print STDERR "Illegal torsion record\n";
	    exit;
	}
	print
	    "<torsionrec>\n<siteid>$_[0]</siteid><siteid>$_[1]</siteid>";
	print "<siteid>$_[2]</siteid><siteid>$_[3]</siteid>\n";
	print "<torparm>";
	print "<order>0</order><v>$_[4]</v>";
	print "</torparm>\n";
	print "<torparm>";
	print "<order>1</order><v>$_[5]</v>";
	print "</torparm>\n";
	print "<torparm>";
	print "<order>2</order><v>$_[6]</v>";
	print "</torparm>\n";
	print "<torparm>";
	print "<order>3</order><v>$_[7]</v>";
	print "</torparm>\n";
	print "</torsionrec>\n";
    } elsif ($mode == $improperMode) {
	if (scalar(@_) < 8) {
	    print STDERR "Illegal improper torsion record\n";
	    exit;
	}
	print
	    "<improperrec>\n<siteid>$_[0]</siteid><siteid>$_[1]</siteid>";
	print "<siteid>$_[2]</siteid><siteid>$_[3]</siteid>\n";
	print "<torparm>";
	print "<order>0</order><v>$_[4]</v>";
	print "</torparm>\n";
	print "<torparm>";
	print "<order>1</order><v>$_[5]</v>";
	print "</torparm>\n";
	print "<torparm>";
	print "<order>2</order><v>$_[6]</v>";
	print "</torparm>\n";
	print "<torparm>";
	print "<order>3</order><v>$_[7]</v>";
	print "</torparm>\n";
	print "</improperrec>\n";
    } else {
	print STDERR "Illegal mode = $mode\n";
	exit;
    }
    
}
print "</$listTag[$mode]>\n";
print "</ffparams>\n";



