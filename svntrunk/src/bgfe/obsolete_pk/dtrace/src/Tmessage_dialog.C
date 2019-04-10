
// template <class Element>
// class IACollection;
#include "Tmessage_dialog.hpp"

/*******************************************************************************
*
* Function: ScaleDialog
*
*
* Purpose:
*
*******************************************************************************/
MessageDialog::MessageDialog(Widget ParentWidget, char* DisplayText,
                             DialogType BoxType )
{

   Arg       args[20];
   int       n        =0;
   XmString  xmstring;
   XmString  Titlestring;

   Parent = ParentWidget;

   xmstring = XmStringCreateSimple(DisplayText);

   // Setup dialog depending on type required
   if (BoxType == ERROR)
   {
     Titlestring = XmStringCreateSimple("Error");
     XtSetArg( args[n],  XmNdialogTitle, Titlestring); n++;
     XtSetArg( args[n],  XmNallowResize, False ); n++;
     XtSetArg( args[n],  XmNdialogStyle,   XmDIALOG_APPLICATION_MODAL ); n++;
     XtSetArg( args[n],  XmNmessageString, xmstring ); n++;

     wMessageDialog = XmCreateErrorDialog(ParentWidget,"ErrorDialog",args,n);

     XtUnmanageChild(XmMessageBoxGetChild(wMessageDialog,XmDIALOG_HELP_BUTTON));
     XtUnmanageChild(
                   XmMessageBoxGetChild(wMessageDialog,XmDIALOG_CANCEL_BUTTON));
   }

   else if (BoxType == WARNING)
   {
     Titlestring = XmStringCreateSimple("Error");

     XtSetArg( args[n],  XmNdialogTitle, Titlestring ); n++;
     XtSetArg( args[n],  XmNallowResize, False ); n++;
     XtSetArg( args[n],  XmNdialogStyle,   XmDIALOG_MODELESS ); n++;
     XtSetArg( args[n],  XmNmessageString, xmstring ); n++;

     wMessageDialog = XmCreateWarningDialog(ParentWidget,"ErrorDialog",args,n);

     XtUnmanageChild(XmMessageBoxGetChild(wMessageDialog,XmDIALOG_HELP_BUTTON));
     XtUnmanageChild(
                   XmMessageBoxGetChild(wMessageDialog,XmDIALOG_CANCEL_BUTTON));

   }

   else
   {
      printf("Error creating dialog\n");
   }

   XmStringFree(xmstring);
   XmStringFree(Titlestring);

   XtManageChild( wMessageDialog );


}

MessageDialog::~MessageDialog()
{

}

void MessageDialog::OKButtonCB( Widget w, XtPointer client_data,
                                 XtPointer call_data )
{
  MessageDialog *obj;
  obj = (MessageDialog*)client_data;

  if (obj != NULL)
  {
    obj->cbOKButton();
  }
}

void MessageDialog::CancelButtonCB( Widget w, XtPointer client_data,
                                 XtPointer call_data )
{

  MessageDialog *obj;
  obj = (MessageDialog*)client_data;

  if (obj != NULL)
  {
    obj->cbCancelButton();
  }

}

void MessageDialog::cbOKButton(void)
{

}

void MessageDialog::cbCancelButton(void)
{

}

