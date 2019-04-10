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
 * Module:          Tmessage_dialog.hpp
 *
 * Purpose:         Class to display error and warning messages.
 *                  Calling the constructor will automatically display this
 *                  dialog box.
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/
#ifndef TMESSAGE_DIALOG_H
#define TMESSAGE_DIALOG_H


#include "Tinclude.hpp"
#include <Xm/MessageB.h>

enum DialogType {ERROR, WARNING};

class MessageDialog
{
   public:
      // DisplayText parameter - Text to be displayed in the box.
      // BoxType     parameter - Selects the indicator to show either and
      //                         error or a warning.
      MessageDialog(Widget ParentWidget, char* DisplayText,
                    DialogType BoxType );

      ~MessageDialog();

      //
      virtual void cbOKButton(void);
      virtual void cbCancelButton(void);

   protected:
      static void OKButtonCB( Widget w, XtPointer client_data, 
                                 XtPointer call_data );

      static void CancelButtonCB( Widget w, XtPointer client_data,
                                 XtPointer call_data );

      //
      // Data
      //
      Widget Parent;
      Widget wMessageDialog;
      Widget wMessageLabel;

};

#endif
