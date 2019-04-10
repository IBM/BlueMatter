// ***********************************************************************
// File: MDSystemData.hpp
// Author: cwo 
// Date: August 23, 2006 
// Namespace: concept_dbalg
// Class: MDSystemData
// Description: Holds container versions of systems DB2 tables 
// ***********************************************************************
#ifndef INCLUDE_MDSYSTEMDATA_HPP
#define INCLUDE_MDSYSTEMDATA_HPP

#include <BlueMatter/concept_dbalg/RuntimeParams.hpp>

#include <inmemdb/Table.hpp>
#include <inmemdb/QueryResult.hpp>
#include <inmemdb/QueryProcessor.hpp>

#include <stdlib.h>
#include <assert.h>
#include <iostream>


// DEFINES
#define MDSYSTEMDATA_SINGLETON_CLASS

//****************************************************************************
//****************************************************************************
// The following are tables with MSD information 
//****************************************************************************
//****************************************************************************

// Needed for Import --- DB "mdtest" on IP 127.0.0.1, port 5000
$$// INIT_DB2_RELAY ( MDTEST, 0x7F000001L, 5000 ); 

$$// DEFINE_TABLE Rigid4SitesTable
$$//              ( int pst_id, int heavy_site, 
$$//                int hydrogen_site1, double hydrogen_site1_rEQ, 
$$//                int hydrogen_site2, double hydrogen_site2_rEQ,
$$//                int hydrogen_site3, double hydrogen_site3_rEQ )
$$//  PRIMARY_KEY ( pst_id, heavy_site );

$$// DEFINE_TABLE Rigid3SitesTable
$$//              ( int pst_id, int heavy_site, 
$$//                int hydrogen_site1, double hydrogen_site1_rEQ, 
$$//                int hydrogen_site2, double hydrogen_site2_rEQ )
$$//  PRIMARY_KEY ( pst_id, heavy_site );

$$// DEFINE_TABLE Rigid2SitesTable
$$//              ( int pst_id, int heavy_site, 
$$//                int hydrogen_site1, double hydrogen_site1_rEQ )
$$//  PRIMARY_KEY ( pst_id, heavy_site );

$$// DEFINE_TABLE IntExtSiteIDsTable 
$$//              (int pst_id, int ext_site_id, int int_site_id)
$$//  PRIMARY_KEY ( pst_id, ext_site_id, int_site_id );

$$// DEFINE_TABLE FragmentTypeTable
$$//              ( int pst_id, int fragment_type_id, int num_sites, 
$$//                int molecule_type, int tagatom_index, 
$$//                int num_constraints, double extent )
$$//  PRIMARY_KEY ( pst_id, fragment_type_id );

$$// DEFINE_TABLE FragmentMapTable
$$//              ( int pst_id, int start_site, int fragment_type_id, 
$$//                int molecule_id, int frag_index_in_molecule )
$$//  PRIMARY_KEY ( pst_id, start_site, fragment_type_id); 

$$// DEFINE_TABLE SiteInfoTable
$$//              (int pst_id, int info_id, double atomic_mass,
$$//               double half_inverse_mass, double charge)
$$//  PRIMARY_KEY ( pst_id, info_id );

$$// DEFINE_TABLE SiteToSiteInfoTable
$$//              ( int pst_id, int site_id, int info_id )
$$//  PRIMARY_KEY ( pst_id, site_id, info_id);

$$// DEFINE_TABLE Water3SitesTable
$$//              ( int pst_id, int site_id1, int site_id2, int site_id3 )
$$//  PRIMARY_KEY ( pst_id, site_id1, site_id2, site_id3 );




namespace concept_dbalg
{

  class MDSystemData
  {
    private:    
      RuntimeParams *rt_params;

      //****************************************************************************
      // Private constructor
      //****************************************************************************
      #ifdef MDSYSTEMDATA_SINGLETON_CLASS
        static MDSystemData *obj_instance;
      #endif
      MDSystemData(RuntimeParams *rt_params)
        {
        this->rt_params = rt_params;
     
        $$// INSTANTIATE_TABLE ( Rigid4SitesTable, ctor_Rigid4SitesTable, ctor_Rigid4SitesTableT);
        this->mRigid4SitesTable = ctor_Rigid4SitesTable;
        $$// INSTANTIATE_TABLE ( Rigid3SitesTable, ctor_Rigid3SitesTable, ctor_Rigid3SitesTableT);
        this->mRigid3SitesTable = ctor_Rigid3SitesTable;
        $$// INSTANTIATE_TABLE ( Rigid2SitesTable, ctor_Rigid2SitesTable, ctor_Rigid2SitesTableT);
        this->mRigid2SitesTable = ctor_Rigid2SitesTable;

        $$// INSTANTIATE_TABLE ( IntExtSiteIDsTable, ctor_IntExtSiteIDsTable, ctor_IntExtSiteIDsTableT);
        this->mIntExtSiteIDsTable = ctor_IntExtSiteIDsTable;

        $$// INSTANTIATE_TABLE ( FragmentTypeTable, ctor_FragmentTypeTable, ctor_FragmentTypeTableT);
        this->mFragmentTypeTable = ctor_FragmentTypeTable;

        $$// INSTANTIATE_TABLE ( FragmentMapTable, ctor_FragmentMapTable, ctor_FragmentMapTableT);
        this->mFragmentMapTable = ctor_FragmentMapTable;

        $$// INSTANTIATE_TABLE ( SiteInfoTable, ctor_SiteInfoTable, ctor_SiteInfoTableT);
        this->mSiteInfoTable = ctor_SiteInfoTable;

        $$// INSTANTIATE_TABLE ( SiteToSiteInfoTable, ctor_SiteToSiteInfoTable, ctor_SiteToSiteInfoTableT);
        this->mSiteToSiteInfoTable = ctor_SiteToSiteInfoTable;

        $$// INSTANTIATE_TABLE ( Water3SitesTable, ctor_Water3SitesTable, ctor_Water3SitesTableT);
        this->mWater3SitesTable = ctor_Water3SitesTable;


        } //end private constructor 




    public:
      //****************************************************************************
      // Constructors/Destructor
      //****************************************************************************
      ~MDSystemData()
        {
        #ifdef TIMESTEP_SINGLETON_CLASS
          delete obj_instance;
          obj_instance = NULL;
        #endif
        }
      static MDSystemData* instance(RuntimeParams *rt_params)
        { 
        #ifdef TIMESTEP_SINGLETON_CLASS
          if (obj_instance == NULL) 
            {
            obj_instance = new MDSystemData(rt_params);
            assert(obj_instance != NULL);
            }
          return (obj_instance);
        #else
          return (new MDSystemData(rt_params));
        #endif
        }
      
      //*************************************************************
      // ERL member variables (DB2 tables)
      //*************************************************************

      $$// GET_TABLE_TYPEDEF ( Rigid4SitesTable, Rigid4SitesTableT);
      Rigid4SitesTableT *mRigid4SitesTable;
      $$// GET_TABLE_TYPEDEF ( Rigid3SitesTable, Rigid3SitesTableT);
      Rigid3SitesTableT *mRigid3SitesTable;
      $$// GET_TABLE_TYPEDEF ( Rigid2SitesTable, Rigid2SitesTableT);
      Rigid2SitesTableT *mRigid2SitesTable;

      $$// GET_TABLE_TYPEDEF ( IntExtSiteIDsTable, IntExtSiteIDsTableT);
      IntExtSiteIDsTableT *mIntExtSiteIDsTable;

      $$// GET_TABLE_TYPEDEF ( FragmentTypeTable, FragmentTypeTableT);
      FragmentTypeTableT *mFragmentTypeTable;

      $$// GET_TABLE_TYPEDEF ( FragmentMapTable, FragmentMapTableT);
      FragmentMapTableT *mFragmentMapTable;

      $$// GET_TABLE_TYPEDEF ( SiteInfoTable, SiteInfoTableT);
      SiteInfoTableT *mSiteInfoTable;

      $$// GET_TABLE_TYPEDEF ( SiteToSiteInfoTable, SiteToSiteInfoTableT);
      SiteToSiteInfoTableT *mSiteToSiteInfoTable;

      $$// GET_TABLE_TYPEDEF ( Water3SitesTable, Water3SitesTableT);
      Water3SitesTableT *mWater3SitesTable;


      
      //*************************************************************
      // Class Member variables 
      //*************************************************************

      //****************************************************************************
      // Fill internal tables with DB2 tables 
      //****************************************************************************
      void import_tables()
        {
        
        char import_predicate[IMPORT_PREDICATE_BUFFER_SZ];
        snprintf(import_predicate, IMPORT_PREDICATE_BUFFER_SZ, "pst_id=%d", this->rt_params->pst_id);

        $$// IMPORT_FROM_DB2_TABLE ( this->mRigid4SitesTable, MDSYSTEM.rigid4_sites )
        $$//                 WHERE ( import_predicate );
        
        $$// IMPORT_FROM_DB2_TABLE ( this->mRigid3SitesTable, MDSYSTEM.rigid3_sites )
        $$//                 WHERE ( import_predicate );
     
        $$// IMPORT_FROM_DB2_TABLE ( this->mRigid2SitesTable, MDSYSTEM.rigid2_sites )
        $$//                 WHERE ( import_predicate );

        $$// IMPORT_FROM_DB2_TABLE ( this->mIntExtSiteIDsTable, MDSYSTEM.internal_external_siteids )
        $$//                 WHERE ( import_predicate );

        $$// IMPORT_FROM_DB2_TABLE ( this->mFragmentTypeTable, MDSYSTEM.fragment_type )
        $$//                 WHERE ( import_predicate );

        $$// IMPORT_FROM_DB2_TABLE ( this->mFragmentMapTable, MDSYSTEM.fragment_map )
        $$//                 WHERE ( import_predicate );

        $$// IMPORT_FROM_DB2_TABLE ( this->mSiteInfoTable, MDSYSTEM.siteinfo )
        $$//                 WHERE ( import_predicate );

        $$// IMPORT_FROM_DB2_TABLE ( this->mSiteToSiteInfoTable, MDSYSTEM.site_to_siteinfo )
        $$//                 WHERE ( import_predicate );

        $$// IMPORT_FROM_DB2_TABLE ( this->mWater3SitesTable, MDSYSTEM.water3_sites )
        $$//                 WHERE ( import_predicate );


        if (this->rt_params->verbosity > 2) //VERBOSITY
          {
          std::cerr << "MDSystemData Imported Tables:" << std::endl;
          std::cerr << "   Number of rows in Rigid4SitesTable          : " << this->mRigid4SitesTable->rowcount() << std::endl;
          std::cerr << "   Number of rows in Rigid3SitesTable          : " << this->mRigid3SitesTable->rowcount() << std::endl;
          std::cerr << "   Number of rows in Rigid2SitesTable          : " << this->mRigid2SitesTable->rowcount() << std::endl;
          std::cerr << "   Number of rows in IntExtSiteIDsTable        : " << this->mIntExtSiteIDsTable->rowcount() << std::endl;
          std::cerr << "   Number of rows in FragmentTypeTable         : " << this->mFragmentTypeTable->rowcount() << std::endl;
          std::cerr << "   Number of rows in FragmentMapTable          : " << this->mFragmentMapTable->rowcount() << std::endl;
          std::cerr << "   Number of rows in SiteInfoTable             : " << this->mSiteInfoTable->rowcount() << std::endl;
          std::cerr << "   Number of rows in SiteToSiteInfoTable       : " << this->mSiteToSiteInfoTable->rowcount() << std::endl;
          std::cerr << "   Number of rows in Water3SitesTable          : " << this->mWater3SitesTable->rowcount() << std::endl;
          if (this->rt_params->verbosity > 4) //VERBOSITY
            {
            this->mRigid4SitesTable->print();
            this->mRigid3SitesTable->print();
            this->mRigid2SitesTable->print();
            this->mIntExtSiteIDsTable->print();
            this->mFragmentTypeTable->print();
            this->mFragmentMapTable->print();
            this->mSiteInfoTable->print();
            this->mSiteToSiteInfoTable->print();
            this->mWater3SitesTable->print();
            }
          }


        } //end import_tables();
      

  }; //end class

} //end namespace

#endif
