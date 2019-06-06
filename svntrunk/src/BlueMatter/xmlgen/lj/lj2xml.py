#!/usr/bin/env python

# #################################################################
#
# File: lj2xml.py
#
# Author: RSG
# Date: May 26, 2003
#
# Description: Utility to generate xml for a pure Lennard-Jones system
#              comprised of N identical particles.  As usual for
#              BlueMatter parameter files, LJ params are given in
#              terms of Rmin and epsilon.  However, command line
#              arguments specifying LJ params can be given in terms of
#              sigma or Rmin and the appropriate symbolic constants
#              will be inserted in the output.
#
# #################################################################


from __future__ import print_function
from optik import OptionParser
import sys

def outputHeaderParameter(paramID, type, symConst, paramValue):
    print("<header_parameter param_id=\"%s\" type=\"%s\" symbolic_constant=\"%s\">%s</header_parameter>" % (paramID, type, symConst, paramValue))

def outputHeaderParameterList():
    print("<header_parameter_list>")
    outputHeaderParameter("ff_family", "char", "false", "CHARMM")
    outputHeaderParameter("ff_spec", "char", "false", "CHARMM22")
    outputHeaderParameter("charge14", "int", "true", "0")
    outputHeaderParameter("charge14scale", "double", "true", "1.0")
    outputHeaderParameter("lj14", "int", "true", "0")
    outputHeaderParameter("lj14scale", "double", "true", "1.0")
    outputHeaderParameter("units", "char", "false", "CHEM")
    outputHeaderParameter("ChargeConversion", "double", "true", "SciConst::StdChargeConversion")
    outputHeaderParameter("TimeConversionFactor", "double", "true", "SciConst::StdTimeConversion")
    print("</header_parameter_list>")

def outputMoleculeName(name, type):
    print("  <data_list data_class_prepend=\"mn_\">")
    print("    <data_title>molecule_name</data_title>")
    print("    <data_parameter_list param_prepend=\"mnp_\">")
    print("      <data_parameter_spec param_id=\"mnp_1\" type=\"char\">name</data_parameter_spec>")
    print("      <data_parameter_spec param_id=\"mnp_2\" type=\"char\">type</data_parameter_spec>")
    print("    </data_parameter_list>")
    print("    <data_record data_id=\"mn_1\">")
    print("      <data_parameter param_ref=\"mnp_1\">%s</data_parameter>" % (name))
    print("      <data_parameter param_ref=\"mnp_2\">%s</data_parameter>" % (type))
    print("    </data_record>")
    print("  </data_list>")

def outputChemicalComponentInstanceList(count, ccName):
    print("  <data_list data_class_prepend=\"cci_\">")
    print("    <data_title>chemical_component_instance</data_title>")
    print("    <data_parameter_list param_prepend=\"ccip_\">")
    print("      <data_parameter_spec param_id=\"ccip_1\" type=\"char\">cc_id</data_parameter_spec>")
    print("    </data_parameter_list>")
    for i in range(count):
        print("    <data_record data_id=\"cci_%d\">" % (i+1))
        print("      <data_parameter param_ref=\"ccip_1\">%s</data_parameter>" % (ccName))
        print("    </data_record>")
    print("  </data_list>")

def outputMoleculeInstanceList(count):
    print("  <data_list data_class_prepend=\"mi_\">")
    print("    <data_title>molecule_instance</data_title>")
    print("    <data_parameter_list param_prepend=\"mip_\">")
    print("      <data_parameter_spec param_id=\"mip_1\" type=\"idref\" ref_prepend=\"mn_\">molecule_name</data_parameter_spec>")
    print("    </data_parameter_list>")
    for i in range(count):
        print("    <data_record data_id=\"mi_%d\">" % (i+1))
        print("      <data_parameter_ref param_ref=\"mip_1\" ref_ptr=\"mn_1\"/>")
        print("    </data_record>")
    print("  </data_list>")

def outputSiteList(count, mass, atomTag, epsilon, rmin, element):
    print("  <data_list data_class_prepend=\"site_\">")
    print("    <data_title>site_list</data_title>")
    print("    <data_parameter_list param_prepend=\"sp_\">")
    print("      <data_parameter_spec param_id=\"sp_1\" type=\"double\">mass</data_parameter_spec>")
    print("      <data_parameter_spec param_id=\"sp_2\" type=\"double\">charge</data_parameter_spec>")
    print("      <data_parameter_spec param_id=\"sp_3\" type=\"int\">mol_instance_id</data_parameter_spec>")
    print("      <data_parameter_spec param_id=\"sp_4\" type=\"int\">cc_instance_id</data_parameter_spec>")
    print("      <data_parameter_spec param_id=\"sp_5\" type=\"char\">atom_tag</data_parameter_spec>")
    print("      <data_parameter_spec param_id=\"sp_6\" type=\"double\">epsilon</data_parameter_spec>")
    print("      <data_parameter_spec param_id=\"sp_7\" type=\"double\">rmin</data_parameter_spec>")
    print("      <data_parameter_spec param_id=\"sp_8\" type=\"double\">epsilon14</data_parameter_spec>")
    print("      <data_parameter_spec param_id=\"sp_9\" type=\"double\">rmin14</data_parameter_spec>")
    print("      <data_parameter_spec param_id=\"sp_10\" type=\"char\">element</data_parameter_spec>")
    print("    </data_parameter_list>")
    for i in range(count):
        print("    <data_record data_id=\"site_%d\">" % (i+1))
        print("      <data_parameter param_ref=\"sp_1\">%s</data_parameter>" % (mass))
        print("      <data_parameter param_ref=\"sp_2\">0.0</data_parameter>")
        print("      <data_parameter param_ref=\"sp_3\">%d</data_parameter>" % (i+1))
        print("      <data_parameter param_ref=\"sp_4\">%d</data_parameter>" % (i+1))
        print("      <data_parameter param_ref=\"sp_5\">%s</data_parameter>" % (atomTag))
        print("      <data_parameter param_ref=\"sp_6\">%s</data_parameter>" % (epsilon))
        print("      <data_parameter param_ref=\"sp_7\">%s</data_parameter>" % (rmin))
        print("      <data_parameter param_ref=\"sp_8\">%s</data_parameter>" % (epsilon))
        print("      <data_parameter param_ref=\"sp_9\">%s</data_parameter>" % (rmin))
        print("      <data_parameter param_ref=\"sp_10\">%s</data_parameter>" % (element))
        print("    </data_record>")
    print("  </data_list>")

    
def outputXmlHeaderWithDtd():
    print('<?xml version=\'1.0\' encoding=\'US-ASCII\' standalone=\'no\' ?>')
    print('<!DOCTYPE ffparams [')
    print('<!ELEMENT ffparams (header_parameter_list, (data_list)+,')
    print('(udf_invocation_table)?, (site_tuple_list | param_tuple_list)*,')
    print('(group_list)*)>\n')
    
    print('<!-- The header parameter list contains per system information.  Each -->')
    print('<!-- parameter has associated name, type, and symbolic_constant -->')
    print('<!-- attributes.  If symbolic_constant="true" then it is intended that -->')
    print('<!-- the parameter so labeled is to be treated as a symbolic constant -->')
    print('<!-- for use by downstream applications-->')
    print('<!ELEMENT header_parameter_list (header_parameter)+>')
    print('<!ELEMENT header_parameter (#PCDATA)>')
    print('<!ATTLIST header_parameter param_id ID #REQUIRED>')
    print('<!ATTLIST header_parameter type (char | double | int) "char">')
    print('<!ATTLIST header_parameter symbolic_constant (true | false) "false">')
    print(" ")
    print('<!-- The data_list contains a list of type_ids that are used -->')
    print('<!-- for a variety of purposes, including molecule types and molecule -->')
    print('<!-- instances as well as LJ types.   These types may have -->')
    print('<!-- parameter values associated with them which are defined in the -->')
    print('<!-- type_parameter_list. Information about what type_id is -->')
    print('<!-- bound to a particular site_id must be conveyed via a -->')
    print('<!-- site_parameter_ref. It is required that type_ids consist of the -->')
    print('<!-- type_class string with an underscore and number appended -->')
    print('<!ELEMENT data_list (data_title, data_parameter_list, (data_record)*)>')
    print('<!ATTLIST data_list data_class_prepend ID #REQUIRED>')
    print('<!ELEMENT data_title (#PCDATA)>')
    print('<!ELEMENT data_parameter_list (data_parameter_spec)*>')
    print('<!ATTLIST data_parameter_list param_prepend ID #REQUIRED>')
    print('<!ELEMENT data_parameter_spec (#PCDATA)>')
    print('<!ATTLIST data_parameter_spec param_id ID #REQUIRED>')
    print('<!ATTLIST data_parameter_spec type (char | double | int | idref) "char">')
    print('<!ATTLIST data_parameter_spec ref_prepend IDREF #IMPLIED>')
    print('<!ELEMENT data_record ((data_parameter | data_parameter_ref))*>')
    print('<!ATTLIST data_record data_id ID #REQUIRED>')
    print('<!ELEMENT data_parameter (#PCDATA)>')
    print('<!ATTLIST data_parameter param_ref IDREF #REQUIRED>')
    print('<!ELEMENT data_parameter_ref EMPTY>')
    print('<!ATTLIST data_parameter_ref param_ref IDREF #REQUIRED>')
    print('<!ATTLIST data_parameter_ref ref_ptr IDREF #REQUIRED>')
    print('<!-- The invocation table contains a list of all the user defined -->')
    print('<!-- functions required for this system and pointers to tuples of -->')
    print('<!-- sites and parameters required for each UDF   -->')
    print('<!ELEMENT udf_invocation_table (udf_invocation_rec)*>')
    print('<!ELEMENT udf_invocation_rec EMPTY>')
    print('<!ATTLIST udf_invocation_rec udf_id ID #REQUIRED>')
    print('<!ATTLIST udf_invocation_rec site_tuple_ref IDREF #REQUIRED>')
    print('<!ATTLIST udf_invocation_rec param_tuple_ref IDREF #IMPLIED>')
    print(" ")
    print('<!-- The site tuple list contains a collection of site_id tuples that are -->')
    print('<!-- used by one or more udfs.  The order of the tuples in this list -->')
    print('<!-- must correspond to the order of the tuples in the corresponding -->')
    print('<!-- param_tuple_list -->')
    print('<!ELEMENT site_tuple_list (site_tuple)+>')
    print('<!ATTLIST site_tuple_list id ID #REQUIRED>')
    print('<!ELEMENT site_tuple (site)+>')
    print('<!ELEMENT site EMPTY>')
    print('<!ATTLIST site site_ref IDREF #REQUIRED>')
    print('<!ATTLIST site site_ordinal CDATA #REQUIRED>')
    print(" ")
    print('<!-- The param tuple list contains a collection of parameter tuples -->')
    print('<!-- that are used by one or more udfs.  The order of the tuples in -->')
    print('<!-- this list must correspond to the order of the tuples in the -->')
    print('<!-- corresponding site_tuple_list.  This list also has the usual -->')
    print('<!-- parameter specification block at the beginning.  The element -->')
    print('<!-- value of the param_spec element is the name of the parameter -->')
    print('<!ELEMENT param_tuple_list (param_spec_list, (param_tuple)+)>')
    print('<!ATTLIST param_tuple_list id ID #REQUIRED>')
    print('<!ELEMENT param_spec_list (param_spec)+>')
    print('<!ATTLIST param_spec_list param_prepend NMTOKEN #REQUIRED>')
    print('<!ELEMENT param_spec (#PCDATA)>')
    print('<!ATTLIST param_spec param_id ID #REQUIRED>')
    print('<!ATTLIST param_spec type (double | int) "double">')
    print('<!ELEMENT param_tuple (param)+>')
    print('<!ELEMENT param (#PCDATA)>')
    print('<!ATTLIST param param_ref IDREF #REQUIRED>')
    print(" ")
    print('<!-- User defined groups contain group information and a list of group -->')
    print('<!-- members specified by site_id-->')
    print('<!ELEMENT group_list (group_info, (group_member)+)>')
    print('<!ATTLIST group_list group_id ID #REQUIRED>')
    print('<!ATTLIST group_list group_class CDATA #REQUIRED>')
    print('<!ELEMENT group_info (#PCDATA)>')
    print('<!ELEMENT group_member EMPTY>')
    print('<!ATTLIST group_member site_ref IDREF #REQUIRED>')
    print(']>')


def main():
    usage = "usage: %prog [options] epsilon sigma/Rmin"
    parser = OptionParser(usage)
    
    parser.add_option("-s", "--sigma", action="store_true",
                      dest="sigma", default="store_false",
                      help="sets input type of sigma/Rmin to sigma")
    parser.add_option("-r", "--rmin", action="store_false",
                      dest="sigma", default="store_false",
                      help="sets input type of sigma/Rmin to Rmin")
    parser.add_option("-n", "--number", action="store",
                      dest="count", default=4,
                      help="particle count")
    parser.add_option("-c", "--chemcomp", action="store",
                      dest="chemcomp", default="AR",
                      help="chemical component")
    parser.add_option("-e", "--element", action="store",
                      dest="element", default="Ar",
                      help="element symbol")
    parser.add_option("-t", "--atomtag", action="store",
                      dest="atomtag", default="AR_",
                      help="atom tag")
    parser.add_option("-m", "--mass", action="store",
                      dest="mass", default="39.948",
                      help="mass (amu)")
    parser.add_option("-d", "--moleculename", action="store",
                      dest="moleculename", default="Argon",
                      help="molecule name")
    parser.add_option("-k", "--kelvin", action="store_true",
                      dest="kelvin", default="store_false",
                      help="energy units in kelvin")
    
    (options, args) = parser.parse_args()
    if len(args) < 2:
        parser.error("incorrect number of args")

    if int(options.count) <= 0:
        parser.error("Illegal value for particle number")

    epsilon = args[0]
    if options.kelvin:
        epsilon = epsilon + "*SciConst::KBoltzmann_IU"
    rMinValue = args[1]
    if options.sigma:
        rMinValue = rMinValue + "*SciConst::SixthRootOfTwo"

    outputXmlHeaderWithDtd()
    print("<ffparams>")
    outputHeaderParameterList()
    outputMoleculeName(options.moleculename, "SALT")
    outputChemicalComponentInstanceList(int(options.count), options.chemcomp)
    outputMoleculeInstanceList(int(options.count))
    outputSiteList(int(options.count), options.mass, options.atomtag, epsilon, rMinValue, options.element)
    print("</ffparams>")

main()
