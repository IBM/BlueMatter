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
 
/***************************************************************************
 * Project:         BlueMatter
 *
 * Module:          NeighborList
 *
 * Purpose:         Contain fast Neighbor list generation code.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         040805 ARAYSHU Created from code in NSQ*hpp
 *
 ***************************************************************************/

#ifndef __NEIGHBOUR_LIST__
#define __NEIGHBOUR_LIST__
#ifndef BM_SPATIAL_DECOMPOSITION

         // A Neighbour list is able to produce, for a fragment, a list of those fragments which are
         // close enough in a periodic image sense that at least one of the atom pairs is within range
         // of the nonbond force cutoff. (Trivially you could produce a list of all the fragments, but
         // that would lead to poor performance as IFP would get asked to process everything;
         // the point of this is to produce the smallest correct list that is reasonably easy to compute)

         // To do this, we have to set up with a list of all the fragments; we keep the minimum
         // bounding box for the fragment, specifically we keep the centre of this box, and the
         // vector from the centre to the top right corner (i.e. the one with all elements positive)
         // We also keep the distance from the centre to the furthest atom in the fragment, as
         // this will enable us to discard some fragment pairs at 'NSQ' stage in some geometrical
         // configurations.

         // Integer versions (generally, the vector as a fraction of the bounding box multiplied
         // by 2**24) are also kept; the algorithm can be run either in integers or doubles, and
         // the fastest one varies according to the machine, the compiler, and the phase of the moon :-)


         // Having set this list up, we can produce on demand the sublist for those fragments
         // in range of a given fragment, as an array; and we will be able to drive 'IFP' as a
         // simple 'for' loop indexing the array.


         class NeighbourList
         {
         public:
           class remainder
           {
           public:
             double e2 ;
             int    a  ;
             int dummy ;
           } ;

           enum
           {
             k_PadCount = 20 , // Amount of padding when we want to overcompute for vectorised square roots
             k_FragCount = NUMBER_OF_FRAGMENTS
           };

           enum
           {
             k_ScaleShift = 8
           };

           XYZ p ;
           XYZ k ;

           double x[k_FragCount] ;
           double y[k_FragCount] ;
           double z[k_FragCount] ;

           double e[k_FragCount+k_PadCount] ;
           double ex[k_FragCount] ;
           double ey[k_FragCount] ;
           double ez[k_FragCount] ;

           int ix[k_FragCount] ;
           int iy[k_FragCount] ;
           int iz[k_FragCount] ;
           int iex[k_FragCount] ;
           int iey[k_FragCount] ;
           int iez[k_FragCount] ;

           int result[k_FragCount] ;

           NeighbourList(const XYZ& ap, const XYZ& ak): p(ap), k(ak) {
             BegLogLine(PKFXLOG_NSQSOURCEFRAG_SUMMARY)
               << "NeighbourList( p=" << p
               << " k=" << k
               << EndLogLine ;
           } ;

           NeighbourList(){}

           void Setup( XYZ& ap, XYZ& ak)
             {
               p = ap;
               k = ak;
             }

           // This is the 'trivial, diagnostic' version; it considers that all fragment pairs are
           // in range. IFP will go through all atom pairs, very expensively, and get the same result
           // assuming everything is working.
           int ProduceAll(
                          const XYZ& aXYZ
                          , const XYZ& eXYZ
                          , double e0
                          , int qstart
                          , int qend
                          )
             {
             int q0 = 0 ;
             for (int a0=qstart; a0<qend; a0+=1 )
               {
                 result[q0] = a0 ;
                 q0 += 1 ;
               }
             return q0 ;
             }

           // This is the floating-point version. First it goes through all the fragments in the X
           // dimension, to produce all those within a slab of the wanted one.
           // Then it goes through the slab in the Y dimension, to produce all those within
           // a cylinder of the wanted one.
           // Then it goes through the cylinder in the Z dimension, to produce all those
           // within a sphere of the wanted one; which is what we wanted in the first place.
           // Each of these loops is set up without conditional branches. This enables the
           // compiler to work on several iterations of the loop at the same time; an
           // optimisation known as 'loop unrolling' or 'modulo scheduling'. This, with a good
           // basic block scheduler, should keep some component of the machine busy
           // 100% of the time; it will either be the FPUs, the load/store unit, or the dispatcher;
           // on POWER3, I think the limit is on FPUs, giing 2 FP ops every cycle.
           int
           Produce( const XYZ & aXYZ,
                    const XYZ & eXYZ,
                    double      e0,
                    int         qstart,
                    int         qend )
             {
             double x0 = aXYZ.mX ;
             double px = p.mX ;
             double kx = k.mX ;
             /*
              * Slice for slab
              */
         #pragma execution_frequency(very_low)
             remainder xr[k_FragCount] ;
             remainder yr[k_FragCount] ;
             int q1 = 0 ;
             for (int a0=qstart; a0<qend; a0+=1 )
               {
                 double dx = NearestDistanceInFullyPeriodicLine(x0,x[a0],px,kx) ;
                 double em = e0 + e[a0] ;
                 double ex2 = em*em - dx*dx ;
                 xr[q1].e2 = ex2 ;
                 xr[q1].a = a0 ;
                 double FragmentIndexAdd = fsel(ex2,1.0,0.0) ;
                 int IndexAdd = FragmentIndexAdd ;
                 q1 += IndexAdd ;
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG )
                   << "NeighbourList::Produce X"
                   << " IndexAdd " << IndexAdd
                   << " a0 " << a0
                   << " x0 " << x0
                   << " x " << x[a0]
                   << " dx " << dx
                   << " e0 " << e0
                   << " ex2 " << ex2
                   << " q1 " << q1
                   << EndLogLine;
               } /* endfor */
             BegLogLine( PKFXLOG_NSQSOURCEFRAG_SUMMARY1 )
               << "NeighbourList::Produce X Summary"
               << " q1 " << q1
               << EndLogLine ;

             double y0 = aXYZ.mY ;
             double py = p.mY ;
             double ky = k.mY ;
             /*
              * Slice for cylinder
              */
             int q2 = 0 ;
             for (int b1=0; b1<q1; b1+=1 )
               {
                 int a1 = xr[b1].a ;
                 double dy = NearestDistanceInFullyPeriodicLine(y0,y[a1],py,ky) ;
                 double ey2 = xr[b1].e2 - dy*dy ;
                 yr[q2].e2 = ey2 ;
                 yr[q2].a = a1 ;
                 double FragmentIndexAdd = fsel(ey2,1.0,0.0) ;
                 int IndexAdd = FragmentIndexAdd ;
                 q2 += IndexAdd ;
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG )
                   << "NeighbourList::Produce Y"
                   << " IndexAdd " << IndexAdd
                   << " a1 " << a1
                   << " y0 " << y0
                   << " y " << y[a1]
                   << " dy " << dy
                   << " e2 " << xr[b1].e2
                   << " ey2 " << ey2
                   << " q2 " << q2
                   << EndLogLine;
               } /* endfor */

             double z0 = aXYZ.mZ ;
             double pz = p.mZ ;
             double kz = k.mZ ;
             /*
              * Slice for sphere
              */
             int q3 = 0 ;
             for (int b2=0; b2<q2; b2+=1 )
               {
                 int a2 = yr[b2].a ;
                 double dz = NearestDistanceInFullyPeriodicLine(z0,z[a2],pz,kz) ;
                 double ez2 = yr[b2].e2 - dz*dz ;
                 result[q3] = a2 ;
                 double FragmentIndexAdd = fsel(ez2,1.0,0.0) ;
                 int IndexAdd = FragmentIndexAdd ;
                 q3 += IndexAdd ;
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG )
                   << "NeighbourList::Produce Z"
                   << " IndexAdd " << IndexAdd
                   << " a2 " << a2
                   << " z0 " << z0
                   << " z " << z[a2]
                   << " dz " << dz
                   << " e2 " << xr[b2].e2
                   << " ez2 " << ez2
                   << " q3 " << q3
                   << EndLogLine;
               } /* endfor */

             BegLogLine( PKFXLOG_NSQSOURCEFRAG_SUMMARY1 )
               << "NeighbourList::Produce Z Summary"
               << " q3 " << q3
               << EndLogLine ;
             return q3 ;
             }

           // This is the algorithm set up for integers, using the implied wrap at 2**32 to operate
           // the periodic boundary condition.
           // We slice for slab (same as floating-point version); then for square prism
           // (becuse slicing for cylinder would take more computation); then for cube;
           // then as a final pass we slice for sphere using the floating-point algorithm, because
           // we can discard some out-of-range fragments here, and it is cheaper to do so
           // than to let them through to IFP.
           int
           iProduce( const XYZ & aXYZ,
                     const XYZ & eXYZ,
                     double      e0,
                     int         qstart,
                     int         qend )
             {
             const double tp32 = 1024.0*1024.0*1024.0*4.0 ;
             int aix = FracScale(aXYZ.mX,k.mX) ;
             int aiy = FracScale(aXYZ.mY,k.mY) ;
             int aiz = FracScale(aXYZ.mZ,k.mZ) ;
             int aiex = FracScale(eXYZ.mX,k.mX/(1<<k_ScaleShift))  ;
             int aiey = FracScale(eXYZ.mY,k.mY/(1<<k_ScaleShift))  ;
             int aiez = FracScale(eXYZ.mZ,k.mZ/(1<<k_ScaleShift))  ;

             // BG/L compiler can't seem to handle the manipulators
         #if 0
             BegLogLine( PKFXLOG_NSQSOURCEFRAG )
               << "iProduce aXYZ=" << aXYZ
               << " k=" << k
               << " eXYZ=" << eXYZ
               << " aix=" << hex << aix
               << " aiy=" << hex << aiy
               << " aiz=" << hex << aiz
               << " aiex=" << hex << aiex
               << " aiey=" << hex << aiey
               << " aiez=" << hex << aiez
               << dec
               << EndLogLine ;
         #endif
             /*
              * Slice for slab
              */

             int xr[k_FragCount] ;
             int q1 = 0 ;
             for (int a0=qstart; a0<qend; a0+=1 )
               {

                 int idx = aix - ix[a0] ;    // Difference in 'x' coordinate, scaled on full integer range
                 int idxq = idx >> k_ScaleShift ; // Difference in 'x' coordinate, scaled down to keep away from overflows
                 int iem = (-aiex) - iex[a0] ; // Max difference for things to be worth computing, scaled like idxq
                 int nexp = iem + idxq  ;
                 int nexn = iem - idxq  ;
                 unsigned int nex = nexp & nexn ;  // Negative if both of the above are negative, i.e. in range
                 int IndexAdd = nex >> 31 ;
                 xr[q1] = a0 ;
                 q1 += IndexAdd ;
         #if 0
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG1 )
                   << "NeighbourList::Produce X"
                   << " IndexAdd " << IndexAdd
                   << " a0 " << a0
                   << " idx " << idx/tp32
                   << " iem " << iem/(tp32/(1<<k_ScaleShift))
                   << " nexp " << nexp/(tp32/(1<<k_ScaleShift))
                   << " nexn " << nexn/(tp32/(1<<k_ScaleShift))
                   << " nex " << hex << nex
                   << " q1 " << dec << q1
                   << EndLogLine;
         #endif
               }
             BegLogLine( PKFXLOG_NSQSOURCEFRAG_SUMMARY1 )
               << "NeighbourList::Produce X Summary"
               << " q1 " << q1
               << EndLogLine ;

             /*
              * Slice for square prism
              */
             int yr[k_FragCount] ;
             int q2 = 0 ;
             for (int b1=0; b1<q1; b1+=1 )
               {
                 int a1 = xr[b1] ;
                 int idy = aiy - iy[a1] ;
                 int idyq = idy >> k_ScaleShift ; // Difference in 'y' coordinate, scaled down to keep away from overflows
                 int iem = (-aiey) - iey[a1] ;
                 int neyp = iem - idyq ;
                 int neyn = iem + idyq ;
                 unsigned int ney = neyp & neyn ;
                 int IndexAdd = ney >> 31 ;
                 yr[q2] = a1 ;
                 q2 += IndexAdd ;
         #if 0
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG1 )
                   << "NeighbourList::Produce Y"
                   << " IndexAdd " << IndexAdd
                   << " a1 " << a1
                   << " idy " << idy/tp32
                   << " iem " << iem/(tp32/(1<<k_ScaleShift))
                   << " neyp " << neyp/(tp32/(1<<k_ScaleShift))
                   << " neyn " << neyn/(tp32/(1<<k_ScaleShift))
                   << " ney " << hex << ney
                   << " q2 " << dec << q2
                   << EndLogLine;
         #endif
               } /* endfor */

             /*
              * Slice for cube
              */
             int zr[k_FragCount] ;
             int q3 = 0 ;
             for (int b2=0; b2<q2; b2+=1 )
               {
                 int a2 = yr[b2] ;
                 int idz = aiz - iz[a2] ;
                 int idzq = idz >> k_ScaleShift ; // Difference in 'z' coordinate, scaled down to keep away from overflows
                 int iem = (-aiez) - iez[a2] ;
                 int nezp = iem - idzq ;
                 int nezn = iem + idzq ;
                 unsigned int nez = nezp & nezn ;
                 int IndexAdd = nez >> 31 ;
                 zr[q3] = a2 ;
                 q3 += IndexAdd ;
         #if 0
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG1 )
                   << "NeighbourList::Produce Z"
                   << " IndexAdd " << IndexAdd
                   << " a2 " << a2
                   << " idz " << idz/tp32
                   << " iem " << iem/(tp32/(1<<k_ScaleShift))
                   << " nezp " << nezp/(tp32/(1<<k_ScaleShift))
                   << " nezn " << nezn/(tp32/(1<<k_ScaleShift))
                   << " nez " << hex << nez
                   << " q3 " << dec << q3
                   << EndLogLine;
         #endif
               } /* endfor */

             /*
              * Examine cuboid for sphere
              */
             int q4 = 0 ;
             double x0=aXYZ.mX ;
             double y0=aXYZ.mY ;
             double z0=aXYZ.mZ ;
             double px = p.mX ;
             double py = p.mY ;
             double pz = p.mZ ;
             double kx = k.mX ;
             double ky = k.mY ;
             double kz = k.mZ ;
             for (int b3=0; b3<q3 ; b3+=1)
               {
                 int a3 = zr[b3] ;
                 double dx = NearestDistanceInFullyPeriodicLine(x0,x[a3],px,kx) ;
                 double dy = NearestDistanceInFullyPeriodicLine(y0,y[a3],py,ky) ;
                 double dz = NearestDistanceInFullyPeriodicLine(z0,z[a3],pz,kz) ;
                 double em = e0 + e[a3] ;
                 double ex2 = em*em - dx*dx - dy*dy - dz*dz ;
                 result[q4] = a3 ;
                 double FragmentIndexAdd = fsel(ex2,1.0,0.0) ;
                 int IndexAdd = FragmentIndexAdd ;
                 q4 += IndexAdd ;
         #if 0
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG1 )
                   << "NeighbourList::Produce S"
                   << " IndexAdd " << IndexAdd
                   << " a3 " << a3
                   << " dx " << dx
                   << " dy " << dy
                   << " dz " << dz
                   << " em " << em
                   << " ex2 " << ex2
                   << " q4 " << q4
                   << EndLogLine;
         #endif
               } /* endfor */
             BegLogLine( PKFXLOG_NSQSOURCEFRAG_SUMMARY1 )
               << "NeighbourList::Produce S Summary"
               << " q4 " << q4
               << EndLogLine ;

             return q4 ;

             }

           // The BlueGene/L compiler didn't seem able to handle the manipulators
           // (some of the integers need functional logging as hex, to make sense of what is
           // going on) so a separate 'logged' version of this function made sense.
           int
           iProduce_logged( const XYZ & aXYZ,
                            const XYZ & eXYZ,
                            double      e0,
                            int         qstart,
                            int         qend )
             {
             const double tp32 = 1024.0*1024.0*1024.0*4.0 ;
             int aix = FracScale_logged(aXYZ.mX,k.mX) ;
             int aiy = FracScale_logged(aXYZ.mY,k.mY) ;
             int aiz = FracScale_logged(aXYZ.mZ,k.mZ) ;
             //    int aiex = FracScale_logged(e0,k.mX) >> k_ScaleShift ;
             //    int aiey = FracScale_logged(e0,k.mY) >> k_ScaleShift ;
             //    int aiez = FracScale_logged(e0,k.mZ) >> k_ScaleShift ;
             int aiex = FracScale_logged(eXYZ.mX,k.mX/(1<<k_ScaleShift))  ;
             int aiey = FracScale_logged(eXYZ.mY,k.mY/(1<<k_ScaleShift))  ;
             int aiez = FracScale_logged(eXYZ.mZ,k.mZ/(1<<k_ScaleShift))  ;

         #if 0
             BegLogLine( 0 )
               << "iProduce aXYZ=" << aXYZ
               << " k=" << k
               << " eXYZ=" << eXYZ
               << " aix=" << hex << aix
               << " aiy=" << hex << aiy
               << " aiz=" << hex << aiz
               << " aiex=" << hex << aiex
               << " aiey=" << hex << aiey
               << " aiez=" << hex << aiez
               << dec
               << EndLogLine ;
         #endif
             /*
              * Slice for slab
              */

             int xr[k_FragCount] ;
             int q1 = 0 ;
             for (int a0=qstart; a0<qend; a0+=1 )
               {
                 int idx = aix - ix[a0] ;    // Difference in 'x' coordinate, scaled on full integer range
                 int idxq = idx >> k_ScaleShift ; // Difference in 'x' coordinate, scaled down to keep away from overflows
                 int iem = (-aiex) - iex[a0] ; // Max difference for things to be worth computing, scaled like idxq
                 int nexp = iem + idxq  ;
                 int nexn = iem - idxq  ;
                 unsigned int nex = nexp & nexn ;  // Negative if both of the above are negative, i.e. in range
                 int IndexAdd = nex >> 31 ;
                 xr[q1] = a0 ;
                 q1 += IndexAdd ;
         #if 0
                 BegLogLine( 1 )
                   << "NeighbourList::Produce X"
                   << " IndexAdd " << IndexAdd
                   << " a0 " << a0
                   << " idx " << idx/tp32
                   << " iem " << iem/(tp32/(1<<k_ScaleShift))
                   << " nexp " << nexp/(tp32/(1<<k_ScaleShift))
                   << " nexn " << nexn/(tp32/(1<<k_ScaleShift))
                   << " nex " << hex << nex
                   << " q1 " << dec << q1
                   << EndLogLine;
         #endif
               }
             BegLogLine( 1 )
               << "NeighbourList::Produce X Summary"
               << " q1 " << q1
               << EndLogLine ;

             /*
              * Slice for square prism
              */
             int yr[k_FragCount] ;
             int q2 = 0 ;
             for (int b1=0; b1<q1; b1+=1 )
               {
                 int a1 = xr[b1] ;
                 int idy = aiy - iy[a1] ;
                 int idyq = idy >> k_ScaleShift ; // Difference in 'y' coordinate, scaled down to keep away from overflows
                 int iem = (-aiey) - iey[a1] ;
                 int neyp = iem - idyq ;
                 int neyn = iem + idyq ;
                 unsigned int ney = neyp & neyn ;
                 int IndexAdd = ney >> 31 ;
                 yr[q2] = a1 ;
                 q2 += IndexAdd ;
         #if 0
                 BegLogLine( 1 )
                   << "NeighbourList::Produce Y"
                   << " IndexAdd " << IndexAdd
                   << " a1 " << a1
                   << " idy " << idy/tp32
                   << " iem " << iem/(tp32/(1<<k_ScaleShift))
                   << " neyp " << neyp/(tp32/(1<<k_ScaleShift))
                   << " neyn " << neyn/(tp32/(1<<k_ScaleShift))
                   << " ney " << hex << ney
                   << " q2 " << dec << q2
                   << EndLogLine;
         #endif
               } /* endfor */
             //    BegLogLine( 1 )
             //      << "NeighbourList::Produce Y Summary"
             //      << " q2 " << q2
             //      << EndLogLine ;

             /*
              * Slice for cube
              */
             int zr[k_FragCount] ;
             int q3 = 0 ;
             for (int b2=0; b2<q2; b2+=1 )
               {
                 int a2 = yr[b2] ;
                 int idz = aiz - iz[a2] ;
                 int idzq = idz >> k_ScaleShift ; // Difference in 'z' coordinate, scaled down to keep away from overflows
                 int iem = (-aiez) - iez[a2] ;
                 int nezp = iem - idzq ;
                 int nezn = iem + idzq ;
                 unsigned int nez = nezp & nezn ;
                 int IndexAdd = nez >> 31 ;
                 zr[q3] = a2 ;
                 q3 += IndexAdd ;
         #if 0
                 BegLogLine( 1 )
                   << "NeighbourList::Produce Z"
                   << " IndexAdd " << IndexAdd
                   << " a2 " << a2
                   << " idz " << idz/tp32
                   << " iem " << iem/(tp32/(1<<k_ScaleShift))
                   << " nezp " << nezp/(tp32/(1<<k_ScaleShift))
                   << " nezn " << nezn/(tp32/(1<<k_ScaleShift))
                   << " nez " << hex << nez
                   << " q3 " << dec << q3
                   << EndLogLine;
         #endif
               } /* endfor */
             //    BegLogLine( 1 )
             //      << "NeighbourList::Produce Z Summary"
             //      << " q3 " << q3
             //      << EndLogLine ;

             /*
              * Examine cuboid for sphere
              */
             int q4 = 0 ;
             double x0=aXYZ.mX ;
             double y0=aXYZ.mY ;
             double z0=aXYZ.mZ ;
             double px = p.mX ;
             double py = p.mY ;
             double pz = p.mZ ;
             double kx = k.mX ;
             double ky = k.mY ;
             double kz = k.mZ ;
             for (int b3=0; b3<q3 ; b3+=1)
               {
                 int a3 = zr[b3] ;
                 double dx = NearestDistanceInFullyPeriodicLine(x0,x[a3],px,kx) ;
                 double dy = NearestDistanceInFullyPeriodicLine(y0,y[a3],py,ky) ;
                 double dz = NearestDistanceInFullyPeriodicLine(z0,z[a3],pz,kz) ;
                 double em = e0 + e[a3] ;
                 double ex2 = em*em - dx*dx - dy*dy - dz*dz ;
                 result[q4] = a3 ;
                 double FragmentIndexAdd = fsel(ex2,1.0,0.0) ;
                 int IndexAdd = FragmentIndexAdd ;
                 q4 += IndexAdd ;
         #if 0
                 BegLogLine( 1 )
                   << "NeighbourList::Produce S"
                   << " IndexAdd " << IndexAdd
                   << " a3 " << a3
                   << " dx " << dx
                   << " dy " << dy
                   << " dz " << dz
                   << " em " << em
                   << " ex2 " << ex2
                   << " q4 " << q4
                   << EndLogLine;
         #endif
               } /* endfor */
             BegLogLine( 1 )
               << "NeighbourList::Produce S Summary"
               << " q4 " << q4
               << EndLogLine ;

             return q4 ;

             }

           // This is how to set up what we need to know about each fragment
           // 'Any' fragment bounding box, and 'any' distance not smaller than the distance of
           // the furthest atom, will get a functionally correct result; but it is worth going to some
           // trouble to get the 'minimum' ones because this function is called N times, but it is
           // used to reduce the multiplier in the O(N*N) evaluations that will follow.
           // It will be worth using 'dynamic' extents (i.e. the actual value at the timestep), rather
           // than the 'static' pessimistic ones in the MSD, becuase of this reductive effect.
           void
           SetXYZE( int         q,     // The fragment sequence number
                    const XYZ & aXYZ,  // The location of the centre of the min bounding box
                    double      ae,    // The square of the distance of the furthest atom from the centre
                    const XYZ & eXYZ ) // The vector from the centre to the corner of the min bounding box
             {
             // The compiler seemed concerned that aXYZ and eXYZ might alias something
             // in the NeighbourList. Reassure it that this will not happen.
             double aX = aXYZ.mX ;
             double aY = aXYZ.mY ;
             double aZ = aXYZ.mZ ;

             double eX = eXYZ.mX ;
             double eY = eXYZ.mY ;
             double eZ = eXYZ.mZ ;

             double kX = k.mX ;
             double kY = k.mY ;
             double kZ = k.mZ ;

             x[q] = aX ;
             y[q] = aY ;
             z[q] = aZ ;
             ex[q] = eX ;
             ey[q] = eY ;
             ez[q] = eZ ;
             e[q] = ae;

             ix[q] = FracScale(aX,kX) ;
             iy[q] = FracScale(aY,kY) ;
             iz[q] = FracScale(aZ,kZ) ;
             iex[q] = FracScale(eX,kX/(1<<k_ScaleShift)) ;
             iey[q] = FracScale(eY,kY/(1<<k_ScaleShift)) ;
             iez[q] = FracScale(eZ,kZ/(1<<k_ScaleShift)) ;

             BegLogLine( PKFXLOG_NSQSOURCEFRAG_SUMMARY )
               << "NeighbourList::SetXYZE q=" << q
               << " aXYZ=" << aXYZ
               << " ae=" << ae
               << " eXYZ=" << eXYZ
               << EndLogLine ;
             }

           // We were recording square distances, but we need actual distances.
           // We have a 'fix' to ensure that we never try to square root 0, so we can use the
           // faster 'X/sqrt(X)' form
           void
           CompleteXYZE( int qcount)
             {
             double e0 = e[0] ;
             double e1 = e[1] ;
             double e2 = e[2] ;
             double e3 = e[3] ;
             double e4 = e[4] ;
             double e5 = e[5] ;
             double e6 = e[6] ;
             double e7 = e[7] ;
             double e8 = e[8] ;
             double e9 = e[9] ;
             for ( int q=0;q<qcount;q+=10)
               {
                 double r0 = e0/sqrt(e0) ;
                 double r1 = e1/sqrt(e1) ;
                 double r2 = e2/sqrt(e2) ;
                 double r3 = e3/sqrt(e3) ;
                 double r4 = e4/sqrt(e4) ;
                 double r5 = e5/sqrt(e5) ;
                 double r6 = e6/sqrt(e6) ;
                 double r7 = e7/sqrt(e7) ;
                 double r8 = e8/sqrt(e8) ;
                 double r9 = e9/sqrt(e9) ;
                 e0 = e[q+10] ;
                 e1 = e[q+11] ;
                 e2 = e[q+12] ;
                 e3 = e[q+13] ;
                 e4 = e[q+14] ;
                 e5 = e[q+15] ;
                 e6 = e[q+16] ;
                 e7 = e[q+17] ;
                 e8 = e[q+18] ;
                 e9 = e[q+19] ;
                 e[q+0] = r0 ;
                 e[q+1] = r1 ;
                 e[q+2] = r2 ;
                 e[q+3] = r3 ;
                 e[q+4] = r4 ;
                 e[q+5] = r5 ;
                 e[q+6] = r6 ;
                 e[q+7] = r7 ;
                 e[q+8] = r8 ;
                 e[q+9] = r9 ;
               }
             }

         double
         GetFragmentExtent(int q) const
           {
           return e[q] ;
           }

         XYZ
         GetCorner(int q) const
           {
           XYZ r ;
           r.mX = ex[q] ;
           r.mY = ey[q] ;
           r.mZ = ez[q] ;
           return r ;
           }

         XYZ
         GetFragmentCentre(int q) const
           {
           XYZ r ;
           r.mX = x[q] ;
           r.mY = y[q] ;
           r.mZ = z[q] ;
           return r ;
           }

         int
         Get(int q) const
           {
           return result[q] ;
           }

         };


#else // PHASE4 Defined
         // A Neighbour list is able to produce, for a fragment, a list of those fragments which are
         // close enough in a periodic image sense that at least one of the atom pairs is within range
         // of the nonbond force cutoff. (Trivially you could produce a list of all the fragments, but
         // that would lead to poor performance as IFP would get asked to process everything;
         // the point of this is to produce the smallest correct list that is reasonably easy to compute)

         // To do this, we have to set up with a list of all the fragments; we keep the minimum
         // bounding box for the fragment, specifically we keep the centre of this box, and the
         // vector from the centre to the top right corner (i.e. the one with all elements positive)
         // We also keep the distance from the centre to the furthest atom in the fragment, as
         // this will enable us to discard some fragment pairs at 'NSQ' stage in some geometrical
         // configurations.

         // Integer versions (generally, the vector as a fraction of the bounding box multiplied
         // by 2**24) are also kept; the algorithm can be run either in integers or doubles, and
         // the fastest one varies according to the machine, the compiler, and the phase of the moon :-)


         // Having set this list up, we can produce on demand the sublist for those fragments
         // in range of a given fragment, as an array; and we will be able to drive 'IFP' as a
         // simple 'for' loop indexing the array.
         #ifndef PKFXLOG_NSQSOURCEFRAG
         #define PKFXLOG_NSQSOURCEFRAG ( 0 )
         #endif

         #ifndef PKFXLOG_NSQSOURCEFRAG1
         #define PKFXLOG_NSQSOURCEFRAG1 ( 0 )
         #endif

         #ifndef PKFXLOG_NSQSOURCEFRAG2
         #define PKFXLOG_NSQSOURCEFRAG2 ( 0 )
         #endif

         #ifndef PKFXLOG_NSQSOURCEFRAG_SUMMARY
         #define PKFXLOG_NSQSOURCEFRAG_SUMMARY ( 0 )
         #endif

         #ifndef PKFXLOG_NSQSOURCEFRAG_SUMMARY1
         #define PKFXLOG_NSQSOURCEFRAG_SUMMARY1 ( 0 )
         #endif

         static inline unsigned int FracScale(double n, double rd);

         static inline double NearestDistanceInFullyPeriodicLine(
           const double a
           , const double b
           , const double Period
           , const double ReciprocalPeriod
           );

         class NeighbourList
         {
         public:
           class remainder
           {
           public:
             double e2 ;
             int    a  ;
             int dummy ;
           } ;

           enum
           {
             k_PadCount = 20 , // Amount of padding when we want to overcompute for vectorised square roots
             k_FragCount = NUMBER_OF_FRAGMENTS
           };

           enum
           {
             k_ScaleShift = 8
           };

           XYZ p ;
           XYZ k ;

           double x[k_FragCount] ;
           double y[k_FragCount] ;
           double z[k_FragCount] ;

           double e[k_FragCount+k_PadCount] ;
           double ex[k_FragCount] ;
           double ey[k_FragCount] ;
           double ez[k_FragCount] ;

           int ix[k_FragCount] ;
           int iy[k_FragCount] ;
           int iz[k_FragCount] ;
           int iex[k_FragCount] ;
           int iey[k_FragCount] ;
           int iez[k_FragCount] ;

           int result[k_FragCount] ;

           NeighbourList(const XYZ& ap, const XYZ& ak): p(ap), k(ak) {
             BegLogLine(PKFXLOG_NSQSOURCEFRAG_SUMMARY)
               << "NeighbourList( p=" << p
               << " k=" << k
               << EndLogLine ;
           } ;

           NeighbourList(){}

           void Setup( XYZ& ap, XYZ& ak)
             {
               p = ap;
               k = ak;
             }

           // This is the 'trivial, diagnostic' version; it considers that all fragment pairs are
           // in range. IFP will go through all atom pairs, very expensively, and get the same result
           // assuming everything is working.
           int ProduceAll(
                          const XYZ& aXYZ
                          , const XYZ& eXYZ
                          , double e0
                          , int qstart
                          , int qend
                          ) {
             int q0 = 0 ;
             for (int a0=qstart; a0<qend; a0+=1 )
               {
                 result[q0] = a0 ;
                 q0 += 1 ;
               }
             return q0 ;
           }

           // This is the floating-point version. First it goes through all the fragments in the X
           // dimension, to produce all those within a slab of the wanted one.
           // Then it goes through the slab in the Y dimension, to produce all those within
           // a cylinder of the wanted one.
           // Then it goes through the cylinder in the Z dimension, to produce all those
           // within a sphere of the wanted one; which is what we wanted in the first place.
           // Each of these loops is set up without conditional branches. This enables the
           // compiler to work on several iterations of the loop at the same time; an
           // optimisation known as 'loop unrolling' or 'modulo scheduling'. This, with a good
           // basic block scheduler, should keep some component of the machine busy
           // 100% of the time; it will either be the FPUs, the load/store unit, or the dispatcher;
           // on POWER3, I think the limit is on FPUs, giing 2 FP ops every cycle.
           int Produce(
                       const XYZ& aXYZ
                       , const XYZ& eXYZ
                       , double e0
                       , int qstart
                       , int qend
                       ) {
             double x0 = aXYZ.mX ;
             double px = p.mX ;
             double kx = k.mX ;
             /*
              * Slice for slab
              */
         #pragma execution_frequency(very_low)
             remainder xr[k_FragCount] ;
             remainder yr[k_FragCount] ;
             int q1 = 0 ;
             for (int a0=qstart; a0<qend; a0+=1 )
               {
                 double dx = NearestDistanceInFullyPeriodicLine(x0,x[a0],px,kx) ;
                 double em = e0 + e[a0] ;
                 double ex2 = em*em - dx*dx ;
                 xr[q1].e2 = ex2 ;
                 xr[q1].a = a0 ;
                 double FragmentIndexAdd = fsel(ex2,1.0,0.0) ;
                 int IndexAdd = FragmentIndexAdd ;
                 q1 += IndexAdd ;
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG )
                   << "NeighbourList::Produce X"
                   << " IndexAdd " << IndexAdd
                   << " a0 " << a0
                   << " x0 " << x0
                   << " x " << x[a0]
                   << " dx " << dx
                   << " e0 " << e0
                   << " ex2 " << ex2
                   << " q1 " << q1
                   << EndLogLine;
               } /* endfor */
             BegLogLine( PKFXLOG_NSQSOURCEFRAG_SUMMARY1 )
               << "NeighbourList::Produce X Summary"
               << " q1 " << q1
               << EndLogLine ;

             double y0 = aXYZ.mY ;
             double py = p.mY ;
             double ky = k.mY ;
             /*
              * Slice for cylinder
              */
             int q2 = 0 ;
             for (int b1=0; b1<q1; b1+=1 )
               {
                 int a1 = xr[b1].a ;
                 double dy = NearestDistanceInFullyPeriodicLine(y0,y[a1],py,ky) ;
                 double ey2 = xr[b1].e2 - dy*dy ;
                 yr[q2].e2 = ey2 ;
                 yr[q2].a = a1 ;
                 double FragmentIndexAdd = fsel(ey2,1.0,0.0) ;
                 int IndexAdd = FragmentIndexAdd ;
                 q2 += IndexAdd ;
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG )
                   << "NeighbourList::Produce Y"
                   << " IndexAdd " << IndexAdd
                   << " a1 " << a1
                   << " y0 " << y0
                   << " y " << y[a1]
                   << " dy " << dy
                   << " e2 " << xr[b1].e2
                   << " ey2 " << ey2
                   << " q2 " << q2
                   << EndLogLine;
               } /* endfor */

             double z0 = aXYZ.mZ ;
             double pz = p.mZ ;
             double kz = k.mZ ;
             /*
              * Slice for sphere
              */
             int q3 = 0 ;
             for (int b2=0; b2<q2; b2+=1 )
               {
                 int a2 = yr[b2].a ;
                 double dz = NearestDistanceInFullyPeriodicLine(z0,z[a2],pz,kz) ;
                 double ez2 = yr[b2].e2 - dz*dz ;
                 result[q3] = a2 ;
                 double FragmentIndexAdd = fsel(ez2,1.0,0.0) ;
                 int IndexAdd = FragmentIndexAdd ;
                 q3 += IndexAdd ;
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG )
                   << "NeighbourList::Produce Z"
                   << " IndexAdd " << IndexAdd
                   << " a2 " << a2
                   << " z0 " << z0
                   << " z " << z[a2]
                   << " dz " << dz
                   << " e2 " << xr[b2].e2
                   << " ez2 " << ez2
                   << " q3 " << q3
                   << EndLogLine;
               } /* endfor */

             BegLogLine( PKFXLOG_NSQSOURCEFRAG_SUMMARY1 )
               << "NeighbourList::Produce Z Summary"
               << " q3 " << q3
               << EndLogLine ;
             return q3 ;
           } ;

           // This is the algorithm set up for integers, using the implied wrap at 2**32 to operate
           // the periodic boundary condition.
           // We slice for slab (same as floating-point version); then for square prism
           // (becuse slicing for cylinder would take more computation); then for cube;
           // then as a final pass we slice for sphere using the floating-point algorithm, because
           // we can discard some out-of-range fragments here, and it is cheaper to do so
           // than to let them through to IFP.
           int iProduce(
                        const XYZ& aXYZ
                        , const XYZ& eXYZ
                        , double e0
                        , int qstart
                        , int qend
                        ) {
             const double tp32 = 1024.0*1024.0*1024.0*4.0 ;
             int aix = FracScale(aXYZ.mX,k.mX) ;
             int aiy = FracScale(aXYZ.mY,k.mY) ;
             int aiz = FracScale(aXYZ.mZ,k.mZ) ;
             int aiex = FracScale(eXYZ.mX,k.mX/(1<<k_ScaleShift))  ;
             int aiey = FracScale(eXYZ.mY,k.mY/(1<<k_ScaleShift))  ;
             int aiez = FracScale(eXYZ.mZ,k.mZ/(1<<k_ScaleShift))  ;

             // BG/L compiler can't seem to handle the manipulators
         #if 0
             BegLogLine( PKFXLOG_NSQSOURCEFRAG )
               << "iProduce aXYZ=" << aXYZ
               << " k=" << k
               << " eXYZ=" << eXYZ
               << " aix=" << hex << aix
               << " aiy=" << hex << aiy
               << " aiz=" << hex << aiz
               << " aiex=" << hex << aiex
               << " aiey=" << hex << aiey
               << " aiez=" << hex << aiez
               << dec
               << EndLogLine ;
         #endif
             /*
              * Slice for slab
              */

             int xr[k_FragCount] ;
             int q1 = 0 ;
             for (int a0=qstart; a0<qend; a0+=1 )
               {
                 int idx = aix - ix[a0] ;    // Difference in 'x' coordinate, scaled on full integer range
                 int idxq = idx >> k_ScaleShift ; // Difference in 'x' coordinate, scaled down to keep away from overflows
                 int iem = (-aiex) - iex[a0] ; // Max difference for things to be worth computing, scaled like idxq
                 int nexp = iem + idxq  ;
                 int nexn = iem - idxq  ;
                 unsigned int nex = nexp & nexn ;  // Negative if both of the above are negative, i.e. in range
                 int IndexAdd = nex >> 31 ;
                 xr[q1] = a0 ;
                 q1 += IndexAdd ;
         #if 0
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG1 )
                   << "NeighbourList::Produce X"
                   << " IndexAdd " << IndexAdd
                   << " a0 " << a0
                   << " idx " << idx/tp32
                   << " iem " << iem/(tp32/(1<<k_ScaleShift))
                   << " nexp " << nexp/(tp32/(1<<k_ScaleShift))
                   << " nexn " << nexn/(tp32/(1<<k_ScaleShift))
                   << " nex " << hex << nex
                   << " q1 " << dec << q1
                   << EndLogLine;
         #endif
               }
             BegLogLine( PKFXLOG_NSQSOURCEFRAG_SUMMARY1 )
               << "NeighbourList::Produce X Summary"
               << " q1 " << q1
               << EndLogLine ;

             /*
              * Slice for square prism
              */
             int yr[k_FragCount] ;
             int q2 = 0 ;
             for (int b1=0; b1<q1; b1+=1 )
               {
                 int a1 = xr[b1] ;
                 int idy = aiy - iy[a1] ;
                 int idyq = idy >> k_ScaleShift ; // Difference in 'y' coordinate, scaled down to keep away from overflows
                 int iem = (-aiey) - iey[a1] ;
                 int neyp = iem - idyq ;
                 int neyn = iem + idyq ;
                 unsigned int ney = neyp & neyn ;
                 int IndexAdd = ney >> 31 ;
                 yr[q2] = a1 ;
                 q2 += IndexAdd ;
         #if 0
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG1 )
                   << "NeighbourList::Produce Y"
                   << " IndexAdd " << IndexAdd
                   << " a1 " << a1
                   << " idy " << idy/tp32
                   << " iem " << iem/(tp32/(1<<k_ScaleShift))
                   << " neyp " << neyp/(tp32/(1<<k_ScaleShift))
                   << " neyn " << neyn/(tp32/(1<<k_ScaleShift))
                   << " ney " << hex << ney
                   << " q2 " << dec << q2
                   << EndLogLine;
         #endif
               } /* endfor */

             /*
              * Slice for cube
              */
             int zr[k_FragCount] ;
             int q3 = 0 ;
             for (int b2=0; b2<q2; b2+=1 )
               {
                 int a2 = yr[b2] ;
                 int idz = aiz - iz[a2] ;
                 int idzq = idz >> k_ScaleShift ; // Difference in 'z' coordinate, scaled down to keep away from overflows
                 int iem = (-aiez) - iez[a2] ;
                 int nezp = iem - idzq ;
                 int nezn = iem + idzq ;
                 unsigned int nez = nezp & nezn ;
                 int IndexAdd = nez >> 31 ;
                 zr[q3] = a2 ;
                 q3 += IndexAdd ;
         #if 0
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG1 )
                   << "NeighbourList::Produce Z"
                   << " IndexAdd " << IndexAdd
                   << " a2 " << a2
                   << " idz " << idz/tp32
                   << " iem " << iem/(tp32/(1<<k_ScaleShift))
                   << " nezp " << nezp/(tp32/(1<<k_ScaleShift))
                   << " nezn " << nezn/(tp32/(1<<k_ScaleShift))
                   << " nez " << hex << nez
                   << " q3 " << dec << q3
                   << EndLogLine;
         #endif
               } /* endfor */

             /*
              * Examine cuboid for sphere
              */
             int q4 = 0 ;
             double x0=aXYZ.mX ;
             double y0=aXYZ.mY ;
             double z0=aXYZ.mZ ;
             double px = p.mX ;
             double py = p.mY ;
             double pz = p.mZ ;
             double kx = k.mX ;
             double ky = k.mY ;
             double kz = k.mZ ;
             for (int b3=0; b3<q3 ; b3+=1)
               {
                 int a3 = zr[b3] ;
                 double dx = NearestDistanceInFullyPeriodicLine(x0,x[a3],px,kx) ;
                 double dy = NearestDistanceInFullyPeriodicLine(y0,y[a3],py,ky) ;
                 double dz = NearestDistanceInFullyPeriodicLine(z0,z[a3],pz,kz) ;
                 double em = e0 + e[a3] ;
                 double ex2 = em*em - dx*dx - dy*dy - dz*dz ;
                 result[q4] = a3 ;
                 double FragmentIndexAdd = fsel(ex2,1.0,0.0) ;
                 int IndexAdd = FragmentIndexAdd ;
                 q4 += IndexAdd ;
         #if 0
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG1 )
                   << "NeighbourList::Produce S"
                   << " IndexAdd " << IndexAdd
                   << " a3 " << a3
                   << " dx " << dx
                   << " dy " << dy
                   << " dz " << dz
                   << " em " << em
                   << " ex2 " << ex2
                   << " q4 " << q4
                   << EndLogLine;
         #endif
               } /* endfor */
             BegLogLine( PKFXLOG_NSQSOURCEFRAG_SUMMARY1 )
               << "NeighbourList::Produce S Summary"
               << " q4 " << q4
               << EndLogLine ;

             return q4 ;

           } ;
           // This is the algorithm set up for integers, using the implied wrap at 2**32 to operate
           // the periodic boundary condition.
           // We slice for slab (same as floating-point version); then for square prism
           // (becuse slicing for cylinder would take more computation); then for cube;
           // then as a final pass we slice for sphere using the floating-point algorithm, because
           // we can discard some out-of-range fragments here, and it is cheaper to do so
           // than to let them through to IFP.
           int iProduceOnListOfFrags(
                        const XYZ& aXYZ
                        , const XYZ& eXYZ
                        , double e0
                        , int* aFragList
                        , int  aStartIndex
                        , int  aFragListLength
                        ) {
             const double tp32 = 1024.0*1024.0*1024.0*4.0 ;
             int aix = FracScale(aXYZ.mX,k.mX) ;
             int aiy = FracScale(aXYZ.mY,k.mY) ;
             int aiz = FracScale(aXYZ.mZ,k.mZ) ;
             int aiex = FracScale(eXYZ.mX,k.mX/(1<<k_ScaleShift))  ;
             int aiey = FracScale(eXYZ.mY,k.mY/(1<<k_ScaleShift))  ;
             int aiez = FracScale(eXYZ.mZ,k.mZ/(1<<k_ScaleShift))  ;

             // BG/L compiler can't seem to handle the manipulators
         #if 1
             BegLogLine( PKFXLOG_NSQSOURCEFRAG )
               << "iProduce aXYZ=" << aXYZ
               << " k=" << k
               << " eXYZ=" << eXYZ
               << " aix=" <<  aix
               << " aiy=" <<  aiy
               << " aiz=" <<  aiz
               << " aiex=" << aiex
               << " aiey=" << aiey
               << " aiez=" << aiez
               << EndLogLine ;
         #endif
             /*
              * Slice for slab
              */

             int xr[k_FragCount] ;
             int q1 = 0 ;

             for( int i=aStartIndex; i < aFragListLength; i++ )
               {
                 int a0 = aFragList[ i ];

                 int idx = aix - ix[a0] ;    // Difference in 'x' coordinate, scaled on full integer range
                 int idxq = idx >> k_ScaleShift ; // Difference in 'x' coordinate, scaled down to keep away from overflows
                 int iem = (-aiex) - iex[a0] ; // Max difference for things to be worth computing, scaled like idxq
                 int nexp = iem + idxq  ;
                 int nexn = iem - idxq  ;
                 unsigned int nex = nexp & nexn ;  // Negative if both of the above are negative, i.e. in range
                 int IndexAdd = nex >> 31 ;
                 xr[q1] = a0 ;
                 q1 += IndexAdd ;
         #if 1
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG1 )
                   << "NeighbourList::Produce X"
                   << " IndexAdd " << IndexAdd
                   << " a0 " << a0
                   << " idx " << idx/tp32
                   << " iem " << iem/(tp32/(1<<k_ScaleShift))
                   << " nexp " << nexp/(tp32/(1<<k_ScaleShift))
                   << " nexn " << nexn/(tp32/(1<<k_ScaleShift))
                   << " nex " << nex
                   << " q1 " << q1
                   << EndLogLine;
         #endif
               }
             BegLogLine( PKFXLOG_NSQSOURCEFRAG_SUMMARY1 )
               << "NeighbourList::Produce X Summary"
               << " q1 " << q1
               << EndLogLine ;

             /*
              * Slice for square prism
              */
             int yr[k_FragCount] ;
             int q2 = 0 ;
             for (int b1=0; b1<q1; b1+=1 )
               {
                 int a1 = xr[b1] ;
                 int idy = aiy - iy[a1] ;
                 int idyq = idy >> k_ScaleShift ; // Difference in 'y' coordinate, scaled down to keep away from overflows
                 int iem = (-aiey) - iey[a1] ;
                 int neyp = iem - idyq ;
                 int neyn = iem + idyq ;
                 unsigned int ney = neyp & neyn ;
                 int IndexAdd = ney >> 31 ;
                 yr[q2] = a1 ;
                 q2 += IndexAdd ;
         #if 1
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG1 )
                   << "NeighbourList::Produce Y"
                   << " IndexAdd " << IndexAdd
                   << " a1 " << a1
                   << " idy " << idy/tp32
                   << " iem " << iem/(tp32/(1<<k_ScaleShift))
                   << " neyp " << neyp/(tp32/(1<<k_ScaleShift))
                   << " neyn " << neyn/(tp32/(1<<k_ScaleShift))
                   << " ney " << ney
                   << " q2 " << q2
                   << EndLogLine;
         #endif
               } /* endfor */

             /*
              * Slice for cube
              */
             int zr[k_FragCount] ;
             int q3 = 0 ;
             for (int b2=0; b2<q2; b2+=1 )
               {
                 int a2 = yr[b2] ;
                 int idz = aiz - iz[a2] ;
                 int idzq = idz >> k_ScaleShift ; // Difference in 'z' coordinate, scaled down to keep away from overflows
                 int iem = (-aiez) - iez[a2] ;
                 int nezp = iem - idzq ;
                 int nezn = iem + idzq ;
                 unsigned int nez = nezp & nezn ;
                 int IndexAdd = nez >> 31 ;
                 zr[q3] = a2 ;
                 q3 += IndexAdd ;
         #if 1
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG1 )
                   << "NeighbourList::Produce Z"
                   << " IndexAdd " << IndexAdd
                   << " a2 " << a2
                   << " idz " << idz/tp32
                   << " iem " << iem/(tp32/(1<<k_ScaleShift))
                   << " nezp " << nezp/(tp32/(1<<k_ScaleShift))
                   << " nezn " << nezn/(tp32/(1<<k_ScaleShift))
                   << " nez " << nez
                   << " q3 " << q3
                   << EndLogLine;
         #endif
               } /* endfor */

             /*
              * Examine cuboid for sphere
              */
             int q4 = 0 ;
             double x0=aXYZ.mX ;
             double y0=aXYZ.mY ;
             double z0=aXYZ.mZ ;
             double px = p.mX ;
             double py = p.mY ;
             double pz = p.mZ ;
             double kx = k.mX ;
             double ky = k.mY ;
             double kz = k.mZ ;
             for (int b3=0; b3<q3 ; b3+=1)
               {
                 int a3 = zr[b3] ;
                 double dx = NearestDistanceInFullyPeriodicLine(x0,x[a3],px,kx) ;
                 double dy = NearestDistanceInFullyPeriodicLine(y0,y[a3],py,ky) ;
                 double dz = NearestDistanceInFullyPeriodicLine(z0,z[a3],pz,kz) ;
                 double em = e0 + e[a3] ;
                 double ex2 = em*em - dx*dx - dy*dy - dz*dz ;
                 result[q4] = a3 ;
                 double FragmentIndexAdd = fsel(ex2,1.0,0.0) ;
                 int IndexAdd = FragmentIndexAdd ;
                 q4 += IndexAdd ;
         #if 1
                 BegLogLine( PKFXLOG_NSQSOURCEFRAG2 )
                   << "NeighbourList::Produce S"
                   << " IndexAdd " << IndexAdd
                   << " a3 " << a3
                   << " dx " << dx
                   << " dy " << dy
                   << " dz " << dz
                   << " em " << em
                   << " ex2 " << ex2
                   << " q4 " << q4
                   << EndLogLine;
         #endif
               } /* endfor */
             BegLogLine( PKFXLOG_NSQSOURCEFRAG_SUMMARY1 )
               << "NeighbourList::Produce S Summary"
               << " q4 " << q4
               << EndLogLine ;

             return q4 ;

           } ;

           // void SetXYZE(
           //     int q
           //   , const XYZ& aXYZ
           //   , double ae
           // ) {
           //      x[q] = aXYZ.mX ;
           //      y[q] = aXYZ.mY ;
           //      z[q] = aXYZ.mZ ;
           //      e[q] = ae;
           //
           //      ix[q] = FracScale(aXYZ.mX,k.mX) ;
           //      iy[q] = FracScale(aXYZ.mY,k.mY) ;
           //      iz[q] = FracScale(aXYZ.mZ,k.mZ) ;
           //      iex[q] = FracScale(ae,k.mX) >> k_ScaleShift ;
           //      iey[q] = FracScale(ae,k.mY) >> k_ScaleShift ;
           //      iez[q] = FracScale(ae,k.mZ) >> k_ScaleShift ;
           //
           //    } ;

           // This is how to set up what we need to know about each fragment
           // 'Any' fragment bounding box, and 'any' distance not smaller than the distance of
           // the furthest atom, will get a functionally correct result; but it is worth going to some
           // trouble to get the 'minimum' ones because this function is called N times, but it is
           // used to reduce the multiplier in the O(N*N) evaluations that will follow.
           // It will be worth using 'dynamic' extents (i.e. the actual value at the timestep), rather
           // than the 'static' pessimistic ones in the MSD, becuase of this reductive effect.
           void SetXYZE(
                        int q                                                    // The fragment sequence number
                        , const XYZ& aXYZ        // The location of the centre of the min bounding box
                        , double ae                              // The square of the distance of the furthest atom from the centre
                        , const XYZ& eXYZ  // The vector from the centre to the corner of the min bounding box
                        ) {
             // The compiler seemed concerned that aXYZ and eXYZ might alias something
             // in the NeighbourList. Reassure it that this will not happen.
             double aX = aXYZ.mX ;
             double aY = aXYZ.mY ;
             double aZ = aXYZ.mZ ;

             double eX = eXYZ.mX ;
             double eY = eXYZ.mY ;
             double eZ = eXYZ.mZ ;

             double kX = k.mX ;
             double kY = k.mY ;
             double kZ = k.mZ ;

             x[q] = aX ;
             y[q] = aY ;
             z[q] = aZ ;
             ex[q] = eX ;
             ey[q] = eY ;
             ez[q] = eZ ;
             e[q] = ae;

             ix[q] = FracScale(aX,kX) ;
             iy[q] = FracScale(aY,kY) ;
             iz[q] = FracScale(aZ,kZ) ;
             iex[q] = FracScale(eX,kX/(1<<k_ScaleShift)) ;
             iey[q] = FracScale(eY,kY/(1<<k_ScaleShift)) ;
             iez[q] = FracScale(eZ,kZ/(1<<k_ScaleShift)) ;

             BegLogLine( PKFXLOG_NSQSOURCEFRAG_SUMMARY )
               << "NeighbourList::SetXYZE q=" << q
               << " aXYZ=" << aXYZ
               << " ae=" << ae
               << " eXYZ=" << eXYZ
               << EndLogLine ;
           } ;
           // We were recording square distances, but we need actual distances.
           // We have a 'fix' to ensure that we never try to square root 0, so we can use the
           // faster 'X/sqrt(X)' form
           void CompleteXYZE( int qcount)
             {
               double e0 = e[0] ;
               double e1 = e[1] ;
               double e2 = e[2] ;
               double e3 = e[3] ;
               double e4 = e[4] ;
               double e5 = e[5] ;
               double e6 = e[6] ;
               double e7 = e[7] ;
               double e8 = e[8] ;
               double e9 = e[9] ;
               for ( int q=0;q<qcount;q+=10)
                 {
                   double r0 = e0/sqrt(e0) ;
                   double r1 = e1/sqrt(e1) ;
                   double r2 = e2/sqrt(e2) ;
                   double r3 = e3/sqrt(e3) ;
                   double r4 = e4/sqrt(e4) ;
                   double r5 = e5/sqrt(e5) ;
                   double r6 = e6/sqrt(e6) ;
                   double r7 = e7/sqrt(e7) ;
                   double r8 = e8/sqrt(e8) ;
                   double r9 = e9/sqrt(e9) ;
                   e0 = e[q+10] ;
                   e1 = e[q+11] ;
                   e2 = e[q+12] ;
                   e3 = e[q+13] ;
                   e4 = e[q+14] ;
                   e5 = e[q+15] ;
                   e6 = e[q+16] ;
                   e7 = e[q+17] ;
                   e8 = e[q+18] ;
                   e9 = e[q+19] ;
                   e[q+0] = r0 ;
                   e[q+1] = r1 ;
                   e[q+2] = r2 ;
                   e[q+3] = r3 ;
                   e[q+4] = r4 ;
                   e[q+5] = r5 ;
                   e[q+6] = r6 ;
                   e[q+7] = r7 ;
                   e[q+8] = r8 ;
                   e[q+9] = r9 ;
                 }
             } ;

           double GetFragmentExtent(int q) const
             {
               return e[q] ;
             } ;

           XYZ GetCorner(int q) const
             {
               XYZ r ;
               r.mX = ex[q] ;
               r.mY = ey[q] ;
               r.mZ = ez[q] ;
               return r ;
             } ;

           XYZ GetFragmentCentre(int q) const
             {
               XYZ r ;
               r.mX = x[q] ;
               r.mY = y[q] ;
               r.mZ = z[q] ;
               return r ;
             } ;

           int Get(int q) const { return result[q] ; } ;
         };

         NeighbourList NSQ_NeighbourList;
         #endif


         #if 0
           // The BlueGene/L compiler didn't seem able to handle the manipulators
           // (some of the integers need functional logging as hex, to make sense of what is
           // going on) so a separate 'logged' version of this function made sense.
           int iProduce_logged(
                               const XYZ& aXYZ
                               , const XYZ& eXYZ
                               , double e0
                               , int qstart
                               , int qend
                               ) {
             const double tp32 = 1024.0*1024.0*1024.0*4.0 ;
             int aix = FracScale_logged(aXYZ.mX,k.mX) ;
             int aiy = FracScale_logged(aXYZ.mY,k.mY) ;
             int aiz = FracScale_logged(aXYZ.mZ,k.mZ) ;
             //    int aiex = FracScale_logged(e0,k.mX) >> k_ScaleShift ;
             //    int aiey = FracScale_logged(e0,k.mY) >> k_ScaleShift ;
             //    int aiez = FracScale_logged(e0,k.mZ) >> k_ScaleShift ;
             int aiex = FracScale_logged(eXYZ.mX,k.mX/(1<<k_ScaleShift))  ;
             int aiey = FracScale_logged(eXYZ.mY,k.mY/(1<<k_ScaleShift))  ;
             int aiez = FracScale_logged(eXYZ.mZ,k.mZ/(1<<k_ScaleShift))  ;

         #if 0
             BegLogLine( 0 )
               << "iProduce aXYZ=" << aXYZ
               << " k=" << k
               << " eXYZ=" << eXYZ
               << " aix=" << hex << aix
               << " aiy=" << hex << aiy
               << " aiz=" << hex << aiz
               << " aiex=" << hex << aiex
               << " aiey=" << hex << aiey
               << " aiez=" << hex << aiez
               << dec
               << EndLogLine ;
         #endif
             /*
              * Slice for slab
              */

             int xr[k_FragCount] ;
             int q1 = 0 ;
             for (int a0=qstart; a0<qend; a0+=1 )
               {
                 int idx = aix - ix[a0] ;    // Difference in 'x' coordinate, scaled on full integer range
                 int idxq = idx >> k_ScaleShift ; // Difference in 'x' coordinate, scaled down to keep away from overflows
                 int iem = (-aiex) - iex[a0] ; // Max difference for things to be worth computing, scaled like idxq
                 int nexp = iem + idxq  ;
                 int nexn = iem - idxq  ;
                 unsigned int nex = nexp & nexn ;  // Negative if both of the above are negative, i.e. in range
                 int IndexAdd = nex >> 31 ;
                 xr[q1] = a0 ;
                 q1 += IndexAdd ;
         #if 0
                 BegLogLine( 1 )
                   << "NeighbourList::Produce X"
                   << " IndexAdd " << IndexAdd
                   << " a0 " << a0
                   << " idx " << idx/tp32
                   << " iem " << iem/(tp32/(1<<k_ScaleShift))
                   << " nexp " << nexp/(tp32/(1<<k_ScaleShift))
                   << " nexn " << nexn/(tp32/(1<<k_ScaleShift))
                   << " nex " << hex << nex
                   << " q1 " << dec << q1
                   << EndLogLine;
         #endif
               }
             BegLogLine( 1 )
               << "NeighbourList::Produce X Summary"
               << " q1 " << q1
               << EndLogLine ;

             /*
              * Slice for square prism
              */
             int yr[k_FragCount] ;
             int q2 = 0 ;
             for (int b1=0; b1<q1; b1+=1 )
               {
                 int a1 = xr[b1] ;
                 int idy = aiy - iy[a1] ;
                 int idyq = idy >> k_ScaleShift ; // Difference in 'y' coordinate, scaled down to keep away from overflows
                 int iem = (-aiey) - iey[a1] ;
                 int neyp = iem - idyq ;
                 int neyn = iem + idyq ;
                 unsigned int ney = neyp & neyn ;
                 int IndexAdd = ney >> 31 ;
                 yr[q2] = a1 ;
                 q2 += IndexAdd ;
         #if 0
                 BegLogLine( 1 )
                   << "NeighbourList::Produce Y"
                   << " IndexAdd " << IndexAdd
                   << " a1 " << a1
                   << " idy " << idy/tp32
                   << " iem " << iem/(tp32/(1<<k_ScaleShift))
                   << " neyp " << neyp/(tp32/(1<<k_ScaleShift))
                   << " neyn " << neyn/(tp32/(1<<k_ScaleShift))
                   << " ney " << hex << ney
                   << " q2 " << dec << q2
                   << EndLogLine;
         #endif
               } /* endfor */
             //    BegLogLine( 1 )
             //      << "NeighbourList::Produce Y Summary"
             //      << " q2 " << q2
             //      << EndLogLine ;

             /*
              * Slice for cube
              */
             int zr[k_FragCount] ;
             int q3 = 0 ;
             for (int b2=0; b2<q2; b2+=1 )
               {
                 int a2 = yr[b2] ;
                 int idz = aiz - iz[a2] ;
                 int idzq = idz >> k_ScaleShift ; // Difference in 'z' coordinate, scaled down to keep away from overflows
                 int iem = (-aiez) - iez[a2] ;
                 int nezp = iem - idzq ;
                 int nezn = iem + idzq ;
                 unsigned int nez = nezp & nezn ;
                 int IndexAdd = nez >> 31 ;
                 zr[q3] = a2 ;
                 q3 += IndexAdd ;
         #if 0
                 BegLogLine( 1 )
                   << "NeighbourList::Produce Z"
                   << " IndexAdd " << IndexAdd
                   << " a2 " << a2
                   << " idz " << idz/tp32
                   << " iem " << iem/(tp32/(1<<k_ScaleShift))
                   << " nezp " << nezp/(tp32/(1<<k_ScaleShift))
                   << " nezn " << nezn/(tp32/(1<<k_ScaleShift))
                   << " nez " << hex << nez
                   << " q3 " << dec << q3
                   << EndLogLine;
         #endif
               } /* endfor */
             //    BegLogLine( 1 )
             //      << "NeighbourList::Produce Z Summary"
             //      << " q3 " << q3
             //      << EndLogLine ;

             /*
              * Examine cuboid for sphere
              */
             int q4 = 0 ;
             double x0=aXYZ.mX ;
             double y0=aXYZ.mY ;
             double z0=aXYZ.mZ ;
             double px = p.mX ;
             double py = p.mY ;
             double pz = p.mZ ;
             double kx = k.mX ;
             double ky = k.mY ;
             double kz = k.mZ ;
             for (int b3=0; b3<q3 ; b3+=1)
               {
                 int a3 = zr[b3] ;
                 double dx = NearestDistanceInFullyPeriodicLine(x0,x[a3],px,kx) ;
                 double dy = NearestDistanceInFullyPeriodicLine(y0,y[a3],py,ky) ;
                 double dz = NearestDistanceInFullyPeriodicLine(z0,z[a3],pz,kz) ;
                 double em = e0 + e[a3] ;
                 double ex2 = em*em - dx*dx - dy*dy - dz*dz ;
                 result[q4] = a3 ;
                 double FragmentIndexAdd = fsel(ex2,1.0,0.0) ;
                 int IndexAdd = FragmentIndexAdd ;
                 q4 += IndexAdd ;
         #if 0
                 BegLogLine( 1 )
                   << "NeighbourList::Produce S"
                   << " IndexAdd " << IndexAdd
                   << " a3 " << a3
                   << " dx " << dx
                   << " dy " << dy
                   << " dz " << dz
                   << " em " << em
                   << " ex2 " << ex2
                   << " q4 " << q4
                   << EndLogLine;
         #endif
               } /* endfor */
             BegLogLine( 1 )
               << "NeighbourList::Produce S Summary"
               << " q4 " << q4
               << EndLogLine ;

             return q4 ;

           } ;
#endif
#endif
