// ***********************************************************************
// File: NSQInteractionSimpleUni.hpp
// Author: cwo 
// Date: August 29, 2006 
// Namespace: concept_dbalg
// Class: NSQInteractionSimpleUni
// Description: NSQ Interactions Handler
//              Simplest method possible for UNIprocessor
// ***********************************************************************
#ifndef INCLUDE_NSQINTERACTION_SIMPLEUNI_HPP
#define INCLUDE_NSQINTERACTION_SIMPLEUNI_HPP


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

$$// DEFINE_TABLE AtomForceTable
$$//              (int site_id, double force_x, double force_y, 
$$//               double force_z )
$$//  PRIMARY_KEY ( site_id );

//****************************************************************************
//****************************************************************************
// The following are tables with MSD information 
//****************************************************************************
//****************************************************************************

$$// DEFINE_TABLE ExclusionSitesTable
$$//              ( int pst_id, int site1, int site2 )
$$//  PRIMARY_KEY ( pst_id, site1, site2);


extern double dk1;
double dk1 = 1.0;


namespace concept_dbalg
{
  
  template< class AtomPosTableT,
            class AtomForceTableT,
            class ForceModel,
            class MDVM >
  class NSQInteractionSimpleUni
  {
    private:    
      
      RuntimeParams *rt_params;

      //**********************************************************************
      // Taken from PeriodicImage.hpp
      //**********************************************************************
      //double dk1;
      inline double NearestInteger(const double x)
        {
        const double two10 = 1024.0 ;
        const double two50 = two10 * two10 * two10 * two10 * two10 ;
        const double two52 = two50 * 4.0 ;
        const double two51 = two50 * 2.0 ;
        const double offset = two52 + two51 ;

        // Force add and subtract of appropriate constant to drop lose fractional part
        // .. hide it from the compiler so the optimiser won't reassociate things ..
        const double losebits = (dk1*x) + offset ;
        const double result = (dk1*losebits) - offset ;
        return result ;
        } //end NearestInteger()
      //**********************************************************************
      // Taken from PeriodicImage.hpp
      //**********************************************************************
      inline double NearestImageInFullyPeriodicLine( const double a,
                                                     const double b,
                                                     const double Period,
                                                     const double ReciprocalPeriod )
        {
        const double d = b-a ; // 'Regular' distance between them, if small enough the result will be 'b'
        const double d_unit = d * ReciprocalPeriod ; // express with respect to unit periodicity,
                                                     // for -0.5 < d_unit < 0.5 result will be 'b'
        const double d_unit_rounded = NearestInteger( d_unit ) ;
        const double result = b - d_unit_rounded * Period ;
        return result ;
        } //end NearestImageInFullyPeriodicLine()


      //**********************************************************************
      //**********************************************************************
      inline typename AtomPosTableT::Tuple get_nearest_image( 
                                          typename AtomPosTableT::Tuple& posA, 
                                          typename AtomPosTableT::Tuple& posB )
        {
///*
        double nearestB_x = NearestImageInFullyPeriodicLine( posA.pos_x, 
                                                         posB.pos_x, 
                                                         this->rt_params->boundingbox_dim_x,
                                                         this->rt_params->inv_boundingbox_dim_x );
        double nearestB_y = NearestImageInFullyPeriodicLine( posA.pos_y, 
                                                         posB.pos_y, 
                                                         this->rt_params->boundingbox_dim_y,
                                                         this->rt_params->inv_boundingbox_dim_y );
        double nearestB_z = NearestImageInFullyPeriodicLine( posA.pos_z, 
                                                         posB.pos_z, 
                                                         this->rt_params->boundingbox_dim_z,
                                                         this->rt_params->inv_boundingbox_dim_z );

        return typename AtomPosTableT::Tuple(posB.site_id, nearestB_x, nearestB_y, nearestB_z);
//*/
//****************************************************************************  
// This reimaging only works for sites outside of  
/*
        // First Reimage posB to nearest
        double bnd_x = this->rt_params->boundingbox_dim_x;
        double bnd_y = this->rt_params->boundingbox_dim_y;
        double bnd_z = this->rt_params->boundingbox_dim_z;

        double trans_x = 0;
        double trans_y = 0;
        double trans_z = 0;

        AtomPosTableT::Tuple nearest_posB;

        int no_trans_count = 3;

        double p_x = (posB.pos_x - posA.pos_x);
        if ( p_x <= -bnd_x / 2.0 )
          trans_x = bnd_x;
        else if ( p_x > bnd_x / 2.0 )
          trans_x = -bnd_x;
        else
          no_trans_count--;

        double p_y = (posB.pos_y - posA.pos_y);
        if ( p_y <= -bnd_y / 2.0 )
          trans_y = bnd_y;
        else if ( p_y > bnd_y / 2.0 )
          trans_y = -bnd_y;
        else
          no_trans_count--;

        double p_z = (posB.pos_z - posA.pos_z);
        if ( p_z <= -bnd_z / 2.0 )
          trans_z = bnd_z;
        else if ( p_z > bnd_z / 2.0 )
          trans_z = -bnd_z;
        else
          no_trans_count--;

        if (no_trans_count)
          {
          nearest_posB.pos_x = posB.pos_x + trans_x;
          nearest_posB.pos_y = posB.pos_y + trans_y;
          nearest_posB.pos_z = posB.pos_z + trans_z;
          }
        else
          {
          nearest_posB.pos_x = posB.pos_x;
          nearest_posB.pos_y = posB.pos_y;
          nearest_posB.pos_z = posB.pos_z;
          }

        return typename AtomPosTableT::Tuple( posB.site_id, 
                                              nearest_posB.pos_x, 
                                              nearest_posB.pos_y,
                                              nearest_posB.pos_z);
*/
//****************************************************************************  
        } //end get_imaged_dist_sqrd()


   
      //**********************************************************************
      // Private constructor
      //**********************************************************************
      NSQInteractionSimpleUni(RuntimeParams *rt_params) 
        {
        this->rt_params = rt_params;

        // Used for Periodic Imaging
        //this->dk1 = 1.0;

        $$// INSTANTIATE_TABLE ( ExclusionSitesTable, ctor_ExclusionSitesTable, ctor_ExclusionSitesTableT);
        this->mExclusionSitesTable = ctor_ExclusionSitesTable;

        } //end private constructor()

      $$// GET_TABLE_TYPEDEF ( ExclusionSitesTable, ExclusionSitesTableT);
      ExclusionSitesTableT *mExclusionSitesTable;


    public:
      //**********************************************************************
      // Constructors/Destructors
      //**********************************************************************
      ~NSQInteractionSimpleUni()
        {
        }
      static NSQInteractionSimpleUni* instance(RuntimeParams *rt_params)
        { 
        return new NSQInteractionSimpleUni(rt_params);
        }
      
      
      //**********************************************************************
      //**********************************************************************
      void import_tables()
        {
        
        char import_predicate[IMPORT_PREDICATE_BUFFER_SZ];
        snprintf(import_predicate, IMPORT_PREDICATE_BUFFER_SZ, "pst_id=%d", this->rt_params->pst_id);

        $$// IMPORT_FROM_DB2_TABLE ( this->mExclusionSitesTable, MDSYSTEM.exclusion_sites )
        $$//                 WHERE ( import_predicate );

        if (this->rt_params->verbosity > 2) //VERBOSITY
          {
          std::cerr << "NSQInteractionSimpleUni Imported Tables:" << std::endl;
          std::cerr << "   Number of rows in ExclusionSitesTable       : " << this->mExclusionSitesTable->rowcount() << std::endl;
          if (this->rt_params->verbosity > 4) //VERBOSITY
            {
            this->mExclusionSitesTable->print();
            }
          }

        } //end import_tables()



      //**********************************************************************
      // Takes tables with all sites (that should be checked), 
      // updates forceTable with NSQ forces 
      //**********************************************************************
      $$// REG_TABLE_TO_TYPE ( mExclusionSitesTable, ExclusionSitesTableT );
      void handle_nsq_interactions(AtomPosTableT   *posTable,
                                   AtomForceTableT *forceTable,
                                   ForceModel      *forcemodel,
                                   MDVM& aMDVM)
        {
        if (this->rt_params->verbosity > 3) //VERBOSITY
          {
          std::cerr << "handle_NSQ_interactions()" << std::endl;
          }
        
        forcemodel->tot_nsqcharge_energy = 0;
        forcemodel->tot_nsqlj_energy = 0;
   
        // Check every unique pairing in position table for distance
        // If within cutoff, determine single NSQ force values
        double cutoff_sqrd = this->rt_params->cutoff_dist * this->rt_params->cutoff_dist;
        double switch_cutoff_sqrd = (this->rt_params->cutoff_dist + this->rt_params->switch_width) * 
                                    (this->rt_params->cutoff_dist + this->rt_params->switch_width);

        //Find the position of this site 
        typename AtomPosTableT::iterator posA_itr = posTable->begin(); 
        typename AtomPosTableT::iterator posA_end = posTable->end(); 
        for (; posA_itr != posA_end; ++posA_itr)
          {
          typename AtomPosTableT::Tuple posA = *posA_itr;
           
          typename AtomPosTableT::iterator posB_itr = posA_itr;
          ++posB_itr;

          for (; posB_itr != posA_end; ++posB_itr)
            {
            typename AtomPosTableT::Tuple unimaged_posB = *posB_itr;

            // Now determine distance and calculate NSQ forces

            typename AtomPosTableT::Tuple posB = this->get_nearest_image(posA, unimaged_posB); 

            double deltaX = (posA.pos_x - posB.pos_x); 
            double deltaY = (posA.pos_y - posB.pos_y); 
            double deltaZ = (posA.pos_z - posB.pos_z); 

            double dist_sqrd = (deltaX*deltaX) + (deltaY*deltaY) + (deltaZ*deltaZ);
#if 0
  std::cerr << "NSQ distance squared check - Sites (" << posA.site_id << "," << posB.site_id << ")=" << dist_sqrd << std::endl; 
#endif
            
            if (dist_sqrd < switch_cutoff_sqrd)
              {

//**************************************************************************************************
// Below is SOMEWHAT LIKE what we'd want to do if SELECT_TO predicate was working
//**************************************************************************************************
/*
              // Check if sites are on exclusion list, ifso, don't compute force
              bool on_exclusion_list = false;

              $$// SELECT_TO find_ex_sites, find_ex_sitesT
              $$//           ( site1, site2 )
              $$//      FROM ( mExclusionSitesTable )
              $$//     WHERE ( site1=posA.site_id and site2=posB.site_id ); //WHERE doesn't work

              typename find_ex_sitesT::iterator ex_sites_itr = find_ex_sites->begin();
              typename find_ex_sitesT::iterator ex_sites_end = find_ex_sites->end();

              for (; ex_sites_itr != ex_sites_end; ++ex_sites_itr)
                {
                if ( ( ex_sites_itr->site1 == posA.site_id && ex_sites_itr->site2 == posB.site_id ) ||
                     ( ex_sites_itr->site1 == posB.site_id && ex_sites_itr->site2 == posA.site_id ) )
                  {
                  on_exclusion_list = true;
                  break;
                  }
                }
              if (on_exclusion_list)
                {
                break;
                }
*/
//**************************************************************************************************
// Instead we form a key and do a direct table lookup
// (normally we dont really want to have to use pstid during select)
//**************************************************************************************************
              int site_lower = posA.site_id;
              int site_higher = posB.site_id;
              if (posA.site_id > posB.site_id)
                {
                site_lower = posB.site_id;
                site_higher = posA.site_id;
                }
              ExclusionSitesTableT::Key check_key( this->rt_params->pst_id,
                                                   site_lower,
                                                   site_higher);
              ExclusionSitesTableT::iterator find_exclusion = this->mExclusionSitesTable->getTuple(check_key);
              if (find_exclusion != this->mExclusionSitesTable->end())
                {
                continue;
                }
//**************************************************************************************************

#if 0
    std::cerr << "NSQInteraction: Calculating force on sites " << posA.site_id << " and " << posB.site_id << std::endl; 
#endif
              
              aMDVM.setPositionInCache(posB);
              forcemodel->calculate_single_NSQForces( posA.site_id,
                                                      posB.site_id,
                                                      aMDVM );

              aMDVM.setPositionInCache(unimaged_posB);

              } //end if within switch_cutoff_sqrd
             
            } //end for each position B in posTable

          } //end for each position A in posTable


        if (this->rt_params->verbosity > 2) //VERBOSITY
          {
          printf("      Charge Energy = %0.16f\n", forcemodel->tot_nsqcharge_energy);
          printf("      LJ Energy = %0.16f\n", forcemodel->tot_nsqlj_energy);
          }

        } //end handle_NSQ_interactions()


  }; //end class

} //end namespace

#endif
