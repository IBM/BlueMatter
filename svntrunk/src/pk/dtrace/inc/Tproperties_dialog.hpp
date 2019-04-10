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
 * Module:          Tproperties_dialog.hpp
 *
 * Purpose:         Class to display and modify the properties of a trace
 *                  item. 
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/

#ifndef TGROUP_PROPERTIES_DIALOG_CLASS_H
#define TGROUP_PROPERTIES_DIALOG_CLASS_H

#include <Tinclude.hpp>
#include <Titem_class.hpp>
#include <Xm/SeparatoG.h>
class TraceItem;

class PropDialog
{
   public:
      // The pParentItem parameter is the TraceItem whose data will be 
      // displayed and modified.
      PropDialog(TraceItem *pParentItem);

      ~PropDialog();

      // Updates the widgets with new data.
      void UpdateDialog();

      static PropDialog* GetPropDialog(TraceItem *pParentItem = NULL);

      static void cbCancelButton(Widget w,
                                   XtPointer client_data, XtPointer call_data);

      virtual void PropDialogOKButtonPressedCB(Widget w,XtPointer call_data);

      static void cbPropDialogOKButtonPressed(Widget w,
                                   XtPointer client_data, XtPointer call_data);

      static void cbColourChoice( Widget w, XtPointer client_data,
                                      XtPointer call_data );

      Widget CreateGCPullDownMenu( Widget w,
                                 XtCallbackProc call_data,
                                 XtPointer client_data,
                                 int      SelectedValue  );

      static void cbGCChoice( Widget w, XtPointer client_data,
                                      XtPointer call_data );


      static void cbStyleChoice( Widget w, XtPointer client_data,
                                      XtPointer call_data );

      //
      // Data
      //
      char  ColourSelection[32];
      char  GC_Function[32];
      char  SelectedStyle[32];

      TraceItem *ParentItem;
      Widget wPrevFileTextBox;
      Widget wPostFileTextBox;
      Widget wPropDialog;
      Widget ColourMenu;
      Widget wInvertedToggle;
      Widget CancelButton;
      Widget OKButton;
      Widget mu_color;
      Widget mu_style;
      Widget tbPrevNumHits;
      Widget tbPostNumHits;
      Widget mu_GC;
      Widget wVertLineButton;
};

#endif
