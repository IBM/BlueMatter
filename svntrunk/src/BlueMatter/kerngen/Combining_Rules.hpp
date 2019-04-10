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
 #ifndef _COMBINING_RULES_HPP_
#define _COMBINING_RULES_HPP_

#include <math.h>

// For CHARMM the 1,4 and the standard combiner are the same
class LennardJones14CHARMMCombiner
{
  public:
    template
      <class Param>
      static
      inline
      void
      Combine( const Param &A, const Param  &B, Param &C)
      {
        C.epsilon = sqrt(A.epsilon * B.epsilon);
        //   Correct combining rule.  NOTE must correspond to change in filter
        C.sigma   =     0.5 * (A.sigma   + B.sigma  );
      }
};

class LennardJones14AMBERCombiner
{
  public:
    template
      <class Param>
      static
      inline
      void
      Combine( const Param &A, const Param  &B, Param &C)
      {
        C.epsilon = sqrt(A.epsilon * B.epsilon) * 0.5;
        C.sigma   =     (A.sigma   + B.sigma  ) * 0.5;
      }
};


class LennardJones14OPLSAACombiner
{
  public:
    template
      <class Param>
      static
      inline
      void
      Combine( const Param &A, const Param  &B, Param &C)
      {
        C.epsilon = sqrt(A.epsilon * B.epsilon) * 0.5; // lj14 scaling factor
        C.sigma   = sqrt(A.sigma   * B.sigma  );
      }
};



#endif

