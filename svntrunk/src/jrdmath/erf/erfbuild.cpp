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
 /*
 * (c) Copyright IBM Corp. 2006. CPL
 */ 
#include <cheberf_build.hpp>
#include <iostream>
#include <iomanip>
using namespace std ;
ChebyshevFit Fitter ;


int main(void)
{
	for ( int aSlice=0;aSlice<Fitter.k_Slices;aSlice += 1)
	{
		if (aSlice > 0) cout << ',' ;
		cout << "{ {" ;
		for (int aTerm=0;aTerm<Fitter.k_Terms;aTerm += 1)
		{
			if (aTerm > 0 ) cout << ',' ;
			cout << "  { "
					<< setw(40) << setprecision(30) 
					<< Fitter.mTable.SliceTable[aSlice].c[aTerm].pa
					<< " , "
					<< setw(40) << setprecision(30) 
				    <<  Fitter.mTable.SliceTable[aSlice].c[aTerm].pb
				    << " }\n" ;
		}
		cout << "} }\n" ;
		
	}
	return 0 ;
} ;
