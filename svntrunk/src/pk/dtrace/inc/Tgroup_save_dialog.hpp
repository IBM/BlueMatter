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
 * Module:          Tgroup_save_dialog.hpp
 *
 * Purpose:         Classes to create and display the dialog box for
 *                  Savng groups and window data to file.
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/
#ifndef TGROUP_SAVE_DIALOG_CLASS_HPP
#define TGROUP_SAVE_DIALOG_CLASS_HPP

#include <Tinclude.hpp>
#include <Tgroup_class.hpp>
#include <Tgroup_file_class.hpp>
#include <Twindow_class.hpp>
class TraceGroup;

/****************************************************************************
 *
 * Class: GroupSaveDialog
 *
 * Purpose: Dialog box to save group data to file
 *
 ***************************************************************************/

class GroupSaveDialog
{
  public:
    //
    // Data
    //
    TraceGroup *ParentItem;
    Widget wGroupSaveDialog;
    Widget CancelButton;
    Widget OKButton;
 
    //
    // Functions
    //
    // Constructors
    GroupSaveDialog();
    GroupSaveDialog(TraceGroup *pParentGroup);
    // Destructor
    ~GroupSaveDialog();

    // Callbacks
    virtual void cbCancelButtonPressed(XtPointer call_data);
    virtual void cbOKButtonPressed(XtPointer call_data);

  protected:
    static void CancelButtonCB(Widget w,
                                   XtPointer client_data, XtPointer call_data);
    static void OKButtonCB(Widget w,
                                   XtPointer client_data, XtPointer call_data);

};

/****************************************************************************
 *
 * Class: WindowGroupSaveDialog
 *
 * Purpose: Dialog box to save all on screen groups data to file
 *
 ***************************************************************************/

class WindowGroupSaveDialog: public GroupSaveDialog
{
  TracerWindow *ParentWindow;

  public :
    // Constructor
    WindowGroupSaveDialog(TracerWindow *pParentWindow);
    // Destructor
    ~WindowGroupSaveDialog();

    // Callbacks
    virtual void cbCancelButtonPressed(XtPointer call_data);
    virtual void cbOKButtonPressed(XtPointer call_data);
};

#endif
