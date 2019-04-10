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
 #include<string>
#include <string.h>
#include<iostream>
#include<iomanip>
#include<fstream>
#include "BlueMatter/Fit.hpp"   

void read_input(char *data_filename, vector<double> &x, vector<double> &y, 
                int num_points)
    {
    x.clear();
    y.clear();
    double xval, yval;
    string first_col;
    char junk[1024];
    ifstream inp(data_filename);
    int num_lines = 0;
    while ( (inp >> first_col) && (num_lines < num_points) )
        {
        if (first_col.find('#') != 0) // if the first character isn't "#"
            {
            xval = atof(first_col.c_str());
            inp >> yval;
            x.push_back(xval);
            y.push_back(yval);
            num_lines++;
            }
        else // read off the rest of the line and discard
            {
            inp.getline(junk,1024);
            }
        }

    inp.close();
    }


int main(int argc, char *argv[])
{
    FittingFunctions F;

    if (!strncmp(argv[1], "-h", 2))
        {
        cerr << "Usage: " << argv[0] << " num_points file1 [file2 ...]" << endl;
        exit(-1);
        }

    int num_points = atoi(argv[1]);
    
    vector<double> x, y;
    Fit SingleExp(x,y);
    vector<double> params;
    vector<int> param_flag;
    // initialize all parameters to 1.0 and freely varying
    string s("SingleExp");
    int num_params = F.NumParams(s);
    params.assign(num_params, 1.0);
    param_flag.assign(num_params, 1);
    SingleExp.SetFunction(F[s], params, param_flag, 
                          F.NumParams(s));


    

    s = string("ConstrDoubleExp");
    //s = string("DoubleExp");
    Fit DoubleExp(x,y);
    num_params = F.NumParams(s);
    vector<double> params2;
    vector<int> param_flag2;
    // with constrained double exponential, we're forcing the sum
    // of the two exponentials at 0 to be equal to the first value
    // of the time series
    // accordingly -- we set the second weight to be immutable, fixed
    // at the first value of the time series
    params2.assign(num_params+1, 1.0);
    param_flag2.assign(num_params+1, 1);
    DoubleExp.SetFunction(F[s], params2, param_flag2, 
                          F.NumParams(s));
    DoubleExp.FixParameter(2);

    
    cout.setf(ios_base::showpoint);
    cout.setf(ios_base::scientific, ios_base::floatfield);
    cout << "       W         T        Off     ChiSq           W1         T1         W2        T2        Off      ChiSq    File" << endl;

    for (int i=2; i<argc; i++)
        {
        read_input(argv[i], x, y, num_points);
        SingleExp.NewData(x,y);
        SingleExp.ResetParameters();
        int iterations = SingleExp.DoFit(100);
        // mark unconverged fits with a "-"
        if (iterations > 0)
            {
            cout << "   ";
            }
        else  
            {
            cout << " **";
            }
        cout.precision(2);
        for (int j=0; j<3; j++)
            {
            cout.width(9);
            cout << SingleExp.GetParameter(j) << " ";
            }
        cout << SingleExp.m_chisq << " " << "||";
        
        
        DoubleExp.ResetParameters();
        // seed the parameters for the double-exponential fit
        // fix initial value at x = 0
        DoubleExp.SetParameter(2, y[0]);
        DoubleExp.SetParameter(4, 0.0);
        // set the decay times to ballpark reasonable, given the single
        // exponential results
        // There are typically two decay times in these curves -- one less than
        // 1 and one in the range 3-5
        
        double single = SingleExp.GetParameter(1);
        DoubleExp.SetParameter(0, SingleExp.GetParameter(0));
        DoubleExp.SetParameter(1, single);
        if (single > 1.5)
            {
            DoubleExp.SetParameter(3, 0.5);
            }
        else
            {
            DoubleExp.SetParameter(3, 5.0);
            }

        DoubleExp.NewData(x,y);
        iterations = DoubleExp.DoFit(100);
        if (iterations > 0)
            {
            cout << "  ";
            }
        else
            {
            cout << "**";
            }

        for (int j=0; j<5; j++)
            {
            if (j==2)
                {
                double weight = DoubleExp.GetParameter(2) -
                                DoubleExp.GetParameter(0) -
                                DoubleExp.GetParameter(4);
                cout.width(9);
                cout << weight << " ";
                }
            else
                {
                cout.width(9);
                cout << DoubleExp.GetParameter(j) << "  ";
                }
            }
        cout << DoubleExp.m_chisq << "  ";
        
        cout << argv[i] << endl;
       
        }

}
