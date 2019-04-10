/* Copyright 2001, 2019 IBM Corporation
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
 * following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the 
 * following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the 
 * following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 // ***********************************************************************
// File: MSD.hpp
// Author: cwo 
// Date: August 9, 2006 
// Namespace: MSD 
// Description: Namespace that holds MSD DATA structure names that must
//              match between ECA generating tool and probspectrans
//        Note: This file wiped clean from old 'retired' version
//              (arayshu: 01/09/02)
// ***********************************************************************


#ifndef INCLUDE_MSD_HPP
#define INCLUDE_MSD_HPP

#define MSD_COMPILE_DATA_ONLY

#ifndef assert
#include <assert.h>
#endif

#include <BlueMatter/UDF_Binding.hpp>
#include <BlueMatter/MSD_Prefix.hpp>

class MSD
  {
  public:  
  //*********************************************
  // Global system-specific constants
  static const int SystemID;
  static const double TimeConversionFactor;

  //*********************************************
  // Site / Fragment information 

  static const SiteInfo SiteInformation[];
  static const int SiteInformationCount;

  static const int SiteIDtoSiteTypeMap[];
  static const int SiteIDtoSiteTypeMapCount;

  static const SiteId ExternalToInternalSiteIdMap[];
  static const SiteId InternalToExternalSiteIdMap[];
  static const int SiteCount;

  static const SiteId Water3sites[];
  static const int Water3sitesCount;
  
  static const PartitionType FragmentTypeList[];
  static const int FragmentTypeListCount;

  static const PartitionMap FragmentIndexToTypeMap[];
  static const int FragmentCount;

  static const int MaxSitesInFragment;
  static const int MaxFragIDDeltaToHaveExclusions;


  //*********************************************
  // NSQ Information 

  static const int NsqSiteIDtoSiteTypeMap[];
  static const int NsqSiteIDtoSiteTypeMapCount;

  static const UDF_Binding::NSQCoulomb_Params ChargeNsqParams[];
  static const UDF_Binding::NSQLennardJones_Params LJNsqParams[];
  static const int NumberOfNsqSiteTypes;

  static const SiteId ExclusionList1_2_3_4[];
  static const int ExclusionList1_2_3_4Count;

  static const SiteId Pair14SiteList[];
  static const UDF_Binding::LennardJonesForce_Params LJ14PairTable[];
  static const UDF_Binding::Coulomb14_Params Charge14PairTable[];
  static const int Pair14SiteListCount;


  //*********************************************
  // Bonded Parameters

  static const SiteId StdHarmonicBondForce_sitesSiteTupleList[];
  static const UDF_Binding::StdHarmonicBondForce_Params StdHarmonicBondForce_paramsParamTupleList[];
  static const int StdHarmonicBondForce_sitesSiteTupleListCount;

  static const SiteId StdHarmonicAngleForce_sitesSiteTupleList[];
  static const UDF_Binding::StdHarmonicAngleForce_Params StdHarmonicAngleForce_paramsParamTupleList[];
  static const int StdHarmonicAngleForce_sitesSiteTupleListCount;

  static const SiteId UreyBradleyForce_sitesSiteTupleList[];
  static const UDF_Binding::UreyBradleyForce_Params UreyBradleyForce_paramsParamTupleList[];
  static const int UreyBradleyForce_sitesSiteTupleListCount;

  static const SiteId SwopeTorsionForce_sitesSiteTupleList[];
  static const UDF_Binding::SwopeTorsionForce_Params SwopeTorsionForce_paramsParamTupleList[];
  static const int SwopeTorsionForce_sitesSiteTupleListCount;

  static const SiteId ImproperDihedralForce_sitesSiteTupleList[];
  static const UDF_Binding::ImproperDihedralForce_Params ImproperDihedralForce_paramsParamTupleList[];
  static const int ImproperDihedralForce_sitesSiteTupleListCount;

  static const SiteId OPLSTorsionForce_sitesSiteTupleList[];
  static const UDF_Binding::OPLSTorsionForce_Params OPLSTorsionForce_paramsParamTupleList[];
  static const int OPLSTorsionForce_sitesSiteTupleListCount;

  static const SiteId OPLSImproperForce_sitesSiteTupleList[];
  static const UDF_Binding::OPLSImproperForce_Params OPLSImproperForce_paramsParamTupleList[];
  static const int OPLSImproperForce_sitesSiteTupleListCount;


  //*********************************************
  // Shake/Rattle Parameters

  static const SiteId Rigid2Sites[];
  static const UDF_Binding::ShakeGroup2_Params Rigid2Params[];
  static const int Rigid2SitesCount;

  static const SiteId Rigid3Sites[];
  static const UDF_Binding::ShakeGroup3_Params Rigid3Params[];
  static const int Rigid3SitesCount;

  static const SiteId Rigid4Sites[];
  static const UDF_Binding::ShakeGroup4_Params Rigid4Params[];
  static const int Rigid4SitesCount;


  }; //end class MSD


#endif

