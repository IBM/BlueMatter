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
// File: transform_sysparams.hpp
// Author: cwo 
// Date: June 15, 2006 
// Namespace: probspectrans
// Class: TransformSysParams
// Description: Main class that transforms existing tables in MDSYSTEM
//              to fill empty tables in MDSYSTEM 
// ***********************************************************************
#ifndef INCLUDE_PROBSPECTRANS_MDSYS_TRANSFORM_SYSPARAMS_HPP
#define INCLUDE_PROBSPECTRANS_MDSYS_TRANSFORM_SYSPARAMS_HPP

#include <BlueMatter/probspectrans/runtimeparams.hpp>
#include <BlueMatter/probspectrans/udf_registryif.hpp>
#include <BlueMatter/probspectrans/math_hpp.hpp>

#include <BlueMatter/Combining_Rules.hpp>

#include <db2/dbname.hpp>
#include <db2/stmthandle.hpp>
#include <db2/handleexception.hpp>

#include <stdlib.h>

#include <map>
#include <set>
#include <vector>
#include <string>

#include <limits.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>

//#define TRANSFORMSYSPARAMS_SINGLETON_CLASS

namespace probspectrans 
{
  //enum MOLECULE_TYPE {PROTEIN=0, WATER=1, OTHER=2, PROTEIN_H=3};
  enum MOLECULE_TYPE {WATER=0, AMINOACID=1, OTHER=2, PROTEIN=3, PROTEIN_H=4};
  enum NSQ {NSQ0=0, NSQ1=1, NSQ2=2};
  enum EXCLUSION {Exclusion=0, Inclusion=1};
  enum COMBINED {NORM=0, COMB=1};

  //Only used for Combining_Rules in pair14 sites
  // This allows us not to have to #include <BlueMatter/UDF_Binding>
  // which would bring along a lot of baggage
  typedef struct LJ_PARAMS
    {
    double epsilon;
    double sigma;
    } LJ_PARAMS;

  typedef struct RIGID2_DB_ENTRY
    {
    const int heavy_site;
    const int hydrogen_site1;
    const double param_hydrogen_site1;
    RIGID2_DB_ENTRY(int, int, double);
    RIGID2_DB_ENTRY operator=(const RIGID2_DB_ENTRY&) const;
    } RIGID2_DB_ENTRY;

  typedef struct RIGID3_DB_ENTRY
    {
    const int heavy_site;
    const int hydrogen_site1;
    const double param_hydrogen_site1;
    const int hydrogen_site2;
    const double param_hydrogen_site2;
    RIGID3_DB_ENTRY(int, int, double, int, double);
    RIGID3_DB_ENTRY operator=(const RIGID3_DB_ENTRY&) const;
    } RIGID3_DB_ENTRY;

  typedef struct RIGID4_DB_ENTRY
    {
    const int heavy_site;
    const int hydrogen_site1;
    const double param_hydrogen_site1;
    const int hydrogen_site2;
    const double param_hydrogen_site2;
    const int hydrogen_site3;
    const double param_hydrogen_site3;
    RIGID4_DB_ENTRY(int, int, double, int, double, int, double);
    RIGID4_DB_ENTRY operator=(const RIGID4_DB_ENTRY&) const;
    } RIGID4_DB_ENTRY;

  typedef struct SITE_REP 
    {
    const int site_id;
    const std::string element_symbol;
    SITE_REP(int, const char*);
    bool operator< (const SITE_REP&) const;
    //bool operator== (const SITE_REP& cmp) const;
    } SITE_REP;

  typedef struct FRAGMENT_TYPE
    {
    const int num_sites;
    const MOLECULE_TYPE molecule_type; 
    const int tagatom_index; 
    const int num_constraints;
    const double extent;
    FRAGMENT_TYPE(int, MOLECULE_TYPE, int, int, double); 
    bool operator< (const FRAGMENT_TYPE&) const;
    } FRAGMENT_TYPE;

  typedef struct SITEINFO_STR_REP
    {
    const std::string atomic_mass;
    const std::string charge;
    SITEINFO_STR_REP(std::string, std::string);
    bool operator< (const SITEINFO_STR_REP&) const;
    } SITEINFO_STR_REP;

  typedef struct NSQ_SITEINFO_STR_REP
    {
    const std::string charge;
    const std::string lj_epsilon;
    const std::string lj_rmin;
    NSQ_SITEINFO_STR_REP(std::string, std::string, std::string);
    bool operator< (const NSQ_SITEINFO_STR_REP&) const;
    } NSQ_SITEINFO_STR_REP;

  typedef struct PARAM_TUPLE_REP
    {
    std::vector<std::string> names; //necessary, but redundant storage
    std::vector<std::string> values;
    std::vector<int> types; //necessary, but redundant storage
    } PARAM_TUPLE_REP;


  class TransformSysParams
  {
    private:
      
      //////////////////////////////////////// 
      // global fields
#ifdef TRANSFORMSYSPARAMS_SINGLETON_CLASS
      static TransformSysParams *obj_instance;
#endif
      RuntimeParams *runtimeparams;
      UDF_RegistryIF *udf_registry;
      db2::ConnHandle& db2conn;
      int pst_id;
      int sys_id;
     
      //////////////////////////////////////// 
      //EXTERNAL-INDEX SITE-MAPPED STRUCTURE
      //map<external site_id, internal site_id>
      std::map<int,int> ext_int_site_map;
      //map<site_id -> pair<MOLECULE_TYPE, molecule_instance_id> > //contains every site
      std::map<int, std::pair<MOLECULE_TYPE,int> > molecule_type_map;  
      //map<site_id -> element_symbol string>
      std::map<int, std::string> site_element_symbol_map;  
      //multimap<site_id -> adjacent sites>
      std::multimap<int, SITE_REP> bond_adjacency_map;
      //set<site_id>
      std::set<int> hydrogen_site_set;
      //vector<rigid# site DB row entries>
      std::vector<RIGID2_DB_ENTRY> rigid2_dbentries;
      std::vector<RIGID3_DB_ENTRY> rigid3_dbentries;
      std::vector<RIGID4_DB_ENTRY> rigid4_dbentries;

      //vector<site_id>
      std::vector<int> crumb_list; 

      //below used to determine fragmentation
      //map<site_id, fragment index>
      std::map<int, int> site_frag_id_map; 
      //map<fragment index, sites in fragment>
      std::map<int, std::vector<int> > frag_id_sites_map;
      //map<molecule id, vector<fragment ids> >
      std::map<int, std::vector<int> > molecule_fragment_map;
      int num_fragments;
      int num_molecules;
      // Speeds up fragmentation search
      int oxygen_pos_in_first_water;

      //set<pair<site A, site 1 or 2 bonds away from A>>
      std::set<std::pair<int, int> > excluded_sites;
      //set<pair<site A, site 3 bonds away from A>>
      std::set<std::pair<int, int> > pair14_sites;

      //vector<ordered 3 atoms in watersite>
      std::vector<std::vector<int> > water3_sites_vec;

      //////////////////////////////////////// 
      // main methods 
      void determine_molecule_types();
      void build_bond_adjacency_map();
      void create_rigidsites_params();
      void write_rigidsites_params();
      void create_rigidgroup_fragmentation();
      void create_bondedsites_params();
      void create_nsqsites_params();
      void create_siteinfo_table();
      void create_pair_exclusion_tables();
      void write_pair_exclusion_tables();
      void fill_UDF_invocation_table();
      void determine_water3_sites();
      void write_water3_sites();


      //////////////////////////////////////// 
      //utility methods
      int separation_between_bonded_sites(std::vector<int>&, int, 
                                          std::vector<int>&, int);
      //
      std::vector<double> compile_symbolic_double(const std::vector<std::string>&);
      int get_number_sites_in_system();
      int get_first_water_siteid();
      int get_oxygen_pos(const int&);
      int get_tagatom_index_for_amino_acid(const std::string&);
      double get_maxextent_for_amino_acid(const std::string&);
      //
      double find_chain_depth(double, int, std::vector<int>);
      double guess_radius_of_sites(const std::vector<int>&);
      FRAGMENT_TYPE get_fragment_type(const std::vector<int>&, const std::string&);

      TransformSysParams(std::string, int, int, RuntimeParams*, UDF_RegistryIF*);
    public:
      
      static TransformSysParams* instance(std::string, int, int, RuntimeParams*,
                                          UDF_RegistryIF*);
      ~TransformSysParams();
      
      void transform_all_sysparams();
      bool check_for_dirty_DB();
  };
}

#endif
