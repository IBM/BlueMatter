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
 #ifndef TLOAD_SET_DIALOG_H
#define TLOAD_SET_DIALOG_H

#include <Twindow_class.hpp>
#include <Tmessage_dialog.hpp>
class TracerWindow;

#define TRACE_FILE_DIALOG  0
#define REGEXP_FILE_DIALOG 1

class LoadGroup
{
  public:
    TracerWindow *Parent;

    Widget wLoadGroupMain;
    Widget wtbLoadGroupTraceFile;
    Widget wtbLoadGroupRexpFile;
    Widget wFileSelDialog;
    Widget wTraceFileButton, wRexpFileButton;
    Widget wCancelButton, wOKButton;
    LoadGroup(TracerWindow *ParentWindow);
    ~LoadGroup();

    int     FileDialogType;
   protected:
      static void cbOKButtonPressed        (Widget w, XtPointer client_data,
                                                      XtPointer call_data);
      static void cbCancelButtonPressed    (Widget w, XtPointer client_data,
                                                      XtPointer call_data);
      static void cbFileSelectButtonPressed(Widget w, XtPointer client_data,
                                                      XtPointer call_data);

      static void cbFDOKButtonPressed(Widget w, XtPointer client_data,
                                                      XtPointer call_data);

      static void cbFDCancelButtonPressed(Widget w, XtPointer client_data,
                                                      XtPointer call_data);

};

#endif
