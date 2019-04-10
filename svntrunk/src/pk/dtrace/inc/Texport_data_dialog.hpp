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
 * Module:          Texport_data_dialog.hpp
 *
 * Purpose:         Class for the Export data dialog box
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/

#ifndef TEXPORT_DATA_DIALOG_H
#define TEXPORT_DATA_DIALOG_H

#include <Twindow_class.hpp>
#include <Tfile_class.hpp>

#include <Tmessage_dialog.hpp>
#include <Tapplication_class.hpp>
#include <Tgnuplot_dialog.hpp>
#include <Texported_file_dialog.hpp>

#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>

#include <errno.h>

class TracerWindow;

struct ExportList{
    TraceItem*  ListItem;
    char        DataFileName[(MAX_FILENAME * 2) + 5];
    char        PostDataFileName[(MAX_FILENAME * 2) + 5];
    Boolean     PrevSelected;
    Boolean     PostSelected;

    };

class ExportDialog
{
  private:
    int         NumTracesInList ;
    ExportList  ExportTraceList[MAX_TRACES];

  
  public:
    TracerWindow *Parent;

    Widget wDialogShell;
    Widget wExportDataMain;
    Widget wPrevTraceList;
    Widget wPostTraceList;
    Widget fm_FromTime;
    Widget wToggleExpFileAll;
    Widget wToggleExpFileSelected;

    Widget wtbFromTime;
    Widget wtbToTime;

    Widget FileOptToggleButtonBox;
    Widget FileFormatToggleButtonBox;

    Widget wToggleExpFileSecs;
    Widget wToggleExpFileLTime;
    Widget wToggleExpFileRawData;

    Widget wToggleExpFile;
    Widget wToggleExpPlot;

    LTime ExportStartTime;
    LTime ExportEndTime;

    Widget wExportDialogCancelButton;
    Widget wExportDialogOKButton;

    ExportDialog(Widget        parentWidget,
                 TracerWindow *ParentWindow,
                 LTime         StartTime,
                 LTime         EndTime);

    ~ExportDialog();

    static void cbOKButtonPressed        (Widget w, XtPointer client_data,
                                                    XtPointer call_data);
    static void cbCancelButtonPressed    (Widget w, XtPointer client_data,
                                                      XtPointer call_data);
    static void cbPrevTraceSelected          (Widget w, XtPointer client_data,
                                                      XtPointer call_data);

    static void cbPostTraceSelected          (Widget w, XtPointer client_data,
                                                      XtPointer call_data);


    static void cbTogglePressed      (Widget w, XtPointer client_data,
                                                      XtPointer call_data);

    static void cbFileOptTogglePressed      (Widget w, XtPointer client_data,
                                                      XtPointer call_data);


    void LoadListData();
    void CreateSelectedTraceData();
    void CreateSelectedTraceDataForFiles(TraceFile*  FileToExport,
                                         char*        Filename);

    void FillScrolledList();
};

#endif
