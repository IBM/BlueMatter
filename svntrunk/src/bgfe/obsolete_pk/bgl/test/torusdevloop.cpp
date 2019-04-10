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
 #include "standalone.h"
#include "bgltoruspacket.h"
//#include "bglmemmap.h"
#include "assert.h"

void X_packet_dump(BGLTorusPacket *packet);
void X_packet_initrand (BGLTorusPacket *packet);
int X_packet_compare (BGLTorusPacket *packet0, BGLTorusPacket *packet1);

void _start()
{
  BGLTorusPacket  packet0, packet1, packet2, packet3,
    packet4, packet5, packet6, packet7, packetr;
  BGLTorusDevice  torusa, torusb, torusc;
  BGLDeviceStatus o;
  int x, y, z ;
  unsigned i;
  unsigned long long t1, t2, elapsed;


  t1 = s_timebase();

  /* --------------------------------- */
  /*       initialize the torus        */
  /* --------------------------------- */
  BGLTorus_Initialize (0, 0, 0, 1, 1, 1);
  BGLTorus_nodeAddress (&x, &y, &z);
  sim_printf("Torus initialized. I am torus element %d,%d,%d\n", x, y, z);

  /* --------------------------------- */
  /*       initialize the device       */
  /* --------------------------------- */
  BGLTorusDevice_Init (&torusa, BGLTorusDevice_General0, (void *)0 );
  BGLTorusDevice_Init (&torusb, BGLTorusDevice_General1, (void *)0 );
  BGLTorusDevice_Init (&torusc, BGLTorusDevice_All, (void *)0 );
  sim_printf("Torus Device(s) initialized.\n");

  /* --------------------------------- */
  /*       initialize the packets      */
  /* --------------------------------- */
  BGLTorusPacketHeader_Init (&packet0.header, 0, 0, 0, (void *)0, 0);
  BGLTorusPacketHeader_Init (&packet1.header, 0, 0, 0, (void *)0, 0);
  BGLTorusPacketHeader_Init (&packet2.header, 0, 0, 0, (void *)0, 0);
  BGLTorusPacketHeader_Init (&packet3.header, 0, 0, 0, (void *)0, 0);
  BGLTorusPacketHeader_Init (&packet4.header, 0, 0, 0, (void *)0, 0);
  BGLTorusPacketHeader_Init (&packet5.header, 0, 0, 0, (void *)0, 0);
  BGLTorusPacketHeader_Init (&packet6.header, 0, 0, 0, (void *)0, 0);
  BGLTorusPacketHeader_Init (&packet7.header, 0, 0, 0, (void *)0, 0);
  packet0.header.hh.size = 0;
  packet1.header.hh.size = 1;
  packet2.header.hh.size = 2;
  packet3.header.hh.size = 3;
  packet4.header.hh.size = 4;
  packet5.header.hh.size = 5;
  packet6.header.hh.size = 6;
  packet7.header.hh.size = 7;
  X_packet_initrand(&packet0);
  X_packet_initrand(&packet1);
  X_packet_initrand(&packet2);
  X_packet_initrand(&packet3);
  X_packet_initrand(&packet4);
  X_packet_initrand(&packet5);
  X_packet_initrand(&packet6);
  X_packet_initrand(&packet7);
  sim_printf("Packets initialized.\n");

  /* --------------------------------- */
  /*       send the packet(s)          */
  /* --------------------------------- */
  o = BGLTorusDevice_send(&torusa, (BGLQuad *)&packet0);
  sim_printf("Packet 0 sent\n", o);
  o = BGLTorusDevice_send(&torusb, (BGLQuad *)&packet1);
  sim_printf("Packet 1 sent\n", o);
  o = BGLTorusDevice_send(&torusa, (BGLQuad *)&packet2);
  sim_printf("Packet 2 sent\n", o);
  o = BGLTorusDevice_send(&torusb, (BGLQuad *)&packet3);
  sim_printf("Packet 3 sent\n", o);
  o = BGLTorusDevice_send(&torusa, (BGLQuad *)&packet4);
  sim_printf("Packet 4 sent\n", o);
  o = BGLTorusDevice_send(&torusb, (BGLQuad *)&packet5);
  sim_printf("Packet 5 sent\n", o);
  o = BGLTorusDevice_send(&torusa, (BGLQuad *)&packet6);
  sim_printf("Packet 6 sent\n", o);
  o = BGLTorusDevice_send(&torusb, (BGLQuad *)&packet7);
  sim_printf("Packet 7 sent\n", o);

  /* --------------------------------- */
  /*   wait for packets to come back   */
  /* --------------------------------- */
  for (i=0;i<8;i++)
    {
      while (BGLTorusDevice_recv(&torusc, &packetr)!=BGLDeviceStatus_OK)
        sim_printf("*");
      if      (X_packet_compare(&packet0, &packetr)) sim_printf("Pkt 0 OK\n");
      else if (X_packet_compare(&packet1, &packetr)) sim_printf("Pkt 1 OK\n");
      else if (X_packet_compare(&packet2, &packetr)) sim_printf("Pkt 2 OK\n");
      else if (X_packet_compare(&packet3, &packetr)) sim_printf("Pkt 3 OK\n");
      else if (X_packet_compare(&packet4, &packetr)) sim_printf("Pkt 4 OK\n");
      else if (X_packet_compare(&packet5, &packetr)) sim_printf("Pkt 5 OK\n");
      else if (X_packet_compare(&packet6, &packetr)) sim_printf("Pkt 6 OK\n");
      else if (X_packet_compare(&packet7, &packetr)) sim_printf("Pkt 7 OK\n");
      else sim_printf("Broken Packet\n");
    }

  t2 = s_timebase();
  elapsed = t2 - t1;
  sim_printf("TorusDevTest timing: %ld\n", (unsigned) elapsed);


  /* --------------------------------- */
  /*     that's it                     */
  /* --------------------------------- */
  sim_stop(0x1234);
}





















void X_dump_header (BGLTorusPacketHeader *header)
{
  char xhint = header->hh.hintXPlus?'+':(header->hh.hintXMinus?'-':' ');
  char yhint = header->hh.hintYPlus?'+':(header->hh.hintYMinus?'-':' ');
  char zhint = header->hh.hintZPlus?'+':(header->hh.hintZMinus?'-':' ');

  sim_printf("Header (%d%c,%d%c,%d%c) size(%d) AF=%x ARG=%x\n",
             header->hh.destX,xhint,
             header->hh.destY,yhint,
             header->hh.destZ,zhint,
             header->hh.size,
             header->sh.fcn,
             header->sh.arg);
}

void X_packet_dump(BGLTorusPacket *packet)
{

  int i;
  char *p = (char *)packet->data;
  X_dump_header(&packet->header);
  for (i=0; i<((1+packet->header.hh.size)<<5)-16; i++)
    {
      sim_printf("0x%02x ", p[i]);
      if ((i%16)==15) sim_printf("\n");
    }
  sim_printf("\n");
}

unsigned long long X_random_gen ()
{
  static unsigned long long mask = 0x7FFFFFFFULL; /* 2^31 - 1 */
  static unsigned long long a = 1103515245ULL;
  static unsigned long long c = 12345ULL;
  static unsigned long long ran = 99ULL;
  ran = ((a * ran + c) & mask);
  return ran;
}

void X_packet_initrand (BGLTorusPacket *packet)
{
  int i;
  char *p;
  assert (packet != (BGLTorusPacket *)0);
  p = (char *)packet->data;
  for (i=0; i<((1+packet->header.hh.size)<<5)-16; i++)
    {
      p[i] = (X_random_gen() & 0xFF00) >> 8;
    }
}

int X_packet_compare (BGLTorusPacket *packet0, BGLTorusPacket *packet1)
{
  int size0, size1, i;
  char *p, *q;
  assert (packet0 != 0 && packet1 != 0);
  size0 = packet0->header.hh.size;
  size1 = packet1->header.hh.size;
  if (size0 != size1) return 0;
  p = (char *)packet0->data;
  q = (char *)packet1->data;
  for (i=0; i<((1+packet0->header.hh.size)<<5)-16; i++)
    if (p[i] != q[i]) return 0;
  return 1;
}



