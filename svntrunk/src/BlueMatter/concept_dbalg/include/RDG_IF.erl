// ***********************************************************************
// File: RDG_IF.hpp
// Author: cwo 
// Date: August 23, 2006 
// Namespace: concept_dbalg
// Class: RDG_IF
// Description:  Read/Write .dvs and .rdg files
// ***********************************************************************
#ifndef INCLUDE_RDGFILES_HPP
#define INCLUDE_RDGFILES_HPP

#define NO_PK_PLATFORM

#include <BlueMatter/concept_dbalg/RuntimeParams.hpp>

#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/BoundingBox.hpp>
#include <BlueMatter/XYZ.hpp>

#include <stdlib.h>
#include <string>
#include <assert.h>
#include <iostream>


//****************************************************************************
//****************************************************************************
// The following tables with MSD information 
//****************************************************************************
//****************************************************************************

$$// DEFINE_TABLE IntExtSiteIDsTable 
$$//              (int pst_id, int ext_site_id, int int_site_id)
$$//  PRIMARY_KEY ( pst_id, ext_site_id, int_site_id );

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





namespace concept_dbalg
{

  class RDG_IF
    {

    private:


    public:

      //***********************************************************************
      //***********************************************************************
      $$// REG_TABLE_TO_TYPE ( pos_table, AtomPosTable );
      $$// REG_TABLE_TO_TYPE ( vel_table, AtomVelTable );
      $$// REG_TABLE_TO_TYPE ( aIntExtSiteIDsTable, IntExtSiteIDsTable);
      template< class AtomPosTable,
                class AtomVelTable,
                class IntExtSiteIDsTable >
      static bool read_DVS_initialization( long number_of_sites,
                                           long timestep, 
                                           RuntimeParams *rt_params,
                                           AtomPosTable *aAtomPosTable,
                                           AtomVelTable *aAtomVelTable,
                                           IntExtSiteIDsTable *aIntExtSiteIDsTable)
        {
#if 0
  std::cerr << "read_DVS_initializiation():" << std::endl;
  std::cerr << "   number_of_sites: " << number_of_sites << std::endl;
  std::cerr << "   timestep: " << timestep << std::endl;
  std::cerr << "   rt_params->dvs_filename: " << rt_params->dvs_filename << std::endl;
#endif  

        //BoundingBox aBoundingBox;
        //int FirstInTS = 1;
        //int isRanNumGenStatePresent = 0;

        int dvs_fd = -1;
        dvs_fd = open( rt_params->dvs_filename.c_str(), O_RDONLY );

        if ( dvs_fd < 0 )
          {
          std::cerr << "Error Reading DVS file " << rt_params->dvs_filename.c_str() << std::endl;
          return false;
          }

        char record[ 1024 ];
        ED_Header hdr;

        for(;;)
          {
          if ( Packet::load( dvs_fd, record, hdr) != 0 )
            {
            break;
            }
          switch( hdr.mClass )
            {
            case ED_CLASS::Information:
              {
              switch( hdr.mType )
                {
                //************************************************************
                // Read in Bounding Box 
                //************************************************************
                case ED_INFORMATION_TYPES::RTP :
                  {
                  ED_InformationRTP dvs;
                  char * RecordPtr = &record[0];
                  ED_OpenPacket_InformationRTP( RecordPtr, dvs );

                  BoundingBox tempBox;
                  tempBox.mMinBox = dvs.mDynamicBoundingBoxMin;
                  tempBox.mMaxBox = dvs.mDynamicBoundingBoxMax;
                  XYZ dimensions = tempBox.GetDimensionVector();
                  
#if 1
  std::cerr << "   Read InfoRTP Packet: BoundingBox Dimensions = " << dimensions << std::endl;
#endif 
                  rt_params->boundingbox_dim_x = dimensions.mX;
                  rt_params->boundingbox_dim_y = dimensions.mY;
                  rt_params->boundingbox_dim_z = dimensions.mZ;
                  rt_params->inv_boundingbox_dim_x = 1.0 / dimensions.mX;
                  rt_params->inv_boundingbox_dim_y = 1.0 / dimensions.mY;
                  rt_params->inv_boundingbox_dim_z = 1.0 / dimensions.mZ;

                  break;
                  } // End case: INFORMATION_TYPES::RTP
                default:
                  {
                  }
                } // End of switch ( hdr.mType )
                break;
              } // End case: ED_CLASS::Information
            case ED_CLASS::DynamicVariables :
              {
              switch( hdr.mType )
                {
                //************************************************************
                // Read in Position and Velocity
                //************************************************************
                case ED_DYNAMICVARIABLES_TYPES::Site :
                  {
                  ED_DynamicVariablesSite dvs;
                  char *ptr = &record[0];
                  ED_OpenPacket_DynamicVariablesSite(ptr, dvs.mVoxelId, dvs.mSiteId, dvs.mPosition, dvs.mVelocity);

                  XYZ& pos_xyz = dvs.mPosition; 
                  XYZ& vel_xyz = dvs.mVelocity; 

                  // Get Internal site_id from external site id 
                  // (FIXME: should be a SELECT_TO, not a loop!)
                  IntExtSiteIDsTable::iterator site_id_itr = aIntExtSiteIDsTable->begin();
                  IntExtSiteIDsTable::iterator site_id_end = aIntExtSiteIDsTable->end();

                  int int_site_id = -1;
                  for(; site_id_itr != site_id_end; ++site_id_itr)
                    {
                    if (dvs.mSiteId == site_id_itr->ext_site_id)
                      {
                      int_site_id = site_id_itr->int_site_id;
                      break;
                      }
                    } 
#if 0
  std::cerr << "   Read Site Packet: Pos=" << pos_xyz << " Vel=" << vel_xyz << " IntsiteID=" << int_site_id << std::endl;
#endif 

                  // check bounds and then stuff dynamic vars into starting pos/vel.
                  if( int_site_id < 0 || int_site_id >= number_of_sites)
                    {
                    std::cerr << "Error: DVS Start contains PosVel packet for site that doesn't exist (int_site_id="
                              << int_site_id << ")" << std::endl;
                    return false;
                    }

                  AtomPosTable::Tuple pos_tuple(int_site_id, pos_xyz.mX, pos_xyz.mY, pos_xyz.mZ);
                  AtomPosTable::Key pos_key(int_site_id);

                  if (!(aAtomPosTable->insert( pos_key, pos_tuple )))
                    {
                    //must already be inserted, check to make sure packets match 
                    if (*(aAtomPosTable->getTuple(pos_key)) != pos_tuple)
                      {
                      std::cerr << "Error: DVS Start contained duplicate Pos/Vel packet with inconsistant values" << std::endl;
                      return false;
                      }
                    }
                  
                  AtomVelTable::Tuple vel_tuple(int_site_id, vel_xyz.mX, vel_xyz.mY, vel_xyz.mZ);
                  AtomVelTable::Key vel_key(int_site_id);

                  if (!(aAtomVelTable->insert( vel_key, vel_tuple )))
                    {
                    //must already be inserted, check to make sure packets match 
                    if (*(aAtomVelTable->getTuple(vel_key)) != vel_tuple)
                      {
                      std::cerr << "Error: DVS Start contained duplicate Pos/Vel packet with inconsistant values" << std::endl;
                      return false;
                      }
                    }
 
                  // Keep track of the true bounding box to report into log.
                  //if( FirstInTS )
                  //  {
                  //  FirstInTS = 0;
                  //  aBoundingBox.Init( dvs.mPosition );
                  //  }
                  //else
                  //  {
                  //  aBoundingBox.Update( dvs.mPosition );
                  //  }

                  break;
                  } // End case: pos/vel record for selected ts.
                default:
                  {
                  }
                } // End of switch ( hdr.mType )
                break;
              } // End of case:  ED_CLASS::DynamicVariables
            default:
              {
              }
            } // End of switch ( hdr.mClass )
          } // End of while scanning dvs file

        close(dvs_fd);

#if 0
  std::cerr << "   AtomPosTable rowcount= " << aAtomPosTable->rowcount() << std::endl;
  std::cerr << "   AtomVelTable rowcount= " << aAtomVelTable->rowcount() << std::endl;
#endif

        return true;

        } //end readDVSInitSites()



      //***********************************************************************
      //***********************************************************************
      static bool write_RDG_energy_packet( std::string& rdg_filename,
                                           int timestep,
                                           unsigned int UDF_code,
                                           double energy_val )
        {

/*
        // Create packet, write to file directly

typedef struct
{
    DCL_ED_HEADER;
    unsigned  mCode;
    unsigned  mcount;
    double  mValue;
    enum {PackedSize = 20,
          PackedTaggedSize = 20+TaggedPacket::TAGSIZE};
} ED_UDFd1;

        ED_FillPacket_UDFd1( packet_buf,
                             timestep,
                             UDF_code,
                             INT_MAX,
                             energy_val );

        write( myfile, packet_buf );

*/

        return true;
        } //end write_RDG_energy_packet()



    }; //end class RDG_IF


} //end namespace

#endif
