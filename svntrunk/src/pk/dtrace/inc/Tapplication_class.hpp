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
 * Module:          Tapplication_class.hpp
 *
 * Purpose:         Class to set up the application and window frame buttons
 *                  Also contains the main application loop.
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/

#ifndef TRACER_APPLICATION_CLASS_H
#define TRACER_APPLICATION_CLASS_H

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
#include <Xm/ToggleB.h>
#include <Xm/MainW.h>
#include <Xm/CascadeB.h>
#include <Xm/Separator.h>

#include <sys/types.h>
#include <fcntl.h>

#include <Twindow_class.hpp>

#include <Ttypes.hpp>
#include <Tdefs.hpp>
#include <Tlib.hpp>
#include <Ttimer_class.hpp>
#include <Tgroup_save_dialog.hpp>
#include <Tload_set_dialog.hpp>
#include <Texport_data_dialog.hpp>

/*******************************************************************************
 * Class Definition for a Tracer Application
 ******************************************************************************/
class TracerWindow;

class TracerApplication
{
   public:
     static TracerApplication* GetApp(int argc = 0, char **argv =NULL);

      // 
      // Functions
      //
      TracerApplication(int argc, char **argv);
      ~TracerApplication();

      // 
      void CreateMainMenu();
      void ParseCommandLine( int argc, char **argv );

      // Callbacks for the menu bar
      static void cbNewWindow(Widget w, XtPointer client_data,
                             XtPointer call_data );

      static void cbOpenFile(Widget w, XtPointer client_data,
                             XtPointer call_data );

      static void cbSaveFile(Widget w, XtPointer client_data,
                             XtPointer call_data );

      static void cbExitFile(Widget w, XtPointer client_data,
                             XtPointer call_data );

      static void cbSaveAsFile(Widget w, XtPointer client_data,
                             XtPointer call_data );

      static void cbZoomDialog(Widget w, XtPointer client_data, 
                             XtPointer call_data );

      static void cbGrid( Widget w, XtPointer client_data,
                             XtPointer call_data );

      static void cbCreatePixelDialog(Widget w,XtPointer client_data,
                             XtPointer call_data);

      static void cbCreateTrace(Widget w,XtPointer client_data,
                             XtPointer call_data);

      void SetTimeScale(int NewScale);

      static void TimeScaleProc( Widget w, XtPointer client_data,
                                       XtPointer call_data );

      static void cbZoomIn( Widget w, XtPointer client_data,
                                       XtPointer call_data );


      static void cbZoomOut( Widget w, XtPointer client_data,
                                       XtPointer call_data );


      static void cbSaveAllSets( Widget w, XtPointer client_data,
                                       XtPointer call_data );

      static void cbExportData( Widget w, XtPointer client_data,
                                       XtPointer call_data );

      int TimeIndex;
      XtWorkProcId WorkId;
      int ZoomProcActive;

      static Boolean ZoomOutWorkProc( XtPointer client_data );

      static void ZoomOutTimerProc(XtPointer client_data,XtIntervalId *timerId);

      static void WarpClickUp( Widget w, XtPointer client_data,
                                 XtPointer call_data );

      static void WarpClickDown( Widget w, XtPointer client_data,
                                 XtPointer call_data );

      void run();

  private:
      // Functions to create sets of widgets
      void CreateScaleToggles(Widget Parent);
      void CreateZoomButtons(Widget Parent);
      void CreateScaleArea(Widget Parent);
      void CreateButtonBar(Widget Parent);
      void CreateMessageArea(Widget Parent);



      // Customised application loop
      void CustomLoop(XtAppContext context);

      //
      // Data
      //
      // Widgets 

  public:
      Widget top;
      Boolean   ExportingDataFlag;
      char   StartDirectory[1024];
      char   RegExpPath[1024];
      XtAppContext   context;
      Display        *display;
      int    AppHeight;
      int    AppWidth;
      Widget MainWinForm;
      Widget bnScale[32];
      Widget frWindowArea;
      Widget lbScaleValue;

  private:
      Widget mainW;
      Widget MenuBar;
      Widget ScaleRadioBox;
      Widget fmScaleRadioBox;
      Widget fr_ZoomFrame;
      Widget fr_ScaleButt;
      Widget ZoomRadioBox;
      Widget fmZoomRadioBox;

      Widget frScaleDisplay;
      Widget ExportButton;
      Widget FilePulldown;
      Widget OptionPulldown;
      Widget ButtonBarForm;

      Widget bnZoomIn;
      Widget bnZoomOut;

      Widget rcScale;

      Widget tbZoomFactor;

      // Message Area
      Widget frMessageWindow;
      Widget tbMessage;
      Widget frZoomSpinBox;
      Widget rcZoomSpinBox;

  public:
      TracerWindow *CurrentWindow;
  
      void SendMessageText(char* chMessage);

      void ResetExportValues();

};

#endif
