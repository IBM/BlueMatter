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
 #ifndef FFT3DLIB_HPP_
#define FFT3DLIB_HPP_

template <typename T, typename Tcomplex> class PrivateFFT ;

template<class FFT_PLAN, int FWD_REV, class T, class Tcomplex> class BGL3DFFT 
{
public:  
  enum{ DIM = 4 };
  typedef FFT_PLAN FFT_PLAN_IF;
  PrivateFFT<T ,Tcomplex > *privateFFT;
  
  enum { P_X = FFT_PLAN::P_X };
  enum { P_Y = FFT_PLAN::P_Y };
  enum { P_Z = FFT_PLAN::P_Z };
  
  void Init(BGL3DFFT** ptr, const int& globalNx, const int& globalNy, const int& globalNz,int subPx, int subPy, int subPz); 
  void Init(const int& localNx, const int& localNy, const int& localNz, const int& pX, const int& pY, const int& pZ,int subPx, int subPy, int subPz, int compress);
  

//  void Init(int localNx, int localNy, int localNz, int pX, int pY, int pZ);
//  void Init(int localNx, int localNy, int localNz, int pX, int pY, int pZ,
//	    int subPx, int subPy, int subPz); 

  void Finalize();

  void ZeroRealSpace();
  
  // Function calls to retrive data element by element.

  inline T GetRealSpaceElement(const int x, const int y, const int z ) const;
  inline void PutRealSpaceElement(const int x, const int y, const int z, T realInput);
  inline Tcomplex GetRecipSpaceElement(const int kx,const int ky, const int kz) const;
  inline void PutRecipSpaceElement( int kx, int ky, int kz, Tcomplex value);
  
  // Returns the original and the transform data distributions
  void GetLocalDimensions( int& aLocalX, int& aLocalSizeX,
         int& aLocalY, int& aLocalSizeY,
         int& aLocalZ, int& aLocalSizeZ,
         int& aLocalRecipX, int& aLocalRecipSizeX,
         int& aLocalRecipY, int& aLocalRecipSizeY,
         int& aLocalRecipZ, int& aLocalSizeRecipZ );
  
  void DoFFT( Tcomplex * bglfftIn, Tcomplex * bglfftOut ) ; 
  void DoFFT();

  // These fields  are for phase_4: CHECK do we still need them. 
public:

    // accepts rectangle in simulation space - vmin and vmax are the corners of it
    // fills in the list of nodes where the effected mesh lives - idnum is num of entries returned
    int InitProcMesh( int grdvoxleft_X, int grdvoxleft_Y, int grdvoxleft_Z);
    inline void GetGridOwnerIDList( const iXYZ& vmin, const iXYZ& vmax, int idlist[], int& idnum );

private:

    // this facilitates GetGridOwnerIDList(...)
    int *subgridX;
    int *subgridY;
    int *subgridZ;
};



#endif /*FFT3DLIB_HPP_*/
