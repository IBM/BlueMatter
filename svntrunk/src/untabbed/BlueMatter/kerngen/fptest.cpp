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
 

#ifndef PKTRACE_PKMAIN_CNTL
#define PKTRACE_PKMAIN_CNTL (0)
#endif


#include <pk/platform.hpp>
extern "C" {
extern void * PkMain( int, char**, char** );
};


#include <pk/fxlogger.hpp>
#include <pk/pktrace_pk.hpp>

#include <pk/AtomicOps.hpp>
#include <pk/ThreadCreate.hpp>
#include <pk/Barrier.hpp>
// #include <BlueMatter/random.hpp>
/////////////////#include <BlueMatter/performance.hpp>

#include <BlueMatter/math.hpp>

// #include <fstream.h>


TraceClient PkMainStart;
//void *
//PkMain(int argc, char** argv, char** envp)
//  {
////  PkMainStart.HitOE(     PKTRACE_PKMAIN_CNTL,
////                         ACT,
////                         "PKMAIN_Cntl",
////                         THREAD_NUM + 1,
////                         PkMainStart );
//
//
//////////////  timebasestruct_t setupTimeS, setupTimeF;
//////////////  ReadTime(GET_SETUP_TIME, setupTimeS);
//
//
//   BegLogLine(1)
//    << argv[ 0 ]
//    << "::PkMain() running. "
//    << EndLogLine;
//
//#if 0
//   Math::hlog(0.759256) ;
///*
//   Math::hlog(1.0) ;
//   Math::hlog(0.5) ;
//   Math::hlog(2.0) ;
//   Math::hlog(4.0) ;
//   Math::hlog(1.0 + 1.0/16.0) ;
//   Math::hlog(1.0 + 1.0/256.0) ;
//*/
//#else
//   double x =0.759256 ;
//   for (int a=0; a<=800; a+=1)
//   {
//      double f0, f;
//      f0 = Math::hlog(x) ;
//      f = log(x) ;
//
//
//     BegLogLine(1)
//      << "log(" << x
//      << ") --> " << f
//      << " compare " << f0
//      << EndLogLine;
//
//      x *= 1.1 ;
//   } /* endfor */
//#endif
//
//   BegLogLine(1)
//    << argv[ 0 ]
//    << "::PkMain() completed. "
//    << EndLogLine;
//
//  return(NULL);
//  }
void *
PkMain (int argc, char** argv, char** envp)
  {
//  PkMainStart.HitOE(     PKTRACE_PKMAIN_CNTL,
//                         ACT,
//                         "PKMAIN_Cntl",
//                         THREAD_NUM + 1,
//                         PkMainStart );


////////////  timebasestruct_t setupTimeS, setupTimeF;
////////////  ReadTime(GET_SETUP_TIME, setupTimeS);


   BegLogLine(1)
    << argv[ 0 ]
    << "::PkMain() running. "
    << EndLogLine;

   double e0l = 0.0 ;
   double e1l = 0.0 ; 
   double x0l = -1.0 ; 
   double x1l = -1.0 ;
   double x = 0.0 ;
   for (int a=0; a<=5000000; a+=1)
   {
      double f0, df0, f, df;
      f0 = erfc(x) ;
      df0 = (-2.0/sqrt(M_PI)) * exp(-x*x) ;

      Math::erfccd(x,f,df) ;
      
      double e0 = fabs(f-f0) ;
      double e1 = fabs(df-df0) ;
      
      if ( e0 > e0l )
      {
        e0l = e0 ; 
        x0l = x ;
      }
      
      if ( e1 > e1l ) 
      {
        e1l = e1 ; 
        x1l = x ;
      }

//     BegLogLine(1)
//      << "erfccd(" << x
//      << ") --> (" << f
//      << ','       << df
//      << ") compare " << f0
//      << " predict-around (" << f-0.01*df
//      << ',' << f+0.01*df
//      << ')'
//      << EndLogLine;

      x += 0.000001 ;
   } /* endfor */

   BegLogLine(1)
   << "erfc max abs error = " << e0l*1e6 << "e-6 at x= " << x0l 
   << EndLogLine ;
   
    BegLogLine(1)
   << "derivative max abs error = " << e1l*1e6 << "e-6 at x= " << x1l 
   << EndLogLine ;
   
   BegLogLine(1)
    << argv[ 0 ]
    << "::PkMain() completed. "
    << EndLogLine;

  return(NULL);
  }

void *
PkMainb(int argc, char** argv, char** envp)
  {
//  PkMainStart.HitOE(     PKTRACE_PKMAIN_CNTL,
//                         ACT,
//                         "PKMAIN_Cntl",
//                         THREAD_NUM + 1,
//                         PkMainStart );


////////////  timebasestruct_t setupTimeS, setupTimeF;
////////////  ReadTime(GET_SETUP_TIME, setupTimeS);


   BegLogLine(1)
    << argv[ 0 ]
    << "::PkMain() running. "
    << EndLogLine;

   SwitchFunctionRadii sf0 ;
   sf0.SetRadii(7.0, 10.0) ;

   SwitchFunction sf;
   sf.Set(sf0) ;

   double x = 6.9 ;
   double ss = 0.0 ;
   for (int a=0; a<=320; a+=1)
   {
      double f, df;
      sf.Evaluate(x,f,df) ;

     BegLogLine(1)
      << "sf.Evaluate(" << x
      << ") --> (" << f
      << ','       << df
      << " predict-around (" << f-0.01*df
      << ',' << f+0.01*df
      << ')'
      << EndLogLine;

      x += 0.01 ;
      ss += f*df ;
   } /* endfor */

   BegLogLine(1)
    << argv[ 0 ]
    << "::PkMain() completed. "
    << EndLogLine;

    if (ss > 0.0 )
    {
       return NULL ;
    }
    else
    {
       return (void*) 1 ;
    } /* endif */
  }
