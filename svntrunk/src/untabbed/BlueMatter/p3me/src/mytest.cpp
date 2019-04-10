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
 #include "P3MElib.hpp"
#include "P3MEimpl.hpp"
#include <stdio.h>
#include <string.h>

#define __P3M__
#define MAXCHARGE 100000

int p3m_main(int argc, char *argv[]);

int greenFunc_main(int argc, char *argv[]);

void p3m_read_parameters(p3m_t     *that,
       char      *parFile);

void p3m_read_coordinates(int  *    numChg,
        double    chg[],
        cartVec_t xyz[]);

int main(int argc, char *argv[])
{
#ifdef __GREENFUNC__
  greenFunc_main(argc, argv);
#endif

#ifdef __P3M__ 
  p3m_main(argc, argv);
#endif 
  
  return 0;
}
  

int greenFunc_main(int argc, char *argv[])
{
  int n1, n2, n3;
  static const double boxx = 19.7308, boxy = 19.7308, boxz = 19.7308;
  static const double gridSpacing = 1.233175;
  static const double alpha = 0.28382022;
  p3m_t *that = p3m_create(gridSpacing,
        FINITE_4PT,
        TSC
                          );
  for (n1=0; n1<that->meshPtsX; n1++)
    for (n2=0; n2<that->meshPtsY; n2++)
      for (n3=0; n3<that->meshPtsZ/2+1; n3++)
  printf("g[%3d][%3d][%3d] = %21.15e\n", n1, n2, n3,
         that->greenFunct[n1][n2][n3]);

  p3m_destroy(that);
  
  return 0;
}

void p3m_read_parameters(p3m_t     *that,
       char      *parFile)
{
  FILE *fp;
  int meshSize;
  char strRead[64], diffOp[16], assgn[16];
  if (!parFile) {
    parFile = ".p3m";
  }
  if (!(fp = fopen(parFile, "r"))) {
    fprintf(stderr, "cannot open file %s\n", parFile);
    exit(0);
  }
  fscanf(fp, "%s", strRead);
  while (strcmp(strRead, "{"))
    fscanf(fp, "%s", strRead);
  while (strcmp(strRead, "}")) {
    fscanf(fp, "%s", strRead);
    if (!strcmp(strRead, "meshSize")) {
      fscanf(fp, "%d", &meshSize);
      that->meshPtsX = that->meshPtsY = that->meshPtsZ = meshSize;
    }
    else if (!strcmp(strRead, "gridSpacing"))
      fscanf(fp, "%lf", &(that->gridSpacingX));
    else if (!strcmp(strRead, "differentiator")) {
      fscanf(fp, "%s", diffOp);
      that->diffOp = p3m_differentiator_encode(diffOp);
    }
    else if (!strcmp(strRead, "assignment")) {
      fscanf(fp, "%s", assgn);
      that->assgn = p3m_assignment_encode(assgn);
    }
    else if (!strcmp(strRead, "alpha"))
      fscanf(fp, "%lf", &(that->alpha));
  }
}

void p3m_read_coordinates(int  *    numChg,
        double    chg[],
        cartVec_t xyz[])
{
  int i;
  scanf("%d\n", numChg);
  scanf("\n");
  for (i=0; i<*numChg; i++) 
    scanf("%lf\t%lf\t%lf\t%lf\n",
    chg+i, &(xyz[i].x), &(xyz[i].y), &(xyz[i].z));
}

int p3m_main(int argc, char *argv[])
{
  p3m_t dummy, *that;
  int numChg, n1,n2,n3;
  double *chg = (double *)safe_malloc(MAXCHARGE*sizeof(double)),
    *potFld = (double *)safe_malloc(MAXCHARGE*sizeof(double));
  cartVec_t *r = (cartVec_t *)safe_malloc(MAXCHARGE*sizeof(cartVec_t)),
    *eFld = (cartVec_t *)safe_malloc(MAXCHARGE*sizeof(cartVec_t)), cmshft;
  double mE = 0;
  int q;
  
  p3m_read_parameters(&dummy, NULL);
  
  that = p3m_create(dummy.gridSpacingX,
        dummy.diffOp,
        dummy.assgn
                    );

/*     for (n1=0; n1<that.meshPtsX; n1++)   */
/*       for (n2=0; n2<that.meshPtsY; n2++)   */
/*         for (n3=0; n3<that.meshPtsZ/2+1; n3++)   */
/*  	 printf("g[%d][%d][%d] = %e\n", */
/*  		n1, n2, n3,  */
/*  		that.greenFunct[n1][n2][n3]);   */
  
  p3m_read_coordinates(&numChg, chg, r);
  cmshft.x = -0.5*that->gridSpacingX; 
  cmshft.y = -0.5*that->gridSpacingX; 
  cmshft.z = -0.5*that->gridSpacingX; 
  p3m_shift_mesh(that, cmshft); 

  p3m_k_space_field(that, numChg, chg, r, dummy.meshPtsX*dummy.gridSpacingX, dummy.meshPtsY*dummy.gridSpacingX , dummy.meshPtsZ*dummy.gridSpacingX, dummy.alpha, P3M_SRCFIELD, 0, NULL, potFld, eFld);
  
/*    for (n1=0; n1<that.meshPtsX; n1++)  */
/*      for (n2=0; n2<that.meshPtsY; n2++)  */
/*        for (n3=0; n3<that.meshPtsZ; n3++)  */
/*    	printf("f[%d][%d][%d]=%e, E[%d][%d][%d]={%e, %e, %e}\n", */
/*  	       n1,n2,n3, that.potFld[n1][n2][n3], */
/*  	       n1,n2,n3, that.efld[n1][n2][n3].x, that.efld[n1][n2][n3].y, */
/*  	       that.efld[n1][n2][n3].z); */
  
  for (q=0; q<numChg; q++) {
    printf("%d: q=%f, r={%f,%f,%f}, f=%f, E={%f,%f,%f} U=%f\n",
     q, chg[q], r[q].x, r[q].y, r[q].z,
     potFld[q], eFld[q].x, eFld[q].y, eFld[q].z, chg[q]*potFld[q]);
    mE += chg[q]*potFld[q];
  }
  
  mE *= 0.5;
  
  printf("\ntotal energy: %f\n", mE);

/*    mE = 0; */
/*    for (n1=0; n1<that.meshPtsX; n1++) */
/*      for (n2=0; n2<that.meshPtsY; n2++) */
/*        for (n3=0; n3<that.meshPtsZ; n3++) */
/*  	mE += that.chgGrid[n1][n2][n3]*that.potFld[n1][n2][n3]; */

/*    mE *= 0.5; */
/*    printf("\nmesh energy: %f\n", mE); */
  
  p3m_destroy(that);

  free(chg);
  free(potFld);
  free(r);
  free(eFld);
  
  return 0;
}

