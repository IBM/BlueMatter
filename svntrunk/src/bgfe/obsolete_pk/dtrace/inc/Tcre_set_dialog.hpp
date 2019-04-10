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
 * Module:          Tcre_set_dialog.hpp
 *
 * Purpose:   Dialog box class to allow the selection of trace files
 *            for loading into a group.
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/


/***

  Details of actions
  ==================
  Dialog box requires a parent of type TracerWindow

  On selecting a mapped file for the file list the header of the mapped file
  is read to allow the filling out of the fill

***/

#ifndef TRACER_CRE_SET_CLASS_H
#define TRACER_CRE_SET_CLASS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/MessageB.h>
#include <Xm/ToggleB.h>
#include <Xm/FileSB.h>
#include <Xm/Scale.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <locale.h>
#include <pk/pktrace_pk.hpp>

#include <Ttypes.hpp>
#include <Tdefs.hpp>
#include <Tlib.hpp>

#include <Twindow_class.hpp>
#include <Tgroup_file_class.hpp>
#include <Tmessage_dialog.hpp>
class TracerWindow;
#define MAX_TRACE_FILES 128

//static int  QsortComp(const void* Data1 , const void* Data2 );

class CreateSetDialog
{

  public:
    TracerWindow *ParentWindow;
    Widget       wExtraForm;
    Widget       wInvertedToggle;
    Widget       wCreSetDialog;
    Widget       wCreSetPrevTF;
    Widget       wCreSetPostTF;
    Widget       wCreSetPrevDialog;
    Widget       wCreSetPostDialog;
    Widget       tf_SetName;
    Widget       tf_CSOffset;
    Widget       tf_TraceSeparationValue;
    Widget       liPrev, liPost;
    Widget       lb_PostNumFilesDisplay,lb_PrevNumFilesDisplay;
    Widget       fr_post;
    Widget       wContourToggle;
    Widget       wSelectAllToggle;

    Widget       wVertLineButton;

    char           ColourSelection[32];
    entGraphStyle  GraphStyle;
    int            LastOffset;
    __pktraceRead* MappedFile[ MAX_TRACE_FILES ];
    __pktraceRead* TempMappedFile[ MAX_TRACE_FILES ];
    int            CurrentMappedFileIndex;

    //
    // Lists to store traces
    //
    int            NumPreviousFileListItems;
    ListElement    PreviousFileList[MAX_TRACES];

    int            NumPostFileListItems;
    ListElement    PostFileList[MAX_TRACES];

    // Stores the name of the currently selected mapped file
    char           MappedFileName[ MAX_TRACE_FILES ][ MAX_FILENAME ];

    int            IndeciesMarked[ MAX_TRACE_FILES ];

    // Memebers to handle the delay timer for the filter text changing
    XtIntervalId   TextChangedTimer; 
    Widget         TimerWidget;

    // Memeber to ensure files are selected on pressing OK
    Boolean        IsFileSelected;

    //
    // Constructor
    //
    CreateSetDialog(TracerWindow *Parent);

    //
    // Destructor
    //
    ~CreateSetDialog();

    // 
    // Create the trace list frames and forms
    //
    void CreatePreviousForm(Widget Parent, char* Initial_Filter);
    void CreatePostForm(Widget Parent, char* Initial_Filter);

    //
    // Functions to load the trace lists with data
    void LoadPrevInternalData( char *RegExp );
    void LoadPostInternalData( char *RegExp );

    // Refresh the dialog box when it is displayed
    void UpdateDialog();

    // Loads the group
    virtual Boolean LoadGroupFromList();

    //
    // Callback functions
    //

    virtual void FilterTextTimeOutCB( XtIntervalId* timer_Id);

    virtual void FilterTextChangedCB( Widget w, XtPointer call_data );

    virtual void CreSetOKButtonPressedCB(Widget w,XtPointer call_data);

    //
    // Static definitions
    //
    static void cbDirectoryChanged(Widget w,XtPointer client_data,
                                     XtPointer call_data);

    static void cbForceDirectoryCheck(Widget w,XtPointer client_data,
                                       XtPointer call_data);

    static void cbCreSetCancelButtonPressed(Widget w,
                                   XtPointer client_data, XtPointer call_data);

    static void cbColourChoice( Widget w, XtPointer client_data,
                                      XtPointer call_data );

    static void cbStyleChoice( Widget w, XtPointer client_data,
                                      XtPointer call_data );

    static void cbContourChoice( Widget w, XtPointer client_data,
                                      XtPointer call_data );

    static void cbFileListSelected( Widget w, XtPointer client_data,
                                      XtPointer call_data );

    static void cbTraceSelected( Widget w, XtPointer client_data,
                                      XtPointer call_data );

    static void cbFilterTextChanged(Widget w,XtPointer client_data,
                                                           XtPointer call_data);

    static void cbCreSetOKButtonPressed(Widget w,XtPointer client_data,
                                                           XtPointer call_data);

    static void cbSelectAllTracesChoice(Widget w,XtPointer client_data,
                                                           XtPointer call_data);

    static void cbFilterTextTimeOut(XtPointer client_data, 
                                                        XtIntervalId* timer_Id);

  void ConvertStartToFinis(char *name, char* result );
  int GetIndexForTraceName( char * name );
  void CopyPrevDataToPostData();

  void ResetDialog();

};

#endif
