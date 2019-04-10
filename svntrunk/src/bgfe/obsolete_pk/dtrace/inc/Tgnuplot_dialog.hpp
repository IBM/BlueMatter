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
 * Module:          Tgnuplot_dialog.hpp
 *
 * Purpose:         Class to create and display the dialog box for displaying
 *                  and controlling the GNU plot application.
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/

#ifndef TGNUPLOT_DIALOG_H
#define TGNUPLOT_DIALOG_H

#include "Twindow_class.hpp"
#include "Tmessage_dialog.hpp"
#include "Tapplication_class.hpp"
#include <Xm/SelectioB.h>
#include <errno.h>

class TracerWindow;

class GNUPlotDialog
{
  private:
    FILE *command; // Pointer to the file for opening a process to run the
                   // GNU plot application

  public:
    TracerWindow *Parent; // Pointer to the parent window

    Widget wGNUPlotMain; // Main dialog box widget

    Widget wQuitButton, wApplyButton; // Button widgets

    Widget wGNUPlotText; // Command entry widget

    // Constructor
    GNUPlotDialog(Widget        ParentWidget,
                  TracerWindow *ParentWindow,
                  char*         InitialCommand);

    // Destructor
    ~GNUPlotDialog();

  protected:
    //
    // Callback functions
    //
    static void cbApplyButtonPressed   (Widget w, XtPointer client_data,
                                          XtPointer call_data);

    static void cbQuitButtonPressed    (Widget w, XtPointer client_data,
                                          XtPointer call_data);

};

#endif

