// ***********************************************************************
// File: InMemMDVM.hpp
// Author: cwo 
// Date: August 14, 2006 
// Namespace: concept_dbalg
// Class: InMemMDVM
// Description: Fakes MDVM for inmemory containers 
// ***********************************************************************
#ifndef INCLUDE_INMEMMDVM_HPP
#define INCLUDE_INMEMMDVM_HPP

#include <BlueMatter/concept_dbalg/RuntimeParams.hpp>

#include <BlueMatter/math.hpp>
#include <BlueMatter/MDVM_BaseIF.hpp>
#include <BlueMatter/XYZ.hpp>

#include <inmemdb/Table.hpp>
#include <inmemdb/QueryResult.hpp>
#include <inmemdb/QueryProcessor.hpp>

#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <map>


// DEFINES

#define SITE_REG_COUNT   4

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





namespace concept_dbalg
{

  template< class AtomPosTableT,
            class AtomForceTableT >
  class InMemMDVM : public MDVM_BaseIF
  {
    private:    
      RuntimeParams *rt_params;

      ////////////////////////////////////////
      // Cache position and Force values
      // use utility functions to sync with tables
      typedef typename std::map<int, XYZ> XYZCacheT;
      XYZCacheT mPositionCache; 
      XYZCacheT mForceCache; 

      int mSiteRegMap[SITE_REG_COUNT];

      bool mBufferReports;
      XYZ mBufferedForce[SITE_REG_COUNT];
      double mBufferedEnergy;
        
      double SwitchS;
      double SwitchdSdR;

      SwitchFunction mSF;

      //****************************************************************************
      // Private constructor
      //****************************************************************************
      InMemMDVM(RuntimeParams *rt_params)
        {
        this->rt_params = rt_params;
        this->mEnergy = 0;
        for (int i=0; i < SITE_REG_COUNT; ++i)
          {
          this->mSiteRegMap[i] = -1;
          }

        //Set switch function member vars
        bool mBufferReports = false;
        double SwitchS = 0;
        double SwitchdSdR = 0;

        //Setup switch function object
        SwitchFunctionRadii ctor_SwitchFunctionRadii;
        ctor_SwitchFunctionRadii.SetRadii( rt_params->cutoff_dist, 
                                           rt_params->cutoff_dist + rt_params->switch_width );

        this->mSF.Set( ctor_SwitchFunctionRadii ); 

        } //end constructor() 

    public:
      
      double mEnergy;

      enum
        {
          kSuppressVectorReporting = 1
        };


      //****************************************************************************
      // Constructors/Destructor
      //****************************************************************************
      ~InMemMDVM()
        {
        }
      static InMemMDVM* instance(RuntimeParams *rt_params)
        { 
        return (new InMemMDVM(rt_params));
        }
      
      //****************************************************************************
      //****************************************************************************
      inline void setPositionInCache (typename AtomPosTableT::Tuple& pos)
        {
        XYZ pos_entry; 
        pos_entry.Set( pos.pos_x,
                       pos.pos_y,
                       pos.pos_z);

        if (! this->mPositionCache.insert(std::make_pair(pos.site_id, pos_entry)).second) 
          {
          this->mPositionCache.erase(pos.site_id);
          this->mPositionCache.insert(std::make_pair(pos.site_id, pos_entry));
          }

        } //end setPosInCache
 
      //****************************************************************************
      //****************************************************************************
      inline void setForceInCache (typename AtomForceTableT::Tuple& force)
        {
        XYZ force_entry; 
        force_entry.Set( force.force_x,
                         force.force_y,
                         force.force_z);

        if (! this->mForceCache.insert(std::make_pair(force.site_id, force_entry)).second) 
          {
          this->mForceCache.erase(force.site_id);
          this->mForceCache.insert(std::make_pair(force.site_id, force_entry));
          }

        } //end resetPosInCache
      
      //****************************************************************************
      // Writes Table positions to internal cache
      // Writes current forces to internal cache
      //****************************************************************************
      $$// REG_TABLE_TO_TYPE ( MDVM_pos_table, AtomPosTableT );
      $$// REG_TABLE_TO_TYPE ( MDVM_force_table, AtomForceTableT );
      void resetInternalCaches( AtomPosTableT *MDVM_pos_table,
                                AtomForceTableT *MDVM_force_table ) 
        {
        // Clear out and set positions/forces
        this->mPositionCache.clear();
        this->mForceCache.clear();

        AtomPosTableT::iterator pos_itr = MDVM_pos_table->begin();
        AtomPosTableT::iterator pos_end = MDVM_pos_table->end();

        for (; pos_itr != pos_end; ++pos_itr)
          {
          setPositionInCache( *pos_itr );
          }
          
        AtomForceTableT::iterator force_itr = MDVM_force_table->begin();
        AtomForceTableT::iterator force_end = MDVM_force_table->end();

        for (; force_itr != force_end; ++force_itr)
          {
          setForceInCache( *force_itr );
          }

        } //end resetInternalCaches()

      //****************************************************************************
      // Utility function, reads out internal cache of forces to table
      //****************************************************************************
      void readForceCache( AtomForceTableT* MDVM_force_table ) 
        {

        AtomForceTableT::iterator force_itr = MDVM_force_table->begin();
        AtomForceTableT::iterator force_end = MDVM_force_table->end();

        while ( force_itr != force_end )
          {
          XYZ force_xyz = XYZ::INVALID_VALUE();

          XYZCacheT::iterator find_force = this->mForceCache.find(force_itr->site_id);
          if (find_force != this->mForceCache.end())
            {
            force_xyz = find_force->second;
            }

          AtomForceTableT::Key new_key( force_itr->site_id );
          AtomForceTableT::Tuple new_entry( force_itr->site_id,
                                           force_xyz.mX,
                                           force_xyz.mY,
                                           force_xyz.mZ);

#if 0
      XYZ check_force;
      check_force.mX = force_itr->force_x;
      check_force.mY = force_itr->force_y;
      check_force.mZ = force_itr->force_z;
      std::cerr << "Old Force =" << check_force << std::endl;
      std::cerr << "New Force =" << force_xyz << std::endl;
#endif

          MDVM_force_table->removeTuple(force_itr++);
          MDVM_force_table->insert(new_key, new_entry);
          }
        }

      //****************************************************************************
      //****************************************************************************
      inline void SetSiteRegMap( int aUserSiteInReg0,
                                 int aUserSiteInReg1,
                                 int aUserSiteInReg2,
                                 int aUserSiteInReg3 )
        {
          this->mSiteRegMap[ 0 ] = aUserSiteInReg0;
          this->mSiteRegMap[ 1 ] = aUserSiteInReg1;
          this->mSiteRegMap[ 2 ] = aUserSiteInReg2;
          this->mSiteRegMap[ 3 ] = aUserSiteInReg3;
        }

      //****************************************************************************
      //****************************************************************************
      inline void SetSiteForRegMap( const int site, const int aUserSiteInReg )
        {
        this->mSiteRegMap[ site ] = aUserSiteInReg;
        }

      //****************************************************************************
      //****************************************************************************
      inline XYZ& GetPosition( const int aUserSite )
        {
        int actual_site_id = this->mSiteRegMap[ aUserSite ];

        XYZCacheT::iterator ret_xyz = this->mPositionCache.find(actual_site_id);

        if (ret_xyz != this->mPositionCache.end())
          {
          return ret_xyz->second;
          }
        else 
          {
          XYZ *invalid_XYZ = new XYZ; //FIXME
          invalid_XYZ->Set(INVALID_VAL, INVALID_VAL, INVALID_VAL);
          return *invalid_XYZ;
          }
        } //end GetPosition()
      
      //****************************************************************************
      //****************************************************************************
      inline SiteId GetSiteId( int index ) 
        {
        assert (index >= 0 && index < SITE_REG_COUNT);

        return (this->mSiteRegMap[ index ]);
        } //end GetSiteId()

      //****************************************************************************
      //****************************************************************************
      inline XYZ GetVector( int aUserSite0, int aUserSite1 ) 
        {
        // For now, just compute, but should do caching soon.
        XYZ Position0 = this->GetPosition( aUserSite0 );
        XYZ Position1 = this->GetPosition( aUserSite1 );

        XYZ vector01 = Position0 - Position1;

        return (vector01);
        } //end GetVector()


      //****************************************************************************
      //****************************************************************************
      inline const double GetReciprocalDistance( int aUserSite0, int aUserSite1 )
        {
        double distance_squared = GetDistanceSquared( aUserSite0, aUserSite1 );
        double reciprocal_dist = 1.0 / (sqrt( distance_squared ));

        return (reciprocal_dist);
        } //end GetReciprocalDistance()

      //****************************************************************************
      //****************************************************************************
      inline const double GetDistanceSquared( int aUserSite0, int aUserSite1 )
        {
        XYZ vectorAB = GetVector(aUserSite0, aUserSite1);
        return vectorAB.LengthSquared();
        } // GetDistanceSquared()

      //****************************************************************************
      //****************************************************************************
      inline const double GetDistance( int aUserSite0, int aUserSite1 )
        {
        double distance_squared = GetDistanceSquared( aUserSite0, aUserSite1 );
        double distance = sqrt( distance_squared );

        return (distance);
        } //end GetDistance()

      //****************************************************************************
      //****************************************************************************
      inline void ReportdEdr( double adEdr )
        {
         //mdEdr = adEdr ;
         double dVecABMagR = GetReciprocalDistance( SiteA, SiteB ) ;
         XYZ dVecAB = GetVector( SiteA, SiteB ) ;
         XYZ fab = dVecAB * dVecABMagR * (-adEdr);

         ReportForce( SiteA ,   fab ) ;
         ReportForce( SiteB , - fab ) ;
        } //end ReportdEdr()


      //****************************************************************************
      //****************************************************************************
      inline void ReportEnergy( double aEnergy )
        {
        if (!this->mBufferReports)
          {
          this->mEnergy = aEnergy;
          }
        else
          {
          this->mBufferedEnergy = aEnergy;
          }

//  std::cerr << std::scientific << " Energy=" << aEnergy << std::endl;
        } //end ReportEnergy()


      //****************************************************************************
      //****************************************************************************
      inline void ReportForce( const int aUserSite, const XYZ& aForce )
        {

        if (!this->mBufferReports)
          {
          int actual_site_id = this->mSiteRegMap[ aUserSite ];

          XYZCacheT::iterator find_force = this->mForceCache.find(actual_site_id);
          if (find_force != this->mForceCache.end())
            {
            XYZ& force_xyz = find_force->second;
            force_xyz += aForce;
            this->mForceCache.erase(find_force);
            this->mForceCache.insert(std::make_pair(actual_site_id, force_xyz));
            }
          else { assert(0); }
          }
        else
          {
          mBufferedForce[ aUserSite ] = aForce;
          }
 
//  std::cerr << " Force=" << aForce << std::endl;

        } //end ReportForce()

      //****************************************************************************
      //****************************************************************************
      inline void setBufferOn()
        {
        this->mBufferReports = true;
        }
     
      //****************************************************************************
      //****************************************************************************
      inline void setBufferOff()
        {

        for (int i=0; i < SITE_REG_COUNT; ++i)
          {
          int actual_site_id = this->mSiteRegMap[ i ];

          XYZCacheT::iterator find_force = this->mForceCache.find(actual_site_id);
          if (find_force != this->mForceCache.end())
            {
            this->mForceCache.erase(find_force);
            this->mForceCache.insert(std::make_pair(actual_site_id, mBufferedForce[ i ]));
            }
          else { assert(0); }
          } //end for each SITE_REG_COUNT

        this->mEnergy = this->mBufferedEnergy;
        this->mBufferReports = false;

        } //end setBufferOff()

      //****************************************************************************
      //****************************************************************************
      inline void EvaluateSwitch(double aDistance, double &aS, double &aPdSdR)
        {
        mSF.Evaluate(aDistance, aS, aPdSdR);
        std::cerr << "aS=" << aS << " aPdSdR=" << aPdSdR << std::endl; //FIXME FIXME
        } //end EvaluateSwitch()

      //****************************************************************************
      //****************************************************************************
      inline void ReportSwitch(double S, double dSdR)
        {
        this->SwitchS = S ;
        this->SwitchdSdR = dSdR ;
//        std::cerr << "S=" << S << " PdSdR=" << dSdR << std::endl; //FIXME FIXME
        }
      
      //****************************************************************************
      //****************************************************************************
      inline double GetSwitchS(void)
        {
        return this->SwitchS ;
        }

      //****************************************************************************
      //****************************************************************************
      inline double GetSwitchdSdR(void)
        {
        return this->SwitchdSdR;
        }
      
      //****************************************************************************
      //****************************************************************************
      inline const double GetEnergy(void)
        {
        if (!this->mBufferReports)
          {
          return this->mEnergy;
          }
        else
          {
          return this->mBufferedEnergy;
          }
        } //end GetEnergy()
      
      //****************************************************************************
      //****************************************************************************
      inline const XYZ GetForce( const int aRegisterIndex )
        {

        if (!this->mBufferReports)
          {
          int actual_site_id = this->mSiteRegMap[ aRegisterIndex ];

          XYZCacheT::iterator find_force = this->mForceCache.find(actual_site_id);
          if (find_force != this->mForceCache.end())
            {
            return find_force->second;
            }
          else { assert(0); }
          }
        else
          {
          return mBufferedForce[ aRegisterIndex ];
          }
          
        //THIS SHOULD NEVER BE REACHED
        XYZ dummy_xyz;
        dummy_xyz.Set(0,0,0);
        return dummy_xyz;

        } //end GetForce()

      //****************************************************************************
      //****************************************************************************
      inline void ApplySwitch(void)
        {
        XYZ force = GetForce( SiteA );
        double energy = GetEnergy();
        double S = GetSwitchS() ;
        double pdSdR = GetSwitchdSdR() ;


        XYZ dVecAB = GetVector( SiteA, SiteB );
        double dVecABMagR = GetReciprocalDistance( SiteA, SiteB );
        XYZ unitVector = dVecAB*dVecABMagR;

        XYZ ForceSwitched = force*S - unitVector*energy*pdSdR;
        double EnergySwitched = energy * S;

//  std::cerr << "energy = " << energy << " S= " << S << " Force= " << force << std::endl;
//  std::cerr << "EnergySwitched= " << EnergySwitched << " ForceSwitched= " << ForceSwitched << std::endl; //FIXME FIXME
        ReportEnergy( EnergySwitched );

        ReportForce( SiteA,   ForceSwitched );
        ReportForce( SiteB, - ForceSwitched );
        } //end ApplySwitch()

      //****************************************************************************
      //****************************************************************************




  }; //end class

} //end namespace

#endif
