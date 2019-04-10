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
 #ifndef UDF_RegistryRecordMacros_hpp
#define UDF_RegistryRecordMacros_hpp

#include <stdio.h>
#include <string.h>
#include <assert.h>

#define UDF_UNDEF(FX_NO) \
           enum{ UDF_CODE##FX_NO = -1 };  \
           RegistryRecord RegistryRecord##FX_NO = { -1, "NOT DEFINED", "NOT DEFINED" }

#define UDF_1PARM(FX_NO,FXNAME,T0,P0) \
           enum{ UDF_CODE##FX_NO = FX_NO };  \
           RegistryRecord RegistryRecord##FX_NO = { FX_NO, MAC2STR(FXNAME), MAC2STR(T0 P1;)}


#define UDF_2PARM(FX_NO,FXNAME,T0,P0,T1,P1) \
           enum{ UDF_CODE##FX_NO = FX_NO };  \
           RegistryRecord RegistryRecord##FX_NO = { FX_NO, MAC2STR(FXNAME), MAC2STR(\n  T0 P0;\n T1 P1;\n)}

#define UDF_3PARM(FX_NO,FXNAME,T0,P0,T1,P1,T2,P2) \
           enum{ UDF_CODE##FX_NO = FX_NO };  \
           RegistryRecord RegistryRecord##FX_NO = { FX_NO, MAC2STR(FXNAME), MAC2STR(\n  T0 P0;\n T1 P1;\n T2 P2;\n)}

#define UDF_4PARM(FX_NO,FXNAME,T0,P0,T1,P1,T2,P2,T3,P3) \
           enum{ UDF_CODE##FX_NO = FX_NO };  \
           RegistryRecord RegistryRecord##FX_NO = { FX_NO, MAC2STR(FXNAME), MAC2STR(\n  T0 P0;\n T1 P1;\n T2 P2;\n T3 P3;\n)}

#define UDF_5PARM(FX_NO,FXNAME,T0,P0,T1,P1,T2,P2,T3,P3,T4,P4) \
           enum{ UDF_CODE##FX_NO = FX_NO };  \
           RegistryRecord RegistryRecord##FX_NO = { FX_NO, MAC2STR(FXNAME), MAC2STR(\n  T0 P0;\n T1 P1;\n T2 P2;\n T3 P3;\n T4 P4;\n)}


#define MAC2STR(MACRO) #MACRO

class RegistryRecord
  {
  public:
    int        UdfCode;
    char      *UdfName;
    char      *UdfArgs;
  };


#endif
