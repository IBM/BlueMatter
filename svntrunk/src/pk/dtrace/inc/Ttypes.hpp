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
 * Project:         dtrace
 *
 * Module:          Ttypes.hpp
 *
 * Purpose:         File contains general type definitions used throughout
 *                  the tracer classes. Some of these types may be obselete
 *                  but I didn't want to remove them at this late stage.
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/

#ifndef TRACER_TYPES_H
#define TRACER_TYPES_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>
#include <pk/pktrace_pk.hpp>

#include <Tdefs.hpp>

// typedef int bool;

typedef long long LTime;
typedef enum
{
   S_TENTH =0,
   S_ONE,
   S_TWO,
   S_FOUR,
   S_SIX,
   S_EIGHT,
   S_TEN,
   NUM_STEPS
}STEPS;


typedef enum
{
    RELATIVE = 0, PLOT
}TRACE_TYPE;

/* Structure of */
typedef struct
{
    LTime       AbsoluteStartTime;  /*Time when program hit trace first. */
    LTime       StartTime;          /*Right most time on veiwer. */
    LTime       TimeInterval;       /*Time interval for each veiwer time slot.*/
    int         duration;           /*Number of time slots? */

}TRACE_VIEWER;

typedef enum
{
   
   P_ONE =0,
   P_TWO,
   P_FIVE,
   P_TEN,
   P_TWENTY,
   P_FOURTY,
   P_HUNDRED,
   NUM_PIX
}PIX;


enum entGraphStyle{FLAT_LINE_STYLE = 0,VERT_LINE_STYLE ,POINTS_ONLY_STYLE };


typedef enum
{
   PREVIOUS = 1,
   POST,
   NONE
}STATES;

typedef struct 
{
   Widget Label;
   LTime  Time;
   LTime  Scale;
   int    Num;

}MARKER;

struct OpenFileListElement {
   char           Filename[2048];
   __pktraceRead* pMappedFile;
};

struct ListElement{
    char Filename[1024];
    int  MappedHeaderPos;
    int  FileIndex;
};

#endif

