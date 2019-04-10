// template <class Element>
// class IACollection;
// #include <ilanglvl.hpp>
#include "Tapplication_class.hpp"

/*******************************************************************************
*
* Function: Constructor TracerApplication
*
*
* Purpose:
*
*******************************************************************************/

TracerApplication* TracerApplication::GetApp(int argc , char **argv )
{
  static TracerApplication *SingletonPtr;

  if ( SingletonPtr == NULL )
  {
    SingletonPtr = new TracerApplication(argc,argv);
  }
  assert( SingletonPtr != NULL );

  return (SingletonPtr);
}

TracerApplication::TracerApplication(int argc, char **argv)
{
   int            n;
   Arg            args[20];

   strcpy(StartDirectory,"");

   // Process Command line options
   ParseCommandLine(argc,argv);

   /* Set values of Global Vars */
   AppHeight = 640 + daTop + 290 + ButtonBarHeight; // Application specific
   AppWidth  = 1300;       // Application specific

   n = 0;
   XtSetArg( args[n],  XmNtitle,    "Tracer Application" ); n++;
   XtSetArg( args[n],  XmNiconName, "Tracer Application" ); n++;
   XtSetArg( args[n],  XmNwidth,    AppWidth ); n++;
   XtSetArg( args[n],  XmNheight,   AppHeight ); n++;
   XtSetArg( args[n],  XmNx, 5 ); n++;
   XtSetArg( args[n],  XmNy, 5 ); n++;


   /* Setup the top-most Widget */
   top = XtAppInitialize( &context,"TRACER",NULL,0,&argc,argv,NULL,args, n );

   /* Setup the main window values */
   n =0;
   XtSetArg( args[n],  XmNwidth,  AppWidth ); n++;
   XtSetArg( args[n],  XmNheight, AppHeight ); n++;
   XtSetArg( args[n],XmNallowResize, True);n++;

  ExportingDataFlag = FALSE ;

  
   mainW = XmCreateMainWindow( top, "main", args, n);

   XtManageChild( mainW );

   /* Create the Menu Bar */

   CreateMainMenu();

   MainWinForm = XtVaCreateManagedWidget("MainWinForm", xmFormWidgetClass,
                                         mainW,
                                         NULL);

   CreateMessageArea(mainW);

   XtVaSetValues(mainW,
                 XmNmessageWindow, frMessageWindow,
                 XmNworkWindow,    MainWinForm,
                 NULL);

   frWindowArea = XtVaCreateManagedWidget("frWindowArea",
                                           xmFrameWidgetClass,
                                           MainWinForm,
                                           XmNbottomAttachment, XmATTACH_FORM,
                                           XmNbottomOffset,     55,
                                           XmNtopAttachment,    XmATTACH_FORM,
                                           XmNtopOffset,        37,
                                           XmNrightAttachment,  XmATTACH_FORM,
                                           XmNrightOffset,      2,
                                           XmNleftAttachment,   XmATTACH_FORM,
                                           XmNleftOffset,       2,
                                           NULL);


   /* Create the application screen decorations */
   CreateButtonBar(MainWinForm);
   CreateScaleArea(MainWinForm);
   CreateZoomButtons(MainWinForm);
   CreateScaleToggles(MainWinForm);

}
/*******************************************************************************
*
* Function: Destructor ~TracerApplication
*
*
* Purpose:
*
*******************************************************************************/

TracerApplication::~TracerApplication()
{
   delete CurrentWindow;

   XtDestroyWidget(top);


   exit (0);
}

/*******************************************************************************
*
* Function: CreateMainMenu
*
*
* Purpose:
*
*******************************************************************************/

void TracerApplication::CreateMainMenu()
{
   Widget OptionPulldown, FilePulldown, w;
   XmString s;

   Arg args[16];
   int n =0;

   n=0;

   MenuBar = XmCreateMenuBar( mainW, "menu", args , n );

   XtManageChild( MenuBar);

   s = XmStringCreateLocalized("File");
  
   n =0;
   FilePulldown = XmCreatePulldownMenu( MenuBar,"Pulldown", args, n );

   XtVaSetValues( FilePulldown, XmNtearOffModel, XmTEAR_OFF_ENABLED, NULL );

   XtVaCreateManagedWidget( "File", xmCascadeButtonWidgetClass, MenuBar,
                                XmNlabelString, s,
                                XmNmnemonic,    'F',
                                XmNsubMenuId,   FilePulldown,
                                NULL);

   /* Create File Menu Options */
   w =XtVaCreateManagedWidget( "New        ", xmPushButtonWidgetClass,
                               FilePulldown, NULL );

   XtAddCallback(w, XmNactivateCallback,
                 (XtCallbackProc)&TracerApplication::cbNewWindow,
                 (XtPointer)this);



   w =XtVaCreateManagedWidget( "Open", xmPushButtonWidgetClass,
                               FilePulldown, NULL );

   XtAddCallback(w, XmNactivateCallback,
                 (XtCallbackProc)&TracerApplication::cbOpenFile,
                 (XtPointer)this);

   XtVaCreateManagedWidget( "", xmSeparatorWidgetClass, FilePulldown, NULL );

   w = XtVaCreateManagedWidget( "Save",
                                 xmPushButtonWidgetClass, FilePulldown, NULL );

   XtAddCallback(w, XmNactivateCallback,
                 (XtCallbackProc)&TracerApplication::cbSaveFile,
                 (XtPointer)this);

   w = XtVaCreateManagedWidget( "Save As",
                                  xmPushButtonWidgetClass, FilePulldown, NULL );

   XtAddCallback(w, XmNactivateCallback,
                 (XtCallbackProc)&TracerApplication::cbSaveAsFile,
                 (XtPointer)this);

   XtVaCreateManagedWidget( "", xmSeparatorWidgetClass, FilePulldown, NULL );


   /* Button to exit application */
   w = XtVaCreateManagedWidget( "Exit",
                                  xmPushButtonWidgetClass, FilePulldown, NULL );

   XtAddCallback(w, XmNactivateCallback,
                    (XtCallbackProc)&TracerApplication::cbExitFile ,
                    (XtPointer)this);

   n =0;
   OptionPulldown = XmCreatePulldownMenu( MenuBar,"Pulldown", args, n );

   s = XmStringCreateLocalized("Options");

   XtVaCreateManagedWidget( "Options", xmCascadeButtonWidgetClass, MenuBar,
                                XmNlabelString, s,
                                XmNmnemonic,    'O',
                                XmNsubMenuId,   OptionPulldown,
                                NULL);

   w = XtVaCreateManagedWidget( "Show Grid",
                               xmToggleButtonWidgetClass, OptionPulldown,
                               XmNset, True,
                               XmNindicatorType, XmN_OF_MANY,
                               XmNvisibleWhenOff,  True,
                               NULL );

  
   XtAddCallback(w, XmNvalueChangedCallback,
                 (XtCallbackProc) &TracerApplication::cbGrid,
                 (XtPointer)this);


   XmStringFree(s);

}

void TracerApplication::CreateButtonBar(Widget Parent)
{
   Widget Button;
   XmString xmString;
   Widget ButtonBarRowColumn;

   ButtonBarForm = XtVaCreateManagedWidget("ButtonBarForm",
                                           xmFrameWidgetClass,
                                           Parent,
                                           XmNtopAttachment,    XmATTACH_FORM,
                                           XmNtopOffset,        2,
                                           XmNleftAttachment,   XmATTACH_FORM,
                                           XmNleftOffset,       2,
                                           XmNrightAttachment,  XmATTACH_FORM,
                                           XmNrightOffset,      2,
                                           NULL);
   ButtonBarRowColumn= XtVaCreateManagedWidget("ButtonBarRowColumn",
                           xmRowColumnWidgetClass,
                           ButtonBarForm,
                           XmNorientation,       XmHORIZONTAL,
                           NULL );

   // Load a data set button
   xmString = XmStringCreateSimple("Create Trace View");

   Button = CreatePushButton(ButtonBarRowColumn,"cbCreateTrace",
                 (XtCallbackProc)TracerApplication::cbCreateTrace,
                 (XtPointer) this);


   XtVaSetValues(Button,
                 XmNlabelString, xmString,
                 NULL );

   XmStringFree(xmString);

   // Save all sets to directory
   xmString = XmStringCreateSimple("Save All Groups");

   Button = CreatePushButton(ButtonBarRowColumn,"cbSaveAllSets",
                 (XtCallbackProc)TracerApplication::cbSaveAllSets,
                 (XtPointer) this);


   XtVaSetValues(Button,
                 XmNlabelString, xmString,
                 NULL );

   XmStringFree(xmString);


   xmString = XmStringCreateSimple("Previous Hit");

   Button = CreatePushButton(ButtonBarRowColumn,"WarpClickDown",
                 (XtCallbackProc)TracerApplication::WarpClickDown,
                 (XtPointer) this);

   XtVaSetValues(Button,
                 XmNlabelString, xmString,
                 NULL );

   XmStringFree(xmString);



   xmString = XmStringCreateSimple("Next Hit");

   Button = CreatePushButton(ButtonBarRowColumn,"WarpClickUp",
                 (XtCallbackProc)TracerApplication::WarpClickUp,
                 (XtPointer) this);

   XtVaSetValues(Button,
                 XmNlabelString, xmString,
                 NULL );

   XmStringFree(xmString);

   xmString = XmStringCreateSimple("Export Data");

   ExportButton = CreatePushButton(ButtonBarRowColumn,"Export",
                 (XtCallbackProc)TracerApplication::cbExportData,
                 (XtPointer) this);

   XtVaSetValues(ExportButton,
                 XmNlabelString, xmString,
                 NULL );

   XmStringFree(xmString);

}

void TracerApplication::CreateScaleToggles(Widget Parent)
{
   Arg args[32];
   Cardinal n =0;

   register int i;
   TransportMech* TransMech;

   fmScaleRadioBox = XtVaCreateWidget("fmScaleRadioBox",
                             xmFrameWidgetClass,    Parent,
                             XmNtopAttachment,      XmATTACH_WIDGET,
                             XmNtopWidget,          frWindowArea,
                             XmNtopOffset,          1,
                             XmNbottomAttachment,   XmATTACH_FORM,
                             XmNbottomOffset,       1,
                             XmNrightAttachment,    XmATTACH_FORM,
                             XmNrightOffset,        2,
                             XmNleftAttachment,     XmATTACH_FORM,
                             XmNleftOffset,         315,
                             NULL);

   (void) XtVaCreateManagedWidget( "Scale Selection", xmLabelWidgetClass,
                         fmScaleRadioBox,
                         XmNchildType, XmFRAME_TITLE_CHILD,
                         XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                         XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
                         NULL );

   XtSetArg(args[n],XmNorientation, XmHORIZONTAL); n++;
   XtSetArg(args[n],XmNentryAlignment, XmALIGNMENT_CENTER); n++;
   XtSetArg(args[n],XmNspacing,     1  ); n++;

   ScaleRadioBox = XmCreateRadioBox(fmScaleRadioBox,"ScaleRadioBox", args, n);

   if (ScaleRadioBox == NULL)
   {
      printf("(ScaleRadioBox == NULL)\n");
      exit (-1);
   }
  
   for( i=0; i < 12; i++ )
   {
      TransMech = (TransportMech*)malloc(sizeof(struct TransportMech));

      TransMech->pthis = (TracerApplication*)this;
      TransMech->iValue = ScaleArray[i];

      bnScale[i] = CreateToggleButton(ScaleRadioBox,ScaleStrings[i],
                 (XtCallbackProc)TracerApplication::TimeScaleProc,
                 (XtPointer) TransMech);
     


//      XtVaSetValues(bnScale[i],   XtVaTypedArg, XmNbackground,
//                                  XmRString, "grey", 5 , NULL);
   }
   XtManageChild(ScaleRadioBox);
   XtManageChild(fmScaleRadioBox);

}

void TracerApplication::CreateZoomButtons(Widget Parent)
{
   Widget rcZoomSpinBox;

   frZoomSpinBox = XtVaCreateWidget("frZoomSpinBox",
                             xmFrameWidgetClass,
                             Parent,
                             XmNtopAttachment,        XmATTACH_WIDGET,
                             XmNtopWidget,            frWindowArea,
                             XmNtopOffset,            1,
                             XmNleftAttachment,       XmATTACH_FORM,
                             XmNleftOffset,           218,
                             XmNbottomAttachment,     XmATTACH_FORM,
                             XmNbottomOffset,         1,
                             NULL);


   (void) XtVaCreateManagedWidget( "Zoom", xmLabelWidgetClass,
                         frZoomSpinBox,
                         XmNchildType, XmFRAME_TITLE_CHILD,
                         XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                         XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
                         NULL );

   rcZoomSpinBox = XtVaCreateManagedWidget("rcZoomSpinBox",
                           xmRowColumnWidgetClass,
                           frZoomSpinBox,
                           XmNorientation,       XmHORIZONTAL,
                           NULL );

   bnZoomIn = CreateArrowButton(rcZoomSpinBox,"Zoom In",
                &TracerApplication::cbZoomIn,
                (XtPointer)this);

/*   XtAddCallback( bnZoomIn, XmNarmCallback,
                     &TracerApplication::cbZoomIn, (XtPointer)this );
   XtAddCallback( bnZoomIn, XmNdisarmCallback,
                     &TracerApplication::cbZoomIn, (XtPointer)this );*/

   XtVaSetValues(bnZoomIn,
                 XmNarrowDirection, XmARROW_UP,
                 NULL);

   bnZoomOut = CreateArrowButton(rcZoomSpinBox,"Zoom Out",
                 &TracerApplication::cbZoomOut,
                 (XtPointer)this);

/*   XtAddCallback( bnZoomOut, XmNarmCallback,
                     &TracerApplication::cbZoomOut, (XtPointer)this );
   XtAddCallback( bnZoomOut, XmNdisarmCallback,
                     &TracerApplication::cbZoomOut, (XtPointer)this );*/

   XtVaSetValues(bnZoomOut,
                 XmNarrowDirection, XmARROW_DOWN,
                 NULL);

   tbZoomFactor = XtVaCreateManagedWidget("tbMessage",
                                       xmTextWidgetClass, rcZoomSpinBox,
                                       XmNmarginHeight, 2,
                                       XmNmaxLength, 2,
                                       XmNcolumns,   2,
                                       NULL);

   XmTextSetString(tbZoomFactor,"2");

   XtManageChild(frZoomSpinBox);
}

void TracerApplication::CreateScaleArea(Widget Parent)
{
   XmString xmString;
   Widget fmScaleDisplay;
   frScaleDisplay = XtVaCreateWidget("frScaleDisplay",
                             xmFrameWidgetClass,
                             Parent,
                             XmNtopAttachment,        XmATTACH_WIDGET,
                             XmNtopWidget,            frWindowArea,
                             XmNtopOffset,            1,
                             XmNleftAttachment,       XmATTACH_FORM,
                             XmNleftOffset,           2,
                             XmNbottomAttachment,     XmATTACH_FORM,
                             XmNbottomOffset,         1,
                             NULL);

   xmString = XmStringCreateSimple("Current Scale");

   (void) XtVaCreateManagedWidget( "ScaleDisplay", xmLabelWidgetClass,
                         frScaleDisplay,
                         XmNlabelString, xmString,
                         XmNchildType, XmFRAME_TITLE_CHILD,
                         XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                         XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
                         NULL );

   XmStringFree(xmString);

   fmScaleDisplay = XtVaCreateManagedWidget("fmScaleDisplay",
                                            xmFormWidgetClass,frScaleDisplay,
                                            XmNresizePolicy,  XmRESIZE_NONE,
                                            XmNwidth,         210,
                                            NULL);

   xmString = XmStringCreateSimple("---");
   lbScaleValue = XtVaCreateManagedWidget( "lbScaleValue",
                         xmLabelWidgetClass,
                         fmScaleDisplay,
                         XmNlabelString, xmString,
                         XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
                         XmNchildVerticalAlignment, XmALIGNMENT_BASELINE_BOTTOM,
                         XmNleftAttachment,   XmATTACH_FORM,
                         XmNrightAttachment,   XmATTACH_FORM,
                         XmNtopAttachment,    XmATTACH_FORM,
                         NULL );

   XmStringFree(xmString);

   XtManageChild(frScaleDisplay);

}

void TracerApplication::CreateMessageArea(Widget Parent)
{
  Arg args[16];
  int n =0;
  
   frMessageWindow = XtVaCreateWidget("frMessageWindow",
                             xmRowColumnWidgetClass,
                             Parent,
                             XmNorientation, XmHORIZONTAL,
                             XmNpacking,     XmPACK_NONE,
                             NULL);

   XtSetArg( args[n], XmNeditable,              False);n++;
   XtSetArg( args[n], XmNwordWrap,              True);n++;
   XtSetArg( args[n], XmNeditMode,              XmMULTI_LINE_EDIT);n++;
   XtSetArg( args[n], XmNnumColumns,            100);n++;
   XtSetArg( args[n], XmNwidth,                 1000);n++;
   XtSetArg( args[n], XmNscrollHorizontal,      False);n++;
   XtSetArg( args[n], XmNscrollVertical,        True);n++;
   XtSetArg( args[n], XmNcursorPositionVisible, False);n++;

   tbMessage = XmCreateScrolledText(frMessageWindow,"tbMessage", args, n);

   XtManageChild(frMessageWindow);
   XtManageChild(tbMessage);

}

/*******************************************************************************
*
* Function: ParseCommandLine
*
*
* Purpose:
*
*******************************************************************************/

void TracerApplication::ParseCommandLine( int argc, char **argv )
{
   int i;
  

   int quit = 0;
   i = 1;

   StartDirectory[0] = 0x00;
   RegExpPath[0]     = 0x00;

   while( i < argc && !quit)
   {
      if ( argv[i][0] == '-' )
      {
         switch( argv[i][1] )
         {
            case 'f' :
               if ( i + 1 < argc )
               {
                  strcpy( StartDirectory,argv[++i] );
               }

               break;

            case 'l' :
               if ( i + 1 < argc )
               {
                  strcpy( RegExpPath,argv[++i] );
               }
     //          if (RegExpPath[strlen(RegExpPath)-1] != '/')
    //              strcat(RegExpPath,"/");

               break;


            default:
               break;
         }
         i++;

      }
      else
      {
         quit = 1;
      }
   }

   printf("Start Trace File:[%s]\n",StartDirectory);
   printf("RegExpFile      :[%s]\n",RegExpPath);
}

/*******************************************************************************
*
* Function: run
*
*
* Purpose:
*
*******************************************************************************/

void TracerApplication::run()
{
   XtRealizeWidget(top);

   // Create a window after all thge widgets have been realized
   CurrentWindow = new TracerWindow();

   if ((strcmp(RegExpPath,"") != 0) && (strcmp(StartDirectory,"") != 0))
   {
      // Auto-load group
      CurrentWindow->AutoLoadGroups(StartDirectory, RegExpPath);

   }
   
   CustomLoop(context);
}

void TracerApplication::CustomLoop(XtAppContext context)
{
   XEvent event;
   for (;;)
   {
      XtAppNextEvent(context,&event);
      XtDispatchEvent(&event);
   }
}

/*******************************************************************************
*
* Function: cbZoomDialog
*
*
* Purpose:
*
*******************************************************************************/

void TracerApplication::cbZoomDialog(Widget w, XtPointer client_data,
                                   XtPointer call_data )
{
/*
   Arg args[20];
   int n =0, index;
   Widget d;

   XtSetArg( args[n],  XmNallowResize, True ); n++;
   XtSetArg( args[n],  XmNwidth,   300 ); n++;
   XtSetArg( args[n],  XmNheight,  150 ); n++;
   XtSetArg( args[n],  XmNdialogStyle,   XmDIALOG_FULL_APPLICATION_MODAL ); n++;

   d = XmCreateFormDialog( form0,  "Scale", args, n );

   XtVaSetValues( mu_PixelOption, XmNmenuHistory, bn_Zoom[ gZoomIndex], NULL );

   guiDrawZoomControl(d);

   XtManageChild( d );
*/
}

/*******************************************************************************
*
* Function: cbNewWindow
*
*
* Purpose:
*
*******************************************************************************/

void TracerApplication::cbNewWindow(Widget w, XtPointer client_data,
                                 XtPointer call_data )
{
   TracerApplication *App = ( TracerApplication *)client_data;

   // Check whether a window is currently in use
   if (App->CurrentWindow != NULL)
   {
      // Remove the current window
      delete (App->CurrentWindow);
   }

   App->CurrentWindow = new TracerWindow();
}

/*******************************************************************************
*
* Function: cbExitFile
*
*
* Purpose:
*
*******************************************************************************/

void TracerApplication::cbExitFile(Widget w, XtPointer client_data,
                                 XtPointer call_data )
{
   TracerApplication *App = ( TracerApplication *)client_data;

   delete App;

   exit (0);
}

/*******************************************************************************
*
* Function: cbOpenFile
*
*
* Purpose:
*
*******************************************************************************/

void TracerApplication::cbOpenFile(Widget w, XtPointer client_data,
                                 XtPointer call_data )
{
/*
   char *s;
 
   s =  ( char *)client_data;

      CreateFileBrowser(mainW, 0);
*/
}

/*******************************************************************************
*
* Function: cbSaveFile
*
*
* Purpose:
*
*******************************************************************************/

void TracerApplication::cbSaveFile(Widget w, XtPointer client_data,
                                 XtPointer call_data )
{
/*
      if ( gTraceStorageFile[0] == 0x00 )
        CreateFileBrowser(mainW,1);
      else
        SaveTraceDetails(gTraceStorageFile);
*/
}

/*******************************************************************************
*
* Function: cbSaveAsFile
*
*
* Purpose:
*
*******************************************************************************/

void TracerApplication::cbSaveAsFile(Widget w, XtPointer client_data,
                                 XtPointer call_data )
{
/*
   char *s;

   s =  ( char *)client_data;

      CreateFileBrowser(mainW, 1);
*/
}

/*******************************************************************************
*
* Function: cbGrid
*
*
* Purpose:
*
*******************************************************************************/

void TracerApplication::cbGrid( Widget w, XtPointer client_data, XtPointer call_data )
{
/*
  int ret;

  gShowGrid ^= 1;

  SetMidTime(-1);
*/
}

/*******************************************************************************
*
* Function: cbCreatePixelDialog
*
*
* Purpose:
*
*******************************************************************************/

void TracerApplication::cbCreatePixelDialog(Widget w, XtPointer client_data,
                                         XtPointer call_data )
{
/*
   Arg args[20];
   int n =0, index;
   Widget d;

   XtSetArg( args[n],  XmNallowResize, True ); n++;
   XtSetArg( args[n],  XmNwidth,   350 ); n++;
   XtSetArg( args[n],  XmNheight,  150 ); n++;
   XtSetArg( args[n],  XmNdialogStyle,   XmDIALOG_FULL_APPLICATION_MODAL ); n++;

   d = XmCreateFormDialog( form0,  "Scale", args, n );

   XtVaSetValues( mu_PixelOption, XmNmenuHistory, bn_Pixel[ gPixelIndex], NULL )
;
   XtVaSetValues( mu_StepOption,  XmNmenuHistory, bn_Step[gStepIndex], NULL );

   guiDrawPlotControl(d);

   XtManageChild( d );
*/

}

/*******************************************************************************
*
* Function: cbCreateTrace
*
*
* Purpose:
*
*******************************************************************************/

void TracerApplication::cbCreateTrace(Widget w,XtPointer client_data,
                            XtPointer call_data)
{
   TracerApplication *App = ( TracerApplication *)client_data;

   LoadGroup *LoadGroupDialog;

   LoadGroupDialog = new LoadGroup(App->CurrentWindow);

}

/*******************************************************************************
*
* Function: cbSaveAllSets
*
*
* Purpose:
*
*******************************************************************************/

void TracerApplication::cbSaveAllSets(Widget w,XtPointer client_data,
                            XtPointer call_data)
{
   TracerApplication *App = ( TracerApplication *)client_data;

   if (App->CurrentWindow == NULL)
      return;

   WindowGroupSaveDialog  *SaveDialog = 
                                  new WindowGroupSaveDialog(App->CurrentWindow);

}

/*******************************************************************************
*
* Function: TimeScaleProc
*
*
* Purpose:
*
*******************************************************************************/
void TracerApplication::TimeScaleProc( Widget w, XtPointer client_data,
                                       XtPointer call_data )
{
   XmToggleButtonCallbackStruct* cbs = (XmToggleButtonCallbackStruct*)(call_data);

   TransportMech *TransMechanism = (TransportMech*)client_data;

   TracerApplication *Appli = (TracerApplication*)(TransMechanism->pthis);

   if (w == Appli->bnScale[USER_DEFINED])
   {
      return;
   }
   else
   {
      if (cbs->reason == XmCR_ARM)
        Appli->SetTimeScale(  TransMechanism->iValue);
   }
}

/*******************************************************************************
*
* Function: SetTimeScale
*
*
* Purpose:
*
*******************************************************************************/

void TracerApplication::SetTimeScale(int NewScale)
{

   int i;
   long long factor = 1;

   if (CurrentWindow == NULL)
      return;

   if ( NewScale != USER_DEFINED )
   {
      CurrentWindow->UserDefFlag = False;
      for ( i =0; i < NewScale; i++ )
      {
         factor  = factor * 10;
      }

      CurrentWindow->Scale = factor;
   }
   else
   {
      // loop to deselect other values
      for (i = 0; i < 11; i++)
      {
         if (XmToggleButtonGetState(bnScale[i]) == TRUE)
            XmToggleButtonSetState(bnScale[i],FALSE,FALSE);
      }

      XmToggleButtonSetState(bnScale[USER_DEFINED],TRUE,FALSE);
      CurrentWindow->UserDefFlag = True;
   }

   CurrentWindow->SetMidTime(FULL_REDRAW);

}

/*******************************************************************************
*
* Function: cbZoomIn
*
*
* Purpose:
*
*******************************************************************************/
void TracerApplication::cbZoomIn( Widget w, XtPointer client_data,
                                       XtPointer call_data )
{
  XmPushButtonCallbackStruct* cbs = (XmPushButtonCallbackStruct*)(call_data);

  TracerApplication *Appli = (TracerApplication*)client_data;

  int  ZoomFactor = atoi(XmTextGetString(Appli->tbZoomFactor));
  double dZoomFactor;

  // Check we have a current window

  if (Appli->CurrentWindow == NULL)
     return;

  dZoomFactor = pow((double)ZoomFactor,-1);

  Appli->CurrentWindow->UserDefFlag = True;


  // Check the new scale does not exceed the minimum limit
  if( (Appli->CurrentWindow->Scale * dZoomFactor) < 1)
  {
     Appli->CurrentWindow->Scale = 1;
  }
  else
  {
     Appli->CurrentWindow->Scale *= dZoomFactor;
  }

  Appli->SetTimeScale(USER_DEFINED);
}

/*******************************************************************************
*
* Function: ZoomOutWorkProc
*
*
* Purpose:
*
*******************************************************************************/
Boolean TracerApplication::ZoomOutWorkProc(  XtPointer client_data)
{

  TracerApplication *Appli = (TracerApplication*)client_data;

  int  ZoomFactor = atoi(XmTextGetString(Appli->tbZoomFactor));

  printf("Inside TracerApplication::ZoomOutWorkProc\n");
  
  if (Appli->CurrentWindow == NULL)
     return FALSE;

  Appli->CurrentWindow->UserDefFlag = True;

  // Check the new scale does not exceed the minimum limit
  Appli->CurrentWindow->Scale *= ZoomFactor;


  Appli->SetTimeScale(USER_DEFINED);

  Appli->ZoomProcActive = False;

  return FALSE;
}

/*******************************************************************************
*
* Function: cbZoomOut
*
*
* Purpose:
*
*******************************************************************************/
void TracerApplication::cbZoomOut( Widget w, XtPointer client_data,
                           XtPointer call_data )
{
   TracerApplication* TAppli = (TracerApplication*)client_data;

   static XtIntervalId TimerId;
   XmArrowButtonCallbackStruct *cbs = ( XmArrowButtonCallbackStruct *)call_data;

   printf("Inside TracerApplication::cbZoomOut\n");
   printf("cbs->reason:[%d]\n",cbs->reason);

   ZoomOutWorkProc(client_data);

   if ( cbs->reason == XmCR_ARM )
   {
      printf("Arming\n");
      ZoomOutWorkProc(client_data);
/*      TAppli->WorkId = XtAppAddWorkProc(TAppli->context,
                                        &TracerApplication::ZoomOutWorkProc,
                                        client_data);

      TAppli->ZoomProcActive = False;
      TAppli->ZoomOutWorkProc( client_data );
      TimerId = XtAppAddTimeOut( TAppli->context,
                                 200, 
                                 &TracerApplication::ZoomOutTimerProc,
                                 client_data );

   }

   if ( cbs->reason == XmCR_DISARM )
   {
      printf("Disarming\n");
      XtRemoveWorkProc(TAppli->WorkId);


      XtRemoveTimeOut(TimerId );

      if ( TAppli->ZoomProcActive == True )
      {
         XtRemoveWorkProc( TAppli->WorkId );
      }
*/
   }
}

void TracerApplication::ZoomOutTimerProc(XtPointer client_data,
                                         XtIntervalId *timerId )
{
   TracerApplication* TAppli = (TracerApplication*)client_data;
   printf("Inside ZoomOutTimerProc\n");
   TAppli->WorkId = XtAppAddWorkProc(TAppli->context,
                              (XtWorkProc)&TracerApplication::ZoomOutWorkProc,
                              client_data );
   TAppli->ZoomProcActive = True;
}

void TracerApplication::WarpClickUp( Widget w, XtPointer client_data,
                                                           XtPointer call_data )
{
   TracerApplication *TAppli = ( TracerApplication* )client_data;
   LTime NextTime;
   
   if (TAppli->CurrentWindow->CurrentFocusElement != NULL)
   {
      NextTime =  TAppli->CurrentWindow->CurrentFocusElement->GetNextTime(1);

      if ( NextTime != -1 )
      {
         TAppli->CurrentWindow->MidTime = NextTime;
         TAppli->CurrentWindow->SetMidTime(FULL_REDRAW);
      }
   }
   else
   {
      TAppli->SendMessageText("\nThere are no focus items");
   }
}

void TracerApplication::WarpClickDown( Widget w, XtPointer client_data,
                                                           XtPointer call_data )
{
   TracerApplication *TAppli = ( TracerApplication* )client_data;
   LTime NextTime;

   if (TAppli->CurrentWindow->CurrentFocusElement != NULL)
   {
      NextTime =  TAppli->CurrentWindow->CurrentFocusElement->GetNextTime(-1);

      if ( NextTime != -1 )
      {
         TAppli->CurrentWindow->MidTime = NextTime;
         TAppli->CurrentWindow->SetMidTime(FULL_REDRAW);
      }
   }
   else
   {
      TAppli->SendMessageText("\nThere are no focus items");
   }

}

void TracerApplication::SendMessageText(char* chMessage)
{
   XmTextPosition   TextPosition;

   XtVaGetValues(tbMessage,
                 XmNcursorPosition, &TextPosition,
                 NULL);

   XmTextInsert(tbMessage,TextPosition,chMessage);
}

void TracerApplication::cbExportData(Widget w,XtPointer client_data,
                            XtPointer call_data)
{
  TracerApplication *App = ( TracerApplication *)client_data;
   XmString  xmString;

  if (App->CurrentWindow == NULL)
    return;


  if (App->ExportingDataFlag == FALSE)
  {
    //
    App->SendMessageText("Select range to export using Button 3\n");
    App->ExportingDataFlag = TRUE ;

    xmString = XmStringCreateSimple("Cancel Export");

    XtVaSetValues(App->ExportButton,
                 XmNlabelString, xmString,
                 NULL );

    XmStringFree(xmString);

  }

  else
  {
    App->ResetExportValues();
  }

}

void TracerApplication::ResetExportValues()
{
  XmString  xmString;

  SendMessageText("Ready\n");
  ExportingDataFlag = FALSE ;
 
  xmString = XmStringCreateSimple("Export Data");

  XtVaSetValues(ExportButton,
                  XmNlabelString, xmString,
                  NULL );

  XmStringFree(xmString);
}

