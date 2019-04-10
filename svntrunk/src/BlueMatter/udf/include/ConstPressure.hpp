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
 //
// Jed Pitera, August 2001
//
// sample routines for constant pressure integration
// using an Andersen thermostat with scaling of the fragment centers
// of mass & a velocity verlet integrator
//
// WARNING: very very rough draft
// WARNING: method names may be inconsistent/inappropriate for MDVM
// WARNING: iteration over fragments/sites not correct
//

#if 0

// New MDVM fragment/COM functions:

//  Volume Based
GetVolumeVelocity();
GetVolumeForce();
GetVolumeHalfInverseMass();
GetVolumePosition();
GetOldVolumeVelocity();
GetVolumePosition();
GetVolumeRatio();
GetVolumeVelocityRatio();
GetVolumeInversePosition();


// Fragment Based
GetCOMVelocity();
GetCOMHalfInverseMass();
GetCOMForce();
GetFragmentSiteCount();
GetCOMPosition();
GetMolecularVirial();
GetCOMKineticEnergy();

GetPressureRTP();
GetNumberDegreesOfFreedomRTP();

// New member variables:
XYZ VolumePosition
XYZ VolumeVelocity
XYZ VolumeAcceleration
XYZ OldVolumeVelocity
XYZ VolumeRatio
XYZ Virial
XYZ Pressure
XYZ VolumeForce

Fragment variables
COMHalfInverseMass
COMVelocity
COMForce
SiteCount
COMPosition


#endif


void
UpdateVolumeHalfStep( MDVM &aMDVM )
{
    // does a half-step update on the volume velocity to T+dT/2
    //
    // assumes that the volume_force has been set correctly

    double volume_velocity        = aMDVM.GetVolumeVelocity();
    double volume_force           = aMDVM.GetVolumeForce();
    // double volume_mass         = aMDVM.GetVolumeMass();
    double volume_halfinversemass = aMDVM.GetVolumeHalfInverseMass();
    double dt                     = aMDVM.GetDeltaT();

    // aMDVM.SetVolumeVelocity( volume_velocity + volume_force / volume_mass * dt / 2.0 );
    aMDVM.SetVolumeVelocity( volume_velocity + volume_force * volume_halfinversemass * dt );
    aMDVM.SetOldVolumeVelocity( volume_velocity );
    return;
}

void
UpdateVolumePosition( MDVM &aMDVM )
{
    //
    // updates the volume position to T+dT
    //
    double dt                = aMDVM.GetDeltaT();
    double old_inversevolume = aMDVM.GetVolumeInversePosition();
    double new_volume        = old_volume + dt * aMDVM.GetVolumeVelocity();

    aMDVM.SetVolumePosition( new_volume );
    aMDVM.SetVolumeRatio( pow( ( new_volume * old_inversevolume), (1.0 / 3.0) ));

    return;
}

void
UpdateVelocityFirstHalfStepNPT( MDVM &aMDVM )
{
    //
    // updates particle velocities to T+dT/2
    //
    // NOTE: this routine returns with the site velocities holding the
    //       velocity of the site _relative_ to the fragment center of
    //       mass -- we clean things back up to real velocities at the
    //       second half step
    //
    // I am assuming this routine runs over a single irreducible fragment
    // called "thisFragment" or something
    //
    double dt                  = aMDVM.GetDeltaT()
    XYZ vCOM                   = aMDVM.GetCOMVelocity();
    double mCOMHalfInverseMass = aMDVM.GetCOMHalfInverseMass();
    XYZ fCOM                   = aMDVM.GetCOMForce();

    double length_ratio        = aMDVM.GetVolumeRatio();
    double velocity_ratio      = aMDVM.GetVolumeVelocityRatio();

    XYZ new_vCOM = length_ratio * ( vCOM * (1.0 - dt * (1.0 / 3.0) * velocity_ratio) )
                                + dt * fCOM * mCOMHalfInverseMass;

    aMDVM.SetCOMVelocity( new_vCOM );

    //
    // loop over sites in this irreducible fragment
    //

    for( int SiteIndex = 0; SiteIndex < aMDVM.GetFragmentSiteCount(); SiteIndex++ )
        {
            double mSiteHalfInverseMass = aMDVM.GetHalfInverseMass( SiteIndex );
            XYZ vSite                   = aMDVM.GetVelocity( SiteIndex );
            XYZ fSite                   = aMDVM.GetForce( SiteIndex );

            XYZ new_vSite = (vSite - vCOM) +  dt * 
                            ( fSite * mSiteHalfInverseMass - fCOM * mCOMHalfInverseMass );

            aMDVM.SetVelocity( SiteIndex, new_vSite );
        }

    return;
}

void UpdatePositionNPT( MDVM &aMDVM )
{
    //
    // again, operates on all sites of a fragment thisFragment
    // updates particle positions to T+dT
    //
    // first update COM position of the fragment

    double dt   = aMDVM.GetDeltaT();
    XYZ    vCOM = aMDVM.GetCOMVelocity();
    XYZ    pCOM = aMDVM.GetCOMPosition();

    double length_ratio   = aMDVM.GetVolumeRatio();
    double velocity_ratio = aMDVM.GetVolumeVelocityRatio();

    XYZ new_pCOM = length_ratio * pCOM + dt * vCOM;
    aMDVM.SetCOMPosition( new_pCOM );

    //
    // loop over sites in this irreducible fragment
    //

    for( int SiteIndex = 0; SiteIndex < aMDVM.GetFragmentSiteCount(); SiteIndex++ )
        {
            XYZ vSite  = aMDVM.GetVelocity( SiteIndex );
            XYZ pSite  = aMDVM.GetPosition( SiteIndex );

            XYZ new_pSite = (pSite - pCOM) + new_pCOM + dt * vSite;

            aMDVM.SetPosition( SiteIndex, new_pSite );
        }

    return;
}

void UpdateVelocitySecondHalfStepNPT( MDVM &aMDVM )
{
    //
    // updates particle velocities to T+dT using an estimate of the
    // volume velocity at T+dT
    //
    // I am assuming this routine runs over a single irreducible fragment
    // called "thisFragment" or something
    //
    double dt                      = aMDVM.GetDeltaT();
    double volume_vest             = aMDVM.GetOldVolumeVelocity();
    double volume_force            = aMDVM.GetVolumeForce();
    double volume_halfinversemass  = aMDVM.GetVolumeHalfInverseMass();

    volume_vest += dt * volume_force * 2 * volume_halfinversemass;
    double velocity_ratio = volume_vest * aMDVM.GetVolumeInversePosition();

    XYZ    vCOM                = aMDVM.GetCOMVelocity();
    double mCOMHalfInverseMass = aMDVM.GetCOMHalfInverseMass();
    XYZ    fCOM                = aMDVM.GetCOMForce();

    XYZ new_vCOM = ( vCOM + dt * fCOM * mCOMHalfInverseMass ) /
                   ( 1.0 + dt / 3.0 * velocity_ratio );

    aMDVM.SetCOMVelocity( new_vCOM );

    //
    // loop over sites in this irreducible fragment
    //

    for( int SiteIndex = 0; SiteIndex < MDVM::GetFragmentSiteCount(); SiteIndex++ )
        {
            double mSiteHalfInverseMass = aMDVM.GetHalfInverseMass( SiteIndex );
            XYZ    vSite                = aMDVM.GetVelocity( SiteIndex );
            XYZ    fSite                = aMDVM.GetForce( SiteIndex );

            XYZ new_vSite =  new_vCOM + vSite
                + dt * (fSite * mSiteHalfInverseMass - fCOM * mCOMHalfInverseMass);

            aMDVM.SetVelocity( SiteIndex , new_vSite );
        }

    return;
}

void
GetVirialForce( MDVM &aMDVM )
{
    //
    // routine to calculate the internal pressure of the system and thus the
    // force on the simulation box
    //
    // big reductions would happen in GetMolecularVirial and GetCOMKineticEnergy
    //
    double virial                  = aMDVM.GetMolecularVirial();
    double ekecom                  = aMDVM.GetCOMKineticEnergy();
    double volume_inverseposition  = aMDVM.GetVolumeInversePosition();
    double volume_halfinversemass  = aMDVM.GetVolumeInverseMass();
    double external_pressure       = aMDVM.GetPressureRTP();
    //
    // note that there can be a sign flip based on the accumulation of the
    // virial. . . we assume virial has been accumulated as - dU/dr dot r
    //
    double internal_pressure =   volume_inverseposition / 3.0
                               * (2.0 * ekecom + virial );

    aMDVM.SetVolumeForce( (internal_pressure - external_pressure) * 2 * volume_halfinversemass );

    return;
}

void
SetFragmentMass{ MDVM &aMDVM )
{
    double mass = 0;
    for ( int i=0; i<MDVM::GetFragmentSiteCount(); i++ )
    {
        mass += aMDVM.GetSiteMass(i);
    }

    aMDVM.SetCOMHalfInverseMass( 0.5 / mass );
}

void
SetCOMKineticEnergy
{
    double KE = 0.5 * aMDVM.GetCOMMass() * aMDVM.GetCOMVelocity() * aMDVM.GetCOMVelocity();
}





        
      
        
