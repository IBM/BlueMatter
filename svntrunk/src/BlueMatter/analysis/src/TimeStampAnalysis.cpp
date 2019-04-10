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
 
// To compile: xlC_r -I$BG_INSTALL_BASE/usr/opt/bluegene/include thisfile.cpp

#include <iostream>
#include <strstream>
using namespace std ;
#define BegLogLine(x) cerr
#define EndLogLine endl

#include <BlueMatter/Tag.hpp>

double timettt[Tag::TAG_COUNT];
double tts;

double avr[Tag::TAG_COUNT];
int    num[Tag::TAG_COUNT];

int begstat = TIMESTEPS_TO_TUNE, endstat = -1;

void doout( int t )
{
    if( tts >= 0 )
    {
        cout << t << " 0 & 0 " << (timettt[0]-tts) << " TimeStep " << endl;

        if( (begstat < 0 || t >= begstat) && (endstat < 0 || t <= endstat) )
        {
            avr[0] += (timettt[0]-tts);
            num[0]++;
        }
    }

    for( int i=0; 2*i+2 < Tag::TAG_COUNT; i++ )
        if( timettt[2*i+2] >= 0 )
        {
            char name[50];
            Tag::GetNameForTag( 2*i+2, name );
            cout << t << " " << (2*i+1) << " & " << (2*i+2) << " " << timettt[2*i+2] << " " << name <<endl;

            if( (begstat < 0 || t >= begstat) && (endstat < 0 || t <= endstat) )
            {
                avr[2*i+2] += timettt[2*i+2];
                num[2*i+2]++;
            }
        }
}

void reset()
{
    tts = timettt[0];
    for( int i=0; i<sizeof(timettt)/sizeof(timettt[0]); i++ )
        timettt[i] = -1;
}

int main( int argc, char** argv )
{
    if( argc >= 2 )
    {
        istrstream is( argv[1] );
        is >> begstat;
        if( !is )
        {
            cerr << "Can't read the first argument: " << argv[1] << endl;
            cerr << "Usage: TimeStampAnalysis [begin_average [end_average]]" << endl;
            return 1;
        }
    }

    if( argc >= 3 )
    {
        istrstream is( argv[2] );
        is >> endstat;
        if( !is )
        {
            cerr << "Can't read the second argument: " << argv[2] << endl;
            cerr << "Usage: TimeStampAnalysis [begin_average [end_average]]" << endl;
            return 1;
        }
    }
    
    for( int i=0; i < Tag::TAG_COUNT; i++ )
    {
        avr[i] = 0;
        num[i] = 0;
    }

    int prevs1 = -1;
    timettt[0] = -1; // for checking if timestep is ready - only start printing from 2nd record

    while(1)
    {
        int s1, s2, s3, s4, s5, s6;
        cin >> s1 >> s2 >> s3 >> s4 >> s5 >> s6;
        if( !cin )
            break;
        double time = s5+s6*1e-9;

        if( s1 != prevs1 ) // new time step begins
        {
            if( prevs1 >= 0 ) // not the first time?
                doout( prevs1 );

            reset();

            prevs1 = s1;
        }

        if( s3%2==1 || s3==0 ) // beginning of something or 0
        {
            timettt[s3] = time;
        } 
        else // end of something
        {
            double lapsed = time - timettt[s3-1];
            if( timettt[s3]<0 ) // new record
                timettt[s3]  = lapsed;
            else  // accumulate
                timettt[s3] += lapsed;
        }
    }

    cout << "=== Averages:" << endl;

    if( num[0] > 0 )
    {
        char name[50];
        Tag::GetNameForTag( 0, name );
        cout << avr[0]/num[0] << " " << name << " " << num[0] << endl;
    }

    for( int i=0; 2*i+2 < Tag::TAG_COUNT; i++ )
        if( num[2*i+2] > 0 )
        {
            char name[50];
            Tag::GetNameForTag( 2*i+2, name );
            cout << avr[2*i+2]/num[2*i+2] << " " << name << " " << num[2*i+2] << endl;
        }

    return 0;
}
