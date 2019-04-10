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
 void tenrootl(double* f, const double* x, int groupcount) ;

extern "C" {
  int _CP_main(void)  ;
  int _IOP_main(void) ;
  void s0printf(const char *, ...) ;
  unsigned long long GetTimeBase(void) ; 
} ;

void call_tenrootl(void)
{
   double src[210] ;
   double tgt[200] ;
   double d = 1.0 ;
   for (int a=0; a<400; a+=1)
   {
      src[a] = d ;
      d += 1.0 ;
   } /* endfor */
   long long tbstart = GetTimeBase() ; 
   for (int b=0; b<400000; b+=1)
   {
      tenrootl(tgt,src,20) ;
   } /* endfor */
   long long tbend = GetTimeBase() ; 
   int taken = tbend-tbstart ;
   s0printf("Ending after %i cycles\n",taken) ; 
} ;

int _CP_main(void)
{
   call_tenrootl() ;
   return 0 ;
}

int _IOP_main(void)
{
   call_tenrootl() ;
   return 0 ;
}
