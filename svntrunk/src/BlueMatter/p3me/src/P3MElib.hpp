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
 * History:         010915 RZ (JCP 115, 2348-2358,2001)
 *
 ***************************************************************************/

#ifndef __INCLUDE_P3M_H__
#define __INCLUDE_P3M_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * the differentiator candidates. ANALYTICAL is $i{\mathbf{k}}$
 * differentiator in Fourier space. GRADIENT is the exact
 * differentiation of the computed energy function
 */
typedef enum
{ FINITE_2PT=0, FINITE_4PT, FINITE_6PT, FINITE_8PT, FINITE_10PT,
  FINITE_12PT, ANALYTICAL, GRADIENT } diffop_t;

/*
 * the scheme to assign charges on to the grid points
 *
 * NGP: Nearest Grid Point (1-point assignment)
 * CIC: Charge In Cloud (2-point)
 * TSC: Triangular Shaped Cloud (3-point)
 */
typedef enum
{ NGP=1, CIC, TSC, A4P } assign_t;

/* vector in Cartesian coordinates */
typedef struct
{
  double x, y, z;
} cartVec_t;

typedef struct p3m p3m_t;

/*
 * where do we need to compute the field.
 * P3M_SRCFIELD: need to compute the field at the source point
 * P3M_OTHERFIELD: need to compute the field at points other than the source
 * P3M_SRCFIELD | P3M_OTHERFIELD : both of the above
 */
#define P3M_SRCFIELD 0x1
#define P3M_OTHERFIELD 0x2

/*
 * this is called once at the beginning of the simulation. it returns
 * a p3m_t that will be used later in P3M calculations.
 *
 * the initial size of the simulation box should be given, even though
 * the box size may change in a const pressure simulation. a grid
 * spacing should also be supplied. the smaller the grid spacing, the
 * more accurate is the mesh calculation. the program will partition
 * the space into a mesh according to the boxsize and the
 * user-specified grid spacing. since the number of mesh points must
 * be integers, the program will tailor the grid spacing a little in
 * each direction. but the resulting grid spacing will be close to
 * what the user specifies. 0.5 Anstrom is the recommended grid spacing
 * value.
 */
p3m_t *p3m_create(double    grdSp,  /* grid spacing */
		  diffop_t  diffOp, /* differentiator */
		  assign_t  assgn   /* order of assignment scheme(0,1,2) */
		  );
 

/*
 * shift the center of the mesh to the new position newCm
 */
void p3m_shift_mesh(p3m_t       *that,
		    cartVec_t   newCm);

/*
 * compute the reciprocal space contribution to the potential and electric
 * field induced by a set of point charges.
 *
 * if selfFld is true, the arguments numFldPts and fldPos are ignored.
 */
void p3m_k_space_field
( p3m_t       *,
  int         numChg,    /* number of source charges */
  double      *chgMag,   /* charge magnitude */
  cartVec_t   *chgPos,   /* charge position */
  double      boxx,      /* box size, only used in constant pressure */
  double      boxy,
  double      boxz,
  double      alpha,     /* charge screening coefficient */
  int         fldDesc,   /* where do we need to compute the field.
			  P3M_SRCFIELD: compute the field at the source points
			  P3M_OTHERFIELD: compute the field at other points
			  P3M_SRCFIELD | P3M_OTHERFIELD : both of the above */
  
  int         numFldPts, /* number of points that we want field */
  cartVec_t   *fldPos,   /* the locations of the points that we want field */
  double      *potFld,   /* return: electrostatic potential at the
                            desired points */
  cartVec_t   *eFld);    /* return: electric field at the desired points*/

/*
 * this must be called before running constant pressure simulation
 */
int p3m_const_pressure_set(p3m_t *that);

/*
 * this is called at the end of the simulation or when P3M is no
 * longer to be used. it takes care of the proper disposal of the
 * allocated storage, including freeing the pointer that!
 */
void p3m_destroy(p3m_t *that);

#ifdef __cplusplus
}
#endif

#endif




