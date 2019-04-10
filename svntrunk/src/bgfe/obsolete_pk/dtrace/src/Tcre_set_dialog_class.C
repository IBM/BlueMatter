// template <class Element>
// class IACollection;
// #include <ilanglvl.hpp>

#include "Tcre_set_dialog.hpp"

CreateSetDialog::CreateSetDialog(TracerWindow *Parent)
{
   Boolean StartFileGiven = FALSE;
   Arg args[20];
   int n =0;
   char cstring[20];
   XmString s, sDir, sFilter, str;
   char *CurrentDirectory;
   char   *Initial_Filter = "*";
   char   *Initial_Prev_Filter = "*Start*";
   char   *Initial_Post_Filter = "*Finis*";
   LastOffset = 0;
   char SepStr[12];
   
   char DirName[MAX_FILENAME];
   char FileName[MAX_FILENAME];

   for(int i=0; i < MAX_TRACE_FILES; i++) 
     {
       IndeciesMarked[ i ] = 0;
       TempMappedFile[i] = NULL;
       MappedFile[i] = NULL;
     }

   Widget lb_Offset;
   Widget  mu_color;
   Widget lb_SetName, lb_TraceSeparation, mu_style, InvertLabel;
   Widget FileSelectionList, ContourLabel;

   ParentWindow = Parent;

   TextChangedTimer  = (int)-1;

   IsFileSelected = FALSE;

   if (strcmp(Parent->StartDirectory,"") == 0)
   {
      CurrentDirectory = (char*)malloc(MAX_FILENAME*(sizeof(char))); 
      if (getcwd(CurrentDirectory,MAX_FILENAME) == NULL)
      {
          printf("getcwd failed");
          sDir = XmStringCreateLocalized("~");
      }
      sDir = XmStringCreateLocalized(CurrentDirectory);

      free(CurrentDirectory);

   }
   else
   {
      GetPathFromFile(Parent->StartDirectory,&(FileName[0]),&(DirName[0]));

      sDir = XmStringCreateLocalized(DirName);
      StartFileGiven = TRUE;
   }

   strcpy (ColourSelection,ParentWindow->CurrentColourSelection);
   GraphStyle  = ParentWindow->CurrentSelectedStyle ;

   sFilter = XmStringCreateLocalized( Initial_Filter );

   s = XmStringCreateLocalized("Create Trace Set");

   XtSetArg(args[n],XmNallowResize, False ); n++;
   XtSetArg(args[n],XmNresizePolicy, XmRESIZE_NONE ); n++;
   XtSetArg(args[n],XmNwidth,   1200 ); n++;
   XtSetArg(args[n],XmNheight,  800 ); n++;
   XtSetArg(args[n],XmNdialogTitle, s  ); n++;
   XtSetArg(args[n],XmNfileTypeMask,XmFILE_REGULAR); n++;
   XtSetArg(args[n],XmNdirectory,  sDir); n++;
   XtSetArg(args[n],XmNdirMask,  sFilter); n++;   
   XtSetArg(args[n],XmNchildPlacement, XmPLACE_BELOW_SELECTION); n++;

   wCreSetDialog = XmCreateFileSelectionDialog(ParentWindow->form0,
                                                        "wCreSetDialog",args,n);

   // Free up the XmStrings
   XmStringFree(sDir);
   XmStringFree(sFilter);
   XmStringFree(s);

   /* Remove unwanted widgets from the top dialog box */
   XtUnmanageChild( XmFileSelectionBoxGetChild(wCreSetDialog,
                                XmDIALOG_HELP_BUTTON));

   XtUnmanageChild( XmFileSelectionBoxGetChild(wCreSetDialog,
                                XmDIALOG_TEXT));

   XtUnmanageChild( XmFileSelectionBoxGetChild(wCreSetDialog,
                                XmDIALOG_SELECTION_LABEL));
   
   // Widget fileList = XmFileSelectionBoxGetChild(wCreSetDialog, XmDIALOG_LIST);

   XtVaSetValues(XmFileSelectionBoxGetChild(wCreSetDialog, XmDIALOG_LIST), 
                 XmNselectionPolicy, XmMULTIPLE_SELECT,
                 NULL);   

   /* Create the form containing the new widgets */
   wExtraForm = XtVaCreateWidget("wExtraForm",xmFormWidgetClass,wCreSetDialog,
                                 XmNallowResize, True,
                                 NULL);

   str = XmStringCreateSimple("Set Name:");

   /* Create the Set name text box */
   lb_SetName = XtVaCreateManagedWidget("SetName", xmLabelWidgetClass,
                                          wExtraForm,
                                          XmNlabelString, str,
                                          XmNtopAttachment,    XmATTACH_FORM,
                                          XmNtopOffset,        5,
                                          XmNleftAttachment,   XmATTACH_FORM,
                                          NULL);

   tf_SetName = XtVaCreateManagedWidget("tf_SetName",
                              xmTextFieldWidgetClass, wExtraForm,
                              XmNmarginHeight,         3,
                              XmNmaxLength,            30,
                              XmNcolumns,              30,
                              XmNcursorPosition,       1,
                              XmNtopAttachment,        XmATTACH_WIDGET,
                              XmNtopWidget,            lb_SetName,
                              XmNtopOffset,            5,
                              XmNleftAttachment,       XmATTACH_FORM,
                              NULL);

   /* Create the Trace Separation text box */
   str = XmStringCreateSimple("Trace Separation:");

   lb_TraceSeparation = XtVaCreateManagedWidget("lb_TraceSeparation",
                                          xmLabelWidgetClass,
                                          wExtraForm,
                                          XmNlabelString, str,
                                          XmNtopAttachment,    XmATTACH_FORM,
                                          XmNtopOffset,        5,
                                          XmNleftAttachment,   XmATTACH_WIDGET,
                                          XmNleftWidget,       tf_SetName,
                                          NULL);

   tf_TraceSeparationValue = XtVaCreateManagedWidget("tf_TraceSeparation",
                              xmTextFieldWidgetClass, wExtraForm,
                              XmNmarginHeight,         3,
                              XmNmaxLength,            15,
                              XmNcolumns,              15,
                              XmNcursorPosition,       1,
                              XmNtopAttachment,        XmATTACH_WIDGET,
                              XmNtopWidget,            lb_TraceSeparation,
                              XmNtopOffset,            5,
                              XmNleftAttachment,       XmATTACH_WIDGET,
                              XmNleftWidget,tf_SetName ,
                              NULL);


   /* Create the Offset text box */
   str = XmStringCreateSimple("Offset:");
   lb_Offset = XtVaCreateManagedWidget("Offset", xmLabelWidgetClass,
                                          wExtraForm,
                                          XmNlabelString, str,
                                          XmNtopAttachment,    XmATTACH_FORM,
                                          XmNtopOffset,        5,
                                          XmNleftAttachment,   XmATTACH_WIDGET,
                                          XmNleftWidget,tf_TraceSeparationValue,
                                          NULL);
   

   XmStringFree(str);
   tf_CSOffset = XtVaCreateManagedWidget("tf_Offset",
                              xmTextFieldWidgetClass, wExtraForm,
                              XmNmarginHeight,         3,
                              XmNmaxLength,            5,
                              XmNcolumns,              5,
                              XmNcursorPosition,       1,
                              XmNtopAttachment,        XmATTACH_WIDGET,
                              XmNtopWidget,            lb_Offset,
                              XmNtopOffset,            5,
                              XmNleftAttachment,       XmATTACH_WIDGET,
                              XmNleftWidget,           tf_TraceSeparationValue,
                              NULL);

   /* Insert the value in to the text box */
   sprintf(cstring,"%d",ParentWindow->y_pos);
   XmTextSetString(tf_CSOffset,cstring);

   /* Create the colour menu */
   mu_color = CreateGenericColorPullDownMenu( wExtraForm,
                                              &CreateSetDialog::cbColourChoice,
                                              this ,
                      GetPositionOfColor(ColourSelection));

   XtVaSetValues(mu_color,
                 XmNtopAttachment,    XmATTACH_FORM,
                 XmNtopOffset,        0,
                 XmNleftAttachment,   XmATTACH_WIDGET,
                 XmNleftWidget,       tf_CSOffset,
                 XmNleftOffset,       5,
                 NULL );

   mu_style = CreateGenericGraphStylePullDownMenu( wExtraForm,
                                              &CreateSetDialog::cbStyleChoice,
                                              this, 
                                      (int)ParentWindow->CurrentSelectedStyle,
                                       &wVertLineButton );

   XtVaSetValues(mu_style,
                 XmNtopAttachment,    XmATTACH_FORM,
                 XmNtopOffset,        0,
                 XmNleftAttachment,   XmATTACH_WIDGET,
                 XmNleftWidget,       mu_color,
                 XmNleftOffset,       5,
                 NULL );


   // Create the inverted toggle button
   str = XmStringCreateSimple("Trace Inversion:");
   InvertLabel = XtVaCreateManagedWidget("InvertLabel", xmLabelWidgetClass,
                                          wExtraForm,
                                          XmNlabelString, str,
                                          XmNtopAttachment,    XmATTACH_FORM,
                                          XmNtopOffset,        5,
                                          XmNleftAttachment,   XmATTACH_WIDGET,
                                          XmNleftWidget,       mu_style,
                                          XmNleftOffset,       10,
                                          NULL);

   XmStringFree(str);

   str = XmStringCreateSimple("Inverted Trace");

   wInvertedToggle = XtVaCreateManagedWidget("wInvertedToggle",
                            xmToggleButtonWidgetClass, wExtraForm,
                            XmNtopAttachment,    XmATTACH_WIDGET,
                            XmNtopWidget,        InvertLabel,
                            XmNtopOffset,        5,
                            XmNleftAttachment,   XmATTACH_WIDGET,
                            XmNleftWidget,       mu_style,
                            XmNleftOffset,       10,
                            XmNlabelType,        XmSTRING,
                            XmNlabelString,      str,
                            XmNset,          ParentWindow->CurrentInvertedValue,
                            NULL);

   XmStringFree(str);

   // Create the contour toggle button
   str = XmStringCreateSimple("Contour Tracing:");
   ContourLabel = XtVaCreateManagedWidget("ContourLabel", xmLabelWidgetClass,
                                          wExtraForm,
                                          XmNlabelString, str,
                                          XmNtopAttachment,    XmATTACH_FORM,
                                          XmNtopOffset,        5,
                                          XmNleftAttachment,   XmATTACH_WIDGET,
                                          XmNleftWidget,       wInvertedToggle,
                                          XmNleftOffset,       10,
                                          NULL);

   XmStringFree(str);

   str = XmStringCreateSimple("Contour Tracing");

   wContourToggle = XtVaCreateManagedWidget("wContourToggle",
                            xmToggleButtonWidgetClass, wExtraForm,
                            XmNtopAttachment,    XmATTACH_WIDGET,
                            XmNtopWidget,        ContourLabel,
                            XmNtopOffset,        5,
                            XmNleftAttachment,   XmATTACH_WIDGET,
                            XmNleftWidget,       wInvertedToggle,
                            XmNleftOffset,       10,
                            XmNlabelType,        XmSTRING,
                            XmNlabelString,      str,
                            XmNset,           ParentWindow->CurrentContourValue,
                            NULL);

   XmStringFree(str);

   XtAddCallback(wContourToggle,XmNvalueChangedCallback,
                 CreateSetDialog::cbContourChoice,(XtPointer)this );

   str = XmStringCreateSimple("Select ALL Traces");

   wSelectAllToggle = XtVaCreateManagedWidget("wSelectAllToggle",
                                              xmToggleButtonWidgetClass, wExtraForm,
                                              XmNleftAttachment,   XmATTACH_WIDGET,
                                              XmNleftWidget,       wContourToggle,
                                              XmNleftOffset,       10,
                                              XmNlabelType,        XmSTRING,
                                              XmNlabelString,      str,
                                              XmNset,              0,
                                              NULL);
   XmStringFree(str);

   XtAddCallback(wSelectAllToggle, XmNvalueChangedCallback,
                 CreateSetDialog::cbSelectAllTracesChoice,(XtPointer)this );

   /* Create the Previous filter text box */
   CreatePreviousForm(wExtraForm, "*Start*");
   CreatePostForm(wExtraForm,  "*Finis*");

   /* Add callbacks to the main window */
   XtAddCallback(wCreSetDialog,XmNokCallback,
                 CreateSetDialog::cbCreSetOKButtonPressed,(XtPointer)this );
   XtAddCallback(wCreSetDialog,XmNcancelCallback,
                 CreateSetDialog::cbCreSetCancelButtonPressed,(XtPointer)this);


   /* Add callback to file selection so that when file is selected new
      response is carriedout */
   FileSelectionList = XmFileSelectionBoxGetChild(wCreSetDialog,XmDIALOG_LIST);

   XtAddCallback(FileSelectionList,
                 XmNmultipleSelectionCallback,
                 CreateSetDialog::cbFileListSelected,
                 (XtPointer)this);

   /* Open the screen */
   XtManageChild(wExtraForm);
   XtManageChild(mu_color);
   XtManageChild(mu_style);


   XtManageChild(wCreSetDialog);
   sprintf(SepStr,"%d",ParentWindow->CurrentSpacing);
   XmTextSetString(tf_TraceSeparationValue,SepStr);

   if (StartFileGiven == TRUE)
   {

     str = XmStringCreateSimple(Parent->StartDirectory);


     XmListSelectItem(FileSelectionList, str, TRUE);

     XmStringFree(str);
   }

   // Set sensitivity
   if (GraphStyle == POINTS_ONLY_STYLE)
   {
     XtSetSensitive(fr_post,FALSE);
   }
   else
   {
     XtSetSensitive(fr_post,TRUE);
   }

   // Set keyboard focus to the Prev Text Filter box
   XtSetKeyboardFocus(wExtraForm,wCreSetPrevTF);
}

void CreateSetDialog::UpdateDialog()
{
   char cstring[20];

   /* Insert the value in to the text box */
   sprintf(cstring,"%d",ParentWindow->y_pos);
   XmTextSetString(tf_CSOffset,cstring);
}

void CreateSetDialog::ResetDialog() 
{
  for (int i=0; i < MAX_TRACE_FILES; i++)
    {
      if (this->MappedFile[ i ] != NULL)
        {
          int FileIndex = 
            this->ParentWindow->GetFilePosition(this->MappedFileName[ i ]);
          
          if (FileIndex == -1)
            {
              // Deleting mapped file
              // Check before deleting
              delete (this->MappedFile[ i ]);
              this->MappedFile[ i ] = NULL;
            }
           this->IndeciesMarked[ i ] = 0;
        }
    }
}

void CreateSetDialog::CreatePreviousForm(Widget Parent, char* Initial_Filter)
{
   XmString xmString;
   char     text[32];
   Arg args[20];
   Cardinal n;
   Widget fr_prev,fmPrev ,lb_PrevNumFilesTitle,lb_PrevRegTitle,
           lb_PrevFileListTitle;

   fr_prev = XtVaCreateManagedWidget("fr_prev",xmFrameWidgetClass,Parent,
                                          XmNtopAttachment,    XmATTACH_WIDGET,
                                          XmNtopWidget     ,   tf_SetName,
                                          XmNtopOffset,         3,
                                          XmNleftAttachment,   XmATTACH_FORM,
                                          XmNleftOffset,       5,
                                          XmNbottomAttachment,  XmATTACH_FORM,
                                          XmNbottomOffset,      5,
                                          NULL);

   sprintf(text,"Prev Trace");
   xmString = XmStringCreateSimple(text);

   /* Create the Prev filter text box */
   (void)XtVaCreateManagedWidget("lb_Prev", xmLabelWidgetClass,
                             fr_prev,
                             XmNlabelString, xmString,
                             XmNchildType, XmFRAME_TITLE_CHILD,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
                             NULL);

   XmStringFree(xmString);

   fmPrev = XtVaCreateManagedWidget("fmPrev",
                                    xmFormWidgetClass,fr_prev,
                                    XmNwidth,         575,
                                    XmNheight,        350,
                                    XmNresizePolicy,  XmRESIZE_NONE,
                                    NULL);

   // Title for reg exp
   sprintf(text,"Regular Expression");
   xmString = XmStringCreateSimple(text);

   /* Create the Post filter text box */
   lb_PrevRegTitle = XtVaCreateManagedWidget("lb_PrevRegTitle",
                             xmLabelWidgetClass,
                             fmPrev,
                             XmNlabelString, xmString,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
                             XmNtopAttachment,        XmATTACH_FORM,
                             XmNtopOffset,            5,
                             XmNleftAttachment,   XmATTACH_FORM,
                             XmNleftOffset,       5,
                             NULL);

   XmStringFree(xmString);

   wCreSetPrevTF = XtVaCreateManagedWidget("wCreSetPrevTF",
                              xmTextFieldWidgetClass, fmPrev,
                              XmNmarginHeight,         3,
                              XmNmaxLength,            64,
                              XmNcolumns,              40,
                              XmNcursorPosition,       1,
                              XmNtopAttachment,        XmATTACH_WIDGET,
                              XmNtopWidget,            lb_PrevRegTitle,
                              XmNtopOffset,            3,
                              XmNleftAttachment,   XmATTACH_FORM,
                              XmNleftOffset,       5,
                              NULL);

   XmTextSetString(wCreSetPrevTF,Initial_Filter);

   XtAddCallback(wCreSetPrevTF,XmNvalueChangedCallback,
                 (XtCallbackProc)&CreateSetDialog::cbFilterTextChanged,
                 (XtPointer)this);



   // Title for Number of files
   sprintf(text,"Number Of Files");
   xmString = XmStringCreateSimple(text);

   /* Create the Prev filter text box */
   lb_PrevNumFilesTitle = XtVaCreateManagedWidget("lb_PrevNumFilesTitle",
                             xmLabelWidgetClass,
                             fmPrev,
                             XmNlabelString,              xmString,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment,   XmALIGNMENT_CENTER,
                             XmNtopAttachment,            XmATTACH_FORM,
                             XmNtopOffset,                5,
                             XmNleftAttachment,           XmATTACH_WIDGET,
                             XmNleftWidget,               wCreSetPrevTF,
                             XmNleftOffset,               5,
                             NULL);

   XmStringFree(xmString);

   // Title for Number of files
   sprintf(text,"0");
   xmString = XmStringCreateSimple(text);

   /* Create the Prev filter text box */
   lb_PrevNumFilesDisplay = XtVaCreateManagedWidget("lb_PrevNumFilesTitle",
                             xmLabelWidgetClass,
                             fmPrev,
                             XmNlabelString,              xmString,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment,   XmALIGNMENT_CENTER,
                             XmNtopAttachment,            XmATTACH_WIDGET,
                             XmNtopWidget,                lb_PrevNumFilesTitle,
                             XmNtopOffset,                5,
                             XmNleftAttachment,           XmATTACH_WIDGET,
                             XmNleftWidget,               wCreSetPrevTF,
                             XmNleftOffset,               5,
                             NULL);

   XmStringFree(xmString);

   // Title for Number of files
   sprintf(text,"File List");
   xmString = XmStringCreateSimple(text);
   /* Create the Prev filter text box */
   lb_PrevFileListTitle = XtVaCreateManagedWidget("lb_PrevFileListTitle",
                             xmLabelWidgetClass,
                             fmPrev,
                             XmNlabelString,              xmString,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment,   XmALIGNMENT_CENTER,
                             XmNtopAttachment,            XmATTACH_WIDGET,
                             XmNtopWidget,                wCreSetPrevTF,
                             XmNtopOffset,                5,
                             XmNleftAttachment,           XmATTACH_FORM,
                             XmNleftOffset,               5,
                             NULL);

   XmStringFree(xmString);

   n = 0;
   XtSetArg(args[n],XmNtopAttachment,XmATTACH_WIDGET);n++;
   XtSetArg(args[n],XmNtopWidget,lb_PrevFileListTitle);n++;
   XtSetArg(args[n],XmNtopOffset,5);n++;
   XtSetArg(args[n],XmNleftAttachment,XmATTACH_FORM);n++;
   XtSetArg(args[n],XmNleftOffset,10);n++;
   XtSetArg(args[n],XmNrightAttachment,XmATTACH_FORM);n++;
   XtSetArg(args[n],XmNrightOffset,5);n++;
   XtSetArg(args[n],XmNbottomAttachment,XmATTACH_FORM);n++;
   XtSetArg(args[n],XmNbottomOffset,5);n++;
   XtSetArg(args[n],XmNbottomOffset,5);n++;
   XtSetArg(args[n],XmNlistSizePolicy,XmCONSTANT);n++;



   liPrev = XmCreateScrolledList(fmPrev,"liPrev",args,n);

   XtAddCallback(liPrev,
                 XmNbrowseSelectionCallback,CreateSetDialog::cbTraceSelected,
                 (XtPointer)this);

   XtManageChild(liPrev);

}

void CreateSetDialog::cbTraceSelected( Widget w, XtPointer client_data,
                                      XtPointer call_data )
{
   CreateSetDialog *Dialog = (CreateSetDialog*)client_data;

   register int nloop;

   XmListCallbackStruct * ptr;
   char  *NewFilter;

   ptr = (XmListCallbackStruct *) call_data;

   XmStringGetLtoR( ptr->item, XmSTRING_DEFAULT_CHARSET, &NewFilter );

   for (nloop = 0; nloop < strlen(NewFilter); nloop++)
   {
      // Filter out characters that screw up reg exps
      if (
          (NewFilter[nloop] == '[') || 
          (NewFilter[nloop] == ']')
         )
      {
         NewFilter[nloop] = '*';
      }
   }

   if (w == Dialog->liPrev)
      XmTextSetString(Dialog->wCreSetPrevTF,NewFilter);
   else
      XmTextSetString(Dialog->wCreSetPostTF,NewFilter);

}

void CreateSetDialog::CreatePostForm(Widget Parent, char* Initial_Filter)
{
   XmString xmString;
   char     text[32];
   Arg args[20];
   Cardinal n;
   Widget fmPost,lb_PostNumFilesTitle,lb_PostRegTitle,
           lb_PostFileListTitle;

   fr_post = XtVaCreateManagedWidget("fr_post",xmFrameWidgetClass,Parent,
                                          XmNtopAttachment,     XmATTACH_WIDGET,
                                          XmNtopWidget,         tf_SetName,
                                          XmNtopOffset,         3,
                                          XmNrightAttachment,   XmATTACH_FORM,
                                          XmNrightOffset,       5,
                                          XmNbottomAttachment,  XmATTACH_FORM,
                                          XmNbottomOffset,      5,
                                          NULL);

   sprintf(text,"Post Trace");
   xmString = XmStringCreateSimple(text);

   /* Create the Post filter text box */
   (void)XtVaCreateManagedWidget("lb_Post", xmLabelWidgetClass,
                             fr_post,
                             XmNlabelString, xmString,
                             XmNchildType, XmFRAME_TITLE_CHILD,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
                             NULL);

   XmStringFree(xmString);

   fmPost = XtVaCreateManagedWidget("fmPost",xmFormWidgetClass,fr_post,
                                    XmNwidth,         575,
                                    XmNheight,        350,
                                    XmNresizePolicy,  XmRESIZE_NONE,
                                    NULL);

   // Title for reg exp
   sprintf(text,"Regular Expression");
   xmString = XmStringCreateSimple(text);

   /* Create the Post filter text box */
   lb_PostRegTitle = XtVaCreateManagedWidget("lb_PostRegTitle",
                             xmLabelWidgetClass,
                             fmPost,
                             XmNlabelString, xmString,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
                             XmNtopAttachment,        XmATTACH_FORM,
                             XmNtopOffset,            5,
                             XmNleftAttachment,   XmATTACH_FORM,
                             XmNleftOffset,       5,
                             NULL);

   XmStringFree(xmString);

   wCreSetPostTF = XtVaCreateManagedWidget("wCreSetPostTF",
                              xmTextFieldWidgetClass, fmPost,
                              XmNmarginHeight,         3,
                              XmNmaxLength,            64,
                              XmNcolumns,              40,
                              XmNcursorPosition,       1,
                              XmNtopAttachment,        XmATTACH_WIDGET,
                              XmNtopWidget,            lb_PostRegTitle,
                              XmNtopOffset,            3,
                              XmNleftAttachment,       XmATTACH_FORM,
                              XmNleftOffset,           5,
                              NULL);

   XmTextSetString(wCreSetPostTF,Initial_Filter);

   XtAddCallback(wCreSetPostTF,XmNvalueChangedCallback,
                 (XtCallbackProc) &CreateSetDialog::cbFilterTextChanged,
                 (XtPointer)this);

   // Title for Number of files
   sprintf(text,"Number Of Files");
   xmString = XmStringCreateSimple(text);

   /* Create the Post filter text box */
   lb_PostNumFilesTitle = XtVaCreateManagedWidget("lb_PostNumFilesTitle",
                             xmLabelWidgetClass,
                             fmPost,
                             XmNlabelString,              xmString,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment,   XmALIGNMENT_CENTER,
                             XmNtopAttachment,            XmATTACH_FORM,
                             XmNtopOffset,                5,
                             XmNleftAttachment,           XmATTACH_WIDGET,
                             XmNleftWidget,               wCreSetPostTF,      
                             XmNleftOffset,               5,
                             NULL);

   XmStringFree(xmString);

   // Title for Number of files
   sprintf(text,"0");
   xmString = XmStringCreateSimple(text);

   /* Create the Post filter text box */
   lb_PostNumFilesDisplay = XtVaCreateManagedWidget("lb_PostNumFilesTitle",
                             xmLabelWidgetClass,
                             fmPost,
                             XmNlabelString,              xmString,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment,   XmALIGNMENT_CENTER,
                             XmNtopAttachment,            XmATTACH_WIDGET,
                             XmNtopWidget,                lb_PostNumFilesTitle,
                             XmNtopOffset,                5,
                             XmNleftAttachment,           XmATTACH_WIDGET,
                             XmNleftWidget,               wCreSetPostTF,
                             XmNleftOffset,               5,
                             NULL);

   XmStringFree(xmString);

   // Title for Number of files
   sprintf(text,"File List");
   xmString = XmStringCreateSimple(text);
   /* Create the Post filter text box */
   lb_PostFileListTitle = XtVaCreateManagedWidget("lb_PostFileListTitle",
                             xmLabelWidgetClass,
                             fmPost,
                             XmNlabelString,              xmString,
                             XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                             XmNchildVerticalAlignment,   XmALIGNMENT_CENTER,
                             XmNtopAttachment,            XmATTACH_WIDGET,
                             XmNtopWidget,                wCreSetPostTF,
                             XmNtopOffset,                5,
                             XmNleftAttachment,           XmATTACH_FORM,
                             XmNleftOffset,               5,
                             NULL);

   XmStringFree(xmString);

   n = 0;
   XtSetArg(args[n],XmNtopAttachment,XmATTACH_WIDGET);n++;
   XtSetArg(args[n],XmNtopWidget,lb_PostFileListTitle);n++;
   XtSetArg(args[n],XmNtopOffset,5);n++;
   XtSetArg(args[n],XmNleftAttachment,XmATTACH_FORM);n++;
   XtSetArg(args[n],XmNleftOffset,5);n++;
   XtSetArg(args[n],XmNrightAttachment,XmATTACH_FORM);n++;
   XtSetArg(args[n],XmNrightOffset,5);n++;        
   XtSetArg(args[n],XmNbottomAttachment,XmATTACH_FORM);n++;
   XtSetArg(args[n],XmNbottomOffset,5);n++;        
   XtSetArg(args[n],XmNlistSizePolicy,XmCONSTANT);n++;

   liPost = XmCreateScrolledList(fmPost,"liPost",args,n);

   XtAddCallback(liPost,
                 XmNbrowseSelectionCallback,CreateSetDialog::cbTraceSelected,
                 (XtPointer)this);

   XtManageChild(liPost);
}

CreateSetDialog::~CreateSetDialog()
{
   XtUnmanageChild(wCreSetDialog);
}

void CreateSetDialog::cbFilterTextChanged(Widget w,XtPointer client_data,
                                          XtPointer call_data)
{
   CreateSetDialog *Dialog = (CreateSetDialog*)client_data;

   if (Dialog != NULL)
   {
      Dialog->FilterTextChangedCB(w, call_data);
   }
}

void CreateSetDialog::FilterTextChangedCB(Widget w, XtPointer call_data )
{
    XmAnyCallbackStruct* cbs = (XmAnyCallbackStruct*)(call_data);
    
    int AreAllNull = 1;
    for(int i=0; i < MAX_TRACE_FILES; i++) 
      { 
        if (MappedFile[ i ] != NULL)
          {
            AreAllNull = 0;
          }
      }

    if( AreAllNull )
      return;

    TimerWidget = w;

    if (TextChangedTimer != -1)
    {
       XtRemoveTimeOut(TextChangedTimer);
    }

    // Add time out
    TextChangedTimer = XtAppAddTimeOut(*(ParentWindow->GetApplicationContext()),
                                       750,
                                       &CreateSetDialog::cbFilterTextTimeOut,
                                       (XtPointer)this);
}

void CreateSetDialog::cbFilterTextTimeOut(XtPointer client_data,
                                          XtIntervalId* timer_Id)
{
   CreateSetDialog *Dialog = (CreateSetDialog*)client_data;

   if (Dialog != NULL)
   {
      Dialog->FilterTextTimeOutCB(timer_Id);
   }
}

void CreateSetDialog::FilterTextTimeOutCB( XtIntervalId* timer_Id)
{
    Boolean BeforeFound;
    Widget w  = TimerWidget;
    register int loop;
    char *Filter = XmTextGetString(w);
    char NewPostFilter[512];
    if (w == wCreSetPrevTF)
    {
       LoadPrevInternalData(Filter);
    }
    else if (w ==  wCreSetPostTF)
    {
      LoadPostInternalData(Filter);
    }

    if (w ==  wCreSetPrevTF)
    {
      // Copy Prev filter to temp string
      BeforeFound = FALSE;
      strcpy(NewPostFilter,"");

      // Loop through all characters of the prev string
      for (loop =0; loop + 5 < strlen(Filter); loop++)
      {
        // Check whether the prev string contains before
        if ((Filter[loop]     == 'S') &&
            (Filter[loop + 1] == 't') &&
            (Filter[loop + 2] == 'a') &&
            (Filter[loop + 3] == 'r') &&
            (Filter[loop + 4] == 't') )
        {
           // Filter does contain the Start value
           // Update the temp string with the converted start to finis
           strncpy(NewPostFilter,Filter,loop);
           strncpy(&NewPostFilter[loop], '\0', 1);
           strcat(NewPostFilter,"Finis");
           strncpy(&NewPostFilter[loop+5], '\0', 1);
           strcat(NewPostFilter,&(Filter[loop + 5]));           
           BeforeFound = TRUE;
        }
      }

      if (BeforeFound == TRUE)
      {
        XmTextSetString(wCreSetPostTF,NewPostFilter);
      }
      else
      {
        XmTextSetString(wCreSetPostTF,Filter);
      }
    }

    TextChangedTimer = -1;
}

void CreateSetDialog::cbDirectoryChanged(Widget w,XtPointer client_data,
                                         XtPointer call_data)
{
    XmFileSelectionBoxCallbackStruct* cbs = 
                                 (XmFileSelectionBoxCallbackStruct*)(call_data);

    CreateSetDialog *Dialog = (CreateSetDialog*)client_data;

    XmString    xmsDir,xmsFilter, xmsTemp;
    char *string;
    char chprev[1024],chpost[1024];

    XtVaGetValues(Dialog->wCreSetDialog,
                  XmNdirectory,&xmsDir,
                  XmNdirMask,  &xmsFilter,
                  NULL);

    XmStringGetLtoR(xmsFilter,XmSTRING_DEFAULT_CHARSET,&string);

    sprintf(chprev,"%s%s",string,XmTextGetString(Dialog->wCreSetPrevTF));

    XmStringGetLtoR(xmsDir,XmSTRING_DEFAULT_CHARSET,&string);

    sprintf(chprev,"%s%s",string,XmTextGetString(Dialog->wCreSetPrevTF));

    XtVaSetValues(Dialog->wCreSetPrevDialog,XmNdirectory,xmsDir,
                                    XmNdirMask, xmsFilter,NULL);
    XtVaSetValues(Dialog->wCreSetPostDialog,XmNdirectory,xmsDir,
                                    XmNdirMask, xmsFilter,NULL);

    /* Obtain the filter from the text boxes */
    xmsTemp = XmStringCreateSimple(chprev);

    XmFileSelectionDoSearch(Dialog->wCreSetPrevDialog,xmsTemp);

    XtFree(string);
    XmStringFree(xmsTemp);
    string = NULL;

    //
    // Do the Post file search
    //
    XmStringGetLtoR(xmsDir,XmSTRING_DEFAULT_CHARSET,&string);
    sprintf(chpost,"%s%s",string,XmTextGetString(Dialog->wCreSetPostTF));

    xmsTemp = XmStringCreateSimple(chpost);
    XmFileSelectionDoSearch(Dialog->wCreSetPostDialog,xmsTemp);

    // Free the strings that have been used
    XtFree(string);
    XmStringFree(xmsTemp);

    Dialog->ResetDialog();
}

void CreateSetDialog::cbCreSetCancelButtonPressed(Widget w,
                                     XtPointer client_data, XtPointer call_data)
{
    XmFileSelectionBoxCallbackStruct* cbs = 
                                 (XmFileSelectionBoxCallbackStruct*)(call_data);

   CreateSetDialog *Dialog = (CreateSetDialog*)client_data;

   Dialog->ResetDialog();

   XtUnmanageChild(Dialog->wCreSetDialog);
}

void CreateSetDialog::cbCreSetOKButtonPressed(Widget w,XtPointer client_data,
                                              XtPointer call_data)
{
   CreateSetDialog *Dialog = (CreateSetDialog*)client_data;

   if (Dialog != NULL)
   {
      Dialog->CreSetOKButtonPressedCB(w, call_data);
   }
}

void CreateSetDialog::CreSetOKButtonPressedCB(Widget w, XtPointer call_data)
{
    XmFileSelectionBoxCallbackStruct* cbs = 
                                 (XmFileSelectionBoxCallbackStruct*)(call_data);

  char            *dirString;
  Boolean    LoadGroupFromListResult;

  // Check if text modified timer is active
  if (TextChangedTimer != -1)
  {
    return;
  }

   /* Find current Directory */
   XmStringGetLtoR(cbs->value,XmSTRING_DEFAULT_CHARSET, &dirString);
   strcpy (ParentWindow->StartDirectory,dirString);
   strcpy (ParentWindow->CurrentPrevRegExp,XmTextGetString(wCreSetPrevTF));
   strcpy (ParentWindow->CurrentPostRegExp,XmTextGetString(wCreSetPostTF));
   ParentWindow->CurrentSpacing =atoi(XmTextGetString(tf_TraceSeparationValue));

   strcpy (ParentWindow->CurrentColourSelection,ColourSelection);
   ParentWindow->CurrentSelectedStyle = GraphStyle;
   ParentWindow->CurrentInvertedValue = XmToggleButtonGetState(wInvertedToggle);
   ParentWindow->CurrentContourValue  = XmToggleButtonGetState(wContourToggle);

   XtFree(dirString);

   LoadGroupFromListResult = LoadGroupFromList();

   if (LoadGroupFromListResult == TRUE)
   {
     ParentWindow->SetMidTime(FULL_REDRAW);
     XtUnmanageChild(wCreSetDialog);
   }

}

Boolean CreateSetDialog::LoadGroupFromList()
{
  MessageDialog             *MessageBox;
  TraceGroupPtr              Group;
  register int               nloop;
  char                       chGroupName[MAX_FILENAME];
  // TraceGroupListType::Cursor cursor(ParentWindow->TraceListSeq);
  __pktraceRead*             MappedFilePointer[ MAX_TRACE_FILES ];
  int                        FileIndex;
  int                        Offset;
  int                        SepValue;
  int                        InvertValue;
  int                        ContourValue;

  if (IsFileSelected == FALSE)
  {
     MessageBox = new MessageDialog(wCreSetDialog,"No file selected",ERROR);
     return FALSE;
  }


  // Check if selected style is POINTS_ONLY_STYLE
  // If not check for duplicate names and file numbers
  if (GraphStyle != POINTS_ONLY_STYLE)
  {
    if (strcmp(XmTextGetString(wCreSetPrevTF),
               XmTextGetString(wCreSetPostTF)) == 0)
    {
      /* Error both names the same selection */
      MessageBox = new MessageDialog(wCreSetDialog,
                         "Regular Expression Text fields contain the same data",
                         ERROR);

      return FALSE;
    }

    if (NumPreviousFileListItems != NumPostFileListItems)
    {
      /* Error incorrect selection */
      MessageBox = new MessageDialog(wCreSetDialog,
                                     "Number of file is different",ERROR);

      return FALSE;
    }
  }

   /* To reach here the selection must be OK */
  
  SepValue = atoi(XmTextGetString(tf_TraceSeparationValue));
  InvertValue = XmToggleButtonGetState(wInvertedToggle);
  ContourValue = XmToggleButtonGetState(wContourToggle);
  Offset = atoi(XmTextGetString(tf_CSOffset));
  strcpy(chGroupName, XmTextGetString(tf_SetName)); 

  /* Create a set here */
  try
    {
      Group = new TraceGroup;
      
      Group->Initialize( ParentWindow, chGroupName, " No_file_name",
                         ParentWindow->CurrentPrevRegExp,
                         ParentWindow->CurrentPostRegExp,
                         ColourSelection,
                         GraphStyle, Offset, SepValue, (Boolean)InvertValue,
                         (Boolean)ContourValue);
    }          
  catch(const char *string)
    {
      printf("Error raised [%s]\n",string);
    }

  // Add the open file to the window open file list
  for (nloop = 0; nloop < NumPreviousFileListItems; nloop++) 
    {
      FileIndex = PreviousFileList[nloop].FileIndex;
        
      // FileIndex = ParentWindow->AddToFileList( MappedFileName[i], MappedFile[i] );    
      int internalIndex = ParentWindow->AddToFileList( MappedFileName[ FileIndex ], MappedFile[ FileIndex ] );    
      
      MappedFilePointer[ FileIndex ] = ParentWindow->GetMappedPointer( internalIndex );
      
      {
              /* Get the path of the current position */
              
        Group->AddTrace( atoi(XmTextGetString(tf_CSOffset)) - (nloop),
                         1, ColourSelection,
                         RELATIVE,
                         MappedFilePointer[ FileIndex ],
                         PreviousFileList[nloop].MappedHeaderPos,
                         PostFileList[nloop].MappedHeaderPos,
                         GraphStyle,
                         XmToggleButtonGetState(wInvertedToggle));
      }
      
//          LastOffset = atoi(XmTextGetString(tf_CSOffset)) + 
//             ( NumPreviousFileListItems * 
//               atoi(XmTextGetString(tf_TraceSeparationValue)));
          
    }
  
  // Check if the add traces were successful
  if (Group->TraceList.size() == 0)
    {
      delete (TraceGroup*)Group;
    }
  else
    {
      // ParentWindow->TraceListSeq.add(Group,cursor);
      ParentWindow->TraceListSeq.push_back( Group );
    }
  
  return TRUE;
}


void CreateSetDialog::cbSelectAllTracesChoice( Widget w, XtPointer client_data,
                                               XtPointer call_data )
{
  CreateSetDialog *Dialog = (CreateSetDialog*) client_data;
  XmToggleButtonCallbackStruct* cbs =(XmToggleButtonCallbackStruct*)(call_data);
  Widget fileSelectionList = XmFileSelectionBoxGetChild(Dialog->wCreSetDialog, XmDIALOG_LIST);
  if (cbs->set == TRUE)
    {
      int itemCount = 0;
      XtVaGetValues(fileSelectionList,
                    XmNitemCount, &itemCount,
                    NULL);

      for(int i=1; i <= itemCount; i++) 
        {
          XmListSelectPos(fileSelectionList, i, 1);
        }
    }
  else 
    {
      XmListDeselectAllItems(fileSelectionList);
      XmListDeselectAllItems(Dialog->liPost);
      XmListDeleteAllItems(Dialog->liPost);
      XmListDeselectAllItems(Dialog->liPrev);
      XmListDeleteAllItems(Dialog->liPrev);

      char text[32];
      sprintf(text,"%d",0);
      XmString xmString = XmStringCreateSimple(text);
      XtVaSetValues(Dialog->lb_PrevNumFilesDisplay,
                    XmNlabelString, xmString, 
                    NULL);

      xmString = XmStringCreateSimple(text);
      XtVaSetValues(Dialog->lb_PostNumFilesDisplay,
                    XmNlabelString, xmString, 
                    NULL);
      XmStringFree(xmString);                     
    }
}
/*******************************************************************************
*
* Function: cbColourChoice
*
*
* Purpose:
*
*******************************************************************************/
void CreateSetDialog::cbColourChoice( Widget w, XtPointer client_data,
                                      XtPointer call_data )
{
   XmPushButtonCallbackStruct* cbs = (XmPushButtonCallbackStruct*)(call_data);
   CreateSetDialog *Dialog = (CreateSetDialog*) client_data;
   XmString xmstring;
   char *color_ptr;

   XtVaGetValues(w,XmNlabelString,&xmstring,NULL);
   XmStringGetLtoR(xmstring,XmSTRING_DEFAULT_CHARSET,&color_ptr);

   /* Set the global colour value to the selected colour */
   strcpy(Dialog->ColourSelection,color_ptr);

   XtFree(color_ptr);
}

void CreateSetDialog::cbFileListSelected( Widget w, XtPointer client_data,
                                          XtPointer call_data )
{
   CreateSetDialog *Dialog = (CreateSetDialog*)client_data;

   XmListCallbackStruct * ptr;
   XmStringTable    ItemList;
   int FileIndex;

   XtVaGetValues(w,XmNitems,&ItemList,NULL);

   ptr = (XmListCallbackStruct *) call_data;

   // int numberOfElementsSelected = ptr->selected_item_count;

   //   for(int i=0; i<numberOfElementsSelected; i++)
     {
       char *FullPath;
       
       // int indexMarked = ptr->selected_item_positions[ i ];
       int indexMarked = ptr->item_position;

       Dialog->IndeciesMarked[ indexMarked ] = 1;

       XmStringGetLtoR( ptr->item, XmSTRING_DEFAULT_CHARSET, &FullPath );
       strcpy (Dialog->MappedFileName[ indexMarked ], FullPath);
       free(FullPath);
       
       FileIndex = Dialog->ParentWindow->GetFilePosition( Dialog->MappedFileName[ indexMarked ] );
       
       if (FileIndex == -1)
         {
           // Load the new file
           Dialog->MappedFile[ indexMarked ] = new __pktraceRead(Dialog->MappedFileName[ indexMarked ]);
           
           if (Dialog->MappedFile[ indexMarked ]->IsValidMagicNum() == FALSE)
             {
               MessageDialog* MessageBox = new MessageDialog(Dialog->wCreSetDialog,
                                                             "File is not correct format for this version",
                                                             ERROR);
               
               delete Dialog->MappedFile[ indexMarked ];
               Dialog->MappedFile[ indexMarked ] = NULL;
       
               return;
             }           
         }
       else
         {
           Dialog->MappedFile[ indexMarked ] = Dialog->ParentWindow->GetMappedPointer( FileIndex );
         }
     }     

   // Set up the filter lists on the new loaded file
   Dialog->LoadPrevInternalData(XmTextGetString(Dialog->wCreSetPrevTF));
   Dialog->LoadPostInternalData(XmTextGetString(Dialog->wCreSetPostTF));
   // Dialog->CopyPrevDataToPostData();

   Dialog->IsFileSelected = TRUE;
}

void CreateSetDialog::ConvertStartToFinis(char *name, char* result )
{
  for(int i=0; i < strlen(name); i++)
    {
      if ((name[i]     == 'S') &&
          (name[i + 1] == 't') &&
          (name[i + 2] == 'a') &&
          (name[i + 3] == 'r') &&
          (name[i + 4] == 't') )
        {
          // Filter does contain the Start value
           // Update the temp string with the converted start to finis
           strncpy(result, name, i);
           strncpy(&result[i], '\0', 1);
           strcat(result,"Finis");
           strncpy(&result[i+5], '\0', 1);
           strcat(result,&(name[i + 5]));           
        }
    }
}

int CreateSetDialog::GetIndexForTraceName( char * name )
{ 
  for (int i = 0; i < MAX_TRACE_FILES; i++)
     {
       if( IndeciesMarked[ i ] ) 
         {
           
           for (int j = 0; j < MappedFile[ i ]->GetNumTraces(); j++)
             {
               if (strcmp(MappedFile[ i ]->GetTraceName( j ), name) == 0 )
                 {
                   return j;
                 }
             }
         }
       
     }
  
  return -1;
}

void CreateSetDialog::CopyPrevDataToPostData()
{
   register int loop;
   XmString* PostXmString;
   XmString xmString;
   char text[32];

   NumPostFileListItems = 0;

   XmListDeselectAllItems(liPost);
   XmListDeleteAllItems(liPost);

   int totalNumberOfTraces=0;
   for (loop = 0; loop < MAX_TRACE_FILES; loop++)
     if( IndeciesMarked[ loop ] ) 
       totalNumberOfTraces += MappedFile[loop]->GetNumTraces();


   PostXmString = (XmString*)malloc(sizeof(XmString) * totalNumberOfTraces);

   for(int i=0; i < NumPreviousFileListItems; i++)
     {
       char* currentString = PreviousFileList[ i ].Filename;

       char* currentStringWithFinis = new char[ strlen( currentString ) ];
       ConvertStartToFinis( currentString, currentStringWithFinis );
       
       int index = GetIndexForTraceName( currentStringWithFinis );
       
       if(index == -1) 
         {
           // Copy to the Post data list

           strcpy(currentStringWithFinis, " ");
           strcat(currentStringWithFinis, '\0');
         }

       strcpy(PostFileList[ NumPostFileListItems ].Filename,
              currentStringWithFinis );
       PostFileList[ NumPostFileListItems ].MappedHeaderPos = index;
           
       // Create string for entry to list
       PostXmString[ NumPostFileListItems ] 
         = XmStringCreateSimple( PostFileList[NumPostFileListItems].Filename );
       
       NumPostFileListItems++;
       delete  currentStringWithFinis ;
     }
   
#if 0
   for (int i = 0; i < MAX_TRACE_FILES; i++)
     {
       if( IndeciesMarked[ i ] ) 
         {
           
           for (loop = 0; loop < MappedFile[ i ]->GetNumTraces(); loop++)
             {
               if (CheckReg(RegExp, MappedFile[ i ]->GetTraceName(loop)) == True)
                 {
                   // Copy to the Post data list
                   strcpy(PostFileList[NumPostFileListItems].Filename,
                          MappedFile[i]->GetTraceName(loop));
                   PostFileList[NumPostFileListItems].MappedHeaderPos = loop;
                   
                   // Create string for entry to list
                   PostXmString[NumPostFileListItems] 
                     = XmStringCreateSimple( PostFileList[NumPostFileListItems].Filename );
                   
                   // Increment the number of items
                   NumPostFileListItems++;
                 }
             }
         }
     }
#endif

   XmListAddItemsUnselected(liPost,PostXmString,NumPostFileListItems,0);
   free(PostXmString);

   // Set the number of files selected
   sprintf(text,"%d",NumPostFileListItems);
   xmString = XmStringCreateSimple(text);
   XtVaSetValues(lb_PostNumFilesDisplay,
                 XmNlabelString, xmString, 
                 NULL);
   XmStringFree(xmString);
}

void CreateSetDialog::LoadPrevInternalData(char *RegExp )
{
   register int loop;
   XmString* PrevXmString;
   XmString xmString;
   char text[32];

   NumPreviousFileListItems = 0;

   XmListDeselectAllItems(liPrev);
   XmListDeleteAllItems(liPrev);

   int totalNumberOfTraces=0;
   for (loop = 0; loop < MAX_TRACE_FILES; loop++)
     if( IndeciesMarked[ loop ] ) 
       totalNumberOfTraces += MappedFile[loop]->GetNumTraces();


   PrevXmString = (XmString*)malloc(sizeof(XmString) * totalNumberOfTraces );

   for (int i = 0; i < MAX_TRACE_FILES; i++)
     {
     if( IndeciesMarked[ i ] ) 
       {
         for (loop = 0; loop < MappedFile[i]->GetNumTraces(); loop++)
           {
             if (CheckReg(RegExp, MappedFile[i]->GetTraceName(loop)) == True)
               {
                 // Copy to the Previous data list
                 strcpy(PreviousFileList[NumPreviousFileListItems].Filename,
                        MappedFile[i]->GetTraceName(loop));
                 PreviousFileList[NumPreviousFileListItems].MappedHeaderPos = loop;
                 PreviousFileList[NumPreviousFileListItems].FileIndex = i;

                 // Create string for entry to list
                 PrevXmString[NumPreviousFileListItems] 
                   = XmStringCreateSimple( PreviousFileList[NumPreviousFileListItems].Filename);

                 // Increment the number of items
                 NumPreviousFileListItems++;
               }
           }
       }
     }
   
   XmListAddItemsUnselected(liPrev,PrevXmString,NumPreviousFileListItems,0);
   free(PrevXmString);

   // Set the number of files selected
   sprintf(text,"%d",NumPreviousFileListItems);
   xmString = XmStringCreateSimple(text);
   XtVaSetValues(lb_PrevNumFilesDisplay,
                  XmNlabelString, xmString, 
                 NULL);
   XmStringFree(xmString);                  
}

void CreateSetDialog::LoadPostInternalData(char *RegExp )
{
   register int loop;
   XmString* PostXmString;
   XmString xmString;
   char text[32];

   NumPostFileListItems = 0;

   XmListDeselectAllItems(liPost);
   XmListDeleteAllItems(liPost);

   int totalNumberOfTraces=0;
   for (loop = 0; loop < MAX_TRACE_FILES; loop++)
     if( IndeciesMarked[ loop ] ) 
       totalNumberOfTraces += MappedFile[loop]->GetNumTraces();


   PostXmString = (XmString*)malloc(sizeof(XmString) * totalNumberOfTraces);

   for (int i = 0; i < MAX_TRACE_FILES; i++)
     {
       if( IndeciesMarked[ i ] ) 
         {
           
           for (loop = 0; loop < MappedFile[ i ]->GetNumTraces(); loop++)
             {
               if (CheckReg(RegExp, MappedFile[ i ]->GetTraceName(loop)) == True)
                 {
                   // Copy to the Post data list
                   strcpy(PostFileList[NumPostFileListItems].Filename,
                          MappedFile[i]->GetTraceName(loop));
                   PostFileList[NumPostFileListItems].MappedHeaderPos = loop;
                   PostFileList[NumPostFileListItems].FileIndex = i;
                   
                   // Create string for entry to list
                   PostXmString[NumPostFileListItems] 
                     = XmStringCreateSimple( PostFileList[NumPostFileListItems].Filename );
                   
                   // Increment the number of items
                   NumPostFileListItems++;
                 }
             }
         }
     }

   XmListAddItemsUnselected(liPost,PostXmString,NumPostFileListItems,0);
   free(PostXmString);

   // Set the number of files selected
   sprintf(text,"%d",NumPostFileListItems);
   xmString = XmStringCreateSimple(text);
   XtVaSetValues(lb_PostNumFilesDisplay,
                 XmNlabelString, xmString, 
                 NULL);
   XmStringFree(xmString);
}

int QsortComp(const void* Data1 , const void* Data2 )
{
   return strcmp(((ListElement*)(Data1))->Filename,
                 ((ListElement*)(Data2))->Filename);
}

/*******************************************************************************
*
* Function: cbStyleChoice
*
*
* Purpose:
*
*******************************************************************************/
void CreateSetDialog::cbStyleChoice( Widget w, XtPointer client_data,
                                      XtPointer call_data )
{
   XmPushButtonCallbackStruct* cbs = (XmPushButtonCallbackStruct*)(call_data);

   CreateSetDialog *Dialog = (CreateSetDialog*) client_data;
   XmString xmstring;
   char *pch_style_ptr;

   XtVaGetValues(w,XmNlabelString,&xmstring,NULL);
   XmStringGetLtoR(xmstring,XmSTRING_DEFAULT_CHARSET,&pch_style_ptr);

   if(strcmp(pch_style_ptr,GraphStyleStrings[0]) == 0)
   {
      Dialog->GraphStyle = FLAT_LINE_STYLE;
      XtSetSensitive(Dialog->fr_post,TRUE);
      XtSetSensitive(Dialog->wContourToggle,TRUE);
   }
   else if (strcmp(pch_style_ptr,GraphStyleStrings[1]) == 0)
   {
      Dialog->GraphStyle = VERT_LINE_STYLE;
      XtSetSensitive(Dialog->fr_post,TRUE);
      XtSetSensitive(Dialog->wContourToggle,FALSE);
   }
   else if (strcmp(pch_style_ptr,GraphStyleStrings[2]) == 0)
   {
      Dialog->GraphStyle = POINTS_ONLY_STYLE;
      XtSetSensitive(Dialog->fr_post,FALSE);
      XtSetSensitive(Dialog->wContourToggle,TRUE);
   }
   else
   {
      printf("Error in  CreateSetDialog::cbStyleChoice\n");
   }

   XtFree(pch_style_ptr);
}

void CreateSetDialog::cbContourChoice( Widget w, XtPointer client_data,
                                       XtPointer call_data )
{
  CreateSetDialog *Dialog = (CreateSetDialog*) client_data;

  XmToggleButtonCallbackStruct* cbs =(XmToggleButtonCallbackStruct*)(call_data);
  if (cbs->set == TRUE)
    XtSetSensitive(Dialog->wVertLineButton,FALSE);
  else
    XtSetSensitive(Dialog->wVertLineButton,TRUE);
 
}
