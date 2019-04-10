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
 
#include <BlueMatter/levmarq.hpp>
#include <string.h>

// y(x; a) is the sum of na/3 Gaussians (15.5.16). The amplitude, center, and width of the
// Gaussians are stored in consecutive locations of a: a[i] = Bk, a[i+1] = Ek, a[i+2] = Gk,
// k = 1, ..., na/3. The dimensions of the arrays are a[1..na], dyda[1..na].
void fgauss(double x, double a[], double *y, double dyda[], int na)
{
    int i;
    double fac,ex,arg;
    *y=0.0;
    for (i=1;i<=na-1;i+=3) {
	arg=(x-a[i+1])/a[i+2];
	ex=exp(-arg*arg);
	fac=a[i]*ex*2.0*arg;
	*y += a[i]*ex;
	dyda[i]=ex;
	dyda[i+1]=fac/a[i+2];
	dyda[i+2]=fac*arg/a[i+2];
    }
    // printf("X = %g; Y = %g\n",x,*y); 
}

// sum of terms with no constant
void fexpsum(double x, double a[], double *y, double dyda[], int na)
{
    int i;
    double fac,ex,arg;
    *y=0.0;
    for (i=1;i<=na-1;i+=2) {
	arg = x/a[i+1];
	ex = exp(-arg);
	fac = a[i]*ex;
	*y += fac;
	dyda[i] = ex;
	dyda[i+1] = fac*arg/a[i+1];
    }
    // printf("%f\n", *y);
}


// sum of terms with constant
void fexpsumoffset(double x, double a[], double *y, double dyda[], int na)
{
    int i;
    double fac,ex,arg;
    *y=a[1];
    dyda[1] = 1;
    for (i=2;i<=na-1;i+=2) {
	arg = x/a[i+1];
	ex = exp(-arg);
	fac = a[i]*ex;
	*y += fac;
	dyda[i] = ex;
	dyda[i+1] = fac*arg/a[i+1];
    }
}

double specdens4(double omega, double a, double t)
{
    return 2*a*t/(1+(omega*t)*(omega*t));
}

// This leaks memory, but shouldn't matter
int FitToExp(double *x, double *y, double *e, int npts, int nexp, double *c, double *t,
	     double &offset, double &tfinal, bool fixoffset, bool fixtfinal, double &chisq)
{
    NR_LevMarq LM;
    
    double *X, *Y, *S, *par;
    int *fit;
    int N = npts;
    int V = 2*nexp+1;

    double **covar, **alpha;
    
    // allocate 1-based vectors
    X = LM.vector(1,N);
    Y = LM.vector(1,N);
    S = LM.vector(1,N);

    par = LM.vector(1,V);
    fit = LM.ivector(1,V);
    covar = LM.matrix(1,V,1,V);
    alpha = LM.matrix(1,V,1,V);

    for (int i=0; i<npts; i++) {
	X[i+1] = x[i];
	Y[i+1] = y[i];
	S[i+1] = e[i];
    }

    par[1] = 0;

    if (nexp > 0) {
	par[2] = 0.008;
	par[3] = 0.05;
    }
    if (nexp > 1) {
	par[4] = 0.0005;
	par[5] = 1.0;
    }
    if (nexp > 2) {
	par[6] = 0.0002;
	par[7] = 10.0;
    }
    if (nexp > 3) {
	par[8] = 0.00002;
	par[9] = 50.0;
    }

    for (int i=1; i<=V; i++)
	fit[i] = 1.0;

    if (fixoffset) {
	fit[1] = 0;
	par[1] = offset;
    }
    if (fixtfinal) {
	fit[V] = 0;
	par[V] = tfinal;
    }

    double alambda = -1.0;
    chisq = 0.0;
        
    int num = 10;

    while (num--) {
	LM.mrqmin(X,Y,S,npts,par,fit,V,covar,alpha,&chisq,fexpsumoffset,&alambda);
    }

    alambda = 0.0;
    LM.mrqmin(X,Y,S,npts,par,fit,V,covar,alpha,&chisq,fexpsumoffset,&alambda);

    offset = par[1];

    for (int i=2; i<=V; i+=2) {
	c[i/2-1] = par[i];
	t[i/2-1] = par[i+1];
    }
    return 0;
}



int main(int argc,char **argv)
{

    if (argc < 3) {
	printf("NoesyTimes 8 4 [170.0] [3]\n");
	printf("Loads 8_4.cor 8_4_intra.cor tfinal nexp\n");
	printf("If tfinal is 0, optimizes it\n");
	exit(-1);
    }
    
#define MAXPTS 10000
    double X[MAXPTS], Y[MAXPTS], S[MAXPTS];
    double coeff[10], t[10];
    char corname[512], intraname[512];
    sprintf(corname, "%s_%s.cor", argv[1], argv[2]);
    sprintf(intraname, "%s_%s_intra.cor", argv[1], argv[2]);

    int I,J;
    I = atoi(argv[1]);
    J = atoi(argv[2]);

    bool fixtfinal = false;

    double usertfinal = 100.0;
    if (argc > 3) {
	double tf = atof(argv[3]);
	if (tf > 0) {
	    usertfinal = tf;
	    fixtfinal = true;
	}
    }
    int usernexp = 4;
    if (argc > 4)
	usernexp = atoi(argv[4]);

    int npts = 0;
    FILE *f = fopen(intraname, "r");
    while (3 == fscanf(f, "%lf %lf %lf", &X[npts], &Y[npts], &S[npts]))
	npts++;
    fclose(f);

    double chisq = 0.0;
    double intra_offset = 0;
    double tfinal = 10;
    FitToExp(X, Y, S, npts, 2, coeff, t, intra_offset, tfinal, false, false, chisq);

    printf("intra_chisq = %g\n", chisq);

    printf("ia0 = %g\n", intra_offset);

    for (int i=0; i<2; i++) {
	printf("ia%d = %g\n", i+1, coeff[i]);
	printf("it%d = %g\n", i+1, t[i]);
    }
    printf("ifexp(x) = ia0 + ia1*exp(-x/it1) + ia2*exp(-x/it2) + ia3*exp(-x/it3)\n");
    printf("iasym(x) = ia0\n");


    npts = 0;
    f = fopen(corname, "r");
    while (3 == fscanf(f, "%lf %lf %lf", &X[npts], &Y[npts], &S[npts]))
	npts++;
    fclose(f);

    chisq = 0.0;
    tfinal = usertfinal;
    FitToExp(X, Y, S, npts, usernexp, coeff, t, intra_offset, tfinal, true, fixtfinal, chisq);
    
    printf("chisq = %g\n", chisq);
    printf("a0 = %g\n", intra_offset);

    for (int i=0; i<usernexp; i++) {
	printf("a%d = %g\n", i+1, coeff[i]);
	printf("t%d = %g\n", i+1, t[i]);
    }
    for (int i=usernexp; i<4; i++) {
	printf("a%d = %g\n", i+1, 0);
	printf("t%d = %g\n", i+1, 0);
    }

    printf("fexp(x) = a0 ");
    for (int i=1; i<usernexp+1; i++)
	printf("+ a%d*exp(-x/t%d) ", i, i);
    printf("\n");



    double pi = 3.1415926535;
    double mu0 = 4*pi*1.0E-7;
    double gamma = 42.58E6*2*pi;
    double plk = 6.63E-34;
    double eta = 2*pi/5 * pow((gamma/2/pi), 4) * pow((plk/2/pi),2) * pow((mu0/4/pi),2)*1.0E60;  // m -> angs
    double omega = 498E6 * 2 * pi;
    double j1 = 0;
    double j0 = 0;
    double j1p = 0;
    double j0p = 0;
    for (int p=0; p<usernexp; p++) {
	double sp1, sp0;
	sp1 = specdens4(2*omega, coeff[p], t[p]*1E-9);
	j1 += sp1;
	sp0 = specdens4(0, coeff[p], t[p]*1E-9);
	j0 += sp0;
	if (coeff[p] > 0) {
	    j1p += sp1;
	    j0p += sp0;
	}
	printf("sp0_%d = %g\n", p+1, sp0);
	printf("sp1_%d = %g\n", p+1, sp1);
    }
    for (int p=usernexp; p<4; p++) {
	printf("sp0_%d = %g\n", p+1, 0);
	printf("sp1_%d = %g\n", p+1, 0);
    }
    double F = eta*(3*j1 - 0.5*j0);
    double Fp = eta*(3*j1p - 0.5*j0p);
    printf("j1 = %g\n", j1);
    printf("j0 = %g\n", j0);
    printf("f = %g\n", F);

    printf("j1p = %g\n", j1p);
    printf("j0p = %g\n", j0p);
    printf("fp = %g\n", Fp);



    printf("\n");
    
    return 0;
}
