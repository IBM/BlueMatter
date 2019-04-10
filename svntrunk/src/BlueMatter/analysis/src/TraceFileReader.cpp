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
#include <iomanip>
#include <strstream>
#include <string.h>
#include <stdlib.h>

using namespace std ;

char PKTRACE_ID_STR[] = "PKTRACE ID";
char PKTRACE_DATA_STR[] = "PKTRACE DATA";
char PKTRACE_STR[] = ">PKTRACE";

const unsigned SZ = 1000;
const unsigned MAX_TRACE_ID = 100;

char trace_lbl3[MAX_TRACE_ID][SZ];
char trace_lbl4[MAX_TRACE_ID][SZ];
int  trace_id[MAX_TRACE_ID];

int main()
{
    char buf[SZ];
    
    int linecnt = 0;

    while( 1 )
    {
        char *s;
        
        // skip lines without data
        while( cin.getline(buf,sizeof(buf)-1) )
        {
            linecnt++;
            if( strstr(buf,PKTRACE_STR) ) 
                break;
        }

        if( ! cin )
            break;

        if( s = strstr(buf,PKTRACE_ID_STR) )
        {
            s += strlen( PKTRACE_ID_STR );
            istrstream is( s );
            unsigned node, id;
            is >> hex >> node >> id;
            if( !is )
            {
                cerr << "ERROR: bad format in line " << linecnt << ":\n" << buf << endl;
                exit(-1);
            }
            else if( id >= MAX_TRACE_ID )
            {
                cerr << "WARNING: trace id " << id << " in line " << linecnt 
                     << " is greater than MAX_TRACE_ID " << MAX_TRACE_ID 
                     << ". This trace point will not be analyzed:\n"<< buf << endl;
            }
            else
            {
                is.getline( trace_lbl3[id], sizeof( trace_lbl3 ), ':' );
                is.getline( trace_lbl3[id], sizeof( trace_lbl3 ), ':' );
                is.getline( trace_lbl3[id], sizeof( trace_lbl3 ), ':' );
                is.getline( trace_lbl4[id], sizeof( trace_lbl4 ), ':' );
            }
        }
        else if( s = strstr(buf,PKTRACE_DATA_STR) )
        {
            s += strlen( PKTRACE_DATA_STR );
            istrstream is( s );
            unsigned node;
            is >> hex >> node;

            s += 6; // skip " NODE "

            while( s[0] != '<' )
            {
                char _4[50];
                strncpy( _4, s, 4 );
                _4[4] = 0;
                
                istrstream isid( _4 );
                int id;
                isid >> hex >> id;

                s += 4;

                strncpy( _4, s, 12 );
                _4[12] = 0;
                istrstream isll( _4 );
                unsigned long long ll;
                isll >> hex >> ll;

                s += 12;
                
                cout << node << " " << id << " " << ll << " " << trace_lbl3[id] << " " <<trace_lbl4[id]<<endl;
            }
        }
    }

    return 0;
}
