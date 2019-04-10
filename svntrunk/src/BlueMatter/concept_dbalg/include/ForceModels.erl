// ***********************************************************************
// File: ForceModels.hpp
// Author: cwo 
// Date: August 14, 2006 
// Namespace: concept_dbalg
// Class: OPLSAA_ForceModel 
//        CHARMM_ForceModel 
// Description: Integrator for core MD loop
// ***********************************************************************
#ifndef INCLUDE_FORCEMODELS_HPP
#define INCLUDE_FORCEMODELS_HPP

#include <BlueMatter/concept_dbalg/RuntimeParams.hpp>

////////////
// Required to build with UDF_Binding.hpp
#define OMIT_BOND_VECTOR_TABLE
#define BegLogLine(x) if (x) { std::cerr 
#define EndLogLine             std::endl; }

#include <BlueMatter/MSD_Prefix.hpp>
#include <BlueMatter/UDF_Binding.hpp>
#include <BlueMatter/UDF_BindingWrapper.hpp>

#include <inmemdb/Table.hpp>
#include <inmemdb/QueryResult.hpp>
#include <inmemdb/QueryProcessor.hpp>

#include <stdlib.h>
#include <assert.h>
#include <iostream>


// DEFINES


//****************************************************************************
//****************************************************************************
// The following tables hold parameters for UDF calculations
//****************************************************************************
//****************************************************************************

// Needed for Import --- DB "mdtest" on IP 127.0.0.1, port 5000
$$// INIT_DB2_RELAY ( MDTEST, 0x7F000001L, 5000 ); 

$$// DEFINE_TABLE StdHarmonicBondForce
$$//              (int pst_id, int tuple_id, int site1, int site2,
$$//               double k, double r0 )
$$//  PRIMARY_KEY ( pst_id, tuple_id );

$$// DEFINE_TABLE StdHarmonicAngleForce
$$//              (int pst_id, int tuple_id, int site1, int site2,
$$//               int site3, double k, double th0 )
$$//  PRIMARY_KEY ( pst_id, tuple_id );

$$// DEFINE_TABLE UreyBradleyForce 
$$//              (int pst_id, int tuple_id, int site1, int site2,
$$//               double k, double s0 )
$$//  PRIMARY_KEY ( pst_id, tuple_id );

$$// DEFINE_TABLE SwopeTorsionForce
$$//              (int pst_id, int tuple_id, int site1, int site2,
$$//               int site3, int site4, double cosDelta, double k,
$$//               double sinDelta, int n)
$$//  PRIMARY_KEY ( pst_id, tuple_id );

$$// DEFINE_TABLE ImproperDihedralForce
$$//              (int pst_id, int tuple_id, int site1, int site2,
$$//               int site3, int site4, double k, double psi0)
$$//  PRIMARY_KEY ( pst_id, tuple_id );

$$// DEFINE_TABLE OPLSTorsionForce
$$//              (int pst_id, int tuple_id, int site1, int site2,
$$//               int site3, int site4, double cosDelta, double k,
$$//               double sinDelta, int n)
$$//  PRIMARY_KEY ( pst_id, tuple_id );

$$// DEFINE_TABLE OPLSImproperForce
$$//              (int pst_id, int tuple_id, int site1, int site2,
$$//               int site3, int site4, double cosDelta, double k,
$$//               double sinDelta, int n)
$$//  PRIMARY_KEY ( pst_id, tuple_id );


//****************************************************************************
//****************************************************************************
// Used for combiner function for NSQ forces 
//****************************************************************************
//****************************************************************************

$$// DEFINE_TABLE NSQSiteInfoTable
$$//              ( int pst_id, int info_id, double charge,
$$//                double lj_epsilon, double lj_rmin )
$$//  PRIMARY_KEY ( pst_id, info_id );

$$// DEFINE_TABLE NSQSiteToNSQSiteInfoTable
$$//              ( int pst_id, int site_id, int info_id )
$$//  PRIMARY_KEY ( pst_id, site_id );

$$// DEFINE_TABLE Pair14SitesTable
$$//              ( int pst_id, int site1, int site2, double charge,
$$//                double lj_epsilon, double lj_rmin )
$$//  PRIMARY_KEY ( pst_id, site1, site2 );




namespace concept_dbalg
{
  

 

//*****************************************************************************
//*****************************************************************************
// CHARMM CLASS
//*****************************************************************************
//*****************************************************************************

  template< class MDVM >
  class CHARMM_ForceModel
    {
    private: 
      RuntimeParams *rt_params;
   
      //**********************************************************************
      // Private constructor
      //**********************************************************************
      CHARMM_ForceModel(RuntimeParams *rt_params) 
        {
        this->rt_params = rt_params;
        tot_nsqcharge_energy = 0;
        tot_nsqlj_energy = 0;
        
        $$// INSTANTIATE_TABLE( StdHarmonicBondForce, ctor_CHARMMBondTable, ctor_CHARMMBondTableT);
        this->mStdBondTable = ctor_CHARMMBondTable;

        $$// INSTANTIATE_TABLE( StdHarmonicAngleForce, ctor_CHARMMAngleTable, ctor_CHARMMAngleTableT);
        this->mStdAngleTable = ctor_CHARMMAngleTable;

        $$// INSTANTIATE_TABLE( UreyBradleyForce, ctor_CHARMMUreyBradleyTable, ctor_CHARMMUreyBradleyTableT);
        this->mUreyBradleyTable = ctor_CHARMMUreyBradleyTable;

        $$// INSTANTIATE_TABLE( SwopeTorsionForce, ctor_CHARMMSwopeTorsionTable, ctor_CHARMMSwopeTorsionTableT);
        this->mSwopeTorsionTable = ctor_CHARMMSwopeTorsionTable;

        $$// INSTANTIATE_TABLE( ImproperDihedralForce, ctor_CHARMMImproperDihedralTable, ctor_CHARMMImproperDihedralTableT);
        this->mImproperDihedralTable = ctor_CHARMMImproperDihedralTable;

        $$// INSTANTIATE_TABLE ( Pair14SitesTable, ctor_Pair14SitesTable, ctor_Pair14SitesTableT);
        this->mPair14SitesTable = ctor_Pair14SitesTable;

        $$// INSTANTIATE_TABLE ( NSQSiteInfoTable, ctor_NSQSiteInfoTable, ctor_NSQSiteInfoTableT);
        this->mNSQSiteInfoTable = ctor_NSQSiteInfoTable;

        $$// INSTANTIATE_TABLE ( NSQSiteToNSQSiteInfoTable, ctor_NSQSiteToNSQSiteInfoTable, ctor_NSQSiteToNSQSiteInfoTableT);
        this->mNSQSiteToNSQSiteInfoTable =  ctor_NSQSiteToNSQSiteInfoTable;


        } //end constructor

    public:
      //**********************************************************************
      // Constructors/Destructors
      //**********************************************************************
      ~CHARMM_ForceModel()
        {
        }
      static CHARMM_ForceModel* instance(RuntimeParams *rt_params)
        { 
        return new CHARMM_ForceModel(rt_params);
        }

      // Create member variables for tables to be filled by constructor

      $$// GET_TABLE_TYPEDEF( StdHarmonicBondForce, StdBondTableT);
      StdBondTableT *mStdBondTable;
      $$// GET_TABLE_TYPEDEF( StdHarmonicAngleForce, StdAngleTableT);
      StdAngleTableT *mStdAngleTable;
      $$// GET_TABLE_TYPEDEF( UreyBradleyForce, UreyBradleyTableT);
      UreyBradleyTableT *mUreyBradleyTable;
      $$// GET_TABLE_TYPEDEF( SwopeTorsionForce, SwopeTorsionTableT);
      SwopeTorsionTableT *mSwopeTorsionTable;
      $$// GET_TABLE_TYPEDEF( ImproperDihedralForce, ImproperDihedralTableT);
      ImproperDihedralTableT *mImproperDihedralTable;
      $$// GET_TABLE_TYPEDEF ( Pair14SitesTable, Pair14SitesTableT);
      Pair14SitesTableT *mPair14SitesTable;
      $$// GET_TABLE_TYPEDEF ( NSQSiteInfoTable, NSQSiteInfoTableT);
      NSQSiteInfoTableT *mNSQSiteInfoTable;
      $$// GET_TABLE_TYPEDEF ( NSQSiteToNSQSiteInfoTable, NSQSiteToNSQSiteInfoTableT);
      NSQSiteToNSQSiteInfoTableT *mNSQSiteToNSQSiteInfoTable;

      // Member variables
      double tot_nsqcharge_energy;
      double tot_nsqlj_energy;

      //**********************************************************************
      //**********************************************************************
      void calculate_StdHarmonicBond( MDVM& aMDVM )
        {
        if (this->rt_params->verbosity > 3) //VERBOSITY
          {
          std::cout << "calculate_StdHarmonicBond()" << std::endl;
          }

        typename StdBondTableT::iterator bond_itr = this->mStdBondTable->begin();
        typename StdBondTableT::iterator bond_end = this->mStdBondTable->end();

        double tot_udf_energy = 0; 

        for (; bond_itr != bond_end; ++bond_itr)
          {
          struct UDF_Binding::StdHarmonicBondForce_Params params_struct;
          params_struct.k  = bond_itr->k;
          params_struct.r0 = bond_itr->r0;

          aMDVM.mEnergy = 0; 
          aMDVM.SetSiteRegMap( bond_itr->site1,
                               bond_itr->site2,
                               -1,
                               -1               );

          UDF_Binding::UDF_StdHarmonicBondForce_Execute<MDVM, UDF_Binding::StdHarmonicBondForce_Params>
                                           ( aMDVM, params_struct );

          tot_udf_energy += aMDVM.mEnergy; 
          } //end for each StdBond tuple

          
          if (this->rt_params->verbosity > 2) //VERBOSITY
            {
            printf("      StdHarmonicBond Energy = %0.16f\n", tot_udf_energy);
            }

        } //end calculate_StdHarmonicBond()

      //**********************************************************************
      //**********************************************************************
      void calculate_StdHarmonicAngle( MDVM& aMDVM )
        {
        if (this->rt_params->verbosity > 3) //VERBOSITY
          {
          std::cout << "calculate_StdHarmonicAngle()" << std::endl;
          }

        typename StdAngleTableT::iterator angle_itr = this->mStdAngleTable->begin();
        typename StdAngleTableT::iterator angle_end = this->mStdAngleTable->end();

        double tot_udf_energy = 0; 

        for (; angle_itr != angle_end; ++angle_itr)
          {
          struct UDF_Binding::StdHarmonicAngleForce_Params params_struct;
          params_struct.k   = angle_itr->k;
          params_struct.th0 = angle_itr->th0;

          aMDVM.mEnergy = 0; 
          aMDVM.SetSiteRegMap( angle_itr->site1,
                               angle_itr->site2,
                               angle_itr->site3,
                               -1               );

          UDF_Binding::UDF_StdHarmonicAngleForce_Execute<MDVM, UDF_Binding::StdHarmonicAngleForce_Params>
                                           ( aMDVM, params_struct );

          tot_udf_energy += aMDVM.mEnergy; 
          } //end for each StdAngle tuple

          if (this->rt_params->verbosity > 2) //VERBOSITY
            {
            printf("      StdHarmonicAngle Energy = %0.16f\n", tot_udf_energy);
            }

        } //end calculate_StdHarmonicAngle()

      //**********************************************************************
      //**********************************************************************
      void calculate_UreyBradley( MDVM& aMDVM )
        {
        if (this->rt_params->verbosity > 3) //VERBOSITY
          {
          std::cout << "calculate_UreyBradley()" << std::endl;
          }

        typename UreyBradleyTableT::iterator urey_itr = this->mUreyBradleyTable->begin();
        typename UreyBradleyTableT::iterator urey_end = this->mUreyBradleyTable->end();

        double tot_udf_energy = 0; 

        for (; urey_itr != urey_end; ++urey_itr)
          {
          struct UDF_Binding::UreyBradleyForce_Params params_struct;
          params_struct.k  = urey_itr->k;
          params_struct.s0 = urey_itr->s0;

          aMDVM.mEnergy = 0; 
          aMDVM.SetSiteRegMap( urey_itr->site1,
                               urey_itr->site2,
                               -1,
                               -1               );

          UDF_Binding::UDF_UreyBradleyForce_Execute<MDVM, UDF_Binding::UreyBradleyForce_Params>
                                           ( aMDVM, params_struct );

          tot_udf_energy += aMDVM.mEnergy; 
          } //end for each UreyBradley tuple

          if (this->rt_params->verbosity > 2) //VERBOSITY
            {
            printf("      UreyBradley Energy = %0.16f\n", tot_udf_energy);
            }

        } //end calculate_UreyBradley()


      //**********************************************************************
      //**********************************************************************
      void calculate_SwopeTorsion( MDVM& aMDVM )
        {
        if (this->rt_params->verbosity > 3) //VERBOSITY
          {
          std::cout << "calculate_SwopeTorsion()" << std::endl;
          }

        typename SwopeTorsionTableT::iterator swope_itr = this->mSwopeTorsionTable->begin();
        typename SwopeTorsionTableT::iterator swope_end = this->mSwopeTorsionTable->end();

        double tot_udf_energy = 0; 

        for (; swope_itr != swope_end; ++swope_itr)
          {
          struct UDF_Binding::SwopeTorsionForce_Params params_struct;
          params_struct.cosDelta = swope_itr->cosDelta;
          params_struct.k        = swope_itr->k;
          params_struct.sinDelta = swope_itr->sinDelta;
          params_struct.n        = swope_itr->n;

          aMDVM.mEnergy = 0; 
          aMDVM.SetSiteRegMap( swope_itr->site1,
                               swope_itr->site2,
                               swope_itr->site3,
                               swope_itr->site4 );

          UDF_Binding::UDF_SwopeTorsionForce_Execute<MDVM, UDF_Binding::SwopeTorsionForce_Params>
                                           ( aMDVM, params_struct );

          tot_udf_energy += aMDVM.mEnergy; 
          } //end for each SwopeTorsion tuple

          if (this->rt_params->verbosity > 2) //VERBOSITY
            {
            printf("      SwopeTorsion Energy = %0.16f\n", tot_udf_energy);
            }

        } //end calculate_SwopeTorsion()


      //**********************************************************************
      //**********************************************************************
      void calculate_ImproperDihedral( MDVM& aMDVM )
        {
        if (this->rt_params->verbosity > 3) //VERBOSITY
          {
          std::cout << "calculate_ImproperDihedral()" << std::endl;
          }

        typename ImproperDihedralTableT::iterator improper_itr = this->mImproperDihedralTable->begin();
        typename ImproperDihedralTableT::iterator improper_end = this->mImproperDihedralTable->end();

        double tot_udf_energy = 0; 

        for (; improper_itr != improper_end; ++improper_itr)
          {
          struct UDF_Binding::ImproperDihedralForce_Params params_struct;
          params_struct.k    = improper_itr->k;
          params_struct.psi0 = improper_itr->psi0;

          aMDVM.mEnergy = 0; 
          aMDVM.SetSiteRegMap( improper_itr->site1,
                               improper_itr->site2,
                               improper_itr->site3,
                               improper_itr->site4 );

          UDF_Binding::UDF_ImproperDihedralForce_Execute<MDVM, UDF_Binding::ImproperDihedralForce_Params>
                                           ( aMDVM, params_struct );

          tot_udf_energy += aMDVM.mEnergy; 
          } //end for each ImproperDihedral tuple

          if (this->rt_params->verbosity > 2) //VERBOSITY
            {
            printf("      ImproperDihedral Energy = %0.16f\n", tot_udf_energy);
            }
            
        } //end calculate_ImproperDihedral()

      //**********************************************************************
      //**********************************************************************
      void calculate_pair14( MDVM& aMDVM )
        {
        if (this->rt_params->verbosity > 3) //VERBOSITY
          {
          std::cout << "calculate_pair14()" << std::endl;
          }

        typename Pair14SitesTableT::iterator pair14_itr = this->mPair14SitesTable->begin();
        typename Pair14SitesTableT::iterator pair14_end = this->mPair14SitesTable->end();

        double tot_charge_energy = 0; 
        double tot_lj_energy = 0; 

        for (; pair14_itr != pair14_end; ++pair14_itr)
          {
          aMDVM.SetSiteRegMap( pair14_itr->site1,
                               pair14_itr->site2,
                               -1,
                               -1 );

          aMDVM.mEnergy = 0;
          struct UDF_Binding::Coulomb14_Params charge_params;
          charge_params.ChargeAxB = pair14_itr->charge;

          UDF_Binding::UDF_Coulomb14_Execute<MDVM, UDF_Binding::Coulomb14_Params>
                                           ( aMDVM, charge_params);
          tot_charge_energy += aMDVM.mEnergy; 

          aMDVM.mEnergy = 0;
          struct UDF_Binding::LennardJones14Force_Params lj_params;
          lj_params.epsilon = pair14_itr->lj_epsilon;
          lj_params.sigma   = pair14_itr->lj_rmin;

          UDF_Binding::UDF_LennardJones14Force_Execute<MDVM, UDF_Binding::LennardJones14Force_Params>
                                           ( aMDVM, lj_params);
          tot_lj_energy += aMDVM.mEnergy; 

          } //end for each Pair14Sites tuple

          if (this->rt_params->verbosity > 2) //VERBOSITY
            {
            printf("      Pair14 Charge Energy = %0.16f\n", tot_charge_energy);
            printf("      Pair14 LennardJones Energy = %0.16f\n", tot_lj_energy);
            }
        } //end calculate_pair14




      //**********************************************************************
      //**********************************************************************
      //**********************************************************************
      //**********************************************************************



      //**********************************************************************
      //**********************************************************************
      $$// REG_TABLE_TO_TYPE ( mNSQSiteInfoTable, NSQSiteInfoTableT );
      $$// REG_TABLE_TO_TYPE ( mNSQSiteToNSQSiteInfoTable, NSQSiteToNSQSiteInfoTableT );
      void calculate_single_NSQForces( int siteA,
                                       int siteB,
                                       MDVM& aMDVM )
        {
       
        int siteA_info_id = -1;
        int siteB_info_id = -1;


//**************************************************************************************************
// Below is SOMEWHAT LIKE what we'd want to do if SELECT_TO was working
//**************************************************************************************************
/*
        bool seen_siteA = false;
        bool seen_siteB = false;

        $$// SELECT_TO find_nsqsites, find_nsqsitesT
        $$//           ( site_id, info_id )
        $$//      FROM ( mNSQSiteToNSQSiteInfoTable )
        $$//     WHERE ( site_id=siteA or site_id=siteB );  //WHERE doesn't work

        typename find_nsqsitesT::iterator nsqsites_itr = find_nsqsites->begin();
        typename find_nsqsitesT::iterator nsqsites_end = find_nsqsites->end();

        for (; nsqsites_itr != nsqsites_end; ++nsqsites_itr)
          {
            if (nsqsites_itr->site_id == siteA)
              {
              siteA_info_id = nsqsites_itr->info_id;
              seen_siteA = true;
              }
            else if (nsqsites_itr->site_id == siteB)
              {
              siteB_info_id = nsqsites_itr->info_id;
              seen_siteB = true;
              }
            else if (seen_siteA && seen_siteB)
              {
              break;
              }
          } //end for every site_id
*/
//**************************************************************************************************
// Instead we form a key and lookup... more C++ than container type logic
//**************************************************************************************************
        NSQSiteToNSQSiteInfoTableT::Key siteA_key( this->rt_params->pst_id, siteA );
        NSQSiteToNSQSiteInfoTableT::iterator find_siteA = 
                        this->mNSQSiteToNSQSiteInfoTable->getTuple(siteA_key);
        if (find_siteA != this->mNSQSiteToNSQSiteInfoTable->end())
          {
          siteA_info_id = find_siteA->info_id;
          }

        NSQSiteToNSQSiteInfoTableT::Key siteB_key( this->rt_params->pst_id, siteB );
        NSQSiteToNSQSiteInfoTableT::iterator find_siteB = 
                        this->mNSQSiteToNSQSiteInfoTable->getTuple(siteB_key);
        if (find_siteB != this->mNSQSiteToNSQSiteInfoTable->end())
          {
          siteB_info_id = find_siteB->info_id;
          }
//**************************************************************************************************

        // Must grab params for both sites, then use Combiner
        struct UDF_Binding::NSQCoulomb_Params siteA_charge_params;
        struct UDF_Binding::NSQLennardJones_Params siteA_lj_params;

        struct UDF_Binding::NSQCoulomb_Params siteB_charge_params;
        struct UDF_Binding::NSQLennardJones_Params siteB_lj_params;
        
//**************************************************************************************************
// Below is SOMEWHAT LIKE what we'd want to do if SELECT_TO was working
//**************************************************************************************************
/*
        bool seen_siteA_params = false;
        bool seen_siteB_params = false;

        $$// SELECT_TO find_nsqinfo, find_nsqinfoT
        $$//           ( info_id, charge, lj_epsilon, lj_rmin )
        $$//      FROM ( mNSQSiteInfoTable )
        $$//     WHERE ( info_id=siteA_info_id or info_id=siteB_info_id ); //WHERE doesn't work
        
        typename find_nsqinfoT::iterator nsqinfo_itr = find_nsqinfo->begin();
        typename find_nsqinfoT::iterator nsqinfo_end = find_nsqinfo->end();

        for (; nsqinfo_itr != nsqinfo_end; ++nsqinfo_itr)
          {
            if (nsqinfo_itr->info_id == siteA_info_id)
              {
              siteA_charge_params.ChargeAxB = nsqinfo_itr->charge; 
              siteA_lj_params.epsilon       = nsqinfo_itr->lj_epsilon; 
              siteA_lj_params.sigma         = nsqinfo_itr->lj_rmin; 
              seen_siteA_params = true;
              }
            else if (nsqinfo_itr->info_id == siteB_info_id)
              {
              siteB_charge_params.ChargeAxB = nsqinfo_itr->charge; 
              siteB_lj_params.epsilon       = nsqinfo_itr->lj_epsilon; 
              siteB_lj_params.sigma         = nsqinfo_itr->lj_rmin; 
              seen_siteB_params = true;
              }
            else if (seen_siteA_params && seen_siteB_params)
              {
              break;
              }
          } //end for every info_id
*/
//**************************************************************************************************
// Instead we form a key and lookup... more C++ than container type logic
//**************************************************************************************************
        NSQSiteInfoTableT::Key infoA_key( this->rt_params->pst_id, siteA_info_id );
        NSQSiteInfoTableT::iterator find_infoA = 
                        this->mNSQSiteInfoTable->getTuple(infoA_key);
        if (find_infoA != this->mNSQSiteInfoTable->end())
          {
          siteA_charge_params.ChargeAxB = find_infoA->charge; 
          siteA_lj_params.epsilon       = find_infoA->lj_epsilon; 
          siteA_lj_params.sigma         = find_infoA->lj_rmin; 
          }

        NSQSiteInfoTableT::Key infoB_key( this->rt_params->pst_id, siteB_info_id );
        NSQSiteInfoTableT::iterator find_infoB = 
                        this->mNSQSiteInfoTable->getTuple(infoB_key);
        if (find_infoB != this->mNSQSiteInfoTable->end())
          {
          siteB_charge_params.ChargeAxB = find_infoB->charge; 
          siteB_lj_params.epsilon       = find_infoB->lj_epsilon; 
          siteB_lj_params.sigma         = find_infoB->lj_rmin; 
          }
//**************************************************************************************************
 

        // Now combine the parameters using CHARMM combiner

        struct UDF_Binding::NSQCoulomb_Params combined_charge_params;
        struct UDF_Binding::NSQLennardJones_Params combined_lj_params;

        UDF_NSQCoulomb::Combine < UDF_Binding::NSQCoulomb_Params > 
                                ( siteA_charge_params, 
                                  siteB_charge_params,
                                  combined_charge_params );
        
        UDF_NSQCHARMMLennardJonesForce::Combine < UDF_Binding::NSQLennardJones_Params > 
                                                ( siteA_lj_params, 
                                                  siteB_lj_params,
                                                  combined_lj_params );

    //std::cerr << "charge= " << combined_charge_params.ChargeAxB << std::endl;
    
        aMDVM.SetSiteRegMap( siteA,
                             siteB,
                             -1,
                             -1     );
       
        aMDVM.mEnergy = 0;
        aMDVM.setBufferOn();

        UDF_Binding::UDF_NSQCoulomb_Execute < MDVM > ( aMDVM, combined_charge_params );
        UDF_Binding::UDF_SwitchFunction_Execute < MDVM > ( aMDVM );

        aMDVM.setBufferOff();
        this->tot_nsqcharge_energy += aMDVM.mEnergy;
    
        aMDVM.mEnergy = 0;
        aMDVM.setBufferOn();

        UDF_Binding::UDF_NSQLennardJones_Execute < MDVM > ( aMDVM, combined_lj_params );
        UDF_Binding::UDF_SwitchFunction_Execute < MDVM > ( aMDVM );

        aMDVM.setBufferOff();
        this->tot_nsqlj_energy += aMDVM.mEnergy;

        } //end calculate_single_NSQForces()



      //**********************************************************************
      // clears local tables, reads DB2 and performs import of all data
      // for a given system ID
      //**********************************************************************
      void import_tables()
        {

        char import_predicate[IMPORT_PREDICATE_BUFFER_SZ];
        snprintf(import_predicate, IMPORT_PREDICATE_BUFFER_SZ, "pst_id=%d", this->rt_params->pst_id);

        $$// IMPORT_FROM_DB2_TABLE ( this->mStdBondTable, MDSYSTEM.StdHarmonicBondForce_TupleList)
        $$//                 WHERE ( import_predicate );

        $$// IMPORT_FROM_DB2_TABLE ( this->mStdAngleTable, MDSYSTEM.StdHarmonicAngleForce_TupleList)
        $$//                 WHERE ( import_predicate );
       
        $$// IMPORT_FROM_DB2_TABLE ( this->mUreyBradleyTable, MDSYSTEM.UreyBradleyForce_TupleList)
        $$//                 WHERE ( import_predicate );
        
        $$// IMPORT_FROM_DB2_TABLE ( this->mSwopeTorsionTable, MDSYSTEM.SwopeTorsionForce_TupleList)
        $$//                 WHERE ( import_predicate );
        
        $$// IMPORT_FROM_DB2_TABLE ( this->mImproperDihedralTable, MDSYSTEM.ImproperDihedralForce_TupleList)
        $$//                 WHERE ( import_predicate );

        $$// IMPORT_FROM_DB2_TABLE ( this->mPair14SitesTable, MDSYSTEM.pair14_sites )
        $$//                 WHERE ( import_predicate );

        $$// IMPORT_FROM_DB2_TABLE ( this->mNSQSiteInfoTable, MDSYSTEM.nsq_siteinfo )
        $$//                 WHERE ( import_predicate );

        $$// IMPORT_FROM_DB2_TABLE ( this->mNSQSiteToNSQSiteInfoTable, MDSYSTEM.nsq_site_to_nsq_siteinfo )
        $$//                 WHERE ( import_predicate );


        if (this->rt_params->verbosity > 2) //VERBOSITY
          {
          std::cout << "CHARMM_ForceModel Imported Tables:" << std::endl;
          std::cout << "   Number of rows in StdBondTable          : " << this->mStdBondTable->rowcount() << std::endl;
          std::cout << "   Number of rows in StdAngleTable         : " << this->mStdAngleTable->rowcount() << std::endl;
          std::cout << "   Number of rows in UreyBradleyTable      : " << this->mUreyBradleyTable->rowcount() << std::endl;
          std::cout << "   Number of rows in SwopeTorsionTable     : " << this->mSwopeTorsionTable->rowcount() << std::endl;
          std::cout << "   Number of rows in ImproperDihedralTable : " << this->mImproperDihedralTable->rowcount() << std::endl;
          std::cout << "   Number of rows in Pair14SitesTable          : " << this->mPair14SitesTable->rowcount() << std::endl;
          std::cout << "   Number of rows in NSQSiteInfoTable          : " << this->mNSQSiteInfoTable->rowcount() << std::endl;
          std::cout << "   Number of rows in NSQSiteToNSQSiteInfoTable : " << this->mNSQSiteToNSQSiteInfoTable->rowcount() << std::endl;
          if (this->rt_params->verbosity > 4) //VERBOSITY
            {
            this->mStdBondTable->print();
            this->mStdAngleTable->print();
            this->mUreyBradleyTable->print();
            this->mSwopeTorsionTable->print();
            this->mImproperDihedralTable->print();
            this->mPair14SitesTable->print();
            this->mNSQSiteInfoTable->print();
            this->mNSQSiteToNSQSiteInfoTable->print();
            }
          }

        } //end importParameterTables()


      //**********************************************************************
      //**********************************************************************
      void calculateListBasedForces( MDVM& aMDVM )
        {

        calculate_StdHarmonicBond(aMDVM);
        calculate_StdHarmonicAngle(aMDVM);
        calculate_UreyBradley(aMDVM);
        calculate_SwopeTorsion(aMDVM);
        calculate_ImproperDihedral(aMDVM);
        calculate_pair14(aMDVM);

        } //end calculateListBasedForces()

  }; //end class




//*****************************************************************************
//*****************************************************************************
// OPLSAA CLASS
//*****************************************************************************
//*****************************************************************************








//*****************************************************************************
//*****************************************************************************
// AMBER CLASS
//*****************************************************************************
//*****************************************************************************








} //end namespace

#endif
