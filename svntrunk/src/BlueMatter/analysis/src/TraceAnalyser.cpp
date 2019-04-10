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
 

#include <iostream>
#include <stdlib.h>
#include <iostream>
//#include <strstream>
#include <sstream>
#include <string.h>
#include <math.h>
#include <limits.h>

using namespace std ;

const unsigned SZ = 50;
const unsigned BUFSZ = 200;
const int MAXWRNCNT = 10;

const double TimeFactor = 1e-9;

const char SIMTICKSTART_STR[] = "SimtickLoopStart";
const char SIMTICKFINIS_STR[] = "SimtickLoopFinis";
const char START_STR[] = "Start";
const char FINIS_STR[] = "Finis";
const int  StartLen = strlen(START_STR);
const int  FinisLen = strlen(FINIS_STR);

int nodenum;

class IdInfo
{
public:

    enum IdType { INACTIVE, START, FINIS, IGNORE };

    IdInfo() { label[0] = '\0'; type = INACTIVE; pairid = -1; count = 0; }

    char label[SZ];
    IdType type;
    int pairid;
    int count; // 0,1,... - just a counter for this timestep

    unsigned long long steptime;  // timer for start type, accumulated time for finis type

    // these only for finis type
    double sumtime;   // sum of steptimes
    double sum2time;  // sum of squares of steptime
};

void help()
{
    cout << "Usage: TraceAnalyser [-nodenum nodenum] [-maxtraceid maxtraceid] [-start time] [-stop time] [-printraw] [-warningsoff] < blah.in > blah.out" << endl;
    exit(0);
}

int main( int argc, char *argv[] )
{
    int nodenum = 1, maxtraceid = 100;
    int start_time = 0, stop_time = INT_MAX;
    int warningson = 1;
    int printraw = 0;

    for( int i = 1; i < argc; i++ )
    {
        if ( !strcmp( argv[i], "-nodenum" ) )
        {
            if( ++i >= argc )
                help();
            istringstream is( argv[i] );
            is >> nodenum;
            if( !is )
                help();
        }
        else if( !strcmp( argv[i], "-maxtraceid" ) )
        {
            if( ++i >= argc )
                help();
            istringstream is( argv[i] );
            is >> maxtraceid;
            if( !is )
                help();
        }
        else if( !strcmp( argv[i], "-start" ) )
        {
            if( ++i >= argc )
                help();
            istringstream is( argv[i] );
            is >> start_time;
            if( !is )
                help();
        }
        else if( !strcmp( argv[i], "-stop" ) )
        {
            if( ++i >= argc )
                help();
            istringstream is( argv[i] );
            is >> stop_time;
            if( !is )
                help();
        }
        else if( !strcmp( argv[i], "-warningsoff" ) )
        {
            warningson = 0;
        }
        else if( !strcmp( argv[i], "-printraw" ) )
        {
            printraw = 1;
        }
        else
        {
            help();
        }
    }

    IdInfo *IdData = new IdInfo[maxtraceid * nodenum];
    int *timestep = new int[nodenum];
    if( !IdData || !timestep )
    {
        cerr << "ERROR: Out of memory. Exiting." << endl;
        exit( -1 );
    }
    for( int i = 0; i < nodenum; i++ )
        timestep[i] = -1;

    char buf[BUFSZ];

    int linecnt = 0;

    while( 1 )
    {
        if( ! cin.getline(buf,sizeof(buf)-1) )
            break;
        
        linecnt++;

        if( warningson && strlen( buf ) >= sizeof(buf)-2 )
        {
            cerr << "WARNING: line " << linecnt << " is too long. Possibly will not be interpreted correctly.\n" 
                 << buf << endl;
        }
        
        int node;
        int tagid;
        unsigned long long time;
        char b3[SZ], b4[SZ];
        istringstream is( buf );
        is >> node >> tagid >> time >> b3 >> b4;
        if( ! is || node < 0 || tagid < 0 || time < 0 ) 
        {
            if( warningson )
            {
                static int wrncnt = 0;
                if( ++wrncnt > MAXWRNCNT ) continue;
                cerr << "WARNING: line " << linecnt << " has bad format:\n" << buf << endl;
                if( wrncnt == MAXWRNCNT )
                    cerr << "Further warnings of this type will be suppressed." << endl;
            }
            continue;
        }
        if( tagid >= maxtraceid )
        {
            if( warningson )
            {
                static int wrncnt = 0;
                if( ++wrncnt > MAXWRNCNT ) continue;
                cerr << "WARNING: line " << linecnt << " has large trace_id " << tagid 
                     << " (maxtraceid=" << maxtraceid << "). Skipping.\n" << buf << endl;
                if( wrncnt == MAXWRNCNT )
                    cerr << "Further warnings of this type will be suppressed." << endl;
            }
            continue;
        }
        if( node >= nodenum )
        {
            if( warningson )
            {
                static int wrncnt = 0;
                if( ++wrncnt > MAXWRNCNT ) continue;
                cerr << "WARNING: line " << linecnt << " has large node_id " << node 
                     << " (nodenum=" << nodenum << "). Skipping.\n" << buf << endl;
                if( wrncnt == MAXWRNCNT )
                    cerr << "Further warnings of this type will be suppressed." << endl;
            }
            continue;
        }

        if( strstr( b4, SIMTICKSTART_STR ) )
            timestep[node]++;

        if( timestep[node] < start_time || timestep[node] > stop_time )
            continue;
        
        //--
        //-- label 3 used for labeling, label 4 has start/finis marks 
        //--

        if( IdData[tagid*nodenum+node].type == IdInfo::INACTIVE )
        {
            char *s3;
            if( strstr( b4+strlen(b4)-strlen(START_STR), START_STR ) )
            {
                IdData[tagid*nodenum+node].type = IdInfo::START;
                s3 = strstr( b3, START_STR );
            }
            else if( strstr( b4+strlen(b4)-strlen(FINIS_STR), FINIS_STR ) )
            {
                IdData[tagid*nodenum+node].type = IdInfo::FINIS;
                s3 = strstr( b3, FINIS_STR );
            }
            else
            {
                if( warningson )
                    cerr << "WARNING: label is not Start/Finis type in line " << linecnt << ":\n" << b4 << endl;
                IdData[tagid*nodenum+node].type = IdInfo::IGNORE;
                continue;
            }

            // cut start/finis part from the label
            if( !s3 ) s3 = strstr( b3, "[" );
            if(s3) { if(*(s3-1)=='_') *(s3-1)='\0'; else s3[0] = '\0'; }
            strcpy( IdData[tagid*nodenum+node].label, b3 );

            // pairid: search for the matching pair
            IdInfo::IdType pairtype = ( IdData[tagid*nodenum+node].type==IdInfo::START ) ? IdInfo::FINIS : IdInfo::START;
            int i;
            for( i=0; i<maxtraceid; i++ )
                if( i != tagid && IdData[i*nodenum+node].type == pairtype && !strcmp(IdData[i*nodenum+node].label,IdData[tagid*nodenum+node].label) )
                {
                    IdData[i*nodenum+node].pairid = tagid;
                    IdData[tagid*nodenum+node].pairid = i;
                    break;
                }

            // if the pair is bad - set current to bad too
            if( i < maxtraceid && IdData[IdData[tagid*nodenum+node].pairid*nodenum+node].type == IdInfo::IGNORE )
            {
                IdData[tagid*nodenum+node].type = IdInfo::IGNORE;
                continue;
            }
        }
        
        IdData[tagid*nodenum+node].count++;

        // check for errors in coupling with the pair

        if( IdData[tagid*nodenum+node].type == IdInfo::START && IdData[tagid*nodenum+node].count > 1    // not the first start
            && ( IdData[tagid*nodenum+node].pairid < 0 || IdData[IdData[tagid*nodenum+node].pairid*nodenum+node].count < IdData[tagid*nodenum+node].count-1 ) )
        {
            // invalidate id's
            if( warningson )
                cerr << "WARNING: mismatch in counts in line " << linecnt << ":\n" << buf 
                     << "\nStart count is " << IdData[tagid*nodenum+node].count;

            IdData[tagid*nodenum+node].type = IdInfo::IGNORE;

            if( IdData[tagid*nodenum+node].pairid >= 0 )
            {
                IdData[IdData[tagid*nodenum+node].pairid*nodenum+node].type = IdInfo::IGNORE;

                if( warningson )
                    cerr << " and finis count is " << IdData[IdData[tagid*nodenum+node].pairid*nodenum+node].count;
            } 
            else
            {
                if( warningson )
                    cerr << " and finis is not initialized yet";
            }

            if( warningson )
            {
                cerr << ". Will ignore these trace(s): " << tagid;
                if( IdData[tagid*nodenum+node].pairid >= 0 )
                    cerr << " " << IdData[tagid*nodenum+node].pairid;
                cerr << " for node " << node << endl;
            }
            continue;
        }
        else if( IdData[tagid*nodenum+node].type == IdInfo::FINIS 
            && ( IdData[tagid*nodenum+node].pairid < 0 || IdData[IdData[tagid*nodenum+node].pairid*nodenum+node].count != IdData[tagid*nodenum+node].count ) )
        {
            // invalidate id's
            if( warningson )
                cerr << "WARNING: mismatch in counts in line " << linecnt << ":\n" 
                     << buf << "\nFinis count is " << IdData[tagid*nodenum+node].count;

            IdData[tagid*nodenum+node].type = IdInfo::IGNORE;

            if( IdData[tagid*nodenum+node].pairid >= 0 )
            {
                IdData[IdData[tagid*nodenum+node].pairid*nodenum+node].type = IdInfo::IGNORE;

                if( warningson )
                    cerr << " and start count is " << IdData[IdData[tagid*nodenum+node].pairid*nodenum+node].count;
            } 
            else
            {
                if( warningson )
                    cerr << " and start is not initialized yet";
            }

            if( warningson )
            {
                cerr << ". Will ignore these trace(s): " << tagid;
                if( IdData[tagid*nodenum+node].pairid >= 0 )
                    cerr << " " << IdData[tagid*nodenum+node].pairid;
                cerr << " for node " << node << endl;
            }
            continue;
        }
        
        // all is ok - now process time label

        if( IdData[tagid*nodenum+node].type == IdInfo::START )
            IdData[tagid*nodenum+node].steptime = time;
        else if( IdData[tagid*nodenum+node].type == IdInfo::FINIS )
            IdData[tagid*nodenum+node].steptime += time - IdData[IdData[tagid*nodenum+node].pairid*nodenum+node].steptime;

        // end of timestep - finalize all timers
        if( strstr( buf, SIMTICKFINIS_STR ) )
        {
            for( int i=0; i<maxtraceid; i++ )
                if( IdData[i*nodenum+node].type == IdInfo::FINIS )
                {
                    int ind = i*nodenum+node;

                    double dtime = TimeFactor * IdData[i*nodenum+node].steptime;
                    IdData[i*nodenum+node].sumtime += dtime;
                    IdData[i*nodenum+node].sum2time += dtime * dtime;
                    IdData[i*nodenum+node].steptime = 0;

                    if( printraw )
                    {
                        cout << timestep[node] << "\t" << IdData[ind].label 
                             << "\t" << node << "\t" << dtime
                             << "\t" << IdData[ind].count << endl;
                    }
                }

            // efficiency: exit when stop_time reached for all nodes
            if( timestep[node] == stop_time )
            {
                static int nodes_done = 0;
                if( ++ nodes_done == nodenum )
                    break;
            }
        }
    }

//    cin.close();

    for( int n=0; n<nodenum; n++ )
    {
        // assumes that timestep[n] was also the last step computed for this node
        int size = timestep[n] - start_time + 1;
        for( int i=0; i<maxtraceid; i++ )
            if( IdData[i*nodenum+n].type == IdInfo::FINIS )
            {
                int k = i*nodenum+n;

                double t2_av = IdData[k].sum2time/size;
                double t_av = IdData[k].sumtime/size;
                double diff = t2_av - t_av*t_av;
                if( diff < 0.0 ) diff = 0.0;
                double delta_t = sqrt( diff / size );
                cout << IdData[k].label << "\t" << n << "\t" 
                     << t_av << " +- " << delta_t << "\t" << size << "\t" << double(IdData[k].count)/size << endl;
            }
    }
}
