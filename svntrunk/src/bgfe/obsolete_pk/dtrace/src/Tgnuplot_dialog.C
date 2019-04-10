// template <class Element>
// class IACollection;
#include "Tgnuplot_dialog.hpp"

GNUPlotDialog::GNUPlotDialog(Widget        ParentWidget,
                             TracerWindow *ParentWindow,
                             char*         InitialCommand)
{
  Arg args[20];
  int n =0;
  Parent = ParentWindow;
  XmString xmString;

  XtSetArg( args[n],  XmNallowResize,    True );               n++;
  XtSetArg( args[n],  XmNwidth,          850 );                n++;
  XtSetArg( args[n],  XmNheight,         600 );                n++;
  XtSetArg( args[n],  XmNdialogStyle,    XmDIALOG_MODELESS );  n++;
  XtSetArg( args[n],  XmNtitle,          "GNU Plot Commands" );n++;
  XtSetArg( args[n],  XmNallowOverlap,   FALSE );              n++;
  XtSetArg( args[n],  XmNchildPlacement, XmPLACE_ABOVE_SELECTION );  n++;

  wGNUPlotMain = XmCreatePromptDialog( ParentWidget,
                                     "wGNUPlotMain", args, n );

  XtUnmanageChild(XmSelectionBoxGetChild(wGNUPlotMain,XmDIALOG_HELP_BUTTON));
  XtManageChild(XmSelectionBoxGetChild(wGNUPlotMain,XmDIALOG_APPLY_BUTTON));
  XtUnmanageChild(XmSelectionBoxGetChild(wGNUPlotMain,XmDIALOG_OK_BUTTON));

  xmString = XmStringCreateSimple("Apply");
  XtVaSetValues(XmSelectionBoxGetChild(wGNUPlotMain,XmDIALOG_OK_BUTTON),
                XmNlabelString,             xmString,
                NULL );

  XmStringFree(xmString);

  xmString = XmStringCreateSimple("Quit");
  XtVaSetValues(XmSelectionBoxGetChild(wGNUPlotMain,XmDIALOG_CANCEL_BUTTON),
                XmNlabelString,             xmString,
                NULL );

  XmStringFree(xmString);

  XtAddCallback(wGNUPlotMain, XmNcancelCallback,
                (XtCallbackProc)cbQuitButtonPressed,
                (XtPointer)     this);

  XtRemoveAllCallbacks(wGNUPlotMain,XmNokCallback);

  XtAddCallback(wGNUPlotMain, XmNapplyCallback,
                (XtCallbackProc)cbApplyButtonPressed,
                (XtPointer)     this);


  n = 0;
  XtSetArg( args[n],  XmNeditable, False );              n++;
  XtSetArg( args[n],  XmNeditMode, XmMULTI_LINE_EDIT );  n++;
  XtSetArg( args[n],  XmNrows,     20 );                 n++;

  wGNUPlotText = XmCreateScrolledText(wGNUPlotMain,
                               "wGNUPlotText",args,n++);

//  XmTextSetString(wGNUPlotText,InitialCommand);

  XmTextInsert(wGNUPlotText,
              XmTextGetInsertionPosition(wGNUPlotText),InitialCommand);

  command = popen("gnuplot","w");

//  command = popen("gnuplot > /tmp/gnuplot.out 2> /tmp/gnuplot.err","w");

  fprintf(command,InitialCommand);
  fflush(command);
  XtManageChild( wGNUPlotText );

  XtManageChild(ParentWidget);

  XtManageChild( wGNUPlotMain );
}

GNUPlotDialog::~GNUPlotDialog()
{
  pclose(command);
}

void GNUPlotDialog::cbQuitButtonPressed(Widget w,XtPointer client_data,
                                          XtPointer call_data)
{
   GNUPlotDialog *Dialog = (GNUPlotDialog*)client_data;

   printf("QuitButtonPressedCB\n");
//   delete Dialog;

}

void GNUPlotDialog::cbApplyButtonPressed(Widget w,XtPointer client_data,
                                          XtPointer call_data)
{
   char             Command[2048];
   GNUPlotDialog *Dialog = (GNUPlotDialog*)client_data;
   XmSelectionBoxCallbackStruct* ptr;
   char  *string;

   ptr = (XmSelectionBoxCallbackStruct*) call_data;

   XmStringGetLtoR(ptr->value,
                   XmSTRING_DEFAULT_CHARSET,
                   &string);

   printf("Selected string [%s]\n",string);


   sprintf(Command,"%s\n",string);

   XmTextInsert(Dialog->wGNUPlotText,
                XmTextGetInsertionPosition(Dialog->wGNUPlotText),Command);

   fprintf(Dialog->command,Command);
   fflush(Dialog->command);

   XmTextSetString(XmSelectionBoxGetChild(Dialog->wGNUPlotMain,XmDIALOG_TEXT),
                   "");

   XtFree(string);
}
