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

int main(int argc,char **argv)
{
    NR_LevMarq LM;
    
    double *X, *Y, *S, B, G, E, *par;
    int *fit;
    int N = 100;
    int V = 3;
    
    double **covar, **alpha;
    
    // allocate 1-based vectors
    X = LM.vector(1,N);
    Y = LM.vector(1,N);
    S = LM.vector(1,N);

    par = LM.vector(1,V);
    fit = LM.ivector(1,V);
    covar = LM.matrix(1,V,1,V);
    alpha = LM.matrix(1,V,1,V);
    

    // for y=b*exp[((x-e)/g)^2]
    // these generate the original data
    B = 1.0;
    E = 0.5;
    G = 2.0;

    par[1] = B;
    par[2] = E;
    par[3] = G;

    fit[1] = fit[2] = fit[3] = 1;
  
    double *foo = LM.vector(1,N);

    for (int i=1; i<=N; i++)
	X[i] = i-N/2;

    // assign x, y, s values.  s is constant
    for(int i=1; i<=N; i++)
    {
	fgauss(X[i], par, &Y[i], foo, V);
	S[i] = 0.1;
    }

    double alambda = -1.0;
    double chisq = 0.0;
    
    par[1] *= 2.001;
    par[2] += 0.21;
    par[3] += 0.51;
    
    int num = 10;
    while (num--)
    {
	LM.mrqmin(X,Y,S,N,par,fit,V,covar,alpha,&chisq,fgauss,&alambda);
	printf("chisq = %f; lambda = %f  par[1] = %f  par[2] = %f par[3] = %f\n",chisq,alambda,par[1],par[2],par[3]); 
    }

    alambda = 0.0;
    LM.mrqmin(X,Y,S,N,par,fit,V,covar,alpha,&chisq,fgauss,&alambda);
    printf("chisq = %f; lambda = %f  par[1] = %f  par[2] = %f par[3] = %f\n",chisq,alambda,par[1],par[2],par[3]); 
    
    return 0;
}
