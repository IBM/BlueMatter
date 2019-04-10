#!/usr/bin/perl
###############################################################################
# File: probspectrans_to_MSD_merge.pl
# Author: cwo
# Date: July 14, 2006 
# Description: Script that takes the output of probspectrans_to_MSD
#              (ignoring comments), inserts it into an MSD file
#              and removes all arrays that should be replaced in the MSD
# Usage: 
#   probspectrans_to_MSD_merge.pl <output of probspectrans_to_MSD> <old MSD>
###############################################################################
# NOTE: The output of probspectrans_to_MSD will ONLY merge correctly with
#       a probspecdb2 generated MSD if the TEST_PROBSPECDB2 was defined
#       during probspectrans_to_MSD's compile -- otherwise, 
#       probspetrans_to_MSD adheres to the /molsysdef/include/MSD.hpp interface
###############################################################################

$PROBSPECTRANS_OUTPUT_FILENAME = @ARGV[0];
$OLD_MSD_FILENAME = @ARGV[1];





############################################
# Open old MSD... just read through, spitting text back
# to stdout, except for array's we're replacing
$past_definition = 0;
$do_output = 1;
$in_class_def = 0;
$saw_combiner = 0;

open(MSDFILE, "$OLD_MSD_FILENAME");
while (<MSDFILE>) 
  {
  $fileline = $_;

  ########################
  # Make sure all our variables are defined in the class MSD for the UDF tables
  # So delete these defs as we encounter them, and we'll fill in the defs later
  if (/^class MSD$/) {
    $in_class_def = 1;
  }

  if (
      /static const SiteId Rigid2Sites\[\];/ or
      /static const SiteId Rigid3Sites\[\];/ or
      /static const SiteId Rigid4Sites\[\];/ or
      /static const UDF_Binding::ShakeGroup2_Params Rigid2Params\[\];/ or
      /static const UDF_Binding::ShakeGroup3_Params Rigid3Params\[\];/ or
      /static const UDF_Binding::ShakeGroup4_Params Rigid4Params\[\];/ or
      /static const SiteId StdHarmonicBondForce_sitesSiteTupleList\[\];/ or
      /static const UDF_Binding::StdHarmonicBondForce_Params StdHarmonicBondForce_paramsParamTupleList\[\];/ or
      /static const SiteId StdHarmonicAngleForce_sitesSiteTupleList\[\];/ or
      /static const UDF_Binding::StdHarmonicAngleForce_Params StdHarmonicAngleForce_paramsParamTupleList\[\];/ or
      /static const SiteId UreyBradleyForce_sitesSiteTupleList\[\];/ or
      /static const UDF_Binding::UreyBradleyForce_Params UreyBradleyForce_paramsParamTupleList\[\];/ or
      /static const SiteId SwopeTorsionForce_sitesSiteTupleList\[\];/ or
      /static const UDF_Binding::SwopeTorsionForce_Params SwopeTorsionForce_paramsParamTupleList\[\];/ or
      /static const SiteId ImproperDihedralForce_sitesSiteTupleList\[\];/ or
      /static const UDF_Binding::ImproperDihedralForce_Params ImproperDihedralForce_paramsParamTupleList\[\];/ or
      /static const SiteId OPLSTorsionForce_sitesSiteTupleList\[\];/ or
      /static const UDF_Binding::OPLSTorsionForce_Params OPLSTorsionForce_paramsParamTupleList\[\]/ or
      /static const SiteId OPLSImproperForce_sitesSiteTupleList\[\]/ or
      /static const UDF_Binding::OPLSImproperForce_Params OPLSImproperForce_paramsParamTupleList\[\]/
     ) {
    $fileline = "";
    }

  if (/^};$/ and ($in_class_def == 1))   
    {
      print "static const SiteId Rigid2Sites[];\n";
      print "static const SiteId Rigid3Sites[];\n";
      print "static const SiteId Rigid4Sites[];\n";
      print "static const UDF_Binding::ShakeGroup2_Params Rigid2Params[];\n";
      print "static const UDF_Binding::ShakeGroup3_Params Rigid3Params[];\n";
      print "static const UDF_Binding::ShakeGroup4_Params Rigid4Params[];\n";
      print "static const SiteId StdHarmonicBondForce_sitesSiteTupleList[];\n";
      print "static const UDF_Binding::StdHarmonicBondForce_Params StdHarmonicBondForce_paramsParamTupleList[];\n";
      print "static const SiteId StdHarmonicAngleForce_sitesSiteTupleList[];\n";
      print "static const UDF_Binding::StdHarmonicAngleForce_Params StdHarmonicAngleForce_paramsParamTupleList[];\n";
      print "static const SiteId UreyBradleyForce_sitesSiteTupleList[];\n";
      print "static const UDF_Binding::UreyBradleyForce_Params UreyBradleyForce_paramsParamTupleList[];\n";
      print "static const SiteId SwopeTorsionForce_sitesSiteTupleList[];\n";
      print "static const UDF_Binding::SwopeTorsionForce_Params SwopeTorsionForce_paramsParamTupleList[];\n";
      print "static const SiteId ImproperDihedralForce_sitesSiteTupleList[];\n";
      print "static const UDF_Binding::ImproperDihedralForce_Params ImproperDihedralForce_paramsParamTupleList[];\n";
      print "static const SiteId OPLSTorsionForce_sitesSiteTupleList[];\n";
      print "static const UDF_Binding::OPLSTorsionForce_Params OPLSTorsionForce_paramsParamTupleList[];\n";
      print "static const SiteId OPLSImproperForce_sitesSiteTupleList[];\n";
      print "static const UDF_Binding::OPLSImproperForce_Params OPLSImproperForce_paramsParamTupleList[];\n";
      print "static const int Water3sitesSize;\n";
      $in_class_def = 0;
    }

  ########################
  # This is were we want to insert the output of probspectrans_to_MSD
  # -> this is the first occurance of MSD_COMPILE_CODE_ONLY right
  #    after the end of the MSD class definition

  if (/^#ifndef MSD_COMPILE_CODE_ONLY/ and ($past_definition == 0))
    {
    print "\n#ifndef MSD_COMPILE_CODE_ONLY\n";
    open(PBTFILE, "$PROBSPECTRANS_OUTPUT_FILENAME");
    while (<PBTFILE>)
      {
      print $_ ;
      }
    $past_definition = 1;
    next;
    } #endif insert output of probspectrans_to_MSD

  ########################
  # Remove the various arrays we don't want from the original MSD
  # (multi-line removes)
  if (   
      /const SiteInfo MSD::SiteInformation\[\]/ or
      /const int MSD::SiteIDtoSiteTypeMap\[\]/ or
      /const UDF_Binding::NSQCoulomb_Params MSD::ChargeNsqParams\[\]/ or
      /const UDF_Binding::NSQLennardJones_Params MSD::LJNsqParams\[\]/ or
      /const int MSD::NsqSiteIDtoSiteTypeMap\[\]/ or
      /const SiteId MSD::ExclusionList1_2_3_4\[\]/ or
      /const UDF_Binding::LennardJonesForce_Params MSD::LJ14PairTable\[\]/ or
      /const UDF_Binding::Coulomb14_Params MSD::Charge14PairTable\[\]/ or
      /const SiteId MSD::Pair14SiteList\[\]/ or
      /const IrreduciblePartitionType MSD::IrreduciblePartitionTypeList\[\]/ or
      /const IrreduciblePartitionMap MSD::IrreduciblePartitionIndexToTypeMap\[\]/ or
      /const SiteId MSD::ExternalToInternalSiteIdMap\[\]/ or
      /const SiteId MSD::InternalToExternalSiteIdMap\[\]/ or
      /const SiteId MSD::Water3sites\[\]/ or
      /const SiteId MSD::Rigid2Sites\[\]/ or
      /const SiteId MSD::Rigid3Sites\[\]/ or
      /const SiteId MSD::Rigid4Sites\[\]/ or
      /const UDF_Binding::ShakeGroup2_Params MSD::Rigid2Params\[\]/ or
      /const UDF_Binding::ShakeGroup3_Params MSD::Rigid3Params\[\]/ or
      /const UDF_Binding::ShakeGroup4_Params MSD::Rigid4Params\[\]/ or
      /const SiteId MSD::StdHarmonicBondForce_sitesSiteTupleList\[\]/ or
      /const UDF_Binding::StdHarmonicBondForce_Params MSD::StdHarmonicBondForce_paramsParamTupleList\[\]/ or
      /const SiteId MSD::StdHarmonicAngleForce_sitesSiteTupleList\[\]/ or
      /const UDF_Binding::StdHarmonicAngleForce_Params MSD::StdHarmonicAngleForce_paramsParamTupleList\[\]/ or
      /const SiteId MSD::UreyBradleyForce_sitesSiteTupleList\[\]/ or
      /const UDF_Binding::UreyBradleyForce_Params MSD::UreyBradleyForce_paramsParamTupleList\[\]/ or
      /const SiteId MSD::SwopeTorsionForce_sitesSiteTupleList\[\]/ or
      /const UDF_Binding::SwopeTorsionForce_Params MSD::SwopeTorsionForce_paramsParamTupleList\[\]/ or
      /const SiteId MSD::ImproperDihedralForce_sitesSiteTupleList\[\]/ or
      /const UDF_Binding::ImproperDihedralForce_Params MSD::ImproperDihedralForce_paramsParamTupleList\[\]/ or
      /const SiteId MSD::OPLSImproperForce_sitesSiteTupleList\[\]/ or
      /const UDF_Binding::OPLSImproperForce_Params MSD::OPLSImproperForce_paramsParamTupleList\[\]/ or
      /const SiteId MSD::OPLSTorsionForce_sitesSiteTupleList\[\]/ or
      /const UDF_Binding::OPLSTorsionForce_Params MSD::OPLSTorsionForce_paramsParamTupleList\[\]/ 
     ) {
      $do_output = 0;
    } #endif must remove multi-line section
  
  ########################
  # Remove the various arrays size variables we don't want from the 
  # original MSD (single-line removes)
  if (   
       /#define RIGID2SITES_TUPLE_CNT / or
       /#define RIGID3SITES_TUPLE_CNT / or
       /#define RIGID4SITES_TUPLE_CNT / or
       /const int MSD::ExternalToInternalSiteIdMapSize / or
       /const int MSD::InternalToExternalSiteIdMapSize / or
       /const int MSD::IrreduciblePartitionTypeListSize / or
       /const int MSD::IrreduciblePartitionCount / or
       /const int MSD::IrreduciblePartitionIndexToTypeMapSize / or
       /const int MSD::SiteInformationSize / or
       /const int MSD::SiteIDtoSiteTypeMapSize / or
       /const int MSD::ExclusionList1_2_3_4Size / or
       /const int MSD::Pair14SiteListSize / or
       /const int MSD::Charge14PairTableSize / or
       /const int MSD::LJ14PairTableSize / or
       /#define NUMBER_OF_StdHarmonicBondForce_sites_SITE_TUPLES / or
       /#define NUMBER_OF_StdHarmonicBondForce_params_PARAM_TUPLES / or
       /#define NUMBER_OF_StdHarmonicAngleForce_sites_SITE_TUPLES / or
       /#define NUMBER_OF_StdHarmonicAngleForce_params_PARAM_TUPLES / or
       /#define NUMBER_OF_UreyBradleyForce_sites_SITE_TUPLES / or
       /#define NUMBER_OF_UreyBradleyForce_params_PARAM_TUPLES / or
       /#define NUMBER_OF_SwopeTorsionForce_sites_SITE_TUPLES / or
       /#define NUMBER_OF_SwopeTorsionForce_params_PARAM_TUPLES / or
       /#define NUMBER_OF_ImproperDihedralForce_sites_SITE_TUPLES / or
       /#define NUMBER_OF_ImproperDihedralForce_params_PARAM_TUPLES / or
       /#define NUMBER_OF_OPLSTorsionForce_sites_SITE_TUPLES / or
       /#define NUMBER_OF_OPLSTorsionForce_params_PARAM_TUPLES / or
       /#define NUMBER_OF_OPLSImproperForce_sites_SITE_TUPLES / or
       /#define NUMBER_OF_OPLSImproperForce_params_PARAM_TUPLES / or
       /const int MSD::NumberOfNsqSiteTypes / or
       /const int MSD::NsqSiteIDtoSiteTypeMapSize / or
       /const int MSD::Water3sitesSize / or
       /#define EXCLUSION_LIST1_2_3_4 / or
       /#define NUMBER_OF_SITES / or
       /#define NUMBER_OF_FRAGMENTS / or
       /#define NUMBER_OF_NSQ_SITE_TYPES / or
       /#define PAIR14_SITE_LIST_SIZE / or
       /#define MAX_SITES_IN_FRAGMENT / or
       /#define MAX_FRAGID_DELTA_TO_HAVE_EXCLUSIONS /
     ) {
      $fileline = "";
    } #endif must remove single-line section

  ########################
  # Must handle special case of LJ14PairTable
  if (/Combiner::Combine<ParamType>\( MSD::LJ14PairTable\[ siteType1 \],  MSD::LJ14PairTable\[ siteType2 \], combinedParameterArray\[ i \] \)/)
    {
    $fileline = "// Removed Combiner function\n";
    $saw_combiner = 1;
    }
  if (/return\( CombinedParameterArray\[ i \] \);/ and ($saw_combiner == 1))
    {
    $fileline = "return (MSD::LJ14PairTable[i]);\n";
    $saw_combiner = 0;
    }

  
  ########################
  if ($do_output == 1)
    {
    print $fileline;
    }
  
  ########################
  if (/};(\s+?)$/ and ($do_output == 0))
    {
      $do_output = 1;
    }


  } # end while <MSDFILE>
close(MSDFILE);




