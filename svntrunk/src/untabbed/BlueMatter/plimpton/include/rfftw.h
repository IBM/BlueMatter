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
 * Copyright (c) 1997-1999 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* rfftw.h -- system-wide definitions for rfftw */
#ifndef RFFTW_H
#define RFFTW_H

#include <BlueMatter/fftw.h>

#ifdef __cplusplus
extern "C" {
#endif				/* __cplusplus */

/****************************************************************************/

#define RFFTW_V2

typedef fftw_plan rfftw_plan;
typedef fftwnd_plan rfftwnd_plan;

#define FFTW_REAL_TO_COMPLEX FFTW_FORWARD
#define FFTW_COMPLEX_TO_REAL FFTW_BACKWARD

extern void rfftw(rfftw_plan plan, int howmany, fftw_real *in, int istride,
      int idist, fftw_real *out, int ostride, int odist);
extern void rfftw_one(rfftw_plan plan, fftw_real *in, fftw_real *out);
     
extern rfftw_plan rfftw_create_plan_specific(int n, fftw_direction dir,
              int flags,
              fftw_real *in, int istride,
              fftw_real *out, int ostride);

extern rfftw_plan rfftw_create_plan(int n, fftw_direction dir, int flags);
extern void rfftw_destroy_plan(rfftw_plan plan);

extern void rfftw_fprint_plan(FILE *f, rfftw_plan p);
extern void rfftw_print_plan(rfftw_plan p);

extern void rfftw_executor_simple(int n, fftw_real *in,
          fftw_real *out,
          fftw_plan_node *p,
          int istride,
          int ostride,
          fftw_recurse_kind recurse_kind);

extern rfftwnd_plan rfftwnd_create_plan_specific(int rank, const int *n,
            fftw_direction dir, int flags,
            fftw_real *in, int istride,
            fftw_real *out, int ostride);
extern rfftwnd_plan rfftw2d_create_plan_specific(int nx, int ny,
             fftw_direction dir, int flags,
                fftw_real *in, int istride,
              fftw_real *out, int ostride);
extern rfftwnd_plan rfftw3d_create_plan_specific(int nx, int ny, int nz,
             fftw_direction dir, int flags,
                fftw_real *in, int istride,
              fftw_real *out, int ostride);
extern rfftwnd_plan rfftwnd_create_plan(int rank, const int *n,
            fftw_direction dir, int flags);
extern rfftwnd_plan rfftw2d_create_plan(int nx, int ny,
            fftw_direction dir, int flags);
extern rfftwnd_plan rfftw3d_create_plan(int nx, int ny, int nz,
            fftw_direction dir, int flags);
extern void rfftwnd_destroy_plan(rfftwnd_plan plan);
extern void rfftwnd_fprint_plan(FILE *f, rfftwnd_plan plan);
extern void rfftwnd_print_plan(rfftwnd_plan plan);
extern void rfftwnd_real_to_complex(rfftwnd_plan p, int howmany,
           fftw_real *in, int istride, int idist,
            fftw_complex *out, int ostride, int odist);
extern void rfftwnd_complex_to_real(rfftwnd_plan p, int howmany,
        fftw_complex *in, int istride, int idist,
         fftw_real *out, int ostride, int odist);
extern void rfftwnd_one_real_to_complex(rfftwnd_plan p,
          fftw_real *in, fftw_complex *out);
extern void rfftwnd_one_complex_to_real(rfftwnd_plan p,
          fftw_complex *in, fftw_real *out);

/****************************************************************************/

#ifdef __cplusplus
}                               /* extern "C" */
#endif                          /* __cplusplus */
#endif                          /* RFFTW_H */
