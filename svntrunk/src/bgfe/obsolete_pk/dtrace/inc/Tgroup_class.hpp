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
 * Module:          Tgroup_class.hpp
 *
 * Purpose:         Class to store groups of TraceItems.
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/

#ifndef TGROUP_CLASS_H
#define TGROUP_CLASS_H
// typedef int bool;
#include <pk/pktrace_pk.hpp>

#include <Tinclude.hpp>
#include <Tapplication_class.hpp>
#include <Twindow_class.hpp>
#include <Titem_class.hpp>
#include <Tgroup_properties_dialog.hpp>
#include <Tgroup_save_dialog.hpp>

/*******************************************************************************
 * Class Definition for a Trace Group
 ******************************************************************************/
class TracerApplication;
class TracerWindow;
class TraceItem;

/*******************************************************************************
 * Class Definition for a TraceListType
 ******************************************************************************/

/* Instantiation of the collection classes to store the TraceItems. */
// typedef IElemPointer<TraceItem> TraceItemPtr;
// typedef IEqualitySequence<TraceItemPtr> TraceListType;
// #include <stl.h>
#include <algorithm>
#include <vector>
typedef TraceItem* TraceItemPtr;
typedef std::vector<TraceItemPtr> TraceListType;

class TraceGroup{

  public:
    // 
    // Standard parent access functions
    //
    TracerApplication* GetApplication();
    XtAppContext*      GetApplicationContext();
    Widget             GetApplicationShell();
    Widget             GetApplicationTopWindow();
    TracerWindow*      GetWindow();

    // 
    // Data members
    //
    TracerWindow *ParentWindow;
    int           StepHeight;
    int           ContourSpacing;
    int           Lab_button;
    int           MoveActive;
    int           Movey;

    Widget        wPopUp;
    Widget        wGroupButton;
    Widget        wVirtualGroupButton;

    char          GroupName[LABEL_SIZE];

    //
    // Flags to indicate group drawing characteristics 
    //
    Boolean       InvertedTrace;
    Boolean       ContourEnabled;

    // Group file information
    char          DirName[MAX_FILENAME];
    char          PrevRegExp[MAX_FILENAME];
    char          PostRegExp[MAX_FILENAME];
    char          GroupColor[MAX_COLOUR_SIZE];
    entGraphStyle GroupGraphStyle;

    int           GroupYOffset;
    int           TraceSeparation;

    TraceItem     *CurrentHighest;
    TraceItem     *CurrentLowest;

    TraceListType TraceList;

  private:
    LTime GroupStartTime;
    LTime GroupEndTime;

  //
  // Functional members
  //
  public:
    // Constructors
    TraceGroup();
    TraceGroup(TracerWindow* Parent,
                 char*         Name,
                 char*         pchDirName,
                 char*         pchPrevRegExp,
                 char*         pchPostRegExp,
                 char*         pchColor,
                 entGraphStyle enGraphStyle,
                 int           GroupOffset,
                 int           NewTraceSeparation = 5,
                 Boolean       Inverted  = FALSE,
                 Boolean       Contoured = FALSE);

    // Destructors
    ~TraceGroup();

    // Initializer
    void Initialize(TracerWindow* Parent,
                    char*         Name,
                    char*         pchDirName,
                    char*         pchPrevRegExp,
                    char*         pchPostRegExp,
                    char*         pchColor,
                    entGraphStyle enGraphStyle,
                    int           GroupOffset,
                    int           NewTraceSeparation = 5,
                    Boolean       Inverted  = FALSE,
                    Boolean       Contoured = FALSE);

    Widget      CreateButton( );

    //
    // Operators
    //
    int operator == (TraceGroup const& t) const;
    int operator  < (TraceGroup const& t) const;
    TraceGroup& operator = (TraceGroup const& from) ;

    // Group drawing functions
    // Simply uses a cursor to iterate through all the loaded items 
    // and calls their Draw functions.
    void DrawGroup(int flag);

    void SetItemsOutOfDate(); // Iterates through all the items of the group
                              // setting their DataUpToDate flag to false.
                              // Prepares them for a redraw.

    void DoContourMapping(); // Draw the contours on lines if it is enabled


    // 
    // Event handlers for the group button
    //
    static void group_press_handler(Widget w,  XtPointer p,
                                              XButtonPressedEvent *event);

    static void group_release_handler(Widget w,  XtPointer p,
                                              XButtonPressedEvent *event);

    static void group_motion_handler(Widget w,  XtPointer p,
                                              XPointerMovedEvent *event);

    // Functions to resize and move the group button.
    Widget     GetHighestTraceYCoord(); // Finds the top of the bar
    Widget     GetLowestTraceYCoord();  // Finds the bottom of the bar
    void       ResizeGroupBar();   // Resizes the bar when attached items have
                                   // been moved
    void       ReattachItems(); // Reset offset of attached items after the bar
                                // has finished movving
    void       DetachItems(); // Prepare items for moving when the bar is first
                              // moved

    //
    // Popup menu callback functions
    // 
    static void cbLabelPopUp(Widget w, XtPointer client_data,
                               XtPointer call_data );

    static void cbDelete(Widget w, XtPointer client_data,
                               XtPointer call_data );

    static void cbProperties(Widget w, XtPointer client_data,
                               XtPointer call_data );

    static void cbSave(Widget w, XtPointer client_data, XtPointer call_data );


    //
    // Group property modification functions
    //
    void ChangeColor(char *NewColour);
    void ChangeInverted(Boolean Value);
    void ChangeContour(Boolean Value);
    void ChangeProperties(char *NewColour, Boolean Value, Boolean ContourValue);

    // Function to add TraceItems to the group
    // This function takes the paremeters needed to contruct a trace item,
    // creates a TraceItem object and adds it to the TraceList.
    // Group start and end times are modified during this function.
    void        AddTrace(int            YOffset,
                         int            Show,   
                         char          *Colour,
                         TRACE_TYPE     Type,
                         __pktraceRead* MappedFile,
                         int            pchFile1,
                         int            pchFile2,
                         entGraphStyle  NewGraphStyle,
                         Boolean        Inverted = FALSE);



    // Access functions for GroupStart and GroupEnd times.
    LTime GetGroupStartTime();
    LTime GetGroupEndTime();

    void SetGroupStartTime(LTime NewValue);
    void SetGroupEndTime(LTime NewValue);
};
#endif
