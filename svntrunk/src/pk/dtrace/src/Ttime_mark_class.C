/*
 * NAME:        tracer_time_mark_class.C
 *
 * AUTHOR:      <<Author>>
 *
 * VERSION:     1.1
 *
 * DATE:        97/01/20
 *
 * COPYRIGHT:   %copyright%
 *
 * SYNOPSIS:
 * 
 * DESCRIPTION:
 *
 * CAVEATS/WARNINGS:
 *
 * RETURNED VALUES:
 *
 * GENERATED MESSAGES:
 *
 * PRODUCT TRACE:
 *
 * SIDE EFFECTS:
 *
 * PERFORMANCE:
 *
 * SPECIAL CONSIDERATIONS:
 *
 * RELATED INFORMATION:
 *
 * BUILD OPTIONS:
 *
 * DESIGN:
 * N/A
 *
 */
// template <class Element>
// class IACollection;
#include <Ttime_mark.hpp>

TraceTimeMark::TraceTimeMark(TracerWindow *Parent, int StartPos, 
                             char* pchScaleValue,
                             long long ScaleVal)
{
   ParentWindow = Parent;
   StartPosition = StartPos;
   EndPosition   = StartPos;
   strcpy(ScaleValue,pchScaleValue);
   Scale = ScaleVal;

   CreateMark();

   XDrawLine(XtDisplay(ParentWindow->da), XtWindow(ParentWindow->da),
                ParentWindow->invertedGC,
                StartPosition, 0,  StartPosition, daHeight );

}

TraceTimeMark::TraceTimeMark(const TraceTimeMark&)
{

}

TraceTimeMark::~TraceTimeMark()
{

   XtDestroyWidget(StartButton);
   XtDestroyWidget(StopButton);
   XtDestroyWidget(ControlButton);
   XtDestroyWidget(ValueLabel);

   XDrawLine(XtDisplay(ParentWindow->da), XtWindow(ParentWindow->da),
                       ParentWindow->invertedGC,
                StartPosition, 0,  StartPosition, daHeight );

   if (EndPosition != StartPosition)
   {
      XDrawLine(XtDisplay(ParentWindow->da), XtWindow(ParentWindow->da),
                ParentWindow->invertedGC,
                EndPosition, 0,  EndPosition, daHeight );
   }

   XClearWindow( XtDisplay( ParentWindow->formTime ),
                 XtWindow( ParentWindow->formTime ) );

}

void  TraceTimeMark::CreateMark()
{
     XmString xmString;
     char str[256];

     Arg  args[16];
     char *color = (char *) BG_COLOR;

     int n =0;

     XmFontList       fontlist;
     XmFontListEntry  entry;
     XFontStruct      *font;

     font = XLoadQueryFont( XtDisplay(TracerApplication::GetApp()->top),
                             "6x10" );
     entry = XmFontListEntryCreate( (char *) "tag1", XmFONT_IS_FONT , font);
     fontlist = XmFontListAppendEntry( NULL, entry );

     // Label for trace
     XtSetArg( args[n],XmNtopAttachment,    XmATTACH_FORM); n++;
     XtSetArg( args[n],XmNtopOffset,        8 ); n++;
//     XtSetArg( args[n],XmNdragInitiatorProtocolStyle, XmDRAG_NONE); n++;
//     XtSetArg( args[n],XmNdragReceiverProtocolStyle, XmDRAG_NONE); n++;
     XtSetArg( args[n],XmNleftAttachment,   XmATTACH_FORM ); n++;
     XtSetArg( args[n],XmNleftOffset,       StartPosition ); n++;
     XtSetArg( args[n],XmNfontList,         fontlist ); n++;
     XtSetArg( args[n],XmNheight,         6 ); n++;
     XtSetArg( args[n],XmNwidth,         6 ); n++;

     // Create the Widget
     StartButton = XmCreatePushButton( ParentWindow->formTime, (char *) ".", args, n );


     // Label for trace
     XtAddEventHandler(StartButton, ButtonPressMask, False,
                       (XtEventHandler)&(TraceTimeMark::mark_press_handler),
                       (XtPointer)this );


     XtAddEventHandler(StartButton, ButtonReleaseMask, False,
                       (XtEventHandler)&(TraceTimeMark::mark_release_handler),
                       (XtPointer)this );

     XtAddEventHandler(StartButton, ButtonMotionMask, False,
                       (XtEventHandler)&(TraceTimeMark::mark_motion_handler),
                       (XtPointer)this );

     XtVaSetValues( StartButton,
                      XtVaTypedArg, XmNbackground,  XmRString, "DimGrey", 8,
                      NULL);

     XtManageChild( StartButton );


     //
     // Create the stop button
     //
     // Label for trace
     n = 0;

     XtSetArg( args[n],XmNtopAttachment,    XmATTACH_FORM); n++;
     XtSetArg( args[n],XmNtopOffset,        50 ); n++;
//     XtSetArg( args[n],XmNdragInitiatorProtocolStyle, XmDRAG_NONE); n++;
//     XtSetArg( args[n],XmNdragReceiverProtocolStyle, XmDRAG_NONE); n++;
     XtSetArg( args[n],XmNleftAttachment,   XmATTACH_FORM ); n++;
     XtSetArg( args[n],XmNleftOffset,       StartPosition ); n++;
     XtSetArg( args[n],XmNfontList,         fontlist ); n++;
     XtSetArg( args[n],XmNheight,         6 ); n++;
     XtSetArg( args[n],XmNwidth,         6 ); n++;


     // Create the Widget
     StopButton = XmCreatePushButton( ParentWindow->formTime, (char *) ".", args, n );



     XtVaSetValues( StopButton,
                      XtVaTypedArg, XmNbackground,  XmRString, "DimGrey", 8,
                      NULL);

     XtManageChild( StopButton );

     //
     // Create the control button
     //
     // Label for trace
     n = 0;
     XtSetArg( args[n],XmNtopAttachment,        XmATTACH_FORM); n++;
     XtSetArg( args[n],XmNtopOffset,            13 ); n++;
//     XtSetArg( args[n],XmNdragInitiatorProtocolStyle, XmDRAG_NONE); n++;
//     XtSetArg( args[n],XmNdragReceiverProtocolStyle, XmDRAG_NONE); n++;
     XtSetArg( args[n],XmNleftAttachment,       XmATTACH_OPPOSITE_WIDGET ); n++;
     XtSetArg( args[n],XmNleftWidget,           StartButton ); n++;
     XtSetArg( args[n],XmNleftOffset,           0); n++;
     XtSetArg( args[n],XmNrightAttachment,     XmATTACH_OPPOSITE_WIDGET ); n++;
     XtSetArg( args[n],XmNrightWidget,         StopButton ); n++;
     XtSetArg( args[n],XmNrightOffset,         0); n++;
     XtSetArg( args[n],XmNheight,         6 ); n++;
     XtSetArg( args[n],XmNfontList,             fontlist ); n++;

     // Create the Widget
     ControlButton = XmCreatePushButton( ParentWindow->formTime, (char *) ".", args, n );

     XtVaSetValues( ControlButton,
                      XtVaTypedArg, XmNbackground,  XmRString, "DimGrey", 8,
                      NULL);


     XmFontListFree( fontlist );

     XtManageChild( ControlButton );

     ValueLabel = XtVaCreateManagedWidget( "ValueLabel",
                              xmLabelWidgetClass, ParentWindow->formTime,
                              XmNalignment,       XmALIGNMENT_CENTER,
                              XmNtopAttachment,   XmATTACH_FORM,
                              XmNtopOffset,       0,
                              XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET,
                              XmNleftWidget,      ControlButton,
                              XmNleftOffset,      0,
                                           //                              XmNfontList,        fontlist,
                              NULL );

   strcpy (str,CreateSecondsFromLTime((LTime)0));

   xmString = XmStringCreateSimple(str);

   XtVaSetValues(ValueLabel,
                 XmNlabelString, xmString,
                 XtVaTypedArg, XmNbackground,
                 XmRString, color, strlen(color),
                 NULL);

   XmStringFree(xmString);

}

void TraceTimeMark::mark_press_handler(Widget w,XtPointer p,
                                       XButtonPressedEvent *event)
{

}

void TraceTimeMark::mark_release_handler(Widget w,XtPointer p,
                                       XButtonPressedEvent *event)
{

}

void TraceTimeMark::mark_motion_handler(Widget w,XtPointer p,
                                       XButtonPressedEvent *event)
{
 
}

void TraceTimeMark::MoveEndPoint(int X_Position)
{
   char str[256];
   XmString xmString;
   LTime StartTime, EndTime, TimeDiff;
   if (X_Position < StartPosition)
      return;

   /* Reposition the end point marker */
   XtVaSetValues(StopButton,
                 XmNleftOffset,X_Position,
                 NULL);

   ParentWindow->GetScanTime(StartPosition, &StartTime);
   ParentWindow->GetScanTime(X_Position,  &EndTime);

   // TimeDiff is time difference in ns
   TimeDiff = EndTime - StartTime;

   strcpy (str,CreateSecondsFromLTime(TimeDiff));

   xmString = XmStringCreateSimple(str);

   XtVaSetValues(ValueLabel,
                 XmNlabelString, xmString,
                 NULL);

   XmStringFree(xmString);
   EndPosition = X_Position; 
}

LTime TraceTimeMark::GetStartTime()
{
  LTime StartTime;
  ParentWindow->GetScanTime(StartPosition, &StartTime);
  return StartTime;
}

LTime TraceTimeMark::GetEndTime()
{
  int X_Position;

  LTime EndTime;

  XtVaGetValues(StopButton,
                 XmNleftOffset,&X_Position,
                 NULL);

  ParentWindow->GetScanTime(X_Position, &EndTime);
  return EndTime;
}

