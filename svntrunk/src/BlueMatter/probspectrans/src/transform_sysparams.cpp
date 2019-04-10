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
 //// ***********************************************************************
// File: transform_sysparams.cpp
// Author: cwo 
// Date: June 15, 2006 
// Namespace: probspectrans
// Class: TransformSysParams
// Description: Main class that transforms existing tables in MDSYSTEM
//              to fill empty tables in MDSYSTEM 
// ***********************************************************************

#include <fstream>
#include <limits.h>
#include <float.h>
using namespace std ;
#include <BlueMatter/probspectrans/transform_sysparams.hpp>

// #define SQLCMD_BUFSZ 512 
// #define SQLVARCHAR_BUFSZ 255

#define STREAM_PRECISION 16

namespace probspectrans 
{ 

  //****************************************************************************
  // Utility functions needed for structures to be placed in STL containers 
  //****************************************************************************

  RIGID2_DB_ENTRY::RIGID2_DB_ENTRY(int s1, int h1, double p1) :
    heavy_site(s1), hydrogen_site1(h1), param_hydrogen_site1(p1)
    {} 
  RIGID2_DB_ENTRY RIGID2_DB_ENTRY::operator= (const RIGID2_DB_ENTRY& c) const 
    {
    return RIGID2_DB_ENTRY(c.heavy_site, c.hydrogen_site1, c.param_hydrogen_site1);
    }

  RIGID3_DB_ENTRY::RIGID3_DB_ENTRY(int s1, int h1, double p1, int h2, double p2) :
    heavy_site(s1), hydrogen_site1(h1), param_hydrogen_site1(p1),
    hydrogen_site2(h2), param_hydrogen_site2(p2)
    {} 
  RIGID3_DB_ENTRY RIGID3_DB_ENTRY::operator= (const RIGID3_DB_ENTRY& c) const 
    {
    return RIGID3_DB_ENTRY(c.heavy_site, c.hydrogen_site1, c.param_hydrogen_site1,
                           c.hydrogen_site2, c.param_hydrogen_site2);
    }

  RIGID4_DB_ENTRY::RIGID4_DB_ENTRY(int s1, int h1, double p1, int h2, double p2,
    int h3, double p3) :
    heavy_site(s1), hydrogen_site1(h1), param_hydrogen_site1(p1),
    hydrogen_site2(h2), param_hydrogen_site2(p2),
    hydrogen_site3(h3), param_hydrogen_site3(p3)
    {} 
  RIGID4_DB_ENTRY RIGID4_DB_ENTRY::operator= (const RIGID4_DB_ENTRY& c) const 
    {
    return RIGID4_DB_ENTRY(c.heavy_site, c.hydrogen_site1, c.param_hydrogen_site1,
                           c.hydrogen_site2, c.param_hydrogen_site2,
                           c.hydrogen_site3, c.param_hydrogen_site3);
    }

  SITE_REP::SITE_REP (int site_id, const char *element_symbol) :
    site_id (site_id), element_symbol (element_symbol)
    {} 
  
  //////////////////////////////////////////////////
  FRAGMENT_TYPE::FRAGMENT_TYPE(int num_sites, MOLECULE_TYPE molecule_type, 
    int tagatom_index, int num_constraints, double extent) :
    num_sites(num_sites), molecule_type(molecule_type), tagatom_index(tagatom_index),
    num_constraints(num_constraints), extent(extent) 
    {} 
  bool FRAGMENT_TYPE::operator< (const FRAGMENT_TYPE& cmp) const 
    {
    std::stringstream x;
    std::stringstream y;
    x << num_sites << ":" << molecule_type << ":" << tagatom_index 
      << ":" << num_constraints << ":";
    x.precision(STREAM_PRECISION);
    x << std::scientific << extent;
    y << cmp.num_sites << ":" << cmp.molecule_type << ":" << cmp.tagatom_index 
      << ":" << cmp.num_constraints << ":";
    y.precision(STREAM_PRECISION);
    y << std::scientific << cmp.extent;
    return (x.str() < y.str());
    } //end overloaded <

  //////////////////////////////////////////////////
  SITEINFO_STR_REP::SITEINFO_STR_REP(std::string atomic_mass, std::string charge) :
    atomic_mass(atomic_mass), charge(charge)
    {}
  bool SITEINFO_STR_REP::operator< (const SITEINFO_STR_REP& cmp) const 
    {
    std::stringstream x;
    std::stringstream y;
    x << atomic_mass << ":" << charge;
    y << cmp.atomic_mass << ":" << cmp.charge;
    return (x.str() < y.str());
    } //end overloaded <
  
  //////////////////////////////////////////////////
  NSQ_SITEINFO_STR_REP::NSQ_SITEINFO_STR_REP(std::string charge, std::string lj_epsilon,
    std::string lj_rmin) :
    charge(charge), lj_epsilon(lj_epsilon), lj_rmin(lj_rmin)
    {}
  bool NSQ_SITEINFO_STR_REP::operator< (const NSQ_SITEINFO_STR_REP& cmp) const 
    {
    std::stringstream x;
    std::stringstream y;
    x << charge << ":" << lj_epsilon << ":" << lj_rmin;
    y << cmp.charge << ":" << cmp.lj_epsilon << ":" << cmp.lj_rmin;
    return (x.str() < y.str());
    } //end overloaded <
  
//****************************************************************************
// Constructors/Destructors
//****************************************************************************
#ifdef TRANSFORMSYSPARAMS_SINGLETON_CLASS
  TransformSysParams *TransformSysParams::obj_instance = NULL;
#endif

  TransformSysParams::TransformSysParams(
          std::string dbname, 
          int pst_id, 
          int sys_id, 
          RuntimeParams *runtimeparams,
          UDF_RegistryIF *udf_registry)
    :  db2conn (db2::DBName::instance(dbname.c_str())->connection()),
       pst_id (pst_id), sys_id (sys_id),
       runtimeparams (runtimeparams),
       udf_registry(udf_registry)
    {
    this->num_fragments = 0; 
    this->num_molecules = 0; 
    } 
  TransformSysParams::~TransformSysParams()
    {
#ifdef TRANSFORMSYSPARAMS_SINGLETON_CLASS
    delete obj_instance;
    obj_instance = NULL;
#endif
    }

  TransformSysParams* TransformSysParams::instance(
          std::string dbname,
          int pst_id,
          int sys_id,
          RuntimeParams *runtimeparams,
          UDF_RegistryIF *udf_registry)
    { 
#ifdef TRANSFORMSYSPARAMS_SINGLETON_CLASS
    if (obj_instance == NULL) 
      {
      obj_instance = new TransformSysParams(dbname, 
                                            pst_id, 
                                            sys_id, 
                                            runtimeparams,
                                            udf_registry );
      assert(obj_instance != NULL);
      }
    return (obj_instance);
#else
      return (new TransformSysParams(dbname, pst_id, sys_id, runtimeparams, udf_registry));
#endif
    }


//****************************************************************************
// Utility Functions
//****************************************************************************
  int TransformSysParams::get_number_sites_in_system()
  {
    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[SQLCMD_BUFSZ];
    snprintf(sqlcmd1, SQLCMD_BUFSZ, "select count(*) from mdsystem.site where "
             " sys_id=%d FOR READ ONLY WITH UR", this->sys_id);
    stmt1.prepare(sqlcmd1);

    int ret_count;
    stmt1.bindCol(1, (SQLINTEGER&)ret_count);
    try { stmt1.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
    stmt1.fetch(); //only need to fetch 1 value
    
    return ret_count;
  } //end get_number_sites_in_system


//****************************************************************************
  std::vector<double> TransformSysParams::compile_symbolic_double(const std::vector<std::string>& sym_val_vec)
    {
#if 0
  std::cerr << "compile_symbolic_double(vector<string>)" << std::endl;
#endif

      std::vector<double> ret_dbl_vec;

      ofstream eval_file("eval_symbolic.cpp");
      eval_file << "#include <stdlib.h>\n";
      eval_file << "#include <stdio.h>\n";
      eval_file << "#define MSD_COMPILE_DATA_ONLY\n";
      eval_file << SRC_MATH_HPP_STRING << "\n";

      //Must determine #defines from DB2
      db2::StmtHandle stmt(this->db2conn);
      char sqlcmd[SQLCMD_BUFSZ];
      snprintf(sqlcmd, SQLCMD_BUFSZ, "select param_id, param_value from "
               " mdsystem.global_parameter where sys_id=%d and "
               " symbolic_constant=1 ", this->sys_id);
      stmt.prepare(sqlcmd);
      
      char param_id_cstr[SQLVARCHAR_BUFSZ];
      char param_name_cstr[SQLVARCHAR_BUFSZ];
      stmt.bindCol(1, (SQLCHAR*)&param_id_cstr[0], SQLVARCHAR_BUFSZ);
      stmt.bindCol(2, (SQLCHAR*)&param_name_cstr[0], SQLVARCHAR_BUFSZ);
      
      try { stmt.execute(); }
      catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
      
      while(stmt.fetch())
        {
        eval_file << "#define " << param_id_cstr << " " << param_name_cstr << "\n";
        }
      eval_file << "#define null 0.0\n";
      eval_file << "static double all_values[" << sym_val_vec.size() << "] = {\n";
      std::vector<std::string>::const_iterator vec_itr = sym_val_vec.begin();
      std::vector<std::string>::const_iterator vec_end = sym_val_vec.end();
      for (; vec_itr != vec_end; ++vec_itr)
        {
        if (vec_itr != sym_val_vec.begin())
          {
          eval_file << ",";
          }
        eval_file << (*vec_itr).c_str() << "\n";
        }
      eval_file << "};\n";

      eval_file << "int main(int argc, char* argv[]) {\n";
      eval_file << "  FILE *symfile = fopen(\"eval_symbolic.out\", \"w\");\n";
      eval_file << "  for (int i=0; i < " << sym_val_vec.size() << "; ++i) {\n";
      eval_file << "    fwrite(&(all_values[i]), sizeof(double), 1, symfile);\n";
      eval_file << "  }\n";
      eval_file << "  fclose(symfile); }\n";
      eval_file.close();

      //FIXME -- this would be better/faster with pipes/sockets!
      system("xlC eval_symbolic.cpp -o eval_symbolic.bin");
      system("./eval_symbolic.bin");
  
      FILE *symbolic_file = fopen("eval_symbolic.out", "r");
      for (int i=0; i < sym_val_vec.size() ; ++i)
        {
        double ret_dbl;
        fread (&ret_dbl, sizeof(double), 1, symbolic_file);
        ret_dbl_vec.push_back(ret_dbl);
        }
      fclose(symbolic_file);

      system("rm ./eval_symbolic.cpp");
      system("rm ./eval_symbolic.out");
      system("rm ./eval_symbolic.bin");
    
      return ret_dbl_vec;
    } //end compile_symbolic_double();


//****************************************************************************
  int TransformSysParams::get_first_water_siteid()
    {
#if 0
    std::cerr << "get_first_water_siteid()" << std::endl;
#endif
    db2::StmtHandle stmt(this->db2conn);
    char sqlcmd[SQLCMD_BUFSZ];
    snprintf(sqlcmd, SQLCMD_BUFSZ, "select std.site_id from mdsystem.sitetuplelist as stl, "
              " mdsystem.sitetuple as st,  "
              " mdsystem.sitetupledata as std  "
              " where st.sys_id=%d " 
              " and stl.sys_id=%d " 
              " and std.sys_id=%d "
              " and stl.site_tuple_desc LIKE 'Water3sites'"
              " and st.site_tuple_list_id=std.site_tuple_list_id"
              " and stl.site_tuple_list_id=std.site_tuple_list_id"
              " and std.site_tuple_id=0"
              " order by std.site_id asc"
              " FOR READ ONLY WITH UR", 
              this->sys_id, this->sys_id, this->sys_id);

    stmt.prepare(sqlcmd);

    int ret_siteid = -1;
    stmt.bindCol(1, (SQLINTEGER&)ret_siteid);
    try { stmt.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
    stmt.fetch(); //only need to fetch FIRST value
    
    return ret_siteid;
    } //end get_first_water_siteid()


//****************************************************************************
  int TransformSysParams::get_oxygen_pos(const int& siteid)
    {
#if 0
    std::cerr << "get_oxygen_pos()" << std::endl;
#endif

    for(int i=0; i < 3; ++i)
      {
      db2::StmtHandle stmt(this->db2conn);
      char sqlcmd[SQLCMD_BUFSZ];
      snprintf(sqlcmd, SQLCMD_BUFSZ, "SELECT atom_tag FROM MDSYSTEM.SITE "
               " WHERE sys_id=%d AND site_id=%d "
               " FOR READ ONLY WITH UR", this->sys_id, (siteid+i));
      stmt.prepare(sqlcmd);
    
      char ret_tagatom[SQLVARCHAR_BUFSZ];
      stmt.bindCol(1, (SQLCHAR*)&ret_tagatom[0], SQLVARCHAR_BUFSZ);
      try { stmt.execute(); }
      catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
      stmt.fetch(); //only need to fetch 1 value

      if (ret_tagatom[0] == 'O') 
        {
          return i;
        } 
      } //end for()
      
      return -1;
    } //end get_oxygen_pos

//****************************************************************************
// Expects site_path to contain the first site when started
// NOTE: This function is currently unused (formerly used)
  int TransformSysParams::separation_between_bonded_sites(std::vector<int>& site_path, int dest_site, 
                                             std::vector<int>& sites_at_Nth_distance, int Nth_distance)
    {
#if 0
    std::cerr << "sep_btwn_bsites() - currdist=" << site_path.size() << " curr_site=" << site_path.back() << " dest_site=" << dest_site << std::endl;
#endif

    int smallest_sep = INT_MAX;
    int curr_site = site_path.back();
    
    typedef std::multimap<int, SITE_REP>::const_iterator MMI;
	  std::pair<MMI,MMI> adj_sites = this->bond_adjacency_map.equal_range(curr_site);
	  for (MMI adj_i = adj_sites.first; adj_i != adj_sites.second; ++adj_i) 
      {
      int bonded_site = adj_i->second.site_id;

      std::vector<int>::const_iterator checkpath_itr = site_path.begin();
      std::vector<int>::const_iterator checkpath_end = site_path.end();
      bool already_seen = false;
      for (; checkpath_itr != checkpath_end; ++checkpath_itr)
        {
        if (*checkpath_itr == bonded_site)
          {
          already_seen = true;
          break;
          }
        }
      if (already_seen) 
        { 
        continue; 
        }

      if (site_path.size() == Nth_distance)
        {
        sites_at_Nth_distance.push_back(bonded_site);
        }

      if (bonded_site == dest_site)
        {
        return site_path.size();
        }
      else
        {
        site_path.push_back(bonded_site);
        int ret_sep = separation_between_bonded_sites(site_path, dest_site,
                                  sites_at_Nth_distance, Nth_distance); 
        if (ret_sep < smallest_sep)
          {
          smallest_sep = ret_sep;
          }
        site_path.pop_back();
        }
      } //end for each site bonded to first site

    return smallest_sep;
    } //end separation_between_bonded_sites()

//****************************************************************************
  double TransformSysParams::find_chain_depth(double branch_depth, 
                                              int first_site, 
                                              std::vector<int> other_sites)
    {
#if 0
  std::cerr << "find_chain_depth() - branch_depth=" << branch_depth << std::endl;
  std::vector<int>::const_iterator i_itr = other_sites.begin();
  std::vector<int>::const_iterator i_end = other_sites.end();
  for (; i_itr != i_end; ++i_itr) 
    {
    std::cerr << "   find_chain: Site=" << *i_itr << std::endl;
    }
#endif


    if (other_sites.empty())
      {
      return branch_depth;
      }

    double max_loop_val = branch_depth;

    typedef std::multimap<int, SITE_REP>::const_iterator MMI;
	  std::pair<MMI,MMI> adj_sites = this->bond_adjacency_map.equal_range(first_site);
	  for (MMI adj_i = adj_sites.first; adj_i != adj_sites.second; ++adj_i) 
      {
      std::vector<int>::iterator s_itr = other_sites.begin(); 
      std::vector<int>::iterator s_end = other_sites.end(); 
      while(s_itr != s_end)
        { 
        if (*s_itr == adj_i->second.site_id)
          {
          int new_first = *s_itr;
          s_itr = other_sites.erase(s_itr); //s_itr++
          double new_depth = branch_depth + 1.0;
          double ret_depth = find_chain_depth(new_depth, new_first, other_sites);
          if (ret_depth > max_loop_val)
            {
            max_loop_val = ret_depth;
            }
          break;
          }
        else
          {
          ++s_itr;
          }
        }
      } //end for each other_sites
    
      return max_loop_val;
    } //end find_chain_depth()

//****************************************************************************
  double TransformSysParams::guess_radius_of_sites(const std::vector<int>& sites_vec)
    {
#if 0
  std::cerr << "guess_radius_of_sites()" << std::endl;
  std::vector<int>::const_iterator i_itr = sites_vec.begin();
  std::vector<int>::const_iterator i_end = sites_vec.end();
  for (; i_itr != i_end; ++i_itr) 
    {
    std::cerr << "   Site=" << *i_itr << std::endl;
    }
#endif

    double max_depth = -1;
    std::vector<int> saved_vec = sites_vec;

    std::vector<int>::const_iterator startpos_itr = sites_vec.begin();
    std::vector<int>::const_iterator startpos_end = sites_vec.end();
    for(; startpos_itr != startpos_end; ++startpos_itr)
      {
      std::vector<int> input_vec;
      int first_site = *startpos_itr;

      std::vector<int>::const_iterator createvec_itr = sites_vec.begin();
      std::vector<int>::const_iterator createvec_end = sites_vec.end();
      for(; createvec_itr != createvec_end; ++createvec_itr)
        {
        if (*createvec_itr != *startpos_itr)
          {
          input_vec.push_back(*createvec_itr);
          }
        }

      double chain_depth = find_chain_depth(0, first_site, input_vec);

      if (chain_depth > max_depth)
        {
        max_depth = chain_depth;
        }
      } //end foreach start position 

    return (double) max_depth;
    } //end guess_radius_of_sites()

//****************************************************************************
  double TransformSysParams::get_maxextent_for_amino_acid(const std::string& amino_acid)
    {
#if 0
  std::cerr << "get_maxextent_for_amino_acid" << std::endl;
#endif
    db2::StmtHandle stmt(this->db2conn);
    char sqlcmd[SQLCMD_BUFSZ];
    snprintf(sqlcmd, SQLCMD_BUFSZ, "SELECT max_extent from chem.extent_table "
             " where component_name='%s' FOR READ ONLY WITH UR", amino_acid);

    char ret_extent_cstr[SQLVARCHAR_BUFSZ];
    stmt.bindCol(1, (SQLCHAR*)&ret_extent_cstr[0], SQLVARCHAR_BUFSZ);
    try { stmt.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
    stmt.fetch(); //only need to fetch 1 value
     
    std::vector<std::string> in_str_vec;
    in_str_vec.push_back(std::string(ret_extent_cstr));
    std::vector<double> ret_double = compile_symbolic_double(in_str_vec);
    return ret_double.front();
    } //end get_maxextent_for_amino_acid()


//****************************************************************************
  int TransformSysParams::get_tagatom_index_for_amino_acid(const std::string& amino_acid)
    {
#if 0
  std::cerr << "get_tagatom_index_for_amino_acid()" << std::endl;
#endif
      db2::StmtHandle stmt1(this->db2conn);
      char sqlcmd1[SQLCMD_BUFSZ];
      snprintf(sqlcmd1, SQLCMD_BUFSZ, "SELECT atom_tag from chem.extent_table"
              " where component_name='%s' FOR READ ONLY WITH UR", amino_acid.c_str());
      stmt1.prepare(sqlcmd1);

      char ret_tagatom1[SQLVARCHAR_BUFSZ];
      stmt1.bindCol(1, (SQLCHAR*)&ret_tagatom1[0], SQLVARCHAR_BUFSZ);
      try { stmt1.execute(); stmt1.fetch(); }
      catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
      stmt1.fetch(); //only need to fetch 1 value

      db2::StmtHandle stmt2(this->db2conn);
      char sqlcmd2[SQLCMD_BUFSZ];
      snprintf(sqlcmd2, SQLCMD_BUFSZ, "SELECT atom_tag from mdsystem.site "
               " where sys_id=%d and component_name='%s' FOR READ ONLY WITH UR",
               this->sys_id, amino_acid);
      stmt2.prepare(sqlcmd2);
      
      char ret_tagatom2[SQLVARCHAR_BUFSZ];
      stmt2.bindCol(1, (SQLCHAR*)&ret_tagatom2[0], SQLVARCHAR_BUFSZ);
      try { stmt2.execute(); }
      catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
    
      int idx = -1;
      while (stmt2.fetch()) 
        {
          if (strcmp(ret_tagatom1,ret_tagatom2)==0) 
            {
              return idx;
            }
          idx++;
        }
      
      return idx;
    } //end get_tagatom_index_for_amino_acid()

// ASSUMES: the input vector site_ids is NON-EMPTY 
//****************************************************************************
  FRAGMENT_TYPE TransformSysParams::get_fragment_type(const std::vector<int>& site_ids, 
                                  const std::string& amino_acid)
    {
#if 0
  std::cerr << "get_fragment_type()" << std::endl;
#endif

    int num_sites = site_ids.size();
    MOLECULE_TYPE molecule_type; 
    int tagatom_index; 
    int num_constraints;
    double extent;

    //To speed things up, only do a find for the MOLECULE_TYPE of site_ids once
    std::vector<MOLECULE_TYPE> site_ids_mol_type;
    std::vector<int>::const_iterator sites_itr = site_ids.begin();
    std::vector<int>::const_iterator sites_end = site_ids.end();
    for (; sites_itr != sites_end; ++sites_itr)
      {
      std::map<int, std::pair<MOLECULE_TYPE,int> >::const_iterator find_itr = 
                  this->molecule_type_map.find(*sites_itr);
        if (find_itr != this->molecule_type_map.end())
          {
          site_ids_mol_type.push_back(find_itr->second.first);
          }
        else
          {
          std::cerr << "ERROR: Site ID has no MOLECULE_TYPE (site_id " << *sites_itr << ") "
                    << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
          }
      }

    bool is_water = false;
    std::map<int, std::pair<MOLECULE_TYPE,int> >::const_iterator find_itr = 
        this->molecule_type_map.find(site_ids.front());
    if (find_itr != this->molecule_type_map.end()) 
      {
      if (find_itr->second.first == WATER) 
        {
        is_water = true;
        }
      }
      
    ////////////
    // Fill FRAGMENT_TYPE.molecule_type 
    if ((num_sites == 3) && is_water)
      {
      molecule_type = WATER;
      }
    else if (amino_acid.compare("NONE")!=0) 
      {
      molecule_type = AMINOACID;
      }
    else
      {
      molecule_type = OTHER;
      }

    ////////////
    // Fill FRAGMENT_TYPE.tagatom_index
    if(is_water)
      {
      tagatom_index = this->oxygen_pos_in_first_water;
      }
    else if (amino_acid.compare("NONE")!=0)
      {
      tagatom_index = get_tagatom_index_for_amino_acid(amino_acid);
      }
    else
      {
      tagatom_index = 0;  
      }
   
    ////////////
    // Fill FRAGMENT_TYPE.num_constraints
    num_constraints = 0;
    if(this->runtimeparams->doShakeRattleWater) 
      {
      int cnt1=0;
      int num_waters = 0;
      int vec_cnt = 0;
      std::vector<int>::const_iterator sites_itr1 = site_ids.begin();
      std::vector<int>::const_iterator sites_end1 = site_ids.end();
      for (; sites_itr1 != sites_end1; ++sites_itr1)
        {
        if (site_ids_mol_type.at(vec_cnt++) == WATER)
          {
          cnt1++;
          }
        }
        num_waters = (int) ( cnt1 / 3 );
        num_constraints += num_waters * 3;
      }

    if(this->runtimeparams->doShakeRattleProtein) 
      {

      int cnt2=0;
      int vec_cnt = 0;
      std::vector<int>::const_iterator sites_itr2 = site_ids.begin();
      std::vector<int>::const_iterator sites_end2 = site_ids.end();
      for (; sites_itr2 != sites_end2; ++sites_itr2)
        {
        if (site_ids_mol_type.at(vec_cnt++) == PROTEIN_H)
          {
          cnt2++;
          }
        }
        num_constraints += cnt2;
      }
    
    ////////////
    // Fill FRAGMENT_TYPE.extent
    if(is_water) 
      {
      if(this->runtimeparams->water_method.compare("NONE")==0)
        {
        std::cerr << "ERROR: No Water Method Chosen, cannot determine extent of water fragment" << std::endl;
        extent = -1;
        }
      if( this->runtimeparams->water_method.compare("TIP3P")==0 ||
          this->runtimeparams->water_method.compare("CHARMM_TIP3P")==0) 
        {
        extent = (1.1 * 0.9572);
        }
      else if( this->runtimeparams->water_method.compare("SPC")==0 ||
               this->runtimeparams->water_method.compare("SPCE")==0) 
        {
        extent = (1.1 * 1.0000);
        }
      else if( this->runtimeparams->water_method.compare("TIP3FLOPPY")==0 ||
               this->runtimeparams->water_method.compare("CHARMM_TIP3PFLOPPY")==0) 
        {
        extent = (1.5 * 0.9572);
        }
      else if( this->runtimeparams->water_method.compare("SPCFLOPPY")==0 ||
               this->runtimeparams->water_method.compare("SPCEFLOPPY")==0) 
        {
        extent = (1.5 * 1.0000);
        }
      else
        {
        std::cerr << "ERROR: during get_fragment_type(), couldn't determine extent" << std::endl;
        extent = -1;
        }

      }
    else if (amino_acid.compare("NONE")!=0)
      {
      extent = get_maxextent_for_amino_acid(amino_acid);
      }
    else 
      {
      if (num_sites == 1)
        {
        extent = 0.0;
        }
      else 
        {
        extent = 3.0;
        }
      } 

    return FRAGMENT_TYPE(num_sites, molecule_type, tagatom_index, num_constraints, extent);
    } //end get_fragment_type()


//****************************************************************************
// Main transformation steps
//****************************************************************************
  void TransformSysParams::determine_molecule_types()
    {
    std::cout << "Determining Molecule Types" << std::endl; //VERBOSE

    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[SQLCMD_BUFSZ];
    snprintf(sqlcmd1, SQLCMD_BUFSZ, "select site_id, mol_instance_id, component_name, "
             "element_symbol, atom_tag from mdsystem.site where sys_id=%d "
             " FOR READ ONLY WITH UR", this->sys_id);
    stmt1.prepare(sqlcmd1);

    int site_id;
    int mol_instance_id;
    char component_name[SQLVARCHAR_BUFSZ];
    char element_symbol[SQLVARCHAR_BUFSZ];
    char atom_tag[SQLVARCHAR_BUFSZ];
    stmt1.bindCol(1, (SQLINTEGER&)site_id);
    stmt1.bindCol(2, (SQLINTEGER&)mol_instance_id);
    stmt1.bindCol(3, (SQLCHAR*)&component_name[0], SQLVARCHAR_BUFSZ);
    stmt1.bindCol(4, (SQLCHAR*)&element_symbol[0], SQLVARCHAR_BUFSZ);
    stmt1.bindCol(5, (SQLCHAR*)&atom_tag[0], SQLVARCHAR_BUFSZ);
    try { stmt1.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
    
    while(stmt1.fetch()) 
      {
#if 0
    std::cerr << "determine_molecule_types():" << std::endl;
    std::cerr << "   Fetched site_id=" << site_id << std::endl;
    std::cerr << "   Fetched mol_instance_id=" << mol_instance_id << std::endl;
    std::cerr << "   Fetched component_name=" << component_name << std::endl;
    std::cerr << "   Fetched element_symbol=" << element_symbol << std::endl;
    std::cerr << "   Fetched atom_tag=" << atom_tag << std::endl;
#endif

      this->site_element_symbol_map.insert(std::make_pair(site_id, std::string(element_symbol)));

      if ( (strcmp("HOH", component_name)==0) ||
          (strcmp("OHH", component_name)==0) ||
          (strcmp("HHO", component_name)==0) )
        {
        this->molecule_type_map.insert(std::make_pair(site_id, 
              std::make_pair(WATER, mol_instance_id)));
        } 
      else if (strcmp("NAP", component_name)==0) 
        {
        this->molecule_type_map.insert(std::make_pair(site_id, 
              std::make_pair(OTHER, mol_instance_id)));
        } 
      else if (strcmp("H", element_symbol)==0) 
        {
        this->molecule_type_map.insert(std::make_pair(site_id, 
              std::make_pair(PROTEIN_H, mol_instance_id)));
        this->hydrogen_site_set.insert(site_id);
        } 
      else 
        {
        this->molecule_type_map.insert(std::make_pair(site_id, 
              std::make_pair(PROTEIN, mol_instance_id)));
        }

      } //end while
      
      std::cout << "      -->total number of sites: " << this->molecule_type_map.size() << std::endl; //VERBOSE

      //This is used later for fragmentation
      this->oxygen_pos_in_first_water = get_oxygen_pos(get_first_water_siteid());  

    } //end determine_molecule_types()



//****************************************************************************
  bool TransformSysParams::check_for_dirty_DB()
    {
    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[SQLCMD_BUFSZ];
    snprintf(sqlcmd1, SQLCMD_BUFSZ, "select count(*) from "
             " mdsystem.online_parameter where pst_id=%d", this->pst_id);
    stmt1.prepare(sqlcmd1);

    int num_params = -1;
    stmt1.bindCol(1, (SQLINTEGER&)num_params);

    try { stmt1.execute(); stmt1.fetch(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
    
    if (num_params == 0)
      {
      return false;
      }
    else
      {
      return true;
      }

    } //end check_for_dirty_DB()


//****************************************************************************
  void TransformSysParams::build_bond_adjacency_map()
    {
    std::cout << "Building Bond Adjacency Map" << std::endl;  //VERBOSE

    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[SQLCMD_BUFSZ];
    snprintf(sqlcmd1, SQLCMD_BUFSZ, "select distinct  mb.site_id1, "
             " ms1.element_symbol, mb.site_id2, ms2.element_symbol "
             " from mdsystem.bond as mb, "
             "        mdsystem.site as ms1,"
             "        mdsystem.site as ms2"
             " where ms1.sys_id=%d and "
             "       ms2.sys_id=%d and "
             "       mb.sys_id=%d  and "
             "       mb.site_id1=ms1.site_id and "
             "       mb.site_id2=ms2.site_id"
             " order by mb.site_id1;", this->sys_id, this->sys_id, this->sys_id);
    stmt1.prepare(sqlcmd1);

    int site_id1;
    char element_symbol1[SQLVARCHAR_BUFSZ];
    int site_id2;
    char element_symbol2[SQLVARCHAR_BUFSZ];
    stmt1.bindCol(1, (SQLINTEGER&)site_id1);
    stmt1.bindCol(2, (SQLCHAR*)&element_symbol1[0], SQLVARCHAR_BUFSZ);
    stmt1.bindCol(3, (SQLINTEGER&)site_id2);
    stmt1.bindCol(4, (SQLCHAR*)&element_symbol2[0], SQLVARCHAR_BUFSZ);
    
    try { stmt1.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
    

    while(stmt1.fetch()) 
      {
#if 0 
    std::cerr << "build_bond_adjacency_map():" << std::endl;
    std::cerr << "   Fetched site_id1=" << site_id1 << std::endl;
    std::cerr << "   Fetched element_symbol1=" << element_symbol1 << std::endl;
    std::cerr << "   Fetched site_id2=" << site_id2 << std::endl;
    std::cerr << "   Fetched element_symbol2=" << element_symbol2 << std::endl;
#endif

      SITE_REP site_rep1(site_id1, element_symbol1);
      SITE_REP site_rep2(site_id2, element_symbol2);

      this->bond_adjacency_map.insert(std::make_pair(site_id1, site_rep2));
      this->bond_adjacency_map.insert(std::make_pair(site_id2, site_rep1));

      } //end while each fetch of bonded sites
        
    } //end build_bond_adjacency_map()


//****************************************************************************
  void TransformSysParams::create_rigidsites_params()
    {
    std::cout << "Creating Rigidsites groupings" << std::endl; //VERBOSE
    std::cout << "   ...fetching bond lists from DB" << std::endl; //VERBOSE
    
    /////////////////////////////////////////////////////////////////////////////////////
    // First collect the bonds and bond parameters between rigid sites
    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[3*SQLCMD_BUFSZ];
    snprintf(sqlcmd1, 3*SQLCMD_BUFSZ, "select distinct mstd1.site_id, mstd2.site_id, mptd.param_value "
             "  from mdsystem.sitetuplelist as mstl, "
             "       mdsystem.sitetupledata as mstd1, "
             "       mdsystem.sitetupledata as mstd2, "
             "       mdsystem.paramtuplelist as mptl, "
             "       mdsystem.paramtuplespec as mpts, "
             "       mdsystem.paramtupledata as mptd "
             " where mstl.sys_id=%d and "
             "       mptd.sys_id=%d and "
             "       mstd1.sys_id=%d and "
             "       mstd2.sys_id=%d and "
             "       mptd.sys_id=%d and "
             "       mpts.sys_id=%d and "
             "       mstl.site_tuple_desc='StdHarmonicBondForce_sites' and "
             "       mptl.param_tuple_desc='StdHarmonicBondForce_params' and "
             "       mptd.param_tuple_list_id=mptl.param_tuple_list_id and "
             "       mpts.param_tuple_list_id=mptl.param_tuple_list_id and "
             "       mpts.param_name='r0' and "
             "       mptd.param_id=mpts.param_id and "
             "       mstd1.site_tuple_list_id=mstl.site_tuple_list_id and "
             "       mstd2.site_tuple_list_id=mstl.site_tuple_list_id and "
             "       mstd1.site_tuple_id=mstd2.site_tuple_id and "
             "       mptd.param_tuple_id=mstd1.site_tuple_id and "
             "       mstd1.site_id < mstd2.site_id "
             " order by mstd1.site_id FOR READ ONLY WITH UR; ",
             this->sys_id, this->sys_id, this->sys_id, this->sys_id, this->sys_id, this->sys_id);
    stmt1.prepare(sqlcmd1);

    int map_site_id1;
    int map_site_id2;
    char param_val[SQLVARCHAR_BUFSZ];
    stmt1.bindCol(1, (SQLINTEGER&)map_site_id1);
    stmt1.bindCol(2, (SQLINTEGER&)map_site_id2);
    stmt1.bindCol(3, (SQLCHAR*)&param_val[0], SQLVARCHAR_BUFSZ);
    
    try { stmt1.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

    std::map<std::pair<int,int>, std::string> site_pair_vals_map;
    std::map<std::pair<int,int>, double> site_pair_vals_dbl_map;
    
    while(stmt1.fetch()) 
      {
  if(this->runtimeparams->verbosity >= 5)  //VERBOSE
  { std::cerr << "Fetched() - site1=" << map_site_id1 << " site2=" << map_site_id2 << " paramval=" << param_val << std::endl; }

        std::pair<int,int> map_pair_key; 
        if (map_site_id1 < map_site_id2)
          {
          map_pair_key = std::make_pair(map_site_id1, map_site_id2);
          }
        else
          {
          map_pair_key = std::make_pair(map_site_id2, map_site_id1);
          }
        site_pair_vals_map.insert(std::make_pair(map_pair_key, std::string(param_val)));
      }
        
    /////////////////////////////////////////////////////////////////////////////////////
    // Collect string parameters, feed into compiler and compue double-values
    // NOTE: assumes 2 parsings of site_pair_vals_map in a row will be in the same order
    std::vector<std::string> param_strings;

    std::map<std::pair<int,int>, std::string>::const_iterator paramval_itr1 = site_pair_vals_map.begin();
    std::map<std::pair<int,int>, std::string>::const_iterator paramval_end1 = site_pair_vals_map.end();
    for (; paramval_itr1 != paramval_end1; ++paramval_itr1)
      {
      param_strings.push_back(paramval_itr1->second);
      }

    std::vector<double> ret_dbl_vec = compile_symbolic_double(param_strings);

    int dbl_vec_index = 0;
    std::map<std::pair<int,int>, std::string>::const_iterator paramval_itr2 = site_pair_vals_map.begin();
    std::map<std::pair<int,int>, std::string>::const_iterator paramval_end2 = site_pair_vals_map.end();
    for (; paramval_itr2 != paramval_end2; ++paramval_itr2)
      {
      double param_dbl_val = ret_dbl_vec.at(dbl_vec_index);
      site_pair_vals_dbl_map.insert(std::make_pair(paramval_itr2->first, param_dbl_val));
      dbl_vec_index++;
      }

    /////////////////////////////////////////////////////////////////////////////////////
    // Insert groups of heavy atoms into tables
    std::cout << "   ...creating heavy-atom groups" << std::endl; //VERBOSE
    db2::StmtHandle stmt2(this->db2conn);
    char sqlcmd2[SQLCMD_BUFSZ];
    snprintf(sqlcmd2, SQLCMD_BUFSZ, "select site_id "
             " from mdsystem.site where sys_id=%d and "
             " component_name NOT LIKE 'HOH' AND "
             " component_name NOT LIKE 'NAP' AND "
             " element_symbol NOT LIKE 'H' " 
             " order by site_id FOR READ ONLY WITH UR",
             this->sys_id);
    stmt2.prepare(sqlcmd2);
    
    int site_id;
    stmt2.bindCol(1, (SQLINTEGER&)site_id);
    try { stmt2.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
  
    while(stmt2.fetch()) 
      {

  if(this->runtimeparams->verbosity >= 5)  //VERBOSE
  { std::cerr << "Fetched Heavy Atom - siteid=" << site_id << std::endl; }

          int number_of_hydrogens = 0;
          double param_for_heavysite[3]; 
          int hydrogen_site_for_heavysite[3]; 
          for (int k=0; k < 3; k++) { hydrogen_site_for_heavysite[k]=INT_MAX; }
          for (int k=0; k < 3; k++) { param_for_heavysite[k]=0; }

	        typedef std::multimap<int, SITE_REP>::const_iterator MMI;
	        std::pair<MMI,MMI> adj_sites = this->bond_adjacency_map.equal_range(site_id);
        
          if (adj_sites.first == adj_sites.second)
            {
  if(this->runtimeparams->verbosity >= 5)  //VERBOSE
  { std::cerr << "Couldn't find crumb in adjacency map: site_id=" << site_id << std::endl; }
            
            continue;
            }

	        for (MMI adj_i = adj_sites.first; adj_i != adj_sites.second; ++adj_i) 
            {
            int rigid_site_id = adj_i->second.site_id;
            std::string bonded_elem_symbol = adj_i->second.element_symbol;
  
            if (bonded_elem_symbol.compare("H")==0)
              {                
              number_of_hydrogens++;
              
              std::pair<int,int> map_pair_key; 

              if (site_id < rigid_site_id)
                {
                  map_pair_key = std::make_pair( site_id, rigid_site_id );
                }
              else 
                {
                  map_pair_key = std::make_pair( rigid_site_id, site_id );
                }
              std::map<std::pair<int,int>, double>::const_iterator find_itr =
                          site_pair_vals_dbl_map.find(map_pair_key);
                
              if (find_itr != site_pair_vals_dbl_map.end())
                {
                param_for_heavysite[number_of_hydrogens-1] = find_itr->second;
                hydrogen_site_for_heavysite[number_of_hydrogens-1] = rigid_site_id;
                }
              else
                { 
                std::cerr << "ERROR: No rEQ found for sites: (" << site_id << "," 
                          << rigid_site_id << ")" << std::endl;
                }
              } //end if Hydrogen in list
            } //end for each matching hydrogen

          if (number_of_hydrogens == 1) 
            {
            //Fill the DB representation 
            RIGID2_DB_ENTRY new_r2_entry(site_id, 
                                         hydrogen_site_for_heavysite[0],
                                         param_for_heavysite[0]);
            this->rigid2_dbentries.push_back(new_r2_entry);

            //Fill internal representation
            this->site_frag_id_map.insert(std::make_pair(site_id, this->num_fragments));
            this->site_frag_id_map.insert(std::make_pair(hydrogen_site_for_heavysite[0], this->num_fragments));
            std::vector<int> hydrogen_list;
            hydrogen_list.push_back(site_id);
            hydrogen_list.push_back(hydrogen_site_for_heavysite[0]);
            this->frag_id_sites_map.insert(std::make_pair(this->num_fragments, hydrogen_list));
            this->num_fragments++;
            }
          else if (number_of_hydrogens == 2)
            {
            //Fill the DB representation 
            RIGID3_DB_ENTRY new_r3_entry(site_id, 
                                         hydrogen_site_for_heavysite[0],
                                         param_for_heavysite[0],
                                         hydrogen_site_for_heavysite[1],
                                         param_for_heavysite[1]);
            this->rigid3_dbentries.push_back(new_r3_entry);
              
            //Fill internal representation (Which is differen than DB rep)
            this->site_frag_id_map.insert(std::make_pair(site_id, this->num_fragments));
            this->site_frag_id_map.insert(std::make_pair(hydrogen_site_for_heavysite[0], this->num_fragments));
            this->site_frag_id_map.insert(std::make_pair(hydrogen_site_for_heavysite[1], this->num_fragments));
            std::vector<int> hydrogen_list;
            hydrogen_list.push_back(site_id);
            hydrogen_list.push_back(hydrogen_site_for_heavysite[0]);
            hydrogen_list.push_back(hydrogen_site_for_heavysite[1]);
            this->frag_id_sites_map.insert(std::make_pair(this->num_fragments, hydrogen_list));
            this->num_fragments++;
            }
          else if (number_of_hydrogens == 3)
            {
            //Fill the DB representation 
            RIGID4_DB_ENTRY new_r4_entry(site_id, 
                                         hydrogen_site_for_heavysite[0],
                                         param_for_heavysite[0],
                                         hydrogen_site_for_heavysite[1],
                                         param_for_heavysite[1],
                                         hydrogen_site_for_heavysite[2],
                                         param_for_heavysite[2]);
            this->rigid4_dbentries.push_back(new_r4_entry);
      
            //Fill internal representation
            this->site_frag_id_map.insert(std::make_pair(site_id, this->num_fragments));
            this->site_frag_id_map.insert(std::make_pair(hydrogen_site_for_heavysite[0], this->num_fragments));
            this->site_frag_id_map.insert(std::make_pair(hydrogen_site_for_heavysite[1], this->num_fragments));
            this->site_frag_id_map.insert(std::make_pair(hydrogen_site_for_heavysite[2], this->num_fragments));
            std::vector<int> hydrogen_list;
            hydrogen_list.push_back(site_id);
            hydrogen_list.push_back(hydrogen_site_for_heavysite[0]);
            hydrogen_list.push_back(hydrogen_site_for_heavysite[1]);
            hydrogen_list.push_back(hydrogen_site_for_heavysite[2]);
            this->frag_id_sites_map.insert(std::make_pair(this->num_fragments, hydrogen_list));
            this->num_fragments++;
            }
          else if (number_of_hydrogens >= 4)
            {
            std::cerr << "ERROR: Found heavy atom (" << site_id << ") with >= 4 hydrogens connected." 
                      << std::endl;
            }
          else
            {
  if(this->runtimeparams->verbosity >= 5)  //VERBOSE
  { std::cerr << "Added crumb, site_id=" << site_id << std::endl; }

            this->crumb_list.push_back(site_id);
            }

      } //end while each return of stmt2

    } //end create_rigidsites_params()


//****************************************************************************
// This function assumes the internal site IDs have been assigned, and outputs
// the rigid2,3,4 sites rows to the DB
  void TransformSysParams::write_rigidsites_params()
    {
    std::cout << "Writing Rigidsite groupings to DB" << std::endl; //VERBOSE

    if(!this->runtimeparams->doShakeRattleProtein) 
      {
      return;
      }

    int rigid2_sz = this->rigid2_dbentries.size();
    int *r2_heavysite = new int[rigid2_sz];
    int *r2_h1 = new int[rigid2_sz];
    double *r2_h1_param = new double[rigid2_sz];
    int r2_cnt = 0;

    ///////////////////////////////
    std::vector<RIGID2_DB_ENTRY>::const_iterator r2_itr = this->rigid2_dbentries.begin();
    std::vector<RIGID2_DB_ENTRY>::const_iterator r2_end = this->rigid2_dbentries.end();
    for (; r2_itr != r2_end; ++r2_itr)
      {

      std::map<int,int>::const_iterator find_site1 = this->ext_int_site_map.find(r2_itr->heavy_site);
      std::map<int,int>::const_iterator find_hydrogen1 = this->ext_int_site_map.find(r2_itr->hydrogen_site1);
      if ( (find_site1 == this->ext_int_site_map.end())     ||
           (find_hydrogen1 == this->ext_int_site_map.end()) 
         ) {
            std::cerr << "ERROR: Could not find Internal SiteID for External SiteID " 
                      << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
        }
      else 
        {
        r2_heavysite[r2_cnt] = find_site1->second;
        r2_h1[r2_cnt] = find_hydrogen1->second;
        r2_h1_param[r2_cnt] = r2_itr->param_hydrogen_site1;
        r2_cnt++;
        }

      } //end for each RIGID2 entry
 
    if (rigid2_sz > 0)
      {
      db2::StmtHandle input_stmt1(this->db2conn);
      char input_sqlcmd1[SQLCMD_BUFSZ];
      snprintf(input_sqlcmd1, SQLCMD_BUFSZ, "insert into mdsystem.rigid2_sites "
                           " ( pst_id, heavy_site, "
                           " hydrogen_site1, hydrogen_site1_rEQ ) "
                           " values (%d,?,?,?) ", this->pst_id);
      input_stmt1.prepare(input_sqlcmd1);

      input_stmt1.bindParamArray(1, (SQLINTEGER*)r2_heavysite, rigid2_sz);
      input_stmt1.bindParamArray(2, (SQLINTEGER*)r2_h1, rigid2_sz);
      input_stmt1.bindParamArray(3, (SQLDOUBLE*)r2_h1_param, rigid2_sz);

      try { input_stmt1.execute(); }
      catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
      }

    delete [] r2_heavysite;
    delete [] r2_h1;
    delete [] r2_h1_param;

    int rigid3_sz = this->rigid3_dbentries.size();
    int *r3_heavysite = new int[rigid3_sz];
    int *r3_h1 = new int[rigid3_sz];
    double *r3_h1_param = new double[rigid3_sz];
    int *r3_h2 = new int[rigid3_sz];
    double *r3_h2_param = new double[rigid3_sz];
    int r3_cnt = 0;

    ///////////////////////////////
    std::vector<RIGID3_DB_ENTRY>::const_iterator r3_itr = this->rigid3_dbentries.begin();
    std::vector<RIGID3_DB_ENTRY>::const_iterator r3_end = this->rigid3_dbentries.end();
    for (; r3_itr != r3_end; ++r3_itr)
      {

      std::map<int,int>::const_iterator find_site1 = this->ext_int_site_map.find(r3_itr->heavy_site);
      std::map<int,int>::const_iterator find_hydrogen1 = this->ext_int_site_map.find(r3_itr->hydrogen_site1);
      std::map<int,int>::const_iterator find_hydrogen2 = this->ext_int_site_map.find(r3_itr->hydrogen_site2);
      if ( (find_site1 == this->ext_int_site_map.end())     ||
           (find_hydrogen1 == this->ext_int_site_map.end()) || 
           (find_hydrogen2 == this->ext_int_site_map.end()) 
         ) {
            std::cerr << "ERROR: Could not find Internal SiteID for External SiteID " 
                      << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
        }
      else 
        {
        r3_heavysite[r3_cnt] = find_site1->second;
        r3_h1[r3_cnt] = find_hydrogen1->second;
        r3_h1_param[r3_cnt] = r3_itr->param_hydrogen_site1;
        r3_h2[r3_cnt] = find_hydrogen2->second;
        r3_h2_param[r3_cnt] = r3_itr->param_hydrogen_site2;
        r3_cnt++;
        }
      } //end for each RIGID3 entry
 
    if (rigid3_sz > 0)
      {
      db2::StmtHandle input_stmt2(this->db2conn);
      char input_sqlcmd2[SQLCMD_BUFSZ];
      snprintf(input_sqlcmd2, SQLCMD_BUFSZ, "insert into mdsystem.rigid3_sites "
                           " ( pst_id, heavy_site, "
                           " hydrogen_site1, hydrogen_site1_rEQ,  "
                           " hydrogen_site2, hydrogen_site2_rEQ ) "
                           " values (%d,?,?,?,?,?) ", this->pst_id);
      input_stmt2.prepare(input_sqlcmd2);

      input_stmt2.bindParamArray(1, (SQLINTEGER*)r3_heavysite, rigid3_sz);
      input_stmt2.bindParamArray(2, (SQLINTEGER*)r3_h1, rigid3_sz);
      input_stmt2.bindParamArray(3, (SQLDOUBLE*)r3_h1_param, rigid3_sz);
      input_stmt2.bindParamArray(4, (SQLINTEGER*)r3_h2, rigid3_sz);
      input_stmt2.bindParamArray(5, (SQLDOUBLE*)r3_h2_param, rigid3_sz);

      try { input_stmt2.execute(); }
      catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
      }


    delete [] r3_heavysite;
    delete [] r3_h1;
    delete [] r3_h1_param;
    delete [] r3_h2;
    delete [] r3_h2_param;

    int rigid4_sz = this->rigid4_dbentries.size();
    int *r4_heavysite = new int[rigid4_sz];
    int *r4_h1 = new int[rigid4_sz];
    double *r4_h1_param = new double[rigid4_sz];
    int *r4_h2 = new int[rigid4_sz];
    double *r4_h2_param = new double[rigid4_sz];
    int *r4_h3 = new int[rigid4_sz];
    double *r4_h3_param = new double[rigid4_sz];
    int r4_cnt = 0;

    ///////////////////////////////
    std::vector<RIGID4_DB_ENTRY>::const_iterator r4_itr = this->rigid4_dbentries.begin();
    std::vector<RIGID4_DB_ENTRY>::const_iterator r4_end = this->rigid4_dbentries.end();
    for (; r4_itr != r4_end; ++r4_itr)
      {

      std::map<int,int>::const_iterator find_site1 = this->ext_int_site_map.find(r4_itr->heavy_site);
      std::map<int,int>::const_iterator find_hydrogen1 = this->ext_int_site_map.find(r4_itr->hydrogen_site1);
      std::map<int,int>::const_iterator find_hydrogen2 = this->ext_int_site_map.find(r4_itr->hydrogen_site2);
      std::map<int,int>::const_iterator find_hydrogen3 = this->ext_int_site_map.find(r4_itr->hydrogen_site3);
      if ( (find_site1 == this->ext_int_site_map.end())     ||
           (find_hydrogen1 == this->ext_int_site_map.end()) || 
           (find_hydrogen2 == this->ext_int_site_map.end()) ||
           (find_hydrogen3 == this->ext_int_site_map.end()) 
         ) {
            std::cerr << "ERROR: Could not find Internal SiteID for External SiteID " 
                      << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
        }
      else 
        {
        r4_heavysite[r4_cnt] = find_site1->second;
        r4_h1[r4_cnt] = find_hydrogen1->second;
        r4_h1_param[r4_cnt] = r4_itr->param_hydrogen_site1;
        r4_h2[r4_cnt] = find_hydrogen2->second;
        r4_h2_param[r4_cnt] = r4_itr->param_hydrogen_site2;
        r4_h3[r4_cnt] = find_hydrogen3->second;
        r4_h3_param[r4_cnt] = r4_itr->param_hydrogen_site3;
        r4_cnt++;
        }
      } //end for each RIGID4 entry

    if (rigid4_sz > 0)
      {
      db2::StmtHandle input_stmt3(this->db2conn);
      char input_sqlcmd3[SQLCMD_BUFSZ];
      snprintf(input_sqlcmd3, SQLCMD_BUFSZ, "insert into mdsystem.rigid4_sites "
                          " ( pst_id, heavy_site, "
                          " hydrogen_site1, hydrogen_site1_rEQ,  "
                          " hydrogen_site2, hydrogen_site2_rEQ,  "
                          " hydrogen_site3, hydrogen_site3_rEQ ) "
                           " values (%d,?,?,?,?,?,?,?) ", this->pst_id);
      input_stmt3.prepare(input_sqlcmd3);

      input_stmt3.bindParamArray(1, (SQLINTEGER*)r4_heavysite, rigid4_sz);
      input_stmt3.bindParamArray(2, (SQLINTEGER*)r4_h1, rigid4_sz);
      input_stmt3.bindParamArray(3, (SQLDOUBLE*)r4_h1_param, rigid4_sz);
      input_stmt3.bindParamArray(4, (SQLINTEGER*)r4_h2, rigid4_sz);
      input_stmt3.bindParamArray(5, (SQLDOUBLE*)r4_h2_param, rigid4_sz);
      input_stmt3.bindParamArray(6, (SQLINTEGER*)r4_h3, rigid4_sz);
      input_stmt3.bindParamArray(7, (SQLDOUBLE*)r4_h3_param, rigid4_sz);

      try { input_stmt3.execute(); }
      catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
      }


    delete [] r4_heavysite;
    delete [] r4_h1;
    delete [] r4_h1_param;
    delete [] r4_h2;
    delete [] r4_h2_param;
    delete [] r4_h3;
    delete [] r4_h3_param;

    } //end write_rigidsites_params()



//****************************************************************************
  void TransformSysParams::create_rigidgroup_fragmentation()
    {
    std::cout << "Creating Fragments based on Rigidgroups" << std::endl; //VERBOSE
    
    /////////////////////////////////////////////////////////////////////////////////////
    // NOTE: This function assumed that create_rigidsites_params() already placed
    // every rigidsite into its own fragment in the internal structs 
    // this->site_frag_id_map  and this->frag_id_sites_map

    /////////////////////////////////////////////////////////////////////////////////////
    // For each crumb, find its bonded atoms, 
    // run through list of all fragments (rigid_sites), find the fragments
    // containing these bonded atoms, and add crumb to fragment with smallest radius 
    //
    std::cout << "   ...adding crumbs to rigidsites" << std::endl; //VERBOSE

    int max_sites_in_frag = 0;
    int max_radius = 0;

    std::set<int> added_crumbs;

    std::vector<int>::const_iterator crumb_itr = this->crumb_list.begin();
    std::vector<int>::const_iterator crumb_end = this->crumb_list.end();
    for (; crumb_itr != crumb_end; ++crumb_itr)
      {
      int crumb_site = *crumb_itr;
      int other_crumb_site = -1;
#if 0
  std::cerr << " crumb_site=" << crumb_site << std::endl;
#endif

      std::set<int>::const_iterator find_added_crumb = added_crumbs.find(crumb_site);
      if (find_added_crumb != added_crumbs.end())
        {
#if 0
  std::cerr << "    already added " << crumb_site << std::endl;
#endif
        continue;
        }

      //Foreach atom bonded to this crumb
	    typedef std::multimap<int, SITE_REP>::const_iterator MMI;
	    std::pair<MMI,MMI> adj_sites = this->bond_adjacency_map.equal_range(crumb_site);
      if (adj_sites.first == adj_sites.second)
        {
        std::cerr << "ERROR: Crumb found with NO Bonds! (site_id " << crumb_site << ") "
                  << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
        continue;
        }

      int frag_index_smallest_radius;
      double smallest_radius = DBL_MAX-1;
      bool made_new_frag = false;
      std::vector<int> smallest_radius_vec;
	    for (MMI adj_i = adj_sites.first; adj_i != adj_sites.second; ++adj_i) 
        {
        int bonded_site = adj_i->second.site_id;
        std::string bonded_elem_symbol = adj_i->second.element_symbol;

        std::set<int>::const_iterator find_bonded_crumb = added_crumbs.find(bonded_site);
#if 0
  std::cerr << "   checking bonded_site=" << bonded_site << std::endl;
#endif

        ///////////////////
        // Check if this crumb is bonded to atoms in an existing fragment
        std::map<int, int>::const_iterator find_frag = this->site_frag_id_map.find(bonded_site);
        if (find_frag != this->site_frag_id_map.end())
          {
          std::map<int, std::vector<int> >::const_iterator find_sites_vec = 
                    this->frag_id_sites_map.find(find_frag->second);
          if (find_sites_vec != this->frag_id_sites_map.end())
            {
            int frag_index = find_sites_vec->first;
            std::vector<int> sites_vec = find_sites_vec->second;
            sites_vec.push_back(crumb_site);
            double radius = guess_radius_of_sites(sites_vec);  

            //FIXME -- Change below to artificially limit fragment size to 4 atoms
            //         (incase core doesnt handle large fragments
            if (radius < smallest_radius)
            //if (radius < smallest_radius && sites_vec.size() < 5 ) //Change here to limit frag size 
              {
#if 0
  std::cerr << "   Looking at EXISTING Fragment - new smallest_radius -- crumb=" << crumb_site << " bonded=" << bonded_site << " rad=" << radius << std::endl;
#endif
              smallest_radius_vec = find_sites_vec->second;
              smallest_radius_vec.push_back(crumb_site);
              smallest_radius = radius;
              frag_index_smallest_radius = frag_index;
              made_new_frag = false;
              }
            
            if (find_sites_vec->second.size() > max_sites_in_frag)
              {
              max_sites_in_frag = find_sites_vec->second.size();
              }
            }
          else
            {
            std::cerr << "ERROR: Internal Fragment ID " << find_frag->second  
                      << " does not have an entry for its sites" 
                      << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
            }
          }
        ///////////////////
        //this crumb bonded to ANOTHER crumb/site that has NOT been added yet
        else if (find_bonded_crumb == added_crumbs.end())
          {
          std::vector<int> crumb_pair;
          crumb_pair.push_back(crumb_site);
          crumb_pair.push_back(bonded_site); 
          double radius = guess_radius_of_sites(crumb_pair);
          
          if (radius < smallest_radius) 
            {
#if 0
  std::cerr << "  Looking at NEW Fragment - new smallest_radius -- crumb=" << crumb_site << " other-crumb=" << bonded_site << " rad=" << radius << std::endl;
#endif

            //We've determined this crumb (A) would do best to be paired with
            //another crumb (B) we haven't seen yet... lets just go one level
            //further to see if crumb B also would do best to pair with crumb A 
            bool pair_with_other_crumb = true;
	          
            std::pair<MMI,MMI> check_adj = this->bond_adjacency_map.equal_range(bonded_site);
	          for (MMI k = check_adj.first; k != check_adj.second; ++k) 
              {
              std::map<int, int>::const_iterator f_itr = this->site_frag_id_map.find(k->second.site_id);
              if (f_itr != this->site_frag_id_map.end())
                {
                std::map<int, std::vector<int> >::const_iterator fs_itr = 
                        this->frag_id_sites_map.find(f_itr->second);
                if (fs_itr != this->frag_id_sites_map.end())
                  {
                  std::vector<int> sites_vec = fs_itr->second;
                  sites_vec.push_back(bonded_site);
                  double other_crumb_radius = guess_radius_of_sites(sites_vec);
                  if (other_crumb_radius < radius)
                    {
                    pair_with_other_crumb = false;
                    }
                  }
                }
              } //end for each adjacent bond to crumb B

            if (pair_with_other_crumb) 
              {
              //pair with other crumb,  add both to list of handled crumbs
              
              smallest_radius_vec.clear();
              smallest_radius_vec.push_back(crumb_site);
              smallest_radius_vec.push_back(bonded_site); 
              added_crumbs.insert(crumb_site); 
              added_crumbs.insert(bonded_site); 
              other_crumb_site = bonded_site;
              smallest_radius = radius;
              frag_index_smallest_radius = this->num_fragments;
              made_new_frag = true;
              }
            }
          }

        } //end for each adjacent/bonded atom

#if 0
  std::cerr << "Final addition of crumb=" << crumb_site << " to fragIndex=" << frag_index_smallest_radius << std::endl;
#endif
        this->site_frag_id_map.insert(std::make_pair(crumb_site, frag_index_smallest_radius));
        if (other_crumb_site >= 0)
          {
          this->site_frag_id_map.insert(std::make_pair(other_crumb_site, frag_index_smallest_radius));
          }
        
        this->frag_id_sites_map.erase(frag_index_smallest_radius);
        this->frag_id_sites_map.insert(std::make_pair(frag_index_smallest_radius, smallest_radius_vec));
        
        if (made_new_frag)
          {
          this->num_fragments++; 
          }
            
        if (smallest_radius_vec.size() > max_sites_in_frag)
          {
          max_sites_in_frag = (smallest_radius_vec.size());
          }

        if (max_radius < smallest_radius)
          {
          max_radius = smallest_radius;
          }
      } //end for each crumb
      
      std::cout << "      -->maximum bond chain length in any fragment: " << max_radius << std::endl; //VERBOSE
      std::cout << "      -->maximum number of atoms in any fragment: " << max_sites_in_frag << std::endl; //VERBOSE
    
      ///////////////////////////////////////////////////////////////////////////////
      // Remap fragment_id's so that "close" fragments (wrt bond chains) 
      // have nearby numbers
      // This is because of an optimization in the core that does not create
      // exclusion lists for fragments with frag_id's beyond some delta apart
      std::cout << "   ...remapping fragment IDs" << std::endl; //VERBOSE

      // map<old_frag_id, new_frag_id>
      std::vector<int> frag_id_remap_vec;
      // set<all already marked frag_ids>
      std::set<int> inserted_fragids;

      /////////////
      // For every pair in the eclusion list, check their fragment ID
      // When not already done, insert fragment IDs next to each other who
      //   have exclusion list pair sites 
      //   (using vector to keep track of proximity)
      std::set<std::pair<int,int> >::const_iterator exc_sites_itr1 = this->excluded_sites.begin();
      std::set<std::pair<int,int> >::const_iterator exc_sites_end1 = this->excluded_sites.end();
      for (; exc_sites_itr1 != exc_sites_end1; ++exc_sites_itr1)
        {
        int site1 = exc_sites_itr1->first;
        int site2 = exc_sites_itr1->second;
        int site1_frag_id = -1;
        int site2_frag_id = -1;

        std::map<int,int>::const_iterator find_site1_fragid = this->site_frag_id_map.find(site1);
        std::map<int,int>::const_iterator find_site2_fragid = this->site_frag_id_map.find(site2);
        if ( (find_site1_fragid == this->site_frag_id_map.end()) ||
             (find_site2_fragid == this->site_frag_id_map.end()) )
          {
          // Maybe its a pair14/eclusion site for water or ions... Ignore this error-case 
          continue;
          }
        else
          {
          site1_frag_id = find_site1_fragid->second;
          site2_frag_id = find_site2_fragid->second;
          }

        if (site1_frag_id == site2_frag_id)
          {
          continue;
          }

        bool site1_inserted = (inserted_fragids.find(site1_frag_id) != inserted_fragids.end());
        bool site2_inserted = (inserted_fragids.find(site2_frag_id) != inserted_fragids.end());


        if (!site1_inserted) 
          {
          frag_id_remap_vec.push_back(site1_frag_id); 
          inserted_fragids.insert(site1_frag_id);
          }
        if (!site2_inserted)
          {
          frag_id_remap_vec.push_back(site2_frag_id); 
          inserted_fragids.insert(site2_frag_id);
          }

        } //end for every excluded site pair

      /////////////
      // Using frag_id_remap_vec, peform remap of all fragment IDs
      //map<old_frag_id, new_frag_id>
      std::map<int,int> frag_id_remap;
      int new_frag_id_cnt = 0;

      std::vector<int>::const_iterator old_fragid_itr = frag_id_remap_vec.begin();
      std::vector<int>::const_iterator old_fragid_end = frag_id_remap_vec.end();
      for (; old_fragid_itr != old_fragid_end; ++old_fragid_itr)
        {
#if 0
  std::cerr << "Remapped old fragID=" << *old_fragid_itr << " to new fragID=" << new_frag_id_cnt << std::endl;
#endif
        frag_id_remap.insert(std::make_pair(*old_fragid_itr, new_frag_id_cnt));
        new_frag_id_cnt++; 
        } //end for each exclusion-list-fragment-id

      // Perhaps not every fragment was included in list earlier (unlikely)
      // so go through and add the rest of the fragments
      if (new_frag_id_cnt != this->num_fragments)
        {
        std::cout << "   ...verifying remap" << std::endl; //VERBOSE

        std::map<int, std::vector<int> >::const_iterator all_frags_itr = this->frag_id_sites_map.begin();
        std::map<int, std::vector<int> >::const_iterator all_frags_end = this->frag_id_sites_map.end();
        for (; all_frags_itr != all_frags_end; ++all_frags_itr)
          {
          bool already_added = false;
          std::vector<int>::const_iterator remap_itr = frag_id_remap_vec.begin();
          std::vector<int>::const_iterator remap_end = frag_id_remap_vec.end();
          for (; remap_itr != remap_end; ++remap_itr)
            {
            if (*remap_itr == all_frags_itr->first)
              {
              already_added = true;
              break;
              }
            }
          if (!already_added)
            {
            frag_id_remap.insert(std::make_pair(all_frags_itr->first, new_frag_id_cnt));
            new_frag_id_cnt++;
            }
          } //for every frag
        } //end if new_frag_id_cnt != num_fragments

      if (new_frag_id_cnt != this->num_fragments)
        {
        std::cerr << "ERROR: Mismatch between original FragIDs and remapped FragIDs"
                  << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
        }
      
      /////////////
      // Now replace site_frag_id_map and frag_id_sites_map with
      // new maps with new frag_ids
      std::map<int, int> new_site_frag_id_map;
      std::map<int, std::vector<int> > new_frag_id_sites_map;
      std::map<int, std::vector<int> > new_molecule_fragment_map;

      std::map<int,int>::const_iterator rep_itr1 = this->site_frag_id_map.begin();
      std::map<int,int>::const_iterator rep_end1 = this->site_frag_id_map.end();
      for (; rep_itr1 != rep_end1; ++rep_itr1)
        {
        std::map<int,int>::const_iterator find_id = frag_id_remap.find(rep_itr1->second);
        if (find_id == frag_id_remap.end())
          {
          std::cerr << "ERROR: Couldn't find remapped FragID for Old FragID " << rep_itr1->second
                    << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
          }
        else
          {
          new_site_frag_id_map.insert(std::make_pair(rep_itr1->first, find_id->second));
          }
        }
      this->site_frag_id_map = new_site_frag_id_map; 
      
      std::map<int,std::vector<int> >::const_iterator rep_itr2 = this->frag_id_sites_map.begin();
      std::map<int,std::vector<int> >::const_iterator rep_end2 = this->frag_id_sites_map.end();
      for (; rep_itr2 != rep_end2; ++rep_itr2)
        {
        std::map<int,int>::const_iterator find_id = frag_id_remap.find(rep_itr2->first);
        if (find_id == frag_id_remap.end())
          {
          std::cerr << "ERROR: Couldn't find remapped FragID for Old FragID " << rep_itr2->first
                    << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
          }
        else
          {
          new_frag_id_sites_map.insert(std::make_pair(find_id->second, rep_itr2->second));
          }
        }
      this->frag_id_sites_map = new_frag_id_sites_map; 
    

      /////////////
      // Determine the maximum frag_id value difference between any 2 fragments 
      // that share sites in an exclusion pair
      int max_fragid_diff = 1;
      std::set<std::pair<int,int> >::const_iterator exc_sites_itr2 = this->excluded_sites.begin();
      std::set<std::pair<int,int> >::const_iterator exc_sites_end2 = this->excluded_sites.end();
      for (; exc_sites_itr2 != exc_sites_end2; ++exc_sites_itr2)
        {
        int site1 = exc_sites_itr2->first;
        int site2 = exc_sites_itr2->second;

        std::map<int,int>::const_iterator find_site1_fragid = this->site_frag_id_map.find(site1);
        std::map<int,int>::const_iterator find_site2_fragid = this->site_frag_id_map.find(site2);
        if ( (find_site1_fragid == this->site_frag_id_map.end()) ||
             (find_site2_fragid == this->site_frag_id_map.end()) )
          {
          // Maybe its a pair14/exclusion site for water or ions... Ignore this error-case 
          continue;
          }
        else
          {
          int site1_frag_id = find_site1_fragid->second;
          int site2_frag_id = find_site2_fragid->second;
          if (site1_frag_id == site2_frag_id)
            {
            continue;
            }
          int diff = abs (site1_frag_id - site2_frag_id);
          if (diff > max_fragid_diff)
            {
            max_fragid_diff = diff;
            }
          }
        } //end for every exclusion list site pair
      std::cout << "      -->max FragID difference between adjacent fragments: " << max_fragid_diff << std::endl; //VERBOSE

      /////////////////
      // write the max-fragid-diff and online parameters to the DB 
      int do_shake_water = 0;
      if (this->runtimeparams->doShakeRattleWater)
        {
        do_shake_water = 1;
        }
      int do_shake_protein = 0;
      if (this->runtimeparams->doShakeRattleProtein)
        {
        do_shake_protein = 1;
        }

      std::vector<std::string> time_conv_vec;
      time_conv_vec.push_back(std::string("TimeConversionFactor"));
      std::vector<double> ret_vec = compile_symbolic_double(time_conv_vec); //should only return 1 value
      double time_conversion_factor = ret_vec.front();

      db2::StmtHandle input_stmt0(this->db2conn);
      char input_sqlcmd0[SQLCMD_BUFSZ];
      snprintf(input_sqlcmd0, SQLCMD_BUFSZ, " insert into mdsystem.online_parameter "
                           " (pst_id, fragid_exclusions_delta, water_method, "
                           "  do_shake_water, do_shake_protein, time_conversion_factor) values "
                           " (%d,%d,'%s',%d,%d,?)", 
                           this->pst_id, 
                           max_fragid_diff, 
                           this->runtimeparams->water_method.c_str(),
                           do_shake_water,
                           do_shake_protein);
      input_stmt0.prepare(input_sqlcmd0);

      input_stmt0.bindParam(1, (SQLDOUBLE&)time_conversion_factor);
      
      try { input_stmt0.execute(); }
      catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }


      ///////////////////////////////////////////////////////////////////////////////
      // Determine what molecule each fragment belongs do... 
      // fill this->molecule_fragment_map
      std::cout << "   ...reading protein fragment molecule types from DB" << std::endl; //VERBOSE

      std::map<int, std::vector<int> >::const_iterator frag_sites_itr = this->frag_id_sites_map.begin();
      std::map<int, std::vector<int> >::const_iterator frag_sites_end = this->frag_id_sites_map.end();
      for (; frag_sites_itr != frag_sites_end; ++frag_sites_itr)
        {
        int frag_id = frag_sites_itr->first;
        std::vector<int> sites_vec = frag_sites_itr->second;
        
        //Find the molecule_instance_id for the sites within this fragment
        //ASSUMES: sites_vec is NON-EMPTY
        //ASSUMES: all sites within fragment belong to the same molecule id
        std::map<int, std::pair<MOLECULE_TYPE,int> >::const_iterator find_mol_id = 
                      this->molecule_type_map.find(sites_vec.front());  
        if (find_mol_id != this->molecule_type_map.end())
          {
          int mol_instance_id = find_mol_id->second.second;

          //Look in molecule_fragment_map to add this frag_id to the vector of fragments
          //belonging to this molecule
          std::map<int, std::vector<int> >::const_iterator mol_frag_itr = 
                    this->molecule_fragment_map.find(mol_instance_id);
          if (mol_frag_itr != this->molecule_fragment_map.end())
            {
            std::vector<int> frag_id_vec = mol_frag_itr->second;
            frag_id_vec.push_back(frag_id);
            this->molecule_fragment_map.erase(mol_instance_id);
            this->molecule_fragment_map.insert(std::make_pair(mol_instance_id, frag_id_vec));
            }
          else
            {
            std::vector<int> frag_id_vec;
            frag_id_vec.push_back(frag_id);
            this->molecule_fragment_map.insert(std::make_pair(mol_instance_id, frag_id_vec));
            }
          
          if (mol_instance_id >= this->num_molecules)
            {
            this->num_molecules = mol_instance_id + 1; //num_molecules should start at 1
            }
          }
        else
          {
          std::cerr << "ERROR: Couldn't find molecule instance ID for site " << *sites_vec.begin() 
                    << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
          }
        } //end for each fragment

 

      ///////////////////////////////////////////////////////////////////////////////
      // Add all Ions, where ION = (!crumb && !rigid_site && !water) 
      //                  or ION = ZERO bonds
      std::cout << "   ...adding ION fragments" << std::endl; //VERBOSE

      std::map<int, std::pair<MOLECULE_TYPE,int> >::const_iterator all_sites_itr1 = this->molecule_type_map.begin();  
      std::map<int, std::pair<MOLECULE_TYPE,int> >::const_iterator all_sites_end1 = this->molecule_type_map.end();  
      for (; all_sites_itr1 != all_sites_end1; ++all_sites_itr1)
        {
        int this_site = all_sites_itr1->first;
        std::multimap<int, SITE_REP>::const_iterator find_bonding = 
                  this->bond_adjacency_map.find(this_site);
        if (find_bonding == this->bond_adjacency_map.end())
          {
          //Found an ION, add it to lists 

          this->site_frag_id_map.insert(std::make_pair(this_site, this->num_fragments));
          std::vector<int> ion_vec;
          ion_vec.push_back(this_site);
          this->frag_id_sites_map.insert(std::make_pair(this->num_fragments, ion_vec));
          std::vector<int> mol_vec;
          mol_vec.push_back(this->num_fragments);
          this->molecule_fragment_map.insert(std::make_pair(this->num_molecules, mol_vec));
          this->num_fragments++; 
          this->num_molecules++; 
          }
        } //end for every site_id

      ///////////////////////////////////////////////////////////////////////////////
      // Add Water atoms to fragment lists
      // (We could treat water specially later on when making types, etc, but
      //  adding it here is more clean and may help organization if we later change
      //  the way fragmentation is done)
      std::cout << "   ...adding water molecule fragments" << std::endl; //VERBOSE

      std::vector<std::vector<int> >::const_iterator water3_itr = this->water3_sites_vec.begin();
      std::vector<std::vector<int> >::const_iterator water3_end = this->water3_sites_vec.end();
      for (; water3_itr != water3_end; ++water3_itr)
        {
        const std::vector<int>& water_sites = *water3_itr;

        std::vector<int>::const_iterator each_site_itr = water_sites.begin();
        std::vector<int>::const_iterator each_site_end = water_sites.end();
        for (; each_site_itr != each_site_end; ++each_site_itr)
          {
          this->site_frag_id_map.insert(std::make_pair(*each_site_itr, this->num_fragments));
          }

        this->frag_id_sites_map.insert(std::make_pair(this->num_fragments, water_sites));
        std::vector<int> mol_vec;
        mol_vec.push_back(this->num_fragments);
        this->molecule_fragment_map.insert(std::make_pair(this->num_molecules, mol_vec));
        this->num_fragments++;
        this->num_molecules++;

        } //end for every water3 

      std::cout << "      -->total number of fragments: " << this->frag_id_sites_map.size() << std::endl; //VERBOSE


      ///////////////////////////////////////////////////////////////////////////////
      //Generate a Fragment type for every site in this->site_frag_id_map
      // placing these in a set will weed out duplicates
      std::cout << "   ...determining unique fragments types" << std::endl; //VERBOSE

      std::map<FRAGMENT_TYPE, int> fragment_types; //<fragment_type, type_id>
      std::map<int, int> frag_id_type_id_map;      //<fragment_id, type_id>
      int frag_type_idx = 0;

      std::map<int, std::vector<int> >::const_iterator frag_itr = this->frag_id_sites_map.begin();
      std::map<int, std::vector<int> >::const_iterator frag_end = this->frag_id_sites_map.end();
      for (; frag_itr != frag_end; ++frag_itr)
        {
        //Check to see if this fragment was already seen in set.. if not, add it
        //ifso, assign this fragment to use the type ID from what's already been seen
        FRAGMENT_TYPE this_frag_type = get_fragment_type(frag_itr->second, "NONE");
        std::map<FRAGMENT_TYPE, int>::const_iterator find_type =
                fragment_types.find(this_frag_type);
        if (find_type != fragment_types.end())
          {
          frag_id_type_id_map.insert(std::make_pair(frag_itr->first, find_type->second));
          }
        else
          {
          fragment_types.insert(std::make_pair(this_frag_type, frag_type_idx));
          frag_id_type_id_map.insert(std::make_pair(frag_itr->first, frag_type_idx));
          frag_type_idx++;
          }
        }


      ///////////////////////////////////////////////////////////////////////////////
      // Foreach molecule, Foreach fragment, determine its type, and pull out all sites
      // Determine/Write external site_id --> internal site_id mappings
      // Write table MDSYSTEM.fragment_map using internal site_ids
      std::cout << "   ...remapping external site_ids to internal site_ids" << std::endl; //VERBOSE

      // Keep array of rows that'll be entered into DB, so we only have to do one
      // DB insert
      int num_sites = this->molecule_type_map.size();
      int site_row_cnt = 0;
      int *ext_site_arr = new int[num_sites];
      int *int_site_arr = new int[num_sites];

      int fragmap_row_cnt = 0;
      int *fragmap_firstsite_arr = new int[this->num_fragments];
      int *fragmap_type_arr = new int[this->num_fragments];
      int *fragmap_mol_id_arr = new int[this->num_fragments];
      int *fragmap_idx_in_mol_arr = new int[this->num_fragments];

      int internal_site_id = 0;

      std::map<int, std::vector<int> >::const_iterator mol_frag_itr = this->molecule_fragment_map.begin(); 
      std::map<int, std::vector<int> >::const_iterator mol_frag_end = this->molecule_fragment_map.end(); 
      for (; mol_frag_itr != mol_frag_end; ++mol_frag_itr)
        {
        int molecule_id = mol_frag_itr->first;
        int frag_idx_in_mol = 0;
#if 0
  std::cerr << " Mapping molecule_id=" << molecule_id << " num_frags=" << mol_frag_itr->second.size() << std::endl;
#endif
        std::vector<int> frags_in_mol = mol_frag_itr->second;
        std::vector<int>::const_iterator frag_itr = frags_in_mol.begin();
        std::vector<int>::const_iterator frag_end = frags_in_mol.end();
        for (; frag_itr != frag_end; ++frag_itr)
          {
          int this_frag_id = *frag_itr;
#if 0    
  std::cerr << "   Fragment_id= " << this_frag_id << std::endl;
#endif
          /////////////////
          //First -- Re-number all sites within this fragment
          int internal_first_site_id; //internal first site_id
          std::map<int, std::vector<int> >::const_iterator frag_sites_itr = 
              this->frag_id_sites_map.find(this_frag_id);
          if (frag_sites_itr != this->frag_id_sites_map.end())
            {
            const std::vector<int>& frag_vec = frag_sites_itr->second;
#if 0    
  std::cerr << "   Number of sites in fragment= " << frag_vec.size() << std::endl;
#endif
            std::vector<int>::const_iterator site_itr = frag_vec.begin();
            std::vector<int>::const_iterator site_end = frag_vec.end();
            for (; site_itr != site_end; ++site_itr)
              {
              //FIXME FIXME FIXME FIXME FIXME FIXME
              int int_site_id = internal_site_id++;  //DO EXTERNAL/INTERNAL SITEID REMAP HERE!
              //int int_site_id = *site_itr;  //DO EXTERNAL/INTERNAL SITEID REMAP HERE!
              int ext_site_id = *site_itr;
             
              this->ext_int_site_map.insert(std::make_pair(ext_site_id, int_site_id));

              if (site_itr == frag_vec.begin()) 
                {
                internal_first_site_id = int_site_id;
                }
#if 0
  std::cerr << "Inserted ext site: " << ext_site_id << " (Internal site=" << int_site_id << ")" << std::endl;
#endif
              ext_site_arr[site_row_cnt] = ext_site_id;
              int_site_arr[site_row_cnt] = int_site_id;
              site_row_cnt++;

              } //end for each external site_id within fragment
            }
          else 
            {
            std::cerr << "ERROR: Internal Fragment ID " << this_frag_id 
                      << " does not have an entry for its sites " 
                      << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
            }

          /////////////////
          //Next -- determine fragment's type and write out table MDSYSTEM.fragment_map 

          int frag_type_id=0;
          std::map<int, int>::const_iterator find_type_id = frag_id_type_id_map.find(this_frag_id);
          if (find_type_id != frag_id_type_id_map.end())
            {
            frag_type_id = find_type_id->second;
            }
          else
            {
            std::cerr << "ERROR: Could not find Fragment Type ID for " 
                      << "Fragment ID " << this_frag_id 
                      << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
            }
#if 0
  std::cerr << " Added internal first site=" << internal_first_site_id << std::endl;
#endif
          fragmap_firstsite_arr[fragmap_row_cnt] = internal_first_site_id;
          fragmap_type_arr[fragmap_row_cnt] = frag_type_id;
          fragmap_mol_id_arr[fragmap_row_cnt] = molecule_id;
          fragmap_idx_in_mol_arr[fragmap_row_cnt] = frag_idx_in_mol;
          fragmap_row_cnt++;

          frag_idx_in_mol++; 
          } //end for each fragment

        } //end for each molecule

      /////////////////
      // Enter the External-Internal mapping into DB
      db2::StmtHandle input_stmt1(this->db2conn);
      char input_sqlcmd1[SQLCMD_BUFSZ];
      snprintf(input_sqlcmd1, SQLCMD_BUFSZ, " insert into mdsystem.internal_external_siteids "
                           " (pst_id, ext_site_id, int_site_id) values (%d,?,?)", this->pst_id);
      input_stmt1.prepare(input_sqlcmd1);
      
      input_stmt1.bindParamArray(1, (SQLINTEGER*)ext_site_arr, num_sites);
      input_stmt1.bindParamArray(2, (SQLINTEGER*)int_site_arr, num_sites);
      try { input_stmt1.execute(); }
      catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

      /////////////////
      // Enter the Fragment map into DB 
      db2::StmtHandle input_stmt2(this->db2conn);
      char input_sqlcmd2[SQLCMD_BUFSZ];
      snprintf(input_sqlcmd2, SQLCMD_BUFSZ, " insert into mdsystem.fragment_map "
                           " (pst_id, start_site, fragment_type_id, molecule_id, "
                           " frag_index_in_molecule) values (%d,?,?,?,?)", this->pst_id); 
      input_stmt2.prepare(input_sqlcmd2);
             
      input_stmt2.bindParamArray(1, (SQLINTEGER*)fragmap_firstsite_arr, this->num_fragments);
      input_stmt2.bindParamArray(2, (SQLINTEGER*)fragmap_type_arr, this->num_fragments);
      input_stmt2.bindParamArray(3, (SQLINTEGER*)fragmap_mol_id_arr, this->num_fragments);
      input_stmt2.bindParamArray(4, (SQLINTEGER*)fragmap_idx_in_mol_arr, this->num_fragments);
      try { input_stmt2.execute(); }
      catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
 
      delete [] ext_site_arr;
      delete [] int_site_arr;
      delete [] fragmap_firstsite_arr;
      delete [] fragmap_type_arr;
      delete [] fragmap_mol_id_arr;
      delete [] fragmap_idx_in_mol_arr;


      ///////////////////////////////////////////////////////////////////////////////
      // Foreach fragment type, output information into DB
      std::cout << "   ...filling in fragment tables in DB" << std::endl; //VERBOSE

      std::map<FRAGMENT_TYPE, int>::const_iterator types_itr = fragment_types.begin(); 
      std::map<FRAGMENT_TYPE, int>::const_iterator types_end = fragment_types.end(); 
      for (; types_itr != types_end; ++types_itr)
        {
        int fragment_type_id = types_itr->second;
        int num_sites = types_itr->first.num_sites;
        int molecule_type = types_itr->first.molecule_type;
        int tagatom_index = types_itr->first.tagatom_index;
        int num_constraints = types_itr->first.num_constraints;
        double extent = types_itr->first.extent;

        //Write this to DB
        db2::StmtHandle input_stmt(this->db2conn);
        char *input_sqlcmd = " insert into mdsystem.fragment_type "
                             " (pst_id, fragment_type_id, num_sites, molecule_type, "
                             " tagatom_index, num_constraints, extent) "
                             " values (?,?,?,?,?,?,?)";
        input_stmt.prepare(input_sqlcmd);
           
        input_stmt.bindParam(1, (SQLINTEGER&)this->pst_id);
        input_stmt.bindParam(2, (SQLINTEGER&)fragment_type_id);
        input_stmt.bindParam(3, (SQLINTEGER&)num_sites);
        input_stmt.bindParam(4, (SQLINTEGER&)molecule_type);
        input_stmt.bindParam(5, (SQLINTEGER&)tagatom_index);
        input_stmt.bindParam(6, (SQLINTEGER&)num_constraints);
        input_stmt.bindParam(7, (SQLDOUBLE&)extent);
        try { input_stmt.execute(); }
        catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

        } //end for each fragment type


    } //end create_rigidgroup_fragmentation()



//****************************************************************************
  void TransformSysParams::create_bondedsites_params()
    {
    std::cout << "Creating Bondedsites Parameters" << std::endl; //VERBOSE

    ////////////////////////////////////////////////
    // Foreach UDF Entry in UDF InvocationTable
    std::cout << "   ...parsing UDF Invocation Table" << std::endl; //VERBOSE

    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[SQLCMD_BUFSZ];
    snprintf(sqlcmd1, SQLCMD_BUFSZ, "SELECT udf_id, site_tuple_list_id, param_tuple_list_id FROM "
            "MDSYSTEM.UDF_Invocation_Table WHERE sys_id=%d order by site_tuple_list_id "
            " FOR READ ONLY WITH UR ", this->sys_id);
    stmt1.prepare(sqlcmd1);

    char udf_id_cstr[SQLVARCHAR_BUFSZ];
    int site_tuple_list_id = -1;
    int param_tuple_list_id = -1;
    stmt1.bindCol(1, (SQLCHAR*)&udf_id_cstr[0], SQLVARCHAR_BUFSZ);
    stmt1.bindCol(2, (SQLINTEGER&)site_tuple_list_id);
    stmt1.bindCol(3, (SQLINTEGER&)param_tuple_list_id);

    try { stmt1.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

    while (stmt1.fetch()) 
      {

      int udf_id = this->udf_registry->get_UDFcode_by_name(std::string(udf_id_cstr));

      if (site_tuple_list_id != -1 &&
          param_tuple_list_id != -1)
        {
        /////////////////////////////////
        // Get the name of this UDF table
        // Only look at site_tuple_desc since param_tuple_desc SHOULD match
        //           (except for tail "_sites" vs. "_params")
        std::cout << "      Processing UDF \"" << udf_id_cstr << "\" (UDF-ID=" 
                  << udf_id << ")" << std::endl; //VERBOSE
#if 0
  std::cerr << "site_tuple_list_id=" << site_tuple_list_id << " param_tuple_list_id=" << param_tuple_list_id << std::endl;
#endif 
        db2::StmtHandle stmt2(this->db2conn);
        char sqlcmd2[SQLCMD_BUFSZ];
        snprintf(sqlcmd2, SQLCMD_BUFSZ, "SELECT site_tuple_desc FROM "
                 " MDSYSTEM.SiteTupleList WHERE sys_id=%d AND site_tuple_list_id=%d "
                 " FOR READ ONLY WITH UR", this->sys_id, site_tuple_list_id);
        stmt2.prepare(sqlcmd2);  
    
        char tuple_descr_cstr[SQLVARCHAR_BUFSZ];
        stmt2.bindCol(1, (SQLCHAR*)&tuple_descr_cstr[0], SQLVARCHAR_BUFSZ);
        
        try { stmt2.execute(); }
        catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

        stmt2.fetch(); //only fetching one value

        std::string sitetuple_tablename(tuple_descr_cstr);
        sitetuple_tablename.erase(sitetuple_tablename.find("_sites",0));
        sitetuple_tablename.append("_TupleList");
        
        /////////////////////////////////
        // Collect all SiteIDs that belong to each tuple_id, 
        //  also convert to internal siteIDs
       
        //map<site_tuple_id, vector<internal site_id's in proper order>>
        std::map<int, std::vector<int> > tuple_id_site_map;
        
        db2::StmtHandle stmt3(this->db2conn);
        char sqlcmd3[SQLCMD_BUFSZ];
        snprintf(sqlcmd3, SQLCMD_BUFSZ, "SELECT site_id, site_tuple_id FROM "
                 " MDSYSTEM.SiteTupleData WHERE sys_id=%d AND site_tuple_list_id=%d "
                 " ORDER BY site_tuple_id, site_ordinal FOR READ ONLY WITH UR",
                 this->sys_id, site_tuple_list_id);
        stmt3.prepare(sqlcmd3);  

        int ext_site_id = -1;
        int site_tuple_id = -1;
        stmt3.bindCol(1, (SQLINTEGER&)ext_site_id);
        stmt3.bindCol(2, (SQLINTEGER&)site_tuple_id);
        try { stmt3.execute(); }
        catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
    
        while (stmt3.fetch())
          {
          std::map<int, std::vector<int> >::const_iterator find_tuple = 
                      tuple_id_site_map.find(site_tuple_id);
          if (find_tuple == tuple_id_site_map.end())
            {
            std::vector<int> new_vec;
            new_vec.push_back(ext_site_id);
            tuple_id_site_map.insert(std::make_pair(site_tuple_id, new_vec));
            }
          else
            {
            std::vector<int> existing_vec = find_tuple->second;
            existing_vec.push_back(ext_site_id);
            tuple_id_site_map.erase(site_tuple_id);
            tuple_id_site_map.insert(std::make_pair(site_tuple_id, existing_vec));
            }
          } //end while fetch() site_id, site_tuple_id
   

        /////////////////////////////////
        // Foreach param_tuple_id, buffer up parmeter's string 'values' to
        //   be compiled into doubles later -- order this according to 'param_id'
        // Note: For every distinct tuple, param_tuple_id MUST match site_tuple_id above
        std::cout << "         -->reading parameter values from DB" << std::endl; //VERBOSE

        //map contains the ordered vector of parameters per param_tuple_id (ordered by param_id)
        std::map<int, PARAM_TUPLE_REP> param_id_nameval_map;
 
        db2::StmtHandle stmt4(this->db2conn);
        char sqlcmd4[SQLCMD_BUFSZ];
        snprintf(sqlcmd4, SQLCMD_BUFSZ, "select "
                 " x.param_name, "
                 " x.param_type, "  //0=int 1=double
                 " y.param_tuple_list_id, "
                 " y.param_tuple_id, "
                 " y.param_id, "
                 " y.param_value "
                 " from mdsystem.paramtuplespec x, mdsystem.paramtupledata y "
                 " where x.sys_id=%d and y.sys_id=%d and "
                 " x.param_tuple_list_id=%d and y.param_tuple_list_id=%d "
                 " and x.param_id=y.param_id "
                 " order by y.param_tuple_id, y.param_id for read only with UR",
                 this->sys_id, this->sys_id, param_tuple_list_id, param_tuple_list_id);
        stmt4.prepare(sqlcmd4);  

        char param_name_cstr[SQLVARCHAR_BUFSZ];
        int param_type = -1; //0=int 1=double
        int param_tuple_list_id = -1;
        int param_tuple_id = -1;
        int param_id = -1;
        char param_value_cstr[SQLVARCHAR_BUFSZ];
        stmt4.bindCol(1, (SQLCHAR*)&param_name_cstr[0], SQLVARCHAR_BUFSZ);
        stmt4.bindCol(2, (SQLINTEGER&)param_type);
        stmt4.bindCol(3, (SQLINTEGER&)param_tuple_list_id);
        stmt4.bindCol(4, (SQLINTEGER&)param_tuple_id);
        stmt4.bindCol(5, (SQLINTEGER&)param_id);
        stmt4.bindCol(6, (SQLCHAR*)&param_value_cstr[0], SQLVARCHAR_BUFSZ);
        try { stmt4.execute(); }
        catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
    
        while (stmt4.fetch())
          {
          std::string param_name_str(param_name_cstr);
          std::string param_value_str(param_value_cstr);

          std::map<int, PARAM_TUPLE_REP>::const_iterator find_id = 
                    param_id_nameval_map.find(param_tuple_id);
          if (find_id == param_id_nameval_map.end())
            {
            PARAM_TUPLE_REP new_rep;
            new_rep.names.push_back(param_name_str);
            new_rep.values.push_back(param_value_str);
            new_rep.types.push_back(param_type);
            param_id_nameval_map.insert(std::make_pair(param_tuple_id, new_rep));
            }
          else
            {
            PARAM_TUPLE_REP new_rep = find_id->second;
            new_rep.names.push_back(param_name_str);
            new_rep.values.push_back(param_value_str);
            new_rep.types.push_back(param_type);
            param_id_nameval_map.erase(param_tuple_id);
            param_id_nameval_map.insert(std::make_pair(param_tuple_id, new_rep));
            }

          } //end while fetch() param_name, param_value, etc
        
        /////////////////////////////////
        // IF we are doing ShakeRattle, then must remove any 
        // StdHarmonicBondForce_TupleList tuples that contain a HYDROGEN... 
        // so we must remove from param_id_nameval_map and tuple_id_site_map
        if( (this->runtimeparams->doShakeRattleProtein) &&
            (sitetuple_tablename.compare("StdHarmonicBondForce_TupleList")==0) )
	        {		
          std::cout << "         -->Processing ShakeRattle version of UDF \"StdHarmonicBondForce\"" << std::endl; //VERBOSE
          std::cout << "            (removing duples with HYDROGEN)" << std::endl; //VERBOSE
          std::vector<int> erase_keys;
           
          std::map<int, std::vector<int> >::const_iterator sitemap_itr = tuple_id_site_map.begin();
          std::map<int, std::vector<int> >::const_iterator sitemap_end = tuple_id_site_map.end();
          for (; sitemap_itr != sitemap_end; ++sitemap_itr)
            {
            std::vector<int>::const_iterator sitesvec_itr = sitemap_itr->second.begin();
            std::vector<int>::const_iterator sitesvec_end = sitemap_itr->second.end();
            for (; sitesvec_itr != sitesvec_end; ++sitesvec_itr)
              {
              std::map<int, std::pair<MOLECULE_TYPE,int> >::const_iterator find_moltype =
                      this->molecule_type_map.find(*sitesvec_itr);
              if (find_moltype != this->molecule_type_map.end())
                {
                if (find_moltype->second.first == PROTEIN_H)
                  {
                  erase_keys.push_back(sitemap_itr->first);

                  } //end if a HYDROGEN
                }
              else
                {
                std::cerr << "ERROR: Could not find Molecule type for SiteID " << *sitesvec_itr 
                          << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
                }
              } //end for each site in this tuple

            }//end for each tuple in tuple_id_site_map

          std::vector<int>::const_iterator erase_itr = erase_keys.begin();
          std::vector<int>::const_iterator erase_end = erase_keys.end();
          for (; erase_itr != erase_end; ++erase_itr)
            {
            tuple_id_site_map.erase(*erase_itr);
            param_id_nameval_map.erase(*erase_itr);
            }

          } //end if doShakeRattleProtein

        /////////////////////////////////
        // Using ordered parameter values, determine DOUBLE values using compiler
        // NOTE: This assumes that between the 2 for-loops below, param_id_nameval_map
        //       will iterate in the exact same order (ie concurrency worry)
        std::cout << "         -->determining parameter values using compiler" << std::endl; //VERBOSE
        
       
        //map<param_tuple_id, vector<doubles of parameters> >
        std::map<int, std::vector<double> > tuple_id_double_map;
        std::vector<std::string> all_vals_vec;

        //First build up strings into 1 vector to be sent to compiler
        std::map<int, PARAM_TUPLE_REP>::const_iterator valstr_itr1 = param_id_nameval_map.begin();
        std::map<int, PARAM_TUPLE_REP>::const_iterator valstr_end1 = param_id_nameval_map.end();
        for (; valstr_itr1 != valstr_end1; ++valstr_itr1)
          {
          std::vector<std::string>::const_iterator inner_itr = valstr_itr1->second.values.begin();
          std::vector<std::string>::const_iterator inner_end= valstr_itr1->second.values.end();
          for (; inner_itr != inner_end; ++inner_itr)
            {
            all_vals_vec.push_back(*inner_itr);
            }
          } //end for each param_tuple_rep

        //Now call compiler with this string vector
        std::vector<double> ret_doubles = compile_symbolic_double(all_vals_vec);

        //Now rebuild a map from compiler's double vector results 
        int vec_pos = 0;
        std::map<int, PARAM_TUPLE_REP>::const_iterator valstr_itr2 = param_id_nameval_map.begin();
        std::map<int, PARAM_TUPLE_REP>::const_iterator valstr_end2 = param_id_nameval_map.end();
        for (; valstr_itr2 != valstr_end2; ++valstr_itr2)
          {
          std::vector<double> tuple_doubles;
          std::vector<std::string>::const_iterator inner_itr = valstr_itr2->second.values.begin();
          std::vector<std::string>::const_iterator inner_end= valstr_itr2->second.values.end();
          for (; inner_itr != inner_end; ++inner_itr)
            {
            tuple_doubles.push_back(ret_doubles.at(vec_pos++));
            }
          tuple_id_double_map.insert(std::make_pair(valstr_itr2->first, tuple_doubles)); //WARNING!
          } //end for each param_tuple_rep
    
        /////////////////////////////////
        // With the known parameter names, and double values, build SQL insert command
        // and insert into each table
        // NOTE: We do not insert params for H-bonds 

        ///////
        //Setup necessary structures to allow us to input values into DB using a single
        //set of array and one SQL call (instead of Many SQL calls)
        int num_sites_in_tuple = -1;
        int num_params_in_tuple = this->udf_registry->get_argcount_by_UDFcode(udf_id);
        int num_tuples = tuple_id_site_map.size();

        db2::StmtHandle stmt5(this->db2conn);
        char sqlcmd5[SQLCMD_BUFSZ];
        snprintf(sqlcmd5, SQLCMD_BUFSZ, "select site_count "
                 " from Registry.UDF_Record where udf_id=%d for read only with UR ",
                 udf_id);
        stmt5.prepare(sqlcmd5);  
        stmt5.bindCol(1, (SQLINTEGER&)num_sites_in_tuple);
        
        try { stmt5.execute(); }
        catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

        stmt5.fetch(); //only fetching one value

        int input_sites[num_sites_in_tuple][num_tuples]; 
        double dbl_params[num_params_in_tuple][num_tuples];
        int int_params[num_params_in_tuple][num_tuples];

        int tuple_id_arr[num_tuples];
        for(int i=0; i < num_tuples; i++)
          {
          tuple_id_arr[i] = i;
          }
    
        //NOTE: Since we're munging things in to an array before we put them into the DB,
        //we must assume the parameter name & type order is the same between all param_tuple_ids
        //This is a safe b/c we keep the ordering all the way until the point when it is
        //placed into the arrays. 
        //Its possible to pull out this (redundant) ordering storage earlier, but may be less-clean
        std::vector<std::string> param_name_order;
        std::vector<int> param_type_order;

        ///////
        // Go through the tuples maps, and rearrange them into arrays so they can
        // be efficiently/quickly entered into the DB
        int loop_tuple_cnt = 0;
        std::map<int, std::vector<int> >::const_iterator tuple_itr = tuple_id_site_map.begin();
        std::map<int, std::vector<int> >::const_iterator tuple_end = tuple_id_site_map.end();
        for (; tuple_itr != tuple_end; ++tuple_itr)
          {

          PARAM_TUPLE_REP tuple_rep;
          std::map<int, PARAM_TUPLE_REP>::const_iterator find_tuple_rep = 
                    param_id_nameval_map.find(tuple_itr->first);
          if (find_tuple_rep != param_id_nameval_map.end())
            {
            tuple_rep = find_tuple_rep->second;
            }
          else
            {
            std::cerr << "ERROR: Could not find ParamTupleID that matches known SiteTupleID " 
                      << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
            }
#if 0
  std::cerr << "Bonded Params Tuples:" << std::endl;
  std::cerr << "   tuple_rep name_sz=" << tuple_rep.names.size() << std::endl;
  std::cerr << "   tuple_rep values_sz=" << tuple_rep.values.size() << std::endl;
  std::cerr << "   tuple_rep types_sz=" << tuple_rep.types.size() << std::endl;
#endif

          //Fill the ordered param names
          if (param_name_order.empty())
            {
            param_name_order = tuple_rep.names;
            }
          if (param_type_order.empty())
            {
            param_type_order = tuple_rep.types;
            }
          
          // Add the sites to the sites array
          // ALSO change from external to internal SITE-IDs here
          int loop_sites_cnt = 0;
          std::vector<int>::const_iterator sites_itr = tuple_itr->second.begin();
          std::vector<int>::const_iterator sites_end = tuple_itr->second.end();
          for (; sites_itr != sites_end; ++sites_itr)
            {
            // CHANGE TO INTERNAL SITE IDs
            int int_site_id = -1;
            std::map<int,int>::const_iterator find_siteid = this->ext_int_site_map.find(*sites_itr);
            if (find_siteid != this->ext_int_site_map.end())
              {
              int_site_id = find_siteid->second;
              }
            else
              {
              std::cerr << "ERROR: Could not find Internal SiteID for External SiteID " 
                        << *sites_itr 
                        << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
              }
            input_sites[loop_sites_cnt][loop_tuple_cnt] = int_site_id;
            loop_sites_cnt++;
            }
    
          // For each parameter double, see its TYPE and enter it into the double array
          //   or the int array
          // NOTE: assumes vectors tuple_id_double_map.second and PARAM_TUPLE_REP.types are PARALLEL
          std::vector<double> tuple_double_params;
          std::map<int, std::vector<double> >::const_iterator find_dbl_vec =
                    tuple_id_double_map.find(tuple_itr->first);
          if (find_dbl_vec != tuple_id_double_map.end())
            {
            tuple_double_params = find_dbl_vec->second;
            }
          else
            {
            std::cerr << "ERROR: Could not find Compiled Parameters for tuple_id " << tuple_itr->first 
                      << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
            }
    
          int loop_params_cnt = 0;
          std::vector<int>::const_iterator param_type_itr = tuple_rep.types.begin();
          std::vector<int>::const_iterator param_type_end = tuple_rep.types.end();
          for (; param_type_itr != param_type_end; ++param_type_itr)
            {

            if ((*param_type_itr) == 0) //INTEGER
              {
              int_params[loop_params_cnt][loop_tuple_cnt] = (int)tuple_double_params.at(loop_params_cnt);
              }
            else if ((*param_type_itr) == 1) //DOUBLE
              {
              dbl_params[loop_params_cnt][loop_tuple_cnt] = tuple_double_params.at(loop_params_cnt);
              }
            else
              {
              std::cerr << "ERROR: Unknown parameter type: " << *param_type_itr 
                        << " - Can only parse 0=INT and 1=DBL "
                        << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
              }

            loop_params_cnt++;
            } //end for each param_type
    
          loop_tuple_cnt++;
          } //end for every tuple to be added for this particular tuple_list_id

        ///////
        // Build SQL insert string 
        std::cout << "         -->filling tables in DB" << std::endl; //VERBOSE

        std::string input_sqlcmd = "insert into MDSYSTEM.";
        std::stringstream val_sqlcmd_ss;
        val_sqlcmd_ss << ") values (" << this->pst_id << ", ? ";
        std::string val_sqlcmd = val_sqlcmd_ss.str();
        input_sqlcmd.append(sitetuple_tablename.c_str());
        input_sqlcmd.append(" ( pst_id, tuple_id ");
        for (int j=0; j < num_sites_in_tuple; ++j)
          {
          std::stringstream ss;
          ss << input_sqlcmd << ", site" << (j+1) << " ";
          input_sqlcmd = ss.str();
          val_sqlcmd.append(", ?");
          }

        std::vector<std::string>::const_iterator name_order_itr = param_name_order.begin();
        std::vector<std::string>::const_iterator name_order_end = param_name_order.end();
        for (; name_order_itr != name_order_end; ++name_order_itr)
          {
          input_sqlcmd.append(", ");
          input_sqlcmd.append(*name_order_itr);
          val_sqlcmd.append(", ?");
          }
        input_sqlcmd.append(val_sqlcmd);
        input_sqlcmd.append(")");
#if 0
        std::cerr << "      Input SQL Statement: \"" << input_sqlcmd.c_str() << "\"" << std::endl;
#endif

        ///////
        // Attach inputs to SQL string and input values 
        int bind_slot = 1;
        db2::StmtHandle input_stmt(this->db2conn);
        input_stmt.prepare(input_sqlcmd.c_str());
        
        input_stmt.bindParamArray(bind_slot++, (SQLINTEGER*)tuple_id_arr, num_tuples);

        for (int m=0; m < num_sites_in_tuple; ++m)
          {
          input_stmt.bindParamArray(bind_slot++, (SQLINTEGER*)input_sites[m], num_tuples);
          }
  
        int type_order_cnt = 0;
        std::vector<int>::const_iterator type_order_itr = param_type_order.begin();
        std::vector<int>::const_iterator type_order_end = param_type_order.end();
        for (; type_order_itr != type_order_end; ++type_order_itr)
          {
          if ((*type_order_itr) == 0) //INTEGER
            {
            input_stmt.bindParamArray(bind_slot++, (SQLINTEGER*)int_params[type_order_cnt++], num_tuples); 
            }
          else if ((*type_order_itr) == 1) //DOUBLE
            {
            input_stmt.bindParamArray(bind_slot++, (SQLDOUBLE*)dbl_params[type_order_cnt++], num_tuples); 
            }
          }

        //YAY! finally enter this into DB
        try { input_stmt.execute(); }
        catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

        } //end if site/param_tuple_list_id != -1
      else
        {
        std::cerr << "ERROR: site/parm_tuple_list_id is -1 !!" 
                  << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
        }


      } //end for each entry in UDF_Invocation_Table


    } //end create_bondedsites_params()




//****************************************************************************
  void TransformSysParams::create_siteinfo_table()
    {
    std::cout << "Reading SiteInfo from DB" << std::endl; //VERBOSE

    //map<site_info_rep, siteinfo_index>
    std::map<SITEINFO_STR_REP, int> unique_siteinfo_map;
    //map<internal site_id, siteinfo_index>
    std::map<int, int> site_to_siteinfo_map;

    //////////////////////////////////////////////
    // If using non-null water method
    // Determine water parameters q1,q2  (o_charge, h_charge)
    std::cout << "   ...determining water method parameters" << std::endl; //VERBOSE

    std::string o_charge;
    std::string h_charge;

    std::string w_method = this->runtimeparams->water_method; 
    if (w_method.compare("NONE")!=0)
      {
      db2::StmtHandle stmt1(this->db2conn);
      char sqlcmd1[SQLCMD_BUFSZ];
      snprintf(sqlcmd1, SQLCMD_BUFSZ, "SELECT q1, q2 from chem.water_parameters where "
              " water_model='%s' FOR READ ONLY WITH UR", w_method.c_str());
      stmt1.prepare(sqlcmd1);
 
      char q1_str[SQLVARCHAR_BUFSZ];
      char q2_str[SQLVARCHAR_BUFSZ];
      q1_str[0] = '\n';
      q2_str[0] = '\n';
      stmt1.bindCol(1, (SQLCHAR*)&q1_str[0], SQLVARCHAR_BUFSZ);
      stmt1.bindCol(2, (SQLCHAR*)&q2_str[0], SQLVARCHAR_BUFSZ);
      try { stmt1.execute(); stmt1.fetch(); } //only need to fetch 1 value
      catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
      stmt1.fetch(); //only need to fetch 1 value

      o_charge = std::string(q1_str).append("*ChargeConversion");
      h_charge = std::string(q2_str).append("*ChargeConversion");
      }

    std::cout << "   ...recalculating and compressing site information" << std::endl; //VERBOSE

    db2::StmtHandle stmt2(this->db2conn);
    char sqlcmd2[SQLCMD_BUFSZ];
    snprintf(sqlcmd2, SQLCMD_BUFSZ, "SELECT site_id, atomic_mass, "
            " charge, element_symbol FROM MDSYSTEM.SITE WHERE sys_id=%d ORDER BY "
            " atomic_mass, charge FOR READ ONLY WITH UR;", this->sys_id);
    stmt2.prepare(sqlcmd2);

    int ext_site_id = -1;
    char atomic_mass_cstr[SQLVARCHAR_BUFSZ];
    char charge_cstr[SQLVARCHAR_BUFSZ];
    char element_symbol_cstr[SQLVARCHAR_BUFSZ];

    stmt2.bindCol(1, (SQLINTEGER&)ext_site_id);
    stmt2.bindCol(2, (SQLCHAR*)&atomic_mass_cstr[0], SQLVARCHAR_BUFSZ);
    stmt2.bindCol(3, (SQLCHAR*)&charge_cstr[0], SQLVARCHAR_BUFSZ);
    stmt2.bindCol(4, (SQLCHAR*)&element_symbol_cstr[0], SQLVARCHAR_BUFSZ);

    try { stmt2.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

    int num_siteinfo_reps = 0;

    //Siteinfo is stored in sparse format... 
    //  (2 tables: <siteid->siteinfo_id> and <siteinfo>)
    //
    //Use std::map<> here to only keep unique siteinfos so it'll be compressed

    while (stmt2.fetch())
      {

      int int_site_id = -1;

      std::string atomic_mass_str(atomic_mass_cstr);
      std::string charge_str(charge_cstr);
      std::string element_symbol_str(element_symbol_cstr);

      std::map<int,int>::const_iterator find_site_map = this->ext_int_site_map.find(ext_site_id);
      if (find_site_map != this->ext_int_site_map.end())
        {
        int_site_id = find_site_map->second;
        }
      else
        {
        std::cerr << "ERROR: Could not find Internal Site ID for External Site ID " << ext_site_id 
                  << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
        }

      if ((charge_str.find("NULL",0)!=std::string::npos) || 
          (charge_str.find("null",0)!=std::string::npos)   )
        {
        if (w_method.compare("NONE")==0)
          {
          std::cerr << "ERROR: There are NULLs assigned to charges in XML, " 
                    << "but no Water Method defined (external site ID=" << ext_site_id << ")" 
                    << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
          }
        if (element_symbol_str.find("H")!=std::string::npos)
          {
          charge_str = h_charge;
          }
        else if (element_symbol_str.find("O")!=std::string::npos)
          {
          charge_str = o_charge;
          }
        else
          {
          std::cerr << "ERROR: Charge parameter is NULL but the site (external ID=" << ext_site_id << ")"
                    << " is not an O or H "
                    << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
          }
        }
      
      /////////////
      // Enter SiteInfo into set to keep track of unique site_infos
      SITEINFO_STR_REP new_rep(atomic_mass_str, charge_str);

      std::map<SITEINFO_STR_REP, int>::const_iterator find_siteinfo = unique_siteinfo_map.find(new_rep); 
      if (find_siteinfo != unique_siteinfo_map.end())
        {
        site_to_siteinfo_map.insert(std::make_pair(int_site_id, find_siteinfo->second));
        }
      else
        {
        unique_siteinfo_map.insert(std::make_pair(new_rep, num_siteinfo_reps));
        site_to_siteinfo_map.insert(std::make_pair(int_site_id, num_siteinfo_reps));
        num_siteinfo_reps++;
        }

      } //end while each fetch() of a siteinfo


    ///////////////////////////////
    // Use compiler to parse all SITEINFO_STR_REP parameter strings 
    // This assumes the order for walking the map remains the same between the 2 loops
    std::cout << "   ...determining parameter values using compiler" << std::endl; //VERBOSE

    std::vector<std::string> all_str_params;

    std::map<SITEINFO_STR_REP, int>::const_iterator unq_siteinfo_itr1 = unique_siteinfo_map.begin();
    std::map<SITEINFO_STR_REP, int>::const_iterator unq_siteinfo_end1 = unique_siteinfo_map.end();
    for (; unq_siteinfo_itr1 != unq_siteinfo_end1; ++unq_siteinfo_itr1)
      {
      all_str_params.push_back(unq_siteinfo_itr1->first.atomic_mass);
      all_str_params.push_back(unq_siteinfo_itr1->first.charge);
      } //end for each unique siteinfo rep

    std::vector<double> ret_doubles = compile_symbolic_double(all_str_params);

    ///////////////////////////////
    // Fill MDSYSTEM.siteinfo Table
    std::cout << "   ...entering site information into DB" << std::endl; //VERBOSE

    int vec_pos = 0;
    std::map<SITEINFO_STR_REP, int>::const_iterator unq_siteinfo_itr2 = unique_siteinfo_map.begin();
    std::map<SITEINFO_STR_REP, int>::const_iterator unq_siteinfo_end2 = unique_siteinfo_map.end();
    for (; unq_siteinfo_itr2 != unq_siteinfo_end2; ++unq_siteinfo_itr2)
      {
      int info_index = unq_siteinfo_itr2->second;
      double atomic_mass = ret_doubles.at(vec_pos++);
      double charge      = ret_doubles.at(vec_pos++);

      // Fill DB with this information
      db2::StmtHandle input_stmt(this->db2conn);
      char *input_sqlcmd = " insert into mdsystem.siteinfo "
                           " (pst_id, info_id, atomic_mass, half_inverse_mass, "
                           " charge ) values (?,?,?,?,?)";
      input_stmt.prepare(input_sqlcmd);

      double half_inverse_mass = ((double)1.0) / (double)(((double)2.0)*atomic_mass);

      input_stmt.bindParam(1, (SQLINTEGER&)this->pst_id);
      input_stmt.bindParam(2, (SQLINTEGER&)info_index);
      input_stmt.bindParam(3, (SQLDOUBLE&)atomic_mass);
      input_stmt.bindParam(4, (SQLDOUBLE&)half_inverse_mass);
      input_stmt.bindParam(5, (SQLDOUBLE&)charge);
      try { input_stmt.execute(); }
      catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
      
      } //end for each siteinfo rep

    ///////////////////////////////
    // Fill MDSYSTEM.site_to_siteinfo Table
 
    //Use arrays here so we only have to issue one DB query
    int sitemap_sz = site_to_siteinfo_map.size();
    int sitemap_row_cnt = 0;
    int *sitemap_siteid_arr = new int[sitemap_sz];
    int *sitemap_infoid_arr = new int[sitemap_sz];
      
    std::map<int,int>::const_iterator ssi_itr = site_to_siteinfo_map.begin();
    std::map<int,int>::const_iterator ssi_end = site_to_siteinfo_map.end();
    for (; ssi_itr != ssi_end; ++ssi_itr)
      {
      sitemap_siteid_arr[sitemap_row_cnt] = ssi_itr->first;
      sitemap_infoid_arr[sitemap_row_cnt] = ssi_itr->second;
      sitemap_row_cnt++;
      } //end for each site to siteinfo map

    db2::StmtHandle input_stmt2(this->db2conn);
    char input_sqlcmd2[SQLCMD_BUFSZ];
    snprintf(input_sqlcmd2, SQLCMD_BUFSZ, " insert into mdsystem.site_to_siteinfo "
             " (pst_id, site_id, info_id) values (%d,?,?) ", this->pst_id);
    input_stmt2.prepare(input_sqlcmd2);

    input_stmt2.bindParamArray(1, (SQLINTEGER*)sitemap_siteid_arr, sitemap_sz);
    input_stmt2.bindParamArray(2, (SQLINTEGER*)sitemap_infoid_arr, sitemap_sz);
    try { input_stmt2.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

    delete [] sitemap_siteid_arr;
    delete [] sitemap_infoid_arr;

    } //end create_siteinfo_table()


//****************************************************************************
  void TransformSysParams::create_pair_exclusion_tables()
    {
    std::cout << "Creating Exclusion List and Pair14 List" << std::endl; //VERBOSE

    ///////////////////////////////////////////
    // Fill Pair14List and ExcludeList
    // Must check distance between all bonded atoms
    // which is a "multimap<site_id -> pair<adjacent site, # bonds away>>"
    std::cout << "   ...determining distances between bonded sites" << std::endl; //VERBOSE

    std::multimap<int, SITE_REP >::const_iterator all_bonds_itr =
              this->bond_adjacency_map.begin();
    std::multimap<int, SITE_REP >::const_iterator all_bonds_end =
              this->bond_adjacency_map.end();
    for (; all_bonds_itr != all_bonds_end; ++all_bonds_itr)
      {
      int site1 = all_bonds_itr->first;
      int site2 = all_bonds_itr->second.site_id;

      if (site1 < site2) //don't want duplicate pairs in list
        {
          this->excluded_sites.insert(std::make_pair(site1, site2));
        }
      else
        {
          this->excluded_sites.insert(std::make_pair(site2, site1));
        }

      typedef std::multimap<int, SITE_REP>::const_iterator MMI;
	    std::pair<MMI,MMI> dist2_sites = this->bond_adjacency_map.equal_range(site2);
	    for (MMI d2_itr = dist2_sites.first; d2_itr != dist2_sites.second; ++d2_itr) 
        {
        int site3 = d2_itr->second.site_id;
  
        if (site3 == site1) { continue; }
      
        if (site1 < site3) //don't want duplicate pairs in list
          {
            this->excluded_sites.insert(std::make_pair(site1, site3));
          }
        else
          {
            this->excluded_sites.insert(std::make_pair(site3, site1));
          }

        std::pair<MMI,MMI> dist3_sites = this->bond_adjacency_map.equal_range(site3);
        for (MMI d3_itr = dist3_sites.first; d3_itr != dist3_sites.second; ++d3_itr) 
          {
          int site4 = d3_itr->second.site_id;
  
          if ((site4 == site1) || (site4 == site2)) { continue; }
      
          if (site1 < site4) //don't want duplicate pairs in list
            {
              this->pair14_sites.insert(std::make_pair(site1, site4));
            }
          else
            {
              this->pair14_sites.insert(std::make_pair(site4, site1));
           }

          } //end for each site bonded to site3 (depth 2)
      
        } //end for each site bonded to site2 (depth 1)
      
      } //end for each adjacent site  (depth 0)

    ///////////////////
    // We don't include pair14 separated atoms that are 
    // also pair 1,2 and pair 1,3 separated
    std::set<std::pair<int, int> >::iterator rm_ring_pairs_itr = this->pair14_sites.begin();
    std::set<std::pair<int, int> >::iterator rm_ring_pairs_end = this->pair14_sites.end();
    for (; rm_ring_pairs_itr != rm_ring_pairs_end;)
      {
      int site1 = rm_ring_pairs_itr->first;
      int site2 = rm_ring_pairs_itr->second;
      std::set<std::pair<int, int> >::const_iterator find_match = 
                this->excluded_sites.find(std::make_pair(site1, site2));
      if (find_match != this->excluded_sites.end())
        {
        this->pair14_sites.erase(rm_ring_pairs_itr++);
        }
      else
        {
        rm_ring_pairs_itr++;
        }
      this->excluded_sites.insert(std::make_pair(site1, site2));
      } //end for each pair14_sites pair

    } //end create_pair_exclusion_tables()


//****************************************************************************
  void TransformSysParams::write_pair_exclusion_tables()
    {
    std::cout << "Writing Exclusion List and Pair14 List" << std::endl; //VERBOSE

    ///////////////////////////////////////////
    // Determining the ff_family type
    db2::StmtHandle stmt0(this->db2conn);
    char sqlcmd0[SQLCMD_BUFSZ];
    snprintf(sqlcmd0, SQLCMD_BUFSZ, "select param_value from mdsystem.global_parameter "
             " where sys_id=%d and param_id like 'ff_family' "
             " FOR READ ONLY WITH UR", this->sys_id);
    stmt0.prepare(sqlcmd0);

    char param_value_cstr[SQLVARCHAR_BUFSZ];
    stmt0.bindCol(1, (SQLCHAR*)&param_value_cstr[0], SQLVARCHAR_BUFSZ);

    try { stmt0.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
    stmt0.fetch(); //only 1 item to fetch
    std::string param_value_str(param_value_cstr);


    ///////////////////////////////////////////
    // Foreach grab all the 14 params from DB, 
    // later check against the pair14_sites to pull out pair14 params
    // when feeding into compiler
    std::map<int, std::string> charge_params_map;
    std::map<int, std::string> lj_epsilon_params_map;
    std::map<int, std::string> lj_rmin_params_map;

    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[SQLCMD_BUFSZ];
    snprintf(sqlcmd1, SQLCMD_BUFSZ, "SELECT site_id, charge, epsilon14, rmin14 from "
             " mdsystem.site where sys_id=%d "
             " FOR READ ONLY WITH UR", this->sys_id);
    stmt1.prepare(sqlcmd1);

    int site_id = -1;
    char charge_cstr[SQLVARCHAR_BUFSZ];
    char lj_epsilon_cstr[SQLVARCHAR_BUFSZ];
    char lj_rmin_cstr[SQLVARCHAR_BUFSZ];

    stmt1.bindCol(1, (SQLINTEGER&)site_id);
    stmt1.bindCol(2, (SQLCHAR*)&charge_cstr[0], SQLVARCHAR_BUFSZ);
    stmt1.bindCol(3, (SQLCHAR*)&lj_epsilon_cstr[0], SQLVARCHAR_BUFSZ);
    stmt1.bindCol(4, (SQLCHAR*)&lj_rmin_cstr[0], SQLVARCHAR_BUFSZ);

    try { stmt1.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

    while (stmt1.fetch())
      {
      charge_params_map.insert(std::make_pair(site_id, std::string(charge_cstr)));
      lj_epsilon_params_map.insert(std::make_pair(site_id, std::string(lj_epsilon_cstr)));
      lj_rmin_params_map.insert(std::make_pair(site_id, std::string(lj_rmin_cstr)));
      }
   
    ///////////////////////////////
    // Use compiler to parse all pair14 parameter strings 
    // This assumes the order for walking the vector remains the same between the 2 loops
    std::cout << "   ...determining pair14 parameter values using compiler" << std::endl; //VERBOSE

    std::vector<std::string> all_str_params;

    std::set<std::pair<int, int> >::const_iterator pair14_itr1 = this->pair14_sites.begin();
    std::set<std::pair<int, int> >::const_iterator pair14_end1 = this->pair14_sites.end();
    for (; pair14_itr1 != pair14_end1; ++pair14_itr1) 
      {
      int site1 = pair14_itr1->first;
      int site2 = pair14_itr1->second;
      std::string charge_str;
      
      std::map<int, std::string>::const_iterator find_charge1 = charge_params_map.find(site1);
      std::map<int, std::string>::const_iterator find_charge2 = charge_params_map.find(site2);
      std::map<int, std::string>::const_iterator find_epsilon1 = lj_epsilon_params_map.find(site1);
      std::map<int, std::string>::const_iterator find_epsilon2 = lj_epsilon_params_map.find(site2);
      std::map<int, std::string>::const_iterator find_rmin1 = lj_rmin_params_map.find(site1);
      std::map<int, std::string>::const_iterator find_rmin2 = lj_rmin_params_map.find(site2);
      if ( (find_charge1 == charge_params_map.end()) ||
           (find_charge1 == charge_params_map.end()) ||
           (find_epsilon1 == lj_epsilon_params_map.end()) ||
           (find_epsilon2 == lj_epsilon_params_map.end()) ||
           (find_rmin1 == lj_rmin_params_map.end()) ||
           (find_rmin2 == lj_rmin_params_map.end()) 
        ) {
        std::cerr << "ERROR: Could not find pair14 parameter for sites " << site1
                    << " and " << site2 << " in internal map "
                    << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
        }
      else
        {
        charge_str.append(find_charge1->second);
        charge_str.append(" * ");
        charge_str.append(find_charge2->second);
        charge_str.append(" * charge14scale");
  
        all_str_params.push_back(charge_str);
        all_str_params.push_back(find_epsilon1->second);
        all_str_params.push_back(find_rmin1->second);
        all_str_params.push_back(find_epsilon2->second);
        all_str_params.push_back(find_rmin2->second);
        }

      } //end for each pair14_rep

    std::vector<double> ret_doubles = compile_symbolic_double(all_str_params);


    ///////////////////////////////
    // Insert pair14_sites into MDSYSTEM.pair14_sites
    // Change from external site_ids to internal site_ids on insert 
    //
    // Use "Combiner" class/functions from UDF_BindingWrapper.hpp in /BlueMatter/kerngen
    // to combine the epsilon and rmin parameters according to what force
    // model is being used
    std::cout << "   ...combining epsilon and rmin parameters according to force model" << std::endl; //VERBOSE
    std::cout << "   ...entering pair14 site information into DB" << std::endl; //VERBOSE

    int pair14_sz = this->pair14_sites.size();
    int pair14_row_cnt = 0;
    int *pair14_site1_arr = new int[pair14_sz];
    int *pair14_site2_arr = new int[pair14_sz];
    double *pair14_charge_arr = new double[pair14_sz];
    double *pair14_lj_epsilon_arr = new double[pair14_sz];
    double *pair14_lj_rmin_arr = new double[pair14_sz];

    int vec_pos = 0;
    std::set<std::pair<int, int> >::const_iterator pair14_itr2 = this->pair14_sites.begin();
    std::set<std::pair<int, int> >::const_iterator pair14_end2 = this->pair14_sites.end();
    for (; pair14_itr2 != pair14_end2; ++pair14_itr2) 
      {

      int int_site1 = -1;
      int int_site2 = -1;
      std::map<int,int>::const_iterator find_site1 = 
                        this->ext_int_site_map.find(pair14_itr2->first);
      if (find_site1 != this->ext_int_site_map.end())
        {
        int_site1 = find_site1->second;
        }
      else
        {
        std::cerr << "ERROR: Could not find Internal Site ID for External Site ID "
                  << pair14_itr2->first
                  << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
        }
      std::map<int,int>::const_iterator find_site2 = 
                        this->ext_int_site_map.find(pair14_itr2->second);
      if (find_site2 != this->ext_int_site_map.end())
        {
        int_site2 = find_site2->second;
        }
      else
        {
        std::cerr << "ERROR: Could not find Internal Site ID for External Site ID "
                  << pair14_itr2->second
                  << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
        }

      double final_charge  = ret_doubles.at(vec_pos++);
      double lj_epsilon1   = ret_doubles.at(vec_pos++);
      double lj_rmin1      = ret_doubles.at(vec_pos++);
      double lj_epsilon2   = ret_doubles.at(vec_pos++);
      double lj_rmin2      = ret_doubles.at(vec_pos++);

      double final_epsilon = 0.0;
      double final_rmin    = 0.0;

      //////////////////////////////
      //Use Combiner functions from UDF_BindingWrapper.hpp in /kerngen
      LJ_PARAMS params1;
      params1.epsilon = lj_epsilon1;
      params1.sigma = lj_rmin1;
      LJ_PARAMS params2;
      params2.epsilon = lj_epsilon2;
      params2.sigma = lj_rmin2;
      LJ_PARAMS combined;

      if (param_value_str.compare("CHARMM")==0)  
        {
        LennardJones14CHARMMCombiner combiner;
        combiner.Combine<LJ_PARAMS>(params1, params2, combined);
        final_epsilon = combined.epsilon; 
        final_rmin = combined.sigma; 
        }
      else if (param_value_str.compare("AMBER")==0)  
        {
        LennardJones14AMBERCombiner combiner;
        combiner.Combine<LJ_PARAMS>(params1, params2, combined);
        final_epsilon = combined.epsilon; 
        final_rmin = combined.sigma; 
        }
      else if (param_value_str.compare("OPLSAA")==0)  
        {
        LennardJones14OPLSAACombiner combiner;
        combiner.Combine<LJ_PARAMS>(params1, params2, combined);
        final_epsilon = combined.epsilon; 
        final_rmin = combined.sigma; 
        }
      else 
        {
        std::cerr << "ERROR: Invalid 'ff_family': " << param_value_cstr
                  << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
        } //end if each ff_family

      //////////
      // NOTE: Pair14 list pairs MUST have lower site first for bsearch
      if (int_site1 < int_site2)
        {
        pair14_site1_arr[pair14_row_cnt] = int_site1;
        pair14_site2_arr[pair14_row_cnt] = int_site2;
        }
      else
        {
        pair14_site1_arr[pair14_row_cnt] = int_site2;
        pair14_site2_arr[pair14_row_cnt] = int_site1;
        }

      pair14_charge_arr[pair14_row_cnt] = final_charge;
      pair14_lj_epsilon_arr[pair14_row_cnt] = final_epsilon;
      pair14_lj_rmin_arr[pair14_row_cnt] = final_rmin;
      
      pair14_row_cnt++;
      
      } //end for each pair14_rep
      
      
    //////////////////////////////
    // Enter data into DB

    db2::StmtHandle input_stmt1(this->db2conn);
    char input_sqlcmd1[SQLCMD_BUFSZ];
    snprintf(input_sqlcmd1, SQLCMD_BUFSZ, " insert into mdsystem.pair14_sites "
             " (pst_id, site1, site2, charge, lj_epsilon, lj_rmin) "
             "  values (%d,?,?,?,?,?) ", this->pst_id);
    input_stmt1.prepare(input_sqlcmd1);

    input_stmt1.bindParamArray(1, (SQLINTEGER*)pair14_site1_arr, pair14_sz);
    input_stmt1.bindParamArray(2, (SQLINTEGER*)pair14_site2_arr, pair14_sz);
    input_stmt1.bindParamArray(3, (SQLDOUBLE*)pair14_charge_arr, pair14_sz);
    input_stmt1.bindParamArray(4, (SQLDOUBLE*)pair14_lj_epsilon_arr, pair14_sz);
    input_stmt1.bindParamArray(5, (SQLDOUBLE*)pair14_lj_rmin_arr, pair14_sz);
    try { input_stmt1.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }


    ///////////////////////////////////////////
    // Insert this->excluded_list into MDSYSTEM.exclusion_list
    // Change from external site_ids to internal site_ids on insert 
    std::cout << "   ...filling excluded list in DB" << std::endl; //VERBOSE

    //Use extra arrays here so its only one DB call
    int exc_sz = this->excluded_sites.size();
    int exc_row_cnt = 0;
    int *exc_site1_arr = new int[exc_sz];
    int *exc_site2_arr = new int[exc_sz];

    std::set<std::pair<int, int> >::const_iterator excluded_itr = this->excluded_sites.begin();
    std::set<std::pair<int, int> >::const_iterator excluded_end = this->excluded_sites.end();
    for (; excluded_itr != excluded_end; ++excluded_itr) 
      {
      int int_site1 = -1;
      int int_site2 = -1;
      std::map<int,int>::const_iterator find_site1 = 
                        this->ext_int_site_map.find(excluded_itr->first);
      if (find_site1 != this->ext_int_site_map.end())
        {
        int_site1 = find_site1->second;
        }
      std::map<int,int>::const_iterator find_site2 = 
                        this->ext_int_site_map.find(excluded_itr->second);
      if (find_site1 != this->ext_int_site_map.end())
        {
        int_site2 = find_site2->second;
        }

      //////////
      // NOTE: Exclusion list pairs MUST have lower site first for bsearch
      if (int_site1 < int_site2)
        {
        exc_site1_arr[exc_row_cnt] = int_site1;
        exc_site2_arr[exc_row_cnt] = int_site2;
        }
      else
        {
        exc_site1_arr[exc_row_cnt] = int_site2;
        exc_site2_arr[exc_row_cnt] = int_site1;
        }
      exc_row_cnt++;
      } //end for each pair in excluded_list


    //////////////////////////////
    // Enter data into DB
    db2::StmtHandle input_stmt2(this->db2conn);
    char input_sqlcmd2[SQLCMD_BUFSZ];
    snprintf(input_sqlcmd2, SQLCMD_BUFSZ, " insert into mdsystem.exclusion_sites "
                         " (pst_id, site1, site2) values (%d,?,?) ", this->pst_id);
    input_stmt2.prepare(input_sqlcmd2);

    input_stmt2.bindParamArray(1, (SQLINTEGER*)exc_site1_arr, exc_sz);
    input_stmt2.bindParamArray(2, (SQLINTEGER*)exc_site2_arr, exc_sz);
    try { input_stmt2.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

    delete [] exc_site1_arr;
    delete [] exc_site2_arr;

    } //end write_pair_exclusion_tables()
    

//****************************************************************************
  void TransformSysParams::create_nsqsites_params()
    {
    std::cout << "Determining N-Squared Forces Parameters" << std::endl; //VERBOSE

    //map<nsq_siteinfo_rep, index>
    std::map<NSQ_SITEINFO_STR_REP, int> unique_nsqsiteinfo_map;
    //map<internal site_id, nsq_siteinfo_index>
    std::map<int, int> nsqsite_to_nsqsiteinfo_map;

    ///////////////////////////////////////////
    // First we fetch every unique NSQ SITE_TYPE_REP
    // Later we'll add to tables 
    std::cout << "   ...determining water method parameters" << std::endl; //VERBOSE

    std::string o_charge;
    std::string h_charge;
    std::string o_epsilon;
    std::string h_epsilon;
    std::string o_rmin;
    std::string h_rmin;

    std::string w_method = this->runtimeparams->water_method; 
    if (w_method.compare("NONE")!=0)
      {
      db2::StmtHandle stmt1(this->db2conn);
      char sqlcmd1[SQLCMD_BUFSZ];
      snprintf(sqlcmd1, SQLCMD_BUFSZ, "SELECT q1, q2, O_EPSILON, H_EPSILON, O_SIGMA, H_SIGMA "
              " from chem.water_parameters where water_model='%s' FOR READ ONLY WITH UR",
              w_method.c_str());
      stmt1.prepare(sqlcmd1);

      char q1_cstr[SQLVARCHAR_BUFSZ];
      char q2_cstr[SQLVARCHAR_BUFSZ];
      char o_eps_cstr[SQLVARCHAR_BUFSZ];
      char h_eps_cstr[SQLVARCHAR_BUFSZ];
      char o_rmin_cstr[SQLVARCHAR_BUFSZ];
      char h_rmin_cstr[SQLVARCHAR_BUFSZ];
      stmt1.bindCol(1, (SQLCHAR*)&q1_cstr[0], SQLVARCHAR_BUFSZ);
      stmt1.bindCol(2, (SQLCHAR*)&q2_cstr[0], SQLVARCHAR_BUFSZ);
      stmt1.bindCol(3, (SQLCHAR*)&o_eps_cstr[0], SQLVARCHAR_BUFSZ);
      stmt1.bindCol(4, (SQLCHAR*)&h_eps_cstr[0], SQLVARCHAR_BUFSZ);
      stmt1.bindCol(5, (SQLCHAR*)&o_rmin_cstr[0], SQLVARCHAR_BUFSZ);
      stmt1.bindCol(6, (SQLCHAR*)&h_rmin_cstr[0], SQLVARCHAR_BUFSZ);
      
      try { stmt1.execute(); }
      catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

      stmt1.fetch(); //Only need to fetch 1 row 

      o_charge = std::string(q1_cstr).append(" * ChargeConversion");
      h_charge = std::string(q2_cstr).append(" * ChargeConversion");
      o_epsilon = std::string(o_eps_cstr);
      h_epsilon = std::string(h_eps_cstr);
      o_rmin = std::string(o_rmin_cstr).append(" * SciConst::SixthRootOfTwo");
      h_rmin = std::string(h_rmin_cstr).append(" * SciConst::SixthRootOfTwo");

      } //end if watermethod != null

    //Use set here to only keep unique siteinfos 
    //map<site_info_rep, siteinfo_index>
    std::cout << "   ...recalculating and compressing N-squared forces parameters" << std::endl; //VERBOSE

    int num_siteinfo_reps = 0;

    db2::StmtHandle stmt2(this->db2conn);
    char sqlcmd2[SQLCMD_BUFSZ];
    snprintf(sqlcmd2, SQLCMD_BUFSZ, "SELECT site_id, epsilon, rmin, charge, element_symbol FROM "
             " MDSYSTEM.SITE WHERE sys_id=%d ORDER BY epsilon, rmin, charge, element_symbol "
             " FOR READ ONLY WITH UR", this->sys_id); 
    stmt2.prepare(sqlcmd2);

    int ext_site_id = -1;
    char lj_epsilon_cstr[SQLVARCHAR_BUFSZ];
    char lj_rmin_cstr[SQLVARCHAR_BUFSZ];
    char charge_cstr[SQLVARCHAR_BUFSZ];
    char element_symbol_cstr[SQLVARCHAR_BUFSZ];

    stmt2.bindCol(1, (SQLINTEGER&)ext_site_id);
    stmt2.bindCol(2, (SQLCHAR*)&lj_epsilon_cstr[0], SQLVARCHAR_BUFSZ);
    stmt2.bindCol(3, (SQLCHAR*)&lj_rmin_cstr[0], SQLVARCHAR_BUFSZ);
    stmt2.bindCol(4, (SQLCHAR*)&charge_cstr[0], SQLVARCHAR_BUFSZ);
    stmt2.bindCol(5, (SQLCHAR*)&element_symbol_cstr[0], SQLVARCHAR_BUFSZ);
    try { stmt2.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
        
    while (stmt2.fetch())
      {
      int int_site_id = -1;
      std::string charge_str(charge_cstr);
      std::string lj_epsilon_str(lj_epsilon_cstr);
      std::string lj_rmin_str(lj_rmin_cstr);
      std::string element_symbol_str(element_symbol_cstr);

      std::map<int,int>::const_iterator find_site_map = this->ext_int_site_map.find(ext_site_id);
      if (find_site_map != this->ext_int_site_map.end())
        {
        int_site_id = find_site_map->second;
        }
      else
        {
        std::cerr << "ERROR: Could not find Internal Site ID for External Site ID " << ext_site_id 
                  << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
        }

      /////////////
      // Run through, checking if the parameters might be NULL

      if ((charge_str.find("NULL",0)!=std::string::npos) || 
          (charge_str.find("null",0)!=std::string::npos)   )
        {
        if (w_method.compare("NONE")==0)
          {
          std::cerr << "ERROR: There are NULLs assigned to charges in XML, " 
                    << "but no Water Method defined (external site ID=" << ext_site_id << ")" 
                    << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
          }
        if (element_symbol_str.find("H")!=std::string::npos)
          {
          charge_str = h_charge;
          }
        else if (element_symbol_str.find("O")!=std::string::npos)
          {
          charge_str = o_charge;
          }
        else
          {
          std::cerr << "ERROR: Charge parameter is NULL but the site (external ID=" << ext_site_id << ")"
                    << " is not an O or H "
                    << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
          }
        }

      if ((lj_epsilon_str.find("NULL",0)!=std::string::npos) || 
          (lj_epsilon_str.find("null",0)!=std::string::npos)   )
        {
        if (w_method.compare("NONE")==0)
          {
          std::cerr << "ERROR: There are NULLs assigned to epsilons in XML, " 
                    << "but no Water Method defined (external site ID=" << ext_site_id << ")" 
                    << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
          }
        if (w_method.compare("CHARMM_TIP3P")!=0)
          {
          if (element_symbol_str.find("H")!=std::string::npos)
            {
            lj_epsilon_str = h_epsilon;
            } 
          else if (element_symbol_str.find("O")!=std::string::npos)
            {
            lj_epsilon_str = o_epsilon;
            }
          else
            {
            std::cerr << "ERROR: Epsilon parameter is NULL but the site (external ID=" << ext_site_id << ")"
                      << " is not an O or H "
                      << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
            }
          }
        else //CHARMM_TIP3P watermethod
          {
          if (element_symbol_str.find("H")!=std::string::npos)
            {
            lj_epsilon_str = std::string("0.0");
            } 
          else if (element_symbol_str.find("O")!=std::string::npos)
            {
            lj_epsilon_str = std::string("0.15207259450172");
            }
          else
            {
            std::cerr << "ERROR: Epsilon parameter is NULL but the site (external ID=" << ext_site_id << ")"
                      << " is not an O or H "
                      << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
            }
          }
        }

      if ((lj_rmin_str.find("NULL",0)!=std::string::npos) || 
          (lj_rmin_str.find("null",0)!=std::string::npos)   )
        {
        if (w_method.compare("NONE")==0)
          {
          std::cerr << "ERROR: There are NULLs assigned to rmins in XML, " 
                    << "but no Water Method defined (external site ID=" << ext_site_id << ")" 
                    << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
          }
        if (w_method.compare("CHARMM_TIP3P")!=0)
          {
          if (element_symbol_str.find("H")!=std::string::npos)
            {
            lj_rmin_str = h_rmin;
            } 
          else if (element_symbol_str.find("O")!=std::string::npos)
            {
            lj_rmin_str = o_rmin;
            }
          else
            {
            std::cerr << "ERROR: Epsilon parameter is NULL but the site (external ID=" << ext_site_id << ")"
                      << " is not an O or H "
                      << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
            }
          }
        else //CHARMM_TIP3P watermethod
          {
          if (element_symbol_str.find("H")!=std::string::npos)
            {
            lj_rmin_str = std::string("0.0");
            } 
          else if (element_symbol_str.find("O")!=std::string::npos)
            {
            lj_rmin_str = std::string("2*1.76824595565275");
            }
          else
            {
            std::cerr << "ERROR: Epsilon parameter is NULL but the site (external ID=" << ext_site_id << ")"
                      << " is not an O or H "
                      << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
            }
          }
        }

      /////////////
      // Enter SiteInfo into set to keep track of unique site_infos
      NSQ_SITEINFO_STR_REP new_rep(charge_str, lj_epsilon_str, lj_rmin_str);

      std::map<NSQ_SITEINFO_STR_REP, int>::const_iterator find_siteinfo = unique_nsqsiteinfo_map.find(new_rep); 
      if (find_siteinfo != unique_nsqsiteinfo_map.end())
        {
        nsqsite_to_nsqsiteinfo_map.insert(std::make_pair(int_site_id, find_siteinfo->second));
        }
      else
        {
        unique_nsqsiteinfo_map.insert(std::make_pair(new_rep, num_siteinfo_reps));
        nsqsite_to_nsqsiteinfo_map.insert(std::make_pair(int_site_id, num_siteinfo_reps));
        num_siteinfo_reps++;
        }

      } //end while fetching each (epsilon, rmin, charge)

   
    ///////////////////////////////
    // Use compiler to parse all SITEINFO_STR_REP parameter strings 
    // This assumes the order for walking the map remains the same between the 2 loops
    std::cout << "   ...determining parameter values using compiler" << std::endl; //VERBOSE

    std::vector<std::string> all_str_params;

    std::map<NSQ_SITEINFO_STR_REP, int>::const_iterator unq_nsqsiteinfo_itr1 = unique_nsqsiteinfo_map.begin();
    std::map<NSQ_SITEINFO_STR_REP, int>::const_iterator unq_nsqsiteinfo_end1 = unique_nsqsiteinfo_map.end();
    for (; unq_nsqsiteinfo_itr1 != unq_nsqsiteinfo_end1; ++unq_nsqsiteinfo_itr1)
      {
      all_str_params.push_back(unq_nsqsiteinfo_itr1->first.charge);
      all_str_params.push_back(unq_nsqsiteinfo_itr1->first.lj_epsilon);
      all_str_params.push_back(unq_nsqsiteinfo_itr1->first.lj_rmin);
      } //end for each unique siteinfo rep

    std::vector<double> ret_doubles = compile_symbolic_double(all_str_params);

    ///////////////////////////////////////////
    // Now that we have unique NSQ SITE_TYPES, we can write out the tables
    // Fill MDSYSTEM.nsq_sitetypes 
    // Fill MDSYSTEM.nsq_site_to_sitetypes tables
    std::cout << "   ...entering N-squared forces parameters into DB" << std::endl; //VERBOSE

    // Use arrays to allow only a single input into DB
    int siteinfo_sz = unique_nsqsiteinfo_map.size();
    int siteinfo_row_cnt = 0;
    int *siteinfo_idx_arr = new int[siteinfo_sz];
    double *siteinfo_charge_arr = new double[siteinfo_sz];
    double *siteinfo_lj_epsilon_arr = new double[siteinfo_sz];
    double *siteinfo_lj_rmin_arr = new double[siteinfo_sz];

    int nsqmap_sz = nsqsite_to_nsqsiteinfo_map.size();
    int nsqmap_row_cnt = 0;
    int *nsqmap_siteid_arr = new int[nsqmap_sz];
    int *nsqmap_infoid_arr = new int[nsqmap_sz];

    int vec_pos = 0;
    std::map<NSQ_SITEINFO_STR_REP, int>::const_iterator unq_nsqsiteinfo_itr2 = unique_nsqsiteinfo_map.begin();
    std::map<NSQ_SITEINFO_STR_REP, int>::const_iterator unq_nsqsiteinfo_end2 = unique_nsqsiteinfo_map.end();
    for (; unq_nsqsiteinfo_itr2 != unq_nsqsiteinfo_end2; ++unq_nsqsiteinfo_itr2)
      {
      siteinfo_idx_arr[siteinfo_row_cnt] = unq_nsqsiteinfo_itr2->second;
      siteinfo_charge_arr[siteinfo_row_cnt] = ret_doubles.at(vec_pos++);
      siteinfo_lj_epsilon_arr[siteinfo_row_cnt] = ret_doubles.at(vec_pos++);
      siteinfo_lj_rmin_arr[siteinfo_row_cnt] = ret_doubles.at(vec_pos++);
      siteinfo_row_cnt++;
      } //end for each unique siteinfo rep

    std::map<int,int>::const_iterator ssi_itr = nsqsite_to_nsqsiteinfo_map.begin();
    std::map<int,int>::const_iterator ssi_end = nsqsite_to_nsqsiteinfo_map.end();
    for (; ssi_itr != ssi_end; ++ssi_itr)
      {
      nsqmap_siteid_arr[nsqmap_row_cnt] = ssi_itr->first;
      nsqmap_infoid_arr[nsqmap_row_cnt] = ssi_itr->second;
      nsqmap_row_cnt++;
      } //end for each nsq_site to siteinfo map

    ///////////////////////////
    //Fill in the DB tables
    db2::StmtHandle input_stmt1(this->db2conn);
    char input_sqlcmd1[SQLCMD_BUFSZ];
    snprintf(input_sqlcmd1, SQLCMD_BUFSZ, " insert into mdsystem.nsq_siteinfo "
      " (pst_id, info_id, charge, lj_epsilon, lj_rmin) "
      " values (%d,?,?,?,?)", this->pst_id);
    input_stmt1.prepare(input_sqlcmd1);

    input_stmt1.bindParamArray(1, (SQLINTEGER*)siteinfo_idx_arr, siteinfo_sz);
    input_stmt1.bindParamArray(2, (SQLDOUBLE*)siteinfo_charge_arr, siteinfo_sz);
    input_stmt1.bindParamArray(3, (SQLDOUBLE*)siteinfo_lj_epsilon_arr, siteinfo_sz);
    input_stmt1.bindParamArray(4, (SQLDOUBLE*)siteinfo_lj_rmin_arr, siteinfo_sz);
    try { input_stmt1.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

    // Fill MDSYSTEM.nsq_site_to_sitetypes tables
      
    db2::StmtHandle input_stmt2(this->db2conn);
    char input_sqlcmd2[SQLCMD_BUFSZ];
    snprintf(input_sqlcmd2, SQLCMD_BUFSZ, " insert into mdsystem.nsq_site_to_nsq_siteinfo "
        " (pst_id, site_id, info_id) values (%d,?,?) ", this->pst_id);
    input_stmt2.prepare(input_sqlcmd2);

    input_stmt2.bindParamArray(1, (SQLINTEGER*)nsqmap_siteid_arr, nsqmap_sz);
    input_stmt2.bindParamArray(2, (SQLINTEGER*)nsqmap_infoid_arr, nsqmap_sz);
    try { input_stmt2.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

    delete [] siteinfo_idx_arr;
    delete [] siteinfo_charge_arr;
    delete [] siteinfo_lj_epsilon_arr;
    delete [] siteinfo_lj_rmin_arr;

    delete [] nsqmap_siteid_arr;
    delete [] nsqmap_infoid_arr;

    } //end create_nsqsites_params()


//****************************************************************************
  void TransformSysParams::determine_water3_sites()
    { 
    std::cout << "Determining Water3 Site List" << std::endl; //VERBOSE

    //Determine the site_tuple_list_id for WATER3
    db2::StmtHandle stmt1(this->db2conn);
    char sqlcmd1[SQLCMD_BUFSZ];
    snprintf(sqlcmd1, SQLCMD_BUFSZ, "SELECT site_tuple_list_id FROM MDSYSTEM.SiteTupleList "
             " WHERE sys_id=%d AND site_tuple_desc LIKE 'Water3sites' FOR READ ONLY WITH UR",
             this->sys_id);
    stmt1.prepare(sqlcmd1);

    int site_tuplelist_id = -1;
    stmt1.bindCol(1, (SQLINTEGER&)site_tuplelist_id);
    
    try { stmt1.execute(); stmt1.fetch(); } //only need to run this once
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }
    
    /////////////////////////////
    //Extract every water3 from sitetupelist
    //ASSUMES: that there are mod-3 number of sites in this DB select
    db2::StmtHandle stmt3(this->db2conn);
    char sqlcmd3[SQLCMD_BUFSZ];
    snprintf(sqlcmd3, SQLCMD_BUFSZ, "SELECT site_id FROM "
             " MDSYSTEM.SiteTupleData WHERE sys_id=%d AND site_tuple_list_id=%d "
             " ORDER BY site_tuple_id, site_ordinal FOR READ ONLY WITH UR", 
             this->sys_id, site_tuplelist_id);
    stmt3.prepare(sqlcmd3);

    int ext_site_id = -1;
    stmt3.bindCol(1, (SQLINTEGER&)ext_site_id);
    try { stmt3.execute(); }
    catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

    while (stmt3.fetch())
      {
      std::vector<int> water_sites;
      
      water_sites.push_back(ext_site_id);
      stmt3.fetch();
      water_sites.push_back(ext_site_id);
      stmt3.fetch();
      water_sites.push_back(ext_site_id);

      this->water3_sites_vec.push_back(water_sites);
      } //end while fetching each water site

    } //end determine_water3_sites();



//****************************************************************************
  void TransformSysParams::write_water3_sites()
    { 
    std::cout << "Writing Out Water3 Site List with Internal Site IDs" << std::endl; //VERBOSE

    int num_waters = this->water3_sites_vec.size();
    int *site1_arr = new int[num_waters];
    int *site2_arr = new int[num_waters];
    int *site3_arr = new int[num_waters];
    int water_row_cnt = 0;

    /////////////////////////////
    // Grab every water3 from water3_sites_list 
    // ASSUMES: that water3_sites_list holds vectors of 3 sites each
    std::vector<std::vector<int> >::const_iterator water3_itr = this->water3_sites_vec.begin();
    std::vector<std::vector<int> >::const_iterator water3_end = this->water3_sites_vec.end();
    for (; water3_itr != water3_end; ++water3_itr)
      {
      int int_site1 = -1;
      int int_site2 = -1;
      int int_site3 = -1;
      
      std::map<int,int>::const_iterator find_site1 = this->ext_int_site_map.find(water3_itr->at(0));
      std::map<int,int>::const_iterator find_site2 = this->ext_int_site_map.find(water3_itr->at(1));
      std::map<int,int>::const_iterator find_site3 = this->ext_int_site_map.find(water3_itr->at(2));

      if ( (find_site1 == this->ext_int_site_map.end()) ||
           (find_site2 == this->ext_int_site_map.end()) ||
           (find_site3 == this->ext_int_site_map.end()) )
        {
        std::cerr << "ERROR: Could not find Internal Site ID for one of External Site IDs " 
                  << water3_itr->at(0) << ", " << water3_itr->at(1) << ", or " << water3_itr->at(2)
                  << " - Line " << __LINE__ << " in " << __FILE__ << std::endl;
        }
      else
        {
        int_site1 = find_site1->second;
        int_site2 = find_site2->second;
        int_site3 = find_site3->second;
        }

      site1_arr[water_row_cnt] = int_site1;
      site2_arr[water_row_cnt] = int_site2;
      site3_arr[water_row_cnt] = int_site3;
      water_row_cnt++;
      } //end while fetching each water site

      db2::StmtHandle input_stmt(this->db2conn);
      char input_sqlcmd[SQLCMD_BUFSZ];
      snprintf(input_sqlcmd, SQLCMD_BUFSZ, " insert into mdsystem.water3_sites "
                           " (pst_id, site_id1, site_id2, site_id3) "
                           " values (%d,?,?,?) ", this->pst_id);
      input_stmt.prepare(input_sqlcmd);

      input_stmt.bindParamArray(1, (SQLINTEGER*)site1_arr, num_waters);
      input_stmt.bindParamArray(2, (SQLINTEGER*)site2_arr, num_waters);
      input_stmt.bindParamArray(3, (SQLINTEGER*)site3_arr, num_waters);
      try { input_stmt.execute(); }
      catch(db2::HandleException HE) { std::cerr << HE << " - Caught at Line " << __LINE__ << " in " << __FILE__ << std::endl; }

      delete [] site1_arr;
      delete [] site2_arr;
      delete [] site3_arr;

    } //end write_water3_sites();



//****************************************************************************
// Main Driver function
//****************************************************************************
  void TransformSysParams::transform_all_sysparams()
    {

    determine_molecule_types();
    build_bond_adjacency_map();

    create_pair_exclusion_tables();  
    
    determine_water3_sites();

    create_rigidsites_params();

    create_rigidgroup_fragmentation();
    
    write_rigidsites_params();
    
    write_pair_exclusion_tables();  
    
    create_bondedsites_params(); 
    
    create_siteinfo_table();
    
    create_nsqsites_params();   

    write_water3_sites();

    std::cout << std::endl << "Finished transformations." << std::endl;

    } //end transform_all_dbparams()

} //end namespace

////****************************************************************************
//// Arbitrary Test Driver
////****************************************************************************
//int main(int argc, char* argv[])
//  {
//  return 0;
//  } //end main

 




