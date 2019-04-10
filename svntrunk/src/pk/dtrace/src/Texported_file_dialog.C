template <class Element>
class IACollection;
#include <Texported_file_dialog.hpp>

ExportedFileDialog::ExportedFileDialog(Widget        ParentWidget,
                                       TracerWindow *ParentWindow,
                                       ExportList   *ListOfExportFiles,
                                       int           NumFilesExported)
{
  Widget fr_TraceFile, Sep1, fm_TraceFileList;

  Arg args[20];
  int n =0;
  Parent = ParentWindow;
  XmString xmString;

  SelectedFilePos = -1;
  XtSetArg( args[n],  XmNallowResize, True );              n++;
  XtSetArg( args[n],  XmNwidth,       1250 );              n++;
  XtSetArg( args[n],  XmNheight,      800 );               n++;
  XtSetArg( args[n],  XmNdialogStyle, XmDIALOG_MODELESS ); n++;
  XtSetArg( args[n],  XmNtitle,       "Exported Data" );     n++;



  wExportedDataMain = XmCreateFormDialog( ParentWidget,
      (char *)  "wExportedDataMain", args, n );

   n = 0;
   XtSetArg( args[n],  XmNleftAttachment,   XmATTACH_FORM ); n++;
   XtSetArg( args[n],  XmNleftOffset,       0 );             n++;
   XtSetArg( args[n],  XmNrightAttachment,  XmATTACH_FORM ); n++;
   XtSetArg( args[n],  XmNrightOffset,      0 );             n++;
   XtSetArg( args[n],  XmNbottomAttachment, XmATTACH_FORM ); n++;
   XtSetArg( args[n],  XmNbottomOffset,     45 );            n++;

   Sep1 =  XmCreateSeparatorGadget(wExportedDataMain,(char *) "ExpSep1",args,n);

   XtManageChild(Sep1);

  /* Create the trace file data area */
  fr_TraceFile = XtVaCreateManagedWidget("fr_ExpTraceFile",
                                         xmFrameWidgetClass,  wExportedDataMain,
                                         XmNtopAttachment,    XmATTACH_FORM,
                                         XmNtopOffset,        5,
                                         XmNleftAttachment,   XmATTACH_FORM,
                                         XmNleftOffset,       5,
                                         XmNrightAttachment,  XmATTACH_FORM,
                                         XmNrightOffset,      5,
                                         XmNbottomAttachment, XmATTACH_WIDGET,
                                         XmNbottomWidget,     Sep1,
                                         XmNbottomOffset,     5,
                                         NULL);

  xmString = XmStringCreateSimple((char *) "List Of Exported Trace Files");

  (void)XtVaCreateManagedWidget("lb_ExpTraceFile",          xmLabelWidgetClass,
                             fr_TraceFile,
                             XmNlabelString,              xmString,
                             XmNchildType,                XmFRAME_TITLE_CHILD,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment,   XmALIGNMENT_CENTER,
                             NULL);
  XmStringFree(xmString);

  fm_TraceFileList = XtVaCreateManagedWidget("fm_ExpTraceFileList",
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
   XtSetArg(args[n],XmNrightAttachment,XmATTACH_FORM);n++;
   XtSetArg(args[n],XmNrightOffset,5);n++;
   XtSetArg(args[n],XmNlistSizePolicy,XmCONSTANT);n++;
   XtSetArg(args[n],XmNwidth,450);n++;

   wTraceList = XmCreateScrolledList(fm_TraceFileList,
       (char *) "wExpTraceList",args,n);

   XtAddCallback(wTraceList,
                 XmNbrowseSelectionCallback,ExportedFileDialog::cbTraceSelected,
                 (XtPointer)this);

   XtManageChild(wTraceList);

   LoadListData(ListOfExportFiles,NumFilesExported);

   xmString = XmStringCreateSimple((char *) "View");

   wViewButton = CreatePushButton(wExportedDataMain,(char *) "ExpViewbutton",
                 cbViewButtonPressed,
                 (XtPointer) this);

   XtVaSetValues(wViewButton,
                 XmNbottomAttachment,        XmATTACH_FORM,
                 XmNbottomOffset,            10,
                 XmNleftAttachment,          XmATTACH_FORM,
                 XmNleftOffset,              10,
                 XmNlabelString,             xmString,
                 NULL );

   XmStringFree(xmString);

   xmString = XmStringCreateSimple((char *) "Cancel");

   wCancelButton = CreatePushButton(wExportedDataMain,(char *) "ExpCancelbutton",
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

   XtManageChild( ParentWidget );

   XtManageChild( wExportedDataMain );

}

void ExportedFileDialog::LoadListData(ExportList *ListOfExportFiles,
                                      int         NumFilesExported)
{
  int loop;
  XmString* PrevTraceXmString;
  int NumStrings = 0;
  PrevTraceXmString = (XmString*)malloc(sizeof(XmString)* NumFilesExported * 3);

  for (loop = 0; loop < NumFilesExported; loop++)
  {
    // Create string for entry to list
    if (ListOfExportFiles[loop].PrevSelected == TRUE)
    {
      PrevTraceXmString[NumStrings] = XmStringCreateSimple(
              ListOfExportFiles[loop].DataFileName);
      NumStrings++;
    }

    if (ListOfExportFiles[loop].PostSelected == TRUE)
    {
      PrevTraceXmString[NumStrings] = XmStringCreateSimple(
              ListOfExportFiles[loop].PostDataFileName);
      NumStrings++;
    }
  }


  XmListAddItemsUnselected(wTraceList,PrevTraceXmString,NumStrings,0);

  free(PrevTraceXmString);


}

ExportedFileDialog::~ExportedFileDialog()
{
//   XtDestroyWidget(wExportedDataMain);
}

void ExportedFileDialog::cbViewButtonPressed(Widget w,XtPointer client_data,
                                          XtPointer call_data)
{
  XmStringTable   SelectedItems;
  char*           string;

  ExportedFileDialog *Dialog = (ExportedFileDialog*)client_data;


  XtVaGetValues(Dialog->wTraceList,
                XmNselectedItems,    &SelectedItems,
                NULL);

  XmStringGetLtoR(SelectedItems[0],
                  XmSTRING_DEFAULT_CHARSET,
                  &string);

 
  XtFree(string);

}

void ExportedFileDialog::cbCancelButtonPressed(Widget w,XtPointer client_data,
                                          XtPointer call_data)
{
   ExportedFileDialog *Dialog = (ExportedFileDialog*)client_data;

   delete Dialog;

}


void ExportedFileDialog::cbTraceSelected(Widget w,XtPointer client_data,
                                                   XtPointer call_data)
{
  ExportedFileDialog *Dialog = (ExportedFileDialog*)client_data;
  XmListCallbackStruct* ptr = (XmListCallbackStruct*)call_data;


  Dialog->SelectedFilePos = ptr->item_position;
}
