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
 * Module:          Tproperties_dialog.hpp
 *
 * Purpose:         Class to display time differences between two points on 
 *                  a trace stream.
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/

#ifndef TRACER_TIME_MARK_CLASS_H
#define TRACER_TIME_MARK_CLASS_H

#include <Tinclude.hpp>
#include <Twindow_class.hpp>

/*******************************************************************************
 * Class Definition for a Trace File
 ******************************************************************************/

class TracerWindow;

class TraceTimeMark
{
   public:
      //
      //Data Elements
      //
      Widget StartButton;
      Widget StopButton;
      Widget ControlButton;
      Widget ValueLabel;
      int    StartPosition;
      int    EndPosition;
      TracerWindow *ParentWindow;
      char   ScaleValue[32];
      long long Scale;

      // 
      // Functions
      //

      // Constructors 
      TraceTimeMark(TracerWindow *Parent,int StartPos,char* pchScaleValue,
                    long long ScaleVal);
      TraceTimeMark(const TraceTimeMark&);

      // Destructor
      ~TraceTimeMark();

      // Creates the button marks above the drawing area to show start and
      // end points
      void CreateMark();

      // Moves the end point marker of the time mark, recalculates the 
      // time difference and displays the new time
      void MoveEndPoint(int X_Position);

      // Finds the times at the start and end point markers
      LTime  GetStartTime();
      LTime  GetEndTime();

      // Event handlers
      static void mark_press_handler(Widget w,XtPointer p,
                               XButtonPressedEvent *event);

      static void mark_release_handler(Widget w, XtPointer p,
                                 XButtonPressedEvent *event);

      static void mark_motion_handler(Widget w,  XtPointer p,
                                XButtonPressedEvent *event);
};

#endif
