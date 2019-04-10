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
 * Module:          Twindow_class.hpp
 *
 * Purpose:         Class for the window area. The window area manages the 
 * drawing area of the tracer application. 
 * The main widgets of this class arte:-
 *    form0 - the parent widget
 *    da    - the drawing area, all traces are drawn into this widget
 *    formLabel - the trace control buttons are inserted as children of this
 *                widget
 *
 *    Main point - The SetMidTime is the functions called to cause a 
 *                 screen redraw 
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/

#ifndef TWINDOW_CLASS_H
#define TWINDOW_CLASS_H
// typedef int bool;
#include <pk/pktrace_pk.hpp>

#include <Tinclude.hpp>
#include <Tapplication_class.hpp>
#include <Ttime_mark.hpp>
#include <Titem_class.hpp>
#include <Tgroup_class.hpp>
#include <Tset_scale_dialog.hpp>
#include <Tcre_set_dialog.hpp>
#include <Texport_data_dialog.hpp>
#include <X11/cursorfont.h>

/*******************************************************************************
 * Class Definition for a Trace File
 ******************************************************************************/
class TracerApplication;
class TraceTimeMark;
class TraceItem;
class ExportDialog;
class CreateSetDialog;

// Collection class instantiations for storing lists of groups
// typedef IElemPointer<TraceGroup> TraceGroupPtr;
// typedef ISequence<TraceGroupPtr> TraceGroupListType;
#include <algorithm>
#include <vector>
typedef TraceGroup* TraceGroupPtr;
typedef std::vector<TraceGroupPtr> TraceGroupListType;

class TracerWindow
{
  public:
    // Standard Class Functions
    TracerApplication* GetApplication();
    XtAppContext*      GetApplicationContext();
    Widget             GetApplicationShell();
    Widget             GetApplicationTopWindow();
    void               SendMessageText(char* chMessage);


    // 
    // Data
    //
    // Widgets 
    Widget form0;   // Main form for the window
    Widget da;      // Widget in which trace data is drawn
    Widget formLabel; // Widget containing the trace control buttons
    Widget formTime;  // Widget containing the time data above the da
    Widget formScale; // Widget containing the scale data below the da

    Widget LeftLabel; // 
    Widget MidLabel;  // Widget to display the current midtime

    // The scroll bars and virtual screen 
    int VirtualHeight;
    Widget daVerticalScrollBar;
    Widget daHorizontalScrollBar;
    Widget formVirtualMap;

    // Other data items
    // Data member store the last selected values from the create set dialog
    // Ensures that when the screen is reopenned the old values are retained
    char            StartDirectory[MAX_FILENAME];
    char            CurrentPrevRegExp[MAX_FILENAME];
    char            CurrentPostRegExp[MAX_FILENAME];
    int             CurrentSpacing;
    Boolean         CurrentInvertedValue;
    Boolean         CurrentContourValue;
    char            CurrentColourSelection[MAX_COLOUR_SIZE];
    entGraphStyle   CurrentSelectedStyle;

    // Button press data
    int  y_pos;      // Variable for button press events
    int  XOffset;    // Variable for button press events
    int  button;     // Variable for button press events

    // Time display members
    LTime MidTime;
    LTime MinTime;
    LTime ScanTime;
    LTime WindowStartTime;
    LTime WindowEndTime;

    // Scale values 
    long long Scale;
    int UserDefFlag;
    int  ScaleIndex; // the current

    // Dialog box pointers
    CreateSetDialog* CreateGroupDialogBox;
    ExportDialog  *ExportDialogBox;
    TraceTimeMark *TimeMark;

    // Graphic Contexts
    GC invertedGC;
    GC formGC;
    GC timeGC;
    GC midGC;

    // Pointer to the items that is the current focus
    TraceItem  *CurrentFocusElement;

    // Most important class member
    // A collection class containing all the groups currently loaded and
    // displayed in the drawing area
    TraceGroupListType TraceListSeq;


  public:
    //
    // Functions
    //
    TracerWindow();
    ~TracerWindow();

    // Window time access and manipulation functions
    void SetWindowStartTime(LTime Value);
    void SetWindowEndTime(LTime Value);
    LTime GetWindowStartTime();
    LTime GetWindowEndTime();
    void SetMidPoint(int x);
    int GetScanTime( int x, LTime *thisTime );

    // Window drawing functions
    void SetMidTime(int flag); // The main functions for kicking off 
                               // Drawing
    void guiDrawScaleClicks();
    void DisplayFormTime(LTime t, int button, int xpos );


    void SetCurrentFocusElement(TraceItem *TItem);
    void RecalcScrollBar();
    void AutoLoadGroups(char* StartDirectory, char* RegExpPath);

    //
    // Event Handlers
    //
    static XtEventHandler button_motion_handler(Widget w, XtPointer p,
                                                  XPointerMovedEvent * event);
    static XtEventHandler button_press_handler(Widget w, XtPointer p,
                                                 XButtonPressedEvent *event);
    static XtEventHandler button_release_handler(Widget w, XtPointer ptr,
                                                   XButtonPressedEvent *event);

    static XtEventHandler form_motion_handler(Widget w, XtPointer p,
                                                 XPointerMovedEvent *event);

    static XtEventHandler form_release_handler(Widget w, XtPointer p,
                                                 XButtonPressedEvent *event);

    static XtEventHandler form_press_handler(Widget w, XtPointer p,
                                                 XButtonPressedEvent *event);

    static XtEventHandler form_entered_handler(Widget w, XtPointer p,
                                                 XEnterWindowEvent *event);

    static XtEventHandler form_exited_handler(Widget w, XtPointer p,
                                                 XLeaveWindowEvent *event);

    //
    //Callbacks
    //
    static void drawCB (Widget w, XtPointer client_data, XtPointer call_data);
    static void InputCB(Widget w, XtPointer client_data, XtPointer call_data);

    static void cbCreTraceOption(Widget w, XtPointer client_data,
                                    XtPointer call_data );

    static void cbCreSetOption(Widget w, XtPointer client_data,
                                   XtPointer call_data );

    virtual void HorScrollBarCB(Widget w, XtPointer call_data);
    virtual void VertScrollBarCB(Widget w, XtPointer call_data);

    static  void cbHorScrollBar(Widget w, XtPointer client_data,
                                           XtPointer call_data );
    static  void cbVertScrollBar(Widget w, XtPointer client_data,
                                           XtPointer call_data );
    //
    // File server functions
    //
    // Adds a trace data file to the open file list
    int AddToFileList(char* FileName, __pktraceRead* ptrMappedFile);

    // Get the file position in the open file list for a file name
    int GetFilePosition(char* FileName);

    // Returns a file reader pointer for a given file index
    __pktraceRead* GetMappedPointer(int Index);

  // Mapped file server
  private:
    int                 NumOpenFiles;
    OpenFileListElement OpenFileList[1024];

};
#endif
