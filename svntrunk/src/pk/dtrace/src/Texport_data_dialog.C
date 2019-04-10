/***************************************************************************
 * Project:         dtrace
 *
 * Module:          Texport_data_dialog.C
 *
 * Purpose:         Class for the Export data dialog box
 *
 * Classification:  IBM Internal Use Only
 *
 ***************************************************************************/

// template <class Element>
// class IACollection;
#include <Texport_data_dialog.hpp>

ExportDialog::ExportDialog(Widget parentWidget,
                           TracerWindow *ParentWindow,
                           LTime         StartTime,
                           LTime         EndTime)
{
  Widget fr_TraceFile, fr_FileOptToggleButt;
  Widget fr_ToggleButt,fm_ExpDialogExtraForm;
  Widget fr_FromTime;
  Widget fr_ToTime,   fm_ToTime;
  Widget ToggleButtonBox;
  Widget fm_TraceFileList, fr_FileFormatToggleButt;
  char Fromstr[256];
  char Tostr[256];

  Arg args[20];
  int n =0;
  Parent = ParentWindow;
  XmString xmString, OKxmString, CancelxmString;

  wDialogShell = XtVaAppCreateShell(NULL,"ExportDialog",
                                    topLevelShellWidgetClass,
                                    XtDisplay(TracerApplication::GetApp()->top),
                                    XmNtitle,       "Export Data",
                                    NULL);

  NumTracesInList = 0;

  ExportStartTime = StartTime;
  ExportEndTime   = EndTime;

  OKxmString = XmStringCreateSimple((char *) "OK");
  CancelxmString = XmStringCreateSimple((char *) "Cancel");

  XtSetArg( args[n],  XmNallowResize,       True );           n++;
  XtSetArg( args[n],  XmNwidth,             1250 );           n++;
  XtSetArg( args[n],  XmNheight,            800 );            n++;
  XtSetArg( args[n],  XmNokLabelString,     OKxmString );     n++;
  XtSetArg( args[n],  XmNcancelLabelString, CancelxmString ); n++;
  XtSetArg( args[n],  XmNtitle,             "Export Data");   n++;

  wExportDataMain = XmCreateTemplateDialog( wDialogShell,
      (char *) "wExportDataMain", args, n );

   XmStringFree(OKxmString);
   XmStringFree(CancelxmString);

   fm_ExpDialogExtraForm = XtVaCreateManagedWidget("fm_ExpDialogExtraForm",
                                    xmFormWidgetClass,wExportDataMain,
                                    NULL);

   /* Create the From time label and text box */
   fr_FromTime = XtVaCreateManagedWidget("fr_ExpDialogFromTime",
                                    xmFrameWidgetClass,  fm_ExpDialogExtraForm,
                                    XmNleftAttachment,   XmATTACH_FORM,
                                    XmNleftOffset,       2,
                                    XmNbottomAttachment, XmATTACH_WIDGET,
                                    XmNbottomWidget  ,  fm_ExpDialogExtraForm,
                                    XmNbottomOffset,        5,
                                    NULL);

   xmString = XmStringCreateSimple((char *) "From Time");

   (void)XtVaCreateManagedWidget("lb_ExpDialogFromTime", xmLabelWidgetClass,
                             fr_FromTime,
                             XmNlabelString, xmString,
                             XmNchildType, XmFRAME_TITLE_CHILD,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
                             NULL);
   XmStringFree(xmString);

   fm_FromTime = XtVaCreateManagedWidget("fm_ExpDialogFromTime",
                                    xmFormWidgetClass,fr_FromTime,
                                    XmNresizePolicy,  XmRESIZE_NONE,
                                    NULL);

   wtbFromTime = XtVaCreateManagedWidget("wtbExpDialogLoadGroupRexpFile",
                              xmTextFieldWidgetClass, fm_FromTime,
                              XmNmarginHeight,         4,
                              XmNmaxLength,            30,
                              XmNcolumns,              30,
                              XmNcursorPosition,       1,
                              XmNtopAttachment,        XmATTACH_FORM,
                              XmNtopOffset,            2,
                              XmNleftAttachment,       XmATTACH_WIDGET,
                              XmNleftWidget,           fm_FromTime,
                              XmNleftOffset,           5,
                              XmNrightAttachment,      XmATTACH_WIDGET,
                              XmNrightWidget,          fm_FromTime,
                              XmNrightOffset,          5,
                              NULL);

   /* Create the From time label and text box */
   fr_ToTime = XtVaCreateManagedWidget("fr_ExpDialogToTime",
                                      xmFrameWidgetClass, fm_ExpDialogExtraForm,
                                      XmNrightAttachment,  XmATTACH_FORM,
                                      XmNrightOffset,      2,
                                      XmNbottomAttachment, XmATTACH_WIDGET,
                                      XmNbottomWidget  ,  fm_ExpDialogExtraForm,
                                      XmNbottomOffset,        5,
                                      NULL);

   xmString = XmStringCreateSimple((char *) "To Time");

   (void)XtVaCreateManagedWidget("lb_ExpDialogToTime", xmLabelWidgetClass,
                             fr_ToTime,
                             XmNlabelString, xmString,
                             XmNchildType, XmFRAME_TITLE_CHILD,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
                             NULL);
   XmStringFree(xmString);

   fm_ToTime = XtVaCreateManagedWidget("fm_ExpDialogToTime",
                                    xmFormWidgetClass,fr_ToTime,
                                    XmNresizePolicy,  XmRESIZE_NONE,
                                    NULL);

   wtbToTime = XtVaCreateManagedWidget("wtbExpDialogToTime",
                              xmTextFieldWidgetClass, fm_ToTime,
                              XmNmarginHeight,         4,
                              XmNmaxLength,            30,
                              XmNcolumns,              30,
                              XmNcursorPosition,       1,
                              XmNtopAttachment,        XmATTACH_FORM,
                              XmNtopOffset,            2,
                              XmNleftAttachment,       XmATTACH_WIDGET,
                              XmNleftWidget,           fm_ToTime,
                              XmNleftOffset,           5,
                              XmNrightAttachment,      XmATTACH_WIDGET,
                              XmNrightWidget,          fm_ToTime,
                              XmNrightOffset,          5,
                              NULL);

  strcpy(Fromstr, CreateSecondsFromLTime(StartTime));
  strcpy(Tostr, CreateSecondsFromLTime(EndTime));

  XmTextSetString(wtbFromTime,Fromstr);
  XmTextSetString(wtbToTime,Tostr);

  /* Create the export toggle box */
  fr_ToggleButt = XtVaCreateManagedWidget("fr_ExpDialogToggleButt",
                                     xmFrameWidgetClass,  fm_ExpDialogExtraForm,
                                     XmNtopAttachment,        XmATTACH_FORM,
                                     XmNtopOffset,            2,
                                     XmNleftAttachment,       XmATTACH_FORM,
                                     XmNleftOffset,           2,
                                     NULL);

  xmString = XmStringCreateSimple((char *) "Export Method");

  (void)XtVaCreateManagedWidget("lb_ExpDialogExportMethod", xmLabelWidgetClass,
                             fr_ToggleButt,
                             XmNlabelString, xmString,
                             XmNchildType, XmFRAME_TITLE_CHILD,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
                             NULL);
  XmStringFree(xmString);

  ToggleButtonBox = XtVaCreateManagedWidget("ExpDialogToggleButtonBox",
                          xmRowColumnWidgetClass, fr_ToggleButt,
                          XmNradioBehavior,        TRUE,
                          XmNradioAlwaysOne,       TRUE,
                          XmNorientation,          XmHORIZONTAL,
                          NULL);
 
  // Create the Export to file toggle
  xmString = XmStringCreateSimple((char *) "Export to file");

  n= 0;
  XtSetArg(args[n],XmNindicatorType, XmONE_OF_MANY); n++;
  XtSetArg(args[n],XmNlabelString,   xmString );     n++;
  XtSetArg(args[n],XmNlabelType,     XmSTRING );     n++;
  XtSetArg(args[n],XmNset,           FALSE );        n++;


  wToggleExpFile = XmCreateToggleButton(ToggleButtonBox,
      (char *) "wExpDialogToggleExpFile",
                                          args, n);

  XmStringFree(xmString);
 
  XtManageChild(wToggleExpFile);


  // Create the Export to plot toggle
  xmString = XmStringCreateSimple((char *) "Export to plot");

  n= 0;
  XtSetArg(args[n],XmNindicatorType, XmONE_OF_MANY); n++;
  XtSetArg(args[n],XmNlabelString,   xmString );     n++;
  XtSetArg(args[n],XmNlabelType,     XmSTRING );     n++;
  XtSetArg(args[n],XmNset,           TRUE );        n++;


  wToggleExpPlot = XmCreateToggleButton(ToggleButtonBox,
      (char *)  "wExpDialogToggleExpPlot",
                                          args, n);

  XmStringFree(xmString);


  XtManageChild(wToggleExpPlot);


  XtAddCallback(wToggleExpPlot,
                 XmNvalueChangedCallback,ExportDialog::cbTogglePressed,
                 (XtPointer)this);

  XtAddCallback(wToggleExpFile,
                 XmNvalueChangedCallback,ExportDialog::cbTogglePressed,
                 (XtPointer)this);



  /* Create the export toggle box */
  fr_FileOptToggleButt = XtVaCreateManagedWidget("fr_FileOptToggleButt",
                                     xmFrameWidgetClass,  fm_ExpDialogExtraForm,
                                     XmNtopAttachment,    XmATTACH_WIDGET,
                                     XmNtopWidget,        fr_ToggleButt,
                                     XmNtopOffset,        2,
                                     XmNleftAttachment,   XmATTACH_FORM,
                                     XmNleftOffset,       2,
                                     NULL);

  xmString = XmStringCreateSimple((char *) "File Options");

  (void)XtVaCreateManagedWidget("lb_ExpDialogFileOptExportMethod", 
                             xmLabelWidgetClass,
                             fr_FileOptToggleButt,
                             XmNlabelString, xmString,
                             XmNchildType, XmFRAME_TITLE_CHILD,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
                             NULL);
  XmStringFree(xmString);

  FileOptToggleButtonBox = XtVaCreateManagedWidget("FileOptToggleButtonBox",
                          xmRowColumnWidgetClass, fr_FileOptToggleButt,
                          XmNradioBehavior,        TRUE,
                          XmNradioAlwaysOne,       TRUE,
                          XmNorientation,          XmHORIZONTAL,
                          NULL);

  // Create the Export to file toggle
  xmString = XmStringCreateSimple((char *) "Export all trace points");

  n= 0;
  XtSetArg(args[n],XmNindicatorType, XmONE_OF_MANY); n++;
  XtSetArg(args[n],XmNlabelString,   xmString );     n++;
  XtSetArg(args[n],XmNlabelType,     XmSTRING );     n++;
  XtSetArg(args[n],XmNset,           FALSE );        n++;


  wToggleExpFileAll = XmCreateToggleButton(FileOptToggleButtonBox,
      (char *) "wToggleExpFileAll",
                                          args, n);

  XmStringFree(xmString);

  XtManageChild(wToggleExpFileAll);


  // Create the Export to plot toggle
  xmString = XmStringCreateSimple((char *) "Export selected trace points");

  n= 0;
  XtSetArg(args[n],XmNindicatorType, XmONE_OF_MANY); n++;
  XtSetArg(args[n],XmNlabelString,   xmString );     n++;
  XtSetArg(args[n],XmNlabelType,     XmSTRING );     n++;
  XtSetArg(args[n],XmNset,           TRUE );        n++;


  wToggleExpFileSelected = XmCreateToggleButton(FileOptToggleButtonBox,
      (char *) "wToggleExpFileSelected",
                                          args, n);
  XmStringFree(xmString);

  XtManageChild(wToggleExpFileSelected);


  XtAddCallback(wToggleExpFileAll,
                 XmNvalueChangedCallback,ExportDialog::cbFileOptTogglePressed,
                 (XtPointer)this);

  XtAddCallback(wToggleExpFileSelected,
                 XmNvalueChangedCallback,ExportDialog::cbFileOptTogglePressed,
                 (XtPointer)this);

  /* Create the toggle box to select export file format */
  fr_FileFormatToggleButt = XtVaCreateManagedWidget("fr_FileFormatToggleButt",
                                     xmFrameWidgetClass,  fm_ExpDialogExtraForm,
                                     XmNtopAttachment,    XmATTACH_WIDGET,
                                     XmNtopWidget,        fr_ToggleButt,
                                     XmNtopOffset,        2,
                                     XmNleftAttachment,   XmATTACH_WIDGET,
                                     XmNleftWidget,      FileOptToggleButtonBox,
                                     XmNleftOffset,       2,
                                     NULL);

  xmString = XmStringCreateSimple((char *) "File Format Options");

  (void)XtVaCreateManagedWidget("lb_ExpDialogFileFormatExport",
                             xmLabelWidgetClass,
                             fr_FileFormatToggleButt,
                             XmNlabelString, xmString,
                             XmNchildType, XmFRAME_TITLE_CHILD,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
                             NULL);
  XmStringFree(xmString);

  FileFormatToggleButtonBox=XtVaCreateManagedWidget("FileFormatToggleButtonBox",
                          xmRowColumnWidgetClass, fr_FileFormatToggleButt,
                          XmNradioBehavior,        TRUE,
                          XmNradioAlwaysOne,       TRUE,
                          XmNorientation,          XmHORIZONTAL,
                          NULL);

  // Create the Export to file toggle
  xmString = XmStringCreateSimple((char *) "Seconds");

  n= 0;
  XtSetArg(args[n],XmNindicatorType, XmONE_OF_MANY); n++;
  XtSetArg(args[n],XmNlabelString,   xmString );     n++;
  XtSetArg(args[n],XmNlabelType,     XmSTRING );     n++;
  XtSetArg(args[n],XmNset,           TRUE );        n++;


  wToggleExpFileSecs = XmCreateToggleButton(FileFormatToggleButtonBox,
      (char *) "wToggleExpFileSecs",
                                          args, n);

  XtManageChild(wToggleExpFileSecs);

  XmStringFree(xmString);


  // Create the Export to file toggle
  xmString = XmStringCreateSimple((char *) "LTime");

  n= 0;
  XtSetArg(args[n],XmNindicatorType, XmONE_OF_MANY); n++;
  XtSetArg(args[n],XmNlabelString,   xmString );     n++;
  XtSetArg(args[n],XmNlabelType,     XmSTRING );     n++;
  XtSetArg(args[n],XmNset,           FALSE );        n++;


  wToggleExpFileLTime = XmCreateToggleButton(FileFormatToggleButtonBox,
      (char *) "wToggleExpFileLTime",
                                          args, n);

  XtManageChild(wToggleExpFileLTime);

  XmStringFree(xmString);


  xmString = XmStringCreateSimple((char *) "Raw Data");

  n= 0;
  XtSetArg(args[n],XmNindicatorType, XmONE_OF_MANY); n++;
  XtSetArg(args[n],XmNlabelString,   xmString );     n++;
  XtSetArg(args[n],XmNlabelType,     XmSTRING );     n++;
  XtSetArg(args[n],XmNset,           FALSE );        n++;


  wToggleExpFileRawData = XmCreateToggleButton(FileFormatToggleButtonBox,
      (char *) "wToggleExpFileRawData",
                                          args, n);

  XtManageChild(wToggleExpFileRawData);

  XmStringFree(xmString);

  XtManageChild(FileFormatToggleButtonBox);


  /* Create the trace file list data area */
  fr_TraceFile = XtVaCreateManagedWidget("fr_ExpDialogTraceFile",
                                     xmFrameWidgetClass,  fm_ExpDialogExtraForm,
                                         XmNtopAttachment,    XmATTACH_WIDGET,
                                         XmNtopOffset,        3,
                                         XmNtopWidget,     fr_FileOptToggleButt,
                                         XmNleftAttachment,   XmATTACH_FORM,
                                         XmNleftOffset,       5,
                                         XmNrightAttachment,  XmATTACH_FORM,
                                         XmNrightOffset,      5,
                                         XmNbottomAttachment, XmATTACH_WIDGET,
                                         XmNbottomWidget,     fr_ToTime,
                                         XmNbottomOffset,     10,
                                         NULL);

  xmString = XmStringCreateSimple((char *) "List Of Traces");

  (void)XtVaCreateManagedWidget("lb_ExpDialogTraceFile",   xmLabelWidgetClass,
                             fr_TraceFile,
                             XmNlabelString,              xmString,
                             XmNchildType,                XmFRAME_TITLE_CHILD,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment,   XmALIGNMENT_CENTER,
                             NULL);
  XmStringFree(xmString);

  fm_TraceFileList = XtVaCreateManagedWidget("fm_ExpDialogTraceFileList",
                                    xmFormWidgetClass,fr_TraceFile,
                                    XmNresizePolicy,  XmRESIZE_NONE,
                                    NULL);

   n = 0;
   XtSetArg(args[n],XmNtopAttachment,XmATTACH_FORM);n++;
   XtSetArg(args[n],XmNtopOffset,5);n++;
   XtSetArg(args[n],XmNleftAttachment,XmATTACH_FORM);n++;
   XtSetArg(args[n],XmNleftOffset,5);n++;
   XtSetArg(args[n],XmNbottomAttachment,XmATTACH_FORM);n++;
   XtSetArg(args[n],XmNbottomOffset,5);n++;
   XtSetArg(args[n],XmNlistSizePolicy,XmCONSTANT);n++;
   XtSetArg(args[n],XmNwidth,500);n++;

   wPrevTraceList = XmCreateScrolledList(fm_TraceFileList,
       (char *) "wExpDialogPrevTraceList",args,n);

   XtVaSetValues(wPrevTraceList,
                 XmNselectionPolicy,  XmMULTIPLE_SELECT,
                 NULL);

   XtAddCallback(wPrevTraceList,
                 XmNmultipleSelectionCallback,ExportDialog::cbPrevTraceSelected,
                 (XtPointer)this);

   XtManageChild(wPrevTraceList);

   n = 0;
   XtSetArg(args[n],XmNtopAttachment,XmATTACH_FORM);n++;
   XtSetArg(args[n],XmNtopOffset,5);n++;
   XtSetArg(args[n],XmNrightAttachment,XmATTACH_FORM);n++;
   XtSetArg(args[n],XmNrightOffset,5);n++;
   XtSetArg(args[n],XmNbottomAttachment,XmATTACH_FORM);n++;
   XtSetArg(args[n],XmNbottomOffset,5);n++;
   XtSetArg(args[n],XmNwidth,500);n++;
   XtSetArg(args[n],XmNlistSizePolicy,XmCONSTANT);n++;

   wPostTraceList = XmCreateScrolledList(fm_TraceFileList,
       (char *)  "wExpDialogPostTraceList",args,n);

   XtVaSetValues(wPostTraceList,
                 XmNselectionPolicy,  XmMULTIPLE_SELECT,
                 NULL);

   XtAddCallback(wPostTraceList,
                 XmNmultipleSelectionCallback,ExportDialog::cbPostTraceSelected,
                 (XtPointer)this);

   XtManageChild(wPostTraceList);

   LoadListData();


   // Set toggle sensitivity
   XtSetSensitive(FileOptToggleButtonBox, FALSE);
   XtSetSensitive(FileFormatToggleButtonBox,FALSE);

   // Add buttons callbacks
   XtAddCallback(wExportDataMain,
                 XmNokCallback,ExportDialog::cbOKButtonPressed,
                 (XtPointer)this);

   XtAddCallback(wExportDataMain,
                 XmNcancelCallback,ExportDialog::cbCancelButtonPressed,
                 (XtPointer)this);

   XtManageChild( wExportDataMain );
}

void ExportDialog::LoadListData()
{

   int         GroupId = 1;
   int         ItemId  = 1;

   // TraceGroupListType::Cursor cursor(Parent->TraceListSeq);

   // Loop through all groups
   for ( int i=0; i < Parent->TraceListSeq.size(); i++)
   {
      // Create cursor on all traces within the group
       // TraceListType::Cursor cursoritem( Parent->TraceListSeq.elementAt(cursor)->TraceList );

      ItemId  = 1;
      // Loop through all traces in the group
//       for (cursoritem.setToFirst(); cursoritem.isValid();
//            cursoritem.setToNext())
//       {
      for( int j=0; j < Parent->TraceListSeq[ i ]->TraceList.size(); j++ )
          {
              ExportTraceList[NumTracesInList].ListItem = 
                  Parent->TraceListSeq[ i ]->TraceList[ j ];
              
              ExportTraceList[NumTracesInList].PrevSelected = FALSE;
              ExportTraceList[NumTracesInList].PostSelected = FALSE;
              
              strcpy(ExportTraceList[NumTracesInList].DataFileName ,"");
              
              
              // Inc list counter
              NumTracesInList++;
              
              ItemId++;
              
          }
      GroupId++;
   }

   FillScrolledList();
}

void ExportDialog::FillScrolledList()
{
  int loop;
  XmString* PrevTraceXmString;
  XmString* PostTraceXmString;

  PrevTraceXmString = (XmString*)malloc(sizeof(XmString) * NumTracesInList);
  PostTraceXmString = (XmString*)malloc(sizeof(XmString) * NumTracesInList);

  for (loop = 0; loop < NumTracesInList; loop++)
  {
    // Create string for entry to list
    PrevTraceXmString[loop] = XmStringCreateSimple(
              ExportTraceList[loop].ListItem->relative->PrevFile->GetName());

    PostTraceXmString[loop] = XmStringCreateSimple(
              ExportTraceList[loop].ListItem->relative->PostFile->GetName());

  }

  XmListAddItemsUnselected(wPrevTraceList,PrevTraceXmString,NumTracesInList,0);
  XmListAddItemsUnselected(wPostTraceList,PostTraceXmString,NumTracesInList,0);

  free(PrevTraceXmString);
  free(PostTraceXmString);

}

ExportDialog::~ExportDialog()
{
   XtDestroyWidget(wExportDataMain);
}

void ExportDialog::cbOKButtonPressed(Widget w,XtPointer client_data,
                                          XtPointer call_data)
{
  register int loop;
  int Error = 0;
  GNUPlotDialog *GNUPlotDialogBox;

  ExportDialog *Dialog = (ExportDialog*)client_data;
  char Command_String[2048];
  char SubCommand[2048];

  // check what action to carry out
  if (XmToggleButtonGetState(Dialog->wToggleExpPlot) == TRUE)
  {
    // Create the data files
    Dialog->CreateSelectedTraceData();

    // loop through all data file into one command line
    sprintf(Command_String,"plot ");
    for (loop = 0; loop < Dialog->NumTracesInList; loop++)
    {
      if (Dialog->ExportTraceList[loop].PrevSelected == TRUE)
      {
        sprintf(SubCommand," \"%s\" with lines",
                                    Dialog->ExportTraceList[loop].DataFileName);
        strcat(Command_String,SubCommand);
        if (loop < Dialog->NumTracesInList - 1)
        {
          strcat(Command_String,",");
        }
      }
    }
    Command_String[(strlen(Command_String) - 1)] = '\n';


    // Display plot
    GNUPlotDialogBox = new GNUPlotDialog(Dialog->wExportDataMain,
                                         Dialog->Parent,
                                         Command_String);
  }

  else
  {
    // Export to text files
    for (loop = 0;
         loop < Dialog->NumTracesInList;
         loop++)
    {
      if (Dialog->ExportTraceList[loop].PrevSelected == TRUE)
      {
         Dialog->CreateSelectedTraceDataForFiles(
                    Dialog->ExportTraceList[loop].ListItem->relative->PrevFile,
                    &(Dialog->ExportTraceList[loop].DataFileName[0]));
      }
     
      if (Dialog->ExportTraceList[loop].PostSelected == TRUE)
      {
         Dialog->CreateSelectedTraceDataForFiles(
                    Dialog->ExportTraceList[loop].ListItem->relative->PostFile,
                    &(Dialog->ExportTraceList[loop].PostDataFileName[0]));
      }
    }

    static ExportedFileDialog *ExportedFileDialogBox;

    ExportedFileDialogBox = new ExportedFileDialog(Dialog->wExportDataMain,
                                             Dialog->Parent,
                                             &(Dialog->ExportTraceList[0]),
                                             Dialog->NumTracesInList);
  }

  return;
}

void ExportDialog::CreateSelectedTraceDataForFiles( TraceFile*  FileToExport,
                                                    char*       Filename)
{
  FILE *data2;
  int PrevStartPos, PrevEndPos, PrevFilePos;
  float LTimeSeconds;
  mkdir( "./ExportFileData", S_IRUSR | S_IWUSR | S_IXUSR );

  // Generate a data file name
  sprintf(Filename,"./ExportFileData/File%s.dat",FileToExport->GetName());

  if (XmToggleButtonGetState(wToggleExpFileSelected) == TRUE)
  {
    // Do Prev file
    if ( FileToExport->IsHitInRange(ExportStartTime) == TRUE)
    {
      PrevStartPos = FileToExport->FindHitPos(ExportStartTime);
    }
    else if (FileToExport->IsHitInRange(ExportStartTime) == LOWER)
    {
      PrevStartPos = 0;
    }
    else if (FileToExport->IsHitInRange(ExportStartTime) == HIGHER)
    {
      PrevStartPos = FileToExport->GetNumberOfHits() - 1;
    }

    // Find end pos
    if (FileToExport->IsHitInRange(ExportEndTime) == TRUE)
    {
      PrevEndPos = FileToExport->FindHitPos(ExportEndTime);
    }
    else if (FileToExport->IsHitInRange(ExportEndTime) == LOWER)
    {
      PrevEndPos = 0;
    }
    else if (FileToExport->IsHitInRange(ExportEndTime) == HIGHER)
    {
      PrevEndPos = FileToExport->GetNumberOfHits() - 1;
    }
  }
  else
  {
    PrevStartPos = 0;
    PrevEndPos = FileToExport->GetNumberOfHits() - 1;
  }

  data2 = fopen(Filename,"w");

  PrevFilePos = PrevStartPos;


  // Print all the data to the file
  while (PrevFilePos  <= PrevEndPos)
  {

    // Select the file output format
    if (XmToggleButtonGetState(wToggleExpFileSecs) == TRUE)
    {
      LTimeSeconds = FileToExport->GetLTimeAtPosition(PrevFilePos)/
                                                              (float)1000000000;

      fprintf(data2,"%f\n",LTimeSeconds);
    }
    else if (XmToggleButtonGetState(wToggleExpFileLTime) == TRUE)
    {
      LTimeSeconds = FileToExport->GetLTimeAtPosition(PrevFilePos);

      fprintf(data2,"%lld\n",LTimeSeconds);
    }
    else
    {
      LTimeSeconds = FileToExport->GetsmalltimeAtPosition(PrevFilePos);

      fprintf(data2,"%lld\n",LTimeSeconds);
    }

    PrevFilePos++;
  }
  fclose(data2);
}

void ExportDialog::CreateSelectedTraceData()
{
  FILE *data2;
  int PrevStartPos, PrevEndPos, PrevFilePos;
  int PostStartPos, PostEndPos, PostFilePos;
  int loop;
  int CurrentState;
  float LTimeSeconds;

  mkdir( "./ExportPlotData", S_IRUSR | S_IWUSR | S_IXUSR );

  for (loop = 0;
       loop < NumTracesInList;
       loop++)
  {
    if (ExportTraceList[loop].PrevSelected == TRUE)
    {
      // Generate a data file name
      sprintf(ExportTraceList[loop].DataFileName,"./ExportPlotData/File%s.dat",
        ExportTraceList[loop].ListItem->relative->PrevFile->GetName());

      // Do Prev file
      if (ExportTraceList[loop].ListItem->relative->
                              PrevFile->IsHitInRange(ExportStartTime) == TRUE)
      {
        PrevStartPos = ExportTraceList[loop].ListItem->relative->PrevFile->
                                                    FindHitPos(ExportStartTime);
      }
      else if (ExportTraceList[loop].ListItem->relative->
                              PrevFile->IsHitInRange(ExportStartTime) == LOWER)
      {
        PrevStartPos = 0;
      }
      else if (ExportTraceList[loop].ListItem->relative->
                              PrevFile->IsHitInRange(ExportStartTime) == HIGHER)
      {
        PrevStartPos = ExportTraceList[loop].ListItem->relative->
                              PrevFile->GetNumberOfHits() - 1;
      }


      if (ExportTraceList[loop].ListItem->relative->
                              PostFile->IsHitInRange(ExportStartTime) == TRUE)
      {
        PostStartPos = ExportTraceList[loop].ListItem->relative->PostFile->
                                                    FindHitPos(ExportStartTime);
      }
      else if (ExportTraceList[loop].ListItem->relative->
                              PostFile->IsHitInRange(ExportStartTime) == LOWER)
      {
        PostStartPos = 0;
      }
      else if (ExportTraceList[loop].ListItem->relative->
                              PostFile->IsHitInRange(ExportStartTime) == HIGHER)
      {
        PostStartPos = ExportTraceList[loop].ListItem->relative->
                                               PostFile->GetNumberOfHits() - 1;
      }


      if (ExportTraceList[loop].ListItem->relative->
                              PrevFile->IsHitInRange(ExportEndTime) == TRUE)
      {
        PrevEndPos = ExportTraceList[loop].ListItem->relative->PrevFile->
                                                    FindHitPos(ExportEndTime);
      }
      else if (ExportTraceList[loop].ListItem->relative->
                              PrevFile->IsHitInRange(ExportEndTime) == LOWER)
      {
        PrevEndPos = 0;
      }
      else if (ExportTraceList[loop].ListItem->relative->
                              PrevFile->IsHitInRange(ExportEndTime) == HIGHER)
      {
        PrevEndPos = ExportTraceList[loop].ListItem->relative->
                              PrevFile->GetNumberOfHits() - 1;
      }


      if (ExportTraceList[loop].ListItem->relative->
                              PostFile->IsHitInRange(ExportEndTime) == TRUE)
      {
        PostEndPos = ExportTraceList[loop].ListItem->relative->PostFile->
                                                    FindHitPos(ExportEndTime);
      }
      else if (ExportTraceList[loop].ListItem->relative->
                              PostFile->IsHitInRange(ExportEndTime) == LOWER)
      {
        PostEndPos = 0;
      }
      else if (ExportTraceList[loop].ListItem->relative->
                              PostFile->IsHitInRange(ExportEndTime) == HIGHER)
      {
        PostEndPos = ExportTraceList[loop].ListItem->relative->
                                               PostFile->GetNumberOfHits() - 1;
      }

      data2 = fopen(ExportTraceList[loop].DataFileName,"w");

      PrevFilePos = PrevStartPos;
      PostFilePos = PostStartPos;

      if (PrevFilePos == PostFilePos)
      {
         CurrentState = 0;
      }

      else if (PrevFilePos < PostFilePos)
      {
        printf("Unexpect result\n");
      }

      else if (PrevFilePos > PostFilePos)
      {
         CurrentState = 1;
      }
       
      else
        printf("Error\n");

      while (PrevFilePos  <= PrevEndPos || PostFilePos  <= PostEndPos)
      {

        if (PrevFilePos == PostFilePos)
        {
          if (PrevFilePos <= PrevEndPos)
          {
            CurrentState++;

            LTimeSeconds = ExportTraceList[loop].ListItem->
                           relative->PrevFile->GetLTimeAtPosition(PrevFilePos)/
                                                              (float)1000000000;

            fprintf(data2,"%f, %d\n",LTimeSeconds,CurrentState);
            PrevFilePos++;
          }

          if (PostFilePos <= PostEndPos)
          {
            CurrentState--;

            LTimeSeconds = ExportTraceList[loop].ListItem->
               relative->PostFile->GetLTimeAtPosition(PostFilePos)/
                                                              (float)1000000000;

            fprintf(data2,"%f, %d\n",LTimeSeconds,CurrentState);
            PostFilePos++;
          }
        }
        else
        {
          if (PostFilePos <= PostEndPos)
          {
            CurrentState--;

            LTimeSeconds = ExportTraceList[loop].ListItem->
               relative->PostFile->GetLTimeAtPosition(PostFilePos)/
                                                              (float)1000000000;

            fprintf(data2,"%f, %d\n",LTimeSeconds,CurrentState);
            PostFilePos++;
          }

          if (PrevFilePos <= PrevEndPos)
          {
            CurrentState++;

            LTimeSeconds = ExportTraceList[loop].ListItem->
                           relative->PrevFile->GetLTimeAtPosition(PrevFilePos)/
                                                              (float)1000000000;

            fprintf(data2,"%f, %d\n",LTimeSeconds,CurrentState);
            PrevFilePos++;
          }
        }
      }
      fclose(data2);
    }

  } /* End looping through the data */

}

void ExportDialog::cbCancelButtonPressed(Widget w,XtPointer client_data,
                                          XtPointer call_data)
{
   ExportDialog *Dialog = (ExportDialog*)client_data;

   delete Dialog;


  return;
}


void ExportDialog::cbPrevTraceSelected(Widget w,XtPointer client_data,
                                                   XtPointer call_data)
{
  ExportDialog *Dialog = (ExportDialog*)client_data;
  XmListCallbackStruct* ptr = (XmListCallbackStruct*)call_data;


  if (Dialog->ExportTraceList[ptr->item_position - 1].PrevSelected == FALSE)
  {
    Dialog->ExportTraceList[ptr->item_position - 1].PrevSelected = TRUE; 

    // Check if the current selection is for the plot
    if (XmToggleButtonGetState(Dialog->wToggleExpPlot) == TRUE)
    {
      XmListSelectPos(Dialog->wPostTraceList,ptr->item_position,FALSE);
      Dialog->ExportTraceList[ptr->item_position - 1].PostSelected = TRUE;
    }
  }
  else
  {
    Dialog->ExportTraceList[ptr->item_position - 1].PrevSelected = FALSE; 

    if (XmToggleButtonGetState(Dialog->wToggleExpPlot) == TRUE)
    {
      XmListDeselectPos(Dialog->wPostTraceList,ptr->item_position);
      Dialog->ExportTraceList[ptr->item_position - 1].PostSelected = FALSE;
    }
  }
}

void ExportDialog::cbPostTraceSelected(Widget w,XtPointer client_data,
                                                   XtPointer call_data)
{
  ExportDialog *Dialog = (ExportDialog*)client_data;
  XmListCallbackStruct* ptr = (XmListCallbackStruct*)call_data;


  if (Dialog->ExportTraceList[ptr->item_position - 1].PostSelected == FALSE)
  {
    Dialog->ExportTraceList[ptr->item_position - 1].PostSelected = TRUE;

    // Check if the current selection is for the plot
    if (XmToggleButtonGetState(Dialog->wToggleExpPlot) == TRUE)
    {
      XmListSelectPos(Dialog->wPrevTraceList,ptr->item_position,FALSE);
      Dialog->ExportTraceList[ptr->item_position - 1].PrevSelected = TRUE;
    }
  }
  else
  {
    Dialog->ExportTraceList[ptr->item_position - 1].PostSelected = FALSE;

    if (XmToggleButtonGetState(Dialog->wToggleExpPlot) == TRUE)
    {
      XmListDeselectPos(Dialog->wPrevTraceList,ptr->item_position);
      Dialog->ExportTraceList[ptr->item_position - 1].PrevSelected = FALSE;
    }
  }
}


void ExportDialog::cbTogglePressed(Widget w,XtPointer client_data,
                                                   XtPointer call_data)
{
  int loop;
  int NumSelected;
  int *ArrayOfSelect;
  ExportDialog *Dialog = (ExportDialog*)client_data;
  XmListCallbackStruct* ptr = (XmListCallbackStruct*)call_data;

  if (XmToggleButtonGetState(Dialog->wToggleExpFile) == TRUE)
  {
    //Export to file
    // No action
    XtSetSensitive(Dialog->FileOptToggleButtonBox, TRUE);
    XtSetSensitive(Dialog->FileFormatToggleButtonBox,TRUE);

  }
  else
  {
    //Export to plot
    // pair up files that have been selected

    XtSetSensitive(Dialog->FileOptToggleButtonBox, FALSE);
    XtSetSensitive(Dialog->FileFormatToggleButtonBox,FALSE);

    if (XmListGetSelectedPos(Dialog->wPrevTraceList,
                             &ArrayOfSelect,
                             &NumSelected) == TRUE)
    {
      printf("NumSelected [%d]\n",NumSelected);

      for (loop = 0; loop < NumSelected; loop++)
      {
        if (XmListPosSelected(Dialog->wPostTraceList,
                              ArrayOfSelect[loop]) == FALSE)
        {
          XmListSelectPos(Dialog->wPostTraceList,
                        ArrayOfSelect[loop], FALSE);
          Dialog->ExportTraceList[ArrayOfSelect[loop] - 1].PostSelected = TRUE;

        }
      }
    }

    delete ArrayOfSelect;

    ArrayOfSelect = NULL;

    if (XmListGetSelectedPos(Dialog->wPostTraceList,
                             &ArrayOfSelect,
                             &NumSelected) == TRUE)
    {
      for (loop = 0; loop < NumSelected; loop++)
      {
        if (XmListPosSelected(Dialog->wPrevTraceList,
                              ArrayOfSelect[loop]) == FALSE)
        {
          XmListSelectPos(Dialog->wPrevTraceList,
                        ArrayOfSelect[loop], FALSE);
          Dialog->ExportTraceList[ArrayOfSelect[loop] - 1].PrevSelected = TRUE;

        }
      }
    }

  }
 
}

void ExportDialog::cbFileOptTogglePressed(Widget w,XtPointer client_data,
                                                   XtPointer call_data)
{
  ExportDialog *Dialog = (ExportDialog*)client_data;
  XmListCallbackStruct* ptr = (XmListCallbackStruct*)call_data;


}

