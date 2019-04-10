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
 #ifndef _CONFIG_H_
#define _CONFIG_H_

///////////////////////////////////////////////////////////////////
//
// config.h
//
// this file defines the differences between various gnu and windows
// C++ header implementations.  It is designed to be compatible
// with the format used by the gnu autoconf utility.
//
// Each header file which is optionally available is defined 
// with a HAVE_xxxxx_H declaration.
//
// Currently it keys off of platform specific keys for the
// implementations that we are supporting on blue light.
//
// These are 
//
// defined (WIN32).
// defined (AIX)
// defined (LINUX)
// defined (CIGWIN_NT-5.0)
//
//
// These strings are the strings supplied by the 'uname' system
// utility under unix that prints the OS name to standard out.
//


//
// Some GNUC common stuff...
//
#if defined(__GNUC__)
    #define UNUSED __attribute__((unused))
    #define PACKED __attribute__((packed))
    #define UNUSEDPARAM(n) ((n) = (n))
#else
    #define UNUSED
    #define UNUSEDPARAM
    #if !defined(PACKED)
        #define PACKED
    #endif
#endif

//
// If the system has not already defined the byte order, then define the BYTE order..
//
#ifndef BYTE_ORDER
#define LITTLE_ENDIAN   1234    /* least-significant byte first (vax) */
#define BIG_ENDIAN      4321    /* most-significant byte first (IBM, net) */
#define PDP_ENDIAN      3412    /* LSB first in word, MSW first in long (pdp) */
#endif

#if defined(_MSC_VER)
#ifndef BYTE_ORDER
    #define BYTE_ORDER LITTLE_ENDIAN
#endif
#define USE_GDB_STDINT_H        1                 // <gdb_stdint.h>
#define HAVE_STDIO_H            1                 // <stdio.h>
#define HAVE_MEMORY_H           1                 // <memory.h>
#define HAVE_IO_H               1                 // <io.h>
#define HAVE_FCNTL_H            1                 // <fcnt.h>
#define HAVE_STRING_H           1                 // <string.h>
#endif



#if defined(AIX)
#define HAVE_STDLIB_H           1                 // <stdlib.h>
#define HAVE_UNISTD_H           1                 // <unistd.h>
#define HAVE_SYS_IOCTL_H        1                 // <sys/ioctl.h>
#define HAVE_SYS_TIME_H         1                 // <sys/time.h>
#define HAVE_ASM_IOCTLS_H       1                 // <asm/ioctl.s.h>
#define HAVE_SYS_LIBCSYS_H      1                 // <sys/libcsys.h>
#define HAVE_STRINGS_H          1                 // <strings.h>
#define HAVE_NETDB_H            1                 // <netdb.h>
#define HAVE_ARPA_INET_H        1                 // <arpa/inet.h>
#define HAVE_NET_IF_H           1                 // <net/if.h>
#define HAVE_SYS_SOCKET_H       1                 // <sys/socket.h>
#define HAVE_SYS_POLL_H         1                 // <sys/poll.h>
#define HAVE_MEMORY_H           1                 // <memory.h>
#define HAVE_STDIO_H            1                 // <stdio.h>
#define HAVE_UNISTD_H           1                 // <unistd.h>
#define HAVE_FCNTL_H            1                 // <fcnt.h>


#endif


#if  defined(LINUX)
#define HAVE_UNISTD_H           1                 // <unistd.h>
#define HAVE_SYS_IOCTL_H        1                 // <sys/ioctl.h>
#define HAVE_SYS_TIME_H         1                 // <sys/time.h>
#define HAVE_LTDL_H             1                 // <ltdl.h>
#define HAVE_STDINT_H           1                 // <stdint.h>
#define HAVE_NETDB_H            1                 // <netdb.h>
#define HAVE_ARPA_INET_H        1                 // <arpa/inet.h>
#define HAVE_NET_IF_H           1                 // <net/if.h>
#define HAVE_SYS_SOCKET_H       1                 // <sys/socket.h>
#define HAVE_SYS_POLL_H         1                 // <sys/poll.h>
#define HAVE_MEMORY_H           1                 // <memory.h>
#define HAVE_STDIO_H            1                 // <stdio.h>
#define HAVE_STDLIB_H            1                // <stdlib.h>
#define HAVE_STRING_H           1                 // <string.h>
#define HAVE_FCNTL_H            1                 // <fcnt.h>
#define HAVE_SYS_STAT_H         1                 // <sys/stat.h>
#define HAVE_SYS_TYPES_H        1                 // <sys/types.h>
#define HAVE_NETINET_IN_H       1                 // <sys/netinet/in>
#define HAVE_NETINET_TCP_H      1                 // <sys/netinet/tcp>
//
// This puppy is i386 linux only...
//
#define HAVE_ASM_MSR_H          1                 // <asm/msr.h>

#endif


#if defined(CYGWIN)
#define HAVE_UNISTD_H           1                 // <unistd.h>
#define HAVE_SYS_IOCTL_H        1                 // <sys/ioctl.h>
#define HAVE_SYS_TIME_H         1                 // <sys/time.h>
#define USE_GDB_STDINT_H        1                 // <gdb_stdint.h>
#define HAVE_MEMORY_H           1                 // <memory.h>
#define HAVE_STDIO_H            1                 // <stdio.h>
#define HAVE_IO_H               1                 // <io.h>
#define HAVE_FCNTL_H            1                 // <fcnt.h>

#endif

//
// Bluelight high performance kernel.
//
#if defined(BLHPK)
#ifndef BYTE_ORDER
    #define BYTE_ORDER BIG_ENDIAN
#endif
#define USE_GDB_STDINT_H         0               // <gdb_stdint.h>
#define HAVE_HPK_TYPES_H         1               // <linux/types.h>
#define HAVE_TIME_H              1               // <time.h>
#define HAVE_LINUX_IN_H          1               // <linux/in.h>
#define HAVE_LINUX_STRING_H      1               // <linnux/string.h>
#define HAVE_FCNTL_H             1               // <fcnt.h>
#endif


#endif




