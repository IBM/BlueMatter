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
#include <fstream>
#include <mpi.h>
#include <algorithm>
#include <vector>
#include "TimeHelper.hpp"
using namespace std;

#define MAXSITES 100000
#define DOUBLES_PER_SITE 3
static double SendBuffer[ MAXSITES * DOUBLES_PER_SITE ];
static double RecieveBuffer[ MAXSITES * DOUBLES_PER_SITE ];

int main(int argc, char **argv, char **envp) 
{
    int myRank = 0;

    MPI_Init( &argc, &argv );  /* initialize MPI environment */
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank );
    

    int doublesPerSite = 3;
    
    vector<double> bcastTimes; 
    vector<double> allReduceTimes; 
    vector<int> sitesVector;
    
    for(int sites=1; sites <= MAXSITES; sites+=1000 )
        {
            int numDoubles = sites * DOUBLES_PER_SITE;
            
            // cout << "Number Of Doubles: " << numDoubles << endl;
            sitesVector.push_back( sites );

            
            TimeHelper startTime = TimeHelper::GetTimeValue();
            
            MPI_Bcast( SendBuffer, numDoubles, MPI_DOUBLE, 0, MPI_COMM_WORLD );

            TimeHelper finishTime = TimeHelper::GetTimeValue();
            TimeHelper difference = finishTime - startTime;
            double timeDiff = TimeHelper::ConvertTimeValueToDouble(difference);
        
            bcastTimes.push_back( timeDiff );
            
            startTime = TimeHelper::GetTimeValue();

            MPI_Allreduce(SendBuffer, RecieveBuffer , numDoubles , MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

            finishTime = TimeHelper::GetTimeValue();
            difference = finishTime - startTime;
            timeDiff = TimeHelper::ConvertTimeValueToDouble(difference);    

            allReduceTimes.push_back( timeDiff );
            // cout << "Number Of Sites: " << sites << endl;
        }

    if( myRank == 0 )
        {
            // cout << "Got here" << endl;
            ofstream os;            
            os.open("basicblocktimes.plot");
            for ( int i=0; i < bcastTimes.size(); i++)
                {
                    // cout << i << endl;
                    os << sitesVector[ i ] << "\t" 
                       << bcastTimes[ i ] << "\t"
                       << allReduceTimes[ i ] 
                       << endl;
                }
            os.close();

            ofstream os1;
            os1.open("bcastbasic.gplot", ios::out);
            os1 << "set xlabel 'Number of Sites'" << endl;
            os1 << "set ylabel 'Bcast Elapsed Time'" << endl;
            os1 << "plot \"basicblocktimes.plot\" using 1:2 with points" << endl;
            os1.close();

            ofstream os2;
            os2.open("allreducebasic.gplot", ios::out);
            os2 << "set xlabel 'Number of Sites'" << endl;
            os2 << "set ylabel 'AllReduce Elapsed Time'" << endl;
            os2 << "plot \"basicblocktimes.plot\" using 1:3 with points" << endl;
            os2.close();
        }

    MPI_Finalize();
    return 0;
}

