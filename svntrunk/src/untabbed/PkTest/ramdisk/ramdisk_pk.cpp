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
//  IBM T.J.W  R.C.
//  Date : 2007-02-08
//  Name : Chris Ward
//  Tests using GPFS to manage I/O node ramdisks 


#include <BonB/BGL_PartitionSPI.h>
#include <rts.h>
#include <Pk/API.hpp>
#include <Pk/Compatibility.hpp>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

int PkStartType = PkStartAllNodesCore0 ;

#ifdef PKTRACE
int PkTracing = 1 ;
#else
int PkTracing = 0 ;
#endif

#ifndef PKFXLOG_RAMDISK
#define PKFXLOG_RAMDISK ( 1 ) 
#endif

#ifndef PKTRACE_RAMDISK 
#define PKTRACE_RAMDISK ( 1 )
#endif

TraceClient TR_RAMDISK_Start;
TraceClient TR_RAMDISK_Finis;

int PkMain(int argc, char** argv, char** envp)
{
  // 'Barrier' here spins until all nodes have finshed 
  
  BegLogLine( PKFXLOG_RAMDISK )
    << "Before open() "
    << " errno= " << errno
    << EndLogLine;


  char filename[100] ;
  sprintf(filename,"/ioramdisk/public/seekfile.%d",PkNodeGetId()) ;
  //sprintf(filename,"/bgdhome/arayshu/gpfs_test/seekfile.%d",PkNodeGetId()) ;
  //int fd=open(filename, O_CREAT|O_APPEND, S_IRUSR | S_IWUSR ) ;
  //  int fd=open(filename, O_CREAT, S_IRUSR | S_IWUSR ) ;
  int fd=open(filename, O_CREAT|O_TRUNC|O_RDWR|O_SYNC, S_IRUSR | S_IWUSR ) ;

  BegLogLine( PKFXLOG_RAMDISK )
    << "After open() "
    << "filename: " << filename  
    << " fd= " << fd
    << " errno= " << errno
    << EndLogLine;

  int writelen = -1 ;
  int errcode = -1 ;
  
  if ( fd >= 0 )
  {
    char * hellobuf = "Hello\n" ;
    writelen = write(fd,hellobuf,6) ; 
    if ( -1 == writelen )
    {
      errcode = errno ;
    
    BegLogLine( PKFXLOG_RAMDISK )
      << "ERROR:: Could not write to a file:  " 
      << filename
      << " errno: " << errno
      << EndLogLine;
    } 
    close(fd) ;
  }	
  PkCo_Barrier() ; 
  int fdr=open(filename,O_RDONLY) ;
  PkCo_Barrier() ; 
  char inbuf[ 1024 ] ; 
  if ( fdr >= 0 )
  {
    TR_RAMDISK_Start.HitOE( PKTRACE_RAMDISK,
         "TR_RAMDISK_Start", 
         Platform::Thread::GetId(),
         TR_RAMDISK_Start );
    int result = read(fdr,inbuf,sizeof(inbuf)) ;

    TR_RAMDISK_Finis.HitOE( PKTRACE_RAMDISK,
         "TR_RAMDISK_Finis", 
         Platform::Thread::GetId(),
         TR_RAMDISK_Finis );
    close(fdr) ; 
  }
  PkCo_Barrier() ;
  inbuf[sizeof(inbuf)-1] = 0 ; 
  
  BegLogLine(PKFXLOG_RAMDISK)
    << "Read fd=" << fd
    << " writelen=" << writelen
    << " errcode=" << errcode
    << " fdr=" << fdr
    << " (" << inbuf
    << ")"
    << EndLogLine ;
    
#if defined(PKTRACE)
  pkTraceServer::FlushBuffer() ;
#endif
  
   exit(0) ; // Bring 'Pk' down explicitly
  
  return 0 ;
} 
