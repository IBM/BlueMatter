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
 * Random reader-writer for GPFS exercising
 */
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std ; 

enum { k_BlockSize = 1<<12 , 
       k_BlockCount = 1<<7 , 
       k_BlockSkip = 1<<20 
     } ;
     
enum { k_OpWrite ,
       k_OpRead
} ;     

     
static char blockBuffer[ k_BlockSize ] ;

int main(int argc, const char **argv, const char **envp)
{
    const char * e_BlockSize=getenv("GPFSPERF_BLOCKSIZE") ;
    const unsigned int u_BlockSize=e_BlockSize ? atoi(e_BlockSize) : 0 ;
    const unsigned int BlockSize = ( 0 == u_BlockSize ) ? k_BlockSize : ( 1 << u_BlockSize ) ;
    const char * e_BlockCount=getenv("GPFSPERF_BLOCKCOUNT") ;
    const unsigned int u_BlockCount=e_BlockCount ? atoi(e_BlockCount) : 0 ;
    const unsigned int BlockCount = ( 0 == u_BlockCount ) ? k_BlockCount : ( 1 << u_BlockCount ) ;
    const char * e_BlockSkip=getenv("GPFSPERF_BLOCKSKIP") ;
    const unsigned int u_BlockSkip=e_BlockSkip ? atoi(e_BlockSkip) : 0 ;
    const unsigned int BlockSkip = ( 0 == u_BlockSkip ) ? k_BlockSkip : ( 1 << u_BlockSkip ) ;
    const char * e_BlockOp=getenv("GPFSPERF_BLOCKOP") ; 
    const unsigned int BlockOp= e_BlockOp ? (
                                              ( 0 == strcmp(e_BlockOp,"READ")) ? k_OpRead : k_OpWrite 
                                            )
                                          : k_OpWrite ;
    fprintf(stderr,"BlockSize=%d BlockCount=%d BlockSkip=%d BlockOp=%d\n",BlockSize,BlockCount,BlockSkip,BlockOp) ;
    int fd=open("gpfs_exercise_file",O_RDWR|O_CREAT,0777) ;
    if ( fd >= 0 )
    {
        int lockrc=lockf(fd,F_LOCK,0) ; // take a lock on the whole file
        if ( 0 == lockrc )
        {
            for ( int blockIndex = 0 ; blockIndex<BlockCount ; blockIndex+=1 )
            {
                int op_size = ( BlockOp == k_OpWrite ) ? write(fd, blockBuffer, BlockSize) 
                                                       : read(fd, blockBuffer, BlockSize) ;
                if ( op_size != BlockSize )
                {
                   if ( op_size < 0 )
                   {
                      int e=errno ;
                      fprintf(stderr,"Op failed, errno=%d, blockIndex=%d\n",e,blockIndex) ;
                   } else {
                      fprintf(stderr,"Op short, op_size=%d, blockIndex=%d\n",op_size,blockIndex) ;
                   }
                   break ; 
                }
                off_t seekrc = lseek(fd, blockIndex*BlockSkip, SEEK_SET) ;
            }
        } else {
            int e=errno ;
            fprintf(stderr,"Lock failed, errno=%d\n",e) ;
        }
        int closecode=close(fd) ;
        if ( 0 != closecode )
        {
          int e=errno ;
          fprintf(stderr,"Close failed, errno=%d\n", e) ;
//          cerr << "Close failed, errno=" << e << endl ;
        }
    } else {
        int e=errno ;
        fprintf(stderr, "Open failed, errno=%d\n",e) ;
//        cerr << "Open failed, errno=" << e << endl ;
    }
    return 0 ;
}
