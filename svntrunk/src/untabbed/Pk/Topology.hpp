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
 #ifndef __PK_TOPOLOGY_HPP
#define __PK_TOPOLOGY_HPP

#if !defined(PKFXLOG_TOPOLOGY)
#define PKFXLOG_TOPOLOGY (1)
#endif

#include <Pk/XYZ.hpp>

// Make the 'TorusXYZ' agree with the pk/platform one
// typedef tXYZ<signed char> TorusXYZ;
typedef cXYZ TorusXYZ;

inline
TorusXYZ
PkTopo3DMakeXYZ( int aRank )
  {
  int x, y, z;
  BGLPartitionMakeXYZ( aRank, &x, &y, &z );
  TorusXYZ rc;
  rc.mX = x;
  rc.mY = y;
  rc.mZ = z;
  return(rc);
  }

inline
int
PkTopo3DMakeRank( int aX, int aY, int aZ )
  {
  // Convert Coord to BGL Rank;
  int BGLRank = BGLPartitionMakeRank( aX, aY, aZ );
  return( BGLRank );
  }
  
#define PA_MEMORY_ALIGN(NUMBITS) __attribute__((aligned(0x01<<NUMBITS)))
  
struct SGE
{
int mRank;
TorusXYZ mCoord;
} PA_MEMORY_ALIGN( 5 );

#define GDIM 3
class GroupContainer_Actors
{
public:
  double padding0[4];
    int   mMyRank ;
    int   mSize ;
    SGE*  mElements ;
    int   mPlaneMask[ GDIM ] PA_MEMORY_ALIGN( 5 );
    int   mDim ;
    int   mDimSize[ GDIM ] PA_MEMORY_ALIGN( 5 );
    int   mMyCoord[ GDIM ] PA_MEMORY_ALIGN( 5 );
    int * mBGLRankToGroupRankMap PA_MEMORY_ALIGN( 5 );
    int   mMachineSize ;
  double padding1[4];

    void DumpState(int i)
      {
      printf("DumpState i: %d mMyRank: %d\n", i, mMyRank );
//       printf("mSize: %d\n", mSize );
//       printf("mPlaneMask: { %d %d %d }\n", mPlaneMask[0], mPlaneMask[1], mPlaneMask[2] );
//       printf("mDimSize: { %d %d %d }\n", mDimSize[0], mDimSize[1], mDimSize[2] );
//       for( int i=0; i<mSize; i++ )
//         {
//         printf("mElements[ %d ].mRank= %d mElements[ %d ].mCoord= { %d %d %d }\n",
//                i, mElements[ i ].mRank,
//                i, mElements[ i ].mCoord.mX, mElements[ i ].mCoord.mY, mElements[ i ].mCoord.mZ );
//         }
      }

    void
    Init( int aPlaneMask[ GDIM ], int aDimX=1, int aDimY=1, int aDimZ=1 )
      {
      int MyCoordX, MyCoordY, MyCoordZ;
      BGLPartitionGetCoords( &MyCoordX, &MyCoordY, &MyCoordZ );

      mMyCoord[ 0 ] = MyCoordX;
      mMyCoord[ 1 ] = MyCoordY;
      mMyCoord[ 2 ] = MyCoordZ;

      int MachineDimX, MachineDimY, MachineDimZ;
      BGLPartitionGetDimensions( &MachineDimX, &MachineDimY, &MachineDimZ );

      mMachineSize = MachineDimX * MachineDimY * MachineDimZ;

      mBGLRankToGroupRankMap = (int *) PkHeapAllocate( sizeof( int ) * mMachineSize );
      assert( mBGLRankToGroupRankMap );

      for( int i=0; i < mMachineSize; i++ )
        {
        mBGLRankToGroupRankMap[ i ] = -1;
        }

      memcpy(mPlaneMask, aPlaneMask, sizeof(int) * GDIM );

      // index = (DimY*x + y)*DimZ + z

      mDim = mPlaneMask[ 0 ] + mPlaneMask[ 1 ] + mPlaneMask[ 2 ];

      mDimSize[ 0 ] = mPlaneMask[ 0 ] ? aDimX : 1;
      mDimSize[ 1 ] = mPlaneMask[ 1 ] ? aDimY : 1;
      mDimSize[ 2 ] = mPlaneMask[ 2 ] ? aDimZ : 1;

      mSize = mDimSize[ 0 ] * mDimSize[ 1 ] * mDimSize[ 2 ];

      mElements = (SGE *) PkHeapAllocate( sizeof( SGE ) * mSize );
      assert( mElements );

      int xMyCoord = mPlaneMask[ 0 ] ? MyCoordX : 0;
      int yMyCoord = mPlaneMask[ 1 ] ? MyCoordY : 0;
      int zMyCoord = mPlaneMask[ 2 ] ? MyCoordZ : 0;

      for( int x=0; x < mDimSize[ 0 ]; x++ )
        {
        for( int y=0; y < mDimSize[ 1 ]; y++ )
          {
          for( int z=0; z < mDimSize[ 2 ]; z++ )
            {
            int index = (mDimSize[ 1 ] * x + y) * mDimSize[ 2 ] + z;
            assert( index >=0 && index < mSize );

            mElements[ index ].mRank = index;
            mElements[ index ].mCoord.Set( mPlaneMask[ 0 ] ? x : mMyCoord[ 0 ],
                                           mPlaneMask[ 1 ] ? y : mMyCoord[ 1 ],
                                           mPlaneMask[ 2 ] ? z : mMyCoord[ 2 ] );

            int BGLRank = BGLPartitionMakeRank( mElements[ index ].mCoord.mX,
                                                mElements[ index ].mCoord.mY,
                                                mElements[ index ].mCoord.mZ );

            mBGLRankToGroupRankMap[ BGLRank ] = index;

            if( x==xMyCoord && y==yMyCoord && z==zMyCoord )
              mMyRank = index;
            }
          }
        }
      }

    inline int GetSize()
      {
      return mSize;
      }

    inline int GetMyRank()
      {
      return mMyRank;
      }

    inline TorusXYZ& GetCoordFromRank( int aRank )
      {
      assert( aRank>=0 && aRank < mSize );
      return mElements[ aRank ].mCoord;
      }

    inline void GetMyCoords( int* x, int* y, int* z )
      {
      *x = mMyCoord[ 0 ];
      *y = mMyCoord[ 1 ];
      *z = mMyCoord[ 2 ];
      }

#if 0
    inline int GetRankFromCoord( int* aCoord ) const
      {
      int Coord[ 3 ];
      int curI = 0;
      for( int i=0; i<GDIM; i++ )
        {
        if( mPlaneMask[ i ] )
          Coord[ i ] = aCoord[ curI++ ];
        else
          Coord[ i ] = -1;
        }

      int x = Coord[ 0 ] * mPlaneMask[ 0 ];
      int y = Coord[ 1 ] * mPlaneMask[ 1 ];
      int z = Coord[ 2 ] * mPlaneMask[ 2 ];

      int index = (mDimSize[ 1 ]*x + y)*mDimSize[ 2 ] + z;

      if( !( index >=0 && index < mSize ) )
        {
        printf("index: %d {x,y,z}: { %d %d %d } mPlaneMask: { %d %d %d } aCoord: { %d %d %d } mDimSize: { %d %d %d }\n",
               index,
               x,y,z,
               mPlaneMask[ 0 ], mPlaneMask[ 1 ], mPlaneMask[ 2 ],
               aCoord[ 0 ], aCoord[ 1 ], aCoord[ 1 ],
               mDimSize[ 0 ],mDimSize[ 1 ], mDimSize[ 2 ] );
        }

      assert( index >=0 && index < mSize );
      return mElements[ index ].mRank;
      }
#endif

#if 1
    inline int GetRankFromCoord( int* aCoord ) const
      {
      int Coord[ 3 ];
      int curI = 0;
      for( int i=0; i<GDIM; i++ )
        {
        if( mPlaneMask[ i ] )
          Coord[ i ] = aCoord[ curI++ ];
        else
          Coord[ i ] = mMyCoord[ i ];
        }

      // Convert Coord to BGL Rank;
      int BGLRank = BGLPartitionMakeRank( Coord[ 0 ], Coord[ 1 ], Coord[ 2 ] );

      assert( BGLRank>=0 && BGLRank < mMachineSize );
      int Rank = mBGLRankToGroupRankMap[ BGLRank ];

      assert( Rank != -1 );
      assert( Rank == mElements[ Rank ].mRank );
      assert( mElements[ Rank ].mCoord[ 0 ] == Coord[ 0 ] &&
              mElements[ Rank ].mCoord[ 1 ] == Coord[ 1 ] &&
              mElements[ Rank ].mCoord[ 2 ] == Coord[ 2 ] );

      return Rank;
      }
#endif
};
  
    //********************** Topology ***********************************
    // Get the logical address space id - that is, an index into the route table.
    class PkTopology
      {
      public:

        static int mSubPartitionCount;

        static int mSubPartitionId;

        static int xSize_cmdprompt;
        static int ySize_cmdprompt;
        static int zSize_cmdprompt;

        static int mVoxDimx;
        static int mVoxDimy;
        static int mVoxDimz;

  static int mfftProcDimx;
        static int mfftProcDimy;
        static int mfftProcDimz;

        static int ThisAddressSpaceId;
        static int Size;

        static int mDimX;
        static int mDimY;
        static int mDimZ;

        static int mMachineDimX;
        static int mMachineDimY;
        static int mMachineDimZ;

        enum{ MAX_DIMENSIONALITY = 512 };  // Maximum logical dimensionality supported.

          // class Group {};  // Group of address space ids.  We support this in concept.

#ifdef PK_BLADE_SPI
        static GroupContainer_Actors mGroup;
#endif

        typedef int AddressSpaceId;

        // Stateless calls to virtualiziation layer.


        // CREATES THE TOPOLOGY FOR RUNNING THE BE-INSTANCES
#if defined( PK_MPI_ALL )
        static
        void
        InitCommunicator()
          {
          int xDim, yDim, zDim;
          GetMachineDimensions( &xDim, &yDim, &zDim );
          int MachineDims[ 3 ] = { xDim, yDim, zDim };
          int periods[ 3 ]   = { 0, 0, 0 };
          int reorder = 0;

          MPI_Comm WorldCartComm;

          MPI_Cart_create( MPI_COMM_WORLD, 3, MachineDims,
                           periods, reorder, &WorldCartComm );


          int nOrbs = (int) (log((double) mSubPartitionCount ) / log(2.) + 0.5);
          assert(nOrbs>=0);

          int xCuts = ( nOrbs / 3 );
          int yCuts = ( nOrbs / 3 );
          int zCuts = ( nOrbs / 3 );

          if (nOrbs % 3 == 1)
            {
            if( xDim > zDim )
              xCuts++;
            else
              zCuts++;
            }
          else if( nOrbs % 3 == 2 )
            {
            yCuts++;
            if( xDim > zDim )
              xCuts++;
            else
              zCuts++;
            }

          int xIncr = xDim / (double) pow( 2., xCuts );
          int yIncr = yDim / (double) pow( 2., yCuts );
          int zIncr = zDim / (double) pow( 2., zCuts );

          assert( xIncr >= 1 && yIncr >= 1 && zIncr >= 1 );

          // fill in the ranks within the rectangle of procs

          int MachineSize;
          MPI_Comm_size( MPI_COMM_WORLD, & MachineSize );

          int* rank2GroupId = new int[ MachineSize ];
          if( rank2GroupId == NULL )
            PLATFORM_ABORT("rank2GroupId == NULL");

          int** procRanks = new int*[ mSubPartitionCount ];
          if( procRanks == NULL )
            PLATFORM_ABORT("procRanks == NULL");

          for( int i = 0; i < mSubPartitionCount; i++ )
            {
            procRanks[ i ] = new int[ MachineSize ];
            if( procRanks[ i ] == NULL )
              PLATFORM_ABORT("procRanks[ i ] == NULL");
            }

          int* procSize = new int[ mSubPartitionCount ];
          if( procSize == NULL )
            PLATFORM_ABORT("procSize == NULL");

          int* subCartDims[ 3 ];
          for( int i = 0; i < 3; i++ )
            {
            subCartDims[ i ] = new int[ mSubPartitionCount ];

            if( subCartDims[ i ] == NULL )
              PLATFORM_ABORT("subCartDims[ i ] == NULL");
            }

          int groupID = -1;
          for(int i = 0; i < (int) pow( 2., xCuts ); i++)
            for(int j = 0; j < (int) pow( 2., yCuts ); j++)
              for(int k = 0; k < (int) pow( 2., zCuts ); k++)
                {
                groupID++;
                int counter=0;

                for(int ii = xIncr * i; ii < xIncr * (i+1); ii++)
                  for(int jj = yIncr * j; jj < yIncr * (j+1); jj++)
                    for(int kk = zIncr * k; kk < zIncr * (k+1); kk++)
                      {
                      int coords[ 3 ];
                      coords[ 0 ] = ii;
                      coords[ 1 ] = jj;
                      coords[ 2 ] = kk;

                      int rank = -1;
                      MPI_Cart_rank( WorldCartComm, &coords[ 0 ], &rank );

                      rank2GroupId[ rank ] = groupID;
                      procRanks[ groupID ][ counter ] = rank;
                      counter++;
                      }

                procSize[ groupID ] = counter;
                subCartDims[ 0 ][ groupID ] = xIncr;
                subCartDims[ 1 ][ groupID ] = yIncr;
                subCartDims[ 2 ][ groupID ] = zIncr;


                // printf( "groupID: %d\n", groupID );
                }

          int MyGlobalRank;
          MPI_Comm_rank( MPI_COMM_WORLD, &MyGlobalRank );

          assert( MyGlobalRank >= 0 && MyGlobalRank < MachineSize );
          int MyGroupId = rank2GroupId[ MyGlobalRank ];
          mSubPartitionId = MyGroupId;

          assert( MyGroupId >= 0 && MyGroupId < mSubPartitionCount );

          // Set partition dimension
          mDimX = subCartDims[ 0 ][ MyGroupId ];
          mDimY = subCartDims[ 1 ][ MyGroupId ];
          mDimZ = subCartDims[ 2 ][ MyGroupId ];

          MPI_Group NewGroup;
          MPI_Group WorldGroup;

          MPI_Comm_group( MPI_COMM_WORLD, &WorldGroup );

          MPI_Group_incl( WorldGroup,
                          procSize[ MyGroupId ],
                          procRanks[ MyGroupId ],
                          &NewGroup );

          MPI_Comm NewComm;
          MPI_Comm_create( MPI_COMM_WORLD, NewGroup, &NewComm );

          int PartitionDims[ 3 ] = { mDimX, mDimY, mDimZ };

          MPI_Cart_create( NewComm, 3, PartitionDims,
                           periods, reorder, &cart_comm );

          MPI_Comm_rank( cart_comm, &ThisAddressSpaceId );

          Size = mDimX * mDimY * mDimZ;

//           printf( "Size: %d ThisAddressSpaceId: %d dims { %d %d %d } cuts { %d %d %d } incr { %d %d %d }\n",
//                   Size, ThisAddressSpaceId,
//                   mDimX, mDimY, mDimZ,
//                   xCuts, yCuts, zCuts,
//                   xIncr, yIncr, zIncr );

          // Free memory
          delete [] rank2GroupId;

          for( int i = 0; i < mSubPartitionCount; i++ )
            {
            delete [] procRanks[ i ];
            }
          delete [] procRanks;

          delete [] procSize;

          for( int i = 0; i < 3; i++ )
            delete [] subCartDims[ i ];

          MPI_Barrier( MPI_COMM_WORLD );
          }
#endif

        static void Init( int* argc, char **argv[] )
          {
#if defined(PK_BLADE_SPI)
          int all_planes[ 3 ] = { 1, 1, 1 };
          int xDim, yDim, zDim;
          GetMachineDimensions( &xDim, &yDim, &zDim );

          Size = xDim * yDim * zDim;

          mGroup.Init( all_planes, xDim, yDim, zDim );

          ThisAddressSpaceId = mGroup.mMyRank;

          // Set partition dimensions
          mDimX = xDim;
          mDimY = yDim;
          mDimZ = zDim;

#elif defined( PK_MPI_ALL )
          // Figure out the dimensions of the machine
          // based on the partition size
          MPI_Init( argc, argv );

          InitCommunicator();
#endif
          BegLogLine(PKFXLOG_TOPOLOGY)
            << "Topology::Init Size=" << Size
            << " mDimX=" << mDimX
            << " mDimY=" << mDimY
            << " mDimZ=" << mDimZ
            << EndLogLine ;
          }

#if defined(PK_BLADE_SPI)
        static GroupContainer_Actors* GetGroup()
          {
          return &mGroup;
          }
#endif

        static inline AddressSpaceId
        GetAddressSpaceAtCoord( int   aDimensionality, // number of dimensions
                                int * aExtent,         // extent in each dimension
                                int * aCoordVector )   // coords
          {
          return(0);
          }

        static inline int
        GetCoordOfAddressSpace( AddressSpaceId   aAddressSpaceId,
                                int              aDimensionality,
                                int            * aExtent,
                                int            * aCoordVector     )
          {
          return(0);
          }

        // OR... ..... .... .... ... ..  .... .. .. ... ..... ...

        // Might be more efficient to have a setup phase:

        // Structure to hold the dimensionality and the extent in each each dimension.
        // A context is virtual mapping of a perfect grid machine.
        class Context
          {
          public:
            int mDimensionality;
            int mExtent[ MAX_DIMENSIONALITY ];
            //Machine dependant member vars (maybe).
          };

        // Setup the context
        static inline int
        SetupContext( int      aDimensionality, // Number of dimensions
                      int     *aExtent,   // Extent in each dimension - must fit in partition size.
                      Context &aContext ) // reference to an area for returned context
          {
          aContext.mDimensionality = aDimensionality;
          for(int i = 0; i < aDimensionality; i++ )
            aContext.mExtent[ i ] = aExtent[i];
          // Machine dependant code to figure out mapping
          return(0);
          }

        // Given a context and a coord vector, return an AddressSpaceId
        static inline AddressSpaceId
        GetAddressSpaceAtCoord( Context        &aContext,
                                int            *aCoordVector,
                                AddressSpaceId &aAddressSpaceId )
          {
          return(0);
          }

        // Given a context, and an address space id, return the coords.
        static inline int
        GetCoordOfAddressSpace( Context         &aContext,
                               AddressSpaceId  aAddressSpaceId,
                               int            *aCoordVector   ) // return
          {
          return(0);
          }

        // This will be -1 until it is set - it is questionable how useful this will be, but there
        // is the god awful chance that static constructors will call this GetAddressSpaceId before
        // the environment has set it up.  Maybe we should assert in this case???
        #if  defined( PK_MPI_ALL )
        static MPI_Comm cart_comm;
        #endif

        static inline
        int
        GetAddressSpaceId()
          {
          return( ThisAddressSpaceId );
          }

        // Find where we are within a chip. Intended for BG/L, where we want
        // to know whether to come in at 'main' or be a helper
        static inline
        int
        GetLocationInASIC()
          {
#if PK_BLADE_SPI
          return rts_get_processor_id();
#else
          return 0 ;
#endif
          }

        static inline
        int
        GetSize()
          {
          return( Size );
          }

        static inline void xyzFromProcCount(int procs, int &x, int &y, int &z)
        {
          // Find full space power of 2

          // As long as x >= y >= z the orb will create the correct partitioning.
          // y >= 1/2 x && z >= 1/2 y

          int d = 3;
          int levels = 0;
          int t;
          do
            {
              levels++;
              t = 1 << (levels*d);
            }while (procs > t);
          int base = (levels-1)*d;
          while(d > 0)
            {
              t = 1 << (base + d);
              if( procs == t ) break;
              d--;
            }

          assert(d>0);

//           switch(d)
//             {
//             case 3:
//               x = 1 << (base + 1);
//               y = 1 << (base + 1);
//               z = 1 << (base + 1);
//               break;
//             case 2:
//               x = 1 << (base + 1);
//               y = 1 << (base + 1);
//               z = 1 << base;
//               break;
//             case 1:
//               x = 1 << (base + 1);
//               y = 1 << base;
//               z = 1 << base;
//               break;
//             }
          switch( d )
            {
            case 3:
              x = 1 << levels;
              y = 1 << levels;
              z = 1 << levels;
              break;
            case 2:
              x = 1 << levels;
              y = 1 << levels;
              z = 1 << levels-1;
              break;
            case 1:
              x = 1 << levels;
              y = 1 << levels-1;
              z = 1 << levels-1;
              break;
            }
        }

        static
        inline
        int
        MakeRankFromCoords( int x, int y, int z )
        {
         #ifndef NDEBUG
           int sizeX, sizeY, sizeZ;
           GetDimensions( &sizeX, &sizeY, &sizeZ );
           assert( x >= 0 && x < sizeX );
           assert( y >= 0 && y < sizeY );
           assert( z >= 0 && z < sizeZ );
         #endif

#if defined( PK_BLADE_SPI )
           // return BGLPartitionMakeRank( x, y, z );
          int coords[3];
          coords[ 0 ] = x;
          coords[ 1 ] = y;
          coords[ 2 ] = z;

          return mGroup.GetRankFromCoord( coords );
#elif defined( PK_MPI_ALL )
          int coords[3];
          coords[ 0 ] = x;
          coords[ 1 ] = y;
          coords[ 2 ] = z;
          int rank;

          MPI_Cart_rank(cart_comm, coords, &rank);
          return rank;
#else
         return x;
#endif
        }

        static
        inline
        void
        GetCoordsFromRank( int rank, int* x, int* y, int* z )
        {
#if defined( PK_BLADE_SPI )
          // BGLPartitionMakeXYZ( rank, x, y, z );
            TorusXYZ Coords = mGroup.GetCoordFromRank( rank );
            *x = Coords.mX;
            *y = Coords.mY;
            *z = Coords.mZ;
#elif defined( PK_MPI_ALL )
          int coords[ 3 ], myRank;
          MPI_Cart_coords( cart_comm, rank, 3, coords);
          *x = coords[ 0 ];
          *y = coords[ 1 ];
          *z = coords[ 2 ];
#else
          *x=1;
          *y=1;
          *z=1;
#endif
        }

        // This call is used by the application configurator
        // to attempt to best map the application topology
        // to the machine topology.
        static
        inline
        void
        GetMyCoords( int* procsX, int* procsY, int* procsZ )
        {
#if defined( PK_BLADE_SPI )
        mGroup.GetMyCoords( procsX, procsY, procsZ );
        // BGLPartitionGetCoords( procsX, procsY, procsZ );
        // BGLPartitionMakeZYX( BGLPartitionGetRank(), procsX, procsY, procsZ );
#elif defined( PK_MPI_ALL )
          int coords[ 3 ], myRank;
          MPI_Cart_coords( cart_comm, ThisAddressSpaceId, 3, coords);
          *procsX = coords[ 0 ];
          *procsY = coords[ 1 ];
          *procsZ = coords[ 2 ];
#elif defined( PK_BGL )
          return GetCoordsFromRank( ThisAddressSpaceId, procsX, procsY, procsZ );
#else
          *procsX = 0;
          *procsY = 0;
          *procsZ = 0;
#endif
        }

        static
        inline
        void
        GetMachineDimensions( int* procsX, int* procsY, int* procsZ )
          {
#if defined( PK_BLADE_SPI )
        BGLPartitionGetDimensions( procsX, procsY, procsZ );
#elif defined( PK_MPI_ALL )

#ifndef PK_BGL
        int s;
        MPI_Comm_size(MPI_COMM_WORLD, &s );
        // MPI_Comm_size( cart_comm, &s );

            if( xSize_cmdprompt > 0 )  // then use it
              {
                *procsX = xSize_cmdprompt;
                *procsY = ySize_cmdprompt;
                *procsZ = zSize_cmdprompt;
              }
            else
              {
              xyzFromProcCount( s, *procsX, *procsY, *procsZ );
              }
#else
            if( xSize_cmdprompt > 0 )  // then use it
              {
                *procsX = xSize_cmdprompt;
                *procsY = ySize_cmdprompt;
                *procsZ = zSize_cmdprompt;
              }
            else
              {
              BGLPersonality personality;
              rts_get_personality( &personality, sizeof(personality) );
              *procsX = personality.xSize;
              *procsY = personality.ySize;
              *procsZ = personality.zSize;
              }
#endif
#else
          *procsX = 1;
          *procsY = 1;
          *procsZ = 1;
#endif
          }

        static
        inline
        void
        GetDimensions( int* procsX, int* procsY, int* procsZ )
          {
          *procsX = mDimX;
          *procsY = mDimY;
          *procsZ = mDimZ;
          }

        // This is mostly a statement about the distance between nodes.
        static inline
        int
        GetPhysicalDescription( int &AddressSpaceCount,
                                int &AddressSpaceConnectivity,
                                int &PhysicalDimensionality )
          {
#ifdef PK_BLADE_SPI
          int xNodes, yNodes, zNodes;
#ifdef PK_PARALLEL
          BGLPartitionGetDimensions( &xNodes, &yNodes, &zNodes );
#else
          // If we're not running parallel, we are '1x1x1'
          xNodes = 1 ;
          yNodes = 1 ;
          zNodes = 1 ;
#endif
          // MPI_Comm_size(MPI_COMM_WORLD, &s );
          AddressSpaceCount = xNodes * yNodes * zNodes;
          AddressSpaceConnectivity = AddressSpaceCount - 1;
          PhysicalDimensionality = 0;
#else
#ifdef PK_MPI_ALL
          int s;
          MPI_Comm_size(cart_comm, &s );
          AddressSpaceCount = s;
          AddressSpaceConnectivity = s - 1;
          PhysicalDimensionality = 0;
#else
          // Answers for a single address space partition.
          AddressSpaceCount = 1;
          AddressSpaceConnectivity = 0;
          PhysicalDimensionality = 0;
#endif
#endif
          return(0);
          }

        // Cheep little call (only use when dimensionality > 0 )
        // used to get the extent of grid space in each dimension.
        static inline int
        GetPhysicalExtentInDimension( int DimensionNumber )
          {
          //SP pops an assert on this.
          return(0);
          }

        static inline
        int
        GetAddressSpaceCount()
          {
#if 0
          int AddressSpaceCount;
          int AddressSpaceConnectivity;
          int PhysicalDimensionality;
          GetPhysicalDescription( AddressSpaceCount,
                                  AddressSpaceConnectivity,
                                  PhysicalDimensionality );
          return( AddressSpaceCount );
#endif
          return ( Size );
          }

      };

#endif

