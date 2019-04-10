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
 #ifndef __UDF_REGISTRY_HPP__
#define __UDF_REGISTRY_HPP__

////////// THESE ARE THE OLD CODE - MATCHING THEM BELOW FOR NO SPECIAL REASON
////////#define TERMINATECODE    0   // NEED: change this to something more unlikely.
////////#define BONDCODE         1
////////#define ANGLECODE        2
////////#define TORSIONCODE      3
////////#define IMPROPERCODE     4
////////#define EXCLUDEDCODE     5
////////#define UBCODE           6
////////#define PAIR14CODE       7
////////#define OPLSTORSIONCODE  8
////////#define OPLSIMPROPERCODE 9

#include "UDF_RegistryBindingMacros.hpp"

UDF_UNDEF(0 );
UDF_2PARM(1,StdHarmonicBond,double,mSpringConstant,double,mEquilibriumDistance);
UDF_2PARM(2,StdHarmonicAngle,double,mSpringConstant,double,mEquilibriumAngle);
UDF_3PARM( 3, SwopeTorsion,double,k0,double,delta,int,multiplicity);
UDF_3PARM( 4, ImproperDihedral, double, Kpsi, double, psi0, int, multiplicity);
UDF_UNDEF(5 );
UDF_2PARM(6,UreyBradley,double,mSpringConstant,double,mEquilibriumDistance);
UDF_UNDEF(7 );
UDF_UNDEF(8 );
UDF_UNDEF(9 );
UDF_UNDEF(10);
UDF_UNDEF(11);
UDF_UNDEF(12);
UDF_UNDEF(13);
UDF_UNDEF(14);
UDF_UNDEF(15);
UDF_UNDEF(16);
UDF_UNDEF(17);
UDF_UNDEF(18);
UDF_UNDEF(19);


#endif
