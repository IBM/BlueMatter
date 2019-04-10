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
 #ifndef __TIMER_HPP__
#define __TIMER_HPP__

// NEED NEED NEED: To correct the handling of MPI times... this
// is VERY expensive as it is.

class ClockTime
  {
  private:
    timebasestruct_t mClockTime;

  public:
    ClockTime()
      {
      Update();
      }

    void
    Update()
      {
      read_real_time( &mClockTime, TIMEBASE_SZ );
      }

    double
    GetUSecs()
      {
      timebasestruct_t TimeCopy = mClockTime;
      time_base_to_time( &TimeCopy, TIMEBASE_SZ );
//      double Rc = (long long)TimeCopy.tb_high * (long long)1000000000 + TimeCopy.tb_low ;
      double Rc  = TimeCopy.tb_low;
      Rc        /= 1000000000.0;
      Rc        += TimeCopy.tb_high;
      return( Rc );
      }

    double
    GetDeltaUSecs()
      {
      ClockTime Now;
      return( GetDeltaUSecs( Now ) );
      }

    double
    GetDeltaUSecs( ClockTime &Other )
      {
      double Rc = Other.GetUSecs() - GetUSecs();
      return( Rc );
      }

  };
#endif  // add nothing except dead code below this line


#if 0
      printf("\nDone.\n");
      timebasestruct_t Now;
      read_real_time( &Now, TIMEBASE_SZ );

      time_base_to_time( &Now, TIMEBASE_SZ );
      time_base_to_time( &(pcstate[ Handle ].StartTime), TIMEBASE_SZ );

      long long llThen = (long long)Then.tb_high * (long long)1000000000 + Then.tb_low ;
      long long llNow  = (long long)Now.tb_high * (long long)1000000000 + Now.tb_low ;

      long long llDelta = llNow - llThen;

      llTotalDelta += llDelta;

      unsigned h = llDelta / 1000000000 ;
      unsigned l = llDelta % 1000000000 ;

      unsigned th = llTotalDelta / 1000000000 ;
      unsigned tl = llTotalDelta % 1000000000 ;

      char Str[1024];
      sprintf( Str,
               " TimeDelta : %9u.%09u ",
               h,
               l  );
      fprintf( stdout, "%s %s\n", XActStr, Str );

      sprintf( Str,
               " TotalDelta : %9u.%09u ",
               th,
               tl  );
      fprintf( stdout, "%s %s\n", XActStr, Str );
      fprintf( stdout, "WARNING: This timer out put will be removed in next version of FOX!!!\n");

      fflush(stdout);
      printf("\nDone.\n");
#endif
