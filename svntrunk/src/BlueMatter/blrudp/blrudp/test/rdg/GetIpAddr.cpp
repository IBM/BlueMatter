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
 

#include <iostream.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <stddef.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#include <BlErrors.h>
#include <blrudp.h>

#define FAIL goto Fail
#define FAILERR(nErr, nValue) {(nErr) = (nValue); goto Fail;}


#define SIZEOF_ARRAY(n) (sizeof(n)/sizeof(n[0]))
#define BLMAKE_IPADDR(n1,n2,n3,n4) (htonl( ((n1)<<24) | ((n2)<<16) | ((n3)<<8) | (n4)))

//=================================================

int
GetALocalIpAddr( unsigned long & LocalIpAddr )
  {
  static struct sockaddr_in my_sock_addr;
  int     namelength;
  struct  hostent hostent_copy;               /* Storage for hostent data.  */
  struct  hostent *hostentptr;                 /* Pointer to hostent data.   */
  static  char hostname[256];
  int     nRet;


  /*
   * Get the local host name.
   */
  nRet = gethostname( hostname, sizeof hostname );
  if( nRet )
    {
    perror ("gethostname");
    exit(-1);
    }

  cerr << "gethostbyname() returned >" << hostname << "<" << endl;

  /*
   * Get pointer to network data structure for local host.
   */
  if( ( hostentptr = gethostbyname( hostname ) ) == NULL)
    {
    perror( "gethostbyname");
    exit(-1);
    }


  /*
   * Copy hostent data to safe storage.
   */
  hostent_copy = *hostentptr;


  /*
   * Fill in the address structure for socket 2.
   */
  my_sock_addr.sin_family = hostent_copy.h_addrtype;
  my_sock_addr.sin_port = htons( 0 );
  my_sock_addr.sin_addr = * ((struct in_addr *) hostent_copy.h_addr);

  cerr << "hostent_copy.h_addr " << (void *) hostent_copy.h_addr << endl;

  LocalIpAddr = * ((unsigned long *) hostent_copy.h_addr);

  return( 0 );
  }
//=================================================

