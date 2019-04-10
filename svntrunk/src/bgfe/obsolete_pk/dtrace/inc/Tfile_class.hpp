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
 * Project: dtrace
 *
 * Module:  Tfile_class.hpp
 *
 * Purpose: Classes to store the trace data for loading into the GUI.
 *          This class porvides the interface between the GUI and the pktrace
 *          data. The file class stores a pointer to an opened mapped file
 *          and an integer to point to a trace stream within that file.
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/


#ifndef TFILE_CLASS_H
#define TFILE_CLASS_H
//typedef int bool;
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
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/MessageB.h>
#include <Xm/ScrolledW.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrollBarP.h>
#include <Xm/ToggleB.h>
#include <Xm/List.h>
#include <Xm/DrawingA.h>
#include <Xm/MainW.h>
#include <Xm/FileSB.h>
#include <Xm/MessageB.h>
#include <Xm/CascadeB.h>
#include <Xm/PanedW.h>
#include <Xm/Separator.h>
#include <Xm/Scale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/mman.h>
#include <pk/pktrace_pk.hpp>
#include <Ttypes.hpp>
#include <Tdefs.hpp>
#include <Tlib.hpp>
#include <Ttimer_class.hpp>
//#include "pkfxlog.hpp"
#include <pk/fxlogger.hpp>

/*******************************************************************************
 * Class Definition for a Trace File
 ******************************************************************************/
class TraceFile
{
  private:

  public:
    //
    // Functions
    //
    TraceFile(__pktraceRead* MappedFile, int Position);
    ~TraceFile();

    // This function fills the array representing screen pixels
    // The buckets represent the number of hits that occurred for each 
    // time segment represented by a pixel.
    // The calculation is done by finding the hit number that occured at the
    // end of the bucket time slice and subtracting it from the previous bucket
    // hit number. All searches are done using a binary chop to make the 
    // bucket filling function as fast as possible. This is a performance 
    // critical function.

    // MidTime is the time currently at the centre of the drawing area.
    // Width is the number of pixels the drawng area is wide
    // Scale is the number of nanoseconds represented by each pixel
    void       FillBucket(LTime     MidTime,
                          int       Width,
                          long long Scale );

    //
    // Data access functions
    //
    LTime      GetLTimeAtPosition(int Position);
    smalltime  GetsmalltimeAtPosition(int Position);

    // Returns the firsts and last hits in the buffer
    LTime      GetFirstHit();
    LTime      GetLastHit();     
    int        GetTimeBucketValue(int Position);
    void       SetTimeBucketValue(int Position, int Value);

    LTime      GetBucketThreshold(int Position);
    LTime      SetBucketThreshold(int Position);

    LTime      GetNextTime(int  Direction, LTime MidTime);

    int        GetNumberOfHits();
    LTime      GetStartTime();

    int        GetHitBeforeBucket();
    void       SetHitBeforeBucket(int Value);

    char*      GetName();

    int        FindHitPos(LTime      SearchTime);
    int        IsHitInRange(LTime      SearchTime);

    //
    // Data
    //

  private:
    int        BinarySearch(LTime TargetTime);

    __pktraceRead* pMappedFile; /* The mapped file that the trace points will be
                                   loaded from */
    int            FilePosition;/* The position in the mapped file header that 
                                   the TraceFile class will represent */

    int        NumTraceTimes;   /* Number of time stamps */

    int        HitBeforeBucket; /* Stores the hit number that occurs before the
                                   valid bucket */

    /* Time bucket structure to contain the number of hits. */
    struct     stTimeBucket{
                 int value;          // The number of hits stored in the bucket
                 long long Threshold;// The time segment boundary of the bucket.
                 };

    // The time bucket array, declared to be the same size as the drawing area
    stTimeBucket  TimeBucket[ daWidth ];

};

/*******************************************************************************
 * Class Definition for a Relative Trace
 ******************************************************************************/
class Trace_Relative
{
   public:
      Trace_Relative(__pktraceRead*, int, int);
      ~Trace_Relative();

   //Data
   TraceFile *PrevFile;
   TraceFile *PostFile;
};

/*******************************************************************************
 * Class Definition for a Plot Trace
 ******************************************************************************/
class Trace_Plot
{
   public:
      Trace_Plot(__pktraceRead*, int );
      ~Trace_Plot();

   // Data
   TraceFile *PlotFile;
};

#endif
