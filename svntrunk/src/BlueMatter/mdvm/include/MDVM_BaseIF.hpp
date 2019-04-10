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
 #ifndef INCLUDE_MDVM_BASEIF_HPP
#define INCLUDE_MDVM_BASEIF_HPP

// This file contains ALL the possible MDVM interfaces used by BlueMatter UDFs.

// ONLY the methods called by UDFs on should defined here NOT MDVM methods used only by
// the supporting framework.

// MDVMs used in different contexts will inherit this class and implement by
// overloading the desired function.

// NOTE: By order of inheritance, one can create subsets of function which
// can be used by more than one MDVM.  Make sure to get the order of inheritance
// right... probably this file should follow any others providing function subsets.

// NOTE: Any new MDVM interface must be referenced in three places:
// 1) Create the interface here with an assert body.
// 2) Implement the new function in the MDVM of choice (overloading the sig here)
// 3) Write and register the UDF which uses this new function

#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/complex.hpp>

// static XYZ XYZ_ReturnDummy;

class MDVM_BaseIF
  {
  public: 
    XYZ mTempXYZ; 
      
    enum { SiteA = 0, SiteB = 1, SiteC = 2, SiteD = 3 };
    
    enum { kVector0 = 0 , kVector1 = 1, kVector2 = 2 } ;
    
    enum {
    	kSuppressVectorReporting = 0     // steps towards tiling
    } ;

    inline void         Update()                                           { assert(0); return; }
    inline double       GetEwaldAlpha()                                    { assert(0); return 0.0; }
    inline double       GetLeknerEpsilon()                                    { assert(0); return 0.0; }
    inline XYZ          GetBoundingBoxDimensionVector()                    { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline void         SetBoundingBoxDimensionVector(const XYZ& bbdv)     { assert(0); return; }

    inline void         SetSiteIdInRegister( int reg, SiteId siteId )         { return; };

    inline void         ReportBoxSizeAbsoluteRatio ( const XYZ& ratio )    { assert(0); return; }
    inline XYZ          GetBoxSizeAbsoluteRatio ()                         { assert(0); return( XYZ::ZERO_VALUE() ); }

    inline const XYZ    GetCenterOfMassForce()                             { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline const XYZ    GetCenterOfMassPosition()                          { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline const XYZ    GetCenterOfMassVelocity()                          { assert(0); return( XYZ::ZERO_VALUE() ); }
//    inline const Complex& GetComplexRegister(const int aRegisterIndex ) const { assert(0); static Complex c(0.0); return c; }
////////    inline XYZ  GetForce( int SiteIndex )                          { assert(0); return( XYZ::ZERO_VALUE() ); }
/////// already defined below    inline XYZ    GetPosition( int SiteIndex ){ assert(0); return( XYZ::ZERO_VALUE() ); }
///////    inline XYZ   GetVelocity( int SiteIndex )                       { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline double       GetCenterOfMassHalfInverseMass()                   { assert(0); return( 0.0 ); }
inline double       GetCenterOfMassMass()                   { assert(0); return( 0.0 ); }
    inline double       GetCenterOfMassKineticEnergy()                     { assert(0); return( 0.0 ); }
/////    inline double GetDeltaT()                                         { assert(0); return( 0.0 ); }
////////////////    inline double GetHalfInverseMass( int SiteIndex )      { assert(0); return( 0.0 ); }
    inline const XYZ    GetVolumeRatio()                                   { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline const XYZ    GetMolecularVirial()                               { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline const XYZ    GetOldVolumeVelocity()                             { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline const double GetPressureRTP()                                   { assert(0); return( 0.0 ); }
    inline const XYZ    GetVolumeForce()                                   { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline const double GetVolumeHalfInverseMass()                         { assert(0); return( 0.0 ); }
    inline const double GetVolumeInverseMass()                             { assert(0); return( 0.0 ); }
    inline const XYZ    GetVolumeInversePosition()                         { assert(0); return( XYZ::ZERO_VALUE() ); }

#ifndef NOT_TIME_REVERSIBLE

    inline void SetExpVolumeVelocityRatio( double aRatio )                 { assert(0); }

    inline double GetExpVolumeVelocityRatio()                              { assert(0); return 0; }

    inline void UpdateExpVolumeVelocityRatioFullStepNPT()                  { assert(0); }

    inline void UpdateExpVolumeVelocityRatioHalfStepNPT()                  { assert(0); }

#endif

    inline const XYZ    GetVolumePosition()                                { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline const XYZ    GetVolumeVelocity()                                { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline const XYZ    GetVolumeVelocityRatio()                           { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline void         ReportInternalPressure( const XYZ InternalPressure )  { assert(0); return; }
    inline const int    GetFragmentSiteCount()                             { assert(0); return( 0 ); }
    inline void         ReportCenterOfMassForce( const XYZ new_pCOM )      { assert(0); return; }
    inline void         ReportCenterOfMassPosition( const XYZ new_pCOM )      { assert(0); return; }
    inline void         ReportCenterOfMassVelocity( const XYZ new_vCOM )      { assert(0); return; }
 inline void            SetCenterOfMassKineticEnergy( const double new_keCOM )      { assert(0); return; }
//  inline void         ReportComplexRegister( const int aUserSite, const Complex& aZ ) { assert(0); }
//IS THE FOLLOWING STILL PART OF THE INTERFACE?
    inline void         ReportVolumeRatio( const XYZ new_lRatio )             { assert(0); return; }
    inline void         ReportVolumeVelocityRatio( const XYZ new_lRatio )             { assert(0); return; }
    inline void         ReportOldVolumeVelocity( const XYZ volume_velocity )  { assert(0); return; }
    inline void         ReportPosition( int SiteIndex, XYZ new_pSite )        { assert(0); return; }
    inline void         ReportVelocity( int SiteIndex, XYZ new_vSite )        { assert(0); return; }
    inline void         ReportVolumeForce( const XYZ new_force )              { assert(0); return; }
    inline void         ReportVolumePosition( const XYZ new_volume )          { assert(0); return; }
    inline void         ReportVolumeVelocity( const XYZ new_velocity )        { assert(0); return; }

    inline const  int    GetSimTick()                                { assert(0); return(0); }
    inline XYZ & GetPosition( const int aUserSite )                     { assert(0); XYZ & rc = (XYZ&)mTempXYZ; return ( rc ); }
    inline void   ReportForce( const int aUserSite, XYZ aForce )                { assert(0); return; }
    //inline void   ReportForce( const int aUserSite, XYZ aForce )                { assert(0); return; }
// BGF I REALLY QUESTION THIS BEING A UDF MDVM INTERFACE METHOD
inline void   ZeroForce( const int aUserSite )                { assert(0); return; }
    inline XYZ& GetForce( const int aRegisterIndex )                   { assert(0);  XYZ & rc = (XYZ&)mTempXYZ; return ( rc ); }
    inline void   SetPositionPtr( const int aRegisterIndex, XYZ * aPositRef ){ assert(0); return; }
    //    inline void   ReportEnergy( double aEnergy )                                { assert(0); return; }
    inline void   ReportEnergy( double aEnergy )                                { assert(0); return; }
    // Allow someone to report dE/dr even if we do not catch it
    inline void   ReportdEdr( double adEdr )                                 { return; }
    // Likewise, allow reporting of virial
    inline void ReportVirial ( XYZ aVirial ) { } 
    inline void ReportSwitch(double S, double dSdR)	{ assert(0) ; return; }
    inline void ApplySwitch()	                                            { assert(0) ; return; }
    inline void ReportAdditionalScalarVirial( double aScalarVirial ) {assert(0) ; return ; } 
    
    inline double GetEnergy()                                                { assert(0); return( 0.0 ); }
    inline double GetdEdr()                                                  { assert(0); return( 0.0 ); }
    inline double GetSwitchS()												{assert(0) ; return 0.0 ; } 
    inline double GetSwitchdSdR()                                         { assert(0) ; return 0.0 ; } 
    inline double GetAdditionalScalarVirial( ) { assert(0) ; return 0.0 ; }
inline void   ReportMolecularVirial( XYZ aVirial )                                { assert(0); return; }
    inline XYZ GetVirial()                                                   { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline XYZ GetVector( int aUserSite0, int aUserSite1 )            { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline XYZ GetVectorN(int aVectorIndex) { assert(0) ; return XYZ::ZERO_VALUE() ; } 
    inline double GetDistance( int aUserSite0, int aUserSite1 )              { assert(0); return( 0.0 ); }
    inline double GetDistanceSquared( int aUserSite0, int aUserSite1 )       { assert(0); return( 0.0 ); }
    inline XYZ GetDynamicBoxDimensionVector()                                { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline XYZ GetDynamicBoxInverseDimensionVector()                                { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline SiteId    GetSiteId(int aUserSite0 )                                 { assert(0); return( 0 ); }
    inline XYZ& GetVelocity(int aUserSite0 )                           { assert(0); XYZ & rc = (XYZ&)mTempXYZ; return ( rc ); }
    inline double GetDeltaT()                                                { assert(0); return( 0.0 ); }
    inline double GetHalfInverseMass( int aUserSite0 )                       { assert(0); return( 0.0 ); }
inline double GetMass( int aUserSite0 )                       { assert(0); return( 0.0 ); }

    inline double GetImageDistanceSquared ( int aUserSite0, int aUserSite1 ) { assert(0); return ( 0.0 ); }
    inline XYZ GetImpulse( int aSiteIndex )              { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline XYZ GetDisplacement( int aSiteIndex )         { assert(0); return( XYZ::ZERO_VALUE() ); }
    inline void GetSwitchFunctionRadii(SwitchFunctionRadii& aResult) { assert(0); return; }

    inline void SetSwitchFunctionRadii( SwitchFunctionRadii aSwitchFunctionRadii ) { assert(0); return;}

    // Support for vector-form of bonded interactions
    inline double GetLength(void) { assert(0) ; return 0.0 ; } 
    inline double GetLengthN(int aVectorIndex) { assert(0) ; return 0.0 ; } 
    inline double GetRecipLength(void) { assert(0) ; return 0.0 ; } 
    inline double GetRecipLengthN(int aVectorIndex) { assert(0) ; return 0.0 ; } 
    
    
    
	inline void TearDown(void) { assert(0) ; } // End of life processing if/when needed
      
  };

#endif
