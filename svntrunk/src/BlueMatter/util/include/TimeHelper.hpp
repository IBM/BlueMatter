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
 #ifndef __TIME_HELPER_HPP__
#define __TIME_HELPER_HPP__

class TimeHelper 
{
    public:
    int Seconds;      // often seconds
    int Nanoseconds;  // often nano-seconds         
    
    static inline TimeHelper MinValue() 
    {
        TimeHelper rc;
        rc.Seconds = 0;
        rc.Nanoseconds = 0;
        return ( rc );
    }
          
    static inline TimeHelper MaxValue() 
    {
        TimeHelper rc;
        rc.Seconds = 0x7fffffff;
        rc.Nanoseconds = 0x7fffffff;
        return ( rc );
    }
    
    inline void Zero()
    {
        Seconds = 0;
        Nanoseconds = 0;
    }            

    inline TimeHelper operator+=( const TimeHelper& aOther )
    {
        int nanoSum = Nanoseconds + aOther.Nanoseconds;
        
        int difference = nanoSum - 1000000000;
        
        // If carry over occured adjust for it
        if ( difference < 0 )
            {
                Seconds += aOther.Seconds;
                Nanoseconds = nanoSum;                
            }
        else
        {
            Seconds += (aOther.Seconds + 1);
            Nanoseconds = difference;
        }
        
        return (*this);
    }
    
    inline TimeHelper operator-( const TimeHelper& aOther ) const
    {
        TimeHelper rc;
        rc.Seconds = Seconds - aOther.Seconds;
        rc.Nanoseconds = Nanoseconds - aOther.Nanoseconds;
        
        // If carry over occured adjust for it
        if( rc.Nanoseconds < 0 )
            {
                rc.Seconds--;
                rc.Nanoseconds += 1000000000;
            }                
        
        return ( rc );
    }
    
    inline
        int operator<( const TimeHelper& aOther )
    {
        if(Seconds < aOther.Seconds)
            return 1;
        else if(Seconds ==  aOther.Seconds)
            if(Nanoseconds < aOther.Nanoseconds)
                return 1;
            else 
                return 0;
        else
            return 0;
    }

    inline
        int operator<=( const TimeHelper& aOther )
    {
        if(Seconds < aOther.Seconds)
            return 1;
        else if (Seconds == aOther.Seconds)
            if(Nanoseconds <=  aOther.Nanoseconds)
                return 1;
            else
                return 0;
        else 
            return 0;
    }

    inline
        int operator>=( const TimeHelper& aOther )
    {
        if(Seconds > aOther.Seconds)
            return 1;
        else if (Seconds == aOther.Seconds)
            if(Nanoseconds >=  aOther.Nanoseconds)
                return 1;
            else
                return 0;
        else 
            return 0;
    }

    inline
        int operator>( const TimeHelper& aOther )
    {
        if(Seconds > aOther.Seconds)
            return 1;
        else if(Seconds ==  aOther.Seconds)
            if(Nanoseconds > aOther.Nanoseconds)
                return 1;
            else 
                return 0;
        else
            return 0;
    }

    static inline TimeHelper  GetTimeValue()
    {
        TimeHelper aTimeValue;
        
        timebasestruct_t Now;
        read_real_time( &Now, TIMEBASE_SZ );
        time_base_to_time( &Now, TIMEBASE_SZ );
        aTimeValue.Seconds      = Now.tb_high;
        aTimeValue.Nanoseconds  = Now.tb_low ;
        
        return( aTimeValue );
    }
    
    // NEED: Move this out to a header file..
    
    // Should move the stuff that works on doubles elsewhere.
    static inline double ConvertTimeValueToDouble( TimeHelper &aTimeValue )
    {
        double rc = aTimeValue.Nanoseconds * 0.000000001 + aTimeValue.Seconds;
        return(rc);
    }
};
#endif
