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
 
#include<ctype.h>
#include<string>
#include<iostream>
#include<fstream>
#include <string.h>
#include "BlueMatter/Fit.hpp"

int main(int argc, char *argv[])
{
    FittingFunctions F;
    //typedef void(FittingFunctions::*func_pointer)(double, vector<double> &, 
    //                                              double*, vector<double>&) ;

    if (argc < 3)
        {
        if ( (argc > 1) && (!strncmp(argv[1], "-f", 2)))
            {
            cerr << "Function\tNumber of Parameters" << endl;
            F.ListFunctions();
            exit(-1);
            }
        cerr << "Usage: " << argv[0];
        cerr << " Datafile FunctionName Parameter1 [Parameter2 ...]";
        cerr << endl;
        cerr << "\t-f prints a list of available fitting functions" << endl;
        exit(-1);
        }

    char *data_filename = argv[1];
    string func_name = string(argv[2]);
    vector<double> params, dyda;
    vector<int> param_flag;
    // the rest of the command line arguments are assumed to be
    // initial values for the parameters
    // for now, assume all parameters are free variables
    for (int i=3; i<argc; i++)
        {
        params.push_back(atof(argv[i]));
        dyda.push_back(0.0);
        param_flag.push_back(1);
        }

    // read in the data file
    // format is assumed to be either free form comments which start
    // with "#" or 2 floating point numbers

    vector<double> x,y;
    vector<double> x2,y2;
    
    double xval, yval;
    string first_col;
    char junk[1024];
    ifstream inp(data_filename);
    while (inp >> first_col)
        {
        //if (first_col[0] != '#')
        if (first_col.find('#') != 0)
            {
            xval = atof(first_col.c_str());
            inp >> yval;
            x.push_back(xval);
            y.push_back(yval);
            }
        else // read off the rest of the line and discard
            {
            inp.getline(junk,1024);
            }
        }

    inp.close();

    Fit Fitter(x,y);
    Fitter.SetFunction(F[func_name], params, param_flag,
                       F.NumParams(func_name), F.LamdaScale(func_name));
    Fitter.SetConvergenceTolerance(1e-10);

    int iterations = Fitter.DoFit(100);
    if (iterations == -1)
        {
        cout << "failed to converge after 100 iterations" << endl;
        exit(-1);
        }
    else if (iterations < -1)
        {
        cout << "failed due to singular matrix: try new initial parameters" << endl;
        exit(-1);
        }
    else
        {
        cout << "#Converged after " << iterations << " iterations" << endl;
        }

    cout << endl;
    cout << "#Parameters: " << endl;
    for (int i=0; i<F.NumParams(func_name);  i++)
        {
        cout <<"#"<< i << "\t" << Fitter[i] << endl;
        }
    
    cout << endl;
    cout << "#Chi Sq = " << Fitter.m_chisq << endl;
    cout << "#Reduced Chi Sq = " << Fitter.m_chisq/x.size() << endl;
#if 0
    cout << "#writing out function values" << endl;
    vector<double> residual = Fitter.Residual();
    cerr << "got here" << endl;
    for (int i =0; i<Fitter.m_x.size(); i++)
        {
        cout << Fitter.m_x[i] << "\t" << residual[i] << endl;
        }
#endif
}

