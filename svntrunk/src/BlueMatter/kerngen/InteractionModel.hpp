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
 #ifndef __INTERACTION_MODEL__
#define __INTERACTION_MODEL__

// NearestSquareDistanceInPeriodicVolume comes from PeriodicImage.hpp, but this needs DynVarMgrIf for the box size
inline
double
NearestSquareDistanceInPeriodicVolume(const XYZ &PositionA, const XYZ &PositionB) ;

#ifndef PKFXLOG_VERLET_LIST_DEBUG
#define PKFXLOG_VERLET_LIST_DEBUG ( 0 )
#endif

static XYZ GetPositionFromFullTable( int aSiteId );
static double GetCurrentVerletGuardZone();

class InteractionModel
{
public:

  // Figure how we are going to do the load balance modelling
  enum {
    k_OrbTableModel =
#if defined(USE_TABLE_MODEL)
    1
#else
    0
#endif	
    ,
    k_OrbVVLModel = 
#if defined(ORB_WEIGHT_FRAG)
    0
#else
    1
#endif   
    ,
    k_OrbVVLAAModel =
#if defined(ORB_WEIGHT_VVL33)
    0
#else
    1
#endif
  } ;

  static int CountSitesNotExcluded(int aTargetI, int aSourceI)
  {

    if ( aTargetI == aSourceI ) return 0 ; 
    int SourceFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aSourceI );
    int TargetFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( aTargetI );

    int result = 0 ; 

	
    for(int SourceFragmentSiteIndex = SourceFragmentSiteCount-1;
	SourceFragmentSiteIndex >= 0;
	SourceFragmentSiteIndex-- )
      {
	unsigned int SourceAbsSiteId =
	  MSD_IF::GetIrreducibleFragmentMemberSiteId( aSourceI, SourceFragmentSiteIndex );
	
	// Scan the Target Fragment since that should be more local.
	for( int TargetFragmentSiteIndex = (TargetFragmentSiteCount-1);
	     TargetFragmentSiteIndex >= 0 ;
	     TargetFragmentSiteIndex-- )
	  {
	    unsigned int TargetAbsSiteId =
	      MSD_IF::GetIrreducibleFragmentMemberSiteId( aTargetI, TargetFragmentSiteIndex );
	
	    unsigned IsInExcList = MSD_IF::AreSitesInExcIncList( SourceAbsSiteId, TargetAbsSiteId );
	      
	    if ( 0 == IsInExcList ) 
	      {
	      	result += 1 ;
	      }
	
	  }
      }
    return result ;
  }
		
  // Model the interaction weight for when we are doing 'vector verlet'
  // with all atom-atom
  template <class DVM_IF>
  static double AAWeight(int aTargetAbsSiteId, int aSourceAbsSiteId, DVM_IF* aDVM_IF, int debug=0 )
  {

#if defined(AA_USE_LOADTUNE)    	
    double BelowLowerSwitchCost = Platform::LoadBalance::GetTune(0) ; // '2.0' would be reasonable
    double InSwitchCost = Platform::LoadBalance::GetTune(1) ; // '3.0' would be reasonable
#else
    // Timings from a sample run with optimisations turned off
    double TimeToVectors =  174154.0 ; 
    double TimeTodEdr    =  541766.0 ;
    double TimeToS       =  679800.0 ; 
    double TimeToEnd     = 1108209.0 ;

    // Derived timings
    double CostForAboveUpper = TimeToVectors ;
    double CostForBelowLower = TimeToEnd-(TimeToS-TimeTodEdr) ;
    double CostForInSwitch   = TimeToEnd ;
    // Values for degree-of-freedom calculation      
    double BelowLowerSwitchCost = 1.0;
    double InSwitchCost = 1.0;
                
#endif    	
    double AboveUpperSwitchCost = 1.0 ; 
    XYZ TargetAtomLoc ;
    XYZ SourceAtomLoc ;
//     TargetAtomLoc.mX = aDVM_IF->PositionXFromFullTable(aTargetAbsSiteId);
//     TargetAtomLoc.mY = aDVM_IF->PositionYFromFullTable(aTargetAbsSiteId);
//     TargetAtomLoc.mZ = aDVM_IF->PositionZFromFullTable(aTargetAbsSiteId);
//     SourceAtomLoc.mX = aDVM_IF->PositionXFromFullTable(aSourceAbsSiteId);
//     SourceAtomLoc.mY = aDVM_IF->PositionYFromFullTable(aSourceAbsSiteId);
//     SourceAtomLoc.mZ = aDVM_IF->PositionZFromFullTable(aSourceAbsSiteId);
    TargetAtomLoc = GetPositionFromFullTable( aTargetAbsSiteId );
    SourceAtomLoc = GetPositionFromFullTable( aSourceAbsSiteId );

    double SquareDistance = NearestSquareDistanceInPeriodicVolume(TargetAtomLoc, SourceAtomLoc) ;

    double SwitchLowerCutoff = MSD_IF::GetSwitchLowerCutoffForShell( 0 );
    double SwitchDelta       = MSD_IF::GetSwitchDeltaForShell( 0 );
      
    double Cutoff = SwitchLowerCutoff + SwitchDelta;
        
    //double VerletGuardZone = aDVM_IF->GetCurrentVerletGuardZone() ; 
    double VerletGuardZone = GetCurrentVerletGuardZone();
    double VerletLimit = Cutoff + VerletGuardZone ;
        
    double weight = fsel(
			 (SwitchLowerCutoff*SwitchLowerCutoff-SquareDistance), BelowLowerSwitchCost
			 , fsel(
				(Cutoff*Cutoff-SquareDistance), InSwitchCost
				,  fsel(
					(VerletLimit*VerletLimit-SquareDistance), AboveUpperSwitchCost, 0.0
					) 
				)
			 ) ;
    	
    BegLogLine( (PKFXLOG_VERLET_LIST_DEBUG && Platform::Topology::GetAddressSpaceId() == 0)
		|| debug )
		  << "aTargetAbsSiteId: " << aTargetAbsSiteId
		  << " aSourceAbsSiteId: " << aSourceAbsSiteId
		  << " TargetAtomLoc=" << TargetAtomLoc
		  << " SourceAtomLoc=" << SourceAtomLoc
		  << " Distance=" << sqrt(SquareDistance)
		  << " Weight=" << weight
		  << " "
		  << EndLogLine ;
    TargetAtomLoc.Zero() ;
    SourceAtomLoc.Zero() ;
          
    return weight ; 
  }
    
  template <class DVM_IF>
  static double ModelInteractionWeight_IFPWaterSite_AA(int TargetI, int SourceI, DVM_IF* aDVM_IF, int debug = 0 )
  {
    int at1 = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetI );
    int at2 = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceI );
    int mlid1 = MSD_IF::GetMoleculeId( TargetI );
    int mlid2 = MSD_IF::GetMoleculeId( SourceI );
    int FirstSiteId1   = MSD_IF::GetIrreducibleFragmentFirstSiteId( TargetI );
    int FirstSiteId2   = MSD_IF::GetIrreducibleFragmentFirstSiteId( SourceI );

    double weight = 0.0 ; 

	  
    assert(at1 <= 6 ) ; 
    assert(at2 <= 6 ) ; 
    unsigned char atable[36] ; 
    unsigned char btable[36] ; 
    unsigned int atx=0 ; 
    for ( int x1=0;x1<at1;x1+=1)
      {
	for (int x2 = 0 ; x2 < at2 ; x2 += 1)
	  {
	    atable[atx] = x1 ; 
	    btable[atx] = x2 ; 
	    atx += 1 ;
	  }
      }
    if ( mlid1 != mlid2 )
      {
	for ( int x=0;x<at1*at2;x+=1 )
	  {
	    weight += AAWeight(FirstSiteId1+atable[x],FirstSiteId2+btable[x],aDVM_IF, debug ) ;
	  }
      } else 
	{
	  double wtable[36] ; 
	  for ( int x=0;x<at1*at2;x+=1 )
	    {
              wtable[x]= AAWeight(FirstSiteId1+atable[x],FirstSiteId2+btable[x],aDVM_IF, debug) ;
	    }

	  for ( int y=0;y<at1*at2;y += 1)
	    {
              unsigned IsInExcList = MSD_IF::AreSitesInExcIncList(FirstSiteId1+atable[y],FirstSiteId2+btable[y]);
              if ( 0 == IsInExcList )
	      	{
		  weight += wtable[y] ;
	      	} 
	    }
	}
	  

    return weight ;
  }
	
  template <class DVM_IF>
  static double GetWeight(int TargetI,int SourceI, DVM_IF* aDVM_IF, int debug=0 )
  {
    /// Alex - look here
    double ModWeight;
    int mlid1 = MSD_IF::GetMoleculeId( TargetI );
    int mlid2 = MSD_IF::GetMoleculeId( SourceI );
    int at1 = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetI );
    int at2 = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceI );
        
    if ( k_OrbVVLModel )
      {
	// 'Vector Verlet' model just looks how much will be processed with the '3:3' optimisation
	// (and at the moment guesses it; really it should take into account exclusions)
	if ( k_OrbVVLAAModel )
	  {
            ModWeight = ModelInteractionWeight_IFPWaterSite_AA( TargetI, SourceI, aDVM_IF, debug ) ;
	  } 
	else {
	  ModWeight = ( 3 == at1 && 3 == at2 ) ? 1.55412 : ( 0.586728 * at1 * at2 ) ;
	}            
      } else {
	int atcount1 = (at1 < at2) ? at1 : at2;
	int atcount2 = (at1 < at2) ? at2 : at1;
	if(mlid1 == mlid2)
	  {
	    if ( k_OrbTableModel )
	      {
		switch(atcount1)
		  {
		  case 1:  
		    switch(atcount2)
		      {
		      case 1: ModWeight = 1.217803; break; // 1.072657; break;
		      case 2: ModWeight = 1.657851; break; // 1.78807; break;
		      case 3: ModWeight = 2.092172; break; // 2.285994; break;
		      case 4: ModWeight = 2.678360; break; //2.968841; break;
		      default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
			PLATFORM_ABORT(msg);		  
		      }
		    break;
		  case 2:  
		    switch(atcount2)
		      {
		      case 2: ModWeight = 2.678111; break; // 2.768698; break;
		      case 3: ModWeight = 3.560940; break;  // 3.729389; break;
		      case 4: ModWeight = 4.307348; break; //4.670057; break;
		      default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
			PLATFORM_ABORT(msg);		  
		      }
		    break;
		  case 3:  
		    switch(atcount2)
		      {
		      case 3: ModWeight = 4.448243; break; // 5.2042 ; break;
		      case 4: ModWeight = 5.990370; break; // 6.382139 ; break;
		      default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
			PLATFORM_ABORT(msg);		  
		      }
		    break;
		  case 4:  
		    switch(atcount2)
		      {
		      case 4: ModWeight = 5.626951; break; // 7.762542; break;
		      default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
			PLATFORM_ABORT(msg);		  
		      }
		    break;
		  default:
		    char msg[256]; 
		    sprintf(msg,"Frag atom count out of range: %d\n", atcount1);		  
		    PLATFORM_ABORT(msg);		  
		  }
	      } else {
		ModWeight = 1.28 + 0.406*(atcount1*atcount2);
	      }
	  }
	else
	  {
	    if ( k_OrbTableModel )
	      {
		switch(atcount1)
		  {
		  case 1:  
		    switch(atcount2)
		      {
		      case 1: ModWeight = 1.217803; break; // 1.072657; break;
		      case 2: ModWeight = 1.657851; break; // 1.78807; break;
		      case 3: ModWeight = 2.092172; break; // 2.285994; break;
		      case 4: ModWeight = 2.678360; break; //2.968841; break;
		      default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
			PLATFORM_ABORT(msg);		  
		      }
		    break;
		  case 2:  
		    switch(atcount2)
		      {
		      case 2: ModWeight = 2.678111; break; // 2.768698; break;
		      case 3: ModWeight = 3.560940; break;  // 3.729389; break;
		      case 4: ModWeight = 4.307348; break; //4.670057; break;
		      default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
			PLATFORM_ABORT(msg);		  
		      }
		    break;
		  case 3:  
		    switch(atcount2)
		      {
		      case 3: ModWeight = 3.462162; break; // 3.439775; break;
		      case 4: ModWeight = 5.990370; break; // 6.382139 ; break;
		      default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
			PLATFORM_ABORT(msg);		  
		      }
		    break;
		  case 4:  
		    switch(atcount2)
		      {
		      case 4: ModWeight = 5.626951; break; // 7.762542; break;
		      default: char msg[256]; sprintf(msg,"Frag atom count out of range: %d\n", atcount2);
			PLATFORM_ABORT(msg);		  
		      }
		    break;
		  default:
		    char msg[256]; 
		    sprintf(msg,"Frag atom count out of range: %d\n", atcount1);		  
		    PLATFORM_ABORT(msg);		  
		  }
	      } else {
		ModWeight = 1.28 + 0.294*(atcount1*atcount2);
	      }
	  }
      }
	
    return ModWeight;
  }
};
#endif
