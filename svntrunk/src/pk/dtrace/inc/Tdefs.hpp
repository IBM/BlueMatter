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
 #ifndef TRACER_DEFS_H
#define TRACER_DEFS_H



#define PIXEL_INTERVAL   1
#define STEP_HEIGHT      4
#define BUTTON_OFFSET    0
#define BUTTON_PIXEL     3
#define BUTTON_QUERY     1
#define TIMER_INTERVAL   20
#define APPTITLE         "Tracer v 1.01"
#define APPCLASS         "TracerUtility"
#define LISTBOX_ITEMS     5
#define BELOW_SB         20
#define NS_SECOND        1000000000
#define NS_TENTH         100000000
#define NS_HUNDREDTH     10000000
#define NS_MILLI         1000000
#define NS_MICRO         1000
#define NS               1
#define ButtonBarHeight 30
#define USER_DEFINED     11
#define MAX_COLOUR_SIZE  48

#define daLeft         30
#define daTop          20
#define daHeight       735
#define VIRTUAL_FACTOR 4
#define vdaHeight      (VIRTUAL_FACTOR * daHeight)

#define daWidth     1205
#define FORM_WIDTH  150
#define FIRST_BUCKET  0

#define FORM_OFFSET  0x00 

#define MAX_F           1024
#define MAX_TIME_BUCKET 2048
#define LABEL_SIZE      16

#define BG_COLOR "LightSlateGrey"

#define NUM_GRAPH_STYLES 3

#define MYNEW VALUE    12

// #define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define FULL_REDRAW 1
#define REFRESH     2

#define AUTO_LOAD   0
#define MANUAL_LOAD 1

//typedef int bool;

#endif
