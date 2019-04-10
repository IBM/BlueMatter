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
 /*********************************************************************/
/*                        I.B.M. CONFIDENTIAL                        */
/*      (c) COPYRIGHT IBM CORP. 2001, 2003 ALL RIGHTS RESERVED.      */
/*                                                                   */
/*                LICENSED MATERIALS-PROPERTY OF IBM.                */
/*                                                                   */
/*********************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "bgltoruspacket.h"
#include "bglpersonality.h"
#include "rts.h"

int
main(int argc, char **argv)
   {
   // get personality info
   //
   BGLPersonality pers;
   rts_get_personality(&pers, sizeof(pers));
   
   const unsigned x = pers.xCoord, xsize = pers.xSize;
   const unsigned y = pers.yCoord, ysize = pers.ySize;
   const unsigned z = pers.zCoord, zsize = pers.zSize;

   printf("I am node <%u,%u,%u> in a <%u,%u,%u> torus\n", x, y, z, xsize, ysize, zsize);

   // initialize the torus
   //
   BGLTorusDevice device;
   BGLTorusDevice_Init(&device, BGLTorusDevice_All, 0);

   // if we are node (0,0,0) then we expect everyone to send us a packet.
   // otherwise we send a packet to (0,0,0).
   //

   #define DUMMY_FCN 545

   if (x == 0 && y == 0 && z == 0)
      {
      printf("<%u,%u,%u>: receiving\n", x, y, z);
      
      for (unsigned i = 1; i < xsize * ysize * zsize; ++i)
         {
         BGLTorusPacket pkt;

         while (BGLTorusDevice_recv (&device, &pkt) != BGLDeviceStatus_OK)
            ;
            
         if ((unsigned)pkt.header.sh.fcn == DUMMY_FCN)
            printf("<%u,%u,%u>: got `%s'\n", x, y, z, (char *)pkt.data);
         else
            printf("<%u,%u,%u>: got something unexpected\n", x, y, z);
         }
      }
   else
      {
      printf ("<%u,%u,%u> sending to <0,0,0>\n", x, y, z);

      BGLTorusPacket pkt;
      BGLTorusPacketHeader_Init(&pkt.header, 0, 0, 0, (void *)DUMMY_FCN, 0);
    
      sprintf((char *)pkt.data, "Greetings from <%u,%u,%u>", x, y, z);
    
      // note from george:
      //
      // pkt.hader.hh.size is the number of "chunks" (cache lines) 
      // that are in the packet. the formula for this number is 
      //     ( <#bytes in payload> + 16 ) / 32
      //
      pkt.header.hh.size = 3;
      
      while (BGLTorusDevice_sendH(&device, &pkt.header, pkt.data) != BGLDeviceStatus_OK)
         ;
      }
   printf("<%u,%u,%u>: done\n", x, y, z);
   return 0;
   }

#if 0
    /* ----------------------------------  */
    /*  initialize the torus the LINUX way */
    /* ----------------------------------  */
   {
     #warning "Compiling for LINUX"
     void *vc0, *vc1;
     BGLPersonality pers;
     int n, fd = open("/dev/torus0", O_RDWR);
     if (fd == -1) perror ("open /dev/torus0"), exit(1);
     vc0 = mmap(0, 0x8000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
     close(fd);
     if ((int) vc0 == -1) perror("cannot open /dev/torus0"), exit(1);

     fd = open("/dev/torus1", O_RDWR);
     if (fd == -1) perror ("open /dev/torus1"), exit(1);
     vc1 = mmap(0, 0x8000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
     close (fd);
     if ((int)vc1 == -1) perror("cannot open /dev/torus1"), exit(1);

     BGLTorusDevice_Init (&device, BGLTorusDevice_All, (void *)0);
     BGLTorusDevice_setVbase0(&device, (char *)vc0);
     BGLTorusDevice_setVbase1(&device, (char *)vc1);

     fd = open("/proc/personality", O_RDONLY);
     if (fd < 0) perror ("open /proc/personality"), exit(1);
     n = read(fd, &pers, sizeof(pers));
     if (n != sizeof(pers)) perror("reading /proc/personality"), exit(1);
     x = pers.xCoord; y = pers.yCoord; z = pers.zCoord;
     xmax = pers.xSize; ymax = pers.ySize; zmax = pers.zSize;

   }
#endif
