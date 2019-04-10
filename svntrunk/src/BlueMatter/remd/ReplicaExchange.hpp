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
 //remd.hpp
// This file contains 3 classes:
// Distributed_Data   ---input data in the 
// REMD         -- abstact class
// Standard     -- implemenation  class ( Stardard Replica Exchange Method)
// Stochastic   -- implementaion class ( Stochastic Replica Exchange Method)
//  Maria Eleftheriou
//  06-25-2005
//  Jed W Pitera (minor edits)
//  08-22-2005

#ifndef __REMD_HPP
#define __REMD_HPP
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
//#define  DEBUG_REMD 1

#if STANDALONE
class SciConst{
public:
  static double KBoltzmann_IU;
};

double SciConst::KBoltzmann_IU = 1;
#endif

class REMD {
public:
  virtual ~REMD(){}
  virtual void remdSwapping(double* potentialEnergy)=0;
  virtual void getReplicaID(unsigned& myReplicaID)=0;
};

// Standard implementation follows the sequentical approach descripted in the following paper.
//R. Zhou, Protein Folding Free Energy Landscape with Parallel Replica Exchange Molecular Dynamics,, Ed. Albert Zomaya, "Parallel Computing for Bioinformatics and Computational Biology", John Wiley & Sons Publisher, New York, 2005

class REMD_Standard: public REMD
{
private:
  // spec:if numReplica is not defined then do a 
  enum{NUM_REPLICA=1};
  
  double *_temperature;
  unsigned *_temperature2Replica;
  unsigned *_replica2Temperature;
  unsigned *_currentTemperature;
  double *_inverseKTemperature;
  unsigned  _numReplica;
  unsigned  _swapPeriodOTS;
  unsigned  _myReplicaID;
  unsigned  _swapReplicaStart;
  unsigned* _numAccept;
  unsigned* _numAttempt;
  int  _myGlobalRank;
 
public:
  void getReplicaID(unsigned &myReplicaID){ myReplicaID = _myReplicaID;}
  // for debugging
  void print()
    {
      printf("numReplica = %d\n", _numReplica);
      
      for(int i=0; i<_numReplica; i++)
	{
	  printf(" temperature[%d] = %lf  \n, _inverseKTemperature[%d] = %lf \n  currentTemperature[%d]=%d \n temperature2Replica[%d] = %d \n replica2Temperature[%d]=%d \n ",
		 i, _temperature[i], i, _inverseKTemperature[i],i, _currentTemperature[i],
		 i, _temperature2Replica[i],i, _replica2Temperature[i]);
	}
    }
  
  const double& getInverseKTemperature(const unsigned& integerTemp)
    {
      return _inverseKTemperature[integerTemp];
    }
  
public:
  REMD_Standard(int myGlobalRank,
		double *temperature,
		unsigned* temperature2Replica, 
		unsigned& numReplica,
		unsigned& swapPeriodOTS,
		unsigned& myReplicaID,  
		unsigned& seed,
	        unsigned& dummy):
    _myGlobalRank(myGlobalRank),
    _numReplica(numReplica),
    _swapPeriodOTS(swapPeriodOTS),
    _myReplicaID(myReplicaID),
    _temperature(new double[numReplica<1?NUM_REPLICA:numReplica]),
    _inverseKTemperature(new double[numReplica<1?NUM_REPLICA:numReplica]),
    _temperature2Replica(new unsigned[numReplica<1?NUM_REPLICA:numReplica]),
    _replica2Temperature(new unsigned[numReplica<1?NUM_REPLICA:numReplica]),
    _currentTemperature(new unsigned[numReplica<1?NUM_REPLICA:numReplica])
    {
      assert(_temperature); 
      assert(_temperature2Replica); 
      assert(_replica2Temperature);
          
      _numAccept     = new unsigned[numReplica];
      _numAttempt    = new unsigned[numReplica];
      
      for(int i=0; i<numReplica; i++)
	{
	  _numAttempt[i] = 0;
	  _numAccept[i] = 0;
	}

      for(int i=0; i<numReplica; i++)
	{
	  _temperature[i]=temperature[i];
	  _currentTemperature[i]=i;
	  _inverseKTemperature[i]= 1./(_temperature[i]*SciConst::KBoltzmann_IU);
	  _temperature2Replica[i]=temperature2Replica[i];
	  _replica2Temperature[temperature2Replica[i]]=i;	  
	}
 
      _swapReplicaStart = 0;

    }
  
  ~REMD_Standard()
    {
      delete[] _numAccept;
      delete[] _numAttempt;

      delete[] _temperature;
      delete[] _inverseKTemperature;
      delete[] _temperature2Replica;
      delete[] _replica2Temperature;
      delete[] _currentTemperature;
    }
  
  void remdSwapping(double *potentialEnergy)
    {
      // print for debugging

#ifdef DEBUG_REMD
      if (_myGlobalRank==0) printf("swapReplicaStart: %d\n", _swapReplicaStart);
#endif
      // loop over the temperature.
     
    
      for( int i =(_numReplica-1-_swapReplicaStart); i>0; i=i-2)
	{
	  unsigned j=i-1;
	  // find the replicas that are at temperature i and i-1
	  
	  unsigned jReplica=_temperature2Replica[j]; 
	  unsigned iReplica=_temperature2Replica[i];
	  
	  assert( jReplica >=0 && jReplica < _numReplica );
	  assert( iReplica >=0 && iReplica < _numReplica );

	  double Ej=potentialEnergy[jReplica];
	  double Ei=potentialEnergy[iReplica];
	  
	  //remapping temperatures from the indecies to actual double values
	  double inverseKTempj = getInverseKTemperature(j);
	  double inverseKTempi = getInverseKTemperature(i);
     
          // print for debugging 
#ifdef DEBUG_REMD
          if (_myGlobalRank == 0) 
	    {
	      printf("TRIAL [%d]=%d:\n replicaIDi = %d 1/KT=%f PE[%d] = %f \n replicaIDj %d 1/KT=%f PE[%d]=%f\n",
		     j,_numAttempt[j],
		     i,1.0/inverseKTempi,i,Ei,
		     j,1.0/inverseKTempj,j,Ej);
	    }
#endif
	  // if (Ei<Ej  the always accept
	  // attempt a swap b/w the i and i-1 pair of temperatures	  
	  
	  unsigned accept=0;	  
	  
	  if(Ei<Ej) 
	    { 
	      accept=1;
	    }
	  else
	    {
	      double prob = (Ei-Ej)*(inverseKTempj-inverseKTempi);
	      prob = exp(-prob);
              double trial = drand48();
	      if(trial < prob) accept=1;

              // debugging
#ifdef DEBUG_REMD
              if (_myGlobalRank == 0)
		{
		  printf("prob: %f trial: %d ", prob,trial);
		}
#endif
	    }
	  
	  _numAccept[j] += accept;
	  // _numAccept[jReplica] += accept;
	  _numAttempt[j]++;
#ifdef DEBUG_REMD
	  if (_myGlobalRank == 0)
	    printf("CommWorldID:%d numAccept[%d]=%d numAttempt[%d]=%d\n",
		   _myGlobalRank, j, _numAccept[j],j,_numAttempt[j]);
#endif
		   // if accept update
	  if(accept==1)
	  {
	    _temperature2Replica[i]=jReplica;
	    _temperature2Replica[j]=iReplica;
	    _currentTemperature[jReplica]=i;
	    _currentTemperature[iReplica]=j;
#ifdef DEBUG_REMD
	      printf("\n ReplicaExchange.hpp:remdSwapping  CommWorldID:%d  temp2replica[%d]=%d temp2replica[%d]=%d : currentTemp[%d]=%d currentTemp[%d]",
		     _myGlobalRank, i, _temperature2Replica[i],
		     j,_temperature2Replica[j],
		     jReplica,_currentTemperature[jReplica],
		     iReplica,_currentTemperature[iReplica]);
#endif
	    }

  
	  _numAccept[j] += accept;
	  _numAttempt[j]++;

#ifdef DEBUG_REMD
          if (_myGlobalRank == 0) 
	    {
	      printf("TRIAL delta: %f accept: %d\n",
		     Ej-Ei,accept);
	    }
#endif

	}
        if (_swapReplicaStart == 0) {
            _swapReplicaStart = 1;
        } else {
            _swapReplicaStart = 0;
        }
    }

  // need those two functions for restart 
  void setNumAccept(const unsigned numAccept, const unsigned tempID)
    {
      assert( tempID >=0 && tempID < _numReplica );      
      _numAccept[tempID] = numAccept; 
    }

  void setNumAttempt(const unsigned numAttempt, const unsigned tempID)
    {
      assert( tempID >=0 && tempID < _numReplica );      
      _numAttempt[tempID] = numAttempt; 
    }
 
  unsigned& getSwapAccept( const unsigned tempID) const
    {
      assert( tempID >=0 && tempID < _numReplica );      
      return _numAccept[tempID];
    }

  unsigned& getSwapAttempt( const unsigned tempID) const
    {
      assert( tempID >=0 && tempID < _numReplica );      
      return _numAttempt[tempID];
    }

  void getAcceptRatio(double &acceptRatio, const unsigned replicaID)
    {
      assert( replicaID>=0 && replicaID < _numReplica);
      if (_numAttempt[replicaID] > 0) {
      acceptRatio = (double) _numAccept[replicaID]/(double) _numAttempt[replicaID];
      } else {
      acceptRatio = 0.0;
      }
    }
  
  void getTemperature2Replica(const unsigned tempID, unsigned& replicaID)
    {
      assert( replicaID>=0 && replicaID < _numReplica);
      replicaID = _temperature2Replica[tempID];
      assert( tempID >=0 && tempID < _numReplica );
    }

  
  double getTemperature( const unsigned replicaID )
    {
      //unsigned tempID = 0;
      //getReplica2Temperature( replicaID, tempID );
      
       int tempID = _currentTemperature[replicaID];// CHECK AGAIN 
       assert( tempID >=0 && tempID < _numReplica );	
        return _temperature [ tempID ];
   }

  void getReplica2Temperature(const unsigned replicaID, unsigned& tempID)
    {
      static int count=0;
      assert( replicaID>=0 && replicaID < _numReplica);
      tempID = _currentTemperature[replicaID];
      assert( tempID >=0 && tempID < _numReplica );

#ifdef DEBUG_REMD  
    if(_myGlobalRank==0)
	printf("REMD: replica2Temperature[%d]=%d \n", replicaID, _replica2Temperature[replicaID]);
#endif
    } 

};


//unsigned REMD_Standard::_numAccept = 0;
//unsigned REMD_Standard::_numAttempt = 0;

class REMD_Stochastic: public REMD
{
private:
  enum{NUM_REPLICA=1};
  
  double *_temperature;
  unsigned *_temperature2Replica;
  unsigned *_replica2Temperature;
  unsigned *_currentTemperature;
  double *_inverseKTemperature;
  unsigned  _numReplica;
  unsigned  _swapPeriodOTS;
  unsigned  _myReplicaID;
  unsigned  _numTrials;
  unsigned* _numAccept;
  unsigned* _numAttempt;
  int  _myGlobalRank;
   
public:
  void getReplicaID(unsigned &myReplicaID){ myReplicaID = _myReplicaID;}
  // for debugging
  void print()
    {
      printf("numReplica = %d\n", _numReplica);
      
      for(int i=0; i<_numReplica; i++)
	{
	  printf(" temperature[%d] = %lf  \n, _inverseKTemperature[%d] = %lf \n  currentTemperature[%d]=%d \n temperature2Replica[%d] = %d \n replica2Temperature[%d]=%d \n ",
		 i, _temperature[i], i, _inverseKTemperature[i],i, _currentTemperature[i],
		 i, _temperature2Replica[i],i, _replica2Temperature[i]);
	}
    }
  
  const double& getInverseKTemperature(const unsigned& integerTemp)
    {
      return _inverseKTemperature[integerTemp];
    }
  
public:
  REMD_Stochastic(int myGlobalRank,
		  double *temperature,
		  unsigned* temperature2Replica,
		  unsigned& numReplica, 
		  unsigned& swapPeriodOTS,
		  unsigned& myReplicaID,  
		  unsigned& seed,
		  unsigned& numTrials):
    _myGlobalRank(myGlobalRank),
    _numReplica(numReplica),
    _swapPeriodOTS(swapPeriodOTS),
    _myReplicaID(myReplicaID),
    _temperature(new double[numReplica<1?NUM_REPLICA:numReplica]),
    _inverseKTemperature(new double[numReplica<1?NUM_REPLICA:numReplica]),
    _temperature2Replica(new unsigned[numReplica<1?NUM_REPLICA:numReplica]),
    _replica2Temperature(new unsigned[numReplica<1?NUM_REPLICA:numReplica]),
    _currentTemperature(new unsigned[numReplica<1?NUM_REPLICA:numReplica]),
    _numTrials(numTrials)
    {
      assert(_temperature); 
      assert(_temperature2Replica); 
      assert(_replica2Temperature);
      assert(_currentTemperature);
      assert(_inverseKTemperature);
    

      _numAccept = new unsigned[numReplica];
      _numAttempt = new unsigned[numReplica];
    
      
      assert(_numAccept);
      assert(_numAttempt);


      for(int i=0; i<numReplica; i++)
	{
	  _numAttempt[i] = 0;
	  _numAccept[i] = 0;	  
	}

      // spec: if numTrials not specified, it is set to the num of replicas
      if(_numTrials==0)_numTrials=_numReplica;

      for(int i=0; i<_numReplica; i++)
	{
	  _temperature[i]=temperature[i];
	  _currentTemperature[i]=i;
	  _inverseKTemperature[i]= 1./(_temperature[i]*SciConst::KBoltzmann_IU);
	  _temperature2Replica[i]=temperature2Replica[i];
	  _replica2Temperature[temperature2Replica[i]]=i;	
	  //_numAccept=0;
	  //_numAttempt=0;
	  //_temperature2Replica[i]=i; // pass that...
	  //  _replica2Temperature[i]=i;	  	
	}
    }
  
  ~REMD_Stochastic()
    {
      delete[] _temperature;
      delete[] _inverseKTemperature;
      delete[] _temperature2Replica;
      delete[] _replica2Temperature;
      delete[] _currentTemperature;
    }
  
  void remdSwapping(double *potentialEnergy)
    {
      // print for debugging
#ifdef DEBUG_REMD
      if (_myGlobalRank==0) printf("swapReplicaStoc: %d\n", _numTrials);
#endif
      // loop over the temperature.
      for( int trial=0; trial<_numTrials; trial++)
	{
          // randomly select replica j from 0 to numReplica-2
          // make sure j is not the last replica (numReplica-1)
#ifdef DEBUG_REMD
          if (_myGlobalRank==0) printf("start trial %d\n",trial);
#endif
          unsigned j = (unsigned) _numReplica;
          while (j >= (_numReplica-1)) {
	       j = drand48()*(_numReplica-1);
#ifdef DEBUG_REMD
          if (_myGlobalRank==0) printf("j %d %d\n",j,(_numReplica-1));
#endif
          }
	  unsigned i=j+1;

	  
	  // find the replicas that are at temperature i and i-1
	  unsigned jReplica=_temperature2Replica[j]; 
	  unsigned iReplica=_temperature2Replica[i];

           assert( jReplica >=0 && jReplica < _numReplica );
          assert( iReplica >=0 && iReplica < _numReplica );


	  double Ej=potentialEnergy[jReplica];
	  double Ei=potentialEnergy[iReplica];
	  
	  //remapping temperatures from the indecies to actual double values
	  double inverseKTempj = getInverseKTemperature(j);
	  double inverseKTempi = getInverseKTemperature(i);
#ifdef DEBUG_REMD
          // prunsigned for debugging 
          if (_myGlobalRank == 0)
	    {
	      printf("TRIAL %d %f %f : %d %f %f delta: %f prob: %f %d \n",
		     i,1.0/inverseKTempi,Ei,
		     j,1.0/inverseKTempj,Ej);
	    }
#endif
	  // if (Ei<Ej  the always accept
	  // attempt a swap b/w the i and i-1 pair of temperatures	  
	  
	  unsigned accept=0;	  
	  
	  if(Ei<Ej) 
	    { 
	      accept=1;
	    }
	  else
	    {
	      double prob = (Ei-Ej)* (inverseKTempj-inverseKTempi);
	      prob = exp(-prob);
              double trial = drand48();
	      if(trial < prob) accept=1;
              
              // debugging
#ifdef DEBUG_REMD
              if (_myGlobalRank == 0)
		{
		  printf("prob: %f trial: %f ", prob,trial);
		}
#endif
	    }
	  
	  _numAccept[jReplica] += accept;
	  _numAttempt[jReplica]++;
	   
	  // if accept update
	  if(accept==1)
	    {
	      _temperature2Replica[i]=jReplica;
	      _temperature2Replica[j]=iReplica;
	      _currentTemperature[jReplica]=i;
	      _currentTemperature[iReplica]=j;
#ifdef DEBUG_REMD
	      if(_myGlobalRank==0)
	      printf("tempIdex_i %d tempIndex_j%d Temperature_i%lf Temperature_j%lf iReplica %d jReplica %d", 
		     i, j, inverseKTempi, inverseKTempj, iReplica, jReplica);
#endif
	    }

          // print for debugging
#ifdef DEBUG_REMD
          if (_myGlobalRank == 0) 
	    {
	      printf("TRIAL delta: %f accept: %d\n",
		     Ej-Ei,accept);
	    }
#endif
	  
	}
    }
 // need those two functions for restart 
  void setNumAccept(const unsigned numAccept, const unsigned replicaID)
    {
      assert( replicaID >=0 && replicaID < _numReplica );      
      _numAccept[replicaID] = numAccept; 
    }

  void setNumAttempt(const unsigned numAttempt, const unsigned replicaID)
    {
      assert( replicaID >=0 && replicaID < _numReplica );      
      _numAttempt[replicaID] = numAttempt; 
    }
 
  void getNumAccept( unsigned& numAccept, const unsigned replicaID) const
    {
      assert( replicaID >=0 && replicaID < _numReplica );      
      numAccept =_numAccept[replicaID];
    }


 // void getSwapAccept( unsigned& numAccept, const unsigned tempID) const
//     {
//       assert( tempID >=0 && tempID < _numReplica );      
//       numAccept =_numAccept[tempID];
//     }

//   void getSwapAttempt( unsigned& numAttempt, const unsigned tempID) const
//     {
//       assert( tempID >=0 && tempID < _numReplica );      
//       numAttempt=_numAttempt[tempID];
//     }



  unsigned& getSwapAccept( const unsigned tempID) const
    {
      assert( tempID >=0 && tempID < _numReplica );      
      return _numAccept[tempID];
    }

  unsigned& getSwapAttempt( const unsigned tempID) const
    {
      assert( tempID >=0 && tempID < _numReplica );      
      return _numAttempt[tempID];
    }


  void getTemperature2Replica(const unsigned tempID, unsigned& replicaID)
    {
      assert( tempID >= 0 && tempID < _numReplica );          
      replicaID = _temperature2Replica[tempID];
      assert( replicaID >=0 && replicaID < _numReplica );      
    }

  inline double getTemperature( const unsigned replicaID )
    {
      //unsigned tempID = 0;
      //getReplica2Temperature( replicaID, tempID );
      
      int tempID = _currentTemperature[replicaID];// CHECK AGAIN 
      assert( tempID >=0 && tempID < _numReplica );	
      return _temperature [ tempID ];
    }

  

  

  inline void getReplica2Temperature(const unsigned replicaID, unsigned& tempID)
    {
      static int count=0;
      assert( replicaID>=0 && replicaID < _numReplica);
      tempID = _currentTemperature[replicaID];
      assert( tempID >=0 && tempID < _numReplica );

#ifdef DEBUG_REMD  
    if(_myGlobalRank==0)
      printf("REMD: replica2Temperature[%d]=%d \n", replicaID, _replica2Temperature[replicaID]);
#endif
    } 

};



//unsigned REMD_Stochastic::_numAccept = 0;
//unsigned REMD_Stochastic::_numAttempt = 0;

#endif
