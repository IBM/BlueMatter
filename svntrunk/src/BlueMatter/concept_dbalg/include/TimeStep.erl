// ***********************************************************************
// File: TimeStep.hpp
// Author: cwo 
// Date: August 14, 2006 
// Namespace: concept_dbalg
// Class: TimeStep
// Description: Performs loop for each MD timestep 
// ***********************************************************************
#ifndef INCLUDE_TIMESTEP_HPP
#define INCLUDE_TIMESTEP_HPP

#include <BlueMatter/concept_dbalg/VelVerletIntegrator.hpp>
#include <BlueMatter/concept_dbalg/ForceModels.hpp>
#include <BlueMatter/concept_dbalg/MDSystemData.hpp>
#include <BlueMatter/concept_dbalg/InMemMDVM.hpp>
#include <BlueMatter/concept_dbalg/RDG_IF.hpp>
#include <BlueMatter/concept_dbalg/RuntimeParams.hpp>
#include <BlueMatter/concept_dbalg/NSQInteractionSimpleUni.hpp>

#include <inmemdb/Table.hpp>
#include <inmemdb/QueryResult.hpp>
#include <inmemdb/QueryProcessor.hpp>

#include <stdlib.h>
#include <assert.h>
#include <iostream>


// DEFINES
#define TIMESTEP_SINGLETON_CLASS


//****************************************************************************
//****************************************************************************
// The following tables hold dynamic system information
//****************************************************************************
//****************************************************************************

$$// DEFINE_TABLE AtomPosTable
$$//              (int site_id, double pos_x, double pos_y, 
$$//               double pos_z )
$$//  PRIMARY_KEY ( site_id );

$$// DEFINE_TABLE AtomVelTable
$$//              (int site_id, double vel_x, double vel_y, 
$$//               double vel_z )
$$//  PRIMARY_KEY ( site_id );

$$// DEFINE_TABLE AtomForceTable
$$//              (int site_id, double force_x, double force_y, 
$$//               double force_z )
$$//  PRIMARY_KEY ( site_id );

$$// DEFINE_TABLE AtomMassTable
$$//              (int site_id, double mass )
$$//  PRIMARY_KEY ( site_id );




namespace concept_dbalg
{

  class TimeStep
  {
    private:    
      RuntimeParams *rt_params;
 
      //****************************************************************************
      // Private constructor
      //****************************************************************************
      #ifdef TIMESTEP_SINGLETON_CLASS
        static TimeStep *obj_instance;
      #endif
      TimeStep(RuntimeParams *rt_params)
        {
        this->rt_params = rt_params;
        
        $$// INSTANTIATE_TABLE( AtomPosTable, constructor_posTable, constructor_posTableT);
        this->mAtomPosTable = constructor_posTable;
        $$// INSTANTIATE_TABLE( AtomVelTable, constructor_velTable, constructor_velTableT);
        this->mAtomVelTable = constructor_velTable;
        $$// INSTANTIATE_TABLE( AtomForceTable, constructor_forceTable, constructor_forceTableT);
        this->mAtomForceTable = constructor_forceTable;
        $$// INSTANTIATE_TABLE( AtomMassTable, constructor_massTable, constructor_massTableT);
        this->mAtomMassTable = constructor_massTable;

        this->mIntegrator = IntegratorType::instance(this->rt_params);
        this->mForceModel = DefaultForceModel::instance(this->rt_params);
        this->mNSQInteractionMethod = DefaultNSQInteractionMethod::instance(this->rt_params);

        this->mMDSystemData = MDSystemData::instance(this->rt_params);

        } //end private constructor 



      //*************************************************************
      // ERL member variables (Dynamic information tables)

      $$// GET_TABLE_TYPEDEF( AtomPosTable, AtomPosTableT);
      AtomPosTableT *mAtomPosTable;
      $$// GET_TABLE_TYPEDEF( AtomVelTable, AtomVelTableT);
      AtomVelTableT *mAtomVelTable;
      $$// GET_TABLE_TYPEDEF( AtomForceTable, AtomForceTableT);
      AtomForceTableT *mAtomForceTable;
      $$// GET_TABLE_TYPEDEF( AtomMassTable, AtomMassTableT);
      AtomMassTableT *mAtomMassTable;


      typedef InMemMDVM< AtomPosTableT, AtomForceTableT > DefaultMDVM;

      typedef CHARMM_ForceModel< DefaultMDVM > DefaultForceModel;

      typedef NSQInteractionSimpleUni< AtomPosTableT,
                                       AtomForceTableT,
                                       DefaultForceModel,
                                       DefaultMDVM >
                DefaultNSQInteractionMethod;

      typedef VelVerletIntegrator< AtomPosTableT,
                                   AtomVelTableT,
                                   AtomForceTableT,
                                   AtomMassTableT,
                                   DefaultForceModel,
                                   DefaultNSQInteractionMethod,
                                   DefaultMDVM > 
                IntegratorType;


      //*************************************************************
      // Class Member variables 
      //*************************************************************
      IntegratorType *mIntegrator;
      DefaultForceModel *mForceModel;
      DefaultNSQInteractionMethod *mNSQInteractionMethod;
      MDSystemData *mMDSystemData;


      //****************************************************************************
      // Fill internal tables with initial system state
      //****************************************************************************
      void initialize_system()
        {
        this->mMDSystemData->import_tables(); 

        this->mForceModel->import_tables();
        this->mNSQInteractionMethod->import_tables();

        int num_sites = this->mMDSystemData->mSiteToSiteInfoTable->rowcount(); 

        if (! RDG_IF::read_DVS_initialization< AtomPosTableT,
                                               AtomVelTableT,
                                               MDSystemData::IntExtSiteIDsTableT >
                                             ( num_sites,
                                               0,                //start timestep=0
                                               this->rt_params,
                                               this->mAtomPosTable,
                                               this->mAtomVelTable,
                                               this->mMDSystemData->mIntExtSiteIDsTable ) )
          {
          std::cerr << "Error: Problem reading start pos/vel .DVS" << std::endl;
          exit(1);
          }

          //fill in masses from siteinfo
          // Should use SELECT_TO, etc !
          MDSystemData::SiteToSiteInfoTableT::iterator site_to_info_itr = this->mMDSystemData->mSiteToSiteInfoTable->begin();
          MDSystemData::SiteToSiteInfoTableT::iterator site_to_info_end = this->mMDSystemData->mSiteToSiteInfoTable->end();
          for (; site_to_info_itr != site_to_info_end; ++site_to_info_itr)
            {
            MDSystemData::SiteInfoTableT::iterator siteinfo_itr = this->mMDSystemData->mSiteInfoTable->begin();
            MDSystemData::SiteInfoTableT::iterator siteinfo_end = this->mMDSystemData->mSiteInfoTable->end();
            for (; siteinfo_itr != siteinfo_end; ++siteinfo_itr)
              {
              if (siteinfo_itr->info_id == site_to_info_itr->info_id)
                {
                AtomMassTableT::Key new_key( site_to_info_itr->site_id );
                AtomMassTableT::Tuple new_tuple( site_to_info_itr->site_id, siteinfo_itr->atomic_mass );

                this->mAtomMassTable->insert( new_key, new_tuple );
                break;
                }
              } //end for siteinfo match
            } //end for every site

          //fill in blank Force entries
          AtomPosTableT::iterator pos_itr = this->mAtomPosTable->begin();
          AtomPosTableT::iterator pos_end = this->mAtomPosTable->end();
          for (; pos_itr != pos_end; ++pos_itr)
            {
            AtomForceTableT::Key new_key( pos_itr->site_id );
            AtomForceTableT::Tuple new_tuple( pos_itr->site_id, 0, 0, 0 );

            this->mAtomForceTable->insert(new_key, new_tuple);
            }


        } //end initialize_system()












    public:
      //****************************************************************************
      // Constructors/Destructor
      //****************************************************************************
      ~TimeStep()
        {
        #ifdef TIMESTEP_SINGLETON_CLASS
          delete obj_instance;
          obj_instance = NULL;
        #endif
        }
      static TimeStep* instance(RuntimeParams *rt_params)
        { 
        #ifdef TIMESTEP_SINGLETON_CLASS
          if (obj_instance == NULL) 
            {
            obj_instance = new TimeStep(rt_params);
            assert(obj_instance != NULL);
            }
          return (obj_instance);
        #else
          return (new TimeStep(rt_params));
        #endif
        }

      //****************************************************************************
      //****************************************************************************
      void core_loop(long num_timesteps)
        {

        this->initialize_system();

        if (num_timesteps < 0)
          {
          num_timesteps = LONG_MAX;
          }

        // Compute Forces for timestep 0 to check against RDG
        this->mIntegrator->compute_ts_force( this->mAtomPosTable,
                                             this->mAtomForceTable,
                                             this->mForceModel,
                                             this->mNSQInteractionMethod );

        // Start the simulation loop
        for (long ts=1; ts < num_timesteps; ++ts)
          {

          std::cout << "Timestep: " << ts << std::endl;

          this->mIntegrator->integrate( this->mAtomPosTable,
                                        this->mAtomVelTable,
                                        this->mAtomForceTable,
                                        this->mAtomMassTable,
                                        this->mForceModel,
                                        this->mNSQInteractionMethod,
                                        this->rt_params->timestep_length );

          } //end for each timestep


        } //end core_loop()



  }; //end class

} //end namespace

#endif
