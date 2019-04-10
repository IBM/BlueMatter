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
 #ifndef __ARRAY_HELPER_HPP__
#define __ARRAY_HELPER_HPP__

template<class type>
class t3D {

public:

    inline void Init( int sz_x, int sz_y, int sz_z, void *ptr = NULL );
    inline void Zero();

    inline type& operator()( int x, int y, int z );
    inline type& operator[]( int ind ) { assert( ind >= 0 && ind < size ); return array[ ind ]; }
    inline operator type *() const { return array; }
    inline t3D& operator=( const t3D& at3D );

protected:

    int szx, szy, szz;
    int size;

    type *array;
};

template<class type>
void t3D<type>
::Init( int sz_x, int sz_y, int sz_z, void *ptr )
{
    szx = sz_x; szy = sz_y; szz = sz_z;
    size = szx * szy * szz;
    if( ptr == NULL )
    {
      // array = new type[size];
      pkNew(&array, size, __FILE__, __LINE__);
        
        assert( array != NULL );
    }
    else
        array = static_cast<type*>(ptr);
}

template<class type>
void t3D<type>
::Zero()
{
    for( int i=0; i<size; i++ )
        array[i] = static_cast<type>(0);
}

template<class type>
type& t3D<type>
::operator()( int x, int y, int z )
{
    int ind = z + szz*(y + szy*x); // row-major format, as in C
    assert( ind >= 0 && ind < size );
    return array[ ind ];
}

template<class type>
t3D<type>&  t3D<type>
::operator=( const t3D<type>& at3D )
{
    assert( size == at3D.size );

    for( int i=0; i<size; i++ )
        (*this)[i] = at3D[i];

    return *this;
}

//------------------


template<class type>
class t1D {

public:

    inline void Init( int sz_x, void *ptr = NULL );
    inline void Zero();

    inline type& operator()( int x );
    inline type& operator[]( int ind ) { return assert( ind >= 0 && ind < size ); indarray[ind]; }
    inline operator type *() const { return array; }
    inline t1D& operator=( const t1D& at1D );

protected:

    int size;

    type *array;
};

template<class type>
void t1D<type>
::Init( int sz_x, void *ptr )
{
    size = sz_x;
    if( ptr == NULL )
    {
      // array = new type[size];
        pkNew(&array, size, __FILE__, __LINE__);
        assert( array != NULL );
    }
    else
        array = static_cast<type*>(ptr);
}

template<class type>
void t1D<type>
::Zero()
{
    for( int i=0; i<size; i++ )
        array[i] = static_cast<type>(0);
}

template<class type>
type& t1D<type>
::operator()( int x )
{
    int ind = x; // row-major format, as in C
    assert( ind >= 0 && ind < size );
    return array[ind];
}

template<class type>
t1D<type>&  t1D<type>
::operator=( const t1D<type>& at1D )
{
    assert( size == at1D.size );

    for( int i=0; i<size; i++ )
        (*this)[ i ] = at1D[ i ];

    return *this;
}

#endif
