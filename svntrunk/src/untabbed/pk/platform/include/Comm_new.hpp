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
 
/*#####################################################################
#                        I.B.M. CONFIDENTIAL                        #
#      (c) COPYRIGHT IBM CORP. 2001, 2003 ALL RIGHTS RESERVED.      #
#                                                                   #
#                LICENSED MATERIALS-PROPERTY OF IBM.                #
#                                                                   #
#####################################################################
*/
//  
//  IBM T.J.W  R.C.
//  Date : 06/25/2005
//  Name : Maria Eleftheriou
//  This code creates the appropriate topologies for the machine and each replica
//


#ifndef __TOPOLOGY_COMM
#define __TOPOLOGY_COMM



#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "math.h"

// 

//Create Cart topologies 
class CreateTopology
{
private:
  enum{DIM=3};
public:
  MPI_Comm _comm;       // input communicator 
  MPI_Comm _cart_comm;  // communicator with a cartesian structure 
  int _ndims;           // dims of the cartesian grid
  int* _dims;           // ndims array, specifies the number of procs in each dim
  int* _periods;        // ndims array, true if the grid is periodic in each dim 
  int _reoder;          // ranking is not reordered
  double *buffer;	
  
  CreateTopology()
    {
      _reoder=0;
      _ndims=0;
      _periods = new int[DIM];
      _periods[0]=0;
      _periods[1]=0;
      _periods[2]=0;
    }
  
  CreateTopology( MPI_Comm comm,  
      const int& ndims,
      int* dims,
      int* periods, 
      int reoder=0):
    _comm(comm),  
    _ndims(ndims),
    _dims(dims),  
    _periods(periods),
    _reoder(reoder)
    { 
      MPI_Cart_create(_comm, _ndims, _dims, _periods, _reoder, &(_cart_comm));
    }

  CreateTopology(CreateTopology const& ct):
    _comm(ct._comm),  
    _ndims(ct._ndims),
    _dims(ct._dims),  
    _periods(ct._periods),
    _reoder(ct._reoder)
    {
      for(int i=0; i<DIM; i++)
  {
    _dims[i]=ct._dims[i];
    _periods[i]=ct._periods[i];  
  }
    }
  
  ~CreateTopology()
    {
      delete _dims;
      delete _periods;
      _dims = NULL;
      _periods = NULL;
    }
  
  CreateTopology& operator=(const CreateTopology& ct)
    {
      _comm = ct._comm;  
      _ndims = ct._ndims;
      _dims = ct._dims; 
      _periods =ct._periods;
      _reoder =ct._reoder;
      
      for(int i=0; i<DIM; i++)
  {
    _dims[i]=ct._dims[i];
    _periods[i]=ct._periods[i];
  } 
      return *this;
    }



  
  void Print() const
    {
      printf(" Communitor info: {ndims %d, dims = {%d %d %d}, periods = {%d %d %d}, reoder %d} \n",
       _ndims, _dims[0], _dims[1], _dims[2],  _periods[0], _periods[1], _periods[2], _reoder);
    }
  
  inline  MPI_Comm getCartComm(){return (_cart_comm);}  
  
  inline void getCartCommCoords( int rank, int* coords) const
    {
      MPI_Cart_coords(_cart_comm, rank, _ndims, coords);
    }
  
  inline void getCartCommCoords( int* coords) const
    {
      int rank=0;
      MPI_Comm_rank(_cart_comm, &rank);
      MPI_Cart_coords(_cart_comm, rank, _ndims, coords);
    }
  
  inline void getCartCommRank( int* coords, int* rank) const
    {
      MPI_Cart_rank(_cart_comm,  coords, rank);
    }
  
  //const int& getOwningProc()const;
  inline void Barrier(){MPI_Barrier(_cart_comm);}
  
  inline void Allreduce(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) 
    {
      MPI_Allreduce(sendbuf, recvbuf, count, MPI_DOUBLE, MPI_SUM, _cart_comm); 
    }
};

// creates communicator-group
class CreateCommGroups
{
private:
  MPI_Comm   _new_comm;
  MPI_Group _orig_group_comm,  _group_comm;
  MPI_Comm   _comm;
  
  int _nGroupOfProcs;
  int *_groupSize;//group's procs
  int **_procRanks;  // [groupID][groupSize] for every subgroup we get the ids of the global comm
  
public:
  CreateCommGroups(MPI_Comm comm, int nGroupOfProcs,
       int** procRanks, int* groupSize):
    _comm(comm),
    _nGroupOfProcs(nGroupOfProcs),

    _groupSize( new int[_nGroupOfProcs]),
    _procRanks( new  int*[_nGroupOfProcs])
    {
      _group_comm;// = new MPI_Group[_nGroupOfProcs];
      //assert(_group_comm);
      // Get group handle 
      MPI_Comm_group(_comm, &(_orig_group_comm));
      int size=-1;
      MPI_Comm_size(_comm, &size);
      assert(size >= _nGroupOfProcs);
      
      for(int i=0; i<_nGroupOfProcs; i++)
  {
    _groupSize[i] = groupSize[i];
    _procRanks[i] = new int[_groupSize[i]];
  }
      
      for(int i=0; i<_nGroupOfProcs; i++)
  for(int j=0; j<_groupSize[i]; j++) 
    {
      _procRanks[i][j]=procRanks[i][j]; 
    }	
    }
  
  ~CreateCommGroups()
    {      
      for(int i=0; i<_nGroupOfProcs; i++)
  delete _procRanks[i];
      delete _procRanks;
      delete _groupSize;
    }
  
  inline void createSubGroups(const int&i)
    {
      int rank=-1, size;
      MPI_Comm_rank(MPI_COMM_WORLD,&rank);
      MPI_Comm_size(MPI_COMM_WORLD,&size);
      
      MPI_Group_incl(_orig_group_comm, _groupSize[i], _procRanks[i], &(_group_comm));
      MPI_Comm_create(_comm,_group_comm, &_new_comm);
  
      int myID=-1;
      MPI_Group_rank(_group_comm, &myID);
      int myCommID;
      MPI_Group_rank(_new_comm, &myCommID);
      
      if(myID !=MPI_UNDEFINED)
  MPI_Barrier((_new_comm));
    }
  
  inline void createMySubGroup(const int& myGroup)
    {
      // MPI_Group_incl(_orig_group_comm, _groupSize[myGroup], _procRanks[myGroup], &(_group_comm[myGroup]));
      MPI_Group_incl(_orig_group_comm, _groupSize[myGroup], _procRanks[myGroup], &(_group_comm));
    }
  
  inline void getGroupSize(const int& thisGroup, int& groupSize)
    {
      groupSize = _groupSize[thisGroup]; 
    }
  
  inline MPI_Group &getGroupComm( const int& groupID )
    { 
      // return (_group_comm[groupID]);
      return (_new_comm);
    }
  
  inline void getGroupProcs(const int& thisGroup, int* procsRanks)
    {
      for(int i=0; i<_groupSize[thisGroup]; i++)
  procsRanks[i]=_procRanks[thisGroup][i];       
    }

};

// creates the cart comm world and all the subcommunicators for the REMD
class CommLayer
{
protected:
  enum{DIM=3};
  
private:
  int _nGroupOfProcs;
  CreateTopology* _commWorldCart;  // create a cartesian comm for the whole machine
  CreateCommGroups* _commGroup;     // partition the machine to groups
  CreateTopology* _commSubCart;    // create catesian sub comm from the groups
  
  int** _procRanks;
  int*  _procSize;
  int   _physicalDims[DIM];
  int*  _subCartDims[DIM];
  int* _rank2GroupID;
public:
  
  //CommLayer(){}

  CommLayer(MPI_Comm comm, const int* physicalDims, const int& nGroupOfProcs):
    _nGroupOfProcs(nGroupOfProcs)
    { 
      for(int i=0; i<DIM; i++)
  _physicalDims[i]=physicalDims[i];
      
      _commWorldCart=NULL;
      _commSubCart=NULL;
      _commGroup=NULL;
      _procRanks = new int*[nGroupOfProcs];
      int size=-1;
      
      MPI_Comm_size(MPI_COMM_WORLD, &size);
      _rank2GroupID = new int[size];
      
      for(int i=0; i<_nGroupOfProcs; i++)
  _procRanks[i] = new int[size*nGroupOfProcs];
      
      for(int i=0; i<DIM; i++)
  {
    _subCartDims[i]=new int[_nGroupOfProcs];
  }
      
      _procSize = new int[_nGroupOfProcs];
    }  
  
  ~CommLayer()
    {
      for(int i=0; i<_nGroupOfProcs; i++)
  delete _procRanks[i];
      
      delete _procRanks;
      
      for(int i=0; i<DIM; i++)
  {
    delete _subCartDims[i];
  }
    }
  
  void Init()
    {
      // create a catesian comm_world comm
      int cart_ndim = DIM;
      int period[DIM] = {0,0,0};
      
      _commWorldCart = new CreateTopology(MPI_COMM_WORLD, cart_ndim, _physicalDims, period);
      
      int subCart_dims[DIM] = {0,0,0};
      int subCart_ndim = DIM;
     
      _commSubCart = new CreateTopology[_nGroupOfProcs];
      
      createAllSubMeshes();
      
      int cartRank=-1;
      MPI_Comm_rank((*_commWorldCart).getCartComm(), &cartRank);
      assert(cartRank!=MPI_UNDEFINED);
      _commGroup = new CreateCommGroups((*_commWorldCart).getCartComm(),_nGroupOfProcs, _procRanks, _procSize);

      MPI_Barrier(MPI_COMM_WORLD);
      int rank=-1,size;
      
      MPI_Comm_rank(MPI_COMM_WORLD,&rank);
      MPI_Comm_size(MPI_COMM_WORLD,&size);
      
      assert(rank==cartRank);
      int myGroupID=_rank2GroupID[rank];
      (*_commGroup).createSubGroups(myGroupID);
      
      int i=myGroupID;
      
      assert(i<=size);
      assert((*_commGroup).getGroupComm(i));
      MPI_Barrier((*_commGroup).getGroupComm(i));

      //  printf("before sub top create : procSize[%d]=%d, subCartDims = {%d %d %d} \n",
      //     i, _procSize[i], _subCartDims[0][i],_subCartDims[1][i],_subCartDims[2][i]);

      // MPI_Barrier(MPI_COMM_WORLD);
      
      int *subCartDims= new int[DIM];

      for (int index=0; index<DIM; index++) 
  subCartDims[index]=_subCartDims[index][i];
      
      
      _commSubCart = new CreateTopology(((*_commGroup).getGroupComm(i)), DIM ,subCartDims,  period);
       assert(_commSubCart!=NULL);
    }
  
  void Finalize()
    {
      delete  _commWorldCart;
      delete _commGroup;
      delete _commSubCart;
  
    }
  
  
  CreateTopology* GetCommWorldCart(){return _commWorldCart;}  // return a cartesian comm for the whole machine
  CreateTopology* GetCommSubCart(){ return _commSubCart;}    // return sub catesian comm

  // CREATES THE TOPOLOGY FOR RUNNING THE BE-INSTANCES 
  void createAllSubMeshes()
    {        
      int nOrbs = (int) (log((double)_nGroupOfProcs)/log(2.));
      assert(nOrbs>=0);
      int xCuts = (nOrbs/3); int yCuts = (nOrbs/3); int zCuts = (nOrbs/3);
      
      if(nOrbs%3 == 1){if(_physicalDims[0]>_physicalDims[2]){xCuts++;} else {zCuts++;}}
      else if(nOrbs%3 == 2){yCuts++; if(_physicalDims[0]>_physicalDims[2]){xCuts++; } else {zCuts++;}}
      
      int xIncr = _physicalDims[0] / (double)pow(2., xCuts); 
      int yIncr = _physicalDims[1] / (double)pow(2., yCuts); 
      int zIncr = _physicalDims[2] / (double)pow(2., zCuts);
  
      assert(xIncr>=1 && yIncr>=1 && zIncr>=1);      
      int myRank;
      
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
      
      int groupID=-1;
      // fill in the ranks within the rectangle of procs
      
      for(int i=0; i<(int)pow(2.,xCuts); i++)
  for(int j=0; j<(int)pow(2.,yCuts); j++)
    for(int k=0; k<(int)pow(2.,zCuts); k++)
      {
#ifdef DEBUG_CREATEALLSUBMESHES
        printf("{i,j,k}={%d,%d,%d}\n", i,j,k);
#endif
        groupID++;
        int counter=0;
        
        for(int ii=xIncr*i; ii<xIncr*(i+1); ii++)
    for(int jj=yIncr*j; jj<yIncr*(j+1); jj++)
      for(int kk=zIncr*k; kk<zIncr*(k+1); kk++)
        {  
          int coords[DIM];
          coords[0]=ii; coords[1]=jj; coords[2]=kk;
          
#ifdef DEBUG_CREATEALLSUBMESHES
          printf("{ii,jj,kk}={%d,%d,%d}\n", ii,jj,kk);
          printf("coords[%d] = {%d, %d, %d} size = %d\n",groupID,
           coords[0],coords[1],coords[2],counter);
#endif    
          int rank = -1;
          (*_commWorldCart).getCartCommRank(&coords[0], &rank);
          
          _rank2GroupID[rank]=groupID;
          _procRanks[groupID][counter]=rank; 	  
          counter++;		    
        }
        
        _procSize[groupID]=counter;	      
        _subCartDims[0][groupID]=xIncr;
        _subCartDims[1][groupID]=yIncr;
        _subCartDims[2][groupID]=zIncr;
      }
      
      MPI_Barrier(MPI_COMM_WORLD);
    }
  
  const int* getSubMeshID(const int groupID, const int* procGlobalCoord) const
    {
      return _procRanks[groupID];
    }
};

#endif
