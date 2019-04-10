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
 /***************************************************************************
 * Project:         BlueMatter
 *
 * Module:          P3ME
 *
 * Purpose:         P3ME method for periodic Coulomb Interactions
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010915 RZ 
 *
 ***************************************************************************/

#ifndef __INCLUDE_P3M_IMP_H__
#define __INCLUDE_P3M_IMP_H__

#include <fftw.h>
#include <rfftw.h>
#include <stdio.h>

#include "P3MEutils.hpp"
#include "P3MElib.hpp"

typedef fftw_complex complex_t;

typedef struct
{
  int refgd1, refgd2, refgd3;  /* nearest grid to the charge if
                                  odd-order assignment; left boundary
                                  of the interval containing the
                                  charge if even-order assignment*/
  double *wgtx, *wgty, *wgtz;   /* assignment weights in each direction */
} asgnw_t;


typedef struct
{
  int dirty;            /* if true, don't use cache */
  int numChg;           /* the number of source charges */
  asgnw_t *asgnwgt;     /* cached assignment weight for the charges */
} p3mCache_t;


typedef struct
{ 
  rfftwnd_plan chgFFT;  /* the FFT plan to transform the charge density */
  rfftwnd_plan fldFFT;  /* inverse FFT of G(k)\rho(k) to potential field */
} fftPlan_t;


typedef struct
{
  complex_t x, y, z;
} cmplxCart_t;

#define INFINITESIMAL 1.0e-10
#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define fourth(x) (sqr(sqr((x))))
#define sixth(x) (cube(sqr((x))))
#define is_almost_zero(x) (fabs((x)) < INFINITESIMAL)
#define P3M_SPACE_DIM 3

struct p3m
{
  int meshPtsX;         /* number of mesh points in X direction */
  int meshPtsY;
  int meshPtsZ;
  double gridSpacingX;  /* the distance between the neighbor grid points */
  double gridSpacingY;
  double gridSpacingZ;
  double grdSpX_1;      /* 1.0/gridSpacingX */
  double grdSpY_1;
  double grdSpZ_1;
  cartVec_t meshCenter; /* the center position of the mesh. by default
                           it is zero, user can change the mesh
                           position by p3m_shift_mesh(p3m_t *,cartVec_t *) */
  diffop_t diffOp;      /* differentiator */
  assign_t assgn;       /* the order of the assignment function */
  double alpha;         /* the charge screening coefficient */

  int isFlexibleBox;       /* whether the box size changes in time */
  int isPrepared;       /* is it p3m ready for calculation? */
  double ***greenFunct; /* the FT of the Green's function on the mesh */
  double ***chgGrid;    /* the charges on the grid points */
  double ***potFld;     /* the potential field on the mesh */
  fftPlan_t fftPlan;    /* how to do the fast Fourier transforms */
  cartVec_t ***efld;    /* electric field on the mesh */

  p3mCache_t cache;     /* store reusable data to avoid recomputing */
};

/* ==================================================
 *   Internal funcitons
 * ================================================== */

#define MVEC_MAX  1       /* the number of aliases to be considered to
                             converge the green function */
#define P3M_MAXASSIGN 7   /* the maximum order of charge assignment */

static double p3mFinite2pCoeff[] =
{1.0};

static double p3mFinite4pCoeff[] =
{4.0/3.0, -1.0/3.0};

static double p3mFinite6pCoeff[] =
{3.0/2.0, -3.0/5.0, 1.0/10.0};

static double p3mFinite8pCoeff[] =
{8.0/5.0, -4.0/5.0, 8.0/35.0, -1.0/35.0};

static double p3mFinite10pCoeff[] =
{5.0/3.0, -20.0/21.0, 5.0/14.0, -5.0/63.0, 1.0/126.0};

static double p3mFinite12pCoeff[] =
{12.0/7.0, -15.0/14.0, 10.0/21.0, -1.0/7.0, 2.0/77.0, -1.0/462.0};

static double *p3mFiniteDiffCoeff[] =
{ p3mFinite2pCoeff, p3mFinite4pCoeff, p3mFinite6pCoeff,
  p3mFinite8pCoeff, p3mFinite10pCoeff, p3mFinite12pCoeff};


int p3m_prepare(p3m_t *,
    double   boxx,
    double   boxy,
    double   boxz,
    double   alpha);


void p3m_allocate(p3m_t *);

void p3m_zero_mesh_charge(p3m_t  *);

void p3m_create_FFT_plan(p3m_t   *that);

void p3m_destroy_FFT_plan(p3m_t   *that);

void p3m_green_function(p3m_t *);

int p3m_choose_mesh_points(double    boxsize,
         double    grdSp);

void p3m_assign_charge(int        numChg,   /* number of charges */
           double     *chgMag,  /* charge magnitude */
           cartVec_t  *chgPos,  /* charge positions */
           p3m_t      *that);


void p3m_resize_cache(p3m_t      *that,
          int        newNumChg);

void p3m_free_cache(p3m_t      *);


void p3m_mesh_field(p3m_t   *);


void p3m_interpolate_field(p3m_t       *,
         int         numFldPts,
         cartVec_t   *fldPos,
         double      *potFld,
         cartVec_t   *eFld);

void p3m_gradient_field(p3m_t       *,
      int         numFldPts,
      cartVec_t   *fldPos,
      double      *potFld,
      cartVec_t   *eFld);

void p3m_assign_weight_derivative(cartVec_t     *rd,
          p3m_t         *,
          double        *dwx,
          double        *dwy,
          double        *dwz);

void p3m_cache_dump(p3m_t   *);

void p3m_assignment_weight(cartVec_t  *rd,
         p3m_t      *,
         double     *wx,
         double     *wy,
         double     *wz);

diffop_t p3m_differentiator_encode(char  *diffStr);

assign_t p3m_assignment_encode(char  *assgnStr);

#endif








