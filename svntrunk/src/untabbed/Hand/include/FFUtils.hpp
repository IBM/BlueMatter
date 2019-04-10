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
 
#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include <stream.h>
#include <iostream>
#include <math.h>
#include <assert.h>

class MVError {
  private:
    int index;
  public:
    MVError(int indx, char* amsg):index(indx) {
      cerr << amsg << endl;
      cerr << "Out of range: " << index << endl;
    };
};

class Matrix;

class Vector {
  
  public:

    Vector();
    Vector(const double&, const double&, const double&);
    Vector(const Vector&);

    // vector addition
    friend class Vector operator+(const Vector&, const Vector&);
    // unary +
    friend class Vector operator+(const Vector&);
    // vector subtraction
    friend class Vector operator-(const Vector&, const Vector&);
    // unary -
    friend class Vector operator-(const Vector&); 
    // cross product
    friend class Vector operator*(const Vector&, const Vector&); 
    // dot product
    friend double operator/(const Vector&, const Vector&);
    // scalar-vector product
    friend class Vector operator*(const double&, const Vector&);
    // scalar-vector product
    friend class Vector operator*(const Vector&, const double&);
    // scalar-vector product
    friend class Vector operator/(const Vector&, const double&);

    // Assignment
    const Vector& operator=(const Vector&);
    // Accumulate
    const Vector& operator+=(const Vector&);
    // Accumulate neg.
    const Vector& operator-=(const Vector&);
    // Accumulate prod.
    const Vector& operator*=(const double&);
    // Accumulate prod.
    const Vector& operator/=(const double&);

    // outer product
    friend class Matrix operator&(const Vector&, const Vector&);
    // inner product
    friend class Vector operator/(const Matrix&, const Vector&);
    // inner product
    friend class Vector operator/(const Vector&, const Matrix&);

    friend class Matrix;

    // Matrix Addition
    friend class Matrix operator+(const Matrix&, const Matrix&);
    // Unary +
    friend class Matrix operator+(const Matrix&);
    // Matrix Subtraction
    friend class Matrix operator-(const Matrix&, const Matrix&);
    // Unary -
    friend class Matrix operator-(const Matrix&); 
    friend class Matrix operator/(const Matrix&, const Matrix&); 
    // Scalar-matrix product
    friend class Matrix operator*(const Matrix&, const double&); 
    // Scalar-matrix product
    friend class Matrix operator*(const double&, const Matrix&); 
  
    friend class Matrix VX1(const Vector&);

    double& operator[](int i) 
    {
// 			if (i < 0 || i >= 3) {
// 			  cerr << "index = " << i << " out of range." << endl;
// 				assert(0);
// 				return component[0];
// 			} else {
        return component[i];
        //			}
    };
    const double& operator[](int i) const 
    {
// 			if (i < 0 || i >= 3) {
// 			  cerr << "index = " << i << " out of range." << endl;
// 				assert(0);
// 				return component[0];
// 			} else {
        return component[i];
        //			}
    };

  private:

    double component[3];

};

class Matrix {

  public:

    Matrix();
    Matrix(const double&, const double&, const double&,
      const double&, const double&, const double&,
      const double&, const double&, const double&);
    Matrix(const Matrix&);

    // Matrix Addition
    friend class Matrix operator+(const Matrix&, const Matrix&);
    // Unary +
    friend class Matrix operator+(const Matrix&);
    // Matrix Subtraction
    friend class Matrix operator-(const Matrix&, const Matrix&);
    // Unary -
    friend class Matrix operator-(const Matrix&); 
    // outer product
    friend class Matrix operator&(const Vector&, const Vector&); 
    // inner product
    friend class Vector operator/(const Matrix&, const Vector&); 
    // inner product
    friend class Vector operator/(const Vector&, const Matrix&); 
    // inner product
    friend class Matrix operator/(const Matrix&, const Matrix&); 
    // Scalar-matrix product
    friend class Matrix operator*(const Matrix&, const double&); 
    // Scalar-matrix product
    friend class Matrix operator/(const Matrix&, const double&); 
    // Scalar-matrix product
    friend class Matrix operator*(const double&, const Matrix&); 

    // Transpose
    Matrix operator!()const;

    // Trace
    friend double Tr(const Matrix& m);

    // Determinant
    friend double Det(const Matrix& m);

    // Assignment
    const Matrix& operator=(const Matrix& m2);
    // Accumulate
    const Matrix& operator+=(const Matrix& m2);
    // neg Accumulate
    const Matrix& operator-=(const Matrix& m2);
    // Accumulate
    const Matrix& operator/=(const double& m2);
    // Accumulate
    const Matrix& operator*=(const double& m2);

    friend class Matrix VX1(const Vector&);

    friend class Vector;

    Vector& operator[](int i) {
// 			if (i < 0 || i >= 3) {
// 			  cerr << "index = " << i << " out of range." << endl;
// 				assert(0);
// 				return component[0];
// 			} else {
        return component[i];
//			}
    };

    const Vector& operator[](int i) const 
    {
// 			if (i < 0 || i >= 3) {
// 			  cerr << "index = " << i << " out of range." << endl;
// 				assert(0);
// 				return component[0];
// 			} else {
        return component[i];
// 			}
    };

    void Diag(double[3], Vector[3]) const;

  private:
    Vector component[3];
};

inline Vector operator+(const Vector& v1, const Vector& v2)
{
  return Vector(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
}

inline Vector operator+(const Vector& v)
{
  return Vector(v[0], v[1], v[2]);
}

inline Vector operator-(const Vector& v1, const Vector& v2)
{
  return Vector(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]);
}

inline Vector operator-(const Vector& v)
{
  return Vector(-v[0], -v[1], -v[2]);
}

inline Vector operator*(const Vector& v1, const Vector& v2)
{
  return Vector(
    v1[1] * v2[2] - v1[2] * v2[1],
    v1[2] * v2[0] - v1[0] * v2[2],
    v1[0] * v2[1] - v1[1] * v2[0]
  );
}

inline Vector operator*(const Vector& v, const double& d)
{
  return Vector(d * v[0], d * v[1], d * v[2]);
}

inline Vector operator/(const Vector& v, const double& d)
{
  return Vector(v[0] / d, v[1] / d, v[2] / d);
}

inline Vector operator*(const double& d, const Vector& v)
{
  return Vector(d * v[0], d * v[1], d * v[2]);
}

inline double operator/(const Vector& v1, const Vector& v2)
{
  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

inline const Vector& Vector::operator=(const Vector& v2)
{
  (*this)[0] = v2[0]; (*this)[1] = v2[1]; (*this)[2] = v2[2];
  return *this;
}

inline const Vector& Vector::operator+=(const Vector& v2)
{
  (*this)[0] += v2[0]; (*this)[1] += v2[1]; (*this)[2] += v2[2];
  return *this;
}

inline const Vector& Vector::operator*=(const double& d)
{
  (*this)[0] *= d; (*this)[1] *= d; (*this)[2] *= d;
  return *this;
}

inline const Vector& Vector::operator/=(const double& d)
{
  (*this)[0] /= d; (*this)[1] /= d; (*this)[2] /= d;
  return *this;
}

inline const Vector& Vector::operator-=(const Vector& v2)
{
  (*this)[0] -= v2[0]; (*this)[1] -= v2[1]; (*this)[2] -= v2[2];
  return *this;
}


inline double mag(const Vector& v)
{
  return sqrt(v/v);
}

inline Matrix VX1(const Vector& v) 
{
  return Matrix(
    0.0, - v[2], v[1],
    v[2], 0.0, - v[0],
    -v[1], v[0], 0.0
  );
}

ostream &operator << (ostream &s, const Matrix &m);

ostream &operator << (ostream &s, const Vector &v);

const Vector X(1.0, 0.0, 0.0);
const Vector Y(0.0, 1.0, 0.0);
const Vector Z(0.0, 0.0, 1.0);
const Matrix I = (X&X) + (Y&Y) + (Z&Z);

#endif




#ifndef __ROT_HPP__
#define __ROT_HPP__

void
genMatrix(double theta, double phi, double psi, Matrix& T);

void
decodeMatrix(const Matrix& T, double& theta, double& phi, double& psi);

// Compute a rotation R such that R/v1[j] = v2[j];

void
computeAfineFromSkew(const Vector v1[3], const Vector v2[3], Matrix& R);

//  Compute rotation R and Translation T such that To(n) = (R / From(n) ) + T

int
ComputeTransform(const Vector &To1, const Vector & To2, const Vector & To3,
  const Vector &From1, const Vector & From2, const Vector & From3,
  Matrix & R, Vector & T);

/***********************************************************************

Given six atom positions (Vectors) and dihedral angle phi:
   V1-D1  and   D2-V2              (V1,D2')-----(D1,V2')
  /                  \     ==>     /        phi        \
F1                    F2         F1                     F2'


give R, and T such that:
 D2' = (R / D2) + T = V1,
 V2' = (R / V2) + T = D1,
 F2' = (R / F2) + T, and
 dihedral angle(F1,V1,V2',F2') = phi;
 where positive phi means ((F1-V1) cross (V2'-V1)) dot (F2' - V2') > 0

************************************************************************/

int 
ConstructDihedral(
  const Vector & V1, const Vector & D1, const Vector & F1,
  const Vector & V2, const Vector & D2, const Vector & F2,
  const double& phi,
  Matrix & R, Vector & T);


#endif


#ifndef __DIAG_HPP__
#define __DIAG_HPP__

void eigsrt(double* const d, double * const * const v, const int& n);

void tred2(double * const * const a, double * const d, double * const e,
  const int& n);

int tqli(double * const d, double * const e, double * const * const z,
  const int& n);

void Eigen(double * const * const a, double* const e, double * const * const v,
  const int& n);

void Svd(double * const * const a, double * const * const u,
  double * const * const s, double * const * const v,
  const int& m, const int& n);

#endif



#ifndef __LSTSQROT_HPP__
#define __LSTSQROT_HPP__

/**************************************************************************
**  The following routines compute least squares estimates of poses or
**  rotation matrices from corresponding points or from arrays of
**	(possibly affine) matrices.
**************************************************************************/


int 
lstSqAlign(const Vector x[], const Vector y[], const int& n, Matrix& R, Vector& d);

/*
** All variables in following are identical to previous except error weighting
** is applied for estimation of best fit.
**
** dy[i] is error (spherical distribution) about y[i].
*/

int 
lstSqAlign(const Vector x[], const Vector y[], const double dy[],
  const int& n, Matrix& R, Vector& d);

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
Matrix& R, Vector& d, double dR[3][3][3][3], double dd[3][3],
double dRd[3][3][3]);


/*
** a[k][i][j] = R[i][j] for the k'th estimate to be ``averaged.''
** This should have dimensions of a[n][3][3].
** w[k] = weight of k'th estimate to be ``averaged.''
** n = number of estimates to be averaged over.
** R[i][j] = result of average over a[k][i][j]'s.
*/

int 
lstSqAverage(const Matrix* const a, const double* const w, const int& n,
Matrix& R);




class Torsion
{
  private:
  double d_rad;
  int d_valid;

  public:
  
  Torsion(Vector const &v1,Vector const &v2,Vector const &v3,Vector const &v4)
  {
    Vector n1,n2,m1,m2;
    double d;
    m1 = v1-v2;
    m2 = v3-v2;
    d = mag(m1);
    if(d == 0)
    {	d_valid = 0; assert(d_valid); }
    m1 = m1 / d;
    d = mag(m2);
    if(d == 0)
    {	d_valid = 0; assert(d_valid); }
    m2 = m2 / d;
    n1 = m1 * m2;
    m2 = -m2;
    m1 = v4-v3;
    d = mag(m1);
    if(d == 0)
    {	d_valid = 0; assert(d_valid); }
    m1 = m1 / d;
    n2 = m2 * m1;
    d = mag(n1);
    if(d == 0)
    {	d_valid = 0; assert(d_valid); }
    n1 = n1 / d;
    d = mag(n2);
    if(d == 0)
    {	d_valid = 0; assert(d_valid); }
    n2 = n2 / d;
    d = n1 / n2;
    d = acos(d);
    if((n1 / m1) < 0.0)
      d = -d;

    d_rad = d;
    d_valid = 1;
  }

  double Rad()const { assert(d_valid); return d_rad;}
  double Deg()const { assert(d_valid); return d_rad*180.0/M_PI;}

};


#endif
