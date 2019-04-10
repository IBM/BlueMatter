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
 #ifndef _DBGPRINT_H_
#define _DBGPRINT_H_

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////
// This file contains the definition needed to do debug printf
// to the debug output.
//
#ifndef BLHPK
int DBGPrintf(const char *szFormat, ... );
#endif

#ifndef INLINE
#if defined(_MSC_VER)
    #define INLINE static
#else
    #if ((__GNUC__ >= 2) || (defined(_XLC_)))
                #define INLINE static inline
    #else
                #define INLINE
    #endif
#endif
#endif

//
// If these are not defined by include <assert> then they are not going to be defined.
// make some defaults.
//
#ifndef __ASSERT_VOID_CAST
#define __ASSERT_VOID_CAST (void)
#endif

#ifndef __ASSERT_FUNCTION
#define __ASSERT_FUNCTION "\0"
#endif

#ifndef __STRING
#define __STRING(x) #x
#endif

#ifdef _DEBUG


#ifdef BLHPK
        #include <sys/blhpk_env.h>
        #define dbgPrintf s0printf
#else
        #define dbgPrintf DBGPrintf
#endif

//
// for Windows, assert is non fatal and puts the error right in your face...
//
#if defined(_MSC_VER)
    #include <assert.h>
    #define ASSERT(n) assert((n))
#else
    //
    // For GNU assert is fatal. so don't use it...  Just debug print the error instead.
    //
        #ifdef BLHPK
            #define __ASSERT_TEXT__ "!!!!!!!!!!!!!!!ASSERT!!!!!!!!!!!!!!!!!\r\n%s: %s: %d in %s\r\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n"
        #else
            #define __ASSERT_TEXT__ "!!!!!!!!!!!!!!!ASSERT!!!!!!!!!!!!!!!!!\n%s: %s: %d in %s\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
        #endif
    # define ASSERT(expr) \
      (__ASSERT_VOID_CAST ((expr) ? 0 :                                       \
                           (dbgPrintf(__ASSERT_TEXT__, \
                              __STRING(expr), __FILE__, __LINE__,    \
                                              __ASSERT_FUNCTION))))

#endif

#else

# define ASSERT(expr)           (__ASSERT_VOID_CAST (0))

#if defined(_MSC_VER)
    #define  dbgPrintf (void)(0)
#else
        #ifdef __cplusplus
                INLINE void dbgPrintf(const char *, ...){};
        #else
//BGF                INLINE void dbgPrintf(const char *szFormat UNUSED, ...){};
                INLINE void dbgPrintf(const char *szFormat, ...);

        #endif
#endif


#endif

#ifdef __cplusplus
};
#endif



#endif
