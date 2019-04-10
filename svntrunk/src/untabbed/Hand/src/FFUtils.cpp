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
 
#include <stream.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <FFUtils.hpp>


#define EPS 1.0e-6




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
  int i, j;

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

// How much, for matrix A, the product A*A' may differ from the identity
// matrix for A to be acceptable as orthogonal
const double Orthogonality_Epsilon = 4e-2;

// This threshold determines which of three alternative ways is used to solve
// the expression R = R(theta, phi, psi) for theta, phi, psi.
const double RotationAngle_Epsilon = 1e-3;

#ifndef DBL_SNAN
#define DBL_SNAN HUGE_VAL
#endif

const double RobustSqrt_Epsilon = 1e-6;
inline double robust_sqrt(const double x) {
  if (x >= 0) return sqrt(x);
  else if (x >= - RobustSqrt_Epsilon) return 0;
  else return DBL_SNAN;    // from float.h
}

//----------------------------------------------------------------------
// Calculate the rotation matrix that rotates through an axis
// specified by theta and phi, about angle psi.
//
// Axis:  x = sin(theta) * cos(phi)
//        y = sin(theta) * sin(phi)
//        z = cos(theta)
//
// Matrix: R = [ txx + c          txy + sz      txz - sy ]
//             [ txy - sz         tyy + c       tyz + sx ]
//             [ txz + sy         tzy - sx      tzz + c  ]
//
// where s=sin(psi),  c=cos(psi),   t=1-cos(psi)
// See: Michael E. Pique, "Rotation Tools" in
// Graphics Gems, Vol. 1  (p. 466), Academic Press Inc. San Diego, 1990
//----------------------------------------------------------------------
void genMatrix(double theta, double phi, double psi, Matrix& T)
{
   // These conditions are of course not necessary, due to the periodicity
   // of angles. But since decodeMatrix() only outputs angles that obey these
   // conditions, any violation would be indicative of a bug...
  //   assert ((theta >= 0)     && (theta<= M_PI) &&
  //         (phi   >= -M_PI) && (phi  <= M_PI) &&
  //         (psi   >= 0)     && (psi  <= M_PI));

   double x = sin(theta) * cos(phi);
   double y = sin(theta) * sin(phi);
   double z = cos(theta);
   double s = sin(psi);
   double c = cos(psi);
   double t = 1-c;
   T[0][0]=  t*x*x + c;     T[0][1]=  t*x*y + s*z;   T[0][2]= t*x*z - s*y;
   T[1][0]=  t*x*y - s*z;   T[1][1]=  t*y*y + c;     T[1][2]= t*y*z + s*x;
   T[2][0]=  t*x*z + s*y;   T[2][1]=  t*y*z - s*x;   T[2][2]= t*z*z + c;
}

//----------------------------------------------------------------------
//  The inverse of genMatrix: given matrix T, find axis (theta, phi)
//  and angle psi.
//----------------------------------------------------------------------
void decodeMatrix(const Matrix& T, double& theta, double& phi, double& psi)
{
  // check if T is orthogonal
  for (int i=0; i<3; i++)
    for (int j=0; j<3; j++) {
      double s=0;
      for (int k=0; k<3; k++)
        s += T[i][k]*T[j][k];
      if (fabs(s - ((i==j) ? 1 : 0)) > Orthogonality_Epsilon) {
        cerr << "decodeMatrix: Matrix not orthogonal (1)\n" << T << endl;
        assert(0);
      }
    }

  // check if T is a proper rotation
  if (Det(T)<0) {
//    cerr << form("decodeMatrix: Not a proper rotation - det(T)=%g\n", Det(T)) << endl;
    cerr << "decodeMatrix: Not a proper rotation - det(T)=" << Det(T) << endl;
    assert(0);
  }

  // Use the relation Tr(R) = 2 cos(psi) + 1 to solve for psi:
  double trmo = T[0][0]+T[1][1]+T[2][2] - 1;
  if (fabs(trmo) > 2 + Orthogonality_Epsilon) {
//    cerr << form("decodeMatrix: Trace of matrix is out range - Tr(T)=%g\n", trmo+1);
    cerr << "decodeMatrix: Trace of matrix is out range - Tr(T)=" << trmo+1 << endl;
    cerr << T << endl;
    assert(0);
  }
  if (fabs(trmo) <= 2)
    // acos returns values in the range [0, pi]
    psi = acos( trmo/2 );
  else
    // this takes care of the cases where trmo is between 2 and 2+epsilon,
    // which can sometime happen due to round-off errors
    psi = (trmo>0) ? 0 : M_PI;

  //----------------------------------------------------------------------
  // Now find the axis, i.e. theta and phi. Use one of three alternative
  // ways to solve the expression R = R(theta, phi, psi) (see above)
  // for theta, phi, psi.
  //----------------------------------------------------------------------
  double s = sin(psi);
  if (s > RotationAngle_Epsilon) {
    //----------------------------------------------------------------------
    // 1.) This is the "regular" case: a rotation about an angle psi different
    // from zero and pi. The following can be immediately read off from the
    // equation for R, given theta, phi, psi (see above, genMatrix(..))
    double x = (T[1][2] - T[2][1]) / (2*s);
    double y = (T[2][0] - T[0][2]) / (2*s);
    double z = (T[0][1] - T[1][0]) / (2*s);
    double aa = fabs(x*x+y*y+z*z-1.0);
    if (aa > Orthogonality_Epsilon) {
      cerr << "decodeMatrix: Matrix not orthogonal (2)\n" << aa << "\n" << T << endl;
      assert(0);
    }

    theta = atan2(hypot(x,y), z);
    phi   = atan2(y,x);

  } else if (psi < M_PI/2) {
    //----------------------------------------------------------------------
    // 2.) Together with the above if-condition, this means that
    // psi is close to zero: the axis is not defined.
    // Make an arbitray choice....
    phi = 0;
    theta = 0;

  } else {
    //----------------------------------------------------------------------
    // 3.) Come here if psi close to pi. If psi==pi then s=0, c=-1, t=2,
    // (see comments for genMatrix()) and the matrix R has the form
    //
    // R = [ 2xx - 1          2xy           2xz      ]
    //     [ 2xy              2yy - 1       2yz      ]
    //     [ 2xz              2zy           2zz - 1  ]
    //
    // Get x, y, z from diagonal elements, up to sign:
    double x = robust_sqrt( (T[0][0]+1)*0.5 );
    double y = robust_sqrt( (T[1][1]+1)*0.5 );
    double z = robust_sqrt( (T[2][2]+1)*0.5 );

    // Determine the signs from the two largest off-diagonal elements. This
    // is convoluted stuff.  Why couldn't we just take quaternions, they have
    // much less singularities...
    char smallest = 'x';
    double smallestvalue = fabs(T[1][2]);
    if (fabs(T[0][2])<smallestvalue) { smallestvalue=fabs(T[0][2]); smallest='y'; }
    if (fabs(T[0][1])<smallestvalue) { smallestvalue=fabs(T[0][1]); smallest='z'; }

    // The overall sign of the axis is undetermined: rotation about (x,y,z) about pi
    // is the same as about (-x,-y,-z). So we are free choose either x, y, or z to be
    // positive, and adjust the other two.

    switch(smallest) {
    case 'x' : if (T[0][2]<0) z=-z;   // T[0][2] is 2xz, and x is positive
               if (T[0][1]<0) y=-y;   // T[0][1] is 2xy,     "
         break;
    case 'y':  if (T[1][2]<0) z=-z;   // T[1][2] is 2yz, and y is positive
               if (T[0][1]<0) x=-x;   // T[0][1] is 2xy,     "
         break;
    case 'z':  if (T[1][2]<0) y=-y;   // T[1][2] is 2yz, and z is positive
               if (T[0][2]<0) x=-x;   // T[0][2] is 2xz,     "
         break;
    default: assert(0);
    }

    theta = atan2(hypot(x,y), z);
    phi   = atan2(y,x);

  }
  assert (finite(theta) && finite(phi) && finite(psi));
}
//------------------------------------------------------------
//
void computeAfineFromSkew(const Vector v1[3], const Vector v2[3], Matrix& R)
{
  Vector V1[3];

  double D = (v1[0] * v1[1]) / v1[2];

  V1[0] = v1[1] * v1[2] / D;
  V1[1] = v1[2] * v1[0] / D;
  V1[2] = v1[0] * v1[1] / D;

  int i, j;

  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      R[i][j] = 0.0;

  R = Matrix(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

  for (i = 0; i < 3; i++)
    R += v2[i] & V1[i];

  return;
}


int ComputeTransform(
  const Vector & To1, const Vector & To2, const Vector & To3,
  const Vector & From1, const Vector & From2, const Vector & From3,
  Matrix & R, Vector & T)
{

  Vector x[3] = { To1, To2, To3};
  Vector y[3] = { From1, From2, From3};
  return lstSqAlign(x, y, 3, R, T);
}

int ConstructDihedral(
  const Vector & V1, const Vector & D1, const Vector & F1,
  const Vector & V2, const Vector & D2, const Vector & F2,
  const double& phi,
  Matrix & R, Vector & T)
{

  double M;

  Vector l1 = D1 - V1;
  M = mag(l1);
  if (M == 0.0)
    assert( 0);
  l1 = l1 / M;

  Vector m1 = F1 - V1; m1 = m1 - (l1 / m1) * l1;
  M = mag(m1);
  if (M == 0.0)
    assert(0);
  m1 = m1 / M;

  Vector n1 = l1 * m1;

  Vector l2 = V2 - D2;
  M = mag(l2);
  if (M == 0.0)
    assert(0);
  l2 = l2 / M;

  Vector m2 = F2 - V2; m2 = m2 - (l2 / m2) * l2;
  M = mag(m2);
  if (M == 0.0)
    assert(0);
  m2 = m2 / M;
  Vector n2 = l2 * m2;

  R = (l1 & l2) + ((m1 * cos(phi) - n1 * sin(phi)) & m2) +
    ((m1 * sin(phi) + n1 * cos(phi)) & n2);

  T = (V1 + D1) / 2.0 - (R / (V2 + D2)) / 2.0; // This is least squares sol'n.

  return 1;
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

          cerr << "itter = " << itter << " > MAX_ITTER = " << MAX_ITTER << " in tqli" << endl;
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

  int i, j, k, l;

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

/*
** All variables in following are identical to previous except error weighting
** is applied for estimation of best fit.
**
** dy[i] is error (spherical distribution) about y[i].
*/


int 
lstSqAlign(const Vector x[], const Vector y[], const double dy[], const int& n,
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

  Vector * x0 = new Vector[n];
  Vector xM;
  Vector * y0 = new Vector[n];
  Vector yM;
  double w;

  for (w = 0.0, yM = xM = Vector(0.0, 0.0, 0.0), i = 0; i < n; i++) {
    w += 1.0 / (dy[i] * dy[i]);
    xM += x[i] / (dy[i] * dy[i]);
    yM += y[i] / (dy[i] * dy[i]);
  }
  xM /= w;
  yM /= w;
  for (i = 0; i < n; i++) {
    x0[i] = x[i] - xM;
    y0[i] = y[i] - yM;
  }
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      for (H[i][j] = 0.0, k = 0; k < n; k++) {
        H[i][j] += x0[k][i] * y0[k][j] / (dy[k] * dy[k]);
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

/*
** All of this is identical to the preceding except that variance and
** covariance estimates are returned.
**
** dR[i1][j1][i2][j2] = cov(R[i1][j1], R[i2][j2])
** dd[i1][i2] = cov(d[i1], d[i2])
** dRd[i1][j1][i2] = cov(R[i1][j1], d[i2]).
*/

int 
lstSqAlign(const Vector x[], const Vector y[], const double dy[], const int& n,
Matrix& R, Vector& d,
double dR[3][3][3][3], double dd[3][3], double dRd[3][3][3])
{
  int i, j, k;
  int i1, j1, k1;

  // Error computation matrices
  static double c_[12][12];
  static double C_[12];
  static double dphi[12][12];
  static double Dphi[12][12];
  static double c_1_[12];
  static double c_2_[12];

    double Hi[3][3];
    double * H[3] = {Hi[0], Hi[1], Hi[2]};
    double Ui[3][3];
    double * U[3] = {Ui[0], Ui[1], Ui[2]};
    double Si[3][3];
    double * S[3] = {Si[0], Si[1], Si[2]};
    double Vi[3][3];
    double * V[3] = {Vi[0], Vi[1], Vi[2]};

  // Space to compute diagonalized error matrix
  static double* c__[12] = 
  {
    c_[0], c_[1], c_[2], c_[3], c_[4], c_[5], 
    c_[6], c_[7], c_[8], c_[9], c_[10], c_[11]  
  };
  static double* c_1 = c_1_;
  static double* c_2 = c_2_;


  /*
  ** Compute basis moments.
  */

  Vector Xi[3];
  Vector Xip[3];
  Vector Eta[3];
  Vector Lambda[3];
  Vector xM, yM;
  Vector * x0 = new Vector[n];
  Vector * y0 = new Vector[n];
  double w;

  for (i = 0; i < 3; i++) {
    Xi[i] = Eta[i] = Vector(0.0, 0.0, 0.0);
  }

  for (w = 0.0, xM = yM = Vector(0.0, 0.0, 0.0), k = 0; k < n; k++) {
    for (i = 0; i < 3; i++) {
      for (j = 0; j < 3; j++) {
        Xi[i][j] += x[k][i] * x[k][j] / dy[k] / dy[k];
        Eta[i][j] += x[k][i] * y[k][j] / dy[k] / dy[k];
      }
      xM[i] += x[k][i] / dy[k] / dy[k];
      yM[i] += y[k][i] / dy[k] / dy[k];
    }
    w += 1.0 / (dy[k] * dy[k]);
  }

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      Xi[i][j] -= xM[i] * xM[j] / w;
      Eta[i][j] -= xM[i] * yM[j] / w;
    }
  }
  xM /= w;
  yM /= w;
  for (i = 0; i < n; i++) {
    x0[i] = x[i] - xM;
    y0[i] = y[i] - yM;
  }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            for (H[i][j] = 0.0, k = 0; k < n; k++) {
                H[i][j] += x0[k][i] * y0[k][j] / (dy[k] * dy[k]);
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

  for (i = 0; i < 3; i++) {
    Lambda[i] = Xi[i] - (!R) / Eta[i];
  }

  /*
  ** Compute the error quadratic form matrix.
  */

  for (j = 0; j < 3; j++) {
    for (k = 0; k < 3; k++) {
      for (j1 = 0; j1 < 3; j1++) {
        for (k1 = 0; k1 < 3; k1++) {
          c_[3 * j + k][3 * j1 + k1] = 0.0;
          if (j == j1) {
            for (i = 0; i < n; i++) {
              c_[3 * j + k][3 * j1 + k1] +=
                x[i][k] * x[i][k1] / dy[i] / dy[i];
            }
            c_[3 * j + k][3 * j1 + k1] -= Lambda[k][k1];
          }
        }
      }
    }
  }

  for (j = 0; j < 3; j++) {
    for (k = 0; k < 3; k++) {
      for (j1 = 0; j1 < 3; j1++) {
        c_[9 + j1][3 * j + k] = c_[3 * j + k][9 + j1] = 0.0;
        if (j == j1) {
          for (i = 0; i < n; i++) {
            c_[9 + j1][3 * j + k] += x[i][k] / dy[i] / dy[i];
            c_[3 * j + k][9 + j1] += x[i][k] / dy[i] / dy[i];
          }
        }
      }
    }
  }

  for (j = 0; j < 3; j++) {
    for (j1 = 0; j1 < 3; j1++) {
      c_[9 + j][9 + j1] = 0.0;
      if (j == j1) {
        for (i = 0; i < n; i++) {
          c_[9 + j][9 + j1] += 1.0 / dy[i] / dy[i];
        }
      }
    }
  }

  /*
  ** Diagonalize quadratic form matrix
  */

  for (i = 0; i < 12; i++) {
    for (j = 0; j < 12; j++) {
      c__[i][j] = c_[i][j];
    }
  }
  tred2(c__, c_1, c_2, 12);
  if(!tqli(c_1, c_2, c__, 12))
    {assert (0);}
  eigsrt(c_1, c__, 12);

  for (i = 0; i < 12; i++) {
    C_[i] = c_1[i];
    for (j = 0; j < 12; j++) {
      dphi[j][i] = c__[i][j];
    }
  }


  /*
  ** Compute covariances
  */

  for (k = 0; k < 12; k++) {
    for (k1 = 0; k1 < 12; k1++) {
      Dphi[k][k1] = 0;
      for (j = 0; j < 12; j++) {
        if (fabs(C_[j]) > EPS) {
          Dphi[k][k1] += dphi[j][k] * dphi[j][k1] / C_[j];
        }
      }
    }
  }  

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      for (i1 = 0; i1 < 3; i1++) {
        for (j1 = 0; j1 < 3; j1++) {
          dR[i][j][i1][j1] = Dphi[3 * i + j][3 * i1 + j1] / (double)n;
        }
      }
    }
  }
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      for (i1 = 0; i1 < 3; i1++) {
        dRd[i][j][i1] = Dphi[3 * i + j][9 + i1] / (double)n;
      }
    }
  }

  for (i = 0; i < 3; i++) {
    for (i1 = 0; i1 < 3; i1++) {
      dd[i][i1] = Dphi[9 + i][9 + i1] / (double)n;
    }
  }

  return 1;
}


/*
** a[k][i][j] = R[i][j] for the k'th estimate to be ``averaged.''
** This should have dimensions of a[n][3][3].
** w[k] = weight of k'th estimate to be ``averaged.''
** n = number of estimates to be averaged over.
** R[i][j] = result of average over a[k][i][j]'s.
*/

int 
lstSqAverage(const Matrix* const a, const double* const w, const int& n,
Matrix& R)
{
  int i, j, k;

    double Hi[3][3];
    double * H[3] = {Hi[0], Hi[1], Hi[2]};
    double Ui[3][3];
    double * U[3] = {Ui[0], Ui[1], Ui[2]};
    double Si[3][3];
    double * S[3] = {Si[0], Si[1], Si[2]};
    double Vi[3][3];
    double * V[3] = {Vi[0], Vi[1], Vi[2]};

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      for (H[i][j] = 0.0, k = 0; k < n; k++) {
        H[i][j] += w[k] * a[k][j][i];
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

  return 1;
}



