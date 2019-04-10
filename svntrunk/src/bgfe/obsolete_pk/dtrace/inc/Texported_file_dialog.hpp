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
 * Module:          Texported_file_dialog.hpp
 *
 * Purpose:         Class to create and display the dialog box for displaying
 *                  information about exported trace stream files.
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/

#ifndef TEXPORTED_FILE_DIALOG_H
#define TEXPORTED_FILE_DIALOG_H

#include "Twindow_class.hpp"
#include "Tfile_class.hpp"

#include "Tmessage_dialog.hpp"
#include "Tapplication_class.hpp"
#include "Texport_data_dialog.hpp"
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>

#include <errno.h>

class TracerWindow;

class ExportedFileDialog
{
  
  //
  // Data
  //
  private:

    Widget wExportedDataMain; // Dialog widget

    TracerWindow *Parent;     // Pointer to parent window

    Widget wTraceList;        // List widget

    Widget wCancelButton, wViewButton; // Buttons
    int    SelectedFilePos;            // Marker for the currently selected  
                                       // trace stream

  //
  // Member functions
  //
  public:
    // Constructor
    ExportedFileDialog(Widget             ParentWidget,
                       TracerWindow      *ParentWindow,
                       struct ExportList *ListOfExportFiles,
                       int                NumFilesExported);
    // Destructor
    ~ExportedFileDialog();

  protected:
    // Function to load data
    void LoadListData(struct ExportList *ListOfExportFiles,
                      int                NumFilesExported);

    //
    // Callback functions
    //
    static void cbViewButtonPressed      (Widget w, XtPointer client_data,
                                          XtPointer call_data);

    static void cbCancelButtonPressed    (Widget w, XtPointer client_data,
                                          XtPointer call_data);

    static void cbTraceSelected          (Widget w, XtPointer client_data,
                                          XtPointer call_data);

};

#endif
