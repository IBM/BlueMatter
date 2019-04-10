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
 

// 0 is slowest and most precise
class TMantissa0: public TMantissaBase
{
public:
   void Init(const double* Source, int maxExponent) ;
   void GenerateTarget(double * Target, int maxExponent) ;
} ;

// Construct an integer-scaled mantissa, for sumission to the tree ALU
// Note ... where any of the values being reduced is NaN or Inf, 'maxExponent' will be
// 0x7ff, and it does not really matter what values this routine comes up with. It will
// be sorted out later after the 'integer add reduce'
void TMantissa0::Init(const double * Source, int maxExponent)
{
   doublemap mappedDouble = * ( doublemap * ) Source ;
   int exponent_bits = mappedDouble.exponent_bits() ;
   unsigned int s_lo_bits = mappedDouble.significand_lo_bits() ;
   int s_hi_bits = mappedDouble.significand_hi_bits() ;

   // 'normalised' numbers have an implied leading '1', set it explicitly if appropriate
   if (0 != exponent_bits)
   {
      s_hi_bits |= 0x00100000 ;
   } /* endif */

   // Floating-point numbers are formatted as sign+value, convert this to 2's complement
   if (mappedDouble.sign_bit() )
   {
      s_lo_bits ^= 0xffffffff ;
      s_hi_bits ^= 0xffffffff ;
      s_lo_bits += 1  ;
      if (s_lo_bits == 0)
      {
         s_hi_bits += 1 ;
      } /* endif */
   } /* endif */

   // Compensate the exponent so that
   // 1) Everything is aligned matching
   // 2) Integer overflow will not occur in the tree ALU
   // 3) Loss of significance is minimised
   // The 'worst case' for overflow is when everything to be added is the same, and represented like
   //   1.11111111.. in binary in the exponent
   // The scaling is set so that when these are all added, things just avoid overflowing into the sign bit
   unsigned int right_shift_to_match = maxExponent - ( exponent_bits >> 20 ) ;
   unsigned int right_shift_actual = k_SpareBits + right_shift_to_match ;


   Hi =  s_hi_bits >> right_shift_actual  ;
   Lo = ( s_hi_bits << (32-right_shift_actual) ) | ( s_lo_bits >> right_shift_actual) ;

   // Arrange for 'round-to-nearest' (half rounds up) to take out the bias which would result
   // from a 'round down' policy.
   unsigned int dropped = s_lo_bits << (32-right_shift_actual) ;
   if (dropped & 0x80000000)
   {
      Lo += 1  ;
      if (0 == Lo)
      {
         Hi += 1;
      } /* endif */
   } /* endif */
}

void TMantissa0::GenerateTarget(double * Target, int maxExponent)
{
   doublemap * mappedTarget = (doublemap *) Target ;
   // In case of missing a pattern, leave a NaN which we stand a chance of recognising
   (*mappedTarget).m_value.m_u.m_hi = 0xffff0123 ;
   (*mappedTarget).m_value.m_u.m_lo = 0x456789ab ;
   // If any of the sources was Inf or NaN , maxExponent will be 0x7ff, and we need to return NaN
   if (0x7ff == maxExponent)
   {
      (*mappedTarget).m_value.m_u.m_hi = 0xffffffff ;
      (*mappedTarget).m_value.m_u.m_lo = 0xffffffff ;
   }
   else
   {
     // If the sum is zero, we need to return zero
     if ( 0 == Hi && 0 == Lo )
     {
       (*mappedTarget).m_value.m_u.m_hi = 0x00000000 ;
       (*mappedTarget).m_value.m_u.m_lo = 0x00000000 ;
     }
     else
     {
        // General case, representable unless it overflows or underflows
        unsigned int s_hi_bits = Hi  ;
        unsigned int s_lo_bits = Lo ;
        int sign_bit = Hi & 0x80000000 ;

        if (Hi & 0x80000000)
        {
          // Result was negative. Generate corresponding positive number with 2's complement
          s_lo_bits ^= 0xffffffff  ;
          s_hi_bits ^= 0xffffffff ;
          s_lo_bits += 1 ;
          if (0 == s_lo_bits)
          {
             s_hi_bits += 1 ;
          } /* endif */
          // The case of trying to handle 0x8000_0000_0000_0000 would be an overflow, and should have been
          // prevented by the prenormalisation. Firewall here !
          if (s_hi_bits & 0x80000000)
          {
             assert(0) ;
             (*mappedTarget).m_value.m_u.m_hi = 0xffffffff ;
             (*mappedTarget).m_value.m_u.m_lo = 0xffffffff ;
             return ;
          } /* endif */
        } /* endif */

        // Now we need to figure a shift so that the 'implied leading 1' will appear in position
        // 0x0010_0000_0000_0000
        // This could be either direction.
        // Do it the portable way, but bear in mind that 'cntlz' needs to be used eventually, or maybe the FPU

        if (s_hi_bits >= 0x00200000)
        {
           unsigned int right_shift_count = 1 ;
           unsigned int rounding_bit = s_lo_bits & 1 ;
           s_lo_bits = ( s_lo_bits >> 1 ) | ( s_hi_bits << 31 ) ;
           s_hi_bits >>= 1 ;
           while (s_hi_bits >= 0x00200000)
           {
              right_shift_count += 1 ;
              rounding_bit = s_lo_bits & 1 ;
              s_lo_bits = ( s_lo_bits >> 1 ) | ( s_hi_bits << 31 ) ;
              s_hi_bits >>= 1 ;
           } /* endwhile */
           if (rounding_bit)
           {
              s_lo_bits += 1 ;
              if (0 == s_lo_bits)
              {
                 s_hi_bits += 1 ;
                 if (s_hi_bits >= 0x0020000)      // The add-in of the rounding bit caused a carry all the way
                 {
                   assert(0x00200000 == s_hi_bits ) ;
                   assert(0x00000000 == s_lo_bits ) ;
                   right_shift_count += 1 ;
                   s_hi_bits = 0x00100000 ;
                   s_lo_bits = 0x00000000 ;
                 } /* endif */
              } /* endif */
           } /* endif */
           // Should have everything ready to assemble the result

           int new_exponent = maxExponent + right_shift_count - k_SpareBits + 4 ; // !!!tjcw check whether we are accidentally scaling
           if (new_exponent <= 0)
           {
              // Underflow, set result to zero
              (*mappedTarget).m_value.m_u.m_hi = 0x00000000 ;
              (*mappedTarget).m_value.m_u.m_lo = 0x00000000 ;
           }
           else
           {
              if (new_exponent >= 0x7ff)
              {
                 // Overflow, set to NaN
                 (*mappedTarget).m_value.m_u.m_hi = 0xffffffff ;
                 (*mappedTarget).m_value.m_u.m_lo = 0xffffffff ;
              }
              else
              {
                 (*mappedTarget).m_value.m_u.m_hi = sign_bit | (s_hi_bits & 0x000fffff) | ( new_exponent << 20 ) ;
                 (*mappedTarget).m_value.m_u.m_lo = s_lo_bits ;
              } /* endif */
           } /* endif */
        }
        else
        {
           // We have a value that needs to be left-shifted 0 or more bits
           unsigned int left_shift_count = 0 ;
           while (s_hi_bits < 0x00100000)
           {
              left_shift_count += 1 ;
              s_hi_bits = ( s_hi_bits << 1 ) | ( s_lo_bits >> 31 ) ;
              s_lo_bits = s_lo_bits << 1 ;
           } /* endwhile */
           int new_exponent = maxExponent - left_shift_count - k_SpareBits + 4 ; // !!!tjcw check whether we are accidentally scaling
           if (new_exponent <= 0 )
           {
              // Underflow, set result to zero
              (*mappedTarget).m_value.m_u.m_hi = 0x00000000 ;
              (*mappedTarget).m_value.m_u.m_lo = 0x00000000 ;
           }
           else
           {
              assert(new_exponent < 0x3ff) ;
              (*mappedTarget).m_value.m_u.m_hi = sign_bit | (s_hi_bits & 0x000fffff) | ( new_exponent << 20 ) ;
              (*mappedTarget).m_value.m_u.m_lo = s_lo_bits ;
           } /* endif */
        } /* endif */
     } /* endif */
   } /* endif */
}

