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

#ifndef RFFTW_MPI_H
#define RFFTW_MPI_H

#include <BlueMatter/fftw_mpi.h>
#include <BlueMatter/rfftw.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/***********************************************************************/

typedef struct {
     fftw_plan p_fft_x;  /* plan for first dimension */
     rfftwnd_plan p_fft;  /* plan for subsequent dimensions */
     transpose_mpi_plan p_transpose, p_transpose_inv;
     fftw_complex *work; /* extra workspace, if needed */
} rfftwnd_mpi_plan_data;

typedef rfftwnd_mpi_plan_data *rfftwnd_mpi_plan;

extern rfftwnd_mpi_plan rfftwnd_mpi_create_plan(MPI_Comm comm,
                int rank, const int *n,
                fftw_direction dir,
                int flags);
extern rfftwnd_mpi_plan rfftw2d_mpi_create_plan(MPI_Comm comm,
                int nx, int ny,
            fftw_direction dir, int flags);
extern rfftwnd_mpi_plan rfftw3d_mpi_create_plan(MPI_Comm comm,
                int nx, int ny, int nz,
            fftw_direction dir, int flags);

extern void rfftwnd_mpi_destroy_plan(rfftwnd_mpi_plan p);

extern void rfftwnd_mpi_local_sizes(rfftwnd_mpi_plan p,
           int *local_nx,
           int *local_x_start,
           int *local_ny_after_transpose,
           int *local_y_start_after_transpose,
           int *total_local_size);

extern void rfftwnd_mpi(rfftwnd_mpi_plan p,
           int n_fields,
           fftw_real *local_data, fftw_real *work,
           fftwnd_mpi_output_order output_order);

/***********************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* RFFTW_MPI_H */
