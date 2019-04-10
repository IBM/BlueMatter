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
 * Module:  Titem_class.hpp
 *
 * Purpose: Class to graphically represent the trace items in the application.
 *          The most important members of this class are:-
 *          relative, plot - the members that link the application to  the 
 *                           pktrace mapped file.
 *          traceGC - the graphic context to draw the trace plots to the
 *                    parent window drawing area. This is create at 
 *                    construction and deleted at destruction, remains
 *                    in place through objects life to improve drawing
 *                    performance.
 *
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/

#ifndef TITEM_CLASS_H
#define TITEM_CLASS_H
//#include "../../foxpk/pktrace.hpp"
//#include "../../foxpk/pkfxlog.hpp"
//typedef int bool;
#include <pk/fxlogger.hpp>
#include <pk/pktrace_pk.hpp>

#include <Tinclude.hpp>
#include <Tapplication_class.hpp>
#include <Twindow_class.hpp>
#include <Tfile_class.hpp>
#include <Tproperties_dialog.hpp>
#include <pthread.h>

#include <algorithm>
#include <vector>

/*******************************************************************************
 * Class Definition for a Trace Item
 ******************************************************************************/
class TracerApplication;
class TraceGroup;
class TracerWindow;

class TraceItem{

  // Data Members
  public:
    TraceGroup*     Parent ;
    Widget          wButton ; // Widget for the trace control button
    Widget          wPopUp;   // Widget for the item popup menu
    int             show;
    int             Movey;

    // The file memebers, very important memeber
    Trace_Relative *relative;  // Trace values for relative trace
    Trace_Plot     *plot;      // Trace values for plot trace

    // Graphic contexts
    GC              traceGC;    // Graphic context to draw the trace lines

    // Trace characteristics
    char            Label[LABEL_SIZE]; // Trace name
    int             Y_AxisOffset;      // Relative offset of the trace
    char            color[MAX_COLOUR_SIZE]; // Colour of the trace line
    TRACE_TYPE      type;                   // Type, plot or relative
    Boolean         InvertedTrace; // Flag for whether trace inversion is 
                                   // enabled
    entGraphStyle   GraphStyle;    // Type of plot, flat, vertical or points

    // Data structures for drawing 
    // Flat Line
    int        n;
    XPoint     PointArray[(daWidth * 2)];

    // Vertical plots and crosses for the POINTS_ONLY trace
    int        paPos;
    int        saPos;
    XSegment   SegmentArray[(daWidth * 2)];

    // Member for determining how new the drawing data is
    Boolean    DataUpToDate;

  // Function members
  public:

    // Functions to access parent data
    TracerApplication* GetApplication();
    XtAppContext*      GetApplicationContext();
    Widget             GetApplicationShell();
    Widget             GetApplicationTopWindow();
    TraceGroup*        GetGroup();
    TracerWindow*      GetWindow();

    // Constructors
    TraceItem(TraceGroup*    ptgParent,
              int            YOffset,
              int            Show,
              char*          Colour,
              TRACE_TYPE     Type,
              __pktraceRead* MappedFile,
              int            pchFile1,
              int            pchFile2,
              entGraphStyle  NewGraphStyle,
              Boolean        Inverted = FALSE);

    // Destructor
    ~TraceItem();

    // Operators
    int   operator == (TraceItem const& t) const;
    int   operator  < (TraceItem const& t) const;
    TraceItem& operator  = (TraceItem const& from) ;


    LTime      GetStartTime();
    LTime      GetEndTime();


    // The drawing functions, one for each type of plot available
    void RedrawTrace(int flag);

    void        RelativePlot(int flag);
    void        RelativePlotNoVert(int flag);
    void        RelativePlotPointsOnly(int flag);

    Widget      CreateButton( );
    LTime   GetNextTime( int );

    //
    // Event handlers
    //
    static void label_press_handler(Widget w,  XtPointer p,
                                              XButtonPressedEvent *event);

    static void label_release_handler(Widget w,  XtPointer p,
                                              XButtonPressedEvent *event);

    static void label_motion_handler(Widget w,  XtPointer p,
                                              XPointerMovedEvent *event);

    static XtEventHandler label_entered_handler(Widget w, XtPointer ptr,
                                      XEnterWindowEvent *event);

    static XtEventHandler label_exited_handler(Widget w, XtPointer ptr,
                                      XLeaveWindowEvent *event);

    static XtEventHandler label_exited_bd_handler(Widget w, XtPointer ptr,
                                      XLeaveWindowEvent *event);

    //
    // Callback functions
    //
    static void cbProperties(Widget w, XtPointer client_data,
                                                          XtPointer call_data );
    static void cbDelete(Widget w, XtPointer client_data,
                                                          XtPointer call_data );
    static void cbMakeFocus(Widget w, XtPointer client_data,
                                                          XtPointer call_data );

    static void cbGotoStart(Widget w, XtPointer client_data,
                               XtPointer call_data );

    static void cbGotoEnd(Widget w, XtPointer client_data,
                               XtPointer call_data );


    //
    // Functions to modify class properties
    //
    void ChangeGCFunction(char* NewGC);
    void ChangeColor(char *NewColour);
    void ResetColourMemebers(char *NewColor);
    void ResetInvertedMemebers(Boolean Value);
    void ChangeSelectedStyle(char* NewStyle);

    // 
    // Functions for moving items control button
    //
    void Detach();
    void Attach();
  
  // Internal functions  
  private:
    int      GetTraceStartingState();
    void     DrawVertGraphics(int StartCurrentState, int flag);
    int      IsPointValid(    int Position);
};

struct TransMechItem{
              TraceItem* pthis;
              int x;
              int y;
             };

// typedef IElemPointer<TraceItem> TraceItemPtr;
// typedef IEqualitySequence<TraceItemPtr> TraceListType;
typedef TraceItem* TraceItemPtr;
typedef std::vector<TraceItemPtr> TraceListType;

#endif
