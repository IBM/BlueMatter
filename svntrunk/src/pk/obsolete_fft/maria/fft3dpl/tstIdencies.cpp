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
 #include <stdio.h>

#define nx  64
#define ny  64
#define nz  64
#define Px  4
#define Py  4
#define Pz  4

main(){
int barSzZ = nx/Pz;
int barSzX = ny/Px;
int barSzY = nz/Py;
int myPx = 0;
int myPy = 0; 
int myPz = 0; 
#define D
#ifdef D
 for(myPz =0 ; myPz<Pz; myPz++)
   for(myPx =0 ; myPx<Px; myPx++)
     for(int trgPz=0; trgPz<Pz; trgPz++) { 
       for(int trgPx=0; trgPx<Px; trgPx++) {
	 for(int ax=0; ax<nz*barSzX; ax++) {
	   for(int bx=myPz*barSzZ+myPx*nx; bx<(myPz+1)*barSzZ+myPx*nx; bx++) {
	     // int trgPx = bx/nx;
	     int az = ((bx%nx)%barSzZ)*ny+ax/nz + trgPx*barSzX;
	     int bz = ax%nz + nz*trgPz;
	     // int trgPz = b/nz;
	     // printf(" buf[%d][%d] (%d %d) ==  buf[%d][%d] (%d %d) \n",
	     //	ax, bx, trgPx, trgPz, a, b, myPx, myPz);
	     int iaz = az/ny+myPz*barSzZ;
	     int jaz = az%ny;
	     int kaz = bz%nz;
	     //   if(myPx==0 && myPz==0) {
	       //   printf("myCoords: %d %d %d\n", myPx, myPy, myPz);
	     //  printf("bufaz [%d][%d][%d](%d %d) \n", ia, ja, ka, myPx, trgPz);
	     //}
	     int iax = bx%nx;
	     int jax = ax/nz+trgPx*barSzX;
	     int kax = ax%nz;
	     if((iaz!=iax)||(jaz!=jax)||(kaz!=kax))
	       printf("ERROR z-> x \n\n");
	     // if(myPx==0 && myPz==0){
	     //printf("bufax[%d][%d][%d](%d %d) \n\n",
	     //      iax,jax, kax, trgPx, trgPz);
	     //}
	   }
	 }
       }
       // printf(" \n\n");
     }
#endif
 for(myPx =0 ; myPx<Px; myPx++)
   for(myPy =0 ; myPy<Py; myPy++)
     for(int trgPx=0; trgPx<Pz; trgPx++) { 
       for(int trgPy=0; trgPy<Py; trgPy++) {
	 for(int ay=0; ay<nx*barSzY; ay++) {
	   for(int by=myPx*barSzX+myPy*ny; by<(myPx+1)*barSzX+myPy*ny; by++) {
	     int ax = ((by%ny)%barSzX)*nz+ay/nx + trgPy*barSzY;
	     int bx = ay%nx + nx*trgPx;
	     int jax = ax/nz+myPx*barSzX;
	     int kax = ax%nz;
	     int iax = bx%nx;
	
	     //if(myPx==0 && myPy==0) {
	       //   printf("myCoords: %d %d %d\n", myPx, myPy, myPz);
	     //printf("bufax [%d][%d][%d](%d %d) \n", 
	     //      iax, jax, kax, myPy, trgPx);
	     //}

	     int jay = by%ny;
	     int kay = ay/nx+trgPy*barSzY;
	     int iay = ay%nx;
	     
	     //if(myPx==0 && myPy==0){
	     //printf("bufay[%d][%d][%d](%d %d)\n\n",
	     //      iay, jay, kay, trgPy, trgPx);
	     //}
	     if((iay!=iax)||(jay!=jax)||(kay!=kax))
	       printf("ERROR x->y \n\n");
	   }
	 }
       }
       
     } 

}
