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
 #ifndef _TAGS__HPP_
#define _TAGS__HPP_

#include <string.h>
#define TIMESTEPS_TO_TUNE 10

class Tag 
{
public:
  enum { 
      TimeStep=0, 
      Bcast_Begin=1, 
      Bcast_End=2, 
      AllReduce_Begin=3,
      AllReduce_End=4,
      KSpace_Begin=5,
      KSpace_End=6,
      RealSpace_Begin=7,
      RealSpace_End=8,
      SiteTupleDriver_Begin=9,
      SiteTupleDriver_End=10,
      LocalFragmentDirectDriver_Begin=11,
      LocalFragmentDirectDriver_End=12,
      KSpaceChargeAssignment_Begin=13,
      KSpaceChargeAssignment_End=14,
      KSpaceForwardFFT_Begin=15,
      KSpaceForwardFFT_End=16,
      KSpaceConvolve_Begin=17,
      KSpaceConvolve_End=18,
      KSpaceInverseFFT_Begin=19,
      KSpaceInverseFFT_End=20,
      KSpaceInterpolation_Begin=21,
      KSpaceInterpolation_End=22,
      EwaldEiks_Begin = 23,
      EwaldEiks_End = 24,
      EwaldAllReduce_Begin = 25,
      EwaldAllReduce_End = 26,
      EwaldLoop_Begin = 27,
      EwaldLoop_End = 28
  };

  enum { TAG_COUNT=29 };

  static void GetNameForTag( int tag, char* name)
    {
      switch( tag )
        {
          case TimeStep:
            {
              strcpy(name,"Time_Step");
              strcat(name,"\0");
              break;
            }
          case Bcast_Begin:
            {
              strcpy(name,"Bcast_Begin");
              strcat(name,"\0");
              break;
            }
          case Bcast_End:
            {
              strcpy(name,"Bcast_End");
              strcat(name,"\0");
              break;
            }
          case AllReduce_Begin:
            {
              strcpy(name,"AllReduce_Begin");
              strcat(name,"\0");
              break;
            }          
          case AllReduce_End:
            {
              strcpy(name,"AllReduce_End");
              strcat(name,"\0");
              break;
            }          
          case KSpace_Begin:
            {
              strcpy(name,"KSpace_Begin");
              strcat(name,"\0");
              break;
            }          
          case KSpace_End:
            {
              strcpy(name,"KSpace_End");
              strcat(name,"\0");
              break;
            }          
          case RealSpace_Begin:
            {
              strcpy(name,"RealSpace_Begin");
              strcat(name,"\0");
              break;
            }          
          case RealSpace_End:
            {
              strcpy(name,"RealSpace_End");
              strcat(name,"\0");
              break;
            }          
          case SiteTupleDriver_Begin:
            {
              strcpy(name,"STDriver_Begin");
              strcat(name,"\0");
              break;
            }          
          case SiteTupleDriver_End:
            {
              strcpy(name,"STDriver_End");
              strcat(name,"\0");
              break;
            }          
          case LocalFragmentDirectDriver_Begin:
            {
              strcpy(name,"LFDDriver_Begin");
              strcat(name,"\0");
              break;
            }          
          case LocalFragmentDirectDriver_End:
            {
              strcpy(name,"LFDDriver_End");
              strcat(name,"\0");
              break;
            }          
          case KSpaceChargeAssignment_Begin:
            {
              strcpy(name,"KSpaceChargeAssignment_Begin");
              strcat(name,"\0");
              break;
            }          
          case KSpaceChargeAssignment_End:
            {
              strcpy(name,"KSpaceChargeAssignment_End");
              strcat(name,"\0");
              break;
            }
          case KSpaceForwardFFT_Begin:
            {
              strcpy(name,"KSpaceForwardFFT_Begin");
              strcat(name,"\0");
              break;
            }          
          case KSpaceForwardFFT_End:
            {
              strcpy(name,"KSpaceForwardFFT_End");
              strcat(name,"\0");
              break;
            }          
          case KSpaceConvolve_Begin:
            {
              strcpy(name,"KSpaceConvolve_Begin");
              strcat(name,"\0");
              break;
            }          
          case KSpaceConvolve_End:
            {
              strcpy(name,"KSpaceConvolve_End");
              strcat(name,"\0");
              break;
            }          
          case KSpaceInverseFFT_Begin:
            {
              strcpy(name,"KSpaceInverseFFT_Begin");
              strcat(name,"\0");
              break;
            }          
          case KSpaceInverseFFT_End:
            {
              strcpy(name,"KSpaceInverseFFT_End");
              strcat(name,"\0");
              break;
            }          
          case KSpaceInterpolation_Begin:
            {
              strcpy(name,"KSpaceInterpolation_Begin");
              strcat(name,"\0");
              break;
            }          
          case KSpaceInterpolation_End:
            {
              strcpy(name,"KSpaceInterpolation_End");
              strcat(name,"\0");
              break;
            }          
          case EwaldEiks_Begin:
            {
              strcpy(name,"EwaldEiks_Begin");
              strcat(name,"\0");
              break;
            }
          case EwaldEiks_End:
            {
              strcpy(name,"EwaldEiks_End");
              strcat(name,"\0");
              break;
            }
          case EwaldAllReduce_Begin:
            {
              strcpy(name,"EwaldAllReduce_Begin");
              strcat(name,"\0");
              break;
            }
          case EwaldAllReduce_End:
            {
              strcpy(name,"EwaldAllReduce_End");
              strcat(name,"\0");
              break;
            }
          case EwaldLoop_Begin:
            {
              strcpy(name,"EwaldLoop_Begin");
              strcat(name,"\0");
              break;
            }
          case EwaldLoop_End:
            {
              strcpy(name,"EwaldLoop_End");
              strcat(name,"\0");
              break;
            }
        default:
          {
            strcpy(name,"Tag_Not_Recognized");
            strcat(name,"\0");
	    BegLogLine(1) 
	      << "Tag " << tag << " not recognized"
	      << EndLogLine ;
          }
        }
    }
};

#endif
