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
 #include <BlueMatter/fft4.hpp>

void fft4::forward(void)
{
   inline_forward() ;
}

void fft4::inverse(void)
{
   inline_inverse() ;
}


// These for calling from 'C' ...
extern "C" {
   void fft4_forward(double* d) ;
   void fft4_inverse(double* d) ;
   } ;

void fft4_forward(double* d)
{
   complex m_table[4] ;
   m_table[0].re = d[0] ; m_table[0].im = d[1] ;
   m_table[1].re = d[2] ; m_table[1].im = d[3] ;
   m_table[2].re = d[4] ; m_table[2].im = d[5] ;
   m_table[3].re = d[6] ; m_table[3].im = d[7] ;

   fft4 transformer(m_table) ;
   transformer.inline_forward() ;

   d[0] = m_table[0].re ; d[1] = m_table[0].im ;
   d[2] = m_table[1].re ; d[3] = m_table[1].im ;
   d[4] = m_table[2].re ; d[5] = m_table[2].im ;
   d[6] = m_table[3].re ; d[7] = m_table[3].im ;
}

void fft4_inverse(double* d)
{
   complex m_table[4] ;
   m_table[0].re = d[0] ; m_table[0].im = d[1] ;
   m_table[1].re = d[2] ; m_table[1].im = d[3] ;
   m_table[2].re = d[4] ; m_table[2].im = d[5] ;
   m_table[3].re = d[6] ; m_table[3].im = d[7] ;

   fft4 transformer(m_table) ;
   transformer.inline_inverse() ;

   d[0] = m_table[0].re ; d[1] = m_table[0].im ;
   d[2] = m_table[1].re ; d[3] = m_table[1].im ;
   d[4] = m_table[2].re ; d[5] = m_table[2].im ;
   d[6] = m_table[3].re ; d[7] = m_table[3].im ;
}
