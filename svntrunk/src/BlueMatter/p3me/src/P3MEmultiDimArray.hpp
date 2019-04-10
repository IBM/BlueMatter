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
 #ifndef __INCLUDE_MULTIDIMARRAY_H__
#define __INCLUDE_MULTIDIMARRAY_H__

#include <stdlib.h>
#include <stdio.h>

#include "P3MEutils.hpp"

/*
 *  allocate a new 3d row-major format array
 *
 *  n1, n2, n3:     rank in each dimension
 *
 *  array[0][0] gives the pointer to the one-dimensional array in
 *  which the 3d array is stored in row-major format 
 */

#define NEW_3D_ARRAY(data_t, n1, n2, n3, array)    {                        \
  data_t *multiDimArray_glbRowMajor =                                       \
    (data_t *)safe_malloc((n1)*(n2)*(n3)*sizeof(data_t));                   \
  data_t **multiDimArray_glbPtr2d =                                         \
    (data_t **)safe_malloc((n1)*(n2)*sizeof(data_t *));                     \
  data_t ***multiDimArray_glbPtr3d =                                        \
    (data_t ***)safe_malloc((n1)*sizeof(data_t **));                        \
  int  multiDimArray_glbI1, multiDimArray_glbI2;                            \
                                                                            \
  for (multiDimArray_glbI1=0; multiDimArray_glbI1<(n1);                     \
       multiDimArray_glbI1++) {                                             \
    multiDimArray_glbPtr3d[multiDimArray_glbI1] =                           \
      (data_t **)multiDimArray_glbPtr2d + multiDimArray_glbI1*(n2);         \
    for (multiDimArray_glbI2=0; multiDimArray_glbI2<(n2);                   \
	 multiDimArray_glbI2++)                                             \
      multiDimArray_glbPtr3d[multiDimArray_glbI1][multiDimArray_glbI2] =    \
	(data_t *)multiDimArray_glbRowMajor +                               \
	multiDimArray_glbI1*(n2)*(n3) + multiDimArray_glbI2*(n3);           \
  }                                                                         \
  array = (data_t ***)multiDimArray_glbPtr3d;                               \
}

void destroy_3d_array(void ***array)
{
  if (array) {
    if (array[0]) {
      if (array[0][0]) {
	free(array[0][0]);
	array[0][0] = 0;
      }
      free(array[0]);
      array[0] = 0;
    }
    free(array);
    array = 0;
  }
}

#endif





