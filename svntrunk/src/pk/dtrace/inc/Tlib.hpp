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
 #ifndef TRACER_LIB_H
#define TRACER_LIB_H

// typedef int bool;

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
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/ArrowB.h>
#include <fnmatch.h>
#include <Ttypes.hpp>

extern char * scaleLabels[];

extern char* scaleValues[];

extern int ScaleArray[12];

extern char* ScaleStrings[12];

extern long zoomValues[];

extern char* GraphStyleStrings[];

extern double ZoomArray[];
extern char* ZoomStrings[];

#define NUM_GC_FUNCTIONS  16
#define NUM_COLOUR_STRINGS 19

extern char* GCStrings[NUM_GC_FUNCTIONS];


// Functions

LTime TbToLTime( timebasestruct_t tb );
Boolean IsSelected(XmListCallbackStruct* ptr);
int ConstructGC( GC * gc, Widget w, int width, char *fg,
                 char *bg, char *f, int Extraflags, int function );

Widget CreateGenericColorPullDownMenu(Widget parent,
                                      XtCallbackProc callback,
                                      XtPointer client_data,
                                      int SelectedValue);

Widget CreateGenericGraphStylePullDownMenu(Widget parent, 
                               XtCallbackProc callback,
                               XtPointer client_data,
                               int SelectedValue,
                               Widget *VertButton);

Widget CreatePushButton (Widget parent,
                         char * name,
                         XtCallbackProc  callback,
                         XtPointer       client_data);

Widget CreateToggleButton (Widget parent,
                         char * name,
                         XtCallbackProc  callback,
                         XtPointer       client_data);

Widget CreateArrowButton (Widget parent,
                         char * name,
                         XtCallbackProc  callback,
                         XtPointer       client_data);

int GetPositionOfColor(char* SelectedColor);

char* CreateSecondsFromLTime(LTime ltTime);

void no_storage_handler();

struct TransportMech{
                     void* pthis;
                     int        iValue;
                     double     dValue;
                     long       lValue;
                     char      *pchValue;
                     long long  llValue;
                    };

void GetPathFromFile(char* FileName,char* FileOnly,  char* DirName);
Boolean CheckReg(char *RegExp, char* Tracename);

#endif
