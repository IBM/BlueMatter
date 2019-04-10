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
 #include <BlueMatter/LSQ.hpp>

Vector::Vector(){component[0] = component[1] = component[2] = 0.0;}

Vector::Vector(const double& x, const double& y, const double& z) 
{
    component[0] = x; component[1] = y; component[2] = z;
}

Vector::Vector(const Vector& v) {
    component[0] = v[0]; component[1] = v[1]; component[2] = v[2];
}


Matrix::Matrix()
{
};

Matrix::Matrix(
	       const double& xx, const double& xy, const double& xz, 
	       const double& yx, const double& yy, const double& yz, 
	       const double& zx, const double& zy, const double& zz)
{
    component[0] = Vector(xx, xy, xz);
    component[1] = Vector(yx, yy, yz);
    component[2] = Vector(zx, zy, zz);
};

Matrix::Matrix(const Matrix& m)
{
    component[0][0] = m[0][0];
    component[0][1] = m[0][1];
    component[0][2] = m[0][2];
    
    component[1][0] = m[1][0];
    component[1][1] = m[1][1];
    component[1][2] = m[1][2];
    
    component[2][0] = m[2][0];
    component[2][1] = m[2][1]; 
    component[2][2] = m[2][2];
}


Vector operator/(const Vector& v, const Matrix& m) 
{
    Vector vRes;
    int i, j;
    for (i = 0; i < 3; i++) {
	for (vRes[i] = 0.0, j = 0; j < 3; j++) {
	    vRes[i] += v[j] * m[j][i];
	}
    }
    return vRes;
}

Vector operator/(const Matrix& m, const Vector& v)
{
    Vector vRes;
    int i, j;
    for (i = 0; i < 3; i++) {
	for (vRes[i] = 0.0, j = 0; j < 3; j++) {
	    vRes[i] += m[i][j] * v[j];
	}
    }
    return vRes;
}

Matrix operator+(const Matrix& m1, const Matrix& m2)
{
    Matrix mRes;
    int i, j;
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    mRes[i][j] = m1[i][j] + m2[i][j];
	}
    }
    return mRes;
}

Matrix operator+(const Matrix& m)
{
    Matrix mRes;
    int i, j;
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    mRes[i][j] = m[i][j];
	}
    }
    return mRes;
}

Matrix operator-(const Matrix& m1, const Matrix& m2)
{
    Matrix mRes;
    int i, j;
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    mRes[i][j] = m1[i][j] - m2[i][j];
	}
    }
    return mRes;
}

Matrix operator-(const Matrix& m)
{
    Matrix mRes;
    int i, j;
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    mRes[i][j] = - m[i][j];
	}
    }
    return mRes;
}

Matrix operator/(const Matrix& m1, const Matrix& m2)
{
    Matrix mRes;
    int i, j, k;
    
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    for (mRes[i][j] = 0.0, k = 0; k < 3; k++) {
		mRes[i][j] += m1[i][k] * m2[k][j];
	    }
	}
    }
    return mRes;
}

Matrix operator*(const Matrix& m, const double& d) 
{
    Matrix mRes;
    int i, j;
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    mRes[i][j] = d * m[i][j];
	}
    }
    return mRes;
}

Matrix operator/(const Matrix& m, const double& d) 
{
    Matrix mRes;
    int i, j;
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    mRes[i][j] = m[i][j] / d;
	}
    }
    return mRes;
}

Matrix operator*(const double& d, const Matrix& m) 
{
    Matrix mRes;
    int i, j;
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    mRes[i][j] = d * m[i][j];
	}
    }
    return mRes;
}

const Matrix& 
Matrix::operator=(const Matrix& m2) 
{
    int i, j;
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    (*this)[i][j] = m2[i][j];
	}
    }
    return (*this);
}

const Matrix& 
Matrix::operator+=(const Matrix& m2) 
{
    int i, j;
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    (*this)[i][j] += m2[i][j];
	}
    }
    return (*this);
}

const Matrix& 
Matrix::operator*=(const double& d) 
{
    int i, j;
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    (*this)[i][j] *= d;
	}
    }
    return (*this);
}

const Matrix& 
Matrix::operator/=(const double& d) 
{
    int i, j;
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    (*this)[i][j] /= d;
	}
    }
    return (*this);
}

const Matrix& 
Matrix::operator-=(const Matrix& m2) 
{
    int i, j;
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    (*this)[i][j] -= m2[i][j];
	}
    }
    return (*this);
}

Matrix operator&(const Vector& v1, const Vector& v2)
{
    int i, j;
    Matrix mRes;
    
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    mRes[i][j] = v1[i] * v2[j];
	}
    }
    return mRes;
}

Matrix 
Matrix::operator!()const
{
    Matrix mRes;
    int i, j;
    
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    mRes[i][j] = (*this)[j][i];
	}
    }
    return mRes;
}

double
Tr(const Matrix& m)
{
    double mRes;
    int i;
    
    for (mRes = 0.0, i = 0; i < 3; i++) {
	mRes += m[i][i];
    }
    return mRes;
}

double Det(const Matrix& m)
{
    // * is cross product, / is scalar product
    return (m[0] * m[1]) / m[2];
}

void 
Matrix::Diag(double eigvals[3], Vector eigvecs[3]) const
{
    static double d[3];
    static double e[3];
    static double a0[3], a1[3], a2[3];
    static double* a[3] = {a0, a1, a2};
    int i, j;
    
    for (i = 0; i < 3; i++)
	for (j = 0; j < 3; j++)
	    a[i][j] = (*this)[i][j];
	
	tred2(a, d, e, 3);
	tqli(d, e, a, 3);
	eigsrt(d, a, 3);
	
	for (i = 0; i < 3; i++) {
	    for (eigvals[i] = d[i], j = 0; j < 3; j++) {
		eigvecs[i][j] = a[j][i];
	    }
	}
}
//--------------------------------------------------
// << operator for Matrix
ostream &operator << (ostream &s, const Matrix &m)
{
    s.setf(ios::fixed,ios::floatfield);
    for (int i=0; i<3; i++) 
	for (int j=0; j<3; j++) 
	    s << m[i][j] << ((j==2)?'\n':' ');
	return s;
}

// << operator for Vector
ostream &operator << (ostream &s, const Vector &v)
{
    s.setf(ios::fixed,ios::floatfield);
    s << v[0] << ' ' << v[1] << ' ' << v[2];
    return s;
}


#define MAX_ITTER 1000
#define __COMPLAIN_ABOUT_ITTER__

/****************************************************
**	eigsrt sorts the n eigenvalues d[]
**	and n eigenvectors v[][].
****************************************************/

void eigsrt(double * const d, double * const * const v, const int& n)
{
    int		i, j, k;
    double	p;
    
    for (i = 0; i < n - 1; i++) {
	
	for (k = i, p = d[i], j = i + 1; j < n; j++) 
	    if (d[j] <= p) 
		p = d[k = j];
	    
	    if (k != i) 
		for (d[k] = d[i], d[i] = p, j = 0; j < n; j++) 
		    p = v[j][i], v[j][i] = v[j][k], v[j][k] = p;
		
    }
}

void eigsrt2(double * const d, double * const * const v, const int& n)
{
    int		i, j, k;
    double	p;
    
    for (i = 0; i < n - 1; i++) {
	
	for (k = i, p = d[i], j = i + 1; j < n; j++) 
	    if (d[j] >= p) 
		p = d[k = j];
	    
	    if (k != i) 
		for (d[k] = d[i], d[i] = p, j = 0; j < n; j++) 
		    p = v[j][i], v[j][i] = v[j][k], v[j][k] = p;
		
    }
}


/********************************************************
**	tred2 effects the Housholder tri-diagonalization of
**	nXn matrix a[][].  a[][] is replaced by the orthogonal
**	matrix q[][] that effects the transformation on a,
**	d[] returns the diagonal components, and e[] returns
**	the off-diagonal components with e[0] = 0.0.
********************************************************/

void tred2(double * const * const a, double * const d, double * const e, 
	   const int& n)
{
    int		i, j, k, l;
    double	h, scale,
	f, g, hh;
    
    for (i = n - 1; i >= 1; i--) { 
	
	l = i - 1; 
	h = scale = 0.0;
	
	if (l > 0) {
	    
	    for (k = 0; k <= l; k++) 
		scale += fabs(a[i][k]);
	    
	    if (scale == 0.0) {
		
		e[i] = a[i][l];
		
	    } else {
		
		for (k = 0; k <= l; k++) {
		    a[i][k] /= scale;
		    h += a[i][k] * a[i][k];
		}
		
		f = a[i][l];
		g = ((f > 0.0) ? (-sqrt(h)) : sqrt(h)); 
		e[i] = scale * g;
		h -= f * g;
		a[i][l] = f - g;
		
		for (f = 0.0, j = 0; j <= l; j++) {
		    
		    a[j][i] = a[i][j] / h;
		    
		    for (g = 0.0, k = 0; k <= j; k++)
			g += a[j][k] * a[i][k];
		    
		    for (k = j + 1; k <= l; k++)
			g += a[k][j] * a[i][k];
		    
		    e[j] = g / h;
		    f += e[j] * a[i][j];
		    
		}
		
		for (hh = f / (h + h), j = 0; j <= l; j++) {
		    
		    f = a[i][j];
		    g = e[j] - hh * f;
		    e[j] = g;
		    
		    for (k = 0; k <= j; k++)
			a[j][k] += - f * e[k] - g * a[i][k];
		    
		}
		
	    }
	    
	} else {
	    
	    e[i] = a[i][l];
	    
	}
	
	d[i] = h;
	
    }
    
    e[0] = d[0] = 0.0;
    
    for (i = 0; i < n; i++) {
	
	l = i - 1;
	
	if (d[i] != 0.0) {
	    
	    for (j = 0; j <= l; j++) {
		
		for (g = 0.0, k = 0; k <= l; k++)
		    g+= a[i][k] * a[k][j];
		
		for (k = 0; k <= l; k++)
		    a[k][j] -= g * a[k][i];
		
	    }
	}
	
	d[i] = a[i][i];
	a[i][i] = 1.0;
	
	for (j = 0; j <= l; j++)
	    a[i][j] = a[j][i] = 0.0;
    }
    
    return;
}

/************************************************************
**	tqli computes eigenvalues and eigenvectors of
**	a real symmetric matrix.
**
**	d[] and e[] come from tred2, z[][] input contains the
**	results stored in a[][] that came from tred2, and
**	returns the eigenvectors of what was in a[][] in input
**	to tred2.
************************************************************/

int
tqli(double * const d, double * const e, double * const * const z, 
     const int& n)
{
    int		i, k, l, m;
    int		itter, iflag, iflag2;
    double	dd, r, g, s, c, p, f, b;
    
    for (i = 1; i < n; i++) 
	e[i-1] = e[i];
    
    e[n-1] = 0.0;
    
    for (l = 0; l < n; l++) {
	
	itter = 0;
	
	do {
	    
	    iflag2 = 0;
	    
	    for (iflag = 0, m = l; m < n - 1; m++) {
		
		dd = fabs(d[m]) + fabs(d[m + 1]);
		
		if (fabs(e[m]) + dd == dd) {
		    iflag = 1; break;
		}
		
	    }
	    
	    if (!iflag)
		m = n - 1;
	    
	    if (m != l) {
		
		if (itter >= MAX_ITTER) {
		    // #ifdef __COMPLAIN_ABOUT_ITTER__
		    // cerr << form("itter = %d > MAX_ITTER = %d in tqli",
		    // 	itter, MAX_ITTER) << endl;
		    //#endif
		    return 0;
		}
		
		itter++;
		
		g = (d[l + 1] - d[l]) / (2.0 * e[l]);
		r = sqrt(g * g + 1.0);
		g = d[m] - d[l] + e[l] / (g + ((g > 0.0) ? r : (-r)));
		s = c = 1.0; p = 0.0;
		
		for (i = m - 1; i >= l; i--) {
		    
		    f = s * e[i]; b = c * e[i];
		    
		    if (fabs(f) >= fabs(g)) {
			
			c = g / f;
			r = sqrt(c * c + 1.0);
			e[i + 1] = f * r;
			s = 1.0 / r;
			c *= s;
			
		    } else {
			
			s = f / g;
			r = sqrt(s * s + 1.0);
			e[i + 1] = g * r; 
			c = 1.0 / r;
			s *= c;
			
		    }
		    
		    g = d[i + 1] - p;
		    r = (d[i] - g) * s + 2.0 * c * b;
		    p = s * r;
		    d[i + 1] = g + p;
		    g = c * r - b;
		    
		    for (k = 0; k < n; k++) {
			
			f = z[k][i + 1];
			z[k][i + 1] = s * z[k][i] + c * f;
			z[k][i] = c * z[k][i] - s * f;
			
		    }
		    
		}
		
		d[l] -= p; e[l] = g; e[m] = 0.0;
		iflag2 = 1;
	    } 
	    
	} while (iflag2);
	
    }
    return 1;
}

/*************************************************************************
**
**	Given a[.][.] an n x n matrix, compute e[.], an array of eigenvalues
**	of length n, and v[.][.], an array of vectors n x n containing the
**	eigenvectors (backwards by some conventions).
**
*************************************************************************/

void Eigen(double * const * const a, double * const e, 
	   double * const * const v, const int& n)
{
    double** c;
    double* c1;
    double* c2;
    int i, j;
    
    c = new double*[n];
    c1 = new double[n];
    c2 = new double[n];
    for (i = 0; i < n; i++) {
	c[i] = new double[n];
	for (j = 0; j < n; j++) {
	    c[i][j] = a[i][j];
	}
    }
    
    tred2(c, c1, c2, n);
    if(!tqli(c1, c2, c, n))
    {assert(0);}
    eigsrt(c1, c, n);
    
    for (i = 0; i < n; i++) {
	e[i] = c1[i];
	for (j = 0; j < n; j++) {
	    v[j][i] = c[i][j];
	}
    }
    
    for (i = 0; i < n; i++)
	delete[] c[i];
    delete[] c;
    delete[] c1;
    delete[] c2;
    
}

static void Eigen2(double * const * const a, double * const e,
		   double * const * const v, const int& n)
{
    double** c;
    double* c1;
    double* c2;
    int i, j;
    
    c = new double*[n];
    c1 = new double[n];
    c2 = new double[n];
    for (i = 0; i < n; i++) {
	c[i] = new double[n];
	for (j = 0; j < n; j++) {
	    c[i][j] = a[i][j];
	}
    }
    
    tred2(c, c1, c2, n);
    if(!tqli(c1, c2, c, n))
    {  assert(0);	  }
    eigsrt2(c1, c, n);
    
    for (i = 0; i < n; i++) {
	e[i] = c1[i];
	for (j = 0; j < n; j++) {
	    v[j][i] = c[j][i];
	}
    }
    
    for (i = 0; i < n; i++)
	delete[] c[i];
    delete[] c;
    delete[] c1;
    delete[] c2;
    
}

/*******************************************************************
**
**	Svd returns u(mxn), s(nxn) diagonal, v(nxn) orthogonal, 
**	with uTu = 1 (nxn) given a(mxn).
**
**	Corresponds to matlab's SVD(x, 0).
**
*******************************************************************/

void Svd(double * const * const a, double * const * const u, 
	 double * const * const s, double * const * const v,
	 const int& m, const int& n)
{
    int i, j, k;
    
    double** c = new double*[n];
    for (i = 0; i < n; i++) {
	c[i] = new double[n];
    }
    
    for (i = 0; i < n; i++) {
	for (j = i; j < n; j++) {
	    for (c[i][j] = 0.0, k = 0; k < m; k++) {
		c[i][j] += a[k][i] * a[k][j];
	    }
	    c[j][i] = c[i][j];
	}
    }
    
    double* sd = new double[n];
    Eigen2(c, sd, v, n);
    for (i = 0; i < n; i++) {
	for (j = 0; j < n; j++) {
	    s[i][j] = 0.0;
	}
	s[i][i] = ((sd[i] >= 0.0) ? sqrt(sd[i]) : 0.0);
    }
    
    for (i = 0; i < n; i++) {
	sd[i] = ((s[i][i] == 0.0) ? 0.0 : 1.0 / s[i][i]);
    }
    
    for (i = 0; i < m; i++) {
	for (j = 0; j < n; j++) {
	    for (u[i][j] = 0.0, k = 0; k < n; k++) {
		u[i][j] += a[i][k] * v[k][j] * sd[j];
	    }
	}
    }
    
    for (i = 0; i < n; i++) delete[] c[i];
    delete[] sd, c;
}



/**************************************************************************
**  The following routines compute least squares estimates of poses or
**  rotation matrices from corresponding points or from arrays of
**	(possibly affine) matrices.
**************************************************************************/


int 
lstSqAlign(const Vector x[], const Vector y[], const int& n, 
	   Matrix& R, Vector& d)
{
    double Hi[3][3];
    double * H[3] = {Hi[0], Hi[1], Hi[2]};
    
    double Ui[3][3];
    double * U[3] = {Ui[0], Ui[1], Ui[2]};
    
    double Si[3][3];
    double * S[3] = {Si[0], Si[1], Si[2]};
    
    double Vi[3][3];
    double * V[3] = {Vi[0], Vi[1], Vi[2]};
    
    int i, j, k;
    
    Vector *x0 = new Vector[n];
    Vector xM;
    Vector *y0 = new Vector[n];
    Vector yM;
    
    for (yM = xM = Vector(0.0, 0.0, 0.0), i = 0; i < n; i++) {
	xM += x[i];
	yM += y[i];
    }
    xM /= (double)n;
    yM /= (double)n;
    
    for (i = 0; i < n; i++) {
	x0[i] = x[i] - xM;
	y0[i] = y[i] - yM;
    }
    
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    for (H[i][j] = 0.0, k = 0; k < n; k++) {
		H[i][j] += x0[k][i] * y0[k][j];
	    }
	}
    }
    
    
    Svd(H, U, S, V, 3, 3);
    
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    for (R[i][j] = 0.0, k = 0; k < 3; k++) {
		R[i][j] += V[i][k] * U[j][k];
	    }
	}
    }
    
    d = yM - R / xM;
    
    delete[] x0;
    delete[] y0;
    
    return 1;
}

