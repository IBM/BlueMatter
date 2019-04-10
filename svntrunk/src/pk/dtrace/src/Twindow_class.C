// template <class Element>
// class IACollection;
#include <Twindow_class.hpp>
#include <Ttimer_class.hpp>

#include <pk/fxlogger.hpp>

#ifndef PKFXLOG_TRACER
#define PKFXLOG_TRACER (1)
#endif

TracerApplication* TracerWindow::GetApplication()
{
   return TracerApplication::GetApp();
}

void  TracerWindow::SendMessageText(char* chMessage)
{
   GetApplication()->SendMessageText(chMessage);
}

XtAppContext* TracerWindow::GetApplicationContext()
{
   return &(GetApplication()->context);
}

Widget TracerWindow::GetApplicationShell()
{
   return GetApplication()->top;
}

Widget TracerWindow::GetApplicationTopWindow()
{
   return GetApplication()->MainWinForm;
}

TracerWindow::~TracerWindow( )
{
   TraceGroup* TempPtr;

   // Clear the groups
   while ( !TraceListSeq.empty() )
   {
      TempPtr = TraceListSeq.back();

      TraceListSeq.pop_back();
      delete TempPtr;
   }

   XFreeGC( XtDisplay(da),invertedGC);
   XFreeGC( XtDisplay(form0),formGC);
   XFreeGC( XtDisplay(form0),timeGC);
   XFreeGC( XtDisplay(da),midGC);

   // Destroy the main widget
   XtDestroyWidget(form0);

}

TracerWindow::TracerWindow()
{
  BegLogLine(PKFXLOG_TRACER)
    << "TracerWindow::TracerWindow"
    << EndLogLine ;
  XGCValues      values;
  XColor         colour, exact;
  XFontStruct      *font;
  long long WindowSpan;
  Widget       ParentFrame;
  Scale = 10000000;
  ScaleIndex = 7;
  CurrentFocusElement = NULL;
  MidTime = 0;
  MinTime = 0;

  WindowStartTime = -1;
  WindowEndTime = -1;

  VirtualHeight = 0;

  WindowSpan = Scale;
  WindowSpan *= daWidth;

//  CurrentLeftTime = (MidTime - 1) - (WindowSpan / 2 );
//  CurrentRightTime = (MidTime - 1) + (WindowSpan / 2 );

  CreateGroupDialogBox = NULL;

  strcpy(StartDirectory,GetApplication()->StartDirectory); 
  strcpy(CurrentPrevRegExp,"*");
  strcpy(CurrentPostRegExp,"*");
  BegLogLine(PKFXLOG_TRACER)
    << "TracerWindow::TracerWindow StartDirectory=" << StartDirectory
    << EndLogLine ;
  CurrentSpacing = 4;
  CurrentInvertedValue = FALSE;
  CurrentContourValue  = FALSE;

  strcpy(CurrentColourSelection,"white");
  CurrentSelectedStyle = FLAT_LINE_STYLE;
  XmToggleButtonSetState(TracerApplication::GetApp()->bnScale[ScaleIndex],
                                                TRUE, FALSE);
  TimeMark = NULL;
  UserDefFlag = FALSE;

  ParentFrame = TracerApplication::GetApp()->frWindowArea;

  form0 = XtVaCreateManagedWidget("form", xmFormWidgetClass,
                             ParentFrame,
                             XmNtopAttachment,    XmATTACH_WIDGET,
                             XmNtopWidget,        ParentFrame,
                             XmNtopOffset,        2,
                             XmNleftAttachment,   XmATTACH_WIDGET,
                             XmNleftWidget,       ParentFrame,
                             XmNleftOffset,       2,
                             XmNrightAttachment,  XmATTACH_WIDGET,
                             XmNrightWidget,      ParentFrame,
                             XmNrightOffset,      2,
                             XmNbottomAttachment, XmATTACH_WIDGET,
                             XmNbottomWidget,     ParentFrame,
                             XmNbottomOffset,     2,
                             NULL);

  /* Create the drawing area widget that will display the traces */
   da = XtVaCreateManagedWidget("da", xmDrawingAreaWidgetClass,
                                form0,
                                XmNleftAttachment,   XmATTACH_WIDGET,
                                XmNleftWidget,       form0,
                                XmNleftOffset,       daLeft,
                                XmNtopAttachment,    XmATTACH_FORM,
                                XmNtopOffset,        daTop,
                                XmNwidth,            daWidth,
                                XmNheight,           daHeight,
                                XmNresizePolicy,     XmNONE,
                                NULL );

   /* Add Event handlers for the drawing area widget */
   XtAddEventHandler( da, ButtonMotionMask , False,
                      (XtEventHandler)&TracerWindow::button_motion_handler,
                      (XtPointer)this);
   XtAddEventHandler( da, ButtonPressMask,   False,
                      (XtEventHandler)&TracerWindow::button_press_handler,
                      (XtPointer)this);
   XtAddEventHandler( da, ButtonReleaseMask, False,
                      (XtEventHandler)&TracerWindow::button_release_handler,
                      (XtPointer)this);

   /* Add callbacks for the drawing area widget */
   XtAddCallback( da, XmNexposeCallback, &TracerWindow::drawCB,
                  (XtPointer)this  );
   XtAddCallback( da, XmNinputCallback,  &TracerWindow::InputCB,
                  (XtPointer)this  );


   daHorizontalScrollBar = XtVaCreateManagedWidget("daHorizontalScrollBar",
                                         xmScrollBarWidgetClass,
                                         form0,
                                         XmNorientation,      XmHORIZONTAL,
                                         XmNminimum,          0,
                                         XmNmaximum,          100,
                                         XmNtopAttachment,    XmATTACH_WIDGET,
                                         XmNtopWidget,        da,
                                         XmNtopOffset,        0,
                                         XmNleftAttachment,   XmATTACH_WIDGET,
                                         XmNleftOffset,       daLeft,
                                         XmNwidth,            daWidth,
                                         XmNshowArrows,       TRUE,
                                         NULL);

   XtAddCallback( daHorizontalScrollBar,
                  XmNdecrementCallback,
                  (XtCallbackProc)&TracerWindow::cbHorScrollBar,
                  (XtPointer)this );

   XtAddCallback( daHorizontalScrollBar,
                  XmNincrementCallback,
                  (XtCallbackProc)&TracerWindow::cbHorScrollBar,
                  (XtPointer)this );

   XtAddCallback( daHorizontalScrollBar,
                  XmNpageDecrementCallback,
                  (XtCallbackProc)&TracerWindow::cbHorScrollBar,
                  (XtPointer)this );

   XtAddCallback( daHorizontalScrollBar,
                  XmNpageIncrementCallback,
                  (XtCallbackProc)&TracerWindow::cbHorScrollBar,
                  (XtPointer)this );


   XtAddCallback( daHorizontalScrollBar,
                  XmNdragCallback,
                  (XtCallbackProc)&TracerWindow::cbHorScrollBar,
                  (XtPointer)this );

   XtAddCallback( daHorizontalScrollBar,
                  XmNtoBottomCallback,
                  (XtCallbackProc)&TracerWindow::cbHorScrollBar,
                  (XtPointer)this );

   XtAddCallback( daHorizontalScrollBar,
                  XmNtoTopCallback,
                  (XtCallbackProc)&TracerWindow::cbHorScrollBar,
                  (XtPointer)this );

   XtAddCallback( daHorizontalScrollBar,
                  XmNvalueChangedCallback,
                  (XtCallbackProc)&TracerWindow::cbHorScrollBar,
                  (XtPointer)this );

   formTime = XtVaCreateManagedWidget("formTime", xmFormWidgetClass,
                             form0,
                             XmNtopAttachment,    XmATTACH_FORM,
                             XmNtopOffset,        0,
                             XmNrightAttachment,  XmATTACH_OPPOSITE_WIDGET,
                             XmNrightWidget,      da,
                             XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET,
                             XmNleftWidget,       da,
                             XmNbottomAttachment, XmATTACH_WIDGET,
                             XmNbottomWidget,     da,
                             XmNallowResize,      False,
                             NULL);


   formScale = XtVaCreateManagedWidget("formScale", xmFormWidgetClass,  form0,
                             XmNtopAttachment,    XmATTACH_WIDGET,
                             XmNtopWidget,        daHorizontalScrollBar,
                             XmNrightAttachment,  XmATTACH_OPPOSITE_WIDGET,
                             XmNrightWidget,      da,
                             XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET,
                             XmNleftWidget,       da,
                             XmNbottomAttachment, XmATTACH_FORM,
                             XmNallowResize,      False,
                             NULL);

   formVirtualMap = XtVaCreateManagedWidget("formVirtualMap", xmFormWidgetClass,
                                            form0,
                             XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET,
                             XmNtopWidget,        da,
                             XmNtopOffset,        0,
                             XmNwidth,            8,
                             XmNleftAttachment,   XmATTACH_WIDGET,
                             XmNleftWidget,       da,
                             XmNleftOffset,       0,
                             XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
                             XmNbottomWidget,     da,
                             XmNbottomOffset,     0,
                             NULL);

   daVerticalScrollBar = XtVaCreateManagedWidget("daVerticalScrollBar",
                                  xmScrollBarWidgetClass,
                                  form0,
                                  XmNorientation,      XmVERTICAL,
                                  XmNminimum,          0,
                                  XmNmaximum,          vdaHeight,
                                  XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET,
                                  XmNtopWidget,        da,
                                  XmNtopOffset,        0,
                                  XmNleftAttachment,   XmATTACH_WIDGET,
                                  XmNleftWidget,       formVirtualMap,
                                  XmNleftOffset,       0,
                                  XmNrightAttachment,  XmATTACH_FORM,
                                  XmNrightOffset,      0,
                                  XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
                                  XmNbottomWidget,     da,
                                  XmNbottomOffset,     0,
                                  XmNshowArrows,       TRUE,
                                  XmNsliderSize,       daHeight,
                                  NULL);

   XtAddCallback( daVerticalScrollBar,
                  XmNdecrementCallback,
                  (XtCallbackProc)&TracerWindow::cbVertScrollBar,
                  (XtPointer)this );

   XtAddCallback( daVerticalScrollBar,
                  XmNincrementCallback,
                  (XtCallbackProc)&TracerWindow::cbVertScrollBar,
                  (XtPointer)this );

   XtAddCallback( daVerticalScrollBar,
                  XmNpageDecrementCallback,
                  (XtCallbackProc)&TracerWindow::cbVertScrollBar,
                  (XtPointer)this );

   XtAddCallback( daVerticalScrollBar,
                  XmNpageIncrementCallback,
                  (XtCallbackProc)&TracerWindow::cbVertScrollBar,
                  (XtPointer)this );

   XtAddCallback( daVerticalScrollBar,
                  XmNdragCallback,
                  (XtCallbackProc)&TracerWindow::cbVertScrollBar,
                  (XtPointer)this );

   XtAddCallback( daVerticalScrollBar,
                  XmNtoBottomCallback,
                  (XtCallbackProc)&TracerWindow::cbVertScrollBar,
                  (XtPointer)this );

   XtAddCallback( daVerticalScrollBar,
                  XmNtoTopCallback,
                  (XtCallbackProc)&TracerWindow::cbVertScrollBar,
                  (XtPointer)this );

   XtAddCallback( daVerticalScrollBar,
                  XmNvalueChangedCallback,
                  (XtCallbackProc)&TracerWindow::cbVertScrollBar,
                  (XtPointer)this );

   formLabel = XtVaCreateManagedWidget("formLabel", xmFormWidgetClass,
                             form0,
                             XmNtopAttachment,    XmATTACH_FORM,
                             XmNtopOffset,        daTop,
                             XmNrightAttachment,  XmATTACH_WIDGET,
                             XmNrightWidget,      da,
                             XmNrightOffset,      0,
                             XmNleftAttachment,   XmATTACH_FORM,
                             XmNleftOffset,       0,
                             XmNresize,           False,
                             XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
                             XmNbottomWidget,     da,
                             XmNbottomOffset,     0,
                             NULL);

  /* Set the middle label Widget */
  MidLabel = XtVaCreateManagedWidget( "MidLabel",
                              xmLabelWidgetClass,
                              formScale,
                              XmNtopAttachment,    XmATTACH_FORM,
                              XmNtopOffset,        20,
                              XmNleftAttachment,   XmATTACH_FORM,
                              XmNleftOffset,       (daWidth / 2) - 50,
                              XmNmarginTop,        1,
                              XmNmarginBottom,     1,
                              XmNmarginWidth,      1,
                              XmNmarginHeight,     1,
                              NULL );

  /* Set the middle label Widget */
  XtVaSetValues( da,
                 XtVaTypedArg, XmNbackground,
                 XmRString, "black", 6,
                 NULL);

  /* Add event handlers for the formLabel Widget (Left side form) */


  XtAddEventHandler(formLabel,EnterWindowMask,False,
                    (XtEventHandler)&TracerWindow::form_entered_handler,
                    (XtPointer)this );

  XtAddEventHandler(formLabel,LeaveWindowMask,False,
                    (XtEventHandler)&TracerWindow::form_exited_handler,
                    (XtPointer)this );


  // Constrcut all the Graphic Contextes
  values.line_width = 1;

  values.function = GXxor;
  values.line_width = 1;

  invertedGC = XCreateGC (XtDisplay(da),
                     XtWindow(da),
                     GCBackground | GCForeground | GCFunction | GCLineWidth,
                     &values );

  if (!XAllocNamedColor(XtDisplay(da), DefaultColormapOfScreen(XtScreen(da)),
                        "white", &colour, &exact))
  {
    fprintf( stderr, "Cannot allocate colour\n" );
    colour.pixel = WhitePixelOfScreen( XtScreen(da));
  }

  XSetForeground(XtDisplay(da), invertedGC, colour.pixel);


  font = XLoadQueryFont( XtDisplay(GetApplicationShell()), "7x14" );

  ConstructGC( &formGC, form0, 1 ,
      (char *) "yellow", (char *) "lightslategrey", (char *) "6x10", NULL , NULL);

  ConstructGC( &timeGC, form0, 1 ,
      (char *) "green", (char *) "lightslategrey", (char *) "8x13", NULL , NULL);

 
  ConstructGC( &midGC, da, 1 , (char *) "yellow", NULL, NULL, NULL , NULL);

  guiDrawScaleClicks();
  BegLogLine(PKFXLOG_TRACER)
    << "TracerWindow::TracerWindow completes"
    << EndLogLine ;

}



XtEventHandler TracerWindow::button_motion_handler(Widget w, XtPointer p,
                                     XPointerMovedEvent * event)
{

  TracerWindow *TWindow = (TracerWindow*)p;
  int x;
  int xpos;

  if ( w != TWindow->da )
     return NULL;

  x = event->x;

  switch  ( TWindow->button)
  {
     case 3 :
     case 2 :
     case BUTTON_QUERY :


        if ( x >= 0 && x <= daWidth  )
        {
           TWindow->GetScanTime(x, &(TWindow->ScanTime));

          if ( TWindow->XOffset != -1 )
          {
              xpos = daWidth - TWindow->XOffset;

              XDrawLine(XtDisplay(TWindow->da), XtWindow(TWindow->da),
                        TWindow->invertedGC, xpos, 0,  xpos, daHeight );

          }
          TWindow->XOffset = daWidth - event->x;

          if ( TWindow->ScanTime < 0 )
          {
              TWindow->XOffset = -1;
          }
          else
          {
              TWindow->XOffset = daWidth - event->x;
              xpos = daWidth - TWindow->XOffset;

              XDrawLine(XtDisplay(TWindow->da), XtWindow(TWindow->da),
                       TWindow->invertedGC, xpos, 0,  xpos, daHeight );

              XClearWindow( XtDisplay(TWindow->formTime),
                            XtWindow(TWindow->formTime) );

              if (TWindow->TimeMark == NULL)
              {
                 TWindow->DisplayFormTime(TWindow->ScanTime,
                  TWindow->button, xpos);
              }
           }

           if (TWindow->TimeMark != NULL)
           {
              TWindow->TimeMark->MoveEndPoint(x);
           }
       }
       break;

       default:
          break;
  }
  return NULL;
}


XtEventHandler TracerWindow::button_press_handler(Widget w, XtPointer p,
                                    XButtonPressedEvent *event)
{
  TracerWindow* TWindow = (TracerWindow*)p;

  int x;
  char ScaleValue[32];
  double index  = log10((double) TWindow->Scale);

  if (TWindow->UserDefFlag == False )
  {
     // Not User defined scale so print the menu value
     strcpy(ScaleValue, ScaleStrings[(int)index]);
  }
  else
  {
     // User defined scale
     strcpy(ScaleValue, "ns");
  }

  TWindow->button = event->button;

  if ( w != TWindow->da )
     return NULL;

  x = event->x;

  switch  (TWindow->button)
  {
     case 3 :
        /* Create New Time Block*/
        TWindow->TimeMark = new TraceTimeMark(TWindow,
                                              x,ScaleValue,
                                              TWindow->Scale);
        break;

     case 2 :
     case BUTTON_QUERY:
        if ( x >= 0 && x <= daWidth  )
        {
           TWindow->GetScanTime(x, &(TWindow->ScanTime));

           if ( TWindow->ScanTime  < 0 )
               TWindow->XOffset = -1;
           else
           {
              int xpos;

              TWindow->XOffset = daWidth - event->x;
              xpos = daWidth - TWindow->XOffset;

              XDrawLine(XtDisplay(TWindow->da), XtWindow(TWindow->da), 
                        TWindow->invertedGC, xpos, 0,  xpos, daHeight );

             /* now interrogate the plotted line for each intersection*/

             TWindow->DisplayFormTime(TWindow->ScanTime, TWindow->button, xpos);

            }
        }
        break;
  }
  return NULL;
}

XtEventHandler TracerWindow::form_entered_handler(Widget w, XtPointer ptr,
                                      XEnterWindowEvent *event)
{
  TracerWindow* TWindow = (TracerWindow*)ptr;
 
  XtAddEventHandler(TWindow->formLabel, PointerMotionMask, True,
                         (XtEventHandler)&TracerWindow::form_motion_handler,
                         (XtPointer)TWindow );

  XtAddEventHandler(TWindow->formLabel, ButtonPressMask, False,
                      (XtEventHandler)&TracerWindow::form_press_handler,
                      (XtPointer)TWindow );

  XtAddEventHandler(TWindow->formLabel, ButtonReleaseMask, False,
                      (XtEventHandler)&TracerWindow::form_release_handler,
                      (XtPointer)TWindow );

    return NULL;
}

XtEventHandler TracerWindow::form_exited_handler(Widget w, XtPointer ptr,
                                      XLeaveWindowEvent *event)
{
   TracerWindow* TWindow = (TracerWindow*)ptr;

   XtRemoveEventHandler(TWindow->formLabel, PointerMotionMask, True,
                         (XtEventHandler)&TracerWindow::form_motion_handler,
                         (XtPointer)TWindow );

  XtRemoveEventHandler(TWindow->formLabel, ButtonPressMask, False,
                      (XtEventHandler)&TracerWindow::form_press_handler,
                      (XtPointer)TWindow );

  XtRemoveEventHandler(TWindow->formLabel, ButtonReleaseMask, False,
                      (XtEventHandler)&TracerWindow::form_release_handler,
                      (XtPointer)TWindow );

    return NULL;

}

XtEventHandler TracerWindow::button_release_handler(Widget w, XtPointer ptr,
                                      XButtonPressedEvent *event)
{
  TracerWindow* TWindow = (TracerWindow*)ptr;

  int x,  xpos;

  x = event->x;

  TWindow->button = 0;             /* button released, so reset this flag */

  switch  ( event->button)
  {
     case 3:
     case 2 :
     case BUTTON_QUERY :

       if ( x >= 0 && x <= daWidth  )
       {
         if ( TWindow->XOffset != -1 )
         {
             xpos = daWidth - TWindow->XOffset;

           if (TWindow->TimeMark == NULL)
           {
              XDrawLine(XtDisplay(TWindow->da), XtWindow(TWindow->da),
                       TWindow->invertedGC, xpos, 0,  xpos, daHeight );
           }

         }
         TWindow->XOffset = -1;

       }

       XClearWindow( XtDisplay( TWindow->formTime ),
                     XtWindow( TWindow->formTime ) );


       if ( event->button == 2 )
          TWindow->SetMidPoint( x );
       else if ( event->button == 3 
                 && TWindow->ScanTime >= 0 
                 && TWindow->TimeMark == NULL)
       {
/*
          for( i=0; i < 10; i++ )
          {
             if ( Marker[i].Num == -1 )
             {
               p = i;
               break;
             }
          }

          Marker[ p ].Label  = CreateLabel( formTime , gNumMarkers , x);
          Marker[ p ].Time   = TWindow->ScanTime;
          Marker[ p ].Scale  = Scale;
          Marker[ p ].Num    = p +1;

          sprintf(str,"%02d", Marker[ p ].Num);
          s = XmStringCreateSimple( str);
          XtVaSetValues( Marker[ p ].Label , XmNlabelString, s, NULL );
          XmStringFree( s );

          gNumMarkers++;
*/
       }

       if (TWindow->TimeMark != NULL)
       {
           if (TracerApplication::GetApp()->ExportingDataFlag == TRUE)
           {
             static ExportDialog* ExportDialogBox1;
             ExportDialogBox1 = new ExportDialog(TWindow->formScale,
                                              TWindow,
                                              TWindow->TimeMark->GetStartTime(),
                                              TWindow->TimeMark->GetEndTime());

             TracerApplication::GetApp()->ResetExportValues();

           }  
           delete (TWindow->TimeMark);
           TWindow->TimeMark = NULL;
       }
       break;

  }
  return (XtEventHandler)(NULL);
}

/*******************************************************************************
*
* Function: drawCB
*
*
* Purpose:
*
*******************************************************************************/
void TracerWindow::drawCB(Widget w, XtPointer client_data, XtPointer call_data)
{
  TracerWindow* TWindow = (TracerWindow*)client_data;

   XmDrawingAreaCallbackStruct *ptr;

   XEvent *event;
  
   static int rendered = False;
  
   ptr = (XmDrawingAreaCallbackStruct  *) call_data;

   event = ptr->event;

   if ( ptr == NULL )
       return;
   
   if ( event->xexpose.count == 0 )
     TWindow->SetMidTime(REFRESH);
}

/*******************************************************************************
*
* Function: InputCB
*
*
* Purpose:
*
*******************************************************************************/
void TracerWindow::InputCB(Widget w, XtPointer client_data, XtPointer call_data)
{
   XmDrawingAreaCallbackStruct *ptr;
   XEvent *event;
  

  
   ptr = (XmDrawingAreaCallbackStruct  *) call_data;

   if ( ptr == NULL )
      return;
  
   event = ptr->event;

}

/*******************************************************************************
*
* Function: form_motion_handler
*
*
* Purpose:  Handler for mouse movement inside a form
*
*******************************************************************************/
XtEventHandler TracerWindow::form_motion_handler(Widget w, XtPointer p,
                                   XPointerMovedEvent *event)
{
   TracerWindow* TWindow = (TracerWindow*)p;

  switch (  TWindow->button )
  {


     case Button2:

           break;
  }
  return NULL;

}

XtEventHandler TracerWindow::form_press_handler(Widget w, XtPointer p,
                                                XButtonPressedEvent *event)
{
  TracerWindow* TWindow = (TracerWindow*)p;
  Position rootx, rooty;

  TWindow->button = event->button;
  
  TWindow->y_pos = (event->y + TWindow->VirtualHeight);

  XtTranslateCoords( w,event->x,event->y, &rootx, &rooty );

  /* Determine which button has been pressed */
  switch (TWindow->button)
  {
    /* Only interested in button 1 presses */
    case Button1:
    {
      /* Display the Directory selection dialog box */

      if (TWindow->CreateGroupDialogBox == NULL)
      {
        TWindow->CreateGroupDialogBox = new CreateSetDialog(TWindow);
      }
      else
      {
        TWindow->CreateGroupDialogBox->UpdateDialog();
        XtManageChild(TWindow->CreateGroupDialogBox->wCreSetDialog);
 
      }

    }
    break;

  }
  return NULL;
}

XtEventHandler TracerWindow::form_release_handler(Widget w, XtPointer p,
                                                XButtonPressedEvent *event)
{
  TracerWindow* TWindow = (TracerWindow*)p;

  TWindow->button = 0;

  return NULL;

}

/*******************************************************************************
*
* Function: guiDrawScaleClicks
*
*
* Purpose:
*
*******************************************************************************/
void TracerWindow::guiDrawScaleClicks()
{
   char  str[256];
   char  Scale_str[256];
   LTime  CurrentBucketTime;
   register int i;
   long long WindowSpan, LeftTime;

   double index  = log10( (double) Scale);

   // Set up the scale values
   if (UserDefFlag == False )
   {
      // Not User defined scale so print the menu value
      strcpy(Scale_str, ScaleStrings[(int)index]);
   }
   else
   {
      // User defined scale
      strcpy(Scale_str, "ns");
   }

   XClearWindow ( XtDisplay( formScale ), XtWindow( formScale ) );
 
   WindowSpan = Scale;
   WindowSpan *= daWidth;
         
   LeftTime =  (long long)(MidTime % Scale );
   LeftTime =  (long long)(MidTime - LeftTime);
   LeftTime =  LeftTime  - (WindowSpan / 2 );

   for( i= FIRST_BUCKET; i < daWidth ; i++)
   {
      CurrentBucketTime = (long long)(LeftTime + (i * Scale));

      if (CurrentBucketTime < 0)
      {
         continue;
      }

      else if (CurrentBucketTime == 0)
      {
            strcpy (str,CreateSecondsFromLTime(CurrentBucketTime));
            XDrawLine( XtDisplay( formScale), XtWindow(formScale), formGC, i,
                     10, i, 0  );

            XDrawImageString( XtDisplay( formScale), XtWindow( formScale ),
                 formGC, i, 20, str, (int)(strlen( str)) );
      }

      else
      {
         /* Show marks every 100 */
         if (i % 100 == 0)
         {
            strcpy (str,CreateSecondsFromLTime(CurrentBucketTime));
            XDrawLine( XtDisplay( formScale), XtWindow(formScale), formGC, i,
                     10, i, 0  );
 
            XDrawImageString( XtDisplay( formScale), XtWindow( formScale ),
                 formGC, i, 20, str, (int)(strlen( str)) );
         }

         /* Show marks every 10 */
         if (i % 10 == 0)
         {
            XDrawLine( XtDisplay( formScale), XtWindow(formScale), formGC, i,
                     3, i, 0  );
         }

         /* Show marks every 50 */
         if (i % 50 == 0)
         {
            XDrawLine( XtDisplay( formScale), XtWindow(formScale), formGC, i,
                     7, i, 0  );
         }


      }    

   }

   /* draw two clicks at the top and bottom */
   XDrawLine( XtDisplay( da), XtWindow(da), midGC, (daWidth/2),
                0, (daWidth/2), 10   );
   XDrawLine( XtDisplay( da), XtWindow(da), midGC, (daWidth/2),
                 daHeight , (daWidth/2), daHeight - 10  );

}

void TracerWindow::SetMidTime(int flag)
{
   XmString XMSPrevFocus;
   XmString XMSPostFocus;
   XmString XMSMidTime;

   char     chPrevFocus[1024];
   char     chPostFocus[1024];
   
   char TempString[512];
   char str[1024];
   long long d;
   long long WindowSpan;

   Cursor Mouse_cursor;

   //    TraceGroupListType::Cursor cursor(TraceListSeq);


   WindowSpan = Scale;
   WindowSpan *= daWidth;

//   CurrentLeftTime = (MidTime - 1) - (WindowSpan / 2 );
//   CurrentRightTime = (MidTime - 1) + (WindowSpan / 2 );

   Mouse_cursor = XCreateFontCursor(XtDisplay( GetApplicationShell() ) ,
                                    XC_gumby);

   XDefineCursor(XtDisplay(GetApplicationShell()),
                 XtWindow(GetApplicationShell()),
                 Mouse_cursor);
   GetApplication()->SendMessageText((char *) "\nDrawing...");

   if ((flag == REFRESH) || (flag == FULL_REDRAW))
   {

      if (flag == FULL_REDRAW)
      {
          
         RecalcScrollBar();
      }

      if (CurrentFocusElement == NULL)
      {
         sprintf( chPrevFocus, "No focus Items");
         sprintf( chPostFocus, "No focus Items");

      }
      else
      {
         sprintf( chPrevFocus, "Focus Prev Trace: %s",
               CurrentFocusElement->relative->PrevFile->GetName());
         sprintf( chPostFocus, "Focus Post Trace: %s",
               CurrentFocusElement->relative->PostFile->GetName());

      }

      XMSPrevFocus = XmStringCreateSimple( chPrevFocus );
      XMSPostFocus = XmStringCreateSimple( chPostFocus );

      XmStringFree(XMSPrevFocus);
      XmStringFree(XMSPostFocus);

      d = MidTime - MinTime ;

      if ( d < 0LL )
      {
           MidTime = MinTime;
      }
  
      strcpy(str,CreateSecondsFromLTime(MidTime));

      XMSMidTime = XmStringCreateSimple( str );
      XtVaSetValues( MidLabel, XmNlabelString, XMSMidTime, NULL);

      XmStringFree(XMSMidTime);

      XClearArea( XtDisplay( da ), XtWindow( da ), 0, 0 , 0, 0 , False );

      
      for( int i=0; i < TraceListSeq.size(); i++ )
      {
         TraceListSeq[ i ]->DrawGroup( flag );
      }

  
      guiDrawScaleClicks();

      strcpy(str,CreateSecondsFromLTime(Scale));

      sprintf(TempString,"%s per pixel",str);
      XMSMidTime = XmStringCreateSimple( TempString );

      XtVaSetValues(GetApplication()->lbScaleValue, 
                    XmNlabelString, XMSMidTime, NULL);

      XmStringFree(XMSMidTime);

   }

   else 
   {
      printf("Unknown flag value\n");
      exit (-1);
   }
   XUndefineCursor(XtDisplay(GetApplicationShell()),
                   XtWindow(GetApplicationShell()));

   GetApplication()->SendMessageText((char *) "\nReady");

}


void TracerWindow::cbCreTraceOption(Widget w, XtPointer client_data,
                                    XtPointer call_data )
{

     TracerWindow *TWindow = ( TracerWindow* )client_data;


//     CreateDirectoryDialog(TWindow->y_pos);
}

void TracerWindow::cbCreSetOption(Widget w, XtPointer client_data,
                                   XtPointer call_data )
{
  TracerWindow *TWindow = ( TracerWindow* )client_data;

  if (TWindow->CreateGroupDialogBox == NULL)
  {
    TWindow->CreateGroupDialogBox = new CreateSetDialog(TWindow);
  }
  else
  {
    TWindow->CreateGroupDialogBox->UpdateDialog();
    XtManageChild(TWindow->CreateGroupDialogBox->wCreSetDialog);
  }
}

void TracerWindow::SetMidPoint(int x)
{

     int  slot,xslot;
     slot = x ;
   
     xslot =  slot - (daWidth / 2) ;

     MidTime = MinTime + MidTime +  (xslot *Scale);

     SetMidTime(FULL_REDRAW);
}


int TracerWindow::GetScanTime( int x, LTime *thisTime )
{
    LTime    XX = (LTime) x;
    long long WindowSpan, LeftTime, RightTime,  ht;
   
    WindowSpan = Scale;
    WindowSpan *= daWidth;
   
    LeftTime  = MidTime  - (WindowSpan / 2 );
    RightTime = MidTime  + (WindowSpan / 2 );

    ht =  LeftTime + ( XX * Scale );
   
    *thisTime = ( LTime) ht;

    if ( *thisTime < LeftTime )
        return -1;

    if ( *thisTime > RightTime )
         return 1;

    return 0;

}

/*******************************************************************************
*
* Function: DisplayFormTime
*
*
* Purpose:
*
*******************************************************************************/

void TracerWindow::DisplayFormTime(LTime t, int button, int xpos )
{
     char str[32];
     char txt_str[32];

     double index  = log10( (double) Scale);

     if (UserDefFlag == False )
     {
        // Not User defined scale so print the menu value
        strcpy(txt_str, ScaleStrings[(int)index]);
     }
     else
     {
        // User defined scale
        strcpy(txt_str, "ns");
     }

     strcpy(txt_str,CreateSecondsFromLTime(t));

     if ( button == 3 )
       sprintf(str,"Mark:< %s >", txt_str  );
     else if ( button == 2 )
       sprintf(str,"Mid:< %s  >", txt_str );
     else
       sprintf(str,"< %s >", txt_str );

     XDrawImageString( XtDisplay( formTime), XtWindow( formTime ), timeGC,
                xpos, 15,
                str, (int)(strlen( str) ));

}

void TracerWindow::SetCurrentFocusElement(TraceItem* TItem)
{
   CurrentFocusElement = TItem;
   SetMidTime(REFRESH);
}

void TracerWindow::HorScrollBarCB(Widget w, XtPointer call_data)
{
   char str[256];
   XmString XMSMidTime;
   long long NumberPixelsToDisplayAll;
   double NewValue;

   LTime WindowSpan;
   XmScrollBarCallbackStruct* ptr = (XmScrollBarCallbackStruct*)call_data;

   WindowSpan = Scale;
   WindowSpan *= daWidth;

   NumberPixelsToDisplayAll=(long long)((WindowSpan +
                                            GetWindowEndTime())/Scale);

   NewValue = ((double)ptr->value *(double)NumberPixelsToDisplayAll) /
                                                                    (double)100;

   MidTime = (NewValue * Scale);

   switch(ptr->reason)
   {
      case XmCR_DECREMENT:
        // printf("XmCR_DECREMENT\n");
         break;

      case XmCR_INCREMENT:
        // printf("XmCR_INCREMENT\n");
         break;

      case XmCR_PAGE_DECREMENT:
        // printf("XmCR_PAGE_DECREMENT\n");
         break;

      case XmCR_PAGE_INCREMENT:
        // printf("XmCR_PAGE_INCREMENT\n");
         break;

      case XmCR_TO_TOP:
        // printf("XmCR_TO_TOP\n");
         break;

      case XmCR_TO_BOTTOM:
        // printf("XmCR_TO_BOTTOM\n");
         break;

      case XmCR_VALUE_CHANGED:
         //printf("XmCR_VALUE_CHANGED\n");
         break;

      case XmCR_DRAG:
        // printf("XmCR_DRAG\n");
         // Just update the midtime
         strcpy(str,CreateSecondsFromLTime(MidTime));

         XMSMidTime = XmStringCreateSimple( str );
         XtVaSetValues( MidLabel, XmNlabelString, XMSMidTime, NULL);

         XmStringFree(XMSMidTime);

         return;

      default: 
        break;
   }

   SetMidTime(FULL_REDRAW);
}


void TracerWindow::VertScrollBarCB(Widget w, XtPointer call_data)
{

   XmScrollBarCallbackStruct* ptr = (XmScrollBarCallbackStruct*)call_data;

   VirtualHeight = ptr->value;

   switch(ptr->reason)
   {
      case XmCR_DECREMENT:
         break;

      case XmCR_INCREMENT:
         break;

      case XmCR_PAGE_DECREMENT:
         break;

      case XmCR_PAGE_INCREMENT:
         break;

      case XmCR_TO_TOP:
         break;

      case XmCR_TO_BOTTOM:
         break;

      case XmCR_VALUE_CHANGED:
         break;

      case XmCR_DRAG:
         return;

      default:
         break;
   }

   SetMidTime(FULL_REDRAW);

}
void TracerWindow::cbHorScrollBar(Widget w, XtPointer client_data,
                                           XtPointer call_data )
{

   
   TracerWindow* TWindow = (TracerWindow*)client_data;
   if (TWindow != NULL)
   {
      TWindow->HorScrollBarCB(w,call_data);
   }

}

void TracerWindow::cbVertScrollBar(Widget w, XtPointer client_data,
                                           XtPointer call_data )
{

  
   TracerWindow* TWindow = (TracerWindow*)client_data;
   if (TWindow != NULL)
   {
      TWindow->VertScrollBarCB(w,call_data);
   }

}

void TracerWindow::SetWindowStartTime(LTime Value)
{
   WindowStartTime = Value;
}

void TracerWindow::SetWindowEndTime(LTime Value)
{
   WindowEndTime = Value;

}

LTime TracerWindow::GetWindowStartTime()
{
   return WindowStartTime;
}

LTime TracerWindow::GetWindowEndTime()
{
   return WindowEndTime;
}

void TracerWindow::RecalcScrollBar()
{
   LTime WindowSpan;
   long long NumberPixelsToDisplayAll;

   WindowSpan = Scale;
   WindowSpan *= daWidth;
   int PercentageOfScreenVisible;
   int PercentageForMidTime;
   long long NewValue;
   double    ScreenCalc;

   NumberPixelsToDisplayAll=(long long)((WindowSpan +
                                            GetWindowEndTime())/Scale);


   ScreenCalc = (double)daWidth/(double)NumberPixelsToDisplayAll;


   PercentageOfScreenVisible = ScreenCalc * 100;


   if (PercentageOfScreenVisible < 1)
   {
      PercentageOfScreenVisible = 1;
   }

   if (MidTime != 0)
      NewValue = MidTime / Scale;
   else
      NewValue = 1/Scale;

   PercentageForMidTime = (int)((
                      (double)NewValue/(double)NumberPixelsToDisplayAll) * 100);

   if (PercentageOfScreenVisible > 100)
   {
      PercentageOfScreenVisible = 100;
   }

   if (PercentageForMidTime > 100)
   {
      printf("PercentageForMidTime > 100\n");

     PercentageForMidTime = 100 - PercentageOfScreenVisible;
   }

   if (MidTime >= GetWindowEndTime())
   {
      PercentageForMidTime = 100 - PercentageOfScreenVisible;
   }
/*
   printf("XmNsliderSize:[%d] XmNvalue:[%d]\n",PercentageOfScreenVisible,
                                               PercentageForMidTime);
*/
/*
   if (PercentageForMidTime < 0)
   {
      PercentageForMidTime = 0;
   }
*/
   XtVaSetValues(daHorizontalScrollBar,
                 XmNsliderSize, PercentageOfScreenVisible,
                 XmNvalue,      PercentageForMidTime,
                 NULL);
}

int TracerWindow::AddToFileList(char* FileName, __pktraceRead* ptrMappedFile)
{

   int CurrentFile = GetFilePosition(FileName);

   if (CurrentFile != -1)
   {
      return CurrentFile;
   }
   else
   {
      OpenFileList[NumOpenFiles].pMappedFile = ptrMappedFile;
      strcpy(OpenFileList[NumOpenFiles].Filename, FileName);
   }
   NumOpenFiles++;
 
   return (NumOpenFiles - 1);
}

int TracerWindow::GetFilePosition(char* FileName)
{
   register int loop;
   for (loop = 0; loop < NumOpenFiles; loop++)
   {
      if (strcmp(FileName,OpenFileList[loop].Filename) == 0)
      {
         return loop;
      }
   }
   return -1;
}

__pktraceRead* TracerWindow::GetMappedPointer(int Index)
{
   if (Index  > NumOpenFiles)
   {
      printf("Returning NULL\n");
      return NULL;
   }
   else
   {
      return (OpenFileList[Index].pMappedFile);
   }
}

void TracerWindow::AutoLoadGroups(char* StartDirectory, char* RegExpPath)
{
  BegLogLine(PKFXLOG_TRACER)
    << "TracerWindow::AutoLoadGroups " 
    << EndLogLine ;
   register int nloop;
   register int OuterLoop;
   register int loop;
   GroupFile *LoadedFile;
   int FileIndex;
   struct stat buf;
   __pktraceRead* MappedFile = NULL;
   BegLogLine(PKFXLOG_TRACER)
     << "TracerWindow::AutoLoadGroups " 
     << EndLogLine ;
  MessageDialog *MessageBox;

   // TraceGroupListType::Cursor cursor(TraceListSeq);

      int            NumPreviousFileListItems;
//      ListElement   PreviousFileList[MAX_TRACES];
      int            NumPostFileListItems;
//      ListElement    PostFileList[MAX_TRACES];
      
      ListElement * PreviousFileList = new ListElement[MAX_TRACES] ;
      ListElement * PostFileList = new ListElement[MAX_TRACES] ;

 
   // Check the file exists
//   if (stat(StartDirectory,&buf) < 0)
//   {
//      // file not accessible
//      MessageBox = new MessageDialog(form0,"Error loading file",ERROR);
//      strcpy(StartDirectory,"");
//      return;
//   }

   // Open the source file
      BegLogLine(PKFXLOG_TRACER)
        << "TracerWindow::AutoLoadGroups " 
        << EndLogLine ;
      BegLogLine(PKFXLOG_TRACER)
        << "TracerWindow::AutoLoadGroups StartDirectory=" <<  StartDirectory
        << EndLogLine ;
   FileIndex = GetFilePosition(StartDirectory);

   BegLogLine(PKFXLOG_TRACER)
     << "TracerWindow::AutoLoadGroups StartDirectory=" <<  StartDirectory
     << " FileIndex=" << FileIndex
     << EndLogLine ;
   // Clear the current file
   // Check if the selected file is currently loaded
   if (FileIndex == -1)
   {
      // Load the new file
      MappedFile =  new __pktraceRead(StartDirectory);
      if (MappedFile->IsValidMagicNum() == FALSE)
      {
         MessageDialog* MessageBox = new MessageDialog(form0,
             (char *)  "File is not correct format for this version",
                                  ERROR);
         delete MappedFile;
         MappedFile = NULL;
         return;
      }

      FileIndex = AddToFileList(StartDirectory,
                                MappedFile);
   }
   else
   {
      // The file is already loaded, retreive the pointer
      MappedFile = GetMappedPointer(FileIndex);
   }

   LoadedFile = new GroupFile(RegExpPath);

   for (OuterLoop = 0;
        OuterLoop < LoadedFile->GetNumberOfLoadedGroups(); OuterLoop++)
   {
      NumPreviousFileListItems = 0;
      //Loop to load prev files
      for (loop = 0; loop < MappedFile->GetNumTraces(); loop++)
      {
        if (CheckReg(LoadedFile->GetPrevRegExp(OuterLoop),
                     MappedFile->GetTraceName(loop)) == True)
        {
          // Copy to the Previous data list
          strcpy(PreviousFileList[NumPreviousFileListItems].Filename,
                                             MappedFile->GetTraceName(loop));
          PreviousFileList[NumPreviousFileListItems].MappedHeaderPos = loop;

          // Increment the number of items
          NumPreviousFileListItems++;
        }
      }

      NumPostFileListItems = 0;
      //Loop to load Post files
      for (loop = 0; loop < MappedFile->GetNumTraces(); loop++)
      {
        if (CheckReg(LoadedFile->GetPostRegExp(OuterLoop),
                        MappedFile->GetTraceName(loop)) == True)
        {
          // Copy to the Previous data list
          strcpy(PostFileList[NumPostFileListItems].Filename,
                                             MappedFile->GetTraceName(loop));
          PostFileList[NumPostFileListItems].MappedHeaderPos = loop;

          // Increment the number of items
          NumPostFileListItems++;
        }
      }

      if (NumPreviousFileListItems != NumPostFileListItems)
      {
         printf("NumPreviousFileListItems:[%d] NumPostFileListItems:[%d]\n",
                          NumPreviousFileListItems,NumPostFileListItems);
      }

      /* Create a set here */
      TraceGroupPtr Group = new TraceGroup(this,
                        LoadedFile->GetGroupName(OuterLoop),
                        StartDirectory,
                        LoadedFile->GetPrevRegExp(OuterLoop),
                        LoadedFile->GetPostRegExp(OuterLoop),
                        LoadedFile->GetColor(OuterLoop),
                        LoadedFile->GetGraphStyle(OuterLoop),
                        LoadedFile->GetGroupYOffset(OuterLoop),
                        LoadedFile->GetTraceSeparation(OuterLoop),
                        LoadedFile->GetInverted(OuterLoop),
                        LoadedFile->GetContourEnabled(OuterLoop));

      for (nloop = 0; nloop < NumPreviousFileListItems; nloop++)
      {
         Group->AddTrace( LoadedFile->GetGroupYOffset(OuterLoop) - (nloop),
                          1,
                          LoadedFile->GetColor(OuterLoop), 
                          RELATIVE,
                          MappedFile,
                          PreviousFileList[nloop].MappedHeaderPos,
                          PostFileList[nloop].MappedHeaderPos,
                          LoadedFile->GetGraphStyle(OuterLoop),
                          LoadedFile->GetInverted(OuterLoop));

      }

      // TraceListSeq.add(Group,cursor);
      TraceListSeq.push_back( Group );

   } /* End of the create trace */

   SetMidTime(FULL_REDRAW);
   
   delete [] PreviousFileList ;  
   delete [] PostFileList ;
}
