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
// File: probspectrans_to_MSD.cpp
// Author: cwo 
// Date: July 13, 2006
// Namespace: probspectrans 
// Class: ProbSpecTransToMSD
// Description: Reads DB entry from a run of probspectrans and outputs
//              strings with array in MSD file format
// ***********************************************************************

#include <BlueMatter/probspectrans/probspectrans_to_MSD.hpp>
#include <BlueMatter/probspectrans/transform_sysparams.hpp>

//////////////////////////////////////////
// Static globals for this file only

#define SQLCMD_BUFSZ 512
#define SQLVARCHAR_BUFSZ 255
#define STREAM_PRECISION 16

static int verbosity = 0;


namespace probspectrans
{
  
//****************************************************************************
// Constructors/Destructors
//****************************************************************************
#ifdef PROBSPECTRANSTOMSD_SINGLETON_CLASS
  ProbSpecTransToMSD *ProbSpecTransToMSD::obj_instance = NULL;
#endif

  ProbSpecTransToMSD::ProbSpecTransToMSD(
          std::string dbname, 
          int pst_id, int sys_id) 
    :  db2conn (db2::DBName::instance(dbname.c_str())->connection()),
       pst_id (pst_id), sys_id (sys_id)
    {
    } 
  ProbSpecTransToMSD::~ProbSpecTransToMSD()
    {
#ifdef PROBSPECTRANSTOMSD_SINGLETON_CLASS
    delete obj_instance;
    obj_instance = NULL;
#endif
    }

  ProbSpecTransToMSD *ProbSpecTransToMSD::instance(
          std::string dbname,
          int pst_id,
          int sys_id )
    { 
#ifdef PROBSPECTRANSTOMSD_SINGLETON_CLASS
    if (obj_instance == NULL) 
      {
      obj_instance = new ProbSpecTransToMSD(dbname, pst_id, sys_id);
      assert(obj_instance != NULL);
      }
    return (obj_instance);
#else
    return (new ProbSpecTransToMSD(dbname, pst_id, sys_id));
#endif
    }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProbSpecTransToMSD::output_rigidsites_tables(std::ostream& out)
  {
  std::cout << "   ...outputting Rigid Site Tables" << std::endl; //VERBOSE

  /////////////////////////////////////////////////
  // Check rigid2_sites
  std::stringstream rigid2_sites;  
  std::stringstream rigid2_params;  
  rigid2_params.precision(STREAM_PRECISION);
  int tuple2_cnt = 0;

  rigid2_sites << "const SiteId MSD::Rigid2Sites[] = {" << std::endl;
  rigid2_params << "const UDF_Binding::ShakeGroup2_Params MSD::Rigid2Params[] = {" << std::endl;

  db2::StmtHandle stmt1(this->db2conn);
  char sqlcmd1[SQLCMD_BUFSZ];
  snprintf(sqlcmd1, SQLCMD_BUFSZ, "select heavy_site, "
           " hydrogen_site1, hydrogen_site1_rEQ " 
           " from mdsystem.rigid2_sites "
           " where pst_id=%d ORDER by heavy_site, hydrogen_site1 "
           " FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare(sqlcmd1);

  int r2_heavy_site;
  int r2_hydrogen_site1;
  double r2_hydrogen_site1_rEQ;
  stmt1.bindCol(1, (SQLINTEGER&)r2_heavy_site);
  stmt1.bindCol(2, (SQLINTEGER&)r2_hydrogen_site1);
  stmt1.bindCol(3, (SQLDOUBLE&)r2_hydrogen_site1_rEQ);

  try { stmt1.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry2 = true; 
  while (stmt1.fetch())
    {
    tuple2_cnt++;
    if (!first_entry2) 
      { 
      rigid2_sites << ", "; 
      rigid2_params << ", "; 
      }
    if (first_entry2) { first_entry2 = false; }

    rigid2_sites << r2_heavy_site << ", " << r2_hydrogen_site1 << std::endl;;
    rigid2_params << std::scientific << "{{" << r2_hydrogen_site1_rEQ << "}}" << std::endl;;
    }
  if (first_entry2)
    {
    rigid2_sites << " -1 ";
    rigid2_params << "{{1.0}}";
    }
  rigid2_sites << "};" << std::endl;
  rigid2_params << "};" << std::endl;

  out << rigid2_sites.str();
  out << rigid2_params.str();
#ifdef TEST_PROBSPECDB2
  out << "#define RIGID2SITES_TUPLE_CNT " << tuple2_cnt << std::endl;
#else
  out << "const int MSD::Rigid2SitesCount = " << tuple2_cnt << ";" << std::endl;
#endif

  /////////////////////////////////////////////////
  // Check rigid3_sites
  std::stringstream rigid3_sites;  
  std::stringstream rigid3_params;  
  rigid3_params.precision(STREAM_PRECISION);
  int tuple3_cnt = 0;

  rigid3_sites << "const SiteId MSD::Rigid3Sites[] = {" << std::endl;
  rigid3_params << "const UDF_Binding::ShakeGroup3_Params MSD::Rigid3Params[] = {" << std::endl;

  db2::StmtHandle stmt2(this->db2conn);
  char sqlcmd2[SQLCMD_BUFSZ];
  snprintf(sqlcmd2, SQLCMD_BUFSZ, "select heavy_site, "
           " hydrogen_site1, hydrogen_site1_rEQ, " 
           " hydrogen_site2, hydrogen_site2_rEQ " 
           " from mdsystem.rigid3_sites "
           " where pst_id=%d ORDER by heavy_site, "
           " hydrogen_site1, hydrogen_site2 "
           " FOR READ ONLY WITH UR", pst_id);
  stmt2.prepare(sqlcmd2);

  int r3_heavy_site;
  int r3_hydrogen_site1;
  double r3_hydrogen_site1_rEQ;
  int r3_hydrogen_site2;
  double r3_hydrogen_site2_rEQ;
  stmt2.bindCol(1, (SQLINTEGER&)r3_heavy_site);
  stmt2.bindCol(2, (SQLINTEGER&)r3_hydrogen_site1);
  stmt2.bindCol(3, (SQLDOUBLE&)r3_hydrogen_site1_rEQ);
  stmt2.bindCol(4, (SQLINTEGER&)r3_hydrogen_site2);
  stmt2.bindCol(5, (SQLDOUBLE&)r3_hydrogen_site2_rEQ);

  try { stmt2.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry3 = true; 
  while (stmt2.fetch())
    {
    tuple3_cnt++;
    if (!first_entry3) 
      { 
      rigid3_sites << ", "; 
      rigid3_params << ", "; 
      }
    if (first_entry3) { first_entry3 = false; }

    rigid3_sites << r3_heavy_site << ", " << r3_hydrogen_site1 << ", "
                 << r3_hydrogen_site2 << std::endl;;
    rigid3_params << std::scientific << "{{" << r3_hydrogen_site1_rEQ << ", "
                  << r3_hydrogen_site2_rEQ << "}}" << std::endl;;
    }
  if (first_entry3)
    {
    rigid3_sites << " -1 ";
    rigid3_params << "{{1.0}}";
    }
  rigid3_sites << "};" << std::endl;
  rigid3_params << "};" << std::endl;

  out << rigid3_sites.str();
  out << rigid3_params.str();
#ifdef TEST_PROBSPECDB2
  out << "#define RIGID3SITES_TUPLE_CNT " << tuple3_cnt << std::endl;
#else
  out << "const int MSD::Rigid3SitesCount = " << tuple3_cnt << ";" << std::endl;
#endif


  /////////////////////////////////////////////////
  // Check rigid4_sites
  std::stringstream rigid4_sites;  
  std::stringstream rigid4_params;  
  rigid4_params.precision(STREAM_PRECISION);
  int tuple4_cnt = 0;

  rigid4_sites << "const SiteId MSD::Rigid4Sites[] = {" << std::endl;
  rigid4_params << "const UDF_Binding::ShakeGroup4_Params MSD::Rigid4Params[] = {" << std::endl;

  db2::StmtHandle stmt3(this->db2conn);
  char sqlcmd3[SQLCMD_BUFSZ];
  snprintf(sqlcmd3, SQLCMD_BUFSZ, "select heavy_site, "
           " hydrogen_site1, hydrogen_site1_rEQ, " 
           " hydrogen_site2, hydrogen_site2_rEQ, " 
           " hydrogen_site3, hydrogen_site3_rEQ " 
           " from mdsystem.rigid4_sites "
           " where pst_id=%d ORDER by heavy_site, "
           " hydrogen_site1, hydrogen_site2, hydrogen_site3 "
           " FOR READ ONLY WITH UR", pst_id);
  stmt3.prepare(sqlcmd3);

  int r4_heavy_site;
  int r4_hydrogen_site1;
  double r4_hydrogen_site1_rEQ;
  int r4_hydrogen_site2;
  double r4_hydrogen_site2_rEQ;
  int r4_hydrogen_site3;
  double r4_hydrogen_site3_rEQ;
  stmt3.bindCol(1, (SQLINTEGER&)r4_heavy_site);
  stmt3.bindCol(2, (SQLINTEGER&)r4_hydrogen_site1);
  stmt3.bindCol(3, (SQLDOUBLE&)r4_hydrogen_site1_rEQ);
  stmt3.bindCol(4, (SQLINTEGER&)r4_hydrogen_site2);
  stmt3.bindCol(5, (SQLDOUBLE&)r4_hydrogen_site2_rEQ);
  stmt3.bindCol(6, (SQLINTEGER&)r4_hydrogen_site3);
  stmt3.bindCol(7, (SQLDOUBLE&)r4_hydrogen_site3_rEQ);

  try { stmt3.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry4 = true; 
  while (stmt3.fetch())
    {
    tuple4_cnt++;
    if (!first_entry4) 
      { 
      rigid4_sites << ", "; 
      rigid4_params << ", "; 
      }
    if (first_entry4) { first_entry4 = false; }

    rigid4_sites << r4_heavy_site << ", " << r4_hydrogen_site1 << ", "
                 << r4_hydrogen_site2 << ", " << r4_hydrogen_site3 << std::endl;;
    rigid4_params << std::scientific << "{{" << r4_hydrogen_site1_rEQ << ", "
                  << r4_hydrogen_site2_rEQ << ", " << r4_hydrogen_site3_rEQ << "}}" << std::endl;;
    }
  if (first_entry4)
    {
    rigid4_sites << " -1 ";
    rigid4_params << "{{1.0}}";
    }
  rigid4_sites << "};" << std::endl;
  rigid4_params << "};" << std::endl;

  out << rigid4_sites.str();
  out << rigid4_params.str();
#ifdef TEST_PROBSPECDB2
  out << "#define RIGID4SITES_TUPLE_CNT " << tuple4_cnt << std::endl;
#else
  out << "const int MSD::Rigid4SitesCount = " << tuple4_cnt << ";" << std::endl;
#endif

  } //end output_rigidsites_tables()

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProbSpecTransToMSD::output_siteid_mapping_tables(std::ostream& out)
  {
  std::cout << "   ...outputting Site ID Map" << std::endl; //VERBOSE
  
  /////////////////////////////////////
  // Get number of sites
  db2::StmtHandle stmt1(this->db2conn);
  char sqlcmd1[SQLCMD_BUFSZ];
  snprintf(sqlcmd1, SQLCMD_BUFSZ, "select count(*) "
           " from mdsystem.internal_external_siteids "
           " where pst_id=%d FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare(sqlcmd1);

  int num_sites = 1;
  stmt1.bindCol(1, (SQLINTEGER&)num_sites);

  try { stmt1.execute(); stmt1.fetch(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  /////////////////////////////////////
  // Get external/internal mappings

  int ext_int_arr[num_sites];
  int int_ext_arr[num_sites];

  db2::StmtHandle stmt2(this->db2conn);
  char sqlcmd2[SQLCMD_BUFSZ];
  snprintf(sqlcmd2, SQLCMD_BUFSZ, "select ext_site_id, int_site_id "
           " from mdsystem.internal_external_siteids "
           " where pst_id=%d FOR READ ONLY WITH UR", pst_id);
  stmt2.prepare(sqlcmd2);

  int ext_site_id;
  int int_site_id;
  stmt2.bindCol(1, (SQLINTEGER&)ext_site_id);
  stmt2.bindCol(2, (SQLINTEGER&)int_site_id);

  try { stmt2.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  while (stmt2.fetch())
    {
    ext_int_arr[ext_site_id] = int_site_id;
    int_ext_arr[int_site_id] = ext_site_id;
    }
 
  /////////////////////////////////////
  // Write mappings to strings 

  std::stringstream ext_int_sites;  
  std::stringstream int_ext_sites;  

  ext_int_sites << "const SiteId MSD::ExternalToInternalSiteIdMap[] = {" << std::endl;
  int_ext_sites << "const SiteId MSD::InternalToExternalSiteIdMap[] = {" << std::endl;

  bool first_entry = true;
  for (int k=0; k < num_sites; ++k)
    {
    if (!first_entry) 
      { 
      ext_int_sites << ", " << std::endl; 
      int_ext_sites << ", " << std::endl; 
      }
    if (first_entry) { first_entry = false; }

    ext_int_sites << ext_int_arr[k];
    int_ext_sites << int_ext_arr[k];
    }
  if (first_entry)
    {
    ext_int_sites << "-1";
    int_ext_sites << "-1";
    }
  ext_int_sites << "};" << std::endl;
  int_ext_sites << "};" << std::endl;

  out << ext_int_sites.str();
  out << int_ext_sites.str();
#ifdef TEST_PROBSPECDB2
  out << "const int MSD::ExternalToInternalSiteIdMapSize = " 
      << num_sites << ";" << std::endl;
  out << "const int MSD::InternalToExternalSiteIdMapSize = " 
      << num_sites << ";" << std::endl;
  out << "#endif" << std::endl;
  out << "#define NUMBER_OF_SITES "
      << num_sites << std::endl;
  out << "#ifndef MSD_COMPILE_CODE_ONLY" << std::endl;
#else
  out << "const int MSD::SiteCount = " 
      << num_sites << ";" << std::endl;
#endif


  } //end output_siteid_mapping_tables()

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProbSpecTransToMSD::output_fragmentation_tables(std::ostream& out)
  {
  std::cout << "   ...outputting Fragmentation Tables" << std::endl; //VERBOSE

  /////////////////////////////////////////////////
  // Output Fragment TYPES
  std::stringstream frag_types;  
  frag_types.precision(STREAM_PRECISION);
  int num_types = 0;

#ifdef TEST_PROBSPECDB2
  frag_types << "const IrreduciblePartitionType MSD::IrreduciblePartitionTypeList[] = {" << std::endl;
#else
  frag_types << "const FragmentType MSD::FragmentTypeList[] = {" << std::endl;
#endif

  db2::StmtHandle stmt1(this->db2conn);
  char sqlcmd1[SQLCMD_BUFSZ];
  snprintf(sqlcmd1, SQLCMD_BUFSZ, "select "
           " num_sites, molecule_type, tagatom_index, "
           " num_constraints, extent "
           " from mdsystem.fragment_type "
           " where pst_id=%d ORDER by fragment_type_id "
           " FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare(sqlcmd1);

  int num_sites;
  int molecule_type;
  int tagatom_index;
  int num_constraints;
  double extent;
  stmt1.bindCol(1, (SQLINTEGER&)num_sites);
  stmt1.bindCol(2, (SQLINTEGER&)molecule_type);
  stmt1.bindCol(3, (SQLINTEGER&)tagatom_index);
  stmt1.bindCol(4, (SQLINTEGER&)num_constraints);
  stmt1.bindCol(5, (SQLDOUBLE&)extent);

  try { stmt1.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry1 = true; 
  while (stmt1.fetch())
    {
    num_types++;
    if (!first_entry1) 
      { 
      frag_types << ", "; 
      }
    if (first_entry1) { first_entry1 = false; }

    frag_types << "{" << num_sites << ", " << molecule_type << ", " 
               << tagatom_index << ", " << num_constraints << ", ";
    frag_types << std::scientific << extent << "}" << std::endl;
    }
  if (first_entry1)
    {
    frag_types << "{ -1, -1, -1, -1, -1 }";
    }
  frag_types << "};" << std::endl;

  out << frag_types.str();
#ifdef TEST_PROBSPECDB2
  out << "const int MSD::IrreduciblePartitionTypeListSize = " 
      << num_types << ";" << std::endl;
#else
  out << "const int MSD::FragmentTypeListCount = " 
      << num_types << ";" << std::endl;
#endif


  /////////////////////////////////////////////////
  // Output SiteID -> Fragment Map 
  std::stringstream frag_to_type;  
  frag_to_type.precision(STREAM_PRECISION);
  int num_frags = 0;

#ifdef TEST_PROBSPECDB2
  frag_to_type << "const IrreduciblePartitionMap MSD::IrreduciblePartitionIndexToTypeMap[] = {" << std::endl;
#else
  frag_to_type << "const FragmentMapType MSD::FragmentIndexToTypeMap[] = {" << std::endl;
#endif

  db2::StmtHandle stmt2(this->db2conn);
  char sqlcmd2[SQLCMD_BUFSZ];
  snprintf(sqlcmd2, SQLCMD_BUFSZ, "select "
           " start_site, fragment_type_id, molecule_id, "
           " frag_index_in_molecule "
           " from mdsystem.fragment_map "
           " where pst_id=%d ORDER by start_site"
           " FOR READ ONLY WITH UR", pst_id);
  stmt2.prepare(sqlcmd2);

  int start_site;
  int fragment_type_id;
  int molecule_id;
  int frag_index_in_molecule;
  stmt2.bindCol(1, (SQLINTEGER&)start_site);
  stmt2.bindCol(2, (SQLINTEGER&)fragment_type_id);
  stmt2.bindCol(3, (SQLINTEGER&)molecule_id);
  stmt2.bindCol(4, (SQLINTEGER&)frag_index_in_molecule);

  try { stmt2.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry2 = true; 
  while (stmt2.fetch())
    {
    num_frags++;
    if (!first_entry2) 
      { 
      frag_to_type << ", "; 
      }
    if (first_entry2) { first_entry2 = false; }

    frag_to_type << "{" << start_site << ", " << fragment_type_id << ", " 
               << molecule_id << ", " << frag_index_in_molecule << "}" << std::endl;
    }
  if (first_entry2)
    {
    frag_to_type << "{ -1, -1, -1, -1 }";
    }
  frag_to_type << "};" << std::endl;

  out << frag_to_type.str();
#ifdef TEST_PROBSPECDB2
  out << "const int MSD::IrreduciblePartitionCount = " 
      << num_frags << ";" << std::endl;
  out << "const int MSD::IrreduciblePartitionIndexToTypeMapSize = " 
      << num_frags << ";" << std::endl;
  out << "#endif" << std::endl;
  out << "#define NUMBER_OF_FRAGMENTS "
      << num_frags << std::endl;
  out << "#ifndef MSD_COMPILE_CODE_ONLY" << std::endl;
#else
  out << "const int MSD::FragmentCount = " 
      << num_frags << ";" << std::endl;
#endif


  /////////////////////////////////////////////////
  // Determine the maximum number of sites in any fragment 

  db2::StmtHandle stmt3(this->db2conn);
  char sqlcmd3[SQLCMD_BUFSZ];
  snprintf(sqlcmd3, SQLCMD_BUFSZ, "select max(num_sites) "
           " from mdsystem.fragment_type"
           " where pst_id=%d "
           " FOR READ ONLY WITH UR", pst_id);
  stmt3.prepare(sqlcmd3);

  int max_num_sites = -1;
  stmt3.bindCol(1, (SQLINTEGER&)max_num_sites);

  try { stmt3.execute(); stmt3.fetch(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

#ifdef TEST_PROBSPECDB2
  out << "#endif" << std::endl;
  out << "#define MAX_SITES_IN_FRAGMENT "
      << max_num_sites << std::endl;
  out << "#ifndef MSD_COMPILE_CODE_ONLY" << std::endl;
#else
  out << "const int MSD::MaxSitesInFragment = "
      << max_num_sites << ";" << std::endl;
#endif

  db2::StmtHandle stmt4(this->db2conn);
  char sqlcmd4[SQLCMD_BUFSZ];
  snprintf(sqlcmd4, SQLCMD_BUFSZ, "select fragid_exclusions_delta "
           " from mdsystem.online_parameter "
           " where pst_id=%d "
           " FOR READ ONLY WITH UR", pst_id);
  stmt4.prepare(sqlcmd4);

  int fragid_exclusions_delta = -1;
  stmt4.bindCol(1, (SQLINTEGER&)fragid_exclusions_delta);

  try { stmt4.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  stmt4.fetch();

#ifdef TEST_PROBSPECDB2
  out << "#endif" << std::endl;
  out << "#define MAX_FRAGID_DELTA_TO_HAVE_EXCLUSIONS "
      << fragid_exclusions_delta << std::endl;
  out << "#ifndef MSD_COMPILE_CODE_ONLY" << std::endl;
#else
  out << "const int MSD::MaxFragIDDeltaToHaveExclusions = "
      << fragid_exclusions_delta << ";" << std::endl;
#endif

  } //end output_fragmentation_tables()

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProbSpecTransToMSD::output_siteinfo_tables(std::ostream& out)
  {
  std::cout << "   ...outputting Site Info Tables" << std::endl; //VERBOSE

  /////////////////////////////////////////////////
  // Output Site TYPES
  std::stringstream site_types;  
  site_types.precision(STREAM_PRECISION);
  int num_types = 0;

  site_types << "const SiteInfo MSD::SiteInformation[] = {" << std::endl;

  db2::StmtHandle stmt1(this->db2conn);
  char sqlcmd1[SQLCMD_BUFSZ];
  snprintf(sqlcmd1, SQLCMD_BUFSZ, "select "
           " atomic_mass, half_inverse_mass, charge "
           " from mdsystem.siteinfo "
           " where pst_id=%d ORDER by info_id "
           " FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare(sqlcmd1);

  double atomic_mass;
  double half_inverse_mass;
  double charge;
  stmt1.bindCol(1, (SQLDOUBLE&)atomic_mass);
  stmt1.bindCol(2, (SQLDOUBLE&)half_inverse_mass);
  stmt1.bindCol(3, (SQLDOUBLE&)charge);

  try { stmt1.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry1 = true; 
  while (stmt1.fetch())
    {
    num_types++;
    if (!first_entry1) 
      { 
      site_types << ", "; 
      }
    if (first_entry1) { first_entry1 = false; }

    site_types << std::scientific << "{" << atomic_mass << ", "
               << half_inverse_mass << ", " << charge << "}" << std::endl;
    }
  if (first_entry1)
    {
    site_types << "{ -1, -1, -1 }";
    }
  site_types << "};" << std::endl;

  out << site_types.str();
#ifdef TEST_PROBSPECDB2
  out << "const int MSD::SiteInformationSize = " << num_types << ";" << std::endl;
#else
  out << "const int MSD::SiteInformationCount = " << num_types << ";" << std::endl;
#endif



  /////////////////////////////////////////////////
  // Output SiteID -> SITE TYPE
  std::stringstream site_to_type;  
  site_to_type.precision(STREAM_PRECISION);
  int num_sites = 0;

  site_to_type << "const int MSD::SiteIDtoSiteTypeMap[] = {" << std::endl;

  db2::StmtHandle stmt2(this->db2conn);
  char sqlcmd2[SQLCMD_BUFSZ];
  snprintf(sqlcmd2, SQLCMD_BUFSZ, "select info_id "
           " from mdsystem.site_to_siteinfo"
           " where pst_id=%d ORDER by site_id"
           " FOR READ ONLY WITH UR", pst_id);
  stmt2.prepare(sqlcmd2);

  int info_id;
  stmt2.bindCol(1, (SQLINTEGER&)info_id);

  try { stmt2.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry2 = true; 
  while (stmt2.fetch())
    {
    num_sites++;
    if (!first_entry2) 
      { 
      site_to_type << ", "; 
      }
    if (first_entry2) { first_entry2 = false; }

    site_to_type << info_id << std::endl;
    }
  if (first_entry2)
    {
    site_to_type << "-1";
    }
  site_to_type << "};" << std::endl;

  out << site_to_type.str();
#ifdef TEST_PROBSPECDB2
  out << "const int MSD::SiteIDtoSiteTypeMapSize = "
      << num_sites << ";" << std::endl;
#else
  out << "const int MSD::SiteIDtoSiteTypeMapCount = "
      << num_sites << ";" << std::endl;
#endif


  } //end output_siteinfo_tables()

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProbSpecTransToMSD::output_excludedsites_tables(std::ostream& out)
  {
  std::cout << "   ...outputting Exclusion List" << std::endl; //VERBOSE

  /////////////////////////////////////////////////
  // Output Excluded_1_2_3_4 list 
  std::stringstream excluded_sites;  
  int num_excl_sites = 0;

  excluded_sites << "const SiteId MSD::ExclusionList1_2_3_4[] = {" << std::endl;

  db2::StmtHandle stmt1(this->db2conn);
  char sqlcmd1[SQLCMD_BUFSZ];
  snprintf(sqlcmd1, SQLCMD_BUFSZ, "select "
           " site1, site2 "
           " from mdsystem.exclusion_sites "
           " where pst_id=%d ORDER by site1 "
           " FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare(sqlcmd1);

  int site_id1;
  int site_id2;
  stmt1.bindCol(1, (SQLINTEGER&)site_id1);
  stmt1.bindCol(2, (SQLINTEGER&)site_id2);

  try { stmt1.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry1 = true; 
  while (stmt1.fetch())
    {
    num_excl_sites++;
    if (!first_entry1) 
      { 
      excluded_sites << ", "; 
      }
    if (first_entry1) { first_entry1 = false; }

    excluded_sites << site_id1 << ", " << site_id2 << std::endl;
    }
  if (first_entry1)
    {
    excluded_sites << "-1";
    }
  excluded_sites << "};" << std::endl;

  out << excluded_sites.str();
#ifdef TEST_PROBSPECDB2
  out << "const int MSD::ExclusionList1_2_3_4Size = "
      << num_excl_sites << ";" << std::endl;
  out << "#endif" << std::endl;
  out << "#define EXCLUSION_LIST1_2_3_4 "
      << num_excl_sites << std::endl;
  out << "#ifndef MSD_COMPILE_CODE_ONLY" << std::endl;
#else
  out << "const int MSD::ExclusionList1_2_3_4Count = "
      << num_excl_sites << ";" << std::endl;
#endif

  } //end output_excludedsites_tables()


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProbSpecTransToMSD::output_pair14sites_tables(std::ostream& out)
  {
  std::cout << "   ...outputting Pair14 List" << std::endl; //VERBOSE

  /////////////////////////////////////////////////
  // Output Pair14SiteList and Charge14PairTable
  // 
  // We must handle LJ14PairTable separately/specially
  //
  // This is because the MSD looks at the SitetoSiteIDType map 
  // to determine the LJParams TYPE of a pair14 siteID 
  //
  // This information is not available in the DB tables that
  // probspectrans fills out (b/c the tables are each given their
  // own logical distinction, AND the 'Combiner' is already run,
  // combining the LJ parameters from 2 sites)
  //
  // So, we'll just output a 'new' array that has the combined lj values
  // of the 2 sites in each pair14 entry

  std::stringstream pair14_sites;  
  std::stringstream charge_params;  
  std::stringstream lj_params;  
  charge_params.precision(STREAM_PRECISION);
  lj_params.precision(STREAM_PRECISION);
  int num_pair14_sites = 0;

  pair14_sites << "const SiteId MSD::Pair14SiteList[] = {" << std::endl;
  charge_params << "const UDF_Binding::Coulomb14_Params MSD::Charge14PairTable[] = {" << std::endl;
  lj_params << "const UDF_Binding::LennardJonesForce_Params MSD::LJ14PairTable[] = {" << std::endl;

  db2::StmtHandle stmt1(this->db2conn);
  char sqlcmd1[SQLCMD_BUFSZ];
  snprintf(sqlcmd1, SQLCMD_BUFSZ, "select "
           " site1, site2, charge, lj_epsilon, lj_rmin "
           " from mdsystem.pair14_sites "
           " where pst_id=%d ORDER by site1 "
           " FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare(sqlcmd1);

  int site1;
  int site2;
  double charge;
  double lj_epsilon;
  double lj_rmin;
  stmt1.bindCol(1, (SQLINTEGER&)site1);
  stmt1.bindCol(2, (SQLINTEGER&)site2);
  stmt1.bindCol(3, (SQLDOUBLE&)charge);
  stmt1.bindCol(4, (SQLDOUBLE&)lj_epsilon);
  stmt1.bindCol(5, (SQLDOUBLE&)lj_rmin);

  try { stmt1.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry1 = true; 
  while (stmt1.fetch())
    {
    num_pair14_sites++;
    if (!first_entry1) 
      { 
      pair14_sites << ", "; 
      charge_params << ", ";
      lj_params << ", ";
      }
    if (first_entry1) { first_entry1 = false; }

    pair14_sites << site1 << ", " << site2 << std::endl;
    charge_params << std::scientific << "{" << charge << "}" << std::endl;
    lj_params << std::scientific << "{" << lj_epsilon << ", "
              << lj_rmin << "}" << std::endl;
    }

  if (first_entry1)
    {
    pair14_sites << "-1";
    charge_params << "{ -1 }";
    lj_params << "{ -1, -1 }";
    }
  pair14_sites << "};" << std::endl;
  charge_params << "};" << std::endl;
  lj_params << "};" << std::endl;

  out << pair14_sites.str();
  out << charge_params.str();
  out << lj_params.str();
#ifdef TEST_PROBSPECDB2
  out << "const int MSD::Pair14SiteListSize = "
      << num_pair14_sites << ";" << std::endl;
  out << "const int MSD::Charge14PairTableSize = "
      << num_pair14_sites << ";" << std::endl;
  out << "const int MSD::LJ14PairTableSize = "
      << num_pair14_sites << ";" << std::endl;
  out << "#endif" << std::endl;
  out << "#define PAIR14_SITE_LIST_SIZE "
      << num_pair14_sites << std::endl;
  out << "#ifndef MSD_COMPILE_CODE_ONLY" << std::endl;
#else
  out << "const int MSD::Pair14SiteListCount = "
      << num_pair14_sites << ";" << std::endl;
#endif

  } //end output_pair14sites_tables()


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProbSpecTransToMSD::output_harmbondedsites_tables(std::ostream& out)
  {
  std::cout << "   ...outputting StdHarmonicBond Parameters" << std::endl; //VERBOSE

  std::stringstream tuple_sites;  
  std::stringstream tuple_params;  
  tuple_params.precision(STREAM_PRECISION);
  int num_tuples = 0;

  tuple_sites << "const SiteId MSD::StdHarmonicBondForce_sitesSiteTupleList[] = {" << std::endl;
  tuple_params << "const UDF_Binding::StdHarmonicBondForce_Params MSD::StdHarmonicBondForce_paramsParamTupleList[] = {" << std::endl;

  db2::StmtHandle stmt1(this->db2conn);
  char sqlcmd1[SQLCMD_BUFSZ];
  snprintf(sqlcmd1, SQLCMD_BUFSZ, "select "
           " site1, site2, k, r0 "
           " from mdsystem.stdharmonicbondforce_tuplelist"
           " where pst_id=%d ORDER by tuple_id "
           " FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare(sqlcmd1);
 
  int site1;
  int site2;
  double k;
  double r0;
  stmt1.bindCol(1, (SQLINTEGER&)site1);
  stmt1.bindCol(2, (SQLINTEGER&)site2);
  stmt1.bindCol(3, (SQLDOUBLE&)k);
  stmt1.bindCol(4, (SQLDOUBLE&)r0);

  try { stmt1.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry = true; 
  while (stmt1.fetch())
    {
    num_tuples++;
    if (!first_entry) 
      { 
      tuple_sites << ", "; 
      tuple_params << ", "; 
      }
    if (first_entry) { first_entry = false; }

    tuple_sites << site1 << ", " << site2 << std::endl;
    tuple_params << std::scientific << "{" << k << ", " << r0 << "}" << std::endl;
    }
  if (first_entry)
    {
    tuple_sites << "-1";
    tuple_params << "{ -1, -1 }";
    }
  tuple_sites << "};" << std::endl;
  tuple_params << "};" << std::endl;

  out << tuple_sites.str();
  out << tuple_params.str();
#ifdef TEST_PROBSPECDB2
  out << "#define NUMBER_OF_StdHarmonicBondForce_sites_SITE_TUPLES "
      << num_tuples << ";" << std::endl;
  out << "#define NUMBER_OF_StdHarmonicBondForce_params_PARAM_TUPLES "
      << num_tuples << ";" << std::endl;
#else
  out << "const int MSD::StdHarmonicBondForce_sitesSiteTupleListCount = "
      << num_tuples << ";" << std::endl;
#endif

  } //end output_harmbondedsites_tables()

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProbSpecTransToMSD::output_harmanglesites_tables(std::ostream& out)
  {
  std::cout << "   ...outputting StdHarmonicAngle Parameters" << std::endl; //VERBOSE

  std::stringstream tuple_sites;  
  std::stringstream tuple_params;  
  tuple_params.precision(STREAM_PRECISION);
  int num_tuples = 0;

  tuple_sites << "const SiteId MSD::StdHarmonicAngleForce_sitesSiteTupleList[] = {" << std::endl;
  tuple_params << "const UDF_Binding::StdHarmonicAngleForce_Params MSD::StdHarmonicAngleForce_paramsParamTupleList[] = {" << std::endl;

  db2::StmtHandle stmt1(this->db2conn);
  char sqlcmd1[SQLCMD_BUFSZ];
  snprintf(sqlcmd1, SQLCMD_BUFSZ, "select "
           " site1, site2, site3, k, th0 "
           " from mdsystem.stdharmonicangleforce_tuplelist"
           " where pst_id=%d ORDER by tuple_id "
           " FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare(sqlcmd1);

  int site1;
  int site2;
  int site3;
  double k;
  double th0;
  stmt1.bindCol(1, (SQLINTEGER&)site1);
  stmt1.bindCol(2, (SQLINTEGER&)site2);
  stmt1.bindCol(3, (SQLINTEGER&)site3);
  stmt1.bindCol(4, (SQLDOUBLE&)k);
  stmt1.bindCol(5, (SQLDOUBLE&)th0);

  try { stmt1.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry = true; 
  while (stmt1.fetch())
    {
    num_tuples++;
    if (!first_entry) 
      { 
      tuple_sites << ", "; 
      tuple_params << ", "; 
      }
    if (first_entry) { first_entry = false; }

    tuple_sites << site1 << ", " << site2 << ", " << site3 << std::endl;
    tuple_params << std::scientific << "{" << k << ", " << th0 << "}" << std::endl;
    }
  if (first_entry)
    {
    tuple_sites << "-1";
    tuple_params << "{ -1, -1 }";
    }
  tuple_sites << "};" << std::endl;
  tuple_params << "};" << std::endl;

  out << tuple_sites.str();
  out << tuple_params.str();
#ifdef TEST_PROBSPECDB2
  out << "#define NUMBER_OF_StdHarmonicAngleForce_sites_SITE_TUPLES "
      << num_tuples << ";" << std::endl;
  out << "#define NUMBER_OF_StdHarmonicAngleForce_params_PARAM_TUPLES "
      << num_tuples << ";" << std::endl;
#else
  out << "const int MSD::StdHarmonicAngleForce_sitesSiteTupleListCount = "
      << num_tuples << ";" << std::endl;
#endif

  } //end output_harmanglesites_tables()


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProbSpecTransToMSD::output_ureybradleysites_tables(std::ostream& out)
  {
  std::cout << "   ...outputting UreyBradley Parameters" << std::endl; //VERBOSE

  std::stringstream tuple_sites;  
  std::stringstream tuple_params;  
  tuple_params.precision(STREAM_PRECISION);
  int num_tuples = 0;

  tuple_sites << "const SiteId MSD::UreyBradleyForce_sitesSiteTupleList[] = {" << std::endl;
  tuple_params << "const UDF_Binding::UreyBradleyForce_Params MSD::UreyBradleyForce_paramsParamTupleList[] = {" << std::endl;
  
  db2::StmtHandle stmt1(this->db2conn);
  char sqlcmd1[SQLCMD_BUFSZ];
  snprintf(sqlcmd1, SQLCMD_BUFSZ, "select "
           " site1, site2, k, s0 "
           " from mdsystem.ureybradleyforce_tuplelist"
           " where pst_id=%d ORDER by tuple_id "
           " FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare(sqlcmd1);

  int site1;
  int site2;
  double k;
  double s0;
  stmt1.bindCol(1, (SQLINTEGER&)site1);
  stmt1.bindCol(2, (SQLINTEGER&)site2);
  stmt1.bindCol(3, (SQLDOUBLE&)k);
  stmt1.bindCol(4, (SQLDOUBLE&)s0);

  try { stmt1.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry = true; 
  while (stmt1.fetch())
    {
    num_tuples++;
    if (!first_entry) 
      { 
      tuple_sites << ", "; 
      tuple_params << ", "; 
      }
    if (first_entry) { first_entry = false; }

    tuple_sites << site1 << ", " << site2 << std::endl;
    tuple_params << std::scientific << "{" << k << ", " << s0 << "}" << std::endl;
    }
  if (first_entry)
    {
    tuple_sites << "-1";
    tuple_params << "{ -1, -1 }";
    }
  tuple_sites << "};" << std::endl;
  tuple_params << "};" << std::endl;

  out << tuple_sites.str();
  out << tuple_params.str();
#ifdef TEST_PROBSPECDB2
  out << "#define NUMBER_OF_UreyBradleyForce_sites_SITE_TUPLES "
      << num_tuples << ";" << std::endl;
  out << "#define NUMBER_OF_UreyBradleyForce_params_PARAM_TUPLES "
      << num_tuples << ";" << std::endl;
#else
  out << "const int MSD::UreyBradleyForce_sitesSiteTupleListCount = "
      << num_tuples << ";" << std::endl;
#endif

  } //end output_ureybradleysites_tables()


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProbSpecTransToMSD::output_swopetorsionsites_tables(std::ostream& out)
  {
  std::cout << "   ...outputting SwopeTorsion Parameters" << std::endl; //VERBOSE

  std::stringstream tuple_sites;  
  std::stringstream tuple_params;  
  tuple_params.precision(STREAM_PRECISION);
  int num_tuples = 0;

  tuple_sites << "const SiteId MSD::SwopeTorsionForce_sitesSiteTupleList[] = {" << std::endl;
  tuple_params << "const UDF_Binding::SwopeTorsionForce_Params MSD::SwopeTorsionForce_paramsParamTupleList[] = {" << std::endl;
  
  db2::StmtHandle stmt1(this->db2conn);
  char sqlcmd1[SQLCMD_BUFSZ];
  snprintf(sqlcmd1, SQLCMD_BUFSZ, "select "
           " site1, site2, site3, site4, "
           " cosDelta, k, sinDelta, n "
           " from mdsystem.swopetorsionforce_tuplelist"
           " where pst_id=%d ORDER by tuple_id "
           " FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare(sqlcmd1);

  int site1;
  int site2;
  int site3;
  int site4;
  double cosDelta;
  double k;
  double sinDelta;
  int n;
  stmt1.bindCol(1, (SQLINTEGER&)site1);
  stmt1.bindCol(2, (SQLINTEGER&)site2);
  stmt1.bindCol(3, (SQLINTEGER&)site3);
  stmt1.bindCol(4, (SQLINTEGER&)site4);
  stmt1.bindCol(5, (SQLDOUBLE&)cosDelta);
  stmt1.bindCol(6, (SQLDOUBLE&)k);
  stmt1.bindCol(7, (SQLDOUBLE&)sinDelta);
  stmt1.bindCol(8, (SQLINTEGER&)n);

  try { stmt1.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry = true; 
  while (stmt1.fetch())
    {
    num_tuples++;
    if (!first_entry) 
      { 
      tuple_sites << ", "; 
      tuple_params << ", "; 
      }
    if (first_entry) { first_entry = false; }

    tuple_sites << site1 << ", " << site2 << ", " 
                << site3 << ", " << site4 << std::endl;
    tuple_params << std::scientific << "{" << cosDelta << ", " << k << ", "
                 << sinDelta << ", " << std::fixed << n << "}" << std::endl;
    }
  if (first_entry)
    {
    tuple_sites << "-1";
    tuple_params << "{ -1, -1, -1, -1 }";
    }
  tuple_sites << "};" << std::endl;
  tuple_params << "};" << std::endl;

  out << tuple_sites.str();
  out << tuple_params.str();
#ifdef TEST_PROBSPECDB2
  out << "#define NUMBER_OF_SwopeTorsionForce_sites_SITE_TUPLES "
      << num_tuples << ";" << std::endl;
  out << "#define NUMBER_OF_SwopeTorsionForce_params_PARAM_TUPLES "
      << num_tuples << ";" << std::endl;
#else
  out << "const int MSD::SwopeTorsionForce_sitesSiteTupleListCount = "
      << num_tuples << ";" << std::endl;
#endif

  } //end output_swopetorsionsites_tables()


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProbSpecTransToMSD::output_improperdihedralsites_tables(std::ostream& out)
  {
  std::cout << "   ...outputting ImproperDihedral Parameters" << std::endl; //VERBOSE

  std::stringstream tuple_sites;  
  std::stringstream tuple_params;  
  tuple_params.precision(STREAM_PRECISION);
  int num_tuples = 0;

  tuple_sites << "const SiteId MSD::ImproperDihedralForce_sitesSiteTupleList[] = {" << std::endl;
  tuple_params << "const UDF_Binding::ImproperDihedralForce_Params MSD::ImproperDihedralForce_paramsParamTupleList[] = {" << std::endl;

  db2::StmtHandle stmt1(this->db2conn);
  char sqlcmd1[SQLCMD_BUFSZ];
  snprintf(sqlcmd1, SQLCMD_BUFSZ, "select "
           " site1, site2, site3, site4, "
           " k, psi0 "
           " from mdsystem.improperdihedralforce_tuplelist"
           " where pst_id=%d ORDER by tuple_id "
           " FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare(sqlcmd1);

  int site1;
  int site2;
  int site3;
  int site4;
  double k;
  double psi0;
  stmt1.bindCol(1, (SQLINTEGER&)site1);
  stmt1.bindCol(2, (SQLINTEGER&)site2);
  stmt1.bindCol(3, (SQLINTEGER&)site3);
  stmt1.bindCol(4, (SQLINTEGER&)site4);
  stmt1.bindCol(5, (SQLDOUBLE&)k);
  stmt1.bindCol(6, (SQLDOUBLE&)psi0);

  try { stmt1.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry = true; 
  while (stmt1.fetch())
    {
    num_tuples++;
    if (!first_entry) 
      { 
      tuple_sites << ", "; 
      tuple_params << ", "; 
      }
    if (first_entry) { first_entry = false; }

    tuple_sites << site1 << ", " << site2 << ", " 
                << site3 << ", " << site4 << std::endl;
    tuple_params << std::scientific << "{" << k << ", " << psi0 << "}" << std::endl;
    }
  if (first_entry)
    {
    tuple_sites << "-1";
    tuple_params << "{ -1, -1 }";
    }
  tuple_sites << "};" << std::endl;
  tuple_params << "};" << std::endl;

  out << tuple_sites.str();
  out << tuple_params.str();
#ifdef TEST_PROBSPECDB2
  out << "#define NUMBER_OF_ImproperDihedralForce_sites_SITE_TUPLES "
      << num_tuples << ";" << std::endl;
  out << "#define NUMBER_OF_ImproperDihedralForce_params_PARAM_TUPLES "
      << num_tuples << ";" << std::endl;
#else
  out << "const int MSD::ImproperDihedralForce_sitesSiteTupleListCount = "
      << num_tuples << ";" << std::endl;
#endif

  } //end output_improperdihedralsites_tables()


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProbSpecTransToMSD::output_oplstorsionsites_tables(std::ostream& out)
  {
  std::cout << "   ...outputting OPLSTorsion Parameters" << std::endl; //VERBOSE

  std::stringstream tuple_sites;  
  std::stringstream tuple_params;  
  tuple_params.precision(STREAM_PRECISION);
  int num_tuples = 0;

  tuple_sites << "const SiteId MSD::OPLSTorsionForce_sitesSiteTupleList[] = {" << std::endl;
  tuple_params << "const UDF_Binding::OPLSTorsionForce_Params MSD::OPLSTorsionForce_paramsParamTupleList[] = {" << std::endl;
  
  db2::StmtHandle stmt1(this->db2conn);
  char sqlcmd1[SQLCMD_BUFSZ];
  snprintf(sqlcmd1, SQLCMD_BUFSZ, "select "
           " site1, site2, site3, site4, "
           " cosDelta, k, sinDelta, n "
           " from mdsystem.oplstorsionforce_tuplelist"
           " where pst_id=%d ORDER by tuple_id "
           " FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare(sqlcmd1);

  int site1;
  int site2;
  int site3;
  int site4;
  double cosDelta;
  double k;
  double sinDelta;
  int n;
  stmt1.bindCol(1, (SQLINTEGER&)site1);
  stmt1.bindCol(2, (SQLINTEGER&)site2);
  stmt1.bindCol(3, (SQLINTEGER&)site3);
  stmt1.bindCol(4, (SQLINTEGER&)site4);
  stmt1.bindCol(5, (SQLDOUBLE&)cosDelta);
  stmt1.bindCol(6, (SQLDOUBLE&)k);
  stmt1.bindCol(7, (SQLDOUBLE&)sinDelta);
  stmt1.bindCol(8, (SQLINTEGER&)n);

  try { stmt1.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry = true; 
  while (stmt1.fetch())
    {
    num_tuples++;
    if (!first_entry) 
      { 
      tuple_sites << ", "; 
      tuple_params << ", "; 
      }
    if (first_entry) { first_entry = false; }

    tuple_sites << site1 << ", " << site2 << ", " 
                << site3 << ", " << site4 << std::endl;
    tuple_params << std::scientific << "{" << cosDelta << ", " << k << ", "
                 << sinDelta << ", " << std::fixed << n << "}" << std::endl;
    }
  if (first_entry)
    {
    tuple_sites << "-1";
    tuple_params << "{ -1, -1, -1, -1 }";
    }
  tuple_sites << "};" << std::endl;
  tuple_params << "};" << std::endl;

  out << tuple_sites.str();
  out << tuple_params.str();
#ifdef TEST_PROBSPECDB2
  out << "#define NUMBER_OF_OPLSTorsionForce_sites_SITE_TUPLES "
      << num_tuples << ";" << std::endl;
  out << "#define NUMBER_OF_OPLSTorsionForce_params_PARAM_TUPLES "
      << num_tuples << ";" << std::endl;
#else
  out << "const int MSD::OPLSTorsionForce_sitesSiteTupleListCount = "
      << num_tuples << ";" << std::endl;
#endif

  } //end output_oplstorsionsites_tables()

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProbSpecTransToMSD::output_oplsimpropersites_tables(std::ostream& out)
  {
  std::cout << "   ...outputting OPLSImproperDihedral Parameters" << std::endl; //VERBOSE

  std::stringstream tuple_sites;  
  std::stringstream tuple_params;  
  tuple_params.precision(STREAM_PRECISION);
  int num_tuples = 0;

  tuple_sites << "const SiteId MSD::OPLSImproperForce_sitesSiteTupleList[] = {" << std::endl;
  tuple_params << "const UDF_Binding::OPLSImproperForce_Params MSD::OPLSImproperForce_paramsParamTupleList[] = {" << std::endl;
  
  db2::StmtHandle stmt1(this->db2conn);
  char sqlcmd1[SQLCMD_BUFSZ];
  snprintf(sqlcmd1, SQLCMD_BUFSZ, "select "
           " site1, site2, site3, site4, "
           " cosDelta, k, sinDelta, n "
           " from mdsystem.oplsimproperforce_tuplelist"
           " where pst_id=%d ORDER by tuple_id "
           " FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare(sqlcmd1);

  int site1;
  int site2;
  int site3;
  int site4;
  double cosDelta;
  double k;
  double sinDelta;
  int n;
  stmt1.bindCol(1, (SQLINTEGER&)site1);
  stmt1.bindCol(2, (SQLINTEGER&)site2);
  stmt1.bindCol(3, (SQLINTEGER&)site3);
  stmt1.bindCol(4, (SQLINTEGER&)site4);
  stmt1.bindCol(5, (SQLDOUBLE&)cosDelta);
  stmt1.bindCol(6, (SQLDOUBLE&)k);
  stmt1.bindCol(7, (SQLDOUBLE&)sinDelta);
  stmt1.bindCol(8, (SQLINTEGER&)n);

  try { stmt1.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry = true; 
  while (stmt1.fetch())
    {
    num_tuples++;
    if (!first_entry) 
      { 
      tuple_sites << ", "; 
      tuple_params << ", "; 
      }
    if (first_entry) { first_entry = false; }

    tuple_sites << site1 << ", " << site2 << ", " 
                << site3 << ", " << site4 << std::endl;
    tuple_params << std::scientific << "{" << cosDelta << ", " << k << ", "
                 << sinDelta << ", " << std::fixed << n << "}" << std::endl;
    }
  if (first_entry)
    {
    tuple_sites << "-1";
    tuple_params << "{ -1, -1, -1, -1 }";
    }
  tuple_sites << "};" << std::endl;
  tuple_params << "};" << std::endl;

  out << tuple_sites.str();
  out << tuple_params.str();
#ifdef TEST_PROBSPECDB2
  out << "#define NUMBER_OF_OPLSImproperForce_sites_SITE_TUPLES "
      << num_tuples << ";" << std::endl;
  out << "#define NUMBER_OF_OPLSImproperForce_params_PARAM_TUPLES "
      << num_tuples << ";" << std::endl;
#else
  out << "const int MSD::OPLSImproperForce_sitesSiteTupleListCount = "
      << num_tuples << ";" << std::endl;
#endif

  } //end output_oplsimpropersites_tables()


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProbSpecTransToMSD::output_nsqsites_tables(std::ostream& out)
  {
  std::cout << "   ...outputting NSQ Site Info" << std::endl; //VERBOSE

  /////////////////////////////////////////////////
  // Output NSQ Site TYPES for Charge and LJ Params
  std::stringstream charge_params;  
  std::stringstream lj_params;  
  charge_params.precision(STREAM_PRECISION);
  lj_params.precision(STREAM_PRECISION);
  int num_types = 0;

  charge_params << "const UDF_Binding::NSQCoulomb_Params MSD::ChargeNsqParams[] = {" << std::endl;
  lj_params << "const UDF_Binding::NSQLennardJones_Params MSD::LJNsqParams[] = {" << std::endl;

  db2::StmtHandle stmt1(this->db2conn);
  char sqlcmd1[SQLCMD_BUFSZ];
  snprintf(sqlcmd1, SQLCMD_BUFSZ, "select "
           " charge, lj_epsilon, lj_rmin "
           " from mdsystem.nsq_siteinfo "
           " where pst_id=%d ORDER by info_id "
           " FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare(sqlcmd1);

  double charge;
  double lj_epsilon;
  double lj_rmin;
  stmt1.bindCol(1, (SQLDOUBLE&)charge);
  stmt1.bindCol(2, (SQLDOUBLE&)lj_epsilon);
  stmt1.bindCol(3, (SQLDOUBLE&)lj_rmin);

  try { stmt1.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry1 = true; 
  while (stmt1.fetch())
    {
    num_types++;
    if (!first_entry1) 
      { 
      charge_params << ", "; 
      lj_params << ", "; 
      }
    if (first_entry1) { first_entry1 = false; }

    charge_params << std::scientific << "{" << charge << "}" << std::endl;
    lj_params << std::scientific << "{" << lj_epsilon << ", " << lj_rmin << "}" << std::endl;
    }
  if (first_entry1)
    {
    charge_params << "{ -1 }";
    lj_params << "{ -1, -1 }";
    }
  charge_params << "};" << std::endl;
  lj_params << "};" << std::endl;

  out << charge_params.str();
  out << lj_params.str();
#ifdef TEST_PROBSPECDB2
  out << "#endif" << std::endl;
  out << "#define NUMBER_OF_NSQ_SITE_TYPES " << num_types << std::endl;
  out << "#ifndef MSD_COMPILE_CODE_ONLY" << std::endl;
#else
  out << "const int MSD::NumberOfNsqSiteTypes = " << num_types << ";" << std::endl;
#endif

  /////////////////////////////////////////////////
  // Output NSQ SiteID -> NSQ SITE TYPE
  std::stringstream site_to_type;  
  site_to_type.precision(STREAM_PRECISION);
  int num_sites = 0;

  site_to_type << "const int MSD::NsqSiteIDtoSiteTypeMap[] = {" << std::endl;

  db2::StmtHandle stmt2(this->db2conn);
  char sqlcmd2[SQLCMD_BUFSZ];
  snprintf(sqlcmd2, SQLCMD_BUFSZ, "select info_id "
           " from mdsystem.nsq_site_to_nsq_siteinfo"
           " where pst_id=%d ORDER by site_id"
           " FOR READ ONLY WITH UR", pst_id);
  stmt2.prepare(sqlcmd2);

  int info_id;
  stmt2.bindCol(1, (SQLINTEGER&)info_id);

  try { stmt2.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry2 = true; 
  while (stmt2.fetch())
    {
    num_sites++;
    if (!first_entry2) 
      { 
      site_to_type << ", "; 
      }
    if (first_entry2) { first_entry2 = false; }

    site_to_type << info_id << std::endl;
    }
  if (first_entry2)
    {
    site_to_type << "-1";
    }
  site_to_type << "};" << std::endl;

  out << site_to_type.str();
#ifdef TEST_PROBSPECDB2
  out << "const int MSD::NsqSiteIDtoSiteTypeMapSize = "
      << num_sites << ";" << std::endl;
#else
  out << "const int MSD::NsqSiteIDtoSiteTypeMapCount = "
      << num_sites << ";" << std::endl;
#endif


  } //end output_nsqsites_tables()



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProbSpecTransToMSD::output_watersites_tables(std::ostream& out)
  {
  std::cout << "   ...outputting Water Sites" << std::endl; //VERBOSE

  /////////////////////////////////////////////////
  // Output Fragment TYPES
  std::stringstream water_sites;  
  water_sites.precision(STREAM_PRECISION);
  int num_waters = 0;

  water_sites << "const SiteId MSD::Water3sites[] = {" << std::endl;

  db2::StmtHandle stmt1(this->db2conn);
  char sqlcmd1[SQLCMD_BUFSZ];
  snprintf(sqlcmd1, SQLCMD_BUFSZ, "select "
           " site_id1, site_id2, site_id3 "
           " from mdsystem.water3_sites "
           " where pst_id=%d ORDER by site_id1 "
           " FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare(sqlcmd1);

  int site_id1;
  int site_id2;
  int site_id3;
  stmt1.bindCol(1, (SQLINTEGER&)site_id1);
  stmt1.bindCol(2, (SQLINTEGER&)site_id2);
  stmt1.bindCol(3, (SQLINTEGER&)site_id3);

  try { stmt1.execute(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  bool first_entry1 = true; 
  while (stmt1.fetch())
    {
    num_waters++;
    if (!first_entry1) 
      { 
      water_sites << ", "; 
      }
    if (first_entry1) { first_entry1 = false; }

    water_sites << site_id1 << ", " << site_id2 << ", "
                << site_id3 << std::endl;
    }
  if (first_entry1)
    {
    water_sites << "-1";
    }
  water_sites << "};" << std::endl;

  out << water_sites.str();
#ifdef TEST_PROBSPECDB2
  out << "const int MSD::Water3sitesSize = "
      << num_waters << ";" << std::endl;
#else
  out << "const int MSD::Water3sitesCount = "
      << num_waters << ";" << std::endl;
#endif

  } //end output_watersites_tables()


void ProbSpecTransToMSD::output_entire_MSD(std::ostream& file_ostream)
  {
  std::cout << "Writing out all MSD data to output stream" << std::endl; //VERBOSE

  db2::StmtHandle stmt0(this->db2conn);
  char sqlcmd0[SQLCMD_BUFSZ];
  snprintf(sqlcmd0, SQLCMD_BUFSZ, "select "
           " param_value "
           " from mdsystem.global_parameter "
           " where sys_id=%d and param_id like 'ff_family' "
           " FOR READ ONLY WITH UR", sys_id);
  stmt0.prepare(sqlcmd0);

  char ff_family[SQLVARCHAR_BUFSZ];
  stmt0.bindCol(1, (SQLCHAR*)&ff_family[0], SQLVARCHAR_BUFSZ);
  
  try { stmt0.execute(); stmt0.fetch(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }  

  // Output Header
  db2::StmtHandle stmt1(this->db2conn);
  char sqlcmd1[SQLCMD_BUFSZ];
  snprintf(sqlcmd1, SQLCMD_BUFSZ, "select "
           " water_method, do_shake_water, do_shake_protein, time_conversion_factor "
           " from mdsystem.online_parameter "
           " where pst_id=%d "
           " FOR READ ONLY WITH UR", pst_id);
  stmt1.prepare(sqlcmd1);
     
  char water_method[SQLVARCHAR_BUFSZ];
  int do_shake_water = -1;
  int do_shake_protein = -1;
  double time_conversion_factor = -1;

  stmt1.bindCol(1, (SQLCHAR*)&water_method[0], SQLVARCHAR_BUFSZ);
  stmt1.bindCol(2, (SQLINTEGER&)do_shake_water);
  stmt1.bindCol(3, (SQLINTEGER&)do_shake_protein);
  stmt1.bindCol(4, (SQLDOUBLE&)time_conversion_factor);

  try { stmt1.execute(); stmt1.fetch(); }
  catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

  file_ostream << "///////////////////////////////" << std::endl;
  file_ostream << "// MSD Data File" << std::endl;
  file_ostream << "///////////////////////////////" << std::endl;
  file_ostream << "// PST ID        : " << this->pst_id << std::endl;
  file_ostream << "// System ID        : " << this->sys_id << std::endl;
  file_ostream << "// Water Method     : " << water_method << std::endl;
  file_ostream << "// Do Shake Water   : " << do_shake_water << std::endl;
  file_ostream << "// Do Shake Protein : " << do_shake_protein << std::endl;
  file_ostream << "///////////////////////////////" << std::endl;
  file_ostream << std::endl;;
#ifndef TEST_PROBSPECDB2
  file_ostream << "#include <BlueMatter/MSD.hpp>" << std::endl;
  file_ostream << std::endl;;
#endif

  file_ostream << "const int MSD::SystemID = " << this->pst_id << ";" << std::endl;
  file_ostream << "const double MSD::TimeConversionFactor = " << time_conversion_factor << ";" << std::endl;
  file_ostream << "const int MSD::WATER = " << WATER << ";" << std::endl;
  file_ostream << "const int MSD::AMINOACID = " << AMINOACID << ";" << std::endl;
  file_ostream << "const int MSD::OTHER = " << OTHER << ";" << std::endl;
  file_ostream << "const int MSD::PROTEIN = " << PROTEIN << ";" << std::endl;
  file_ostream << "const int MSD::PROTEIN_H = " << PROTEIN_H << ";" << std::endl;

  if( do_shake_water )
    {
    file_ostream << "const int ECA_Constant_DoShakeRattleWater = 1;" << std::endl;    
    }

  if( do_shake_protein )
    {
    file_ostream << "const int ECA_Constant_DoShakeRattleProtein = 1;" << std::endl;    
    }

  file_ostream << "const int ECA_Constant_WaterMethod_" << water_method << " = 1;" << std::endl;    
  file_ostream << "const int ECA_Constant_ForceField_" << ff_family << " = 1;" << std::endl;       

  // Output main data body
  this->output_rigidsites_tables(file_ostream);

  this->output_siteid_mapping_tables(file_ostream);

  this->output_fragmentation_tables(file_ostream);

  this->output_siteinfo_tables(file_ostream);

  this->output_excludedsites_tables(file_ostream);
  
  this->output_pair14sites_tables(file_ostream);
 
  this->output_harmbondedsites_tables(file_ostream);
  this->output_harmanglesites_tables(file_ostream);
  this->output_ureybradleysites_tables(file_ostream);
  this->output_swopetorsionsites_tables(file_ostream);
  this->output_improperdihedralsites_tables(file_ostream);
  this->output_oplstorsionsites_tables(file_ostream);
  this->output_oplsimpropersites_tables(file_ostream);

  this->output_nsqsites_tables(file_ostream);

  this->output_watersites_tables(file_ostream);

  std::cout << std::endl << "Finished writing to output stream." << std::endl;

  } //end output_entire_MSD()


} //end namespace probspectrans






