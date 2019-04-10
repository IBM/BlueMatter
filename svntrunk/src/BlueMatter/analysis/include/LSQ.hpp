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
 #ifndef __LSQ_HPP__
#define __LSQ_HPP__

#include <iostream>
#include <cstdlib>
#include <math.h>
#include <assert.h>

using namespace std;

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
	if (i < 0 || i >= 3) {
	    // cerr << form("index = %d out of range.", i) << endl;
	    assert(0);
	    return component[0];
	} else {
	    return component[i];
	}
    };
    const double& operator[](int i) const 
    {
	if (i < 0 || i >= 3) {
	    // cerr << form("index = %d out of range.", i) << endl;
	    assert(0);
	    return component[0];
	} else {
	    return component[i];
	}
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
	if (i < 0 || i >= 3) {
	    // cerr << form("index = %d out of range.", i) << endl;
	    assert(0);
	    return component[0];
	} else {
	    return component[i];
	}
    };
    
    const Vector& operator[](int i) const 
    {
	if (i < 0 || i >= 3) {
	    // cerr << form("index = %d out of range.", i) << endl;
	    assert(0);
	    return component[0];
	} else {
	    return component[i];
	}
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
// Was 'const Matrix I', which got confused with I from 'complex.h'
const Matrix Identity = (X&X) + (Y&Y) + (Z&Z);



// -------------------------------------------------------------

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


// ---------------------------------------------

void ludcmp(double * const * const a, int * const indx, double& d,
	    const int& n);
void lubksb(double * const * const a, int * const indx, double * const b, 
	    const int& n);
void matinv(double * const * const a, double * const * const y, 
	    double& d, const int& n);


#define EPS 1.0e-6


int 
lstSqAlign(const Vector x[], const Vector y[], const int& n, Matrix& R, Vector& d);


#endif
