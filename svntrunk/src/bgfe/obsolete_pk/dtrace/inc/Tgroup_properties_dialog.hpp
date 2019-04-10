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
 * Module:          Tgroup_properties_dialog.hpp
 *
 * Purpose:         Class to create and display the dialog box for displaying
 *                  and modifying the characteristics of a group of trace items
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/
#ifndef TPROPERTIES_DIALOG_CLASS_H
#define TPROPERTIES_DIALOG_CLASS_H

#include "Tinclude.hpp"
#include "Tgroup_class.hpp"
#include <Xm/SeparatoG.h>
class TraceGroup;

class GroupPropDialog
{
  public:
    //
    // Data
    //
    TraceGroup *ParentItem;  // Parent group to which this properties bax is
                             // associated

    char  ColourSelection[MAX_COLOUR_SIZE]; // The colour of the group

    Widget wTraceFileTextBox; // Text box to display the name of the file
                              // containing the collection of trace streams

    Widget wPrevFileTextBox;  // Text box to display the Prev regular expression
                              // used to create the group
    Widget wPostFileTextBox;  // Text box to display the Post regular expression
                              // used to create the group


    Widget wGroupPropDialog;   // Dialog box main widget
    Widget CancelButton;       // Button
    Widget OKButton;           // Button
    Widget mu_color;           // Colour pulldown menu
    Widget wInvertedToggle;    // Inverted trace toggle button
    Widget wContourToggle;     // Contoured trace toggle button

    //
    // Constructor
    //
    GroupPropDialog(TraceGroup *pParentGroup);

    //
    // Destructor
    //
    ~GroupPropDialog();

    // Function to update dialog box when it's displayed
    void UpdateDialog();

    //
    // Callback functions
    //
    static void cbCancelButton(Widget w,
                                    XtPointer client_data, XtPointer call_data);

    static void cbOKButton(Widget w,
                                    XtPointer client_data, XtPointer call_data);
 
    static void cbColourChoice( Widget w, XtPointer client_data,
                                                          XtPointer call_data );


};

#endif
