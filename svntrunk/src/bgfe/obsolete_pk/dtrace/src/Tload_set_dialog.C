// template <class Element>
// class IACollection;
#include "Tload_set_dialog.hpp"

LoadGroup::LoadGroup(TracerWindow *ParentWindow)
{
   Widget fr_TraceFile,fm_TraceFile,lb_TraceFiletbTitle, Sep1;
   Widget fr_RexpFile, fm_RexpFile, lb_RexpFiletbTitle;
   

   FileDialogType = -1;

   Arg args[20];
   int n =0;
   Parent = ParentWindow;
   XmString xmString;

   XtSetArg( args[n],  XmNallowResize, True );              n++;
   XtSetArg( args[n],  XmNwidth,       850 );               n++;
   XtSetArg( args[n],  XmNheight,      200 );               n++;
   XtSetArg( args[n],  XmNdialogStyle, XmDIALOG_MODELESS ); n++;
   XtSetArg( args[n],  XmNtitle,       "Load Group" );      n++;



   wLoadGroupMain = XmCreateFormDialog( Parent->form0,
                                     "wLoadGroupMain", args, n );
   /* 
    * Create the trace file data area
    */
   fr_TraceFile = XtVaCreateManagedWidget("fr_TraceFile",
                                          xmFrameWidgetClass,  wLoadGroupMain,
                                          XmNtopAttachment,    XmATTACH_FORM,
                                          XmNtopOffset,        3,
                                          XmNleftAttachment,   XmATTACH_FORM,
                                          XmNleftOffset,       5,
                                          XmNrightAttachment,  XmATTACH_FORM,
                                          XmNrightOffset,      5,
                                          NULL);

   xmString = XmStringCreateSimple("Trace Data File");
   (void)XtVaCreateManagedWidget("lb_TraceFile",          xmLabelWidgetClass,
                             fr_TraceFile,
                             XmNlabelString,              xmString,
                             XmNchildType,                XmFRAME_TITLE_CHILD,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment,   XmALIGNMENT_CENTER,
                             NULL);
   XmStringFree(xmString);

   fm_TraceFile = XtVaCreateManagedWidget("fm_TraceFile",
                                    xmFormWidgetClass,fr_TraceFile,
                                    XmNwidth,         250,
                                    XmNheight,        40,
                                    XmNresizePolicy,  XmRESIZE_NONE,
                                    NULL);

   xmString = XmStringCreateSimple("File");
   lb_TraceFiletbTitle = XtVaCreateManagedWidget("lb_TraceFiletbTitle",
                             xmLabelWidgetClass,
                             fm_TraceFile,
                             XmNlabelString, xmString,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
                             XmNtopAttachment,        XmATTACH_FORM,
                             XmNtopOffset,            7,
                             XmNleftAttachment,   XmATTACH_FORM,
                             XmNleftOffset,       5,
                             NULL);

   XmStringFree(xmString);
   xmString = XmStringCreateSimple("Select File");

   wTraceFileButton = CreatePushButton(fm_TraceFile,"wTraceFileButton",
                 cbFileSelectButtonPressed,
                 (XtPointer) this);


   XtVaSetValues(wTraceFileButton,
                 XmNtopAttachment,        XmATTACH_FORM,
                 XmNtopOffset,            2,
                 XmNrightAttachment,      XmATTACH_FORM,
                 XmNrightOffset,          5,
                 XmNlabelString,          xmString,
                 NULL );

   XmStringFree(xmString);

   wtbLoadGroupTraceFile = XtVaCreateManagedWidget("wtbLoadGroupTraceFile",
                              xmTextFieldWidgetClass,  fm_TraceFile,
                              XmNmarginHeight,         3,
                              XmNmaxLength,            64,
                              XmNcolumns,              40,
                              XmNcursorPosition,       1,
                              XmNtopAttachment,        XmATTACH_FORM,
                              XmNtopOffset,            2,
                              XmNleftAttachment,       XmATTACH_WIDGET,
                              XmNleftWidget,           lb_TraceFiletbTitle,
                              XmNleftOffset,           5,
                              XmNrightAttachment,      XmATTACH_WIDGET,
                              XmNrightWidget,          wTraceFileButton,
                              XmNrightOffset,          5,
                              NULL);

   if (strcmp(Parent->StartDirectory,"") == 0)
   {
      XmTextSetString(wtbLoadGroupTraceFile,"");
   }
   else
   {
      XmTextSetString(wtbLoadGroupTraceFile,Parent->StartDirectory);
   }

   /*
    * Create the trace file data area
    */
   fr_RexpFile = XtVaCreateManagedWidget("fr_RexpFile",
                                          xmFrameWidgetClass,  wLoadGroupMain,
                                          XmNtopAttachment,    XmATTACH_WIDGET,
                                          XmNtopWidget  ,      fr_TraceFile,
                                          XmNtopOffset,        3,
                                          XmNleftAttachment,   XmATTACH_FORM,
                                          XmNleftOffset,       5,
                                          XmNrightAttachment,  XmATTACH_FORM,
                                          XmNrightOffset,      5,
                                          NULL);

   xmString = XmStringCreateSimple("Regular Expression File");

   (void)XtVaCreateManagedWidget("lb_RexpFile", xmLabelWidgetClass,
                             fr_RexpFile,
                             XmNlabelString, xmString,
                             XmNchildType, XmFRAME_TITLE_CHILD,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
                             NULL);
   XmStringFree(xmString);

   fm_RexpFile = XtVaCreateManagedWidget("fm_RexpFile",
                                    xmFormWidgetClass,fr_RexpFile,
                                    XmNwidth,         250,
                                    XmNheight,        40,
                                    XmNresizePolicy,  XmRESIZE_NONE,
                                    NULL);

   xmString = XmStringCreateSimple("File");
   lb_RexpFiletbTitle = XtVaCreateManagedWidget("lb_RexpFiletbTitle",
                             xmLabelWidgetClass,
                             fm_RexpFile,
                             XmNlabelString, xmString,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
                             XmNtopAttachment,        XmATTACH_FORM,
                             XmNtopOffset,            7,
                             XmNleftAttachment,   XmATTACH_FORM,
                             XmNleftOffset,       5,
                             NULL);

   XmStringFree(xmString);
   xmString = XmStringCreateSimple("Select File");

   wRexpFileButton = CreatePushButton(fm_RexpFile,"wRexpFileButton",
                 cbFileSelectButtonPressed,
                 (XtPointer) this);


   XtVaSetValues(wRexpFileButton,
                 XmNtopAttachment,        XmATTACH_FORM,
                 XmNtopOffset,            2,
                 XmNrightAttachment,      XmATTACH_WIDGET,
                 XmNrightOffset,          5,
                 XmNlabelString,          xmString,
                 NULL );

   XmStringFree(xmString);

   wtbLoadGroupRexpFile = XtVaCreateManagedWidget("wtbLoadGroupRexpFile",
                              xmTextFieldWidgetClass, fm_RexpFile,
                              XmNmarginHeight,         3,
                              XmNmaxLength,            64,
                              XmNcolumns,              40,
                              XmNcursorPosition,       1,
                              XmNtopAttachment,        XmATTACH_FORM,
                              XmNtopOffset,            2,
                              XmNleftAttachment,       XmATTACH_WIDGET,
                              XmNleftWidget,           lb_RexpFiletbTitle,
                              XmNleftOffset,           5,
                              XmNrightAttachment,      XmATTACH_WIDGET,
                              XmNrightWidget,          wRexpFileButton,
                              XmNrightOffset,          5,
                              NULL);

   if (strcmp(TracerApplication::GetApp()->RegExpPath,"") == 0)
   {
      XmTextSetString(wtbLoadGroupRexpFile,"");
   }
   else
   {
      XmTextSetString(wtbLoadGroupRexpFile,
                      TracerApplication::GetApp()->RegExpPath);
   }

   // Create buttons
   xmString = XmStringCreateSimple("OK");

   wOKButton = CreatePushButton(wLoadGroupMain,"OKbutton",
                 cbOKButtonPressed,
                 (XtPointer) this);

   XtVaSetValues(wOKButton,
                 XmNbottomAttachment,        XmATTACH_FORM,
                 XmNbottomOffset,            10,
                 XmNleftAttachment,          XmATTACH_FORM,
                 XmNleftOffset,              10,
                 XmNlabelString,             xmString,
                 NULL );

   XmStringFree(xmString);

   xmString = XmStringCreateSimple("Cancel");

   wCancelButton = CreatePushButton(wLoadGroupMain,"Cancelbutton",
                 cbCancelButtonPressed,
                 (XtPointer) this);

   XtVaSetValues(wCancelButton,
                 XmNbottomAttachment,        XmATTACH_FORM,
                 XmNbottomOffset,            10,
                 XmNrightAttachment,         XmATTACH_FORM,
                 XmNrightOffset,             10,
                 XmNlabelString,             xmString,
                 NULL );

   XmStringFree(xmString);

   n = 0;
   XtSetArg( args[n],  XmNleftAttachment,   XmATTACH_FORM ); n++;
   XtSetArg( args[n],  XmNleftOffset,       0 );             n++;
   XtSetArg( args[n],  XmNrightAttachment,  XmATTACH_FORM ); n++;
   XtSetArg( args[n],  XmNrightOffset,      0 );             n++;
   XtSetArg( args[n],  XmNbottomAttachment, XmATTACH_FORM ); n++;
   XtSetArg( args[n],  XmNbottomOffset,     45 );            n++;

   Sep1 =  XmCreateSeparatorGadget(wLoadGroupMain,"Sep1",args,n);

   XtManageChild(Sep1);
   XtManageChild( wLoadGroupMain );
}

LoadGroup::~LoadGroup()
{
   printf("LoadGroup::~LoadGroup called\n");
   XtDestroyWidget(wLoadGroupMain);
}

void LoadGroup::cbOKButtonPressed(Widget w,XtPointer client_data,
                                          XtPointer call_data)
{
  LoadGroup *Dialog = (LoadGroup*)client_data;

  strcpy (Dialog->Parent->StartDirectory,
          XmTextGetString(Dialog->wtbLoadGroupTraceFile));
  strcpy (TracerApplication::GetApp()->RegExpPath,
          XmTextGetString(Dialog->wtbLoadGroupRexpFile));

  Dialog->Parent->AutoLoadGroups(
                               XmTextGetString(Dialog->wtbLoadGroupTraceFile),
                               XmTextGetString(Dialog->wtbLoadGroupRexpFile));

  return;
}

void LoadGroup::cbCancelButtonPressed(Widget w,XtPointer client_data,
                                          XtPointer call_data)
{
   LoadGroup *Dialog = (LoadGroup*)client_data;

   printf("CancelButtonPressedCB\n");
//   delete Dialog;

}


void LoadGroup::cbFileSelectButtonPressed(Widget w,XtPointer client_data,
                                                   XtPointer call_data)
{
  Boolean StartFileGiven = FALSE;
  Arg args[20];
  int n =0;
  XmString s, sFilter, str,sDir;
  char *CurrentDirectory;
  char   *Initial_Filter = "*";
  char FullSelectedPath[2048];
  char DirName[1024];
  char FileName[1024];

  Widget FileSelectionList;

  LoadGroup *Dialog = (LoadGroup*)client_data;

  sFilter = XmStringCreateLocalized( Initial_Filter );

  // set up the box depending on the button pressed
  if (w == Dialog->wTraceFileButton)
  {
    s = XmStringCreateLocalized("Trace File Selection");
    Dialog->FileDialogType = TRACE_FILE_DIALOG;

    // Check whether value is set
    if (strcmp(Dialog->Parent->StartDirectory,"") == 0)
    {
      CurrentDirectory = (char*)malloc(2048*(sizeof(char)));
      if (getcwd(CurrentDirectory,2048) == NULL)
      {
          printf("getcwd failed");
          sDir = XmStringCreateLocalized("~");
      }
      sDir = XmStringCreateLocalized(CurrentDirectory);

      free(CurrentDirectory);

    }
    else
    {
      GetPathFromFile(Dialog->Parent->StartDirectory,
                      &(FileName[0]),&(DirName[0]));

      sDir = XmStringCreateLocalized(DirName);
      strcpy(FullSelectedPath,Dialog->Parent->StartDirectory);
      StartFileGiven = TRUE;
    }
  }
  else if (w == Dialog->wRexpFileButton)
  {
    s = XmStringCreateLocalized("Regular Expressoion File Selection");
    Dialog->FileDialogType = REGEXP_FILE_DIALOG;

    // Check whether value is set
    if (strcmp(TracerApplication::GetApp()->RegExpPath,"") == 0)
    {
      CurrentDirectory = (char*)malloc(2048*(sizeof(char)));
      if (getcwd(CurrentDirectory,2048) == NULL)
      {
          printf("getcwd failed");
          sDir = XmStringCreateLocalized("~");
      }
      sDir = XmStringCreateLocalized(CurrentDirectory);

      free(CurrentDirectory);

    }
    else
    {
      GetPathFromFile(TracerApplication::GetApp()->RegExpPath,
                      &(FileName[0]),&(DirName[0]));
      strcpy(FullSelectedPath,
             TracerApplication::GetApp()->RegExpPath);
      sDir = XmStringCreateLocalized(DirName);
      StartFileGiven = TRUE;
    }

  }
  else
  {
    printf("Error, unknown parent\n");
  }

  // Set the initial values
  
  XtSetArg(args[n],XmNallowResize, False ); n++;
  XtSetArg(args[n],XmNresizePolicy, XmRESIZE_NONE ); n++;
  XtSetArg(args[n],XmNwidth,   1000 ); n++;
  XtSetArg(args[n],XmNheight,  500 ); n++;
  XtSetArg(args[n],XmNdialogTitle, s  ); n++;
  XtSetArg(args[n],XmNfileTypeMask,XmFILE_REGULAR); n++;
  XtSetArg(args[n],XmNdirMask,  sFilter); n++;
  XtSetArg(args[n],XmNdirectory,  sDir); n++;

  Dialog->wFileSelDialog = XmCreateFileSelectionDialog(Dialog->wLoadGroupMain,
                                                        "wCreSetDialog",args,n);


   XtUnmanageChild( XmFileSelectionBoxGetChild(Dialog->wFileSelDialog,
                                XmDIALOG_HELP_BUTTON));

   XtAddCallback(Dialog->wFileSelDialog,XmNokCallback,
                 cbFDOKButtonPressed,(XtPointer)client_data );
   XtAddCallback(Dialog->wFileSelDialog,XmNcancelCallback,
                 cbFDCancelButtonPressed,(XtPointer)client_data);


  FileSelectionList = XmFileSelectionBoxGetChild(Dialog->wFileSelDialog,
                                                 XmDIALOG_LIST);

  if (StartFileGiven == TRUE)
  {
    str = XmStringCreateSimple(FullSelectedPath);
    XmListSelectItem(FileSelectionList, str, TRUE);
    XmStringFree(str);
  }

  XtManageChild(Dialog->wFileSelDialog);
}

void LoadGroup::cbFDOKButtonPressed(Widget w,XtPointer client_data,
                                          XtPointer call_data)
{
  char* Filename;

  XmFileSelectionBoxCallbackStruct* cbs =
                                (XmFileSelectionBoxCallbackStruct*)(call_data);

  XmStringGetLtoR(cbs->value,
                  XmSTRING_DEFAULT_CHARSET,
                  &Filename);

  LoadGroup *Dialog = (LoadGroup*)client_data;

  if (Dialog->FileDialogType == TRACE_FILE_DIALOG)
  {
    XmTextSetString(Dialog->wtbLoadGroupTraceFile,Filename);
  }
  else if (Dialog->FileDialogType == REGEXP_FILE_DIALOG)
  {
    XmTextSetString(Dialog->wtbLoadGroupRexpFile,Filename);

  }

  else
  {
    printf("Shouldn't happen\n");
  }
  XtFree(Filename);

  Dialog->FileDialogType = -1;
  XtDestroyWidget(Dialog->wFileSelDialog);

}

void LoadGroup::cbFDCancelButtonPressed(Widget w,XtPointer client_data,
                                          XtPointer call_data)
{
  LoadGroup *Dialog = (LoadGroup*)client_data;
  Dialog->FileDialogType = -1;
  XtDestroyWidget(Dialog->wFileSelDialog);
}

