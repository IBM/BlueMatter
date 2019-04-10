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
 
/***************************************************************************
 * Project:         pK
 *
 * Module:          BLE.hpp
 *
 * Purpose:         Blue Light Emulation - help pk/platform uses SP2/MPI like BL
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010802 BGF Created from factor.cpp
 *
 ***************************************************************************/

// EVERYTHING IN THIS FILE IS FAIRLY HOKEY - FEEL FREE TO SIGN UP TO DO A
// BETTER JOB - BGF


#ifndef __BLE_HPP__
#define __BLE_HPP__

#include <stdio.h>
#include <math.h>

#define MAX_NODE_CNT 32*1024

struct VolDes
  {
  int x, y, z;
  double rating;
  };

struct RectDes
  {
  int x, y;
  double rating;
  };


extern int     BLE_MachineAddressSpaceCount;
extern VolDes  BLE_Machine3D;
extern RectDes BLE_Machine2D;


// This finds a fairly cubic us of a given Count of units.
// It also returns the dimensions of a good 2D unfolding.

void
FindBestCubicDimension( int      aCount ,
                        int     &aUsedNodeCount,
                        VolDes  &aVolDes,
                        RectDes &aRectDes )
  {
  VolDes VD[ MAX_NODE_CNT ];
  RectDes RD[ MAX_NODE_CNT ];

  int PrimeList[100000];
  int PrimeCount = 0;
  for(int i = 2; i <= aCount ; i++ )
    {
    int Factors[ 1000 ];
    int FactorCount = 0;
    int FactorValue = 1;
    int Unfactored = i;
    for(int p = 0; p < PrimeCount; p++ )
      {
      while( Unfactored % PrimeList[ p ] == 0 )
        {
        Unfactored /= PrimeList[ p ];
        Factors[ FactorCount ] = PrimeList[ p ];
        FactorCount++;
        FactorValue *= PrimeList[ p ];
        if( FactorValue == i )
          break;
        }
      }
    if( FactorCount == 0 )
      {
      //printf("%3d: New Prime\n", i );
      PrimeList[ PrimeCount ] = i;
      PrimeCount++;
      FactorCount = 1;
      Factors[ 0 ] = i;
      }
    printf("%3d: ", i );
    for(int f = 0; f < FactorCount; f++ )
      printf("%3d ", Factors[ f ] );
    printf("\n");

    int rectangle[2] = { 1, 1};
    int volume[3]    = { 1, 1, 1 };

    for(int f = FactorCount - 1; f >= 0 ; f-- )
      {
      if( rectangle[0] < rectangle[1] )
        rectangle[0] *= Factors[ f ];
      else
        rectangle[1] *= Factors[ f ];

      if( volume[ 0 ] < volume[ 1 ] && volume[ 0 ] < volume[ 2 ] )
        volume[ 0 ] *= Factors[ f ];
      else
        if( volume[ 1 ] < volume[ 2 ] )
          volume[ 1 ] *= Factors[ f ];
        else
          volume[ 2 ] *= Factors[ f ];
      }
    double RectS2V = sqrt( 1.0 * rectangle[0]*rectangle[0]-1 + rectangle[1]*rectangle[1]-1 ) / (rectangle[0]*rectangle[1] );
    double VolS2V  = sqrt( 1.0 * volume[0]*volume[0] - 1 + volume[1]*volume[1] - 1 + volume[2]*volume[2] - 1 ) / (volume[0]*volume[1]*volume[2]);

    VD[ i ].x = volume[0];
    VD[ i ].y = volume[1];
    VD[ i ].z = volume[2];
    VD[ i ].rating = VolS2V;

    RD[ i ].x = rectangle[ 0 ];
    RD[ i ].y = rectangle[ 1 ];
    RD[ i ].rating = RectS2V;

    }

  // These vars will be assigned during the following loop
  int SelectIndex;
  double WorseSelectRating;

  for( int i = 2; i <= aCount ; i++ )
    {

    double NodeValue = 1.0/i;  // value each node as 1/n

    SelectIndex = i;
    WorseSelectRating = VD[SelectIndex].rating > RD[SelectIndex].rating ? VD[SelectIndex].rating : RD[SelectIndex].rating ;

    // scan back
    for( int j = 0; i-j > 2; j++ )
      {
      double LostNodeCost = NodeValue * j;
      int CheckIndex = i - j;
      double WorseCheckRating = VD[CheckIndex].rating > RD[CheckIndex].rating ? VD[CheckIndex].rating : RD[CheckIndex].rating ;

      WorseCheckRating += LostNodeCost;  // add in the cost of leaving nodes out.

      if( WorseSelectRating > WorseCheckRating  )
        {
        SelectIndex = CheckIndex;
        WorseSelectRating = WorseCheckRating;
        }
      }
    }
  int i = SelectIndex;
  printf("BLE: Nodes %5d Rect dim %5d x %5d Rate %6f, volume %5d x %5d x %5d Rate %6f   Select: %3d Rate %6f : %6f, Loss %d\n",
    i, RD[i].x, RD[i].y, RD[i].rating,  VD[i].x, VD[i].y, VD[i].z, VD[i].rating,
    SelectIndex, RD[SelectIndex].rating, VD[SelectIndex].rating, i - SelectIndex);

  aUsedNodeCount = SelectIndex;
  aVolDes        = VD[ SelectIndex ];
  aRectDes       = RD[ SelectIndex ];

  return;
  }

#endif
