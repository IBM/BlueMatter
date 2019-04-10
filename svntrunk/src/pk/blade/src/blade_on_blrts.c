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
 

#include <blade_on_blrts.h>
#include <rts.h>
#include <bglpersonality.h>
#include <stdlib.h>
#include <string.h>

__BEGIN_DECLS

_Blade_Kernel_Config _Blade_Config_Area ALIGN_L3_CACHE;

//#define _DB_MMCS_PERS
#undef _DB_MMCS_PERS

//#define _DB_MMCS_PERS_VERBOSE
#undef _DB_MMCS_PERS_VERBOSE

#define BGLPERSONALITY_UNINITIALIZED_0 (0x00000000)
#define BGLPERSONALITY_UNINITIALIZED_E (0xeeeeeeee)

static char *_MPE K_DATA = "(E) MMCS Personality Error: ";

void _blade_show_tree_info( char *msg, int num, unsigned tinfo )
{
   union {
         BGLPersonalityTreeInfo TI;
         unsigned               u;
         }
         hack;

  hack.u = tinfo;

  printf("%s TreeInfo[%d]=0x%08x V=%d R=%d LT=%d CW=%d DP=%d P2P=0x%08x.\n",
         (msg ? msg : ""),
         num,
         tinfo,
         hack.TI.valid,         // 1 -> this info is valid
         hack.TI.redundant,     // 1 -> redundant wire
         hack.TI.linkType,      // 0 -> no wire, 1 -> compute node, 2 -> I/O
         hack.TI.commWorld,     // 1 -> child port, 2 -> parent port on comm_world tree
         hack.TI.destPort,      // dest port 0,1,2 -> A,B,C
         hack.TI.destP2Paddr ); // destination tree addr on this port
}


void _blade_show_tree_route( char *msg, int num, uint16_t spec )
{
   unsigned long s_spec = spec;

   printf("%s Tree Class[%2d]: 0x%04x=(", (msg ? msg : ""), num, spec );
   if ( s_spec & _BGL_TREE_RDR_SRC0 ) { s_spec &= ~_BGL_TREE_RDR_SRC0; printf("S0%s", s_spec ? " | " : ""); }
   if ( s_spec & _BGL_TREE_RDR_SRC1 ) { s_spec &= ~_BGL_TREE_RDR_SRC1; printf("S1%s", s_spec ? " | " : ""); }
   if ( s_spec & _BGL_TREE_RDR_SRC2 ) { s_spec &= ~_BGL_TREE_RDR_SRC2; printf("S2%s", s_spec ? " | " : ""); }
   if ( s_spec & _BGL_TREE_RDR_SRCL ) { s_spec &= ~_BGL_TREE_RDR_SRCL; printf("SL%s", s_spec ? " | " : ""); }
   if ( s_spec & _BGL_TREE_RDR_TGT0 ) { s_spec &= ~_BGL_TREE_RDR_TGT0; printf("T0%s", s_spec ? " | " : ""); }
   if ( s_spec & _BGL_TREE_RDR_TGT1 ) { s_spec &= ~_BGL_TREE_RDR_TGT1; printf("T1%s", s_spec ? " | " : ""); }
   if ( s_spec & _BGL_TREE_RDR_TGT2 ) { s_spec &= ~_BGL_TREE_RDR_TGT2; printf("T2%s", s_spec ? " | " : ""); }
   if ( s_spec & _BGL_TREE_RDR_TGTL ) { s_spec &= ~_BGL_TREE_RDR_TGTL; printf("TL%s", s_spec ? " | " : ""); }
   printf(").\n");
}

void _blade_Show_MMCS_Personalization( void )
{
   BGLPersonality *p = &(_Blade_Config_Area.mmcs_personality);
   int i;
   char bid[17];

   printf("(I) Pers: CRC = 0x%04x\n", p->CRC );

   printf("(I) Pers: personalitySize = %d\n", p->personalitySize );

   printf("(I) Pers: version = %d\n", p->version );

   printf("(I) Pers: DDRSize = %d\n", p->DDRSize );

   printf("(I) Pers: DDRModuleType = %d\n", p->DDRModuleType );

   printf("(I) Pers: RBS0 = 0x%02x, RBS1 = 0x%02x\n", p->RBS0, p->RBS1 );

   printf("(I) Pers: opFlags = 0x%02x %s %s %s %s %s %s %s.\n",
          p->opFlags,
          (p->opFlags & BGLPERSONALITY_OPFLAGS_ISTORUS_X  ? "XT" : "XM"),
          (p->opFlags & BGLPERSONALITY_OPFLAGS_ISTORUS_Y  ? "YT" : "YM"),
          (p->opFlags & BGLPERSONALITY_OPFLAGS_ISTORUS_Z  ? "YT" : "YM"),
          (p->opFlags & BGLPERSONALITY_OPFLAGS_GLINT      ? "GI=1" : "GI=0"),
          (p->opFlags & BGLPERSONALITY_OPFLAGS_SIMULATOR  ? "SIM=1" : "SIM=0"),
          (p->opFlags & BGLPERSONALITY_OPFLAGS_VIRTUALNM  ? "VNM=1" : "VNM=0"),
          (p->opFlags & BGLPERSONALITY_OPFLAGS_VERBOSE    ? "Verbose=1" : "Verbose=0") );

   printf("(I) Pers: clockHz = %d\n", p->clockHz );

   printf("(I) Pers: location = 0x%08x\n", p->location );

   printf("(I) Pers: xSize = %d, ySize = %d, zSize = %d.\n",
          p->xSize, p->ySize, p->zSize );

   printf("(I) Pers: xCoord = %d, yCoord = %d, zCoord = %d.\n",
          p->xCoord, p->yCoord, p->zCoord );

   printf("(I) Pers: xPsetSize = %d, yPsetSize = %d, zPsetSize = %d.\n",
          p->xPsetSize, p->yPsetSize, p->zPsetSize );

   printf("(I) Pers: reserved1 = %d\n", p->reserved1 );

   printf("(I) Pers: psetNum = %d\n", p->psetNum );

   for ( i = 0 ; i < 8 ; i++ )
      {
      _blade_show_tree_route("(I) Pers:", (i << 1),       (p->treeClassRoute[i] >> 16)    );
      _blade_show_tree_route("(I) Pers:", ((i << 1) + 1), (p->treeClassRoute[i] & 0xFFFF) );
      }

   for ( i = 0 ; i < 3 ; i++ )
      _blade_show_tree_info("(I) Pers:", i, p->treeInfo[i] );

   printf("(I) Pers: masterPsetNum = %d\n", p->masterPsetNum );

   printf("(I) Pers: treeHops = %d\n", p->treeHops );

   printf("(I) Pers: glintHops = %d\n", p->glintHops );

   printf("(I) Pers: xRootCoord = %d, yRootCoord = %d, zRootCoord = %d.\n", p->xRootCoord, p->yRootCoord, p->zRootCoord );

   printf("(I) Pers: setupFifo = %04x %s %s %s %s %s %s %s %s %s %s %s.\n",
          p->setupFifo,
          (p->setupFifo & BGLPERSONALITY_SETUPFIFO_XP             ? "X+"    : "  "),
          (p->setupFifo & BGLPERSONALITY_SETUPFIFO_YP             ? "Y+"    : "  "),
          (p->setupFifo & BGLPERSONALITY_SETUPFIFO_ZP             ? "Z+"    : "  "),
          (p->setupFifo & BGLPERSONALITY_SETUPFIFO_XM             ? "X-"    : "  "),
          (p->setupFifo & BGLPERSONALITY_SETUPFIFO_YM             ? "Y-"    : "  "),
          (p->setupFifo & BGLPERSONALITY_SETUPFIFO_ZM             ? "Z-"    : "  "),
          (p->setupFifo & BGLPERSONALITY_SETUPFIFO_A              ? "A"     : " " ),
          (p->setupFifo & BGLPERSONALITY_SETUPFIFO_B              ? "B"     : " " ),
          (p->setupFifo & BGLPERSONALITY_SETUPFIFO_C              ? "C"     : " " ),
          (p->setupFifo & BGLPERSONALITY_SETUPFIFO_LOOPBACK_TORUS ? "TS_LB" : ""  ),
          (p->setupFifo & BGLPERSONALITY_SETUPFIFO_LOOPBACK_TREE  ? "TR_LB" : ""  )  );

   for ( i = 0 ; i < 16 ; i++ )
      bid[i] = p->blockID[i];
   bid[i] = 0;
   printf("(I) Pers: blockID = >%s<\n", bid );
}


int _blade_Import_Personalization( void )
{
   BGLPersonality *p = &(_Blade_Config_Area.mmcs_personality);
   Bit32 ddr_size;
   int rc, numComputeNodes, numComputeNodesPerPset, numPsets;

   if ( (rc = rts_get_personality( p, sizeof(BGLPersonality))) )
      {
      printf("%s Unable to get mmcs personality.\n", _MPE );
      return( -1 );
      }

   if ( (_Blade_Config_Area.hard_processor_id = rts_get_processor_id()) > 1 )
      {
      printf("%s Unexpected processor id %d.\n", _MPE, _Blade_Config_Area.hard_processor_id );
      return( -1 );
      }

   ddr_size               = (p->DDRSize / MEGA);
   numComputeNodes        = (p->xSize * p->ySize * p->zSize);
   numComputeNodesPerPset = (p->xPsetSize * p->yPsetSize * p->zPsetSize);
   numPsets               = (numComputeNodes / numComputeNodesPerPset);

   _Blade_Config_Area.bs_flags.flags = 0;

#if defined( _DB_MMCS_PERS_VERBOSE )
   _blade_Show_MMCS_Personalization();
#endif // _DB_MMCS_PERS_VERBOSE

   if ( p->version != BGLPERSONALITY_VERSION )
      {
      printf("%s Version Mismatch! Expectd=%d, Received=%d.\n",
              _MPE, BGLPERSONALITY_VERSION, p->version );
      return( -1 );
      }

   // Set up the config area from the SRAM Personalization
   _Blade_Config_Area.freq_in_MHz = (p->clockHz / 1000000);
   if ( (_Blade_Config_Area.freq_in_MHz <  200) ||
        (_Blade_Config_Area.freq_in_MHz > 1000)    )
      {
      printf("%s clockHz out of reasonable range!\n", _MPE );
      }

   if ( ddr_size == 2048 )
      {
      _Blade_Config_Area.bs_flags.ddr_size = 3;
      }
   else if ( ddr_size == 1024 )
      {
      _Blade_Config_Area.bs_flags.ddr_size = 2;
      }
   else if ( ddr_size ==  512 )
      {
      _Blade_Config_Area.bs_flags.ddr_size = 1;
      }
   else
      {
      _Blade_Config_Area.bs_flags.ddr_size = 0;
      }

   if ( p->opFlags & BGLPERSONALITY_OPFLAGS_VIRTUALNM )
      _Blade_Config_Area.is_VirtualNodeMode = 1;
   else
      _Blade_Config_Area.is_VirtualNodeMode = 0;

   if ( p->opFlags & BGLPERSONALITY_OPFLAGS_GLINT )
      _Blade_Config_Area.have_GlobalInts = 1;
   else
      _Blade_Config_Area.have_GlobalInts = 0;

   if ( !(p->setupFifo & (BGLPERSONALITY_SETUPFIFO_XP |
                          BGLPERSONALITY_SETUPFIFO_YP |
                          BGLPERSONALITY_SETUPFIFO_ZP |
                          BGLPERSONALITY_SETUPFIFO_XM |
                          BGLPERSONALITY_SETUPFIFO_YM |
                          BGLPERSONALITY_SETUPFIFO_ZM  )) )
      {
      printf("%s Compute Node Given No Torus Links!\n", _MPE );
      return( -1 );
      }

   if ( !(p->setupFifo & (BGLPERSONALITY_SETUPFIFO_A |
                          BGLPERSONALITY_SETUPFIFO_B |
                          BGLPERSONALITY_SETUPFIFO_C  )) )
      {
      printf("%s Compute Node Given No Tree Links!\n", _MPE );
      return( -1 );
      }

   _Blade_Config_Area.is_io_node          = 0;
   _Blade_Config_Area.bs_flags.io_node    = 0;
   _Blade_Config_Area.bs_flags.tr_enable  = 1;
   _Blade_Config_Area.bs_flags.ts_enable  = 1;
   _Blade_Config_Area.bs_flags.eth_enable = 0;
   _Blade_Config_Area.X                   = p->xCoord;
   _Blade_Config_Area.Y                   = p->yCoord;
   _Blade_Config_Area.Z                   = p->zCoord;

   _Blade_Config_Area.max_X              = (p->xSize - 1);
   _Blade_Config_Area.max_Y              = (p->ySize - 1);
   _Blade_Config_Area.max_Z              = (p->zSize - 1);
   _Blade_Config_Area.nodes_X            = p->xSize;
   _Blade_Config_Area.nodes_Y            = p->ySize;
   _Blade_Config_Area.nodes_Z            = p->zSize;
   _Blade_Config_Area.is_mesh_X          = ((p->opFlags & BGLPERSONALITY_OPFLAGS_ISTORUS_X) ? 0 : 1);
   _Blade_Config_Area.is_mesh_Y          = ((p->opFlags & BGLPERSONALITY_OPFLAGS_ISTORUS_Y) ? 0 : 1);
   _Blade_Config_Area.is_mesh_Z          = ((p->opFlags & BGLPERSONALITY_OPFLAGS_ISTORUS_Z) ? 0 : 1);
   _Blade_Config_Area.real_Rank          = BGLPartitionMakeRank( _Blade_Config_Area.X,
                                                                 _Blade_Config_Area.Y,
                                                                 _Blade_Config_Area.Z );
   _Blade_Config_Area.Rank               = _Blade_Config_Area.real_Rank;
   _Blade_Config_Area.max_Rank           = BGLPartitionMakeRank( _Blade_Config_Area.max_X,
                                                                 _Blade_Config_Area.max_Y,
                                                                 _Blade_Config_Area.max_Z );
   _Blade_Config_Area.nodes_IO           = numPsets;
   _Blade_Config_Area.io_node_p2p_addr   = ((p->xSize * p->ySize * p->zSize) + p->psetNum);
   _Blade_Config_Area.node_p2p_addr      = _Blade_Config_Area.Rank;

#if defined( _DB_MMCS_PERS )
   printf("(I) _blade_Import_Personalization[%2d]: Core %d: (%2d,%2d,%2d) in {%2d%c,%2d%c,%2d%c} (%dC + %dI).\n",
          _Blade_Config_Area.Rank, hard_processor_id(),
          _Blade_Config_Area.X, _Blade_Config_Area.Y, _Blade_Config_Area.Z,
          _Blade_Config_Area.nodes_X, (_Blade_Config_Area.is_mesh_X ? 'M' : 'T'),
          _Blade_Config_Area.nodes_Y, (_Blade_Config_Area.is_mesh_Y ? 'M' : 'T'),
          _Blade_Config_Area.nodes_Z, (_Blade_Config_Area.is_mesh_Z ? 'M' : 'T'),
          BGLPartitionGetNumNodesCompute(),
          BGLPartitionGetNumNodesIO() );
#endif
   return( 0 );
}


Bit32 mfdcr( unsigned dcrn )
{
   int rc = 0;
   unsigned val = 0;

   if ( (rc  = rts_read_dcr( dcrn, &val )) )
      {
      printf("(E) mfdcr(%d) fails with rc=%d\n", dcrn, val );
      }
  return( val );
}

void mtdcr( unsigned dcrn, unsigned val )
{
   int rc = 0;

   if ( (rc = rts_write_dcr( dcrn, val )) )
      {
      printf("(E) mtdcr(%d,%d) fails with rc=%d\n", dcrn, val, rc );
      }
}


// We can't read the hint cutoff dcrs, so have to recalc
// From devices/bgltorusdcr.c (which was a ~copy of blade/bl_torus.c)
int _blade_torus_calc_hints_same_as_blrts( void )
{
   BGLPersonality *p = &(_Blade_Config_Area.mmcs_personality);
   int x, y, z, sizex, sizey, sizez;
   int n_xp, n_xm, n_yp, n_ym, n_zp, n_zm;
   int h_xp, h_xm, h_yp, h_ym, h_zp, h_zm;

   x = p->xCoord;
   y = p->yCoord;
   z = p->zCoord;
   sizex = p->xSize;
   sizey = p->ySize;
   sizez = p->zSize;

  /* ---------------------------------------------------------- */
  /* calculate direction cutoffs and neighbor coordinates.      */
  /* These values are calculated differently for torus and mesh */
  /* Calculatons for torus are adapeted from the torus users    */
  /* Guide.  This code breaks routing ties in an uneven way     */
  /* If the node is an odd coorinate, it breaks the tie in favor*/
  /* of the plus direction, if it is odd, the minus direction   */
  /* ---------------------------------------------------------- */
#ifndef MIN
#define MIN(a,b) ((a)<(b))?(a):(b)
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif
  if (BGLPersonality_isTorusX(p))
    {
      n_xp = ((x+1)%sizex);
      n_xm = ((x-1+sizex)%sizex);
      if(n_xp % 2)
	{
	  h_xp = MIN(sizex-1, x+sizex/2);
	  h_xm = MAX(0,x-(sizex/2 -1));
	}
      else
	{
	  h_xp = MIN(sizex-1, x+(sizex/2 -1));
	  h_xm = MAX(0, x - sizex/2);
	}
    }
  else
    {
      n_xp = (x==sizex-1) ? sizex : (x+1);
      n_xm = (x==0) ? 0 : (x-1);
      h_xp = sizex;
      h_xm = 0;
    }

  if (BGLPersonality_isTorusY(p))
    {
      n_yp = ((y+1)%sizey);
      n_ym = ((y-1+sizey)%sizey);
      if(n_yp % 2)
	{
	  h_yp = MIN(sizey-1, y+sizey/2);
	  h_ym = MAX(0,y-(sizey/2 -1));
	}
      else
	{
	  h_yp = MIN(sizey-1, y+(sizey/2 -1));
	  h_ym = MAX(0, y - sizey/2);
	}
    }
  else
    {
      n_yp = (y==sizey-1) ? sizey : (y+1);
      n_ym = (y==0) ? 0 : (y-1);
      h_yp = sizey;
      h_ym = 0;
    }

  if (BGLPersonality_isTorusZ(p))
    {
      n_zp = ((z+1)%sizez);
      n_zm = ((z-1+sizez)%sizez);
      if(n_zp % 2)
	{
	  h_zp = MIN(sizez-1, z+sizez/2);
	  h_zm = MAX(0,z-(sizez/2 -1));
	}
      else
	{
	  h_zp = MIN(sizez-1, z+(sizez/2 -1));
	  h_zm = MAX(0, z - sizez/2);
	}
    }
  else
    {
      n_zp = (z==sizez-1) ? sizez : (z+1);
      n_zm = (z==0) ? 0 : (z-1);
      h_zp = sizez;
      h_zm = 0;
    }

   _Blade_Config_Area.hint_cutoff_XP = h_xp;
   _Blade_Config_Area.hint_cutoff_YP = h_yp;
   _Blade_Config_Area.hint_cutoff_ZP = h_zp;

   _Blade_Config_Area.hint_cutoff_XM = h_xm;
   _Blade_Config_Area.hint_cutoff_YM = h_ym;
   _Blade_Config_Area.hint_cutoff_ZM = h_zm;

#if defined( _DB_MMCS_PERS_VERBOSE )
   printf("(I) Hint Cutoffs: XP=%d, XM=%d, YP=%d, YM=%d, ZP=%d, ZM=%d.\n",
          _Blade_Config_Area.hint_cutoff_XP,
          _Blade_Config_Area.hint_cutoff_XM,
          _Blade_Config_Area.hint_cutoff_YP,
          _Blade_Config_Area.hint_cutoff_YM,
          _Blade_Config_Area.hint_cutoff_ZP,
          _Blade_Config_Area.hint_cutoff_ZM  );
#endif

   return(0);
}


int _blade_on_blrts_init( void )
{
   int rc = 0;
   char *p = (char *)&_Blade_Config_Area;

   memset( p, 0, sizeof(_Blade_Config_Area) );

   if ( !(rc = _blade_Import_Personalization()) )
      rc = _blade_torus_calc_hints_same_as_blrts();

   _Blade_Config_Area.AppCodeSeg = (((Bit32)&_blade_on_blrts_init) & 0xFF000000);

   dcache_store_bytes( (void *)((uint32_t)p & ~0x1F), (sizeof(_Blade_Config_Area) + 64) );

   if ( _Blade_Config_Area.is_VirtualNodeMode )
      _blLockBoxBarrier( _BLADE_VNM_BARRIER );

   return( rc );
}


__END_DECLS

