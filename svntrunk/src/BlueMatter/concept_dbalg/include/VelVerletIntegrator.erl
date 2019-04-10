// ***********************************************************************
// File: VelVerletIntegrator.hpp
// Author: cwo 
// Date: August 14, 2006 
// Namespace: concept_dbalg
// Class: VelVerletIntegrator
// Description: Integrator for core MD loop
// ***********************************************************************
#ifndef INCLUDE_VELVELOCITYINTEGRATOR_HPP
#define INCLUDE_VELVELOCITYINTEGRATOR_HPP

#include <BlueMatter/concept_dbalg/InMemMDVM.hpp>
#include <BlueMatter/concept_dbalg/RuntimeParams.hpp>

#include <inmemdb/Table.hpp>
#include <inmemdb/QueryResult.hpp>
#include <inmemdb/QueryProcessor.hpp>

#include <stdlib.h>
#include <assert.h>
#include <iostream>


// DEFINES



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
  
  template< class AtomPosTable,
            class AtomVelTable,
            class AtomForceTable,
            class AtomMassTable,
            class ForceModel,
            class NSQInteractionMethod,
            class MDVM >
  class VelVerletIntegrator
  {
    private:    
      
      RuntimeParams *rt_params;
      MDVM *mMDVM;
 
      //**********************************************************************
      // Private constructor
      //**********************************************************************
      VelVerletIntegrator(RuntimeParams *rt_params) 
        {
        this->mMDVM = MDVM::instance(rt_params);
        this->rt_params = rt_params;
        } //end private constructor()

    public:
      //**********************************************************************
      // Constructors/Destructors
      //**********************************************************************
      ~VelVerletIntegrator()
        {
        }
      static VelVerletIntegrator* instance(RuntimeParams *rt_params)
        { 
        return new VelVerletIntegrator(rt_params);
        }


      //**********************************************************************
      //
      //
      // Integration Eq: r_{n+1} = r_n + (v_{n+0.5})(dt)
      // Actually does : r_n     = r_n + (v_n)(delta_ts)
      //
      // Assumes: The tables are non-empty and parallel
      //**********************************************************************
      $$// REG_TABLE_TO_TYPE ( pos_table, AtomPosTable );
      $$// REG_TABLE_TO_TYPE ( vel_table, AtomVelTable );
      void compute_ts_pos(AtomPosTable *pos_table,
                                        AtomVelTable *vel_table,
                                        double delta_ts)
        {
        if (this->rt_params->verbosity > 3) //VERBOSITY
          {
          std::cerr << "compute_ts_pos()" << std::endl;
          }
          
        AtomPosTable::iterator pos_itr = pos_table->begin();
        AtomPosTable::iterator pos_end = pos_table->end();
        AtomVelTable::iterator vel_itr = vel_table->begin();
        AtomVelTable::iterator vel_end = vel_table->end();
        
        while( pos_itr != pos_end &&
               vel_itr != vel_end )
          {

          if (pos_itr->site_id != vel_itr->site_id )
            {
            assert(0);
            } 

          double next_pos_x = pos_itr->pos_x + (vel_itr->vel_x * delta_ts);
          double next_pos_y = pos_itr->pos_y + (vel_itr->vel_y * delta_ts);
          double next_pos_z = pos_itr->pos_z + (vel_itr->vel_z * delta_ts);

          AtomPosTable::Key new_key( pos_itr->site_id );
          AtomPosTable::Tuple new_entry( pos_itr->site_id,
                                         next_pos_x,
                                         next_pos_y,
                                         next_pos_z );

          pos_table->removeTuple(pos_itr++);
          pos_table->insert(new_key, new_entry);
          ++vel_itr;
          } //end for each itr
        
        } //compute_ts_pos()


      //**********************************************************************
      //
      //
      // Integration Eq: v_{n+0.5} = v_n + (f_n)(dt)/(2m)
      // Actually does : v_n       = v_n + (f_n)(delta_ts)/(2*m_n)
      //
      // Assumes: The tables are non-empty and parallel
      //**********************************************************************
      $$// REG_TABLE_TO_TYPE ( vel_table, AtomVelTable );
      $$// REG_TABLE_TO_TYPE ( force_table, AtomForceTable);
      $$// REG_TABLE_TO_TYPE ( mass_table, AtomMassTable);
      void compute_ts_halfvelocity(AtomVelTable *vel_table, 
                                                 AtomForceTable *force_table,
                                                 AtomMassTable *mass_table,
                                                 double delta_ts)
        {
        //ASSUMES that tables output site_id's in same order during iteration
        if (this->rt_params->verbosity > 3) //VERBOSITY
          {
          std::cerr << "compute_ts_halfvelocity()" << std::endl;
          }
          
        AtomVelTable::iterator vel_itr = vel_table->begin();
        AtomVelTable::iterator vel_end = vel_table->end();
        AtomForceTable::iterator force_itr = force_table->begin();
        AtomForceTable::iterator force_end = force_table->end();
        AtomMassTable::iterator mass_itr = mass_table->begin();
        AtomMassTable::iterator mass_end = mass_table->end();

        while( vel_itr != vel_end &&
               mass_itr != mass_end &&
               force_itr != force_end ) 
          {
          if (vel_itr->site_id != force_itr->site_id ||
              vel_itr->site_id != mass_itr->site_id )
            {
            assert(0);
            } 

          double next_vel_x = vel_itr->vel_x + 
                       ( (force_itr->force_x * delta_ts) / (2 * mass_itr->mass)); 
          double next_vel_y = vel_itr->vel_y + 
                       ( (force_itr->force_y * delta_ts) / (2 * mass_itr->mass)); 
          double next_vel_z = vel_itr->vel_z + 
                       ( (force_itr->force_z * delta_ts) / (2 * mass_itr->mass)); 

          AtomVelTable::Key new_key( vel_itr->site_id );
          AtomVelTable::Tuple new_entry( vel_itr->site_id,
                                         next_vel_x,
                                         next_vel_y,
                                         next_vel_z);
          
          vel_table->removeTuple(vel_itr++);
          vel_table->insert(new_key, new_entry);
          ++mass_itr;
          ++force_itr;
          } // end for each tuple

        } //end compute_ts_halfvelocity()


      //**********************************************************************
      // Use r_n to compute force f_n
      //**********************************************************************
      $$// REG_TABLE_TO_TYPE ( computeforce_pos_table, AtomPosTable );
      $$// REG_TABLE_TO_TYPE ( computeforce_force_table, AtomForceTable );
      void compute_ts_force(AtomPosTable *computeforce_pos_table,
                            AtomForceTable *computeforce_force_table,
                            ForceModel *forcemodel,
                            NSQInteractionMethod *nsq_method)
        {
        if (this->rt_params->verbosity > 3) //VERBOSITY
          {
          std::cerr << "compute_ts_force()" << std::endl;
          }
          
        this->mMDVM->resetInternalCaches(computeforce_pos_table,
                                        computeforce_force_table);
      
        forcemodel->calculateListBasedForces(*(this->mMDVM));
        nsq_method->handle_nsq_interactions( computeforce_pos_table,
                                             computeforce_force_table,
                                             forcemodel,
                                             *(this->mMDVM) );

        this->mMDVM->readForceCache(computeforce_force_table);

#if 0
/*
        AtomForceTable::iterator force_itr = computeforce_force_table->begin();
        AtomForceTable::iterator force_end = computeforce_force_table->end();
        for (; force_itr != force_end; ++force_itr)
          {
          if ( force_itr->force_x != 0 ||
               force_itr->force_y != 0 ||
               force_itr->force_z != 0   )
            {
            std::cerr << "Force site=" << force_itr->site_id << " [ "
                      << force_itr->force_x << " , "
                      << force_itr->force_y << " , "
                      << force_itr->force_z << " ] " << std::endl;
            }
          }
*/
        AtomPosTable::iterator pos_itr = computeforce_pos_table->begin();
        AtomPosTable::iterator pos_end = computeforce_pos_table->end();
        for (; pos_itr != pos_end; ++pos_itr)
          {
          if ( pos_itr->site_id < 13 )
            {
            std::cerr << "Pos site=" << pos_itr->site_id << " [ "
                      << pos_itr->pos_x << " , "
                      << pos_itr->pos_y << " , "
                      << pos_itr->pos_z << " ] " << std::endl;
            }
          }

#endif

        } //end compute_ts_force()


      //**********************************************************************
      //**********************************************************************
      void integrate(AtomPosTable         *posTable,
                     AtomVelTable         *velTable,
                     AtomForceTable       *forceTable,
                     AtomMassTable        *massTable,
                     ForceModel           *forcemodel,
                     NSQInteractionMethod *nsq_method,
                     double                delta_ts)
        {
        if (this->rt_params->verbosity > 3) //VERBOSITY
          {
          std::cerr << "integrate(delta_ts=" << delta_ts << ")" << std::endl;
          //std::cerr << "   AtomPosTable.sz=" << posTable->rowcount() << std::endl;
          //std::cerr << "   AtomVelTable.sz=" << velTable->rowcount() << std::endl;
          //std::cerr << "   AtomForceTable.sz=" << forceTable->rowcount() << std::endl;
          //std::cerr << "   AtomMassTable.sz=" << massTable->rowcount() << std::endl;
          }

        
        this->compute_ts_halfvelocity(velTable, forceTable, massTable, delta_ts);

        this->compute_ts_pos(posTable, velTable, delta_ts);

        this->compute_ts_force(posTable, forceTable, forcemodel, nsq_method);

        this->compute_ts_halfvelocity(velTable, forceTable, massTable, delta_ts);

        } //end integrate()


  }; //end class

} //end namespace

#endif
