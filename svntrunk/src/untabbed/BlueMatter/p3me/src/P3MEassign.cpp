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

#include <math.h>
#include <assert.h>
#include <stdio.h>
#include "P3MEimpl.hpp"

void p3m_assign_charge(int        numChg,   /* number of charges */
           double     *chgMag,  /* charge magnitude */
           cartVec_t  *chgPos,  /* charge positions */
           p3m_t      *that)
{
  int mptsX=that->meshPtsX, mptsY=that->meshPtsY, mptsZ=that->meshPtsZ;
  double grdSpX = that->gridSpacingX, grdSpX_1 = 1.0/grdSpX,
    grdSpY = that->gridSpacingY, grdSpY_1 = 1.0/grdSpY,
    grdSpZ = that->gridSpacingZ, grdSpZ_1 = 1.0/grdSpZ;
  cartVec_t cm = that->meshCenter,
    mExt,  /* the spatial extent of the mesh */
    llc;   /* the coordinates of the first grid point, the lower left corner */
  double ***qm = that->chgGrid;
    
  int chgi;
  /* {refgd1, refgd2, refgd3} is the nearest grid point to the charge */
  int refgd1, refgd2, refgd3;
  int p1, p2, p3, grdi1, grdi2, grdi3;
  double shift;
  cartVec_t rpos;   /* relative dimensionless position: (r - r0)/H */
  cartVec_t rd;
  int pleft, pright;
  double wx[P3M_MAXASSIGN], wy[P3M_MAXASSIGN], wz[P3M_MAXASSIGN], wgt, wcume;
  double wq, /* weighted charge */
    ttlq;    /* total assignment of an individual charge, check
    charge conservation */
  int n1, n2, n3;
  double invhcube;  /* 1/H^3 */

  mExt.x = mptsX*grdSpX;    
  mExt.y = mptsY*grdSpY;
  mExt.z = mptsZ*grdSpZ;

  llc.x = cm.x - 0.5*mExt.x + 0.5*grdSpX;
  llc.y = cm.y - 0.5*mExt.y + 0.5*grdSpY;
  llc.z = cm.z - 0.5*mExt.z + 0.5*grdSpZ;

  /* initialized at field level after first FFT */
  /*  for (n1=0; n1<mptsX; n1++)
      for (n2=0; n2<mptsY; n2++)
      for (n3=0; n3<mptsZ; n3++)
      qm[n1][n2][n3] = 0;  */
  
  /* in case of odd-order assignment, find the nearest grid point to
     the charge; otherwise find the left boundary of the mesh cell
     that contains the charge */
  if ((int)(that->assgn) % 2) {
    pright = ((int)(that->assgn))/2;
    pleft = - pright;
    shift = 0.5;
  }
  else {
    pright = ((int)(that->assgn))/2;
    pleft = -(pright-1);
    shift = 0;
  }

  for (chgi=0; chgi<numChg; chgi++) {
    /* if the charge is zero, we don't assign it onto the lattice so
       as to save time */
    if (chgMag[chgi]==0) {
      if (!(that->cache.dirty)) {
  that->cache.asgnwgt[chgi].refgd1 = 0;
  that->cache.asgnwgt[chgi].refgd2 = 0;
  that->cache.asgnwgt[chgi].refgd3 = 0;
  for (p1=0; p1<(int)(that->assgn); p1++) {
    that->cache.asgnwgt[chgi].wgtx[p1] = 0;
    that->cache.asgnwgt[chgi].wgty[p1] = 0;
    that->cache.asgnwgt[chgi].wgtz[p1] = 0;
  }
      }
      continue;
    }
    
    rpos.x = (chgPos[chgi].x - llc.x)*grdSpX_1;
    rpos.y = (chgPos[chgi].y - llc.y)*grdSpY_1;
    rpos.z = (chgPos[chgi].z - llc.z)*grdSpZ_1;
    refgd1 = (int)(floor(rpos.x+shift));
    refgd2 = (int)(floor(rpos.y+shift));
    refgd3 = (int)(floor(rpos.z+shift));

    /* if even-order assignment, take the distance to the mid-point of
       the nearest grid, else take the distance to the nearest grid
       point */
    rd.x = rpos.x - refgd1 + (shift-0.5);
    rd.y = rpos.y - refgd2 + (shift-0.5);
    rd.z = rpos.z - refgd3 + (shift-0.5);

    refgd1 %= mptsX;
    refgd2 %= mptsY;
    refgd3 %= mptsZ;
    if (refgd1<0) refgd1+=mptsX;
    if (refgd2<0) refgd2+=mptsY;
    if (refgd3<0) refgd3+=mptsZ;

#define MODULO_MESH(x, mpts) {           \
   if ((x)<0) (x)+=(mpts);               \
   else                                  \
     if ((x)>=(mpts)) (x)-=(mpts);       \
}

    p3m_assignment_weight(&rd, that, wx, wy, wz);
    
    for (p1=pleft; p1<=pright; p1++) {
      grdi1 = refgd1 + p1;
      MODULO_MESH(grdi1, mptsX);
     
      for (p2=pleft; p2<=pright; p2++) {
  grdi2 = refgd2 + p2;
  MODULO_MESH(grdi2, mptsY);
  wcume = wx[p1-pleft]*wy[p2-pleft];
    
  for (p3=pleft; p3<=pright; p3++) {
    grdi3 = refgd3 + p3;
    MODULO_MESH(grdi3, mptsZ);
    wgt = wcume*wz[p3-pleft];
    wq = chgMag[chgi]*wgt;
    qm[grdi1][grdi2][grdi3] += wq;
  }
      }
    }

    /* cache the weights if it is to be used later */
    if (!(that->cache.dirty)) {
      that->cache.asgnwgt[chgi].refgd1 = refgd1;
      that->cache.asgnwgt[chgi].refgd2 = refgd2;
      that->cache.asgnwgt[chgi].refgd3 = refgd3;
      for (p1=0; p1<(int)(that->assgn); p1++) {
  that->cache.asgnwgt[chgi].wgtx[p1] = wx[p1];
  that->cache.asgnwgt[chgi].wgty[p1] = wy[p1];
  that->cache.asgnwgt[chgi].wgtz[p1] = wz[p1];
      }
    }
  }

}

void p3m_assignment_weight(cartVec_t  *rd,
         p3m_t      *that,
         double     *wx,
         double     *wy,
         double     *wz)
{
  switch (that->assgn) {
  case NGP:
    wx[0] = wy[0] = wz[0] = 1.0;
    break;
  case CIC:
#define CIC_W_HALF(x) (0.5-(x))
#define CIC_WHALF(x) (0.5+(x))
    wx[0] = CIC_W_HALF(rd->x);
    wy[0] = CIC_W_HALF(rd->y);
    wz[0] = CIC_W_HALF(rd->z);
    wx[1] = CIC_WHALF(rd->x);
    wy[1] = CIC_WHALF(rd->y);
    wz[1] = CIC_WHALF(rd->z);
    
#undef CIC_W_HALF
#undef CIC_WHALF
#undef CIC_LEFT
    
    break;
    
  case TSC:  
#define TSC_W_1(x) (sqr(1-2*(x))/8.0)
#define TSC_W0(x) (0.75 - sqr(x))
#define TSC_W1(x) (sqr(1+2*(x))/8.0)
#define TSC_SHIFT_INDEX(x) ((x)+1)

    wx[TSC_SHIFT_INDEX(-1)] = TSC_W_1(rd->x);
    wy[TSC_SHIFT_INDEX(-1)] = TSC_W_1(rd->y);
    wz[TSC_SHIFT_INDEX(-1)] = TSC_W_1(rd->z);
    wx[TSC_SHIFT_INDEX(0)] = TSC_W0(rd->x);
    wy[TSC_SHIFT_INDEX(0)] = TSC_W0(rd->y);
    wz[TSC_SHIFT_INDEX(0)] = TSC_W0(rd->z);
    wx[TSC_SHIFT_INDEX(1)] = TSC_W1(rd->x);
    wy[TSC_SHIFT_INDEX(1)] = TSC_W1(rd->y);
    wz[TSC_SHIFT_INDEX(1)] = TSC_W1(rd->z);
    
#undef TSC_W_1
#undef TSC_W0
#undef TSC_W1
#undef TSC_SHIFT_INDEX

    break;

  case A4P:
    
#define A4P_W_3HALF(x) (1.0/48.0*(1.0-6.0*(x)+12.0*sqr(x)-8.0*cube(x)))
#define A4P_W_HALF(x) (1.0/48.0*(23.0-30.0*(x)-12.0*sqr(x)+24.0*cube(x)))
#define A4P_WHALF(x) (1.0/48.0*(23.0+30.0*(x)-12.0*sqr(x)-24.0*cube(x)))
#define A4P_W3HALF(x) (1.0/48.0*(1.0+6.0*(x)+12.0*sqr(x)+8.0*cube(x)))
    
    wx[0] = A4P_W_3HALF(rd->x);
    wy[0] = A4P_W_3HALF(rd->y);
    wz[0] = A4P_W_3HALF(rd->z);
    
    wx[1] = A4P_W_HALF(rd->x);
    wy[1] = A4P_W_HALF(rd->y);
    wz[1] = A4P_W_HALF(rd->z);

    wx[2] = A4P_WHALF(rd->x);
    wy[2] = A4P_WHALF(rd->y);
    wz[2] = A4P_WHALF(rd->z);

    wx[3] = A4P_W3HALF(rd->x);
    wy[3] = A4P_W3HALF(rd->y);
    wz[3] = A4P_W3HALF(rd->z);
    
#undef A4P_W_3HALF
#undef A4P_W_HALF
#undef A4P_WHALF
#undef A4P_W3HALF
    
    break;
  }
}


