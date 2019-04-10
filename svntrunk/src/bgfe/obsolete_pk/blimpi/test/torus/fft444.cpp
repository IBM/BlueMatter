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
 /*
 * Distributed 4x4x4 FFT on a 2x2x2 torus machine
 *
 * For test case purposes, this leads a BG/L machine (in a 2x2x2 configuration)
 * through a 3d Fast Fourier Transform on a 4x4x4 grid.
 *
 * Each node manages the 2x2x2 cube of points in a corner of the grid, in a
 * spatial decomposition.
 *
 * First the 16 1-d 4-point transforms along the X axis are performed.
 * Each node will perform 2 transforms. The node sends to its partner the data which
 * its partner will need; then accepts the data which its partner has sent; then
 * performs the 2 transforms; then sends the frequency-domain data which belongs
 * to its partner; then accepts the frequency-domain data which its partner has
 * sent.
 * This is then repeated for the Y axis, and then for the Z axis.
 *
 * Depending on the timing, it would be possible for data from the Y partner to
 * arrive before data from the X partner, and so on. This is handled by tagging
 * each packet with a 'phase number' ( for a total of 6 phases here )
 *
 * For ease of setup, each node starts with a copy of the whole 4x4x4 real-space
 * data cube. By the end of the operation, one 2x2x2 corner has been replaced with
 * the appropriate frequency-domain data.
 */
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }

#include <sys/blade_types.h>
#include <spi/BGL_TorusPktSPI.h>
#include <BlueMatter/fft4.hpp>

// For the moment, give every node a copy of the whole data structure
complex_without_constructor d[4][4][4] ;


class shuttle_data
{
   public:
   enum {
      k_shuttle_value_count = 4
      } ;
   complex_without_constructor shuttle_values[k_shuttle_value_count] ;
} ;

class shuttle_send_item: public shuttle_data
{
   public:
   unsigned int key ;
} ;

class shuttle_receive_item : public shuttle_data
{
   public:
   int received ;
} ;

class shuttle_receive_block
{
   public:
   enum {
      k_receive_limit = 6
      } ;
   shuttle_receive_item receive_block[k_receive_limit] ;
   void clear(void)
   {
      for (int a=0; a<k_receive_limit;a+=1 )
      {
         receive_block[a].received = 0 ;
      } /* endfor */
   } ;
} ;

shuttle_receive_block srb  ;

ACTOR_DECL(torus_exchange_actor) ( _BGL_TorusPktPayload something, Bit32 dumb, Bit32 Dumber )
{
   shuttle_send_item * payload = ( shuttle_send_item *) something ;
   unsigned int key = payload -> key ;
// assert ( key < srb.k_receive_limit ) ;
   if ( key < srb.k_receive_limit)
   {
//    assert ( 0 == srb.receive_block[key].received ) ; // Packet should not be recieved twice
      srb.receive_block[key].received = 1 ;
      for (int p=0; p<shuttle_data::k_shuttle_value_count; p+= 1)
      {
         srb.receive_block[key].shuttle_values[p] = payload -> shuttle_values[p] ;
      } /* endfor */
   } /* endif */
   return 1 ;
}

void torus_exchange(const shuttle_send_item & shuttle_send,
                          shuttle_data & shuttle_receive,
                    int target_x, int target_y, int target_z
                    )
{

        _BGL_TorusPkt    Pkt;
        BGLTorusMakeHdr( & Pkt.hdr,             // Filled in on return
                         target_x,target_y,target_z,                 // destination coordinates
                         0,          //???      // destination Fifo Group
                         ACTOR_CAST(torus_exchange_actor),  // Active Packet Function matching signature above
                         0,                     // primary argument to actor
                         0,                     // secondary 10bit argument to actor
                         _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)

        BGLTorusSendPacketUnaligned( & Pkt.hdr ,      // Previously created header to use
                                     (void *) &shuttle_send,     // Source address of data (16byte aligned)
                                     sizeof(shuttle_send) ); // Payload bytes = ((chunks * 32) - 16)

        int key = shuttle_send.key ;
        // Packets for different dimensions might arrive out of order with respect to what we expect,
        // so check and loop ...
        while (0 == srb.receive_block[key].received)
        {
          BGLTorusWait() ; // The 'torus exchange actor' will end the wait when a packet arrives
        } /* endwhile */
        // Copy the received data from where the actor function put it.
        for (int a=0;a<shuttle_data::k_shuttle_value_count ; a+=1)
        {
           shuttle_receive.shuttle_values[a] = srb.receive_block[key].shuttle_values[a] ;
        } /* endfor */
}

static inline int axis1(int x, int y, int z ) { return x ; } ;
static inline int axis2(int x, int y, int z ) { return y ; } ;
static inline int axis3(int x, int y, int z ) { return z ; } ;

int _CP_main( int argc, char *argv[], char **envp )
{
   int my_x, my_y, my_z ;
#if BLNIE
   BGLPartitionGetCoords( &my_x, &my_y, &my_z );
#else
   my_x = 0 ;
   my_y = 0 ;
   my_z  =0 ;
#endif

   for (int x=0; x<4; x+=1)
   {
      for (int y=0; y<4; y+=1)
      {
         for (int z=0; z<4; z+=1)
         {
            d[x][y][z] = complex(0.0,0.0)  ;
         } /* endfor */
      } /* endfor */
   } /* endfor */

  d[0][0][1] = complex(1.0,0.0) ;

/*
 *  Nominally, node (a,b,c) owns the octant beginning at d[2*a][2*b][2*c]
 *  and increasing by 1 in each dimension.
 *  For the transform in each dimension, there are 16 of them to be done, and each node will do 2.
 *  Of the 4 that it owns partial data for ,the node will do 2 locally ... it will first exchange
 *  data that it owns but does not need, for data that it needs but does not own, with the appropriate
 *  partner node. Then it will do the local FFTs. Then it will exchange back
 */

  complex fft1[4] ;
  complex fft2[4] ;
  fft4 fftf1(fft1) ;
  fft4 fftf2(fft2) ;
  shuttle_send_item ssi ;
  shuttle_data shuttle_receive ;

  srb.clear() ;

  // Send the 'away' data, and receive the 'foreign' data
  ssi.shuttle_values[0] = d[2*axis1(my_x, my_y, my_z)  ][2*axis2(my_x, my_y, my_z)  ][2*axis3(my_x, my_y, my_z)+(1-axis1(my_x, my_y, my_z))] ;
  ssi.shuttle_values[1] = d[2*axis1(my_x, my_y, my_z)+1][2*axis2(my_x, my_y, my_z)  ][2*axis3(my_x, my_y, my_z)+(1-axis1(my_x, my_y, my_z))] ;
  ssi.shuttle_values[2] = d[2*axis1(my_x, my_y, my_z)  ][2*axis2(my_x, my_y, my_z)+1][2*axis3(my_x, my_y, my_z)+(1-axis1(my_x, my_y, my_z))] ;
  ssi.shuttle_values[3] = d[2*axis1(my_x, my_y, my_z)+1][2*axis2(my_x, my_y, my_z)+1][2*axis3(my_x, my_y, my_z)+(1-axis1(my_x, my_y, my_z))] ;
  ssi.key = 0 ;

  torus_exchange(ssi, shuttle_receive, 1-axis1(my_x, my_y, my_z), axis2(my_x,my_y,my_z), axis3(my_x, my_y, my_z))  ;

  // Assemble the FFTs to do

  fft1[2*axis1(my_x, my_y, my_z)  ] = d[2*axis1(my_x, my_y, my_z)  ][2*axis2(my_x, my_y, my_z)  ][2*axis3(my_x, my_y, my_z)  ] ;
  fft1[2*axis1(my_x, my_y, my_z)+1] = d[2*axis1(my_x, my_y, my_z)+1][2*axis2(my_x, my_y, my_z)  ][2*axis3(my_x, my_y, my_z)  ] ;
  fft1[2*(1-axis1(my_x, my_y, my_z))  ] = shuttle_receive.shuttle_values[0] ;
  fft1[2*(1-axis1(my_x, my_y, my_z))+1] = shuttle_receive.shuttle_values[1] ;

  fft2[2*axis1(my_x, my_y, my_z)  ] = d[2*axis1(my_x, my_y, my_z)  ][2*axis2(my_x, my_y, my_z)+1][2*axis3(my_x, my_y, my_z)  ] ;
  fft2[2*axis1(my_x, my_y, my_z)+1] = d[2*axis1(my_x, my_y, my_z)+1][2*axis2(my_x, my_y, my_z)+1][2*axis3(my_x, my_y, my_z)  ] ;
  fft2[2*(1-axis1(my_x, my_y, my_z))  ] = shuttle_receive.shuttle_values[2] ;
  fft2[2*(1-axis1(my_x, my_y, my_z))+1] = shuttle_receive.shuttle_values[3] ;

  // Perform the local FFTs
  fftf1.inline_forward() ;
  fftf2.inline_forward() ;

  // Send the 'foreign' results home, and receive the 'away' results
  ssi.shuttle_values[0] = fft1[2*(1-axis1(my_x, my_y, my_z))  ] ;
  ssi.shuttle_values[1] = fft1[2*(1-axis1(my_x, my_y, my_z))+1] ;
  ssi.shuttle_values[2] = fft2[2*(1-axis1(my_x, my_y, my_z))  ] ;
  ssi.shuttle_values[3] = fft2[2*(1-axis1(my_x, my_y, my_z))+1] ;
  ssi.key = 1 ;

  torus_exchange(ssi, shuttle_receive, 1-axis1(my_x, my_y, my_z), axis2(my_x,my_y,my_z), axis3(my_x, my_y, my_z))  ;


  // Reassemble the octant
  d[2*axis1(my_x, my_y, my_z)  ][2*axis2(my_x, my_y, my_z)  ][2*axis3(my_x, my_y, my_z)+(1-axis1(my_x, my_y, my_z))] = shuttle_receive.shuttle_values[0] ;
  d[2*axis1(my_x, my_y, my_z)+1][2*axis2(my_x, my_y, my_z)  ][2*axis3(my_x, my_y, my_z)+(1-axis1(my_x, my_y, my_z))] = shuttle_receive.shuttle_values[1] ;
  d[2*axis1(my_x, my_y, my_z)  ][2*axis2(my_x, my_y, my_z)+1][2*axis3(my_x, my_y, my_z)+(1-axis1(my_x, my_y, my_z))] = shuttle_receive.shuttle_values[2] ;
  d[2*axis1(my_x, my_y, my_z)+1][2*axis2(my_x, my_y, my_z)+1][2*axis3(my_x, my_y, my_z)+(1-axis1(my_x, my_y, my_z))] = shuttle_receive.shuttle_values[3] ;
  d[2*axis1(my_x, my_y, my_z)  ][2*axis2(my_x, my_y, my_z)  ][2*axis3(my_x, my_y, my_z)  ] = fft1[2*axis1(my_x, my_y, my_z)  ] ;
  d[2*axis1(my_x, my_y, my_z)+1][2*axis2(my_x, my_y, my_z)  ][2*axis3(my_x, my_y, my_z)  ] = fft1[2*axis1(my_x, my_y, my_z)+1] ;
  d[2*axis1(my_x, my_y, my_z)  ][2*axis2(my_x, my_y, my_z)+1][2*axis3(my_x, my_y, my_z)  ] = fft2[2*axis1(my_x, my_y, my_z)  ] ;
  d[2*axis1(my_x, my_y, my_z)+1][2*axis2(my_x, my_y, my_z)+1][2*axis3(my_x, my_y, my_z)  ] = fft2[2*axis1(my_x, my_y, my_z)+1] ;

  // X is complete. Now repeat the exercise for y
  // Send the 'away' data, and receive the 'foreign' data
  ssi.shuttle_values[0] = d[2*axis1(my_y, my_z, my_x)  ][2*axis2(my_y, my_z, my_x)  ][2*axis3(my_y, my_z, my_x)+(1-axis1(my_y, my_z, my_x))] ;
  ssi.shuttle_values[1] = d[2*axis1(my_y, my_z, my_x)+1][2*axis2(my_y, my_z, my_x)  ][2*axis3(my_y, my_z, my_x)+(1-axis1(my_y, my_z, my_x))] ;
  ssi.shuttle_values[2] = d[2*axis1(my_y, my_z, my_x)  ][2*axis2(my_y, my_z, my_x)+1][2*axis3(my_y, my_z, my_x)+(1-axis1(my_y, my_z, my_x))] ;
  ssi.shuttle_values[3] = d[2*axis1(my_y, my_z, my_x)+1][2*axis2(my_y, my_z, my_x)+1][2*axis3(my_y, my_z, my_x)+(1-axis1(my_y, my_z, my_x))] ;
  ssi.key = 2 ;

  torus_exchange(ssi, shuttle_receive, 1-axis1(my_y, my_z, my_x), axis2(my_x,my_y,my_z), axis3(my_y, my_z, my_x))  ;

  // Assemble the FFTs to do

  fft1[2*axis1(my_y, my_z, my_x)  ] = d[2*axis1(my_y, my_z, my_x)  ][2*axis2(my_y, my_z, my_x)  ][2*axis3(my_y, my_z, my_x)  ] ;
  fft1[2*axis1(my_y, my_z, my_x)+1] = d[2*axis1(my_y, my_z, my_x)+1][2*axis2(my_y, my_z, my_x)  ][2*axis3(my_y, my_z, my_x)  ] ;
  fft1[2*(1-axis1(my_y, my_z, my_x))  ] = shuttle_receive.shuttle_values[0] ;
  fft1[2*(1-axis1(my_y, my_z, my_x))+1] = shuttle_receive.shuttle_values[1] ;

  fft2[2*axis1(my_y, my_z, my_x)  ] = d[2*axis1(my_y, my_z, my_x)  ][2*axis2(my_y, my_z, my_x)+1][2*axis3(my_y, my_z, my_x)  ] ;
  fft2[2*axis1(my_y, my_z, my_x)+1] = d[2*axis1(my_y, my_z, my_x)+1][2*axis2(my_y, my_z, my_x)+1][2*axis3(my_y, my_z, my_x)  ] ;
  fft2[2*(1-axis1(my_y, my_z, my_x))  ] = shuttle_receive.shuttle_values[2] ;
  fft2[2*(1-axis1(my_y, my_z, my_x))+1] = shuttle_receive.shuttle_values[3] ;

  // Perform the local FFTs
  fftf1.inline_forward() ;
  fftf2.inline_forward() ;

  // Send the 'foreign' results home, and receive the 'away' results
  ssi.shuttle_values[0] = fft1[2*(1-axis1(my_y, my_z, my_x))  ] ;
  ssi.shuttle_values[1] = fft1[2*(1-axis1(my_y, my_z, my_x))+1] ;
  ssi.shuttle_values[2] = fft2[2*(1-axis1(my_y, my_z, my_x))  ] ;
  ssi.shuttle_values[3] = fft2[2*(1-axis1(my_y, my_z, my_x))+1] ;
  ssi.key = 3 ;

  torus_exchange(ssi, shuttle_receive, 1-axis1(my_y, my_z, my_x), axis2(my_x,my_y,my_z), axis3(my_y, my_z, my_x))  ;


  // Reassemble the octant
  d[2*axis1(my_y, my_z, my_x)  ][2*axis2(my_y, my_z, my_x)  ][2*axis3(my_y, my_z, my_x)+(1-axis1(my_y, my_z, my_x))] = shuttle_receive.shuttle_values[0] ;
  d[2*axis1(my_y, my_z, my_x)+1][2*axis2(my_y, my_z, my_x)  ][2*axis3(my_y, my_z, my_x)+(1-axis1(my_y, my_z, my_x))] = shuttle_receive.shuttle_values[1] ;
  d[2*axis1(my_y, my_z, my_x)  ][2*axis2(my_y, my_z, my_x)+1][2*axis3(my_y, my_z, my_x)+(1-axis1(my_y, my_z, my_x))] = shuttle_receive.shuttle_values[2] ;
  d[2*axis1(my_y, my_z, my_x)+1][2*axis2(my_y, my_z, my_x)+1][2*axis3(my_y, my_z, my_x)+(1-axis1(my_y, my_z, my_x))] = shuttle_receive.shuttle_values[3] ;
  d[2*axis1(my_y, my_z, my_x)  ][2*axis2(my_y, my_z, my_x)  ][2*axis3(my_y, my_z, my_x)  ] = fft1[2*axis1(my_y, my_z, my_x)  ] ;
  d[2*axis1(my_y, my_z, my_x)+1][2*axis2(my_y, my_z, my_x)  ][2*axis3(my_y, my_z, my_x)  ] = fft1[2*axis1(my_y, my_z, my_x)+1] ;
  d[2*axis1(my_y, my_z, my_x)  ][2*axis2(my_y, my_z, my_x)+1][2*axis3(my_y, my_z, my_x)  ] = fft2[2*axis1(my_y, my_z, my_x)  ] ;
  d[2*axis1(my_y, my_z, my_x)+1][2*axis2(my_y, my_z, my_x)+1][2*axis3(my_y, my_z, my_x)  ] = fft2[2*axis1(my_y, my_z, my_x)+1] ;

  // Send the 'away' data, and receive the 'foreign' data
  ssi.shuttle_values[0] = d[2*axis1(my_z ,my_x, my_y)  ][2*axis2(my_z ,my_x, my_y)  ][2*axis3(my_z ,my_x, my_y)+(1-axis1(my_z ,my_x, my_y))] ;
  ssi.shuttle_values[1] = d[2*axis1(my_z ,my_x, my_y)+1][2*axis2(my_z ,my_x, my_y)  ][2*axis3(my_z ,my_x, my_y)+(1-axis1(my_z ,my_x, my_y))] ;
  ssi.shuttle_values[2] = d[2*axis1(my_z ,my_x, my_y)  ][2*axis2(my_z ,my_x, my_y)+1][2*axis3(my_z ,my_x, my_y)+(1-axis1(my_z ,my_x, my_y))] ;
  ssi.shuttle_values[3] = d[2*axis1(my_z ,my_x, my_y)+1][2*axis2(my_z ,my_x, my_y)+1][2*axis3(my_z ,my_x, my_y)+(1-axis1(my_z ,my_x, my_y))] ;
  ssi.key = 4 ;

  torus_exchange(ssi, shuttle_receive, 1-axis1(my_z ,my_x, my_y), axis2(my_x,my_y,my_z), axis3(my_z ,my_x, my_y))  ;

  // Assemble the FFTs to do

  fft1[2*axis1(my_z ,my_x, my_y)  ] = d[2*axis1(my_z ,my_x, my_y)  ][2*axis2(my_z ,my_x, my_y)  ][2*axis3(my_z ,my_x, my_y)  ] ;
  fft1[2*axis1(my_z ,my_x, my_y)+1] = d[2*axis1(my_z ,my_x, my_y)+1][2*axis2(my_z ,my_x, my_y)  ][2*axis3(my_z ,my_x, my_y)  ] ;
  fft1[2*(1-axis1(my_z ,my_x, my_y))  ] = shuttle_receive.shuttle_values[0] ;
  fft1[2*(1-axis1(my_z ,my_x, my_y))+1] = shuttle_receive.shuttle_values[1] ;

  fft2[2*axis1(my_z ,my_x, my_y)  ] = d[2*axis1(my_z ,my_x, my_y)  ][2*axis2(my_z ,my_x, my_y)+1][2*axis3(my_z ,my_x, my_y)  ] ;
  fft2[2*axis1(my_z ,my_x, my_y)+1] = d[2*axis1(my_z ,my_x, my_y)+1][2*axis2(my_z ,my_x, my_y)+1][2*axis3(my_z ,my_x, my_y)  ] ;
  fft2[2*(1-axis1(my_z ,my_x, my_y))  ] = shuttle_receive.shuttle_values[2] ;
  fft2[2*(1-axis1(my_z ,my_x, my_y))+1] = shuttle_receive.shuttle_values[3] ;

  // Perform the local FFTs
  fftf1.inline_forward() ;
  fftf2.inline_forward() ;

  // Send the 'foreign' results home, and receive the 'away' results
  ssi.shuttle_values[0] = fft1[2*(1-axis1(my_z ,my_x, my_y))  ] ;
  ssi.shuttle_values[1] = fft1[2*(1-axis1(my_z ,my_x, my_y))+1] ;
  ssi.shuttle_values[2] = fft2[2*(1-axis1(my_z ,my_x, my_y))  ] ;
  ssi.shuttle_values[3] = fft2[2*(1-axis1(my_z ,my_x, my_y))+1] ;
  ssi.key = 5 ;

  torus_exchange(ssi, shuttle_receive, 1-axis1(my_z ,my_x, my_y), axis2(my_x,my_y,my_z), axis3(my_z ,my_x, my_y))  ;


  // Reassemble the octant
  d[2*axis1(my_z ,my_x, my_y)  ][2*axis2(my_z ,my_x, my_y)  ][2*axis3(my_z ,my_x, my_y)+(1-axis1(my_z ,my_x, my_y))] = shuttle_receive.shuttle_values[0] ;
  d[2*axis1(my_z ,my_x, my_y)+1][2*axis2(my_z ,my_x, my_y)  ][2*axis3(my_z ,my_x, my_y)+(1-axis1(my_z ,my_x, my_y))] = shuttle_receive.shuttle_values[1] ;
  d[2*axis1(my_z ,my_x, my_y)  ][2*axis2(my_z ,my_x, my_y)+1][2*axis3(my_z ,my_x, my_y)+(1-axis1(my_z ,my_x, my_y))] = shuttle_receive.shuttle_values[2] ;
  d[2*axis1(my_z ,my_x, my_y)+1][2*axis2(my_z ,my_x, my_y)+1][2*axis3(my_z ,my_x, my_y)+(1-axis1(my_z ,my_x, my_y))] = shuttle_receive.shuttle_values[3] ;
  d[2*axis1(my_z ,my_x, my_y)  ][2*axis2(my_z ,my_x, my_y)  ][2*axis3(my_z ,my_x, my_y)  ] = fft1[2*axis1(my_z ,my_x, my_y)  ] ;
  d[2*axis1(my_z ,my_x, my_y)+1][2*axis2(my_z ,my_x, my_y)  ][2*axis3(my_z ,my_x, my_y)  ] = fft1[2*axis1(my_z ,my_x, my_y)+1] ;
  d[2*axis1(my_z ,my_x, my_y)  ][2*axis2(my_z ,my_x, my_y)+1][2*axis3(my_z ,my_x, my_y)  ] = fft2[2*axis1(my_z ,my_x, my_y)  ] ;
  d[2*axis1(my_z ,my_x, my_y)+1][2*axis2(my_z ,my_x, my_y)+1][2*axis3(my_z ,my_x, my_y)  ] = fft2[2*axis1(my_z ,my_x, my_y)+1] ;

  // Now we have a complete 3d transform. The results are distributed back to overlaying that portion of the
  // original data that was used; i.e. each node now has a 4x4x4 cube of data, of which one 2x2x2 corner is valid.
  return 0 ;
}

int _IOP_main( int argc, char *argv[], char **envp )
{
   return 0 ;
}
